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

#include "core.h"
#include "system_time.h"
#include "watchdog.h"
#include "debug.h"

Core::Core(void) {
    SystemTime::init();
    Debug::init();
    Task::initInfraStructure();
    WatchDog * watchdog {new WatchDog()};
    watchdog->configure();
    watchdog->schedule();
}


