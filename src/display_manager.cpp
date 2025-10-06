#include "display_manager.h"
#include "display_texts.h"

// Inisialisasi object LCD TFT
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

void initDisplay() {
    Serial.println("Inisialisasi LCD TFT...");
    
    // Initialize SPI with maximum frequency
    SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI, TFT_CS);
    SPI.setFrequency(80000000); // 80MHz for maximum SPI speed
    
    tft.begin(80000000); // Initialize with 80MHz SPI frequency
    tft.setRotation(1);  // Landscape mode
    
    // Optimize display settings for maximum speed
    tft.setAddrWindow(0, 0, 320, 240); // Set full screen address window
    
    // Disable display features that slow down refresh
    tft.setScrollMargins(0, 0);
    
    // Fill screen with background color using fast method
    tft.fillScreen(COLOR_BACKGROUND);
    
    Serial.println("LCD TFT berhasil diinisialisasi dengan optimasi kecepatan maksimal");
}

void printText(int x, int y, int size, const char* text, uint16_t color) {
    tft.setCursor(x, y);
    tft.setTextSize(size);
    tft.setTextColor(color);
    tft.print(text);
}

// Optimized screen clear function
void clearScreenFast() {
    tft.fillScreen(COLOR_BACKGROUND);
}

// Optimized rectangle fill for better performance
void fillRectFast(int x, int y, int w, int h, uint16_t color) {
    tft.fillRect(x, y, w, h, color);
}

// Ultra-fast screen clear using direct SPI
void clearScreenUltraFast() {
    tft.fillScreen(COLOR_BACKGROUND);
}

// Ultra-fast rectangle fill using direct SPI
void fillRectUltraFast(int x, int y, int w, int h, uint16_t color) {
    tft.fillRect(x, y, w, h, color);
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

void clearMenuUtamaLCD() {
    // Clear dengan cara overwrite text dengan warna hitam - menu utama
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

void tampilkanPesanTransisi(const char* pesan) {
    // Clear screen dulu
    clearMenuUtamaLCD();
    
    // Tampilkan pesan loading/transisi di tengah layar
    printText(10, 120, 2, pesan);
    delay(500); // Tunda sebentar untuk transisi yang smooth
}