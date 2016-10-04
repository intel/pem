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

#ifndef FIFO_H
#define FIFO_H
#include "task.h"

template <class Payload> class Fifo {
    protected:
        Payload *fifo;
        unsigned char head;
        unsigned char tail;
        unsigned char count;
        unsigned char max;
        Task *producer;
        Task *consumer;

        inline bool
        isFull(void) {
            return count == max;
        }

        inline bool
        isEmpty(void) {
            return count == 0;
        }

        inline void
        consume(void) {
            if (consumer) {
                consumer->schedule();
            }
        }

        inline void
        produce(void) {
            if (producer) {
                producer->schedule();
            }
        }

    public:
        Fifo(unsigned char size, Task *consumer = 0, Task *producer = 0);

        inline bool
        push(Payload item) {
            if (isFull()) {
                consume();
                return false;
            }
            fifo[tail] = item;
            tail = (tail + 1) % max;
            count++;
            consume();
            return true;
        }

        inline Payload
        pull(void) {
            if (isEmpty()) {
                produce();
                return (Payload)0;
            }
            unsigned char tmp = head;
            head = (head + 1) % max;
            count--;
            consume();
            return fifo[tmp];
        }
};

template <class Payload>
Fifo<Payload>::Fifo(unsigned char size, Task *consumer, Task *producer) {
    fifo = new Payload[size];
    this->producer = producer;
    this->consumer = consumer;
    head = tail = 0;
    count = 0;
    max = size;
}
#endif
