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

#ifndef SYSTEM_TIME_H
#define SYSTEM_TIME_H
#include "hw-adaptation.h"

#define HALF_TIME ((((Time)0) - 1) / 2)

class SystemTime {
    protected:
        volatile static Time
        current_time;
    public:
        inline static void
        init(void) {
            current_time = 0;
        }

        inline static void
        bump(unsigned int tics = 1) {
            current_time += tics *
                            ((SYSTEM_TIC == SYSTEM_TIC_100kHz) ?    10 :
                             (SYSTEM_TIC == SYSTEM_TIC_10kHz)  ?   100 :
                             (SYSTEM_TIC == SYSTEM_TIC_1kHz)   ?  1000 :
                             (SYSTEM_TIC == SYSTEM_TIC_100Hz)  ? 10000 :
                             0); // Intentionally b0rk the system if the TIC
                                // frequency is not recognised.
        }

        inline static Time
        getTime(Time offset = 0) {
            return current_time + offset;
        }

        inline static bool
        ordered(Time t1, Time t2) {
            return (t2 - t1) < HALF_TIME;
        }

        inline static bool
        isExpired(Time deadline) {
            return ordered(deadline, current_time);
        }

};

#endif
