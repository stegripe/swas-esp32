#include "touch_gui.h"
#include "state.h"
#include "display_manager.h"

// Global touch GUI manager
TouchGUIManager* touchGUI = nullptr;

// TouchButton implementation
TouchButton::TouchButton(int x, int y, int w, int h, String text, ButtonType type) 
    : x(x), y(y), width(w), height(h), text(text), type(type), state(BUTTON_NORMAL), 
      enabled(true), icon("") {
    updateColors();
}

TouchButton::TouchButton(int x, int y, int w, int h, String text, String icon, ButtonType type) 
    : x(x), y(y), width(w), height(h), text(text), icon(icon), type(type), state(BUTTON_NORMAL), 
      enabled(true) {
    updateColors();
}

void TouchButton::updateColors() {
    switch (type) {
        case BUTTON_PRIMARY:
            bgColor = COLOR_PRIMARY;
            textColor = COLOR_LIGHT;
            borderColor = COLOR_PRIMARY;
            break;
        case BUTTON_SECONDARY:
            bgColor = COLOR_GRAY;
            textColor = COLOR_LIGHT;
            borderColor = COLOR_GRAY;
            break;
        case BUTTON_SUCCESS:
            bgColor = COLOR_SUCCESS;
            textColor = COLOR_LIGHT;
            borderColor = COLOR_SUCCESS;
            break;
        case BUTTON_WARNING:
            bgColor = COLOR_WARNING;
            textColor = COLOR_DARK;
            borderColor = COLOR_WARNING;
            break;
        case BUTTON_DANGER:
            bgColor = COLOR_DANGER;
            textColor = COLOR_LIGHT;
            borderColor = COLOR_DANGER;
            break;
        case BUTTON_INFO:
            bgColor = COLOR_INFO;
            textColor = COLOR_LIGHT;
            borderColor = COLOR_INFO;
            break;
    }
    
    if (state == BUTTON_PRESSED) {
        // Darken the color when pressed
        bgColor = (bgColor >> 1) & 0x7BEF;
    } else if (state == BUTTON_DISABLED) {
        bgColor = COLOR_GRAY;
        textColor = (textColor >> 1) & 0x7BEF;
    }
}

void TouchButton::draw(Adafruit_ILI9341& tft) {
    if (!enabled) return;
    
    updateColors();
    
    // Draw button background
    tft.fillRoundRect(x, y, width, height, 8, bgColor);
    tft.drawRoundRect(x, y, width, height, 8, borderColor);
    
    // Calculate text position
    int textX = x + (width / 2);
    int textY = y + (height / 2) + 6;
    
    // Draw icon if present
    if (icon.length() > 0) {
        int iconSize = 16;
        int iconX = textX - (iconSize / 2);
        int iconY = textY - 20;
        drawIcon(tft, iconX, iconY);
        textY += 10; // Move text down to make room for icon
    }
    
    // Draw text
    tft.setTextSize(1);
    tft.setTextColor(textColor);
    tft.setCursor(textX - (text.length() * 3), textY - 4);
    tft.print(text);
}

void TouchButton::drawIcon(Adafruit_ILI9341& tft, int iconX, int iconY) {
    if (icon == "user") {
        Icons::drawUser(tft, iconX, iconY, 16, textColor);
    } else if (icon == "fingerprint") {
        Icons::drawFingerprint(tft, iconX, iconY, 16, textColor);
    } else if (icon == "clock") {
        Icons::drawClock(tft, iconX, iconY, 16, textColor);
    } else if (icon == "settings") {
        Icons::drawSettings(tft, iconX, iconY, 16, textColor);
    } else if (icon == "home") {
        Icons::drawHome(tft, iconX, iconY, 16, textColor);
    } else if (icon == "check") {
        Icons::drawCheck(tft, iconX, iconY, 16, textColor);
    } else if (icon == "cross") {
        Icons::drawCross(tft, iconX, iconY, 16, textColor);
    } else if (icon == "arrow") {
        Icons::drawArrow(tft, iconX, iconY, 16, textColor, true);
    } else if (icon == "wifi") {
        Icons::drawWifi(tft, iconX, iconY, 16, textColor, true);
    }
}

