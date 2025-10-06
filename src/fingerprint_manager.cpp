#include "fingerprint_manager.h"
#include "menu_handler.h"
#include "state.h"
#include "admin_handler.h"
#include "ntp_manager.h" // <--- Tambahkan ini
#include "db_client.h"

const int AMBANG_CONFIDENCE = 70;  // Nilai minimum confidence untuk validasi sidik jari

// Fungsi fingerprint untuk Mahasiswa
bool enrollFingerprintMahasiswa(uint16_t id, bool* dibatalkan) {
  Serial.println("======= Pendaftaran Sidik Jari Mahasiswa =======");
  Serial.println("Sidik jari akan di scan sebanyak 3 kali");
  Serial.println("Pastikan scan sidik jari yang sama");
  Serial.println("Pastikan sidik jari yang ingin di scan kering");
  Serial.println("Jika sudah, silakan scan sidik jari anda!");
  Serial.println("=========================================");

  for (int i = 1; i <= 3; i++) {
    Serial.print("Letakkan jari ke-"); Serial.println(i);
    while (finger.getImage() != FINGERPRINT_OK) { //
      uint8_t index = keyPad.getKey();
      if (index < 16) {
        char key = keys[index];
        if (key == 'B') {
          Serial.println("🔙 Batal, kembali ke konfirmasi data.");
          if (dibatalkan) *dibatalkan = true;
          return false;
        }
      }
    }

    if (finger.image2Tz(i == 1 ? 1 : 2) != FINGERPRINT_OK) { //
      Serial.println("❌ Gagal mengubah gambar sidik jari ke template.");
      return false;
    }

    if (i == 2 && finger.createModel() != FINGERPRINT_OK) { //
      Serial.println("❌ Gagal membuat model sidik jari.");
      return false;
    }

    while (finger.getImage() != FINGERPRINT_NOFINGER) delay(50); //
    delay(500);

    Serial.println("======= Pendaftaran Sidik Jari Mahasiswa =======");
    if (i == 1) Serial.println("Proses scan sidik jari pertama berhasil");
    else if (i == 2) Serial.println("Proses scan sidik jari kedua berhasil");
    else {
      Serial.println("Proses scan sidik jari ketiga berhasil");
      Serial.println("Proses pendaftaran sidik jari mahasiswa selesai");
    }
    Serial.println("=========================================");
  }

  if (finger.storeModel(id) == FINGERPRINT_OK) { //
    Serial.println("✅ Sidik jari berhasil disimpan.");
    return true;
  } else {
    Serial.println("❌ Gagal menyimpan sidik jari ke memori sensor.");
    return false;
  }
}

// Fungsi fingerprint untuk Admin (dengan deteksi tombol 'B')
bool enrollFingerprintAdmin(uint16_t id, bool* dibatalkan) {
  Serial.println("======= Pendaftaran Sidik Jari Admin =======");
  Serial.println("Sidik jari akan di scan sebanyak 3 kali");
  Serial.println("Pastikan scan sidik jari yang sama");
  Serial.println("Pastikan sidik jari yang ingin di scan kering");
  Serial.println("Jika sudah, silakan scan sidik jari anda!");
  Serial.println("===========================================");

  for (int i = 1; i <= 3; i++) {
    Serial.print("Letakkan jari ke-"); Serial.println(i);
    while (finger.getImage() != FINGERPRINT_OK) { //
      uint8_t index = keyPad.getKey();
      if (index < 16) {
        char key = keys[index];
        if (key == 'B') {
          Serial.println("🔙 Batal, kembali ke konfirmasi data.");
          if (dibatalkan) *dibatalkan = true;
          return false;
        }
      }
      delay(50);
    }

    if (finger.image2Tz(i == 1 ? 1 : 2) != FINGERPRINT_OK) { //
      Serial.println("❌ Gagal mengubah gambar sidik jari ke template.");
      return false;
    }
    if (i == 2 && finger.createModel() != FINGERPRINT_OK) { //
      Serial.println("❌ Gagal membuat model sidik jari.");
      return false;
    }
    Serial.println("Lepaskan jari...");
    while (finger.getImage() != FINGERPRINT_NOFINGER) delay(50); //
    delay(500);

    Serial.println("======= Pendaftaran Sidik Jari Admin =======");
    if (i == 1) Serial.println("Proses scan sidik jari pertama berhasil");
    else if (i == 2) Serial.println("Proses scan sidik jari kedua berhasil");
    else {
      Serial.println("Proses scan sidik jari ketiga berhasil");
      Serial.println("Proses pendaftaran sidik jari admin selesai");
    }
    Serial.println("===========================================");
  }

  if (finger.storeModel(id) == FINGERPRINT_OK) { //
    Serial.println("✅ Sidik jari berhasil disimpan.");
    return true;
  } else {
    Serial.println("❌ Gagal menyimpan sidik jari ke memori sensor.");
    return false;
  }
}

