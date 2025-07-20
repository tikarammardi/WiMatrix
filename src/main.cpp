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

// Night mode settings
struct NightModeSettings {
    bool enabled = false;
    int startHour = 22;  // 10 PM
    int endHour = 6;     // 6 AM
    int dimBrightness = 1;
};

NightModeSettings nightMode;

// Helper function to check if current time is in night mode
bool isNightTime() {
    if (!nightMode.enabled || !TimeManager::isTimeSet()) return false;

    int currentHour = TimeManager::getCurrentHour();
    if (nightMode.startHour < nightMode.endHour) {
        return (currentHour >= nightMode.startHour && currentHour < nightMode.endHour);
    } else {
        return (currentHour >= nightMode.startHour || currentHour < nightMode.endHour);
    }
}

// Apply night mode brightness if needed
void applyNightModeIfNeeded() {
    DisplaySettings settings = DisplayModeManager::getSettings();
    if (isNightTime()) {
        if (settings.brightness > nightMode.dimBrightness) {
            settings.brightness = nightMode.dimBrightness;
            DisplayModeManager::setSettings(settings);
            Logger::info("Night mode applied - brightness dimmed to " + String(nightMode.dimBrightness), "NIGHT");
        }
    }
}

// Web page handlers
void handleDashboard() {
    DisplaySettings settings = DisplayModeManager::getSettings();
    String html = WebTemplates::getDashboardPage(WiFi.SSID(), WiFi.localIP().toString(), settings);
    server.send(200, "text/html", html);
    Logger::logWebRequest("GET", "/", 200);
}

void handleControlPage() {
    DisplaySettings settings = DisplayModeManager::getSettings();
    String html = WebTemplates::getControlPage(settings);
    server.send(200, "text/html", html);
    Logger::logWebRequest("GET", "/control", 200);
}

void handleModesPage() {
    DisplaySettings settings = DisplayModeManager::getSettings();
    String html = WebTemplates::getModesPage(settings);
    server.send(200, "text/html", html);
    Logger::logWebRequest("GET", "/modes", 200);
}

void handleAdvancedPage() {
    DisplaySettings settings = DisplayModeManager::getSettings();
    String html = WebTemplates::getAdvancedPage(settings);
    server.send(200, "text/html", html);
    Logger::logWebRequest("GET", "/advanced", 200);
}

void handleStatusPage() {
    DisplaySettings settings = DisplayModeManager::getSettings();
    String html = WebTemplates::getStatusPage(WiFi.SSID(), WiFi.localIP().toString(), settings);
    server.send(200, "text/html", html);
    Logger::logWebRequest("GET", "/status", 200);
}

