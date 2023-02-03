#include "StatusLED.h"

void StatusLED::init(int pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
  
  this->off();
}

void StatusLED::on() {
  this->mode = LEDMode::ON;
  digitalWrite(this->pin, HIGH);
}

void StatusLED::off() {
  this->mode = LEDMode::OFF;
  digitalWrite(this->pin, LOW);
}

void StatusLED::blink(unsigned long high, unsigned long low) {
  this->start = millis();
  this->mode = LEDMode::BLINK_HIGH;
  this->high_time = high;
  this->low_time = low;

  digitalWrite(this->pin, HIGH);
}

void StatusLED::pulse(unsigned long high) {
  this->pulse(high, 0, 1);
}

void StatusLED::pulse(unsigned long high, unsigned long low, unsigned int count) {
  if (count == 0) {
    this->off();
    return;
  }

  this->start = millis();
  this->mode = LEDMode::PULSE_HIGH;
  this->high_time = high;
  this->low_time = low;
  this->count = count - 1;

  digitalWrite(this->pin, HIGH);
}

bool StatusLED::update_mode_if_needed(unsigned long current, unsigned long interval, LEDMode::LEDMode new_mode, int pin_value) {
  if (current - this->start >= interval) {
    this->mode = new_mode;
    this->start = current;

    digitalWrite(this->pin, pin_value);

    return true;
  }

  return false;
}

void StatusLED::refresh(unsigned long current) {
  if (this->mode == LEDMode::ON || this->mode == LEDMode::OFF) {
    return;
  }

  switch(this->mode) {
    case LEDMode::BLINK_HIGH:
      update_mode_if_needed(current, this->high_time, LEDMode::BLINK_LOW, LOW);
      break;

    case LEDMode::BLINK_LOW:
      update_mode_if_needed(current, this->low_time, LEDMode::BLINK_HIGH, HIGH);
      break;

    case LEDMode::PULSE_HIGH:
      if (this->count == 0) {
        update_mode_if_needed(current, this->high_time, LEDMode::OFF, LOW);
      } else {
        update_mode_if_needed(current, this->high_time, LEDMode::PULSE_LOW, LOW);
      }
      break;

    case LEDMode::PULSE_LOW:
      if (update_mode_if_needed(current, this->low_time, LEDMode::PULSE_HIGH, HIGH)) {
        this->count--;
      }
      break;
  }
}
