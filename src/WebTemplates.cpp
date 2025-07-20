#include "WebTemplates.h"
#include "Config.h"
#include "DisplayModeManager.h"
#include "TimeManager.h"

// Compressed shared CSS stored in PROGMEM - FIXED OVERFLOW ISSUES
const char SHARED_CSS[] PROGMEM = R"(
body{font-family:Arial;margin:0;padding:0;background:#667eea;min-height:100vh;overflow-x:hidden}
.container{max-width:480px;margin:0 auto;background:white;min-height:100vh;overflow:hidden}
.header{background:#4CAF50;color:white;padding:12px;text-align:center}
.header h1{margin:0;font-size:1.3em;word-wrap:break-word}
.nav{background:#f5f5f5;display:flex;overflow-x:auto;flex-wrap:wrap}
.nav a{flex:1;padding:8px 4px;text-decoration:none;color:#333;border-bottom:2px solid transparent;white-space:nowrap;text-align:center;min-width:70px;font-size:12px}
.nav a:hover{background:#e8e8e8}
.nav a.active{border-bottom-color:#4CAF50;background:white;color:#4CAF50}
.content{padding:12px;overflow-y:auto;max-height:calc(100vh - 160px)}
.form-group{margin:12px 0}
.form-group label{display:block;margin-bottom:5px;font-weight:bold;font-size:14px}
.form-group input,.form-group select{width:100%;padding:8px;border:1px solid #ddd;border-radius:4px;box-sizing:border-box}
.btn{background:#4CAF50;color:white;padding:10px 15px;border:none;border-radius:4px;cursor:pointer;margin:5px 0;width:100%;font-size:14px}
.btn:hover{background:#45a049}
.btn-orange{background:#FF9800}
.btn-orange:hover{background:#F57C00}
.info{background:#e3f2fd;padding:8px;border-radius:4px;margin:8px 0;font-size:12px;word-wrap:break-word}
.preview{background:#000;color:#0f0;padding:8px;font-family:monospace;text-align:center;border-radius:4px;margin:8px 0;font-size:14px;overflow:hidden}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin:8px 0}
.radio-group{margin:8px 0}
.radio-item{display:flex;align-items:center;background:#f8f9fa;padding:6px;border-radius:4px;margin:4px 0;border:1px solid #ddd;font-size:13px}
.radio-item input{margin-right:6px}
.status{background:#f8f9fa;padding:8px;border-radius:4px;margin:8px 0;border-left:3px solid #4CAF50;font-size:13px;word-wrap:break-word}
@media(max-width:500px){.grid{grid-template-columns:1fr}.nav{flex-wrap:wrap}.nav a{min-width:60px;font-size:11px}.container{max-width:100%}}
)";

String WebTemplates::getSharedCSS() {
    return String(FPSTR(SHARED_CSS));
}

String WebTemplates::getNavigation(const String& currentPage) {
    String nav = F("<div class=\"nav\">");
    nav += F("<a href=\"/\" class=\"");
    nav += (currentPage == "dashboard") ? F("active") : F("");
    nav += F("\">🏠 Home</a>");
    nav += F("<a href=\"/control\" class=\"");
    nav += (currentPage == "control") ? F("active") : F("");
    nav += F("\">💬 Control</a>");
    nav += F("<a href=\"/modes\" class=\"");
    nav += (currentPage == "modes") ? F("active") : F("");
    nav += F("\">🎯 Modes</a>");
    nav += F("<a href=\"/advanced\" class=\"");
    nav += (currentPage == "advanced") ? F("active") : F("");
    nav += F("\">⚙️ Advanced</a>");
    nav += F("<a href=\"/status\" class=\"");
    nav += (currentPage == "status") ? F("active") : F("");
    nav += F("\">📊 Status</a>");
    nav += F("</div>");
    return nav;
}

String WebTemplates::getCurrentModeString(int mode) {
    switch (mode) {
        case MODE_MANUAL: return F("Message");
        case MODE_CLOCK: return F("Clock");
        case MODE_DAY: return F("Day");
        case MODE_DATE: return F("Date");
        case MODE_SCROLL: return F("Scroll");
        case MODE_AUTO_CYCLE: return F("Auto Cycle");
        default: return F("Unknown");
    }
}

// FIXED DASHBOARD - NO MORE OVERFLOW
String WebTemplates::getDashboardPage(const String& networkSSID, const String& ipAddress, const DisplaySettings& settings) {
    String html = F("<!DOCTYPE html><html><head>");
    html += F("<meta charset=\"UTF-8\">");
    html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
    html += F("<title>WiMatrix Dashboard</title>");
    html += F("<style>");
    html += getSharedCSS();
    html += F("</style>");
    html += F("</head><body>");
    html += F("<div class=\"container\">");
    html += F("<div class=\"header\">");
    html += F("<h1>🚀 WiMatrix Controller</h1>");
    html += F("<div style=\"font-size:12px\">📡 ");
    html += networkSSID;
    html += F(" | 🔗 ");
    html += ipAddress;
    html += F("</div>");
    html += F("</div>");
    html += getNavigation("dashboard");
    html += F("<div class=\"content\">");
    html += F("<div class=\"status\">");
    html += F("<strong>Current Status:</strong><br>");
    html += F("Mode: ");
    html += getCurrentModeString(settings.mode);
    html += F("<br>Message: \"");
    html += settings.message;
    html += F("\"<br>Time: <span id=\"time\">");
    html += TimeManager::getTimeString();
    html += F("</span><br>Uptime: <span id=\"uptime\">0</span>s");
    html += F("</div>");
    html += F("<div class=\"info\">");
    html += F("<strong>📏 Quick Guide:</strong><br>");
    html += F("• Control: Message & display settings<br>");
    html += F("• Modes: Auto-cycle behavior<br>");
    html += F("• Advanced: Quick actions<br>");
    html += F("• Status: System monitoring");
    html += F("</div>");
    html += F("<div class=\"grid\">");
    html += F("<a href=\"/control\" class=\"btn\">💬 Control</a>");
    html += F("<a href=\"/modes\" class=\"btn\">🎯 Modes</a>");
    html += F("<a href=\"/advanced\" class=\"btn btn-orange\">⚙️ Advanced</a>");
    html += F("<a href=\"/status\" class=\"btn btn-orange\">📊 Status</a>");
    html += F("</div></div></div>");
    html += F("<script>");
    html += F("function updateStatus(){");
    html += F("fetch('/api/uptime').then(r=>r.text()).then(d=>document.getElementById('uptime').textContent=d);");
    html += F("fetch('/api/current-time').then(r=>r.text()).then(d=>document.getElementById('time').textContent=d)");
    html += F("}");
    html += F("setInterval(updateStatus,2000);updateStatus()");
    html += F("</script>");
    html += F("</body></html>");
    return html;
}

String WebTemplates::getControlPage(const DisplaySettings& settings) {
    String html = F("<!DOCTYPE html><html><head>");
    html += F("<meta charset=\"UTF-8\">");
    html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
    html += F("<title>Message Control</title>");
    html += F("<style>");
    html += getSharedCSS();
    html += F("</style>");
    html += F("</head><body>");
    html += F("<div class=\"container\">");
    html += F("<div class=\"header\"><h1>💬 Message Control</h1></div>");
    html += getNavigation("control");
    html += F("<div class=\"content\">");
    html += F("<div class=\"info\">");
    html += F("<strong>📏 8x32 Display Guide:</strong> 1-5 chars = static, 6+ chars = scroll");
    html += F("</div>");
    html += F("<form method=\"POST\" action=\"/update\">");
    html += F("<input type=\"hidden\" name=\"page\" value=\"control\">");
    html += F("<div class=\"form-group\">");
    html += F("<label>Custom Message:</label>");
    html += F("<input type=\"text\" name=\"message\" value=\"");
    html += settings.message;
    html += F("\" maxlength=\"50\" placeholder=\"Enter your message...\">");
    html += F("<div class=\"preview\">");
    html += settings.message;
    html += F("</div>");
    html += F("</div>");
    html += F("<div class=\"grid\">");
    html += F("<div class=\"form-group\">");
    html += F("<label>💡 Brightness: <span id=\"bval\">");
    html += String(settings.brightness);
    html += F("</span>/8</label>");
    html += F("<input type=\"range\" name=\"brightness\" min=\"0\" max=\"8\" value=\"");
    html += String(settings.brightness);
    html += F("\" oninput=\"document.getElementById('bval').textContent=this.value\">");
    html += F("</div>");
    html += F("<div class=\"form-group\">");
    html += F("<label>⚡ Speed: <span id=\"sval\">");
    html += String(settings.speed);
    html += F("</span>ms</label>");
    html += F("<input type=\"range\" name=\"speed\" min=\"40\" max=\"150\" value=\"");
    html += String(settings.speed);
    html += F("\" oninput=\"document.getElementById('sval').textContent=this.value\">");
    html += F("</div>");
    html += F("</div>");
    html += F("<button type=\"submit\" class=\"btn\">🚀 Apply Settings</button>");
    html += F("</form>");
    html += F("</div></div></body></html>");
    return html;
}

// FIXED MODES PAGE - REMOVED DATE FORMAT SELECTION
String WebTemplates::getModesPage(const DisplaySettings& settings) {
    String html = F("<!DOCTYPE html><html><head>");
    html += F("<meta charset=\"UTF-8\">");
    html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
    html += F("<title>Display Modes</title>");
    html += F("<style>");
    html += getSharedCSS();
    html += F("</style>");
    html += F("</head><body>");
    html += F("<div class=\"container\">");
    html += F("<div class=\"header\"><h1>🎯 Display Modes</h1></div>");
    html += getNavigation("modes");
    html += F("<div class=\"content\">");
    html += F("<form method=\"POST\" action=\"/update\">");
    html += F("<input type=\"hidden\" name=\"page\" value=\"modes\">");
    html += F("<div class=\"form-group\">");
    html += F("<label>🎮 Display Behavior:</label>");
    html += F("<div class=\"radio-group\">");

    // ALL MODE OPTIONS INCLUDING DAY AND DATE
    html += F("<label class=\"radio-item\">");
    html += F("<input type=\"radio\" name=\"displayMode\" value=\"message\"");
    html += (settings.mode == MODE_MANUAL) ? F(" checked") : F("");
    html += F("> 📜 Message Only");
    html += F("</label>");

    html += F("<label class=\"radio-item\">");
    html += F("<input type=\"radio\" name=\"displayMode\" value=\"clock\"");
    html += (settings.mode == MODE_CLOCK) ? F(" checked") : F("");
    html += F("> 🕐 Clock Only");
    html += F("</label>");

    html += F("<label class=\"radio-item\">");
    html += F("<input type=\"radio\" name=\"displayMode\" value=\"day\"");
    html += (settings.mode == MODE_DAY) ? F(" checked") : F("");
    html += F("> 📅 Day Only");
    html += F("</label>");

    html += F("<label class=\"radio-item\">");
    html += F("<input type=\"radio\" name=\"displayMode\" value=\"date\"");
    html += (settings.mode == MODE_DATE) ? F(" checked") : F("");
    html += F("> 📆 Date Only (20 JAN)");
    html += F("</label>");

    html += F("<label class=\"radio-item\">");
    html += F("<input type=\"radio\" name=\"displayMode\" value=\"auto\"");
    html += (settings.mode == MODE_AUTO_CYCLE) ? F(" checked") : F("");
    html += F("> 🔄 Auto Cycle");
    html += F("</label>");

    html += F("</div></div>");

    // AUTO CYCLE OPTIONS (REMOVED DATE FORMAT SECTION)
    html += F("<div id=\"autoOptions\" style=\"display:");
    html += (settings.mode == MODE_AUTO_CYCLE) ? F("block") : F("none");
    html += F("\">");
    html += F("<div class=\"form-group\">");
    html += F("<label>⏱️ Switch Every: <span id=\"ival\">");
    html += String(settings.modeSettings.cycleInterval);
    html += F("</span>s</label>");
    html += F("<input type=\"range\" name=\"cycleInterval\" min=\"2\" max=\"10\" value=\"");
    html += String(settings.modeSettings.cycleInterval);
    html += F("\" oninput=\"document.getElementById('ival').textContent=this.value\">");
    html += F("</div>");
    html += F("<div class=\"form-group\">");
    html += F("<label>📋 Include These Modes:</label>");
    html += F("<label class=\"radio-item\">");
    html += F("<input type=\"checkbox\" name=\"enableClock\"");
    html += (settings.modeSettings.clockEnabled) ? F(" checked") : F("");
    html += F("> 🕐 Clock (HH:MM)");
    html += F("</label>");
    html += F("<label class=\"radio-item\">");
    html += F("<input type=\"checkbox\" name=\"enableDay\"");
    html += (settings.modeSettings.dayEnabled) ? F(" checked") : F("");
    html += F("> 📅 Day (SUN/MON)");
    html += F("</label>");
    html += F("<label class=\"radio-item\">");
    html += F("<input type=\"checkbox\" name=\"enableDate\"");
    html += (settings.modeSettings.dateEnabled) ? F(" checked") : F("");
    html += F("> 📆 Date (20 JAN)");
    html += F("</label>");
    html += F("<label class=\"radio-item\">");
    html += F("<input type=\"checkbox\" name=\"enableMessage\"");
    html += (settings.modeSettings.messageEnabled) ? F(" checked") : F("");
    html += F("> 💬 Custom Message");
    html += F("</label>");
    html += F("</div></div>");
    html += F("<button type=\"submit\" class=\"btn\">💾 Save Mode Settings</button>");
    html += F("</form>");
    html += F("<div class=\"status\">");
    html += F("<strong>Current:</strong> ");
    html += getCurrentModeString(settings.mode);
    html += F(" | Time: <span id=\"time\">");
    html += TimeManager::getTimeString();
    html += F("</span>");
    html += F("</div>");
    html += F("</div></div>");
    html += F("<script>");
    html += F("document.querySelectorAll('input[name=\"displayMode\"]').forEach(r=>r.onchange=()=>document.getElementById('autoOptions').style.display=document.querySelector('input[value=\"auto\"]:checked')?'block':'none');");
    html += F("function updateTime(){fetch('/api/current-time').then(r=>r.text()).then(d=>document.getElementById('time').textContent=d)}");
    html += F("setInterval(updateTime,5000);updateTime()");
    html += F("</script>");
    html += F("</body></html>");
    return html;
}

String WebTemplates::getAdvancedPage(const DisplaySettings& settings) {
    String html = F("<!DOCTYPE html><html><head>");
    html += F("<meta charset=\"UTF-8\">");
    html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
    html += F("<title>Advanced Settings</title>");
    html += F("<style>");
    html += getSharedCSS();
    html += F("</style>");
    html += F("</head><body>");
    html += F("<div class=\"container\">");
    html += F("<div class=\"header\"><h1>⚙️ Advanced Settings</h1></div>");
    html += getNavigation("advanced");
    html += F("<div class=\"content\">");
    html += F("<div class=\"form-group\">");
    html += F("<label>🎯 Quick Actions:</label>");
    html += F("<button class=\"btn btn-orange\" onclick=\"quickAction('next')\">⏭️ Next Mode</button>");
    html += F("<button class=\"btn btn-orange\" onclick=\"quickAction('pause')\">⏸️ Pause/Resume</button>");
    html += F("<button class=\"btn btn-orange\" onclick=\"resetDefaults()\">🔄 Reset Defaults</button>");
    html += F("</div>");
    html += F("<div class=\"info\">");
    html += F("<strong>💡 8x32 Display Tips:</strong><br>");
    html += F("• Use shorter messages for better readability<br>");
    html += F("• Slower scroll speeds improve readability<br>");
    html += F("• Lower brightness saves power<br>");
    html += F("• Auto cycle shows different info throughout day<br>");
    html += F("• Date format is fixed to '20 JAN' for optimal display");
    html += F("</div>");
    html += F("</div></div>");
    html += F("<script>");
    html += F("function quickAction(action){");
    html += F("const url=action==='next'?'/api/next-mode':'/api/pause-mode';");
    html += F("fetch(url,{method:'POST'}).then(()=>setTimeout(()=>location.href='/',1000))");
    html += F("}");
    html += F("function resetDefaults(){");
    html += F("if(confirm('Reset all settings?')){");
    html += F("fetch('/api/reset',{method:'POST'}).then(()=>location.href='/')");
    html += F("}");
    html += F("}");
    html += F("</script>");
    html += F("</body></html>");
    return html;
}

String WebTemplates::getStatusPage(const String& networkSSID, const String& ipAddress, const DisplaySettings& settings) {
    String html = F("<!DOCTYPE html><html><head>");
    html += F("<meta charset=\"UTF-8\">");
    html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
    html += F("<title>System Status</title>");
    html += F("<style>");
    html += getSharedCSS();
    html += F("</style>");
    html += F("</head><body>");
    html += F("<div class=\"container\">");
    html += F("<div class=\"header\"><h1>📊 System Status</h1></div>");
    html += getNavigation("status");
    html += F("<div class=\"content\">");
    html += F("<div class=\"status\">");
    html += F("<strong>📊 Current Settings:</strong><br>");
    html += F("Message: \"");
    html += settings.message;
    html += F("\"<br>Mode: ");
    html += getCurrentModeString(settings.mode);
    html += F("<br>Brightness: ");
    html += String(settings.brightness);
    html += F("/8<br>Speed: ");
    html += String(settings.speed);
    html += F("ms<br>Cycle Interval: ");
    html += String(settings.modeSettings.cycleInterval);
    html += F("s");
    html += F("</div>");
    html += F("<div class=\"status\">");
    html += F("<strong>🔧 System Information:</strong><br>");
    html += F("Free Memory: <span id=\"memory\">Loading...</span><br>");
    html += F("WiFi Signal: <span id=\"signal\">Loading...</span><br>");
    html += F("Controller: ESP8266 (Wemos D1)<br>");
    html += F("Display: MAX7219 8x32 LED Matrix<br>");
    html += F("Firmware: WiMatrix v2.0<br>");
    html += F("Date Format: Fixed to '20 JAN'");
    html += F("</div>");
    html += F("<div class=\"status\">");
    html += F("<strong>🌐 Network Details:</strong><br>");
    html += F("SSID: ");
    html += networkSSID;
    html += F("<br>IP Address: ");
    html += ipAddress;
    html += F("<br>MAC Address: <span id=\"mac\">Loading...</span><br>");
    html += F("Uptime: <span id=\"uptime\">Loading...</span>");
    html += F("</div>");
    html += F("</div></div>");
    html += F("<script>");
    html += F("function updateStatus(){");
    html += F("fetch('/api/settings').then(r=>r.json()).then(d=>{");
    html += F("if(d.freeHeap)document.getElementById('memory').textContent=d.freeHeap+' bytes';");
    html += F("if(d.rssi)document.getElementById('signal').textContent=d.rssi+' dBm';");
    html += F("if(d.macAddress)document.getElementById('mac').textContent=d.macAddress");
    html += F("});");
    html += F("fetch('/api/uptime').then(r=>r.text()).then(d=>{");
    html += F("const h=Math.floor(d/3600);");
    html += F("const m=Math.floor((d%3600)/60);");
    html += F("document.getElementById('uptime').textContent=h+'h '+m+'m '+(d%60)+'s'");
    html += F("})");
    html += F("}");
    html += F("setInterval(updateStatus,3000);updateStatus()");
    html += F("</script>");
    html += F("</body></html>");
    return html;
}

String WebTemplates::get404Page(const String& uri, const String& method, int args) {
    String html = F("<!DOCTYPE html><html><head>");
    html += F("<title>404 Not Found</title>");
    html += F("<style>body{font-family:Arial;text-align:center;margin:50px}</style>");
    html += F("</head><body>");
    html += F("<h1>404 - Page Not Found</h1>");
    html += F("<p>");
    html += uri;
    html += F("</p>");
    html += F("<a href=\"/\" style=\"background:#4CAF50;color:white;padding:10px 20px;text-decoration:none;border-radius:4px\">🏠 Return Home</a>");
    html += F("</body></html>");
    return html;
}
