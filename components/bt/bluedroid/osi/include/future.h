/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef __FUTURE_H__
#define __FUTURE_H__
// #pragma once

#include "osi_arch.h"

struct future {
    bool ready_can_be_called;
    osi_sem_t semaphore; // NULL semaphore means immediate future
    void *result;
};
typedef struct future future_t;

#define FUTURE_SUCCESS ((void *)1)
#define FUTURE_FAIL ((void *)0)

// Constructs a new future_t object. Returns NULL on failure.
future_t *future_new(void);

// Constructs a new future_t object with an immediate |value|. No waiting will
// occur in the call to |future_await| because the value is already present.
// Returns NULL on failure.
future_t *future_new_immediate(void *value);

// Signals that the |future| is ready, passing |value| back to the context
// waiting for the result. Must only be called once for every future.
// |future| may not be NULL.
void future_ready(future_t *future, void *value);

// Waits for the |future| to be ready. Returns the value set in |future_ready|.
// Frees the future before return. |future| may not be NULL.
void *future_await(future_t *async_result);

#endif /* __FUTURE_H__ */
