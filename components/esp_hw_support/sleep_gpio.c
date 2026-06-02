/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_memory_utils.h"
#include "soc/soc_caps.h"
#include "soc/uart_pins.h"

#include "sdkconfig.h"

#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "hal/rtc_io_hal.h"
#include "hal/rtc_io_periph.h"
#include "soc/uart_pins.h"

#include "hal/rtc_hal.h"
#if SOC_PMU_SUPPORTED
#include "hal/pmu_ll.h"
#include "esp_private/esp_pmu.h"
#else
#include "soc/rtc.h"
#include "hal/rtc_cntl_ll.h"
#endif

#include "esp_private/esp_gpio_reserve.h"
#include "esp_private/gpio.h"
#include "esp_private/sleep_gpio.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/startup_internal.h"
#include "bootloader_flash.h"

ESP_LOG_ATTR_TAG(TAG, "sleep_gpio");

#if SOC_PM_SUPPORT_EXT0_WAKEUP
#define _IO_WAKEUP_EXT0     RTC_EXT0_TRIG_EN
#else
#define _IO_WAKEUP_EXT0     0
#endif
#if SOC_PM_SUPPORT_EXT1_WAKEUP
#define _IO_WAKEUP_EXT1     RTC_EXT1_TRIG_EN
#else
#define _IO_WAKEUP_EXT1     0
#endif
#define IO_WAKEUP_ENABLED   (RTC_GPIO_TRIG_EN | _IO_WAKEUP_EXT0 | _IO_WAKEUP_EXT1)

#if CONFIG_IDF_TARGET_ESP32
/* On ESP32, for IOs with RTC functionality, setting SLP_PU, SLP_PD couldn't change IO status
 * from FUN_PU, FUN_PD to SLP_PU, SLP_PD at sleep.
 */
typedef struct gpio_slp_mode_cfg {
    volatile uint32_t fun_pu;
    volatile uint32_t fun_pd;
} gpio_slp_mode_cfg_t;

static DRAM_ATTR gpio_slp_mode_cfg_t gpio_cfg = {};

void esp_sleep_gpio_pupd_config_workaround_apply(void)
{
    /* Record fun_pu and fun_pd state in bitmap */
    for (gpio_num_t gpio_num = GPIO_NUM_0; gpio_num < GPIO_NUM_MAX; gpio_num++) {
        int rtcio_num = rtc_io_num_map[gpio_num];
        if (rtcio_num >= 0 && gpio_ll_sleep_sel_is_enabled(&GPIO, gpio_num)) {
            if (rtcio_ll_is_pullup_enabled(rtcio_num)) {
                gpio_cfg.fun_pu |= BIT(rtcio_num);
            } else {
                gpio_cfg.fun_pu &= ~BIT(rtcio_num);
            }
            if (rtcio_ll_is_pulldown_enabled(rtcio_num)) {
                gpio_cfg.fun_pd |= BIT(rtcio_num);
            } else {
                gpio_cfg.fun_pd &= ~BIT(rtcio_num);
            }

            if (gpio_ll_sleep_pullup_is_enabled(&GPIO, gpio_num)) {
                rtcio_ll_pullup_enable(rtcio_num);
            } else {
                rtcio_ll_pullup_disable(rtcio_num);
            }
            if (gpio_ll_sleep_pulldown_is_enabled(&GPIO, gpio_num)) {
                rtcio_ll_pulldown_enable(rtcio_num);
            } else {
                rtcio_ll_pulldown_disable(rtcio_num);
            }
        }
    }
}

void esp_sleep_gpio_pupd_config_workaround_unapply(void)
{
    /* Restore fun_pu and fun_pd state from bitmap */
    for (gpio_num_t gpio_num = GPIO_NUM_0; gpio_num < GPIO_NUM_MAX; gpio_num++) {
        int rtcio_num = rtc_io_num_map[gpio_num];
        if (rtcio_num >= 0 && gpio_ll_sleep_sel_is_enabled(&GPIO, gpio_num)) {
            if (gpio_cfg.fun_pu & BIT(rtcio_num)) {
                rtcio_ll_pullup_enable(rtcio_num);
            } else {
                rtcio_ll_pullup_disable(rtcio_num);
            }
            if (gpio_cfg.fun_pd & BIT(rtcio_num)) {
                rtcio_ll_pulldown_enable(rtcio_num);
            } else {
                rtcio_ll_pulldown_disable(rtcio_num);
            }
        }
    }
}
#endif

