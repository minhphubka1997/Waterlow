/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  Output any data on LCD widget!

  App project setup:

    LCD widget, SIMPLE mode, in widget settings :

    - Select pin V0 for zero pin
    - Select pin V1 for first pin
    - Change "Reading Frequency" to PUSH mode
    - Type into first edit field "/pin0/ seconds"
    - Type into second edit field "/pin1/ millis"
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "stdint.h"

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "EH-lfxfpsVLQbxcGVbBgK-34VNcNJA3J";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "HVC-T2";
char pass[] = "havicom2020";


#define PIN_EN4V  04    // Output to on/off 4V power
#define LCD_BTN   39    // Input from button to control on/off LCD

#define LCD_CRT   13    // Output Pin to control on/off LCD
#define LCD_EN    33    // Output EN to LCD
#define LCD_RS    32    // Output RS to LCD
#define LCD_D4    25    // Output D4 to LCD
#define LCD_D5    26    // Output D4 to LCD
#define LCD_D6    27    // Output D4 to LCD
#define LCD_D7    14    // Output D4 to LCD

LiquidCrystal llcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
BlynkTimer timer;
WidgetLCD lcd(V1);


void sendSeconds() {
  
  //lcd.clear();
  lcd.print(0, 0 , "Line 1");
  lcd.print(10, 0, digitalRead(LCD_BTN));
  lcd.print(0, 1, "Line 2");
  
  
}



void setup()
{
  // Debug console
  Serial.begin(115200);
  
  pinMode(PIN_EN4V, OUTPUT);
  pinMode(LCD_BTN, INPUT);
  pinMode(LCD_CRT, OUTPUT);
  digitalWrite(LCD_CRT, HIGH);

  llcd.begin(16,2);
  Serial.print("Lora Gateway, Start..\r\n");
  Blynk.begin(auth, ssid, pass);

  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSeconds);
  
}

unsigned long To0 = 0;
unsigned char state = 0;
uint32_t To1 = 0;

void loop()
{
  Blynk.run();
  timer.run();

  if (millis() - To0 >= 2000)
  {
    if (state == 0) 
    {
      state = 1;
      digitalWrite(PIN_EN4V, HIGH);

      llcd.clear();
      llcd.setCursor(0, 0);
      llcd.print( "Hoang Loi");
      
      Serial.print("Hello world\r\n");
      }
    else
    {
      state = 0;
      digitalWrite(PIN_EN4V, LOW);
      Serial.print("Hello Loi\r\n");
      }
    To0 = millis();
  }

  if (millis() - To1 >= 500)
  {
    Serial.print("btn stt: ");
    Serial.print(digitalRead(LCD_BTN));
    Serial.println();
    To1 = millis();
    }
}