// Fungsi absen masuk/pulang (tetap)
void handleAbsenLoop() {
  static bool menungguLepasJari = false;
  static unsigned long waktuTampilkanMulai = 0;
  static bool sedangTampilkanData = false;

  uint8_t index = keyPad.getKey();
  if (index < 16) {
    char key = keys[index];
    while (keyPad.getKey() != 16) delay(10);
    if (key == 'B') {
      Serial.println("🔙 Kembali ke menu utama...");
      finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE); //
      tampilkanMenuUtama();
      sedangTampilkanData = false;
      menungguLepasJari = false;
      return;
    }
  }

  if (millis() - absenStartTime >= 60000) {
    Serial.println("⏰ Waktu absen habis, kembali ke menu.");
    finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE); //
    tampilkanMenuUtama();
    sedangTampilkanData = false;
    menungguLepasJari = false;
    return;
  }

  if (menungguLepasJari) {
    if (finger.getImage() == FINGERPRINT_NOFINGER) { //
      menungguLepasJari = false;
    }
    return;
  }

  if (sedangTampilkanData && millis() - waktuTampilkanMulai < 5000) {
    return;
  } else if (sedangTampilkanData) {
    Serial.println(isAbsenMasuk ? "=========== Absen Masuk ===========" : "=========== Absen Pulang ===========");
    Serial.println("Silakan scan sidik jari");
    Serial.println("==================================");
    sedangTampilkanData = false;
  }

  if (finger.getImage() != FINGERPRINT_OK) return; //
  if (finger.image2Tz() != FINGERPRINT_OK) return; //

  if (finger.fingerSearch() != FINGERPRINT_OK || finger.confidence < AMBANG_CONFIDENCE) { //
    Serial.println(isAbsenMasuk ? "========= Absen Masuk Gagal ========" : "========= Absen Pulang Gagal =======");
    Serial.println("Silakan scan sidik jari kembali");
    Serial.println("==================================");
    menungguLepasJari = true;
    return;
  }

  Serial.print("📌 Finger ID terdeteksi: ");
  Serial.println(finger.fingerID);

  // Convert fingerprint ID to string for database lookup
  String fingerprintId = String(finger.fingerID);
  
  // Look up student via database
  StudentDB student;
  if (dbGetStudentByFingerprint(fingerprintId, &student)) {
    // Check today's attendance to determine if this is masuk or pulang
    String currentTime = getCurrentFormattedTime();
    String currentDate = getCurrentFormattedDate();
    String timestamp = currentDate + " " + currentTime;
    uint8_t lastAttendanceType = 0;
    bool isCheckIn = true; // Default to check in
    
    if (dbCheckTodayAttendance(student.nim, timestamp, lastAttendanceType)) {
      if (lastAttendanceType == 1) {
        // Already checked in today, this should be check out
        isCheckIn = false;
      } else if (lastAttendanceType == 2) {
        // Already checked out today, this should be check in
        isCheckIn = true;
      } else {
        // No attendance today, use the original isAbsenMasuk flag
        isCheckIn = isAbsenMasuk;
      }
    } else {
      // If check failed, use original flag
      isCheckIn = isAbsenMasuk;
    }

    // Student attendance
    Serial.println(isCheckIn ? "======= Absen Masuk Berhasil =======" : "======= Absen Pulang Berhasil =======");
    Serial.print("Nama : "); Serial.println(student.nama);
    Serial.print("NIM  : "); Serial.println(student.nim);
    Serial.print("Kelas: "); Serial.println(student.kelas);
    Serial.println(" "); // Baris kosong sesuai format

    // Ambil dan tampilkan waktu dari NTP Manager
    Serial.println(currentTime);
    Serial.println(currentDate);

    Serial.println("==================================");

    // Send attendance to database using the determined check in/out status
    if (dbSendAttendance(student.nim, timestamp, isCheckIn)) {
      Serial.println("✅ Absensi berhasil dikirim ke database");
    } else {
      Serial.println("⚠️ Absensi dicatat lokal, gagal kirim ke database");
    }

    sedangTampilkanData = true;
    waktuTampilkanMulai = millis();
    absenStartTime = millis();
    menungguLepasJari = true;
    finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);
  } else {
    Serial.println("❌ Fingerprint tidak ditemukan di database.");
    Serial.println("Silakan coba lagi atau hubungi admin.");
    menungguLepasJari = true;
  }
}

// 🔥 Perbaikan handleVerifikasiAdmin dengan deteksi durasi D ditekan terus menerus
void handleVerifikasiAdmin() {
  Serial.println("========== Verifikasi Admin ==========");
  Serial.println("Silakan scan sidik jari admin lain untuk");
  Serial.println("verifikasi daftar sebagai admin.");
  Serial.println("=======================================");

  unsigned long pressStartTime = 0;
  bool dPressed = false;

  while (true) {
    uint8_t index = keyPad.getKey();
    if (index < 16) {
      char key = keys[index];
      if (key == 'B') {
        Serial.println("🔙 Kembali ke menu utama...");
        tampilkanMenuUtama();
        return;
      }
      if (key == 'D') {
        if (!dPressed) {
          dPressed = true;
          pressStartTime = millis();
        } else if (millis() - pressStartTime >= 10000) {
          Serial.println("✅ Bypass verifikasi berhasil!");
          while (keyPad.getKey() == index) delay(10);
          modeAlfabet = true;
          inputText = "";
          admin = Admin();
          fieldIndex = 0;
          currentAdminField = &admin.nama;

          currentState = ISI_DATA_ADMIN;
          tampilkanFormDataAdmin();
          return;
        }
      } else {
        dPressed = false;
      }
    } else {
      dPressed = false;
    }

    if (finger.getImage() == FINGERPRINT_OK && finger.image2Tz() == FINGERPRINT_OK && finger.fingerSearch() == FINGERPRINT_OK) { //
      if (finger.confidence > AMBANG_CONFIDENCE) {
        // Admin verification - keep using local admin data for now
        // (Admin functionality not implemented in database)
        Serial.println("✅ Verifikasi admin berhasil.");
        modeAlfabet = true;
        inputText = "";
        admin = Admin();
        fieldIndex = 0;
        currentAdminField = &admin.nama;

        currentState = ISI_DATA_ADMIN;
        tampilkanFormDataAdmin();
        return;
        Serial.println("❌ Fingerprint ditemukan tapi bukan admin.");
      } else {
        Serial.println("❌ Confidence sidik jari terlalu rendah.");
      }
    }

    delay(100);
  }
}