#if CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND || CONFIG_PM_SLP_DISABLE_GPIO
void esp_sleep_config_gpio_isolate(void)
{
    ESP_EARLY_LOGI(TAG, "Configure to isolate all GPIO pins in sleep state");
    for (gpio_num_t gpio_num = GPIO_NUM_0; gpio_num < GPIO_NUM_MAX; gpio_num++) {
        if (GPIO_IS_VALID_GPIO(gpio_num)) {
            gpio_sleep_set_direction(gpio_num, GPIO_MODE_DISABLE);
            gpio_sleep_set_pull_mode(gpio_num, GPIO_FLOATING);
        }
    }

#if CONFIG_ESP_SLEEP_MSPI_NEED_ALL_IO_PU && !SOC_MSPI_HAS_INDEPENT_IOMUX
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_CLK), GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_Q),   GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D),   GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_HD),  GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_WP),  GPIO_PULLUP_ONLY);
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    bool octal_mspi_required = bootloader_flash_is_octal_mode_enabled();
#if CONFIG_SPIRAM_MODE_OCT
    octal_mspi_required |= true;
#endif // CONFIG_SPIRAM_MODE_OCT
    if (octal_mspi_required) {
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_DQS), GPIO_PULLUP_ONLY);
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D4),  GPIO_PULLUP_ONLY);
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D5),  GPIO_PULLUP_ONLY);
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D6),  GPIO_PULLUP_ONLY);
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D7),  GPIO_PULLUP_ONLY);
    }
#endif // SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
#endif // CONFIG_ESP_SLEEP_MSPI_NEED_ALL_IO_PU
}

void esp_sleep_enable_gpio_switch(bool enable)
{
    ESP_EARLY_LOGI(TAG, "%s automatic switching of GPIO sleep configuration", enable ? "Enable" : "Disable");

    uint64_t gpio_sleep_sel_dis_mask = 0;
#if CONFIG_ESP_CONSOLE_UART
#if CONFIG_ESP_CONSOLE_UART_CUSTOM
    const gpio_num_t uart_tx_gpio = (CONFIG_ESP_CONSOLE_UART_TX_GPIO >= 0) ? CONFIG_ESP_CONSOLE_UART_TX_GPIO : U0TXD_GPIO_NUM;
    const gpio_num_t uart_rx_gpio = (CONFIG_ESP_CONSOLE_UART_RX_GPIO >= 0) ? CONFIG_ESP_CONSOLE_UART_RX_GPIO : U0RXD_GPIO_NUM;
#else
    const gpio_num_t uart_tx_gpio = U0TXD_GPIO_NUM;
    const gpio_num_t uart_rx_gpio = U0RXD_GPIO_NUM;
#endif
    gpio_sleep_sel_dis_mask |= BIT64(uart_tx_gpio) | BIT64(uart_rx_gpio);
#endif
    /* If the PSRAM is disable in ESP32xx chips equipped with PSRAM, there will be a large current leakage. */
#if CONFIG_ESP_SLEEP_PSRAM_LEAKAGE_WORKAROUND && CONFIG_SPIRAM & !SOC_MSPI_HAS_INDEPENT_IOMUX
    const gpio_num_t psram_cs_gpio = (gpio_num_t)esp_mspi_get_io(ESP_MSPI_IO_CS1);
    if (GPIO_IS_VALID_GPIO(psram_cs_gpio)) {
        gpio_sleep_sel_dis_mask |= BIT64(psram_cs_gpio);
    }
#endif
#if CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND & !SOC_MSPI_HAS_INDEPENT_IOMUX
    const gpio_num_t flash_cs_gpio = (gpio_num_t)esp_mspi_get_io(ESP_MSPI_IO_CS0);
    if (GPIO_IS_VALID_GPIO(flash_cs_gpio)) {
        gpio_sleep_sel_dis_mask |= BIT64(flash_cs_gpio);
    }
#endif

    for (gpio_num_t gpio_num = GPIO_NUM_0; gpio_num < GPIO_NUM_MAX; gpio_num++) {
        if (!GPIO_IS_VALID_GPIO(gpio_num)) {
            continue;
        }
        if (enable && !(gpio_sleep_sel_dis_mask & BIT64(gpio_num))) {
            gpio_sleep_sel_en(gpio_num);
        } else {
            gpio_sleep_sel_dis(gpio_num);
        }
    }
}
#endif

