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
#include <esp_types.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "soc/soc.h"
#include "soc/periph_defs.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/gpio_periph.h"
#include "esp_log.h"
#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#endif

#define GPIO_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(GPIO_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }
#define GPIO_ISR_CORE_ID_UNINIT    (3)

typedef struct {
    gpio_isr_t fn;   /*!< isr function */
    void* args;      /*!< isr function args */
} gpio_isr_func_t;

// Used by the IPC call to register the interrupt service routine.
typedef struct {
    int source;               /*!< ISR source */
    int intr_alloc_flags;     /*!< ISR alloc flag */
    void (*fn)(void*);        /*!< ISR function */
    void *arg;                /*!< ISR function args*/
    void *handle;             /*!< ISR handle */
    esp_err_t ret;
} gpio_isr_alloc_t;

static const char* GPIO_TAG = "gpio";
static gpio_isr_func_t* gpio_isr_func = NULL;
static gpio_isr_handle_t gpio_isr_handle;
static uint32_t isr_core_id = GPIO_ISR_CORE_ID_UNINIT;
static portMUX_TYPE gpio_spinlock = portMUX_INITIALIZER_UNLOCKED;

esp_err_t gpio_pullup_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    if (RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        rtc_gpio_pullup_en(gpio_num);
    } else {
        REG_SET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PU);
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    REG_SET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PU);
#endif
    return ESP_OK;
}

esp_err_t gpio_pullup_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    if (RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        rtc_gpio_pullup_dis(gpio_num);
    } else {
        REG_CLR_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PU);
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    REG_CLR_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PU);
#endif
    return ESP_OK;
}

esp_err_t gpio_pulldown_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    if (RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        rtc_gpio_pulldown_en(gpio_num);
    } else {
        REG_SET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PD);
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    REG_SET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PD);
#endif
    return ESP_OK;
}

esp_err_t gpio_pulldown_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    if (RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        rtc_gpio_pulldown_dis(gpio_num);
    } else {
        REG_CLR_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PD);
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    REG_CLR_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PD);
#endif
    return ESP_OK;
}

esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(intr_type < GPIO_INTR_MAX, "GPIO interrupt type error", ESP_ERR_INVALID_ARG);
    GPIO.pin[gpio_num].int_type = intr_type;
    return ESP_OK;
}

static void gpio_intr_status_clr(gpio_num_t gpio_num)
{
    if (gpio_num < 32) {
        GPIO.status_w1tc = BIT(gpio_num);
    } else {
        GPIO.status1_w1tc.intr_st = BIT(gpio_num - 32);
    }
}

static esp_err_t gpio_intr_enable_on_core (gpio_num_t gpio_num, uint32_t core_id)
{
    gpio_intr_status_clr(gpio_num);
#if CONFIG_IDF_TARGET_ESP32
    if (core_id == 0) {
        GPIO.pin[gpio_num].int_ena = GPIO_PRO_CPU_INTR_ENA;     //enable pro cpu intr
    } else {
        GPIO.pin[gpio_num].int_ena = GPIO_APP_CPU_INTR_ENA;     //enable pro cpu intr
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    if (core_id == 0) {
        GPIO.pin[gpio_num].int_ena = GPIO_PRO_CPU_INTR_ENA;     //enable pro cpu intr
    }
#endif
    return ESP_OK;
}

esp_err_t gpio_intr_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&gpio_spinlock);
    if(isr_core_id == GPIO_ISR_CORE_ID_UNINIT) {
        isr_core_id = xPortGetCoreID();
    }
    portEXIT_CRITICAL(&gpio_spinlock);
    return gpio_intr_enable_on_core (gpio_num, isr_core_id);
}

esp_err_t gpio_intr_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO.pin[gpio_num].int_ena = 0;                             //disable GPIO intr
    gpio_intr_status_clr(gpio_num);
    return ESP_OK;
}

static esp_err_t gpio_output_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    if (gpio_num < 32) {
        GPIO.enable_w1tc = (0x1 << gpio_num);
    } else {
        GPIO.enable1_w1tc.data = (0x1 << (gpio_num - 32));
    }

    // Ensure no other output signal is routed via GPIO matrix to this pin
    REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG + (gpio_num * 4),
              SIG_GPIO_OUT_IDX);

    return ESP_OK;
}

