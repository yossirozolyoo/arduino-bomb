#include <string.h>

RunningModes::RunningModes mode = RunningModes::FAIL;

typedef void (*level_prototype)();
static const level_prototype levels[] = {
  uart_main,
  spi_main,
  i2c_main,
  done_main,
  failiure_main
};

void run_level() {
  levels[(int)mode]();
}

void failiure_main() {
  DEBUG_INIT();
  DEBUG_PRINT("FAIL");
  DEBUG_END();

  while (true);
}

void done_main() {
  DEBUG_INIT();
  DEBUG_PRINT("DONE");
  DEBUG_END();

  while (true);
}

void set_mode(char raw) {
  if (raw == 0) {
    mode = RunningModes::FAIL;
  }

  // Although it is not the case for any known implementation of the C standard,
  // no C standard states that it is mandatory that the same pointer will return
  // when referencing the same string. Since we assume that here, we must hold 
  // pointer in a variable
  const char *raw_modes = RAW_MODES;
  const char *found_location = strchr(raw_modes, raw);
  if (found_location == NULL) {
    mode = RunningModes::FAIL;
  }

  mode = (RunningModes::RunningModes) (found_location - raw_modes);
}

char get_raw_mode() {
  return RAW_MODES[(int)mode];
}