#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP || SOC_GPIO_NEED_SOFT_ISOLATE_DURING_PD
IRAM_ATTR static void sleep_gpio_isolate_one_pin(gpio_num_t gpio_num)
{
#if SOC_GPIO_NEED_SOFT_ISOLATE_DURING_PD
    gpio_io_config_t isolate_config = { .pu = 0, .pd = 0, .ie = 0, .oe = 0, .fun_sel = PIN_FUNC_GPIO };
    gpio_ll_set_pad_config_for_sleep_isolate(gpio_num, &isolate_config);
#else
    gpio_hal_context_t gpio_hal = { .dev = GPIO_HAL_GET_HW(GPIO_PORT_0) };
    gpio_hal_input_disable(&gpio_hal, gpio_num);
    gpio_hal_output_disable(&gpio_hal, gpio_num);
    gpio_hal_pullup_dis(&gpio_hal, gpio_num);
    gpio_hal_pulldown_dis(&gpio_hal, gpio_num);
    gpio_hal_func_sel(&gpio_hal, gpio_num, PIN_FUNC_GPIO);
#endif
}

#if SOC_GPIO_NEED_SOFT_ISOLATE_DURING_PD
typedef struct {
    uint64_t backuped;
    uint64_t pu;
    uint64_t pd;
    uint64_t ie;
    uint64_t oe;
    uint8_t fun_sel[GPIO_NUM_MAX];
} gpio_isolate_backup_t;

static DRAM_ATTR gpio_isolate_backup_t s_gpio_isolate_backup;

IRAM_ATTR static void sleep_gpio_backup_one_pin(gpio_num_t gpio_num)
{
    gpio_hal_context_t gpio_hal = { .dev = GPIO_HAL_GET_HW(GPIO_PORT_0) };
    gpio_io_config_t io_config;
    gpio_ll_backup_pad_config_for_sleep_isolate(gpio_num, &io_config);
    if (io_config.pu) {
        s_gpio_isolate_backup.pu |= (1ULL << gpio_num);
    }
    if (io_config.pd) {
        s_gpio_isolate_backup.pd |= (1ULL << gpio_num);
    }
    if (io_config.ie) {
        s_gpio_isolate_backup.ie |= (1ULL << gpio_num);
    }
    if (gpio_ll_output_is_enabled(gpio_hal.dev, gpio_num)) {
        s_gpio_isolate_backup.oe |= (1ULL << gpio_num);
    }
    s_gpio_isolate_backup.fun_sel[gpio_num] = (uint8_t)io_config.fun_sel;
    s_gpio_isolate_backup.backuped |= (1ULL << gpio_num);
}

IRAM_ATTR void esp_sleep_isolate_mspi_gpio(void)
{
    DRAM_ATTR static const esp_mspi_io_t s_mspi_pins[] = {
        ESP_MSPI_IO_CLK, ESP_MSPI_IO_Q, ESP_MSPI_IO_D, ESP_MSPI_IO_HD, ESP_MSPI_IO_WP
    };
    for (size_t i = 0; i < sizeof(s_mspi_pins) / sizeof(s_mspi_pins[0]); i++) {
        gpio_num_t gpio_num = (gpio_num_t)esp_mspi_get_io(s_mspi_pins[i]);
        sleep_gpio_backup_one_pin(gpio_num);
#if CONFIG_ESP_SLEEP_MSPI_NEED_ALL_IO_PU && !SOC_MSPI_HAS_INDEPENT_IOMUX
        gpio_hal_context_t gpio_hal = { .dev = GPIO_HAL_GET_HW(GPIO_PORT_0) };
        gpio_hal_pullup_en(&gpio_hal, gpio_num);
#else
        sleep_gpio_isolate_one_pin(gpio_num);
#endif
    }
}

