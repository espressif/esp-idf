/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_SNTP_H__
#define __ESP_SNTP_H__

#include "lwip/err.h"
#include "lwip/ip.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * The time update takes place in the sntp_sync_time() function.
 * The user has the ability to redefine this function in order
 * to re-define its functionality. This function has two time update modes,
 * which can be set via the sntp_set_sync_mode() function.
 * Two available modes are as follows:
 * - the first is an immediate update when receiving time from the sntp server,
 * - the second is a smooth time update (if the time error is no more than 35 minutes,
 *   and an immediate update if the error is more than 35 minutes).
 *
 * To receive notification of time synchronization,
 * you can use the callback function or get the synchronization status
 * via the sntp_get_sync_status() function.
 *
 * To determine the time synchronization time on the device, you can use:
 * 1) sntp_set_time_sync_notification_cb() function to set the callback function,
 *    which is convenient to use to receive notification of the update time.
 * 2) sntp_get_sync_status() function for getting time synchronization status.
 *    After the time synchronization is completed, the status will be
 *    SNTP_SYNC_STATUS_COMPLETED, after, it will be reseted to SNTP_SYNC_STATUS_RESET
 *    to wait for the next sync cycle.
 */

/// Aliases for esp_sntp prefixed API (inherently thread safe)
#define esp_sntp_sync_time sntp_sync_time
#define esp_sntp_set_sync_mode sntp_set_sync_mode
#define esp_sntp_get_sync_mode sntp_get_sync_mode
#define esp_sntp_get_sync_status sntp_get_sync_status
#define esp_sntp_set_sync_status sntp_set_sync_status
#define esp_sntp_set_time_sync_notification_cb sntp_set_time_sync_notification_cb
#define esp_sntp_set_sync_interval sntp_set_sync_interval
#define esp_sntp_get_sync_interval sntp_get_sync_interval
#define esp_sntp_restart sntp_restart

#ifndef SNTP_OPMODE_POLL
#define SNTP_OPMODE_POLL ESP_SNTP_OPMODE_POLL
#else
#warning "Defined!"
#endif /* SNTP_OPMODE_POLL */

/// SNTP time update mode
typedef enum {
    SNTP_SYNC_MODE_IMMED,   /*!< Update system time immediately when receiving a response from the SNTP server. */
    SNTP_SYNC_MODE_SMOOTH,  /*!< Smooth time updating. Time error is gradually reduced using adjtime function. If the difference between SNTP response time and system time is large (more than 35 minutes) then update immediately. */
} sntp_sync_mode_t;

/// SNTP sync status
typedef enum {
    SNTP_SYNC_STATUS_RESET,         // Reset status.
    SNTP_SYNC_STATUS_COMPLETED,     // Time is synchronized.
    SNTP_SYNC_STATUS_IN_PROGRESS,   // Smooth time sync in progress.
} sntp_sync_status_t;

/// SNTP operating modes per lwip SNTP module
typedef enum {
    ESP_SNTP_OPMODE_POLL,
    ESP_SNTP_OPMODE_LISTENONLY,
} esp_sntp_operatingmode_t;

/**
 * @brief SNTP callback function for notifying about time sync event
 *
 * @param tv Time received from SNTP server.
 */
typedef void (*sntp_sync_time_cb_t) (struct timeval *tv);

/**
 * @brief This function updates the system time.
 *
 * This is a weak-linked function. It is possible to replace all SNTP update functionality
 * by placing a sntp_sync_time() function in the app firmware source.
 * If the default implementation is used, calling sntp_set_sync_mode() allows
 * the time synchronization mode to be changed to instant or smooth.
 * If a callback function is registered via sntp_set_time_sync_notification_cb(),
 * it will be called following time synchronization.
 *
 * @param tv Time received from SNTP server.
 */
void sntp_sync_time(struct timeval *tv);

/**
 * @brief Set the sync mode
 *
 * Modes allowed: SNTP_SYNC_MODE_IMMED and SNTP_SYNC_MODE_SMOOTH.
 * @param sync_mode Sync mode.
 */
void sntp_set_sync_mode(sntp_sync_mode_t sync_mode);

/**
 * @brief Get set sync mode
 *
 * @return  SNTP_SYNC_MODE_IMMED: Update time immediately.
 *          SNTP_SYNC_MODE_SMOOTH: Smooth time updating.
 */
sntp_sync_mode_t sntp_get_sync_mode(void);

/**
 * @brief Get status of time sync
 *
 * After the update is completed, the status will be returned as SNTP_SYNC_STATUS_COMPLETED.
 * After that, the status will be reset to SNTP_SYNC_STATUS_RESET.
 * If the update operation is not completed yet, the status will be SNTP_SYNC_STATUS_RESET.
 * If a smooth mode was chosen and the synchronization is still continuing (adjtime works), then it will be SNTP_SYNC_STATUS_IN_PROGRESS.
 *
 * @return  SNTP_SYNC_STATUS_RESET: Reset status.
 *          SNTP_SYNC_STATUS_COMPLETED: Time is synchronized.
 *          SNTP_SYNC_STATUS_IN_PROGRESS: Smooth time sync in progress.
 */
