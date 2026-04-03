/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <sys/time.h>
#include <sys/timex.h>
#include "esp_timer.h"
#include "esp_task.h"
#include "esp_eth_clock.h"
#include "esp_eth_mac_esp.h"
#include "esp_libc_clock.h"
#include "sdkconfig.h"

static esp_eth_mac_t *s_mac;
static portMUX_TYPE s_time_update_lock = portMUX_INITIALIZER_UNLOCKED;

/** PTP clock offset-slew state. */
static struct {
    int32_t slew_ppb;
    int64_t remaining_ns;   // remaining offset in nanoseconds
    int64_t mono_start_us;  // start time of the slew in microseconds
} s_ptp_adj;

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

static void ptp_adj_clear_state(void)
{
    s_ptp_adj.slew_ppb = 0;
    s_ptp_adj.remaining_ns = 0;
    s_ptp_adj.mono_start_us = 0;
}

static int esp_eth_clock_ptp_gettime(struct timespec *tp, void *ctx)
{
    (void)ctx;
    if (s_mac == NULL) {
        errno = ENODEV;
        return -1;
    }

    eth_mac_time_t ptp_time;
    esp_err_t ret = esp_eth_mac_get_ptp_time(s_mac, &ptp_time);
    if (ret != ESP_OK) {
        errno = esp_eth_clock_esp_err_to_errno(ret);
        return -1;
    }
    tp->tv_sec = ptp_time.seconds;
    tp->tv_nsec = ptp_time.nanoseconds;

    return 0;
}

static int esp_eth_clock_ptp_settime(const struct timespec *tp, void *ctx)
{
    (void)ctx;
    if (s_mac == NULL) {
        errno = ENODEV;
        return -1;
    }

    eth_mac_time_t ptp_time = {
        .seconds = tp->tv_sec,
        .nanoseconds = tp->tv_nsec
    };
    esp_err_t ret = ESP_OK;
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    if ((s_ptp_adj.slew_ppb != 0) || (s_ptp_adj.remaining_ns != 0)) {
        ret = esp_eth_mac_adj_ptp_time(s_mac, 0);
        if (ret == ESP_OK) {
            ptp_adj_clear_state();
        }
    }
    if (ret == ESP_OK) {
        ret = esp_eth_mac_set_ptp_time(s_mac, &ptp_time);
    }
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
    if (ret != ESP_OK) {
        errno = esp_eth_clock_esp_err_to_errno(ret);
        return -1;
    }

    return 0;
}

static int64_t ptp_adj_update_remaining(int64_t now_us)
{
    if (s_ptp_adj.slew_ppb != 0) {
        int64_t elapsed_us = now_us - s_ptp_adj.mono_start_us;
        s_ptp_adj.mono_start_us = now_us;

        int64_t corrected_ns = (int64_t)s_ptp_adj.slew_ppb * elapsed_us / 1000000LL;
        s_ptp_adj.remaining_ns -= corrected_ns;

        bool done = (s_ptp_adj.slew_ppb > 0)
                  ? (s_ptp_adj.remaining_ns <= 0)
                  : (s_ptp_adj.remaining_ns >= 0);
        if (done) {
            ptp_adj_clear_state();
            esp_eth_mac_adj_ptp_time(s_mac, 0);
        }
    }
    return s_ptp_adj.remaining_ns;
}

static esp_err_t ptp_adj_apply_slew(int64_t now_us, int32_t slew_ppb,
                                         int64_t offset_ns, int64_t *out_remaining_ns)
{
    ptp_adj_update_remaining(now_us);
    esp_err_t ret = esp_eth_mac_adj_ptp_time(s_mac, slew_ppb);
    if (ret == ESP_OK) {
        if (slew_ppb == 0) {
            ptp_adj_clear_state();
        } else {
            s_ptp_adj.remaining_ns = offset_ns;
            s_ptp_adj.slew_ppb = slew_ppb;
            s_ptp_adj.mono_start_us = now_us;
        }
    }
    *out_remaining_ns = s_ptp_adj.remaining_ns;
    return ret;
}

 /**
  * @brief Adjust the PTP clock.
  *
  * @param buf Pointer to struct timex with modes and values to apply.
  * @return 0 on success, -1 on error (with errno set).
  *
  * Modes:
  *   ADJ_FREQUENCY  -- relative ppb via esp_eth_mac_adj_ptp_freq_ppb()
  *                     (emac_hal_ptp_adj_freq: relative to current addend)
  *   ADJ_OFFSET     -- temporary offset slewing via esp_eth_mac_adj_ptp_time()
  *                     over a period of CONFIG_ETH_CLOCK_ADJTIME_PERIOD_MS milliseconds
  *                     (emac_hal_ptp_adj_inc: absolute ppb from base addend)
  *
  *                      esp_timer time is the reference for measuring slew progress
  *                      because the PTP clock itself is being adjusted
  *
  *  Note: Adjusting time by ADJ_FREQUENCY is more accurate than ADJ_OFFSET.
  */
