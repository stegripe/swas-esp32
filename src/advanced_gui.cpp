#include "advanced_gui.h"
#include "state.h"
#include "display_manager.h"
#include "fingerprint_manager.h"
#include "menu_handler.h"
#include "input_handler.h"
#include "admin_handler.h"
#include "db_client.h"
#include <WiFi.h>

// Global GUI manager
AdvancedGUI* advancedGUI = nullptr;

// AdvancedGUI implementation
AdvancedGUI::AdvancedGUI(Adafruit_ILI9341& display) 
    : tft(display), selectedIndex(0), scrollOffset(0), maxVisibleItems(4),
      currentStatus("Ready"), wifiConnected(false), batteryLevel(100), lastUpdateTime(0) {
}

void AdvancedGUI::init() {
    Serial.println("Initializing Advanced GUI...");
    drawBackground();
    drawHeader("Smart Workspace", "Attendance System");
    drawStatusBar();
    Serial.println("Advanced GUI initialized!");
}

void AdvancedGUI::update() {
    // Update status bar every 10 seconds to reduce load
    if (millis() - lastUpdateTime > 10000) {
        drawStatusBar();
        lastUpdateTime = millis();
    }
}

void AdvancedGUI::handleKeypad(char key) {
    // Handle form input states
    if (currentState == ISI_DATA || currentState == ISI_DATA_ADMIN) {
        handleFormInput(key);
        return;
    }
    
    // Handle fingerprint enrollment state
    if (currentState == ISI_FINGERPRINT || currentState == ISI_FINGERPRINT_ADMIN) {
        handleFingerprintEnrollment(key);
        return;
    }
    
    // Handle confirmation states
    if (currentState == KONFIRMASI_DATA || currentState == KONFIRMASI_ADMIN) {
        handleFormConfirmation(key);
        return;
    }
    
    // Handle success/error screens
    if (currentState == REGISTRATION_SUCCESS || currentState == REGISTRATION_ERROR) {
        if (key == '0' || key == 'B') {
            currentState = MENU_UTAMA;
            showMainMenu();
            return;
        }
        return;
    }
    
    // Default menu navigation
    switch (key) {
        case '2': // Up
            selectPrevious();
            break;
        case '8': // Down
            selectNext();
            break;
        case '5': // Select/Enter
            executeSelected();
            break;
        case '0': // Back/Cancel
            showMainMenu();
            break;
        default:
            break;
    }
}

void AdvancedGUI::drawBackground() {
    clearScreenUltraFast(); // Use ultra-fast clear function
}

