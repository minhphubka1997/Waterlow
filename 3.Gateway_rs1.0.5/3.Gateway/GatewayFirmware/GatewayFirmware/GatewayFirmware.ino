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
#include "string.h"
#include "HardwareSerial.h"
#include "lora.h"
#include "node.h"
#include "stdlib.h"
#include "sim.h"
#include "app.h"

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "EH-lfxfpsVLQbxcGVbBgK-34VNcNJA3J";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Hoang Thong";
char pass[] = "0977497022";



LiquidCrystal llcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
BlynkTimer timer;

WidgetLCD lcd(V1);
WidgetLCD lcd2(V2);

WidgetLED GWPumpLed(V8);
WidgetLED N1PumpLed(V9);


//node node1, node2;

float PH, Temp ;
int Distance, Humidity;

extern bool GWPumpNeedOn ;
extern uint8_t GWPumpState;
extern uint8_t N1PumpState;

#define RXD2 16
#define TXD2 17


void sendSeconds() {
//  Temp = random(0,100);
//  Distance = random(30, 200);
//  Humidity = random(10,70);
//  PH = random(0,14);
  
  Blynk.virtualWrite(V3, Temp);
  Blynk.virtualWrite(V4, Humidity);
  Blynk.virtualWrite(V5, Distance);
  Blynk.virtualWrite(V6, PH);

  if (GWPumpState > 0) GWPumpLed.on(); else GWPumpLed.off();
  if (N1PumpState > 0) N1PumpLed.on(); else N1PumpLed.off();
 
  if (nodeth_alive_count() > 0)
  {
    //lcd.clear();
    lcd.print(0, 0 , "Thermal:      °C");
    lcd.print(10, 0, (int)Temp);
    if (Temp >= 10)
    {
      lcd.print(12,0, ".");
      lcd.print(13,0, ((int)(Temp * 10))%10);
    }
    else
    {
      lcd.print(11,0, ".");
      lcd.print(12,0, ((int)(Temp * 10))%10);
    }
    
    
    lcd.print(0, 1, "Humidity:      %");
    lcd.print(10,1, (int)Humidity);
  }
  else
  {
    lcd.print(0,0, "Node1: no signal");
    lcd.print(0,1, "                ");
  }

  if (nodedp_alive_count() > 0)
  {
    lcd2.print(0,  0 , "Distance:     cm");
    lcd2.print(10, 0, (int)Distance);
    
    lcd2.print(0,  1, "PH:             ");
    lcd2.print(10, 1, (int)PH);
    if (PH >= 10)
    {
      lcd2.print(12, 1, ".");
      lcd2.print(13, 1, ((int)(PH*10))%10);
    }
    else
    {
      lcd2.print(11, 1, ".");
      lcd2.print(12, 1, ((int)(PH*10))%10);
    }
  }
  else
  {
    lcd2.print(0,0, "Node2: no signal");
    lcd2.print(0,1, "                ");
  }
}

void pump_process(void);

void setup()
{
  // Debug console
  Serial.begin(115200);
  //HSerial1.begin(19200);  // SIM interface
  sim_serial_init();
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);   // Lora Interface
  
  pinMode(PIN_EN4V, OUTPUT);
  pinMode(LCD_BTN, INPUT);
  pinMode(LCD_CRT, OUTPUT);
  pinMode(PUMP_CRT, OUTPUT);
  PUMP_OFF();
  digitalWrite(LCD_CRT, HIGH);

  llcd.begin(16,2);
  llcd.clear();
  llcd.setCursor(0, 0);
  llcd.print( "Connecting to..");
  llcd.setCursor(0,1);
  llcd.print(ssid);
  
  Serial.print("Lora Gateway, Start..\r\n");
  Blynk.begin(auth, ssid, pass);
  delay(200);
  
  llcd.clear();
  llcd.setCursor(0, 0);
  llcd.print( "Already online !");

  delay(500);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSeconds);
  sim_async_init();
}



void loop()
{
  Blynk.run();
  timer.run();

  lora_process();
  sim_process();
  hardlcd_process();
  pump_process();
}
//




//uint32_t tt2 = 0;
void pump_process(void)
{
  if (GWPumpNeedOn) 
  {
    PUMP_ON();
    GWPumpState = 1;
  }
  if (GWPumpNeedOn == 0)
  {
    PUMP_OFF();
    GWPumpState = 0;
  }
}
//


unsigned long To0 = 0;
void hardlcd_process(void)
{
  if (sim_is_initializing() == false)
  {
    if (millis() - To0 >= 1000)
    {
      llcd.clear();
      llcd.setCursor(0, 0);
      llcd.print("T:     S:   H:  ");
      llcd.setCursor(2,0);
      llcd.print(Temp,1);
      llcd.setCursor(14,0);
      llcd.print((int)Humidity);
      llcd.setCursor(9,0);
      llcd.print(sim_is_init_ok());
  
      llcd.setCursor(0, 1);
      llcd.print("D:        PH:   ");
      llcd.setCursor(2,1);
      llcd.print(Distance);
      llcd.setCursor(13,1);
      llcd.print(PH,1);
        
      To0 = millis();
    } 
  } else To0 = millis();
}


void Hlcd_Clear(void)
{
  llcd.clear();
}

void Hlcd_setCursor(int row, int col) // Minimum col is 1, minimum row is 1
{
  llcd.setCursor(col-1, row-1);
}

void Hlcd_print(char str[])
{
  llcd.print(str);
}

void Hlcd_1stLine(void)
{
  llcd.setCursor(0,0);
}

void Hlcd_2ndLine(void)
{
  llcd.setCursor(0,1);
}
