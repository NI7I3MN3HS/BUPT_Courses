CREATE TABLE `exampaperdetails`  (
  `ExamPaperID` int NOT NULL,
  `QuestionID` int NOT NULL,
  PRIMARY KEY (`ExamPaperID`, `QuestionID`) USING BTREE,
  INDEX `QuestionID`(`QuestionID` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

CREATE TABLE `exampapers`  (
  `ExamPaperID` int NOT NULL AUTO_INCREMENT,
  `Title` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  PRIMARY KEY (`ExamPaperID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

CREATE TABLE `exams`  (
  `ExamID` int NOT NULL AUTO_INCREMENT,
  `Title` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `TeacherID` int NOT NULL,
  `ExamPaperID` int NOT NULL,
  `ExamTime` datetime NOT NULL,
  PRIMARY KEY (`ExamID`) USING BTREE,
  INDEX `TeacherID`(`TeacherID` ASC) USING BTREE,
  INDEX `ExamPaperID`(`ExamPaperID` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

CREATE TABLE `knowledgepoints`  (
  `KnowledgePointID` int NOT NULL AUTO_INCREMENT,
  `Description` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  PRIMARY KEY (`KnowledgePointID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

CREATE TABLE `questions`  (
  `QuestionID` int NOT NULL AUTO_INCREMENT,
  `KnowledgePointID` int NOT NULL,
  `Content` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `Score` int NOT NULL,
  `OptionA` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `OptionB` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `OptionC` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `OptionD` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `CorrectAnswer` char(1) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  PRIMARY KEY (`QuestionID`) USING BTREE,
  INDEX `KnowledgePointID`(`KnowledgePointID` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

CREATE TABLE `studentexamrecords`  (
  `RecordID` int NOT NULL AUTO_INCREMENT,
  `StudentID` int NOT NULL,
  `ExamID` int NOT NULL,
  `QuestionID` int NOT NULL,
  `StudentAnswer` char(1) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  `Score` int NULL DEFAULT NULL,
  PRIMARY KEY (`RecordID`) USING BTREE,
  INDEX `StudentID`(`StudentID` ASC) USING BTREE,
  INDEX `ExamID`(`ExamID` ASC) USING BTREE,
  INDEX `QuestionID`(`QuestionID` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

CREATE TABLE `users`  (
  `UserID` int NOT NULL AUTO_INCREMENT,
  `UserName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `Password` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `Role` enum('Teacher','Student') CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  PRIMARY KEY (`UserID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

ALTER TABLE `exampaperdetails` ADD CONSTRAINT `exampaperdetails_ibfk_1` FOREIGN KEY (`ExamPaperID`) REFERENCES `exampapers` (`ExamPaperID`) ON DELETE RESTRICT ON UPDATE RESTRICT;
ALTER TABLE `exampaperdetails` ADD CONSTRAINT `exampaperdetails_ibfk_2` FOREIGN KEY (`QuestionID`) REFERENCES `questions` (`QuestionID`) ON DELETE RESTRICT ON UPDATE RESTRICT;
ALTER TABLE `exams` ADD CONSTRAINT `exams_ibfk_1` FOREIGN KEY (`TeacherID`) REFERENCES `users` (`UserID`) ON DELETE RESTRICT ON UPDATE RESTRICT;
ALTER TABLE `exams` ADD CONSTRAINT `exams_ibfk_2` FOREIGN KEY (`ExamPaperID`) REFERENCES `exampapers` (`ExamPaperID`) ON DELETE RESTRICT ON UPDATE RESTRICT;
ALTER TABLE `questions` ADD CONSTRAINT `questions_ibfk_1` FOREIGN KEY (`KnowledgePointID`) REFERENCES `knowledgepoints` (`KnowledgePointID`) ON DELETE RESTRICT ON UPDATE RESTRICT;
ALTER TABLE `studentexamrecords` ADD CONSTRAINT `studentexamrecords_ibfk_1` FOREIGN KEY (`StudentID`) REFERENCES `users` (`UserID`) ON DELETE RESTRICT ON UPDATE RESTRICT;
ALTER TABLE `studentexamrecords` ADD CONSTRAINT `studentexamrecords_ibfk_2` FOREIGN KEY (`ExamID`) REFERENCES `exams` (`ExamID`) ON DELETE RESTRICT ON UPDATE RESTRICT;
ALTER TABLE `studentexamrecords` ADD CONSTRAINT `studentexamrecords_ibfk_3` FOREIGN KEY (`QuestionID`) REFERENCES `questions` (`QuestionID`) ON DELETE RESTRICT ON UPDATE RESTRICT;

