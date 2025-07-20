#ifndef WEB_TEMPLATES_H
#define WEB_TEMPLATES_H

#include <Arduino.h>
#include "DisplayModeManager.h"



class WebTemplates {
public:
    static String getMainPage(const String& networkSSID, const String& ipAddress, const DisplaySettings& settings);
    static String get404Page(const String& uri, const String& method, int args);
    static String getSystemInfoSection();
    static String getCommonCSS();
    static String getAdvancedControlsSection(const DisplaySettings& settings);
    static String getAnimationOptionsHTML(int selectedIndex);
};

#endif // WEB_TEMPLATES_H
