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

#if defined(ARDUINO_ARCH_SAM)
#include "task.h"
#include "hwcontrolserialport.h"
#include "usbkeyboard.h"
#include "hw-adaptation.h"
#include "debug.h"

void configureSystemTic(void) {
    // Turn on the timer clock in the power management controller
    pmc_set_writeprotect(false);       // disable write protection for pmc
    pmc_enable_periph_clk(ID_TC7);     // enable peripheral clock TC7

    // We want wavesel 01 with RC
    TC_Configure(
        TC2,                                   // TimeCounter 2
        1,                                     // Channel 1
        TC_CMR_WAVE |                          //
        TC_CMR_WAVSEL_UP_RC |                  //
        TC_CMR_TCCLKS_TIMER_CLOCK2             // 84 MHz / 8 = 10.5 MHz
    );
    TC_SetRC(TC2, 1, 105);                     // 10.5 MHz / 105 = 100 kHz
}

void enableSystemTic(void) {
    // enable timer interrupts on the timer
    TC2->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;   // Interrupt Enable Register
    TC2->TC_CHANNEL[1].TC_IDR = ~TC_IER_CPCS;  // Interrupt Disable Register

    // Enable the interrupt in the nested vector interrupt controller
    // TC4_IRQn where 4 is
    // the timer number * timer channels (3) + the channel number (=(1*3)+1)
    // for timer1 channel1
    NVIC_EnableIRQ(TC7_IRQn);
    TC_Start(TC2, 1);
}

void reloadSystemTic(void) {
    TC_GetStatus(TC2, 1);               // Clear interrupt status
    TC_SetRC(TC2, 1, 105);              // Rearm the interrupt
}

void TC7_Handler()         // interrupt service routine for System TIC
{
    Task::timerHandler();
}

void enableWatchdog(void) {
}

void resetWatchdog(void) {
}

ControlSerialPort *getControlPort(void) {
    return new HWControlSerialPort();
}

KeyBoard *getKeyBoard(void) {
    return new USBKeyBoard();
}
#endif
