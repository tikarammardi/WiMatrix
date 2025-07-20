//
// Created by Tikaram Mardi on 20/07/25.
//
#include "DisplayModeManager.h"
#include "TimeManager.h"
#include "Logger.h"

// Static member initialization
MD_Parola* DisplayModeManager::display = nullptr;
DisplaySettings DisplayModeManager::currentSettings = {
    "Cotton Cat!!",                       // message
    DEFAULT_BRIGHTNESS,          // brightness
    DEFAULT_SCROLL_SPEED,        // speed
    MODE_AUTO_CYCLE,            // mode
    true,                       // autoCycleEnabled
    {true, true, true, true, 3} // modeSettings (all enabled, 3s interval)
};

unsigned long DisplayModeManager::lastModeSwitch = 0;
unsigned long DisplayModeManager::lastDisplayUpdate = 0;
bool DisplayModeManager::isScrollInitialized = false;
int DisplayModeManager::currentCycleIndex = 0;

// Define the cycle order for auto mode
DisplayMode DisplayModeManager::cycleModes[] = {
    MODE_CLOCK, MODE_DAY, MODE_DATE, MODE_MANUAL
};

DisplayMode DisplayModeManager::getNextEnabledMode() {
    int attempts = 0;
    int startIndex = currentCycleIndex;

    while (attempts < 4) { // Max 4 attempts to avoid infinite loop
        currentCycleIndex = (currentCycleIndex + 1) % 4;

        DisplayMode nextMode = cycleModes[currentCycleIndex];

        // Check if this mode is enabled
        bool isEnabled = false;
        switch (nextMode) {
            case MODE_CLOCK:
                isEnabled = currentSettings.modeSettings.clockEnabled;
                break;
            case MODE_DAY:
                isEnabled = currentSettings.modeSettings.dayEnabled;
                break;
            case MODE_DATE:
                isEnabled = currentSettings.modeSettings.dateEnabled;
                break;
            case MODE_MANUAL:
                isEnabled = currentSettings.modeSettings.messageEnabled;
                break;
            default:
                isEnabled = false;
                break;
        }

        if (isEnabled) {
            Logger::info("Next enabled mode: " + String(nextMode) + " (index " + String(currentCycleIndex) + ")", "DISPLAY");
            return nextMode;
        }

        attempts++;

        // If we've cycled back to start and found nothing, break
        if (currentCycleIndex == startIndex && attempts > 0) {
            break;
        }
    }

    // Fallback: enable clock mode if nothing else is enabled
    currentSettings.modeSettings.clockEnabled = true;
    Logger::warn("No modes enabled! Falling back to clock mode", "DISPLAY");
    return MODE_CLOCK;
}

void DisplayModeManager::init(MD_Parola* displayInstance) {
    if (!displayInstance) {
        Logger::error("Display instance is null!", "DISPLAY");
        return;
    }

    display = displayInstance;
    display->begin();
    display->setIntensity(currentSettings.brightness);
    display->displayClear();
    display->setTextAlignment(PA_CENTER);

    lastModeSwitch = millis();
    lastDisplayUpdate = millis();
    isScrollInitialized = false;
    currentCycleIndex = 0;

    Logger::info("Display Mode Manager initialized with individual mode controls", "DISPLAY");
    Logger::info("Current mode: " + getCurrentModeString(), "DISPLAY");
    Logger::info("Mode settings - Clock:" + String(currentSettings.modeSettings.clockEnabled ? "ON" : "OFF") +
                " Day:" + String(currentSettings.modeSettings.dayEnabled ? "ON" : "OFF") +
                " Date:" + String(currentSettings.modeSettings.dateEnabled ? "ON" : "OFF") +
                " Message:" + String(currentSettings.modeSettings.messageEnabled ? "ON" : "OFF"), "DISPLAY");
}

void DisplayModeManager::update() {
    if (!display) {
        Logger::error("Display not initialized!", "DISPLAY");
        return;
    }

    // Handle auto cycle mode with custom interval
    if (currentSettings.mode == MODE_AUTO_CYCLE && currentSettings.autoCycleEnabled) {
        unsigned long interval = currentSettings.modeSettings.cycleInterval * 1000; // Convert to milliseconds

        if (millis() - lastModeSwitch >= interval) {
            nextMode();
        }
    }

    // Update display based on current mode
    displayCurrentMode();

    // Only animate scroll for message mode
    DisplayMode activeMode = currentSettings.mode;
    if (currentSettings.mode == MODE_AUTO_CYCLE) {
        activeMode = cycleModes[currentCycleIndex];
    }

    if (activeMode == MODE_MANUAL && isScrollInitialized) {
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
        default:
            displayClock(); // Fallback
            break;
    }
}

