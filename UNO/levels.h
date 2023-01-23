#pragma once

namespace RunningModes {
  enum RunningModes {
    UART,
    SPI,
    I2C,
    DONE,
    FAIL
  };
};

#define RAW_MODES             "USIDF"
#define MODE_READING_TIMEOUT       10000

extern RunningModes::RunningModes mode;
extern unsigned long end_time;

void uart_main();
void i2c_main();
void spi_main();
void done_main();
void failiure_main();

// This function internally switches `mode` if the user cut the correct wire
bool level_done();
void setup_levels();

char get_raw_mode();