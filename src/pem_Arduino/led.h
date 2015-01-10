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

#ifndef LED_H
#define LED_H
#include "system_time.h"
#include "hw-adaptation.h"


typedef enum {LED_OFF = LOW, LED_ON = HIGH} LEDStatus;

typedef enum {ONBOARD = PIN_ONBOARD_LED} OnboardLEDPin;

typedef enum {LED_TIMED, LED_SCALED, LED_MANUAL} LEDControlModes;

class LED {
    private:
        LEDStatus status;
        int ledpin;
        unsigned long long scaler;
        unsigned long long counter;
        Time deadline;
        Time period;
        LEDControlModes mode;

        inline void
        on(void) {
            digitalWrite(ledpin, status = LED_ON);
        };

        inline void
        off(void) {
            digitalWrite(ledpin, status = LED_OFF);
        };

        inline void
        toggle(void) {
            if (status == LED_ON) {
                off();
            } else {
                on();
            }
        };

    public:
        inline
        LED(int ledpin = ONBOARD) {
            this->ledpin = ledpin;
            pinMode(ledpin, OUTPUT);
            mode = LED_MANUAL;
            off();
        };

        inline void
        turnON(void) {
            mode = LED_MANUAL;
            on();
        };

        inline void
        turnOFF() {
            mode = LED_MANUAL;
            off();
        };

        inline void
        setPeriod(Time period) {
            mode = LED_TIMED;
            this->period = period;
            deadline = SystemTime::getTime(period);
        }

        inline void
        setScaler(unsigned long long scaler) {
            mode = LED_SCALED;
            this->scaler = scaler;
        }

        inline LEDStatus
        getStatus(void) {
            return status;
        };

        inline void blink(void) {
            switch (mode) {
                case LED_TIMED:
                    if (SystemTime::isExpired(deadline)) {
                        deadline += period;
                        toggle();
                    }
                    break;
                case LED_SCALED:
                    if (counter++ == scaler) {
                        counter = 0;
                        toggle();
                    }
                    break;
                case LED_MANUAL:
                    toggle();
                    break;
                default: while (1);
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
