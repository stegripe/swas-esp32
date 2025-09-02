#include "display_manager.h"

// ================================
// MENU TEXT DEFINITIONS - INDONESIAN
// ================================

// Header menu utama
const char* MENU_HEADER_LINE = "==========================";
const char* MENU_TITLE_LINE1 = " Smart Workspace";
const char* MENU_TITLE_LINE2 = "Attendance System";

// Menu options
const char* MENU_OPTION_1 = "1. Absen Masuk";
const char* MENU_OPTION_2 = "2. Absen Pulang";
const char* MENU_OPTION_3 = "3. Mendaftar Sidik";
const char* MENU_OPTION_3_SUB = "   Jari Mahasiswa";
const char* MENU_OPTION_4 = "4. Mendaftar Admin";
const char* MENU_OPTION_5 = "5. Ganti Informasi User";

// Inisialisasi object LCD TFT
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

void initDisplay() {
    Serial.println("Inisialisasi LCD TFT...");
    tft.begin();
    tft.setRotation(1);  // Landscape mode
    tft.fillScreen(COLOR_BACKGROUND);
    Serial.println("LCD TFT berhasil diinisialisasi");
}

void printText(int x, int y, int size, const char* text, uint16_t color) {
    tft.setCursor(x, y);
    tft.setTextSize(size);
    tft.setTextColor(color);
    tft.print(text);
}

void clearLCDScreen() {
    // Use proper fillScreen method instead of text overwrite
    tft.fillScreen(COLOR_BACKGROUND);
}

void clearMenuUtamaLCD() {
    // Same as clearLCDScreen but kept for compatibility
    tft.fillScreen(COLOR_BACKGROUND);
}

void tampilkanMenuUtamaLCD() {
    // Simple coordinate-based text positioning
    // Header top
    printText(0, 0, 2, MENU_HEADER_LINE);
    
    // Title
    printText(0, 20, 3, MENU_TITLE_LINE1);
    printText(0, 50, 3, MENU_TITLE_LINE2);
    
    // Header bottom
    printText(0, 80, 2, MENU_HEADER_LINE);
    
    // Menu options with simple y-coordinate spacing
    printText(0, 100, 2, MENU_OPTION_1);
    printText(0, 120, 2, MENU_OPTION_2);
    printText(0, 140, 2, MENU_OPTION_3);
    printText(0, 160, 2, MENU_OPTION_3_SUB);
    printText(0, 180, 2, MENU_OPTION_4);
    printText(0, 200, 2, MENU_OPTION_5);
}

void tampilkanPesanTransisi(const char* pesan) {
    clearLCDScreen();
    printText(10, 100, 2, pesan);
}