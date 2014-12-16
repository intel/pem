/*
 * Copyright (c) 2014, Intel Corporation.
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

#ifndef LED_H
#define LED_H
#include "hw-adaptation.h"

typedef enum {LED_OFF = LOW, LED_ON = HIGH} LEDStatus;
typedef enum {ONBOARD = PIN_13, RED = PIN_04, GREEN = PIN_05, BLUE = PIN_06} LEDPins;

class LED {
    private:
        LEDStatus status;
        LEDPins ledpin;
        unsigned long long counter;
    public:
        inline
        LED(LEDPins ledpin = ONBOARD) {
            this->ledpin = ledpin;
            pinMode(ledpin, OUTPUT);
            turnOFF();
            counter = 0;
        };

        inline void
        turnON(void) {
            digitalWrite(ledpin, status = LED_ON);
        };

        inline void
        turnOFF() {
            digitalWrite(ledpin, status = LED_OFF);
        };

        inline void
        toggle(void) {
            if (status == LED_ON) {
                turnOFF();
            } else {
                turnON();
            }
        };

        inline LEDStatus
        getStatus(void) {
            return status;
        };

        inline void blink(unsigned long long count) {
            if (counter++ == count) {
                counter = 0;
                toggle();
            }
        }

        inline void
        panic(void) {
            while(true) {
                volatile int a;
                toggle();
                for (int i = 0; i < INT_MAX; i++)
                    a = i; // Do something in the busy loop
            }
        }

};
#endif
