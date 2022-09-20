/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "tusb.h"
#include "tinyusb.h"

/**
 * @brief CDC ports available to setup
 */
typedef enum {
    TINYUSB_CDC_ACM_0 = 0x0,
    TINYUSB_CDC_ACM_1,
    TINYUSB_CDC_ACM_MAX
} tinyusb_cdcacm_itf_t;

/* Callbacks and events
   ********************************************************************* */

/**
 * @brief Data provided to the input of the `callback_rx_wanted_char` callback
 */
typedef struct {
    char wanted_char; /*!< Wanted character */
} cdcacm_event_rx_wanted_char_data_t;

/**
 * @brief Data provided to the input of the `callback_line_state_changed` callback
 */
typedef struct {
    bool dtr; /*!< Data Terminal Ready (DTR) line state */
    bool rts; /*!< Request To Send (RTS) line state */
} cdcacm_event_line_state_changed_data_t;

/**
 * @brief Data provided to the input of the `line_coding_changed` callback
 */
typedef struct {
    cdc_line_coding_t const *p_line_coding; /*!< New line coding value */
} cdcacm_event_line_coding_changed_data_t;

/**
 * @brief Types of CDC ACM events
 */
typedef enum {
    CDC_EVENT_RX,
    CDC_EVENT_RX_WANTED_CHAR,
    CDC_EVENT_LINE_STATE_CHANGED,
    CDC_EVENT_LINE_CODING_CHANGED
} cdcacm_event_type_t;

/**
 * @brief Describes an event passing to the input of a callbacks
 */
typedef struct {
    cdcacm_event_type_t type; /*!< Event type */
    union {
        cdcacm_event_rx_wanted_char_data_t rx_wanted_char_data; /*!< Data input of the `callback_rx_wanted_char` callback */
        cdcacm_event_line_state_changed_data_t line_state_changed_data; /*!< Data input of the `callback_line_state_changed` callback */
        cdcacm_event_line_coding_changed_data_t line_coding_changed_data; /*!< Data input of the `line_coding_changed` callback */
    };
} cdcacm_event_t;

/**
 * @brief CDC-ACM callback type
 */
typedef void(*tusb_cdcacm_callback_t)(int itf, cdcacm_event_t *event);

/*********************************************************************** Callbacks and events*/
/* Other structs
   ********************************************************************* */

/**
 * @brief Configuration structure for CDC-ACM
 */
typedef struct {
    tinyusb_usbdev_t usb_dev; /*!< Usb device to set up */
    tinyusb_cdcacm_itf_t cdc_port;  /*!< CDC port */
    size_t rx_unread_buf_sz; /*!< Amount of data that can be passed to the ACM at once */
    tusb_cdcacm_callback_t callback_rx;  /*!< Pointer to the function with the `tusb_cdcacm_callback_t` type that will be handled as a callback */
    tusb_cdcacm_callback_t callback_rx_wanted_char; /*!< Pointer to the function with the `tusb_cdcacm_callback_t` type that will be handled as a callback */
    tusb_cdcacm_callback_t callback_line_state_changed; /*!< Pointer to the function with the `tusb_cdcacm_callback_t` type that will be handled as a callback */
    tusb_cdcacm_callback_t callback_line_coding_changed; /*!< Pointer to the function with the `tusb_cdcacm_callback_t` type that will be handled as a callback */
} tinyusb_config_cdcacm_t;

/*********************************************************************** Other structs*/
/* Public functions
   ********************************************************************* */
/**
 * @brief Initialize CDC ACM. Initialization will be finished with
 *          the `tud_cdc_line_state_cb` callback
 *
 * @param cfg - init configuration structure
 * @return esp_err_t
 */
esp_err_t tusb_cdc_acm_init(const tinyusb_config_cdcacm_t *cfg);


/**
 * @brief Register a callback invoking on CDC event. If the callback had been
 *        already registered, it will be overwritten
 *
 * @param itf - number of a CDC object
 * @param event_type - type of registered event for a callback
 * @param callback  - callback function
 * @return esp_err_t - ESP_OK or ESP_ERR_INVALID_ARG
 */
esp_err_t tinyusb_cdcacm_register_callback(tinyusb_cdcacm_itf_t itf,
        cdcacm_event_type_t event_type,
        tusb_cdcacm_callback_t callback);


/**
 * @brief Unregister a callback invoking on CDC event.
 *
 * @param itf - number of a CDC object
 * @param event_type - type of registered event for a callback
 * @return esp_err_t - ESP_OK or ESP_ERR_INVALID_ARG
 */
esp_err_t tinyusb_cdcacm_unregister_callback(tinyusb_cdcacm_itf_t itf, cdcacm_event_type_t event_type);


/**
 * @brief Sent one character to a write buffer
 *
 * @param itf - number of a CDC object
 * @param ch - character to send
 * @return size_t - amount of queued bytes
 */
size_t tinyusb_cdcacm_write_queue_char(tinyusb_cdcacm_itf_t itf, char ch);


/**
 * @brief Write data to write buffer from a byte array
 *
 * @param itf - number of a CDC object
 * @param in_buf - a source array
 * @param in_size - size to write from arr_src
 * @return size_t - amount of queued bytes
 */
size_t tinyusb_cdcacm_write_queue(tinyusb_cdcacm_itf_t itf, const uint8_t *in_buf, size_t in_size);

/**
 * @brief Send all data from a write buffer. Use `tinyusb_cdcacm_write_queue` to add data to the buffer.
 *
 *        WARNING! TinyUSB can block output Endpoint for several RX callbacks, after will do additional flush
 *        after the each trasfer. That can leads to the situation when you requested a flush, but it will fail until
 *        ont of the next callbacks ends.
 *        SO USING OF THE FLUSH WITH TIMEOUTS IN CALLBACKS IS NOT RECOMENDED - YOU CAN GET A LOCK FOR THE TIMEOUT
 *
 * @param itf - number of a CDC object
 * @param timeout_ticks - waiting until flush will be considered as failed
 * @return esp_err_t -  ESP_OK if (timeout_ticks > 0) and and flush was successful,
 *                      ESP_ERR_TIMEOUT if timeout occurred3 or flush was successful with (timeout_ticks == 0)
 *                      ESP_FAIL if flush was unsuccessful
 */
esp_err_t tinyusb_cdcacm_write_flush(tinyusb_cdcacm_itf_t itf, uint32_t timeout_ticks);

/**
 * @brief Read a content to the array, and defines it's size to the sz_store
 *
 * @param itf - number of a CDC object
 * @param out_buf - to this array will be stored the object from a CDC buffer
 * @param out_buf_sz - size of buffer for results
 * @param rx_data_size - to this address will be stored the object's size
 * @return esp_err_t ESP_OK, ESP_FAIL or ESP_ERR_INVALID_STATE
 */
esp_err_t tinyusb_cdcacm_read(tinyusb_cdcacm_itf_t itf, uint8_t *out_buf, size_t out_buf_sz, size_t *rx_data_size);


/**
 * @brief Check if the ACM initialized
 *
 * @param itf - number of a CDC object
 * @return true or false
 */
bool tusb_cdc_acm_initialized(tinyusb_cdcacm_itf_t itf);

/*********************************************************************** Public functions*/

#ifdef __cplusplus
}
#endif
