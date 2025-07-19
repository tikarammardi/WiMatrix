//
// Created by Tikaram Mardi on 19/07/25.
//
#include "WebTemplates.h"

String WebTemplates::getCommonCSS() {
    return R"rawliteral(
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }
        .container {
            max-width: 600px;
            margin: 0 auto;
            background: white;
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 30px;
            font-size: 2.2em;
        }
        .info {
            background: linear-gradient(135deg, #e3f2fd 0%, #bbdefb 100%);
            padding: 20px;
            border-radius: 10px;
            margin: 20px 0;
            border-left: 4px solid #2196f3;
        }
        .status-online { color: #4caf50; font-weight: bold; }
        .ip-address {
            font-family: 'Courier New', monospace;
            background: #f5f5f5;
            padding: 5px 10px;
            border-radius: 4px;
            display: inline-block;
        }
    )rawliteral";
}

String WebTemplates::getSystemInfoSection() {
    return R"rawliteral(
        <div class="info">
            <h3>🔧 System Information</h3>
            <p><strong>Status:</strong> <span class="status-online">Web Server Running</span></p>
            <p><strong>Network:</strong> <span class="status-online">Connected</span></p>
            <p><strong>Display:</strong> MAX7219 8x32 LED Matrix</p>
            <p><strong>Controller:</strong> ESP8266 (Wemos D1)</p>
            <p><strong>Uptime:</strong> <span id="uptime">Loading...</span></p>
        </div>
    )rawliteral";
}

String WebTemplates::getMainPage(const String& networkSSID, const String& ipAddress) {
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiMatrix Controller</title>
    <style>
    )rawliteral";

    html += getCommonCSS();

    html += R"rawliteral(
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 WiMatrix LED Controller</h1>

        <div class="info">
            <h3>🌐 Network Information</h3>
            <p><strong>Connected to:</strong> %NETWORK_SSID%</p>
            <p><strong>IP Address:</strong> <span class="ip-address">%IP_ADDRESS%</span></p>
            <p><strong>Access URL:</strong> <code>http://%IP_ADDRESS%</code></p>
        </div>

    )rawliteral";

    html += getSystemInfoSection();

    html += R"rawliteral(

        <div class="info">
            <h3>💡 Quick Info</h3>
            <p>Hello World from your ESP8266 WiMatrix controller!</p>
            <p>Web interface for message control coming soon...</p>
        </div>

        <div style="text-align: center; margin-top: 30px; color: #666;">
            <small>WiMatrix v1.0 | Powered by ESP8266</small>
        </div>
    </div>

    <script>
        // Update uptime every second
        function updateUptime() {
            fetch('/api/uptime')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('uptime').textContent = data + 's';
                })
                .catch(err => console.log('Uptime fetch failed:', err));
        }

        setInterval(updateUptime, 1000);
        updateUptime(); // Initial call
    </script>
</body>
</html>
    )rawliteral";

    // Replace placeholders
    html.replace("%NETWORK_SSID%", networkSSID);
    html.replace("%IP_ADDRESS%", ipAddress);

    return html;
}

String WebTemplates::get404Page(const String& uri, const String& method, int args) {
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>404 - Page Not Found</title>
    <style>
    )rawliteral";

    html += getCommonCSS();

    html += R"rawliteral(
        .error { background: #ffebee; border-left: 4px solid #f44336; }
    </style>
</head>
<body>
    <div class="container">
        <h1>❌ 404 - Page Not Found</h1>

        <div class="info error">
            <h3>Request Details</h3>
            <p><strong>URI:</strong> %URI%</p>
            <p><strong>Method:</strong> %METHOD%</p>
            <p><strong>Arguments:</strong> %ARGS%</p>
        </div>

        <div style="text-align: center; margin-top: 30px;">
            <a href="/" style="background: #2196f3; color: white; padding: 12px 24px; text-decoration: none; border-radius: 6px;">
                🏠 Go Home
            </a>
        </div>
    </div>
</body>
</html>
    )rawliteral";

    // Replace placeholders
    html.replace("%URI%", uri);
    html.replace("%METHOD%", method);
    html.replace("%ARGS%", String(args));

    return html;
}
