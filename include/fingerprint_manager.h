#ifndef FINGERPRINT_MANAGER_H
#define FINGERPRINT_MANAGER_H

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include "models.h"
#include "state.h"

extern Adafruit_Fingerprint finger;
extern Mahasiswa mhs;
extern Admin admin;
extern State currentState;
extern bool isAbsenMasuk;
extern unsigned long absenStartTime;
extern uint16_t nextFingerID;

bool enrollFingerprintMahasiswa(uint16_t id, bool* dibatalkan = nullptr); // ✅ diubah
bool enrollFingerprintAdmin(uint16_t id, bool* dibatalkan);
bool enrollFingerprint(uint16_t id);
void handleAbsenLoop();
void handleVerifikasiAdmin();

#endif
