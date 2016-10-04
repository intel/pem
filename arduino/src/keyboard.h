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

#ifndef KEYBOARD_H
#define KEYBOARD_h
#include "lowtask.h"
#include "commandparser.h"
#include "keyboardfifo.h"

class KeyBoard: public CommandParser, public LowPriorityTask  {
    protected:
        static const MessageChar command_ids_list[];
        static const CharsCounter command_ids_number;

        KeyboardFifo *outputfifo;

        virtual inline const MessageChar *
        getCommandIDsList(void) final {
            return command_ids_list;
        }

        virtual inline CharsCounter
        getCommandIDsNumber(void) final {
            return command_ids_number;
        }

        virtual MessageChar *
        parseCommand(const MessageChar *command) final;

        virtual void
        write(const MessageChar *message, CharsCounter size) = 0;

        inline void
        sendBuffer(void) {
            MessageChar *buf;
            if (outputfifo && (buf = outputfifo->pull())) {
                write(buf + 2, buf[0] - 2);
                delete[] buf;
            }
        }

        inline virtual bool
        lowPriorityExecute(void) final {
            sendBuffer();
            return false;
        }
    public:
        KeyBoard(void):
            outputfifo(new KeyboardFifo(5, this))
        {};

 };
#endif
