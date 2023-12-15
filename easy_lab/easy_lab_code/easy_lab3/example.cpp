#include <iostream>
using namespace std;

int main() {
  void *a = malloc(32);
  cout << "malloc" << endl;
  void *b = malloc(64);
  cout << "Another malloc" << endl;
  void *f = malloc(96);
  cout << a << "address is" << endl;
  cout << b << "address is" << endl;
  cout << f << "address is" << endl;

  return 0;
}