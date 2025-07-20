#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Config.h"
#include "Logger.h"
#include "WebTemplates.h"

// Create display instance
MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// WiFi credentials from environment variables
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Create web server on port 80
ESP8266WebServer server(80);

// Current display settings
DisplaySettings currentSettings = {
    "Welcome to WiMatrix!",  // message
    DEFAULT_BRIGHTNESS,      // brightness
    DEFAULT_SCROLL_SPEED,    // speed
    0                        // animationIndex (Scroll Left)
};

// Helper function to update LED display
void updateLEDDisplay() {
    display.displayClear();
    display.setIntensity(currentSettings.brightness);

    textEffect_t effect = ANIMATION_EFFECTS[currentSettings.animationIndex].effectIn;
    display.displayText(currentSettings.message.c_str(), PA_CENTER, currentSettings.speed, 0, effect, effect);

    Logger::info("Display updated - Message: '" + currentSettings.message +
                "', Brightness: " + String(currentSettings.brightness) +
                ", Speed: " + String(currentSettings.speed) +
                ", Animation: " + String(ANIMATION_EFFECTS[currentSettings.animationIndex].name), "DISPLAY");
}

// Web server handlers
void handleRoot() {
    String html = WebTemplates::getMainPage(WiFi.SSID(), WiFi.localIP().toString(), currentSettings);
    server.send(200, "text/html", html);
    Logger::logWebRequest("GET", "/", 200);
}

void handleMessageUpdate() {
    if (server.method() == HTTP_POST) {
        bool settingsChanged = false;

        // Update message
        if (server.hasArg("message")) {
            String newMessage = server.arg("message");
            newMessage.trim();
            if (newMessage.length() == 0) {
                newMessage = "Empty Message";
            }
            if (newMessage != currentSettings.message) {
                currentSettings.message = newMessage;
                settingsChanged = true;
            }
        }

        // Update brightness
        if (server.hasArg("brightness")) {
            int newBrightness = server.arg("brightness").toInt();
            if (newBrightness >= MIN_BRIGHTNESS && newBrightness <= MAX_BRIGHTNESS) {
                if (newBrightness != currentSettings.brightness) {
                    currentSettings.brightness = newBrightness;
                    settingsChanged = true;
                }
            }
        }

        // Update speed
        if (server.hasArg("speed")) {
            int newSpeed = server.arg("speed").toInt();
            if (newSpeed >= MIN_SPEED && newSpeed <= MAX_SPEED) {
                if (newSpeed != currentSettings.speed) {
                    currentSettings.speed = newSpeed;
                    settingsChanged = true;
                }
            }
        }

        // Update animation
        if (server.hasArg("animation")) {
            int newAnimation = server.arg("animation").toInt();
            if (newAnimation >= 0 && newAnimation < ANIMATION_COUNT) {
                if (newAnimation != currentSettings.animationIndex) {
                    currentSettings.animationIndex = newAnimation;
                    settingsChanged = true;
                }
            }
        }

        // Apply changes to LED display
        if (settingsChanged) {
            updateLEDDisplay();
            Logger::info("Settings updated via web interface", "WEB");
        }

        Logger::logWebRequest("POST", "/update", 200);

        // Redirect back to main page
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "");

    } else {
        Logger::warn("Non-POST request to /update endpoint", "WEB");
        server.send(405, "text/plain", "Method Not Allowed");
    }
}

void handleGetSettings() {
    String json = "{";
    json += "\"message\":\"" + currentSettings.message + "\",";
    json += "\"brightness\":" + String(currentSettings.brightness) + ",";
    json += "\"speed\":" + String(currentSettings.speed) + ",";
    json += "\"animation\":" + String(currentSettings.animationIndex) + ",";
    json += "\"animationName\":\"" + String(ANIMATION_EFFECTS[currentSettings.animationIndex].name) + "\"";
    json += "}";

    server.send(200, "application/json", json);
    Logger::logWebRequest("GET", "/api/settings", 200);
}

