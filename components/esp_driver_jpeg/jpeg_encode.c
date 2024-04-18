/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#if CONFIG_JPEG_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_attr.h"
#include "hal/jpeg_ll.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_private/dma2d.h"
#include "jpeg_private.h"
#include "driver/jpeg_encode.h"
#include "private/jpeg_param.h"
#include "private/jpeg_emit_marker.h"
#include "esp_check.h"
#include "esp_cache.h"
#include "esp_private/esp_cache_private.h"
#include "esp_intr_alloc.h"
#include "soc/dma2d_channel.h"

static const char *TAG = "jpeg.encoder";

static esp_err_t s_jpeg_set_header_info(jpeg_encoder_handle_t encoder_engine);
static uint32_t s_dma_desc_get_len(dma2d_descriptor_t *dsc);
static bool s_jpeg_rx_eof(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data);
static bool s_jpeg_enc_transaction_on_job_picked(uint32_t channel_num, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);
static void s_cfg_desc(jpeg_encoder_handle_t encoder_engine, dma2d_descriptor_t *dsc, uint8_t en_2d, uint8_t mode, uint16_t vb, uint16_t hb, uint8_t eof, uint32_t pbyte, uint8_t owner, uint16_t va, uint16_t ha, uint8_t *buf, dma2d_descriptor_t *next_dsc);
static void s_jpeg_enc_config_picture_color_space(jpeg_encoder_handle_t encoder_engine);
static void s_jpeg_enc_select_sample_mode(jpeg_encoder_handle_t encoder_engine);
static void s_encoder_error_log_print(uint32_t status);