void TouchButton::setOnClick(std::function<void()> callback) {
    onClick = callback;
}

bool TouchButton::isPressed(int touchX, int touchY) {
    return enabled && touchX >= x && touchX <= x + width && 
           touchY >= y && touchY <= y + height;
}

void TouchButton::setEnabled(bool enabled) {
    this->enabled = enabled;
    state = enabled ? BUTTON_NORMAL : BUTTON_DISABLED;
}

void TouchButton::setState(ButtonState state) {
    this->state = state;
}

void TouchButton::setText(String newText) {
    text = newText;
}

void TouchButton::setIcon(String newIcon) {
    icon = newIcon;
}

// Icons implementation
void Icons::drawUser(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color) {
    // Head (circle)
    tft.fillCircle(x + size/2, y + size/3, size/4, color);
    // Body (rectangle)
    tft.fillRect(x + size/4, y + size/2, size/2, size/2, color);
}

void Icons::drawFingerprint(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color) {
    // Simple fingerprint pattern
    tft.drawCircle(x + size/2, y + size/2, size/3, color);
    tft.drawCircle(x + size/2, y + size/2, size/4, color);
    tft.drawCircle(x + size/2, y + size/2, size/6, color);
    // Lines
    tft.drawLine(x + size/3, y + size/2, x + 2*size/3, y + size/2, color);
    tft.drawLine(x + size/2, y + size/3, x + size/2, y + 2*size/3, color);
}

void Icons::drawClock(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color) {
    // Clock face
    tft.drawCircle(x + size/2, y + size/2, size/2, color);
    // Hour hand
    tft.drawLine(x + size/2, y + size/2, x + size/2, y + size/3, color);
    // Minute hand
    tft.drawLine(x + size/2, y + size/2, x + 2*size/3, y + size/2, color);
    // Center dot
    tft.fillCircle(x + size/2, y + size/2, 2, color);
}

void Icons::drawSettings(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color) {
    // Gear icon
    int centerX = x + size/2;
    int centerY = y + size/2;
    int radius = size/3;
    
    // Outer gear teeth
    for (int i = 0; i < 8; i++) {
        float angle = i * PI / 4;
        int x1 = centerX + cos(angle) * radius;
        int y1 = centerY + sin(angle) * radius;
        int x2 = centerX + cos(angle) * (radius + 4);
        int y2 = centerY + sin(angle) * (radius + 4);
        tft.drawLine(x1, y1, x2, y2, color);
    }
    
    // Inner circle
    tft.drawCircle(centerX, centerY, radius/2, color);
}

void Icons::drawHome(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color) {
    // House shape
    int centerX = x + size/2;
    int topY = y + size/4;
    int bottomY = y + 3*size/4;
    
    // Roof
    tft.drawLine(centerX, topY, x, y + size/2, color);
    tft.drawLine(centerX, topY, x + size, y + size/2, color);
    tft.drawLine(x, y + size/2, x + size, y + size/2, color);
    
    // House body
    tft.drawRect(x + size/4, y + size/2, size/2, size/2, color);
}

void Icons::drawCheck(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color) {
    // Checkmark
    tft.drawLine(x + size/4, y + size/2, x + size/2, y + 3*size/4, color);
    tft.drawLine(x + size/2, y + 3*size/4, x + 3*size/4, y + size/4, color);
}

void Icons::drawCross(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color) {
    // X mark
    tft.drawLine(x + size/4, y + size/4, x + 3*size/4, y + 3*size/4, color);
    tft.drawLine(x + 3*size/4, y + size/4, x + size/4, y + 3*size/4, color);
}

void Icons::drawArrow(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color, bool right) {
    if (right) {
        // Right arrow
        tft.drawLine(x + size/4, y + size/4, x + 3*size/4, y + size/2, color);
        tft.drawLine(x + 3*size/4, y + size/2, x + size/4, y + 3*size/4, color);
        tft.drawLine(x + 3*size/4, y + size/2, x + size/4, y + size/2, color);
    } else {
        // Left arrow
        tft.drawLine(x + 3*size/4, y + size/4, x + size/4, y + size/2, color);
        tft.drawLine(x + size/4, y + size/2, x + 3*size/4, y + 3*size/4, color);
        tft.drawLine(x + size/4, y + size/2, x + 3*size/4, y + size/2, color);
    }
}

