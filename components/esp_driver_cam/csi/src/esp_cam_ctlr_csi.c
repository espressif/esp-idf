/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "esp_memory_utils.h"
#include "esp_clk_tree.h"
#include "esp_cam_ctlr.h"
#include "esp_cam_ctlr_csi.h"
#include "esp_cam_ctlr_interface.h"
#include "esp_cam_ctlr_csi_internal.h"
#include "hal/mipi_csi_ll.h"
#include "hal/color_hal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/mipi_csi_share_hw_ctrl.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_cache.h"

#if CONFIG_CAM_CTLR_MIPI_CSI_ISR_CACHE_SAFE
#define CSI_MEM_ALLOC_CAPS   (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define CSI_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT
#endif

typedef struct csi_platform_t {
    _lock_t         mutex;
    csi_controller_t *controllers[MIPI_CSI_LL_HOST_CTLR_NUMS];
} csi_platform_t;

static const char *TAG = "CSI";
static csi_platform_t s_platform;

static bool csi_dma_trans_done_callback(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data);
static esp_err_t s_del_csi_ctlr(csi_controller_t *ctlr);
static esp_err_t s_ctlr_del(esp_cam_ctlr_t *cam_ctlr);
static esp_err_t s_register_event_callbacks(esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_data);
static esp_err_t s_csi_ctlr_get_internal_buffer(esp_cam_ctlr_handle_t handle, uint32_t fb_num, const void **fb0, ...);
static esp_err_t s_csi_ctlr_get_buffer_length(esp_cam_ctlr_handle_t handle, size_t *ret_fb_len);
static esp_err_t s_csi_ctlr_enable(esp_cam_ctlr_handle_t ctlr);
static esp_err_t s_ctlr_csi_start(esp_cam_ctlr_handle_t handle);
static esp_err_t s_ctlr_csi_stop(esp_cam_ctlr_handle_t handle);
static esp_err_t s_csi_ctlr_disable(esp_cam_ctlr_handle_t ctlr);
static esp_err_t s_ctlr_csi_receive(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, uint32_t timeout_ms);