// Form processing handler - REMOVED DATE FORMAT HANDLING
void handleMessageUpdate() {
    if (server.method() == HTTP_POST) {
        DisplaySettings settings = DisplayModeManager::getSettings();
        bool settingsChanged = false;
        String redirectPage = "/"; // Default redirect

        // Check which page sent the form
        if (server.hasArg("page")) {
            String page = server.arg("page");
            if (page == "control") redirectPage = "/control";
            else if (page == "modes") redirectPage = "/modes";
            else if (page == "advanced") redirectPage = "/advanced";
        }

        // Handle radio button display mode selection - ALL MODES
        if (server.hasArg("displayMode")) {
            String modeStr = server.arg("displayMode");
            DisplayMode newMode;

            if (modeStr == "message") {
                newMode = MODE_MANUAL;
            } else if (modeStr == "clock") {
                newMode = MODE_CLOCK;
            } else if (modeStr == "day") {
                newMode = MODE_DAY;
            } else if (modeStr == "date") {
                newMode = MODE_DATE;
            } else if (modeStr == "auto") {
                newMode = MODE_AUTO_CYCLE;
            } else {
                newMode = MODE_AUTO_CYCLE; // Default fallback
            }

            if (newMode != settings.mode) {
                settings.mode = newMode;
                settingsChanged = true;
                Logger::info("Display mode changed via radio button to: " + modeStr, "WEB");
            }
        }

        // Handle individual mode toggles (checkboxes)
        if (server.hasArg("enableClock") || server.hasArg("enableDay") ||
            server.hasArg("enableDate") || server.hasArg("enableMessage")) {

            bool clockWasEnabled = settings.modeSettings.clockEnabled;
            bool dayWasEnabled = settings.modeSettings.dayEnabled;
            bool dateWasEnabled = settings.modeSettings.dateEnabled;
            bool messageWasEnabled = settings.modeSettings.messageEnabled;

            // Reset all to false first (unchecked checkboxes don't send values)
            settings.modeSettings.clockEnabled = false;
            settings.modeSettings.dayEnabled = false;
            settings.modeSettings.dateEnabled = false;
            settings.modeSettings.messageEnabled = false;

            // Set to true only if present in POST data
            if (server.hasArg("enableClock")) {
                settings.modeSettings.clockEnabled = true;
            }
            if (server.hasArg("enableDay")) {
                settings.modeSettings.dayEnabled = true;
            }
            if (server.hasArg("enableDate")) {
                settings.modeSettings.dateEnabled = true;
            }
            if (server.hasArg("enableMessage")) {
                settings.modeSettings.messageEnabled = true;
            }

            // Check if any mode settings changed
            if (clockWasEnabled != settings.modeSettings.clockEnabled ||
                dayWasEnabled != settings.modeSettings.dayEnabled ||
                dateWasEnabled != settings.modeSettings.dateEnabled ||
                messageWasEnabled != settings.modeSettings.messageEnabled) {
                settingsChanged = true;
                Logger::info("Individual mode toggles updated - Clock:" + String(settings.modeSettings.clockEnabled ? "ON" : "OFF") +
                            " Day:" + String(settings.modeSettings.dayEnabled ? "ON" : "OFF") +
                            " Date:" + String(settings.modeSettings.dateEnabled ? "ON" : "OFF") +
                            " Message:" + String(settings.modeSettings.messageEnabled ? "ON" : "OFF"), "WEB");
            }
        }

        // Handle cycle interval
        if (server.hasArg("cycleInterval")) {
            int newInterval = server.arg("cycleInterval").toInt();
            if (newInterval >= 2 && newInterval <= 10) {
                if (newInterval != settings.modeSettings.cycleInterval) {
                    settings.modeSettings.cycleInterval = newInterval;
                    settingsChanged = true;
                    Logger::info("Cycle interval set to: " + String(newInterval) + "s", "WEB");
                }
            }
        }

        // Handle message update
        if (server.hasArg("message")) {
            String newMessage = server.arg("message");
            newMessage.trim();
            if (newMessage.length() == 0) {
                newMessage = "Empty Message";
            }
            if (newMessage != settings.message) {
                settings.message = newMessage;
                settingsChanged = true;
                Logger::info("Message updated to: '" + newMessage + "'", "WEB");
            }
        }

        // Handle brightness update
        if (server.hasArg("brightness")) {
            int newBrightness = server.arg("brightness").toInt();
            if (newBrightness >= 0 && newBrightness <= 8) {
                if (newBrightness != settings.brightness) {
                    settings.brightness = newBrightness;
                    settingsChanged = true;
                    Logger::info("Brightness set to: " + String(newBrightness), "WEB");
                }
            }
        }

        // Handle speed update
        if (server.hasArg("speed")) {
            int newSpeed = server.arg("speed").toInt();
            if (newSpeed >= 40 && newSpeed <= 150) {
                if (newSpeed != settings.speed) {
                    settings.speed = newSpeed;
                    settingsChanged = true;
                    Logger::info("Speed set to: " + String(newSpeed) + "ms", "WEB");
                }
            }
        }

        // Handle night mode update
        if (server.hasArg("nightMode")) {
            bool newNightMode = (server.arg("nightMode") == "true");
            if (newNightMode != nightMode.enabled) {
                nightMode.enabled = newNightMode;
                Logger::info("Night mode " + String(newNightMode ? "enabled" : "disabled"), "WEB");
            }
        }

        // Apply changes to display
        if (settingsChanged) {
            DisplayModeManager::setSettings(settings);
            Logger::info("Display settings updated from page: " + redirectPage, "WEB");
        }

        Logger::logWebRequest("POST", "/update", 200);

        // Redirect back to the originating page
        server.sendHeader("Location", redirectPage);
        server.send(302, "text/plain", "");

    } else {
        server.send(405, "text/plain", "Method Not Allowed");
        Logger::logWebRequest("INVALID", "/update", 405);
    }
}

