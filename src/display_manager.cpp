#include "display_manager.h"

// ================================
// DEFINISI VARIABLE TEKS - IMPLEMENTASI
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

// Layout untuk menu utama
const TextLayout LAYOUT_HEADER_TOP = {0, 0, 2};
const TextLayout LAYOUT_TITLE_1 = {0, 20, 3};
const TextLayout LAYOUT_TITLE_2 = {0, 50, 3};
const TextLayout LAYOUT_HEADER_BOTTOM = {0, 80, 2};
const TextLayout LAYOUT_OPTION_1 = {0, 100, 2};
const TextLayout LAYOUT_OPTION_2 = {0, 120, 2};
const TextLayout LAYOUT_OPTION_3 = {0, 140, 2};
const TextLayout LAYOUT_OPTION_3_SUB = {0, 160, 2};
const TextLayout LAYOUT_OPTION_4 = {0, 180, 2};
const TextLayout LAYOUT_OPTION_5 = {0, 200, 2};

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
    // Clear dengan cara overwrite text dengan warna hitam
    // Header top
    printText(LAYOUT_HEADER_TOP.x, LAYOUT_HEADER_TOP.y, LAYOUT_HEADER_TOP.size, MENU_HEADER_LINE, COLOR_CLEAR);
    
    // Title
    printText(LAYOUT_TITLE_1.x, LAYOUT_TITLE_1.y, LAYOUT_TITLE_1.size, MENU_TITLE_LINE1, COLOR_CLEAR);
    printText(LAYOUT_TITLE_2.x, LAYOUT_TITLE_2.y, LAYOUT_TITLE_2.size, MENU_TITLE_LINE2, COLOR_CLEAR);
    
    // Header bottom  
    printText(LAYOUT_HEADER_BOTTOM.x, LAYOUT_HEADER_BOTTOM.y, LAYOUT_HEADER_BOTTOM.size, MENU_HEADER_LINE, COLOR_CLEAR);
    
    // Menu options
    printText(LAYOUT_OPTION_1.x, LAYOUT_OPTION_1.y, LAYOUT_OPTION_1.size, MENU_OPTION_1, COLOR_CLEAR);
    printText(LAYOUT_OPTION_2.x, LAYOUT_OPTION_2.y, LAYOUT_OPTION_2.size, MENU_OPTION_2, COLOR_CLEAR);
    printText(LAYOUT_OPTION_3.x, LAYOUT_OPTION_3.y, LAYOUT_OPTION_3.size, MENU_OPTION_3, COLOR_CLEAR);
    printText(LAYOUT_OPTION_3_SUB.x, LAYOUT_OPTION_3_SUB.y, LAYOUT_OPTION_3_SUB.size, MENU_OPTION_3_SUB, COLOR_CLEAR);
    printText(LAYOUT_OPTION_4.x, LAYOUT_OPTION_4.y, LAYOUT_OPTION_4.size, MENU_OPTION_4, COLOR_CLEAR);
    printText(LAYOUT_OPTION_5.x, LAYOUT_OPTION_5.y, LAYOUT_OPTION_5.size, MENU_OPTION_5, COLOR_CLEAR);
}

void tampilkanMenuUtamaLCD() {
    // Header top
    printText(LAYOUT_HEADER_TOP.x, LAYOUT_HEADER_TOP.y, LAYOUT_HEADER_TOP.size, MENU_HEADER_LINE);
    
    // Title
    printText(LAYOUT_TITLE_1.x, LAYOUT_TITLE_1.y, LAYOUT_TITLE_1.size, MENU_TITLE_LINE1);
    printText(LAYOUT_TITLE_2.x, LAYOUT_TITLE_2.y, LAYOUT_TITLE_2.size, MENU_TITLE_LINE2);
    
    // Header bottom
    printText(LAYOUT_HEADER_BOTTOM.x, LAYOUT_HEADER_BOTTOM.y, LAYOUT_HEADER_BOTTOM.size, MENU_HEADER_LINE);
    
    // Menu options
    printText(LAYOUT_OPTION_1.x, LAYOUT_OPTION_1.y, LAYOUT_OPTION_1.size, MENU_OPTION_1);
    printText(LAYOUT_OPTION_2.x, LAYOUT_OPTION_2.y, LAYOUT_OPTION_2.size, MENU_OPTION_2);
    printText(LAYOUT_OPTION_3.x, LAYOUT_OPTION_3.y, LAYOUT_OPTION_3.size, MENU_OPTION_3);
    printText(LAYOUT_OPTION_3_SUB.x, LAYOUT_OPTION_3_SUB.y, LAYOUT_OPTION_3_SUB.size, MENU_OPTION_3_SUB);
    printText(LAYOUT_OPTION_4.x, LAYOUT_OPTION_4.y, LAYOUT_OPTION_4.size, MENU_OPTION_4);
    printText(LAYOUT_OPTION_5.x, LAYOUT_OPTION_5.y, LAYOUT_OPTION_5.size, MENU_OPTION_5);
}