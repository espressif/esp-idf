/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include <stdlib.h>
#include <string.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_compiler.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_cpu.h"
#include "soc/soc_caps.h"
#include "soc/gpio_periph.h"
#include "soc/io_mux_reg.h"
#include "hal/dedic_gpio_cpu_ll.h"
#include "hal/gpio_hal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_rom_gpio.h"
#include "freertos/FreeRTOS.h"
#include "driver/dedic_gpio.h"
#include "soc/dedic_gpio_periph.h"
#if SOC_DEDIC_GPIO_ALLOW_REG_ACCESS
#include "soc/dedic_gpio_struct.h"
#endif
#if !SOC_DEDIC_PERIPH_ALWAYS_ENABLE
#include "hal/dedic_gpio_ll.h"
#endif

static const char *TAG = "dedic_gpio";

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
    size_t nr_gpio;      // number of GPIOs in the gpio_array
    int gpio_array[];    // array of GPIO numbers (configured by user)
};

static esp_err_t dedic_gpio_build_platform(int core_id)
{
    esp_err_t ret = ESP_OK;
    if (!s_platform[core_id]) {
        // prevent building platform concurrently
        _lock_acquire(&s_platform_mutexlock[core_id]);
        if (!s_platform[core_id]) {
            s_platform[core_id] = calloc(1, sizeof(dedic_gpio_platform_t));
            if (s_platform[core_id]) {
                // initialize platform members
                s_platform[core_id]->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
                // initial occupy_mask: 1111...100...0
                s_platform[core_id]->out_occupied_mask = UINT32_MAX & ~((1 << SOC_DEDIC_GPIO_OUT_CHANNELS_NUM) - 1);
                s_platform[core_id]->in_occupied_mask = UINT32_MAX & ~((1 << SOC_DEDIC_GPIO_IN_CHANNELS_NUM) - 1);
#if SOC_DEDIC_GPIO_ALLOW_REG_ACCESS
                s_platform[core_id]->dev = &DEDIC_GPIO;
#endif // SOC_DEDIC_GPIO_ALLOW_REG_ACCESS
#if !SOC_DEDIC_PERIPH_ALWAYS_ENABLE
                // enable dedicated GPIO register clock
                PERIPH_RCC_ATOMIC() {
                    dedic_gpio_ll_enable_bus_clock(true);
                    dedic_gpio_ll_reset_register();
                }
#endif // !SOC_DEDIC_PERIPH_ALWAYS_ENABLE
            }
        }
        _lock_release(&s_platform_mutexlock[core_id]);

        ESP_GOTO_ON_FALSE(s_platform[core_id], ESP_ERR_NO_MEM, err, TAG, "no mem for s_platform[%d]", core_id);
        ESP_LOGD(TAG, "build platform on core[%d] at %p", core_id, s_platform);
    }

err:
    return ret;
}

