#ifndef MODELS_H
#define MODELS_H

#include <Arduino.h>

// Hanya definisi struktur data, TIDAK ada enum, TIDAK ada extern!

struct Mahasiswa {
  String nama;
  String nim;
  String kelas;
  String email;
  String password;
  bool isDosen;
  uint16_t sidikJariID;

  Mahasiswa()
      : nama(), nim(), kelas(), email(), password(), isDosen(false), sidikJariID(0) {}

  Mahasiswa(const String &nama_, const String &nim_, const String &kelas_, const String &email_,
            const String &password_, bool isDosen_, uint16_t sidikJariID_)
      : nama(nama_), nim(nim_), kelas(kelas_), email(email_), password(password_), isDosen(isDosen_),
        sidikJariID(sidikJariID_) {}
};

struct Admin {
  String nama;
  String nim;
  String email;
  String password;
  uint16_t sidikJariID;

  Admin() : nama(), nim(), email(), password(), sidikJariID(0) {}
};

struct FingerprintRecord {
  uint16_t fingerprintId;   // ID template pada sensor
  uint32_t userId;          // ID user pada server
  bool isAdmin;
  bool isDosen;

  FingerprintRecord()
      : fingerprintId(0), userId(0), isAdmin(false), isDosen(false) {}
};

#endif
