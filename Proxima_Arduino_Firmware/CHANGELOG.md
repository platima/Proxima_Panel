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
- Added JSON-based settings serialization for future extensibility