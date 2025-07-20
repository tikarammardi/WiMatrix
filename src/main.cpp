#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Config.h"
#include "Logger.h"
#include "WebTemplates.h"
#include "TimeManager.h"
#include "DisplayModeManager.h"

// Create display instance
MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// WiFi credentials from environment variables
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Create web server on port 80
ESP8266WebServer server(80);

// Web server handlers (updated to include mode control)
void handleRoot() {
    DisplaySettings settings = DisplayModeManager::getSettings();
    String html = WebTemplates::getMainPage(WiFi.SSID(), WiFi.localIP().toString(), settings);
    server.send(200, "text/html", html);
    Logger::logWebRequest("GET", "/", 200);
}

void handleMessageUpdate() {
    if (server.method() == HTTP_POST) {
        DisplaySettings settings = DisplayModeManager::getSettings();
        bool settingsChanged = false;

        // Update message
        if (server.hasArg("message")) {
            String newMessage = server.arg("message");
            newMessage.trim();
            if (newMessage.length() == 0) {
                newMessage = "Empty Message";
            }
            if (newMessage != settings.message) {
                settings.message = newMessage;
                settingsChanged = true;
            }
        }

        // Update display mode
        if (server.hasArg("mode")) {
            int newMode = server.arg("mode").toInt();
            if (newMode >= 0 && newMode <= MODE_AUTO_CYCLE) {
                settings.mode = (DisplayMode)newMode;
                settingsChanged = true;
            }
        }

        // Update other settings (brightness, speed, animation)
        if (server.hasArg("brightness")) {
            int newBrightness = server.arg("brightness").toInt();
            if (newBrightness >= MIN_BRIGHTNESS && newBrightness <= MAX_BRIGHTNESS) {
                settings.brightness = newBrightness;
                settingsChanged = true;
            }
        }

        if (server.hasArg("speed")) {
            int newSpeed = server.arg("speed").toInt();
            if (newSpeed >= MIN_SPEED && newSpeed <= MAX_SPEED) {
                settings.speed = newSpeed;
                settingsChanged = true;
            }
        }

        if (server.hasArg("animation")) {
            int newAnimation = server.arg("animation").toInt();
            if (newAnimation >= 0 && newAnimation < ANIMATION_COUNT) {
                settings.animationIndex = newAnimation;
                settingsChanged = true;
            }
        }

        // Apply changes
        if (settingsChanged) {
            DisplayModeManager::setSettings(settings);
            Logger::info("Settings updated via web interface", "WEB");
        }

        Logger::logWebRequest("POST", "/update", 200);

        // Redirect back to main page
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "");

    } else {
        server.send(405, "text/plain", "Method Not Allowed");
    }
}

void handleGetSettings() {
    DisplaySettings settings = DisplayModeManager::getSettings();

    String json = "{";
    json += "\"message\":\"" + settings.message + "\",";
    json += "\"brightness\":" + String(settings.brightness) + ",";
    json += "\"speed\":" + String(settings.speed) + ",";
    json += "\"animation\":" + String(settings.animationIndex) + ",";
    json += "\"mode\":" + String(settings.mode) + ",";
    json += "\"autoCycle\":" + String(settings.autoCycleEnabled ? "true" : "false") + ",";
    json += "\"currentTime\":\"" + TimeManager::getFullDateTime() + "\",";
    json += "\"timeSet\":" + String(TimeManager::isTimeSet() ? "true" : "false");
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

    // Connect to WiFi first (needed for NTP)
    Logger::info("Starting WiFi connection...", "NETWORK");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < WIFI_CONNECTION_TIMEOUT) {
        delay(WIFI_RETRY_DELAY);
        attempts++;

        if (attempts % 5 == 0) {
            Logger::debug("WiFi connection attempt " + String(attempts), "NETWORK");
        }
    }

    if (WiFi.status() == WL_CONNECTED) {
        Logger::info("WiFi connected successfully!", "NETWORK");
        Logger::logNetworkInfo();

        // Initialize time management (requires WiFi)
        TimeManager::init();

        // Initialize display mode manager
        DisplayModeManager::init(&display);

        // Setup web server routes
        Logger::info("Starting web server...", "WEB");
        server.on("/", HTTP_GET, handleRoot);
        server.on("/update", HTTP_POST, handleMessageUpdate);
        server.on("/api/uptime", HTTP_GET, handleUptime);
        server.on("/api/settings", HTTP_GET, handleGetSettings);
        server.onNotFound(handleNotFound);
        server.begin();

        Logger::info("Web server started with time display features", "WEB");
        Logger::info("Access URL: http://" + WiFi.localIP().toString(), "WEB");

    } else {
        Logger::error("WiFi connection failed - time features disabled", "NETWORK");
        // Still initialize display for basic operation
        DisplayModeManager::init(&display);
    }

    Logger::info("Setup complete with integrated time display", "SYSTEM");
}

void loop() {
    // Handle web server requests
    server.handleClient();

    // Update time manager (NTP sync)
    TimeManager::update();

    // Update display mode manager (handles all mode switching and display)
    DisplayModeManager::update();

    // System status monitoring
    static unsigned long lastStatusCheck = 0;
    if (millis() - lastStatusCheck > 30000) {
        lastStatusCheck = millis();

        DisplaySettings settings = DisplayModeManager::getSettings();
        if (WiFi.status() == WL_CONNECTED) {
            Logger::info("System online | Mode: " + DisplayModeManager::getCurrentModeString() +
                        " | Time: " + TimeManager::getFullDateTime() +
                        " | Uptime: " + String(millis() / 1000) + "s", "STATUS");
        } else {
            Logger::warn("WiFi disconnected - attempting reconnection", "NETWORK");
            WiFi.reconnect();
        }
    }
}