static esp_err_t gpio_output_enable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO output gpio_num error", ESP_ERR_INVALID_ARG);
    if (gpio_num < 32) {
        GPIO.enable_w1ts = (0x1 << gpio_num);
    } else {
        GPIO.enable1_w1ts.data = (0x1 << (gpio_num - 32));
    }
    gpio_matrix_out(gpio_num, SIG_GPIO_OUT_IDX, false, false);
    return ESP_OK;
}

esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO output gpio_num error", ESP_ERR_INVALID_ARG);
    if (level) {
        if (gpio_num < 32) {
            GPIO.out_w1ts = (1 << gpio_num);
        } else {
            GPIO.out1_w1ts.data = (1 << (gpio_num - 32));
        }
    } else {
        if (gpio_num < 32) {
            GPIO.out_w1tc = (1 << gpio_num);
        } else {
            GPIO.out1_w1tc.data = (1 << (gpio_num - 32));
        }
    }
    return ESP_OK;
}

int gpio_get_level(gpio_num_t gpio_num)
{
    if (gpio_num < 32) {
        return (GPIO.in >> gpio_num) & 0x1;
    } else {
        return (GPIO.in1.data >> (gpio_num - 32)) & 0x1;
    }
}

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
#if CONFIG_IDF_TARGET_ESP32
    if (gpio_num >= 34 && (mode & GPIO_MODE_DEF_OUTPUT)) {
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    if (gpio_num >= 46 && (mode & GPIO_MODE_DEF_OUTPUT)) {
#endif
        ESP_LOGE(GPIO_TAG, "io_num=%d can only be input", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    if (mode & GPIO_MODE_DEF_INPUT) {
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
    } else {
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
    }
    if (mode & GPIO_MODE_DEF_OUTPUT) {
        gpio_output_enable(gpio_num);
    } else {
        gpio_output_disable(gpio_num);
    }
    if (mode & GPIO_MODE_DEF_OD) {
        GPIO.pin[gpio_num].pad_driver = 1;
    } else {
        GPIO.pin[gpio_num].pad_driver = 0;
    }
    return ret;
}

esp_err_t gpio_config(const gpio_config_t *pGPIOConfig)
{
    uint64_t gpio_pin_mask = (pGPIOConfig->pin_bit_mask);
    uint32_t io_reg = 0;
    uint32_t io_num = 0;
    uint8_t input_en = 0;
    uint8_t output_en = 0;
    uint8_t od_en = 0;
    uint8_t pu_en = 0;
    uint8_t pd_en = 0;
    if (pGPIOConfig->pin_bit_mask == 0 || pGPIOConfig->pin_bit_mask >= (((uint64_t) 1) << GPIO_PIN_COUNT)) {
        ESP_LOGE(GPIO_TAG, "GPIO_PIN mask error ");
        return ESP_ERR_INVALID_ARG;
    }
#if CONFIG_IDF_TARGET_ESP32
    if ((pGPIOConfig->mode) & (GPIO_MODE_DEF_OUTPUT)) {
        //GPIO 34/35/36/37/38/39 can only be used as input mode;
        if ((gpio_pin_mask & ( GPIO_SEL_34 | GPIO_SEL_35 | GPIO_SEL_36 | GPIO_SEL_37 | GPIO_SEL_38 | GPIO_SEL_39))) {
            ESP_LOGE(GPIO_TAG, "GPIO34-39 can only be used as input mode");
            return ESP_ERR_INVALID_ARG;
        }
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    if ( (pGPIOConfig->mode & GPIO_MODE_DEF_OUTPUT) && (gpio_pin_mask & GPIO_SEL_46) ) {
        ESP_LOGE(GPIO_TAG, "GPIO46 can only be used as input mode");
        return ESP_ERR_INVALID_ARG;
    }
#endif
    do {
        io_reg = GPIO_PIN_MUX_REG[io_num];
        if (((gpio_pin_mask >> io_num) & BIT(0))) {
            if (!io_reg) {
                ESP_LOGE(GPIO_TAG, "IO%d is not a valid GPIO",io_num);
                return ESP_ERR_INVALID_ARG;
            }
            if(RTC_GPIO_IS_VALID_GPIO(io_num)){
                rtc_gpio_deinit(io_num);
            }
            if ((pGPIOConfig->mode) & GPIO_MODE_DEF_INPUT) {
                input_en = 1;
                PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[io_num]);
            } else {
                PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[io_num]);
            }
            if ((pGPIOConfig->mode) & GPIO_MODE_DEF_OD) {
                od_en = 1;
                GPIO.pin[io_num].pad_driver = 1; /*0x01 Open-drain */
            } else {
                GPIO.pin[io_num].pad_driver = 0; /*0x00 Normal gpio output */
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
            ESP_LOGI(GPIO_TAG, "GPIO[%d]| InputEn: %d| OutputEn: %d| OpenDrain: %d| Pullup: %d| Pulldown: %d| Intr:%d ", io_num, input_en, output_en, od_en, pu_en, pd_en, pGPIOConfig->intr_type);
            gpio_set_intr_type(io_num, pGPIOConfig->intr_type);
            if (pGPIOConfig->intr_type) {
                gpio_intr_enable(io_num);
            } else {
                gpio_intr_disable(io_num);
            }
            PIN_FUNC_SELECT(io_reg, PIN_FUNC_GPIO); /*function number 2 is GPIO_FUNC for each pin */
        }
        io_num++;
    } while (io_num < GPIO_PIN_COUNT);
    return ESP_OK;
}

esp_err_t gpio_reset_pin(gpio_num_t gpio_num)
{
    assert(gpio_num >= 0 && GPIO_IS_VALID_GPIO(gpio_num));
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

static inline void IRAM_ATTR gpio_isr_loop(uint32_t status, const uint32_t gpio_num_start) {
    while (status) {
        int nbit = __builtin_ffs(status) - 1;
        status &= ~(1 << nbit);
        int gpio_num = gpio_num_start + nbit;
        if (gpio_isr_func[gpio_num].fn != NULL) {
            gpio_isr_func[gpio_num].fn(gpio_isr_func[gpio_num].args);
        }
    }
}

static void IRAM_ATTR gpio_intr_service(void* arg)
{
    //GPIO intr process
    if (gpio_isr_func == NULL) {
        return;
    }
    //read status to get interrupt status for GPIO0-31
    uint32_t gpio_intr_status;
#ifdef CONFIG_IDF_TARGET_ESP32
    gpio_intr_status = (isr_core_id == 0) ? GPIO.pcpu_int : GPIO.acpu_int;
#else
    gpio_intr_status = GPIO.pcpu_int;
#endif
    if (gpio_intr_status) {
        gpio_isr_loop(gpio_intr_status, 0);
        GPIO.status_w1tc = gpio_intr_status;
    }

    //read status1 to get interrupt status for GPIO32-39
    uint32_t gpio_intr_status_h;
#ifdef CONFIG_IDF_TARGET_ESP32
    gpio_intr_status_h = (isr_core_id == 0) ? GPIO.pcpu_int1.intr : GPIO.acpu_int1.intr;
#else
    gpio_intr_status_h = GPIO.pcpu_int1.intr;
#endif
    if (gpio_intr_status_h) {
        gpio_isr_loop(gpio_intr_status_h, 32);
        GPIO.status1_w1tc.intr_st = gpio_intr_status_h;
    }
}

esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void* args)
{
    GPIO_CHECK(gpio_isr_func != NULL, "GPIO isr service is not installed, call gpio_install_isr_service() first", ESP_ERR_INVALID_STATE);
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&gpio_spinlock);
    gpio_intr_disable(gpio_num);
    if (gpio_isr_func) {
        gpio_isr_func[gpio_num].fn = isr_handler;
        gpio_isr_func[gpio_num].args = args;
    }
    gpio_intr_enable_on_core (gpio_num, esp_intr_get_cpu(gpio_isr_handle));
    portEXIT_CRITICAL(&gpio_spinlock);
    return ESP_OK;
}

esp_err_t gpio_isr_handler_remove(gpio_num_t gpio_num)
{
    GPIO_CHECK(gpio_isr_func != NULL, "GPIO isr service is not installed, call gpio_install_isr_service() first", ESP_ERR_INVALID_STATE);
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&gpio_spinlock);
    gpio_intr_disable(gpio_num);
    if (gpio_isr_func) {
        gpio_isr_func[gpio_num].fn = NULL;
        gpio_isr_func[gpio_num].args = NULL;
    }
    portEXIT_CRITICAL(&gpio_spinlock);
    return ESP_OK;
}

