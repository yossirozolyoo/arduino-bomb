#include "debugSerial.h"
#include "levels.h"

#include <SoftwareSerial.h>

#define UART_MESSAGE     "CUT THE RED WIRE"

static size_t readBytes(SoftwareSerial &serial, uint8_t *read_buffer, size_t buffer_size) {
  unsigned long read_end = millis() + serial.getTimeout();
  size_t left = buffer_size;
  do {
    while (left && serial.available()) {
      *read_buffer++ = serial.read();
      left--;
    }

    refresh_screen();
  } while (left && (millis() < read_end));

  return buffer_size - left;
}

void uart_main() {
  DEBUG_PRINT("UART");

  SoftwareSerial serial(11, 12);
  serial.begin(9600);

  while (!level_done(RunningModes::UART)) {
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
      readBytes(serial, (uint8_t *)&response, sizeof(response));
    } while(response != raw_mode);
  }
}