// API handlers
void handleNextMode() {
    DisplayModeManager::nextMode();
    server.send(200, "text/plain", "Mode switched to next");
    Logger::logWebRequest("POST", "/api/next-mode", 200);
}

void handlePauseMode() {
    DisplaySettings settings = DisplayModeManager::getSettings();
    settings.autoCycleEnabled = !settings.autoCycleEnabled;
    DisplayModeManager::setSettings(settings);

    String response = settings.autoCycleEnabled ? "Auto cycle resumed" : "Auto cycle paused";
    server.send(200, "text/plain", response);
    Logger::info("Mode cycling " + String(settings.autoCycleEnabled ? "resumed" : "paused") + " via API", "WEB");
    Logger::logWebRequest("POST", "/api/pause-mode", 200);
}

void handleReset() {
    DisplaySettings settings = DisplayModeManager::getSettings();
    settings.message = "Hi!";
    settings.brightness = 3;
    settings.speed = 80;
    settings.mode = MODE_AUTO_CYCLE;
    settings.autoCycleEnabled = true;
    settings.modeSettings.clockEnabled = true;
    settings.modeSettings.dayEnabled = true;
    settings.modeSettings.dateEnabled = true;
    settings.modeSettings.messageEnabled = true;
    settings.modeSettings.cycleInterval = 3;

    DisplayModeManager::setSettings(settings);
    server.send(200, "text/plain", "Settings reset to defaults");
    Logger::info("Settings reset to defaults via API", "WEB");
    Logger::logWebRequest("POST", "/api/reset", 200);
}

void handleGetSettings() {
    DisplaySettings settings = DisplayModeManager::getSettings();

    String json = "{";
    json += "\"message\":\"" + settings.message + "\",";
    json += "\"brightness\":" + String(settings.brightness) + ",";
    json += "\"speed\":" + String(settings.speed) + ",";
    json += "\"mode\":" + String(settings.mode) + ",";
    json += "\"autoCycle\":" + String(settings.autoCycleEnabled ? "true" : "false") + ",";

    // Add mode settings for individual toggles
    json += "\"clockEnabled\":" + String(settings.modeSettings.clockEnabled ? "true" : "false") + ",";
    json += "\"dayEnabled\":" + String(settings.modeSettings.dayEnabled ? "true" : "false") + ",";
    json += "\"dateEnabled\":" + String(settings.modeSettings.dateEnabled ? "true" : "false") + ",";
    json += "\"messageEnabled\":" + String(settings.modeSettings.messageEnabled ? "true" : "false") + ",";
    json += "\"cycleInterval\":" + String(settings.modeSettings.cycleInterval) + ",";

    // Add time and system info
    json += "\"currentTime\":\"" + TimeManager::getFullDateTime() + "\",";
    json += "\"timeSet\":" + String(TimeManager::isTimeSet() ? "true" : "false") + ",";
    json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"macAddress\":\"" + WiFi.macAddress() + "\",";
    json += "\"nightMode\":" + String(nightMode.enabled ? "true" : "false");
    json += "}";

    server.send(200, "application/json", json);
    Logger::logWebRequest("GET", "/api/settings", 200);
}

void handleCurrentTime() {
    server.send(200, "text/plain", TimeManager::getFullDateTime());
    Logger::logWebRequest("GET", "/api/current-time", 200);
}

