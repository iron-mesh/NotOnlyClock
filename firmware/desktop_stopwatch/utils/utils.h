#ifndef UTILS_H
#define UTILS_H


#if (DEBUG == 1)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#pragma message "Debug is on"
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#pragma message "Debug is off"
#endif


#endif