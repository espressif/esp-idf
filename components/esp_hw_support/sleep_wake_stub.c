/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_sleep.h"

#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"
#include "hal/rtc_cntl_ll.h"
#include "hal/uart_ll.h"

#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/rtc.h"
#endif

void RTC_IRAM_ATTR esp_wake_stub_sleep(esp_deep_sleep_wake_stub_fn_t new_stub)
{
#if SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY
    extern char _rtc_text_start[];
    #if CONFIG_ESP32S3_RTCDATA_IN_FAST_MEM
    extern char _rtc_noinit_end[];
    size_t rtc_fast_length = (size_t)_rtc_noinit_end - (size_t)_rtc_text_start;
    #else
    extern char _rtc_force_fast_end[];
    size_t rtc_fast_length = (size_t)_rtc_force_fast_end - (size_t)_rtc_text_start;
    #endif // CONFIG_ESP32S3_RTCDATA_IN_FAST_MEM
    esp_rom_set_rtc_wake_addr((esp_rom_wake_func_t)new_stub, rtc_fast_length);
#else
    // Set the pointer of the wake stub function.
    REG_WRITE(RTC_ENTRY_ADDR_REG, (uint32_t)new_stub);
    set_rtc_memory_crc();
#endif // SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_MEM

    // Go to sleep.
    rtc_cntl_ll_sleep_enable();
    // A few CPU cycles may be necessary for the sleep to start...
    while (true) {};
    // never reaches here.
}

void RTC_IRAM_ATTR esp_wake_stub_uart_tx_wait_idle(uint8_t uart_no)
{
    while (!uart_ll_is_tx_idle(UART_LL_GET_HW(uart_no))) {};
}

void RTC_IRAM_ATTR esp_wake_stub_set_wakeup_time(uint64_t time_in_us)
{
    uint64_t rtc_count_delta = rtc_cntl_ll_time_to_count(time_in_us);
    uint64_t rtc_curr_count = rtc_cntl_ll_get_rtc_time();
    rtc_cntl_ll_set_wakeup_timer(rtc_curr_count + rtc_count_delta);
}

uint32_t RTC_IRAM_ATTR esp_wake_stub_get_wakeup_cause(void)
{
    return rtc_cntl_ll_get_wakeup_cause();
}
