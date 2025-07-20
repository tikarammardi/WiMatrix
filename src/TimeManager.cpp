#include "TimeManager.h"
#include "Logger.h"

WiFiUDP TimeManager::ntpUDP;
NTPClient TimeManager::timeClient(ntpUDP, "pool.ntp.org", IST_OFFSET, NTP_UPDATE_INTERVAL);
unsigned long TimeManager::lastNTPUpdate = 0;

void TimeManager::init() {
    Logger::info("Initializing NTP time client...", "TIME");
    timeClient.begin();

    // Force initial time update
    if (timeClient.update()) {
        setTime(timeClient.getEpochTime());
        Logger::info("NTP time synchronized successfully", "TIME");
        Logger::info("Current time: " + getFullDateTime(), "TIME");
    } else {
        Logger::warn("Failed to get initial NTP time", "TIME");
    }

    lastNTPUpdate = millis();
}

void TimeManager::update() {
    // Update NTP time periodically
    if (millis() - lastNTPUpdate >= NTP_UPDATE_INTERVAL) {
        if (timeClient.update()) {
            setTime(timeClient.getEpochTime());
            Logger::debug("NTP time updated: " + getFullDateTime(), "TIME");
        } else {
            Logger::warn("NTP time update failed", "TIME");
        }
        lastNTPUpdate = millis();
    }
}

bool TimeManager::isTimeSet() {
    return timeStatus() == timeSet;
}

String TimeManager::getTimeString() {
    if (!isTimeSet()) return "??:??";

    // Always use 24-hour format for 8x32 display (5 chars: "23:45")
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", hour(), minute());
    return String(timeStr);
}

// FIXED: Always returns "20 JAN" format
String TimeManager::getDateString() {
    if (!isTimeSet()) return "???";

    // Use "20 JAN" format for 8x32 display (6 chars)
    char dateStr[7];
    snprintf(dateStr, sizeof(dateStr), "%02d %s", day(), MONTHS_SHORT[month() - 1]);
    return String(dateStr);
}

String TimeManager::getDayString() {
    if (!isTimeSet()) return "???";
    // Use 3-character day names for 8x32
    return String(DAYS_SHORT[weekday() - 1]);
}

String TimeManager::getFullDateTime() {
    if (!isTimeSet()) return "Time not set";

    // Compact format for logging
    char fullDateTime[20];
    snprintf(fullDateTime, sizeof(fullDateTime), "%s %02d/%02d %02d:%02d",
             DAYS_SHORT[weekday() - 1], day(), month(), hour(), minute());
    return String(fullDateTime);
}

// Raw time access methods
int TimeManager::getCurrentHour() { return isTimeSet() ? hour() : -1; }
int TimeManager::getCurrentMinute() { return isTimeSet() ? minute() : -1; }
int TimeManager::getCurrentSecond() { return isTimeSet() ? second() : -1; }
int TimeManager::getCurrentDay() { return isTimeSet() ? day() : -1; }
int TimeManager::getCurrentMonth() { return isTimeSet() ? month() : -1; }
int TimeManager::getCurrentYear() { return isTimeSet() ? year() : -1; }
int TimeManager::getCurrentWeekday() { return isTimeSet() ? weekday() : -1; }