static int esp_eth_clock_ptp_adjtime(struct timex *buf, void *ctx)
{
    (void)ctx;
    if (s_mac == NULL) {
        errno = ENODEV;
        return -1;
    }

    if (buf->modes & ADJ_FREQUENCY) {
        portENTER_CRITICAL_SAFE(&s_time_update_lock);
        esp_err_t ret = esp_eth_mac_adj_ptp_freq_ppb(s_mac, (int32_t)buf->freq);
        portEXIT_CRITICAL_SAFE(&s_time_update_lock);
        if (ret != ESP_OK) {
            errno = esp_eth_clock_esp_err_to_errno(ret);
            return -1;
        }
        return 0;
    }

    int64_t remaining_ns;
    int64_t now_us;
    if ((buf->modes & ADJ_OFFSET) && buf->modes != ADJ_OFFSET_SS_READ) {
        bool offset_in_ns = (buf->modes & ADJ_NANO) == ADJ_NANO;
        int64_t offset_ns = offset_in_ns ? (int64_t)buf->offset : (int64_t)buf->offset * 1000LL;
        int32_t slew_ppb = 0;
        if (offset_ns != 0) {
            // ppb = (offset / period) * 10^9 => ns / ms =>  10^(9+3) / 10^3 == 10^9
            int64_t ppb64 = offset_ns * 1000 / CONFIG_ETH_CLOCK_ADJTIME_PERIOD_MS;
            // clamp to slew limit
            slew_ppb = (ppb64 > CONFIG_ETH_CLOCK_ADJTIME_SLEWLIMIT_PPB)  ?  CONFIG_ETH_CLOCK_ADJTIME_SLEWLIMIT_PPB
                     : (ppb64 < -CONFIG_ETH_CLOCK_ADJTIME_SLEWLIMIT_PPB) ? -CONFIG_ETH_CLOCK_ADJTIME_SLEWLIMIT_PPB
                     : (int32_t)ppb64;
        }
        now_us = esp_timer_get_time();
        esp_err_t ret;
        portENTER_CRITICAL_SAFE(&s_time_update_lock);
        ret = ptp_adj_apply_slew(now_us, slew_ppb, offset_ns, &remaining_ns);
        portEXIT_CRITICAL_SAFE(&s_time_update_lock);
        buf->offset = offset_in_ns ? (long)remaining_ns : (long)(remaining_ns / 1000);
        if (ret != ESP_OK) {
            errno = esp_eth_clock_esp_err_to_errno(ret);
            return -1;
        }
        return 0;
    }

    // Read-only: ADJ_OFFSET_SS_READ or unrecognized mode
    now_us = esp_timer_get_time();
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    remaining_ns = ptp_adj_update_remaining(now_us);
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
    // ADJ_OFFSET_SS_READ always returns microseconds; otherwise respect ADJ_NANO
    if (buf->modes != ADJ_OFFSET_SS_READ && (buf->modes & ADJ_NANO)) {
        buf->offset = (long)remaining_ns;
    } else {
        buf->offset = (long)(remaining_ns / 1000);
    }
    return 0;
}

static int esp_eth_clock_ptp_getres(struct timespec *res, void *ctx)
{
    (void)ctx;
    if (s_mac == NULL) {
        errno = ENODEV;
        return -1;
    }

    res->tv_sec = 0;
    res->tv_nsec = esp_eth_mac_get_ts_resolution(s_mac);
    if (res->tv_nsec == 0) {
        errno = EINVAL;
        return -1;
    }
    return 0;
}

static const esp_libc_clock_ops_t s_ptp_system_ops = {
    .gettime = esp_eth_clock_ptp_gettime,
    .settime = esp_eth_clock_ptp_settime,
    .adjtime = esp_eth_clock_ptp_adjtime,
    .getres = esp_eth_clock_ptp_getres
};

ESP_LIBC_CLOCK_REGISTER(ptp_system, CLOCK_PTP_SYSTEM, s_ptp_system_ops, NULL);

int esp_eth_clock_set_target_time(clockid_t clock_id, struct timespec *tp)
{
    eth_mac_time_t mac_target_time = {
        .seconds = tp->tv_sec,
        .nanoseconds = tp->tv_nsec
    };
    esp_err_t ret = esp_eth_mac_set_target_time(s_mac, &mac_target_time);
    if (ret != ESP_OK) {
        errno = esp_eth_clock_esp_err_to_errno(ret);
        return -1;
    }
    return 0;
}

int esp_eth_clock_register_target_cb(clockid_t clock_id,
                                     ts_target_exceed_cb_from_isr_t ts_callback)
{
    esp_err_t ret = esp_eth_mac_set_target_time_cb(s_mac, ts_callback);
    if (ret != ESP_OK) {
        errno = esp_eth_clock_esp_err_to_errno(ret);
        return -1;
    }
    return 0;
}

esp_err_t esp_eth_clock_init(esp_eth_handle_t eth_hndl, const esp_eth_clock_cfg_t *cfg)
{
    if (s_mac != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    switch (cfg->clock_id) {
    case CLOCK_PTP_SYSTEM: {
        eth_mac_ptp_config_t ptp_cfg = ETH_MAC_ESP_PTP_DEFAULT_CONFIG();
        esp_eth_mac_t *mac;
        if (esp_eth_get_mac_instance(eth_hndl, &mac) != ESP_OK) {
            return ESP_FAIL;
        }
        if (esp_eth_mac_ptp_enable(mac, &ptp_cfg) != ESP_OK) {
            return ESP_FAIL;
        }
        s_mac = mac;
        break;
    }
    default:
        return ESP_FAIL;
    }
    return ESP_OK;
}
