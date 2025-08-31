#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "display_texts.h"

// Pin definitions untuk LCD TFT
#define TFT_DC 4
#define TFT_CS 5
#define TFT_MOSI 23
#define TFT_SCK  18
#define TFT_RST  2
#define TFT_MISO 19

// Colors
#define COLOR_BACKGROUND ILI9341_BLACK
#define COLOR_TEXT ILI9341_CYAN
#define COLOR_CLEAR ILI9341_BLACK

// Global LCD object
extern Adafruit_ILI9341 tft;

// Fungsi inisialisasi
void initDisplay();

// Fungsi untuk clear screen dengan cara overwrite text hitam
void clearLCDScreen();

// Fungsi untuk menampilkan menu utama di LCD - NAMA DIUBAH UNTUK MENGHINDARI CONFLICT
void tampilkanMenuUtamaLCD();

// Fungsi helper untuk print text dengan posisi dan size
void printText(int x, int y, int size, const char* text, uint16_t color = COLOR_TEXT);

#endif