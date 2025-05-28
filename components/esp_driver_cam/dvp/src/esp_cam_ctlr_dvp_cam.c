/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>
#include <sys/param.h>
#include "esp_memory_utils.h"
#include "hal/cam_ll.h"
#include "hal/color_hal.h"
#include "driver/gpio.h"
#include "esp_cache.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_clk_tree.h"
#include "soc/cam_periph.h"
#include "soc/soc_caps.h"
#include "esp_cam_ctlr_dvp_cam.h"
#include "esp_private/esp_cam_dvp.h"
#include "esp_private/esp_clk_tree_common.h"
#include "../../dvp_share_ctrl.h"

#ifdef CONFIG_CAM_CTLR_DVP_CAM_ISR_CACHE_SAFE
#define CAM_DVP_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define CAM_DVP_MEM_ALLOC_CAPS      (MALLOC_CAP_DEFAULT)
#endif

#define ALIGN_UP_BY(num, align)     (((num) + ((align) - 1)) & ~((align) - 1))

#define DVP_CAM_CONFIG_INPUT_PIN(pin, sig, inv)                 \
{                                                               \
    ret = esp_cam_ctlr_dvp_config_input_gpio(pin, sig, inv);    \
    if (ret != ESP_OK) {                                        \
        ESP_LOGE(TAG, "failed to configure pin=%d sig=%d",      \
                 pin, sig);                                     \
        return ret;                                             \
    }                                                           \
}

typedef struct dvp_platform {
    _lock_t         mutex;
    esp_cam_ctlr_dvp_cam_t *ctlrs[CAP_DVP_PERIPH_NUM];
} dvp_platform_t;

static dvp_platform_t s_platform;
static const char *TAG = "dvp_cam";

/**
 * @brief Claim DVP controller
 *
 * @param ctlr_id DVP controller ID
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t s_dvp_claim_ctlr(int ctlr_id, esp_cam_ctlr_dvp_cam_t *ctlr)
{
    esp_err_t ret = ESP_ERR_NOT_FOUND;

    _lock_acquire(&s_platform.mutex);
    if (!s_platform.ctlrs[ctlr_id]) {
        s_platform.ctlrs[ctlr_id] = ctlr;
        ret = ESP_OK;
    }
    _lock_release(&s_platform.mutex);

    return ret;
}

/**
 * @brief Declaim DVP controller
 *
 * @param ctlr_id DVP controller ID
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t s_dvp_declaim_ctlr(int ctlr_id)
{
    esp_err_t ret = ESP_ERR_NOT_FOUND;

    _lock_acquire(&s_platform.mutex);
    if (s_platform.ctlrs[ctlr_id]) {
        s_platform.ctlrs[ctlr_id] = NULL;
        ret = ESP_OK;
    }
    _lock_release(&s_platform.mutex);

    return ret;
}

/**
 * @brief CAM DVP initialzie input GPIO pin.
 *
 * @param pin    DVP pin number
 * @param signal DVP pin mapping signal
 * @param inv    true: DVP pin is inverted, false: DVP pin is not inverted
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_dvp_config_input_gpio(int pin, int signal, bool inv)
{
    esp_err_t ret;

    gpio_func_sel(pin, PIN_FUNC_GPIO);
    ret = gpio_set_direction(pin, GPIO_MODE_INPUT);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = gpio_set_pull_mode(pin, GPIO_FLOATING);
    if (ret != ESP_OK) {
        return ret;
    }
    esp_rom_gpio_connect_in_signal(pin, signal, inv);

    return ESP_OK;
}

/**
 * @brief CAM DVP start receiving frame.
 *
 * @param ctlr CAM DVP controller
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static IRAM_ATTR esp_err_t esp_cam_ctlr_dvp_start_trans(esp_cam_ctlr_dvp_cam_t *ctlr)
{
    bool buffer_ready = false;
    esp_cam_ctlr_trans_t trans;

    if (ctlr->cur_buf) {
        ctlr->cur_buf = NULL;
        cam_hal_stop_streaming(&ctlr->hal);
        ESP_RETURN_ON_ERROR_ISR(esp_cam_ctlr_dvp_dma_stop(&ctlr->dma), TAG, "failed to stop DMA");
    }

    if (ctlr->cbs.on_get_new_trans && ctlr->cbs.on_get_new_trans(&(ctlr->base), &trans, ctlr->cbs_user_data)) {
        buffer_ready = true;
    } else if (!ctlr->bk_buffer_dis) {
        trans.buffer = ctlr->backup_buffer;
        trans.buflen = ctlr->fb_size_in_bytes;
        buffer_ready = true;
    }

    if (!buffer_ready) {
        assert(false && "no new buffer, and no driver internal buffer");
    }

    ESP_RETURN_ON_ERROR_ISR(esp_cam_ctlr_dvp_dma_reset(&ctlr->dma), TAG, "failed to reset DMA");
    ESP_RETURN_ON_ERROR_ISR(esp_cam_ctlr_dvp_dma_start(&ctlr->dma, trans.buffer, ctlr->fb_size_in_bytes), TAG, "failed to start DMA");

    cam_hal_start_streaming(&ctlr->hal);

    ctlr->cur_buf = trans.buffer;

    return ESP_OK;
}

/**
 * @brief Check JPEG file and return JPEG frame actual size
 *
 * @param buffer JPEG buffer pointer
 * @param size   JPEG buffer size
 *
 * @return JPEG frame actual size if success or 0 if not JPEG header or tail TAG is found
 */
