/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "esp_clk_tree.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_pm.h"
#include "esp_memory_utils.h"
#include "driver/ana_cmpr.h"
#include "esp_private/gpio.h"
#include "esp_private/io_mux.h"
#include "esp_private/esp_clk.h"
#include "ana_cmpr_private.h"

struct ana_cmpr_t {
    ana_cmpr_unit_t             unit;               /*!< Analog comparator unit id */
    analog_cmpr_dev_t           *dev;               /*!< Analog comparator unit device address */
    ana_cmpr_ref_source_t       ref_src;            /*!< Analog comparator reference source, internal or external */
    _Atomic ana_cmpr_fsm_t      fsm;                /*!< The state machine of the Analog Comparator unit */
    ana_cmpr_event_callbacks_t  cbs;                /*!< The callback group that set by user */
    void                        *user_data;         /*!< User data that passed to the callbacks */
    intr_handle_t               intr_handle;        /*!< Interrupt handle */
    uint32_t                    intr_mask;          /*!< Interrupt mask */
    int                         intr_priority;      /*!< Interrupt priority */
    uint32_t                    src_clk_freq_hz;    /*!< Source clock frequency of the Analog Comparator unit */
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t        pm_lock;            /*!< The Power Management lock that used to avoid unexpected power down of the clock domain */
#endif
};

