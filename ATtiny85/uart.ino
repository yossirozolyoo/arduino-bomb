#include "debugSerial.h"
#include "levels.h"

#include <SoftwareSerial.h>

#define UART_MESSAGE     "CUT THE RED WIRE"

void uart_main() {
  DEBUG_INIT();
  DEBUG_PRINT("UART");

  SoftwareSerial serial(1, 0);
  serial.begin(9600);

  while (true) {
    serial.write(UART_MESSAGE);
    delay(1000);

    // Check for level done
    while (serial.available()) {
      // Next level packet format: 00 00 LL
      // LL is the raw mode to switch to.
      if (serial.read() != 0) {
        continue;
      }
      // Wait for 2 more characters
      while (serial.available() < 2);
      if (serial.read() != 0) {
        continue;
      }

      set_mode(serial.read());
      if (mode != RunningModes::FAIL) {
        serial.write(get_raw_mode());
        return;
      }
    }
  }
}