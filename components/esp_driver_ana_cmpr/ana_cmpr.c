/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "esp_clk_tree.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_memory_utils.h"
#include "driver/ana_cmpr.h"
#include "esp_private/gpio.h"
#include "esp_private/io_mux.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_clk_tree_common.h"
#include "ana_cmpr_private.h"

/* Global static object of the Analog Comparator unit */
static ana_cmpr_handle_t s_ana_cmpr[ANALOG_CMPR_LL_GET(INST_NUM)] = {
    [0 ...(ANALOG_CMPR_LL_GET(INST_NUM) - 1)] = NULL,
};

/* Global slot state for unit create/delete lifecycle */
static ana_cmpr_unit_slot_state_t s_unit_slot_state[ANALOG_CMPR_LL_GET(INST_NUM)] = {
    [0 ...(ANALOG_CMPR_LL_GET(INST_NUM) - 1)] = ANA_CMPR_UNIT_SLOT_FREE,
};

/* Global spin lock */
portMUX_TYPE s_ana_cmpr_spinlock = portMUX_INITIALIZER_UNLOCKED;

static esp_err_t ana_cmpr_reserve_unit_slot(int unit_id)
{
    esp_err_t ret = ESP_OK;

    ANA_CMPR_CRITICAL_SECTION() {
        if (s_unit_slot_state[unit_id] != ANA_CMPR_UNIT_SLOT_FREE) {
            ret = ESP_ERR_INVALID_STATE;
        } else {
            s_unit_slot_state[unit_id] = ANA_CMPR_UNIT_SLOT_ALLOCATING;
        }
    }

    return ret;
}

static void ana_cmpr_release_unit_slot(int unit_id)
{
    ANA_CMPR_CRITICAL_SECTION() {
        if (s_unit_slot_state[unit_id] == ANA_CMPR_UNIT_SLOT_ALLOCATING || s_unit_slot_state[unit_id] == ANA_CMPR_UNIT_SLOT_DELETING) {
            s_unit_slot_state[unit_id] = ANA_CMPR_UNIT_SLOT_FREE;
        }
    }
}

static esp_err_t ana_cmpr_publish_unit_slot(int unit_id, ana_cmpr_handle_t cmpr)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    ANA_CMPR_CRITICAL_SECTION() {
        if (s_unit_slot_state[unit_id] == ANA_CMPR_UNIT_SLOT_ALLOCATING && s_ana_cmpr[unit_id] == NULL) {
            s_ana_cmpr[unit_id] = cmpr;
            s_unit_slot_state[unit_id] = ANA_CMPR_UNIT_SLOT_READY;
            ret = ESP_OK;
        }
    }
    return ret;
}

static esp_err_t ana_cmpr_unpublish_unit_slot_for_delete(ana_cmpr_handle_t cmpr, int *ret_unit_id)
{
    esp_err_t ret = ESP_ERR_INVALID_ARG;
    int unit_id = cmpr->unit_id;
    ANA_CMPR_CRITICAL_SECTION() {
        if (unit_id >= 0 && unit_id < ANALOG_CMPR_LL_GET(INST_NUM) &&
                s_ana_cmpr[unit_id] == cmpr &&
                s_unit_slot_state[unit_id] == ANA_CMPR_UNIT_SLOT_READY) {
            s_ana_cmpr[unit_id] = NULL;
            s_unit_slot_state[unit_id] = ANA_CMPR_UNIT_SLOT_DELETING;
            ret = ESP_OK;
        }
    }
    if (ret == ESP_OK) {
        *ret_unit_id = unit_id;
    }
    return ret;
}

