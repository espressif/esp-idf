/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <inttypes.h>
#include "sdkconfig.h"
#if CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "esp_clk_tree.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_memory_utils.h"
#include "soc/periph_defs.h"
#include "soc/ana_cmpr_periph.h"
#include "hal/ana_cmpr_ll.h"
#include "driver/ana_cmpr.h"
#include "driver/gpio.h"
#include "esp_private/io_mux.h"
#include "esp_private/esp_clk.h"

struct ana_cmpr_t {
    ana_cmpr_unit_t             unit;               /*!< Analog comparator unit id */
    analog_cmpr_dev_t           *dev;               /*!< Analog comparator unit device address */
    ana_cmpr_ref_source_t       ref_src;            /*!< Analog comparator reference source, internal or external */
    bool                        is_enabled;         /*!< Whether the Analog comparator unit is enabled */
    ana_cmpr_event_callbacks_t  cbs;                /*!< The callback group that set by user */
    intr_handle_t               intr_handle;        /*!< Interrupt handle */
    uint32_t                    intr_mask;          /*!< Interrupt mask */
    int                         intr_priority;      /*!< Interrupt priority */
    void                        *user_data;         /*!< User data that passed to the callbacks */
    uint32_t                    src_clk_freq_hz;    /*!< Source clock frequency of the Analog Comparator unit */
    esp_pm_lock_handle_t        pm_lock;            /*!< The Power Management lock that used to avoid unexpected power down of the clock domain */
};

/* Helper macros */
#define ANA_CMPR_NULL_POINTER_CHECK(p)      ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")
#define ANA_CMPR_NULL_POINTER_CHECK_ISR(p)  ESP_RETURN_ON_FALSE_ISR((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")
#define ANA_CMPR_UNIT_CHECK(unit)           ESP_RETURN_ON_FALSE((unit) >= 0 && (unit) < SOC_ANA_CMPR_NUM, \
                                                                ESP_ERR_INVALID_ARG, TAG, "invalid uint number");

/* Memory allocation caps which decide the section that memory supposed to allocate */
#if CONFIG_ANA_CMPR_ISR_IRAM_SAFE
#define ANA_CMPR_MEM_ALLOC_CAPS         (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#define ANA_CMPR_INTR_FLAG              (ESP_INTR_FLAG_IRAM)
#else
#define ANA_CMPR_MEM_ALLOC_CAPS         MALLOC_CAP_DEFAULT
#define ANA_CMPR_INTR_FLAG              (0)
#endif

/* Driver tag */
static const char *TAG = "ana_cmpr";

/* Global static object of the Analog Comparator unit */
static ana_cmpr_handle_t s_ana_cmpr[SOC_ANA_CMPR_NUM] = {
    [0 ...(SOC_ANA_CMPR_NUM - 1)] = NULL,
};

