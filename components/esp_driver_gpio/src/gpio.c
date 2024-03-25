/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "soc/interrupts.h"
#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#endif

#include "soc/soc_caps.h"
#include "soc/gpio_periph.h"
#include "esp_log.h"
#include "esp_check.h"
#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_private/periph_ctrl.h"

#if SOC_LP_IO_CLOCK_IS_INDEPENDENT && !SOC_RTCIO_RCC_IS_INDEPENDENT
#define RTCIO_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define RTCIO_RCC_ATOMIC()
#endif

#if (SOC_RTCIO_PIN_COUNT > 0)
#include "hal/rtc_io_hal.h"
#endif

static const char *GPIO_TAG = "gpio";
#define GPIO_CHECK(a, str, ret_val) ESP_RETURN_ON_FALSE(a, ret_val, GPIO_TAG, "%s", str)

#define GPIO_ISR_CORE_ID_UNINIT    (3)

//default value for SOC_GPIO_SUPPORT_RTC_INDEPENDENT is 0
#ifndef SOC_GPIO_SUPPORT_RTC_INDEPENDENT
#define SOC_GPIO_SUPPORT_RTC_INDEPENDENT 0
#endif

typedef struct {
    gpio_isr_t fn;   /*!< isr function */
    void *args;      /*!< isr function args */
} gpio_isr_func_t;

// Used by the IPC call to register the interrupt service routine.
typedef struct {
    int source;               /*!< ISR source */
    int intr_alloc_flags;     /*!< ISR alloc flag */
    void (*fn)(void *);       /*!< ISR function */
    void *arg;                /*!< ISR function args*/
    void *handle;             /*!< ISR handle */
    esp_err_t ret;
} gpio_isr_alloc_t;

typedef struct {
    gpio_hal_context_t *gpio_hal;
    portMUX_TYPE gpio_spinlock;
    uint32_t isr_core_id;
    gpio_isr_func_t *gpio_isr_func;
    gpio_isr_handle_t gpio_isr_handle;
    uint64_t isr_clr_on_entry_mask; // for edge-triggered interrupts, interrupt status bits should be cleared before entering per-pin handlers
#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP && SOC_LP_IO_CLOCK_IS_INDEPENDENT
    uint32_t gpio_wakeup_mask;
#endif
} gpio_context_t;

static gpio_hal_context_t _gpio_hal = {
    .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
};

static gpio_context_t gpio_context = {
    .gpio_hal = &_gpio_hal,
    .gpio_spinlock = portMUX_INITIALIZER_UNLOCKED,
    .isr_core_id = GPIO_ISR_CORE_ID_UNINIT,
    .gpio_isr_func = NULL,
    .isr_clr_on_entry_mask = 0,
#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP && SOC_LP_IO_CLOCK_IS_INDEPENDENT
    .gpio_wakeup_mask = 0,
#endif
};

esp_err_t gpio_pullup_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if (!rtc_gpio_is_valid_gpio(gpio_num) || SOC_GPIO_SUPPORT_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_pullup_en(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        rtc_gpio_pullup_en(gpio_num);
#else
        abort(); // This should be eliminated as unreachable, unless a programming error has occurred
#endif
    }

    return ESP_OK;
}

esp_err_t gpio_pullup_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if (!rtc_gpio_is_valid_gpio(gpio_num) || SOC_GPIO_SUPPORT_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_pullup_dis(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        rtc_gpio_pullup_dis(gpio_num);
#else
        abort(); // This should be eliminated as unreachable, unless a programming error has occurred
#endif
    }

    return ESP_OK;
}

esp_err_t gpio_pulldown_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if (!rtc_gpio_is_valid_gpio(gpio_num) || SOC_GPIO_SUPPORT_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_pulldown_en(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        rtc_gpio_pulldown_en(gpio_num);
#else
        abort(); // This should be eliminated as unreachable, unless a programming error has occurred
#endif
    }

    return ESP_OK;
}

esp_err_t gpio_pulldown_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if (!rtc_gpio_is_valid_gpio(gpio_num) || SOC_GPIO_SUPPORT_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_pulldown_dis(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        rtc_gpio_pulldown_dis(gpio_num);
#else
        abort(); // This should be eliminated as unreachable, unless a programming error has occurred
#endif
    }

    return ESP_OK;
}

esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(intr_type < GPIO_INTR_MAX, "GPIO interrupt type error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_set_intr_type(gpio_context.gpio_hal, gpio_num, intr_type);
    if (intr_type == GPIO_INTR_POSEDGE || intr_type == GPIO_INTR_NEGEDGE || intr_type == GPIO_INTR_ANYEDGE) {
        gpio_context.isr_clr_on_entry_mask |= (1ULL << (gpio_num));
    } else {
        gpio_context.isr_clr_on_entry_mask &= ~(1ULL << (gpio_num));
    }
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}

static inline esp_err_t gpio_intr_enable_on_core(gpio_num_t gpio_num, uint32_t core_id)
{
    gpio_hal_intr_enable_on_core(gpio_context.gpio_hal, gpio_num, core_id);
    return ESP_OK;
}

esp_err_t gpio_intr_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    if (gpio_context.isr_core_id == GPIO_ISR_CORE_ID_UNINIT) {
        gpio_context.isr_core_id = xPortGetCoreID();
    }
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return gpio_intr_enable_on_core(gpio_num, gpio_context.isr_core_id);
}

esp_err_t gpio_intr_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_intr_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_input_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_input_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_input_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_input_enable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_output_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_output_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_output_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO output gpio_num error", ESP_ERR_INVALID_ARG);
    gpio_hal_output_enable(gpio_context.gpio_hal, gpio_num);
    esp_rom_gpio_connect_out_signal(gpio_num, SIG_GPIO_OUT_IDX, false, false);
    return ESP_OK;
}

static esp_err_t gpio_od_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_od_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_od_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_od_enable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO output gpio_num error", ESP_ERR_INVALID_ARG);
    gpio_hal_set_level(gpio_context.gpio_hal, gpio_num, level);
    return ESP_OK;
}

int gpio_get_level(gpio_num_t gpio_num)
{
    return gpio_hal_get_level(gpio_context.gpio_hal, gpio_num);
}

#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
static esp_err_t gpio_hysteresis_enable(gpio_num_t gpio_num)
{
    gpio_hal_hysteresis_soft_enable(gpio_context.gpio_hal, gpio_num, true);
    return ESP_OK;
}

static esp_err_t gpio_hysteresis_disable(gpio_num_t gpio_num)
{
    gpio_hal_hysteresis_soft_enable(gpio_context.gpio_hal, gpio_num, false);
    return ESP_OK;
}

#if SOC_GPIO_SUPPORT_PIN_HYS_CTRL_BY_EFUSE
static esp_err_t gpio_hysteresis_by_efuse(gpio_num_t gpio_num)
{
    gpio_hal_hysteresis_from_efuse(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}
#endif
#endif  //SOC_GPIO_SUPPORT_PIN_HYS_FILTER

esp_err_t gpio_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(pull <= GPIO_FLOATING, "GPIO pull mode error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    switch (pull) {
    case GPIO_PULLUP_ONLY:
        gpio_pulldown_dis(gpio_num);
        gpio_pullup_en(gpio_num);
        break;

    case GPIO_PULLDOWN_ONLY:
        gpio_pulldown_en(gpio_num);
        gpio_pullup_dis(gpio_num);
        break;

    case GPIO_PULLUP_PULLDOWN:
        gpio_pulldown_en(gpio_num);
        gpio_pullup_en(gpio_num);
        break;

    case GPIO_FLOATING:
        gpio_pulldown_dis(gpio_num);
        gpio_pullup_dis(gpio_num);
        break;

    default:
        ESP_LOGE(GPIO_TAG, "Unknown pull up/down mode,gpio_num=%u,pull=%u", gpio_num, pull);
        ret = ESP_ERR_INVALID_ARG;
        break;
    }

    return ret;
}

esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if ((GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) != true) && (mode & GPIO_MODE_DEF_OUTPUT)) {
        ESP_LOGE(GPIO_TAG, "io_num=%d can only be input", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    if (mode & GPIO_MODE_DEF_INPUT) {
        gpio_input_enable(gpio_num);
    } else {
        gpio_input_disable(gpio_num);
    }

    if (mode & GPIO_MODE_DEF_OUTPUT) {
        gpio_output_enable(gpio_num);
    } else {
        gpio_output_disable(gpio_num);
    }

    if (mode & GPIO_MODE_DEF_OD) {
        gpio_od_enable(gpio_num);
    } else {
        gpio_od_disable(gpio_num);
    }

    return ret;
}

esp_err_t gpio_config(const gpio_config_t *pGPIOConfig)
{
    uint64_t gpio_pin_mask = (pGPIOConfig->pin_bit_mask);
    uint32_t io_num = 0;
    uint8_t input_en = 0;
    uint8_t output_en = 0;
    uint8_t od_en = 0;
    uint8_t pu_en = 0;
    uint8_t pd_en = 0;

    if (pGPIOConfig->pin_bit_mask == 0 ||
            pGPIOConfig->pin_bit_mask & ~SOC_GPIO_VALID_GPIO_MASK) {
        ESP_LOGE(GPIO_TAG, "GPIO_PIN mask error ");
        return ESP_ERR_INVALID_ARG;
    }

    if (pGPIOConfig->mode & GPIO_MODE_DEF_OUTPUT &&
            pGPIOConfig->pin_bit_mask & ~SOC_GPIO_VALID_OUTPUT_GPIO_MASK) {
        ESP_LOGE(GPIO_TAG, "GPIO can only be used as input mode");
        return ESP_ERR_INVALID_ARG;
    }

    do {
        if (((gpio_pin_mask >> io_num) & BIT(0))) {

#if SOC_RTCIO_PIN_COUNT > 0
            if (rtc_gpio_is_valid_gpio(io_num)) {
                rtc_gpio_deinit(io_num);
            }
#endif

            if ((pGPIOConfig->mode) & GPIO_MODE_DEF_INPUT) {
                input_en = 1;
                gpio_input_enable(io_num);
            } else {
                gpio_input_disable(io_num);
            }

            if ((pGPIOConfig->mode) & GPIO_MODE_DEF_OD) {
                od_en = 1;
                gpio_od_enable(io_num);
            } else {
                gpio_od_disable(io_num);
            }

            if ((pGPIOConfig->mode) & GPIO_MODE_DEF_OUTPUT) {
                output_en = 1;
                gpio_output_enable(io_num);
            } else {
                gpio_output_disable(io_num);
            }

            if (pGPIOConfig->pull_up_en) {
                pu_en = 1;
                gpio_pullup_en(io_num);
            } else {
                gpio_pullup_dis(io_num);
            }

            if (pGPIOConfig->pull_down_en) {
                pd_en = 1;
                gpio_pulldown_en(io_num);
            } else {
                gpio_pulldown_dis(io_num);
            }

            ESP_LOGI(GPIO_TAG, "GPIO[%"PRIu32"]| InputEn: %d| OutputEn: %d| OpenDrain: %d| Pullup: %d| Pulldown: %d| Intr:%d ", io_num, input_en, output_en, od_en, pu_en, pd_en, pGPIOConfig->intr_type);
            gpio_set_intr_type(io_num, pGPIOConfig->intr_type);

            if (pGPIOConfig->intr_type) {
                gpio_intr_enable(io_num);
            } else {
                gpio_intr_disable(io_num);
            }

#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
            if (pGPIOConfig->hys_ctrl_mode == GPIO_HYS_SOFT_ENABLE) {
                gpio_hysteresis_enable(io_num);
            } else if (pGPIOConfig->hys_ctrl_mode == GPIO_HYS_SOFT_DISABLE) {
                gpio_hysteresis_disable(io_num);
            }
#if SOC_GPIO_SUPPORT_PIN_HYS_CTRL_BY_EFUSE
            else {
                gpio_hysteresis_by_efuse(io_num);
            }
#endif
#endif  //SOC_GPIO_SUPPORT_PIN_HYS_FILTER

            /* By default, all the pins have to be configured as GPIO pins. */
            gpio_hal_func_sel(gpio_context.gpio_hal, io_num, PIN_FUNC_GPIO);
        }

        io_num++;
    } while (io_num < GPIO_PIN_COUNT);

    return ESP_OK;
}

esp_err_t gpio_reset_pin(gpio_num_t gpio_num)
{
    assert(GPIO_IS_VALID_GPIO(gpio_num));
    gpio_config_t cfg = {
        .pin_bit_mask = BIT64(gpio_num),
        .mode = GPIO_MODE_DISABLE,
        //for powersave reasons, the GPIO should not be floating, select pullup
        .pull_up_en = true,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);
    return ESP_OK;
}

static inline void IRAM_ATTR gpio_isr_loop(uint32_t status, const uint32_t gpio_num_start)
{
    while (status) {
        int nbit = __builtin_ffs(status) - 1;
        status &= ~(1 << nbit);
        int gpio_num = gpio_num_start + nbit;

        bool intr_status_bit_cleared = false;
        // Edge-triggered type interrupt can clear the interrupt status bit before entering per-pin interrupt handler
        if ((1ULL << (gpio_num)) & gpio_context.isr_clr_on_entry_mask) {
            intr_status_bit_cleared = true;
            gpio_hal_clear_intr_status_bit(gpio_context.gpio_hal, gpio_num);
        }

        if (gpio_context.gpio_isr_func[gpio_num].fn != NULL) {
            gpio_context.gpio_isr_func[gpio_num].fn(gpio_context.gpio_isr_func[gpio_num].args);
        }

        // If the interrupt status bit was not cleared at the entry, then must clear it before exiting
        if (!intr_status_bit_cleared) {
            gpio_hal_clear_intr_status_bit(gpio_context.gpio_hal, gpio_num);
        }
    }
}

static void IRAM_ATTR gpio_intr_service(void *arg)
{
    //GPIO intr process
    if (gpio_context.gpio_isr_func == NULL) {
        return;
    }

    //read status to get interrupt status for GPIO0-31
    uint32_t gpio_intr_status;
    gpio_hal_get_intr_status(gpio_context.gpio_hal, gpio_context.isr_core_id, &gpio_intr_status);

    if (gpio_intr_status) {
        gpio_isr_loop(gpio_intr_status, 0);
    }

    //read status1 to get interrupt status for GPIO32-39
    uint32_t gpio_intr_status_h;
    gpio_hal_get_intr_status_high(gpio_context.gpio_hal, gpio_context.isr_core_id, &gpio_intr_status_h);

    if (gpio_intr_status_h) {
        gpio_isr_loop(gpio_intr_status_h, 32);
    }
}

esp_err_t gpio_install_isr_service(int intr_alloc_flags)
{
    GPIO_CHECK(gpio_context.gpio_isr_func == NULL, "GPIO isr service already installed", ESP_ERR_INVALID_STATE);
    esp_err_t ret = ESP_ERR_NO_MEM;
    const uint32_t alloc_caps = (intr_alloc_flags & ESP_INTR_FLAG_IRAM) ? MALLOC_CAP_INTERNAL : MALLOC_CAP_DEFAULT;
    gpio_isr_func_t *isr_func = (gpio_isr_func_t *) heap_caps_calloc(GPIO_NUM_MAX, sizeof(gpio_isr_func_t), alloc_caps);
    if (isr_func) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        if (gpio_context.gpio_isr_func == NULL) {
            gpio_context.gpio_isr_func = isr_func;
            portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
            ret = gpio_isr_register(gpio_intr_service, NULL, intr_alloc_flags, &gpio_context.gpio_isr_handle);
            if (ret != ESP_OK) {
                // registering failed, uninstall isr service
                gpio_uninstall_isr_service();
            }
        } else {
            // isr service already installed, free allocated resource
            portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
            ret = ESP_ERR_INVALID_STATE;
            free(isr_func);
        }
    }

    return ret;
}

esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)
{
    GPIO_CHECK(gpio_context.gpio_isr_func != NULL, "GPIO isr service is not installed, call gpio_install_isr_service() first", ESP_ERR_INVALID_STATE);
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_intr_disable(gpio_num);
    if (gpio_context.gpio_isr_func) {
        gpio_context.gpio_isr_func[gpio_num].fn = isr_handler;
        gpio_context.gpio_isr_func[gpio_num].args = args;
    }
    gpio_intr_enable_on_core(gpio_num, esp_intr_get_cpu(gpio_context.gpio_isr_handle));
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}

esp_err_t gpio_isr_handler_remove(gpio_num_t gpio_num)
{
    GPIO_CHECK(gpio_context.gpio_isr_func != NULL, "GPIO isr service is not installed, call gpio_install_isr_service() first", ESP_ERR_INVALID_STATE);
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_intr_disable(gpio_num);
    if (gpio_context.gpio_isr_func) {
        gpio_context.gpio_isr_func[gpio_num].fn = NULL;
        gpio_context.gpio_isr_func[gpio_num].args = NULL;
    }
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}

void gpio_uninstall_isr_service(void)
{
    gpio_isr_func_t *gpio_isr_func_free = NULL;
    gpio_isr_handle_t gpio_isr_handle_free = NULL;
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    if (gpio_context.gpio_isr_func == NULL) {
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
        return;
    }
    gpio_isr_func_free = gpio_context.gpio_isr_func;
    gpio_context.gpio_isr_func = NULL;
    gpio_isr_handle_free = gpio_context.gpio_isr_handle;
    gpio_context.gpio_isr_handle = NULL;
    gpio_context.isr_core_id = GPIO_ISR_CORE_ID_UNINIT;
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    esp_intr_free(gpio_isr_handle_free);
    free(gpio_isr_func_free);
    return;
}

