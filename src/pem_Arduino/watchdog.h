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

#ifndef WATCHDOG_H
#define WATCHDOG_H
#include "lowtask.h"

class WatchDog: public LowPriorityTask {
    private:
        virtual inline bool
        lowPriorityExecute(void) final {
            resetWatchdog();
            return false;
        }

    public:
        WatchDog(void);

        void configure(void) {
            setPeriod(7000000);
        }
};

#endif
