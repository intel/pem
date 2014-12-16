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

#if defined (PC_DEBUG)
#include "hw-adaptation.h"
#include "debug.h"
#include <sys/time.h>

void rearmSystemTic(void) {
}

void configureSystemTic(void) {
}

void simulatedKeyboardInit(void) {}
void simulateKeyPress(MessageChar ch) {}
void simulateKeyRelease(MessageChar ch) {}

void initControlSerialPort(unsigned speed) {}
bool charsFromControlSerialPort(void) {return false;}
MessageChar getCharFromControlSerialPort(void) {return 0;}
bool writeCharToControlSerialPort(MessageChar ch) {}
void flushControlSerialPort(void) {}

Time millis(void) {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    // caculate milliseconds
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

int main(void) {
    setup();
    while(true) {
        char a;
        usleep(1);
        TC7_Handler();
    }
}
#endif
