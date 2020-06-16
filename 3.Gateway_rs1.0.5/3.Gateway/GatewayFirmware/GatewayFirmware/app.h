#ifndef _APP_H_
#define _APP_H_

#define HAL_GetTick()   millis()

#define PIN_EN4V  04    // Output to on/off 4V power
#define LCD_BTN   39    // Input from button to control on/off LCD

#define LCD_CRT   13    // Output Pin to control on/off LCD
#define LCD_EN    33    // Output EN to LCD
#define LCD_RS    32    // Output RS to LCD
#define LCD_D4    25    // Output D4 to LCD
#define LCD_D5    26    // Output D4 to LCD
#define LCD_D6    27    // Output D4 to LCD
#define LCD_D7    14    // Output D4 to LCD

#define PUMP_CRT  22
#define PUMP_OFF()  digitalWrite(PUMP_CRT, HIGH)
#define PUMP_ON()   digitalWrite(PUMP_CRT, LOW)

#define RX1 18  // RX for SIM COM, UART1 of ESP32
#define TX1 19  // TX for SIM COM, UART1 of ESP32


#define OFF_4V()  digitalWrite(PIN_EN4V, HIGH);
#define ON_4V()  digitalWrite(PIN_EN4V, LOW);

void sim_send_data(char str[]);

#define LCD1602_1stLine   Hlcd_1stLine
#define LCD1602_2ndLine   Hlcd_2ndLine

#define LCD1602_print     Hlcd_print
#define LCD1602_clear     Hlcd_Clear

#define LCD1602_setCursor Hlcd_setCursor

void Hlcd_Clear(void);
void Hlcd_setCursor(int row, int col) ;
void Hlcd_print(char str[]);
void Hlcd_1stLine(void);
void Hlcd_2ndLine(void);

#endif
//
