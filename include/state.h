#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include <I2CKeyPad.h>
#include <vector>
#include "models.h"

// === Enum State Aplikasi ===
enum State {
  MENU_UTAMA,
  ISI_DATA,
  KONFIRMASI_DATA,
  ISI_FINGERPRINT,
  ABSEN,
  VERIFIKASI_ADMIN,
  ISI_DATA_ADMIN,
  KONFIRMASI_ADMIN,
  ISI_FINGERPRINT_ADMIN,
  VERIFIKASI_MHS_EDIT,
  EDIT_DATA_MHS,
  KONFIRMASI_EDIT_MHS,
  SUBMENU_EDIT_USER,
  VERIFIKASI_ADMIN_EDIT,     // ✅ Tambahan
  EDIT_DATA_ADMIN,           // ✅ Tambahan
  KONFIRMASI_EDIT_ADMIN      // ✅ Tambahan
};

// === Global State & Status ===
extern State currentState;
extern String selectedMenu;
extern bool isAbsenMasuk;
extern unsigned long absenStartTime;

// === Data Mahasiswa & Admin ===
extern Mahasiswa mhs;
extern Admin admin;
extern String* currentField;
extern String* currentAdminField;
extern int fieldIndex;
extern String inputText;

// === Fingerprint ID Tracking ===
extern uint16_t nextFingerID;
extern uint16_t nextMahasiswaFingerID;
extern uint16_t nextAdminFingerID;
extern std::vector<uint16_t> daftarAdminIDs;
extern std::vector<uint16_t> daftarMahasiswaIDs;
extern std::vector<Admin> daftarAdminData;
extern std::vector<Mahasiswa> daftarMahasiswaData;
extern std::vector<FingerprintRecord> daftarFingerprintRecords;

// === Editing Context ===
extern int indeksMahasiswaEdit;
extern int indeksAdminEdit;
extern uint32_t currentMahasiswaUserId;
extern uint32_t currentAdminUserId;
extern uint16_t currentMahasiswaFingerprintId;
extern uint16_t currentAdminFingerprintId;
extern String currentMahasiswaEmail;
extern String currentAdminEmail;

// === Keypad Setup ===
extern I2CKeyPad keyPad;
extern char keys[];

// === Fungsi global ===
void tampilkanMenuUtama();
void tampilkanFormDataMahasiswa();
void tampilkanFormDataAdmin();
bool enrollFingerprint(uint16_t id);

#endif
