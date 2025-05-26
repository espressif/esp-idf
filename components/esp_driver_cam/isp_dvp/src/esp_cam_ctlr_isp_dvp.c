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
#include "driver/isp_types.h"
#include "driver/gpio.h"
#include "hal/isp_hal.h"
#include "hal/isp_ll.h"
#include "hal/mipi_csi_brg_ll.h"
#include "hal/mipi_csi_ll.h"
#include "hal/color_hal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/isp_private.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/mipi_csi_share_hw_ctrl.h"
#include "esp_private/dw_gdma.h"
#include "esp_cam_ctlr_types.h"
#include "esp_cam_ctlr_interface.h"
#include "esp_cache.h"
#include "esp_cam_ctlr_isp_dvp.h"
#include "../../dvp_share_ctrl.h"

#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#if CONFIG_CAM_CTLR_ISP_DVP_ISR_CACHE_SAFE
#define ISP_DVP_MEM_ALLOC_CAPS   (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define ISP_DVP_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT
#endif

typedef struct isp_dvp_controller_t {
    int                       id;                 //dvp id
    isp_fsm_t                 fsm;                //finite state machine
    portMUX_TYPE              spinlock;           //spinlock
    isp_processor_t           *isp_proc;          //isp processor
    int                       in_bpp;             //input data type, bit per pixel
    int                       out_bpp;            //output data type, bit per pixel
    size_t                    fb_size_in_bytes;   //Frame buffer size, in bytes
    esp_cam_ctlr_trans_t      trans;              //Saved done transaction to be given out to callers
    void                      *backup_buffer;     //backup buffer to make csi bridge can work to avoid wrong state
    bool                      bk_buffer_exposed;  //status of if back_buffer is exposed to users
    bool                      bk_buffer_dis;      //allow to not malloc backup_buffer
    QueueHandle_t             trans_que;          //transaction queue
    esp_cam_ctlr_evt_cbs_t    cbs;                //user callbacks
    void                      *cbs_user_data;     //callback userdata
    dw_gdma_channel_handle_t  dma_chan;           //dwgdma channel handle
    size_t                    dvp_transfer_size;  //csi transfer size for dwgdma
    bool                      isr_installed;      //is isr installed
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t      pm_lock;            //Power management lock
#endif
    esp_cam_ctlr_t base;
} isp_dvp_controller_t;

typedef struct isp_dvp_ctx_t {
    _lock_t              mutex;
    isp_dvp_controller_t *dvp_ctlr[SOC_ISP_DVP_CTLR_NUMS];
} isp_dvp_ctx_t;

static const char *TAG = "ISP_DVP";
static isp_dvp_ctx_t s_ctx;

static esp_err_t s_isp_io_init(isp_dvp_controller_t *dvp_ctlr, const esp_cam_ctlr_isp_dvp_cfg_t *ctlr_config);
static esp_err_t s_isp_claim_dvp_controller(isp_proc_handle_t isp_proc, isp_dvp_controller_t *dvp_ctlr);
static esp_err_t s_isp_declaim_dvp_controller(isp_dvp_controller_t *dvp_ctlr);
static esp_err_t s_isp_del_dvp_controller(esp_cam_ctlr_handle_t handle);
static esp_err_t s_isp_dvp_get_frame_buffer(esp_cam_ctlr_handle_t handle, uint32_t fb_num, const void **fb0, ...);
static esp_err_t s_isp_dvp_get_frame_buffer_length(esp_cam_ctlr_handle_t handle, size_t *ret_fb_len);
static esp_err_t s_isp_dvp_register_event_callbacks(esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_data);
static esp_err_t s_isp_dvp_enable(esp_cam_ctlr_handle_t handle);
static esp_err_t s_isp_dvp_disable(esp_cam_ctlr_handle_t handle);
static esp_err_t s_isp_dvp_start(esp_cam_ctlr_handle_t handle);
static esp_err_t s_isp_dvp_stop(esp_cam_ctlr_handle_t handle);
static esp_err_t s_isp_dvp_receive(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, uint32_t timeout_ms);
static bool s_dvp_dma_trans_done_callback(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data);