void DisplayModeManager::displayManualMessage() {
    if (!isScrollInitialized) {
        display->displayClear();

        // Simple scroll left animation for all messages
        if (currentSettings.message.length() <= 5) {
            // Static display for short messages
            display->print(currentSettings.message.c_str());
        } else {
            // Simple scroll left for longer messages
            display->displayText(currentSettings.message.c_str(), PA_CENTER,
                               currentSettings.speed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        }
        isScrollInitialized = true;
        Logger::debug("Message display initialized: '" + currentSettings.message + "'", "DISPLAY");
    }
}

void DisplayModeManager::displayClock() {
    String timeStr = TimeManager::getTimeString();
    display->print(timeStr.c_str());
    Logger::debug("Clock display: " + timeStr, "DISPLAY");
}

void DisplayModeManager::displayDay() {
    String currentDay = TimeManager::getDayString();
    display->print(currentDay.c_str());
    Logger::debug("Day display: " + currentDay, "DISPLAY");
}

void DisplayModeManager::displayDate() {
    String currentDate = TimeManager::getDateString(); // Always "20 JAN" format
    display->print(currentDate.c_str());
    Logger::debug("Date display: " + currentDate, "DISPLAY");
}

void DisplayModeManager::displayScrollText() {
    displayManualMessage();
}

void DisplayModeManager::setSettings(const DisplaySettings& settings) {
    bool modeChanged = (settings.mode != currentSettings.mode);
    bool brightnessChanged = (settings.brightness != currentSettings.brightness);
    bool messageChanged = (settings.message != currentSettings.message);
    bool speedChanged = (settings.speed != currentSettings.speed);

    bool modeSettingsChanged = (
        settings.modeSettings.clockEnabled != currentSettings.modeSettings.clockEnabled ||
        settings.modeSettings.dayEnabled != currentSettings.modeSettings.dayEnabled ||
        settings.modeSettings.dateEnabled != currentSettings.modeSettings.dateEnabled ||
        settings.modeSettings.messageEnabled != currentSettings.modeSettings.messageEnabled ||
        settings.modeSettings.cycleInterval != currentSettings.modeSettings.cycleInterval
    );

    // Log changes
    if (modeChanged) {
        Logger::info("Mode changed from " + getCurrentModeString() + " to " + String(settings.mode), "DISPLAY");
    }
    if (messageChanged) {
        Logger::info("Message changed from '" + currentSettings.message + "' to '" + settings.message + "'", "DISPLAY");
    }
    if (brightnessChanged) {
        Logger::info("Brightness changed from " + String(currentSettings.brightness) + " to " + String(settings.brightness), "DISPLAY");
    }

    // Update settings
    currentSettings = settings;

    // Apply brightness change immediately
    if (brightnessChanged && display) {
        display->setIntensity(currentSettings.brightness);
    }

    // Reset display state if significant changes occurred
    if (modeChanged || messageChanged || speedChanged || modeSettingsChanged) {
        if (display) {
            display->displayClear();
        }
        isScrollInitialized = false;
        lastModeSwitch = millis();
        currentCycleIndex = 0; // Reset cycle position

        Logger::info("Display reset due to settings change", "DISPLAY");
    }

    // Log current mode settings
    if (modeSettingsChanged) {
        Logger::info("Mode settings updated - Clock:" + String(currentSettings.modeSettings.clockEnabled ? "ON" : "OFF") +
                    " Day:" + String(currentSettings.modeSettings.dayEnabled ? "ON" : "OFF") +
                    " Date:" + String(currentSettings.modeSettings.dateEnabled ? "ON" : "OFF") +
                    " Message:" + String(currentSettings.modeSettings.messageEnabled ? "ON" : "OFF") +
                    " Interval:" + String(currentSettings.modeSettings.cycleInterval) + "s", "DISPLAY");
    }
}

DisplaySettings DisplayModeManager::getSettings() {
    return currentSettings;
}

void DisplayModeManager::setMode(DisplayMode mode) {
    if (mode != currentSettings.mode) {
        Logger::info("Mode manually changed from " + getCurrentModeString() + " to " + String(mode), "DISPLAY");
        currentSettings.mode = mode;

        if (display) {
            display->displayClear();
        }
        isScrollInitialized = false;
        lastModeSwitch = millis();
        currentCycleIndex = 0;
    }
}

void DisplayModeManager::enableAutoCycle(bool enable) {
    if (enable != currentSettings.autoCycleEnabled) {
        currentSettings.autoCycleEnabled = enable;
        Logger::info("Auto cycle " + String(enable ? "enabled" : "disabled"), "DISPLAY");

        if (enable) {
            lastModeSwitch = millis(); // Reset timer when enabling
        }
    }
}

void DisplayModeManager::nextMode() {
    lastModeSwitch = millis();

    if (currentSettings.mode == MODE_AUTO_CYCLE) {
        DisplayMode nextMode = getNextEnabledMode();
        if (display) {
            display->displayClear();
        }
        isScrollInitialized = false;

        String modeName = "";
        switch (nextMode) {
            case MODE_CLOCK: modeName = "CLOCK"; break;
            case MODE_DAY: modeName = "DAY"; break;
            case MODE_DATE: modeName = "DATE"; break;
            case MODE_MANUAL: modeName = "MESSAGE"; break;
            default: modeName = "UNKNOWN"; break;
        }

        Logger::info("Auto cycle switched to: " + String(nextMode) + " (" + modeName + ")", "DISPLAY");
    } else {
        // Manual mode cycling through all modes
        int nextModeInt = (static_cast<int>(currentSettings.mode) + 1) % 6;
        currentSettings.mode = static_cast<DisplayMode>(nextModeInt);

        if (display) {
            display->displayClear();
        }
        isScrollInitialized = false;
        Logger::info("Manual cycle to: " + getCurrentModeString(), "DISPLAY");
    }
}

String DisplayModeManager::getCurrentModeString() {
    switch (currentSettings.mode) {
        case MODE_MANUAL: return "Message Only";
        case MODE_CLOCK: return "Clock Only";
        case MODE_DAY: return "Day Only";
        case MODE_DATE: return "Date Only";
        case MODE_SCROLL: return "Scroll Only";
        case MODE_AUTO_CYCLE: return "Auto Cycle";
        default: return "Unknown Mode";
    }
}

bool DisplayModeManager::isAutoCycleMode() {
    return currentSettings.mode == MODE_AUTO_CYCLE;
}

void DisplayModeManager::initializeScrollText() {
    // This method is called internally by displayManualMessage
    // Left here for compatibility but functionality moved to displayManualMessage
}
