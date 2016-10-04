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

#ifndef CORE_H
#define CORE_H
#include "task.h"

#define deploy(ClassName)\
ClassName *main_class;\
\
void setup() {\
    main_class = new ClassName();\
    main_class->setup();\
}\
\
void loop(void) {\
    main_class->loop();\
}

class Core {
    protected:
        virtual void
        customInit(void) = 0;

    public:
        Core(void);

        inline void
        setup(void) {
            customInit();
            Task::start();
        }

        inline void
        loop(void) {
            Task::processRegularQueues();
        }
};
#endif
