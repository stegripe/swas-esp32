#pragma once

#include <Arduino.h>
#include "state.h"
#include "input_handler.h"
#include "fingerprint_manager.h"

// ✅ Deklarasi variabel global yang didefinisikan di main.cpp
extern bool bypassAktif;
extern bool tombolDitekan;
extern unsigned long tombolDitekanMulai;

// ✅ Fungsi pendaftaran Admin
void tampilkanFormDataAdmin();
void handleInputFormAdmin(char key);

// ✅ Tambahan untuk fitur edit Admin
void mulaiEditAdmin();                  // Mulai proses edit data admin setelah verifikasi berhasil
void tampilkanFormEditAdmin();          // Tampilkan form edit data admin (nama, nik)
void handleInputEditAdmin(char key);    // Tangani input perubahan data admin
