#include "debugSerial.h"
#include "levels.h"
#include "screen.h"

#define SPI_MOSI        11
#define SPI_MISO        12
#define SPI_SCK         13
#define SPI_CS          9

#define SPI_MESSAGE     "CUT THE RED WIRE"
#define SPI_HALF_CYCLE  3

static void configure_spi_pins_as_master() {
  pinMode(SPI_MOSI, OUTPUT);
  pinMode(SPI_MISO, INPUT);
  pinMode(SPI_SCK, OUTPUT);
  pinMode(SPI_CS, OUTPUT);

  digitalWrite(SPI_SCK, LOW);
}

static bool spi_exchange_bit(bool bit) {
  // Here we assume SPI mode 0. At the start of every bit SCK is low, and we sample on the rising edge
  digitalWrite(SPI_MOSI, bit ? HIGH : LOW);
  delay(SPI_HALF_CYCLE);
  
  digitalWrite(SPI_SCK, HIGH);
  bool output = digitalRead(SPI_MISO) == HIGH;
  delay(SPI_HALF_CYCLE);
  
  digitalWrite(SPI_SCK, LOW);

  return output;
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

static void spi_exchange_message() {
  // TODO: check how much refresh_screen streches the clock
  // Send message
  for (size_t i = 0; i < sizeof(SPI_MESSAGE) - 1; i++) {
    spi_exchange_byte(SPI_MESSAGE[i]);
    refresh_screen();
  }
}

void spi_main() {
  DEBUG_PRINT("SPI");

  configure_spi_pins_as_master();

  // Wait for ATTiny85 to start slave logic.
  // TODO: Change this to status polling logic
  delay_and_refresh_screen(5000);

  while (!level_done()) {
    // Start transaction
    digitalWrite(SPI_CS, LOW);
    spi_exchange_message();

    // End transaction
    digitalWrite(SPI_CS, HIGH);
    delay_and_refresh_screen(1000);
  }

  if (mode != RunningModes::FAIL) {
    char raw_mode = get_raw_mode();
    char response;

    do {
      // Start switch level packet
      digitalWrite(SPI_CS, LOW);

      spi_exchange_byte(0x00);
      spi_exchange_byte(0x00);
      spi_exchange_byte(raw_mode);
      response = spi_exchange_byte(0x00);

      // End switch level packet
      digitalWrite(SPI_CS, HIGH);
    } while (response != raw_mode);
  }
}