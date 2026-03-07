/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_private/periph_ctrl.h"
#include "soc/soc_caps.h"
#ifdef __PERIPH_CTRL_ALLOW_LEGACY_API
#include "hal/clk_gate_ll.h"
#endif
#include "esp_log.h"

#if SOC_MODEM_CLOCK_IS_INDEPENDENT && SOC_MODEM_CLOCK_SUPPORTED
#include "esp_private/esp_modem_clock.h"
#endif

/// @brief For simplicity and backward compatible, we are using the same spin lock for both bus clock on/off and reset
/// @note  We may want to split them into two spin locks in the future
static portMUX_TYPE __attribute__((unused)) periph_spinlock = portMUX_INITIALIZER_UNLOCKED;

static uint8_t ref_counts[PERIPH_MODULE_MAX] = {0};

void periph_rcc_enter(void)
{
    portENTER_CRITICAL_SAFE(&periph_spinlock);
}

void periph_rcc_exit(void)
{
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
}

uint8_t periph_rcc_acquire_enter(periph_module_t periph)
{
    periph_rcc_enter();
    return ref_counts[periph];
}

void periph_rcc_acquire_exit(periph_module_t periph, uint8_t ref_count)
{
    ref_counts[periph] = ++ref_count;
    periph_rcc_exit();
}

uint8_t periph_rcc_release_enter(periph_module_t periph)
{
    periph_rcc_enter();
    return ref_counts[periph] - 1;
}

void periph_rcc_release_exit(periph_module_t periph, uint8_t ref_count)
{
    ref_counts[periph] = ref_count;
    periph_rcc_exit();
}

void periph_module_enable(periph_module_t periph)
{
#ifdef __PERIPH_CTRL_ALLOW_LEGACY_API
    assert(periph < PERIPH_MODULE_MAX);
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    if (ref_counts[periph] == 0) {
        periph_ll_enable_clk_clear_rst(periph);
    }
    ref_counts[periph]++;
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}

void periph_module_disable(periph_module_t periph)
{
#ifdef __PERIPH_CTRL_ALLOW_LEGACY_API
    assert(periph < PERIPH_MODULE_MAX);
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    ref_counts[periph]--;
    if (ref_counts[periph] == 0) {
        periph_ll_disable_clk_set_rst(periph);
    }
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}

void periph_module_reset(periph_module_t periph)
{
#ifdef __PERIPH_CTRL_ALLOW_LEGACY_API
    assert(periph < PERIPH_MODULE_MAX);
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    periph_ll_reset(periph);
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}

#if !SOC_IEEE802154_BLE_ONLY
#if SOC_BT_SUPPORTED || SOC_WIFI_SUPPORTED || SOC_IEEE802154_SUPPORTED
IRAM_ATTR void wifi_bt_common_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_PHY_MODULE);
#else
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_enable_clk();
    }
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]++;
#if CONFIG_IDF_TARGET_ESP32C2
    if (ref_counts[PERIPH_COEX_MODULE] == 0) {
        periph_ll_coex_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_COEX_MODULE]++;
#endif
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}

IRAM_ATTR void wifi_bt_common_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_PHY_MODULE);
#else
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]--;
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_disable_clk();
    }
#if CONFIG_IDF_TARGET_ESP32C2
    ref_counts[PERIPH_COEX_MODULE]--;
    if (ref_counts[PERIPH_COEX_MODULE] == 0) {
        periph_ll_coex_module_disable_clk_set_rst();
    }
#endif
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}
#endif  //#if SOC_BT_SUPPORTED || SOC_WIFI_SUPPORTED
#endif  //#if !SOC_IEEE802154_BLE_ONLY

#if CONFIG_ESP_WIFI_ENABLED
void wifi_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_WIFI_MODULE);
#else
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    if (ref_counts[PERIPH_WIFI_MODULE] == 0) {
        periph_ll_wifi_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_WIFI_MODULE]++;
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}