IRAM_ATTR void esp_sleep_restore_isolated_digital_gpio(void)
{
    uint64_t backuped = s_gpio_isolate_backup.backuped;
    while (backuped) {
        gpio_num_t gpio_num = (gpio_num_t)__builtin_ctzll(backuped);
        gpio_io_config_t io_config = {
            .pu = (s_gpio_isolate_backup.pu >> gpio_num) & 0x1,
            .pd = (s_gpio_isolate_backup.pd >> gpio_num) & 0x1,
            .ie = (s_gpio_isolate_backup.ie >> gpio_num) & 0x1,
            .oe = (s_gpio_isolate_backup.oe >> gpio_num) & 0x1,
            .fun_sel = (uint32_t)s_gpio_isolate_backup.fun_sel[gpio_num],
        };
        gpio_ll_set_pad_config_for_sleep_isolate(gpio_num, &io_config);
        backuped &= backuped - 1;
    }
}
#endif // SOC_GPIO_NEED_SOFT_ISOLATE_DURING_PD

IRAM_ATTR void esp_sleep_isolate_digital_gpio(bool do_backup)
{
    gpio_hal_context_t gpio_hal = { .dev = GPIO_HAL_GET_HW(GPIO_PORT_0) };
#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
    /* no need to do isolate if digital IOs are not being held in deep sleep */
    if (!gpio_hal_deep_sleep_hold_is_en(&gpio_hal)) {
        return;
    }
#endif

    /**
     * there is a situation where we cannot isolate digital IO before deep sleep:
     * - task stack is located in external ram(mspi ram), since we will isolate mspi io
     *
     * assert here instead of returning directly, because if digital IO is not isolated,
     * the bottom current of deep sleep will be higher than light sleep, and there is no
     * reason to use deep sleep at this time.
     */
    assert(esp_ptr_internal(esp_cpu_get_sp()) && "If hold digital IO, the stack of the task calling esp_deep_sleep_start must be in internal ram!");

    DRAM_ATTR static volatile uint64_t s_pad_mask = SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK;
    uint64_t pad_mask = s_pad_mask;
#if SOC_GPIO_NEED_SOFT_ISOLATE_DURING_PD
    if (do_backup) {
        s_gpio_isolate_backup.backuped = 0;
        s_gpio_isolate_backup.pu = 0;
        s_gpio_isolate_backup.pd = 0;
        s_gpio_isolate_backup.ie = 0;
        s_gpio_isolate_backup.oe = 0;
    }

    uint64_t hold_mask = gpio_ll_get_digital_gpio_hold_mask(gpio_hal.dev);
    gpio_io_config_t isolate_config = { .pu = 0, .pd = 0, .ie = 0, .oe = 0, .fun_sel = PIN_FUNC_GPIO };

    while (pad_mask) {
        gpio_num_t gpio_num = (gpio_num_t)__builtin_ctzll(pad_mask);
        if (!(hold_mask & (1ULL << gpio_num)) &&
            !esp_gpio_is_reserved(BIT64(gpio_num))) {
            if (do_backup) {
                gpio_io_config_t io_config;
                gpio_ll_backup_pad_config_for_sleep_isolate(gpio_num, &io_config);
                if (io_config.pu) {
                    s_gpio_isolate_backup.pu |= (1ULL << gpio_num);
                }
                if (io_config.pd) {
                    s_gpio_isolate_backup.pd |= (1ULL << gpio_num);
                }
                if (io_config.ie) {
                    s_gpio_isolate_backup.ie |= (1ULL << gpio_num);
                }
                if (gpio_ll_output_is_enabled(gpio_hal.dev, gpio_num)) {
                    s_gpio_isolate_backup.oe |= (1ULL << gpio_num);
                }
                s_gpio_isolate_backup.fun_sel[gpio_num] = (uint8_t)io_config.fun_sel;
                s_gpio_isolate_backup.backuped |= (1ULL << gpio_num);
            }
            gpio_ll_set_pad_config_for_sleep_isolate(gpio_num, &isolate_config);
        }
        pad_mask &= pad_mask - 1;
    }
#else
    (void)do_backup;
    while (pad_mask) {
        gpio_num_t gpio_num = (gpio_num_t)__builtin_ctzll(pad_mask);
        if (!gpio_hal_is_digital_io_hold(&gpio_hal, gpio_num) &&
            !esp_gpio_is_reserved(BIT64(gpio_num))) {
            sleep_gpio_isolate_one_pin(gpio_num);
        }
        pad_mask &= pad_mask - 1;
    }
#endif
}
#endif //!SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP || SOC_GPIO_NEED_SOFT_ISOLATE_DURING_PD