static void jpeg_encoder_isr_handle_default(void *arg)
{
    jpeg_encoder_handle_t encoder_engine = (jpeg_encoder_handle_t) arg;
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    jpeg_hal_context_t *hal = &encoder_engine->codec_base->hal;
    jpeg_enc_dma2d_evt_t s_event = {
        .dma_evt = 0,
        .encoder_status = 0,
    };
    uint32_t value = jpeg_ll_get_intr_status(hal->dev);
    jpeg_ll_clear_intr_mask(hal->dev, value);
    s_event.encoder_status = value;
    xQueueSendFromISR(encoder_engine->evt_queue, &s_event, &HPTaskAwoken);

    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t s_jpeg_set_header_info(jpeg_encoder_handle_t encoder_engine)
{
    encoder_engine->header_info->header_len = 0;
    ESP_RETURN_ON_ERROR(emit_soi_marker(encoder_engine->header_info), TAG, "marker emit failed");
    ESP_RETURN_ON_ERROR(emit_app0_marker(encoder_engine->header_info), TAG, "marker emit failed");
    ESP_RETURN_ON_ERROR(emit_dqt_marker(encoder_engine->header_info), TAG, "marker emit failed");
    ESP_RETURN_ON_ERROR(emit_sof_marker(encoder_engine->header_info), TAG, "marker emit failed");
    ESP_RETURN_ON_ERROR(emit_dht_marker(encoder_engine->header_info), TAG, "marker emit failed");
    ESP_RETURN_ON_ERROR(emit_com_marker(encoder_engine->header_info), TAG, "marker emit failed");
    ESP_RETURN_ON_ERROR(emit_sos_marker(encoder_engine->header_info), TAG, "marker emit failed");
    return ESP_OK;
}

esp_err_t jpeg_new_encoder_engine(const jpeg_encode_engine_cfg_t *enc_eng_cfg, jpeg_encoder_handle_t *ret_encoder)
{
#if CONFIG_JPEG_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    jpeg_encoder_handle_t encoder_engine = NULL;
    ESP_RETURN_ON_FALSE(enc_eng_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    encoder_engine = (jpeg_encoder_handle_t)heap_caps_calloc(1, sizeof(jpeg_encoder_t), MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(encoder_engine, ESP_ERR_NO_MEM, TAG, "no memory for jpeg encoder");

    uint32_t cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    uint32_t alignment = cache_line_size;
    size_t dma_desc_mem_size = JPEG_ALIGN_UP(sizeof(dma2d_descriptor_t), cache_line_size);

    encoder_engine->rxlink = (dma2d_descriptor_t*)heap_caps_aligned_calloc(alignment, 1, sizeof(dma2d_descriptor_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(encoder_engine->rxlink, ESP_ERR_NO_MEM, err, TAG, "no memory for jpeg encoder rxlink");
    encoder_engine->txlink = (dma2d_descriptor_t*)heap_caps_aligned_calloc(alignment, 1, sizeof(dma2d_descriptor_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(encoder_engine->txlink, ESP_ERR_NO_MEM, err, TAG, "no memory for jpeg encoder txlink");
    encoder_engine->dma_desc_size = dma_desc_mem_size;

    ESP_GOTO_ON_ERROR(jpeg_acquire_codec_handle(&encoder_engine->codec_base), err, TAG, "JPEG encoder acquires codec handle failed");
    jpeg_hal_context_t *hal = &encoder_engine->codec_base->hal;
    encoder_engine->evt_queue = xQueueCreateWithCaps(2, sizeof(jpeg_enc_dma2d_evt_t), JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(encoder_engine->evt_queue, ESP_ERR_NO_MEM, err, TAG, "No memory for event queue");

    encoder_engine->timeout_tick = (enc_eng_cfg->timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(enc_eng_cfg->timeout_ms);
    jpeg_ll_clear_intr_mask(hal->dev, JPEG_LL_ENCODER_EVENT_INTR);

    ESP_GOTO_ON_ERROR(jpeg_check_intr_priority(encoder_engine->codec_base, enc_eng_cfg->intr_priority), err, TAG, "set group interrupt priority failed");
    if (enc_eng_cfg->intr_priority) {
        ESP_RETURN_ON_FALSE(1 << (enc_eng_cfg->intr_priority) & JPEG_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, TAG, "invalid interrupt priority:%d", enc_eng_cfg->intr_priority);
    }
    int isr_flags = JPEG_INTR_ALLOC_FLAG;
    if (enc_eng_cfg->intr_priority) {
        isr_flags |= 1 << (enc_eng_cfg->intr_priority);
    }

    ret = jpeg_isr_register(encoder_engine->codec_base, jpeg_encoder_isr_handle_default, encoder_engine, JPEG_LL_ENCODER_EVENT_INTR, isr_flags, &encoder_engine->intr_handle);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install jpeg decode interrupt failed");

    dma2d_pool_config_t dma2d_group_config = {
        .pool_id = 0,
    };
    ESP_ERROR_CHECK(dma2d_acquire_pool(&dma2d_group_config, &encoder_engine->dma2d_group_handle));

    encoder_engine->trans_desc = (dma2d_trans_t *)heap_caps_calloc(1, SIZEOF_DMA2D_TRANS_T, JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(encoder_engine->trans_desc, ESP_ERR_NO_MEM, err, TAG, "No memory for dma2d descriptor");

    encoder_engine->header_info = (jpeg_enc_header_info_t*)heap_caps_calloc(1, sizeof(jpeg_enc_header_info_t), JPEG_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(encoder_engine->header_info, ESP_ERR_NO_MEM, err, TAG, "no memory for jpeg header information structure");

    *ret_encoder = encoder_engine;
    return ESP_OK;
err:
    if (encoder_engine) {
        jpeg_del_encoder_engine(encoder_engine);
    }
    return ret;
}

esp_err_t jpeg_encoder_process(jpeg_encoder_handle_t encoder_engine, const jpeg_encode_cfg_t *encode_cfg, const uint8_t *encode_inbuf, uint32_t inbuf_size, uint8_t *bit_stream, uint32_t outbuf_size, uint32_t *out_size)
{
    ESP_RETURN_ON_FALSE(encoder_engine, ESP_ERR_INVALID_ARG, TAG, "jpeg encode handle is null");
    ESP_RETURN_ON_FALSE(encode_cfg, ESP_ERR_INVALID_ARG, TAG, "jpeg encode config is null");
    ESP_RETURN_ON_FALSE(encode_inbuf, ESP_ERR_INVALID_ARG, TAG, "jpeg encode picture buffer is null");
    ESP_RETURN_ON_FALSE(out_size, ESP_ERR_INVALID_ARG, TAG, "jpeg encode picture out_size is null");
    ESP_RETURN_ON_FALSE(((uintptr_t)bit_stream % cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA)) == 0, ESP_ERR_INVALID_ARG, TAG, "jpeg encode bit stream is not aligned, please use jpeg_alloc_encoder_mem to malloc your buffer");
    if (encode_cfg->src_type == JPEG_ENCODE_IN_FORMAT_YUV422) {
        ESP_RETURN_ON_FALSE(encode_cfg->sub_sample == JPEG_DOWN_SAMPLING_YUV422, ESP_ERR_INVALID_ARG, TAG, "Sub sampling is not supported under this source type");
    }

    esp_err_t ret = ESP_OK;

    if (encoder_engine->codec_base->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(encoder_engine->codec_base->pm_lock), TAG, "acquire pm_lock failed");
    }
    jpeg_hal_context_t *hal = &encoder_engine->codec_base->hal;
    uint8_t *raw_buffer = (uint8_t*)encode_inbuf;
    uint32_t compressed_size;
    xSemaphoreTake(encoder_engine->codec_base->codec_mutex, portMAX_DELAY);
    jpeg_ll_soft_rst(hal->dev);
    jpeg_ll_set_codec_mode(hal->dev, JPEG_CODEC_ENCODER);
    /* Reset queue */
    xQueueReset(encoder_engine->evt_queue);

    jpeg_enc_format_hb_t best_hb_idx = 0;

    encoder_engine->picture_format = encode_cfg->src_type;
    color_space_pixel_format_t picture_format;
    picture_format.color_type_id = encoder_engine->picture_format;

    switch (encode_cfg->src_type) {
    case JPEG_ENCODE_IN_FORMAT_RGB888:
        encoder_engine->color_space = JPEG_ENC_SRC_RGB888;
        best_hb_idx = JPEG_ENC_SRC_RGB888_HB;
        break;
    case JPEG_ENCODE_IN_FORMAT_RGB565:
        encoder_engine->color_space = JPEG_ENC_SRC_RGB565;
        best_hb_idx = JPEG_ENC_SRC_RGB565_HB;
        break;
    case JPEG_ENCODE_IN_FORMAT_GRAY:
        encoder_engine->color_space = JPEG_ENC_SRC_GRAY;
        best_hb_idx = JPEG_ENC_SRC_GRAY_HB;
        break;
    case JPEG_ENCODE_IN_FORMAT_YUV422:
        encoder_engine->color_space = JPEG_ENC_SRC_YUV422;
        best_hb_idx = JPEG_ENC_SRC_YUV422_HB;
        break;
    default:
        ESP_LOGE(TAG, "wrong, we don't support encode from such format.");
        ret = ESP_ERR_NOT_SUPPORTED;
        goto err;
    }
    encoder_engine->header_info->sub_sample = encode_cfg->sub_sample;
    encoder_engine->header_info->quality = encode_cfg->image_quality;
    encoder_engine->header_info->origin_h = encode_cfg->width;
    encoder_engine->header_info->origin_v = encode_cfg->height;
    encoder_engine->header_info->header_buf = bit_stream;

    s_jpeg_enc_config_picture_color_space(encoder_engine);
    s_jpeg_enc_select_sample_mode(encoder_engine);
    jpeg_ll_set_picture_height(hal->dev, encoder_engine->header_info->origin_v);
    jpeg_ll_set_picture_width(hal->dev, encoder_engine->header_info->origin_h);
    jpeg_ll_pixel_reverse(hal->dev, false);
    jpeg_ll_add_tail(hal->dev, true);
    jpeg_ll_enable_ff_check(hal->dev, true);
    jpeg_ll_set_qnr_presition(hal->dev, 0);
    ESP_GOTO_ON_ERROR(s_jpeg_set_header_info(encoder_engine), err, TAG, "set header failed");
    jpeg_hal_set_quantization_coefficient(hal, encoder_engine->header_info->m_quantization_tables[0], encoder_engine->header_info->m_quantization_tables[1]);

    uint8_t sample_method_idx = 0;
    switch (encoder_engine->header_info->sub_sample) {
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

    uint32_t dma_hb = enc_hb_tbl[best_hb_idx][sample_method_idx];
    uint32_t dma_vb = encoder_engine->mcuy;

    ESP_GOTO_ON_FALSE((encoder_engine->header_info->header_len % cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA)) == 0, ESP_ERR_INVALID_STATE, err, TAG, "The header is not cache line aligned, please check");

    // 1D direction
    memset(encoder_engine->rxlink, 0, sizeof(dma2d_descriptor_t));
    s_cfg_desc(encoder_engine, encoder_engine->rxlink, JPEG_DMA2D_2D_DISABLE, DMA2D_DESCRIPTOR_BLOCK_RW_MODE_MULTIPLE, outbuf_size & JPEG_DMA2D_MAX_SIZE, 0, JPEG_DMA2D_EOF_NOT_LAST, 1, DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA, outbuf_size >> JPEG_DMA2D_1D_HIGH_14BIT, 0, bit_stream + encoder_engine->header_info->header_len, NULL);

    // 2D direction
    memset(encoder_engine->txlink, 0, sizeof(dma2d_descriptor_t));
    s_cfg_desc(encoder_engine, encoder_engine->txlink, JPEG_DMA2D_2D_ENABLE, DMA2D_DESCRIPTOR_BLOCK_RW_MODE_MULTIPLE, dma_vb, dma_hb, JPEG_DMA2D_EOF_NOT_LAST, dma2d_desc_pixel_format_to_pbyte_value(picture_format), DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA, encoder_engine->header_info->origin_v, encoder_engine->header_info->origin_h, raw_buffer, NULL);

    ret = esp_cache_msync((void*)raw_buffer, encoder_engine->header_info->origin_v * encoder_engine->header_info->origin_h * encoder_engine->bytes_per_pixel, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    assert(ret == ESP_OK);

    dma2d_trans_config_t trans_desc = {
        .tx_channel_num = 1,
        .rx_channel_num = 1,
        .channel_flags = DMA2D_CHANNEL_FUNCTION_FLAG_TX_REORDER,
        .user_config = encoder_engine,
        .on_job_picked = s_jpeg_enc_transaction_on_job_picked,
    };

    ESP_GOTO_ON_ERROR(dma2d_enqueue(encoder_engine->dma2d_group_handle, &trans_desc, encoder_engine->trans_desc), err, TAG, "DMA2D enqueue failed");

    while (1) {
        jpeg_enc_dma2d_evt_t s_rcv_event;
        BaseType_t ret_val = xQueueReceive(encoder_engine->evt_queue, &s_rcv_event, encoder_engine->timeout_tick);
        ESP_GOTO_ON_FALSE(ret_val == pdTRUE, ESP_ERR_TIMEOUT, err, TAG, "jpeg-dma2d handle jpeg decode timeout, please check `timeout_ms`");

        if (s_rcv_event.encoder_status != 0) {
            s_encoder_error_log_print(s_rcv_event.encoder_status);
            ret = ESP_ERR_INVALID_STATE;
            goto err;
        }

        if (s_rcv_event.dma_evt & JPEG_DMA2D_RX_EOF) {
            compressed_size = s_dma_desc_get_len(encoder_engine->rxlink);
            uint32_t _compressed_size = JPEG_ALIGN_UP(compressed_size, cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA));
            ESP_GOTO_ON_ERROR(esp_cache_msync((void*)(bit_stream + encoder_engine->header_info->header_len), _compressed_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C), err, TAG, "sync memory to cache failed");
            break;
        }
    }

    compressed_size += encoder_engine->header_info->header_len;
    *out_size = compressed_size;

err:
    xSemaphoreGive(encoder_engine->codec_base->codec_mutex);
    if (encoder_engine->codec_base->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(encoder_engine->codec_base->pm_lock), TAG, "release pm_lock failed");
    }
    return ret;
}

esp_err_t jpeg_del_encoder_engine(jpeg_encoder_handle_t encoder_engine)
{
    ESP_RETURN_ON_FALSE(encoder_engine, ESP_ERR_INVALID_ARG, TAG, "jpeg encoder handle is null");
    ESP_RETURN_ON_ERROR(jpeg_release_codec_handle(encoder_engine->codec_base), TAG, "release codec failed");

    if (encoder_engine) {
        if (encoder_engine->rxlink) {
            free(encoder_engine->rxlink);
        }
        if (encoder_engine->txlink) {
            free(encoder_engine->txlink);
        }
        if (encoder_engine->header_info) {
            free(encoder_engine->header_info);
        }
        if (encoder_engine->trans_desc) {
            free(encoder_engine->trans_desc);
        }
        if (encoder_engine->evt_queue) {
            vQueueDeleteWithCaps(encoder_engine->evt_queue);
        }
        if (encoder_engine->dma2d_group_handle) {
            dma2d_release_pool(encoder_engine->dma2d_group_handle);
        }
        if (encoder_engine->intr_handle) {
            jpeg_isr_deregister(encoder_engine->codec_base, encoder_engine->intr_handle);
        }
        free(encoder_engine);
    }
    return ESP_OK;
}

void *jpeg_alloc_encoder_mem(size_t size, const jpeg_encode_memory_alloc_cfg_t *mem_cfg, size_t *allocated_size)
{
    /*
       Principle of buffer align.
       For output buffer(for decoder is 2DDMA write to PSRAM), both address and size should be aligned according to cache invalidate.
       For input buffer(for decoder is PSRAM write to 2DDMA), no restriction for any align (both cache writeback and requirement from 2DDMA).
    */
    size_t cache_align = 0;
    esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &cache_align);
    if (mem_cfg->buffer_direction == JPEG_ENC_ALLOC_OUTPUT_BUFFER) {
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

static uint32_t s_dma_desc_get_len(dma2d_descriptor_t *dsc)
{
    uint32_t len = 0;
    len |= dsc->ha_length;
    len = len << 14;
    len |= dsc->hb_length;
    return len;
}

static bool s_jpeg_rx_eof(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data)
{
    jpeg_encoder_handle_t encoder_engine = (jpeg_encoder_handle_t) user_data;
    portBASE_TYPE higher_priority_task_awoken = pdFALSE;
    jpeg_enc_dma2d_evt_t s_event = {
        .dma_evt = 0,
        .encoder_status = 0,
    };
    s_event.dma_evt = JPEG_DMA2D_RX_EOF;
    xQueueSendFromISR(encoder_engine->evt_queue, &s_event, &higher_priority_task_awoken);

    return higher_priority_task_awoken;
}

static void jpeg_enc_config_dma_trans_ability(jpeg_encoder_handle_t encoder_engine)
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

    switch (encoder_engine->header_info->sub_sample) {
    case JPEG_DOWN_SAMPLING_YUV444:
        transfer_ability_config_tx.mb_size = DMA2D_MACRO_BLOCK_SIZE_8_8;
        break;
    case JPEG_DOWN_SAMPLING_YUV422:
        transfer_ability_config_tx.mb_size = DMA2D_MACRO_BLOCK_SIZE_8_16;
        break;
    case JPEG_DOWN_SAMPLING_YUV420:
        transfer_ability_config_tx.mb_size = DMA2D_MACRO_BLOCK_SIZE_16_16;
        break;
    case JPEG_DOWN_SAMPLING_GRAY:
        transfer_ability_config_tx.mb_size = DMA2D_MACRO_BLOCK_SIZE_8_8;
        break;
    default:
        break;
    }

    dma2d_set_transfer_ability(encoder_engine->dma2d_tx_channel, &transfer_ability_config_tx);
    dma2d_set_transfer_ability(encoder_engine->dma2d_rx_channel, &transfer_ability_config_rx);
}

static bool s_jpeg_enc_transaction_on_job_picked(uint32_t channel_num, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(channel_num == 2);
    jpeg_encoder_handle_t encoder_engine = (jpeg_encoder_handle_t) user_config;
    jpeg_hal_context_t *hal = &encoder_engine->codec_base->hal;

    uint32_t rx_idx = 0;
    uint32_t tx_idx = 0;
    if (dma2d_chans[0].dir == DMA2D_CHANNEL_DIRECTION_TX) {
        rx_idx = 1;
        tx_idx = 0;
    } else {
        rx_idx = 0;
        tx_idx = 1;
    }
    dma2d_channel_handle_t tx_chan = dma2d_chans[tx_idx].chan;
    dma2d_channel_handle_t rx_chan = dma2d_chans[rx_idx].chan;

    encoder_engine->dma2d_tx_channel = tx_chan;
    encoder_engine->dma2d_rx_channel = rx_chan;

    // 2ddma connect
    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_JPEG_ENCODER,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_JPEG_TX,
    };
    dma2d_connect(tx_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_JPEG_RX;
    dma2d_connect(rx_chan, &trig_periph);

    jpeg_enc_config_dma_trans_ability(encoder_engine);

    dma2d_csc_config_t tx_csc = {
        .tx_csc_option = DMA2D_CSC_TX_NONE,
    };
    dma2d_configure_color_space_conversion(tx_chan, &tx_csc);

    static dma2d_rx_event_callbacks_t jpeg_dec_cbs = {
        .on_recv_eof = s_jpeg_rx_eof,
    };

    dma2d_register_rx_event_callbacks(rx_chan, &jpeg_dec_cbs, encoder_engine);
    dma2d_set_desc_addr(tx_chan, (intptr_t)encoder_engine->txlink);
    dma2d_set_desc_addr(rx_chan, (intptr_t)encoder_engine->rxlink);
    dma2d_start(tx_chan);
    dma2d_start(rx_chan);
    jpeg_ll_enable_intr_mask(hal->dev, JPEG_LL_ENCODER_EVENT_INTR);
    jpeg_ll_process_start(hal->dev);

    return false;
}

static void s_cfg_desc(jpeg_encoder_handle_t encoder_engine, dma2d_descriptor_t *dsc, uint8_t en_2d, uint8_t mode, uint16_t vb, uint16_t hb, uint8_t eof, uint32_t pbyte, uint8_t owner, uint16_t va, uint16_t ha, uint8_t *buf, dma2d_descriptor_t *next_dsc)
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
    esp_err_t ret = esp_cache_msync((void*)dsc, encoder_engine->dma_desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE);
    assert(ret == ESP_OK);
}

static void s_jpeg_enc_config_picture_color_space(jpeg_encoder_handle_t encoder_engine)
{
    jpeg_hal_context_t *hal = &encoder_engine->codec_base->hal;
    color_space_pixel_format_t picture_format;
    jpeg_ll_config_picture_pixel_format(hal->dev, encoder_engine->color_space);
    picture_format.color_type_id = encoder_engine->picture_format;
    encoder_engine->bytes_per_pixel = color_hal_pixel_format_get_bit_depth(picture_format);
    if (encoder_engine->color_space == JPEG_ENC_SRC_GRAY) {
        encoder_engine->header_info->num_components = 1;
    } else {
        encoder_engine->header_info->num_components = 3;
    }
}

static void s_jpeg_enc_select_sample_mode(jpeg_encoder_handle_t encoder_engine)
{
    jpeg_hal_context_t *hal = &encoder_engine->codec_base->hal;
    switch (encoder_engine->header_info->sub_sample) {
    case JPEG_DOWN_SAMPLING_YUV444:
        encoder_engine->mcux = 8;
        encoder_engine->mcuy = 8;
        break;
    case JPEG_DOWN_SAMPLING_YUV422:
        encoder_engine->mcux = 16;
        encoder_engine->mcuy = 8;
        break;
    case JPEG_DOWN_SAMPLING_YUV420:
        encoder_engine->mcux = 16;
        encoder_engine->mcuy = 16;
        break;
    case JPEG_DOWN_SAMPLING_GRAY:
        encoder_engine->mcux = 8;
        encoder_engine->mcuy = 8;
        break;
    default:
        break;
    }
    if (encoder_engine->header_info->sub_sample != JPEG_DOWN_SAMPLING_GRAY) {
        // Not needed to call this function if 1 channel color down sampling
        jpeg_ll_sample_mode_select(hal->dev, encoder_engine->header_info->sub_sample);
    }
}

static void s_encoder_error_log_print(uint32_t status)
{
    if (status & JPEG_LL_RLE_PARALLEL_ERR) {
        ESP_LOGE(TAG, "Run length encoding error occurs");
    }
    if (status & JPEG_LL_EN_FRAME_EOF_ERR) {
        ESP_LOGE(TAG, "The number of data units obtained after decoding a frame of image is different from the number of data units calculated based on the image resolution configured by the software");
    }
}
