#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <Arduino.h>

// Global variable terkait input alfabet
extern String inputText;
extern char lastKeyPressed;
extern int pressCount;
extern unsigned long lastPressTime;
extern const unsigned long ALPHABET_TIMEOUT;
extern const char* keymap[10];
extern bool modeAlfabet;

// Fungsi input karakter
void handleAlfabetInput(char key);
void commitLastChar();

// Fungsi pengisian form mahasiswa baru
void handleInputFormMahasiswa(char key);

// Fungsi pengisian form admin
void handleInputFormAdmin(char key);

// ✅ Tambahan fungsi pengisian form edit mahasiswa
void handleInputEditMahasiswa(char key);

#endif
