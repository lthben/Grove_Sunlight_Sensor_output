//sensor to I2C port
//LCD to I2C
//RGB LED stick to D6

#include <Wire.h>

#include "Arduino.h"
#include "SI114X.h"

SI114X SI1145 = SI114X();

#include "rgb_lcd.h"

rgb_lcd lcd;

//set brightness 0-255 and colour here
const int colorR = 255;
const int colorG = 255;
const int colorB = 255;

#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

//data is 0 to 9 corresponding to UV index of 0 to 11+
void colorWipe(uint8_t data, uint8_t wait)
{
  for (uint8_t i = 0; i <= data; i++)
  {
    if (i <= 1) strip.setPixelColor(i, strip.Color(0, 255, 0)); //green 0<index<2.9
    else if (i > 1 && i <= 3) strip.setPixelColor(i, strip.Color(255, 255, 0)); //yellow 3<index<5.9
    else if (i > 3 && i <= 5) strip.setPixelColor(i, strip.Color(255, 128, 0)); //orange 6<index<7.9
    else if (i > 5 && i <= 7) strip.setPixelColor(i, strip.Color(255, 0, 0)); //red 8<index<10.9
    else strip.setPixelColor(i, strip.Color(140, 0, 140)); //violet UV_index>11

    strip.show();
    delay(wait);
  }
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.setCursor(0, 0);

  Serial.println("Beginning Si1145!");
  lcd.print("Beginning Si1145!");
  delay(1000);

  while (!SI1145.Begin()) {
    Serial.println("Si1145 is not ready!");
    lcd.clear();
    lcd.print("Si1145 is not ready!");
    delay(1000);
  }
  Serial.println("Si1145 is ready!");
  lcd.clear();
  lcd.print("Si1145 is ready!");
  delay(1000);

  lcd.clear();
  strip.begin();
  strip.setBrightness(255);
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void) {

  lcd.setCursor(0, 0); //1st row
  lcd.print("UV index:");

  lcd.setCursor(0, 1); //2nd row

  Serial.print("//--------------------------------------//\r\n");
  Serial.print("Vis: "); Serial.println(SI1145.ReadVisible());
  Serial.print("IR: "); Serial.println(SI1145.ReadIR());
  //the real UV value must be div 100 from the reg value , datasheet for more information.
  float UV_reading = (float)SI1145.ReadUV() / 100;
  Serial.print("UV: ");  Serial.println(UV_reading);
  lcd.print(UV_reading);
  Serial.println();

  strip.clear();
  strip.show();
  delay(500);

  UV_reading = constrain(UV_reading, 0, 12);
  int data = map(UV_reading, 0, 12, 0, 9);

   colorWipe(data, 100);

  delay(5000);
  lcd.clear();
}
