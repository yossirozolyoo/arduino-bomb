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

void StatusLED::update_mode_if_needed(unsigned long current, unsigned long interval, LEDMode::LEDMode new_mode, int pin_value) {
  if (current - this->start >= interval) {
    this->mode = new_mode;
    this->start = current;

    digitalWrite(this->pin, pin_value);
  }
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
  }
}