/* Global spin lock */
static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR s_ana_cmpr_default_intr_handler(void *usr_data)
{
    ana_cmpr_handle_t cmpr_handle = (ana_cmpr_handle_t)usr_data;
    bool need_yield = false;
    ana_cmpr_cross_event_data_t evt_data = {.cross_type = ANA_CMPR_CROSS_ANY};
    /* Get and clear the interrupt status */
    uint32_t status = analog_cmpr_ll_get_intr_status(cmpr_handle->dev);
    analog_cmpr_ll_clear_intr(cmpr_handle->dev, status);

    /* Call the user callback function if it is specified and the corresponding event triggers*/
    if (cmpr_handle->cbs.on_cross && (status & cmpr_handle->intr_mask)) {
#if SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE
        if (status & ANALOG_CMPR_LL_POS_CROSS_MASK(cmpr_handle->unit)) {
            evt_data.cross_type = ANA_CMPR_CROSS_POS;
        } else if (status & ANALOG_CMPR_LL_NEG_CROSS_MASK(cmpr_handle->unit)) {
            evt_data.cross_type = ANA_CMPR_CROSS_NEG;
        }
#endif  // SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE
        need_yield = cmpr_handle->cbs.on_cross(cmpr_handle, &evt_data, cmpr_handle->user_data);
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t s_ana_cmpr_init_gpio(ana_cmpr_handle_t cmpr, bool is_external_ref)
{
    uint64_t pin_mask = BIT64(ana_cmpr_periph[cmpr->unit].src_gpio);
    if (is_external_ref) {
        pin_mask |= BIT64(ana_cmpr_periph[cmpr->unit].ext_ref_gpio);
    }
    gpio_config_t ana_cmpr_gpio_cfg = {
        .pin_bit_mask = pin_mask,
        .mode = GPIO_MODE_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    return gpio_config(&ana_cmpr_gpio_cfg);
}

esp_err_t ana_cmpr_new_unit(const ana_cmpr_config_t *config, ana_cmpr_handle_t *ret_cmpr)
{
#if CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    ANA_CMPR_NULL_POINTER_CHECK(config);
    ANA_CMPR_NULL_POINTER_CHECK(ret_cmpr);
    ana_cmpr_unit_t unit = config->unit;
    ANA_CMPR_UNIT_CHECK(unit);
    ESP_RETURN_ON_FALSE(config->intr_priority >= 0 && config->intr_priority <= 7, ESP_ERR_INVALID_ARG, TAG, "interrupt priority should be within 0~7");
    ESP_RETURN_ON_FALSE(!s_ana_cmpr[unit], ESP_ERR_INVALID_STATE, TAG,
                        "unit has been allocated already");
    esp_err_t ret = ESP_OK;

    /* Allocate analog comparator unit */
    s_ana_cmpr[unit] = (ana_cmpr_handle_t)heap_caps_calloc(1, sizeof(struct ana_cmpr_t), ANA_CMPR_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(s_ana_cmpr[unit], ESP_ERR_NO_MEM, TAG, "no memory for analog comparator struct");

    /* Assign analog comparator unit */
    s_ana_cmpr[unit]->dev = ANALOG_CMPR_LL_GET_HW(unit);
    s_ana_cmpr[unit]->ref_src = config->ref_src;
    s_ana_cmpr[unit]->intr_priority = config->intr_priority;
    s_ana_cmpr[unit]->is_enabled = false;
    s_ana_cmpr[unit]->pm_lock = NULL;

#if CONFIG_PM_ENABLE
    /* Create PM lock */
    char lock_name[10] = "ana_cmpr\0";
    lock_name[8] = '0' + unit;
    ret  = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, lock_name, &s_ana_cmpr[unit]->pm_lock);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "create NO_LIGHT_SLEEP, lock failed");
#endif

    if (!config->flags.io_loop_back) {
        ESP_GOTO_ON_ERROR(s_ana_cmpr_init_gpio(s_ana_cmpr[unit], config->ref_src == ANA_CMPR_REF_SRC_EXTERNAL), err, TAG, "failed to initialize GPIO");
    }

    /* Analog clock comes from IO MUX, but IO MUX clock might be shared with other submodules as well */
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)config->clk_src,
                                                   ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,
                                                   &s_ana_cmpr[unit]->src_clk_freq_hz),
                      err, TAG, "get source clock frequency failed");
    ESP_GOTO_ON_ERROR(io_mux_set_clock_source((soc_module_clk_t)(config->clk_src)), err, TAG,
                      "potential clock source conflicts from other IOMUX peripherals");

    /* Configure the register */
    portENTER_CRITICAL(&s_spinlock);
    analog_cmpr_ll_set_ref_source(s_ana_cmpr[unit]->dev, config->ref_src);
#if !SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE
    analog_cmpr_ll_set_cross_type(s_ana_cmpr[unit]->dev, config->cross_type);
#endif  // SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE
    /* Record the interrupt mask, the interrupt will be lazy installed when register the callbacks */
    s_ana_cmpr[unit]->intr_mask = analog_cmpr_ll_get_intr_mask_by_type(s_ana_cmpr[unit]->dev, config->cross_type);
    portEXIT_CRITICAL(&s_spinlock);

    if (config->ref_src == ANA_CMPR_REF_SRC_INTERNAL) {
        ESP_LOGD(TAG, "unit %d allocated, source signal: GPIO %d, reference signal: internal",
                 (int)unit, ana_cmpr_periph[unit].src_gpio);
    } else {
        ESP_LOGD(TAG, "unit %d allocated, source signal: GPIO %d, reference signal: GPIO %d",
                 (int)unit, ana_cmpr_periph[unit].src_gpio, ana_cmpr_periph[unit].ext_ref_gpio);
    }

    *ret_cmpr = s_ana_cmpr[unit];
    return ESP_OK;

err:
    /* Delete the unit if allocation failed */
    ana_cmpr_del_unit(s_ana_cmpr[unit]);
    return ret;
}

