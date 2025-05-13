// Animations.ino for Proxima Arduino Firmware
// 2025 Platima (https://github.com/platima https://plati.ma)
// Handles OLED display animations

// Adaptive frame rate variables
const unsigned long TARGET_FRAME_TIME = 5; // Target 5ms per frame (200 FPS)
unsigned long lastFrameCount = 0;
float currentFPS = 30; // Current measured FPS

// Get animation name from enum
const char* getAnimationName(AnimationMode mode) {
  return animationNames[mode];
}

// Get animation enum from name
AnimationMode getAnimationByName(const char* name) {
  for (int i = 0; i < 5; i++) {
    if (strcmp(name, animationNames[i]) == 0) {
      return (AnimationMode)i;
    }
  }
  return STATIC; // Default if not found
}

// Get animation configuration
const AnimationConfig& getAnimationConfig(AnimationMode mode) {
  return animationConfigs[mode];
}

// Process the current animation
void processAnimations() {
  // Skip animations if we're in static mode
  if (currentAnimation == STATIC) {
    return;
  }
  
  // Adaptive frame rate calculation
  unsigned long currentMillis = millis();
  if (currentMillis - lastFrameTime >= animationInterval) {
    // Calculate actual frame time
    unsigned long frameTime = currentMillis - lastFrameTime;
    currentFPS = 1000.0 / frameTime;
    
    // Adjust animation interval based on performance
    if (currentFPS < 30 && animationInterval < 100) {
      animationInterval += 5; // Decrease frame rate
    } else if (currentFPS > 60 && animationInterval > 10) {
      animationInterval -= 5; // Increase frame rate
    }
    
    // Cap between 10ms and 100ms (10-100 FPS)
    animationInterval = constrain(animationInterval, 10, 100);
    
    lastFrameTime = currentMillis;
    lastAnimationUpdate = currentMillis;
    
    // Apply speed multiplier from animation config
    float speedMultiplier = animationConfigs[currentAnimation].speed;
    
    // Process the current animation
    switch (currentAnimation) {
      case BREATHING:
        breathingAnimation(speedMultiplier);
        break;
      case RAINBOW:
        rainbowAnimation(speedMultiplier);
        break;
      case PULSE:
        pulseAnimation(speedMultiplier);
        break;
      case COLOR_FADE:
        colorFadeAnimation(speedMultiplier);
        break;
      default:
        // Static or unknown - do nothing
        break;
    }
  }
}

// Breathing animation - fades LED brightness up and down
void breathingAnimation(float speed) {
  // Update breathing intensity with smooth float values
  // We do this with floats to reduce MCU load
  int16_t breathingIntensityInt = breathingIntensity * 100;
  breathingIntensityInt += (breathingDirection * speed * 200) / 100;
  breathingIntensity = breathingIntensityInt / 100.0;
  
  // Check boundaries and reverse direction
  if (breathingIntensity >= 100.0) {
    breathingIntensity = 100.0;
    breathingDirection = -1.0;
  } else if (breathingIntensity <= 0.0) {
    breathingIntensity = 0.0;
    breathingDirection = 1.0;
  }
  
  // Calculate actual brightness based on the current level
  float intensityFactor = breathingIntensity / 100.0;
  byte actualBrightness = (byte)(rgbBrightnessLevel * intensityFactor);
  
  // Apply the brightness and set all pixels to current RGB
  RGBpanel.setBrightness(actualBrightness);
  for (int i = 0; i < RGBpanel.numPixels(); i++) {
    RGBpanel.setPixelColor(i, red, green, blue);
  }
  RGBpanel.show();
}

// Rainbow animation - cycles through all colors with smooth transitions
void rainbowAnimation(float speed) {
  rainbowOffset += (speed * 0.5); // Slower increment for smoother transitions
  if (rainbowOffset >= 256.0) rainbowOffset -= 256.0;
  
  RGBpanel.setBrightness(rgbBrightnessLevel);
  
  for (int i = 0; i < RGBpanel.numPixels(); i++) {
    // Calculate hue - spread the rainbow across all pixels with smooth float values
    float pixelHue = fmod((i * 256.0 / RGBpanel.numPixels() + rainbowOffset), 256.0);
    
    // Convert HSV to RGB
    byte r, g, b;
    hsvToRgb((byte)pixelHue, 255, 255, &r, &g, &b);
    
    // Set the pixel color
    RGBpanel.setPixelColor(i, r, g, b);
  }
  
  RGBpanel.show();
}

