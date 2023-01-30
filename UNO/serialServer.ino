#include "SerialServer.h"

namespace Commands {
  enum Commands {
    KEEP_ALIVE = 0x60,
    GET_REMAINING_TIME,
  };
};

namespace Responses {
  enum Responses {
    ACK = 0x70
  };
}

static void ack() {
  Serial.write((uint8_t) Responses::ACK);
}

void initServer() {
  Serial.begin(115200);
  ack();
}

static void writeDword(uint32_t value) {
  Serial.write((uint8_t *) &value, sizeof(value));
}

bool handleUserCommands() {
  if (!Serial.available()) {
    // No commands available
    return false;
  }

  Commands::Commands opcode = (Commands::Commands) Serial.read();
  switch (opcode) {
    case Commands::KEEP_ALIVE:
      ack();
      break;

    case Commands::GET_REMAINING_TIME:
      ack();
      writeDword((uint32_t) (end_time - (millis() / 1000)));
      break;
  }

  return false;
}