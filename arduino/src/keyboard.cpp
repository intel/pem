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

#include "keyboard.h"
#include "keyboardfifo.h"
#include "debug.h"
#include "hw-adaptation.h"

enum {CMD_KEY_DATA = 1};

const MessageChar
KeyBoard::command_ids_list[] = {CMD_KEY_DATA};

const CharsCounter
KeyBoard::command_ids_number = sizeof(command_ids_list) / sizeof (MessageChar);

MessageChar *
KeyBoard::parseCommand(const MessageChar *command) {
    switch (command[1]) {
        case CMD_KEY_DATA: {
            MessageChar *buf;
            if (outputfifo && (buf = new MessageChar[command[0]])) {
                for (unsigned short i = 0; i < command[0]; i++)
                    buf[i] = command[i];
                outputfifo->push(buf);
                return NULL;
            }
            return NULL;
        }
        default:
            return NULL;
    }
}
