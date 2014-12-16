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

#if defined(ARDUINO_ARCH_SAM) && !defined(HW_CONTROL_SERIAL_PORT_H)
#define HW_CONTROL_SERIAL_PORT
#include "controlserialport.h"

class HWControlSerialPort: public ControlSerialPort {
    protected:
        inline  virtual bool
        charsAvailable(void) final {
            return Serial.available() > 0;
        };

        inline virtual MessageChar
        readChar(void) final {
            return Serial.read();
        };

        inline virtual bool
        writeChar(const MessageChar c) final {
            return Serial.write(c);
        };

        inline virtual void
        flush(void) final {
            Serial.flush();
        };

        inline virtual SerialSpeeds
        getSpeed(void) final {
            return BAUD_19200;
        }
    public:
        HWControlSerialPort(void);
};
#endif