void AdvancedGUI::drawHeader(String title, String subtitle) {
    // Header background
    fillRectUltraFast(0, 0, 320, 60, COLOR_PRIMARY);
    
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

void AdvancedGUI::drawStatusBar() {
    // Status bar background
    fillRectUltraFast(0, 220, 320, 20, COLOR_DARK);
    
    // Status text
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(5, 230 - 4);
    tft.print(currentStatus);
    
    // WiFi indicator
    tft.setCursor(250, 230 - 4);
    tft.print(wifiConnected ? "WiFi" : "NoWiFi");
    
    // Battery indicator
    tft.setCursor(290, 230 - 4);
    tft.print(String(batteryLevel) + "%");
}

void AdvancedGUI::drawMenu() {
    // Clear menu area
    fillRectUltraFast(0, 60, 320, 160, COLOR_BACKGROUND);
    
    // Draw menu items
    for (int i = 0; i < menuItems.size() && i < maxVisibleItems; i++) {
        int actualIndex = i + scrollOffset;
        if (actualIndex < menuItems.size()) {
            drawMenuItem(actualIndex, actualIndex == selectedIndex);
        }
    }
    
    // Draw scroll indicators if needed
    if (scrollOffset > 0) {
        tft.setTextSize(1);
        tft.setTextColor(COLOR_GRAY);
        tft.setCursor(300, 70);
        tft.print("^");
    }
    
    if (scrollOffset + maxVisibleItems < menuItems.size()) {
        tft.setTextSize(1);
        tft.setTextColor(COLOR_GRAY);
        tft.setCursor(300, 200);
        tft.print("v");
    }
}

void AdvancedGUI::drawMenuItem(int index, bool selected) {
    if (index >= menuItems.size()) return;
    
    MenuItem& item = menuItems[index];
    int y = 70 + (index - scrollOffset) * 35;
    
    // Draw selection background
    if (selected) {
        fillRectUltraFast(10, y - 5, 300, 30, COLOR_SELECTED);
    }
    
    // Draw icon
    if (item.icon.length() > 0) {
        drawIcon(item.icon, 20, y, 20, selected ? COLOR_LIGHT : item.color);
    }
    
    // Draw text
    tft.setTextSize(1);
    tft.setTextColor(selected ? COLOR_LIGHT : item.color);
    tft.setCursor(50, y + 6);
    tft.print(item.text);
    
    // Draw selection indicator
    if (selected) {
        tft.setTextSize(1);
        tft.setTextColor(COLOR_LIGHT);
        tft.setCursor(290, y + 6);
        tft.print(">");
    }
}

void AdvancedGUI::drawIcon(String icon, int x, int y, int size, uint16_t color) {
    if (icon == "user") {
        // User icon
        tft.fillCircle(x + size/2, y + size/3, size/4, color);
        tft.fillRect(x + size/4, y + size/2, size/2, size/2, color);
    } else if (icon == "fingerprint") {
        // Fingerprint icon
        tft.drawCircle(x + size/2, y + size/2, size/3, color);
        tft.drawCircle(x + size/2, y + size/2, size/4, color);
        tft.drawCircle(x + size/2, y + size/2, size/6, color);
    } else if (icon == "clock") {
        // Clock icon
        tft.drawCircle(x + size/2, y + size/2, size/2, color);
        tft.drawLine(x + size/2, y + size/2, x + size/2, y + size/3, color);
        tft.drawLine(x + size/2, y + size/2, x + 2*size/3, y + size/2, color);
    } else if (icon == "settings") {
        // Settings icon
        int centerX = x + size/2;
        int centerY = y + size/2;
        int radius = size/3;
        for (int i = 0; i < 8; i++) {
            float angle = i * PI / 4;
            int x1 = centerX + cos(angle) * radius;
            int y1 = centerY + sin(angle) * radius;
            int x2 = centerX + cos(angle) * (radius + 3);
            int y2 = centerY + sin(angle) * (radius + 3);
            tft.drawLine(x1, y1, x2, y2, color);
        }
        tft.drawCircle(centerX, centerY, radius/2, color);
    } else if (icon == "home") {
        // Home icon
        int centerX = x + size/2;
        int topY = y + size/4;
        tft.drawLine(centerX, topY, x, y + size/2, color);
        tft.drawLine(centerX, topY, x + size, y + size/2, color);
        tft.drawLine(x, y + size/2, x + size, y + size/2, color);
        tft.drawRect(x + size/4, y + size/2, size/2, size/2, color);
    } else if (icon == "check") {
        // Check icon
        tft.drawLine(x + size/4, y + size/2, x + size/2, y + 3*size/4, color);
        tft.drawLine(x + size/2, y + 3*size/4, x + 3*size/4, y + size/4, color);
    } else if (icon == "cross") {
        // Cross icon
        tft.drawLine(x + size/4, y + size/4, x + 3*size/4, y + 3*size/4, color);
        tft.drawLine(x + 3*size/4, y + size/4, x + size/4, y + 3*size/4, color);
    } else if (icon == "wifi") {
        // WiFi icon
        tft.drawCircle(x + size/2, y + size/2, size/4, color);
        tft.drawCircle(x + size/2, y + size/2, size/3, color);
        tft.drawCircle(x + size/2, y + size/2, size/2, color);
        tft.drawLine(x + size/2, y + size/2, x + size/2, y + size, color);
    }
}

void AdvancedGUI::addMenuItem(String text, String icon, std::function<void()> action, uint16_t color) {
    MenuItem item;
    item.text = text;
    item.icon = icon;
    item.action = action;
    item.enabled = true;
    item.color = color;
    menuItems.push_back(item);
}

void AdvancedGUI::clearMenu() {
    menuItems.clear();
    selectedIndex = 0;
    scrollOffset = 0;
}

void AdvancedGUI::selectNext() {
    if (selectedIndex < menuItems.size() - 1) {
        selectedIndex++;
        if (selectedIndex >= scrollOffset + maxVisibleItems) {
            scrollOffset = selectedIndex - maxVisibleItems + 1;
        }
        drawMenu();
    }
}

void AdvancedGUI::selectPrevious() {
    if (selectedIndex > 0) {
        selectedIndex--;
        if (selectedIndex < scrollOffset) {
            scrollOffset = selectedIndex;
        }
        drawMenu();
    }
}

void AdvancedGUI::executeSelected() {
    if (selectedIndex < menuItems.size() && menuItems[selectedIndex].action) {
        menuItems[selectedIndex].action();
    }
}

void AdvancedGUI::showMainMenu() {
    drawBackground();
    drawHeader("Smart Workspace", "Attendance System");
    
    clearMenu();
    
    addMenuItem("Check In", "clock", []() {
        currentState = ABSEN;
        Serial.println("Check In selected");
        // Show attendance screen for check in
        showAttendanceScreen();
    }, COLOR_SUCCESS);
    
    addMenuItem("Check Out", "clock", []() {
        currentState = ABSEN;
        Serial.println("Check Out selected");
        // Show attendance screen for check out
        showAttendanceScreen();
    }, COLOR_WARNING);
    
    addMenuItem("Register Student", "fingerprint", []() {
        currentState = ISI_DATA;
        Serial.println("Register Student selected");
        // Show student registration form
        if (advancedGUI) {
            advancedGUI->showStudentRegistrationForm();
        }
    }, COLOR_PRIMARY);
    
    addMenuItem("Register Admin", "user", []() {
        currentState = ISI_DATA_ADMIN;
        Serial.println("Register Admin selected");
        // Show admin registration form
        if (advancedGUI) {
            advancedGUI->showAdminRegistrationForm();
        }
    }, COLOR_INFO);
    
    addMenuItem("Settings", "settings", []() {
        currentState = SUBMENU_EDIT_USER;
        Serial.println("Settings selected");
        // Show settings screen
        showSettingsScreen();
    }, COLOR_SECONDARY);
    
    drawMenu();
    drawStatusBar();
}

void AdvancedGUI::showAttendanceMenu() {
    drawBackground();
    drawHeader("Attendance", "Select option");
    
    clearMenu();
    
    addMenuItem("Check In", "clock", []() {
        currentState = ABSEN;
        Serial.println("Check In selected");
        // Start check in process
        handleAbsenLoop();
    }, COLOR_SUCCESS);
    
    addMenuItem("Check Out", "clock", []() {
        currentState = ABSEN;
        Serial.println("Check Out selected");
        // Start check out process
        handleAbsenLoop();
    }, COLOR_WARNING);
    
    addMenuItem("Back to Main", "home", []() {
        showMainMenuScreen();
    }, COLOR_SECONDARY);
    
    drawMenu();
    drawStatusBar();
}

void AdvancedGUI::showRegistrationMenu() {
    drawBackground();
    drawHeader("Registration", "Register fingerprint");
    
    clearMenu();
    
    addMenuItem("Register Student", "user", []() {
        currentState = ISI_DATA;
        Serial.println("Register Student selected");
        // Start student registration process
        tampilkanFormDataMahasiswa();
    }, COLOR_PRIMARY);
    
    addMenuItem("Register Admin", "user", []() {
        currentState = ISI_DATA_ADMIN;
        Serial.println("Register Admin selected");
        // Start admin registration process
        tampilkanFormDataAdmin();
    }, COLOR_INFO);
    
    addMenuItem("Back to Main", "home", []() {
        showMainMenuScreen();
    }, COLOR_SECONDARY);
    
    drawMenu();
    drawStatusBar();
}

void AdvancedGUI::showAdminMenu() {
    drawBackground();
    drawHeader("Admin Panel", "Administrative functions");
    
    clearMenu();
    
    addMenuItem("View Users", "user", [this]() {
        Serial.println("View Users selected");
        // Show user list or system info
        showMessageScreen("User List", "Total Users: " + String(daftarMahasiswaData.size() + daftarAdminData.size()));
    }, COLOR_INFO);
    
    addMenuItem("Edit Users", "settings", []() {
        currentState = SUBMENU_EDIT_USER;
        Serial.println("Edit Users selected");
        // Show edit user menu
        tampilkanMenuEditUser();
    }, COLOR_WARNING);
    
    addMenuItem("System Info", "settings", [this]() {
        Serial.println("System Info selected");
        // Show system information
        String info = "WiFi: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + 
                     "\nFree RAM: " + String(ESP.getFreeHeap()) + " bytes";
        showMessageScreen("System Info", info);
    }, COLOR_SECONDARY);
    
    addMenuItem("Back to Main", "home", []() {
        showMainMenuScreen();
    }, COLOR_SECONDARY);
    
    drawMenu();
    drawStatusBar();
}

void AdvancedGUI::showSettingsMenu() {
    drawBackground();
    drawHeader("Settings", "System configuration");
    
    clearMenu();
    
    addMenuItem("WiFi Settings", "wifi", [this]() {
        Serial.println("WiFi Settings selected");
        // Show WiFi status and connection info
        String wifiInfo = "SSID: " + String(WiFi.SSID()) + 
                         "\nIP: " + WiFi.localIP().toString() +
                         "\nStatus: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
        showMessageScreen("WiFi Settings", wifiInfo);
    }, COLOR_INFO);
    
    addMenuItem("Time Settings", "clock", [this]() {
        Serial.println("Time Settings selected");
        // Show current time and NTP status
        String timeInfo = "Current Time: " + String(millis() / 1000) + "s\nNTP: " + 
                         String(WiFi.status() == WL_CONNECTED ? "Synchronized" : "Not Available");
        showMessageScreen("Time Settings", timeInfo);
    }, COLOR_INFO);
    
    addMenuItem("Reset System", "cross", [this]() {
        Serial.println("Reset System selected");
        // Show confirmation for system reset
        showMessageScreen("Reset System", "Are you sure you want to reset?\nPress 5 to confirm, 0 to cancel");
        // Note: Actual reset would need additional implementation
    }, COLOR_DANGER);
    
    addMenuItem("Back to Main", "home", []() {
        showMainMenuScreen();
    }, COLOR_SECONDARY);
    
    drawMenu();
    drawStatusBar();
}

void AdvancedGUI::showLoadingScreen(String message) {
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

void AdvancedGUI::showProgressBar(int x, int y, int width, int height, int progress, uint16_t color) {
    // Background
    fillRectUltraFast(x, y, width, height, COLOR_DARK);
    tft.drawRect(x, y, width, height, COLOR_LIGHT);
    
    // Progress fill
    int fillWidth = (width * progress) / 100;
    if (fillWidth > 0) {
        fillRectUltraFast(x + 1, y + 1, fillWidth - 2, height - 2, color);
    }
}

void AdvancedGUI::updateStatus(String status, bool wifi, int battery) {
    currentStatus = status;
    wifiConnected = wifi;
    batteryLevel = battery;
    drawStatusBar();
}

void AdvancedGUI::showSystemStatus(String message, bool isError) {
    // Clear status area
    fillRectUltraFast(0, 180, 320, 40, COLOR_BACKGROUND);
    
    // Status text
    tft.setTextSize(1);
    tft.setTextColor(isError ? COLOR_DANGER : COLOR_SUCCESS);
    tft.setCursor(160 - (message.length() * 3), 200 - 4);
    tft.print(message);
}

void AdvancedGUI::showMessageScreen(String title, String message) {
    drawBackground();
    drawHeader(title, "");
    
    // Clear main content area
    fillRectUltraFast(0, 60, 320, 160, COLOR_BACKGROUND);
    
    // Display message
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    
    // Split message into lines if it contains \n
    int lineHeight = 12;
    int y = 100;
    int startPos = 0;
    
    while (startPos < message.length()) {
        int endPos = message.indexOf('\n', startPos);
        if (endPos == -1) endPos = message.length();
        
        String line = message.substring(startPos, endPos);
        tft.setCursor(160 - (line.length() * 3), y - 4);
        tft.print(line);
        
        y += lineHeight;
        startPos = endPos + 1;
    }
    
    // Show back instruction
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GRAY);
    tft.setCursor(160 - 30, 200 - 4);
    tft.print("Press 0 to go back");
    
    drawStatusBar();
}

// Global functions
void initAdvancedGUI() {
    if (advancedGUI == nullptr) {
        advancedGUI = new AdvancedGUI(tft);
        advancedGUI->init();
    }
}

void showStartupSequence() {
    if (advancedGUI == nullptr) return;
    
    advancedGUI->drawBackground();
    
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
    advancedGUI->showProgressBar(50, 120, 220, 20, 0, COLOR_PRIMARY);
    
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
    
    // Simulate startup progress
    for (int progress = 0; progress <= 100; progress += 10) {
        advancedGUI->showProgressBar(50, 120, 220, 20, progress, COLOR_PRIMARY);
        
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
        
        advancedGUI->showSystemStatus(statusMsg, false);
        delay(300); // Simulate loading time
    }
    
    delay(1000); // Show "System ready!" for 1 second
}

void showMainMenuScreen() {
    if (advancedGUI) {
        advancedGUI->showMainMenu();
    }
}

void showAttendanceScreen() {
    if (advancedGUI) {
        advancedGUI->showAttendanceMenu();
    }
}

void showRegistrationScreen() {
    if (advancedGUI) {
        advancedGUI->showRegistrationMenu();
    }
}

void showAdminScreen() {
    if (advancedGUI) {
        advancedGUI->showAdminMenu();
    }
}

void showSettingsScreen() {
    if (advancedGUI) {
        advancedGUI->showSettingsMenu();
    }
}

void handleGUIKeypad(char key) {
    if (advancedGUI) {
        advancedGUI->handleKeypad(key);
    }
}

// User Registration Form Implementation
void AdvancedGUI::showStudentRegistrationForm() {
    drawBackground();
    drawHeader("Student Registration", "Enter student information");
    
    // Clear main content area
    fillRectUltraFast(0, 60, 320, 160, COLOR_BACKGROUND);
    
    // Form fields
    drawFormField("Name:", mhs.nama, 80, fieldIndex == 0);
    drawFormField("NIM:", mhs.nim, 100, fieldIndex == 1);
    drawFormField("Class:", mhs.kelas, 120, fieldIndex == 2);
    
    // Current input display
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(10, 170);
    tft.print("Input: " + inputText);
    
    // Instructions
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GRAY);
    tft.setCursor(10, 190);
    tft.print("Use keypad to enter data. Press # to confirm, B to go back");
    
    drawStatusBar();
}

