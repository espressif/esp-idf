#ifndef ESP32_MOCK_H_
#define ESP32_MOCK_H_

typedef void (*esp_timer_cb_t)(void* arg);

typedef enum 
{
    ESP_TIMER_TASK,     //!< Callback is called from timer task
} esp_timer_dispatch_t;

typedef struct 
{
    esp_timer_cb_t callback;        //!< Function to call when timer expires
    void* arg;                      //!< Argument to pass to the callback
    esp_timer_dispatch_t dispatch_method;   //!< Call the callback from task or from ISR
    const char* name;               //!< Timer name, used in esp_timer_dump function
} esp_timer_create_args_t;

esp_err_t esp_timer_delete(esp_timer_handle_t timer);

esp_err_t esp_timer_stop(esp_timer_handle_t timer);

esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period);

esp_err_t esp_timer_create(const esp_timer_create_args_t* create_args,
                           esp_timer_handle_t* out_handle);


// Queue mock
 QueueHandle_t xQueueCreate( uint32_t uxQueueLength,
                             uint32_t uxItemSize );

void vQueueDelete( QueueHandle_t xQueue );

uint32_t xQueueSend(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait);

uint32_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);

void GetLastItem(void *pvBuffer);

void ForceTaskDelete();

#define _mdns_udp_pcb_write(tcpip_if, ip_protocol, ip, port, data, len) len

#endif /* ESP32_MOCK_H_ */