#ifndef TOUCH_GUI_H
#define TOUCH_GUI_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <vector>
#include <functional>

// Touchscreen pins
#define TOUCH_SDA 21
#define TOUCH_SCL 22

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

// Button states
enum ButtonState {
    BUTTON_NORMAL,
    BUTTON_PRESSED,
    BUTTON_DISABLED
};

// Button types
enum ButtonType {
    BUTTON_PRIMARY,
    BUTTON_SECONDARY,
    BUTTON_SUCCESS,
    BUTTON_WARNING,
    BUTTON_DANGER,
    BUTTON_INFO
};

// Touch button class
class TouchButton {
public:
    int x, y, width, height;
    String text;
    String icon;
    ButtonType type;
    ButtonState state;
    std::function<void()> onClick;
    bool enabled;
    uint16_t textColor;
    uint16_t bgColor;
    uint16_t borderColor;
    
    TouchButton(int x, int y, int w, int h, String text, ButtonType type = BUTTON_PRIMARY);
    TouchButton(int x, int y, int w, int h, String text, String icon, ButtonType type = BUTTON_PRIMARY);
    
    void draw(Adafruit_ILI9341& tft);
    void setOnClick(std::function<void()> callback);
    bool isPressed(int touchX, int touchY);
    void setEnabled(bool enabled);
    void setState(ButtonState state);
    void setText(String newText);
    void setIcon(String newIcon);
    
private:
    void updateColors();
    void drawIcon(Adafruit_ILI9341& tft, int iconX, int iconY);
};

// Icon definitions (simple geometric shapes)
class Icons {
public:
    static void drawUser(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color);
    static void drawFingerprint(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color);
    static void drawClock(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color);
    static void drawSettings(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color);
    static void drawHome(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color);
    static void drawCheck(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color);
    static void drawCross(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color);
    static void drawArrow(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color, bool right = true);
    static void drawWifi(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color, bool connected = true);
    static void drawBattery(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color, int level = 100);
};

// Touch GUI Manager
class TouchGUIManager {
public:
    Adafruit_ILI9341& tft;
    Adafruit_FT6206& touch;
    std::vector<TouchButton*> buttons;
    TouchButton* lastPressed;
    
    // Screen update tracking
    bool screenNeedsUpdate;
    unsigned long lastUpdateTime;
    unsigned long lastStatusUpdate;
    String currentStatus;
    bool currentWifiStatus;
    int currentBatteryLevel;
    
    TouchGUIManager(Adafruit_ILI9341& display, Adafruit_FT6206& touchscreen);
    
    void init();
    void update();
    void forceUpdate();
    void addButton(TouchButton* button);
    void removeButton(TouchButton* button);
    void clearButtons();
    void drawBackground();
    void drawHeader(String title, String subtitle = "");
    void drawStatusBar(String status, bool wifiConnected = false, int batteryLevel = 100);
    void updateStatusBar(String status, bool wifiConnected = false, int batteryLevel = 100);
    
    // Menu screens
    void showMainMenu();
    void showAttendanceMenu();
    void showRegistrationMenu();
    void showAdminMenu();
    void showSettingsMenu();
    void showLoadingScreen(String message);
    void showStartupScreen();
    void showMessageScreen(String title, String message, String buttonText = "OK");
    
    // Form screens
    void showInputForm(String title, String field, String currentValue = "");
    void showConfirmationScreen(String title, String message);
    
    // Progress and status
    void showProgressBar(int x, int y, int width, int height, int progress, uint16_t color = COLOR_PRIMARY);
    void updateProgressBar(int x, int y, int width, int height, int progress, uint16_t color = COLOR_PRIMARY);
    void showSystemStatus(String message, bool isError = false);
    
private:
    void handleTouch();
    void drawButton(TouchButton* button);
    void animateButtonPress(TouchButton* button);
};

// Global touch GUI manager
extern TouchGUIManager* touchGUI;

// Initialization function
void initTouchGUI();
void showStartupSequence();

// Screen management
void showMainMenuScreen();
void showAttendanceScreen();
void showRegistrationScreen();
void showAdminScreen();
void showSettingsScreen();

// State change handlers
void onStateChanged();
void updateScreenForCurrentState();

#endif
