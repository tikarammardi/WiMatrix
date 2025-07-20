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
        .form-section {
            background: linear-gradient(135deg, #f3e5f5 0%, #e1bee7 100%);
            padding: 25px;
            border-radius: 10px;
            margin: 20px 0;
            border-left: 4px solid #9c27b0;
        }
        .status-online { color: #4caf50; font-weight: bold; }
        .ip-address {
            font-family: 'Courier New', monospace;
            background: #f5f5f5;
            padding: 5px 10px;
            border-radius: 4px;
            display: inline-block;
        }
        .message-form {
            margin-top: 15px;
        }
        .message-input {
            width: 100%;
            padding: 12px 15px;
            font-size: 16px;
            border: 2px solid #ddd;
            border-radius: 8px;
            box-sizing: border-box;
            margin-bottom: 15px;
            transition: border-color 0.3s ease;
        }
        .message-input:focus {
            outline: none;
            border-color: #9c27b0;
            box-shadow: 0 0 0 3px rgba(156, 39, 176, 0.1);
        }
        .submit-btn {
            background: linear-gradient(135deg, #9c27b0 0%, #673ab7 100%);
            color: white;
            padding: 12px 30px;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            cursor: pointer;
            transition: transform 0.2s ease;
            margin-right: 10px;
        }
        .submit-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(156, 39, 176, 0.3);
        }
        .clear-btn {
            background: #757575;
            color: white;
            padding: 12px 20px;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            cursor: pointer;
            transition: background 0.3s ease;
        }
        .clear-btn:hover {
            background: #616161;
        }
        .current-message {
            background: #f5f5f5;
            padding: 15px;
            border-radius: 8px;
            margin-top: 15px;
            border-left: 4px solid #4caf50;
        }
        .preview-box {
            background: #000;
            color: #00ff00;
            padding: 15px;
            border-radius: 8px;
            font-family: 'Courier New', monospace;
            margin-top: 15px;
            text-align: center;
            font-size: 14px;
            letter-spacing: 2px;
        }
        .char-counter {
            text-align: right;
            font-size: 12px;
            color: #666;
            margin-top: 5px;
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

String WebTemplates::getMessageFormSection(const String& currentMessage) {
    String formSection = R"rawliteral(
        <div class="form-section">
            <h3>💬 Message Control</h3>
            <p>Enter your custom message to display on the LED matrix:</p>

            <form method="POST" action="/update" class="message-form">
                <input type="text"
                       name="message"
                       class="message-input"
                       placeholder="Enter your message here..."
                       value="%CURRENT_MESSAGE%"
                       maxlength="100"
                       id="messageInput"
                       autocomplete="off">
                <div class="char-counter">
                    <span id="charCount">0</span>/100 characters
                </div>
                <div style="margin-top: 15px;">
                    <button type="submit" class="submit-btn">🚀 Update Display</button>
                    <button type="button" class="clear-btn" onclick="clearMessage()">🗑️ Clear</button>
                </div>
            </form>

            <div class="current-message">
                <strong>Current Message:</strong> "<span id="currentMsg">%CURRENT_MESSAGE%</span>"
            </div>

            <div class="preview-box" id="previewBox">
                %CURRENT_MESSAGE%
            </div>
        </div>
    )rawliteral";

    formSection.replace("%CURRENT_MESSAGE%", currentMessage);
    return formSection;
}

String WebTemplates::getMainPage(const String& networkSSID, const String& ipAddress, const String& currentMessage) {
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
    html += getMessageFormSection(currentMessage);

    html += R"rawliteral(

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

        // Update character counter
        function updateCharCounter() {
            const input = document.getElementById('messageInput');
            const counter = document.getElementById('charCount');
            const preview = document.getElementById('previewBox');

            counter.textContent = input.value.length;
            preview.textContent = input.value || 'Preview will appear here...';

            // Color coding for character count
            if (input.value.length > 80) {
                counter.style.color = '#f44336';
            } else if (input.value.length > 60) {
                counter.style.color = '#ff9800';
            } else {
                counter.style.color = '#4caf50';
            }
        }

        // Clear message function
        function clearMessage() {
            const input = document.getElementById('messageInput');
            input.value = '';
            updateCharCounter();
            input.focus();
        }

        // Initialize
        document.getElementById('messageInput').addEventListener('input', updateCharCounter);

        setInterval(updateUptime, 1000);
        updateUptime();
        updateCharCounter();
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
