#include <TFT_eSPI.h>
#include "rm67162.h"
#include "pins_config.h"
#include "fatFont.h"
#include <Adafruit_NeoPixel.h>

// WS2812B Setup
#define LED_PIN 14       // GPIO14
#define NUM_LEDS 1
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite s1 = TFT_eSprite(&tft);
TFT_eSprite s2 = TFT_eSprite(&tft);
TFT_eSprite s3 = TFT_eSprite(&tft);
TFT_eSprite spr = TFT_eSprite(&tft);

int slider[3] = {200, 230, 200};  // RGB slider positions (0–384)
int value[3]; // RGB values (0–255)
uint16_t tmpColor;

void drawGraRect(int16_t x, int16_t y, int leng, uint16_t color_start, uint16_t color_end, TFT_eSprite &sprite) {
  for (int i = 0; i <= leng; i++) {
    uint8_t interpolation = map(i, 0, leng, 0, 255);
    uint8_t red = map(interpolation, 0, 255, (color_start >> 8) & 0xF8, (color_end >> 8) & 0xF8);
    uint8_t green = map(interpolation, 0, 255, (color_start >> 3) & 0xFC, (color_end >> 3) & 0xFC);
    uint8_t blue = map(interpolation, 0, 255, (color_start << 3) & 0xF8, (color_end << 3) & 0xF8);
    uint16_t color = tft.color565(red, green, blue);
    sprite.fillRect(x, y - i, 10, 1, color);
  }
  sprite.fillSmoothCircle(x + 5, y - leng, 14, TFT_SILVER, TFT_BLACK);
  sprite.fillSmoothCircle(x + 5, y - leng, 7, color_start, TFT_SILVER);
}

void drawSliders() {
  int totalWidth = 3 * 60;
  int spacing = 10;
  int contentWidth = totalWidth + 2 * spacing;
  int startX = (240 - contentWidth) / 2;

  s1.fillSprite(TFT_BLACK);
  s1.fillSmoothRoundRect(2, 2, 56, 406, 3, 0x18E3, TFT_BLACK);
  drawGraRect(36, 400, slider[0], 0x6061, 0xFB92, s1);
  lcd_PushColors(startX, 116, 60, 420, (uint16_t *)s1.getPointer());

  s2.fillSprite(TFT_BLACK);
  s2.fillSmoothRoundRect(2, 2, 56, 406, 3, 0x18E3, TFT_BLACK);
  drawGraRect(36, 400, slider[1], 0x0A26, 0x1FD7, s2);
  lcd_PushColors(startX + 60 + spacing, 116, 60, 420, (uint16_t *)s2.getPointer());

  s3.fillSprite(TFT_BLACK);
  s3.fillSmoothRoundRect(2, 2, 56, 406, 3, 0x18E3, TFT_BLACK);
  drawGraRect(36, 400, slider[2], 0x02B0, 0x1D5E, s3);
  lcd_PushColors(startX + 2 * (60 + spacing), 116, 60, 420, (uint16_t *)s3.getPointer());
}

void drawRGBValues() {
  for (int i = 0; i < 3; i++) {
    value[i] = map(slider[i], 0, 384, 0, 255);
  }
  tmpColor = tft.color565(value[0], value[1], value[2]);

  spr.fillSprite(TFT_BLACK);
  spr.loadFont(fatFont);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("R: " + String(value[0]), 0, 0);
  spr.drawString("G: " + String(value[1]), 0, 30);
  spr.drawString("B: " + String(value[2]), 0, 60);
  spr.setTextColor(TFT_BLACK, tmpColor);
  spr.fillRect(112, 0, 120, 90, tmpColor);
  spr.drawString("0x" + String(tmpColor, HEX), 120, 35);
  spr.unloadFont();

  lcd_PushColors(4, 4, 232, 110, (uint16_t *)spr.getPointer());
}

void setup() {
  rm67162_init();
  lcd_setRotation(0);
  Serial.begin(115200);

  strip.begin();         // Initialize NeoPixel
  strip.setBrightness(50);
  strip.show();          // Clear LED

  s1.createSprite(60, 420);
  s2.createSprite(60, 420);
  s3.createSprite(60, 420);
  spr.createSprite(232, 110);
}

void loop() {
  // Read analog values from pins A0, A1, A2
  int rVal = analogRead(11);
  int gVal = analogRead(12);
  int bVal = analogRead(13);

  // Map to slider range
  slider[0] = map(rVal, 0, 4096, 0, 384);
  slider[1] = map(gVal, 0, 4096, 0, 384);
  slider[2] = map(bVal, 0, 4096, 0, 384);

  drawSliders();
  drawRGBValues();

  // Send RGB to WS2812B LED
  strip.setPixelColor(0, strip.Color(value[0], value[1], value[2]));
  strip.show();

  delay(100);
}
