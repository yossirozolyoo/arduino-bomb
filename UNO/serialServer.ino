#include "serialServer.h"
#include "screen.h"

#define SERIAL_TIMEOUT      1000
#define COUNTOF(arr)    (sizeof(arr) / sizeof(arr[0]))


namespace Commands {
  enum Commands {
    KEEP_ALIVE,
    GET_REMAINING_TIME,
    SET_REMAINING_TIME,
    GET_LEVEL,
    SET_LEVEL,
  };
};

namespace Responses {
  enum Responses {
    ACK = 0x70,
    NACK
  };
}

static size_t readBytes(uint8_t *read_buffer, size_t buffer_size) {
  unsigned long read_end = millis() + SERIAL_TIMEOUT;
  size_t left = buffer_size;
  do {
    while (left && Serial.available()) {
      *read_buffer++ = Serial.read();
      left--;
    }

    refresh_screen();
  } while (left && (millis() < read_end));

  return buffer_size - left;
}

template<typename T>
bool read(T &value) {
  return sizeof(value) == readBytes((uint8_t *)&value, sizeof(value));
}

template<typename T>
void write(const T &value) {
  for (size_t i = 0; i < sizeof(value); i++) {
    Serial.write(((uint8_t *)&value) + i, 1);
    refresh_screen();
  }
}

static void ack() {
  Serial.write((uint8_t) Responses::ACK);
}

static void nack() {
  Serial.write((uint8_t) Responses::NACK);
}

typedef void (*command)();

static void keep_alive() {
  ack();
}

static void get_remaining_time() {
  ack();
  write((uint32_t) (end_time - (millis() / 1000)));
}

static void set_remaining_time() {
  unsigned long remaining = 0;

  if (read(remaining)) {
    ack();
    end_time = (millis() / 1000) + remaining;
  } else {
    nack();
  }
}

static void get_level() {
  ack();
  write(get_raw_mode());
}

static void set_level() {
  char raw_mode = 0;

  if (read(raw_mode)) {
    ack();
    set_raw_mode(raw_mode);
  } else {
    nack();
  }
}

static command command_handlers[] = {
  keep_alive,
  get_remaining_time,
  set_remaining_time,
  get_level,
  set_level
};

void initServer() {
  Serial.begin(115200);
  ack();
}

bool handleUserCommands() {
  if (!Serial.available()) {
    // No commands available
    return false;
  }

  Commands::Commands opcode = (Commands::Commands) (Serial.read());
  if (opcode >= COUNTOF(command_handlers)) {
    nack();
  }

  command_handlers[opcode]();

  return false;
}