// Pulse animation - creates a pulse effect that travels through the strip
void pulseAnimation(float speed) {
  // Clear all pixels first
  RGBpanel.setBrightness(rgbBrightnessLevel);
  for (int i = 0; i < RGBpanel.numPixels(); i++) {
    RGBpanel.setPixelColor(i, 0, 0, 0);
  }
  
  // Update pulse position with smooth float values
  pulseStep += (pulseDirection * speed * 2.0);
  
  // Check boundaries and reverse direction
  if (pulseStep >= 100.0) {
    pulseStep = 100.0;
    pulseDirection = -1.0;
  } else if (pulseStep <= 0.0) {
    pulseStep = 0.0;
    pulseDirection = 1.0;
  }
  
  // Calculate pulse position and width
  float pulseCenter = map(pulseStep, 0.0, 100.0, -5.0, RGBpanel.numPixels() + 5.0);
  const float pulseWidth = 7.0; // Width of the pulse
  
  // Draw the pulse
  for (int i = 0; i < RGBpanel.numPixels(); i++) {
    // Calculate distance from center
    float distance = abs(i - pulseCenter);
    
    // Skip pixels too far from center
    if (distance > pulseWidth) {
      continue;
    }
    
    // Calculate intensity based on distance from center
    float falloff = 1.0 - (distance / pulseWidth);
    uint8_t intensity = (uint8_t )(255 * falloff);
    
    // Calculate color with the current RGB values but adjusted intensity
    uint8_t  r = (red * intensity) / 255;
    uint8_t  g = (green * intensity) / 255;
    uint8_t  b = (blue * intensity) / 255;
    
    // Set pixel color
    RGBpanel.setPixelColor(i, r, g, b);
  }
  
  RGBpanel.show();
}

// Color fade animation - slowly fades between colors in the spectrum
void colorFadeAnimation(float speed) {
  fadeHue += (speed * 0.3); // Even slower increment for smoother color transitions
  if (fadeHue >= 256.0) fadeHue -= 256.0;
  
  // Convert HSV to RGB
  byte r, g, b;
  hsvToRgb((byte)fadeHue, 255, 255, &r, &g, &b);
  
  // Set all pixels to the same color
  RGBpanel.setBrightness(rgbBrightnessLevel);
  for (int i = 0; i < RGBpanel.numPixels(); i++) {
    RGBpanel.setPixelColor(i, r, g, b);
  }
  
  RGBpanel.show();
}

// Utility function to convert HSV to RGB with bounds checking
// h: 0-255 for hue, s: 0-255 for saturation, v: 0-255 for value
void hsvToRgb(byte h, byte s, byte v, byte *r, byte *g, byte *b) {
  // Bounds checking
  h = constrain(h, 0, 255);
  s = constrain(s, 0, 255);
  v = constrain(v, 0, 255);
  
  if (s == 0) {
    // No saturation, just grayscale
    *r = *g = *b = v;
    return;
  }
  
  byte region = h / 43;
  byte remainder = (h - (region * 43)) * 6;
  
  // Ensure calculations don't overflow
  uint16_t p = (v * (255 - s)) >> 8;
  uint16_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  uint16_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
  
  // Ensure values are within byte range
  p = constrain(p, 0, 255);
  q = constrain(q, 0, 255);
  t = constrain(t, 0, 255);
  
  switch (region) {
    case 0:  *r = v; *g = t; *b = p; break;
    case 1:  *r = q; *g = v; *b = p; break;
    case 2:  *r = p; *g = v; *b = t; break;
    case 3:  *r = p; *g = q; *b = v; break;
    case 4:  *r = t; *g = p; *b = v; break;
    default: *r = v; *g = p; *b = q; break;
  }
}