#include "state.h"
#include <Wire.h>
#include <vector>
#include "admin_handler.h"

int indeksMahasiswaEdit = -1;  // Untuk edit Mahasiswa
int indeksAdminEdit = -1;      // ✅ Tambahan untuk edit Admin

// === Objek Mahasiswa & Admin ===
Mahasiswa mhs = { "", "", "", 0 };
Admin admin = { "", "", 0 };

// === State kontrol ===
State currentState = MENU_UTAMA;
String selectedMenu = "";
bool isAbsenMasuk = false;
unsigned long absenStartTime = 0;

// === ID Sidik Jari ===
uint16_t nextFingerID = 1;
uint16_t nextMahasiswaFingerID = 1;
uint16_t nextAdminFingerID = 50;

// === Input Tracker ===
String inputText = "";
int fieldIndex = 0;
String* currentField = &mhs.nama;
String* currentAdminField = &admin.nama;

// === Keypad I2C Setup ===
I2CKeyPad keyPad(0x20);
char keys[] = "123A456B789C*0#DNF";

// === Daftar Data Sementara ===
std::vector<uint16_t> daftarAdminIDs;
std::vector<Admin> daftarAdminData;
std::vector<Mahasiswa> daftarMahasiswaData;