static void gpio_isr_register_on_core_static(void *param)
{
    gpio_isr_alloc_t *p = (gpio_isr_alloc_t *)param;
    //We need to check the return value.
    p->ret = esp_intr_alloc(p->source, p->intr_alloc_flags, p->fn, p->arg, p->handle);
}

esp_err_t gpio_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, gpio_isr_handle_t *handle)
{
    GPIO_CHECK(fn, "GPIO ISR null", ESP_ERR_INVALID_ARG);
    gpio_isr_alloc_t p;
#if !CONFIG_IDF_TARGET_ESP32P4  //TODO: IDF-7995
    p.source = ETS_GPIO_INTR_SOURCE;
#else
    p.source = ETS_GPIO_INTR0_SOURCE;
#endif
    p.intr_alloc_flags = intr_alloc_flags;
#if SOC_ANA_CMPR_INTR_SHARE_WITH_GPIO
    p.intr_alloc_flags |= ESP_INTR_FLAG_SHARED;
#endif
    p.fn = fn;
    p.arg = arg;
    p.handle = handle;
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    if (gpio_context.isr_core_id == GPIO_ISR_CORE_ID_UNINIT) {
        gpio_context.isr_core_id = xPortGetCoreID();
    }
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    esp_err_t ret;
#if CONFIG_FREERTOS_UNICORE
    gpio_isr_register_on_core_static(&p);
    ret = ESP_OK;
#else /* CONFIG_FREERTOS_UNICORE */
    ret = esp_ipc_call_blocking(gpio_context.isr_core_id, gpio_isr_register_on_core_static, (void *)&p);
#endif /* !CONFIG_FREERTOS_UNICORE */
    if (ret != ESP_OK) {
        ESP_LOGE(GPIO_TAG, "esp_ipc_call_blocking failed (0x%x)", ret);
        return ESP_ERR_NOT_FOUND;
    }
    if (p.ret != ESP_OK) {
        ESP_LOGE(GPIO_TAG, "esp_intr_alloc failed (0x%x)", p.ret);
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    if ((intr_type == GPIO_INTR_LOW_LEVEL) || (intr_type == GPIO_INTR_HIGH_LEVEL)) {
#if SOC_RTCIO_WAKE_SUPPORTED
        if (rtc_gpio_is_valid_gpio(gpio_num)) {
            ret = rtc_gpio_wakeup_enable(gpio_num, intr_type);
        }
#endif
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_set_intr_type(gpio_context.gpio_hal, gpio_num, intr_type);
        gpio_hal_wakeup_enable(gpio_context.gpio_hal, gpio_num);
#if CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND || CONFIG_PM_SLP_DISABLE_GPIO
        gpio_hal_sleep_sel_dis(gpio_context.gpio_hal, gpio_num);
#endif
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        ESP_LOGE(GPIO_TAG, "GPIO wakeup only supports level mode, but edge mode set. gpio_num:%u", gpio_num);
        ret = ESP_ERR_INVALID_ARG;
    }

    return ret;
}

esp_err_t gpio_wakeup_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
#if SOC_RTCIO_WAKE_SUPPORTED
    if (rtc_gpio_is_valid_gpio(gpio_num)) {
        ret = rtc_gpio_wakeup_disable(gpio_num);
    }
#endif
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_wakeup_disable(gpio_context.gpio_hal, gpio_num);
#if CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND || CONFIG_PM_SLP_DISABLE_GPIO
    gpio_hal_sleep_sel_en(gpio_context.gpio_hal, gpio_num);
#endif
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ret;
}

