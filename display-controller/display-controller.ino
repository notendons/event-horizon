#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <Adafruit_NeoPixel.h> 

MCUFRIEND_kbv tft;

#define LED_PIN        10  
#define NUM_LEDS       10 
#define SAFE_LEDS       9  
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
int targetLED = 0;   
int lastTargetLED = -1; 

const int originX = 160; 
const int originY = 190; 
int oldEndX = originX;
int oldEndY = originY;

#define BLACK   0x0000
#define WHITE   0xFFFF
#define YELLOW  0xFFE0
#define GREEN   0x07E0
#define CYAN    0x07FF
#define GREY    0x7BEF
#define RED     0xF800

void updateVectorDisplay(float angleDegrees, float magnitude, uint16_t vectorColor, bool thickLine);
void updateLEDStrip(); 

void setup() {
  Serial.begin(9600); 

  strip.begin();
  strip.setBrightness(30); 
  strip.show(); 
  //choose the first random diode
  targetLED = random(0, SAFE_LEDS); 
  updateLEDStrip();
  
  //display init
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486; 
  tft.begin(ID);
  
  tft.setRotation(1); 
  tft.fillScreen(BLACK);
  
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(15, 12);
  tft.print("Orbital Simulator");
  
  tft.setTextSize(1);
  tft.setCursor(15, 38);
  tft.setTextColor(YELLOW);
  tft.print("Status: READY");
  
  tft.fillCircle(originX, originY, 6, GREY);
}

void loop() {
  static float lastAngle = 0;
  static float lastMagnitude = 45; 

  if (Serial.available() > 0) {
    String inputData = Serial.readStringUntil('\n');
    inputData.trim(); 

    if (inputData.length() == 0) return; 

    tft.fillRect(15, 60, 290, 15, BLACK); 
    tft.setCursor(15, 60);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.print("Serial rx: [");
    tft.print(inputData);
    tft.print("]");

    if (inputData == "LAUNCH") {
      tft.fillRect(15, 38, 140, 12, BLACK); 
      tft.setCursor(15, 38);
      tft.setTextColor(RED); 
      tft.print("Status: LAUNCHING!");
      
      // vector update in red
      updateVectorDisplay(lastAngle, lastMagnitude, RED, true);
      
      targetLED = random(0, SAFE_LEDS); //randomize a new samizne
      lastTargetLED = -1;               
      updateLEDStrip();                 
    } 
    else if (inputData == "READY") {
      tft.fillRect(15, 38, 140, 12, BLACK);
      tft.setCursor(15, 38);
      tft.setTextColor(YELLOW);
      tft.print("Status: READY");
      
      // return the green vector
      updateVectorDisplay(lastAngle, lastMagnitude, GREEN, false);
    } 
    else {
      int commaIndex = inputData.indexOf(',');
      if (commaIndex > 0 && commaIndex < inputData.length() - 1) {
        String angleStr = inputData.substring(0, commaIndex);
        String speedStr = inputData.substring(commaIndex + 1);
        
        float stepperAngle = angleStr.toFloat();
        int speedDelay = speedStr.toInt();
        
        if (stepperAngle >= 0 && stepperAngle <= 180 && speedDelay > 0) {
          lastAngle = stepperAngle;
          lastMagnitude = map(speedDelay, 2, 15, 95, 35);
          //just an update
          updateVectorDisplay(lastAngle, lastMagnitude, GREEN, false);
        }
      }
    }
  }
}

void updateVectorDisplay(float angleDegrees, float magnitude, uint16_t vectorColor, bool thickLine) {
  tft.drawLine(originX, originY, oldEndX, oldEndY, BLACK);
  tft.drawLine(originX + 1, originY, oldEndX + 1, oldEndY, BLACK); 
  tft.fillCircle(oldEndX, oldEndY, 4, BLACK); 

  tft.fillCircle(originX, originY, 8, GREY);

  float Angle = angleDegrees;
  float angleRadians = Angle * 0.01745329; 
  int newEndX = originX + (int)(magnitude * cos(angleRadians));
  int newEndY = originY - (int)(magnitude * sin(angleRadians)); 

  tft.drawLine(originX, originY, newEndX, newEndY, vectorColor);
  if (thickLine) {
    tft.drawLine(originX + 1, originY, newEndX + 1, newEndY, vectorColor);
    tft.fillCircle(newEndX, newEndY, 3, vectorColor);
  } else {
    tft.fillCircle(newEndX, newEndY, 2, vectorColor);
  }

  oldEndX = newEndX;
  oldEndY = newEndY;
}

// rgb update function
void updateLEDStrip() {
  delay(2000);
  if (targetLED != lastTargetLED) {
    strip.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i == targetLED) {
        strip.setPixelColor(i, strip.Color(255, 0, 0)); // Одиночный случайный красный огонек
      } else {
        strip.setPixelColor(i, strip.Color(0, 0, 100)); // Все остальные - темно-синие (background)
      }
    }
    strip.show();
    lastTargetLED = targetLED;
  }
}