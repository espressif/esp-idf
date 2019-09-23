#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "esp32_compat.h"

void*     g_queue;
int       g_queue_send_shall_fail = 0;
int       g_size = 0;

const char * WIFI_EVENT = "wifi_event";
const char * IP_EVENT = "ip_event";
const char * ETH_EVENT = "eth_event";

esp_err_t esp_event_handler_register(const char * event_base,
                                        int32_t event_id,
                                        void* event_handler,
                                        void* event_handler_arg)
{
    return ESP_OK;
}

esp_err_t esp_event_handler_unregister(const char * event_base, int32_t event_id, void* event_handler)
{
    return ESP_OK;
}

esp_err_t esp_timer_delete(esp_timer_handle_t timer)
{
    return ESP_OK;
}

esp_err_t esp_timer_stop(esp_timer_handle_t timer)
{
    return ESP_OK;
}

esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period)
{
    return ESP_OK;
}

esp_err_t esp_timer_create(const esp_timer_create_args_t* create_args,
                           esp_timer_handle_t* out_handle)
{
    return ESP_OK;
}

uint32_t xTaskGetTickCount(void)
{
    struct timeval tv;
    struct timezone tz;
    if (gettimeofday(&tv, &tz) == 0) {
        return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    }
    return 0;
}

/// Queue mock
 QueueHandle_t xQueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize )
 {
     g_size = uxItemSize;
     g_queue = malloc((uxQueueLength)*(uxItemSize));
     return g_queue;
 }


void vQueueDelete( QueueHandle_t xQueue )
{
    free(xQueue);
}

uint32_t xQueueSend(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait)
{
    if (g_queue_send_shall_fail)
    {
        return pdFALSE;
    }
    else
    {
        memcpy(xQueue, pvItemToQueue, g_size);
        return pdPASS;
    }
}


uint32_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait)
{
    return pdFALSE;
}

void GetLastItem(void *pvBuffer)
{
    memcpy(pvBuffer, g_queue, g_size);
}

void ForceTaskDelete(void)
{
    g_queue_send_shall_fail = 1;
}
