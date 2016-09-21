#include "SPI.h"
#include "Adafruit_WS2801.h"

uint8_t dataPin  = 5;
uint8_t clockPin = 6;
Adafruit_WS2801 strip = Adafruit_WS2801(10, dataPin, clockPin);

int pos = 0, dir = 1;
int LED[10] = {2,3,1,9,4,5,0,8,6,7};
int steps[8] = {1,1,2,1,1,2,1,1};
int currentStep = 0;
int currentLED = 0;
int mode = 0;
int sweepSpeed = 150;
int sweepInc = 10;
uint32_t bright[] = {0x0000FF, 0xFF0000};
uint32_t dim[] = {0x000010, 0x100000};
int interrupt = 0;
int batState = 1;

void setup() {
  strip.begin();
  attachInterrupt(0, modeShift, FALLING);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(1, lowBat, LOW);
  pinMode(3, INPUT);
}

void lowBat() {
  batState = 0;
}

void modeShift() {
  noInterrupts();
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 300) {
    if (mode <2) {
      mode++;
    } else {
      mode = 0;
    }
    interrupt = 1;
  }
  lastInterruptTime = interruptTime;
  interrupts();
}
      
void loop() {
  while (interrupt == 0) {
    if (batState == 0) {
      for (int a=0; a<10; a++) strip.setPixelColor(a, 0x000000);
      strip.show();
      delay(200);
      strip.setPixelColor(2, 0x050000);
      strip.show();
      delay(100);
      break;
    }
    for (int a=0; a<10; a++) strip.setPixelColor(a, 0xFFFFFF);
    strip.show();
    delay(1);
    for (int a=0; a<10; a++) strip.setPixelColor(a, 0x000000);
    strip.show();
    delay(100);
    if (mode <2) {
      while (interrupt == 0 && batState == 1) {
        for (int s=0; s<steps[currentStep]; s++) {
          strip.setPixelColor(LED[currentLED], bright[mode]);
          if (currentLED > 0) {
            if (s<1) {
              if (steps[currentStep-1] == 1) {
                strip.setPixelColor(LED[currentLED-1], dim[mode]);
              } else if (steps[currentStep-1] == 2) {
                strip.setPixelColor(LED[currentLED-1], dim[mode]);
                strip.setPixelColor(LED[currentLED-2], dim[mode]);
              }
            }
          }
          currentLED++;
        }
        strip.show();
        delay(sweepSpeed);
        if (currentStep < 7) {
          currentStep++;
        } else {
          if (steps[currentStep-1] == 1) {
            strip.setPixelColor(LED[currentLED-1], dim[mode]);
          } else if (steps[currentStep-1] == 2) {
            strip.setPixelColor(LED[currentLED-1], dim[mode]);
            strip.setPixelColor(LED[currentLED-2], dim[mode]);
          }
          currentStep = 0;
          currentLED = 0;
          if (sweepSpeed < 150 && sweepInc > 0) {
            sweepSpeed = sweepSpeed + sweepInc;
          } else if (sweepInc > 0) {
            sweepInc = -sweepInc;
          } else if (sweepSpeed > 40 & sweepInc < 0) {
            sweepSpeed = sweepSpeed + sweepInc;
          } else if (sweepInc < 0) {
            sweepInc = -sweepInc;
          }
        }
      }
    } else if (mode == 2) {
      while (interrupt == 0 && batState == 1) {
        int i, j;
        for (j=0; j < 256 * 5; j++) {
          if (interrupt != 0) {
            break;
          }
          for (i=0; i < strip.numPixels(); i++) {
            if (interrupt != 0) {
              break;
            }
            strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
          }  
          strip.show();
          delay(10);
        }
      }
    }
  }
  if(interrupt == 1) {
    interrupt = 0;
    sweepSpeed = 150;
    currentStep = 0;
    currentLED = 0;
  }
}

/* Helper functions */

uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
