/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/stream_buffer.h"
#include "freertos/message_buffer.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "freertos/idf_additions.h"
#include "esp_heap_caps.h"

/*
 * This file contains the implementation for some the functions in
 * idf_additions.h
 */

/* -----------------------------------------------------------------------------
 * Creation With Memory Caps
 * -------------------------------------------------------------------------- */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/* ---------------------------------- Queue --------------------------------- */

    QueueHandle_t xQueueCreateWithCaps( UBaseType_t uxQueueLength,
                                        UBaseType_t uxItemSize,
                                        UBaseType_t uxMemoryCaps )
    {
        QueueHandle_t xQueue;
        StaticQueue_t * pxQueueBuffer;
        uint8_t * pucQueueStorageBuffer;

        /* Allocate memory for the queue using the provided memory caps */
        pxQueueBuffer = heap_caps_malloc( sizeof( StaticQueue_t ), ( uint32_t ) uxMemoryCaps );

        if( uxItemSize == 0 )
        {
            pucQueueStorageBuffer = NULL;
        }
        else
        {
            pucQueueStorageBuffer = heap_caps_malloc( uxQueueLength * uxItemSize, ( uint32_t ) uxMemoryCaps );
        }

        if( ( pxQueueBuffer == NULL ) || ( ( uxItemSize > 0 ) && ( pucQueueStorageBuffer == NULL ) ) )
        {
            goto err;
        }

        /* Create the queue using static creation API */
        xQueue = xQueueCreateStatic( uxQueueLength, uxItemSize, pucQueueStorageBuffer, pxQueueBuffer );

        if( xQueue == NULL )
        {
            goto err;
        }

        return xQueue;

err:
        heap_caps_free( pucQueueStorageBuffer );
        heap_caps_free( pxQueueBuffer );
        return NULL;
    }

    void vQueueDeleteWithCaps( QueueHandle_t xQueue )
    {
        BaseType_t xResult;
        StaticQueue_t * pxQueueBuffer;
        uint8_t * pucQueueStorageBuffer;

        /* Retrieve the buffers used to create the queue before deleting it */
        xResult = xQueueGetStaticBuffers( xQueue, &pucQueueStorageBuffer, &pxQueueBuffer );
        configASSERT( xResult == pdTRUE );

        /* Delete the queue */
        vQueueDelete( xQueue );

        /* Free the memory buffers */
        heap_caps_free( pxQueueBuffer );
        heap_caps_free( pucQueueStorageBuffer );
    }

/* -------------------------------- Semaphore ------------------------------- */

    SemaphoreHandle_t xSemaphoreCreateGenericWithCaps( UBaseType_t uxMaxCount,
                                                       UBaseType_t uxInitialCount,
                                                       const uint8_t ucQueueType,
                                                       UBaseType_t uxMemoryCaps )
    {
        SemaphoreHandle_t xSemaphore;
        StaticSemaphore_t * pxSemaphoreBuffer;

        /* Allocate memory for the semaphore using the provided memory caps */
        pxSemaphoreBuffer = heap_caps_malloc( sizeof( StaticSemaphore_t ), ( uint32_t ) uxMemoryCaps );

        if( pxSemaphoreBuffer == NULL )
        {
            return NULL;
        }

        /* Create the semaphore using static creation API */
        if( ucQueueType == queueQUEUE_TYPE_MUTEX )
        {
            xSemaphore = xSemaphoreCreateMutexStatic( pxSemaphoreBuffer );
        }
        else if( ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE )
        {
            xSemaphore = xSemaphoreCreateCountingStatic( uxMaxCount, uxInitialCount, pxSemaphoreBuffer );
        }
        else if( ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE )
        {
            xSemaphore = xSemaphoreCreateBinaryStatic( pxSemaphoreBuffer );
        }
        else /* ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX */
        {
            xSemaphore = xSemaphoreCreateRecursiveMutexStatic( pxSemaphoreBuffer );
        }

        if( xSemaphore == NULL )
        {
            heap_caps_free( pxSemaphoreBuffer );
        }

        return xSemaphore;
    }

    void vSemaphoreDeleteWithCaps( SemaphoreHandle_t xSemaphore )
    {
        BaseType_t xResult;
        StaticSemaphore_t * pxSemaphoreBuffer;

        /* Retrieve the buffer used to create the semaphore before deleting it
         * */
        xResult = xSemaphoreGetStaticBuffer( xSemaphore, &pxSemaphoreBuffer );
        configASSERT( xResult == pdTRUE );

        /* Delete the semaphore */
        vSemaphoreDelete( xSemaphore );

        /* Free the memory buffer */
        heap_caps_free( pxSemaphoreBuffer );
    }