void ana_cmpr_default_intr_handler(void *usr_data)
{
    bool need_yield = false;
    ana_cmpr_handle_t cmpr_handle = (ana_cmpr_handle_t)usr_data;
    ana_cmpr_cross_event_data_t evt_data = {.cross_type = ANA_CMPR_CROSS_ANY};
    /* Get and clear the interrupt status */
    uint32_t status = analog_cmpr_ll_get_intr_status(cmpr_handle->dev);
    analog_cmpr_ll_clear_intr(cmpr_handle->dev, status);

    /* Call the user callback function if it is specified and the corresponding event triggers*/
    ana_cmpr_cross_cb_t on_cross = cmpr_handle->cbs.on_cross;
    if (on_cross) {
        // some chip can distinguish the edge of the cross event
#if ANALOG_CMPR_LL_SUPPORT(EDGE_SPECIFIC_INTR_MASK)
        for (int i = 0; i < ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM); i++) {
            if (!cmpr_handle->src_chans[i].configured) {
                continue;
            }
            evt_data.src_chan_id = i;
            if (status & ANALOG_CMPR_LL_POS_CROSS_INTR_MASK(cmpr_handle->unit_id, i)) {
                evt_data.cross_type = ANA_CMPR_CROSS_POS;
                need_yield |= on_cross(cmpr_handle, &evt_data, cmpr_handle->user_data);
            }
            if (status & ANALOG_CMPR_LL_NEG_CROSS_INTR_MASK(cmpr_handle->unit_id, i)) {
                evt_data.cross_type = ANA_CMPR_CROSS_NEG;
                need_yield |= on_cross(cmpr_handle, &evt_data, cmpr_handle->user_data);
            }
        }
#else
        need_yield = on_cross(cmpr_handle, &evt_data, cmpr_handle->user_data);
#endif
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static void ana_cmpr_destroy_unit(ana_cmpr_handle_t cmpr)
{
    int unit_id = cmpr->unit_id;

#if CONFIG_PM_ENABLE
    if (cmpr->pm_lock) {
        esp_pm_lock_delete(cmpr->pm_lock);
    }
#endif
    if (cmpr->intr_handle) {
        esp_intr_free(cmpr->intr_handle);
    }
    // Disable function clock first
    analog_cmpr_ll_enable_function_clock(unit_id, false);
    // Disable bus clock last
    analog_cmpr_ll_enable_bus_clock(unit_id, false);
    // Disable the clock source if it is enabled by this driver
#if ANALOG_CMPR_LL_GET(IP_VERSION) <= 1
    if (cmpr->io_mux_acquired) {
        io_mux_release_clock_source(cmpr->clk_src);
        cmpr->io_mux_acquired = false;
    }
#endif
    if (cmpr->clk_src != SOC_MOD_CLK_INVALID) {
        esp_clk_tree_enable_src(cmpr->clk_src, false);
    }

    free(cmpr);
}

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
static int _ana_cmpr_gpio_to_pad_id(ana_cmpr_handle_t cmpr, int gpio_num)
{
    int pad_id = -1;
    for (int i = 0; i < ANALOG_CMPR_LL_GET(PAD_NUM); i++) {
        if (ana_cmpr_periph[cmpr->unit_id].pad_gpios[i] == gpio_num) {
            pad_id = i;
            break;
        }
    }
    return pad_id;
}
#endif

static uint32_t _ana_cmpr_build_scan_mask(ana_cmpr_handle_t cmpr)
{
    uint32_t scan_mask = 0;
    for (int i = 0; i < ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM); i++) {
        if (cmpr->src_chans[i].configured) {
            scan_mask |= BIT(i);
        }
    }
    return scan_mask;
}

static uint32_t _ana_cmpr_build_intr_mask(ana_cmpr_handle_t cmpr)
{
    uint32_t intr_mask = 0;
    for (int i = 0; i < ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM); i++) {
        if (cmpr->src_chans[i].configured) {
            intr_mask |= analog_cmpr_ll_get_intr_mask_by_type(cmpr->unit_id, i, cmpr->src_chans[i].cross_type);
        }
    }
    return intr_mask;
}

static void _ana_cmpr_refresh_masks(ana_cmpr_handle_t cmpr)
{
    cmpr->intr_mask = _ana_cmpr_build_intr_mask(cmpr);

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    uint32_t scan_mask = _ana_cmpr_build_scan_mask(cmpr);
    analog_cmpr_ll_set_scan_mask(cmpr->dev, scan_mask);
#endif
}

