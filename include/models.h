#ifndef MODELS_H
#define MODELS_H

#include <Arduino.h>

// Hanya definisi struktur data, TIDAK ada enum, TIDAK ada extern!

struct Mahasiswa {
  String nama;
  String nim;
  String kelas;
  uint16_t sidikJariID;
};

struct Admin {
  String nama;
  String nik;
  uint16_t sidikJariID;
};

#endif
