#include "admin_handler.h"
#include "state.h"
#include "input_handler.h"
#include "fingerprint_manager.h"
#include "menu_handler.h"
#include "api_client.h"

extern String inputText;
extern bool modeAlfabet;
extern char lastKeyPressed;
extern int pressCount;
extern void commitLastChar();
extern void handleAlfabetInput(char key);

// ================= FORM PENDAFTARAN ADMIN =================
void tampilkanFormDataAdmin() {
  Serial.println("======= Pendaftaran Admin =======");
  Serial.print("Nama: "); Serial.println(admin.nama);
  Serial.print("NIK : "); Serial.println(admin.nik);
  Serial.println("=================================");
  if (fieldIndex < 2) {
    Serial.println("Masukkan data. Tekan '#' untuk lanjut, 'B' untuk kembali.");
    Serial.println("Tekan tombol ‘C’ selama 3 detik untuk melihat keterangan semua tombol");
  } else {
    Serial.println("Tekan tombol 'A' setelah semua data terisi untuk konfirmasi");
  }
}

void handleInputFormAdmin(char key) {
  if (currentState == KONFIRMASI_ADMIN) {
    if (key == 'A') {
      currentState = ISI_FINGERPRINT_ADMIN;

      bool dibatalkan = false;
      bool berhasil = enrollFingerprintAdmin(nextAdminFingerID, &dibatalkan);
      if (dibatalkan) {
        Serial.println("🔙 Kembali ke konfirmasi data admin (tombol B ditekan)...");
        currentState = KONFIRMASI_ADMIN;
        tampilkanFormDataAdmin();
        return;
      }
      if (berhasil) {
        // Create admin via API
        ApiUser apiUser;
        apiUser.nama = admin.nama;
        apiUser.email = admin.nik + "@admin.default"; // Default email
        apiUser.password = "default123"; // Default password
        apiUser.isAdmin = 1;
        apiUser.isDosen = 0;
        apiUser.fingerprint = String(nextAdminFingerID);
        apiUser.kelas = 0; // Default class for admin
        apiUser.createdAt = "2024-01-01T00:00:00Z"; // Default timestamp
        apiUser.updatedAt = "2024-01-01T00:00:00Z"; // Default timestamp

        ApiUser createdUser;
        if (apiCreateUser(apiUser, &createdUser)) {
            admin.sidikJariID = nextAdminFingerID;
            Serial.println("✅ Pendaftaran Admin Berhasil!");
            Serial.println("✅ Data berhasil disimpan ke server");
        } else {
            Serial.println("❌ Gagal menyimpan ke server, data disimpan lokal");
            admin.sidikJariID = nextAdminFingerID;
            daftarAdminIDs.push_back(admin.sidikJariID);
            daftarAdminData.push_back(admin);
        }
        
        nextAdminFingerID++;
        delay(5000);
        tampilkanMenuUtama();
      } else {
        Serial.println("❌ Pendaftaran sidik jari admin gagal, silakan ulangi.");
        currentState = KONFIRMASI_ADMIN;
        tampilkanFormDataAdmin();
      }
    } else if (key == 'B') {
      fieldIndex = 1;
      currentAdminField = &admin.nik;
      inputText = admin.nik;
      currentState = ISI_DATA_ADMIN;
      modeAlfabet = true;
      tampilkanFormDataAdmin();
    } else {
      Serial.println("⚠️ Hanya tombol A (Konfirmasi) dan B (Kembali) yang diizinkan di sini.");
    }
    return;  // 🔒 Menghentikan input lain saat konfirmasi admin
  }

  if (key == 'C') {
    commitLastChar();
    modeAlfabet = !modeAlfabet;
    lastKeyPressed = '\0';
    pressCount = 0;
    Serial.println(modeAlfabet ? "📝 Mode alfabet aktif" : "🔢 Mode angka aktif");
    return;
  }

  if (modeAlfabet && key >= '2' && key <= '9') {
    handleAlfabetInput(key);
    return;
  }

  if (key == 'D') {
    commitLastChar();
    inputText += ' ';
    Serial.print("📥 Input sekarang: ");
    Serial.println(inputText);
    return;
  }

  if (key == '*') {
    commitLastChar();
    if (inputText.length())
      inputText.remove(inputText.length() - 1);
    Serial.print("🗑️ Dihapus: ");
    Serial.println(inputText);
    return;
  }

  if (key == 'B') {
    commitLastChar();
    if (fieldIndex == 0) {
      admin.nama = "";
      tampilkanMenuUtama();
      return;
    }
    if (fieldIndex == 1) {
      admin.nik = "";
    }
    fieldIndex--;
    if (fieldIndex == 0) {
      currentAdminField = &admin.nama;
      inputText = admin.nama;
    }
    modeAlfabet = true;
    tampilkanFormDataAdmin();
    return;
  }

  if (key == '#') {
    commitLastChar();
    *currentAdminField = inputText;
    inputText = "";
    fieldIndex++;
    if (fieldIndex == 1) {
      currentAdminField = &admin.nik;
    } else {
      currentState = KONFIRMASI_ADMIN;
    }
    tampilkanFormDataAdmin();
    return;
  }

  if (!modeAlfabet && key >= '0' && key <= '9') {
    inputText += key;
    Serial.print("📥 Input sekarang: ");
    Serial.println(inputText);
  }
}

