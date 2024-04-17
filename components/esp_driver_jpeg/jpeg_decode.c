/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include "jpeg_private.h"
#include "private/jpeg_parse_marker.h"
#include "private/jpeg_param.h"
#include "driver/jpeg_decode.h"
#include "esp_heap_caps.h"
#include "esp_private/dma2d.h"
#include "hal/jpeg_ll.h"
#include "hal/cache_ll.h"
#include "hal/cache_hal.h"
#include "hal/jpeg_defs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "soc/interrupts.h"
#include "soc/dma2d_channel.h"
#include "soc/interrupts.h"
#include "esp_dma_utils.h"
#include "esp_private/esp_cache_private.h"
#if CONFIG_JPEG_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_cache.h"
#include "esp_check.h"

static const char *TAG = "jpeg.decoder";

static void s_decoder_error_log_print(uint32_t status);
static esp_err_t jpeg_dec_config_dma_descriptor(jpeg_decoder_handle_t decoder_engine);
static esp_err_t jpeg_parse_marker(jpeg_decoder_handle_t decoder_engine, const uint8_t *in_buf, uint32_t inbuf_len);
static esp_err_t jpeg_parse_header_info_to_hw(jpeg_decoder_handle_t decoder_engine);
static bool jpeg_dec_transaction_on_picked(uint32_t channel_num, const dma2d_trans_channel_info_t *dma2d_chans, void *users_config);