void Icons::drawWifi(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color, bool connected) {
    if (connected) {
        // Connected WiFi icon
        tft.drawCircle(x + size/2, y + size/2, size/4, color);
        tft.drawCircle(x + size/2, y + size/2, size/3, color);
        tft.drawCircle(x + size/2, y + size/2, size/2, color);
        // Signal lines
        tft.drawLine(x + size/2, y + size/2, x + size/2, y + size, color);
    } else {
        // Disconnected WiFi icon (crossed out)
        tft.drawCircle(x + size/2, y + size/2, size/4, color);
        tft.drawCircle(x + size/2, y + size/2, size/3, color);
        tft.drawCircle(x + size/2, y + size/2, size/2, color);
        tft.drawLine(x + size/4, y + size/4, x + 3*size/4, y + 3*size/4, color);
    }
}

void Icons::drawBattery(Adafruit_ILI9341& tft, int x, int y, int size, uint16_t color, int level) {
    // Battery outline
    tft.drawRect(x, y, size, size/2, color);
    tft.drawRect(x + size, y + size/8, size/4, size/4, color);
    
    // Battery level
    int fillWidth = (size * level) / 100;
    if (level > 20) {
        tft.fillRect(x + 1, y + 1, fillWidth - 2, size/2 - 2, color);
    } else {
        tft.fillRect(x + 1, y + 1, fillWidth - 2, size/2 - 2, COLOR_DANGER);
    }
}

// TouchGUIManager implementation
TouchGUIManager::TouchGUIManager(Adafruit_ILI9341& display, Adafruit_FT6206& touchscreen) 
    : tft(display), touch(touchscreen), lastPressed(nullptr), screenNeedsUpdate(false),
      lastUpdateTime(0), lastStatusUpdate(0), currentStatus(""), currentWifiStatus(false), currentBatteryLevel(100) {
}

void TouchGUIManager::init() {
    Serial.println("Initializing touchscreen...");
    if (!touch.begin(40)) {  // Sensitivity threshold
        Serial.println("Touchscreen not found!");
    } else {
        Serial.println("Touchscreen initialized!");
    }
}

void TouchGUIManager::update() {
    handleTouch();
    
    // Update status bar every 2 seconds
    if (millis() - lastStatusUpdate > 2000) {
        updateStatusBar(currentStatus, currentWifiStatus, currentBatteryLevel);
        lastStatusUpdate = millis();
    }
    
    // Force screen update if needed
    if (screenNeedsUpdate) {
        forceUpdate();
        screenNeedsUpdate = false;
    }
}

void TouchGUIManager::forceUpdate() {
    // Redraw current screen based on state
    updateScreenForCurrentState();
    lastUpdateTime = millis();
}

void TouchGUIManager::addButton(TouchButton* button) {
    buttons.push_back(button);
}

void TouchGUIManager::removeButton(TouchButton* button) {
    for (auto it = buttons.begin(); it != buttons.end(); ++it) {
        if (*it == button) {
            buttons.erase(it);
            break;
        }
    }
}

void TouchGUIManager::clearButtons() {
    buttons.clear();
}

void TouchGUIManager::drawBackground() {
    tft.fillScreen(COLOR_BACKGROUND);
}

void TouchGUIManager::drawHeader(String title, String subtitle) {
    // Header background
    tft.fillRect(0, 0, 320, 60, COLOR_PRIMARY);
    
    // Title
    tft.setTextSize(2);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(160 - (title.length() * 6), 20 - 8);
    tft.print(title);
    
    // Subtitle
    if (subtitle.length() > 0) {
        tft.setTextSize(1);
        tft.setCursor(160 - (subtitle.length() * 3), 40 - 4);
        tft.print(subtitle);
    }
}

