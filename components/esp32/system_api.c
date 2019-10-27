// Copyright 2013-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>

#include "esp_system.h"
#include "esp_attr.h"
#include "esp_wifi.h"
#include "esp_private/wifi.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp32/rom/efuse.h"
#include "esp32/rom/cache.h"
#include "esp32/rom/uart.h"
#include "soc/dport_reg.h"
#include "soc/gpio_periph.h"
#include "soc/efuse_periph.h"
#include "soc/rtc_periph.h"
#include "soc/timer_periph.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/rtc_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "esp_heap_caps.h"
#include "esp_private/system_internal.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "hal/timer_ll.h"

static const char* TAG = "system_api";

static uint8_t base_mac_addr[6] = { 0 };

#define SHUTDOWN_HANDLERS_NO 2
static shutdown_handler_t shutdown_handlers[SHUTDOWN_HANDLERS_NO];

esp_err_t esp_base_mac_addr_set(uint8_t *mac)
{
    if (mac == NULL) {
        ESP_LOGE(TAG, "Base MAC address is NULL");
        abort();
    }

    memcpy(base_mac_addr, mac, 6);

    return ESP_OK;
}

esp_err_t esp_base_mac_addr_get(uint8_t *mac)
{
    uint8_t null_mac[6] = {0};

    if (memcmp(base_mac_addr, null_mac, 6) == 0) {
        ESP_LOGI(TAG, "Base MAC address is not set, read default base MAC address from BLK0 of EFUSE");
        return ESP_ERR_INVALID_MAC;
    }

    memcpy(mac, base_mac_addr, 6);

    return ESP_OK;
}

esp_err_t esp_efuse_mac_get_custom(uint8_t *mac)
{
    uint8_t version;
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_CUSTOM_VER, &version, 8);
    if (version != 1) {
        ESP_LOGE(TAG, "Base MAC address from BLK3 of EFUSE version error, version = %d", version);
        return ESP_ERR_INVALID_VERSION;
    }

    uint8_t efuse_crc;
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_CUSTOM, mac, 48);
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_CUSTOM_CRC, &efuse_crc, 8);
    uint8_t calc_crc = esp_crc8(mac, 6);

    if (efuse_crc != calc_crc) {
        ESP_LOGE(TAG, "Base MAC address from BLK3 of EFUSE CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
        return ESP_ERR_INVALID_CRC;
    }
    return ESP_OK;
}

esp_err_t esp_efuse_mac_get_default(uint8_t* mac)
{
    uint8_t efuse_crc;
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, mac, 48);
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY_CRC, &efuse_crc, 8);
    uint8_t calc_crc = esp_crc8(mac, 6);

    if (efuse_crc != calc_crc) {
         // Small range of MAC addresses are accepted even if CRC is invalid.
         // These addresses are reserved for Espressif internal use.
        uint32_t mac_high = ((uint32_t)mac[0] << 8) | mac[1];
        if ((mac_high & 0xFFFF) == 0x18fe) {
            uint32_t mac_low = ((uint32_t)mac[2] << 24) | ((uint32_t)mac[3] << 16) | ((uint32_t)mac[4] << 8) | mac[5];
            if ((mac_low >= 0x346a85c7) && (mac_low <= 0x346a85f8)) {
                return ESP_OK;
            }
        } else {
            ESP_LOGE(TAG, "Base MAC address from BLK0 of EFUSE CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
            abort();
        }
    }
    return ESP_OK;
}

