/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _WIFI_TYPES_H
#define _WIFI_TYPES_H
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief WiFi ioctl command type
  *
  */
typedef enum {
    WIFI_IOCTL_SET_STA_HT2040_COEX = 1, /**< Set the configuration of STA's HT2040 coexist management */
    WIFI_IOCTL_GET_STA_HT2040_COEX,     /**< Get the configuration of STA's HT2040 coexist management */
    WIFI_IOCTL_MAX,
} wifi_ioctl_cmd_t;

/**
 * @brief Configuration for STA's HT2040 coexist management
 *
 */
typedef struct {
    int enable;                         /**< Indicate whether STA's HT2040 coexist management is enabled or not */
} wifi_ht2040_coex_t;

/**
  * @brief Configuration for WiFi ioctl
  *
  */
typedef struct {
    union {
        wifi_ht2040_coex_t ht2040_coex; /**< Configuration of STA's HT2040 coexist management */
    } data;                             /**< Configuration of ioctl command */
} wifi_ioctl_config_t;

/**
  * @brief Mode for WiFi beacon drop
  *
  */
typedef enum {
    WIFI_BEACON_DROP_DISABLED,
    WIFI_BEACON_DROP_AUTO,
    WIFI_BEACON_DROP_FORCED,
} wifi_beacon_drop_t;

/**
  * @brief WiFi beacon monitor parameter configuration
  *
  */
typedef struct {
    bool        enable;                     /**< Enable or disable beacon monitor */
    uint8_t     loss_timeout;               /**< Beacon lost timeout */
    uint8_t     loss_threshold;             /**< Maximum number of consecutive lost beacons allowed */
    uint8_t     delta_intr_early;           /**< Delta early time for RF PHY on */
    uint8_t     delta_loss_timeout;         /**< Delta timeout time for RF PHY off */
#if MAC_SUPPORT_PMU_MODEM_STATE
    uint8_t     beacon_abort: 1,            /**< Enable or disable beacon abort */
                broadcast_wakeup: 1,        /**< Enable or disable TIM element multicast wakeup */
                reserved: 6;                /**< Reserved */
    uint8_t     tsf_time_sync_deviation;    /**< Deviation range to sync with AP TSF timestamp */
    uint16_t    modem_state_consecutive;    /**< PMU MODEM state consecutive count limit */
    uint16_t    rf_ctrl_wait_cycle;         /**< RF on wait time (unit: Modem APB clock cycle) */
#endif
    wifi_beacon_drop_t  beacon_drop;        /**< Whether to drop the beacon if the beacon lost exceeds loss_threshold */
} wifi_beacon_monitor_config_t;

/**
  * @brief WiFi beacon sample parameter configuration
  *
  */
typedef struct {
    uint16_t    sample_period;              /**< Sample beacon period, unit: number of beacons */
    uint8_t     resample_period;            /**< Resample period if beacon drop is active under the auto mode, unit: hours */
    /**< Standard triggers beacon drop when the expected rx beacon probability falls below this value under the auto mode, unit: percentage */
    uint8_t     standard;
    /**< Difference triggers an unstable event when the actual rx beacon probability continuously falls below the expected probability by this value, unit: percentage  */
    uint8_t     difference;
} wifi_beacon_offset_config_t;

#ifdef __cplusplus
}
#endif

#endif
