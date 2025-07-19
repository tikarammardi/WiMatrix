#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include "Config.h"

// Create display instance
MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// WiFi credentials from environment variables (injected via build_flags)

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== WiMatrix LED Controller Starting ===");
  Serial.println("Hardware Configuration:");
  Serial.println("- Board: Wemos D1 (ESP8266)");
  Serial.println("- Display: MAX7219 8x32 LED Matrix");
  Serial.println("- Modules: " + String(MAX_DEVICES));
  Serial.println("- Chip ID: " + String(ESP.getChipId()));
  Serial.println("- Flash Size: " + String(ESP.getFlashChipSize()));

  // Initialize display
  Serial.print("Initializing LED display... ");
  display.begin();
  display.setIntensity(DEFAULT_BRIGHTNESS);
  display.displayClear();
  display.displayText("WiFi Connecting...", PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  Serial.println("Done!");

  // Connect to WiFi
  Serial.println("\n--- WiFi Connection ---");
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  Serial.println("Using environment-configured credentials");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < WIFI_CONNECTION_TIMEOUT) {
    delay(WIFI_RETRY_DELAY);
    Serial.print(".");
    attempts++;

    // Show connection progress on display
    String connectMsg = "Connecting... " + String(attempts) + "/" + String(WIFI_CONNECTION_TIMEOUT);
    display.displayClear();
    display.displayText(connectMsg.c_str(), PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

    // Give display time to show message
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected Successfully!");
    Serial.println("🌐 Network SSID: " + WiFi.SSID());
    Serial.println("📡 Signal Strength: " + String(WiFi.RSSI()) + " dBm");
    Serial.println("🔗 IP Address: " + WiFi.localIP().toString());
    Serial.println("🚪 Gateway: " + WiFi.gatewayIP().toString());
    Serial.println("🏠 Subnet Mask: " + WiFi.subnetMask().toString());
    Serial.println("📱 MAC Address: " + WiFi.macAddress());

    // Show success on display with IP
    String successMsg = "Connected! IP: " + WiFi.localIP().toString();
    display.displayClear();
    display.displayText(successMsg.c_str(), PA_CENTER, 40, 5000, PA_SCROLL_LEFT, PA_NO_EFFECT);

  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
    Serial.println("Please check your .envrc.local credentials");
    Serial.println("Expected: WIFI_SSID and WIFI_PASSWORD environment variables");

    // Show failure on display
    display.displayClear();
    display.displayText("WiFi Failed - Check .envrc.local", PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }

  Serial.println("=== Setup Complete ===\n");
}

void loop() {
  // Keep display animating
  if (display.displayAnimate()) {
    // After IP display timeout, show current status
    if (WiFi.status() == WL_CONNECTED) {
      String statusMsg = "Online: " + WiFi.localIP().toString();
      display.displayText(statusMsg.c_str(), PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    } else {
      display.displayText("WiFi Disconnected", PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    }
  }

  // Monitor WiFi status every 15 seconds
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 15000) {
    lastWiFiCheck = millis();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi Status: Connected | IP: " + WiFi.localIP().toString() +
                    " | Signal: " + String(WiFi.RSSI()) + " dBm | Uptime: " + String(millis() / 1000) + "s");
    } else {
      Serial.println("WiFi Status: Disconnected - Attempting reconnection...");
      WiFi.reconnect();

      display.displayClear();
      display.displayText("Reconnecting WiFi...", PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    }
  }
}
