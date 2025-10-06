#ifndef ADVANCED_GUI_H
#define ADVANCED_GUI_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <vector>
#include <functional>

// Color definitions
#define COLOR_PRIMARY 0x001F    // Blue
#define COLOR_SECONDARY 0x07E0  // Green
#define COLOR_ACCENT 0xF800     // Red
#define COLOR_WARNING 0xFFE0    // Yellow
#define COLOR_SUCCESS 0x07E0    // Green
#define COLOR_DANGER 0xF800     // Red
#define COLOR_INFO 0x07FF       // Cyan
#define COLOR_LIGHT 0xFFFF      // White
#define COLOR_DARK 0x0000       // Black
#define COLOR_GRAY 0x8410       // Gray
#define COLOR_BACKGROUND 0x0000 // Black
#define COLOR_SELECTED 0x8410   // Gray for selected item

// Menu item structure
struct MenuItem {
    String text;
    String icon;
    std::function<void()> action;
    bool enabled;
    uint16_t color;
};

// GUI Manager class
class AdvancedGUI {
public:
    Adafruit_ILI9341& tft;
    std::vector<MenuItem> menuItems;
    int selectedIndex;
    int scrollOffset;
    int maxVisibleItems;
    
    // Status tracking
    String currentStatus;
    bool wifiConnected;
    int batteryLevel;
    unsigned long lastUpdateTime;
    
    AdvancedGUI(Adafruit_ILI9341& display);
    
    void init();
    void update();
    void handleKeypad(char key);
    void drawBackground();
    void drawHeader(String title, String subtitle = "");
    void drawStatusBar();
    void drawMenu();
    void drawMenuItem(int index, bool selected);
    void drawIcon(String icon, int x, int y, int size, uint16_t color);
    
    // Menu management
    void addMenuItem(String text, String icon, std::function<void()> action, uint16_t color = COLOR_LIGHT);
    void clearMenu();
    void selectNext();
    void selectPrevious();
    void executeSelected();
    
    // Screen management
    void showMainMenu();
    void showAttendanceMenu();
    void showRegistrationMenu();
    void showAdminMenu();
    void showSettingsMenu();
    void showLoadingScreen(String message);
    void showMessageScreen(String title, String message);
    void showProgressBar(int x, int y, int width, int height, int progress, uint16_t color = COLOR_PRIMARY);
    
    // User registration forms
    void showStudentRegistrationForm();
    void showAdminRegistrationForm();
    void showFingerprintEnrollmentScreen(String userType);
    void showRegistrationSuccessScreen(String userType, String userName);
    void showRegistrationErrorScreen(String error);
    
    // Status updates
    void updateStatus(String status, bool wifi = false, int battery = 100);
    void showSystemStatus(String message, bool isError = false);
    
    // Form handling
    void handleFormInput(char key);
    void handleFingerprintEnrollment(char key);
    void handleFormConfirmation(char key);
    bool validateForm();
    void proceedToFingerprintEnrollment();
    
private:
    void drawButton(int x, int y, int width, int height, String text, String icon, uint16_t color, bool selected);
    void drawProgressBar(int x, int y, int width, int height, int progress, uint16_t color);
    void drawInputField(int x, int y, int width, int height, String label, String value, bool selected);
    void drawFormField(String label, String value, int y, bool selected);
};

// Global GUI manager
extern AdvancedGUI* advancedGUI;

// Initialization function
void initAdvancedGUI();

// Screen management functions
void showMainMenuScreen();
void showAttendanceScreen();
void showRegistrationScreen();
void showAdminScreen();
void showSettingsScreen();
void showStartupSequence();

// Keypad handler
void handleGUIKeypad(char key);

#endif
