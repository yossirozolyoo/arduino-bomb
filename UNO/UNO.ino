#include "levels.h"
#include "debugSerial.h"
#include "screen.h"
#include <SoftwareSerial.h>

#define ATTINY85_RESET_PIN         10

static bool set_running_mode() {
  SoftwareSerial serial(8, 9);
  serial.begin(9600);

  // Send mode over serial
  char raw_mode = RAW_MODES[(int)mode];
  serial.write(raw_mode);

  // Recieve ACK
  char recieved_mode = 0;
  serial.setTimeout(MODE_READING_TIMEOUT);
  
  // If readBytes succeeded, recieved_mode will contain the message recieved. If failed,
  // it will contain 0.
  serial.readBytes(&recieved_mode, sizeof(recieved_mode));
  serial.end();

  if (recieved_mode != raw_mode) {
    return false;
  }

  return true;
}

void setup() {
  setup_levels();

  setup_screen();

  // Reset the ATtiny85
  pinMode(ATTINY85_RESET_PIN, OUTPUT);
  digitalWrite(ATTINY85_RESET_PIN, LOW);

  digitalWrite(ATTINY85_RESET_PIN, HIGH);
  delay(100);

  // Set the running mode
  if (!set_running_mode()) {
    mode = RunningModes::FAIL;
    return;
  }

  // Init debug module
  DEBUG_INIT();
  status_leds[StatusLEDs::GREEN].blink(100, 1900);
}

void loop() {
  // Every mode's main calls to `level_done`, which switches `mode` to the 
  // next mode if the user cut the requested wire
  switch(mode) {
    case RunningModes::UART:
      uart_main();
      break;

    case RunningModes::I2C:
      i2c_main();
      break;

    case RunningModes::SPI:
      spi_main();
      break;

    case RunningModes::DONE:
      done_main();
      break;
    
    default:
      failiure_main();
      break;
  }
}