void TouchGUIManager::drawStatusBar(String status, bool wifiConnected, int batteryLevel) {
    // Status bar background
    tft.fillRect(0, 220, 320, 20, COLOR_DARK);
    
    // Status text
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(5, 230 - 4);
    tft.print(status);
    
    // WiFi icon
    Icons::drawWifi(tft, 250, 222, 16, wifiConnected ? COLOR_SUCCESS : COLOR_DANGER, wifiConnected);
    
    // Battery icon
    Icons::drawBattery(tft, 280, 222, 16, COLOR_LIGHT, batteryLevel);
    
    // Store current values
    currentStatus = status;
    currentWifiStatus = wifiConnected;
    currentBatteryLevel = batteryLevel;
}

void TouchGUIManager::updateStatusBar(String status, bool wifiConnected, int batteryLevel) {
    // Only update if values changed
    if (status != currentStatus || wifiConnected != currentWifiStatus || batteryLevel != currentBatteryLevel) {
        drawStatusBar(status, wifiConnected, batteryLevel);
    }
}

void TouchGUIManager::handleTouch() {
    if (!touch.touched()) {
        return;
    }
    
    TS_Point p = touch.getPoint();
    
    // Convert touch coordinates to display coordinates
    int touchX = map(p.x, 0, 240, 0, 320);
    int touchY = map(p.y, 0, 320, 0, 240);
    
    // Check if any button is pressed
    for (TouchButton* button : buttons) {
        if (button->isPressed(touchX, touchY)) {
            button->setState(BUTTON_PRESSED);
            button->draw(tft);
            delay(100); // Visual feedback
            
            if (button->onClick) {
                button->onClick();
            }
            
            button->setState(BUTTON_NORMAL);
            button->draw(tft);
            break;
        }
    }
}

void TouchGUIManager::drawButton(TouchButton* button) {
    button->draw(tft);
}

void TouchGUIManager::animateButtonPress(TouchButton* button) {
    button->setState(BUTTON_PRESSED);
    button->draw(tft);
    delay(100);
    button->setState(BUTTON_NORMAL);
    button->draw(tft);
}

// Menu screens
void TouchGUIManager::showMainMenu() {
    drawBackground();
    drawHeader("Smart Workspace", "Attendance System");
    
    clearButtons();
    
    // Main menu buttons
    TouchButton* btnCheckIn = new TouchButton(20, 80, 120, 50, "Check In", "clock", BUTTON_SUCCESS);
    TouchButton* btnCheckOut = new TouchButton(180, 80, 120, 50, "Check Out", "clock", BUTTON_WARNING);
    TouchButton* btnRegister = new TouchButton(20, 140, 120, 50, "Register", "fingerprint", BUTTON_PRIMARY);
    TouchButton* btnAdmin = new TouchButton(180, 140, 120, 50, "Admin", "user", BUTTON_INFO);
    TouchButton* btnSettings = new TouchButton(100, 200, 120, 30, "Settings", "settings", BUTTON_SECONDARY);
    
    // Set callbacks
    btnCheckIn->setOnClick([]() {
        currentState = ABSEN;
        Serial.println("Check In selected");
    });
    
    btnCheckOut->setOnClick([]() {
        currentState = ABSEN;
        Serial.println("Check Out selected");
    });
    
    btnRegister->setOnClick([]() {
        currentState = ISI_DATA;
        Serial.println("Register selected");
    });
    
    btnAdmin->setOnClick([]() {
        currentState = ISI_DATA_ADMIN;
        Serial.println("Admin selected");
    });
    
    btnSettings->setOnClick([]() {
        currentState = SUBMENU_EDIT_USER;
        Serial.println("Settings selected");
    });
    
    addButton(btnCheckIn);
    addButton(btnCheckOut);
    addButton(btnRegister);
    addButton(btnAdmin);
    addButton(btnSettings);
    
    // Draw all buttons
    for (TouchButton* button : buttons) {
        button->draw(tft);
    }
    
    drawStatusBar("Ready", true, 85);
}