sntp_sync_status_t sntp_get_sync_status(void);

/**
 * @brief Set status of time sync
 *
 * @param sync_status status of time sync (see sntp_sync_status_t)
 */
void sntp_set_sync_status(sntp_sync_status_t sync_status);

/**
 * @brief Set a callback function for time synchronization notification
 *
 * @param callback a callback function
 */
void sntp_set_time_sync_notification_cb(sntp_sync_time_cb_t callback);

/**
 * @brief Set the sync interval of SNTP operation
 *
 * Note: SNTPv4 RFC 4330 enforces a minimum sync interval of 15 seconds.
 * This sync interval will be used in the next attempt update time throught SNTP.
 * To apply the new sync interval call the sntp_restart() function,
 * otherwise, it will be applied after the last interval expired.
 *
 * @param interval_ms   The sync interval in ms. It cannot be lower than 15 seconds, otherwise 15 seconds will be set.
 */
void sntp_set_sync_interval(uint32_t interval_ms);

/**
 * @brief Get the sync interval of SNTP operation
 *
 * @return  the sync interval
 */
uint32_t sntp_get_sync_interval(void);

/**
 * @brief Restart SNTP
 *
 * @return True  - Restart
 *         False - SNTP was not initialized yet
 */
bool sntp_restart(void);

/**
 * @brief Sets SNTP operating mode. The mode has to be set before init.
 *
 * @param operating_mode Desired operating mode
 */
void esp_sntp_setoperatingmode(esp_sntp_operatingmode_t operating_mode);

/**
 * @brief Init and start SNTP service
 */
void esp_sntp_init(void);

/**
 * @brief Stops SNTP service
 */
void esp_sntp_stop(void);

/**
 * @brief Sets SNTP server address
 *
 * @param idx Index of the server
 * @param addr IP address of the server
 */
void esp_sntp_setserver(u8_t idx, const ip_addr_t *addr);

/**
 * @brief Sets SNTP hostname
 * @param idx Index of the server
 * @param server Name of the server
 */
void esp_sntp_setservername(u8_t idx, const char *server);

/**
 * @brief Gets SNTP server name
 * @param idx Index of the server
 * @return Name of the server
 */
const char *esp_sntp_getservername(u8_t idx);

/**
 * @brief Get SNTP server IP
 * @param idx Index of the server
 * @return IP address of the server
 */
const ip_addr_t* esp_sntp_getserver(u8_t idx);

/**
 * @brief Checks if sntp is enabled
 * @return true if sntp module is enabled
 */
bool esp_sntp_enabled(void);

#if LWIP_DHCP_GET_NTP_SRV
/**
 * @brief Enable acquiring SNTP server from DHCP
 * @param enable True for enabling SNTP from DHCP
 */
void esp_sntp_servermode_dhcp(bool enable);
#endif /* LWIP_DHCP_GET_NTP_SRV */

#if !defined(ESP_LWIP_COMPONENT_BUILD) && !defined(ESP_NETIF_COMPONENT_BUILD)
/**
 * @brief if not build within lwip, provide translating inlines,
 * that will warn about thread safety
 */
static inline __attribute__((deprecated("use esp_sntp_setoperatingmode() instead")))
void sntp_setoperatingmode(u8_t operating_mode)
{
    esp_sntp_setoperatingmode((esp_sntp_operatingmode_t)operating_mode);
}

static inline __attribute__((deprecated("use esp_sntp_servermode_dhcp() instead")))
void sntp_servermode_dhcp(int set_servers_from_dhcp)
{
#if LWIP_DHCP_GET_NTP_SRV
    esp_sntp_servermode_dhcp(set_servers_from_dhcp);
#endif
}

static inline __attribute__((deprecated("use esp_sntp_setservername() instead")))
void sntp_setservername(u8_t idx, const char *server)
{
    esp_sntp_setservername(idx, server);
}

static inline __attribute__((deprecated("use esp_sntp_init() instead")))
void sntp_init(void)
{
    esp_sntp_init();
}

static inline __attribute__((deprecated("use esp_sntp_getservername() instead")))
const char *sntp_getservername(u8_t idx)
{
    return esp_sntp_getservername(idx);
}

static inline __attribute__((deprecated("use esp_sntp_getserver() instead")))
const ip_addr_t* sntp_getserver(u8_t idx)
{
    return esp_sntp_getserver(idx);
}

#endif /* ESP_LWIP_COMPONENT_BUILD */


#ifdef __cplusplus
}
#endif

#endif // __ESP_SNTP_H__
