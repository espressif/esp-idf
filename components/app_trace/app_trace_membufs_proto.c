/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include <sys/param.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_app_trace_membufs_proto.h"

/** Trace data header. Every user data chunk is prepended with this header.
 * User allocates block with esp_apptrace_buffer_get and then fills it with data,
 * in multithreading environment it can happen that tasks gets buffer and then gets interrupted,
 * so it is possible that user data are incomplete when  memory block is exposed to the host.
 * In this case host SW will see that wr_sz < block_sz and will report error.
 */
typedef struct {
    union {
        struct {
            uint8_t  block_sz_8;
            uint8_t  wr_sz_8;
        };
        struct {
            uint16_t block_sz_16;
            uint16_t wr_sz_16;
        };
    };
} esp_tracedata_hdr_t;

/** TODO: docs
 */
typedef struct {
    uint16_t   block_sz; // size of allocated block for user data
} esp_hostdata_hdr_t;

#define ESP_APPTRACE_INBLOCK_MARKER(_hw_data_) \
    ((_hw_data_)->state.markers[(_hw_data_)->state.in_block % 2])

#define ESP_APPTRACE_INBLOCK(_hw_data_) \
    (&(_hw_data_)->blocks[(_hw_data_)->state.in_block % 2])

const static char *TAG = "esp_apptrace";

static uint32_t esp_apptrace_membufs_down_buffer_write_nolock(esp_apptrace_membufs_proto_data_t *proto,
                                                              uint8_t *data, uint32_t size);

esp_err_t esp_apptrace_membufs_init(esp_apptrace_membufs_proto_data_t *proto,
                                    const esp_apptrace_mem_block_t blocks_cfg[2])
{
    // disabled by default
    esp_apptrace_rb_init(&proto->rb_down, NULL, 0);
    // membufs proto init
    for (unsigned int i = 0; i < 2; i++) {
        proto->blocks[i].start = blocks_cfg[i].start;
        proto->blocks[i].sz = blocks_cfg[i].sz;
        proto->state.markers[i] = 0;
    }
    proto->state.in_block = 0;
    return ESP_OK;
}

void esp_apptrace_membufs_down_buffer_config(esp_apptrace_membufs_proto_data_t *data, uint8_t *buf, uint32_t size)
{
    esp_apptrace_rb_init(&data->rb_down, buf, size);
}

// assumed to be protected by caller from multi-core/thread access
static esp_err_t esp_apptrace_membufs_swap(esp_apptrace_membufs_proto_data_t *proto)
{
    int prev_block_num = proto->state.in_block % 2;
    int new_block_num = prev_block_num ? (0) : (1);

    esp_err_t res = proto->hw->swap_start(proto->state.in_block);
    if (res != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to swap to new block: %d", res);
        return res;
    }

    proto->state.markers[new_block_num] = 0;
    // switch to new block
    proto->state.in_block++;

    proto->hw->swap(new_block_num, proto->state.markers[prev_block_num]);

    // handle data from host
    esp_hostdata_hdr_t *hdr = (esp_hostdata_hdr_t *)proto->blocks[new_block_num].start;
    // ESP_APPTRACE_LOGV("Host data %d, sz %d @ %p", proto->hw->host_data_pending(), hdr->block_sz, hdr);
    if (proto->hw->host_data_pending() && hdr->block_sz > 0) {
        // TODO: add support for multiple blocks from host, currently there is no need for that
        uint8_t *p = proto->blocks[new_block_num].start + proto->blocks[new_block_num].sz;
        ESP_APPTRACE_LOGD("Recvd %" PRIu16 " bytes from host (@ %p) [%x %x %x %x %x %x %x %x .. %x %x %x %x %x %x %x %x]",
                          hdr->block_sz, proto->blocks[new_block_num].start,
                          *(proto->blocks[new_block_num].start + 0), *(proto->blocks[new_block_num].start + 1),
                          *(proto->blocks[new_block_num].start + 2), *(proto->blocks[new_block_num].start + 3),
                          *(proto->blocks[new_block_num].start + 4), *(proto->blocks[new_block_num].start + 5),
                          *(proto->blocks[new_block_num].start + 6), *(proto->blocks[new_block_num].start + 7),
                          *(p - 8), *(p - 7), *(p - 6), *(p - 5), *(p - 4), *(p - 3), *(p - 2), *(p - 1));
        uint32_t sz = esp_apptrace_membufs_down_buffer_write_nolock(proto, (uint8_t *)(hdr + 1), hdr->block_sz);
        if (sz != hdr->block_sz) {
            ESP_APPTRACE_LOGE("Failed to write %" PRIu32 " bytes to down buffer (%" PRIu16 " %" PRIu32 ")!",
                              hdr->block_sz - sz, hdr->block_sz, sz);
        }
        hdr->block_sz = 0;
    }
    proto->hw->swap_end(proto->state.in_block, proto->state.markers[prev_block_num]);
    return res;
}