static esp_err_t s_csi_claim_controller(csi_controller_t *controller)
{
    assert(controller);

    _lock_acquire(&s_platform.mutex);
    bool found = false;
    for (int i = 0; i < MIPI_CSI_LL_HOST_CTLR_NUMS; i ++) {
        found = !s_platform.controllers[i];
        if (found) {
            s_platform.controllers[i] = controller;
            controller->csi_id = i;
            PERIPH_RCC_ATOMIC() {
                mipi_csi_ll_enable_host_bus_clock(i, 0);
                mipi_csi_ll_enable_host_bus_clock(i, 1);
                mipi_csi_ll_reset_host_clock(i);
            }
            break;
        }
    }
    _lock_release(&s_platform.mutex);

    if (!found) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

static esp_err_t s_csi_declaim_controller(csi_controller_t *controller)
{
    assert(controller);

    _lock_acquire(&s_platform.mutex);
    s_platform.controllers[controller->csi_id] = NULL;
    PERIPH_RCC_ATOMIC() {
        mipi_csi_ll_enable_host_bus_clock(controller->csi_id, 0);
    }
    _lock_release(&s_platform.mutex);

    return ESP_OK;
}

esp_err_t esp_cam_new_csi_ctlr(const esp_cam_ctlr_csi_config_t *config, esp_cam_ctlr_handle_t *ret_handle)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(config->data_lane_num <= MIPI_CSI_HOST_LL_LANE_NUM_MAX, ESP_ERR_INVALID_ARG, TAG, "lane num should be equal or smaller than %d", MIPI_CSI_HOST_LL_LANE_NUM_MAX);

    csi_controller_t *ctlr = heap_caps_calloc(1, sizeof(csi_controller_t), CSI_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_NO_MEM, TAG, "no mem for csi controller context");

    ret = s_csi_claim_controller(ctlr);
    if (ret != ESP_OK) {
        //claim fail, clean and return directly
        free(ctlr);
        ESP_RETURN_ON_ERROR(ret, TAG, "no available csi controller");
    }

    ESP_LOGD(TAG, "config->queue_items: %d", config->queue_items);
    ctlr->trans_que = xQueueCreateWithCaps(config->queue_items, sizeof(esp_cam_ctlr_trans_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(ctlr->trans_que, ESP_ERR_NO_MEM, err, TAG, "no memory for transaction queue");

#if SOC_ISP_SHARE_CSI_BRG
    ESP_GOTO_ON_ERROR(mipi_csi_brg_claim(MIPI_CSI_BRG_USER_CSI, &ctlr->csi_brg_id), err, TAG, "csi bridge is in use already");
    ctlr->csi_brg_in_use = true;
#endif

    mipi_csi_phy_clock_source_t clk_src = !config->clk_src ? MIPI_CSI_PHY_CLK_SRC_DEFAULT : config->clk_src;
    ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)clk_src, true), err, TAG, "clock source enable failed");
    PERIPH_RCC_ATOMIC() {
        // phy clock source setting
        mipi_csi_ll_set_phy_clock_source(ctlr->csi_id, clk_src);
        // phy clock reset
        mipi_csi_ll_enable_phy_config_clock(ctlr->csi_id, 0);
        mipi_csi_ll_enable_phy_config_clock(ctlr->csi_id, 1);
    }

    ctlr->h_res = config->h_res;
    ctlr->v_res = config->v_res;
    ESP_LOGD(TAG, "ctlr->h_res: 0d %"PRId32, ctlr->h_res);
    ESP_LOGD(TAG, "ctlr->v_res: 0d %"PRId32, ctlr->v_res);

    //in color type
    color_space_pixel_format_t in_color_format = {
        .color_type_id = config->input_data_color_type,
    };
    int in_bits_per_pixel = color_hal_pixel_format_get_bit_depth(in_color_format);
    ctlr->in_color_format = in_color_format;
    ctlr->in_bpp = in_bits_per_pixel;
    ESP_LOGD(TAG, "ctlr->in_bpp: 0d %d", ctlr->in_bpp);

    //out color type
    color_space_pixel_format_t out_color_format = {
        .color_type_id = config->output_data_color_type,
    };
    int out_bits_per_pixel = color_hal_pixel_format_get_bit_depth(out_color_format);
    ctlr->out_bpp = out_bits_per_pixel;
    ESP_LOGD(TAG, "ctlr->out_bpp: 0d %d", ctlr->out_bpp);

    // Note: Width * Height * BitsPerPixel must be divisible by 8
    int fb_size_in_bits = config->v_res * config->h_res * out_bits_per_pixel;
    ESP_GOTO_ON_FALSE((fb_size_in_bits % 8 == 0), ESP_ERR_INVALID_ARG, err, TAG, "framesize not 8bit aligned");
    ctlr->fb_size_in_bytes = fb_size_in_bits / 8;
    ESP_LOGD(TAG, "ctlr->fb_size_in_bytes=%d", ctlr->fb_size_in_bytes);

    ctlr->bk_buffer_dis = config->bk_buffer_dis;
    if (!ctlr->bk_buffer_dis) {
        size_t dma_alignment = 4;  //TODO: IDF-9126, replace with dwgdma alignment API
        size_t cache_alignment = 1;
        ESP_GOTO_ON_ERROR(esp_cache_get_alignment(MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA, &cache_alignment), err, TAG, "failed to get cache alignment");
        size_t alignment = MAX(cache_alignment, dma_alignment);
        ESP_LOGD(TAG, "alignment: 0x%x\n", alignment);

        ctlr->backup_buffer = heap_caps_aligned_alloc(alignment, ctlr->fb_size_in_bytes, MALLOC_CAP_SPIRAM);
        ESP_GOTO_ON_FALSE(ctlr->backup_buffer, ESP_ERR_NO_MEM, err, TAG, "no mem for backup buffer");
        ESP_LOGD(TAG, "ctlr->backup_buffer: %p\n", ctlr->backup_buffer);
        esp_cache_msync((void *)(ctlr->backup_buffer), ctlr->fb_size_in_bytes, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    }

    mipi_csi_hal_config_t hal_config;
    hal_config.frame_height = config->h_res;
    hal_config.frame_width = config->v_res;
    hal_config.lane_bit_rate_mbps = config->lane_bit_rate_mbps;
    hal_config.lanes_num = config->data_lane_num;
    hal_config.byte_swap_en = config->byte_swap_en;
    mipi_csi_hal_init(&ctlr->hal, &hal_config);
    mipi_csi_brg_ll_set_burst_len(ctlr->hal.bridge_dev, 512);

    //---------------DWGDMA Init For CSI------------------//
    dw_gdma_channel_handle_t csi_dma_chan = NULL;
    dw_gdma_channel_alloc_config_t csi_dma_alloc_config = {
        .src = {
            .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
            .role = DW_GDMA_ROLE_PERIPH_CSI,
            .handshake_type = DW_GDMA_HANDSHAKE_HW,
            .num_outstanding_requests = 5,
            .status_fetch_addr = MIPI_CSI_BRG_MEM_BASE,
        },
        .dst = {
            .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
            .role = DW_GDMA_ROLE_MEM,
            .handshake_type = DW_GDMA_HANDSHAKE_HW,
            .num_outstanding_requests = 5,
        },
        .flow_controller = DW_GDMA_FLOW_CTRL_SRC,
        .chan_priority = 1,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_new_channel(&csi_dma_alloc_config, &csi_dma_chan), err, TAG, "failed to new dwgdma channel");
    ctlr->dma_chan = csi_dma_chan;

    size_t csi_transfer_size = ctlr->h_res * ctlr->v_res * ctlr->in_bpp / 64;
    ctlr->csi_transfer_size = csi_transfer_size;
    ESP_LOGD(TAG, "csi_transfer_size: 0d %d", csi_transfer_size);

    dw_gdma_event_callbacks_t csi_dma_cbs = {
        .on_full_trans_done = csi_dma_trans_done_callback,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_channel_register_event_callbacks(csi_dma_chan, &csi_dma_cbs, ctlr), err, TAG, "failed to register dwgdma callback");

#if CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "cam_csi_ctlr", &ctlr->pm_lock), err, TAG, "failed to create pm lock");
#endif //CONFIG_PM_ENABLE

    ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    ctlr->csi_fsm = CSI_FSM_INIT;
    ctlr->base.del = s_ctlr_del;
    ctlr->base.enable = s_csi_ctlr_enable;
    ctlr->base.start = s_ctlr_csi_start;
    ctlr->base.stop = s_ctlr_csi_stop;
    ctlr->base.disable = s_csi_ctlr_disable;
    ctlr->base.receive = s_ctlr_csi_receive;
    ctlr->base.register_event_callbacks = s_register_event_callbacks;
    ctlr->base.get_internal_buffer = s_csi_ctlr_get_internal_buffer;
    ctlr->base.get_buffer_len = s_csi_ctlr_get_buffer_length;

    *ret_handle = &(ctlr->base);

    return ESP_OK;

err:
    s_del_csi_ctlr(ctlr);

    return ret;
}

