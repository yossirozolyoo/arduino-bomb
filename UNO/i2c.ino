#include "debugSerial.h"
#include "levels.h"
#include "screen.h"

#include <BitBang_I2C.h>
#include <SoftwareSerial.h>

#define I2C_ADDRESS      40
#define I2C_MESSAGE      "CUT THE RED WIRE"


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


void i2c_main() {
  DEBUG_PRINT("I2C");

  // Initialize the bitbang i2c library
  BBI2C bbi2c;
  bbi2c.bWire = 0; // use bit banging
  bbi2c.iSDA = 11; // SDA on GPIO pin 11
  bbi2c.iSCL = 13; // SCL on GPIO pin 13
  I2CInit(&bbi2c, 1000); // 1K Clock

  SoftwareSerial serial(12, 9);
  serial.begin(9600);

  uint8_t recieve_buffer[sizeof(I2C_MESSAGE) - 1];
  while (!level_done(RunningModes::I2C)) {
    I2CWrite(&bbi2c, I2C_ADDRESS, I2C_MESSAGE, sizeof(I2C_MESSAGE) - 1);
    delay_and_refresh_screen(2000);
    // I2CRead(&bbi2c, I2C_ADDRESS, recieve_buffer, sizeof(recieve_buffer));
    // delay_and_refresh_screen(2000);
  }

  I2C

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