//
// Created by Tikaram Mardi on 20/07/25.
//
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
    if (!isTimeSet()) return "No Time";

    char timeStr[10];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", hour(), minute());
    return String(timeStr);
}

String TimeManager::getDateString() {
    if (!isTimeSet()) return "No Date";

    char dateStr[12];
    snprintf(dateStr, sizeof(dateStr), "%02d %s", day(), MONTHS_OF_YEAR[month() - 1]);
    return String(dateStr);
}

String TimeManager::getDayString() {
    if (!isTimeSet()) return "No Day";
    return String(DAYS_OF_WEEK[weekday() - 1]);
}

String TimeManager::getFullDateTime() {
    if (!isTimeSet()) return "Time not set";

    char fullDateTime[30];
    snprintf(fullDateTime, sizeof(fullDateTime), "%s %02d %s %04d %02d:%02d:%02d",
             DAYS_OF_WEEK[weekday() - 1], day(), MONTHS_OF_YEAR[month() - 1],
             year(), hour(), minute(), second());
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
