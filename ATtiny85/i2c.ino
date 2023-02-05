#include "debugSerial.h"
#include "levels.h"

#include <TinyWireS.h>
#include <SoftwareSerial.h>


#define I2C_ADDRESS      40

void i2c_request_handler() {
  TinyWireS.send('B');
}

void i2c_recieve_handler(uint8_t size) {
  while (TinyWireS.available()) {
    TinyWireS.receive();
  }
}


void i2c_main() {
  DEBUG_INIT();
  DEBUG_PRINT("I2C");

  SoftwareSerial serial(3, 1);
  serial.begin(9600);

  TinyWireS.begin(I2C_ADDRESS);
  TinyWireS.onRequest(i2c_request_handler);
  TinyWireS.onReceive(i2c_recieve_handler);

  while (true) {
    TinyWireS_stop_check();

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