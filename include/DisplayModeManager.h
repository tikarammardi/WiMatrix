#ifndef DISPLAY_MODE_MANAGER_H
#define DISPLAY_MODE_MANAGER_H

#include <Arduino.h>
#include <MD_Parola.h>
#include "Config.h"

struct DisplaySettings {
    String message;
    int brightness;
    int speed;
    DisplayMode mode;
    bool autoCycleEnabled;
    ModeSettings modeSettings;
};

class DisplayModeManager {
private:
    static MD_Parola* display;
    static DisplaySettings currentSettings;
    static unsigned long lastModeSwitch;
    static unsigned long lastDisplayUpdate;
    static bool isScrollInitialized;
    static DisplayMode cycleModes[];
    static int currentCycleIndex;

    static void initializeScrollText();
    static void displayCurrentMode();
    static DisplayMode getNextEnabledMode();

public:
    static void init(MD_Parola* displayInstance);
    static void update();
    static void setSettings(const DisplaySettings& settings);
    static DisplaySettings getSettings();

    // Mode control
    static void setMode(DisplayMode mode);
    static void enableAutoCycle(bool enable);
    static void nextMode();

    // Display functions for each mode
    static void displayManualMessage();
    static void displayClock();
    static void displayDay();
    static void displayDate();
    static void displayScrollText();

    // Utility functions
    static String getCurrentModeString();
    static bool isAutoCycleMode();
};

#endif // DISPLAY_MODE_MANAGER_H