void handleUptime() {
    server.send(200, "text/plain", String(millis() / 1000));
    Logger::logWebRequest("GET", "/api/uptime", 200);
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
        Logger::info("Starting multi-page web interface with fixed date format...", "WEB");

        // Page routes
        server.on("/", HTTP_GET, handleDashboard);
        server.on("/control", HTTP_GET, handleControlPage);
        server.on("/modes", HTTP_GET, handleModesPage);
        server.on("/advanced", HTTP_GET, handleAdvancedPage);
        server.on("/status", HTTP_GET, handleStatusPage);

        // Form processing
        server.on("/update", HTTP_POST, handleMessageUpdate);

        // API endpoints
        server.on("/api/next-mode", HTTP_POST, handleNextMode);
        server.on("/api/pause-mode", HTTP_POST, handlePauseMode);
        server.on("/api/reset", HTTP_POST, handleReset);
        server.on("/api/settings", HTTP_GET, handleGetSettings);
        server.on("/api/current-time", HTTP_GET, handleCurrentTime);
        server.on("/api/uptime", HTTP_GET, handleUptime);

        // 404 handler
        server.onNotFound(handleNotFound);

        server.begin();

        Logger::info("Multi-page web interface started with fixed date format", "WEB");
        Logger::info("Available pages: Dashboard (/), Control (/control), Modes (/modes), Advanced (/advanced), Status (/status)", "WEB");
        Logger::info("Date format: Fixed to '20 JAN' format for optimal 8x32 display", "WEB");
        Logger::info("Access URL: http://" + WiFi.localIP().toString(), "WEB");

    } else {
        Logger::error("WiFi connection failed - running in offline mode", "NETWORK");
        // Still initialize display for basic operation
        DisplayModeManager::init(&display);
    }

    Logger::info("Setup complete with simplified date formatting", "SYSTEM");
}

void loop() {
    // Handle web server requests
    server.handleClient();

    // Update time manager (NTP sync)
    TimeManager::update();

    // Apply night mode if enabled
    static unsigned long lastNightModeCheck = 0;
    if (millis() - lastNightModeCheck > 60000) { // Check every minute
        lastNightModeCheck = millis();
        applyNightModeIfNeeded();
    }

    // Update display mode manager
    static unsigned long lastModeUpdate = 0;
    if (millis() - lastModeUpdate > 100) { // Update every 100ms for smooth operation
        lastModeUpdate = millis();
        DisplayModeManager::update();
    }

    // System status monitoring
    static unsigned long lastStatusCheck = 0;
    if (millis() - lastStatusCheck > 30000) {
        lastStatusCheck = millis();

        DisplaySettings settings = DisplayModeManager::getSettings();
        if (WiFi.status() == WL_CONNECTED) {
            Logger::info("System Status: " + DisplayModeManager::getCurrentModeString() +
                        " | Time: " + TimeManager::getTimeString() +
                        " | Date: " + TimeManager::getDateString() + // Always "20 JAN" format
                        " | Memory: " + String(ESP.getFreeHeap()) +
                        " | Signal: " + String(WiFi.RSSI()) + "dBm" +
                        " | Night Mode: " + String(nightMode.enabled ? "On" : "Off") +
                        " | Cycle: " + String(settings.modeSettings.cycleInterval) + "s" +
                        " | Modes: C" + String(settings.modeSettings.clockEnabled ? "+" : "-") +
                        "D" + String(settings.modeSettings.dayEnabled ? "+" : "-") +
                        "T" + String(settings.modeSettings.dateEnabled ? "+" : "-") +
                        "M" + String(settings.modeSettings.messageEnabled ? "+" : "-") +
                        " | Uptime: " + String(millis() / 1000) + "s", "STATUS");
        } else {
            Logger::warn("WiFi disconnected - attempting reconnection", "NETWORK");
            WiFi.reconnect();
        }
    }
}
