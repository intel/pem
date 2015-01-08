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

#ifndef MESSAGE_PROCESSOR_H
#define MESSAGE_PROCESSOR_H
#include "hightask.h"
#include "messagefifo.h"
#include "messagecharfifo.h"
#include "commandparser.h"

class MessageProcessor: public HighPriorityTask {
   private:
        MessageFifo *inputfifo;
        MessageCharFifo *replyfifo;

        inline void
        processMessage(void){
            Message *message;
            MessageChar * reply;
            if (inputfifo && (message = inputfifo->pull())) {
                if (reply = CommandParser::parseCommands(message)) {
                    replyfifo->push(reply);
                }
            }
        };

        inline virtual bool
        highPriorityExecute(void) final {
            processMessage();
            return false;
        };

    public:
        MessageProcessor(void);

        virtual inline void
        configure(MessageFifo *inputfifo, MessageCharFifo *replyfifo) final {
            this->inputfifo = inputfifo;
            this->replyfifo = replyfifo;
        }
};
#endif
