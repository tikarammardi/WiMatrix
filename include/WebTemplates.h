#ifndef WEB_TEMPLATES_H
#define WEB_TEMPLATES_H

#include <Arduino.h>

// Forward declaration
struct DisplaySettings;

class WebTemplates {
public:
    // Main pages
    static String getDashboardPage(const String& networkSSID, const String& ipAddress, const DisplaySettings& settings);
    static String getControlPage(const DisplaySettings& settings);
    static String getModesPage(const DisplaySettings& settings);
    static String getAdvancedPage(const DisplaySettings& settings);
    static String getStatusPage(const String& networkSSID, const String& ipAddress, const DisplaySettings& settings);

    // Utilities
    static String get404Page(const String& uri, const String& method, int args);
    static String getSharedCSS();
    static String getNavigation(const String& currentPage);
    static String getCurrentModeString(int mode);
};

#endif // WEB_TEMPLATES_H
