#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include "models.h"

struct ApiUser {
  uint32_t userId = 0;
  String email;
  String password;
  uint8_t isAdmin = 0;  // 0 or 1
  uint8_t isDosen = 0;  // 0 or 1
  String fingerprint;
  String nim;
  String nama;
  uint16_t kelas = 0;
  String createdAt;
  String updatedAt;
};

// User management functions
bool apiCreateUser(const ApiUser& user, ApiUser* created = nullptr);
bool apiGetUserByFingerprint(const String& fingerprint, ApiUser* user);
bool apiGetUserById(uint32_t userId, ApiUser* user);
bool apiUpdateUser(const ApiUser& user);
bool apiDeleteUserByFingerprint(const String& fingerprint);
bool apiDeleteUserById(uint32_t userId);

// Attendance functions
bool apiSendAttendance(const String& fingerprint, const String& timestamp, bool absenMasuk);

// Sync functions
void apiSyncUsersFromServer();
void apiSyncUsersToServer();

#endif

