#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Config.h"

MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup() {
    // Initialize display with constants from Config.h
    display.begin();
    display.setIntensity(DEFAULT_BRIGHTNESS);
    display.displayClear();
    display.displayText("Config Module!", PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
}

void loop() {
    // Keep display animating
    display.displayAnimate();
}
