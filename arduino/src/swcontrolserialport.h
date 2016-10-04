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

#if defined(ARDUINO_ARCH_AVR) && !defined(SW_CONTROL_SERIAL_PORT_H)
#define SW_CONTROL_SERIAL_PORT_H
#include "controlserialport.h"
#include "hw-adaptation.h"
#if defined(ARDUINO_AVR_UNO)
    #include <SoftwareSerial.h>
#endif

 class SWControlSerialPort: public ControlSerialPort {
    protected:
        SoftwareSerial *sw_serial_port;

        inline  virtual bool
        charsAvailable(void) final {
            return sw_serial_port && sw_serial_port->available();
        };

        inline virtual MessageChar
        readChar(void) final {
            return sw_serial_port->read();
        };

        inline virtual bool
        writeChar(const MessageChar c) final {
            return sw_serial_port->write(c);
        };

        inline virtual void
        flush(void) final {
            sw_serial_port->flush();
        };

        inline virtual SerialSpeeds
        getSpeed(void) final {
            return BAUD_19200;
        }
    public:
        SWControlSerialPort(void);
};

#endif
