#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Fingerprint.h>
#include "I2CKeyPad.h"

#include "input_handler.h"
#include "state.h"
#include "menu_handler.h"
#include "fingerprint_manager.h"
#include "admin_handler.h"
#include "wifi_manager.h" // Sudah ada
#include "ntp_manager.h"  // <--- Tambahkan ini
#include "display_manager.h"  // ✅ TAMBAHAN BARU - Include display manager

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

bool bypassAktif = false;
bool tombolDitekan = false;
unsigned long tombolDitekanMulai = 0;

// Definisikan SSID dan Password WiFi Anda di sini
// Ganti dengan kredensial WiFi yang sesuai sebelum mengunggah ke ESP32 Anda
const char* WIFI_SSID = "Makan bang";     // Ganti dengan SSID WiFi Anda
const char* WIFI_PASSWORD = "asdqwe123"; // Ganti dengan Password WiFi Anda

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  keyPad.begin();
  keyPad.setDebounceThreshold(50);

  // ✅ TAMBAHAN BARU - Inisialisasi LCD TFT
  initDisplay();

  // Inisialisasi WiFi
  setupWiFi(WIFI_SSID, WIFI_PASSWORD); // Panggil fungsi setup WiFi

  // Inisialisasi NTP setelah WiFi terhubung
  // Pastikan WiFi sudah terhubung sebelum memanggil setupNTP()
  if (WiFi.status() == WL_CONNECTED) {
    setupNTP(); //
  } else {
    Serial.println("⛔ Tidak dapat menyinkronkan waktu NTP karena WiFi tidak terhubung."); //
  }

  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);
  delay(10);

  if (finger.verifyPassword()) {
    Serial.println("✅ Sensor sidik jari terdeteksi.");
    if (finger.emptyDatabase() == FINGERPRINT_OK) { //
      Serial.println("🧹 Semua data fingerprint dihapus dari sensor.");
    } else {
      Serial.println("❌ Gagal menghapus data fingerprint.");
    }
  } else {
    Serial.println("❌ Gagal mendeteksi sensor fingerprint.");
  }

  tampilkanMenuUtama();
  // ✅ TAMBAHAN BARU - Tampilkan menu utama di LCD juga
  tampilkanMenuUtama(); // Untuk LCD TFT
}

