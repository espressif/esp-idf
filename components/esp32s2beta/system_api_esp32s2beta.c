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
#include "esp_wifi.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp32s2beta/rom/cache.h"
#include "esp32s2beta/rom/uart.h"
#include "soc/dport_reg.h"
#include "soc/gpio_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/rtc_wdt.h"
#include "soc/syscon_reg.h"
#include "freertos/xtensa_api.h"

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
#if !CONFIG_FREERTOS_UNICORE
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_cpu_reset(other_core_id);
    esp_cpu_stall(other_core_id);
#endif

    // Disable TG0/TG1 watchdogs
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_config0.en = 0;
    TIMERG0.wdt_wprotect=0;
    TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG1.wdt_config0.en = 0;
    TIMERG1.wdt_wprotect=0;

    // Flush any data left in UART FIFOs
    uart_tx_wait_idle(0);
    uart_tx_wait_idle(1);
    // Disable cache
    Cache_Disable_ICache();
    Cache_Disable_DCache();

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
            DPORT_TIMERS_RST | DPORT_SPI01_RST | DPORT_UART_RST);
    DPORT_REG_WRITE(DPORT_PERIP_RST_EN_REG, 0);

    // Set CPU back to XTAL source, no PLL, same as hard reset
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_XTAL);

#if !CONFIG_FREERTOS_UNICORE
    // Clear entry point for APP CPU
    DPORT_REG_WRITE(DPORT_APPCPU_CTRL_D_REG, 0);
#endif

    // Reset CPUs
    if (core_id == 0) {
        // Running on PRO CPU: APP CPU is stalled. Can reset both CPUs.
#if !CONFIG_FREERTOS_UNICORE
        esp_cpu_reset(1);
#endif
        esp_cpu_reset(0);
    }
#if !CONFIG_FREERTOS_UNICORE
      else {
        // Running on APP CPU: need to reset PRO CPU and unstall it,
        // then reset APP CPU
        esp_cpu_reset(0);
        esp_cpu_unstall(0);
        esp_cpu_reset(1);
    }
#endif
    while(true) {
        ;
    }
}

void esp_chip_info(esp_chip_info_t* out_info)
{
    memset(out_info, 0, sizeof(*out_info));

    out_info->model = CHIP_ESP32S2BETA;
    out_info->cores = 1;
    out_info->features = CHIP_FEATURE_WIFI_BGN;

    // FIXME: other features?
}