#if SOC_DEEP_SLEEP_SUPPORTED
static void esp_deep_sleep_wakeup_io_reset(void)
{
#if SOC_PM_SUPPORT_EXT1_WAKEUP
    uint32_t rtc_io_mask = rtc_hal_ext1_get_wakeup_pins();
    // Disable ext1 wakeup before releasing hold, such that wakeup status can reflect the correct wakeup pin
    rtc_hal_ext1_clear_wakeup_pins();
    for (int gpio_num = 0; gpio_num < SOC_GPIO_PIN_COUNT && rtc_io_mask != 0; ++gpio_num) {
        int rtcio_num = rtc_io_num_map[gpio_num];
        if ((rtc_io_mask & BIT(rtcio_num)) == 0) {
            continue;
        }
#if SOC_RTCIO_HOLD_SUPPORTED
        rtcio_hal_hold_disable(rtcio_num);
#endif
        rtc_io_mask &= ~BIT(rtcio_num);
    }
#endif

#if SOC_GPIO_SUPPORT_HP_PERIPH_PD_SLEEP_WAKEUP
    gpio_hal_context_t gpio_hal = { .dev = GPIO_HAL_GET_HW(GPIO_PORT_0) };
    uint64_t dslp_io_mask = SOC_GPIO_HP_PERIPH_PD_SLEEP_WAKEABLE_MASK;
    while (dslp_io_mask) {
        int gpio_num = __builtin_ctzll(dslp_io_mask);
        bool wakeup_io_enabled = gpio_hal_wakeup_is_enabled_on_hp_periph_powerdown_sleep(&gpio_hal, gpio_num);
        if (wakeup_io_enabled) {
            // Disable the wakeup before releasing hold, such that wakeup status can reflect the correct wakeup pin
            gpio_hal_wakeup_disable_on_hp_periph_powerdown_sleep(&gpio_hal, gpio_num);
            gpio_hal_hold_dis(&gpio_hal, gpio_num);
        }
        dslp_io_mask &= dslp_io_mask - 1;
    }
#endif
}
#endif

ESP_SYSTEM_INIT_FN(esp_sleep_startup_init, SECONDARY, BIT(0), 105)
{
#if SOC_DEEP_SLEEP_SUPPORTED
    // Need to unhold the IOs that were hold right before entering deep sleep, which are used as wakeup pins
#if SOC_PMU_SUPPORTED
    uint32_t io_wakeup_enabled = pmu_ll_hp_get_wakeup_enable(&PMU) & IO_WAKEUP_ENABLED;
#else
    uint32_t io_wakeup_enabled = rtc_cntl_ll_get_wakeup_enable() & IO_WAKEUP_ENABLED;
#endif
    if (esp_reset_reason() == ESP_RST_DEEPSLEEP && io_wakeup_enabled) {
        esp_deep_sleep_wakeup_io_reset();
    }
#endif  //#if SOC_DEEP_SLEEP_SUPPORTED

#if CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND || CONFIG_PM_SLP_DISABLE_GPIO
    // Configure to isolate (disable the Input/Output/Pullup/Pulldown
    // function of the pin) all GPIO pins in sleep state
    esp_sleep_config_gpio_isolate();
    // Enable automatic switching of GPIO configuration
    esp_sleep_enable_gpio_switch(true);
#endif
    return ESP_OK;
}

/**
 * @brief Convert RTC IO status bit number to GPIO number (for sleep wakeup status translation).
 *
 * @param bit RTC IO intr status bit index (0 to SOC_RTCIO_PIN_COUNT-1).
 * @return GPIO number, or GPIO_NUM_NC if bit is invalid or has no corresponding GPIO.
 */
gpio_num_t esp_sleep_wakeup_io_bit2num(uint32_t bit)
{
#if SOC_RTCIO_PIN_COUNT > 0
    if (bit >= SOC_RTCIO_PIN_COUNT) {
        return GPIO_NUM_NC;
    }
    for (int gpio = 0; gpio < SOC_GPIO_PIN_COUNT; gpio++) {
        if (rtc_io_num_map[gpio] == (int8_t)bit) {
            return (gpio_num_t)gpio;
        }
    }
#elif (SOC_RTCIO_PIN_COUNT == 0)
    return (gpio_num_t)bit;
#endif
    return GPIO_NUM_NC;
}

void esp_sleep_gpio_include(void)
{
    // Linker hook function, exists to make the linker examine this file
}
