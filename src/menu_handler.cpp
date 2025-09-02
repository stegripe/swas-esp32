#include "menu_handler.h"
#include "state.h"
#include "input_handler.h"
#include "admin_handler.h"
#include "fingerprint_manager.h"  // ✅ Tambahan agar bisa panggil handleVerifikasiAdmin()
#include "display_manager.h"      // ✅ TAMBAHAN BARU - Include display manager untuk clear LCD
#include <Arduino.h>

extern bool modeAlfabet;  // ← dibutuhkan agar bisa set alfabet default

// Fungsi: Tampilkan menu utama di serial monitor
void tampilkanMenuUtama() {
  Serial.println("=== SMART WORKSPACE ATTENDANCE ===");
  Serial.println("1. Absen Masuk");
  Serial.println("2. Absen Pulang");
  Serial.println("3. Mendaftar Sidik Jari Mahasiswa");
  Serial.println("4. Mendaftar Admin");
  Serial.println("5. Ganti Informasi User");
  Serial.println("Pilih [1–5], tekan C untuk input teks");
  currentState = MENU_UTAMA;
}

// Fungsi: Eksekusi pilihan berdasarkan input menu
void eksekusiPilihanMenu() {
  // ✅ TAMBAHAN BARU - Clear LCD screen sebelum masuk ke menu pilihan
  clearLCDScreen();
  
  if (selectedMenu == "3") {
    mulaiPendaftaranMahasiswa();
    return;
  }

  if (selectedMenu == "4") {
    currentState = VERIFIKASI_ADMIN;
    handleVerifikasiAdmin();  // ✅ Panggil fungsi verifikasi admin dengan tombol 'B' aktif
    return;
  }

  if (selectedMenu == "1") {
    tampilkanMenuAbsenMasuk();
    return;
  }

  if (selectedMenu == "2") {
    tampilkanMenuAbsenPulang();
    return;
  }

  if (selectedMenu == "5") {
    tampilkanMenuEditUser();
    return;
  }
}

// Fungsi: Tampilkan sub-menu edit user
void tampilkanMenuEditUser() {
  currentState = SUBMENU_EDIT_USER; // ✅ Set state khusus
  Serial.println("======= Ganti Informasi User =======");
  Serial.println("1. Ganti data Mahasiswa");
  Serial.println("2. Ganti data Admin");
  Serial.println("====================================");
  Serial.println("Pilih [1–2], tekan B untuk kembali");
}

// Fungsi: Reset data dan mulai proses pendaftaran mahasiswa
void mulaiPendaftaranMahasiswa() {
  mhs = Mahasiswa();        // reset data
  inputText = "";           // reset input
  fieldIndex = 0;
  currentField = &mhs.nama; // mulai dari nama
  modeAlfabet = true;       // aktifkan mode alfabet default
  currentState = ISI_DATA;
  tampilkanFormDataMahasiswa();
}

// Fungsi: Mulai edit data mahasiswa
void mulaiEditMahasiswa() {
  mhs = Mahasiswa();        // reset data lama
  inputText = "";
  fieldIndex = 0;
  currentField = &mhs.nama;
  modeAlfabet = true;
  currentState = EDIT_DATA_MHS;
  tampilkanFormEditMahasiswa();
}

// Fungsi: Tampilkan form data mahasiswa (saat input baru)
void tampilkanFormDataMahasiswa() {
  Serial.println("======= Pendaftaran Mahasiswa =======");
  Serial.print("Nama : "); Serial.println(mhs.nama);
  Serial.print("NIM  : "); Serial.println(mhs.nim);
  Serial.print("Kelas: "); Serial.println(mhs.kelas);
  Serial.println("=====================================");
  if (fieldIndex < 3)
    Serial.println("Masukkan data. Tekan '#' untuk lanjut, 'B' untuk kembali.");
  else
    Serial.println("Tekan tombol 'A' setelah semua data terisi untuk konfirmasi");
}

// Fungsi: Tampilkan form edit mahasiswa
void tampilkanFormEditMahasiswa() {
  Serial.println("======= Ganti Data Mahasiswa ========");
  Serial.print("Nama : "); Serial.println(mhs.nama);
  Serial.print("NIM  : "); Serial.println(mhs.nim);
  Serial.print("Kelas: "); Serial.println(mhs.kelas);
  Serial.println("=====================================");
  if (fieldIndex < 3)
    Serial.println("Masukkan data baru. Tekan '#' untuk lanjut, 'B' untuk kembali.");
  else
    Serial.println("Tekan tombol 'A' setelah semua data baru terisi untuk konfirmasi");
}

// Fungsi tampilan absen
void tampilkanMenuAbsenMasuk() {
  currentState = ABSEN;
  isAbsenMasuk = true;
  absenStartTime = millis();
  Serial.println("=========== Absen Masuk ===========");
  Serial.println("Silakan scan sidik jari");
  Serial.println("==================================");
}

void tampilkanMenuAbsenPulang() {
  currentState = ABSEN;
  isAbsenMasuk = false;
  absenStartTime = millis();
  Serial.println("=========== Absen Pulang ===========");
  Serial.println("Silakan scan sidik jari");
  Serial.println("==================================");
}