/* Helper macros */
#define ANA_CMPR_NULL_POINTER_CHECK(p)      ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '" #p "' is NULL")
#define ANA_CMPR_NULL_POINTER_CHECK_SAFE(p) \
do {                                        \
    if (unlikely(!(p))) {                   \
        ESP_EARLY_LOGE(TAG, "input parameter '" #p "' is NULL");  \
        return ESP_ERR_INVALID_ARG;         \
    }                                       \
} while(0)

#define ANA_CMPR_UNIT_CHECK(unit)           ESP_RETURN_ON_FALSE((unit) >= 0 && (unit) < SOC_ANA_CMPR_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid unit number")

/* Global static object of the Analog Comparator unit */
static ana_cmpr_handle_t s_ana_cmpr[SOC_ANA_CMPR_NUM] = {
    [0 ...(SOC_ANA_CMPR_NUM - 1)] = NULL,
};

/* Global spin lock */
static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;

void ana_cmpr_default_intr_handler(void *usr_data)
{
    bool need_yield = false;
    ana_cmpr_handle_t cmpr_handle = (ana_cmpr_handle_t)usr_data;
    ana_cmpr_cross_event_data_t evt_data = {.cross_type = ANA_CMPR_CROSS_ANY};
    /* Get and clear the interrupt status */
    uint32_t status = analog_cmpr_ll_get_intr_status(cmpr_handle->dev);
    analog_cmpr_ll_clear_intr(cmpr_handle->dev, status);

    /* Call the user callback function if it is specified and the corresponding event triggers*/
    if (cmpr_handle->cbs.on_cross && (status & cmpr_handle->intr_mask)) {
        // some chip can distinguish the edge of the cross event
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
    esp_err_t err = gpio_config_as_analog(ana_cmpr_periph[cmpr->unit].src_gpio);
    if (err == ESP_OK && is_external_ref) {
        err = gpio_config_as_analog(ana_cmpr_periph[cmpr->unit].ext_ref_gpio);
    }
    return err;
}

static void ana_cmpr_destroy_unit(ana_cmpr_handle_t cmpr)
{
#if CONFIG_PM_ENABLE
    if (cmpr->pm_lock) {
        esp_pm_lock_delete(cmpr->pm_lock);
    }
#endif
    if (cmpr->intr_handle) {
        esp_intr_free(cmpr->intr_handle);
    }
    free(cmpr);
}

esp_err_t ana_cmpr_new_unit(const ana_cmpr_config_t *config, ana_cmpr_handle_t *ret_cmpr)
{
    esp_err_t ret = ESP_OK;
    ana_cmpr_handle_t ana_cmpr_hdl = NULL;
    ANA_CMPR_NULL_POINTER_CHECK(config);
    ANA_CMPR_NULL_POINTER_CHECK(ret_cmpr);
    ana_cmpr_unit_t unit = config->unit;
    ANA_CMPR_UNIT_CHECK(unit);
    ESP_RETURN_ON_FALSE(!s_ana_cmpr[unit], ESP_ERR_INVALID_STATE, TAG, "unit has been allocated already");
    if (config->intr_priority) {
        ESP_RETURN_ON_FALSE(1 << (config->intr_priority) & ANA_CMPR_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG,
                            TAG, "invalid interrupt priority:%d", config->intr_priority);
    }

    // analog comparator unit must be allocated from internal memory because it contains atomic variable
    ana_cmpr_hdl = heap_caps_calloc(1, sizeof(struct ana_cmpr_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(ana_cmpr_hdl, ESP_ERR_NO_MEM, TAG, "no memory for analog comparator object");

    /* Assign analog comparator unit */
    ana_cmpr_hdl->dev = ANALOG_CMPR_LL_GET_HW(unit);
    ana_cmpr_hdl->unit = unit;
    ana_cmpr_hdl->intr_priority = config->intr_priority;
    atomic_init(&ana_cmpr_hdl->fsm, ANA_CMPR_FSM_INIT);

    ana_cmpr_clk_src_t clk_src = config->clk_src ? config->clk_src : ANA_CMPR_CLK_SRC_DEFAULT;
    // Analog comparator located in the IO MUX, but IO MUX clock might be shared with other submodules as well, check if there's conflict
    ESP_GOTO_ON_ERROR(io_mux_set_clock_source((soc_module_clk_t)clk_src), err, TAG, "clock source conflicts with other IOMUX consumers");
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &ana_cmpr_hdl->src_clk_freq_hz),
                      err, TAG, "get source clock frequency failed");

#if CONFIG_PM_ENABLE
    // Create PM lock, because the light sleep may disable the clock and power domain used by the analog comparator
    // TODO: IDF-12818
    ret  = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, ana_cmpr_periph[unit].module_name, &ana_cmpr_hdl->pm_lock);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "create NO_LIGHT_SLEEP lock failed");
#endif

    /* Configure the register */
    analog_cmpr_ll_set_ref_source(ana_cmpr_hdl->dev, config->ref_src);
    ana_cmpr_hdl->ref_src = config->ref_src;
#if !SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE
    // set which cross type can trigger the interrupt
    analog_cmpr_ll_set_intr_cross_type(ana_cmpr_hdl->dev, config->cross_type);
#endif  // SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE
    // record the interrupt mask, the interrupt will be lazy installed when register user callbacks
    // different cross type means different interrupt mask
    ana_cmpr_hdl->intr_mask = analog_cmpr_ll_get_intr_mask_by_type(ana_cmpr_hdl->dev, config->cross_type);

    // different unit share the same interrupt register, so using a spin lock to protect it
    portENTER_CRITICAL(&s_spinlock);
    // disable the interrupt by default, and clear pending status
    analog_cmpr_ll_enable_intr(ana_cmpr_hdl->dev, ANALOG_CMPR_LL_ALL_INTR_MASK(unit), false);
    analog_cmpr_ll_clear_intr(ana_cmpr_hdl->dev, ANALOG_CMPR_LL_ALL_INTR_MASK(unit));
    portEXIT_CRITICAL(&s_spinlock);

    // GPIO configuration
    ESP_GOTO_ON_ERROR(s_ana_cmpr_init_gpio(ana_cmpr_hdl, config->ref_src == ANA_CMPR_REF_SRC_EXTERNAL), err, TAG, "failed to initialize GPIO");
    if (config->ref_src == ANA_CMPR_REF_SRC_INTERNAL) {
        ESP_LOGD(TAG, "unit %d allocated, source signal: GPIO %d, reference signal: internal",
                 (int)unit, ana_cmpr_periph[unit].src_gpio);
    } else {
        ESP_LOGD(TAG, "unit %d allocated, source signal: GPIO %d, reference signal: GPIO %d",
                 (int)unit, ana_cmpr_periph[unit].src_gpio, ana_cmpr_periph[unit].ext_ref_gpio);
    }

    // register the analog comparator unit to the global object array
    s_ana_cmpr[unit] = ana_cmpr_hdl;
    *ret_cmpr = ana_cmpr_hdl;
    return ESP_OK;

err:
    if (ana_cmpr_hdl) {
        ana_cmpr_destroy_unit(ana_cmpr_hdl);
    }
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
    ESP_RETURN_ON_FALSE(unit != -1, ESP_ERR_INVALID_ARG, TAG, "unregistered unit handle");
    ESP_RETURN_ON_FALSE(atomic_load(&cmpr->fsm) == ANA_CMPR_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "not in init state");

    ana_cmpr_destroy_unit(cmpr);
    // unregister it from the global object array
    s_ana_cmpr[unit] = NULL;
    ESP_LOGD(TAG, "unit %d deleted", (int)unit);

    return ESP_OK;
}

esp_err_t ana_cmpr_set_internal_reference(ana_cmpr_handle_t cmpr, const ana_cmpr_internal_ref_config_t *ref_cfg)
{
    ANA_CMPR_NULL_POINTER_CHECK_SAFE(cmpr);
    ANA_CMPR_NULL_POINTER_CHECK_SAFE(ref_cfg);
    if (unlikely(cmpr->ref_src != ANA_CMPR_REF_SRC_INTERNAL)) {
        ESP_EARLY_LOGE(TAG, "the reference voltage does not come from internal");
        return ESP_ERR_INVALID_STATE;
    }

    // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
    portENTER_CRITICAL_SAFE(&s_spinlock);
    analog_cmpr_ll_set_internal_ref_voltage(cmpr->dev, ref_cfg->ref_volt);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    return ESP_OK;
}

