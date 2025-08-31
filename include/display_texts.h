#ifndef DISPLAY_TEXTS_H
#define DISPLAY_TEXTS_H

// ================================
// TEKS MENU UTAMA - DEKLARASI EXTERN
// ================================

// Header menu utama
extern const char* MENU_HEADER_LINE;
extern const char* MENU_TITLE_LINE1;
extern const char* MENU_TITLE_LINE2;

// Menu options
extern const char* MENU_OPTION_1;
extern const char* MENU_OPTION_2;
extern const char* MENU_OPTION_3;
extern const char* MENU_OPTION_3_SUB;
extern const char* MENU_OPTION_4;
extern const char* MENU_OPTION_5;

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
extern const TextLayout LAYOUT_HEADER_TOP;
extern const TextLayout LAYOUT_TITLE_1;
extern const TextLayout LAYOUT_TITLE_2;
extern const TextLayout LAYOUT_HEADER_BOTTOM;
extern const TextLayout LAYOUT_OPTION_1;
extern const TextLayout LAYOUT_OPTION_2;
extern const TextLayout LAYOUT_OPTION_3;
extern const TextLayout LAYOUT_OPTION_3_SUB;
extern const TextLayout LAYOUT_OPTION_4;
extern const TextLayout LAYOUT_OPTION_5;

#endif