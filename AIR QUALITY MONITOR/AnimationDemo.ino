#include "rm67162.h"
#include <TFT_eSPI.h>
#include "NotoSansBold36.h"
#include "NotoSansMonoSCB20.h"
#include <DFRobot_ENS160.h>
#define I2C_COMMUNICATION  //I2C communication. Comment out this line of code if you want to use SPI communication.
#ifdef  I2C_COMMUNICATION
#endif
DFRobot_ENS160_I2C ENS160(&Wire, /*I2CAddr*/ 0x53);
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

// Ring configuration
struct Ring {
  String label;
  uint16_t color;
  byte value;
  byte change;
  byte goal;
  int centerX;
  int centerY;
};

Ring rings[2] = {
  {"TVOC", 0x47DD, 30, 1, 70, 136, 120},  // Left ring
  {"ECO2", 0x86BF, 65, -1, 20, 400, 120}  // Right ring, symmetrically placed
};

void setup() {
  sprite.createSprite(536, 241);
  sprite.setSwapBytes(1);
  rm67162_init();
  lcd_setRotation(1);
  while( NO_ERR != ENS160.begin() ){
    Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }
  Serial.println("Begin ok!");

  ENS160.setINTMode(ENS160.eINTModeEN | 
                    ENS160.eINTPinPP | 
                    ENS160.eINTPinActiveHigh);

}

void drawRing(Ring& r) {
  // Background arc with slightly smaller radii
  sprite.drawSmoothArc(r.centerX, r.centerY, 110, 90, 45, 315, 0x21C9, 0x2A0A);  // Background arc

  // Active arc with slightly smaller radii
  sprite.drawSmoothArc(r.centerX, r.centerY, 110, 90, 45, map(r.value, 0, 100, 46, 315), r.color, 0x2A0A);  // Active arc

  // Value
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.loadFont(NotoSansMonoSCB20);
  sprite.drawString(String(r.value), r.centerX - 18, r.centerY - 10);  // Adjusted value position
  sprite.unloadFont();

  // Label
  sprite.setTextColor(TFT_WHITE, 0x2A0A);
  sprite.loadFont(NotoSansBold36);
  sprite.drawString(r.label, r.centerX - 40, r.centerY + 80);  // Adjusted label position
  sprite.unloadFont();
}

void drawSprite() {
  sprite.fillSprite(TFT_BLACK);

  // Draw the rings
  drawRing(rings[0]);
  drawRing(rings[1]);

  lcd_PushColors(0, 0, 536, 240, (uint16_t*)sprite.getPointer());
}

void loop() {
  // Update ring values
  uint16_t TVOC = ENS160.getTVOC();
  uint16_t ECO2 = ENS160.getECO2();
  delay(100);
  rings[0].value = TVOC; 
  rings[1].value = map(ECO2, 400, 6500, 0, 100); 
  drawSprite();
  delay(100);
}