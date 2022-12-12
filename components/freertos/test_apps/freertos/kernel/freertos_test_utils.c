/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FreeRTOS.h"
#include "semphr.h"
#include "freertos_test_utils.h"

#if ( configNUM_CORES > 1 )

    typedef struct {
        const TestFunction_t pxTestCode;
        void * const pvTestCodeArg;
        const SemaphoreHandle_t xTaskDoneSem;
    } TestArgs_t;

    static void test_func_task( void * pvParameters )
    {
        TestArgs_t * pxTestArgs = ( TestArgs_t * ) pvParameters;
        /* Call the test function */
        pxTestArgs->pxTestCode( pxTestArgs->pvTestCodeArg );
        /* Indicate completion to the creation task and wait to be deleted. */
        xSemaphoreGive( pxTestArgs->xTaskDoneSem );
        vTaskSuspend( NULL );
    }

    void vTestOnAllCores( TestFunction_t pxTestCode, void * pvTestCodeArg, uint32_t ulStackDepth, UBaseType_t uxPriority )
    {
        SemaphoreHandle_t xTaskDoneSem = xSemaphoreCreateCounting( configNUM_CORES, 0 );
        TaskHandle_t xTaskHandles[ configNUM_CORES ];
        TestArgs_t xTestArgs = {
            .pxTestCode = pxTestCode,
            .pvTestCodeArg = pvTestCodeArg,
            .xTaskDoneSem = xTaskDoneSem,
        };

        /* Create a separate task on each core to run the test function */
        for( BaseType_t xCoreID = 0; xCoreID < configNUM_CORES; xCoreID++ ) {
            #if ( CONFIG_FREERTOS_SMP == 1 )
                xTaskCreateAffinitySet( test_func_task,
                                        "task",
                                        ulStackDepth,
                                        ( void * ) &xTestArgs,
                                        uxPriority,
                                        ( UBaseType_t ) ( 1 << xCoreID ),
                                        &( xTaskHandles[ xCoreID ] ) );
            #else
                xTaskCreatePinnedToCore( test_func_task,
                                         "task",
                                         ulStackDepth,
                                         ( void * ) &xTestArgs,
                                         uxPriority,
                                         &( xTaskHandles[ xCoreID ] ),
                                         xCoreID );
            #endif
        }

        /* Wait for each tasks to complete test */
        for( BaseType_t xCoreID = 0; xCoreID < configNUM_CORES; xCoreID++ ) {
            xSemaphoreTake( xTaskDoneSem, portMAX_DELAY );
        }

        /* Cleanup */
        for( BaseType_t xCoreID = 0; xCoreID < configNUM_CORES; xCoreID++ ) {
            vTaskDelete( xTaskHandles[ xCoreID ] );
        }
        vSemaphoreDelete( xTaskDoneSem );
    }

#endif /* ( configNUM_CORES > 1 ) */
