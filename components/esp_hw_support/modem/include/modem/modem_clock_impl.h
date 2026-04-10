/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_private/esp_modem_clock.h"
#include "esp_macros.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/critical_section.h"
#if SOC_MODEM_CLOCK_SUPPORTED
#include "hal/modem_clock_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MODEM_CLOCK_SUPPORTED

#define MODEM_CLOCK(dev) BIT(MODEM_CLOCK_##dev)
#define MODEM_CLOCKS_1(a)                  MODEM_CLOCK(a)
#define MODEM_CLOCKS_2(a, b)               MODEM_CLOCK(a) | MODEM_CLOCK(b)
#define MODEM_CLOCKS_3(a, b, c)            MODEM_CLOCK(a) | MODEM_CLOCKS_2(b, c)
#define MODEM_CLOCKS_4(a, b, c, d)         MODEM_CLOCK(a) | MODEM_CLOCKS_3(b, c, d)
#define MODEM_CLOCKS_5(a, b, c, d, ...)    MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_1(__VA_ARGS__)
#define MODEM_CLOCKS_6(a, b, c, d, ...)    MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_2(__VA_ARGS__)
#define MODEM_CLOCKS_7(a, b, c, d, ...)    MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_3(__VA_ARGS__)
#define MODEM_CLOCKS_8(a, b, c, d, ...)    MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_4(__VA_ARGS__)
#define MODEM_CLOCKS_9(a, b, c, d, ...)    MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_5(__VA_ARGS__)
#define MODEM_CLOCKS_10(a, b, c, d, ...)   MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_6(__VA_ARGS__)
#define MODEM_CLOCKS_11(a, b, c, d, ...)   MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_7(__VA_ARGS__)
#define MODEM_CLOCKS_12(a, b, c, d, ...)   MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_8(__VA_ARGS__)
#define MODEM_CLOCKS_13(a, b, c, d, ...)   MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_9(__VA_ARGS__)
#define MODEM_CLOCKS_14(a, b, c, d, ...)   MODEM_CLOCKS_4(a, b, c, d) | MODEM_CLOCKS_10(__VA_ARGS__)

#define MODEM_CLOCKS_N(n)                  MODEM_CLOCKS_##n
#define MODEM_CLOCKS_DISPATCH(n, ...)      MODEM_CLOCKS_N(n)(__VA_ARGS__)
#define MODEM_CLOCKS(...) \
    MODEM_CLOCKS_DISPATCH(ESP_VA_NARG(__VA_ARGS__), __VA_ARGS__)

#define MODEM_STATUS_IDLE           (0)
#define MODEM_STATUS_WIFI_INITED    (0x1UL)

// Please define the frequently called modules in the low bit,
// which will improve the execution efficiency
typedef enum {
    MODEM_CLOCK_DEVICE_MIN = 0,
    MODEM_CLOCK_MODEM_ADC_COMMON_FE = MODEM_CLOCK_DEVICE_MIN,
    MODEM_CLOCK_MODEM_PRIVATE_FE,
    MODEM_CLOCK_COEXIST,
    MODEM_CLOCK_I2C_MASTER,
#if SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT
    MODEM_CLOCK_WIFI_APB,
    MODEM_CLOCK_WIFI_BB_44M,
#endif
#if SOC_WIFI_SUPPORTED
    MODEM_CLOCK_WIFI_MAC,
    MODEM_CLOCK_WIFI_BB,
#endif
    MODEM_CLOCK_ETM,
#if SOC_BT_SUPPORTED
    MODEM_CLOCK_BLE_MAC,
#endif
#if SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
    MODEM_CLOCK_BT_I154_COMMON_BB,
#endif
#if SOC_IEEE802154_SUPPORTED
    MODEM_CLOCK_802154_MAC,
#endif
    MODEM_CLOCK_DATADUMP,
    MODEM_CLOCK_DEVICE_MAX
} modem_clock_device_t;

