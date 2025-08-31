#ifndef MENU_HANDLER_H
#define MENU_HANDLER_H

#include <Arduino.h>
#include "state.h"

extern State currentState;
extern String selectedMenu;
extern bool isAbsenMasuk;
extern unsigned long absenStartTime;

// Fungsi menu utama dan absensi
void tampilkanMenuUtama();
void eksekusiPilihanMenu();
void tampilkanMenuAbsenMasuk();
void tampilkanMenuAbsenPulang();

// Fungsi mahasiswa baru
void mulaiPendaftaranMahasiswa();
void tampilkanFormDataMahasiswa();  

// ✅ Tambahan untuk fitur edit mahasiswa
void tampilkanMenuEditUser();
void mulaiEditMahasiswa();
void tampilkanFormEditMahasiswa();

#endif
