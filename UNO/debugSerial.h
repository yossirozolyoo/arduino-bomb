#pragma once

// #define DEBUG_SERIAL
#ifdef DEBUG_SERIAL

#define DEBUG_INIT()      Serial.begin(9600)
#define DEBUG_PRINT(x)    Serial.write(x "\r\n")
#define DEBUG_PUT(x)      Serial.write(x);
#define DEBUG_HEX(x)      Serial.print(x, HEX)

#else

#define DEBUG_INIT()
#define DEBUG_PRINT(x)
#define DEBUG_PUT(x)
#define DEBUG_HEX(x)

#endif