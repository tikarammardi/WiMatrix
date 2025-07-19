#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Config.h"

MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup() {

    Serial.begin(115200);
    delay(1000);  // Give serial time to initialize

    Serial.println("\n=== WiMatrix LED Controller Starting ===");
    Serial.println("Hardware Configuration:");
    Serial.println("- Board: Wemos D1 (ESP8266)");
    Serial.println("- Display: MAX7219 8x32 LED Matrix");
    Serial.println("- Data Pin: D7 (GPIO13)");
    Serial.println("- Clock Pin: D5 (GPIO14)");
    Serial.println("- CS Pin: D6 (GPIO12)");
    Serial.println("- Modules: " + String(MAX_DEVICES));

    // Initialize display with constants from Config.h
    Serial.print("Initializing LED display... ");
    display.begin();
    display.setIntensity(DEFAULT_BRIGHTNESS);
    display.displayClear();
    display.displayText("Serial Debug!", PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    Serial.println("Done!");

    Serial.println("Current Message: 'Serial Debug!'");
    Serial.println("Brightness: " + String(DEFAULT_BRIGHTNESS) + "/15");
    Serial.println("Scroll Speed: " + String(DEFAULT_SCROLL_SPEED));
    Serial.println("=== Setup Complete ===\n");
}

void loop() {

    if (display.displayAnimate()) {

        Serial.println("Animation cycle completed, restarting...");
    }

    
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 10000) {
        lastStatus = millis();
        Serial.println("Status: Running | Uptime: " + String(millis() / 1000) + "s");
    }
}