esp_err_t ana_cmpr_del_unit(ana_cmpr_handle_t cmpr)
{
    ANA_CMPR_NULL_POINTER_CHECK(cmpr);
    /* Search the global object array to check if the input handle is valid */
    int unit = -1;
    for (int i = 0; i < SOC_ANA_CMPR_NUM; i++) {
        if (s_ana_cmpr[i] == cmpr) {
            unit = i;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(unit != -1, ESP_ERR_INVALID_ARG, TAG, "wrong analog comparator handle");
    ESP_RETURN_ON_FALSE(!cmpr->is_enabled, ESP_ERR_INVALID_STATE, TAG, "this analog comparator unit not disabled yet");

    /* Delete the pm lock if the unit has */
    if (cmpr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(cmpr->pm_lock), TAG, "delete pm lock failed");
    }

    /* Free interrupt and other resources */
    if (cmpr->intr_handle) {
        esp_intr_free(cmpr->intr_handle);
    }

    free(s_ana_cmpr[unit]);
    s_ana_cmpr[unit] = NULL;

    ESP_LOGD(TAG, "unit %d deleted", (int)unit);

    return ESP_OK;
}

esp_err_t ana_cmpr_set_internal_reference(ana_cmpr_handle_t cmpr, const ana_cmpr_internal_ref_config_t *ref_cfg)
{
    ANA_CMPR_NULL_POINTER_CHECK_ISR(cmpr);
    ANA_CMPR_NULL_POINTER_CHECK_ISR(ref_cfg);
    ESP_RETURN_ON_FALSE_ISR(cmpr->ref_src == ANA_CMPR_REF_SRC_INTERNAL, ESP_ERR_INVALID_STATE,
                            TAG, "the reference channel is not internal, no need to configure internal reference");

    /* Set internal reference voltage */
    portENTER_CRITICAL_SAFE(&s_spinlock);
    analog_cmpr_ll_set_internal_ref_voltage(cmpr->dev, ref_cfg->ref_volt);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    ESP_EARLY_LOGD(TAG, "unit %d internal voltage level %" PRIu32, (int)cmpr->unit, (uint32_t)ref_cfg->ref_volt);

    return ESP_OK;
}

esp_err_t ana_cmpr_set_debounce(ana_cmpr_handle_t cmpr, const ana_cmpr_debounce_config_t *dbc_cfg)
{
    ANA_CMPR_NULL_POINTER_CHECK_ISR(cmpr);
    ANA_CMPR_NULL_POINTER_CHECK_ISR(dbc_cfg);

    /* Transfer the time to clock cycles */
    uint32_t wait_cycle = (uint32_t)(dbc_cfg->wait_us * (cmpr->src_clk_freq_hz / 1000000));
    /* Set the waiting clock cycles */
    portENTER_CRITICAL_SAFE(&s_spinlock);
    analog_cmpr_ll_set_debounce_cycle(cmpr->dev, wait_cycle);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    ESP_EARLY_LOGD(TAG, "unit %d debounce wait cycle %"PRIu32, (int)cmpr->unit, wait_cycle);

    return ESP_OK;
}

esp_err_t ana_cmpr_set_cross_type(ana_cmpr_handle_t cmpr, ana_cmpr_cross_type_t cross_type)
{
#if SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE
    /* Not support to set the cross type after initialized, because it relies on the interrupt types to distinguish the edge,
     * i.e. have to re-allocate the interrupt to change the cross type */
    (void)cmpr;
    (void)cross_type;
    return ESP_ERR_NOT_SUPPORTED;
#else
    ANA_CMPR_NULL_POINTER_CHECK_ISR(cmpr);
    ESP_RETURN_ON_FALSE_ISR(cross_type >= ANA_CMPR_CROSS_DISABLE && cross_type <= ANA_CMPR_CROSS_ANY,
                            ESP_ERR_INVALID_ARG, TAG, "invalid cross type");

    portENTER_CRITICAL_SAFE(&s_spinlock);
#if !SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE
    analog_cmpr_ll_set_cross_type(cmpr->dev, cross_type);
#endif
    cmpr->intr_mask = analog_cmpr_ll_get_intr_mask_by_type(cmpr->dev, cross_type);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    ESP_EARLY_LOGD(TAG, "unit %d cross type updated to %d", (int)cmpr->unit, cross_type);

    return ESP_OK;
#endif
}

esp_err_t ana_cmpr_register_event_callbacks(ana_cmpr_handle_t cmpr, const ana_cmpr_event_callbacks_t *cbs, void *user_data)
{
    ANA_CMPR_NULL_POINTER_CHECK(cmpr);
    ANA_CMPR_NULL_POINTER_CHECK(cbs);
    ESP_RETURN_ON_FALSE(!cmpr->is_enabled, ESP_ERR_INVALID_STATE, TAG,
                        "please disable the analog comparator before registering the callbacks");
#if CONFIG_ANA_CMPR_ISR_IRAM_SAFE
    if (cbs->on_cross) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_cross), ESP_ERR_INVALID_ARG, TAG,
                            "ANA_CMPR_ISR_IRAM_SAFE enabled but the callback function is not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(user_data), ESP_ERR_INVALID_ARG, TAG,
                            "ANA_CMPR_ISR_IRAM_SAFE enabled but the user_data is not in IRAM");
    }
