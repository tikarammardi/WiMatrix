#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Hardware Configuration
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // 4 modules = 8x32 display

#define DATA_PIN  D7   // GPIO13 - DIN
#define CLK_PIN   D5   // GPIO14 - CLK
#define CS_PIN    D6   // GPIO12 - CS/LOAD

MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup() {
    // Initialize display
    display.begin();
    display.setIntensity(5);  // Brightness 0-15
    display.displayClear();
    display.displayText("Hello Matrix!", PA_CENTER, 60, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
}

void loop() {
    // Keep display animating
    display.displayAnimate();
}
