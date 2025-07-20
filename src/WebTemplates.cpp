#include "WebTemplates.h"
#include "Config.h"

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
            max-width: 700px;
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
        .advanced-section {
            background: linear-gradient(135deg, #e8f5e8 0%, #c8e6c9 100%);
            padding: 25px;
            border-radius: 10px;
            margin: 20px 0;
            border-left: 4px solid #4caf50;
        }
        .status-online { color: #4caf50; font-weight: bold; }
        .ip-address {
            font-family: 'Courier New', monospace;
            background: #f5f5f5;
            padding: 5px 10px;
            border-radius: 4px;
            display: inline-block;
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
        .control-group {
            display: flex;
            gap: 20px;
            margin-bottom: 20px;
            flex-wrap: wrap;
        }
        .control-item {
            flex: 1;
            min-width: 200px;
        }
        .control-label {
            display: block;
            margin-bottom: 8px;
            font-weight: bold;
            color: #333;
        }
        .range-input, .select-input {
            width: 100%;
            padding: 8px;
            border: 2px solid #ddd;
            border-radius: 6px;
            font-size: 14px;
        }
        .range-input {
            -webkit-appearance: none;
            height: 8px;
            background: #ddd;
            outline: none;
        }
        .range-input::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 20px;
            height: 20px;
            background: #4caf50;
            cursor: pointer;
            border-radius: 50%;
        }
        .value-display {
            background: #f5f5f5;
            padding: 5px 10px;
            border-radius: 4px;
            font-family: monospace;
            font-size: 14px;
            display: inline-block;
            margin-left: 10px;
        }
        .submit-btn {
            background: linear-gradient(135deg, #4caf50 0%, #45a049 100%);
            color: white;
            padding: 15px 30px;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            cursor: pointer;
            transition: transform 0.2s ease;
            margin-right: 15px;
        }
        .submit-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(76, 175, 80, 0.3);
        }
        .reset-btn {
            background: #757575;
            color: white;
            padding: 15px 25px;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            cursor: pointer;
            transition: background 0.3s ease;
        }
        .reset-btn:hover {
            background: #616161;
        }
        .current-settings {
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
            min-height: 40px;
            display: flex;
            align-items: center;
            justify-content: center;
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

String WebTemplates::getAnimationOptionsHTML(int selectedIndex) {
    String options = "";
    for (int i = 0; i < ANIMATION_COUNT; i++) {
        String selected = (i == selectedIndex) ? " selected" : "";
        options += "<option value=\"" + String(i) + "\"" + selected + ">" +
                   String(ANIMATION_EFFECTS[i].name) + "</option>";
    }
    return options;
}

String WebTemplates::getAdvancedControlsSection(const DisplaySettings& settings) {
    String controlsSection = R"rawliteral(
        <div class="advanced-section">
            <h3>⚙️ Advanced Display Controls</h3>

            <form method="POST" action="/update" id="displayForm">
                <!-- Message Input -->
                <div style="margin-bottom: 20px;">
                    <label class="control-label" for="messageInput">💬 Message Text</label>
                    <input type="text"
                           name="message"
                           id="messageInput"
                           class="message-input"
                           placeholder="Enter your message here..."
                           value="%MESSAGE%"
                           maxlength="100"
                           autocomplete="off">
                    <div class="char-counter">
                        <span id="charCount">0</span>/100 characters
                    </div>
                </div>

                <!-- Advanced Controls -->
                <div class="control-group">
                    <div class="control-item">
                        <label class="control-label" for="brightness">💡 Brightness</label>
                        <input type="range"
                               name="brightness"
                               id="brightness"
                               class="range-input"
                               min="0" max="15"
                               value="%BRIGHTNESS%">
                        <span class="value-display"><span id="brightnessValue">%BRIGHTNESS%</span>/15</span>
                    </div>

                    <div class="control-item">
                        <label class="control-label" for="speed">⚡ Animation Speed</label>
                        <input type="range"
                               name="speed"
                               id="speed"
                               class="range-input"
                               min="20" max="200"
                               value="%SPEED%">
                        <span class="value-display"><span id="speedValue">%SPEED%</span>ms</span>
                    </div>
                </div>

                <div class="control-group">
                    <div class="control-item">
                        <label class="control-label" for="animation">🎭 Animation Effect</label>
                        <select name="animation" id="animation" class="select-input">
                            %ANIMATION_OPTIONS%
                        </select>
                    </div>
                </div>

                <!-- Buttons -->
                <div style="margin-top: 25px;">
                    <button type="submit" class="submit-btn">🚀 Update Display</button>
                    <button type="button" class="reset-btn" onclick="resetToDefaults()">🔄 Reset Defaults</button>
                </div>
            </form>

            <!-- Current Settings Display -->
            <div class="current-settings">
                <h4>📊 Current Settings</h4>
                <p><strong>Message:</strong> "<span id="currentMsg">%MESSAGE%</span>"</p>
                <p><strong>Brightness:</strong> <span id="currentBrightness">%BRIGHTNESS%</span>/15</p>
                <p><strong>Speed:</strong> <span id="currentSpeed">%SPEED%</span>ms</p>
                <p><strong>Animation:</strong> <span id="currentAnimation">%ANIMATION_NAME%</span></p>
            </div>

            <!-- Live Preview -->
            <div class="preview-box" id="previewBox">%MESSAGE%</div>
        </div>
    )rawliteral";

    // Replace placeholders
    controlsSection.replace("%MESSAGE%", settings.message);
    controlsSection.replace("%BRIGHTNESS%", String(settings.brightness));
    controlsSection.replace("%SPEED%", String(settings.speed));
    controlsSection.replace("%ANIMATION_OPTIONS%", getAnimationOptionsHTML(settings.animationIndex));
    controlsSection.replace("%ANIMATION_NAME%", String(ANIMATION_EFFECTS[settings.animationIndex].name));

    return controlsSection;
}

String WebTemplates::getMainPage(const String& networkSSID, const String& ipAddress, const DisplaySettings& settings) {
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiMatrix Advanced Controller</title>
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
    html += getAdvancedControlsSection(settings);

    html += R"rawliteral(

        <div style="text-align: center; margin-top: 30px; color: #666;">
            <small>WiMatrix v2.0 | Advanced LED Matrix Controller</small>
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

        // Update character counter and preview
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

        // Update brightness value display
        function updateBrightnessDisplay() {
            const slider = document.getElementById('brightness');
            const display = document.getElementById('brightnessValue');
            display.textContent = slider.value;
        }

        // Update speed value display
        function updateSpeedDisplay() {
            const slider = document.getElementById('speed');
            const display = document.getElementById('speedValue');
            display.textContent = slider.value;
        }

        // Reset to default values
        function resetToDefaults() {
            document.getElementById('messageInput').value = 'Welcome to WiMatrix!';
            document.getElementById('brightness').value = '5';
            document.getElementById('speed').value = '60';
            document.getElementById('animation').value = '0';

            updateCharCounter();
            updateBrightnessDisplay();
            updateSpeedDisplay();
        }

        // Initialize event listeners
        document.getElementById('messageInput').addEventListener('input', updateCharCounter);
        document.getElementById('brightness').addEventListener('input', updateBrightnessDisplay);
        document.getElementById('speed').addEventListener('input', updateSpeedDisplay);

        // Initialize displays
        setInterval(updateUptime, 1000);
        updateUptime();
        updateCharCounter();
        updateBrightnessDisplay();
        updateSpeedDisplay();
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
    // Keep existing 404 page implementation
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

    html.replace("%URI%", uri);
    html.replace("%METHOD%", method);
    html.replace("%ARGS%", String(args));

    return html;
}
