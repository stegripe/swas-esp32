#ifndef DB_CLIENT_H
#define DB_CLIENT_H

#include <Arduino.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WiFiClient.h>
#include <vector>

// Database connection details
extern const char* DB_HOST;
#define DB_PORT 2014
extern const char* DB_USER;
extern const char* DB_PASS;

// Student structure for database
struct StudentDB {
  String nim;
  String nama;
  String kelas;
  String fingerprints;
};

// Attendance structure for database
struct AttendanceDB {
  uint64_t id;
  String nim;
  uint8_t type;  // 1 = masuk, 2 = pulang
  String createdAt;
};

// Database functions for students only
bool dbConnect();
void dbDisconnect();
bool dbCreateStudent(const StudentDB& student);
bool dbGetStudentByFingerprint(const String& fingerprint, StudentDB* student);
bool dbUpdateStudent(const StudentDB& student);
bool dbDeleteStudent(const String& nim);
bool dbSendAttendance(const String& nim, const String& timestamp, bool absenMasuk);
bool dbGetAttendanceByNIM(const String& nim, std::vector<AttendanceDB>& attendances);
bool dbCheckTodayAttendance(const String& nim, const String& date, uint8_t& lastType);

// Global database objects
extern WiFiClient client;
extern MySQL_Connection conn;
extern MySQL_Cursor* cursor;

#endif
