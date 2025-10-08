#include "ntp_manager.h"
#include <WiFi.h> // Sertakan kembali untuk memastikan ketersediaan fungsi WiFi

// Konfigurasi NTP
const char* ntpServer = "id.pool.ntp.org"; // Indonesian NTP server pool for better accuracy
const long gmtOffset_sec = 7 * 3600;       // WIB (UTC+7)
const int daylightOffset_sec = 0;          // No daylight saving in Indonesia

// Variabel untuk menyimpan struktur waktu
struct tm timeinfo;

// Fungsi untuk inisialisasi NTP
void setupNTP() {
  Serial.print("Mencoba menyinkronkan waktu dengan NTP server...");
  // Konfigurasi time server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Tunggu hingga waktu tersinkronisasi
  int retry_count = 0;
  while (!getLocalTime(&timeinfo) && retry_count < 10) {
    Serial.print(".");
    delay(1000);
    retry_count++;
  }

  if (retry_count >= 10) {
    Serial.println("\n❌ Gagal mendapatkan waktu dari NTP server.");
    // Anda bisa menambahkan logika fallback di sini, misalnya menggunakan waktu internal ESP32
  } else {
    Serial.println("\n✅ Waktu tersinkronisasi.");
  }
}

// Fungsi untuk mendapatkan waktu saat ini dalam format "HH:MM" (24 jam)
String getCurrentFormattedTime() {
  if (!getLocalTime(&timeinfo)) {
    return "00:00"; // Default jika waktu belum tersinkronisasi
  }
  char time_output[6]; // HH:MM\0
  strftime(time_output, sizeof(time_output), "%H:%M", &timeinfo);
  return String(time_output);
}

// Fungsi untuk mendapatkan tanggal saat ini dalam format "dd/mm/yyyy"
String getCurrentFormattedDate() {
  if (!getLocalTime(&timeinfo)) {
    return "01/01/2000"; // Default jika waktu belum tersinkronisasi
  }

  char date_part[15]; // Cukup untuk "dd/mm/yyyy\0"
  strftime(date_part, sizeof(date_part), "%d/%m/%Y", &timeinfo); // Format dd/mm/yyyy

  return String(date_part);
}

// Fungsi untuk mendapatkan tanggal dengan hari dalam format "Hari, dd/mm/yyyy"
String getCurrentFormattedDateWithDay() {
  if (!getLocalTime(&timeinfo)) {
    return "Unknown, 01/01/2000"; // Default jika waktu belum tersinkronisasi
  }

  char date_part[15]; // Cukup untuk "dd/mm/yyyy\0"
  strftime(date_part, sizeof(date_part), "%d/%m/%Y", &timeinfo);

  // Format hari
  String dayOfWeek;
  switch (timeinfo.tm_wday) {
    case 0: dayOfWeek = "Minggu"; break;
    case 1: dayOfWeek = "Senin"; break;
    case 2: dayOfWeek = "Selasa"; break;
    case 3: dayOfWeek = "Rabu"; break;
    case 4: dayOfWeek = "Kamis"; break;
    case 5: dayOfWeek = "Jumat"; break;
    case 6: dayOfWeek = "Sabtu"; break;
    default: dayOfWeek = "Unknown"; break;
  }

  return dayOfWeek + ", " + String(date_part);
}