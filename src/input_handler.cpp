#include "input_handler.h"
#include "state.h"
#include "menu_handler.h"
#include "fingerprint_manager.h"
#include "admin_handler.h"

char lastKeyPressed = '\0';
int pressCount = 0;
unsigned long lastPressTime = 0;
bool modeAlfabet = true;

extern int indeksMahasiswaEdit;

const unsigned long ALPHABET_TIMEOUT = 1500;

const char* keymap[10] = {
    "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"
};

void commitLastChar() {
    if (lastKeyPressed != '\0') {
        const char* group = keymap[lastKeyPressed - '0'];
        int len = strlen(group);
        if (len > 0) {
            char finalChar = toupper(group[(pressCount - 1) % len]);
            inputText += String(finalChar);
            Serial.print("✅ Input terkunci: "); Serial.println(inputText);
        }
        lastKeyPressed = '\0';
        pressCount = 0;
    }
}

void handleAlfabetInput(char key) {
    unsigned long now = millis();
    if (key != lastKeyPressed && lastKeyPressed != '\0') {
        commitLastChar();
    }

    const char* group = keymap[key - '0'];
    int len = strlen(group);
    if (len == 0) return;

    if (key == lastKeyPressed && now - lastPressTime <= ALPHABET_TIMEOUT) {
        pressCount++;
    } else {
        pressCount = 1;
        lastKeyPressed = key;
    }

    lastPressTime = now;
    char currentChar = toupper(group[(pressCount - 1) % len]);
    Serial.print("📟 Input sementara: ");
    Serial.println(inputText + String(currentChar));
}

// ========== Pengisian Form Mahasiswa Baru ==========
void handleInputFormMahasiswa(char key) {
    if (currentState == KONFIRMASI_DATA) {
        if (key == 'A') {
            currentState = ISI_FINGERPRINT;
            bool berhasil = false;
            bool dibatalkan = false;
            do {
                berhasil = enrollFingerprintMahasiswa(nextFingerID, &dibatalkan);
                if (dibatalkan) {
                  currentState = KONFIRMASI_DATA;
                 tampilkanFormDataMahasiswa();
                return;
                }

            } while (!berhasil);

            mhs.sidikJariID = nextFingerID++;
            daftarMahasiswaData.push_back(mhs);

            Serial.println("✅ Pendaftaran Mahasiswa Berhasil!");
            delay(3000);
            tampilkanMenuUtama();
            return;
        } else if (key == 'B') {
            fieldIndex = 2;
            currentField = &mhs.kelas;
            inputText = mhs.kelas;
            currentState = ISI_DATA;
            tampilkanFormDataMahasiswa();
            return;
        }

        // 🛠️ Fix penting: hentikan proses input lain saat konfirmasi
        return;
    }

    if (key == 'C') { commitLastChar(); modeAlfabet = !modeAlfabet; lastKeyPressed = '\0'; pressCount = 0; Serial.println(modeAlfabet ? "📝 Mode alfabet aktif" : "🔢 Mode angka aktif"); return; }
    if (modeAlfabet && key >= '2' && key <= '9') { handleAlfabetInput(key); return; }
    if (key == 'D') { commitLastChar(); inputText += ' '; Serial.print("📥 Input sekarang: "); Serial.println(inputText); return; }
    if (key == '*') { commitLastChar(); if (inputText.length()) inputText.remove(inputText.length() - 1); Serial.print("🗑️ Dihapus: "); Serial.println(inputText); return; }

    if (key == 'B') {
        commitLastChar();
        if (fieldIndex == 0) {
            mhs.nama = "";
            tampilkanMenuUtama();
            return;
        } else if (fieldIndex == 1) {
            mhs.nim = "";
            fieldIndex = 0;
            currentField = &mhs.nama;
            inputText = mhs.nama;
        } else if (fieldIndex == 2) {
            mhs.kelas = "";
            fieldIndex = 1;
            currentField = &mhs.nim;
            inputText = mhs.nim;
        }
        tampilkanFormDataMahasiswa();
        return;
    }

    if (key == '#') {
        commitLastChar();
        *currentField = inputText;
        inputText = "";
        fieldIndex++;
        if (fieldIndex == 1) currentField = &mhs.nim;
        else if (fieldIndex == 2) currentField = &mhs.kelas;
        else currentState = KONFIRMASI_DATA;
        tampilkanFormDataMahasiswa();
        return;
    }

    if (!modeAlfabet && key >= '0' && key <= '9') {
        inputText += key;
        Serial.print("📥 Input sekarang: "); Serial.println(inputText);
    }
}

// ========== Pengisian Form Edit Mahasiswa ==========
void handleInputEditMahasiswa(char key) {
    if (currentState == KONFIRMASI_EDIT_MHS) {
        if (key == 'A') {
            if (indeksMahasiswaEdit >= 0 && indeksMahasiswaEdit < daftarMahasiswaData.size()) {
                mhs.sidikJariID = daftarMahasiswaData[indeksMahasiswaEdit].sidikJariID;
            daftarMahasiswaData[indeksMahasiswaEdit] = mhs;
                Serial.println("✅ Data Mahasiswa berhasil diperbarui!");
            } else {
                Serial.println("❌ Gagal menemukan data mahasiswa untuk diperbarui.");
            }

            indeksMahasiswaEdit = -1;
            delay(2000);
            tampilkanMenuUtama();
            return;
        }

        if (key == 'B') {
            fieldIndex = 2;
            currentField = &mhs.kelas;
            inputText = mhs.kelas;
            currentState = EDIT_DATA_MHS;
            tampilkanFormEditMahasiswa();
            return;
        }

        return; // 🛠️ Fix serupa untuk bagian edit
    }

    if (key == 'C') { commitLastChar(); modeAlfabet = !modeAlfabet; lastKeyPressed = '\0'; pressCount = 0; Serial.println(modeAlfabet ? "📝 Mode alfabet aktif" : "🔢 Mode angka aktif"); return; }
    if (modeAlfabet && key >= '2' && key <= '9') { handleAlfabetInput(key); return; }
    if (key == 'D') { commitLastChar(); inputText += ' '; Serial.print("📥 Input sekarang: "); Serial.println(inputText); return; }
    if (key == '*') { commitLastChar(); if (inputText.length()) inputText.remove(inputText.length() - 1); Serial.print("🗑️ Dihapus: "); Serial.println(inputText); return; }

    if (key == 'B') {
        commitLastChar();
        if (fieldIndex == 0) {
            tampilkanMenuUtama();
            return;
        } else if (fieldIndex == 1) {
            mhs.nim = "";
            fieldIndex = 0;
            currentField = &mhs.nama;
            inputText = mhs.nama;
        } else if (fieldIndex == 2) {
            mhs.kelas = "";
            fieldIndex = 1;
            currentField = &mhs.nim;
            inputText = mhs.nim;
        }
        tampilkanFormEditMahasiswa();
        return;
    }

    if (key == '#') {
        commitLastChar();
        *currentField = inputText;
        inputText = "";
        fieldIndex++;
        if (fieldIndex == 1) currentField = &mhs.nim;
        else if (fieldIndex == 2) currentField = &mhs.kelas;
        else currentState = KONFIRMASI_EDIT_MHS;
        tampilkanFormEditMahasiswa();
        return;
    }

    if (!modeAlfabet && key >= '0' && key <= '9') {
        inputText += key;
        Serial.print("📥 Input sekarang: "); Serial.println(inputText);
    }
}
