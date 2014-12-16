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

#ifndef CONTROL_SERIAL_PORT_H
#define CONTROL_SERIAL_PORT_H
#include "hightask.h"
#include "lowtask.h""
#include "message.h"
#include "messagefifo.h"
#include "messagecharfifo.h"
#include "hw-adaptation.h"
#include "debug.h"

typedef enum {
    BAUD_9600 = 9600, BAUD_19200 = 19200, BAUD_38400 = 38400,
    BAUD_57600 = 57600, BAUD_115200 = 115200
} SerialSpeeds;

class ControlSerialPort: public Message,
                         public HighPriorityTask, public LowPriorityTask {
    private:
        virtual SerialSpeeds getSpeed(void) = 0;
        MessageFifo *rxfifo;
        MessageCharFifo *txfifo;
        bool ping;

        virtual inline void
        dispatch(void) final {
            if(rxfifo) {
                rxfifo->push(this);
            }
        }

        inline void
        tx_reply(void) {
            MessageChar *reply;
            if (txfifo && (reply = txfifo->pull())) {
                out(reply);
                delete[] reply;
            }
        }

        inline virtual void
        pingController(void) final {
            ping = true;
            ((LowPriorityTask*)this)->schedule();
        }

        inline void
        tx_ping(void) {
            const MessageChar ping[] = {2, 0};
            out(ping);
        }

    public:
        ControlSerialPort(void);

        virtual inline void
        configure(MessageFifo *rxfifo, MessageCharFifo *txfifo) final {
            this->rxfifo = rxfifo;
            this->txfifo = txfifo;
            // Calculate the period for the task, in us,
            // assuming worst case: 7 bits, no stop bits.
            HighPriorityTask::setPeriod(1000000 * 7 / getSpeed());
        }

        virtual inline bool
        highPriorityExecute(void) final {
            rxStateMachine();
            return false;
        }

        virtual inline bool
        lowPriorityExecute(void) final {
            Debug::onboard->blink();
            Serial.print(1000000 * 7 / getSpeed());
            if (ping) {
                ping = false;
                tx_ping();
            } else {
                tx_reply();
            }
            return false;
        }
};
#endif