/* ------------------------- Stream & Message Buffers ----------------------- */

    StreamBufferHandle_t xStreamBufferGenericCreateWithCaps( size_t xBufferSizeBytes,
                                                             size_t xTriggerLevelBytes,
                                                             BaseType_t xIsMessageBuffer,
                                                             UBaseType_t uxMemoryCaps )
    {
        StreamBufferHandle_t xStreamBuffer;
        StaticStreamBuffer_t * pxStaticStreamBuffer;
        uint8_t * pucStreamBufferStorageArea;

        /* Allocate memory for the stream or message buffer using the provided
         * memory caps */
        pxStaticStreamBuffer = heap_caps_malloc( sizeof( StaticStreamBuffer_t ), ( uint32_t ) uxMemoryCaps );
        pucStreamBufferStorageArea = heap_caps_malloc( xBufferSizeBytes, ( uint32_t ) uxMemoryCaps );

        if( ( pxStaticStreamBuffer == NULL ) || ( pucStreamBufferStorageArea == NULL ) )
        {
            goto err;
        }

        /* Create the stream or message buffer using static creation API */
        if( xIsMessageBuffer == pdTRUE )
        {
            xStreamBuffer = ( StreamBufferHandle_t ) xMessageBufferCreateStatic( xBufferSizeBytes, pucStreamBufferStorageArea, pxStaticStreamBuffer );
        }
        else
        {
            xStreamBuffer = xStreamBufferCreateStatic( xBufferSizeBytes, xTriggerLevelBytes, pucStreamBufferStorageArea, pxStaticStreamBuffer );
        }

        if( xStreamBuffer == NULL )
        {
            goto err;
        }

        return xStreamBuffer;

err:
        heap_caps_free( pucStreamBufferStorageArea );
        heap_caps_free( pxStaticStreamBuffer );
        return NULL;
    }

    void vStreamBufferGenericDeleteWithCaps( StreamBufferHandle_t xStreamBuffer,
                                             BaseType_t xIsMessageBuffer )
    {
        BaseType_t xResult;
        StaticStreamBuffer_t * pxStaticStreamBuffer;
        uint8_t * pucStreamBufferStorageArea;

        /* Retrieve the buffers used to create the stream or message buffer
         * before deleting it */
        if( xIsMessageBuffer == pdTRUE )
        {
            xResult = xMessageBufferGetStaticBuffers( xStreamBuffer, &pucStreamBufferStorageArea, &pxStaticStreamBuffer );
        }
        else
        {
            xResult = xStreamBufferGetStaticBuffers( xStreamBuffer, &pucStreamBufferStorageArea, &pxStaticStreamBuffer );
        }

        configASSERT( xResult == pdTRUE );

        /* Delete the stream or message buffer */
        if( xIsMessageBuffer == pdTRUE )
        {
            vMessageBufferDelete( xStreamBuffer );
        }
        else
        {
            vSemaphoreDelete( xStreamBuffer );
        }

        /* Free the memory buffers */
        heap_caps_free( pxStaticStreamBuffer );
        heap_caps_free( pucStreamBufferStorageArea );
    }

/* ------------------------------ Event Groups ------------------------------ */

    EventGroupHandle_t xEventGroupCreateWithCaps( UBaseType_t uxMemoryCaps )
    {
        EventGroupHandle_t xEventGroup;
        StaticEventGroup_t * pxEventGroupBuffer;

        /* Allocate memory for the event group using the provided memory caps */
        pxEventGroupBuffer = heap_caps_malloc( sizeof( StaticEventGroup_t ), uxMemoryCaps );

        if( pxEventGroupBuffer == NULL )
        {
            return NULL;
        }

        /* Create the event group using static creation API */
        xEventGroup = xEventGroupCreateStatic( pxEventGroupBuffer );

        if( xEventGroup == NULL )
        {
            heap_caps_free( pxEventGroupBuffer );
        }

        return xEventGroup;
    }

    void vEventGroupDeleteWithCaps( EventGroupHandle_t xEventGroup )
    {
        BaseType_t xResult;
        StaticEventGroup_t * pxEventGroupBuffer;

        /* Retrieve the buffer used to create the event group before deleting it
         * */
        xResult = xEventGroupGetStaticBuffer( xEventGroup, &pxEventGroupBuffer );
        configASSERT( xResult == pdTRUE );

        /* Delete the event group */
        vEventGroupDelete( xEventGroup );

        /* Free the memory buffer */
        heap_caps_free( pxEventGroupBuffer );
    }

#endif /* if ( configSUPPORT_STATIC_ALLOCATION == 1 ) */
