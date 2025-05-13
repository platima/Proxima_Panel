// Storage.ino for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Handles serialisation and config storage

#include <LittleFS.h>
#include <ArduinoJson.h>

// File name for settings
const char* settingsFile = "/settings.json";

// Initialize the file system
void initStorage() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system, attempting format...");
    // Try to format and begin once more
    LittleFS.format();
    if (!LittleFS.begin()) {
      Serial.println("File system mount failed permanently. Continuing with defaults.");
      return;
    }
    Serial.println("File system formatted and mounted successfully");
  } else {
    Serial.println("File system mounted successfully");
  }
}

// Load settings from storage
bool loadSettings() {
  // Check if the settings file exists
  if (!LittleFS.exists(settingsFile)) {
    Serial.println("No settings file found, using defaults");
    return false;
  }
  
  // Open the file for reading
  File file = LittleFS.open(settingsFile, "r");
  if (!file) {
    Serial.println("Failed to open settings file for reading");
    return false;
  }
  
  // Allocate a JsonDocument for animation support
  JsonDocument doc;
  doc.to<JsonObject>(); // Preallocate as object type
  
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("Failed to parse settings file: ");
    Serial.println(error.c_str());
    file.close();
    return false;
  }
  
  // Copy values from the JsonDocument to the settings with clamping
  red = constrain(doc["red"] | 20, 0, 255);
  green = constrain(doc["green"] | 20, 0, 255);
  blue = constrain(doc["blue"] | 20, 0, 255);
  rgbBrightnessLevel = constrain(doc["brightness"] | 50, 0, 255);
  
  // Load animation mode
  currentAnimation = getAnimationByName(doc["animation"] | "Static");
  
  // Close the file
  file.close();
  Serial.println("Settings loaded successfully");
  
  // Print the loaded settings
  Serial.print("Loaded RGB: (");
  Serial.print(red);
  Serial.print(",");
  Serial.print(green);
  Serial.print(",");
  Serial.print(blue);
  Serial.println(")");
  Serial.print("Brightness level: ");
  Serial.println(rgbBrightnessLevel);
  Serial.print("Animation mode: ");
  Serial.println(getAnimationName(currentAnimation));
  
  return true;
}

// Save settings to storage
bool saveSettings() {
  // Feed watchdog timer first thing
  ESP.wdtFeed();

  // Create a JsonDocument with larger size for animation support
  JsonDocument doc;
  doc.to<JsonObject>(); // Preallocate as object type 
  
  // Set the values in the document
  doc["red"] = red;
  doc["green"] = green;
  doc["blue"] = blue;
  doc["brightness"] = rgbBrightnessLevel;
  doc["animation"] = getAnimationName(currentAnimation);
  
  // Open the file for writing
  File file = LittleFS.open(settingsFile, "w");
  if (!file) {
    Serial.println("Failed to open settings file for writing");
    return false;
  }
  
  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write settings to file");
    file.close();
    return false;
  }
  
  // Close the file
  file.close();
  Serial.println("Settings saved successfully");
  return true;
}

// Save settings on significant changes
void saveSettingsIfNeeded() {
  static int last_red = -1;
  static int last_green = -1;
  static int last_blue = -1;
  static int last_brightness = -1;
  static AnimationMode last_animation = (AnimationMode)-1;
  static unsigned long lastSaveTime = 0;
  const unsigned long SAVE_DEBOUNCE_TIME = 1000; // 1 second debounce
  
  // Check if any values have changed significantly
  bool changed = false;
  
  if (abs(last_red - red) > 5) {
    last_red = red;
    changed = true;
  }
  
  if (abs(last_green - green) > 5) {
    last_green = green;
    changed = true;
  }
  
  if (abs(last_blue - blue) > 5) {
    last_blue = blue;
    changed = true;
  }
  
  if (abs(last_brightness - rgbBrightnessLevel) > 5) {
    last_brightness = rgbBrightnessLevel;
    changed = true;
  }
  
  if (last_animation != currentAnimation) {
    last_animation = currentAnimation;
    changed = true;
  }
  
  // If values have changed significantly and enough time has passed, save settings
  if (changed && (millis() - lastSaveTime > SAVE_DEBOUNCE_TIME)) {
    saveSettings();
    lastSaveTime = millis();
  }
}

// Format the file system if needed
ICACHE_FLASH_ATTR void formatStorage() {
  Serial.println("Formatting file system...");

  // Feed watchdog time
  ESP.wdtFeed();

  LittleFS.format();
  
  // Feed watchdog timer
  ESP.wdtFeed();
  Serial.println("File system formatted!");
}

// Function to be called from menu to reset all settings to default
void resetSettings() {
  red = 20;
  green = 20;
  blue = 20;
  rgbBrightnessLevel = 50;
  currentAnimation = STATIC;  // Reset to static mode
  
  saveSettings();
  Serial.println("Settings reset to defaults");
}