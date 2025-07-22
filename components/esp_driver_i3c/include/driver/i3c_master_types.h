/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include "hal/i3c_master_types.h"
#include "hal/gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of I3C master bus handle
 */
typedef struct i3c_master_bus_t *i3c_master_bus_handle_t;

/**
 * @brief Type of I2C device handle on I3C master bus
 */
typedef struct i3c_master_i2c_dev_t *i3c_master_i2c_device_handle_t;

/**
 * @brief Enumeration for I3C event.
 */
typedef enum {
    I3C_MASTER_EVENT_TRANS_DONE,       /*!< I3C bus transaction done */
    I3C_MASTER_EVENT_NACK,             /*!< I3C bus nack */
} i3c_master_event_t;

/**
 * @brief Structure representing event data for I3C/I2C master operations.
 */
typedef struct {
    i3c_master_event_t event;           /*!< The event type that occurred (e.g., transfer complete, error). */
    uint8_t *data;                      /*!< Pointer to the data buffer for the event (e.g., received data). */
    size_t data_size;                   /*!< The size of the data received, in bytes. */
} i3c_master_i2c_device_event_data_t;

/**
 * @brief Type definition for a callback function used in I3C/I2C master operations.
 *
 * This callback function is invoked when an event occurs during I3C/I2C master communication.
 *
 * @param i2c_dev   Handle to the I2C device associated with the event.
 * @param evt_data  Pointer to the structure containing event details and relevant data.
 * @param arg       User-provided argument passed during callback registration.
 *
 * @return Whether a high priority task has been waken up by this function.
 */
typedef bool (*i3c_master_i2c_callback_t)(i3c_master_i2c_device_handle_t i2c_dev, const i3c_master_i2c_device_event_data_t *evt_data, void *arg);

#ifdef __cplusplus
}
#endif
