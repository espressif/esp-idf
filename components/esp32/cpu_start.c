// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdint.h>
#include <string.h>

#include "esp_attr.h"
#include "esp_err.h"

#include "rom/ets_sys.h"
#include "rom/uart.h"
#include "rom/rtc.h"
#include "rom/cache.h"

#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"

#include "tcpip_adapter.h"

#include "heap_alloc_caps.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_spi_flash.h"
#include "esp_ipc.h"
#include "esp_crosscore_int.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_newlib.h"
#include "esp_brownout.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "esp_phy_init.h"
#include "trax.h"

void start_cpu0(void) __attribute__((weak, alias("start_cpu0_default")));
void start_cpu0_default(void) IRAM_ATTR;
#if !CONFIG_FREERTOS_UNICORE
static void IRAM_ATTR call_start_cpu1();
void start_cpu1(void) __attribute__((weak, alias("start_cpu1_default")));
void start_cpu1_default(void) IRAM_ATTR;
static bool app_cpu_started = false;
#endif //!CONFIG_FREERTOS_UNICORE

static void do_global_ctors(void);
static void do_phy_init();
static void main_task(void* args);
extern void app_main(void);

extern int _bss_start;
extern int _bss_end;
extern int _rtc_bss_start;
extern int _rtc_bss_end;
extern int _init_start;
extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);
extern volatile int port_xSchedulerRunning[2];

static const char* TAG = "cpu_start";

/*
 * We arrive here after the bootloader finished loading the program from flash. The hardware is mostly uninitialized,
 * and the app CPU is in reset. We do have a stack, so we can do the initialization in C.
 */

void IRAM_ATTR call_start_cpu0()
{
    //Kill wdt
    REG_CLR_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_FLASHBOOT_MOD_EN);
    REG_CLR_BIT(0x6001f048, BIT(14)); //DR_REG_BB_BASE+48

    cpu_configure_region_protection();

    //Move exception vectors to IRAM
    asm volatile (\
                  "wsr    %0, vecbase\n" \
                  ::"r"(&_init_start));

    uartAttach();
    ets_install_uart_printf();

    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));

    /* Unless waking from deep sleep (implying RTC memory is intact), clear RTC bss */
    if (rtc_get_reset_reason(0) != DEEPSLEEP_RESET) {
        memset(&_rtc_bss_start, 0, (&_rtc_bss_end - &_rtc_bss_start) * sizeof(_rtc_bss_start));
    }

    // Initialize heap allocator
    heap_alloc_caps_init();

    ESP_EARLY_LOGI(TAG, "Pro cpu up.");

#if !CONFIG_FREERTOS_UNICORE
    ESP_EARLY_LOGI(TAG, "Starting app cpu, entry point is %p", call_start_cpu1);
    //Flush and enable icache for APP CPU
    Cache_Flush(1);
    Cache_Read_Enable(1);
    //Un-stall the app cpu; the panic handler may have stalled it.
    CLEAR_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, RTC_CNTL_SW_STALL_APPCPU_C1_M);
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_STALL_APPCPU_C0_M);
    //Enable clock gating and reset the app cpu.
    SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN);
    CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_C_REG, DPORT_APPCPU_RUNSTALL);
    SET_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
    CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_A_REG, DPORT_APPCPU_RESETTING);
    ets_set_appcpu_boot_addr((uint32_t)call_start_cpu1);

    while (!app_cpu_started) {
        ets_delay_us(100);
    }
#else
    ESP_EARLY_LOGI(TAG, "Single core mode");
    CLEAR_PERI_REG_MASK(DPORT_APPCPU_CTRL_B_REG, DPORT_APPCPU_CLKGATE_EN);
#endif
    ESP_EARLY_LOGI(TAG, "Pro cpu start user code");
    start_cpu0();
}

#if !CONFIG_FREERTOS_UNICORE
void IRAM_ATTR call_start_cpu1()
{
    asm volatile (\
                  "wsr    %0, vecbase\n" \
                  ::"r"(&_init_start));

    cpu_configure_region_protection();

    ESP_EARLY_LOGI(TAG, "App cpu up.");
    app_cpu_started = 1;
    start_cpu1();
}
#endif //!CONFIG_FREERTOS_UNICORE

