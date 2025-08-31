#ifndef DISPLAY_TEXTS_H
#define DISPLAY_TEXTS_H

// ================================
// TEKS MENU UTAMA
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

// ================================
// POSISI DAN SIZE UNTUK MENU UTAMA
// ================================

// Struktur untuk menyimpan posisi dan size text
struct TextLayout {
    int x;
    int y; 
    int size;
};

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

#endif