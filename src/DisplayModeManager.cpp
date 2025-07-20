//
// Created by Tikaram Mardi on 20/07/25.
//
#include "DisplayModeManager.h"
#include "TimeManager.h"
#include "Logger.h"

MD_Parola* DisplayModeManager::display = nullptr;
DisplaySettings DisplayModeManager::currentSettings = {
    "Cotton Cat!!",      // message
    DEFAULT_BRIGHTNESS,  // brightness
    DEFAULT_SCROLL_SPEED,// speed
    0,                   // animationIndex
    MODE_AUTO_CYCLE,     // mode
    true                 // autoCycleEnabled
};

unsigned long DisplayModeManager::lastModeSwitch = 0;
unsigned long DisplayModeManager::lastDisplayUpdate = 0;
bool DisplayModeManager::isScrollInitialized = false;
int DisplayModeManager::currentCycleIndex = 0;

// Define the cycle order for auto mode
DisplayMode DisplayModeManager::cycleModes[] = {
    MODE_CLOCK, MODE_DAY, MODE_DATE, MODE_SCROLL
};

void DisplayModeManager::init(MD_Parola* displayInstance) {
    display = displayInstance;
    display->begin();
    display->setIntensity(currentSettings.brightness);
    display->displayClear();
    display->setTextAlignment(PA_CENTER);

    lastModeSwitch = millis();
    lastDisplayUpdate = millis();

    Logger::info("Display Mode Manager initialized", "DISPLAY");
    Logger::info("Current mode: " + getCurrentModeString(), "DISPLAY");
}

void DisplayModeManager::update() {
    if (!display) return;

    // Handle auto cycle mode
    if (currentSettings.mode == MODE_AUTO_CYCLE && currentSettings.autoCycleEnabled) {
        if (millis() - lastModeSwitch >= MODE_SWITCH_INTERVAL) {
            // Handle scroll text completion
            if (cycleModes[currentCycleIndex] == MODE_SCROLL) {
                if (display->displayAnimate()) {
                    // Scroll completed, move to next mode
                    nextMode();
                }
            } else {
                // Non-scroll modes, switch after interval
                nextMode();
            }
        }
    }

    // Update display based on current mode
    displayCurrentMode();

    // Animate scroll text if in scroll mode
    if ((currentSettings.mode == MODE_SCROLL) ||
        (currentSettings.mode == MODE_AUTO_CYCLE && cycleModes[currentCycleIndex] == MODE_SCROLL)) {
        display->displayAnimate();
    }
}

void DisplayModeManager::displayCurrentMode() {
    DisplayMode activeMode = currentSettings.mode;

    // If in auto cycle, use the current cycle mode
    if (currentSettings.mode == MODE_AUTO_CYCLE) {
        activeMode = cycleModes[currentCycleIndex];
    }

    switch (activeMode) {
        case MODE_MANUAL:
            displayManualMessage();
            break;
        case MODE_CLOCK:
            displayClock();
            break;
        case MODE_DAY:
            displayDay();
            break;
        case MODE_DATE:
            displayDate();
            break;
        case MODE_SCROLL:
            displayScrollText();
            break;
    }
}

void DisplayModeManager::displayManualMessage() {
    // For manual mode, only update when message changes
    static String lastMessage = "";
    if (currentSettings.message != lastMessage) {
        display->displayClear();
        display->displayText(currentSettings.message.c_str(), PA_CENTER,
                           currentSettings.speed, 0,
                           ANIMATION_EFFECTS[currentSettings.animationIndex].effectIn,
                           ANIMATION_EFFECTS[currentSettings.animationIndex].effectOut);
        lastMessage = currentSettings.message;
        display->displayAnimate();
    }
}

void DisplayModeManager::displayClock() {
    if (millis() - lastDisplayUpdate >= 1000) { // Update every second
        lastDisplayUpdate = millis();
        String timeStr = TimeManager::getTimeString();
        display->print(timeStr.c_str());
    }
}

void DisplayModeManager::displayDay() {
    String dayStr = TimeManager::getDayString();
    display->print(dayStr.c_str());
}

void DisplayModeManager::displayDate() {
    String dateStr = TimeManager::getDateString();
    display->print(dateStr.c_str());
}

void DisplayModeManager::displayScrollText() {
    if (!isScrollInitialized) {
        display->displayText(currentSettings.message.c_str(), PA_CENTER,
                           100, 100, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        isScrollInitialized = true;
    }
}

void DisplayModeManager::nextMode() {
    lastModeSwitch = millis();

    if (currentSettings.mode == MODE_AUTO_CYCLE) {
        currentCycleIndex = (currentCycleIndex + 1) % 4; // 4 modes in cycle
        display->displayClear();
        isScrollInitialized = false;

        Logger::debug("Switched to mode: " + String(currentCycleIndex) +
                     " (" + getCurrentModeString() + ")", "DISPLAY");
    }
}

void DisplayModeManager::setSettings(const DisplaySettings& settings) {
    bool modeChanged = (settings.mode != currentSettings.mode);
    bool brightnessChanged = (settings.brightness != currentSettings.brightness);

    currentSettings = settings;

    if (brightnessChanged) {
        display->setIntensity(currentSettings.brightness);
    }

    if (modeChanged) {
        display->displayClear();
        isScrollInitialized = false;
        lastModeSwitch = millis();
        currentCycleIndex = 0; // Reset cycle
        Logger::info("Display mode changed to: " + getCurrentModeString(), "DISPLAY");
    }
}

DisplaySettings DisplayModeManager::getSettings() {
    return currentSettings;
}

void DisplayModeManager::setMode(DisplayMode mode) {
    if (mode != currentSettings.mode) {
        currentSettings.mode = mode;
        display->displayClear();
        isScrollInitialized = false;
        lastModeSwitch = millis();
        Logger::info("Mode manually set to: " + getCurrentModeString(), "DISPLAY");
    }
}

void DisplayModeManager::enableAutoCycle(bool enable) {
    currentSettings.autoCycleEnabled = enable;
    Logger::info("Auto cycle " + String(enable ? "enabled" : "disabled"), "DISPLAY");
}

String DisplayModeManager::getCurrentModeString() {
    switch (currentSettings.mode) {
        case MODE_MANUAL: return "Manual Message";
        case MODE_CLOCK: return "Clock Display";
        case MODE_DAY: return "Day Display";
        case MODE_DATE: return "Date Display";
        case MODE_SCROLL: return "Scroll Text";
        case MODE_AUTO_CYCLE: return "Auto Cycle";
        default: return "Unknown Mode";
    }
}

bool DisplayModeManager::isAutoCycleMode() {
    return currentSettings.mode == MODE_AUTO_CYCLE;
}
