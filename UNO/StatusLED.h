#pragma once

namespace LEDMode {
  enum LEDMode {
    ON,
    OFF,
    BLINK_HIGH,
    BLINK_LOW
  };
};

class StatusLED {
public:
  StatusLED() {}
  void init(int pin);
  void refresh(unsigned long current);

  void on();
  void off();
  void blink(unsigned long high, unsigned long low);

private:
  void update_mode_if_needed(unsigned long current, unsigned long interval, LEDMode::LEDMode new_mode, int pin_value);

  LEDMode::LEDMode mode;
  int pin;

  unsigned long start;
  unsigned long high_time;
  unsigned long low_time;
};