void AdvancedGUI::showAdminRegistrationForm() {
    drawBackground();
    drawHeader("Admin Registration", "Enter admin information");
    
    // Clear main content area
    fillRectUltraFast(0, 60, 320, 160, COLOR_BACKGROUND);
    
    // Form fields
    drawFormField("Name:", admin.nama, 80, fieldIndex == 0);
    drawFormField("NIK:", admin.nik, 100, fieldIndex == 1);
    
    // Current input display
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(10, 150);
    tft.print("Input: " + inputText);
    
    // Instructions
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GRAY);
    tft.setCursor(10, 170);
    tft.print("Use keypad to enter data. Press # to confirm, B to go back");
    
    drawStatusBar();
}

void AdvancedGUI::showFingerprintEnrollmentScreen(String userType) {
    drawBackground();
    drawHeader("Fingerprint Enrollment", userType + " Registration");
    
    // Clear main content area
    fillRectUltraFast(0, 60, 320, 160, COLOR_BACKGROUND);
    
    // Instructions
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(10, 100);
    tft.print("Place your finger on the sensor");
    tft.setCursor(10, 120);
    tft.print("Keep it steady for 3 scans");
    tft.setCursor(10, 140);
    tft.print("Press B to cancel");
    
    // Progress indicator
    tft.setTextSize(1);
    tft.setTextColor(COLOR_PRIMARY);
    tft.setCursor(10, 160);
    tft.print("Scanning...");
    
    // Fingerprint icon
    drawIcon("fingerprint", 250, 100, 40, COLOR_PRIMARY);
    
    drawStatusBar();
}

