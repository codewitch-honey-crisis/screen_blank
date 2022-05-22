#include <Arduino.h>
#include <tft_io.hpp>
#include <st7789.hpp>
#include <gfx_cpp14.hpp>

#define LCD_WIDTH 135
#define LCD_HEIGHT 240
#define LCD_HOST    VSPI
#define PIN_NUM_MISO -1
#define PIN_NUM_MOSI 19
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5
#define PIN_NUM_DC   16
#define PIN_NUM_RST  23
#define PIN_NUM_BKL 4
#define LCD_ROTATION 0
#define LCD_WRITE_SPEED_PERCENT 400 // 40MHz
#define LCD_READ_SPEED_PERCENT 200 // 20MHz
using namespace arduino;
using namespace gfx;
using bus_t = tft_spi_ex<LCD_HOST,
                            PIN_NUM_CS,
                            PIN_NUM_MOSI,
                            PIN_NUM_MISO,
                            PIN_NUM_CLK,
                            SPI_MODE0,
// some devices have no SDA read capability, so no read whatsoever.
                            PIN_NUM_MISO<0
#ifdef OPTIMIZE_DMA
                            ,(LCD_WIDTH*LCD_HEIGHT)*2+8
#endif
                            >;
using lcd_t = st7789<LCD_WIDTH,
                        LCD_HEIGHT,
                        PIN_NUM_DC,
                        PIN_NUM_RST,
                        PIN_NUM_BKL,
                        bus_t,
                        LCD_ROTATION,
                        true,
                        LCD_WRITE_SPEED_PERCENT,
                        LCD_READ_SPEED_PERCENT>;


#define PIN_NUM_BLANK 0
#define BLANK_TIMEOUT_SECS 5
lcd_t lcd;
uint32_t blank_ts;
bool blanked;
void setup() {
  Serial.begin(115200);
  blank_ts = millis();
  blanked=false;
  pinMode(PIN_NUM_BLANK,INPUT_PULLUP);
  lcd.fill(lcd.bounds(),color<decltype(lcd)::pixel_type>::white);

}

void loop() {

 if(digitalRead(PIN_NUM_BLANK)==LOW) {
   if(blanked) {
    digitalWrite(PIN_NUM_BKL,HIGH);
    blank_ts = millis();
    Serial.println("wake");
    blanked = false;
   }
 } else if(millis()-blank_ts>1000*BLANK_TIMEOUT_SECS) {
   blank_ts=millis();
   if(!blanked) {
    digitalWrite(PIN_NUM_BKL,LOW);
    Serial.println("blank");
    blanked = true;
   }
 }
}