esp_err_t esp_cam_new_isp_dvp_ctlr(isp_proc_handle_t isp_proc, const esp_cam_ctlr_isp_dvp_cfg_t *ctlr_config, esp_cam_ctlr_handle_t *ret_handle)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(isp_proc && ctlr_config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    isp_dvp_controller_t *dvp_ctlr = heap_caps_calloc(1, sizeof(isp_dvp_controller_t), ISP_DVP_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(dvp_ctlr, ESP_ERR_NO_MEM, TAG, "no mem for isp dvp controller");

    dvp_ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    dvp_ctlr->isp_proc = isp_proc;

#if SOC_ISP_SHARE_CSI_BRG
    ESP_GOTO_ON_ERROR(mipi_csi_brg_claim(MIPI_CSI_BRG_USER_ISP_DVP, &isp_proc->csi_brg_id), err, TAG, "csi bridge is in use already");
#endif
    //claim an DVP controller
    ESP_GOTO_ON_ERROR(s_isp_claim_dvp_controller(isp_proc, dvp_ctlr), err, TAG, "no available controller");
    ESP_GOTO_ON_ERROR(dvp_shared_ctrl_claim_io_signals(), err, TAG, "failed to claim io signals");
    ESP_GOTO_ON_ERROR(s_isp_io_init(dvp_ctlr, ctlr_config), err, TAG, "io init fail");

    dvp_ctlr->trans_que = xQueueCreateWithCaps(ctlr_config->queue_items, sizeof(esp_cam_ctlr_trans_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(dvp_ctlr->trans_que, ESP_ERR_NO_MEM, err, TAG, "no memory for transaction queue");
    //in color type
    int in_bits_per_pixel = color_hal_pixel_format_get_bit_depth(isp_proc->in_color_format);
    dvp_ctlr->in_bpp = in_bits_per_pixel;
    ESP_LOGD(TAG, "dvp_ctlr->in_bpp: 0d %d", dvp_ctlr->in_bpp);

    //out color type
    int out_bits_per_pixel = color_hal_pixel_format_get_bit_depth(isp_proc->out_color_format);
    dvp_ctlr->out_bpp = out_bits_per_pixel;
    ESP_LOGD(TAG, "dvp_ctlr->out_bpp: 0d %d", dvp_ctlr->out_bpp);

    // Note: Width * Height * BitsPerPixel must be divisible by 8
    int fb_size_in_bits = isp_proc->v_res * isp_proc->h_res * out_bits_per_pixel;
    ESP_GOTO_ON_FALSE((fb_size_in_bits % 8 == 0), ESP_ERR_INVALID_ARG, err, TAG, "framesize not 8bit aligned");
    dvp_ctlr->fb_size_in_bytes = fb_size_in_bits / 8;
    ESP_LOGD(TAG, "dvp_ctlr->fb_size_in_bytes=%d", dvp_ctlr->fb_size_in_bytes);

    dvp_ctlr->bk_buffer_dis = ctlr_config->bk_buffer_dis;
    if (!dvp_ctlr->bk_buffer_dis) {
        size_t dma_alignment = 4;
        size_t cache_alignment = 1;
        ESP_GOTO_ON_ERROR(esp_cache_get_alignment(MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA, &cache_alignment), err, TAG, "failed to get cache alignment");
        size_t alignment = MAX(cache_alignment, dma_alignment);
        ESP_LOGD(TAG, "alignment: 0x%x\n", alignment);

        dvp_ctlr->backup_buffer = heap_caps_aligned_alloc(alignment, dvp_ctlr->fb_size_in_bytes, MALLOC_CAP_SPIRAM);
        ESP_GOTO_ON_FALSE(dvp_ctlr->backup_buffer, ESP_ERR_NO_MEM, err, TAG, "no mem for backup buffer");
        ESP_LOGD(TAG, "dvp_ctlr->backup_buffer: %p\n", dvp_ctlr->backup_buffer);
        esp_cache_msync((void *)(dvp_ctlr->backup_buffer), dvp_ctlr->fb_size_in_bytes, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    }

    bool valid_format = isp_ll_dvp_set_data_type(isp_proc->hal.hw, isp_proc->in_color_format);
    ESP_GOTO_ON_FALSE(valid_format, ESP_ERR_INVALID_ARG, err, TAG, "invalid dvp color space config");
    if (ctlr_config->io_flags.pclk_invert) {
        isp_ll_cam_enable_pclk_invert(isp_proc->hal.hw, true);
    }
    if (ctlr_config->io_flags.hsync_invert) {
        isp_ll_cam_enable_hsync_invert(isp_proc->hal.hw, true);
    }
    if (ctlr_config->io_flags.vsync_invert) {
        isp_ll_cam_enable_vsync_invert(isp_proc->hal.hw, true);
    }
    if (ctlr_config->io_flags.de_invert) {
        isp_ll_cam_enable_de_invert(isp_proc->hal.hw, true);
    }
    isp_ll_dvp_cam_reset(isp_proc->hal.hw);

    // configure DW_GDMA for ISP DVP
    dw_gdma_channel_alloc_config_t dvp_dma_alloc_config = {
        .src = {
            .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
#if SOC_ISP_SHARE_CSI_BRG
            .role = DW_GDMA_ROLE_PERIPH_CSI,  //CSI bridge
#endif
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
    ESP_ERROR_CHECK(dw_gdma_new_channel(&dvp_dma_alloc_config, &dvp_ctlr->dma_chan));

    size_t dvp_transfer_size = isp_proc->h_res * isp_proc->v_res * dvp_ctlr->in_bpp / 64;
    dvp_ctlr->dvp_transfer_size = dvp_transfer_size;
    ESP_LOGD(TAG, "dvp_transfer_size: 0d %d", dvp_transfer_size);

    dvp_ctlr->fsm = ISP_FSM_INIT;
    isp_proc->csi_brg_hw = MIPI_CSI_BRG_LL_GET_HW(isp_proc->csi_brg_id);
    mipi_csi_brg_ll_set_intput_data_h_pixel_num(isp_proc->csi_brg_hw, isp_proc->h_res);
    mipi_csi_brg_ll_set_intput_data_v_row_num(isp_proc->csi_brg_hw, isp_proc->v_res);
    mipi_csi_brg_ll_set_burst_len(isp_proc->csi_brg_hw, 512);

    esp_cam_ctlr_t *cam_ctlr = &(dvp_ctlr->base);

#if CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "isp_dvp_cam_ctlr", &dvp_ctlr->pm_lock), err, TAG, "failed to create pm lock");
#endif //CONFIG_PM_ENABLE

    cam_ctlr->del = s_isp_del_dvp_controller;
    cam_ctlr->enable = s_isp_dvp_enable;
    cam_ctlr->start = s_isp_dvp_start;
    cam_ctlr->stop = s_isp_dvp_stop;
    cam_ctlr->disable = s_isp_dvp_disable;
    cam_ctlr->receive = s_isp_dvp_receive;
    cam_ctlr->register_event_callbacks = s_isp_dvp_register_event_callbacks;
    cam_ctlr->get_internal_buffer = s_isp_dvp_get_frame_buffer;
    cam_ctlr->get_buffer_len = s_isp_dvp_get_frame_buffer_length;
    *ret_handle = cam_ctlr;

    return ESP_OK;

err:
    if (dvp_ctlr) {
        s_isp_del_dvp_controller(&(dvp_ctlr->base));
    }

    return ret;
}

esp_err_t s_isp_del_dvp_controller(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_dvp_controller_t *dvp_ctlr = __containerof(handle, isp_dvp_controller_t, base);

    ESP_RETURN_ON_FALSE(dvp_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller isn't in init state");
    if (dvp_ctlr->dma_chan) {
        ESP_RETURN_ON_ERROR(dw_gdma_del_channel(dvp_ctlr->dma_chan), TAG, "failed to delete dwgdma channel");
    }
#if SOC_ISP_SHARE_CSI_BRG
    ESP_RETURN_ON_ERROR(mipi_csi_brg_declaim(dvp_ctlr->isp_proc->csi_brg_id), TAG, "declaim csi bridge fail");
#endif
    ESP_RETURN_ON_ERROR(s_isp_declaim_dvp_controller(dvp_ctlr), TAG, "controller isn't in use");
    ESP_RETURN_ON_ERROR(dvp_shared_ctrl_declaim_io_signals(), TAG, "failed to declaim io signals");
    if (!dvp_ctlr->bk_buffer_dis) {
        free(dvp_ctlr->backup_buffer);
    }
    if (dvp_ctlr->trans_que) {
        vQueueDeleteWithCaps(dvp_ctlr->trans_que);
    }
#if CONFIG_PM_ENABLE
    if (dvp_ctlr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(dvp_ctlr->pm_lock), TAG, "delete pm_lock failed");
    }
#endif // CONFIG_PM_ENABLE

    free(dvp_ctlr);

    return ESP_OK;
}

static esp_err_t s_isp_dvp_get_frame_buffer(esp_cam_ctlr_handle_t handle, uint32_t fb_num, const void **fb0, ...)
{
    isp_dvp_controller_t *dvp_ctlr = __containerof(handle, isp_dvp_controller_t, base);
    ESP_RETURN_ON_FALSE((dvp_ctlr->fsm >= ISP_FSM_INIT) && (dvp_ctlr->backup_buffer), ESP_ERR_INVALID_STATE, TAG, "driver isn't initialized or back_buffer isn't available");
    ESP_RETURN_ON_FALSE(fb_num && fb_num <= 1, ESP_ERR_INVALID_ARG, TAG, "invalid frame buffer number");

    dvp_ctlr->bk_buffer_exposed = true;
    const void **fb_itor = fb0;
    va_list args;
    va_start(args, fb0);
    for (uint32_t i = 0; i < fb_num; i++) {
        if (fb_itor) {
            *fb_itor = dvp_ctlr->backup_buffer;
            fb_itor = va_arg(args, const void **);
        }
    }
    va_end(args);

    return ESP_OK;
}

static esp_err_t s_isp_dvp_get_frame_buffer_length(esp_cam_ctlr_handle_t handle, size_t *ret_fb_len)
{
    isp_dvp_controller_t *dvp_ctlr = __containerof(handle, isp_dvp_controller_t, base);
    ESP_RETURN_ON_FALSE((dvp_ctlr->fsm >= ISP_FSM_INIT) && (dvp_ctlr->backup_buffer), ESP_ERR_INVALID_STATE, TAG, "driver isn't initialized or back_buffer isn't available");

    *ret_fb_len = dvp_ctlr->fb_size_in_bytes;
    return ESP_OK;
}

static esp_err_t s_isp_dvp_register_event_callbacks(esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(handle && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    isp_dvp_controller_t *dvp_ctlr = __containerof(handle, isp_dvp_controller_t, base);
    ESP_RETURN_ON_FALSE(dvp_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller isn't in init state");

#if CONFIG_CAM_CTLR_ISP_DVP_ISR_CACHE_SAFE
    if (cbs->on_get_new_trans) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_get_new_trans), ESP_ERR_INVALID_ARG, TAG, "on_get_new_trans callback not in IRAM");
    }
    if (cbs->on_trans_finished) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_finished), ESP_ERR_INVALID_ARG, TAG, "on_trans_finished callback not in IRAM");
    }
#endif

    if (!dvp_ctlr->isr_installed) {
        dw_gdma_event_callbacks_t csi_dma_cbs = {
            .on_full_trans_done = s_dvp_dma_trans_done_callback,
        };
        ESP_RETURN_ON_ERROR(dw_gdma_channel_register_event_callbacks(dvp_ctlr->dma_chan, &csi_dma_cbs, dvp_ctlr), TAG, "failed to register dma callbacks");
        dvp_ctlr->isr_installed = true;
    }

    dvp_ctlr->cbs.on_get_new_trans = cbs->on_get_new_trans;
    dvp_ctlr->cbs.on_trans_finished = cbs->on_trans_finished;
    dvp_ctlr->cbs_user_data = user_data;

    return ESP_OK;
}

static esp_err_t s_isp_dvp_enable(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_dvp_controller_t *dvp_ctlr = __containerof(handle, isp_dvp_controller_t, base);
    ESP_RETURN_ON_FALSE(dvp_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller isn't in init state");

    portENTER_CRITICAL(&dvp_ctlr->spinlock);
    dvp_ctlr->fsm = ISP_FSM_ENABLE;
#if CONFIG_PM_ENABLE
    if (dvp_ctlr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(dvp_ctlr->pm_lock), TAG, "acquire pm_lock failed");
    }
#endif // CONFIG_PM_ENABLE
    portEXIT_CRITICAL(&dvp_ctlr->spinlock);

    return ESP_OK;
}

static esp_err_t s_isp_dvp_disable(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_dvp_controller_t *dvp_ctlr = __containerof(handle, isp_dvp_controller_t, base);
    ESP_RETURN_ON_FALSE(dvp_ctlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't in enable state");

    portENTER_CRITICAL(&dvp_ctlr->spinlock);
    dvp_ctlr->fsm = ISP_FSM_INIT;
#if CONFIG_PM_ENABLE
    if (dvp_ctlr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(dvp_ctlr->pm_lock), TAG, "release pm_lock failed");
    }
#endif // CONFIG_PM_ENABLE
    portEXIT_CRITICAL(&dvp_ctlr->spinlock);

    return ESP_OK;
}

static esp_err_t s_isp_dvp_start(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_dvp_controller_t *dvp_ctlr = __containerof(handle, isp_dvp_controller_t, base);
    ESP_RETURN_ON_FALSE(dvp_ctlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't in enable state");
    ESP_RETURN_ON_FALSE(dvp_ctlr->isp_proc->isp_fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "ISP processor isn't in enable state");
    ESP_RETURN_ON_FALSE(dvp_ctlr->cbs.on_trans_finished, ESP_ERR_INVALID_STATE, TAG, "no on_trans_finished callback registered");

    mipi_csi_brg_ll_enable(dvp_ctlr->isp_proc->csi_brg_hw, true);
    isp_ll_cam_enable(dvp_ctlr->isp_proc->hal.hw, true);

    esp_cam_ctlr_trans_t trans = {};
    bool has_new_trans = false;

    if (dvp_ctlr->cbs.on_get_new_trans) {
        dvp_ctlr->cbs.on_get_new_trans(handle, &trans, dvp_ctlr->cbs_user_data);
        if (trans.buffer) {
            has_new_trans = true;
        }
    }

    if (!has_new_trans) {
        if (!dvp_ctlr->bk_buffer_dis) {
            trans.buffer = dvp_ctlr->backup_buffer;
            trans.buflen = dvp_ctlr->fb_size_in_bytes;
        } else {
            ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "no ready transaction, and no backup buffer");
        }
    }

    ESP_LOGD(TAG, "trans.buffer: %p, trans.buflen: %d", trans.buffer, trans.buflen);
    dvp_ctlr->trans = trans;

    portENTER_CRITICAL(&dvp_ctlr->spinlock);
    dvp_ctlr->fsm = ISP_FSM_START;
    portEXIT_CRITICAL(&dvp_ctlr->spinlock);

    dw_gdma_block_transfer_config_t dvp_dma_transfer_config = {};
    dvp_dma_transfer_config = (dw_gdma_block_transfer_config_t) {
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
        .size = dvp_ctlr->dvp_transfer_size,
    };
    ESP_RETURN_ON_ERROR(dw_gdma_channel_config_transfer(dvp_ctlr->dma_chan, &dvp_dma_transfer_config), TAG, "failed to configure dwgdma transfer");
    ESP_RETURN_ON_ERROR(dw_gdma_channel_enable_ctrl(dvp_ctlr->dma_chan, true), TAG, "failed to enable dwgdma");

    return ESP_OK;
}

static esp_err_t s_isp_dvp_stop(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    isp_dvp_controller_t *dvp_ctlr = __containerof(handle, isp_dvp_controller_t, base);
    ESP_RETURN_ON_FALSE(dvp_ctlr->fsm == ISP_FSM_START, ESP_ERR_INVALID_STATE, TAG, "driver isn't started");

    isp_ll_cam_enable(dvp_ctlr->isp_proc->hal.hw, false);
    mipi_csi_brg_ll_enable(dvp_ctlr->isp_proc->csi_brg_hw, false);
    ESP_RETURN_ON_ERROR(dw_gdma_channel_enable_ctrl(dvp_ctlr->dma_chan, false), TAG, "failed to disable dwgdma");

    portENTER_CRITICAL(&dvp_ctlr->spinlock);
    dvp_ctlr->fsm = ISP_FSM_INIT;
    portEXIT_CRITICAL(&dvp_ctlr->spinlock);

    return ESP_OK;
}

static esp_err_t s_isp_dvp_receive(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, uint32_t timeout_ms)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    isp_dvp_controller_t *dvp_ctlr = __containerof(handle, isp_dvp_controller_t, base);

    ESP_RETURN_ON_FALSE(trans->buffer, ESP_ERR_INVALID_ARG, TAG, "invalid argument: no trans buffer");
    ESP_RETURN_ON_FALSE((trans->buflen >= dvp_ctlr->fb_size_in_bytes), ESP_ERR_INVALID_ARG, TAG, "invalid argument: trans buffer smaller than framebuffer size");

    TickType_t ticks_to_wait = timeout_ms / portTICK_PERIOD_MS;
    if (timeout_ms == ESP_CAM_CTLR_MAX_DELAY) {
        ticks_to_wait = portMAX_DELAY;
    }

    BaseType_t r = xQueueSend(dvp_ctlr->trans_que, trans, ticks_to_wait);
    if (r != pdTRUE) {
        ret = ESP_ERR_TIMEOUT;
        ESP_LOGW(TAG, "csi recv API, transaction queue is full, failed to send transaction to the queue");
        return ret;
    }

    return ESP_OK;
}

IRAM_ATTR static bool s_dvp_dma_trans_done_callback(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    BaseType_t high_task_woken = pdFALSE;
    isp_dvp_controller_t *dvp_ctlr = (isp_dvp_controller_t *)user_data;
    bool has_new_trans = false;

    dw_gdma_block_transfer_config_t dvp_dma_transfer_config = {};
    dvp_dma_transfer_config = (dw_gdma_block_transfer_config_t) {
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
        .size = dvp_ctlr->dvp_transfer_size,
    };
    esp_cam_ctlr_trans_t new_trans = {};

    if (dvp_ctlr->cbs.on_get_new_trans) {
        need_yield = dvp_ctlr->cbs.on_get_new_trans(&(dvp_ctlr->base), &new_trans, dvp_ctlr->cbs_user_data);
        if (new_trans.buffer && new_trans.buflen >= dvp_ctlr->fb_size_in_bytes) {
            dvp_dma_transfer_config.dst.addr = (uint32_t)(new_trans.buffer);
            has_new_trans = true;
        }
    } else if (xQueueReceiveFromISR(dvp_ctlr->trans_que, &new_trans, &high_task_woken) == pdTRUE) {
        if (new_trans.buffer && new_trans.buflen >= dvp_ctlr->fb_size_in_bytes) {
            dvp_dma_transfer_config.dst.addr = (uint32_t)(new_trans.buffer);
            has_new_trans = true;
        }
    }

    if (!has_new_trans) {
        if (!dvp_ctlr->bk_buffer_dis) {
            new_trans.buffer = dvp_ctlr->backup_buffer;
            new_trans.buflen = dvp_ctlr->fb_size_in_bytes;
            ESP_EARLY_LOGD(TAG, "no new buffer or no long enough new buffer, use driver internal buffer");
            dvp_dma_transfer_config.dst.addr = (uint32_t)dvp_ctlr->backup_buffer;
        } else {
            assert(false && "no new buffer, and no driver internal buffer");
        }
    }

    ESP_EARLY_LOGD(TAG, "new_trans.buffer: %p, new_trans.buflen: %d", new_trans.buffer, new_trans.buflen);
    dw_gdma_channel_config_transfer(chan, &dvp_dma_transfer_config);
    dw_gdma_channel_enable_ctrl(chan, true);

    if ((dvp_ctlr->trans.buffer != dvp_ctlr->backup_buffer) || dvp_ctlr->bk_buffer_exposed) {
        esp_err_t ret = esp_cache_msync((void *)(dvp_ctlr->trans.buffer), dvp_ctlr->trans.received_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
        assert(dvp_ctlr->cbs.on_trans_finished);
        if (dvp_ctlr->cbs.on_trans_finished) {
            dvp_ctlr->trans.received_size = dvp_ctlr->fb_size_in_bytes;
            need_yield |= dvp_ctlr->cbs.on_trans_finished(&(dvp_ctlr->base), &dvp_ctlr->trans, dvp_ctlr->cbs_user_data);
        }
    }

    //dvp_ctlr->trans is the transaction saved before dma starts
    memset(&dvp_ctlr->trans, 0x0, sizeof(esp_cam_ctlr_trans_t));
    dvp_ctlr->trans = new_trans;

    need_yield |= high_task_woken == pdTRUE;
    return need_yield;
}

static esp_err_t s_isp_io_init(isp_dvp_controller_t *dvp_ctlr, const esp_cam_ctlr_isp_dvp_cfg_t *ctlr_config)
{
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = false,
        .pull_up_en = true,
    };

    if (ctlr_config->pclk_io >= 0) {
        gpio_conf.pin_bit_mask = 1ULL << ctlr_config->pclk_io;
        ESP_RETURN_ON_ERROR(gpio_config(&gpio_conf), TAG, "failed to configure pclk gpio");
        ESP_LOGD(TAG, "pclk_io: %d, dvp_pclk_sig: %"PRId32, ctlr_config->pclk_io, isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_pclk_sig);
        esp_rom_gpio_connect_in_signal(ctlr_config->pclk_io, isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_pclk_sig, false);
    }

    if (ctlr_config->hsync_io >= 0) {
        gpio_conf.pin_bit_mask = 1ULL << ctlr_config->hsync_io;
        ESP_RETURN_ON_ERROR(gpio_config(&gpio_conf), TAG, "failed to configure hsync gpio");
        ESP_LOGD(TAG, "hsync_io: %d, dvp_hsync_sig: %"PRId32, ctlr_config->hsync_io, isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_hsync_sig);
        esp_rom_gpio_connect_in_signal(ctlr_config->hsync_io, isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_hsync_sig, false);
    }

    if (ctlr_config->vsync_io >= 0) {
        gpio_conf.pin_bit_mask = 1ULL << ctlr_config->vsync_io;
        ESP_RETURN_ON_ERROR(gpio_config(&gpio_conf), TAG, "failed to configure vsync gpio");
        ESP_LOGD(TAG, "vsync_io: %d, dvp_vsync_sig: %"PRId32, ctlr_config->vsync_io, isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_vsync_sig);
        esp_rom_gpio_connect_in_signal(ctlr_config->vsync_io, isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_vsync_sig, false);
    }

    if (ctlr_config->de_io >= 0) {
        gpio_conf.pin_bit_mask = 1ULL << ctlr_config->de_io;
        ESP_RETURN_ON_ERROR(gpio_config(&gpio_conf), TAG, "failed to configure de gpio");
        ESP_LOGD(TAG, "de_io: %d, dvp_de_sig: %"PRId32, ctlr_config->de_io, isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_de_sig);
        esp_rom_gpio_connect_in_signal(ctlr_config->de_io, isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_de_sig, false);
    }

    for (size_t i = 0; i < ctlr_config->data_width; i++) {
        gpio_conf.pin_bit_mask = 1ULL << ctlr_config->data_io[i];
        ESP_RETURN_ON_ERROR(gpio_config(&gpio_conf), TAG, "failed to configure data gpio");
        ESP_LOGD(TAG, "data_io: %d, dvp_data_sig: %"PRId32, ctlr_config->data_io[i], isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_data_sig[i]);
        esp_rom_gpio_connect_in_signal(ctlr_config->data_io[i], isp_hw_info.dvp_ctlr[dvp_ctlr->id].dvp_data_sig[i], false);
    }

    return ESP_OK;
}

static esp_err_t s_isp_claim_dvp_controller(isp_proc_handle_t isp_proc, isp_dvp_controller_t *dvp_ctlr)
{
    assert(isp_proc && dvp_ctlr);

    _lock_acquire(&s_ctx.mutex);
    bool found = false;
    for (int i = 0; i < SOC_ISP_DVP_CTLR_NUMS; i++) {
        found = !s_ctx.dvp_ctlr[i];
        if (found) {
            s_ctx.dvp_ctlr[i] = dvp_ctlr;
            dvp_ctlr->id = i;

            break;
        }
    }
    _lock_release(&s_ctx.mutex);

    if (!found) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

static esp_err_t s_isp_declaim_dvp_controller(isp_dvp_controller_t *dvp_ctlr)
{
    assert(dvp_ctlr);

    _lock_acquire(&s_ctx.mutex);
    s_ctx.dvp_ctlr[dvp_ctlr->id] = NULL;
    _lock_release(&s_ctx.mutex);

    return ESP_OK;
}