void AdvancedGUI::showRegistrationSuccessScreen(String userType, String userName) {
    currentState = REGISTRATION_SUCCESS; // Set the state for proper keypad handling
    drawBackground();
    drawHeader("Registration Success", userType + " Registered");
    
    // Clear main content area
    fillRectUltraFast(0, 60, 320, 160, COLOR_BACKGROUND);
    
    // Success message
    tft.setTextSize(2);
    tft.setTextColor(COLOR_SUCCESS);
    tft.setCursor(160 - 36, 100 - 8);
    tft.print("SUCCESS!");
    
    // User info
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(10, 120);
    tft.print(userType + ": " + userName);
    tft.setCursor(10, 140);
    tft.print("Fingerprint ID: " + String(userType == "Student" ? mhs.sidikJariID : admin.sidikJariID));
    
    // Success icon
    drawIcon("check", 250, 100, 40, COLOR_SUCCESS);
    
    // Instructions
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GRAY);
    tft.setCursor(10, 180);
    tft.print("Press 0 or B to return to main menu");
    
    drawStatusBar();
}

void AdvancedGUI::showRegistrationErrorScreen(String error) {
    currentState = REGISTRATION_ERROR; // Set the state for proper keypad handling
    drawBackground();
    drawHeader("Registration Error", "Please try again");
    
    // Clear main content area
    fillRectUltraFast(0, 60, 320, 160, COLOR_BACKGROUND);
    
    // Error message
    tft.setTextSize(2);
    tft.setTextColor(COLOR_DANGER);
    tft.setCursor(160 - 30, 100 - 8);
    tft.print("ERROR!");
    
    // Error details
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(10, 120);
    tft.print("Error: " + error);
    
    // Error icon
    drawIcon("cross", 250, 100, 40, COLOR_DANGER);
    
    // Instructions
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GRAY);
    tft.setCursor(10, 180);
    tft.print("Press 0 or B to return to main menu");
    
    drawStatusBar();
}

