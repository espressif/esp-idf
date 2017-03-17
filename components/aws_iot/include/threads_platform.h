/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Additions Copyright 2016 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include "threads_interface.h"

#ifndef AWS_IOTSDK_THREADS_PLATFORM_H
#define AWS_IOTSDK_THREADS_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**
 * @brief Mutex Type
 *
 * definition of the Mutex   struct. Platform specific
 *
 */
struct _IoT_Mutex_t {
    SemaphoreHandle_t mutex;
};

#ifdef __cplusplus
}
#endif

#endif /* AWS_IOTSDK_THREADS_PLATFORM_H */