void TouchGUIManager::showAttendanceMenu() {
    drawBackground();
    drawHeader("Attendance", "Select option");
    
    clearButtons();
    
    TouchButton* btnBack = new TouchButton(10, 10, 60, 30, "Back", "arrow", BUTTON_SECONDARY);
    TouchButton* btnCheckIn = new TouchButton(50, 100, 220, 60, "Check In", "clock", BUTTON_SUCCESS);
    TouchButton* btnCheckOut = new TouchButton(50, 170, 220, 60, "Check Out", "clock", BUTTON_WARNING);
    
    btnBack->setOnClick([]() {
        showMainMenuScreen();
    });
    
    btnCheckIn->setOnClick([]() {
        currentState = ABSEN;
        Serial.println("Check In selected");
    });
    
    btnCheckOut->setOnClick([]() {
        currentState = ABSEN;
        Serial.println("Check Out selected");
    });
    
    addButton(btnBack);
    addButton(btnCheckIn);
    addButton(btnCheckOut);
    
    for (TouchButton* button : buttons) {
        button->draw(tft);
    }
    
    drawStatusBar("Attendance Menu", true, 85);
}

void TouchGUIManager::showRegistrationMenu() {
    drawBackground();
    drawHeader("Registration", "Register fingerprint");
    
    clearButtons();
    
    TouchButton* btnBack = new TouchButton(10, 10, 60, 30, "Back", "arrow", BUTTON_SECONDARY);
    TouchButton* btnStudent = new TouchButton(50, 100, 220, 60, "Register Student", "user", BUTTON_PRIMARY);
    TouchButton* btnAdmin = new TouchButton(50, 170, 220, 60, "Register Admin", "user", BUTTON_INFO);
    
    btnBack->setOnClick([]() {
        showMainMenuScreen();
    });
    
    btnStudent->setOnClick([]() {
        currentState = ISI_DATA;
        Serial.println("Register Student selected");
    });
    
    btnAdmin->setOnClick([]() {
        currentState = ISI_DATA_ADMIN;
        Serial.println("Register Admin selected");
    });
    
    addButton(btnBack);
    addButton(btnStudent);
    addButton(btnAdmin);
    
    for (TouchButton* button : buttons) {
        button->draw(tft);
    }
    
    drawStatusBar("Registration Menu", true, 85);
}

void TouchGUIManager::showAdminMenu() {
    drawBackground();
    drawHeader("Admin Panel", "Administrative functions");
    
    clearButtons();
    
    TouchButton* btnBack = new TouchButton(10, 10, 60, 30, "Back", "arrow", BUTTON_SECONDARY);
    TouchButton* btnViewUsers = new TouchButton(50, 80, 220, 40, "View Users", "user", BUTTON_INFO);
    TouchButton* btnEditUsers = new TouchButton(50, 130, 220, 40, "Edit Users", "settings", BUTTON_WARNING);
    TouchButton* btnSystemInfo = new TouchButton(50, 180, 220, 40, "System Info", "settings", BUTTON_SECONDARY);
    
    btnBack->setOnClick([]() {
        showMainMenuScreen();
    });
    
    btnViewUsers->setOnClick([]() {
        Serial.println("View Users selected");
    });
    
    btnEditUsers->setOnClick([]() {
        currentState = SUBMENU_EDIT_USER;
        Serial.println("Edit Users selected");
    });
    
    btnSystemInfo->setOnClick([]() {
        Serial.println("System Info selected");
    });
    
    addButton(btnBack);
    addButton(btnViewUsers);
    addButton(btnEditUsers);
    addButton(btnSystemInfo);
    
    for (TouchButton* button : buttons) {
        button->draw(tft);
    }
    
    drawStatusBar("Admin Menu", true, 85);
}

