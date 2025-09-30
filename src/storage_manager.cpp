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
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Gagal menginisialisasi SPIFFS.");
  }
}

void loadPersistentState() {
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

  nextFingerID = doc["nextFingerID"] | nextFingerID;
  nextMahasiswaFingerID = doc["nextMahasiswaFingerID"] | nextMahasiswaFingerID;
  nextAdminFingerID = doc["nextAdminFingerID"] | nextAdminFingerID;

  daftarAdminIDs.clear();
  daftarMahasiswaIDs.clear();
  daftarAdminData.clear();
  daftarMahasiswaData.clear();
  daftarFingerprintRecords.clear();

  if (const JsonArray adminArray = doc["adminIDs"].as<JsonArray>()) {
    for (uint16_t id : adminArray) {
      daftarAdminIDs.push_back(id);
    }
  }

  if (const JsonArray mahasiswaArray = doc["mahasiswaIDs"].as<JsonArray>()) {
    for (uint16_t id : mahasiswaArray) {
      daftarMahasiswaIDs.push_back(id);
    }
  }

  if (const JsonArray admins = doc["admins"].as<JsonArray>()) {
    for (const JsonObject obj : admins) {
      Admin a;
      a.nama = obj["nama"].as<const char*>();
      a.nik = obj["nik"].as<const char*>();
      a.email = obj["email"].as<const char*>();
      a.sidikJariID = obj["id"] | 0;
      daftarAdminData.push_back(a);
      daftarAdminIDs.push_back(a.sidikJariID);
    }
  }

  if (const JsonArray mahasiswa = doc["mahasiswa"].as<JsonArray>()) {
    for (const JsonObject obj : mahasiswa) {
      Mahasiswa m;
      m.nama = obj["nama"].as<const char*>();
      m.nim = obj["nim"].as<const char*>();
      m.kelas = obj["kelas"].as<const char*>();
      m.email = obj["email"].as<const char*>();
      m.isDosen = obj["isDosen"] | 0;
      m.sidikJariID = obj["id"] | 0;
      daftarMahasiswaData.push_back(m);
      daftarMahasiswaIDs.push_back(m.sidikJariID);
    }
  }

  if (const JsonArray fingerprints = doc["fingerprints"].as<JsonArray>()) {
    for (const JsonObject obj : fingerprints) {
      FingerprintRecord record;
      record.fingerprintId = obj["fingerprintId"] | 0;
      record.userId = obj["userId"] | 0;
      record.isAdmin = obj["isAdmin"] | false;
      record.isDosen = obj["isDosen"] | false;
      daftarFingerprintRecords.push_back(record);
    }
  }
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
  StaticJsonDocument<4096> doc;
  fillJsonDocument(doc);
  if (writeJsonToFile(doc)) {
    Serial.println("💾 Data users tersimpan.");
  }
}

void persistFingerprintIds() {
  StaticJsonDocument<4096> doc;
  fillJsonDocument(doc);
  if (writeJsonToFile(doc)) {
    Serial.println("💾 Data fingerprint tersimpan.");
  }
}

void persistAll() {
  StaticJsonDocument<4096> doc;
  fillJsonDocument(doc);
  if (writeJsonToFile(doc)) {
    Serial.println("💾 Seluruh data tersimpan.");
  }
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

