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

#ifndef DEBUG_H
#define DEBUG_H
#include "led.h"
#include "hw-adaptation.h"

#define blink_led(led, DIVIDER) { \
    static unsigned counter = 0; \
    if (counter++ == DIVIDER) { \
        counter = 0; \
        Debug::led->toggle(); \
    } \
}

class Debug {
    public:
        static LED *onboard;
        static LED *red;
        static LED *green;
        static LED *blue;

        static inline void
        init(void) {
            onboard = new LED(ONBOARD);
            red = new LED(RED);
            green = new LED(GREEN);
            blue = new LED(BLUE);
            onboard->turnOFF();
            red->turnOFF();
            green->turnOFF();
            blue->turnOFF();
        }
};
#endif