void wifi_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_WIFI_MODULE);
#else
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    ref_counts[PERIPH_WIFI_MODULE]--;
    if (ref_counts[PERIPH_WIFI_MODULE] == 0) {
        periph_ll_wifi_module_disable_clk_set_rst();
    }
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}
#endif // CONFIG_ESP_WIFI_ENABLED

#if SOC_BT_SUPPORTED || SOC_WIFI_SUPPORTED || SOC_IEEE802154_SUPPORTED
// PERIPH_WIFI_BT_COMMON_MODULE is enabled outside
IRAM_ATTR void phy_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_PHY_CALIBRATION_MODULE);
#else
    portENTER_CRITICAL_SAFE(&periph_spinlock);
#if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED
    periph_ll_phy_calibration_module_enable_clk_clear_rst();
    if (ref_counts[PERIPH_RNG_MODULE] == 0) {
        periph_ll_enable_clk_clear_rst(PERIPH_RNG_MODULE);
    }
    ref_counts[PERIPH_RNG_MODULE]++;
#endif
#if SOC_WIFI_SUPPORTED
    if (ref_counts[PERIPH_WIFI_MODULE] == 0) {
        periph_ll_wifi_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_WIFI_MODULE]++;
#endif
#if SOC_BT_SUPPORTED
    if (ref_counts[PERIPH_BT_MODULE] == 0) {
        periph_ll_enable_clk_clear_rst(PERIPH_BT_MODULE);
    }
    ref_counts[PERIPH_BT_MODULE]++;
#endif
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}

// PERIPH_WIFI_BT_COMMON_MODULE is disabled outside
IRAM_ATTR void phy_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_PHY_CALIBRATION_MODULE);
#else
    portENTER_CRITICAL_SAFE(&periph_spinlock);
#if SOC_BT_SUPPORTED
    ref_counts[PERIPH_BT_MODULE]--;
    if (ref_counts[PERIPH_BT_MODULE] == 0) {
        periph_ll_disable_clk_set_rst(PERIPH_BT_MODULE);
    }
#endif
#if SOC_WIFI_SUPPORTED
    ref_counts[PERIPH_WIFI_MODULE]--;
    if (ref_counts[PERIPH_WIFI_MODULE] == 0) {
        periph_ll_wifi_module_disable_clk_set_rst();
    }
#endif
#if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED
    // Do not disable PHY clock and RNG clock
    ref_counts[PERIPH_RNG_MODULE]--;
#endif
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}

IRAM_ATTR bool phy_module_has_clock_bits(uint32_t mask)
{
    uint32_t val = 0;
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    val = modem_clock_module_bits_get(PERIPH_PHY_MODULE);
#else
#if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED
    val = DPORT_REG_READ(periph_ll_get_clk_en_reg(PERIPH_WIFI_BT_COMMON_MODULE));
#else
    return true;
#endif
#endif
    if ((val & mask) != mask) {
        ESP_LOGW("periph_ctrl", "phy module clock bits 0x%x, required 0x%x", val, mask);
        return false;
    }
    return true;
}

IRAM_ATTR void coex_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_COEX_MODULE);
#else
    portENTER_CRITICAL_SAFE(&periph_spinlock);
#if CONFIG_IDF_TARGET_ESP32C2
    if (ref_counts[PERIPH_COEX_MODULE] == 0) {
        periph_ll_coex_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_COEX_MODULE]++;
#endif
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}

IRAM_ATTR void coex_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_COEX_MODULE);
#else
    portENTER_CRITICAL_SAFE(&periph_spinlock);
#if CONFIG_IDF_TARGET_ESP32C2
    ref_counts[PERIPH_COEX_MODULE]--;
    if (ref_counts[PERIPH_COEX_MODULE] == 0) {
        periph_ll_coex_module_disable_clk_set_rst();
    }
#endif
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
#endif
}
#endif  //#if SOC_BT_SUPPORTED || SOC_WIFI_SUPPORTED || SOC_IEEE802154_SUPPORTED