esp_err_t gpio_install_isr_service(int intr_alloc_flags)
{
    GPIO_CHECK(gpio_isr_func == NULL, "GPIO isr service already installed", ESP_ERR_INVALID_STATE);
    esp_err_t ret;
    portENTER_CRITICAL(&gpio_spinlock);
    gpio_isr_func = (gpio_isr_func_t*) calloc(GPIO_NUM_MAX, sizeof(gpio_isr_func_t));
    portEXIT_CRITICAL(&gpio_spinlock);
    if (gpio_isr_func == NULL) {
        ret = ESP_ERR_NO_MEM;
    } else {
        ret = gpio_isr_register(gpio_intr_service, NULL, intr_alloc_flags, &gpio_isr_handle);
    }
    return ret;
}

void gpio_uninstall_isr_service(void)
{
    if (gpio_isr_func == NULL) {
        return;
    }
    portENTER_CRITICAL(&gpio_spinlock);
    esp_intr_free(gpio_isr_handle);
    free(gpio_isr_func);
    gpio_isr_func = NULL;
    isr_core_id = GPIO_ISR_CORE_ID_UNINIT;
    portEXIT_CRITICAL(&gpio_spinlock);
    return;
}

static void gpio_isr_register_on_core_static(void *param)
{
    gpio_isr_alloc_t *p = (gpio_isr_alloc_t *)param;
    //We need to check the return value.
    p->ret = esp_intr_alloc(p->source, p->intr_alloc_flags, p->fn, p->arg, p->handle);
}