void loop() {
  handleWiFi(); // Panggil fungsi untuk memantau dan mengelola koneksi WiFi

  if (modeAlfabet && lastKeyPressed != '\0' && (millis() - lastPressTime > ALPHABET_TIMEOUT)) {
    commitLastChar();
  }

  if (currentState == ABSEN) {
    handleAbsenLoop();
    return;
  }

  if (currentState == VERIFIKASI_ADMIN) {
    uint8_t keyIndex = keyPad.getKey();
    if (keyIndex < 16) {
      char key = keys[keyIndex];
      if (key == 'D') {
        if (!tombolDitekan) {
          tombolDitekan = true;
          tombolDitekanMulai = millis();
        } else if (millis() - tombolDitekanMulai >= 10000) {
          bypassAktif = true;
        }
      } else {
        tombolDitekan = false;
      }
      if (key == 'B') {
        tampilkanMenuUtama();
        // ✅ TAMBAHAN BARU - Update LCD juga saat kembali ke menu
        tampilkanMenuUtama(); // Untuk LCD TFT
        return;
      }
    }

    if (bypassAktif) {
      Serial.println("✅ Bypass verifikasi berhasil!");
      delay(1000);
      admin = Admin();
      inputText = "";
      fieldIndex = 0;
      currentAdminField = &admin.nama;
      modeAlfabet = true;
      currentState = ISI_DATA_ADMIN;
      tampilkanFormDataAdmin();
      bypassAktif = false;
      tombolDitekan = false;
      return;
    }

    if (finger.getImage() == FINGERPRINT_OK && //
        finger.image2Tz() == FINGERPRINT_OK && //
        finger.fingerSearch() == FINGERPRINT_OK) { //
      if (finger.fingerID >= nextAdminFingerID && finger.fingerID < nextFingerID) {
        Serial.println("========= Verifikasi Admin ========");
        Serial.println("Verifikasi berhasil");
        Serial.println("==================================");
        delay(3000);

        admin = Admin();
        inputText = "";
        fieldIndex = 0;
        currentAdminField = &admin.nama;
        modeAlfabet = true;
        currentState = ISI_DATA_ADMIN;
        tampilkanFormDataAdmin();
      } else {
        Serial.println("========= Verifikasi Admin ========");
        Serial.println("❌ Sidik jari bukan milik admin terdaftar!");
        Serial.println("==================================");
        delay(1500);
      }
    }
    return;
  }

  if (currentState == VERIFIKASI_MHS_EDIT || currentState == VERIFIKASI_ADMIN_EDIT) {
    uint8_t keyIndex = keyPad.getKey();
    if (keyIndex < 16) {
      char key = keys[keyIndex];
      if (key == 'B') {
        tampilkanMenuEditUser(); // Kembali ke menu pilih edit user
        currentState = SUBMENU_EDIT_USER; // Pastikan state diubah
        return;
      }
    }

    if (finger.getImage() == FINGERPRINT_OK && //
        finger.image2Tz() == FINGERPRINT_OK && //
        finger.fingerSearch() == FINGERPRINT_OK) { //

      bool verifikasiBerhasil = false;
      int indeksEdit = -1;

      if (currentState == VERIFIKASI_MHS_EDIT && finger.fingerID < nextAdminFingerID) {
        // Verifikasi Mahasiswa
        for (size_t i = 0; i < daftarMahasiswaData.size(); ++i) {
          if (daftarMahasiswaData[i].sidikJariID == finger.fingerID) {
            mhs = daftarMahasiswaData[i];
            indeksMahasiswaEdit = i;
            verifikasiBerhasil = true;
            indeksEdit = i;
            break;
          }
        }
      } else if (currentState == VERIFIKASI_ADMIN_EDIT) {
        // Verifikasi Admin
        for (size_t i = 0; i < daftarAdminData.size(); ++i) {
          if (daftarAdminData[i].sidikJariID == finger.fingerID) {
            admin = daftarAdminData[i];
            indeksAdminEdit = i;
            verifikasiBerhasil = true;
            indeksEdit = i;
            break;
          }
        }
      }

      if (verifikasiBerhasil) {
        Serial.println("✅ Verifikasi berhasil.");
        delay(1000);
        if (currentState == VERIFIKASI_MHS_EDIT) {
          mulaiEditMahasiswa();
        } else {
          mulaiEditAdmin();
        }
      } else {
        Serial.println("❌ Verifikasi gagal. Silakan coba lagi.");
        delay(2000);
        // Tidak perlu mengubah state, tetap di VERIFIKASI...
      }
      return;
    } else {
      Serial.println("🔁 Silakan ulangi scan sidik jari...");
      delay(1000);
    }
    yield(); // Tambahkan ini
    return;
  }

  uint8_t index = keyPad.getKey();
  if (index >= 16) return;

  char key = keys[index];
  while (keyPad.getKey() != 16) delay(10);

  switch (currentState) {
    case MENU_UTAMA:
      if (key == 'C') {
        commitLastChar();
        modeAlfabet = !modeAlfabet;
        Serial.println(modeAlfabet ? "📝 Mode alfabet aktif" : "🔢 Mode angka aktif kembali");
        return;
      }
      if (key >= '1' && key <= '5') {
        commitLastChar();
        modeAlfabet = false;
        selectedMenu = key;
        eksekusiPilihanMenu();
        return;
      }
      break;

    case SUBMENU_EDIT_USER:
      if (key == '1') {
        Serial.println("🧠 Verifikasi sidik jari mahasiswa...");
        currentState = VERIFIKASI_MHS_EDIT;
      } else if (key == '2') {
        Serial.println("🧠 Verifikasi sidik jari admin...");
        currentState = VERIFIKASI_ADMIN_EDIT;
      } else if (key == 'B') {
        tampilkanMenuUtama();
        // ✅ TAMBAHAN BARU - Update LCD juga saat kembali ke menu
        tampilkanMenuUtama(); // Untuk LCD TFT
      }
      return;

    case ISI_DATA:
    case KONFIRMASI_DATA:
      handleInputFormMahasiswa(key);
      return;

    case ISI_DATA_ADMIN:
    case KONFIRMASI_ADMIN:
      handleInputFormAdmin(key);
      return;

    case EDIT_DATA_MHS:
    case KONFIRMASI_EDIT_MHS:
      handleInputEditMahasiswa(key);
      return;

    case EDIT_DATA_ADMIN:
    case KONFIRMASI_EDIT_ADMIN:
      handleInputEditAdmin(key);
      return;
  }
}
