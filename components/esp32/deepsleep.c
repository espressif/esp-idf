/* Wake from deep sleep stub

   See esp_deepsleep.h esp_wake_deep_sleep() comments for details.
*/
#include <stddef.h>
#include <sys/lock.h>
#include "rom/cache.h"
#include "rom/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/dport_reg.h"
#include "esp_attr.h"
#include "esp_deepsleep.h"
#include "rtc.h"

/* Updating RTC_MEMORY_CRC_REG register via set_rtc_memory_crc()
   is not thread-safe. */
static _lock_t lock_rtc_memory_crc;

esp_deep_sleep_wake_stub_fn_t esp_get_deep_sleep_wake_stub(void)
{
    _lock_acquire(&lock_rtc_memory_crc);
    uint32_t stored_crc = REG_READ(RTC_MEMORY_CRC_REG);
    set_rtc_memory_crc();
    uint32_t calc_crc = REG_READ(RTC_MEMORY_CRC_REG);
    REG_WRITE(RTC_MEMORY_CRC_REG, stored_crc);
    _lock_release(&lock_rtc_memory_crc);

    if(stored_crc == calc_crc) {
        return (esp_deep_sleep_wake_stub_fn_t)REG_READ(RTC_ENTRY_ADDR_REG);
    } else {
        return NULL;
    }
}

void esp_set_deep_sleep_wake_stub(esp_deep_sleep_wake_stub_fn_t new_stub)
{
    _lock_acquire(&lock_rtc_memory_crc);
    REG_WRITE(RTC_ENTRY_ADDR_REG, (uint32_t)new_stub);
    set_rtc_memory_crc();
    _lock_release(&lock_rtc_memory_crc);
}

void RTC_IRAM_ATTR esp_default_wake_deep_sleep(void) {
    /* Clear MMU for CPU 0 */
    REG_SET_BIT(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MMU_IA_CLR);
    REG_CLR_BIT(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MMU_IA_CLR);
}

void __attribute__((weak, alias("esp_default_wake_deep_sleep"))) esp_wake_deep_sleep(void);

void esp_deep_sleep(uint64_t time_in_us)
{
    rtc_set_cpu_freq(CPU_XTAL);
    if (esp_get_deep_sleep_wake_stub() == NULL) {
        esp_set_deep_sleep_wake_stub(esp_wake_deep_sleep);
    }
    uint32_t period = rtc_slowck_cali(CALI_RTC_MUX, 128);
    uint32_t cycle_l, cycle_h;
    rtc_usec2rtc(time_in_us >> 32, time_in_us, period, &cycle_h, &cycle_l);
    rtc_slp_prep_lite(1, 0);
    rtc_sleep(cycle_h, cycle_l, TIMER_EXPIRE_EN, 0);
    while (1) {
        ;
    }
}

void system_deep_sleep(uint64_t) __attribute__((alias("esp_deep_sleep")));