esp_err_t gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(strength < GPIO_DRIVE_CAP_MAX, "GPIO drive capability error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    if (!rtc_gpio_is_valid_gpio(gpio_num) || SOC_GPIO_SUPPORT_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_set_drive_capability(gpio_context.gpio_hal, gpio_num, strength);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        ret = rtc_gpio_set_drive_capability(gpio_num, strength);
#else
        abort(); // This should be eliminated as unreachable, unless a programming error has occurred
#endif
    }

    return ret;
}

esp_err_t gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t *strength)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(strength != NULL, "GPIO drive capability pointer error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    if (!rtc_gpio_is_valid_gpio(gpio_num) || SOC_GPIO_SUPPORT_RTC_INDEPENDENT) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_get_drive_capability(gpio_context.gpio_hal, gpio_num, strength);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        ret = rtc_gpio_get_drive_capability(gpio_num, strength);
#else
        abort(); // This should be eliminated as unreachable, unless a programming error has occurred
#endif
    }

    return ret;
}

esp_err_t gpio_hold_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Only output-capable GPIO support this function", ESP_ERR_NOT_SUPPORTED);
    int ret = ESP_OK;

    if (rtc_gpio_is_valid_gpio(gpio_num)) {
#if SOC_RTCIO_HOLD_SUPPORTED
        ret = rtc_gpio_hold_en(gpio_num);
#endif
    } else if (GPIO_HOLD_MASK[gpio_num]) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_hold_en(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        ret = ESP_ERR_NOT_SUPPORTED;
    }

    return ret;
}

esp_err_t gpio_hold_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Only output-capable GPIO support this function", ESP_ERR_NOT_SUPPORTED);
    int ret = ESP_OK;

    if (rtc_gpio_is_valid_gpio(gpio_num)) {
#if SOC_RTCIO_HOLD_SUPPORTED
        ret = rtc_gpio_hold_dis(gpio_num);
#endif
    } else if (GPIO_HOLD_MASK[gpio_num]) {
        portENTER_CRITICAL(&gpio_context.gpio_spinlock);
        gpio_hal_hold_dis(gpio_context.gpio_hal, gpio_num);
        portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    } else {
        ret = ESP_ERR_NOT_SUPPORTED;
    }

    return ret;
}

#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
void gpio_deep_sleep_hold_en(void)
{
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_deep_sleep_hold_en(gpio_context.gpio_hal);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
}

void gpio_deep_sleep_hold_dis(void)
{
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_deep_sleep_hold_dis(gpio_context.gpio_hal);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
}
#endif //!SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP

#if SOC_GPIO_SUPPORT_FORCE_HOLD
esp_err_t IRAM_ATTR gpio_force_hold_all()
{
#if SOC_RTCIO_HOLD_SUPPORTED
    rtc_gpio_force_hold_en_all();
#endif
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_force_hold_all();
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}

esp_err_t IRAM_ATTR gpio_force_unhold_all()
{
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_force_unhold_all();
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
#if SOC_RTCIO_HOLD_SUPPORTED
    rtc_gpio_force_hold_dis_all();
#endif
    return ESP_OK;
}
#endif //SOC_GPIO_SUPPORT_FORCE_HOLD

void gpio_iomux_in(uint32_t gpio, uint32_t signal_idx)
{
    gpio_hal_iomux_in(gpio_context.gpio_hal, gpio, signal_idx);
}

void gpio_iomux_out(uint8_t gpio_num, int func, bool out_en_inv)
{
    gpio_hal_iomux_out(gpio_context.gpio_hal, gpio_num, func, (uint32_t)out_en_inv);
}

