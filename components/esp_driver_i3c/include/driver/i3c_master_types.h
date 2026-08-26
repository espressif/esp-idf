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
 * @brief Forward declaration of base device structure
 */
typedef struct i3c_master_device_t i3c_master_device_t;

/**
 * @brief Type of base device handle containing common members
 */
typedef struct i3c_master_device_t *i3c_master_device_handle_t;

/**
 * @brief Type of I2C device handle on I3C master bus
 */
typedef struct i3c_master_i2c_dev_t *i3c_master_i2c_device_handle_t;

/**
 * @brief Type of I3C device handle on I3C master bus
 */
typedef struct i3c_master_i3c_dev_t *i3c_master_i3c_device_handle_t;

/**
 * @brief Type alias for I3C device table structure
 */
typedef struct i3c_master_i3c_device_table_t i3c_master_i3c_device_table_t;

/**
 * @brief Type of I3C device table handle
 */
typedef struct i3c_master_i3c_device_table_t *i3c_master_i3c_device_table_handle_t;

/**
 * @brief Enumeration for I3C transfer direction.
 */
typedef enum {
    I3C_MASTER_TRANSFER_DIRECTION_WRITE = 0,
    I3C_MASTER_TRANSFER_DIRECTION_READ = 1,
} i3c_master_transfer_direction_t;

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
 * @brief I3C address slot status in address pool.
 */
typedef enum {
    I3C_ADDR_SLOT_FREE = 0, /*!< Address slot is free. */
    I3C_ADDR_SLOT_RESERVED, /*!< Address slot is reserved. Might be used as CCC command */
    I3C_ADDR_SLOT_I2CDEV,   /*!< Address slot is I2C device. */
    I3C_ADDR_SLOT_I3CDEV,   /*!< Address slot is I3C device. */
} i3c_addr_slot_status_t;

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

/**
 * @brief Structure representing event data for I3C master operations.
 */
typedef struct {
    i3c_master_event_t event;           /*!< The event type that occurred (e.g., transfer complete, error). */
    uint8_t *data;                      /*!< Pointer to the data buffer for the event (e.g., received data). */
    size_t data_size;                   /*!< The size of the data received, in bytes. */
} i3c_master_i3c_device_event_data_t;

/**
 * @brief Type definition for a callback function used in I3C master operations.
 */
typedef bool (*i3c_master_i3c_callback_t)(i3c_master_i3c_device_handle_t i3c_dev, const i3c_master_i3c_device_event_data_t *evt_data, void *arg);

/**
 * @brief In-Band Interrupt (IBI) event information.
 *
 * Describes an IBI issued by an I3C target device, including optional payload.
 *
 * - ibi_id: Raw IBI identifier reported by the controller. It typically encodes the
 *           issuer's dynamic address; it is raw value, `dynamic address` + `r/w bit`.
 * - ibi_sts: Controller-reported IBI status flags/fields that indicate the IBI type/state.
 * - data_length: Number of valid bytes in the payload buffer @ref ibi_data, range 0..32.
 * - ibi_data: Optional payload bytes associated with the IBI. Only the first
 *             @ref data_length bytes are valid.
 */
typedef struct {
    uint8_t ibi_id;                                     /*!< Raw IBI identifier as reported by the controller */
    uint8_t ibi_sts;                                    /*!< IBI status field reported by the controller */
    uint8_t data_length;                                /*!< Number of valid bytes in @ref ibi_data (0..32) */
    uint8_t ibi_data[I3C_MASTER_IBI_DATA_SIZE_MAX];     /*!< IBI payload data buffer (up to 32 bytes) */
} i3c_master_ibi_info_t;

/**
 * @brief IBI callback function type.
 *
 * Invoked when the target device issues an IBI. This callback runs in ISR context.
 *
 * @param[in] i3c_dev   Handle to the I3C device that issued the IBI.
 * @param[in] evt_data  Pointer to the IBI event information structure.
 * @param[in] arg       User-provided context pointer supplied at registration.
 *
 * @return true if a higher-priority task was woken and a context switch is requested;
 *         false otherwise.
 */
typedef bool (*i3c_master_ibi_callback_t)(i3c_master_i3c_device_handle_t i3c_dev, const i3c_master_ibi_info_t *evt_data, void *arg);

#ifdef __cplusplus
}
#endif