void TouchGUIManager::showSettingsMenu() {
    drawBackground();
    drawHeader("Settings", "System configuration");
    
    clearButtons();
    
    TouchButton* btnBack = new TouchButton(10, 10, 60, 30, "Back", "arrow", BUTTON_SECONDARY);
    TouchButton* btnWiFi = new TouchButton(50, 80, 220, 40, "WiFi Settings", "wifi", BUTTON_INFO);
    TouchButton* btnTime = new TouchButton(50, 130, 220, 40, "Time Settings", "clock", BUTTON_INFO);
    TouchButton* btnReset = new TouchButton(50, 180, 220, 40, "Reset System", "cross", BUTTON_DANGER);
    
    btnBack->setOnClick([]() {
        showMainMenuScreen();
    });
    
    btnWiFi->setOnClick([]() {
        Serial.println("WiFi Settings selected");
    });
    
    btnTime->setOnClick([]() {
        Serial.println("Time Settings selected");
    });
    
    btnReset->setOnClick([]() {
        Serial.println("Reset System selected");
    });
    
    addButton(btnBack);
    addButton(btnWiFi);
    addButton(btnTime);
    addButton(btnReset);
    
    for (TouchButton* button : buttons) {
        button->draw(tft);
    }
    
    drawStatusBar("Settings Menu", true, 85);
}

void TouchGUIManager::showLoadingScreen(String message) {
    drawBackground();
    
    // Loading animation
    tft.setTextSize(2);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(160 - 36, 100 - 8);
    tft.print("Loading...");
    tft.setCursor(160 - (message.length() * 6), 130 - 8);
    tft.print(message);
    
    // Simple loading dots animation
    for (int i = 0; i < 3; i++) {
        tft.fillCircle(140 + i * 20, 160, 3, COLOR_PRIMARY);
        delay(200);
        tft.fillCircle(140 + i * 20, 160, 3, COLOR_BACKGROUND);
    }
}

void TouchGUIManager::showStartupScreen() {
    drawBackground();
    
    // Title
    tft.setTextSize(3);
    tft.setTextColor(COLOR_PRIMARY);
    tft.setCursor(160 - 72, 50 - 12);
    tft.print("Smart Workspace");
    
    tft.setTextSize(2);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(160 - 72, 80 - 8);
    tft.print("Attendance System");
    
    // Progress bar
    showProgressBar(50, 120, 220, 20, 0, COLOR_PRIMARY);
    
    // Status text
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(160 - 66, 160 - 4);
    tft.print("Initializing system...");
    
    // Version info
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GRAY);
    tft.setCursor(160 - 12, 200 - 4);
    tft.print("v1.0.0");
}

void TouchGUIManager::showProgressBar(int x, int y, int width, int height, int progress, uint16_t color) {
    // Background
    tft.fillRect(x, y, width, height, COLOR_DARK);
    tft.drawRect(x, y, width, height, COLOR_LIGHT);
    
    // Progress fill
    int fillWidth = (width * progress) / 100;
    if (fillWidth > 0) {
        tft.fillRect(x + 1, y + 1, fillWidth - 2, height - 2, color);
    }
}

void TouchGUIManager::updateProgressBar(int x, int y, int width, int height, int progress, uint16_t color) {
    // Only update if progress changed
    static int lastProgress = -1;
    if (progress != lastProgress) {
        showProgressBar(x, y, width, height, progress, color);
        lastProgress = progress;
    }
}

void TouchGUIManager::showSystemStatus(String message, bool isError) {
    // Clear status area
    tft.fillRect(0, 180, 320, 40, COLOR_BACKGROUND);
    
    // Status text
    tft.setTextSize(1);
    tft.setTextColor(isError ? COLOR_DANGER : COLOR_SUCCESS);
    tft.setCursor(160 - (message.length() * 3), 200 - 4);
    tft.print(message);
}

void TouchGUIManager::showMessageScreen(String title, String message, String buttonText) {
    drawBackground();
    drawHeader(title, "");
    
    // Message
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(160 - (message.length() * 3), 120 - 4);
    tft.print(message);
    
    // OK button
    TouchButton* btnOK = new TouchButton(110, 180, 100, 40, buttonText, "check", BUTTON_SUCCESS);
    btnOK->setOnClick([]() {
        showMainMenuScreen();
    });
    
    clearButtons();
    addButton(btnOK);
    btnOK->draw(tft);
    
    drawStatusBar("Message", true, 85);
}

