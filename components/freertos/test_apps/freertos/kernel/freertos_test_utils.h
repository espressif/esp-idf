/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "FreeRTOS.h"

#if ( configNUM_CORES > 1 )

    /**
     * @brief Prototype for test function.
     *
     * A test function can be passed to vTestOnAllCores() which will run the test function from a task on each core.
     */
    typedef void (* TestFunction_t)( void * );

    /**
     * @brief Run a test function on each core
     *
     * This function will internally create a task pinned to each core, where each task will call the provided test
     * function. This function will block until all cores finish executing the test function.
     *
     * @param pxTestCode Test function
     * @param pvTestCodeArg Argument provided to test function
     * @param ulStackDepth Stack depth of the created tasks
     * @param uxPriority Priority of the created tasks
     */
    void vTestOnAllCores( TestFunction_t pxTestCode, void * pvTestCodeArg, uint32_t ulStackDepth, UBaseType_t uxPriority );

#endif /* ( configNUM_CORES > 1 ) */