esp_err_t esp_derive_local_mac(uint8_t* local_mac, const uint8_t* universal_mac)
{
    uint8_t idx;

    if (local_mac == NULL || universal_mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(local_mac, universal_mac, 6);
    for (idx = 0; idx < 64; idx++) {
        local_mac[0] = universal_mac[0] | 0x02;
        local_mac[0] ^= idx << 2;

        if (memcmp(local_mac, universal_mac, 6)) {
            break;
        }
    }

    return ESP_OK;
}

esp_err_t esp_read_mac(uint8_t* mac, esp_mac_type_t type)
{
    uint8_t efuse_mac[6];

    if (mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (type < ESP_MAC_WIFI_STA || type > ESP_MAC_ETH) {
        ESP_LOGE(TAG, "mac type is incorrect");
        return ESP_ERR_INVALID_ARG;
    }

    _Static_assert(UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR \
            || UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR, \
            "incorrect NUM_MAC_ADDRESS_FROM_EFUSE value");

    if (esp_base_mac_addr_get(efuse_mac) != ESP_OK) {
        esp_efuse_mac_get_default(efuse_mac);
    }

    switch (type) {
    case ESP_MAC_WIFI_STA:
        memcpy(mac, efuse_mac, 6);
        break;
    case ESP_MAC_WIFI_SOFTAP:
        if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
            memcpy(mac, efuse_mac, 6);
            mac[5] += 1;
        }
        else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
            esp_derive_local_mac(mac, efuse_mac);
        }
        break;
    case ESP_MAC_BT:
        memcpy(mac, efuse_mac, 6);
        if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
            mac[5] += 2;
        }
        else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
            mac[5] += 1;
        }
        break;
    case ESP_MAC_ETH:
        if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
            memcpy(mac, efuse_mac, 6);
            mac[5] += 3;
        }
        else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
            efuse_mac[5] += 1;
            esp_derive_local_mac(mac, efuse_mac);
        }
        break;
    default:
        ESP_LOGW(TAG, "incorrect mac type");
        break;
    }

    return ESP_OK;
}

