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
#include "hw-adaptation.h"

// The include below is a hack to workaround a shortcoming of the IDE.
// The IDE doesn't acknowledge the includes, when they are in other files,
// unless they first show up in the main file.

#if defined(ARDUINO_AVR_UNO)
    #include <SoftwareSerial.h>
#endif

class Pem: public Core {
    protected:
        inline virtual void
        customInit(void) final {
            Debug::blue->turnOFF();
            Debug::onboard->turnOFF();
            ControlSerialPort * controlport;
            MessageProcessor * messageprocessor;
            KeyBoard * keyboard;
            MessageFifo * inputfifo;
            MessageCharFifo * replyfifo;

            controlport = getControlPort();
            messageprocessor = new MessageProcessor();
            keyboard = getKeyBoard();

            inputfifo = new MessageFifo(1, messageprocessor,
                                        (HighPriorityTask*)controlport);
            replyfifo = new MessageCharFifo(3, (LowPriorityTask*)controlport,
                                        messageprocessor);

            controlport->configure(inputfifo, replyfifo);
            messageprocessor->configure(inputfifo, replyfifo);

            ((HighPriorityTask*)controlport)->schedule();
        };
};

deploy(Pem);

/*
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int freeMemory() {
    int free_memory;

    if((int)__brkval == 0)
        free_memory = ((int)&free_memory) - ((int)&__bss_end);
    else
        free_memory = ((int)&free_memory) - ((int)__brkval);
    return free_memory;
}

int printFreeMemory(void) {
    int mem = freeMemory();
}
*/