static esp_err_t _ana_cmpr_check_pad_conflict(ana_cmpr_handle_t cmpr, int src_chan_id, int pad_id)
{
    if (cmpr->ref_chan.ref_src == ANA_CMPR_REF_SRC_EXTERNAL && cmpr->ref_chan.pad_id == pad_id) {
        ESP_LOGE(TAG, "source channel %d conflicts with external reference PAD %d", src_chan_id, pad_id);
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM); i++) {
        if (i == src_chan_id) {
            continue;
        }
        if (cmpr->src_chans[i].configured && cmpr->src_chans[i].pad_id == pad_id) {
            ESP_LOGE(TAG, "source channel %d conflicts with source channel %d on PAD %d", src_chan_id, i, pad_id);
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}

static esp_err_t _ana_cmpr_init_default_channels(ana_cmpr_handle_t cmpr, const ana_cmpr_config_t *config)
{
    int unit_id = cmpr->unit_id;

    // all channels are disabled by default
    for (int i = 0; i < ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM); i++) {
        cmpr->src_chans[i].chan_id = i;
        cmpr->src_chans[i].cross_type = ANA_CMPR_CROSS_DISABLE;
        cmpr->src_chans[i].gpio_num = -1;
        cmpr->src_chans[i].pad_id = -1;
        cmpr->src_chans[i].configured = false;
    }

    // init source channel 0 according to the config
    cmpr->src_chans[0].cross_type = config->cross_type;
    cmpr->src_chans[0].configured = true;

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    // GPIO number of source channel is configurable
    cmpr->src_chans[0].gpio_num = config->src_chan0_gpio;
    cmpr->src_chans[0].pad_id = _ana_cmpr_gpio_to_pad_id(cmpr, cmpr->src_chans[0].gpio_num);
    ESP_RETURN_ON_FALSE(cmpr->src_chans[0].pad_id != -1, ESP_ERR_INVALID_ARG, TAG, "invalid source channel GPIO: %d", cmpr->src_chans[0].gpio_num);
#else
    cmpr->src_chans[0].gpio_num = ana_cmpr_periph[unit_id].src_gpio;
    cmpr->src_chans[0].pad_id = 0;
#endif

    cmpr->ref_chan.ref_src = config->ref_src;
    if (config->ref_src == ANA_CMPR_REF_SRC_EXTERNAL) {
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
        // GPIO number of external reference channel is configurable
        cmpr->ref_chan.gpio_num = config->ext_ref_gpio;
        cmpr->ref_chan.pad_id = _ana_cmpr_gpio_to_pad_id(cmpr, cmpr->ref_chan.gpio_num);
        ESP_RETURN_ON_FALSE(cmpr->ref_chan.pad_id != -1, ESP_ERR_INVALID_ARG, TAG, "invalid external reference GPIO: %d", cmpr->ref_chan.gpio_num);
        ESP_RETURN_ON_FALSE(cmpr->ref_chan.pad_id != cmpr->src_chans[0].pad_id, ESP_ERR_INVALID_ARG, TAG,
                            "source channel 0 PAD conflicts with external reference PAD");
#else
        cmpr->ref_chan.gpio_num = ana_cmpr_periph[unit_id].ext_ref_gpio;
        cmpr->ref_chan.pad_id = 1;
#endif
    }

    analog_cmpr_ll_set_ref_source(cmpr->dev, config->ref_src);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    analog_cmpr_ll_set_src_pad(cmpr->dev, 0, cmpr->src_chans[0].pad_id);
    if (config->ref_src == ANA_CMPR_REF_SRC_EXTERNAL) {
        analog_cmpr_ll_set_ext_ref_pad(cmpr->dev, cmpr->ref_chan.pad_id);
    }
#endif

    // setup the gpio pad for the source and reference signal after validation has passed
    gpio_config_as_analog(cmpr->src_chans[0].gpio_num);
    if (config->ref_src == ANA_CMPR_REF_SRC_EXTERNAL) {
        gpio_config_as_analog(cmpr->ref_chan.gpio_num);
        ESP_LOGD(TAG, "unit %d: source0 signal from GPIO %d, reference signal from GPIO %d",
                 unit_id, cmpr->src_chans[0].gpio_num, cmpr->ref_chan.gpio_num);
    } else {
        ESP_LOGD(TAG, "unit %d: source0 signal from GPIO %d, reference signal from internal",
                 unit_id, cmpr->src_chans[0].gpio_num);
    }

#if !ANALOG_CMPR_LL_SUPPORT(EDGE_SPECIFIC_INTR_MASK)
    // set which cross type can trigger the interrupt
    analog_cmpr_ll_set_intr_cross_type(cmpr->dev, config->cross_type);
#endif  // !ANALOG_CMPR_LL_SUPPORT(EDGE_SPECIFIC_INTR_MASK)
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    analog_cmpr_ll_set_resample_limit(cmpr->dev, config->resample_limit);
    analog_cmpr_ll_enable_capture_timer(cmpr->dev, config->en_capture_timer);
#endif
    _ana_cmpr_refresh_masks(cmpr);

    return ESP_OK;
}