esp_err_t s_del_csi_ctlr(csi_controller_t *ctlr)
{
    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(ctlr->csi_fsm == CSI_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "processor isn't in init state");

#if CONFIG_PM_ENABLE
    if (ctlr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(ctlr->pm_lock), TAG, "delete pm_lock failed");
    }
#endif // CONFIG_PM_ENABLE

    if (ctlr->dma_chan) {
        ESP_RETURN_ON_ERROR(dw_gdma_del_channel(ctlr->dma_chan), TAG, "failed to delete dwgdma channel");
    }
    //declaim first, then do free
    ESP_RETURN_ON_ERROR(s_csi_declaim_controller(ctlr), TAG, "declaim processor fail");
#if SOC_ISP_SHARE_CSI_BRG
    if (ctlr->csi_brg_in_use) {
        ESP_RETURN_ON_ERROR(mipi_csi_brg_declaim(ctlr->csi_brg_id), TAG, "declaim csi bridge fail");
        ctlr->csi_brg_in_use = false;
    }
#endif
    PERIPH_RCC_ATOMIC() {
        mipi_csi_ll_enable_phy_config_clock(ctlr->csi_id, 0);
    }
    if (!ctlr->bk_buffer_dis) {
        free(ctlr->backup_buffer);
    }
    vQueueDeleteWithCaps(ctlr->trans_que);
    free(ctlr);

    return ESP_OK;
}

