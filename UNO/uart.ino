#include "debugSerial.h"
#include "levels.h"

#include <SoftwareSerial.h>

#define UART_MESSAGE     "CUT THE RED WIRE"

void uart_main() {
  DEBUG_PRINT("UART");

  SoftwareSerial serial(11, 12);
  serial.begin(9600);

  while (!level_done()) {
    serial.write(UART_MESSAGE);
    delay_and_refresh_screen(1000);
  }

  // Clear recieved buffer
  while (serial.available()) {
    serial.read();
  }

  // Switch level on the ATtiny85
  if (mode != RunningModes::FAIL) {
    serial.setTimeout(MODE_READING_TIMEOUT);
    char raw_mode = get_raw_mode();
    char response = 0;
    do {
      // Send the switch level packet
      serial.write("\x00\x00", 2);
      serial.write(raw_mode);

      response = 'F';
      serial.readBytes(&response, sizeof(response));
    } while(response != raw_mode);
  }
}