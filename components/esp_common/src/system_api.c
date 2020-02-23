#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define SHUTDOWN_HANDLERS_NO 2
static shutdown_handler_t shutdown_handlers[SHUTDOWN_HANDLERS_NO];

esp_err_t esp_register_shutdown_handler(shutdown_handler_t handler)
{
    for (int i = 0; i < SHUTDOWN_HANDLERS_NO; i++) {
        if (shutdown_handlers[i] == handler) {
            return ESP_ERR_INVALID_STATE;
        } else if (shutdown_handlers[i] == NULL) {
            shutdown_handlers[i] = handler;
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_unregister_shutdown_handler(shutdown_handler_t handler)
{
    for (int i = 0; i < SHUTDOWN_HANDLERS_NO; i++) {
        if (shutdown_handlers[i] == handler) {
            shutdown_handlers[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

void IRAM_ATTR esp_restart(void)
{
    for (int i = SHUTDOWN_HANDLERS_NO - 1; i >= 0; i--) {
        if (shutdown_handlers[i]) {
            shutdown_handlers[i]();
        }
    }

    // Disable scheduler on this core.
    vTaskSuspendAll();

    esp_restart_noos();
}

uint32_t esp_get_free_heap_size( void )
{
    return heap_caps_get_free_size( MALLOC_CAP_DEFAULT );
}

uint32_t esp_get_minimum_free_heap_size( void )
{
    return heap_caps_get_minimum_free_size( MALLOC_CAP_DEFAULT );
}

const char* esp_get_idf_version(void)
{
    return IDF_VER;
}

