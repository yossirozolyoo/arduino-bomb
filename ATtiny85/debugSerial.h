#pragma once

#define DEBUG_SERIAL
#ifdef DEBUG_SERIAL

#include <SoftwareSerial.h>

#define DEBUG_INIT()      SoftwareSerial debugSerial(3, 4); debugSerial.begin(9600)
#define DEBUG_PRINT(x)    debugSerial.write(x "\r\n")
#define DEBUG_PUT(x)      debugSerial.write(x);
#define DEBUG_END()       debugSerial.end()

#else

#define DEBUG_INIT()
#define DEBUG_PRINT(x)
#define DEBUG_PUT(x)
#define DEBUG_END()

#endif