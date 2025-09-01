#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "CRO_bitmap.h"  // Your splash bitmap

// Pin definitions for ESP32-C6
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21
#define TFT_BL   22
#define TFT_MOSI 6
#define TFT_SCLK 7

// LCD object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// API Key for Finnhub
const char* finnhubToken = "cviaiuhr01qks9q9b24gcviaiuhr01qks9q9b250";

// Tickers to display
const char* tickers[] = {"MSTR", "CRO"};
int tickerCount = 2;
int currentTicker = 0;

// CoinGecko endpoints
const char* coingeckoPrice = "https://api.coingecko.com/api/v3/simple/price?ids=crypto-com-chain&vs_currencies=usd";
const char* coingeckoHistory = "https://api.coingecko.com/api/v3/coins/crypto-com-chain/market_chart?vs_currency=usd&days=1";

// Helper: draw text centered
void drawCenteredText(const char* text, int y, int textSize, uint16_t color = ST77XX_WHITE) {
  tft.setTextSize(textSize);
  tft.setTextColor(color);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int x = (tft.width() - w) / 2;
  tft.setCursor(x, y);
  tft.println(text);
}

// Splash screen
void showSplashScreen() {
  tft.fillScreen(ST77XX_BLACK);
  int x = (tft.width() - 100) / 2;
  int y = (tft.height() - 100) / 2;
  tft.drawBitmap(x, y, epd_bitmap_CRO, 100, 100, ST77XX_WHITE);
  delay(2000);  // show splash for 2 seconds
}

void setup() {
  Serial.begin(115200);

  // Backlight setup
  pinMode(TFT_BL, OUTPUT);
  analogWriteFrequency(TFT_BL, 5000);
  analogWriteResolution(TFT_BL, 8);

  // Fade-in backlight to ~50%
  for (int duty = 0; duty <= 128; duty++) {
    analogWrite(TFT_BL, duty);
    delay(10);
  }

  // LCD init
  tft.init(172, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  // Show splash
  showSplashScreen();

  // Connect WiFi
  drawCenteredText("Connecting WiFi...", 130, 2);
  WiFiManager wm;
  if (!wm.autoConnect("ESP32-Setup", "12345678")) {
    tft.fillScreen(ST77XX_BLACK);
    drawCenteredText("WiFi Failed!", 100, 2);
    delay(3000);
    ESP.restart();
  }

  tft.fillScreen(ST77XX_BLACK);
  drawCenteredText("WiFi Connected!", 80, 2);
  delay(1500);
}

float fetchCROPreviousPrice() {
  HTTPClient http;
  http.begin(coingeckoHistory);
  int code = http.GET();
  float prevPrice = 0.0;
  if (code == 200) {
    String payload = http.getString();
    StaticJsonDocument<4096> doc;
    if (!deserializeJson(doc, payload)) {
      prevPrice = doc["prices"][0][1];
    }
  }
  http.end();
  return prevPrice;
}

void displayTicker(int tickerIndex) {
  const char* stockSymbol = tickers[tickerIndex];
  float price = 0.0;
  float change = 0.0;
  float percentChange = 0.0;
  String priceStr;
  String changeStr;
  String percentStr;
  uint16_t color = ST77XX_WHITE;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url;

    if (strcmp(stockSymbol, "MSTR") == 0) {
      url = "https://finnhub.io/api/v1/quote?symbol=MSTR&token=" + String(finnhubToken);
      http.begin(url);
      int code = http.GET();
      if (code == 200) {
        String payload = http.getString();
        StaticJsonDocument<512> doc;
        if (!deserializeJson(doc, payload)) {
          price = doc["c"];
          float prevClose = doc["pc"];
          change = price - prevClose;
          percentChange = (change / prevClose) * 100.0;
          priceStr = "$" + String(price, 2);
          changeStr = (change >= 0 ? "+" : "") + String(change, 2);
          percentStr = (percentChange >= 0 ? "+" : "") + String(percentChange, 2) + "%";
        }
      }
      http.end();
    } else if (strcmp(stockSymbol, "CRO") == 0) {
      http.begin(coingeckoPrice);
      int code = http.GET();
      if (code == 200) {
        String payload = http.getString();
        StaticJsonDocument<512> doc;
        if (!deserializeJson(doc, payload)) {
          price = doc["crypto-com-chain"]["usd"];
          priceStr = "$" + String(price, 4);
        }
      }
      http.end();

      float prevPrice = fetchCROPreviousPrice();
      change = price - prevPrice;
      percentChange = (change / prevPrice) * 100.0;
      changeStr = (change >= 0 ? "+" : "") + String(change, 4);
      percentStr = (percentChange >= 0 ? "+" : "") + String(percentChange, 2) + "%";
    }

    if (change > 0) color = ST77XX_GREEN;
    else if (change < 0) color = ST77XX_RED;

    tft.fillScreen(ST77XX_BLACK);
    drawCenteredText(stockSymbol, 20, 4);
    drawCenteredText(priceStr.c_str(), 70, 4, color);
    drawCenteredText(changeStr.c_str(), 110, 3, color);
    drawCenteredText(percentStr.c_str(), 140, 2, color);

  } else {
    tft.fillScreen(ST77XX_BLACK);
    drawCenteredText("WiFi Lost!", 80, 2, ST77XX_RED);
  }
}

void loop() {
  static unsigned long lastUpdate = 0;
  const unsigned long updateInterval = 15000; // 15 seconds

  if (millis() - lastUpdate >= updateInterval) {
    displayTicker(currentTicker);
    currentTicker++;
    if (currentTicker >= tickerCount) currentTicker = 0;
    lastUpdate = millis();
  }

  delay(50);
}
