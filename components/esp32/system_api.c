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

#include "esp_system.h"
#include "esp_attr.h"
#include "esp_wifi.h"
#include "esp_wifi_internal.h"
#include "esp_log.h"
#include "rom/efuse.h"
#include "rom/cache.h"
#include "rom/uart.h"
#include "soc/dport_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/cpu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"

static const char* TAG = "system_api";

void system_init()
{
}

esp_err_t esp_efuse_read_mac(uint8_t* mac)
{
    uint8_t efuse_crc;
    uint8_t calc_crc;
    uint32_t mac_low = REG_READ(EFUSE_BLK0_RDATA1_REG);
    uint32_t mac_high = REG_READ(EFUSE_BLK0_RDATA2_REG);

    mac[0] = mac_high >> 8;
    mac[1] = mac_high;
    mac[2] = mac_low >> 24;
    mac[3] = mac_low >> 16;
    mac[4] = mac_low >> 8;
    mac[5] = mac_low;

    efuse_crc = mac_high >> 16;
    calc_crc = esp_crc8(mac, 6);

    if (efuse_crc != calc_crc) {
         // Small range of MAC addresses are accepted even if CRC is invalid.
         // These addresses are reserved for Espressif internal use.
        if ((mac_high & 0xFFFF) == 0x18fe) {
            if ((mac_low >= 0x346a85c7) && (mac_low <= 0x346a85f8)) {
                return ESP_OK;
            }
        } else {
            ESP_LOGE(TAG, "MAC address CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
            abort();
        }
    }
    return ESP_OK;
}

esp_err_t system_efuse_read_mac(uint8_t mac[6]) __attribute__((alias("esp_efuse_read_mac")));


void IRAM_ATTR esp_restart(void)
{
    esp_wifi_stop();

    // Disable scheduler on this core.
    vTaskSuspendAll();
    const uint32_t core_id = xPortGetCoreID();
    const uint32_t other_core_id = core_id == 0 ? 1 : 0;
    esp_cpu_stall(other_core_id);

    // We need to disable TG0/TG1 watchdogs
    // First enable RTC watchdog to be on the safe side
    REG_WRITE(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    REG_WRITE(RTC_CNTL_WDTCONFIG0_REG,
            RTC_CNTL_WDT_FLASHBOOT_MOD_EN_M |
            (1 << RTC_CNTL_WDT_SYS_RESET_LENGTH_S) |
            (1 << RTC_CNTL_WDT_CPU_RESET_LENGTH_S) );
    REG_WRITE(RTC_CNTL_WDTCONFIG1_REG, 128000);

    // Disable TG0/TG1 watchdogs
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_config0.en = 0;
    TIMERG0.wdt_wprotect=0;
    TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG1.wdt_config0.en = 0;
    TIMERG1.wdt_wprotect=0;

    // Disable all interrupts
    xt_ints_off(0xFFFFFFFF);

    // Disable cache
    Cache_Read_Disable(0);
    Cache_Read_Disable(1);

    // Flush any data left in UART FIFOs
    uart_tx_wait_idle(0);
    uart_tx_wait_idle(1);
    uart_tx_wait_idle(2);

    // Reset wifi/bluetooth (bb/mac)
    SET_PERI_REG_MASK(DPORT_WIFI_RST_EN_REG, 0x1f);
    REG_WRITE(DPORT_WIFI_RST_EN_REG, 0);

    // Reset timer/spi/uart
    SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,
            DPORT_TIMERS_RST | DPORT_SPI_RST_1 | DPORT_UART_RST);
    REG_WRITE(DPORT_PERIP_RST_EN_REG, 0);

    // Reset CPUs
    if (core_id == 0) {
        // Running on PRO CPU: APP CPU is stalled. Can reset both CPUs.
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG,
                RTC_CNTL_SW_PROCPU_RST_M | RTC_CNTL_SW_APPCPU_RST_M);
    } else {
        // Running on APP CPU: need to reset PRO CPU and unstall it,
        // then stall APP CPU
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_PROCPU_RST_M);
        esp_cpu_unstall(0);
        esp_cpu_stall(1);
    }
    while(true) {
        ;
    }
}

void system_restart(void) __attribute__((alias("esp_restart")));

void system_restore(void)
{
    esp_wifi_restore();
}

uint32_t esp_get_free_heap_size(void)
{
    return xPortGetFreeHeapSize();
}

uint32_t system_get_free_heap_size(void) __attribute__((alias("esp_get_free_heap_size")));

const char* system_get_sdk_version(void)
{
    return "master";
}


