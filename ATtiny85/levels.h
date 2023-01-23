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

extern RunningModes::RunningModes mode;
void set_mode(char raw);
char get_raw_mode();

void uart_main();
void i2c_main();
void spi_main();
void done_main();
void failiure_main();

void run_level();