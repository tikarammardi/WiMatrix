#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "Config.h"

class TimeManager {
private:
    static WiFiUDP ntpUDP;
    static NTPClient timeClient;
    static unsigned long lastNTPUpdate;

public:
    static void init();
    static void update();
    static bool isTimeSet();

    // Time formatting functions
    static String getTimeString();
    static String getDateString(); // Always returns "20 JAN" format
    static String getDayString();
    static String getFullDateTime();

    // Raw time access
    static int getCurrentHour();
    static int getCurrentMinute();
    static int getCurrentSecond();
    static int getCurrentDay();
    static int getCurrentMonth();
    static int getCurrentYear();
    static int getCurrentWeekday();
};

#endif // TIME_MANAGER_H