esp_err_t ana_cmpr_new_unit(const ana_cmpr_config_t *config, ana_cmpr_handle_t *ret_cmpr)
{
    if (config == NULL || ret_cmpr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    ana_cmpr_handle_t ana_cmpr_hdl = NULL;
    int unit_id = config->unit;
    bool slot_reserved = false;
    ESP_RETURN_ON_FALSE(unit_id >= 0 && unit_id < ANALOG_CMPR_LL_GET(INST_NUM), ESP_ERR_INVALID_ARG, TAG, "invalid unit id");
    ESP_RETURN_ON_FALSE((config->intr_priority >= 0) && (config->intr_priority < 32),
                        ESP_ERR_INVALID_ARG, TAG, "invalid interrupt priority:%d", config->intr_priority);
    if (config->intr_priority > 0) {
        ESP_RETURN_ON_FALSE((1U << config->intr_priority) & ANA_CMPR_ALLOW_INTR_PRIORITY_MASK,
                            ESP_ERR_INVALID_ARG, TAG, "invalid interrupt priority:%d", config->intr_priority);
    }
    ESP_RETURN_ON_ERROR(ana_cmpr_reserve_unit_slot(unit_id), TAG, "unit has been allocated already");
    slot_reserved = true;

    // analog comparator unit must be allocated from internal memory because it contains atomic variable
    ana_cmpr_hdl = heap_caps_calloc(1, sizeof(struct ana_cmpr_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(ana_cmpr_hdl, ESP_ERR_NO_MEM, err, TAG, "no memory for analog comparator object");
    ana_cmpr_hdl->clk_src = SOC_MOD_CLK_INVALID;
    ana_cmpr_hdl->io_mux_acquired = false;

    /* Assign analog comparator unit */
    ana_cmpr_hdl->dev = ANALOG_CMPR_LL_GET_HW(unit_id);
    ana_cmpr_hdl->unit_id = unit_id;
    ana_cmpr_hdl->intr_priority = config->intr_priority;
    atomic_init(&ana_cmpr_hdl->fsm, ANA_CMPR_FSM_INIT);

    // Enable bus clock
    analog_cmpr_ll_enable_bus_clock(unit_id, true);
    // Reset register
    analog_cmpr_ll_reset_register(unit_id);
    // Reset core
    analog_cmpr_ll_reset_core(unit_id);

    // Set clock source (use default if not specified in config)
    soc_module_clk_t clk_src = config->clk_src ? config->clk_src : ANA_CMPR_CLK_SRC_DEFAULT;
    ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src(clk_src, true), err, TAG, "enable clock source failed");
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    analog_cmpr_ll_set_clk_src(unit_id, clk_src);
    // Set clock divider to 1
    analog_cmpr_ll_set_clk_div(unit_id, 1);
    // Enable function clock
    analog_cmpr_ll_enable_function_clock(unit_id, true);
    ana_cmpr_hdl->clk_src = clk_src;
#else
    // Analog comparator located in the IO MUX module in older chips, so the clock source is shared with IO MUX.
    ret = io_mux_acquire_clock_source(clk_src);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "clock source conflicts with other IOMUX consumers");
        esp_clk_tree_enable_src(clk_src, false);
        goto err;
    }
    ana_cmpr_hdl->io_mux_acquired = true;
    ana_cmpr_hdl->clk_src = clk_src;
#endif
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz(clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &ana_cmpr_hdl->src_clk_freq_hz),
                      err, TAG, "get source clock frequency failed");

    // init the default source and reference channels according to the config
    ESP_GOTO_ON_ERROR(_ana_cmpr_init_default_channels(ana_cmpr_hdl, config), err, TAG, "init default channels failed");

#if CONFIG_PM_ENABLE
    // Create PM lock, because the light sleep may disable the clock and power domain used by the analog comparator
    // TODO: IDF-12818
    ret  = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, ana_cmpr_periph[unit_id].module_name, &ana_cmpr_hdl->pm_lock);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "create NO_LIGHT_SLEEP lock failed");
#endif

    // different unit share the same interrupt register, so using a spin lock to protect it
    ANA_CMPR_CRITICAL_SECTION() {
        // disable the interrupt by default, and clear all pending status
        analog_cmpr_ll_enable_intr(ana_cmpr_hdl->dev, ANALOG_CMPR_LL_ALL_INTR_MASK(unit_id), false);
        analog_cmpr_ll_clear_intr(ana_cmpr_hdl->dev, ANALOG_CMPR_LL_ALL_INTR_MASK(unit_id));
    }

    // register the analog comparator unit to the global object array
    ESP_GOTO_ON_ERROR(ana_cmpr_publish_unit_slot(unit_id, ana_cmpr_hdl), err, TAG, "publish unit slot failed");
    *ret_cmpr = ana_cmpr_hdl;
    return ESP_OK;

err:
    if (ana_cmpr_hdl) {
        ana_cmpr_destroy_unit(ana_cmpr_hdl);
    }
    if (slot_reserved) {
        ana_cmpr_release_unit_slot(unit_id);
    }
    return ret;
}