static uint32_t IRAM_ATTR esp_cam_ctlr_dvp_get_jpeg_size(const uint8_t *buffer, uint32_t size)
{
    /* Check JPEG header TAG: ff:d8 */

    if (buffer[0] != 0xff || buffer[1] != 0xd8) {
        return 0;
    }

    for (uint32_t off = size - 2; off > 2; off--) {
        /* Check JPEG tail TAG: ff:d9 */

        if (buffer[off] == 0xff && buffer[off + 1] == 0xd9) {
            return off + 2;
        }
    }

    return 0;
}

/**
 * @brief CAM DVP get actually received data size in byte
 *
 * @param ctlr                 CAM DVP controller
 * @param rx_buffer            CAM DVP receive buffer
 * @param dma_recv_size        CAM DVP DMA receive buffer size
 *
 * @return Received data size if success or 0 if failed
 */
static uint32_t IRAM_ATTR esp_cam_ctlr_dvp_get_recved_size(esp_cam_ctlr_dvp_cam_t *ctlr, uint8_t *rx_buffer, uint32_t dma_recv_size)
{
    esp_err_t ret;
    uint32_t recv_buffer_size;

    if (ctlr->pic_format_jpeg) {
        recv_buffer_size = ALIGN_UP_BY(MIN(dma_recv_size, ctlr->fb_size_in_bytes), 64);
    } else {
        recv_buffer_size = ctlr->fb_size_in_bytes;
    }

    ret = esp_cache_msync(rx_buffer, recv_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    assert(ret == ESP_OK);

    if (ctlr->pic_format_jpeg) {
        recv_buffer_size = esp_cam_ctlr_dvp_get_jpeg_size(rx_buffer, dma_recv_size);
    }

    return recv_buffer_size;
}

/**
 * @brief CAM DVP calculate frame receive buffer size.
 *
 * @param config  CAM DVP controller configurations
 * @param p_size  CAM DVP frame receive buffer size buffer pointer
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_dvp_cam_get_frame_size(const esp_cam_ctlr_dvp_config_t *config, size_t *p_size)
{
    esp_err_t ret = ESP_OK;

    if (config->pic_format_jpeg) {
        *p_size = config->h_res * config->v_res;
    } else {
        color_space_pixel_format_t pixel_format = {
            .color_type_id = config->input_data_color_type
        };
        uint32_t depth = color_hal_pixel_format_get_bit_depth(pixel_format);
        if (!depth) {
            return ESP_ERR_INVALID_ARG;
        }

        *p_size = config->h_res * config->v_res * depth / 8;
    }

    return ret;
}

/**
 * @brief CAM DVP receive frame done interrupt callback function.
 *
 * @param dma_chan   DMA channel
 * @param event_data DMA event data
 * @param user_data  DMA interrupt callback user data
 *
 * @return
 *      - true: trigger task switch
 *      - false: don't trigger task switch
 */
static IRAM_ATTR bool esp_cam_ctlr_recv_frame_done_isr(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    bool need_switch = false;
    esp_cam_ctlr_dvp_cam_t *ctlr = (esp_cam_ctlr_dvp_cam_t *)user_data;
    size_t dma_recv_size = esp_cam_ctlr_dvp_dma_get_recv_size(&ctlr->dma);
    esp_cam_ctlr_trans_t trans = {
        .buffer = ctlr->cur_buf,
    };

    esp_cam_ctlr_dvp_start_trans(ctlr);

    if ((trans.buffer != ctlr->backup_buffer) || ctlr->bk_buffer_exposed) {
        trans.buflen = ctlr->fb_size_in_bytes;
        trans.received_size = esp_cam_ctlr_dvp_get_recved_size(ctlr, trans.buffer, dma_recv_size);
        need_switch = ctlr->cbs.on_trans_finished(&ctlr->base, &trans, ctlr->cbs_user_data);
    }

    return need_switch;
}

/**
 * @brief ESP CAM DVP initialzie clock and GPIO.
 *
 * @param ctlr_id CAM DVP controller ID
 * @param clk_src CAM DVP clock source
 * @param pin     CAM DVP pin configuration
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_init(int ctlr_id, cam_clock_source_t clk_src, const esp_cam_ctlr_dvp_pin_config_t *pin)
{
    esp_err_t ret;

    ESP_RETURN_ON_FALSE(ctlr_id < CAP_DVP_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid argument: ctlr_id >= %d", CAP_DVP_PERIPH_NUM);
    ESP_RETURN_ON_FALSE(pin->data_width == CAM_CTLR_DATA_WIDTH_8, ESP_ERR_INVALID_ARG, TAG, "invalid argument: data_width != CAM_CTLR_DATA_WIDTH_8");
    ESP_RETURN_ON_FALSE(pin, ESP_ERR_INVALID_ARG, TAG, "invalid argument: pin is null");

    DVP_CAM_CONFIG_INPUT_PIN(pin->vsync_io, cam_periph_signals.buses[ctlr_id].vsync_sig, true);
    DVP_CAM_CONFIG_INPUT_PIN(pin->de_io,  cam_periph_signals.buses[ctlr_id].de_sig,  false);
    DVP_CAM_CONFIG_INPUT_PIN(pin->pclk_io,  cam_periph_signals.buses[ctlr_id].pclk_sig,  false);
    for (int i = 0; i < pin->data_width; i++) {
        DVP_CAM_CONFIG_INPUT_PIN(pin->data_io[i], cam_periph_signals.buses[ctlr_id].data_sigs[i], false);
    }

    /* If using external XTAL, don't initialize xclock pin */

    if (pin->xclk_io != GPIO_NUM_NC) {
        gpio_func_sel(pin->xclk_io, PIN_FUNC_GPIO);
        ret = gpio_set_direction(pin->xclk_io, GPIO_MODE_OUTPUT);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "failed to configure pin=%d", pin->xclk_io);
            return ret;
        }
        esp_rom_gpio_connect_out_signal(pin->xclk_io, cam_periph_signals.buses[ctlr_id].clk_sig, false, false);
    }

    PERIPH_RCC_ACQUIRE_ATOMIC(cam_periph_signals.buses[ctlr_id].module, ref_count) {
        if (ref_count == 0) {
            cam_ll_enable_bus_clock(ctlr_id, true);
            cam_ll_reset_register(ctlr_id);
        }
    }

    ESP_ERROR_CHECK(esp_clk_tree_enable_src((soc_module_clk_t)clk_src, true));
    PERIPH_RCC_ATOMIC() {
        cam_ll_enable_clk(ctlr_id, true);
        cam_ll_select_clk_src(ctlr_id, clk_src);
    };

    return ESP_OK;
}

