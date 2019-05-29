#include <pthread.h>
#include "esp_log.h"

const static char *TAG = "esp32_asio_pthread";

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id) 
{
    ESP_LOGW(TAG, "%s: not yet supported!", __FUNCTION__);   
    return 0;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    return 0;
}

void newlib_include_pthread_impl()
{
    // Linker hook, exists for no other purpose
}