static esp_err_t esp_apptrace_membufs_swap_waitus(esp_apptrace_membufs_proto_data_t *proto, esp_apptrace_tmo_t *tmo)
{
    int res;

    while ((res = esp_apptrace_membufs_swap(proto)) != ESP_OK) {
        res = esp_apptrace_tmo_check(tmo);
        if (res != ESP_OK) {
            break;
        }
#if CONFIG_IDF_TARGET_ESP32S3
        /*
        * ESP32S3 has a serious data corruption issue with the transferred data to host.
        * This delay helps reduce the failure rate by temporarily reducing heavy memory writes
        * from RTOS-level tracing and giving OpenOCD more time to read trace memory before
        * the current thread continues execution. While this doesn't completely prevent
        * memory access from other threads/cores/ISRs, it has shown to significantly improve
        * reliability when combined with CRC checks in OpenOCD. In practice, this reduces the
        * number of retries needed to read an entire block without corruption.
        */
        esp_rom_delay_us(100);
#endif
    }
    return res;
}

uint8_t *esp_apptrace_membufs_down_buffer_get(esp_apptrace_membufs_proto_data_t *proto,
                                              uint32_t *size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *ptr = NULL;

    while (1) {
        uint32_t sz = esp_apptrace_rb_read_size_get(&proto->rb_down);
        if (sz != 0) {
            *size = MIN(*size, sz);
            ptr = esp_apptrace_rb_consume(&proto->rb_down, *size);
            if (!ptr) {
                assert(false && "Failed to consume bytes from down buffer!");
            }
            break;
        }
        // may need to flush
        if (proto->hw->host_data_pending()) {
            ESP_APPTRACE_LOGD("force flush");
            int res = esp_apptrace_membufs_swap_waitus(proto, tmo);
            if (res != ESP_OK) {
                ESP_APPTRACE_LOGE("Failed to switch to another block to recv data from host!");
                /*do not return error because data can be in down buffer already*/
            }
        } else {
            // check tmo only if there is no data from host
            int res = esp_apptrace_tmo_check(tmo);
            if (res != ESP_OK) {
                return NULL;
            }
        }
    }
    return ptr;
}

esp_err_t esp_apptrace_membufs_down_buffer_put(esp_apptrace_membufs_proto_data_t *proto,
                                               uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    /* nothing todo */
    return ESP_OK;
}

static uint32_t esp_apptrace_membufs_down_buffer_write_nolock(esp_apptrace_membufs_proto_data_t *proto,
                                                              uint8_t *data, uint32_t size)
{
    uint32_t total_sz = 0;

    while (total_sz < size) {
        ESP_APPTRACE_LOGD("esp_apptrace_trax_down_buffer_write_nolock WRS %" PRIu32 "-%" PRIu32 "-%" PRIu32 " %" PRIu32, proto->rb_down.wr, proto->rb_down.rd,
                          proto->rb_down.cur_size, size);
        uint32_t wr_sz = esp_apptrace_rb_write_size_get(&proto->rb_down);
        if (wr_sz == 0) {
            break;
        }

        if (wr_sz > size - total_sz) {
            wr_sz = size - total_sz;
        }
        ESP_APPTRACE_LOGD("esp_apptrace_trax_down_buffer_write_nolock wr %" PRIu32, wr_sz);
        uint8_t *ptr = esp_apptrace_rb_produce(&proto->rb_down, wr_sz);
        if (!ptr) {
            assert(false && "Failed to produce bytes to down buffer!");
        }
        ESP_APPTRACE_LOGD("esp_apptrace_trax_down_buffer_write_nolock wr %" PRIu32 " to %p from %p", wr_sz, ptr, data + total_sz + wr_sz);
        memcpy(ptr, data + total_sz, wr_sz);
        total_sz += wr_sz;
        ESP_APPTRACE_LOGD("esp_apptrace_trax_down_buffer_write_nolock wr %" PRIu32 "/%" PRIu32 "", wr_sz, total_sz);
    }
    return total_sz;
}