esp_err_t gpio_isr_register(void (*fn)(void*), void * arg, int intr_alloc_flags, gpio_isr_handle_t *handle)
{
    GPIO_CHECK(fn, "GPIO ISR null", ESP_ERR_INVALID_ARG);
    gpio_isr_alloc_t p;
    p.source = ETS_GPIO_INTR_SOURCE;
    p.intr_alloc_flags = intr_alloc_flags;
    p.fn = fn;
    p.arg = arg;
    p.handle = handle;
    portENTER_CRITICAL(&gpio_spinlock);
    if(isr_core_id == GPIO_ISR_CORE_ID_UNINIT) {
        isr_core_id = xPortGetCoreID();
    }
    portEXIT_CRITICAL(&gpio_spinlock);
    esp_err_t ret;
#if CONFIG_FREERTOS_UNICORE
    gpio_isr_register_on_core_static(&p);
    ret = ESP_OK;
#else /* CONFIG_FREERTOS_UNICORE */
    ret = esp_ipc_call_blocking(isr_core_id, gpio_isr_register_on_core_static, (void *)&p);
#endif /* !CONFIG_FREERTOS_UNICORE */
    if(ret != ESP_OK || p.ret != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    if (( intr_type == GPIO_INTR_LOW_LEVEL ) || ( intr_type == GPIO_INTR_HIGH_LEVEL )) {
        if (RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
            ret = rtc_gpio_wakeup_enable(gpio_num, intr_type);
        } else {
            GPIO.pin[gpio_num].int_type = intr_type;
            GPIO.pin[gpio_num].wakeup_enable = 0x1;
        }
    } else {
        ESP_LOGE(GPIO_TAG, "GPIO wakeup only supports level mode, but edge mode set. gpio_num:%u", gpio_num);
        ret = ESP_ERR_INVALID_ARG;
    }
    return ret;
}

esp_err_t gpio_wakeup_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO.pin[gpio_num].wakeup_enable = 0;
    return ESP_OK;
}

esp_err_t gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(strength < GPIO_DRIVE_CAP_MAX, "GPIO drive capability error", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    if (RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        rtc_gpio_set_drive_capability(gpio_num, strength);
    } else {
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[gpio_num], FUN_DRV_V, strength, FUN_DRV_S);
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[gpio_num], FUN_DRV_V, strength, FUN_DRV_S);
#endif
    return ESP_OK;
}

