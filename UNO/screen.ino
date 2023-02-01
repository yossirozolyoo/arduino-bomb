#include "levels.h"
#include "screen.h"
#include <U8glib.h>

#define COUNTOF(arr)    (sizeof(arr) / sizeof(arr[0]))
#define REFRESH_RATE        400
#define CLOCK_Y             30
#define PROTOCOL_Y          55

#define DONE_FAIL_Y         35

unsigned long last_refresh = 0;
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);	// Dev 0, Fast I2C / TWI
static int screenWidth = 0;

StatusLED status_leds[StatusLEDs::NUM_LEDS];

void setup_screen(void) {
  // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

  screenWidth = u8g.getWidth();
  
  status_leds[0].init(6);
  status_leds[1].init(5);
  status_leds[2].init(4);
  status_leds[3].init(3);
  status_leds[4].init(2);
}

void drawCenteredString(const char * str, int y) {
  int width = u8g.getStrWidth(str);

  u8g.drawStr((screenWidth - width) / 2, y, str);
}

void drawClock() {
  char output_buffer[10] = {};

  // Compute left time. Use last_refresh instead of millis, since this
  // function can be called many times during one refresh.
  unsigned long left;
  if (last_refresh / 1000 >= end_time) {
    left = 0;
  } else {
    left = end_time - (last_refresh / 1000);
  }

  // Create the clock string
  int minutes = left / 60;
  int hours = minutes / 60;

  if (left % 2 == 0) {
    sprintf(output_buffer, "%02d:%02d:%02d", hours, minutes % 60, left % 60);
  } else {
    sprintf(output_buffer, "%02d %02d %02d", hours, minutes % 60, left % 60);
  }

  // Draw the string
  u8g.setFont(u8g_font_10x20);
  drawCenteredString(output_buffer, CLOCK_Y);
}

void drawProtocol() {
  u8g.setFont(u8g_font_7x13);
  switch (mode) {
    case RunningModes::UART:
      drawCenteredString("UART", PROTOCOL_Y);
      break;

    case RunningModes::SPI:
      drawCenteredString("SPI", PROTOCOL_Y);
      break;

    case RunningModes::I2C:
      drawCenteredString("I2C", PROTOCOL_Y);
      break;      
  }
}

static void drawLevel() {
  drawClock();
  drawProtocol();
}

static void drawWin() {
  u8g.setFont(u8g_font_10x20);
  drawCenteredString("Bomb neutralized", DONE_FAIL_Y);
}

static void drawFail() {
  u8g.setFont(u8g_font_10x20);
  drawCenteredString("KABOOM", DONE_FAIL_Y);
}

static void draw() {
  switch (mode) {
    case RunningModes::UART:
    case RunningModes::SPI:
    case RunningModes::I2C:
      drawLevel();
      break;

    case RunningModes::DONE:
      drawWin();
      break;

    case RunningModes::FAIL:
      drawFail();
      break;
  }
}

static void refresh_leds(unsigned long current) {
  for (size_t i = 0; i < COUNTOF(status_leds); i++) {
    status_leds[i].refresh(current);
  }
}

void refresh_screen() {
  unsigned long current = millis();
  refresh_leds(current);

  // Check if refresh needed
  if ((current - last_refresh < REFRESH_RATE) && (last_refresh != 0)) {
    return;
  }

  last_refresh = current;

  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
}

void delay_and_refresh_screen(unsigned long ms) {
  unsigned long delay_end = millis() + ms;

  // TODO: handle overflow here
  while (millis() < delay_end) {
    refresh_screen();
  }
}