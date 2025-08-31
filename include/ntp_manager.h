#ifndef NTP_MANAGER_H
#define NTP_MANAGER_H

#include <Arduino.h>
#include <time.h> // Untuk struktur tm dan fungsi waktu

// Deklarasi fungsi untuk inisialisasi NTP
void setupNTP();

// Fungsi untuk mendapatkan waktu saat ini dalam string
String getCurrentFormattedTime();

// Fungsi untuk mendapatkan tanggal saat ini dalam string
String getCurrentFormattedDate();

#endif // NTP_MANAGER_H