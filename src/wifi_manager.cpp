#include "wifi_manager.h"
#include <WiFi.h> // Sertakan kembali untuk memastikan ketersediaan fungsi WiFi

// Variabel lokal untuk menyimpan SSID dan Password
static const char* _ssid;
static const char* _password;

// Fungsi untuk menginisialisasi dan menghubungkan ke WiFi
void setupWiFi(const char* ssid, const char* password) {
  _ssid = ssid;
  _password = password;

  Serial.println("\n");
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(_ssid);

  WiFi.begin(_ssid, _password); // Mulai koneksi WiFi

  // Tunggu hingga terhubung atau timeout (misalnya 10 detik)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Berhasil terhubung ke WiFi!");
    Serial.print("Alamat IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Gagal terhubung ke WiFi.");
    Serial.println("Pastikan SSID dan Password benar dan WiFi tersedia.");
    // Dalam aplikasi nyata, Anda mungkin ingin mencoba lagi atau beralih ke mode AP
  }
}

// Fungsi untuk memantau dan mengelola koneksi WiFi di loop utama
void handleWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ Koneksi WiFi terputus. Mencoba menghubungkan kembali...");
    WiFi.begin(_ssid, _password); // Coba hubungkan kembali
    // Anda bisa menambahkan logika retry yang lebih canggih di sini, seperti delay eksponensial
    // Atau membatasi jumlah percobaan sebelum beralih ke mode error
    delay(5000); // Tunggu beberapa saat sebelum mencoba lagi
  }
}