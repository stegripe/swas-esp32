#include "storage_manager.h"

#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <algorithm>

#include "state.h"

static const char* kStorageNamespace = "/data.json";

Mahasiswa mhs;
Admin admin;

State currentState = MENU_UTAMA;
String selectedMenu = "";
bool isAbsenMasuk = false;
unsigned long absenStartTime = 0;

uint16_t nextFingerID = 1;
uint16_t nextMahasiswaFingerID = 1;
uint16_t nextAdminFingerID = 50;

String inputText = "";
int fieldIndex = 0;
String* currentField = &mhs.nama;
String* currentAdminField = &admin.nama;

I2CKeyPad keyPad(0x20);
char keys[] = "123A456B789C*0#DNF";

std::vector<uint16_t> daftarAdminIDs;
std::vector<uint16_t> daftarMahasiswaIDs;
std::vector<Admin> daftarAdminData;
std::vector<Mahasiswa> daftarMahasiswaData;
std::vector<FingerprintRecord> daftarFingerprintRecords;
int indeksMahasiswaEdit = -1;
int indeksAdminEdit = -1;
uint32_t currentMahasiswaUserId = 0;
uint32_t currentAdminUserId = 0;
uint16_t currentMahasiswaFingerprintId = 0;
uint16_t currentAdminFingerprintId = 0;
String currentMahasiswaEmail = "";
String currentAdminEmail = "";

void initStorage() {
  // SPIFFS initialization kept for potential offline fallback
  if (!SPIFFS.begin(true)) {
    Serial.println("⚠️ SPIFFS tidak tersedia, menggunakan mode online saja.");
  } else {
    Serial.println("✅ SPIFFS tersedia untuk fallback offline.");
  }
}

void loadPersistentState() {
  // Load only essential local state, user data now comes from API
  StaticJsonDocument<4096> doc;
  if (!SPIFFS.exists(kStorageNamespace)) {
    Serial.println("ℹ️ File data.json belum ada, menggunakan default.");
    return;
  }

  File file = SPIFFS.open(kStorageNamespace, "r");
  if (!file) {
    Serial.println("❌ Gagal membuka file data.json.");
    return;
  }

  DeserializationError err = deserializeJson(doc, file);
  if (err) {
    Serial.print("❌ Gagal parse data.json: ");
    Serial.println(err.c_str());
    file.close();
    return;
  }
  file.close();

  // Only load fingerprint ID counters for local sensor management
  nextFingerID = doc["nextFingerID"] | nextFingerID;
  nextMahasiswaFingerID = doc["nextMahasiswaFingerID"] | nextMahasiswaFingerID;
  nextAdminFingerID = doc["nextAdminFingerID"] | nextAdminFingerID;

  Serial.println("✅ Local state loaded, user data will be fetched from API");
}

static bool writeJsonToFile(const JsonDocument& doc) {
  File file = SPIFFS.open(kStorageNamespace, "w");
  if (!file) {
    Serial.println("❌ Gagal membuka file data.json untuk menulis.");
    return false;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println("❌ Gagal menulis JSON ke file.");
    file.close();
    return false;
  }

  file.close();
  return true;
}

static void fillJsonDocument(JsonDocument& doc) {
  doc.clear();
  doc["nextFingerID"] = nextFingerID;
  doc["nextMahasiswaFingerID"] = nextMahasiswaFingerID;
  doc["nextAdminFingerID"] = nextAdminFingerID;

  JsonArray adminArray = doc["adminIDs"].to<JsonArray>();
  for (uint16_t id : daftarAdminIDs) {
    adminArray.add(id);
  }

  JsonArray mahasiswaArray = doc["mahasiswaIDs"].to<JsonArray>();
  for (uint16_t id : daftarMahasiswaIDs) {
    mahasiswaArray.add(id);
  }

  JsonArray admins = doc["admins"].to<JsonArray>();
  for (const Admin& a : daftarAdminData) {
    JsonObject obj = admins.add<JsonObject>();
    obj["nama"] = a.nama;
    obj["nik"] = a.nik;
    obj["email"] = a.email;
    obj["id"] = a.sidikJariID;
  }

  JsonArray mahasiswa = doc["mahasiswa"].to<JsonArray>();
  for (const Mahasiswa& m : daftarMahasiswaData) {
    JsonObject obj = mahasiswa.add<JsonObject>();
    obj["nama"] = m.nama;
    obj["nim"] = m.nim;
    obj["kelas"] = m.kelas;
    obj["email"] = m.email;
    obj["isDosen"] = m.isDosen;
    obj["id"] = m.sidikJariID;
  }

  JsonArray fingerprints = doc["fingerprints"].to<JsonArray>();
  for (const FingerprintRecord& record : daftarFingerprintRecords) {
    JsonObject obj = fingerprints.add<JsonObject>();
    obj["fingerprintId"] = record.fingerprintId;
    obj["userId"] = record.userId;
    obj["isAdmin"] = record.isAdmin;
    obj["isDosen"] = record.isDosen;
  }
}

void persistUsers() {
  // User data now managed by API, only save local state
  StaticJsonDocument<512> doc;
  doc["nextFingerID"] = nextFingerID;
  doc["nextMahasiswaFingerID"] = nextMahasiswaFingerID;
  doc["nextAdminFingerID"] = nextAdminFingerID;
  
  if (writeJsonToFile(doc)) {
    Serial.println("💾 Local state tersimpan.");
  }
}

void persistFingerprintIds() {
  // Same as persistUsers now
  persistUsers();
}

void persistAll() {
  // Same as persistUsers now
  persistUsers();
}

void ensureFingerprintTemplatesSynced() {
  Serial.println("ℹ️ Fingerprint templates assumed synchronized.");
}

void addOrUpdateFingerprintRecord(const FingerprintRecord &record) {
  for (auto &existing : daftarFingerprintRecords) {
    if (existing.fingerprintId == record.fingerprintId) {
      existing = record;
      return;
    }
  }
  daftarFingerprintRecords.push_back(record);
}

bool removeFingerprintRecord(uint16_t fingerprintId) {
  for (auto it = daftarFingerprintRecords.begin(); it != daftarFingerprintRecords.end(); ++it) {
    if (it->fingerprintId == fingerprintId) {
      daftarFingerprintRecords.erase(it);
      return true;
    }
  }
  return false;
}

bool getFingerprintRecord(uint16_t fingerprintId, FingerprintRecord *outRecord) {
  for (const auto &record : daftarFingerprintRecords) {
    if (record.fingerprintId == fingerprintId) {
      if (outRecord != nullptr) {
        *outRecord = record;
      }
      return true;
    }
  }
  return false;
}

const std::vector<uint16_t> &getAdminFingerprintIds() { return daftarAdminIDs; }

const std::vector<uint16_t> &getMahasiswaFingerprintIds() { return daftarMahasiswaIDs; }

