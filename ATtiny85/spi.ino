#include "debugSerial.h"
#include "levels.h"

#define SPI_MOSI        0
#define SPI_MISO        1
#define SPI_SCK         2
#define SPI_CS          3

#define SPI_MESSAGE     "CUT THE RED WIRE"

static void configure_spi_pins_as_slave() {
  pinMode(SPI_MOSI, INPUT);
  pinMode(SPI_MISO, OUTPUT);
  pinMode(SPI_SCK, INPUT);
  pinMode(SPI_CS, INPUT);
}

static void wait_for_pin(int pin, int value) {
  if (pin == SPI_CS) {
    while (digitalRead(pin) != value);
  } else {
    while ((digitalRead(pin) != value) && (digitalRead(SPI_CS) == LOW));
  }
}

static bool spi_exchange_bit(bool bit) {
  // Here we assume SPI mode 0. At the start of every bit SCK is low, and we sample on the rising edge
  // Wait for bit start:
  wait_for_pin(SPI_SCK, LOW);
  digitalWrite(SPI_MISO, bit ? HIGH : LOW);

  // Wait for sample:
  wait_for_pin(SPI_SCK, HIGH);
  return digitalRead(SPI_MOSI) == HIGH;
}

static uint8_t spi_exchange_byte(uint8_t byte) {
  // Here we assume LSB is sent first
  uint8_t output = 0;
  for (size_t i = 0; i < 8; i++) {
    uint8_t bit_mask = 1 << i;
    if (spi_exchange_bit((byte & bit_mask) != 0)) {
      output |= bit_mask;
    }
  }

  return output;
}

static bool spi_exchange_message() {
  // Check for switch-level packet
  if (spi_exchange_byte(SPI_MESSAGE[0]) == 0) {
    if (spi_exchange_byte(0) == 0) {
      set_mode(spi_exchange_byte(0));
      spi_exchange_byte(get_raw_mode());

      if (mode != RunningModes::FAIL) {
        return false;
      }
    }

    return true;
  }

  for (size_t i = 1; i < sizeof(SPI_MESSAGE); i++) {
    spi_exchange_byte(SPI_MESSAGE[i]);
  }

  return true;
}

void spi_main() {
  DEBUG_INIT();
  DEBUG_PRINT("SPI");

  configure_spi_pins_as_slave();

  do {
    DEBUG_PUT(".");

    // Wait for last transaction to end
    wait_for_pin(SPI_CS, HIGH);

    // Wait for cs to go down
    wait_for_pin(SPI_CS, LOW);
  } while(spi_exchange_message());
}