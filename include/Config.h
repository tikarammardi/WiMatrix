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

// Display Settings - Optimized for 8x32
#define DEFAULT_BRIGHTNESS 3
#define DEFAULT_SCROLL_SPEED 80
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 8
#define MIN_SPEED 40
#define MAX_SPEED 150

// Time Display Settings - Optimized for small display
#define MODE_SWITCH_INTERVAL 3000  // 3 seconds for 8x32
#define NTP_UPDATE_INTERVAL 60000
#define IST_OFFSET 19800

// WiFi Configuration
#define WIFI_CONNECTION_TIMEOUT 30  // seconds to wait for WiFi connection
#define WIFI_RETRY_DELAY 1000       // milliseconds between connection attempts

// Display Modes
enum DisplayMode {
    MODE_MANUAL,     // Show custom message only
    MODE_CLOCK,      // Show clock only
    MODE_DAY,        // Day of week only
    MODE_DATE,       // Date only
    MODE_SCROLL,     // Scroll text only
    MODE_AUTO_CYCLE  // Auto cycle through enabled modes
};

// Mode Enable/Disable Structure
struct ModeSettings {
    bool clockEnabled = true;
    bool dayEnabled = true;
    bool dateEnabled = true;
    bool messageEnabled = true;
    int cycleInterval = 3; // seconds
};

// 8x32 Optimized Time Display Constants
static const char* DAYS_SHORT[] = {
    "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

static const char* MONTHS_SHORT[] = {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

#endif // CONFIG_H
