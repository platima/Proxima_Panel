// 
// WebInterface.ino - Web server for remote control
// For Proxima LED Panel
// Version 0.2.4
//

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);  // Create a web server on port 80

// HTML page template with CSS and JavaScript
const char* htmlTemplate = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Proxima LED Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
      background-color: #222;
      color: #fff;
    }
    h1 {
      text-align: center;
      color: #0cf;
    }
    .slider-container {
      margin: 20px 0;
    }
    .slider-label {
      display: inline-block;
      width: 100px;
      margin-right: 10px;
      cursor: pointer;
      color: #0cf;
    }
    .slider-label:hover {
      text-decoration: underline;
    }
    input[type="range"] {
      width: 60%;
      height: 20px;
    }
    input[type="number"] {
      width: 50px;
      background-color: #333;
      color: white;
      border: 1px solid #555;
      display: none;
    }
    .label-input {
      display: none;
      width: 80px;
      background-color: #333;
      color: white;
      border: 1px solid #555;
      padding: 2px;
    }
    .color-preview {
      width: 100%;
      height: 50px;
      margin: 20px 0;
      border-radius: 5px;
      border: 1px solid #444;
    }
    .hex-container {
      margin: 20px 0;
      text-align: center;
    }
    .hex-input {
      background-color: #333;
      color: white;
      border: 1px solid #555;
      padding: 5px;
      font-size: 18px;
      text-align: center;
      width: 120px;
    }
    .mode-controls {
      margin: 30px 0;
      text-align: center;
    }
    .mode-button {
      background-color: #0cf;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 4px 10px;
      cursor: pointer;
      border-radius: 4px;
    }
    .mode-button.active {
      background-color: #555;
    }
    #animationSelect {
      background-color: #333;
      color: white;
      border: 1px solid #555;
      padding: 5px;
      margin-left: 10px;
      display: none;
    }
    .status {
      text-align: center;
      margin-top: 20px;
      font-style: italic;
      color: #0f0;
    }
    .brightness-control {
      /* Marker class for brightness control */
    }
  </style>
