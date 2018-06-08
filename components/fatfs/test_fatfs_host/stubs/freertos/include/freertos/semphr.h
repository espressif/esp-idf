#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#define vSemaphoreDelete( xSemaphore )
#define xSemaphoreCreateMutex()                     ((void*)(1))
#define xSemaphoreGive( xSemaphore )
#define xSemaphoreTake( xSemaphore, xBlockTime )    pdTRUE

typedef void* SemaphoreHandle_t;

#if defined(__cplusplus)
}
#endif
