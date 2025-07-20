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

// Current message state
String currentMessage = "Welcome to WiMatrix!";

// Web server handlers
void handleRoot() {
    String html = WebTemplates::getMainPage(WiFi.SSID(), WiFi.localIP().toString(), currentMessage);
    server.send(200, "text/html", html);
    Logger::logWebRequest("GET", "/", 200);
}

void handleMessageUpdate() {
    if (server.method() == HTTP_POST) {
        if (server.hasArg("message")) {
            String newMessage = server.arg("message");

            // Trim whitespace and validate
            newMessage.trim();
            if (newMessage.length() == 0) {
                newMessage = "Empty Message";
            }

            // Update current message
            String oldMessage = currentMessage;
            currentMessage = newMessage;

            // Update LED display
            display.displayClear();
            display.displayText(currentMessage.c_str(), PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

            Logger::info("Message updated: '" + oldMessage + "' -> '" + currentMessage + "'", "MESSAGE");
            Logger::logWebRequest("POST", "/update", 200);

            // Redirect back to main page to show updated message
            server.sendHeader("Location", "/");
            server.send(302, "text/plain", "");
        } else {
            Logger::warn("POST /update received without 'message' parameter", "WEB");
            server.send(400, "text/plain", "Bad Request: Missing 'message' parameter");
        }
    } else {
        Logger::warn("Non-POST request to /update endpoint", "WEB");
        server.send(405, "text/plain", "Method Not Allowed");
    }
}

void handleGetMessage() {
    server.send(200, "application/json", "{\"message\":\"" + currentMessage + "\"}");
    Logger::logWebRequest("GET", "/api/message", 200);
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
    display.setIntensity(DEFAULT_BRIGHTNESS);
    display.displayClear();
    display.displayText(currentMessage.c_str(), PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    Logger::info("LED display initialized with message: '" + currentMessage + "'", "DISPLAY");

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
        server.on("/api/message", HTTP_GET, handleGetMessage);
        server.onNotFound(handleNotFound);
        server.begin();
        Logger::info("Web server started on port 80", "WEB");
        Logger::info("Available routes: /, /update, /api/uptime, /api/message", "WEB");
        Logger::info("Access URL: http://" + WiFi.localIP().toString(), "WEB");

        // Show web server ready on display
        String serverMsg = "Web Ready! " + WiFi.localIP().toString();
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
        // After temporary messages (like IP display), restore current message
        display.displayText(currentMessage.c_str(), PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    }

    // Monitor system status every 30 seconds
    static unsigned long lastStatusCheck = 0;
    if (millis() - lastStatusCheck > 30000) {
        lastStatusCheck = millis();

        if (WiFi.status() == WL_CONNECTED) {
            Logger::info("System online | Message: '" + currentMessage + "' | Uptime: " + String(millis() / 1000) + "s", "STATUS");
        } else {
            Logger::warn("WiFi disconnected - attempting reconnection", "NETWORK");
            WiFi.reconnect();
            display.displayClear();
            display.displayText("Reconnecting WiFi...", PA_CENTER, DEFAULT_SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        }
    }
}