void AdvancedGUI::drawFormField(String label, String value, int y, bool selected) {
    // Field background
    if (selected) {
        fillRectUltraFast(5, y - 2, 310, 18, COLOR_SELECTED);
    }
    
    // Label
    tft.setTextSize(1);
    tft.setTextColor(selected ? COLOR_LIGHT : COLOR_GRAY);
    tft.setCursor(10, y + 6);
    tft.print(label);
    
    // Value
    tft.setTextColor(selected ? COLOR_LIGHT : COLOR_LIGHT);
    tft.setCursor(80, y + 6);
    if (value.length() > 0) {
        tft.print(value);
    } else {
        tft.print("(empty)");
    }
    
    // Selection indicator
    if (selected) {
        tft.setTextColor(COLOR_LIGHT);
        tft.setCursor(290, y + 6);
        tft.print(">");
    }
}

void AdvancedGUI::drawInputField(int x, int y, int width, int height, String label, String value, bool selected) {
    // Background
    uint16_t bgColor = selected ? COLOR_SELECTED : COLOR_DARK;
    fillRectUltraFast(x, y, width, height, bgColor);
    tft.drawRect(x, y, width, height, COLOR_LIGHT);
    
    // Label
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GRAY);
    tft.setCursor(x + 5, y - 10);
    tft.print(label);
    
    // Value
    tft.setTextColor(COLOR_LIGHT);
    tft.setCursor(x + 5, y + (height/2) - 4);
    if (value.length() > 0) {
        tft.print(value);
    } else {
        tft.print("Enter " + label);
    }
}