</head>
<body>
  <h1>Proxima LED Control</h1>
  
  <div class="color-preview" id="colorPreview"></div>
  
  <div class="hex-container">
    <label>Hex Color: #</label>
    <input type="text" id="hexInput" class="hex-input" value="%HEX%" onchange="updateFromHex()" placeholder="RRGGBB">
  </div>
  
  <div class="slider-container">
    <span class="slider-label" id="redLabel" onclick="toggleInput('red')">Red: <span id="redValue">%RED%</span></span>
    <input type="text" id="redInput" class="label-input" value="%RED%" onblur="updateFromInput('red')" onkeypress="handleEnter(event, 'red')">
    <input type="range" min="0" max="255" value="%RED%" id="redSlider" onchange="debouncedSaveSettings()">
  </div>
  
  <div class="slider-container">
    <span class="slider-label" id="greenLabel" onclick="toggleInput('green')">Green: <span id="greenValue">%GREEN%</span></span>
    <input type="text" id="greenInput" class="label-input" value="%GREEN%" onblur="updateFromInput('green')" onkeypress="handleEnter(event, 'green')">
    <input type="range" min="0" max="255" value="%GREEN%" id="greenSlider" onchange="debouncedSaveSettings()">
  </div>
  
  <div class="slider-container">
    <span class="slider-label" id="blueLabel" onclick="toggleInput('blue')">Blue: <span id="blueValue">%BLUE%</span></span>
    <input type="text" id="blueInput" class="label-input" value="%BLUE%" onblur="updateFromInput('blue')" onkeypress="handleEnter(event, 'blue')">
    <input type="range" min="0" max="255" value="%BLUE%" id="blueSlider" onchange="debouncedSaveSettings()">
  </div>
  
  <div class="slider-container brightness-control">
    <span class="slider-label" id="brightnessLabel" onclick="toggleInput('brightness')">Brightness: <span id="brightnessValue">%BRIGHTNESS%</span></span>
    <input type="text" id="brightnessInput" class="label-input" value="%BRIGHTNESS%" onblur="updateFromInput('brightness')" onkeypress="handleEnter(event, 'brightness')">
    <input type="range" min="0" max="255" value="%BRIGHTNESS%" id="brightnessSlider" onchange="debouncedSaveSettings()">
  </div>
  
  <div class="mode-controls">
    <button id="staticButton" class="mode-button active" onclick="setMode('static')">Static</button>
    <button id="animationButton" class="mode-button" onclick="setMode('animation')">Animation</button>
    <select id="animationSelect">
      <option value="breathing">Breathing</option>
      <option value="rainbow">Rainbow</option>
      <option value="pulse">Pulse</option>
      <option value="colorFade">Color Fade</option>
    </select>
  </div>
  
  <div class="status" id="status"></div>
  
  <script>
    let currentMode = 'static';
    let isUpdating = false;
    let updateTimer = null;
    
    // Animation RGB control mapping
    const animationRGBControl = {
      'static': true,
      'breathing': true,
      'rainbow': false,
      'pulse': true,
      'colorFade': false
    };
    
    function toggleInput(type) {
      var label = document.getElementById(type + 'Label');
      var input = document.getElementById(type + 'Input');
      var valueSpan = document.getElementById(type + 'Value');
      
      if (input.style.display === 'none' || input.style.display === '') {
        input.style.display = 'inline-block';
        valueSpan.style.display = 'none';
        input.focus();
        input.select();
      } else {
        updateFromInput(type);
      }
    }
    
    function handleEnter(event, type) {
      if (event.key === 'Enter') {
        updateFromInput(type);
      }
    }
    
    function updateFromInput(type) {
      var input = document.getElementById(type + 'Input');
      var slider = document.getElementById(type + 'Slider');
      var value = parseInt(input.value);
      
      // Validate input
      if (value < 0 || value > 255) {
        value = Math.max(0, Math.min(255, value));
        input.value = value;
      }
      
      slider.value = value;
      updateValue(type);
      debouncedSaveSettings();
      
      // Hide input, show label
      input.style.display = 'none';
      document.getElementById(type + 'Value').style.display = 'inline';
    }
    
    function updateValue(type) {
      if (isUpdating) return;
      
      var slider = document.getElementById(type + 'Slider');
      var value = parseInt(slider.value);
      
      // Update the label
      document.getElementById(type + 'Value').textContent = value;
      
      // Update the input field
      document.getElementById(type + 'Input').value = value;
      
      // Update color preview and hex
      updateColor();
      
      // Send immediate live update for visual feedback
      if (currentMode === 'static' || 
          (currentMode === 'animation' && document.getElementById('animationSelect').value === 'pulse')) {
        debouncedSaveSettings();
      }
    }
    
    // Debounced settings save for smooth slider dragging
    function debouncedSaveSettings() {
      if (updateTimer) {
        clearTimeout(updateTimer);
      }
      
      updateTimer = setTimeout(function() {
        saveSettings();
      }, 100); // 100ms delay to batch updates
    }
    
    function updateColor() {
      if (isUpdating) return;
      
      var red = parseInt(document.getElementById('redSlider').value);
      var green = parseInt(document.getElementById('greenSlider').value);
      var blue = parseInt(document.getElementById('blueSlider').value);
      
      var colorPreview = document.getElementById('colorPreview');
      colorPreview.style.backgroundColor = 'rgb(' + red + ',' + green + ',' + blue + ')';
      
      // Update hex value
      var hex = ((red << 16) | (green << 8) | blue).toString(16).padStart(6, '0').toUpperCase();
      document.getElementById('hexInput').value = hex;
    }
    
    function updateFromHex() {
      var hex = document.getElementById('hexInput').value.replace('#', '');
      
      if (hex.length === 6) {
        var r = parseInt(hex.substr(0, 2), 16);
        var g = parseInt(hex.substr(2, 2), 16);
        var b = parseInt(hex.substr(4, 2), 16);
        
        document.getElementById('redSlider').value = r;
        document.getElementById('greenSlider').value = g;
        document.getElementById('blueSlider').value = b;
        
        updateValue('red');
        updateValue('green');
        updateValue('blue');
        
        debouncedSaveSettings();
      }
    }
    
    function updateRGBControlVisibility() {
      var currentAnimation = document.getElementById('animationSelect').value;
      var allowRGB = currentMode === 'static' || 
                     (currentMode === 'animation' && animationRGBControl[currentAnimation]);
      
      // Show/hide RGB controls
      var rgbControls = document.querySelectorAll('.slider-container:not(.brightness-control)');
      rgbControls.forEach(function(control) {
        control.style.opacity = allowRGB ? '1' : '0.5';
        var slider = control.querySelector('input[type="range"]');
        var input = control.querySelector('input[type="text"]');
        var label = control.querySelector('.slider-label');
        if (slider) slider.disabled = !allowRGB;
        if (input) input.disabled = !allowRGB;
        if (label) label.style.pointerEvents = allowRGB ? 'auto' : 'none';
      });
      
      // Update hex input
      var hexInput = document.getElementById('hexInput');
      hexInput.disabled = !allowRGB;
      hexInput.style.opacity = allowRGB ? '1' : '0.5';
    }
    
    function setMode(mode) {
      currentMode = mode;
      var staticBtn = document.getElementById('staticButton');
      var animationBtn = document.getElementById('animationButton');
      var animationSelect = document.getElementById('animationSelect');
      
      if (mode === 'static') {
        staticBtn.classList.add('active');
        animationBtn.classList.remove('active');
        animationSelect.style.display = 'none';
      } else {
        staticBtn.classList.remove('active');
        animationBtn.classList.add('active');
        animationSelect.style.display = 'inline-block';
      }
      
      updateRGBControlVisibility();
      saveMode();
    }
    
    function saveSettings() {
      if (isUpdating) return;
      isUpdating = true;
      
      var red = document.getElementById('redSlider').value;
      var green = document.getElementById('greenSlider').value;
      var blue = document.getElementById('blueSlider').value;
      var brightness = document.getElementById('brightnessSlider').value;
      
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          showStatus("Changes saved");
          isUpdating = false;
        }
      };
      xhttp.open("GET", "/save?r=" + red + "&g=" + green + "&b=" + blue + "&br=" + brightness, true);
      xhttp.send();
    }
    
    function saveMode() {
      var mode = currentMode;
      var animation = '';
      
      if (mode === 'animation') {
        animation = document.getElementById('animationSelect').value;
      }
      
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          showStatus("Mode changed");
          updateRGBControlVisibility();
        }
      };
      xhttp.open("GET", "/setMode?mode=" + mode + "&animation=" + animation, true);
      xhttp.send();
    }
    
    function showStatus(message) {
      var status = document.getElementById('status');
      status.innerHTML = message;
      setTimeout(function() { 
        status.innerHTML = ""; 
      }, 1000);
    }
    
    // Animation dropdown change handler
    document.getElementById('animationSelect').onchange = function() {
      if (currentMode === 'animation') {
        saveMode();
      }
    };
    
    // Add oninput handlers for live updates
    document.getElementById('redSlider').oninput = function() { updateValue('red'); };
    document.getElementById('greenSlider').oninput = function() { updateValue('green'); };
    document.getElementById('blueSlider').oninput = function() { updateValue('blue'); };
    document.getElementById('brightnessSlider').oninput = function() { updateValue('brightness'); };
    
    // Initialize
    updateColor();
    setMode('%MODE%');
    document.getElementById('animationSelect').value = '%ANIMATION%';
    updateRGBControlVisibility();
  </script>
