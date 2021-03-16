// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <stdlib.h>
#include <string.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_compiler.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "soc/gpio_periph.h"
#include "soc/io_mux_reg.h"
#include "hal/cpu_hal.h"
#include "hal/cpu_ll.h"
#include "hal/gpio_hal.h"
#include "driver/periph_ctrl.h"
#include "esp_rom_gpio.h"
#include "freertos/FreeRTOS.h"
#include "driver/dedic_gpio.h"
#include "soc/dedic_gpio_periph.h"
#if SOC_DEDIC_GPIO_ALLOW_REG_ACCESS
#include "soc/dedic_gpio_struct.h"
#include "hal/dedic_gpio_ll.h"
#endif


static const char *TAG = "dedic_gpio";

#define DEDIC_CHECK(a, msg, tag, ret, ...)                                        \
    do {                                                                          \
        if (unlikely(!(a))) {                                                     \
            ESP_LOGE(TAG, "%s(%d): " msg, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret_code = ret;                                                       \
            goto tag;                                                             \
        }                                                                         \
    } while (0)

typedef struct dedic_gpio_platform_t dedic_gpio_platform_t;
typedef struct dedic_gpio_bundle_t dedic_gpio_bundle_t;

// Dedicated GPIO driver platform, GPIO bundles will be installed onto it
static dedic_gpio_platform_t *s_platform[SOC_CPU_CORES_NUM];
// platform level mutex lock
static _lock_t s_platform_mutexlock[SOC_CPU_CORES_NUM];

struct dedic_gpio_platform_t {
    portMUX_TYPE spinlock;      // Spinlock, stop GPIO channels from accessing common resource concurrently
    uint32_t out_occupied_mask; // mask of output channels that already occupied
    uint32_t in_occupied_mask;  // mask of input channels that already occupied
#if SOC_DEDIC_GPIO_HAS_INTERRUPT
    intr_handle_t intr_hdl;     // interrupt handle
    dedic_gpio_isr_callback_t cbs[SOC_DEDIC_GPIO_IN_CHANNELS_NUM];   // array of callback function for input channel
    void *cb_args[SOC_DEDIC_GPIO_IN_CHANNELS_NUM];                   // array of callback arguments for input channel
    dedic_gpio_bundle_t *in_bundles[SOC_DEDIC_GPIO_IN_CHANNELS_NUM]; // which bundle belongs to for input channel
#endif
#if SOC_DEDIC_GPIO_ALLOW_REG_ACCESS
    dedic_dev_t *dev;
#endif
};

struct dedic_gpio_bundle_t {
    uint32_t core_id;    // CPU core ID, a GPIO bundle must be installed to a specific CPU core
    uint32_t out_mask;   // mask of output channels in the bank
    uint32_t in_mask;    // mask of input channels in the bank
    uint32_t out_offset; // offset in the bank (seen from output channel)
    uint32_t in_offset;  // offset in the bank (seen from input channel)
    size_t nr_gpio;    // number of GPIOs in the gpio_array
    int gpio_array[0];   // array of GPIO numbers (configured by user)
};

static esp_err_t dedic_gpio_build_platform(uint32_t core_id)
{
    esp_err_t ret_code = ESP_OK;
    if (!s_platform[core_id]) {
        // prevent building platform concurrently
        _lock_acquire(&s_platform_mutexlock[core_id]);
        if (!s_platform[core_id]) {
            s_platform[core_id] = calloc(1, sizeof(dedic_gpio_platform_t));
            if (s_platform[core_id]) {
                // initialize platfrom members
                s_platform[core_id]->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
#if SOC_DEDIC_GPIO_ALLOW_REG_ACCESS
                s_platform[core_id]->dev = &DEDIC_GPIO;
#endif
                periph_module_enable(dedic_gpio_periph_signals.module); // enable APB clock to peripheral
            }
        }
        _lock_release(&s_platform_mutexlock[core_id]);

        DEDIC_CHECK(s_platform[core_id], "no mem for s_platform[%d]", err, ESP_ERR_NO_MEM, core_id);
        ESP_LOGD(TAG, "build platform on core[%d] at %p", core_id, s_platform);
    }

err:
    return ret_code;
}

static void dedic_gpio_break_platform(uint32_t core_id)
{
    if (s_platform[core_id]) {
        // prevent breaking platform concurrently
        _lock_acquire(&s_platform_mutexlock[core_id]);
        if (s_platform[core_id]) {
            free(s_platform[core_id]);
            s_platform[core_id] = NULL;
            periph_module_disable(dedic_gpio_periph_signals.module); // disable module if no GPIO channel is being used
        }
        _lock_release(&s_platform_mutexlock[core_id]);
    }
}

#if SOC_DEDIC_GPIO_HAS_INTERRUPT
static void dedic_gpio_default_isr(void *arg)
{
    bool need_yield = false;
    dedic_gpio_platform_t *platform = (dedic_gpio_platform_t *)arg;

    // get and clear interrupt status
    portENTER_CRITICAL_ISR(&platform->spinlock);
    uint32_t status = dedic_gpio_ll_get_interrupt_status(platform->dev);
    dedic_gpio_ll_clear_interrupt_status(platform->dev, status);
    portEXIT_CRITICAL_ISR(&platform->spinlock);

    // handle dedicated channel one by one
    while (status) {
        uint32_t channel = __builtin_ffs(status) - 1; // get dedicated channel number which triggered the interrupt
        if (platform->cbs[channel]) {
            if (platform->cbs[channel](platform->in_bundles[channel], channel - platform->in_bundles[channel]->in_offset, platform->cb_args[channel])) {
                need_yield = true; // note that we need to yield at the end of isr
            }
        }
        status = status & (status - 1); // clear the right most bit '1'
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t dedic_gpio_install_interrupt(uint32_t core_id)
{
    esp_err_t ret_code = ESP_OK;
    if (!s_platform[core_id]->intr_hdl) {
        // prevent install interrupt concurrently
        _lock_acquire(&s_platform_mutexlock[core_id]);
        if (!s_platform[core_id]->intr_hdl) {
            int isr_flags = 0;
            ret_code = esp_intr_alloc(dedic_gpio_periph_signals.irq, isr_flags, dedic_gpio_default_isr, s_platform[core_id], &s_platform[core_id]->intr_hdl);
            // clear pending interrupt
            uint32_t status = dedic_gpio_ll_get_interrupt_status(s_platform[core_id]->dev);
            dedic_gpio_ll_clear_interrupt_status(s_platform[core_id]->dev, status);
        }
        _lock_release(&s_platform_mutexlock[core_id]);
        DEDIC_CHECK(ret_code == ESP_OK, "alloc interrupt failed", err, ret_code);
    }

err:
    return ret_code;
}

static void dedic_gpio_uninstall_interrupt(uint32_t core_id)
{
    if (s_platform[core_id]->intr_hdl) {
        // prevent uninstall interrupt concurrently
        _lock_acquire(&s_platform_mutexlock[core_id]);
        if (s_platform[core_id]->intr_hdl) {
            esp_intr_free(s_platform[core_id]->intr_hdl);
            s_platform[core_id]->intr_hdl = NULL;
            // disable all interrupt
            dedic_gpio_ll_enable_interrupt(s_platform[core_id]->dev, ~0UL, false);
        }
        _lock_release(&s_platform_mutexlock[core_id]);
    }
}

static void dedic_gpio_set_interrupt(uint32_t core_id, uint32_t channel, dedic_gpio_intr_type_t type)
{
    dedic_gpio_ll_set_interrupt_type(s_platform[core_id]->dev, channel, type);
    if (type != DEDIC_GPIO_INTR_NONE) {
        dedic_gpio_ll_enable_interrupt(s_platform[core_id]->dev, 1 << channel, true);
    } else {
        dedic_gpio_ll_enable_interrupt(s_platform[core_id]->dev, 1 << channel, false);
    }
}
#endif // SOC_DEDIC_GPIO_HAS_INTERRUPT

esp_err_t dedic_gpio_new_bundle(const dedic_gpio_bundle_config_t *config, dedic_gpio_bundle_handle_t *ret_bundle)
{
    esp_err_t ret_code = ESP_OK;
    dedic_gpio_bundle_t *bundle = NULL;
    uint32_t out_mask = 0;
    uint32_t in_mask = 0;
    uint32_t core_id = cpu_hal_get_core_id(); // dedicated GPIO will be binded to the CPU who invokes this API

    DEDIC_CHECK(config && ret_bundle, "invalid argument", err, ESP_ERR_INVALID_ARG);
    DEDIC_CHECK(config->gpio_array && config->array_size > 0, "invalid GPIO array or size", err, ESP_ERR_INVALID_ARG);
    DEDIC_CHECK(config->flags.in_en || config->flags.out_en, "no input/output mode specified", err, ESP_ERR_INVALID_ARG);
    // lazy install s_platform[core_id]
    DEDIC_CHECK(dedic_gpio_build_platform(core_id) == ESP_OK, "build platform %d failed", err, ESP_FAIL, core_id);

    size_t bundle_size = sizeof(dedic_gpio_bundle_t) + config->array_size * sizeof(config->gpio_array[0]);
    bundle = calloc(1, bundle_size);
    DEDIC_CHECK(bundle, "no mem for bundle", err, ESP_ERR_NO_MEM);

    // for performance reasons, we only search for continuous channels
    uint32_t pattern = (1 << config->array_size) - 1;
    // configure outwards channels
    uint32_t out_offset = 0;
    if (config->flags.out_en) {
        DEDIC_CHECK(SOC_DEDIC_GPIO_OUT_CHANNELS_NUM >= config->array_size, "array size(%d) exceeds maximum supported out channels(%d)",
                    err, ESP_ERR_INVALID_ARG, config->array_size, SOC_DEDIC_GPIO_OUT_CHANNELS_NUM);
        // prevent install bundle concurrently
        portENTER_CRITICAL(&s_platform[core_id]->spinlock);
        for (size_t i = 0; i <= SOC_DEDIC_GPIO_OUT_CHANNELS_NUM - config->array_size; i++) {
            if ((s_platform[core_id]->out_occupied_mask & (pattern << i)) == 0) {
                out_mask = pattern << i;
                out_offset = i;
                break;
            }
        }
        if (out_mask) {
            s_platform[core_id]->out_occupied_mask |= out_mask;
#if SOC_DEDIC_GPIO_ALLOW_REG_ACCESS
            // always enable instruction to access output GPIO, which has better performance than register access
            dedic_gpio_ll_enable_instruction_access_out(s_platform[core_id]->dev, out_mask, true);
#endif
        }
        portEXIT_CRITICAL(&s_platform[core_id]->spinlock);
        DEDIC_CHECK(out_mask, "no free outward channels on core[%d]", err, ESP_ERR_NOT_FOUND, core_id);
        ESP_LOGD(TAG, "new outward bundle(%p) on core[%d], offset=%d, mask(%x)", bundle, core_id, out_offset, out_mask);
    }

    // configure inwards channels
    uint32_t in_offset = 0;
    if (config->flags.in_en) {
        DEDIC_CHECK(SOC_DEDIC_GPIO_IN_CHANNELS_NUM >= config->array_size, "array size(%d) exceeds maximum supported in channels(%d)",
                    err, ESP_ERR_INVALID_ARG, config->array_size, SOC_DEDIC_GPIO_IN_CHANNELS_NUM);
        // prevent install bundle concurrently
        portENTER_CRITICAL(&s_platform[core_id]->spinlock);
        for (size_t i = 0; i <= SOC_DEDIC_GPIO_IN_CHANNELS_NUM - config->array_size; i++) {
            if ((s_platform[core_id]->in_occupied_mask & (pattern << i)) == 0) {
                in_mask = pattern << i;
                in_offset = i;
                break;
            }
        }
        if (in_mask) {
            s_platform[core_id]->in_occupied_mask |= in_mask;
        }
        portEXIT_CRITICAL(&s_platform[core_id]->spinlock);
        DEDIC_CHECK(in_mask, "no free inward channels on core[%d]", err, ESP_ERR_NOT_FOUND, core_id);
        ESP_LOGD(TAG, "new inward bundle(%p) on core[%d], offset=%d, mask(%x)", bundle, core_id, in_offset, in_mask);
    }

    // route dedicated GPIO channel signals to GPIO matrix
    if (config->flags.in_en) {
        for (size_t i = 0; i < config->array_size; i++) {
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[config->gpio_array[i]], PIN_FUNC_GPIO);
            esp_rom_gpio_connect_in_signal(config->gpio_array[i], dedic_gpio_periph_signals.cores[core_id].in_sig_per_channel[in_offset + i], config->flags.in_invert);
        }
    }
    if (config->flags.out_en) {
        for (size_t i = 0; i < config->array_size; i++) {
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[config->gpio_array[i]], PIN_FUNC_GPIO);
            esp_rom_gpio_connect_out_signal(config->gpio_array[i], dedic_gpio_periph_signals.cores[core_id].out_sig_per_channel[out_offset + i], config->flags.out_invert, false);
        }
    }

    // it's safe to initialize bundle members without locks here
    bundle->core_id = core_id;
    bundle->out_mask = out_mask;
    bundle->in_mask = in_mask;
    bundle->out_offset = out_offset;
    bundle->in_offset = in_offset;
    bundle->nr_gpio = config->array_size;
    memcpy(bundle->gpio_array, config->gpio_array, config->array_size * sizeof(config->gpio_array[0]));

    *ret_bundle = bundle; // return bundle instance
    return ESP_OK;

err:
    if (s_platform[core_id] && (out_mask || in_mask)) {
        portENTER_CRITICAL(&s_platform[core_id]->spinlock);
        s_platform[core_id]->out_occupied_mask &= ~out_mask;
        s_platform[core_id]->in_occupied_mask &= ~in_mask;
        portEXIT_CRITICAL(&s_platform[core_id]->spinlock);
    }
    if (bundle) {
        free(bundle);
    }
    return ret_code;
}

esp_err_t dedic_gpio_del_bundle(dedic_gpio_bundle_handle_t bundle)
{
    esp_err_t ret_code = ESP_OK;
    bool recycle_all = false;
    DEDIC_CHECK(bundle, "invalid argument", err, ESP_ERR_INVALID_ARG);

    uint32_t core_id = cpu_hal_get_core_id();
    DEDIC_CHECK(core_id == bundle->core_id, "del bundle on wrong CPU", err, ESP_FAIL);

    portENTER_CRITICAL(&s_platform[core_id]->spinlock);
    s_platform[core_id]->out_occupied_mask &= ~(bundle->out_mask);
    s_platform[core_id]->in_occupied_mask &= ~(bundle->in_mask);
    if (!s_platform[core_id]->in_occupied_mask && !s_platform[core_id]->out_occupied_mask) {
        recycle_all = true;
    }
    portEXIT_CRITICAL(&s_platform[core_id]->spinlock);

    free(bundle);

    if (recycle_all) {
#if SOC_DEDIC_GPIO_HAS_INTERRUPT
        dedic_gpio_uninstall_interrupt(core_id);
#endif
        dedic_gpio_break_platform(core_id);
    }

err:
    return ret_code;
}

esp_err_t dedic_gpio_get_out_mask(dedic_gpio_bundle_handle_t bundle, uint32_t *mask)
{
    esp_err_t ret_code = ESP_OK;
    DEDIC_CHECK(bundle && mask, "invalid argument", err, ESP_ERR_INVALID_ARG);
    *mask = bundle->out_mask;
err:
    return ret_code;
}

esp_err_t dedic_gpio_get_in_mask(dedic_gpio_bundle_handle_t bundle, uint32_t *mask)
{
    esp_err_t ret_code = ESP_OK;
    DEDIC_CHECK(bundle && mask, "invalid argument", err, ESP_ERR_INVALID_ARG);
    *mask = bundle->in_mask;
err:
    return ret_code;
}

void dedic_gpio_bundle_write(dedic_gpio_bundle_handle_t bundle, uint32_t mask, uint32_t value)
{
    // For performace reasons, we don't want to check the validation of parameters here
    // Even didn't check if we're working on the correct CPU core (i.e. bundle->core_id == current core_id)
    cpu_ll_write_dedic_gpio_mask(bundle->out_mask & (mask << bundle->out_offset), value << bundle->out_offset);
}

uint32_t dedic_gpio_bundle_read_out(dedic_gpio_bundle_handle_t bundle)
{
    // For performace reasons, we don't want to check the validation of parameters here
    // Even didn't check if we're working on the correct CPU core (i.e. bundle->core_id == current core_id)
    uint32_t value =  cpu_ll_read_dedic_gpio_out();
    return (value & bundle->out_mask) >> (bundle->out_offset);
}

uint32_t dedic_gpio_bundle_read_in(dedic_gpio_bundle_handle_t bundle)
{
    // For performace reasons, we don't want to check the validation of parameters here
    // Even didn't check if we're working on the correct CPU core (i.e. bundle->core_id == current core_id)
    uint32_t value  = cpu_ll_read_dedic_gpio_in();
    return (value & bundle->in_mask) >> (bundle->in_offset);
}

#if SOC_DEDIC_GPIO_HAS_INTERRUPT
esp_err_t dedic_gpio_bundle_set_interrupt_and_callback(dedic_gpio_bundle_handle_t bundle, uint32_t mask, dedic_gpio_intr_type_t intr_type, dedic_gpio_isr_callback_t cb_isr, void *cb_args)
{
    esp_err_t ret_code = ESP_OK;
    DEDIC_CHECK(bundle, "invalid argument", err, ESP_ERR_INVALID_ARG);
    uint32_t core_id = cpu_hal_get_core_id();
    // lazy alloc interrupt
    DEDIC_CHECK(dedic_gpio_install_interrupt(core_id) == ESP_OK, "allocate interrupt on core %d failed", err, ESP_FAIL, core_id);

    uint32_t channel_mask = bundle->in_mask & (mask << bundle->in_offset);
    uint32_t channel = 0;
    while (channel_mask) {
        channel = __builtin_ffs(channel_mask) - 1;
        portENTER_CRITICAL(&s_platform[core_id]->spinlock);
        dedic_gpio_set_interrupt(core_id, channel, intr_type);
        portEXIT_CRITICAL(&s_platform[core_id]->spinlock);

        s_platform[core_id]->cbs[channel] = cb_isr;
        s_platform[core_id]->cb_args[channel] = cb_args;
        s_platform[core_id]->in_bundles[channel] = bundle;
        channel_mask = channel_mask & (channel_mask - 1); // clear the right most bit '1'
    }

err:
    return ret_code;
}
#endif // SOC_DEDIC_GPIO_HAS_INTERRUPT