#endif

    /* Allocate the interrupt, the interrupt source of Analog Comparator is shared with GPIO interrupt source on ESP32H2 */
    if (!cmpr->intr_handle) {
        int intr_flags = ANA_CMPR_INTR_FLAG | (cmpr->intr_priority ?  BIT(cmpr->intr_priority) : ESP_INTR_FLAG_LOWMED);
#if SOC_ANA_CMPR_INTR_SHARE_WITH_GPIO
        intr_flags |= ESP_INTR_FLAG_SHARED;
#endif  // SOC_ANA_CMPR_INTR_SHARE_WITH_GPIO
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(ana_cmpr_periph[cmpr->unit].intr_src, intr_flags, (uint32_t)analog_cmpr_ll_get_intr_status_reg(cmpr->dev),
                                                      cmpr->intr_mask, s_ana_cmpr_default_intr_handler, cmpr, &cmpr->intr_handle), TAG, "allocate interrupt failed");
    }

    /* Save the callback group */
    memcpy(&(cmpr->cbs), cbs, sizeof(ana_cmpr_event_callbacks_t));
    cmpr->user_data = user_data;

    ESP_LOGD(TAG, "unit %d event callback registered", (int)cmpr->unit);

    return ESP_OK;
}

esp_err_t ana_cmpr_enable(ana_cmpr_handle_t cmpr)
{
    ANA_CMPR_NULL_POINTER_CHECK(cmpr);
    ESP_RETURN_ON_FALSE(!cmpr->is_enabled, ESP_ERR_INVALID_STATE, TAG,
                        "the analog comparator has enabled already");
    /* Update the driver status */
    cmpr->is_enabled = true;

    /* Acquire the pm lock if the unit has, to avoid the system start light sleep while Analog comparator still working */
    if (cmpr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(cmpr->pm_lock), TAG, "acquire pm_lock failed");
    }

    /* Enable the Analog Comparator */
    portENTER_CRITICAL(&s_spinlock);
    analog_cmpr_ll_enable_intr(cmpr->dev, cmpr->intr_mask, true);
    analog_cmpr_ll_enable(cmpr->dev, true);
    portEXIT_CRITICAL(&s_spinlock);

    ESP_LOGD(TAG, "unit %d enabled", (int)cmpr->unit);

    return ESP_OK;
}

esp_err_t ana_cmpr_disable(ana_cmpr_handle_t cmpr)
{
    ANA_CMPR_NULL_POINTER_CHECK(cmpr);
    ESP_RETURN_ON_FALSE(cmpr->is_enabled, ESP_ERR_INVALID_STATE, TAG,
                        "the analog comparator not enabled yet");
    /* Disable the Analog Comparator */
    portENTER_CRITICAL(&s_spinlock);
    analog_cmpr_ll_enable_intr(cmpr->dev, cmpr->intr_mask, false);
    analog_cmpr_ll_enable(cmpr->dev, false);
    portEXIT_CRITICAL(&s_spinlock);

    /* Release the pm lock, allow light sleep then */
    if (cmpr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(cmpr->pm_lock), TAG, "release pm_lock failed");
    }

    /* Update the driver status */
    cmpr->is_enabled = false;

    ESP_LOGD(TAG, "unit %d disabled", (int)cmpr->unit);
    return ESP_OK;
}

esp_err_t ana_cmpr_get_gpio(ana_cmpr_unit_t unit, ana_cmpr_channel_type_t chan_type, int *gpio_num)
{
    ANA_CMPR_NULL_POINTER_CHECK(gpio_num);
    ANA_CMPR_UNIT_CHECK(unit);

    /* Get the gpio number according to the channel type */
    switch (chan_type) {
    case ANA_CMPR_SOURCE_CHAN:
        *gpio_num = ana_cmpr_periph[unit].src_gpio;
        break;
    case ANA_CMPR_EXT_REF_CHAN:
        *gpio_num = ana_cmpr_periph[unit].ext_ref_gpio;
        break;
    default:
        ESP_LOGE(TAG, "invalid channel type");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

ana_cmpr_unit_t ana_cmpr_priv_get_unit_by_handle(ana_cmpr_handle_t cmpr)
{
    if (!cmpr) {
        return -1;
    }
    return cmpr->unit;
}
