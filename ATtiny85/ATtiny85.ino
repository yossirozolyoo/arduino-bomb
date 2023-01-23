#include "levels.h"
#include <SoftwareSerial.h>

#define ANALOG_MODE_SELECTOR_PIN   A3
#define MODE_READING_TIMEOUT       10000

void get_running_mode() {
  SoftwareSerial serial(3, 4);
  serial.begin(9600);
  serial.setTimeout(MODE_READING_TIMEOUT);

  char raw_mode = 0;

  // If readBytes succeeded raw_mode will be the message recieved. If 
  // failed (or timed-out), raw_mode will be 0.
  serial.readBytes(&raw_mode, sizeof(raw_mode));

  set_mode(raw_mode);
  serial.write(get_raw_mode());   // Will return 'F' for unrecognized mode
  serial.end();
}

void setup() {
  get_running_mode();
}

void loop() {
  run_level();
}
