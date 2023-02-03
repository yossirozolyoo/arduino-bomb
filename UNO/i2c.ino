#include "debugSerial.h"
#include "levels.h"
#include "screen.h"

#include <BitBang_I2C.h>

#define I2C_ADDRESS      40
#define I2C_MESSAGE      "CUT THE RED WIRE"


void i2c_main() {
  DEBUG_PRINT("I2C");

  // Initialize the bitbang i2c library
  BBI2C bbi2c;
  bbi2c.bWire = 0; // use bit banging
  bbi2c.iSDA = 11; // SDA on GPIO pin 11
  bbi2c.iSCL = 13; // SCL on GPIO pin 13
  I2CInit(&bbi2c, 1000); // 1K Clock

  uint8_t recieve_buffer[sizeof(I2C_MESSAGE) - 1];
  while (!level_done(RunningModes::I2C)) {
    I2CWrite(&bbi2c, I2C_ADDRESS, I2C_MESSAGE, sizeof(I2C_MESSAGE) - 1);
    delay_and_refresh_screen(2000);
    // I2CRead(&bbi2c, I2C_ADDRESS, recieve_buffer, sizeof(recieve_buffer));
    // delay_and_refresh_screen(2000);
  }
}