esp_err_t ana_cmpr_set_debounce(ana_cmpr_handle_t cmpr, const ana_cmpr_debounce_config_t *dbc_cfg)
{
    ANA_CMPR_NULL_POINTER_CHECK_SAFE(cmpr);
    ANA_CMPR_NULL_POINTER_CHECK_SAFE(dbc_cfg);

    /* Transfer the time to clock cycles */
    uint32_t wait_cycle = dbc_cfg->wait_us * (cmpr->src_clk_freq_hz / 1000000);
    // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
    portENTER_CRITICAL_SAFE(&s_spinlock);
    analog_cmpr_ll_set_debounce_cycle(cmpr->dev, wait_cycle);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

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
    ANA_CMPR_NULL_POINTER_CHECK_SAFE(cmpr);
    if (unlikely(cross_type < ANA_CMPR_CROSS_DISABLE || cross_type > ANA_CMPR_CROSS_ANY)) {
        ESP_EARLY_LOGE(TAG, "invalid cross type");
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL_SAFE(&s_spinlock);
    analog_cmpr_ll_set_intr_cross_type(cmpr->dev, cross_type);
    cmpr->intr_mask = analog_cmpr_ll_get_intr_mask_by_type(cmpr->dev, cross_type);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    return ESP_OK;
#endif
}

esp_err_t ana_cmpr_register_event_callbacks(ana_cmpr_handle_t cmpr, const ana_cmpr_event_callbacks_t *cbs, void *user_data)
{
    ANA_CMPR_NULL_POINTER_CHECK(cmpr);
    ANA_CMPR_NULL_POINTER_CHECK(cbs);
    ESP_RETURN_ON_FALSE(atomic_load(&cmpr->fsm) == ANA_CMPR_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "not in init state");
#if CONFIG_ANA_CMPR_ISR_CACHE_SAFE
    if (cbs->on_cross) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_cross), ESP_ERR_INVALID_ARG, TAG, "on_cross is not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user_data is not in internal RAM");
    }
#endif

    if (!cmpr->intr_handle) {
        int intr_flags = ANA_CMPR_INTR_FLAG | ((cmpr->intr_priority > 0) ?  BIT(cmpr->intr_priority) : ESP_INTR_FLAG_LOWMED);
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(ana_cmpr_periph[cmpr->unit].intr_src, intr_flags, (uint32_t)analog_cmpr_ll_get_intr_status_reg(cmpr->dev),
                                                      cmpr->intr_mask, ana_cmpr_default_intr_handler, cmpr, &cmpr->intr_handle),
                            TAG, "allocate interrupt failed");
    }

    /* Save the callback functions */
    memcpy(&(cmpr->cbs), cbs, sizeof(ana_cmpr_event_callbacks_t));
    cmpr->user_data = user_data;

    ESP_LOGV(TAG, "unit %d event callback registered", (int)cmpr->unit);
    return ESP_OK;
}

esp_err_t ana_cmpr_enable(ana_cmpr_handle_t cmpr)
{
    ANA_CMPR_NULL_POINTER_CHECK(cmpr);
    ana_cmpr_fsm_t expected_fsm = ANA_CMPR_FSM_INIT;
    if (atomic_compare_exchange_strong(&cmpr->fsm, &expected_fsm, ANA_CMPR_FSM_WAIT)) {
#if CONFIG_PM_ENABLE
        if (cmpr->pm_lock) {
            esp_pm_lock_acquire(cmpr->pm_lock);
        }
#endif

        // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
        portENTER_CRITICAL(&s_spinlock);
        analog_cmpr_ll_enable_intr(cmpr->dev, cmpr->intr_mask, true);
        analog_cmpr_ll_enable(cmpr->dev, true);
        portEXIT_CRITICAL(&s_spinlock);

        // switch the state machine to enable state
        atomic_store(&cmpr->fsm, ANA_CMPR_FSM_ENABLE);
        ESP_LOGD(TAG, "unit %d enabled", (int)cmpr->unit);
    } else {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "not in init state");
    }

    return ESP_OK;
}

esp_err_t ana_cmpr_disable(ana_cmpr_handle_t cmpr)
{
    ANA_CMPR_NULL_POINTER_CHECK(cmpr);
    ana_cmpr_fsm_t expected_fsm = ANA_CMPR_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&cmpr->fsm, &expected_fsm, ANA_CMPR_FSM_WAIT)) {
        // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
        portENTER_CRITICAL(&s_spinlock);
        analog_cmpr_ll_enable_intr(cmpr->dev, cmpr->intr_mask, false);
        analog_cmpr_ll_enable(cmpr->dev, false);
        portEXIT_CRITICAL(&s_spinlock);

#if CONFIG_PM_ENABLE
        if (cmpr->pm_lock) {
            esp_pm_lock_release(cmpr->pm_lock);
        }
#endif

        // switch the state machine to init state
        atomic_store(&cmpr->fsm, ANA_CMPR_FSM_INIT);
        ESP_LOGD(TAG, "unit %d disabled", (int)cmpr->unit);
    } else {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "not enabled yet");
    }

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

ana_cmpr_unit_t ana_cmpr_get_unit_id(ana_cmpr_handle_t cmpr)
{
    if (!cmpr) {
        return -1;
    }
    return cmpr->unit;
}

#if CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void ana_cmpr_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
}
#endif
