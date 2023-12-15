#include "uthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

// 队列节点
struct QueueNode {
    struct uthread *thread;
    struct QueueNode *next;
};

// 队列结构
struct Queue {
    struct QueueNode *front;
    struct QueueNode *rear;
};

// 初始化队列
void initQueue(struct Queue *q) {
    q->front = q->rear = NULL;
}

// 入队列
void enqueue(struct Queue *q, struct uthread *thread) {
    struct QueueNode *newNode = (struct QueueNode *)malloc(sizeof(struct QueueNode));
    if (newNode == NULL) {
        perror("Failed to allocate memory for queue node");
        exit(EXIT_FAILURE);
    }
    newNode->thread = thread;
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    q->rear->next = newNode;
    q->rear = newNode;
}

// 出队列
struct uthread *dequeue(struct Queue *q) {
    if (q->front == NULL) {
        return NULL;
    }
    struct QueueNode *temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    struct uthread *thread = temp->thread;
    free(temp);
    return thread;
}

static struct Queue readyQueue;

static struct uthread *current_thread = NULL;
static struct uthread *main_thread = NULL;

/// @brief 切换上下文
/// @param from 当前上下文
/// @param to 要切换到的上下文
extern void thread_switch(struct context *from, struct context *to);

/// @brief 线程的入口函数
/// @param tcb 线程的控制块
/// @param thread_func 线程的执行函数
/// @param arg 线程的参数
void _uthread_entry(struct uthread *tcb, void (*thread_func)(void *),
                    void *arg);

/// @brief 清空上下文结构体
/// @param context 上下文结构体指针
static inline void make_dummpy_context(struct context *context) {
  memset((struct context *)context, 0, sizeof(struct context));
}

struct uthread *uthread_create(void (*func)(void *), void *arg,const char* thread_name) {
  struct uthread *uthread = NULL;
  int ret;

  // 申请一块16字节对齐的内存
  ret = posix_memalign((void **)&uthread, 16, sizeof(struct uthread));
  if (0 != ret) {
    printf("error");
    exit(-1);
  }

  //         +------------------------+
  // low     |                        |
  //         |                        |
  //         |                        |
  //         |         stack          |
  //         |                        |
  //         |                        |
  //         |                        |
  //         +------------------------+
  //  high   |    fake return addr    |
  //         +------------------------+

  /*
  TODO: 在这里初始化uthread结构体。可能包括设置rip,rsp等寄存器。入口地址需要是函数_uthread_entry.
        除此以外，还需要设置uthread上的一些状态，保存参数等等。
        
        你需要注意rsp寄存器在这里要8字节对齐，否则后面从context switch进入其他函数的时候会有rsp寄存器
        不对齐的情况（表现为在printf里面Segment Fault）
  */
  uthread->state = THREAD_INIT;
  uthread->name = thread_name;

  make_dummpy_context(&uthread->context);

  uthread->context.rsp = ((long long)&uthread->stack + STACK_SIZE) & (~(long long)15);
  uthread->context.rsp -=8;
  uthread->context.rip = (long long)_uthread_entry;

  uthread->context.rdi = (long long)uthread;
  uthread->context.rsi = (long long)func;
  uthread->context.rdx = (long long)arg;

  //把新创建的线程加入到队列中
  enqueue(&readyQueue, uthread);

  return uthread;
}


void schedule() {
  /*
  TODO: 在这里写调度子线程的机制。这里需要实现一个FIFO队列。这意味着你需要一个额外的队列来保存目前活跃
        的线程。一个基本的思路是，从队列中取出线程，然后使用resume恢复函数上下文。重复这一过程。
  */
  if (current_thread == NULL) {
    // 第一次调度，从主线程开始
    current_thread = main_thread;
    current_thread->state = THREAD_RUNNING;
  }
  //printf("%s\n",current_thread->name);

  //不断取出队头，直到队列为空
  while(1)
  {
    struct uthread *next_thread = dequeue(&readyQueue);


    if (next_thread) {
          uthread_resume(next_thread);
          
          if(current_thread->state == THREAD_STOP)
          {
            //销毁线程
            //printf("线程结束：%s\n",current_thread->name);
            thread_destory(current_thread);
          }
          current_thread=main_thread;//将当前线程设置为主线程
    } else {
      //printf("队列为空\n");
      break;
    }
  }
}

long long uthread_yield() {
  /*
  TODO: 用户态线程让出控制权到调度器。由正在执行的用户态函数来调用。记得调整tcb状态。
  */
  //printf("线程主动让出\n");
  // 设置当前线程的状态为挂起
  current_thread->state = THREAD_SUSPENDED;
  // 将当前线程加入到队列中
  enqueue(&readyQueue, current_thread);
  // 控制权转交回调度器
  //printf("线程切换回主线程\n");
  thread_switch(&current_thread->context, &main_thread->context);
  return 0;
}

void uthread_resume(struct uthread *tcb) {
  /*
  TODO：调度器恢复到一个函数的上下文。
  */
  //printf("当前线程：%s\n",current_thread->name);
  tcb->state = THREAD_RUNNING;

  // 在恢复时，将当前线程设置为被恢复的线程
  //printf("下一线程：%s\n",tcb->name);
  current_thread = tcb;

  // 执行上下文切换，恢复线程的上下文
  thread_switch(&main_thread->context, &tcb->context);
}

void thread_destory(struct uthread *tcb) {
  free(tcb);
}

void _uthread_entry(struct uthread *tcb, void (*thread_func)(void *),
                    void *arg) {
  /*
  TODO: 这是所有用户态线程函数开始执行的入口。在这个函数中，你需要传参数给真正调用的函数，然后设置tcb的状态。
  */
  tcb->state = THREAD_RUNNING;
  thread_func(arg);
  tcb->state = THREAD_STOP;
  
  thread_switch(&tcb->context, &main_thread->context);
}

void init_uthreads() {
  main_thread = malloc(sizeof(struct uthread));
  make_dummpy_context(&main_thread->context);
  main_thread->name="main";
  initQueue(&readyQueue);//初始化队列
}