/* Low-power clock source index for modules that support LP clock */
typedef enum {
#if SOC_WIFI_SUPPORTED
    MODEM_LPCLK_SRC_MODULE_WIFI,
#endif
#if SOC_BT_SUPPORTED
    MODEM_LPCLK_SRC_MODULE_BT,
#endif
    MODEM_LPCLK_SRC_MODULE_COEX,
    MODEM_LPCLK_SRC_MODULE_MAX
} modem_lpclk_src_module_t;

typedef struct modem_clock_context modem_clock_context_t;
typedef void (*wrapper_t)(struct modem_clock_context *, bool, void (*action)(struct modem_clock_context *, bool), int16_t *, uint16_t *);
typedef void (*configure_func_t)(struct modem_clock_context *, int, bool, wrapper_t);
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
typedef esp_err_t (*check_func_t)(struct modem_clock_context *, int);
#endif

#define REFS_FL_WITH_REFCNT (BIT(0)) /* Enable reference count management (true=use refs, false=ignore refs) */
typedef struct {
    struct {
        int16_t     count;   /* Reference count for this device, if with_refcnt is enabled */
        uint16_t    flags;   /* flags for this device */
    } refs[MODEM_CLOCK_DEVICE_MAX];
    configure_func_t configure;
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
    check_func_t check;
#endif
} modem_clock_device_context_t;

typedef struct modem_clock_context {
    modem_clock_hal_context_t     *hal;
    spinlock_t                    lock;
    modem_clock_device_context_t  *dev;
#if SOC_PM_SUPPORT_MODEM_CLOCK_DOMAIN_ICG
    uint8_t *icg_config;
#endif
    /* the low-power clock source for each module */
    modem_clock_lpclk_src_t lpclk_src[MODEM_LPCLK_SRC_MODULE_MAX];
#if SOC_WIFI_SUPPORTED
    uint32_t modem_status;
#endif
} modem_clock_context_t;

/**
 * @brief Get module clock dependencies
 *
 * @param module The modem module
 * @return Bitmask of clock dependencies
 */
uint32_t modem_clock_get_module_deps(shared_periph_module_t module);

/**
 * @brief Get modem clock device context
 *
 * @return Pointer to the modem clock device context
 */
modem_clock_device_context_t *modem_clock_device_context(void);

#if SOC_PM_SUPPORT_MODEM_CLOCK_DOMAIN_ICG

/* the ICG code's bit 0, 1 and 2 indicates the ICG state
 * of pmu SLEEP, MODEM and ACTIVE mode respectively */
#define ICG_NOGATING_ACTIVE (BIT(PMU_HP_ICG_MODEM_CODE_ACTIVE))
#define ICG_NOGATING_SLEEP  (BIT(PMU_HP_ICG_MODEM_CODE_SLEEP))
#define ICG_NOGATING_MODEM  (BIT(PMU_HP_ICG_MODEM_CODE_MODEM))

/**
 * @brief Get initial gating configuration for each clock domain
 *
 * @return Pointer to the initial gating configuration array
 */
uint8_t *modem_clock_domain_icg_config(void);

#endif // SOC_PM_SUPPORT_MODEM_CLOCK_DOMAIN_ICG

#if SOC_BLE_USE_WIFI_PWR_CLK_WORKAROUND
/**
 * @brief Apply or clear BT low-power clock workaround for enabling Wi-Fi power clock
 *
 * @param ctx    modem clock context
 * @param select true to apply (ungate); false to clear (gate)
 * @param src    lowpower clock source
 */
void modem_clock_bt_wifipwr_clk_workaround(modem_clock_context_t *ctx, bool select, modem_clock_lpclk_src_t src);
#endif

#if SOC_MODEM_CLOCK_BLE_RTC_TIMER_WORKAROUND
/**
 * @brief BLE RTC timer LPCLK workaround on ESP32H2
 *
 * @param ctx    modem clock context
 * @param enable true to enable rc32k; false to disable rc32k
 * @param src    lowpower clock source
 */
void modem_clock_select_ble_rtc_timer_clk_workaround(modem_clock_context_t *ctx, bool enable, modem_clock_lpclk_src_t src);

#endif
#endif // SOC_MODEM_CLOCK_SUPPORTED

#ifdef __cplusplus
}
#endif