</body>
</html>
)rawliteral";

// Initialize the web server
void setupWebServer() {
  // Configure mDNS responder
  if (MDNS.begin("proxima")) {
    Serial.println("MDNS responder started");
  }
  
  // Define server routes
  
  // Root path - display the control panel
  server.on("/", HTTP_GET, handleRoot);
  
  // Apply and save settings
  server.on("/save", HTTP_GET, handleSave);
  
  // Set mode (static or animation)
  server.on("/setMode", HTTP_GET, handleSetMode);
  
  // Get current settings (for initialization)
  server.on("/getSettings", HTTP_GET, handleGetSettings);
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

// Process web server requests
void handleWebServer() {
  server.handleClient();
  MDNS.update();
}

// Root handler - display the control panel
void handleRoot() {
  // Replace placeholders with current values
  String html = htmlTemplate;
  html.replace("%RED%", String(red));
  html.replace("%GREEN%", String(green));
  html.replace("%BLUE%", String(blue));
  html.replace("%BRIGHTNESS%", String(brightness_Level)); // Now 0-255 range
  
  // Calculate hex color
  String hexColor = "";
  char hex[7];
  sprintf(hex, "%02X%02X%02X", red, green, blue);
  hexColor = String(hex);
  html.replace("%HEX%", hexColor);
  
  // Mode and animation
  String mode = (currentAnimation == STATIC) ? "static" : "animation";
  html.replace("%MODE%", mode);
  
  String animName = "breathing"; // default
  switch(currentAnimation) {
    case BREATHING: animName = "breathing"; break;
    case RAINBOW: animName = "rainbow"; break;
    case PULSE: animName = "pulse"; break;
    case COLOR_FADE: animName = "colorFade"; break;
    default: break;
  }
  html.replace("%ANIMATION%", animName);
  
  server.send(200, "text/html", html);
}

// Apply and save settings
void handleSave() {
  // Get parameters from URL
  if (server.hasArg("r")) {
    red = server.arg("r").toInt();
  }
  
  if (server.hasArg("g")) {
    green = server.arg("g").toInt();
  }
  
  if (server.hasArg("b")) {
    blue = server.arg("b").toInt();
  }
  
  if (server.hasArg("br")) {
    brightness_Level = server.arg("br").toInt(); // Now directly 0-255
    // Ensure brightness is within valid range
    if (brightness_Level < 0) brightness_Level = 0;
    if (brightness_Level > 255) brightness_Level = 255;
  }
  
  // Apply settings immediately
  if (currentAnimation == STATIC) {
    panelSet(0);
  }
  
  // Save to storage
  saveSettings();
  
  // Confirm success
  server.send(200, "text/plain", "OK");
}

// Set the mode (static or animation)
void handleSetMode() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    
    if (mode == "static") {
      currentAnimation = STATIC;
      panelSet(0); // Apply current static color
    } else if (mode == "animation") {
      if (server.hasArg("animation")) {
        String anim = server.arg("animation");
        
        if (anim == "breathing") {
          currentAnimation = BREATHING;
        } else if (anim == "rainbow") {
          currentAnimation = RAINBOW;
        } else if (anim == "pulse") {
          currentAnimation = PULSE;
        } else if (anim == "colorFade") {
          currentAnimation = COLOR_FADE;
        }
      }
    }
    
    // Save the mode change
    saveSettings();
  }
  
  server.send(200, "text/plain", "OK");
}

// Get current settings (JSON response)
void handleGetSettings() {
  String json = "{\"red\":" + String(red) + 
                ",\"green\":" + String(green) + 
                ",\"blue\":" + String(blue) + 
                ",\"brightness\":" + String(brightness_Level) + 
                ",\"mode\":\"" + ((currentAnimation == STATIC) ? "static" : "animation") + "\"" +
                ",\"animation\":\"" + String(getAnimationName(currentAnimation)) + "\"}";
                
  server.send(200, "application/json", json);
}