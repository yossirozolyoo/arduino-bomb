#include "levels.h"
#include "serialServer.h"

#define COUNTOF(arr)    (sizeof(arr) / sizeof(arr[0]))
#define TOTAL_TIME      90 * 60

static const int level_pins[] = {
  A0, A1, A2, A3
};

RunningModes::RunningModes mode = RunningModes::UART;
static uint32_t level_pins_last_state = 0;
unsigned long end_time = 0; // To be filled by main

void setup_levels() {
  for (size_t i = 0; i < COUNTOF(level_pins); i++) {
    pinMode(level_pins[i], INPUT);
    digitalWrite(level_pins[i], HIGH);
  }

  end_time = (millis() / 1000) + TOTAL_TIME;

  initServer();
}

static uint32_t get_level_pins() {
  uint32_t output = 0;

  for (size_t i = 0; i < COUNTOF(level_pins); i++) {
    if (digitalRead(level_pins[i]) == HIGH) {
      output |= 1 << i;
    }
  }

  return output;
}

void failiure_main() {
  for (size_t i = 0; i < StatusLEDs::NUM_LEDS; i++) {
    status_leds[i].off();
  }

  status_leds[StatusLEDs::RED].blink(100, 100);
  DEBUG_PRINT("FAIL");
  while (true) {
    refresh_screen();
    handleUserCommands();
  }
}

void done_main() {
  DEBUG_PRINT("DONE");

  for (size_t i = 0; i < StatusLEDs::NUM_LEDS; i++) {
    status_leds[i].off();
  }

  unsigned long start = millis();
  unsigned long current = start;
  while (true) {
    // Turn on leds
    for (size_t i = 0; i < StatusLEDs::NUM_LEDS; i++) {
      start = millis();
      status_leds[i].on();
      while ((current = millis()) < start + 100) {
        refresh_screen();
        handleUserCommands();
      }
    }

    // Turn off leds
    for (size_t i = 0; i < StatusLEDs::NUM_LEDS; i++) {
      start = millis();
      status_leds[i].off();
      while ((current = millis()) < start + 100) {
        refresh_screen();
        handleUserCommands();
      }
    }
  }
}

bool level_done() {
  // Refresh screen
  refresh_screen();

  // Check if time passed
  if (end_time < (millis() / 1000)) {
    mode = RunningModes::FAIL;
    return true;
  }

  if (handleUserCommands()) {
    return true;
  }

  // Check if user cut the correct wire
  uint32_t current_state = get_level_pins();
  if (current_state == level_pins_last_state) {
    // The user didn't cut any wire since the last check
    return false;
  }

  if ((current_state & ~(1 << (int)mode)) == level_pins_last_state) {
    // The user cut the correct wire since the last check
    mode = (RunningModes::RunningModes) ((int)mode + 1);
    level_pins_last_state = current_state;

    // Present an animation to the user based on the LEDs
    for (size_t i = 0; i < StatusLEDs::NUM_LEDS; i++) {
      status_leds[i].pulse(100, 200, 3);
    }

    return true;
  }

  // The user cut the wrong wire since the last check
  mode = RunningModes::FAIL;
  return true;
}

char get_raw_mode() {
  return RAW_MODES[(int)mode];
}