void TouchGUIManager::showInputForm(String title, String field, String currentValue) {
    drawBackground();
    drawHeader(title, field);
    
    // Input field background
    tft.fillRect(20, 100, 280, 40, COLOR_DARK);
    tft.drawRect(20, 100, 280, 40, COLOR_LIGHT);
    
    // Current value
    tft.setTextSize(2);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(30, 120 - 4);
    tft.print(currentValue);
    
    // Buttons
    TouchButton* btnOK = new TouchButton(50, 180, 100, 40, "OK", "check", BUTTON_SUCCESS);
    TouchButton* btnCancel = new TouchButton(170, 180, 100, 40, "Cancel", "cross", BUTTON_DANGER);
    
    btnOK->setOnClick([]() {
        showMainMenuScreen();
    });
    
    btnCancel->setOnClick([]() {
        showMainMenuScreen();
    });
    
    clearButtons();
    addButton(btnOK);
    addButton(btnCancel);
    
    for (TouchButton* button : buttons) {
        button->draw(tft);
    }
    
    drawStatusBar("Input Form", true, 85);
}

void TouchGUIManager::showConfirmationScreen(String title, String message) {
    drawBackground();
    drawHeader(title, "");
    
    // Message
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(160 - (message.length() * 3), 120 - 4);
    tft.print(message);
    
    // Buttons
    TouchButton* btnYes = new TouchButton(50, 180, 100, 40, "Yes", "check", BUTTON_SUCCESS);
    TouchButton* btnNo = new TouchButton(170, 180, 100, 40, "No", "cross", BUTTON_DANGER);
    
    btnYes->setOnClick([]() {
        showMainMenuScreen();
    });
    
    btnNo->setOnClick([]() {
        showMainMenuScreen();
    });
    
    clearButtons();
    addButton(btnYes);
    addButton(btnNo);
    
    for (TouchButton* button : buttons) {
        button->draw(tft);
    }
    
    drawStatusBar("Confirmation", true, 85);
}

// Global functions
void initTouchGUI() {
    if (touchGUI == nullptr) {
        touchGUI = new TouchGUIManager(tft, touch);
        touchGUI->init();
    }
}

void showStartupSequence() {
    if (touchGUI == nullptr) return;
    
    touchGUI->showStartupScreen();
    
    // Simulate startup progress
    for (int progress = 0; progress <= 100; progress += 10) {
        touchGUI->updateProgressBar(50, 120, 220, 20, progress, COLOR_PRIMARY);
        
        // Update status messages
        String statusMsg = "";
        if (progress < 20) {
            statusMsg = "Initializing hardware...";
        } else if (progress < 40) {
            statusMsg = "Loading fingerprint sensor...";
        } else if (progress < 60) {
            statusMsg = "Connecting to WiFi...";
        } else if (progress < 80) {
            statusMsg = "Synchronizing time...";
        } else if (progress < 100) {
            statusMsg = "Loading user data...";
        } else {
            statusMsg = "System ready!";
        }
        
        touchGUI->showSystemStatus(statusMsg, false);
        delay(300); // Simulate loading time
    }
    
    delay(1000); // Show "System ready!" for 1 second
}

void onStateChanged() {
    if (touchGUI) {
        touchGUI->screenNeedsUpdate = true;
    }
}

void updateScreenForCurrentState() {
    if (touchGUI == nullptr) return;
    
    switch (currentState) {
        case MENU_UTAMA:
            showMainMenuScreen();
            break;
        case ABSEN:
            showAttendanceScreen();
            break;
        case ISI_DATA:
            showRegistrationScreen();
            break;
        case ISI_DATA_ADMIN:
            showAdminScreen();
            break;
        case SUBMENU_EDIT_USER:
            showSettingsScreen();
            break;
        default:
            showMainMenuScreen();
            break;
    }
}

void showMainMenuScreen() {
    if (touchGUI) {
        touchGUI->showMainMenu();
    }
}

void showAttendanceScreen() {
    if (touchGUI) {
        touchGUI->showAttendanceMenu();
    }
}

void showRegistrationScreen() {
    if (touchGUI) {
        touchGUI->showRegistrationMenu();
    }
}

void showAdminScreen() {
    if (touchGUI) {
        touchGUI->showAdminMenu();
    }
}

void showSettingsScreen() {
    if (touchGUI) {
        touchGUI->showSettingsMenu();
    }
}
