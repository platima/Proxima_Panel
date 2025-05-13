0.1.0 2022-05-30
- Original version by Salvatore Raccardi

0.2.0 2025-05-07
- Initial fork by @Platima
- Removed Objex logo
- Removed unneccessary serial output
- Updated code for WS2812B LEDs
- Refactored code to suit new LCD library
- Changed for SH1106 OLED used
- Tested wi-fi functionality
- Minor refactor of code layout
- Made compatible with Arduino IDE 2.x
- Removed Firebase code

0.2.1 2025-05-07
- Added WiFiManager integration for captive portal
- Showing IP address display on OLED when connected
- Added configuration portal timeout (3 minutes) to prevent device hanging
- Updated visual feedback on OLED during WiFi setup process
- Minor file renames for neatness
- Removed hard-coded wifi credentials

0.2.2 2025-05-07
- Made wireless completely non-blocking using state machine
- Reduced connection timeout
- Added manual wifi config with up+down button hold
- Moved most headers to own file

0.2.3 2025-05-10
- Added persistent storage using LittleFS for all settings
- Implemented web interface for remote control at http://proxima.local/
- Added auto-save functionality so all changes immediately applied and saved
- Enhanced sliders with 0-255 range (brightness internally maps to 0-9)
- Made labels clickable as input fields for precise value entry
- Added hex color input/output field for easy color specification
- Implemented animation system with 4 modes: Breathing, Rainbow, Pulse, Color Fade
- Added Static/Animation toggle with dropdown selection
- Updated menu system with RESET option and confirmation dialog
- Improved web server integration with mDNS
- Enhanced WiFi connection handling with auto-reconnection
- Ensured all settings (including animation mode) now persist across restarts
- Updated layout for mobile-responsiveness
- Added JSON-based settings serialisation for future extensibility

0.2.4 2025-05-10
- Fixed brightness system to use full 0-255 range (no longer limited to 0-9)
- Implemented modular animation system with configuration per animation
- Added RGB control based on animation type (RGB controls disabled for Rainbow/Color Fade)
- Fixed breathing animation to properly display colors
- Improved animation smoothness with float-based calculations
- Added live updates to web interface - sliders now update immediately as dragged
- Improved pulse animation with smoother transitions
- Added debouncing to web interface saves for better performance
- Enhanced OLED display to show actual brightness value (0-255)
- Fixed web interface to properly handle disabled controls for appropriate animations
- Improved visual feedback when controls are disabled based on animation mode
- Added speed control per animation type for better customisation

0.2.5 2025-05-11
- Made display detection automatic
- Added triple-flash to convey no I2C display detected
- Updated source headers and related info
- Removed old AVR code
- Updated to support ArduinoJson 7
- Refactored some function names

0.2.6 2025-05-11
- Fixed physical button responsiveness - OLED display and LEDs now update while buttons are held
- RGB and brightness values change in real-time as buttons are pressed, providing immediate visual feedback
- Improved overall user experience when using physical controls

0.2.7 2025-05-12
- Replaced all delay() calls with millis()-based non-blocking timing
- Implemented throttling with UPDATE_INTERVAL (50ms) in main loop
- Added "Access-Control-Allow-Origin" header to web interface endpoints
- Enabled watchdog timer (8-second timeout) with proper feeding
- Improved file system error handling with format and retry mechanism
- Changed Wi-Fi reconnection to occur periodically (60-second intervals) to reduce power usage
- Added proper bounds checking with constrain() for all RGB and brightness values
- Implemented rate limiting for web interface (10 requests per second max)
- Replaced digitalRead() with Bounce2 library for improved button handling
- Added bounds checking to hsvToRgb() function to prevent calculation errors
- Made animation frame rate adaptive to MCU performance (adjusts between 10-100ms)
- Enhanced reset confirmation with 3-second hold requirement and progress bar
- Moved HTML template to PROGMEM to reduce heap fragmentation
- Fixed various edge cases for improved stability
- Major code cleanup and organization

0.2.8 2025-05-13
- Fixed critical stability issues with web interface and overall responsiveness
- Optimized memory utilization by changing MMU settings to "16KB cache + 48KB IRAM" (reduced IRAM usage from 94% to 69%)
- Reduced network stack overhead by switching to "lwIP v2 Lower Memory (no features)" variant
- Optimized web server to use chunked HTML delivery for the control interface
- Removed unused rgbPanelMode variable (only "Auto" mode was ever used)
- Added more frequent watchdog timer feeding in critical sections
- Fixed memory leaks in web interface HTML template handling
- Implemented improved error handling for web server requests
- Enhanced WiFi reconnection logic to prevent connection state lockups
- Optimized String usage throughout codebase
- Better memory management for animation routines
- Applied ICACHE_FLASH_ATTR to non-time-critical functions to move them from IRAM to Flash
- Added rate limiting for web interface to prevent request flooding
- Improved button responsiveness during web operations