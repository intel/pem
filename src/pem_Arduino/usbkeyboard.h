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

#if defined(ARDUINO_ARCH_SAM) && !defined(USB_KEYBOARD_H)
#define USB_KEYBOARD_H

#include "hw-adaptation.h"
#include <Arduino.h>
#include "keyboard.h"
#include "debug.h"



class USBKeyBoard: public KeyBoard{
    protected:
        virtual inline void
        write(const MessageChar *message, CharsCounter size) final {
            for (CharsCounter i = 0; i < size; i++)
                Keyboard.write(*(message + i));
        }


    public:
        USBKeyBoard();
};

#endif