void start_cpu0_default(void)
{
//Enable trace memory and immediately start trace.
#if CONFIG_MEMMAP_TRACEMEM
#if CONFIG_MEMMAP_TRACEMEM_TWOBANKS
    trax_enable(TRAX_ENA_PRO_APP);
#else
    trax_enable(TRAX_ENA_PRO);
#endif
    trax_start_trace(TRAX_DOWNCOUNT_WORDS);
#endif
    esp_set_cpu_freq();     // set CPU frequency configured in menuconfig
    uart_div_modify(0, (APB_CLK_FREQ << 4) / 115200);
#if CONFIG_BROWNOUT_DET
    esp_brownout_init();
#endif
#if CONFIG_INT_WDT
    esp_int_wdt_init();
#endif
#if CONFIG_TASK_WDT
    esp_task_wdt_init();
#endif
    esp_setup_syscall_table();
    esp_setup_time_syscalls();
    esp_vfs_dev_uart_register();
    esp_reent_init(_GLOBAL_REENT);
    const char* default_uart_dev = "/dev/uart/0";
    _GLOBAL_REENT->_stdin  = fopen(default_uart_dev, "r");
    _GLOBAL_REENT->_stdout = fopen(default_uart_dev, "w");
    _GLOBAL_REENT->_stderr = fopen(default_uart_dev, "w");
    do_global_ctors();
#if !CONFIG_FREERTOS_UNICORE
    esp_crosscore_int_init();
#endif
    esp_ipc_init();
    spi_flash_init();

#if CONFIG_ESP32_PHY_AUTO_INIT
    nvs_flash_init();
    do_phy_init();
#endif

    xTaskCreatePinnedToCore(&main_task, "main",
            ESP_TASK_MAIN_STACK, NULL,
            ESP_TASK_MAIN_PRIO, NULL, 0);
    ESP_LOGI(TAG, "Starting scheduler on PRO CPU.");
    vTaskStartScheduler();
}

#if !CONFIG_FREERTOS_UNICORE
void start_cpu1_default(void)
{
#if CONFIG_MEMMAP_TRACEMEM_TWOBANKS
    trax_start_trace(TRAX_DOWNCOUNT_WORDS);
#endif
    // Wait for FreeRTOS initialization to finish on PRO CPU
    while (port_xSchedulerRunning[0] == 0) {
        ;
    }
    esp_crosscore_int_init();
    ESP_LOGI(TAG, "Starting scheduler on APP CPU.");
    xPortStartScheduler();
}
#endif //!CONFIG_FREERTOS_UNICORE

static void do_global_ctors(void)
{
    void (**p)(void);
    for (p = &__init_array_end - 1; p >= &__init_array_start; --p) {
        (*p)();
    }
}

static void main_task(void* args)
{
    app_main();
    vTaskDelete(NULL);
}

static void do_phy_init()
{
    esp_phy_calibration_mode_t calibration_mode = PHY_RF_CAL_PARTIAL;
    if (rtc_get_reset_reason(0) == DEEPSLEEP_RESET) {
        calibration_mode = PHY_RF_CAL_NONE;
    }
    const esp_phy_init_data_t* init_data = esp_phy_get_init_data();
    if (init_data == NULL) {
        ESP_LOGE(TAG, "failed to obtain PHY init data");
        abort();
    }
    esp_phy_calibration_data_t* cal_data =
            (esp_phy_calibration_data_t*) calloc(sizeof(esp_phy_calibration_data_t), 1);
    if (cal_data == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for RF calibration data");
        abort();
    }
    esp_err_t err = esp_phy_load_cal_data_from_nvs(cal_data);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to load RF calibration data, falling back to full calibration");
        calibration_mode = PHY_RF_CAL_FULL;
    }

    esp_phy_init(init_data, calibration_mode, cal_data);

    if (calibration_mode != PHY_RF_CAL_NONE) {
        err = esp_phy_store_cal_data_to_nvs(cal_data);
    } else {
        err = ESP_OK;
    }
    esp_phy_release_init_data(init_data);
    free(cal_data); // PHY maintains a copy of calibration data, so we can free this
}