esp_err_t gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t* strength)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "GPIO number error", ESP_ERR_INVALID_ARG);
    GPIO_CHECK(strength != NULL, "GPIO drive capability pointer error", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    if (RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        return rtc_gpio_get_drive_capability(gpio_num, strength);
    } else {
        *strength = GET_PERI_REG_BITS2(GPIO_PIN_MUX_REG[gpio_num], FUN_DRV_V, FUN_DRV_S);
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    *strength = GET_PERI_REG_BITS2(GPIO_PIN_MUX_REG[gpio_num], FUN_DRV_V, FUN_DRV_S);
#endif
    return ESP_OK;
}

#if CONFIG_IDF_TARGET_ESP32
static const uint32_t GPIO_HOLD_MASK[34] = {
    0,
    GPIO_SEL_1,
    0,
    GPIO_SEL_0,
    0,
    GPIO_SEL_8,
    GPIO_SEL_2,
    GPIO_SEL_3,
    GPIO_SEL_4,
    GPIO_SEL_5,
    GPIO_SEL_6,
    GPIO_SEL_7,
    0,
    0,
    0,
    0,
    GPIO_SEL_9,
    GPIO_SEL_10,
    GPIO_SEL_11,
    GPIO_SEL_12,
    0,
    GPIO_SEL_14,
    GPIO_SEL_15,
    GPIO_SEL_16,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
#endif

esp_err_t gpio_hold_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Only output-capable GPIO support this function", ESP_ERR_NOT_SUPPORTED);
    esp_err_t r = ESP_OK;
    if (RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        r = rtc_gpio_hold_en(gpio_num);
#if CONFIG_IDF_TARGET_ESP32
    } else if (GPIO_HOLD_MASK[gpio_num]) {
        SET_PERI_REG_MASK(RTC_IO_DIG_PAD_HOLD_REG, GPIO_HOLD_MASK[gpio_num]);
    } else {
        r = ESP_ERR_NOT_SUPPORTED;
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    } else {
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PAD_HOLD_REG, BIT(gpio_num - RTC_GPIO_NUMBER));
    }
#endif
    return r == ESP_OK ? ESP_OK : ESP_ERR_NOT_SUPPORTED;
}

esp_err_t gpio_hold_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Only output-capable GPIO support this function", ESP_ERR_NOT_SUPPORTED);
    esp_err_t r = ESP_OK;
    if (RTC_GPIO_IS_VALID_GPIO(gpio_num)) {
        r = rtc_gpio_hold_dis(gpio_num);
#if CONFIG_IDF_TARGET_ESP32 
    }else if (GPIO_HOLD_MASK[gpio_num]) {
        CLEAR_PERI_REG_MASK(RTC_IO_DIG_PAD_HOLD_REG, GPIO_HOLD_MASK[gpio_num]);
    } else {
        r = ESP_ERR_NOT_SUPPORTED;
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PAD_HOLD_REG, BIT(gpio_num - RTC_GPIO_NUMBER));
    }
#endif
    return r == ESP_OK ? ESP_OK : ESP_ERR_NOT_SUPPORTED;
}

void gpio_deep_sleep_hold_en(void)
{
    portENTER_CRITICAL(&gpio_spinlock);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_AUTOHOLD_EN_M);
    portEXIT_CRITICAL(&gpio_spinlock);
}

void gpio_deep_sleep_hold_dis(void)
{
    portENTER_CRITICAL(&gpio_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_AUTOHOLD_EN_M);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_CLR_DG_PAD_AUTOHOLD);
#endif
    portEXIT_CRITICAL(&gpio_spinlock);
}

#if CONFIG_IDF_TARGET_ESP32S2BETA

esp_err_t gpio_force_hold_all()
{
    rtc_gpio_force_hold_all();
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_UNHOLD);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_HOLD);
    return ESP_OK;
}

esp_err_t gpio_force_unhold_all()
{
    rtc_gpio_force_hold_dis_all();
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_HOLD);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_UNHOLD);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_CLR_DG_PAD_AUTOHOLD);
    return ESP_OK;
}
#endif

void gpio_iomux_in(uint32_t gpio, uint32_t signal_idx)
{
    GPIO.func_in_sel_cfg[signal_idx].sig_in_sel = 0;
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[gpio]);
}

void gpio_iomux_out(uint8_t gpio_num, int func, bool oen_inv)
{
    GPIO.func_out_sel_cfg[gpio_num].oen_sel = 0;
    GPIO.func_out_sel_cfg[gpio_num].oen_inv_sel = oen_inv;
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], func);
}
