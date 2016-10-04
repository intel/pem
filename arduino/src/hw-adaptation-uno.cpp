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

#if defined(ARDUINO_ARCH_AVR)
#include "task.h"
#include "hw-adaptation.h"
#include "debug.h"
#include "avr/wdt.h"
#include "swcontrolserialport.h"
#include "serkeyboard.h"

void configureSystemTic(void) {
    TCCR1A = 0;
    TCCR1B = (1 << CS11);     // 8 prescaler
}

void enableSystemTic(void) {
    TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
}

void reloadSystemTic(void) {
    TCNT1 = SYSTEM_TIC;
}

ISR(TIMER1_OVF_vect)        // interrupt service routine for System TIC
{
    Task::timerHandler();
}

void enableWatchdog(void) {
    wdt_enable(WDTO_8S);
}

void resetWatchdog(void) {
    wdt_reset();
}

ControlSerialPort *getControlPort(void) {
    return new SWControlSerialPort();
}

KeyBoard *getKeyBoard(void) {
    return new SerKeyBoard();
}
#endif
