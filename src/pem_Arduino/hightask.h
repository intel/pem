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

#ifndef HIGH_PRIORITY_TASK_H
#define HIGH_PRIORITY_TASK_H
#include "task.h"

class HighPriorityTask: public Task{
    private:
        virtual inline bool execute(void) final {
            highPriorityExecute();
        }
    protected:
        virtual bool highPriorityExecute(void) = 0;
    public:
        HighPriorityTask(void);
};
#endif
