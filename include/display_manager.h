#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "display_texts.h"
#include "advanced_gui.h"

// Pin definitions untuk LCD TFT
#define TFT_DC 4
#define TFT_CS 5
#define TFT_MOSI 23
#define TFT_SCK  18
#define TFT_RST  2
#define TFT_MISO 19

// Global LCD object
extern Adafruit_ILI9341 tft;

// Fungsi inisialisasi
void initDisplay();

// Optimized display functions
void clearScreenFast();
void fillRectFast(int x, int y, int w, int h, uint16_t color);

// Ultra-fast display functions using direct SPI
void clearScreenUltraFast();
void fillRectUltraFast(int x, int y, int w, int h, uint16_t color);

// Fungsi untuk clear screen dengan cara overwrite text hitam
void clearLCDScreen();

// Fungsi untuk clear menu utama secara spesifik
void clearMenuUtamaLCD();

// Fungsi untuk menampilkan menu utama di LCD
void tampilkanMenuUtamaLCD();

// Fungsi helper untuk print text dengan posisi dan size
void printText(int x, int y, int size, const char* text, uint16_t color = COLOR_TEXT);

// Fungsi untuk tampilkan pesan loading/transisi
void tampilkanPesanTransisi(const char* pesan);

#endif