void handleUptime() {
    server.send(200, "text/plain", String(millis() / 1000));
}

void handleNotFound() {
    String html = WebTemplates::get404Page(server.uri(),
                                          (server.method() == HTTP_GET ? "GET" : "POST"),
                                          server.args());
    server.send(404, "text/html", html);
    Logger::logWebRequest((server.method() == HTTP_GET ? "GET" : "POST"), server.uri(), 404);
}

void setup() {
    // Initialize logger
    Logger::init(115200);
    Logger::setLogLevel(LOG_INFO);
    Logger::logSystemInfo();

    // Initialize display
    Logger::info("Initializing LED display...", "DISPLAY");
    display.begin();
    updateLEDDisplay();  // Apply initial settings
    Logger::info("LED display initialized successfully", "DISPLAY");

    // Connect to WiFi
    Logger::info("Starting WiFi connection...", "NETWORK");
    Logger::info("Connecting to: " + String(ssid), "NETWORK");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < WIFI_CONNECTION_TIMEOUT) {
        delay(WIFI_RETRY_DELAY);
        attempts++;

        String connectMsg = "WiFi... " + String(attempts) + "/" + String(WIFI_CONNECTION_TIMEOUT);
        display.displayClear();
        display.displayText(connectMsg.c_str(), PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

        if (attempts % 5 == 0) {
            Logger::debug("WiFi connection attempt " + String(attempts), "NETWORK");
        }
    }

    if (WiFi.status() == WL_CONNECTED) {
        Logger::info("WiFi connected successfully!", "NETWORK");
        Logger::logNetworkInfo();

        // Setup web server routes
        Logger::info("Starting web server...", "WEB");
        server.on("/", HTTP_GET, handleRoot);
        server.on("/update", HTTP_POST, handleMessageUpdate);
        server.on("/api/uptime", HTTP_GET, handleUptime);
        server.on("/api/settings", HTTP_GET, handleGetSettings);
        server.onNotFound(handleNotFound);
        server.begin();

        Logger::info("Web server started on port 80", "WEB");
        Logger::info("Available routes: /, /update, /api/uptime, /api/settings", "WEB");
        Logger::info("Access URL: http://" + WiFi.localIP().toString(), "WEB");

        // Show web server ready on display briefly, then restore current message
        String serverMsg = "Advanced Web Ready! " + WiFi.localIP().toString();
        display.displayClear();
        display.displayText(serverMsg.c_str(), PA_CENTER, 40, 3000, PA_SCROLL_LEFT, PA_NO_EFFECT);

    } else {
        Logger::error("WiFi connection failed after " + String(attempts) + " attempts", "NETWORK");
        display.displayClear();
        display.displayText("WiFi Failed - No Web Server", PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    }

    Logger::info("Setup complete", "SYSTEM");
}

void loop() {
    // Handle web server requests
    server.handleClient();

    // Keep display animating
    if (display.displayAnimate()) {
        // Animation cycle complete - restart with current settings
        updateLEDDisplay();
    }

    // Monitor system status every 30 seconds
    static unsigned long lastStatusCheck = 0;
    if (millis() - lastStatusCheck > 30000) {
        lastStatusCheck = millis();

        if (WiFi.status() == WL_CONNECTED) {
            Logger::info("System online | Current: '" + currentSettings.message +
                        "' | Brightness: " + String(currentSettings.brightness) +
                        " | Speed: " + String(currentSettings.speed) +
                        " | Animation: " + String(ANIMATION_EFFECTS[currentSettings.animationIndex].name) +
                        " | Uptime: " + String(millis() / 1000) + "s", "STATUS");
        } else {
            Logger::warn("WiFi disconnected - attempting reconnection", "NETWORK");
            WiFi.reconnect();
            display.displayClear();
            display.displayText("Reconnecting WiFi...", PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        }
    }
}
