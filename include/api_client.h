#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include "models.h"

struct ApiUser {
  uint32_t id = 0;
  String email;
  String password;
  bool isAdmin = false;
  bool isDosen = false;
  String fingerprint;
  String nim;
  String nama;
  String kelas;
};

bool apiCreateUser(const ApiUser& user, ApiUser* created = nullptr);
bool apiGetUserByFingerprint(const String& fingerprint, ApiUser* user);
bool apiUpdateUser(const ApiUser& user);
bool apiDeleteUserByFingerprint(const String& fingerprint);
bool apiSendAttendance(const String& fingerprint, const String& timestamp, bool absenMasuk);

void apiSyncUsersFromServer();
void apiSyncUsersToServer();

#endif

