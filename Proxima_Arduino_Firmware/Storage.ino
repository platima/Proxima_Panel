// 
// Storage.ino - Persistent storage implementation
// For Proxima LED Panel
// Version 0.2.0
//

#include <LittleFS.h>
#include <ArduinoJson.h>

// File name for settings
const char* settingsFile = "/settings.json";

// Initialize the file system
void initStorage() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    // Continue anyway - we'll use defaults
    return;
  }
  
  Serial.println("File system mounted successfully");
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
  
  // Allocate a JsonDocument with larger size for animation support
  StaticJsonDocument<512> doc;
  
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("Failed to parse settings file: ");
    Serial.println(error.c_str());
    file.close();
    return false;
  }
  
  // Copy values from the JsonDocument to the settings
  red = doc["red"] | 20;  // Default to 20 if not present
  green = doc["green"] | 20;
  blue = doc["blue"] | 20;
  brightness_Level = doc["brightness"] | 1;
  panelMode = doc["mode"] | "Auto";
  
  // Load animation mode
  String animationModeName = doc["animation"] | "Static";
  currentAnimation = getAnimationByName(animationModeName);
  
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
  Serial.println(brightness_Level);
  Serial.print("Panel mode: ");
  Serial.println(panelMode);
  Serial.print("Animation mode: ");
  Serial.println(animationModeName);
  
  return true;
}

// Save settings to storage
bool saveSettings() {
  // Create a JsonDocument with larger size for animation support
  StaticJsonDocument<512> doc;
  
  // Set the values in the document
  doc["red"] = red;
  doc["green"] = green;
  doc["blue"] = blue;
  doc["brightness"] = brightness_Level;
  doc["mode"] = panelMode;
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
  static String last_mode = "";
  static AnimationMode last_animation = (AnimationMode)-1;
  
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
  
  if (last_brightness != brightness_Level) {
    last_brightness = brightness_Level;
    changed = true;
  }
  
  if (last_mode != panelMode) {
    last_mode = panelMode;
    changed = true;
  }
  
  if (last_animation != currentAnimation) {
    last_animation = currentAnimation;
    changed = true;
  }
  
  // If values have changed significantly, save settings
  if (changed) {
    saveSettings();
  }
}

// Format the file system if needed
void formatStorage() {
  Serial.println("Formatting file system...");
  LittleFS.format();
  Serial.println("File system formatted!");
}

// Function to be called from menu to reset all settings to default
void resetSettings() {
  red = 20;
  green = 20;
  blue = 20;
  brightness_Level = 1;
  panelMode = "Auto";
  currentAnimation = STATIC;  // Reset to static mode
  
  saveSettings();
  Serial.println("Settings reset to defaults");
}