/*
 * Copyright (c) 2014, Intel Corporation.
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

#include "commandparser.h"
#include "hw-adaptation.h"

CommandParser *
CommandParser::list = NULL;

CommandParser::CommandParser(void) {
    next = list;
    list = this;
}

MessageChar *
CommandParser::parseCommands(Message *message) {
    const MessageChar *payload = message->getPayload();
    CharsCounter payload_len = message->getPayloadLen();
    MessageChar *reply = new MessageChar[120];
    reply[0] = 2;
    reply[1] = message->getInputMessageId();
    while (payload_len > 0) {
        CommandParser *cursor;
        for (cursor = list; cursor; cursor = cursor->next) {
            if (cursor->matchesCommandID(getCommandID(payload))) {
                cursor->parseCommand(payload);
                break;
            }
        }
        payload_len -= *payload;
        payload += *payload;
    }
    return reply;
}
