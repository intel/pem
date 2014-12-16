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

#ifndef MESSAGE_H
#define MESSAGE_H
#include "system_time.h"

typedef enum {
    GET_START_OF_SIGNATURE, GET_END_OF_SIGNATURE, GET_MSG_LEN,
    GET_MSG_ID, GET_PAYLOAD, GET_CHECKSUM
} ReadingStates;
typedef enum {
    IDLE, ACQUIRE, ACQUIRING, ACQUIRED, TIMED_OUT
} AcquisitionStates;


class Message{
    protected:
        static const   Time           DEFAULT_TIMEOUT_US   = 400000;
        static const   CharsCounter   MAX_PAYLOAD_LEN      =    120;
        static const   CharsCounter   MIN_MSG_LEN          =      5;
        static const   MessageChar    SIGNATURE[];

        virtual bool        charsAvailable(void) = 0;
        virtual MessageChar readChar(void) = 0;
        virtual bool        writeChar(const MessageChar c) = 0;
        virtual void        flush(void) = 0;

        Time               deadline;
        MessageChar        input_msg_id;
        MessageChar        *payload;
        CharsCounter       payload_len;
        Time               timeout;
        AcquisitionStates  acquisition_state;
        AcquisitionStates  (*handler)(void);

        CharsCounter msg_read_chars;
        CharsCounter msg_len;
        ReadingStates in_fsm;

        static MessageChar
        getChecksum(const MessageChar *buffer, CharsCounter len);

        static MessageChar msg_out;
        static bool ready;

        virtual void
        dispatch(void) = 0;

        virtual void
        pingController(void) = 0;

    public:
        Message(void);

        inline  void
        startAcquisition(Time timeout = DEFAULT_TIMEOUT_US) {
            this->timeout = timeout;
            acquisition_state = ACQUIRE;
        }

        AcquisitionStates
        acquire(void);

        inline void
        resetAcquisition(void) {
            acquisition_state = IDLE;
        }

        inline AcquisitionStates
        getAcquisitionState(void) {
            return acquisition_state;
        }

        inline MessageChar
        getInputMessageId(void) {
            return input_msg_id;
        }

        inline const MessageChar *
        getPayload(void) {
            return payload;
        }

        inline CharsCounter
        getPayloadLen(void) {
            return payload_len;
        }


        CharsCounter
        out(const MessageChar output_buffer[]);

        inline void
        rxStateMachine(void) {
            switch (getAcquisitionState()) {
                case ACQUIRE:
                case ACQUIRING:
                    acquire();
                    break;
                case ACQUIRED:
                    dispatch();
                case IDLE:
                    startAcquisition();
                    break;
                case TIMED_OUT:
                    pingController();
                    resetAcquisition();
                    break;
            }
        }
};
#endif