esp_err_t ana_cmpr_del_unit(ana_cmpr_handle_t cmpr)
{
    esp_err_t ret = ESP_OK;
    bool state_acquired = false;
    ESP_RETURN_ON_FALSE(cmpr, ESP_ERR_INVALID_ARG, TAG, "null pointer");
    /* Search the global object array to check if the input handle is valid */
    int unit_id = -1;
    ANA_CMPR_WITH_TEMP_STATE(cmpr, ANA_CMPR_FSM_INIT, state_acquired) {
        ret = ana_cmpr_unpublish_unit_slot_for_delete(cmpr, &unit_id);
    }
    ESP_RETURN_ON_FALSE(state_acquired, ESP_ERR_INVALID_STATE, TAG, "not in init state");
    ESP_RETURN_ON_ERROR(ret, TAG, "unregistered unit handle");

    ana_cmpr_destroy_unit(cmpr);
    ana_cmpr_release_unit_slot(unit_id);
    ESP_LOGD(TAG, "unit %d deleted", (int)unit_id);

    return ESP_OK;
}

esp_err_t ana_cmpr_set_internal_reference(ana_cmpr_handle_t cmpr, const ana_cmpr_internal_ref_config_t *ref_cfg)
{
    if (cmpr == NULL || ref_cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    // external reference channel doesn't support it
    if (cmpr->ref_chan.ref_src != ANA_CMPR_REF_SRC_INTERNAL) {
        return ESP_ERR_NOT_ALLOWED;
    }

    // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
    ANA_CMPR_CRITICAL_SECTION() {
        analog_cmpr_ll_set_internal_ref_voltage(cmpr->dev, ref_cfg->ref_volt);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
        analog_cmpr_ll_set_ref_hys_level(cmpr->dev, ref_cfg->ref_hys_level);
#endif
    }

    return ESP_OK;
}

esp_err_t ana_cmpr_set_debounce(ana_cmpr_handle_t cmpr, const ana_cmpr_debounce_config_t *dbc_cfg)
{
    if (cmpr == NULL || dbc_cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Transfer the time to clock cycles */
    uint32_t wait_cycle = (uint64_t)dbc_cfg->wait_us * cmpr->src_clk_freq_hz / 1000000;
    // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
    ANA_CMPR_CRITICAL_SECTION() {
        analog_cmpr_ll_set_cross_debounce_cycle(cmpr->dev, wait_cycle);
    }

    return ESP_OK;
}

esp_err_t ana_cmpr_add_src_chan(ana_cmpr_handle_t cmpr, int src_chan_id, const ana_cmpr_src_chan_config_t *src_cfg)
{
    esp_err_t ret = ESP_OK;
    bool state_acquired = false;
    ESP_RETURN_ON_FALSE(cmpr && src_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(_ana_cmpr_is_src_chan_id_valid(src_chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid source channel id %d", src_chan_id);
    ESP_RETURN_ON_FALSE(src_cfg->cross_type >= ANA_CMPR_CROSS_DISABLE && src_cfg->cross_type <= ANA_CMPR_CROSS_ANY,
                        ESP_ERR_INVALID_ARG, TAG, "invalid cross type %d", src_cfg->cross_type);

    int gpio_num = src_cfg->gpio_num;
    int pad_id = -1;
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    // This check depends only on static pad capabilities, not current unit topology.
    pad_id = _ana_cmpr_gpio_to_pad_id(cmpr, gpio_num);
    ESP_RETURN_ON_FALSE(pad_id != -1, ESP_ERR_INVALID_ARG, TAG, "invalid source channel GPIO: %d", gpio_num);
#else
    gpio_num = ana_cmpr_periph[cmpr->unit_id].src_gpio;
    pad_id = 0;
    ESP_RETURN_ON_FALSE(src_cfg->gpio_num == gpio_num, ESP_ERR_INVALID_ARG, TAG, "source channel 0 GPIO is fixed to %d", gpio_num);
#endif

    ANA_CMPR_WITH_TEMP_STATE(cmpr, ANA_CMPR_FSM_INIT, state_acquired) {
        // Topology-dependent validation must stay inside the claimed state window.
        ret = _ana_cmpr_check_pad_conflict(cmpr, src_chan_id, pad_id);
        if (ret == ESP_OK) {
            ANA_CMPR_CRITICAL_SECTION() {
                cmpr->src_chans[src_chan_id].chan_id = src_chan_id;
                cmpr->src_chans[src_chan_id].gpio_num = gpio_num;
                cmpr->src_chans[src_chan_id].pad_id = pad_id;
                cmpr->src_chans[src_chan_id].cross_type = src_cfg->cross_type;
                cmpr->src_chans[src_chan_id].configured = true;
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
                analog_cmpr_ll_set_src_pad(cmpr->dev, src_chan_id, pad_id);
#endif
                _ana_cmpr_refresh_masks(cmpr);
            }
        }
    }
    ESP_RETURN_ON_FALSE(state_acquired, ESP_ERR_INVALID_STATE, TAG, "not in init state");
    ESP_RETURN_ON_ERROR(ret, TAG, "add source channel failed");

    gpio_config_as_analog(gpio_num);
    return ESP_OK;
}

esp_err_t ana_cmpr_remove_src_chan(ana_cmpr_handle_t cmpr, int src_chan_id)
{
    bool state_acquired = false;
    ESP_RETURN_ON_FALSE(cmpr, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(_ana_cmpr_is_src_chan_id_valid(src_chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid source channel id %d", src_chan_id);
    ANA_CMPR_WITH_TEMP_STATE(cmpr, ANA_CMPR_FSM_INIT, state_acquired) {
        ANA_CMPR_CRITICAL_SECTION() {
            cmpr->src_chans[src_chan_id].configured = false;
            cmpr->src_chans[src_chan_id].cross_type = ANA_CMPR_CROSS_DISABLE;
            cmpr->src_chans[src_chan_id].gpio_num = -1;
            cmpr->src_chans[src_chan_id].pad_id = -1;
            _ana_cmpr_refresh_masks(cmpr);
        }
    }
    ESP_RETURN_ON_FALSE(state_acquired, ESP_ERR_INVALID_STATE, TAG, "not in init state");
    return ESP_OK;
}

esp_err_t ana_cmpr_set_src_chan_cross_type(ana_cmpr_handle_t cmpr, int src_chan_id, ana_cmpr_cross_type_t cross_type)
{
#if ANALOG_CMPR_LL_SUPPORT(EDGE_SPECIFIC_INTR_MASK)
    // On edge-type capable hardware, each channel's interrupt source (thus unit intr_mask composition)
    // is fixed when channels are configured. Runtime cross-type switching would require rebuilding and
    // re-binding interrupt sources, which this API intentionally does not do.
    (void)cmpr;
    (void)src_chan_id;
    (void)cross_type;
    return ESP_ERR_NOT_SUPPORTED;
#else
    esp_err_t ret = ESP_OK;
    if (cmpr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cross_type < ANA_CMPR_CROSS_DISABLE || cross_type > ANA_CMPR_CROSS_ANY) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!_ana_cmpr_is_src_chan_id_valid(src_chan_id)) {
        return ESP_ERR_INVALID_ARG;
    }
    ANA_CMPR_CRITICAL_SECTION() {
        if (!cmpr->src_chans[src_chan_id].configured) {
            ret = ESP_ERR_INVALID_STATE;
        } else {
            cmpr->src_chans[src_chan_id].cross_type = cross_type;
            analog_cmpr_ll_set_intr_cross_type(cmpr->dev, cross_type);
            // Keep software mask mirror synchronized even on targets where mask mapping is currently fixed.
            _ana_cmpr_refresh_masks(cmpr);
        }
    }
    return ret;
#endif
}

esp_err_t ana_cmpr_set_scan_config(ana_cmpr_handle_t cmpr, const ana_cmpr_scan_config_t *scan_cfg)
{
    ESP_RETURN_ON_FALSE(cmpr && scan_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    bool state_acquired = false;
    ESP_RETURN_ON_FALSE(scan_cfg->scan_mode == ANA_CMPR_SCAN_MODE_FULL || scan_cfg->scan_mode == ANA_CMPR_SCAN_MODE_STEP,
                        ESP_ERR_INVALID_ARG, TAG, "invalid scan mode %d", scan_cfg->scan_mode);
    uint64_t poll_period_cycle = (uint64_t)scan_cfg->poll_period_us * cmpr->src_clk_freq_hz / 1000000;
    ESP_RETURN_ON_FALSE(poll_period_cycle <= ANALOG_CMPR_LL_MAX_POLL_PERIOD_CYCLES, ESP_ERR_INVALID_ARG, TAG, "poll period out of range: %"PRIu64" cycles", poll_period_cycle);
    ANA_CMPR_WITH_TEMP_STATE(cmpr, ANA_CMPR_FSM_INIT, state_acquired) {
        ANA_CMPR_CRITICAL_SECTION() {
            analog_cmpr_ll_set_scan_mode(cmpr->dev, scan_cfg->scan_mode);
            analog_cmpr_ll_set_poll_period(cmpr->dev, (uint32_t)poll_period_cycle);
        }
    }
    ESP_RETURN_ON_FALSE(state_acquired, ESP_ERR_INVALID_STATE, TAG, "not in init state");
    return ESP_OK;
#else
    (void)scan_cfg;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t ana_cmpr_trigger_scan(ana_cmpr_handle_t cmpr)
{
    if (!cmpr) {
        return ESP_ERR_INVALID_ARG;
    }
#if ANALOG_CMPR_LL_SUPPORT(SW_SCAN)
    bool state_acquired = false;
    ANA_CMPR_WITH_TEMP_STATE(cmpr, ANA_CMPR_FSM_ENABLE, state_acquired) {
        analog_cmpr_ll_start_scan(cmpr->dev);
    }
    if (!state_acquired) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t ana_cmpr_get_output_level(ana_cmpr_handle_t cmpr, int src_chan_id, bool *out_level)
{
    ESP_RETURN_ON_FALSE(cmpr && out_level, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    ESP_RETURN_ON_FALSE(_ana_cmpr_is_src_chan_id_valid(src_chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid source channel id %d", src_chan_id);

    *out_level = analog_cmpr_ll_get_compare_result(cmpr->dev, src_chan_id);
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t ana_cmpr_get_capture_timestamps(ana_cmpr_handle_t cmpr, int src_chan_id, uint32_t *current, uint32_t *previous)
{
    ESP_RETURN_ON_FALSE(cmpr, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(current || previous, ESP_ERR_INVALID_ARG, TAG, "both current and previous are NULL");
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    ESP_RETURN_ON_FALSE(_ana_cmpr_is_src_chan_id_valid(src_chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid source channel id %d", src_chan_id);

    if (current) {
        *current = analog_cmpr_ll_get_current_capture_time(cmpr->dev, src_chan_id);
    }
    if (previous) {
        *previous = analog_cmpr_ll_get_previous_capture_time(cmpr->dev, src_chan_id);
    }
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t ana_cmpr_get_clock_resolution_hz(ana_cmpr_handle_t cmpr, uint32_t *resolution_hz)
{
    ESP_RETURN_ON_FALSE(cmpr && resolution_hz, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *resolution_hz = cmpr->src_clk_freq_hz;
    return ESP_OK;
}

esp_err_t ana_cmpr_register_event_callbacks(ana_cmpr_handle_t cmpr, const ana_cmpr_event_callbacks_t *cbs, void *user_data)
{
    esp_err_t ret = ESP_OK;
    bool state_acquired = false;
    if (cmpr == NULL || cbs == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
#if CONFIG_ANA_CMPR_ISR_CACHE_SAFE
    if (cbs->on_cross) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_cross), ESP_ERR_INVALID_ARG, TAG, "on_cross is not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user_data is not in internal RAM");
    }
#endif
    ANA_CMPR_WITH_TEMP_STATE(cmpr, ANA_CMPR_FSM_INIT, state_acquired) {
        // the interrupt service is lazy installed.
        if (!cmpr->intr_handle) {
            int intr_flags = ANA_CMPR_INTR_FLAG | ((cmpr->intr_priority > 0) ?  BIT(cmpr->intr_priority) : ESP_INTR_FLAG_LOWMED);
            ret = esp_intr_alloc_intrstatus(ana_cmpr_periph[cmpr->unit_id].intr_src, intr_flags,
                                            (uint32_t)analog_cmpr_ll_get_intr_status_reg(cmpr->dev),
                                            ANALOG_CMPR_LL_ALL_INTR_MASK(cmpr->unit_id), ana_cmpr_default_intr_handler,
                                            cmpr, &cmpr->intr_handle);
        }
        if (ret == ESP_OK) {
            /* Save the callback functions */
            memcpy(&(cmpr->cbs), cbs, sizeof(ana_cmpr_event_callbacks_t));
            cmpr->user_data = user_data;
            ESP_LOGV(TAG, "unit %d event callback registered", cmpr->unit_id);
        }
    }
    ESP_RETURN_ON_FALSE(state_acquired, ESP_ERR_INVALID_STATE, TAG, "not in init state");
    ESP_RETURN_ON_ERROR(ret, TAG, "allocate interrupt failed");
    return ESP_OK;
}

esp_err_t ana_cmpr_enable(ana_cmpr_handle_t cmpr)
{
    esp_err_t ret = ESP_OK;
    bool state_acquired = false;
    ana_cmpr_fsm_t final_state = ANA_CMPR_FSM_INIT;
    if (cmpr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ANA_CMPR_WITH_STATE_TRANSITION(cmpr, ANA_CMPR_FSM_INIT, final_state, state_acquired) {
        // Check after claiming INIT->WAIT, so source-channel updates can't race this precondition.
        if (_ana_cmpr_build_scan_mask(cmpr) == 0) {
            ret = ESP_ERR_INVALID_STATE;
            ESP_LOGE(TAG, "no source channel configured");
        }
        if (ret == ESP_OK) {
#if CONFIG_PM_ENABLE
            if (cmpr->pm_lock) {
                esp_pm_lock_acquire(cmpr->pm_lock);
            }
#endif

            // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
            ANA_CMPR_CRITICAL_SECTION() {
                analog_cmpr_ll_enable_intr(cmpr->dev, cmpr->intr_mask, true);
                analog_cmpr_ll_enable(cmpr->dev, true);
            }

            final_state = ANA_CMPR_FSM_ENABLE;
            ESP_LOGD(TAG, "unit %d enabled", (int)cmpr->unit_id);
        }
    }
    ESP_RETURN_ON_FALSE(state_acquired, ESP_ERR_INVALID_STATE, TAG, "not in init state");
    ESP_RETURN_ON_ERROR(ret, TAG, "enable unit failed");
    return ret;
}

esp_err_t ana_cmpr_disable(ana_cmpr_handle_t cmpr)
{
    bool state_acquired = false;
    ana_cmpr_fsm_t final_state = ANA_CMPR_FSM_ENABLE;
    if (cmpr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ANA_CMPR_WITH_STATE_TRANSITION(cmpr, ANA_CMPR_FSM_ENABLE, final_state, state_acquired) {
        // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
        ANA_CMPR_CRITICAL_SECTION() {
            analog_cmpr_ll_enable_intr(cmpr->dev, cmpr->intr_mask, false);
            analog_cmpr_ll_enable(cmpr->dev, false);
        }

#if CONFIG_PM_ENABLE
        if (cmpr->pm_lock) {
            esp_pm_lock_release(cmpr->pm_lock);
        }
#endif

        final_state = ANA_CMPR_FSM_INIT;
        ESP_LOGD(TAG, "unit %d disabled", (int)cmpr->unit_id);
    }
    ESP_RETURN_ON_FALSE(state_acquired, ESP_ERR_INVALID_STATE, TAG, "not enabled yet");
    return ESP_OK;
}

esp_err_t ana_cmpr_get_channel_gpio(ana_cmpr_handle_t cmpr, ana_cmpr_channel_type_t chan_type, int chan_id, gpio_num_t *gpio_num)
{
    ESP_RETURN_ON_FALSE(cmpr && gpio_num, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    switch (chan_type) {
    case ANA_CMPR_SOURCE_CHAN:
        ESP_RETURN_ON_FALSE(_ana_cmpr_is_src_chan_id_valid(chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid source channel id %d", chan_id);
        ESP_RETURN_ON_FALSE(cmpr->src_chans[chan_id].configured, ESP_ERR_INVALID_STATE, TAG, "source channel %d not configured", chan_id);
        *gpio_num = cmpr->src_chans[chan_id].gpio_num;
        break;
    case ANA_CMPR_EXT_REF_CHAN:
        ESP_RETURN_ON_FALSE(chan_id == 0, ESP_ERR_INVALID_ARG, TAG, "reference channel id must be 0");
        ESP_RETURN_ON_FALSE(cmpr->ref_chan.ref_src == ANA_CMPR_REF_SRC_EXTERNAL, ESP_ERR_NOT_FOUND, TAG, "reference channel uses internal source");
        *gpio_num = cmpr->ref_chan.gpio_num;
        break;
    default:
        ESP_LOGE(TAG, "invalid channel type");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

#if CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void ana_cmpr_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// Deprecated APIs for backward compatibility, will be removed in ESP-IDF v7.0 ///////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

esp_err_t ana_cmpr_get_gpio(ana_cmpr_unit_t unit, ana_cmpr_channel_type_t chan_type, int *gpio_num)
{
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    (void)unit;
    (void)chan_type;
    (void)gpio_num;
    // the source channel and reference channel GPIO number are configurable in the new analog comparator IP.
    // we can't get the GPIO number from a fixed mapping like in the old version driver
    return ESP_ERR_NOT_SUPPORTED;
#else
    if (gpio_num == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_RETURN_ON_FALSE(unit >= 0 && unit < ANALOG_CMPR_LL_GET(INST_NUM), ESP_ERR_INVALID_ARG, TAG, "invalid unit id");

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
#endif
}
