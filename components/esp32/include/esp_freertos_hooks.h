#ifndef ESP_FREERTOS_HOOKS_H
#define ESP_FREERTOS_HOOKS_H

#include <stdbool.h>
#include "esp_err.h"


/*
 Definitions for the tickhook and idlehook callbacks
*/
typedef bool (*esp_freertos_idle_cb_t)();
typedef void (*esp_freertos_tick_cb_t)();

/**
  * @brief  Register a callback to be called on the freertos idle hook
  *         The callback should return true if it's okay for the core to
  *         sleep until an interrupt (or FreeRTOS tick) happens and false
  *         if it should be called again as fast as possible.
  *
  * @param  esp_freertos_idle_cb_t new_idle_cb : Callback to be called
  *
  * @return ESP_OK : Callback registered
  * @return ESP_ERR_NO_MEM : No more space to register hook
  */
esp_err_t esp_register_freertos_idle_hook(esp_freertos_idle_cb_t new_idle_cb);

/**
  * @brief  Register a callback to be called on the freertos tick hook
  *
  * @param  esp_freertos_tick_cb_t new_tick_cb : Callback to be called
  *
  * @return ESP_OK : Callback registered
  * @return ESP_ERR_NO_MEM : No more space to register hook
  */
esp_err_t esp_register_freertos_tick_hook(esp_freertos_tick_cb_t tick_cb);


/**
  * @brief  Unregister an idle callback registered earlier
  *
  * @param  esp_freertos_idle_cb_t new_idle_cb : Callback to be unregistered
  *
  * @return void
  */
void esp_deregister_freertos_idle_hook(esp_freertos_idle_cb_t old_idle_cb);


/**
  * @brief  Unregister a tick callback registered earlier
  *
  * @param  esp_freertos_idle_cb_t new_idle_cb : Callback to be unregistered
  *
  * @return void
  */
void esp_deregister_freertos_tick_hook(esp_freertos_tick_cb_t old_tick_cb);


#endif