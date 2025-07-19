//
// Created by Tikaram Mardi on 19/07/25.
//

#ifndef WEB_TEMPLATES_H
#define WEB_TEMPLATES_H

#include <Arduino.h>

class WebTemplates {
public:
    static String getMainPage(const String& networkSSID, const String& ipAddress);
    static String get404Page(const String& uri, const String& method, int args);
    static String getSystemInfoSection();
    static String getCommonCSS();
};

#endif // WEB_TEMPLATES_H
