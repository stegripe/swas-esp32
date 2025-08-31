#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h> // Memasukkan library WiFi ESP32

// Deklarasi fungsi untuk menginisialisasi dan menghubungkan ke WiFi
void setupWiFi(const char* ssid, const char* password);
void handleWiFi(); // Untuk memantau dan mengelola koneksi WiFi di loop

#endif // WIFI_MANAGER_H