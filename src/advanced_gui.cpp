#include "advanced_gui.h"
#include "state.h"
#include "display_manager.h"

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
    // Update status bar every 5 seconds
    if (millis() - lastUpdateTime > 5000) {
        drawStatusBar();
        lastUpdateTime = millis();
    }
}

void AdvancedGUI::handleKeypad(char key) {
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
    tft.fillScreen(COLOR_BACKGROUND);
}

void AdvancedGUI::drawHeader(String title, String subtitle) {
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

void AdvancedGUI::drawStatusBar() {
    // Status bar background
    tft.fillRect(0, 220, 320, 20, COLOR_DARK);
    
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
    tft.fillRect(0, 60, 320, 160, COLOR_BACKGROUND);
    
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
        tft.fillRect(10, y - 5, 300, 30, COLOR_SELECTED);
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
    }, COLOR_SUCCESS);
    
    addMenuItem("Check Out", "clock", []() {
        currentState = ABSEN;
        Serial.println("Check Out selected");
    }, COLOR_WARNING);
    
    addMenuItem("Register Student", "fingerprint", []() {
        currentState = ISI_DATA;
        Serial.println("Register Student selected");
    }, COLOR_PRIMARY);
    
    addMenuItem("Register Admin", "user", []() {
        currentState = ISI_DATA_ADMIN;
        Serial.println("Register Admin selected");
    }, COLOR_INFO);
    
    addMenuItem("Settings", "settings", []() {
        currentState = SUBMENU_EDIT_USER;
        Serial.println("Settings selected");
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
    }, COLOR_SUCCESS);
    
    addMenuItem("Check Out", "clock", []() {
        currentState = ABSEN;
        Serial.println("Check Out selected");
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
    }, COLOR_PRIMARY);
    
    addMenuItem("Register Admin", "user", []() {
        currentState = ISI_DATA_ADMIN;
        Serial.println("Register Admin selected");
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
    
    addMenuItem("View Users", "user", []() {
        Serial.println("View Users selected");
    }, COLOR_INFO);
    
    addMenuItem("Edit Users", "settings", []() {
        currentState = SUBMENU_EDIT_USER;
        Serial.println("Edit Users selected");
    }, COLOR_WARNING);
    
    addMenuItem("System Info", "settings", []() {
        Serial.println("System Info selected");
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
    
    addMenuItem("WiFi Settings", "wifi", []() {
        Serial.println("WiFi Settings selected");
    }, COLOR_INFO);
    
    addMenuItem("Time Settings", "clock", []() {
        Serial.println("Time Settings selected");
    }, COLOR_INFO);
    
    addMenuItem("Reset System", "cross", []() {
        Serial.println("Reset System selected");
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
    tft.fillRect(x, y, width, height, COLOR_DARK);
    tft.drawRect(x, y, width, height, COLOR_LIGHT);
    
    // Progress fill
    int fillWidth = (width * progress) / 100;
    if (fillWidth > 0) {
        tft.fillRect(x + 1, y + 1, fillWidth - 2, height - 2, color);
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
    tft.fillRect(0, 180, 320, 40, COLOR_BACKGROUND);
    
    // Status text
    tft.setTextSize(1);
    tft.setTextColor(isError ? COLOR_DANGER : COLOR_SUCCESS);
    tft.setCursor(160 - (message.length() * 3), 200 - 4);
    tft.print(message);
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
