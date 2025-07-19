//
// Created by Tikaram Mardi on 19/07/25.
//
#include "Logger.h"
#include <ESP8266WiFi.h>

LogLevel Logger::currentLogLevel = LOG_INFO;

void Logger::init(unsigned long baudRate) {
    Serial.begin(baudRate);
    delay(1000);
    info("Logger initialized", "SYSTEM");
}

void Logger::setLogLevel(LogLevel level) {
    currentLogLevel = level;
    info("Log level set to: " + getLevelString(level), "SYSTEM");
}

String Logger::getTimestamp() {
    return "[" + String(millis() / 1000) + "s]";
}

String Logger::getLevelString(LogLevel level) {
    switch(level) {
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO:  return "INFO ";
        case LOG_WARN:  return "WARN ";
        case LOG_ERROR: return "ERROR";
        default:        return "UNKNW";
    }
}

void Logger::debug(const String& message, const String& component) {
    if (currentLogLevel <= LOG_DEBUG) {
        String output = getTimestamp() + " [" + getLevelString(LOG_DEBUG) + "]";
        if (component.length() > 0) output += " [" + component + "]";
        output += " " + message;
        Serial.println(output);
    }
}

void Logger::info(const String& message, const String& component) {
    if (currentLogLevel <= LOG_INFO) {
        String output = getTimestamp() + " [" + getLevelString(LOG_INFO) + "]";
        if (component.length() > 0) output += " [" + component + "]";
        output += " " + message;
        Serial.println(output);
    }
}

void Logger::warn(const String& message, const String& component) {
    if (currentLogLevel <= LOG_WARN) {
        String output = getTimestamp() + " [" + getLevelString(LOG_WARN) + "]";
        if (component.length() > 0) output += " [" + component + "]";
        output += " " + message;
        Serial.println(output);
    }
}

void Logger::error(const String& message, const String& component) {
    if (currentLogLevel <= LOG_ERROR) {
        String output = getTimestamp() + " [" + getLevelString(LOG_ERROR) + "]";
        if (component.length() > 0) output += " [" + component + "]";
        output += " " + message;
        Serial.println(output);
    }
}

void Logger::logSystemInfo() {
    info("=== WiMatrix LED Controller Starting ===", "SYSTEM");
    info("Chip ID: " + String(ESP.getChipId()), "HARDWARE");
    info("Flash Size: " + String(ESP.getFlashChipSize()), "HARDWARE");
    info("Free Heap: " + String(ESP.getFreeHeap()), "MEMORY");
}

void Logger::logNetworkInfo() {
    info("Network SSID: " + WiFi.SSID(), "NETWORK");
    info("IP Address: " + WiFi.localIP().toString(), "NETWORK");
    info("Signal Strength: " + String(WiFi.RSSI()) + " dBm", "NETWORK");
    info("Gateway: " + WiFi.gatewayIP().toString(), "NETWORK");
    info("MAC Address: " + WiFi.macAddress(), "NETWORK");
}

void Logger::logWebRequest(const String& method, const String& uri, int responseCode) {
    info(method + " " + uri + " -> " + String(responseCode), "WEB");
}
