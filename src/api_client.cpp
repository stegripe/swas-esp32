#include "api_client.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "storage_manager.h"
#include "ntp_manager.h"
#include "wifi_manager.h"

static const char* kBaseUrl = "https://swas.stegripe.org";
static const char* kAuthHeader = "87EDPRHV9d6xWC7QiWrIzqclVEmaihUs";

static bool ensureWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  handleWiFi();
  return WiFi.status() == WL_CONNECTED;
}

static void applyCommonHeaders(HTTPClient& http) {
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", kAuthHeader);
}

static bool readResponseJson(HTTPClient& http, JsonDocument& doc) {
  WiFiClient* stream = http.getStreamPtr();
  if (!stream) {
    Serial.println("❌ Tidak bisa mendapatkan stream HTTP.");
    return false;
  }
  DeserializationError err = deserializeJson(doc, *stream);
  if (err) {
    Serial.print("❌ Gagal parse JSON: ");
    Serial.println(err.c_str());
    return false;
  }
  return true;
}

static void fillUserPayload(const ApiUser& user, JsonDocument& doc) {
  doc["email"] = user.email;
  doc["password"] = user.password;
  doc["isAdmin"] = user.isAdmin ? 1 : 0;
  doc["isDosen"] = user.isDosen ? 1 : 0;
  if (user.fingerprint.length()) doc["fingerprint"] = user.fingerprint;
  if (user.nim.length()) doc["nim"] = user.nim;
  if (user.nama.length()) doc["nama"] = user.nama;
  if (user.kelas.length()) doc["kelas"] = user.kelas;
}

bool apiCreateUser(const ApiUser& user, ApiUser* created) {
  if (!ensureWiFi()) return false;

  HTTPClient http;
  String url = String(kBaseUrl) + "/api/esp/users";
  if (!http.begin(url)) {
    Serial.println("❌ Gagal melakukan HTTP begin untuk POST /users");
    return false;
  }

  applyCommonHeaders(http);

  StaticJsonDocument<1024> doc;
  fillUserPayload(user, doc);

  String body;
  serializeJson(doc, body);

  int code = http.POST(body);
  if (code < 200 || code >= 300) {
    Serial.print("❌ POST /users gagal, kode: ");
    Serial.println(code);
    Serial.println(http.getString());
    http.end();
    return false;
  }

  if (created) {
    StaticJsonDocument<1024> resp;
    if (readResponseJson(http, resp)) {
      created->id = resp["userId"] | 0;
      created->fingerprint = resp["fingerprint"].as<String>();
      created->email = resp["email"].as<String>();
      created->nama = resp["nama"].as<String>();
      created->nim = resp["nim"].as<String>();
      created->kelas = resp["kelas"].as<String>();
      created->isAdmin = resp["isAdmin"] | 0;
      created->isDosen = resp["isDosen"] | 0;
    }
  }

  http.end();
  return true;
}

bool apiGetUserByFingerprint(const String& fingerprint, ApiUser* user) {
  if (!ensureWiFi()) return false;
  HTTPClient http;
  String url = String(kBaseUrl) + "/api/esp/users?fingerprint=" + fingerprint;
  if (!http.begin(url)) {
    Serial.println("❌ Gagal HTTP begin GET /users");
    return false;
  }
  applyCommonHeaders(http);

  int code = http.GET();
  if (code == 404) {
    http.end();
    return false;
  }
  if (code < 200 || code >= 300) {
    Serial.print("❌ GET /users gagal, kode: ");
    Serial.println(code);
    Serial.println(http.getString());
    http.end();
    return false;
  }

  if (user) {
    StaticJsonDocument<2048> doc;
    if (readResponseJson(http, doc)) {
      user->id = doc["userId"] | 0;
      user->email = doc["email"].as<String>();
      user->nama = doc["nama"].as<String>();
      user->nim = doc["nim"].as<String>();
      user->kelas = doc["kelas"].as<String>();
      user->fingerprint = doc["fingerprint"].as<String>();
      user->isAdmin = doc["isAdmin"] | 0;
      user->isDosen = doc["isDosen"] | 0;
    }
  }
  http.end();
  return true;
}

bool apiUpdateUser(const ApiUser& user) {
  if (!ensureWiFi()) return false;
  HTTPClient http;
  String url = String(kBaseUrl) + "/api/esp/users";
  if (!http.begin(url)) {
    Serial.println("❌ Gagal HTTP begin PATCH /users");
    return false;
  }
  applyCommonHeaders(http);

  StaticJsonDocument<1024> doc;
  doc["userId"] = user.id;
  fillUserPayload(user, doc);

  String body;
  serializeJson(doc, body);

  int code = http.PATCH(body);
  if (code < 200 || code >= 300) {
    Serial.print("❌ PATCH /users gagal, kode: ");
    Serial.println(code);
    Serial.println(http.getString());
    http.end();
    return false;
  }

  http.end();
  return true;
}

bool apiDeleteUserByFingerprint(const String& fingerprint) {
  if (!ensureWiFi()) return false;
  HTTPClient http;
  String url = String(kBaseUrl) + "/api/esp/users?fingerprint=" + fingerprint;
  if (!http.begin(url)) {
    Serial.println("❌ Gagal HTTP begin DELETE /users");
    return false;
  }
  applyCommonHeaders(http);
  int code = http.sendRequest("DELETE");
  if (code < 200 || code >= 300) {
    Serial.print("❌ DELETE /users gagal, kode: ");
    Serial.println(code);
    Serial.println(http.getString());
    http.end();
    return false;
  }
  http.end();
  return true;
}

bool apiSendAttendance(const String& fingerprint, const String& timestamp, bool absenMasuk) {
  if (!ensureWiFi()) return false;
  HTTPClient http;
  String url = String(kBaseUrl) + "/api/esp/attendances";
  if (!http.begin(url)) {
    Serial.println("❌ Gagal HTTP begin POST /attendances");
    return false;
  }
  applyCommonHeaders(http);

  StaticJsonDocument<512> doc;
  doc["fingerprint"] = fingerprint;
  doc["timestamp"] = timestamp;
  doc["type"] = absenMasuk ? "masuk" : "pulang";

  String body;
  serializeJson(doc, body);

  int code = http.POST(body);
  if (code < 200 || code >= 300) {
    Serial.print("❌ POST /attendances gagal, kode: ");
    Serial.println(code);
    Serial.println(http.getString());
    http.end();
    return false;
  }
  http.end();
  return true;
}

void apiSyncUsersFromServer() {
  if (!ensureWiFi()) return;
  // This function should fetch all users from server - placeholder depending on API support.
}

void apiSyncUsersToServer() {
  if (!ensureWiFi()) return;
  // This function would push local changes to server.
}