static esp_err_t s_ctlr_del(esp_cam_ctlr_t *cam_ctlr)
{
    csi_controller_t *csi_ctlr = __containerof(cam_ctlr, csi_controller_t, base);
    ESP_RETURN_ON_ERROR(s_del_csi_ctlr(csi_ctlr), TAG, "failed to del csi_ctlr");
    return ESP_OK;
}

static esp_err_t s_csi_ctlr_get_internal_buffer(esp_cam_ctlr_handle_t handle, uint32_t fb_num, const void **fb0, ...)
{
    csi_controller_t *csi_ctlr = __containerof(handle, csi_controller_t, base);
    ESP_RETURN_ON_FALSE((csi_ctlr->csi_fsm >= CSI_FSM_INIT) && (csi_ctlr->backup_buffer), ESP_ERR_INVALID_STATE, TAG, "driver don't initialized or back_buffer not available");
    ESP_RETURN_ON_FALSE(fb_num && fb_num <= 1, ESP_ERR_INVALID_ARG, TAG, "invalid frame buffer number");

    csi_ctlr->bk_buffer_exposed = true;
    const void **fb_itor = fb0;
    va_list args;
    va_start(args, fb0);
    for (uint32_t i = 0; i < fb_num; i++) {
        if (fb_itor) {
            *fb_itor = csi_ctlr->backup_buffer;
            fb_itor = va_arg(args, const void **);
        }
    }
    va_end(args);

    return ESP_OK;
}

static esp_err_t s_csi_ctlr_get_buffer_length(esp_cam_ctlr_handle_t handle, size_t *ret_fb_len)
{
    csi_controller_t *csi_ctlr = __containerof(handle, csi_controller_t, base);
    ESP_RETURN_ON_FALSE((csi_ctlr->csi_fsm >= CSI_FSM_INIT) && (csi_ctlr->backup_buffer), ESP_ERR_INVALID_STATE, TAG, "driver don't initialized or back_buffer not available");

    *ret_fb_len = csi_ctlr->fb_size_in_bytes;
    return ESP_OK;
}

IRAM_ATTR static bool csi_dma_trans_done_callback(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    BaseType_t high_task_woken = pdFALSE;
    csi_controller_t *ctlr = (csi_controller_t *)user_data;
    bool has_new_trans = false;

    dw_gdma_block_transfer_config_t csi_dma_transfer_config = {};
    csi_dma_transfer_config = (dw_gdma_block_transfer_config_t) {
        .src = {
            .addr = MIPI_CSI_BRG_MEM_BASE,
            .burst_mode = DW_GDMA_BURST_MODE_FIXED,
            .burst_items = DW_GDMA_BURST_ITEMS_512,
            .burst_len = 16,
            .width = DW_GDMA_TRANS_WIDTH_64,
        },
        .dst = {
            .addr = 0,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .burst_items = DW_GDMA_BURST_ITEMS_512,
            .burst_len = 16,
            .width = DW_GDMA_TRANS_WIDTH_64,
        },
        .size = ctlr->csi_transfer_size,
    };
    esp_cam_ctlr_trans_t new_trans = {};

    if (ctlr->cbs.on_get_new_trans) {
        need_yield = ctlr->cbs.on_get_new_trans(&(ctlr->base), &new_trans, ctlr->cbs_user_data);
        if (new_trans.buffer && new_trans.buflen >= ctlr->fb_size_in_bytes) {
            csi_dma_transfer_config.dst.addr = (uint32_t)(new_trans.buffer);
            has_new_trans = true;
        }
    } else if (xQueueReceiveFromISR(ctlr->trans_que, &new_trans, &high_task_woken) == pdTRUE) {
        if (new_trans.buffer && new_trans.buflen >= ctlr->fb_size_in_bytes) {
            csi_dma_transfer_config.dst.addr = (uint32_t)(new_trans.buffer);
            has_new_trans = true;
        }
    }

    if (!has_new_trans) {
        if (!ctlr->bk_buffer_dis) {
            new_trans.buffer = ctlr->backup_buffer;
            new_trans.buflen = ctlr->fb_size_in_bytes;
            ESP_EARLY_LOGD(TAG, "no new buffer or no long enough new buffer, use driver internal buffer");
            csi_dma_transfer_config.dst.addr = (uint32_t)ctlr->backup_buffer;
        } else {
            assert(false && "no new buffer, and no driver internal buffer");
        }
    }

    ESP_EARLY_LOGD(TAG, "new_trans.buffer: %p, new_trans.buflen: %d", new_trans.buffer, new_trans.buflen);
    dw_gdma_channel_config_transfer(chan, &csi_dma_transfer_config);
    dw_gdma_channel_enable_ctrl(chan, true);

    if ((ctlr->trans.buffer != ctlr->backup_buffer) || ctlr->bk_buffer_exposed) {
        esp_err_t ret = esp_cache_msync((void *)(ctlr->trans.buffer), ctlr->trans.received_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
        assert(ctlr->cbs.on_trans_finished);
        if (ctlr->cbs.on_trans_finished) {
            ctlr->trans.received_size = ctlr->fb_size_in_bytes;
            need_yield = ctlr->cbs.on_trans_finished(&(ctlr->base), &ctlr->trans, ctlr->cbs_user_data);
        }
    }

    //ctlr->trans is the transaction saved before dma starts
    memset(&ctlr->trans, 0x0, sizeof(esp_cam_ctlr_trans_t));
    ctlr->trans = new_trans;

    need_yield |= high_task_woken == pdTRUE;
    return need_yield;
}

esp_err_t s_register_event_callbacks(esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(handle && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    csi_controller_t *ctlr = __containerof(handle, csi_controller_t, base);
    ESP_RETURN_ON_FALSE(ctlr->csi_fsm == CSI_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "driver starts already, not allow cbs register");

#if CONFIG_CAM_CTLR_MIPI_CSI_ISR_CACHE_SAFE
    if (cbs->on_get_new_trans) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_get_new_trans), ESP_ERR_INVALID_ARG, TAG, "on_get_new_trans callback not in IRAM");
    }
    if (cbs->on_trans_finished) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_finished), ESP_ERR_INVALID_ARG, TAG, "on_trans_finished callback not in IRAM");
    }
