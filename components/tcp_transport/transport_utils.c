#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "esp_transport_utils.h"

struct timeval* esp_transport_utils_ms_to_timeval(int timeout_ms, struct timeval *tv)
{
    if (timeout_ms == -1) {
        return NULL;
    }
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms - (tv->tv_sec * 1000)) * 1000;
    return tv;
}