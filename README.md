# ESP32-C6 Stock and Crypto Price Display
This project uses an ESP32-C6 microcontroller to fetch and display real-time stock and cryptocurrency prices on a 1.47" ST7789 TFT display. It retrieves data for MicroStrategy (MSTR) from Finnhub and Crypto.com Coin (CRO) from CoinGecko, displaying the current price, price change, and percentage change. The display updates every 15 seconds, cycling between the configured tickers.
Features

Displays real-time price data for MSTR (stock) and CRO (cryptocurrency).
Shows price change and percentage change with color-coded indicators (green for positive, red for negative).
Uses WiFiManager for easy WiFi setup with a captive portal.
Features a splash screen with a custom CRO bitmap.
Smooth backlight fade-in effect for the TFT display.

Hardware Requirements

Microcontroller: ESP32-C6
Display: 1.47" ST7789 TFT LCD (172x320 resolution)
Pin Connections:
TFT_CS: Pin 14
TFT_DC: Pin 15
TFT_RST: Pin 21
TFT_BL (Backlight): Pin 22
TFT_MOSI: Pin 6
TFT_SCLK: Pin 7


WiFi connection for API requests

Software Dependencies

Arduino Libraries:
WiFiManager - For WiFi configuration
HTTPClient - For making HTTP requests
ArduinoJson - For parsing JSON API responses
Adafruit_GFX - For graphics rendering
Adafruit_ST7789 - For ST7789 display control
SPI - For communication with the TFT display


APIs:
Finnhub - For MSTR stock data
CoinGecko - For CRO cryptocurrency data



Setup Instructions

Hardware Setup:

Connect the ST7789 TFT display to the ESP32-C6 using the specified pins.
Ensure a stable power supply for the ESP32-C6 and display.


Install Arduino IDE:

Download and install the Arduino IDE.
Add ESP32 board support by following the ESP32 Arduino setup guide.


Install Libraries:

Open the Arduino IDE Library Manager (Sketch > Include Library > Manage Libraries).
Search for and install WiFiManager, ArduinoJson, Adafruit_GFX, and Adafruit_ST7789.
The HTTPClient and SPI libraries are included with the ESP32 Arduino core.


Configure API Key:

Replace the finnhubToken in the code with your own Finnhub API key. Sign up at Finnhub to obtain a free API key.
The CoinGecko API does not require an API key for the endpoints used.


Upload Code:

Open the provided .ino file in the Arduino IDE.
Connect your ESP32-C6 to your computer via USB.
Select the correct board (ESP32C6 Dev Module) and port in the Arduino IDE.
Upload the code to the ESP32-C6.


WiFi Configuration:

On first boot, the ESP32-C6 will create a WiFi access point named ESP32-Setup with the password 12345678.
Connect to this access point from a device and follow the captive portal to configure your WiFi credentials.
The device will automatically connect to the configured WiFi network on subsequent boots.



Usage

On startup, the display shows a CRO logo splash screen for 2 seconds.
The device attempts to connect to WiFi. If successful, it displays "WiFi Connected!" briefly.
If WiFi connection fails, it displays "WiFi Failed!" and restarts after 3 seconds.
Once connected, the display cycles between MSTR and CRO data every 15 seconds, showing:
Ticker symbol
Current price
Price change since the previous close (or previous day for CRO)
Percentage change (green for positive, red for negative)


If WiFi is lost, the display shows "WiFi Lost!" until the connection is restored.

File Structure

main.ino: Main Arduino sketch with the core logic.
CRO_bitmap.h: Header file containing the CRO logo bitmap for the splash screen.

Notes

Ensure your Finnhub API key is valid and has sufficient quota for requests.
The CoinGecko API is rate-limited; avoid reducing the update interval (updateInterval) below 15 seconds to prevent hitting limits.
The CRO_bitmap.h file must be present in the same directory as the .ino file for the splash screen to work.
Adjust the pin definitions in the code if your hardware setup differs.

Contributing
Contributions are welcome! Please submit issues or pull requests to the GitHub repository.
License
This project is licensed under the MIT License. See the LICENSE file for details.