static esp_err_t gpio_sleep_pullup_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_sleep_pullup_en(gpio_context.gpio_hal, gpio_num);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);

    return ESP_OK;
}

static esp_err_t gpio_sleep_pullup_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_sleep_pullup_dis(gpio_context.gpio_hal, gpio_num);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);

    return ESP_OK;
}

static esp_err_t gpio_sleep_pulldown_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_sleep_pulldown_en(gpio_context.gpio_hal, gpio_num);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);

    return ESP_OK;
}

static esp_err_t gpio_sleep_pulldown_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_sleep_pulldown_dis(gpio_context.gpio_hal, gpio_num);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);

    return ESP_OK;
}

static esp_err_t gpio_sleep_input_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_sleep_input_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_sleep_input_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_sleep_input_enable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_sleep_output_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_sleep_output_disable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_sleep_output_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO output gpio_num error", ESP_ERR_INVALID_ARG);
    gpio_hal_sleep_output_enable(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

esp_err_t gpio_sleep_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    if ((GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) != true) && (mode & GPIO_MODE_DEF_OUTPUT)) {
        ESP_LOGE(GPIO_TAG, "io_num=%d can only be input", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    if (mode & GPIO_MODE_DEF_INPUT) {
        gpio_sleep_input_enable(gpio_num);
    } else {
        gpio_sleep_input_disable(gpio_num);
    }

    if (mode & GPIO_MODE_DEF_OUTPUT) {
        gpio_sleep_output_enable(gpio_num);
    } else {
        gpio_sleep_output_disable(gpio_num);
    }

    return ret;
}

esp_err_t gpio_sleep_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(pull <= GPIO_FLOATING, "GPIO pull mode error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;

    switch (pull) {
    case GPIO_PULLUP_ONLY:
        gpio_sleep_pulldown_dis(gpio_num);
        gpio_sleep_pullup_en(gpio_num);
        break;

    case GPIO_PULLDOWN_ONLY:
        gpio_sleep_pulldown_en(gpio_num);
        gpio_sleep_pullup_dis(gpio_num);
        break;

    case GPIO_PULLUP_PULLDOWN:
        gpio_sleep_pulldown_en(gpio_num);
        gpio_sleep_pullup_en(gpio_num);
        break;

    case GPIO_FLOATING:
        gpio_sleep_pulldown_dis(gpio_num);
        gpio_sleep_pullup_dis(gpio_num);
        break;

    default:
        ESP_LOGE(GPIO_TAG, "Unknown pull up/down mode,gpio_num=%u,pull=%u", gpio_num, pull);
        ret = ESP_ERR_INVALID_ARG;
        break;
    }

    return ret;
}

esp_err_t gpio_sleep_sel_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_sleep_sel_en(gpio_context.gpio_hal, gpio_num);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);

    return ESP_OK;
}

esp_err_t gpio_sleep_sel_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_sleep_sel_dis(gpio_context.gpio_hal, gpio_num);
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);

    return ESP_OK;
}

