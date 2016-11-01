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
