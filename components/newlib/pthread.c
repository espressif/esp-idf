#include <pthread.h>
#include "esp_log.h"

const static char *TAG = "esp32_asio_pthread";

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id) 
{
    ESP_LOGW(TAG, "%s: not yet supported!", __FUNCTION__);   
    return 0;
}