#endif

    ctlr->cbs.on_get_new_trans = cbs->on_get_new_trans;
    ctlr->cbs.on_trans_finished = cbs->on_trans_finished;
    ctlr->cbs_user_data = user_data;

    return ESP_OK;
}

esp_err_t s_csi_ctlr_enable(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    csi_controller_t *ctlr = __containerof(handle, csi_controller_t, base);
    ESP_RETURN_ON_FALSE(ctlr->csi_fsm == CSI_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "processor isn't in init state");

    portENTER_CRITICAL(&ctlr->spinlock);
    ctlr->csi_fsm = CSI_FSM_ENABLED;
#if CONFIG_PM_ENABLE
    if (ctlr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(ctlr->pm_lock), TAG, "acquire pm_lock failed");
    }
#endif // CONFIG_PM_ENABLE
    portEXIT_CRITICAL(&ctlr->spinlock);

    return ESP_OK;
}

esp_err_t s_csi_ctlr_disable(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    csi_controller_t *ctlr = __containerof(handle, csi_controller_t, base);
    ESP_RETURN_ON_FALSE(ctlr->csi_fsm == CSI_FSM_ENABLED, ESP_ERR_INVALID_STATE, TAG, "processor isn't in enable state");

    portENTER_CRITICAL(&ctlr->spinlock);
    ctlr->csi_fsm = CSI_FSM_INIT;
#if CONFIG_PM_ENABLE
    if (ctlr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(ctlr->pm_lock), TAG, "release pm_lock failed");
    }
#endif // CONFIG_PM_ENABLE
    portEXIT_CRITICAL(&ctlr->spinlock);

    return ESP_OK;
}