// ================= FUNGSI EDIT ADMIN =================
void tampilkanFormEditAdmin() {
  Serial.println("======= Ganti Data Admin ========");
  Serial.print("Nama: "); Serial.println(admin.nama);
  Serial.print("NIK : "); Serial.println(admin.nik);
  Serial.println("=================================");
  if (fieldIndex < 2) {
    Serial.println("Masukkan data baru. Tekan '#' untuk lanjut, 'B' untuk kembali.");
  } else {
    Serial.println("Tekan tombol 'A' setelah semua data baru terisi untuk konfirmasi");
  }
}

void mulaiEditAdmin() {
  currentState = EDIT_DATA_ADMIN;
  fieldIndex = 0;
  inputText = admin.nama;
  currentAdminField = &admin.nama;
  modeAlfabet = true;
  tampilkanFormEditAdmin();
}

void handleInputEditAdmin(char key) {
  if (currentState == KONFIRMASI_EDIT_ADMIN) {
    if (key == 'A') {
      // Update admin via API
      ApiUser apiUser;
      apiUser.nama = admin.nama;
      apiUser.email = admin.nik + "@admin.default"; // Default email
      apiUser.password = "default123"; // Default password
      apiUser.isAdmin = 1;
      apiUser.isDosen = 0;
      apiUser.fingerprint = String(admin.sidikJariID);
      apiUser.userId = currentAdminUserId; // Use stored user ID
      apiUser.kelas = 0; // Default class for admin
      apiUser.createdAt = "2024-01-01T00:00:00Z"; // Default timestamp
      apiUser.updatedAt = "2024-01-01T00:00:00Z"; // Default timestamp

      if (apiUpdateUser(apiUser)) {
        Serial.println("✅ Data Admin berhasil diperbarui di server!");
      } else {
        Serial.println("❌ Gagal memperbarui di server, data diperbarui lokal");
        bool updated = false;
        for (auto& a : daftarAdminData) {
          if (a.sidikJariID == admin.sidikJariID) {
            a = admin;
            updated = true;
            break;
          }
        }
        if (!updated) {
          Serial.println("❌ Gagal menemukan data admin untuk diperbarui.");
        }
      }

      delay(2000);
      tampilkanMenuUtama();
      return;
    }

    if (key == 'B') {
      fieldIndex = 1;
      currentAdminField = &admin.nik;
      inputText = admin.nik;
      currentState = EDIT_DATA_ADMIN;
      tampilkanFormEditAdmin();
      return;
    }
  }

  if (key == 'C') {
    commitLastChar();
    modeAlfabet = !modeAlfabet;
    lastKeyPressed = '\0';
    pressCount = 0;
    Serial.println(modeAlfabet ? "📝 Mode alfabet aktif" : "🔢 Mode angka aktif");
    return;
  }

  if (modeAlfabet && key >= '2' && key <= '9') {
    handleAlfabetInput(key);
    return;
  }

  if (key == 'D') {
    commitLastChar();
    inputText += ' ';
    Serial.print("📥 Input sekarang: ");
    Serial.println(inputText);
    return;
  }

  if (key == '*') {
    commitLastChar();
    if (inputText.length())
      inputText.remove(inputText.length() - 1);
    Serial.print("🗑️ Dihapus: ");
    Serial.println(inputText);
    return;
  }

  if (key == 'B') {
    commitLastChar();
    if (fieldIndex == 0) {
      tampilkanMenuUtama();
      return;
    }
    if (fieldIndex == 1) {
      admin.nik = "";
      fieldIndex = 0;
      currentAdminField = &admin.nama;
      inputText = admin.nama;
    }
    tampilkanFormEditAdmin();
    return;
  }

  if (key == '#') {
    commitLastChar();
    *currentAdminField = inputText;
    inputText = "";
    fieldIndex++;
    if (fieldIndex == 1) currentAdminField = &admin.nik;
    else currentState = KONFIRMASI_EDIT_ADMIN;
    tampilkanFormEditAdmin();
    return;
  }

  if (!modeAlfabet && key >= '0' && key <= '9') {
    inputText += key;
    Serial.print("📥 Input sekarang: ");
    Serial.println(inputText);
  }
}
