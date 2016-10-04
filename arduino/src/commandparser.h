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

#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "message.h"
#include "hw-adaptation.h"

class CommandParser {
    protected:
        static CommandParser *list;

        CommandParser *next;

        virtual inline const MessageChar *
        getCommandIDsList(void) = 0;

        virtual inline CharsCounter
        getCommandIDsNumber(void) = 0;

        static inline CharsCounter
        getCommandLen(const MessageChar *payload) {
            return *payload;
        }

        static inline MessageChar
        getCommandID(const MessageChar *payload) {
            return *(payload + 1);
        }

        static inline bool
        hasCommandData(const MessageChar *payload) {
            return getCommandLen(payload) > 2;
        }

        inline bool
        matchesCommandID(const MessageChar command_id) {
            for (CharsCounter i = 0; i <  getCommandIDsNumber(); i++) {
                if (getCommandIDsList()[i] == command_id) {
                    return true;
                }
            }
            return false;
        }

        static inline const MessageChar *
        getCommandData(const MessageChar *payload) {
            return hasCommandData(payload) ? (payload + 2) : NULL;
        }


        virtual MessageChar *
        parseCommand(const MessageChar *command) = 0;

    public:
        CommandParser(void):
            next(list)
        {
            list = this;
        };

        static MessageChar *
        parseCommands(Message *message);
};

#endif
