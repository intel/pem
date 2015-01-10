/*
 * Copyright (c) 2014, 2015, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef HW_ADAPTATION_H
#define HW_ADAPTATION_H

#if defined(ARDUINO_ARCH_SAM)
    #include <cstddef>
    #include <climits>
    #include <Arduino.h>

typedef enum{
    SYSTEM_TIC_100kHz = 0, SYSTEM_TIC_10kHz,
    SYSTEM_TIC_1kHz,       SYSTEM_TIC_100Hz,
    SYSTEM_TIC_VALUES = 4,
    SYSTEM_TIC = SYSTEM_TIC_100kHz  // Change this to modify TIC frequency
} SystemTicPreloadValues;


#elif defined(ARDUINO_ARCH_AVR)
    #include <stdlib.h>
    #include <limits.h>
    #include <Arduino.h>
    #include <SoftwareSerial.h>

typedef enum{
    TCNT1_100kHz = 65516, TCNT1_10kHz = 65328,
    TCNT1_1kHz   = 63456, TCNT1_100Hz = 44736, TCNT1_VALUES = 4
} TCNT1_PreloadValues;

typedef enum{
    SYSTEM_TIC_100Hz = TCNT1_100Hz, SYSTEM_TIC_1kHz   = TCNT1_1kHz,
    SYSTEM_TIC_10kHz = TCNT1_10kHz, SYSTEM_TIC_100kHz = TCNT1_100kHz,
    SYSTEM_TIC_VALUES = 4,
    SYSTEM_TIC = SYSTEM_TIC_10kHz  // Change this to modify TIC frequency
} SystemTicPreloadValues;

#endif

typedef enum {
    PIN_00 =  0, PIN_01, PIN_02, PIN_03, PIN_04, PIN_05, PIN_06, PIN_07,
    PIN_08, PIN_09, PIN_10, PIN_11, PIN_12, PIN_13
} Pins;

#include "pinmapping.h"

typedef uint32_t Time;

typedef unsigned char MessageChar;
typedef unsigned char CharsCounter;

void configureSystemTic(void);
void reloadSystemTic(void);
void enableSystemTic(void);

void enableWatchdog(void);
void resetWatchdog(void);

class ControlSerialPort;
ControlSerialPort *getControlPort(void);

class KeyBoard;
KeyBoard *getKeyBoard(void);
#endif