static void jpeg_decoder_isr_handle_default(void *arg)
{
    jpeg_decoder_handle_t decoder_engine = (jpeg_decoder_handle_t) arg;
    jpeg_hal_context_t *hal = &decoder_engine->codec_base->hal;
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    jpeg_dma2d_dec_evt_t dec_evt = {
        .jpgd_status = 0,
        .dma_evt = 0,
    };
    uint32_t value = jpeg_ll_get_intr_status(hal->dev);
    jpeg_ll_clear_intr_mask(hal->dev, value);
    dec_evt.jpgd_status = value;
    xQueueSendFromISR(decoder_engine->evt_queue, &dec_evt, &HPTaskAwoken);

    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t jpeg_new_decoder_engine(const jpeg_decode_engine_cfg_t *dec_eng_cfg, jpeg_decoder_handle_t *ret_decoder)
{
#if CONFIG_JPEG_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    jpeg_decoder_handle_t decoder_engine = NULL;
    ESP_RETURN_ON_FALSE(dec_eng_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    decoder_engine = (jpeg_decoder_handle_t)heap_caps_calloc(1, sizeof(jpeg_decoder_t), JPEG_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(decoder_engine, ESP_ERR_NO_MEM, TAG, "no memory for jpeg decode");

    uint32_t cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    uint32_t alignment = cache_line_size;
    size_t dma_desc_mem_size = JPEG_ALIGN_UP(sizeof(dma2d_descriptor_t), cache_line_size);

    decoder_engine->rxlink = (dma2d_descriptor_t*)heap_caps_aligned_calloc(alignment, 1, dma_desc_mem_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(decoder_engine->rxlink, ESP_ERR_NO_MEM, err, TAG, "no memory for jpeg decode rxlink");
    decoder_engine->txlink = (dma2d_descriptor_t*)heap_caps_aligned_calloc(alignment, 1, dma_desc_mem_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(decoder_engine->txlink, ESP_ERR_NO_MEM, err, TAG, "no memory for jpeg decode txlink");
    decoder_engine->dma_desc_size = dma_desc_mem_size;

    decoder_engine->header_info = (jpeg_dec_header_info_t*)heap_caps_calloc(1, sizeof(jpeg_dec_header_info_t), JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(decoder_engine->header_info, ESP_ERR_NO_MEM, err, TAG, "no memory for picture info");

    ESP_GOTO_ON_ERROR(jpeg_acquire_codec_handle(&decoder_engine->codec_base), err, TAG, "JPEG decode acquires codec handle failed");
    jpeg_hal_context_t *hal = &decoder_engine->codec_base->hal;

    decoder_engine->timeout_tick = (dec_eng_cfg->timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(dec_eng_cfg->timeout_ms);
    /// init jpeg interrupt.
    jpeg_ll_clear_intr_mask(hal->dev, JPEG_LL_DECODER_EVENT_INTR);

    ESP_GOTO_ON_ERROR(jpeg_check_intr_priority(decoder_engine->codec_base, dec_eng_cfg->intr_priority), err, TAG, "set group interrupt priority failed");
    if (dec_eng_cfg->intr_priority) {
        ESP_RETURN_ON_FALSE(1 << (dec_eng_cfg->intr_priority) & JPEG_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, TAG, "invalid interrupt priority:%d", dec_eng_cfg->intr_priority);
    }
    int isr_flags = JPEG_INTR_ALLOC_FLAG;
    if (dec_eng_cfg->intr_priority) {
        isr_flags |= 1 << (dec_eng_cfg->intr_priority);
    }

    ret = jpeg_isr_register(decoder_engine->codec_base, jpeg_decoder_isr_handle_default, decoder_engine, JPEG_LL_DECODER_EVENT_INTR, isr_flags, &decoder_engine->intr_handle);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install jpeg decode interrupt failed");

    jpeg_ll_enable_intr_mask(hal->dev, JPEG_LL_DECODER_EVENT_INTR);

    // Initialize queue
    decoder_engine->evt_queue = xQueueCreateWithCaps(2, sizeof(jpeg_dma2d_dec_evt_t), JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(decoder_engine->evt_queue, ESP_ERR_NO_MEM, err, TAG, "No memory for event queue");

    dma2d_pool_config_t dma2d_client_config = {
        .pool_id = 0,
    };

    ESP_GOTO_ON_ERROR(dma2d_acquire_pool(&dma2d_client_config, &decoder_engine->dma2d_group_handle), err, TAG, "dma2d client acquire failed");

    decoder_engine->trans_desc = (dma2d_trans_t *)heap_caps_calloc(1, SIZEOF_DMA2D_TRANS_T, JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(decoder_engine->trans_desc, ESP_ERR_NO_MEM, err, TAG, "No memory for dma2d descriptor");

    *ret_decoder = decoder_engine;
    return ESP_OK;

err:
    if (decoder_engine) {
        jpeg_del_decoder_engine(decoder_engine);
    }
    return ret;
}

esp_err_t jpeg_decoder_get_info(const uint8_t *in_buf, uint32_t inbuf_len, jpeg_decode_picture_info_t *picture_info)
{
    ESP_RETURN_ON_FALSE(in_buf, ESP_ERR_INVALID_ARG, TAG, "jpeg decode input buffer is NULL");
    ESP_RETURN_ON_FALSE(inbuf_len != 0, ESP_ERR_INVALID_ARG, TAG, "jpeg decode input buffer length is 0");

    jpeg_dec_header_info_t* header_info = (jpeg_dec_header_info_t*)heap_caps_calloc(1, sizeof(jpeg_dec_header_info_t), JPEG_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(header_info, ESP_ERR_NO_MEM, TAG, "no memory for picture info");
    header_info->buffer_offset = (uint8_t *)in_buf;
    header_info->buffer_left = inbuf_len;
    header_info->header_size = 0;
    uint16_t height = 0;
    uint16_t width = 0;
    uint8_t thischar = 0;
    uint8_t lastchar = 0;
    uint8_t hivi = 0;

    while (header_info->buffer_left) {
        lastchar = thischar;
        thischar = jpeg_get_bytes(header_info, 1);
        uint16_t marker = (lastchar << 8 | thischar);
        switch (marker) {
        case JPEG_M_SOF0:
            jpeg_get_bytes(header_info, 2);
            jpeg_get_bytes(header_info, 1);
            height = jpeg_get_bytes(header_info, 2);
            width = jpeg_get_bytes(header_info, 2);

            jpeg_get_bytes(header_info, 1);
            jpeg_get_bytes(header_info, 1);
            hivi = jpeg_get_bytes(header_info, 1);
            break;
        }
        // This function only used for get width and height. So only read SOF marker is enough.
        // Can be extended if picture information is extended.
        if (marker == JPEG_M_SOF0) {
            break;
        }
    }

    picture_info->height = height;
    picture_info->width = width;

    switch (hivi) {
    case 0x11:
        picture_info->sample_method = JPEG_DOWN_SAMPLING_YUV444;
        break;
    case 0x21:
        picture_info->sample_method = JPEG_DOWN_SAMPLING_YUV422;
        break;
    case 0x22:
        picture_info->sample_method = JPEG_DOWN_SAMPLING_YUV420;
        break;
    default:
        ESP_LOGE(TAG, "Sampling factor cannot be recognized");
        return ESP_ERR_INVALID_STATE;
    }

    free(header_info);
    return ESP_OK;
}

esp_err_t jpeg_decoder_process(jpeg_decoder_handle_t decoder_engine, const jpeg_decode_cfg_t *decode_cfg, const uint8_t *bit_stream, uint32_t stream_size, uint8_t *decode_outbuf, uint32_t outbuf_size, uint32_t *out_size)
{
    ESP_RETURN_ON_FALSE(decoder_engine, ESP_ERR_INVALID_ARG, TAG, "jpeg decode handle is null");
    ESP_RETURN_ON_FALSE(decode_cfg, ESP_ERR_INVALID_ARG, TAG, "jpeg decode config is null");
    ESP_RETURN_ON_FALSE(decode_outbuf, ESP_ERR_INVALID_ARG, TAG, "jpeg decode picture buffer is null");
    esp_dma_mem_info_t dma_mem_info = {
        .dma_alignment_bytes = 4,
    };
    //TODO: IDF-9637
    ESP_RETURN_ON_FALSE(esp_dma_is_buffer_alignment_satisfied(decode_outbuf, outbuf_size, dma_mem_info), ESP_ERR_INVALID_ARG, TAG, "jpeg decode decode_outbuf or out_buffer size is not aligned, please use jpeg_alloc_decoder_mem to malloc your buffer");

    esp_err_t ret = ESP_OK;

    if (decoder_engine->codec_base->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(decoder_engine->codec_base->pm_lock), TAG, "acquire pm_lock failed");
    }

    xSemaphoreTake(decoder_engine->codec_base->codec_mutex, portMAX_DELAY);
    /* Reset queue */
    xQueueReset(decoder_engine->evt_queue);

    decoder_engine->output_format = decode_cfg->output_format;
    decoder_engine->rgb_order = decode_cfg->rgb_order;
    decoder_engine->conv_std = decode_cfg->conv_std;
    decoder_engine->decoded_buf = decode_outbuf;

    ESP_GOTO_ON_ERROR(jpeg_parse_marker(decoder_engine, bit_stream, stream_size), err, TAG, "jpeg parse marker failed");
    ESP_GOTO_ON_ERROR(jpeg_parse_header_info_to_hw(decoder_engine), err, TAG, "write header info to hw failed");
    ESP_GOTO_ON_ERROR(jpeg_dec_config_dma_descriptor(decoder_engine), err, TAG, "config dma descriptor failed");

    *out_size = decoder_engine->header_info->process_h * decoder_engine->header_info->process_v * decoder_engine->bit_per_pixel / 8;
    ESP_GOTO_ON_FALSE((*out_size <= outbuf_size), ESP_ERR_INVALID_ARG, err, TAG, "Given buffer size % " PRId32 " is smaller than actual jpeg decode output size % " PRId32 "the height and width of output picture size will be adjusted to 16 bytes aligned automatically", outbuf_size, *out_size);

    dma2d_trans_config_t trans_desc = {
        .tx_channel_num = 1,
        .rx_channel_num = 1,
        .channel_flags = DMA2D_CHANNEL_FUNCTION_FLAG_RX_REORDER,
        .user_config = decoder_engine,
        .on_job_picked = jpeg_dec_transaction_on_picked,
    };

    // Before 2DDMA starts. sync buffer from cache to psram
    ret = esp_cache_msync((void*)decoder_engine->header_info->buffer_offset, decoder_engine->header_info->buffer_left, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    assert(ret == ESP_OK);

    ESP_GOTO_ON_ERROR(dma2d_enqueue(decoder_engine->dma2d_group_handle, &trans_desc, decoder_engine->trans_desc), err, TAG, "enqueue dma2d failed");
    bool need_yield;
    // Blocking for JPEG decode transaction finishes.
    while (1) {
        jpeg_dma2d_dec_evt_t jpeg_dma2d_event;
        BaseType_t ret_val = xQueueReceive(decoder_engine->evt_queue, &jpeg_dma2d_event, decoder_engine->timeout_tick);
        ESP_GOTO_ON_FALSE(ret_val == pdTRUE, ESP_ERR_TIMEOUT, err, TAG, "jpeg-dma2d handle jpeg decode timeout, please check `timeout_ms` ");

        // Dealing with JPEG event
        if (jpeg_dma2d_event.jpgd_status != 0) {
            uint32_t status = jpeg_dma2d_event.jpgd_status;
            s_decoder_error_log_print(status);
            dma2d_force_end(decoder_engine->trans_desc, &need_yield);
            xSemaphoreGive(decoder_engine->codec_base->codec_mutex);
            return ESP_ERR_INVALID_STATE;
        }

        if (jpeg_dma2d_event.dma_evt & JPEG_DMA2D_RX_EOF) {
            ret = esp_cache_msync((void*)decoder_engine->decoded_buf, outbuf_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
            assert(ret == ESP_OK);
            break;
        }
    }

err:
    xSemaphoreGive(decoder_engine->codec_base->codec_mutex);
    if (decoder_engine->codec_base->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(decoder_engine->codec_base->pm_lock), TAG, "release pm_lock failed");
    }
    return ret;
}

esp_err_t jpeg_del_decoder_engine(jpeg_decoder_handle_t decoder_engine)
{
    ESP_RETURN_ON_FALSE(decoder_engine, ESP_ERR_INVALID_ARG, TAG, "jpeg decode handle is null");
    ESP_RETURN_ON_ERROR(jpeg_release_codec_handle(decoder_engine->codec_base), TAG, "release codec failed");

    if (decoder_engine) {
        if (decoder_engine->rxlink) {
            free(decoder_engine->rxlink);
        }
        if (decoder_engine->txlink) {
            free(decoder_engine->txlink);
        }
        if (decoder_engine->header_info) {
            free(decoder_engine->header_info);
        }
        if (decoder_engine->trans_desc) {
            free(decoder_engine->trans_desc);
        }
        if (decoder_engine->evt_queue) {
            vQueueDeleteWithCaps(decoder_engine->evt_queue);
        }
        if (decoder_engine->dma2d_group_handle) {
            dma2d_release_pool(decoder_engine->dma2d_group_handle);
        }
        if (decoder_engine->intr_handle) {
            jpeg_isr_deregister(decoder_engine->codec_base, decoder_engine->intr_handle);
        }
        free(decoder_engine);
    }
    return ESP_OK;
}

void *jpeg_alloc_decoder_mem(size_t size, const jpeg_decode_memory_alloc_cfg_t *mem_cfg, size_t *allocated_size)
{
    /*
       Principle of buffer align.
       For output buffer(for decoder is 2DDMA write to PSRAM), both address and size should be aligned according to cache invalidate.
       FOr input buffer(for decoder is PSRAM write to 2DDMA), no restriction for any align (both cache writeback and requirement from 2DDMA).
    */
    size_t cache_align = 0;
    esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &cache_align);
    if (mem_cfg->buffer_direction == JPEG_DEC_ALLOC_OUTPUT_BUFFER) {
        size = JPEG_ALIGN_UP(size, cache_align);
        *allocated_size = size;
        return heap_caps_aligned_calloc(cache_align, 1, size, MALLOC_CAP_SPIRAM);
    } else {
        *allocated_size = size;
        return heap_caps_calloc(1, size, MALLOC_CAP_SPIRAM);
    }
}

/****************************************************************
 * DMA related functions
 ****************************************************************/

static void cfg_desc(jpeg_decoder_handle_t decoder_engine, dma2d_descriptor_t *dsc, uint8_t en_2d, uint8_t mode, uint16_t vb, uint16_t hb, uint8_t eof, uint32_t pbyte, uint8_t owner, uint16_t va, uint16_t ha, uint8_t *buf, dma2d_descriptor_t *next_dsc)
{
    dsc->dma2d_en   = en_2d;
    dsc->mode       = mode;
    dsc->vb_size    = vb;
    dsc->hb_length  = hb;
    dsc->pbyte      = pbyte;
    dsc->suc_eof    = eof;
    dsc->owner      = owner;
    dsc->va_size    = va;
    dsc->ha_length  = ha;
    dsc->buffer     = buf;
    dsc->next       = next_dsc;
    esp_err_t ret = esp_cache_msync((void*)dsc, decoder_engine->dma_desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE);
    assert(ret == ESP_OK);
}

static esp_err_t jpeg_dec_config_dma_descriptor(jpeg_decoder_handle_t decoder_engine)
{
    ESP_LOGD(TAG, "Config 2DDMA parameter start");

    jpeg_dec_format_hb_t best_hb_idx = 0;
    color_space_pixel_format_t picture_format;
    picture_format.color_type_id = decoder_engine->output_format;
    decoder_engine->bit_per_pixel = color_hal_pixel_format_get_bit_depth(picture_format);
    if (decoder_engine->no_color_conversion == false) {
        switch (decoder_engine->output_format) {
        case JPEG_DECODE_OUT_FORMAT_RGB888:
            best_hb_idx = JPEG_DEC_RGB888_HB;
            break;
        case JPEG_DECODE_OUT_FORMAT_RGB565:
            best_hb_idx = JPEG_DEC_RGB565_HB;
            break;
        case JPEG_DECODE_OUT_FORMAT_GRAY:
            best_hb_idx = JPEG_DEC_GRAY_HB;
            break;
        case JPEG_DECODE_OUT_FORMAT_YUV444:
            best_hb_idx = JPEG_DEC_YUV444_HB;
            break;
        default:
            ESP_LOGE(TAG, "wrong, we don't support decode to such format.");
            return ESP_ERR_NOT_SUPPORTED;
        }
    } else {
        best_hb_idx = JPEG_DEC_DIRECT_OUTPUT_HB;
    }

    uint8_t sample_method_idx = 0;
    switch (decoder_engine->sample_method) {
    case JPEG_DOWN_SAMPLING_YUV444:
        sample_method_idx = 0;
        break;
    case JPEG_DOWN_SAMPLING_YUV422:
        sample_method_idx = 1;
        break;
    case JPEG_DOWN_SAMPLING_YUV420:
        sample_method_idx = 2;
        break;
    case JPEG_DOWN_SAMPLING_GRAY:
        sample_method_idx = 3;
        break;
    default:
        ESP_LOGE(TAG, "wrong, we don't support such sampling mode.");
        return ESP_ERR_NOT_SUPPORTED;
    }

    uint32_t dma_hb = dec_hb_tbl[sample_method_idx][best_hb_idx];
    uint32_t dma_vb = decoder_engine->header_info->mcuy;

    // Configure tx link descriptor
    cfg_desc(decoder_engine, decoder_engine->txlink, JPEG_DMA2D_2D_DISABLE, DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, decoder_engine->header_info->buffer_left & JPEG_DMA2D_MAX_SIZE, decoder_engine->header_info->buffer_left & JPEG_DMA2D_MAX_SIZE, JPEG_DMA2D_EOF_NOT_LAST, 1, DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA, (decoder_engine->header_info->buffer_left >> JPEG_DMA2D_1D_HIGH_14BIT), (decoder_engine->header_info->buffer_left >> JPEG_DMA2D_1D_HIGH_14BIT), decoder_engine->header_info->buffer_offset, NULL);

    // Configure rx link descriptor
    cfg_desc(decoder_engine, decoder_engine->rxlink, JPEG_DMA2D_2D_ENABLE, DMA2D_DESCRIPTOR_BLOCK_RW_MODE_MULTIPLE, dma_vb, dma_hb, JPEG_DMA2D_EOF_NOT_LAST, dma2d_desc_pixel_format_to_pbyte_value(picture_format), DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA, decoder_engine->header_info->process_v, decoder_engine->header_info->process_h, decoder_engine->decoded_buf, NULL);

    return ESP_OK;
}

static bool jpeg_rx_eof(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data)
{
    portBASE_TYPE higher_priority_task_awoken = pdFALSE;
    jpeg_dma2d_dec_evt_t dec_evt = {
        .jpgd_status = 0,
        .dma_evt = 0,
    };
    jpeg_decoder_handle_t decoder_engine = (jpeg_decoder_handle_t) user_data;
    dec_evt.dma_evt = JPEG_DMA2D_RX_EOF;
    xQueueSendFromISR(decoder_engine->evt_queue, &dec_evt, &higher_priority_task_awoken);

    return higher_priority_task_awoken;
}

static void jpeg_dec_config_dma_csc(jpeg_decoder_handle_t decoder_engine, dma2d_channel_handle_t rx_chan)
{

    dma2d_scramble_order_t post_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0;
    dma2d_csc_rx_option_t rx_csc_option = DMA2D_CSC_RX_NONE;
    // Config output Endians
    if (decoder_engine->rgb_order == JPEG_DEC_RGB_ELEMENT_ORDER_RGB) {
        if (decoder_engine->output_format == JPEG_DECODE_OUT_FORMAT_RGB565) {
            post_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_0_1;
        } else if (decoder_engine->output_format == JPEG_DECODE_OUT_FORMAT_RGB888) {
            post_scramble = DMA2D_SCRAMBLE_ORDER_BYTE0_1_2;
        }
    }

    // Configure color space conversion option.
    if (decoder_engine->output_format == JPEG_DECODE_OUT_FORMAT_RGB565) {
        if (decoder_engine->conv_std == JPEG_YUV_RGB_CONV_STD_BT601) {
            rx_csc_option = DMA2D_CSC_RX_YUV420_TO_RGB565_601;
        } else if (decoder_engine->conv_std == JPEG_YUV_RGB_CONV_STD_BT709) {
            rx_csc_option = DMA2D_CSC_RX_YUV420_TO_RGB565_709;
        }
    } else if (decoder_engine->output_format == JPEG_DECODE_OUT_FORMAT_RGB888) {
        if (decoder_engine->conv_std == JPEG_YUV_RGB_CONV_STD_BT601) {
            rx_csc_option = DMA2D_CSC_RX_YUV420_TO_RGB888_601;
        } else if (decoder_engine->conv_std == JPEG_YUV_RGB_CONV_STD_BT709) {
            rx_csc_option = DMA2D_CSC_RX_YUV420_TO_RGB888_709;
        }
    } else if (decoder_engine->output_format == JPEG_DECODE_OUT_FORMAT_YUV444) {
        if (decoder_engine->sample_method == JPEG_DOWN_SAMPLING_YUV422) {
            rx_csc_option = DMA2D_CSC_RX_YUV422_TO_YUV444;
        } else if (decoder_engine->sample_method == JPEG_DOWN_SAMPLING_YUV420) {
            rx_csc_option = DMA2D_CSC_RX_YUV420_TO_YUV444;
        }
    } else {
        rx_csc_option = DMA2D_CSC_RX_NONE;
    }

    dma2d_csc_config_t rx_csc = {
        .post_scramble = post_scramble,
        .rx_csc_option = rx_csc_option,
    };
    dma2d_configure_color_space_conversion(rx_chan, &rx_csc);
}

static void jpeg_dec_config_dma_trans_ability(jpeg_decoder_handle_t decoder_engine)
{
    // set transfer ability
    dma2d_transfer_ability_t transfer_ability_config_tx = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_128,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };

    dma2d_transfer_ability_t transfer_ability_config_rx = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_128,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };

    switch (decoder_engine->sample_method) {
    case JPEG_DOWN_SAMPLING_YUV444:
        transfer_ability_config_rx.mb_size = DMA2D_MACRO_BLOCK_SIZE_8_8;
        break;
    case JPEG_DOWN_SAMPLING_YUV422:
        transfer_ability_config_rx.mb_size = DMA2D_MACRO_BLOCK_SIZE_8_16;
        break;
    case JPEG_DOWN_SAMPLING_YUV420:
        transfer_ability_config_rx.mb_size = DMA2D_MACRO_BLOCK_SIZE_16_16;
        break;
    case JPEG_DOWN_SAMPLING_GRAY:
        transfer_ability_config_rx.mb_size = DMA2D_MACRO_BLOCK_SIZE_8_8;
        break;
    default:
        break;
    }

    dma2d_set_transfer_ability(decoder_engine->dma2d_tx_channel, &transfer_ability_config_tx);
    dma2d_set_transfer_ability(decoder_engine->dma2d_rx_channel, &transfer_ability_config_rx);
}

static bool jpeg_dec_transaction_on_picked(uint32_t channel_num, const dma2d_trans_channel_info_t *dma2d_chans, void *users_config)
{
    assert(channel_num == 2);
    jpeg_decoder_handle_t decoder_engine = (jpeg_decoder_handle_t) users_config;
    jpeg_hal_context_t *hal = &decoder_engine->codec_base->hal;
    ESP_LOGD(TAG, "2ddma transaction callbacks start");

    uint32_t rx_idx, tx_idx;
    if (dma2d_chans[0].dir == DMA2D_CHANNEL_DIRECTION_TX) {
        rx_idx = 1;
        tx_idx = 0;
    } else {
        rx_idx = 0;
        tx_idx = 1;
    }
    dma2d_channel_handle_t tx_chan = dma2d_chans[tx_idx].chan;
    dma2d_channel_handle_t rx_chan = dma2d_chans[rx_idx].chan;

    decoder_engine->dma2d_rx_channel = rx_chan;
    decoder_engine->dma2d_tx_channel = tx_chan;

    // 2ddma connect
    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_JPEG_DECODER,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_JPEG_TX,
    };
    dma2d_connect(tx_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_JPEG_RX;
    dma2d_connect(rx_chan, &trig_periph);

    jpeg_dec_config_dma_trans_ability(decoder_engine);
    jpeg_dec_config_dma_csc(decoder_engine, rx_chan);

    dma2d_rx_event_callbacks_t jpeg_dec_cbs = {
        .on_recv_eof = jpeg_rx_eof,
    };

    dma2d_register_rx_event_callbacks(rx_chan, &jpeg_dec_cbs, decoder_engine);

    dma2d_set_desc_addr(tx_chan, (intptr_t)decoder_engine->txlink);
    dma2d_set_desc_addr(rx_chan, (intptr_t)decoder_engine->rxlink);
    dma2d_start(tx_chan);
    dma2d_start(rx_chan);
    jpeg_ll_process_start(hal->dev);

    return false;
}

static esp_err_t jpeg_color_space_support_check(jpeg_decoder_handle_t decoder_engine)
{
    if (decoder_engine->sample_method == JPEG_DOWN_SAMPLING_YUV444) {
        if (decoder_engine->output_format == JPEG_DECODE_OUT_FORMAT_YUV422 || decoder_engine->output_format == JPEG_DECODE_OUT_FORMAT_YUV420) {
            ESP_LOGE(TAG, "Detected YUV444 but want to convert to YUV422/YUV420, which is not supported");
            return ESP_ERR_INVALID_ARG;
        }
    } else if (decoder_engine->sample_method == JPEG_DOWN_SAMPLING_YUV422) {
        if (decoder_engine->output_format == JPEG_DECODE_OUT_FORMAT_YUV420) {
            ESP_LOGE(TAG, "Detected YUV422 but want to convert to YUV420, which is not supported");
            return ESP_ERR_INVALID_ARG;
        }
    } else if (decoder_engine->sample_method == JPEG_DOWN_SAMPLING_YUV420) {
        if (decoder_engine->output_format == JPEG_DECODE_OUT_FORMAT_YUV422) {
            ESP_LOGE(TAG, "Detected YUV420 but want to convert to YUV422, which is not supported");
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}

static esp_err_t jpeg_parse_header_info_to_hw(jpeg_decoder_handle_t decoder_engine)
{
    ESP_RETURN_ON_FALSE(decoder_engine, ESP_ERR_INVALID_ARG, TAG, "jpeg decode handle is null");
    jpeg_dec_header_info_t *header_info = decoder_engine->header_info;
    jpeg_hal_context_t *hal = &decoder_engine->codec_base->hal;

    for (int i = 0; i < header_info->qt_tbl_num; i++) {
        dqt_func[i](hal->dev, header_info->qt_tbl[i]);
    }

    jpeg_ll_set_picture_height(hal->dev, header_info->process_v);
    jpeg_ll_set_picture_width(hal->dev, header_info->process_h);
    jpeg_ll_set_decode_component_num(hal->dev, header_info->nf);
    for (int i = 0; i < header_info->nf; i++) {
        sof_func[i](hal->dev, header_info->ci[i], header_info->hi[i], header_info->vi[i], header_info->qtid[i]);
    }
    // If number of image component frame is 3, get the sampling method with sampling factor.
    if (header_info->nf == 3) {
        switch (header_info->hivi[0]) {
        case 0x11:
            decoder_engine->sample_method = JPEG_DOWN_SAMPLING_YUV444;
            break;
        case 0x21:
            decoder_engine->sample_method = JPEG_DOWN_SAMPLING_YUV422;
            break;
        case 0x22:
            decoder_engine->sample_method = JPEG_DOWN_SAMPLING_YUV420;
            break;
        default:
            ESP_LOGE(TAG, "Sampling factor cannot be recognized");
            return ESP_ERR_INVALID_STATE;
        }
    }
    if (header_info->nf == 1) {
        if (decoder_engine->output_format != JPEG_DECODE_OUT_FORMAT_GRAY) {
            ESP_LOGE(TAG, "your jpg is a gray style picture, but your output format is wrong");
            return ESP_ERR_NOT_SUPPORTED;
        }
        decoder_engine->sample_method = JPEG_DOWN_SAMPLING_GRAY;
    }

    ESP_RETURN_ON_ERROR(jpeg_color_space_support_check(decoder_engine), TAG, "jpeg decoder not support the combination of output format and down sampling format");

    if ((uint32_t)decoder_engine->sample_method == (uint32_t)decoder_engine->output_format) {
        decoder_engine->no_color_conversion = true;
    }

    // Write DHT information
    dht_func[0][0](hal, header_info->huffbits[0][0], header_info->huffcode[0][0], header_info->tmp_huff);
    dht_func[0][1](hal, header_info->huffbits[0][1], header_info->huffcode[0][1], header_info->tmp_huff);
    dht_func[1][0](hal, header_info->huffbits[1][0], header_info->huffcode[1][0], header_info->tmp_huff);
    dht_func[1][1](hal, header_info->huffbits[1][1], header_info->huffcode[1][1], header_info->tmp_huff);

    if (header_info->dri_marker) {
        jpeg_ll_set_restart_interval(hal->dev, header_info->ri);
    }

    return ESP_OK;
}

static esp_err_t jpeg_parse_marker(jpeg_decoder_handle_t decoder_engine, const uint8_t *in_buf, uint32_t inbuf_len)
{
    ESP_RETURN_ON_FALSE(decoder_engine, ESP_ERR_INVALID_ARG, TAG, "jpeg decode handle is null");
    ESP_RETURN_ON_FALSE(in_buf, ESP_ERR_INVALID_ARG, TAG, "jpeg decode input buffer is NULL");
    ESP_RETURN_ON_FALSE(inbuf_len != 0, ESP_ERR_INVALID_ARG, TAG, "jpeg decode input buffer length is 0");

    jpeg_dec_header_info_t* header_info = decoder_engine->header_info;
    jpeg_hal_context_t *hal = &decoder_engine->codec_base->hal;

    memset(decoder_engine->header_info, 0, sizeof(jpeg_dec_header_info_t));
    decoder_engine->header_info->buffer_offset = (uint8_t*)in_buf;
    decoder_engine->header_info->buffer_left = inbuf_len;
    decoder_engine->total_size = inbuf_len;
    decoder_engine->header_info->header_size = 0;

    jpeg_ll_soft_rst(hal->dev);
    jpeg_ll_set_codec_mode(hal->dev, JPEG_CODEC_DECODER);
    // Digital issue. Needs to set height and width to 0 before decoding a new picture.
    jpeg_ll_set_picture_height(hal->dev, 0);
    jpeg_ll_set_picture_width(hal->dev, 0);

    while (header_info->buffer_left) {
        uint8_t lastchar = jpeg_get_bytes(header_info, 1);
        uint8_t thischar = jpeg_get_bytes(header_info, 1);
        uint16_t marker = (lastchar << 8 | thischar);
        switch (marker) {
        case JPEG_M_SOI:
            break;
        case JPEG_M_APP0:
        case JPEG_M_APP1:
        case JPEG_M_APP2:
        case JPEG_M_APP3:
        case JPEG_M_APP4:
        case JPEG_M_APP5:
        case JPEG_M_APP6:
        case JPEG_M_APP7:
        case JPEG_M_APP8:
        case JPEG_M_APP9:
        case JPEG_M_APP10:
        case JPEG_M_APP11:
        case JPEG_M_APP12:
        case JPEG_M_APP13:
        case JPEG_M_APP14:
        case JPEG_M_APP15:
            ESP_RETURN_ON_ERROR(jpeg_parse_appn_marker(header_info), TAG, "deal appn marker failed");
            break;
        case JPEG_M_COM:
            ESP_RETURN_ON_ERROR(jpeg_parse_com_marker(header_info), TAG, "deal com marker failed");
            break;
        case JPEG_M_DQT:
            ESP_RETURN_ON_ERROR(jpeg_parse_dqt_marker(header_info), TAG, "deal dqt marker failed");
            break;
        case JPEG_M_SOF0:
            ESP_RETURN_ON_ERROR(jpeg_parse_sof_marker(header_info), TAG, "deal sof marker failed");
            break;
        case JPEG_M_SOF1:
        case JPEG_M_SOF2:
        case JPEG_M_SOF3:
        case JPEG_M_SOF5:
        case JPEG_M_SOF6:
        case JPEG_M_SOF7:
        case JPEG_M_SOF9:
        case JPEG_M_SOF10:
        case JPEG_M_SOF11:
        case JPEG_M_SOF13:
        case JPEG_M_SOF14:
        case JPEG_M_SOF15:
            ESP_LOGE(TAG, "Only baseline-DCT is supported.");
            return ESP_ERR_NOT_SUPPORTED;
        case JPEG_M_DRI:
            ESP_RETURN_ON_ERROR(jpeg_parse_dri_marker(header_info), TAG, "deal dri marker failed");
            break;
        case JPEG_M_DHT:
            ESP_RETURN_ON_ERROR(jpeg_parse_dht_marker(header_info), TAG, "deal dht marker failed");
            break;
        case JPEG_M_SOS:
            ESP_RETURN_ON_ERROR(jpeg_parse_sos_marker(header_info), TAG, "deal sos marker failed");
            break;
        }
        if (marker == JPEG_M_SOS) {
            break;
        }
    }

    // Update information after parse marker finishes
    decoder_engine->header_info->buffer_left = decoder_engine->total_size - decoder_engine->header_info->header_size;

    return ESP_OK;
}

static void s_decoder_error_log_print(uint32_t status)
{
    if (status & JPEG_LL_INTR_CID_ERR) {
        ESP_LOGE(TAG, "decoded component ID is different from the component ID configured by the software");
    }
    if (status & JPEG_LL_INTR_C_DHT_DC_ID) {
        ESP_LOGE(TAG, "decoded DC Huffman table ID of each component is not the software configured DC0 Huffman table ID or DC1 Huffman table ID");
    }
    if (status & JPEG_LL_INTR_C_DHT_AC_ID) {
        ESP_LOGE(TAG, "decoded AC Huffman table ID of each component is not the software configured AC0 Huffman table ID or AC1 Huffman table ID");
    }
    if (status & JPEG_LL_INTR_C_DQT_ID) {
        ESP_LOGE(TAG, "decoded quantization table ID of each component is different from the software configured quantization table ID");
    }
    if (status & JPEG_LL_INTR_RST_UXP_ERR) {
        ESP_LOGE(TAG, "JPEG_RESTART_INTERVAL configured by the software is 0 but the RST marker is parsed by the decoder");
    }
    if (status & JPEG_LL_INTR_RST_CHECK_NON_ERR) {
        ESP_LOGE(TAG, "JPEG_RESTART_INTERVAL configured by the software is non-0 but the RST marker cannot be parsed by the decoder");
    }
    if (status & JPEG_LL_INTR_RST_CHECK_POS_ERR) {
        ESP_LOGE(TAG, "the MCU number between two parsed RST markers is not equal to the JPEG_RESTART_INTERVAL configured by the software");
    }
    if (status & JPEG_LL_INTR_SCAN_CHECK_NONE) {
        ESP_LOGE(TAG, "an image frame has multiple SCANs to be decoded and the SOS marker is not parsed within (JPEG_SOS_CHECK_BYTE_NUM + 1) bytes in any SCAN header information");
    }
    if (status & JPEG_LL_INTR_SCAN_POS_ERR) {
        ESP_LOGE(TAG, "the position of the SCAN header information parsed by the decoder is wrong");
    }
    if (status & JPEG_LL_INTR_UXP_DET) {
        ESP_LOGE(TAG, "marker parsed by the decoder is not supported by the hardware");
    }
    if (status & JPEG_LL_INTR_DE_FRAME_EOF_ERR) {
        ESP_LOGE(TAG, "the number of data units obtained after decoding a frame of image is different from the number of data units calculated based on the image resolution configured by the software");
    }
    if (status & JPEG_LL_INTR_DE_FRAME_EOF_LACK) {
        ESP_LOGE(TAG, "the bitstream of a image is completely read from 2D DMA but the EOF marker or EOI marker is not read");
    }
    if (status & JPEG_LL_INTR_SOS_UNMATCH_ERR) {
        ESP_LOGE(TAG, "the number of components in the SCAN header information parsed by the decoder is 0 or the header length in the SCAN header information parsed by the decoder does not match the actual header length.");
    }
    if (status & JPEG_LL_INTR_MARKER_ERR_FST) {
        ESP_LOGE(TAG, "there is an error in the first SCAN header information parsed by the decoder.");
    }
    if (status & JPEG_LL_INTR_MARKER_ERR_OTHER) {
        ESP_LOGE(TAG, "there is an error in the non-first SCAN header information parsed by the decoder.");
    }
    if (status & JPEG_LL_INTR_UNDETECT) {
        ESP_LOGE(TAG, "the bitstream of a image is completely read from 2D DMA but the SOS marker is not read");
    }
    if (status & JPEG_LL_INTR_DECODE_TIMEOUT) {
        ESP_LOGE(TAG, "decoder is timeout");
    }
}
