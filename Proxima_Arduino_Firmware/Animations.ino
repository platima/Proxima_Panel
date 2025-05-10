// 
// Animations.ino - LED animation effects
// For Proxima LED Panel
// Version 0.2.0
//

// Animation timing variables
unsigned long lastAnimationUpdate = 0;
unsigned long animationInterval = 30; // Default 30ms (about 33fps)

// Animation parameters
byte breathingIntensity = 0;
byte breathingDirection = 0; // 0 = increasing, 1 = decreasing
byte rainbowOffset = 0;
byte fadeHue = 0;
byte pulseStep = 0;
byte pulseDirection = 0;

// Animation names for display and storage
const char* animationNames[] = {
  "Static",
  "Breathing",
  "Rainbow",
  "Pulse",
  "Color Fade"
};

// Get animation name from enum
String getAnimationName(AnimationMode mode) {
  return String(animationNames[mode]);
}

// Get animation enum from name
AnimationMode getAnimationByName(String name) {
  for (int i = 0; i < 5; i++) {
    if (name == animationNames[i]) {
      return (AnimationMode)i;
    }
  }
  return STATIC; // Default if not found
}

// Process the current animation
void processAnimations() {
  // Skip animations if we're in static mode
  if (currentAnimation == STATIC) {
    return;
  }
  
  // Check if it's time to update the animation
  unsigned long currentMillis = millis();
  if (currentMillis - lastAnimationUpdate < animationInterval) {
    return;
  }
  
  // Update animation timestamp
  lastAnimationUpdate = currentMillis;
  
  // Process the current animation
  switch (currentAnimation) {
    case BREATHING:
      breathingAnimation();
      break;
    case RAINBOW:
      rainbowAnimation();
      break;
    case PULSE:
      pulseAnimation();
      break;
    case COLOR_FADE:
      colorFadeAnimation();
      break;
    default:
      // Static or unknown - do nothing
      break;
  }
}

// Breathing animation - fades LED brightness up and down
void breathingAnimation() {
  // Update breathing intensity
  if (breathingDirection == 0) {
    // Increasing
    breathingIntensity++;
    if (breathingIntensity >= 100) {
      breathingDirection = 1;
    }
  } else {
    // Decreasing
    breathingIntensity--;
    if (breathingIntensity <= 1) {
      breathingDirection = 0;
    }
  }
  
  // Calculate actual brightness based on the current level
  int actualBrightness = map(breathingIntensity, 0, 100, 5, brightness[brightness_Level]);
  
  // Apply the brightness
  RGBpanel.setBrightness(actualBrightness);
  RGBpanel.show();
}

// Rainbow animation - cycles through all colors
void rainbowAnimation() {
  rainbowOffset++;
  
  for (int i = 0; i < RGBpanel.numPixels(); i++) {
    // Calculate hue - spread the rainbow across all pixels
    int pixelHue = (i * 256 / RGBpanel.numPixels() + rainbowOffset) & 255;
    
    // Convert HSV to RGB
    byte r, g, b;
    hsvToRgb(pixelHue, 255, 255, &r, &g, &b);
    
    // Set the pixel color
    RGBpanel.setPixelColor(i, r, g, b);
  }
  
  RGBpanel.show();
}

// Pulse animation - creates a pulse effect that travels through the strip
void pulseAnimation() {
  // Clear all pixels
  for (int i = 0; i < RGBpanel.numPixels(); i++) {
    RGBpanel.setPixelColor(i, 0, 0, 0);
  }
  
  // Calculate pulse position and width
  int pulseCenter = map(pulseStep, 0, 100, -5, RGBpanel.numPixels() + 5);
  const int pulseWidth = 7; // Width of the pulse
  
  // Draw the pulse
  for (int i = pulseCenter - pulseWidth; i <= pulseCenter + pulseWidth; i++) {
    // Skip pixels outside the strip
    if (i < 0 || i >= RGBpanel.numPixels()) {
      continue;
    }
    
    // Calculate intensity based on distance from center
    int distance = abs(i - pulseCenter);
    byte intensity = 255 - map(distance, 0, pulseWidth, 0, 255);
    
    // Calculate color with the current RGB values but adjusted intensity
    byte r = (red * intensity) / 255;
    byte g = (green * intensity) / 255;
    byte b = (blue * intensity) / 255;
    
    // Set pixel color
    RGBpanel.setPixelColor(i, r, g, b);
  }
  
  // Update pulse position
  if (pulseDirection == 0) {
    // Moving forward
    pulseStep++;
    if (pulseStep >= 100) {
      pulseDirection = 1;
    }
  } else {
    // Moving backward
    pulseStep--;
    if (pulseStep <= 0) {
      pulseDirection = 0;
    }
  }
  
  RGBpanel.show();
}

// Color fade animation - slowly fades between colors in the spectrum
void colorFadeAnimation() {
  fadeHue++;
  
  // Convert HSV to RGB
  byte r, g, b;
  hsvToRgb(fadeHue, 255, 255, &r, &g, &b);
  
  // Set all pixels to the same color
  for (int i = 0; i < RGBpanel.numPixels(); i++) {
    RGBpanel.setPixelColor(i, r, g, b);
  }
  
  RGBpanel.show();
}

// Utility function to convert HSV to RGB
// h: 0-255 for hue, s: 0-255 for saturation, v: 0-255 for value
void hsvToRgb(byte h, byte s, byte v, byte *r, byte *g, byte *b) {
  if (s == 0) {
    // No saturation, just grayscale
    *r = *g = *b = v;
    return;
  }
  
  byte region = h / 43;
  byte remainder = (h - (region * 43)) * 6;
  
  byte p = (v * (255 - s)) >> 8;
  byte q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  byte t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
  
  switch (region) {
    case 0:  *r = v; *g = t; *b = p; break;
    case 1:  *r = q; *g = v; *b = p; break;
    case 2:  *r = p; *g = v; *b = t; break;
    case 3:  *r = p; *g = q; *b = v; break;
    case 4:  *r = t; *g = p; *b = v; break;
    default: *r = v; *g = p; *b = q; break;
  }
}

// Update Storage.ino functions to save/load animation mode
// These functions should be copied to Storage.ino

// Add to loadSettings() function:
// String animationModeName = doc["animation"] | "Static";
// currentAnimation = getAnimationByName(animationModeName);

// Add to saveSettings() function:
// doc["animation"] = animationNames[currentAnimation];
