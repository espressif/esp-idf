#ifndef ESP32_MOCK_H_
#define ESP32_MOCK_H_
#include "esp_timer.h"

typedef void (*esp_timer_cb_t)(void* arg);

// Queue mock
QueueHandle_t xQueueCreate( uint32_t uxQueueLength,
                             uint32_t uxItemSize );

void vQueueDelete( QueueHandle_t xQueue );

uint32_t xQueueSend(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait);

uint32_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);

void GetLastItem(void *pvBuffer);

void ForceTaskDelete(void);

esp_err_t esp_event_handler_register(const char * event_base, int32_t event_id, void* event_handler, void* event_handler_arg);

esp_err_t esp_event_handler_unregister(const char * event_base, int32_t event_id, void* event_handler);

#define _mdns_udp_pcb_write(tcpip_if, ip_protocol, ip, port, data, len) len

// Task signify mock
TaskHandle_t xTaskGetCurrentTaskHandle(void);
void xTaskNotifyGive(TaskHandle_t task);
BaseType_t xTaskNotifyWait(uint32_t bits_entry_clear, uint32_t bits_exit_clear, uint32_t *value, TickType_t wait_time );

#endif /* ESP32_MOCK_H_ */