// Form handling implementation
void AdvancedGUI::handleFormInput(char key) {
    // Use existing input handler logic
    if (currentState == ISI_DATA) {
        handleInputFormMahasiswa(key);
    } else if (currentState == ISI_DATA_ADMIN) {
        handleInputFormAdmin(key);
    }
    
    // Refresh the form display
    if (currentState == ISI_DATA) {
        showStudentRegistrationForm();
    } else if (currentState == ISI_DATA_ADMIN) {
        showAdminRegistrationForm();
    }
}

void AdvancedGUI::handleFingerprintEnrollment(char key) {
    if (key == 'B') {
        // Cancel fingerprint enrollment
        Serial.println("🔙 Fingerprint enrollment cancelled");
        if (currentState == ISI_FINGERPRINT) {
            currentState = KONFIRMASI_DATA;
            showStudentRegistrationForm();
        } else if (currentState == ISI_FINGERPRINT_ADMIN) {
            currentState = KONFIRMASI_ADMIN;
            showAdminRegistrationForm();
        }
        return;
    }
    
    // Show fingerprint enrollment screen
    String userType = (currentState == ISI_FINGERPRINT) ? "Student" : "Admin";
    showFingerprintEnrollmentScreen(userType);
    
    // Start fingerprint enrollment process
    bool success = false;
    bool cancelled = false;
    
    if (currentState == ISI_FINGERPRINT) {
        success = enrollFingerprintMahasiswa(nextFingerID, &cancelled);
    } else if (currentState == ISI_FINGERPRINT_ADMIN) {
        success = enrollFingerprintAdmin(nextAdminFingerID, &cancelled);
    }
    
    if (cancelled) {
        // User cancelled, return to form
        if (currentState == ISI_FINGERPRINT) {
            currentState = KONFIRMASI_DATA;
            showStudentRegistrationForm();
        } else if (currentState == ISI_FINGERPRINT_ADMIN) {
            currentState = KONFIRMASI_ADMIN;
            showAdminRegistrationForm();
        }
    } else if (success) {
        // Fingerprint enrollment successful, proceed with registration
        proceedToFingerprintEnrollment();
    } else {
        // Fingerprint enrollment failed
        showRegistrationErrorScreen("Fingerprint enrollment failed. Please try again.");
    }
}