esp_err_t esp_register_shutdown_handler(shutdown_handler_t handler)
{
    for (int i = 0; i < SHUTDOWN_HANDLERS_NO; i++) {
        if (shutdown_handlers[i] == handler) {
            return ESP_ERR_INVALID_STATE;
        } else if (shutdown_handlers[i] == NULL) {
            shutdown_handlers[i] = handler;
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_unregister_shutdown_handler(shutdown_handler_t handler)
{
    for (int i = 0; i < SHUTDOWN_HANDLERS_NO; i++) {
        if (shutdown_handlers[i] == handler) {
            shutdown_handlers[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

void esp_restart_noos(void) __attribute__ ((noreturn));

void IRAM_ATTR esp_restart(void)
{
    for (int i = SHUTDOWN_HANDLERS_NO - 1; i >= 0; i--) {
        if (shutdown_handlers[i]) {
            shutdown_handlers[i]();
        }
    }

    // Disable scheduler on this core.
    vTaskSuspendAll();

    esp_restart_noos();
}

/* "inner" restart function for after RTOS, interrupts & anything else on this
 * core are already stopped. Stalls other core, resets hardware,
 * triggers restart.
*/
void IRAM_ATTR esp_restart_noos(void)
{
    // Disable interrupts
    xt_ints_off(0xFFFFFFFF);

    // Enable RTC watchdog for 1 second
    rtc_wdt_protect_off();
    rtc_wdt_disable();
    rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_RTC);
    rtc_wdt_set_stage(RTC_WDT_STAGE1, RTC_WDT_STAGE_ACTION_RESET_SYSTEM);
    rtc_wdt_set_length_of_reset_signal(RTC_WDT_SYS_RESET_SIG, RTC_WDT_LENGTH_200ns);
    rtc_wdt_set_length_of_reset_signal(RTC_WDT_CPU_RESET_SIG, RTC_WDT_LENGTH_200ns);
    rtc_wdt_set_time(RTC_WDT_STAGE0, 1000);
    rtc_wdt_flashboot_mode_enable();

    // Reset and stall the other CPU.
    // CPU must be reset before stalling, in case it was running a s32c1i
    // instruction. This would cause memory pool to be locked by arbiter
    // to the stalled CPU, preventing current CPU from accessing this pool.
    const uint32_t core_id = xPortGetCoreID();
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_cpu_reset(other_core_id);
    esp_cpu_stall(other_core_id);

    // Other core is now stalled, can access DPORT registers directly
    esp_dport_access_int_abort();

    // Disable TG0/TG1 watchdogs
    timer_ll_wdt_set_protect(&TIMERG0, false);
    timer_ll_wdt_set_enable(&TIMERG0, false);
    timer_ll_wdt_set_protect(&TIMERG0, true);

    timer_ll_wdt_set_protect(&TIMERG1, false);
    timer_ll_wdt_set_enable(&TIMERG1, false);
    timer_ll_wdt_set_protect(&TIMERG1, true);

    // Flush any data left in UART FIFOs
    uart_tx_wait_idle(0);
    uart_tx_wait_idle(1);
    uart_tx_wait_idle(2);

    // Disable cache
    Cache_Read_Disable(0);
    Cache_Read_Disable(1);

    // 2nd stage bootloader reconfigures SPI flash signals.
    // Reset them to the defaults expected by ROM.
    WRITE_PERI_REG(GPIO_FUNC0_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC1_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC2_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC3_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC4_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC5_IN_SEL_CFG_REG, 0x30);

    // Reset wifi/bluetooth/ethernet/sdio (bb/mac)
    DPORT_SET_PERI_REG_MASK(DPORT_CORE_RST_EN_REG,
         DPORT_BB_RST | DPORT_FE_RST | DPORT_MAC_RST |
         DPORT_BT_RST | DPORT_BTMAC_RST | DPORT_SDIO_RST |
         DPORT_SDIO_HOST_RST | DPORT_EMAC_RST | DPORT_MACPWR_RST |
         DPORT_RW_BTMAC_RST | DPORT_RW_BTLP_RST);
    DPORT_REG_WRITE(DPORT_CORE_RST_EN_REG, 0);

    // Reset timer/spi/uart
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,
            DPORT_TIMERS_RST | DPORT_SPI01_RST | DPORT_UART_RST | DPORT_UART1_RST | DPORT_UART2_RST);
    DPORT_REG_WRITE(DPORT_PERIP_RST_EN_REG, 0);

    // Set CPU back to XTAL source, no PLL, same as hard reset
    rtc_clk_cpu_freq_set_xtal();

    // Clear entry point for APP CPU
    DPORT_REG_WRITE(DPORT_APPCPU_CTRL_D_REG, 0);

    // Reset CPUs
    if (core_id == 0) {
        // Running on PRO CPU: APP CPU is stalled. Can reset both CPUs.
        esp_cpu_reset(1);
        esp_cpu_reset(0);
    } else {
        // Running on APP CPU: need to reset PRO CPU and unstall it,
        // then reset APP CPU
        esp_cpu_reset(0);
        esp_cpu_unstall(0);
        esp_cpu_reset(1);
    }
    while(true) {
        ;
    }
}

uint32_t esp_get_free_heap_size( void )
{
    return heap_caps_get_free_size( MALLOC_CAP_DEFAULT );
}

uint32_t esp_get_minimum_free_heap_size( void )
{
    return heap_caps_get_minimum_free_size( MALLOC_CAP_DEFAULT );
}

const char* esp_get_idf_version(void)
{
    return IDF_VER;
}

void esp_chip_info(esp_chip_info_t* out_info)
{
    uint32_t efuse_rd3 = REG_READ(EFUSE_BLK0_RDATA3_REG);
    memset(out_info, 0, sizeof(*out_info));

    out_info->model = CHIP_ESP32;
    out_info->revision = esp_efuse_get_chip_ver();

    if ((efuse_rd3 & EFUSE_RD_CHIP_VER_DIS_APP_CPU_M) == 0) {
        out_info->cores = 2;
    } else {
        out_info->cores = 1;
    }
    out_info->features = CHIP_FEATURE_WIFI_BGN;
    if ((efuse_rd3 & EFUSE_RD_CHIP_VER_DIS_BT_M) == 0) {
        out_info->features |= CHIP_FEATURE_BT | CHIP_FEATURE_BLE;
    }
    int package = (efuse_rd3 & EFUSE_RD_CHIP_VER_PKG_M) >> EFUSE_RD_CHIP_VER_PKG_S;
    if (package == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5 ||
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2 ||
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4) {
        out_info->features |= CHIP_FEATURE_EMB_FLASH;
    }
}
