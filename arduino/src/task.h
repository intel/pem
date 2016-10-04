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

#ifndef TASK_H
#define TASK_H
#include "system_time.h"
#include "debug.h"

typedef enum {
    HELD, ONE_SHOT, PERIODIC, TASK_STATES
} TaskStatus;

typedef enum {
    RT_PRIORITY, HIGH_PRIORITY, LOW_PRIORITY, PRIORITY_TYPES
} TaskPriority;

typedef enum {
    NOW = 0, NEXT_PERIOD, SCHEDULING_OPTIONS
} SchedulingOptions;

typedef unsigned int TimeBase;

class Task {
    static Task *execution_queues[];
    protected:
        Time period_us;
        Time next_run_us;
        TaskStatus status;
        TaskPriority priority;
        Task *queue_next;
        bool enqueued;

        inline void
        hold(void) {
            status = HELD;
        }

        inline void
        release(void) {
            status = (period_us == 0) ? ONE_SHOT : PERIODIC;
        }

        inline bool
        isRunnable(void) {
            return ((status == ONE_SHOT) || (status == PERIODIC));
        }

        inline void
        setPeriod(Time period_us) {
            this->period_us = period_us;
            next_run_us = SystemTime::getTime();
            release();
        }

        inline bool
        readyForExecution(void) {
            return SystemTime::isExpired(next_run_us);
        }

        inline static bool
        executeNext(TaskPriority queue) {
            if (execution_queues[queue] == NULL) {
                return false;
            }
            if (execution_queues[queue]->readyForExecution()) {
                Task *task = execution_queues[queue];
                execution_queues[queue] = execution_queues[queue]->queue_next;
                task->queue_next = NULL;
                task->enqueued = false;
                if (task->isRunnable()) {
                    task->execute();
                }
                if (task->status == PERIODIC) {
                    task->schedule();
                }
                return true;
            }
            return false;
        }

        inline static void
        processRTQueue(void) {
            while (executeNext(RT_PRIORITY));
        }


    public:
        Task(void);

        inline void
        configureTask(TaskPriority priority, Time period_us = 0) {
            this->priority = priority;
            this->period_us = period_us;
            this->next_run_us = SystemTime::getTime();
            release();
            schedule();
        }

        virtual bool execute(void) = 0;

        inline bool
        schedule(SchedulingOptions when = NEXT_PERIOD) {
            if (enqueued) {
                return false;
            }
            switch (when) {
                case NOW:
                    next_run_us = SystemTime::getTime();
                    break;
                case NEXT_PERIOD:
                    next_run_us = SystemTime::getTime(period_us);
                    break;
            }
            Task **cursor = priority + (Task **)&execution_queues;
            while ((NULL != *cursor) &&
                    SystemTime::ordered((*cursor)->next_run_us,
                                        next_run_us)) {
                cursor = &((*cursor)->queue_next);
            }
            queue_next = (*cursor);
            (*cursor) = this;
            enqueued = true;
            return true;
        }

        inline static void
        initInfraStructure(void) {
            for (int i = 0; i < PRIORITY_TYPES; i++) {
                execution_queues[i] = NULL;
            }
            configureSystemTic();
        }

        inline static void
        start(void) {
            enableSystemTic();
        }

        inline static void
        processRegularQueues(void) {
            executeNext(HIGH_PRIORITY) || executeNext(LOW_PRIORITY);
        }

        inline static void
        timerHandler(void) {
            SystemTime::bump();
            reloadSystemTic();
            processRTQueue();
        }
};

#endif