static void dedic_gpio_break_platform(uint32_t core_id)
{
    if (s_platform[core_id]) {
        // prevent breaking platform concurrently
        _lock_acquire(&s_platform_mutexlock[core_id]);
        if (s_platform[core_id]) {
            free(s_platform[core_id]);
            s_platform[core_id] = NULL;
#if !SOC_DEDIC_PERIPH_ALWAYS_ENABLE
            // disable the register clock if no GPIO channel is in use
            PERIPH_RCC_ATOMIC() {
                dedic_gpio_ll_enable_bus_clock(false);
            }
#endif // !SOC_DEDIC_PERIPH_ALWAYS_ENABLE
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
    esp_err_t ret = ESP_OK;
    if (!s_platform[core_id]->intr_hdl) {
        // prevent install interrupt concurrently
        _lock_acquire(&s_platform_mutexlock[core_id]);
        if (!s_platform[core_id]->intr_hdl) {
            int isr_flags = 0;
            ret = esp_intr_alloc(dedic_gpio_periph_signals.irq, isr_flags, dedic_gpio_default_isr, s_platform[core_id], &s_platform[core_id]->intr_hdl);
            // clear pending interrupt
            uint32_t status = dedic_gpio_ll_get_interrupt_status(s_platform[core_id]->dev);
            dedic_gpio_ll_clear_interrupt_status(s_platform[core_id]->dev, status);
        }
        _lock_release(&s_platform_mutexlock[core_id]);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "alloc interrupt failed");
    }

err:
    return ret;
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
    esp_err_t ret = ESP_OK;
    dedic_gpio_bundle_t *bundle = NULL;
    uint32_t out_mask = 0;
    uint32_t in_mask = 0;
    int core_id = esp_cpu_get_core_id(); // dedicated GPIO will be binded to the CPU who invokes this API

    ESP_GOTO_ON_FALSE(config && ret_bundle, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->gpio_array && config->array_size > 0, ESP_ERR_INVALID_ARG, err, TAG, "invalid GPIO array or size");
    ESP_GOTO_ON_FALSE(config->flags.in_en || config->flags.out_en, ESP_ERR_INVALID_ARG, err, TAG, "no input/output mode specified");
    // lazy install s_platform[core_id]
    ESP_GOTO_ON_ERROR(dedic_gpio_build_platform(core_id), err, TAG, "build platform %d failed", core_id);

    size_t bundle_size = sizeof(dedic_gpio_bundle_t) + config->array_size * sizeof(config->gpio_array[0]);
    bundle = calloc(1, bundle_size);
    ESP_GOTO_ON_FALSE(bundle, ESP_ERR_NO_MEM, err, TAG, "no mem for bundle");

    // for performance reasons, we only search for continuous channels
    uint32_t pattern = (1 << config->array_size) - 1;
    // configure outwards channels
    uint32_t out_offset = 0;
    if (config->flags.out_en) {
        ESP_GOTO_ON_FALSE(config->array_size <= SOC_DEDIC_GPIO_OUT_CHANNELS_NUM, ESP_ERR_INVALID_ARG, err, TAG,
                          "array size(%d) exceeds maximum supported out channels(%d)", config->array_size, SOC_DEDIC_GPIO_OUT_CHANNELS_NUM);
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
        ESP_GOTO_ON_FALSE(out_mask, ESP_ERR_NOT_FOUND, err, TAG, "no free outward channels on core[%d]", core_id);
        ESP_LOGD(TAG, "new outward bundle(%p) on core[%d], offset=%"PRIu32", mask(%"PRIx32")", bundle, core_id, out_offset, out_mask);
    }

    // configure inwards channels
    uint32_t in_offset = 0;
    if (config->flags.in_en) {
        ESP_GOTO_ON_FALSE(config->array_size <= SOC_DEDIC_GPIO_IN_CHANNELS_NUM, ESP_ERR_INVALID_ARG, err, TAG,
                          "array size(%d) exceeds maximum supported in channels(%d)", config->array_size, SOC_DEDIC_GPIO_IN_CHANNELS_NUM);
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
        ESP_GOTO_ON_FALSE(in_mask, ESP_ERR_NOT_FOUND, err, TAG, "no free inward channels on core[%d]", core_id);
        ESP_LOGD(TAG, "new inward bundle(%p) on core[%d], offset=%"PRIu32", mask(%"PRIx32")", bundle, core_id, in_offset, in_mask);
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
#if !SOC_DEDIC_GPIO_OUT_AUTO_ENABLE
        dedic_gpio_cpu_ll_enable_output(s_platform[core_id]->out_occupied_mask);
#endif // !SOC_DEDIC_GPIO_OUT_AUTO_ENABLE
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
    return ret;
}

esp_err_t dedic_gpio_del_bundle(dedic_gpio_bundle_handle_t bundle)
{
    esp_err_t ret = ESP_OK;
    bool recycle_all = false;
    ESP_GOTO_ON_FALSE(bundle, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    uint32_t core_id = esp_cpu_get_core_id();
    ESP_GOTO_ON_FALSE(core_id == bundle->core_id, ESP_FAIL, err, TAG, "del bundle on wrong CPU");

    portENTER_CRITICAL(&s_platform[core_id]->spinlock);
    s_platform[core_id]->out_occupied_mask &= ~(bundle->out_mask);
    s_platform[core_id]->in_occupied_mask &= ~(bundle->in_mask);
    if (s_platform[core_id]->in_occupied_mask == (UINT32_MAX & ~((1 << SOC_DEDIC_GPIO_IN_CHANNELS_NUM) - 1)) &&
            s_platform[core_id]->out_occupied_mask == (UINT32_MAX & ~((1 << SOC_DEDIC_GPIO_OUT_CHANNELS_NUM) - 1))) {
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
    return ret;
}

esp_err_t dedic_gpio_get_out_mask(dedic_gpio_bundle_handle_t bundle, uint32_t *mask)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(bundle && mask, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    *mask = bundle->out_mask;
err:
    return ret;
}

esp_err_t dedic_gpio_get_in_mask(dedic_gpio_bundle_handle_t bundle, uint32_t *mask)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(bundle && mask, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    *mask = bundle->in_mask;
err:
    return ret;
}

esp_err_t dedic_gpio_get_out_offset(dedic_gpio_bundle_handle_t bundle, uint32_t *offset)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(bundle && offset, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    *offset = bundle->out_offset;
err:
    return ret;
}

esp_err_t dedic_gpio_get_in_offset(dedic_gpio_bundle_handle_t bundle, uint32_t *offset)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(bundle && offset, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    *offset = bundle->in_offset;
err:
    return ret;
}

void dedic_gpio_bundle_write(dedic_gpio_bundle_handle_t bundle, uint32_t mask, uint32_t value)
{
    // For performance reasons, we don't want to check the validation of parameters here
    // Even didn't check if we're working on the correct CPU core (i.e. bundle->core_id == current core_id)
    dedic_gpio_cpu_ll_write_mask(bundle->out_mask & (mask << bundle->out_offset), value << bundle->out_offset);
}

uint32_t dedic_gpio_bundle_read_out(dedic_gpio_bundle_handle_t bundle)
{
    // For performance reasons, we don't want to check the validation of parameters here
    // Even didn't check if we're working on the correct CPU core (i.e. bundle->core_id == current core_id)
    uint32_t value =  dedic_gpio_cpu_ll_read_out();
    return (value & bundle->out_mask) >> (bundle->out_offset);
}

uint32_t dedic_gpio_bundle_read_in(dedic_gpio_bundle_handle_t bundle)
{
    // For performance reasons, we don't want to check the validation of parameters here
    // Even didn't check if we're working on the correct CPU core (i.e. bundle->core_id == current core_id)
    uint32_t value  = dedic_gpio_cpu_ll_read_in();
    return (value & bundle->in_mask) >> (bundle->in_offset);
}

#if SOC_DEDIC_GPIO_HAS_INTERRUPT
esp_err_t dedic_gpio_bundle_set_interrupt_and_callback(dedic_gpio_bundle_handle_t bundle, uint32_t mask, dedic_gpio_intr_type_t intr_type, dedic_gpio_isr_callback_t cb_isr, void *cb_args)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(bundle, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    int core_id = esp_cpu_get_core_id();
    // lazy alloc interrupt
    ESP_GOTO_ON_ERROR(dedic_gpio_install_interrupt(core_id), err, TAG, "allocate interrupt on core %d failed", core_id);

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
    return ret;
}
#endif // SOC_DEDIC_GPIO_HAS_INTERRUPT
