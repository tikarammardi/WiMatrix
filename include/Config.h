//
// Created by Tikaram Mardi on 19/07/25.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <MD_MAX72xx.h>

// Hardware Configuration Constants
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // 4 modules = 8x32 display

// Pin Definitions for Wemos D1
#define DATA_PIN  D7   // GPIO13 - DIN
#define CLK_PIN   D5   // GPIO14 - CLK
#define CS_PIN    D6   // GPIO12 - CS/LOAD

// Display Settings
#define DEFAULT_BRIGHTNESS 5  // 0-15 range
#define DEFAULT_SCROLL_SPEED 60

#endif // CONFIG_H
