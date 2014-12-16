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
#include "controlserialport.h"
#include "serkeyboard.h"
#include "messageprocessor.h"
#include "messagefifo.h"
#include "system_time.h"
#include "watchdog.h"
#include "debug.h"
#include "hw-adaptation.h"

Core::Core(void) {
    SystemTime::init();
    Debug::init();
    Debug::blue->turnOFF();
    Debug::onboard->turnOFF();
    Task::initInfraStructure();

    ControlSerialPort * controlport;
    MessageProcessor * messageprocessor;
    KeyBoard * keyboard;
    WatchDog * watchdog;
    MessageFifo * inputfifo;
    MessageCharFifo * replyfifo;

    controlport = getControlPort();
    messageprocessor = new MessageProcessor();
    keyboard = getKeyBoard();
    watchdog = new WatchDog();

    inputfifo = new MessageFifo(1, messageprocessor,
                                (HighPriorityTask*)controlport);
    replyfifo = new MessageCharFifo(3, (LowPriorityTask*)controlport,
                                messageprocessor);

    controlport->configure(inputfifo, replyfifo);
    messageprocessor->configure(inputfifo, replyfifo);
    watchdog->configure();

    ((HighPriorityTask*)controlport)->schedule();
    watchdog->schedule();

    Task::start();
}
