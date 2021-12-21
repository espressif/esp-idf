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
#include "esp_private/system_internal.h"
#include "esp_attr.h"
#include "esp_efuse.h"
#include "esp_wifi.h"
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
#include "hal/wdt_hal.h"
#include "freertos/xtensa_api.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/cache_err_int.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/cache_err_int.h"
#endif


/* "inner" restart function for after RTOS, interrupts & anything else on this
 * core are already stopped. Stalls other core, resets hardware,
 * triggers restart.
*/
void IRAM_ATTR esp_restart_noos(void)
{
    // Disable interrupts
    xt_ints_off(0xFFFFFFFF);

    // Enable RTC watchdog for 1 second
    wdt_hal_context_t rtc_wdt_ctx;
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
    uint32_t stage_timeout_ticks = (uint32_t)(1000ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE1, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_set_flashboot_en(&rtc_wdt_ctx, true);

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

    //Todo: Refactor to use Interrupt or Task Watchdog API, and a system level WDT context
    // Disable TG0/TG1 watchdogs
    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);

    wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};
    wdt_hal_write_protect_disable(&wdt1_context);
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);

    // Flush any data left in UART FIFOs
    uart_tx_wait_idle(0);
    uart_tx_wait_idle(1);
    uart_tx_wait_idle(2);

#ifdef CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
    if (esp_ptr_external_ram(get_sp())) {
        // If stack_addr is from External Memory (CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY is used)
        // then need to switch SP to Internal Memory otherwise
        // we will get the "Cache disabled but cached memory region accessed" error after Cache_Read_Disable.
        uint32_t new_sp = SOC_DRAM_LOW + (SOC_DRAM_HIGH - SOC_DRAM_LOW) / 2;
        SET_STACK(new_sp);
    }
#endif

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
            DPORT_TIMERS_RST | DPORT_SPI01_RST | DPORT_SPI2_RST | DPORT_SPI3_RST | DPORT_SPI_DMA_RST |
            //UART TX FIFO cannot be reset correctly on ESP32, so reset the UART memory by DPORT here.
            DPORT_UART_RST | DPORT_UART1_RST | DPORT_UART2_RST | DPORT_UART_MEM_RST);
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
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4 ||
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302) {
        out_info->features |= CHIP_FEATURE_EMB_FLASH;
    }
}

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
inline bool soc_has_cache_lock_bug(void)
{
    return (esp_efuse_get_chip_ver() == 3);
}
#endif
