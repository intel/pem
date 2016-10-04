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

#include "message.h"
#include "hw-adaptation.h"
#include "system_time.h"
#include "debug.h"

const MessageChar
Message::SIGNATURE[] = {0xAA, 0x55};

Message::Message(void):
    payload(new MessageChar[MAX_PAYLOAD_LEN])
{
    resetAcquisition();
}

MessageChar
Message::getChecksum(const MessageChar *buffer, CharsCounter len) {
    MessageChar checksum {0};
    for (CharsCounter i = 0; i < len; i++) {
        checksum += buffer[i];
    }
    return checksum;
}

AcquisitionStates
Message::acquire(void) {
    MessageChar msg_checksum;
    switch (getAcquisitionState()) {
        case IDLE:
        case ACQUIRED:
        case TIMED_OUT:
            return acquisition_state;
        case ACQUIRE:
            deadline = SystemTime::getTime(timeout);
            in_fsm = GET_START_OF_SIGNATURE;
            acquisition_state = ACQUIRING;
            break;
        case ACQUIRING:
            if (SystemTime::isExpired(deadline)) {
                acquisition_state = TIMED_OUT;
                return acquisition_state;
            }
            break;
    }
    if (charsAvailable()) {
        switch (in_fsm) {
            case GET_START_OF_SIGNATURE:
                if (readChar() == SIGNATURE[0]) {
                    in_fsm = GET_END_OF_SIGNATURE;
                }
                break;
            case GET_END_OF_SIGNATURE:
                if (readChar() == SIGNATURE[1]) {
                    in_fsm = GET_MSG_LEN;
                }
                break;
            case GET_MSG_LEN:
                msg_len = readChar();
                if ((msg_len >= MIN_MSG_LEN) && (msg_len <= MAX_PAYLOAD_LEN)) {
                    in_fsm = GET_MSG_ID;
                } else {
                    in_fsm = GET_START_OF_SIGNATURE;
                }
                break;
            case GET_MSG_ID:
                input_msg_id = readChar();
                msg_read_chars = 0;
                in_fsm = GET_PAYLOAD;
                break;
            case GET_PAYLOAD:
                if (msg_read_chars < msg_len - MIN_MSG_LEN) {
                    payload[msg_read_chars++] = readChar();
                } else {
                    in_fsm = GET_CHECKSUM;
                }
                break;
            case GET_CHECKSUM:
                msg_checksum =
                    (MessageChar)(getChecksum(SIGNATURE, 2) +
                                  getChecksum(&msg_len, 1) +
                                  getChecksum(&input_msg_id, 1) +
                                  getChecksum(payload,
                                              msg_len - MIN_MSG_LEN));
                if (readChar() == msg_checksum) {
                    payload_len = msg_len - MIN_MSG_LEN;
                    acquisition_state = ACQUIRED;
                    return acquisition_state;
                } else  {
                    in_fsm = GET_START_OF_SIGNATURE;
                }
        }
    }
    return acquisition_state;
}


CharsCounter
Message::out(const MessageChar output_buffer[]) {
    MessageChar msg_checksum {0};
    CharsCounter msg_len = output_buffer[0] + MIN_MSG_LEN - 2; // XXX
    writeChar(SIGNATURE[0]);
    writeChar(SIGNATURE[1]);
    writeChar(msg_len);
    msg_checksum = SIGNATURE[0] + SIGNATURE[1] + msg_len;
    for (int i = 1; i < output_buffer[0]; i++) {
        writeChar(output_buffer[i]);
        msg_checksum += output_buffer[i];
    }
    writeChar(msg_checksum);
    flush();
}
