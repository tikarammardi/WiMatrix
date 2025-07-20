#ifndef CONFIG_H
#define CONFIG_H

#include <MD_MAX72xx.h>
#include <MD_Parola.h>

// Hardware Configuration Constants
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // 4 modules = 8x32 display

// Pin Definitions for Wemos D1
#define DATA_PIN  D7   // GPIO13 - DIN
#define CLK_PIN   D5   // GPIO14 - CLK
#define CS_PIN    D6   // GPIO12 - CS/LOAD

// Display Settings
#define DEFAULT_BRIGHTNESS 5      // 0-15 range
#define DEFAULT_SCROLL_SPEED 60   // milliseconds
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 15
#define MIN_SPEED 20
#define MAX_SPEED 200

// WiFi Configuration
#define WIFI_CONNECTION_TIMEOUT 30  // seconds
#define WIFI_RETRY_DELAY 1000       // milliseconds

// Animation Effects
struct AnimationEffect {
    const char* name;
    textEffect_t effectIn;
    textEffect_t effectOut;
};

// Available animation effects
static const AnimationEffect ANIMATION_EFFECTS[] = {
    {"Scroll Left", PA_SCROLL_LEFT, PA_SCROLL_LEFT},
    {"Scroll Right", PA_SCROLL_RIGHT, PA_SCROLL_RIGHT},
    {"Scroll Up", PA_SCROLL_UP, PA_SCROLL_UP},
    {"Scroll Down", PA_SCROLL_DOWN, PA_SCROLL_DOWN},
    {"Wipe", PA_WIPE, PA_WIPE},
    {"Wipe Curve", PA_WIPE_CURSOR, PA_WIPE_CURSOR},
    {"Fade", PA_FADE, PA_FADE},
    {"Dissolve", PA_DISSOLVE, PA_DISSOLVE},
    {"Blinds", PA_BLINDS, PA_BLINDS},
    {"Random", PA_RANDOM, PA_RANDOM}
};

#define ANIMATION_COUNT (sizeof(ANIMATION_EFFECTS) / sizeof(AnimationEffect))

#endif // CONFIG_H
