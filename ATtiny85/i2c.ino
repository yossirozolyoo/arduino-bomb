#include "debugSerial.h"
#include "levels.h"

#include <TinyWireS.h>


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

  TinyWireS.begin(I2C_ADDRESS);
  TinyWireS.onRequest(i2c_request_handler);
  TinyWireS.onReceive(i2c_recieve_handler);

  while (true) {
    TinyWireS_stop_check();
  }
}