/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include "esp_eth_time.h"

static esp_eth_handle_t s_eth_hndl;

static int esp_eth_clock_esp_err_to_errno(esp_err_t esp_err)
{
    switch (esp_err)
    {
    case ESP_ERR_INVALID_ARG:
        return EINVAL;
    case ESP_ERR_INVALID_STATE:
        return EBUSY;
    case ESP_ERR_TIMEOUT:
        return ETIME;
    }
    // default "no err" when error cannot be isolated
    return 0;
}

int esp_eth_clock_adjtime(clockid_t clk_id, esp_eth_clock_adj_param_t *adj)
{
    switch (clk_id) {
    case CLOCK_PTP_SYSTEM:
        if (adj->mode == ETH_CLK_ADJ_FREQ_SCALE) {
            esp_err_t ret = esp_eth_ioctl(s_eth_hndl, ETH_MAC_ESP_CMD_ADJ_PTP_FREQ, &adj->freq_scale);
            if (ret != ESP_OK) {
                errno = esp_eth_clock_esp_err_to_errno(ret);
                return -1;
            }
        } else {
            errno = EINVAL;
            return -1;
        }
        break;
    default:
        errno = EINVAL;
        return -1;
    }
    return 0;
}

int esp_eth_clock_settime(clockid_t clock_id, const struct timespec *tp)
{
    switch (clock_id) {
    case CLOCK_PTP_SYSTEM: {
        if (s_eth_hndl) {
            eth_mac_time_t ptp_time = {
                .seconds = tp->tv_sec,
                .nanoseconds = tp->tv_nsec
            };
            esp_err_t ret = esp_eth_ioctl(s_eth_hndl, ETH_MAC_ESP_CMD_S_PTP_TIME, &ptp_time);
            if (ret != ESP_OK) {
                errno = esp_eth_clock_esp_err_to_errno(ret);
                return -1;
            }
        } else {
            errno = ENODEV;
            return -1;
        }
        break;
    }
    default:
        errno = EINVAL;
        return -1;
    }
    return 0;
}

int esp_eth_clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    switch (clock_id) {
    case CLOCK_PTP_SYSTEM: {
        if (s_eth_hndl) {
            eth_mac_time_t ptp_time;
            esp_err_t ret = esp_eth_ioctl(s_eth_hndl, ETH_MAC_ESP_CMD_G_PTP_TIME, &ptp_time);
            if (ret != ESP_OK) {
                errno = esp_eth_clock_esp_err_to_errno(ret);
                return -1;
            }
            tp->tv_sec = ptp_time.seconds;
            tp->tv_nsec = ptp_time.nanoseconds;
        } else {
            errno = ENODEV;
            return -1;
        }
        break;
    }
    default:
        errno = EINVAL;
        return -1;
    }
    return 0;
}

int esp_eth_clock_set_target_time(clockid_t clock_id, struct timespec *tp)
{
    eth_mac_time_t mac_target_time = {
        .seconds = tp->tv_sec,
        .nanoseconds = tp->tv_nsec
    };
    esp_err_t ret = esp_eth_ioctl(s_eth_hndl, ETH_MAC_ESP_CMD_S_TARGET_TIME, &mac_target_time);
    if (ret != ESP_OK) {
        errno = esp_eth_clock_esp_err_to_errno(ret);
        return -1;
    }
    return 0;
}

int esp_eth_clock_register_target_cb(clockid_t clock_id,
                                     ts_target_exceed_cb_from_isr_t ts_callback)
{
    esp_err_t ret = esp_eth_ioctl(s_eth_hndl, ETH_MAC_ESP_CMD_S_TARGET_CB, ts_callback);
    if (ret != ESP_OK) {
        errno = esp_eth_clock_esp_err_to_errno(ret);
        return -1;
    }
    return 0;
}

esp_err_t esp_eth_clock_init(clockid_t clock_id, esp_eth_clock_cfg_t *cfg)
{
    switch (clock_id) {
    case CLOCK_PTP_SYSTEM:
        // PTP Clock is part of Ethernet system
        bool ptp_enable = true;
        if (esp_eth_ioctl(cfg->eth_hndl, ETH_MAC_ESP_CMD_PTP_ENABLE, &ptp_enable) != ESP_OK) {
            return ESP_FAIL;
        }
        s_eth_hndl = cfg->eth_hndl;
        break;
    default:
        return ESP_FAIL;
    }
    return ESP_OK;
}