esp_err_t s_ctlr_csi_start(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    csi_controller_t *ctlr = __containerof(handle, csi_controller_t, base);
    ESP_RETURN_ON_FALSE(ctlr->csi_fsm == CSI_FSM_ENABLED, ESP_ERR_INVALID_STATE, TAG, "driver starts already, should not start again");
    ESP_RETURN_ON_FALSE(ctlr->cbs.on_trans_finished, ESP_ERR_INVALID_STATE, TAG, "no on_trans_finished callback registered");

    esp_cam_ctlr_trans_t trans = {};
    bool has_new_trans = false;

    if (ctlr->cbs.on_get_new_trans) {
        ctlr->cbs.on_get_new_trans(handle, &trans, ctlr->cbs_user_data);
        if (trans.buffer) {
            has_new_trans = true;
        }
    }

    if (!has_new_trans) {
        if (!ctlr->bk_buffer_dis) {
            trans.buffer = ctlr->backup_buffer;
            trans.buflen = ctlr->fb_size_in_bytes;
        } else {
            ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "no ready transaction, and no backup buffer");
        }
    }

    ESP_LOGD(TAG, "trans.buffer: %p, trans.buflen: %d", trans.buffer, trans.buflen);
    ctlr->trans = trans;

    portENTER_CRITICAL(&ctlr->spinlock);
    ctlr->csi_fsm = CSI_FSM_STARTED;
    portEXIT_CRITICAL(&ctlr->spinlock);

    dw_gdma_block_transfer_config_t csi_dma_transfer_config = {};
    csi_dma_transfer_config = (dw_gdma_block_transfer_config_t) {
        .src = {
            .addr = MIPI_CSI_BRG_MEM_BASE,
            .burst_mode = DW_GDMA_BURST_MODE_FIXED,
            .burst_items = DW_GDMA_BURST_ITEMS_512,
            .burst_len = 16,
            .width = DW_GDMA_TRANS_WIDTH_64,
        },
        .dst = {
            .addr = (uint32_t)(trans.buffer),
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .burst_items = DW_GDMA_BURST_ITEMS_512,
            .burst_len = 16,
            .width = DW_GDMA_TRANS_WIDTH_64,
        },
        .size = ctlr->csi_transfer_size,
    };
    ESP_RETURN_ON_ERROR(dw_gdma_channel_config_transfer(ctlr->dma_chan, &csi_dma_transfer_config), TAG, "failed to configure dwgdma transfer");
    ESP_RETURN_ON_ERROR(dw_gdma_channel_enable_ctrl(ctlr->dma_chan, true), TAG, "failed to enable dwgdma");

    //enable CSI bridge
    mipi_csi_brg_ll_enable(ctlr->hal.bridge_dev, true);

    return ESP_OK;
}

esp_err_t s_ctlr_csi_stop(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    csi_controller_t *ctlr = __containerof(handle, csi_controller_t, base);
    ESP_RETURN_ON_FALSE(ctlr->csi_fsm == CSI_FSM_STARTED, ESP_ERR_INVALID_STATE, TAG, "driver isn't started");

    //disable CSI bridge
    mipi_csi_brg_ll_enable(ctlr->hal.bridge_dev, false);
    ESP_RETURN_ON_ERROR(dw_gdma_channel_enable_ctrl(ctlr->dma_chan, false), TAG, "failed to disable dwgdma");

    portENTER_CRITICAL(&ctlr->spinlock);
    ctlr->csi_fsm = CSI_FSM_ENABLED;
    portEXIT_CRITICAL(&ctlr->spinlock);

    return ESP_OK;
}

esp_err_t s_ctlr_csi_receive(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, uint32_t timeout_ms)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(handle && trans, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    csi_controller_t *ctlr = __containerof(handle, csi_controller_t, base);

    ESP_RETURN_ON_FALSE(trans->buffer, ESP_ERR_INVALID_ARG, TAG, "invalid argument: no trans buffer");
    ESP_RETURN_ON_FALSE((trans->buflen >= ctlr->fb_size_in_bytes), ESP_ERR_INVALID_ARG, TAG, "invalid argument: trans buffer smaller than framebuffer size");

    TickType_t ticks_to_wait = timeout_ms / portTICK_PERIOD_MS;
    if (timeout_ms == ESP_CAM_CTLR_MAX_DELAY) {
        ticks_to_wait = portMAX_DELAY;
    }

    BaseType_t r = xQueueSend(ctlr->trans_que, trans, ticks_to_wait);
    if (r != pdTRUE) {
        ret = ESP_ERR_TIMEOUT;
        ESP_LOGW(TAG, "csi recv API, transaction queue is full, failed to send transaction to the queue");
        return ret;
    }

    return ESP_OK;
}
