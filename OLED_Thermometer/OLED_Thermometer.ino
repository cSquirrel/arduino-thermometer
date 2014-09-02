/*

  Uses TMP36 sensor and OLED Adafruit compatible display over I2C bus
  
  Requires:
   - TMP36 sensor
   - OLED Adafruit compatible display
   - https://github.com/adafruit/Adafruit_SSD1306
   - https://github.com/adafruit/Adafruit-GFX-Library
   
*/

#include <SPI.h>
#include <Wire.h>
#include <Math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
#define MAX_TEMPERATURE 40.0
#define DISPLAY_I2C_ADDR 0x3C
#define DISPLAY_UPDATE_FREQ 20

Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int tempSampleCounter = 0;
float samples[DISPLAY_UPDATE_FREQ];

void setup()   {                
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDR);
}

void loop() {

  float value = analogRead(A0);
  float voltage = value * (5.0 / 1023.0) - 0.15;
  float temperature = (100 * voltage) - 40;
  
  samples[tempSampleCounter] = temperature;
  tempSampleCounter++;
  if(tempSampleCounter == DISPLAY_UPDATE_FREQ) {
    float avgTemperature = 0.0;
    for(int i=0; i< DISPLAY_UPDATE_FREQ; i++) {
      avgTemperature += samples[i];
    }    
    avgTemperature = avgTemperature / DISPLAY_UPDATE_FREQ;
    updateDisplay(avgTemperature);
    tempSampleCounter=0;
  }
  
  delay(125);
}

void updateDisplay(float temperatureCelsius) {
  float factor = 1.0;
  if(temperatureCelsius < MAX_TEMPERATURE) {
    factor = ( MAX_TEMPERATURE-(MAX_TEMPERATURE - temperatureCelsius)) / MAX_TEMPERATURE;
  }
  Serial.println(factor);
  if(factor<0.0) factor = 0.0;
  float fillWidth = factor * display.width();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Temperature");
  
  display.print(round(temperatureCelsius));
  display.print("C ");
  
  float fahrenheit = temperatureCelsius*1.8 + 32.00;
  display.print(round(fahrenheit));
  display.print("F ");
  
  // progress bar
  display.drawRoundRect(0, 40, display.width(), 20, 2, WHITE); // x, y, width, height, radius, color  
  display.fillRoundRect(0, 40, fillWidth, 20, 2, WHITE);
  
  display.display();  
}
