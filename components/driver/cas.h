
#ifndef __CAS_H__
#define __CAS_H__

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#define __cas_temp_sync_compare_and_swap_test(ptr, expected, desired) ({\
    bool ret = false;\
    unsigned state = portENTER_CRITICAL_NESTED(); \
    if (*ptr == expected) { \
        *ptr = desired; \
        ret = true;\
    }\
    portEXIT_CRITICAL_NESTED(state); \
    ret; \
})

#define __sync_bool_compare_and_swap(ptr, expected, desired) __cas_temp_sync_compare_and_swap_test(ptr, expected, desired)


#endif