/**
 * @brief ESP CAM DVP output hardware clock
 *
 * @param ctlr_id   CAM DVP controller ID
 * @param clk_src   CAM DVP clock source
 * @param xclk_freq CAM DVP output clock frequency in HZ
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_output_clock(int ctlr_id, cam_clock_source_t clk_src, uint32_t xclk_freq)
{
    esp_err_t ret = ESP_ERR_INVALID_ARG;
    uint32_t src_clk_hz;

    ESP_RETURN_ON_FALSE(ctlr_id < CAP_DVP_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid argument: ctlr_id >= %d", CAP_DVP_PERIPH_NUM);
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &src_clk_hz),
                        TAG, "failed to get clock source frequency");

    ESP_LOGD(TAG, "DVP clock source frequency %" PRIu32 "Hz", src_clk_hz);

    if ((src_clk_hz % xclk_freq) == 0) {
        PERIPH_RCC_ATOMIC() {
            cam_ll_set_group_clock_coeff(ctlr_id, src_clk_hz / xclk_freq, 0, 0);
        };

        ret = ESP_OK;
    }

    return ret;
}

/**
 * @brief ESP CAM DVP de-initialzie.
 *
 * @param ctlr_id DVP controller ID
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_deinit(int ctlr_id)
{
    ESP_RETURN_ON_FALSE(ctlr_id < CAP_DVP_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid argument: ctlr_id >= %d", CAP_DVP_PERIPH_NUM);

    PERIPH_RCC_ATOMIC() {
        cam_ll_enable_clk(ctlr_id, false);
    };

    PERIPH_RCC_ACQUIRE_ATOMIC(cam_periph_signals.buses[ctlr_id].module, ref_count) {
        if (ref_count == 0) {
            cam_ll_reset_register(ctlr_id);
            cam_ll_enable_bus_clock(ctlr_id, false);
        }
    }

    return ESP_OK;
}

/**
 * @brief Enable CAM DVP camera controller
 *
 * @param handle ESP CAM controller handle
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_dvp_cam_enable(esp_cam_ctlr_handle_t handle)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    esp_cam_ctlr_dvp_cam_t *ctlr = (esp_cam_ctlr_dvp_cam_t *)handle;

    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: handle is null");

    portENTER_CRITICAL(&ctlr->spinlock);
    if (ctlr->dvp_fsm == ESP_CAM_CTLR_DVP_CAM_FSM_INIT) {
        ctlr->dvp_fsm = ESP_CAM_CTLR_DVP_CAM_FSM_ENABLED;
        ret = ESP_OK;
    }
#if CONFIG_PM_ENABLE
    if (ctlr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(ctlr->pm_lock), TAG, "acquire pm_lock failed");
    }
#endif // CONFIG_PM_ENABLE

    portEXIT_CRITICAL(&ctlr->spinlock);

    return ret;
}

/**
 * @brief Disable CAM DVP camera controller
 *
 * @param handle ESP CAM controller handle
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_dvp_cam_disable(esp_cam_ctlr_handle_t handle)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    esp_cam_ctlr_dvp_cam_t *ctlr = (esp_cam_ctlr_dvp_cam_t *)handle;

    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: handle is null");

    portENTER_CRITICAL(&ctlr->spinlock);
    if (ctlr->dvp_fsm == ESP_CAM_CTLR_DVP_CAM_FSM_ENABLED) {
        ctlr->dvp_fsm = ESP_CAM_CTLR_DVP_CAM_FSM_INIT;
        ret = ESP_OK;
    }
#if CONFIG_PM_ENABLE
    if (ctlr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(ctlr->pm_lock), TAG, "release pm_lock failed");
    }
#endif // CONFIG_PM_ENABLE
    portEXIT_CRITICAL(&ctlr->spinlock);

    return ret;
}

/**
 * @brief Start CAM DVP camera controller
 *
 * @param handle ESP CAM controller handle
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_dvp_cam_start(esp_cam_ctlr_handle_t handle)
{
    bool start = false;
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    esp_cam_ctlr_dvp_cam_t *ctlr = (esp_cam_ctlr_dvp_cam_t *)handle;

    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: handle is null");

    portENTER_CRITICAL(&ctlr->spinlock);
    if (ctlr->dvp_fsm == ESP_CAM_CTLR_DVP_CAM_FSM_ENABLED) {
        ctlr->cur_buf = NULL;
        ctlr->dvp_fsm = ESP_CAM_CTLR_DVP_CAM_FSM_STARTED;
        start = true;
    }
    portEXIT_CRITICAL(&ctlr->spinlock);

    if (start) {
        ret = esp_cam_ctlr_dvp_start_trans(ctlr);
    }

    return ret;
}

/**
 * @brief Stop CAM DVP camera controller
 *
 * @param handle ESP CAM controller handle
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_dvp_cam_stop(esp_cam_ctlr_handle_t handle)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    esp_cam_ctlr_dvp_cam_t *ctlr = (esp_cam_ctlr_dvp_cam_t *)handle;

    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: handle is null");

    portENTER_CRITICAL(&ctlr->spinlock);
    if (ctlr->dvp_fsm == ESP_CAM_CTLR_DVP_CAM_FSM_STARTED) {
        cam_hal_stop_streaming(&ctlr->hal);
        esp_cam_ctlr_dvp_dma_stop(&ctlr->dma);
        ctlr->dvp_fsm = ESP_CAM_CTLR_DVP_CAM_FSM_ENABLED;
        ret = ESP_OK;
    }
    portEXIT_CRITICAL(&ctlr->spinlock);

    return ret;
}

/**
 * @brief Delete CAM DVP camera controller
 *
 * @param handle ESP CAM controller handle
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_dvp_cam_del(esp_cam_ctlr_handle_t handle)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    esp_cam_ctlr_dvp_cam_t *ctlr = (esp_cam_ctlr_dvp_cam_t *)handle;

    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: handle is null");

    if (ctlr->dvp_fsm == ESP_CAM_CTLR_DVP_CAM_FSM_INIT) {
        esp_cam_ctlr_dvp_dma_stop(&ctlr->dma);
        cam_hal_stop_streaming(&ctlr->hal);

        esp_cam_ctlr_dvp_dma_deinit(&ctlr->dma);

        if (!ctlr->pin_dont_init) {
            esp_cam_ctlr_dvp_deinit(ctlr->ctlr_id);
        }

        cam_hal_deinit(&ctlr->hal);

        if (!ctlr->bk_buffer_dis) {
            heap_caps_free(ctlr->backup_buffer);
        }

#if CONFIG_PM_ENABLE
        if (ctlr->pm_lock) {
            ESP_RETURN_ON_ERROR(esp_pm_lock_delete(ctlr->pm_lock), TAG, "delete pm_lock failed");
        }
#endif // CONFIG_PM_ENABLE

        s_dvp_declaim_ctlr(ctlr->ctlr_id);

        heap_caps_free(ctlr);

        dvp_shared_ctrl_declaim_io_signals();

        ret = ESP_OK;
    }

    return ret;
}

/**
 * @brief Register CAM DVP camera controller event callbacks
 *
 * @param handle        ESP CAM controller handle
 * @param cbs           ESP CAM controller event callbacks
 * @param user_data     ESP CAM controller event user data
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_dvp_cam_register_event_callbacks(esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_data)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    esp_cam_ctlr_dvp_cam_t *ctlr = (esp_cam_ctlr_dvp_cam_t *)handle;

    ESP_RETURN_ON_FALSE(handle && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument: handle or cbs is null");
    ESP_RETURN_ON_FALSE(cbs->on_trans_finished, ESP_ERR_INVALID_ARG, TAG, "invalid argument: on_trans_finished is null");
    ESP_RETURN_ON_FALSE(cbs->on_get_new_trans || !ctlr->bk_buffer_dis, ESP_ERR_INVALID_ARG, TAG, "invalid argument: on_get_new_trans is null");

#if CONFIG_CAM_CTLR_DVP_CAM_ISR_CACHE_SAFE
    if (cbs->on_get_new_trans) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_get_new_trans), ESP_ERR_INVALID_ARG, TAG, "on_get_new_trans callback not in IRAM");
    }
    if (cbs->on_trans_finished) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_finished), ESP_ERR_INVALID_ARG, TAG, "on_trans_finished callback not in IRAM");
    }
#endif

    portENTER_CRITICAL(&ctlr->spinlock);
    if (ctlr->dvp_fsm == ESP_CAM_CTLR_DVP_CAM_FSM_INIT) {
        ctlr->cbs.on_get_new_trans = cbs->on_get_new_trans;
        ctlr->cbs.on_trans_finished = cbs->on_trans_finished;
        ctlr->cbs_user_data = user_data;
        ret = ESP_OK;
    }
    portEXIT_CRITICAL(&ctlr->spinlock);

    return ret;
}

/**
 * @brief Get AM DVP camera controller backup buffer pointer
 *
 * @param handle        ESP CAM controller handle
 * @param fb_num        Backup buffer pointer storage buffer number
 * @param fb0           Backup buffer pointer storage buffer
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_dvp_cam_get_internal_buffer(esp_cam_ctlr_handle_t handle, uint32_t fb_num, const void **fb0, ...)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    esp_cam_ctlr_dvp_cam_t *ctlr = (esp_cam_ctlr_dvp_cam_t *)handle;

    ESP_RETURN_ON_FALSE(ctlr->backup_buffer, ESP_ERR_INVALID_STATE, TAG, "back_buffer is not available");
    ESP_RETURN_ON_FALSE(fb_num && fb_num <= 1, ESP_ERR_INVALID_ARG, TAG, "invalid frame buffer number");

    portENTER_CRITICAL(&ctlr->spinlock);
    if (ctlr->dvp_fsm == ESP_CAM_CTLR_DVP_CAM_FSM_INIT) {
        va_list args;
        const void **fb_itor = fb0;

        va_start(args, fb0);
        for (uint32_t i = 0; i < fb_num; i++) {
            if (fb_itor) {
                *fb_itor = ctlr->backup_buffer;
                fb_itor = va_arg(args, const void **);
            }
        }
        va_end(args);

        ctlr->bk_buffer_exposed = true;
        ret = ESP_OK;
    }
    portEXIT_CRITICAL(&ctlr->spinlock);

    return ret;
}

/**
 * @brief Get CAM DVP camera controller frame buffer length
 *
 * @param handle        ESP CAM controller handle
 * @param ret_fb_len    The size of each frame buffer in bytes
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static esp_err_t esp_cam_ctlr_get_dvp_cam_frame_buffer_len(esp_cam_ctlr_handle_t handle, size_t *ret_fb_len)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    esp_cam_ctlr_dvp_cam_t *ctlr = (esp_cam_ctlr_dvp_cam_t *)handle;

    ESP_RETURN_ON_FALSE(ctlr && ret_fb_len, ESP_ERR_INVALID_ARG, TAG, "invalid argument: handle or ret_fb_len is null");

    portENTER_CRITICAL(&ctlr->spinlock);
    if (ctlr->dvp_fsm == ESP_CAM_CTLR_DVP_CAM_FSM_INIT) {
        *ret_fb_len = ctlr->fb_size_in_bytes;
        ret = ESP_OK;
    }
    portEXIT_CRITICAL(&ctlr->spinlock);

    return ret;
}

/**
 * @brief New ESP CAM DVP controller
 *
 * @param config      DVP controller configurations
 * @param ret_handle  Returned CAM controller handle
 *
 * @return
 *        - ESP_OK on success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_NO_MEM:        Out of memory
 *        - ESP_ERR_NOT_SUPPORTED: Currently not support modes or types
 *        - ESP_ERR_NOT_FOUND:     CSI is registered already
 */