static inline uint32_t esp_apptrace_membufs_usr_data_len_max(esp_apptrace_membufs_proto_data_t *proto)
{
    return proto->header_size == ESP_APPTRACE_HEADER_SIZE_32 ?
           ESP_APPTRACE_INBLOCK(proto)->sz - ESP_APPTRACE_HEADER_SIZE_32 : 255;
}

uint8_t *esp_apptrace_membufs_up_buffer_get(esp_apptrace_membufs_proto_data_t *proto,
                                            uint32_t size, esp_apptrace_tmo_t *tmo)
{
    if (size > esp_apptrace_membufs_usr_data_len_max(proto)) {
        ESP_APPTRACE_LOGE("Too large user data size %" PRIu32 "!", size);
        return NULL;
    }

    if (ESP_APPTRACE_INBLOCK_MARKER(proto) + size + proto->header_size > ESP_APPTRACE_INBLOCK(proto)->sz) {
        int res = esp_apptrace_membufs_swap_waitus(proto, tmo);
        if (res != ESP_OK) {
            return NULL;
        }
    }

    uint8_t *buf_ptr = ESP_APPTRACE_INBLOCK(proto)->start + ESP_APPTRACE_INBLOCK_MARKER(proto);
    // update cur block marker
    proto->state.markers[proto->state.in_block % 2] += size + proto->header_size;

    // update header
    esp_tracedata_hdr_t *hdr = (esp_tracedata_hdr_t *)buf_ptr;
    if (proto->header_size == ESP_APPTRACE_HEADER_SIZE_32) {
        hdr->block_sz_16 = (esp_cpu_get_core_id() << 15) | size;
        hdr->wr_sz_16 = 0;
    } else {
        hdr->block_sz_8 = size;
        hdr->wr_sz_8 = 0;
    }
    ESP_APPTRACE_LOGD("Got %" PRIu32 " bytes from  block", size);

    return buf_ptr + proto->header_size;
}

esp_err_t esp_apptrace_membufs_up_buffer_put(esp_apptrace_membufs_proto_data_t *proto,
                                             uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    // update header
    esp_tracedata_hdr_t *hdr = (esp_tracedata_hdr_t *)(ptr - proto->header_size);
    if (proto->header_size == ESP_APPTRACE_HEADER_SIZE_32) {
        hdr->wr_sz_16 = hdr->block_sz_16;
    } else {
        hdr->wr_sz_8 = hdr->block_sz_8;
    }
    // TODO: mark block as busy in order not to reuse it for other tracing calls until it is completely written
    // TODO: avoid potential situation when all memory is consumed by low prio tasks which can not complete writing due to
    // higher prio tasks and the latter can not allocate buffers at all
    // this is abnormal situation can be detected on host which will receive only uncompleted buffers
    // workaround: use own memcpy which will kick-off dead tracing calls
    return ESP_OK;
}

esp_err_t esp_apptrace_membufs_flush_nolock(esp_apptrace_membufs_proto_data_t *proto,
                                            uint32_t min_sz, esp_apptrace_tmo_t *tmo)
{
    int res = ESP_OK;

    if (ESP_APPTRACE_INBLOCK_MARKER(proto) < min_sz) {
        ESP_APPTRACE_LOGI("Ignore flush request for min %" PRIu32 " bytes. Bytes in  block: %" PRIu32, min_sz, ESP_APPTRACE_INBLOCK_MARKER(proto));
        return ESP_OK;
    }
    // switch  block while size of data is more than min size
    while (ESP_APPTRACE_INBLOCK_MARKER(proto) > min_sz) {
        ESP_APPTRACE_LOGD("Try to flush %" PRIu32 " bytes", ESP_APPTRACE_INBLOCK_MARKER(proto));
        res = esp_apptrace_membufs_swap_waitus(proto, tmo);
        if (res != ESP_OK) {
            if (res == ESP_ERR_TIMEOUT) {
                ESP_APPTRACE_LOGW("Failed to switch to another block in %" PRId32 " us!", (int32_t)tmo->elapsed);
            } else {
                ESP_APPTRACE_LOGE("Failed to switch to another block, res: %d", res);
            }
            return res;
        }
    }

    return res;
}
