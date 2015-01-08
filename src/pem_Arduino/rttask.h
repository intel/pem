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

#ifndef RT_TASK_H
#define RT_TASK_H
#include "task.h"

class RealTimeTask: public Task{
    private:
        virtual inline bool execute(void) final {
            realTimeExecute();
        }
    protected:
        virtual bool realTimeExecute(void) = 0;
    public:
        RealTimeTask(void);
};

#endif