esp_err_t esp_cam_new_dvp_ctlr(const esp_cam_ctlr_dvp_config_t *config, esp_cam_ctlr_handle_t *ret_handle)
{
    esp_err_t ret;
    size_t fb_size_in_bytes;
    size_t alignment_size;

    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: config or ret_handle is null");
    ESP_RETURN_ON_FALSE(config->ctlr_id < CAP_DVP_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid argument: ctlr_id >= %d", CAP_DVP_PERIPH_NUM);
    ESP_RETURN_ON_FALSE(config->pin_dont_init || config->pin, ESP_ERR_INVALID_ARG, TAG, "invalid argument: pin_dont_init is unset and pin is null");
    ESP_RETURN_ON_ERROR(esp_cache_get_alignment(MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA, &alignment_size), TAG, "failed to get cache alignment");
    ESP_RETURN_ON_ERROR(esp_cam_ctlr_dvp_cam_get_frame_size(config, &fb_size_in_bytes), TAG, "invalid argument: input frame pixel format is not supported");
    ESP_RETURN_ON_ERROR(dvp_shared_ctrl_claim_io_signals(), TAG, "failed to claim io signals");

    esp_cam_ctlr_dvp_cam_t *ctlr = heap_caps_calloc(1, sizeof(esp_cam_ctlr_dvp_cam_t), CAM_DVP_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(ctlr, ESP_ERR_NO_MEM, fail0, TAG, "no mem for CAM DVP controller context");

    ESP_GOTO_ON_ERROR(s_dvp_claim_ctlr(config->ctlr_id, ctlr), fail1, TAG, "no available DVP controller");

    ESP_LOGD(TAG, "alignment: 0x%x\n", alignment_size);
    fb_size_in_bytes = ALIGN_UP_BY(fb_size_in_bytes, alignment_size);
    if (!config->bk_buffer_dis) {
        ctlr->backup_buffer = heap_caps_aligned_alloc(alignment_size, fb_size_in_bytes, MALLOC_CAP_SPIRAM);
        ESP_GOTO_ON_FALSE(ctlr->backup_buffer, ESP_ERR_NO_MEM, fail2, TAG, "no mem for DVP backup buffer");
    }

    ESP_GOTO_ON_ERROR(esp_cam_ctlr_dvp_dma_init(&ctlr->dma, config->dma_burst_size, fb_size_in_bytes),
                      fail3, TAG, "failed to initialize DVP DMA");

    gdma_rx_event_callbacks_t cbs = {
        .on_recv_eof = esp_cam_ctlr_recv_frame_done_isr
    };

    ESP_GOTO_ON_ERROR(gdma_register_rx_event_callbacks(ctlr->dma.dma_chan, &cbs, ctlr),
                      fail4, TAG, "failed to register DMA event callbacks");

#if CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "dvp_cam_ctlr", &ctlr->pm_lock), fail5, TAG, "failed to create pm lock");
#endif //CONFIG_PM_ENABLE

    /* Initialize DVP controller */

    cam_hal_config_t cam_hal_config = {
        .port = config->ctlr_id,
        .byte_swap_en = config->byte_swap_en,
    };
    cam_hal_init(&ctlr->hal, &cam_hal_config);

    if (!config->pin_dont_init) {
        ESP_GOTO_ON_ERROR(esp_cam_ctlr_dvp_init(config->ctlr_id, config->clk_src, config->pin),
                          fail5, TAG, "failed to initialize clock and GPIO");
    }

    ctlr->ctlr_id = config->ctlr_id;
    ctlr->fb_size_in_bytes = fb_size_in_bytes;
    ctlr->dvp_fsm = ESP_CAM_CTLR_DVP_CAM_FSM_INIT;
    ctlr->input_data_color_type = config->input_data_color_type;
    ctlr->pic_format_jpeg = config->pic_format_jpeg;
    ctlr->bk_buffer_dis = config->bk_buffer_dis;
    ctlr->pin_dont_init = config->pin_dont_init;
    ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;

    ctlr->base.del = esp_cam_ctlr_dvp_cam_del;
    ctlr->base.enable = esp_cam_ctlr_dvp_cam_enable;
    ctlr->base.start = esp_cam_ctlr_dvp_cam_start;
    ctlr->base.stop = esp_cam_ctlr_dvp_cam_stop;
    ctlr->base.disable = esp_cam_ctlr_dvp_cam_disable;
    ctlr->base.register_event_callbacks = esp_cam_ctlr_dvp_cam_register_event_callbacks;
    ctlr->base.get_internal_buffer = esp_cam_ctlr_dvp_cam_get_internal_buffer;
    ctlr->base.get_buffer_len = esp_cam_ctlr_get_dvp_cam_frame_buffer_len;

    *ret_handle = &ctlr->base;

    return ESP_OK;

fail5:
    cam_hal_deinit(&ctlr->hal);
fail4:
    esp_cam_ctlr_dvp_dma_deinit(&ctlr->dma);
fail3:
    if (!config->bk_buffer_dis) {
        heap_caps_free(ctlr->backup_buffer);
    }
fail2:
    s_dvp_declaim_ctlr(config->ctlr_id);
fail1:
    heap_caps_free(ctlr);
fail0:
    dvp_shared_ctrl_declaim_io_signals();
    return ret;
}