#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
esp_err_t gpio_sleep_pupd_config_apply(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_sleep_pupd_config_apply(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}

esp_err_t gpio_sleep_pupd_config_unapply(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_sleep_pupd_config_unapply(gpio_context.gpio_hal, gpio_num);
    return ESP_OK;
}
#endif // CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
esp_err_t gpio_deep_sleep_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    if (!GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO(gpio_num)) {
        ESP_LOGE(GPIO_TAG, "GPIO %d does not support deep sleep wakeup", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }
    if ((intr_type != GPIO_INTR_LOW_LEVEL) && (intr_type != GPIO_INTR_HIGH_LEVEL)) {
        ESP_LOGE(GPIO_TAG, "GPIO wakeup only supports level mode, but edge mode set. gpio_num:%u", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
#if SOC_LP_IO_CLOCK_IS_INDEPENDENT
    if (gpio_context.gpio_wakeup_mask == 0) {
        RTCIO_RCC_ATOMIC() {
            rtcio_ll_enable_io_clock(true);
        }
    }
    gpio_context.gpio_wakeup_mask |= (1ULL << gpio_num);
#endif
    gpio_hal_deepsleep_wakeup_enable(gpio_context.gpio_hal, gpio_num, intr_type);
#if CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND || CONFIG_PM_SLP_DISABLE_GPIO
    gpio_hal_sleep_sel_dis(gpio_context.gpio_hal, gpio_num);
#endif
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}

esp_err_t gpio_deep_sleep_wakeup_disable(gpio_num_t gpio_num)
{
    if (!GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO(gpio_num)) {
        ESP_LOGE(GPIO_TAG, "GPIO %d does not support deep sleep wakeup", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_hal_deepsleep_wakeup_disable(gpio_context.gpio_hal, gpio_num);
#if CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND || CONFIG_PM_SLP_DISABLE_GPIO
    gpio_hal_sleep_sel_en(gpio_context.gpio_hal, gpio_num);
#endif
#if SOC_LP_IO_CLOCK_IS_INDEPENDENT
    gpio_context.gpio_wakeup_mask &= ~(1ULL << gpio_num);
    if (gpio_context.gpio_wakeup_mask == 0) {
        RTCIO_RCC_ATOMIC() {
            rtcio_ll_enable_io_clock(false);
        }
    }
#endif
    portEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return ESP_OK;
}
#endif // SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

esp_err_t gpio_dump_io_configuration(FILE *out_stream, uint64_t io_bit_mask)
{
    ESP_RETURN_ON_FALSE(out_stream, ESP_ERR_INVALID_ARG, GPIO_TAG, "out_stream error");
    ESP_RETURN_ON_FALSE(!(io_bit_mask & ~SOC_GPIO_VALID_GPIO_MASK), ESP_ERR_INVALID_ARG, GPIO_TAG, "io_bit_mask error");

    fprintf(out_stream, "================IO DUMP Start================\n");
    while (io_bit_mask) {
        uint32_t gpio_num = __builtin_ffsll(io_bit_mask) - 1;
        io_bit_mask &= ~(1ULL << gpio_num);

        bool pu, pd, ie, oe, od, slp_sel;
        uint32_t drv, fun_sel, sig_out;
        gpio_hal_get_io_config(gpio_context.gpio_hal, gpio_num, &pu, &pd, &ie, &oe, &od, &drv, &fun_sel, &sig_out, &slp_sel);

        fprintf(out_stream, "IO[%"PRIu32"]%s -\n", gpio_num, esp_gpio_is_reserved(BIT64(gpio_num)) ? " **RESERVED**" : "");
        fprintf(out_stream, "  Pullup: %d, Pulldown: %d, DriveCap: %"PRIu32"\n", pu, pd, drv);
        fprintf(out_stream, "  InputEn: %d, OutputEn: %d, OpenDrain: %d\n", ie, oe, od);
        fprintf(out_stream, "  FuncSel: %"PRIu32" (%s)\n", fun_sel, (fun_sel == PIN_FUNC_GPIO) ? "GPIO" : "IOMUX");
        if (oe && fun_sel == PIN_FUNC_GPIO) {
            fprintf(out_stream, "  GPIO Matrix SigOut ID: %"PRIu32"%s\n", sig_out, (sig_out == SIG_GPIO_OUT_IDX) ? " (simple GPIO output)" : "");
        }
        if (ie && fun_sel == PIN_FUNC_GPIO) {
            uint32_t cnt = 0;
            fprintf(out_stream, "  GPIO Matrix SigIn ID:");
            for (int i = 0; i < SIG_GPIO_OUT_IDX; i++) {
                if (gpio_hal_get_in_signal_connected_io(gpio_context.gpio_hal, i) == gpio_num) {
                    cnt++;
                    fprintf(out_stream, " %d", i);
                }
            }
            if (cnt == 0) {
                fprintf(out_stream, " (simple GPIO input)");
            }
            fprintf(out_stream, "\n");
        }
        fprintf(out_stream, "  SleepSelEn: %d\n", slp_sel);
        fprintf(out_stream, "\n");
    }
    fprintf(out_stream, "=================IO DUMP End==================\n");
    return ESP_OK;
}

esp_err_t gpio_func_sel(gpio_num_t gpio_num, uint32_t func)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    gpio_hal_func_sel(gpio_context.gpio_hal, gpio_num, func);
    return ESP_OK;
}