void AdvancedGUI::handleFormConfirmation(char key) {
    if (key == 'A') {
        // Confirm registration
        if (validateForm()) {
            // Proceed to fingerprint enrollment
            if (currentState == KONFIRMASI_DATA) {
                currentState = ISI_FINGERPRINT;
                showFingerprintEnrollmentScreen("Student");
            } else if (currentState == KONFIRMASI_ADMIN) {
                currentState = ISI_FINGERPRINT_ADMIN;
                showFingerprintEnrollmentScreen("Admin");
            }
        } else {
            showRegistrationErrorScreen("Please fill all required fields");
        }
    } else if (key == 'B') {
        // Go back to form editing
        if (currentState == KONFIRMASI_DATA) {
            currentState = ISI_DATA;
            showStudentRegistrationForm();
        } else if (currentState == KONFIRMASI_ADMIN) {
            currentState = ISI_DATA_ADMIN;
            showAdminRegistrationForm();
        }
    }
}

bool AdvancedGUI::validateForm() {
    if (currentState == KONFIRMASI_DATA) {
        // Validate student form
        return mhs.nama.length() > 0 && 
               mhs.nim.length() > 0 && 
               mhs.kelas.length() > 0;
    } else if (currentState == KONFIRMASI_ADMIN) {
        // Validate admin form
        return admin.nama.length() > 0 && 
               admin.nik.length() > 0;
    }
    return false;
}

void AdvancedGUI::proceedToFingerprintEnrollment() {
    // This will be called after successful fingerprint enrollment
    // Create user via API and show success screen
    
    bool success = false;
    String userType = "";
    String userName = "";
    
    if (currentState == ISI_FINGERPRINT) {
        // Create student via database
        StudentDB studentDB;
        studentDB.nim = mhs.nim;
        studentDB.nama = mhs.nama;
        studentDB.kelas = mhs.kelas;
        studentDB.fingerprints = String(nextFingerID);

        success = dbCreateStudent(studentDB);
        if (success) {
            mhs.sidikJariID = nextFingerID;
            nextFingerID++;
        }
        userType = "Student";
        userName = mhs.nama;
        
    } else if (currentState == ISI_FINGERPRINT_ADMIN) {
        // Admin creation - keep using local storage for now
        // (Admin functionality not implemented in database)
        admin.sidikJariID = nextAdminFingerID;
        nextAdminFingerID++;
        success = true;
        userType = "Admin";
        userName = admin.nama;
    }
    
    if (success) {
        showRegistrationSuccessScreen(userType, userName);
        // Reset form data
        mhs = Mahasiswa();
        admin = Admin();
        fieldIndex = 0;
        inputText = "";
    } else {
        showRegistrationErrorScreen("Failed to create user. Please try again.");
    }
}
