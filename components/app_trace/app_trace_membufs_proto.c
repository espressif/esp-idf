/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
#if CONFIG_APPTRACE_SV_ENABLE
    uint8_t   block_sz; // size of allocated block for user data
    uint8_t   wr_sz;    // size of actually written data
#else
    uint16_t   block_sz; // size of allocated block for user data
    uint16_t   wr_sz;    // size of actually written data
#endif
} esp_tracedata_hdr_t;

/** TODO: docs
 */
typedef struct {
    uint16_t   block_sz; // size of allocated block for user data
} esp_hostdata_hdr_t;

#if CONFIG_APPTRACE_SV_ENABLE
#define ESP_APPTRACE_USR_BLOCK_CORE(_cid_)          (0)
#define ESP_APPTRACE_USR_BLOCK_LEN(_v_)             (_v_)
#define ESP_APPTRACE_USR_DATA_LEN_MAX(_hw_data_)    255UL
#else
#define ESP_APPTRACE_USR_BLOCK_CORE(_cid_)      ((_cid_) << 15)
#define ESP_APPTRACE_USR_BLOCK_LEN(_v_)         (~(1 << 15) & (_v_))
#define ESP_APPTRACE_USR_DATA_LEN_MAX(_hw_data_)       (ESP_APPTRACE_INBLOCK(_hw_data_)->sz - sizeof(esp_tracedata_hdr_t))
#endif
#define ESP_APPTRACE_USR_BLOCK_RAW_SZ(_s_)     ((_s_) + sizeof(esp_tracedata_hdr_t))

#define ESP_APPTRACE_INBLOCK_MARKER(_hw_data_)          ((_hw_data_)->state.markers[(_hw_data_)->state.in_block % 2])
#define ESP_APPTRACE_INBLOCK_MARKER_UPD(_hw_data_, _v_)   do {(_hw_data_)->state.markers[(_hw_data_)->state.in_block % 2] += (_v_);}while(0)
#define ESP_APPTRACE_INBLOCK(_hw_data_)             (&(_hw_data_)->blocks[(_hw_data_)->state.in_block % 2])

const static char *TAG = "esp_apptrace";

static uint32_t esp_apptrace_membufs_down_buffer_write_nolock(esp_apptrace_membufs_proto_data_t *proto, uint8_t *data, uint32_t size);


esp_err_t esp_apptrace_membufs_init(esp_apptrace_membufs_proto_data_t *proto, const esp_apptrace_mem_block_t blocks_cfg[2])
{
    // disabled by default
    esp_apptrace_rb_init(&proto->rb_down, NULL, 0);
    // membufs proto init
    for (unsigned i = 0; i < 2; i++) {
        proto->blocks[i].start = blocks_cfg[i].start;
        proto->blocks[i].sz = blocks_cfg[i].sz;
        proto->state.markers[i] = 0;
    }
    proto->state.in_block = 0;
#if CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    esp_apptrace_rb_init(&proto->rb_pend, proto->pending_data,
                        sizeof(proto->pending_data));
#endif
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
    esp_err_t res = ESP_OK;

    res = proto->hw->swap_start(proto->state.in_block);
    if (res != ESP_OK) {
        return res;
    }

    proto->state.markers[new_block_num] = 0;
    // switch to new block
    proto->state.in_block++;

    proto->hw->swap(new_block_num);

    // handle data from host
    esp_hostdata_hdr_t *hdr = (esp_hostdata_hdr_t *)proto->blocks[new_block_num].start;
    // ESP_APPTRACE_LOGV("Host data %d, sz %d @ %p", proto->hw->host_data_pending(), hdr->block_sz, hdr);
    if (proto->hw->host_data_pending() && hdr->block_sz > 0) {
        // TODO: add support for multiple blocks from host, currently there is no need for that
        uint8_t *p = proto->blocks[new_block_num].start + proto->blocks[new_block_num].sz;
        ESP_APPTRACE_LOGD("Recvd %" PRIu16 " bytes from host (@ %p) [%x %x %x %x %x %x %x %x .. %x %x %x %x %x %x %x %x]",
            hdr->block_sz, proto->blocks[new_block_num].start,
            *(proto->blocks[new_block_num].start+0), *(proto->blocks[new_block_num].start+1),
            *(proto->blocks[new_block_num].start+2), *(proto->blocks[new_block_num].start+3),
            *(proto->blocks[new_block_num].start+4), *(proto->blocks[new_block_num].start+5),
            *(proto->blocks[new_block_num].start+6), *(proto->blocks[new_block_num].start+7),
            *(p-8), *(p-7), *(p-6), *(p-5), *(p-4), *(p-3), *(p-2), *(p-1));
        uint32_t sz = esp_apptrace_membufs_down_buffer_write_nolock(proto, (uint8_t *)(hdr+1), hdr->block_sz);
        if (sz != hdr->block_sz) {
            ESP_APPTRACE_LOGE("Failed to write %" PRIu32 " bytes to down buffer (%" PRIu16 " %" PRIu32 ")!", hdr->block_sz - sz, hdr->block_sz, sz);
        }
        hdr->block_sz = 0;
    }
#if CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    // copy pending data to  block if any
    while (proto->state.markers[new_block_num] < proto->blocks[new_block_num].sz) {
        uint32_t read_sz = esp_apptrace_rb_read_size_get(&proto->rb_pend);
        if (read_sz == 0) {
            break; // no more data in pending buffer
        }
        if (read_sz > proto->blocks[new_block_num].sz - proto->state.markers[new_block_num]) {
            read_sz = proto->blocks[new_block_num].sz - proto->state.markers[new_block_num];
        }
        uint8_t *ptr = esp_apptrace_rb_consume(&proto->rb_pend, read_sz);
        if (!ptr) {
            assert(false && "Failed to consume pended bytes!!");
            break;
        }
        ESP_APPTRACE_LOGD("Pump %d pend bytes [%x %x %x %x : %x %x %x %x : %x %x %x %x : %x %x...%x %x]",
            read_sz, *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4),
            *(ptr+5), *(ptr+6), *(ptr+7), *(ptr+8), *(ptr+9), *(ptr+10), *(ptr+11), *(ptr+12), *(ptr+13), *(ptr+read_sz-2), *(ptr+read_sz-1));
        memcpy(proto->blocks[new_block_num].start + proto->state.markers[new_block_num], ptr, read_sz);
        proto->state.markers[new_block_num] += read_sz;
    }
#endif
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
    }
    return res;
}

uint8_t *esp_apptrace_membufs_down_buffer_get(esp_apptrace_membufs_proto_data_t *proto, uint32_t *size, esp_apptrace_tmo_t *tmo)
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

esp_err_t esp_apptrace_membufs_down_buffer_put(esp_apptrace_membufs_proto_data_t *proto, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    /* nothing todo */
    return ESP_OK;
}

static uint32_t esp_apptrace_membufs_down_buffer_write_nolock(esp_apptrace_membufs_proto_data_t *proto, uint8_t *data, uint32_t size)
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

static inline uint8_t *esp_apptrace_membufs_wait4buf(esp_apptrace_membufs_proto_data_t *proto, uint16_t size, esp_apptrace_tmo_t *tmo, int *pended)
{
    uint8_t *ptr = NULL;

    int res = esp_apptrace_membufs_swap_waitus(proto, tmo);
    if (res != ESP_OK) {
        return NULL;
    }
#if CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    // check if we still have pending data
    if (esp_apptrace_rb_read_size_get(&proto->rb_pend) > 0) {
        // if after block switch we still have pending data (not all pending data have been pumped to block)
        // alloc new pending buffer
        *pended = 1;
        ptr = esp_apptrace_rb_produce(&proto->rb_pend, size);
        if (!ptr) {
            ESP_APPTRACE_LOGE("Failed to alloc pend buf 1: w-r-s %d-%d-%d!", proto->rb_pend.wr, proto->rb_pend.rd, proto->rb_pend.cur_size);
        }
    } else
#endif
    {
        // update block pointers
        if (ESP_APPTRACE_INBLOCK_MARKER(proto) + size > ESP_APPTRACE_INBLOCK(proto)->sz) {
#if CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX > 0
            *pended = 1;
            ptr = esp_apptrace_rb_produce(&proto->rb_pend, size);
            if (ptr == NULL) {
                ESP_APPTRACE_LOGE("Failed to alloc pend buf 2: w-r-s %d-%d-%d!", proto->rb_pend.wr, proto->rb_pend.rd, proto->rb_pend.cur_size);
            }
#endif
        } else {
            *pended = 0;
            ptr = ESP_APPTRACE_INBLOCK(proto)->start + ESP_APPTRACE_INBLOCK_MARKER(proto);
        }
    }

    return ptr;
}

static inline uint8_t *esp_apptrace_membufs_pkt_start(uint8_t *ptr, uint16_t size)
{
    // it is safe to use esp_cpu_get_core_id() in macro call because arg is used only once inside it
    ((esp_tracedata_hdr_t *)ptr)->block_sz = ESP_APPTRACE_USR_BLOCK_CORE(esp_cpu_get_core_id()) | size;
    ((esp_tracedata_hdr_t *)ptr)->wr_sz = 0;
    return ptr + sizeof(esp_tracedata_hdr_t);
}

static inline void esp_apptrace_membufs_pkt_end(uint8_t *ptr)
{
    esp_tracedata_hdr_t *hdr = (esp_tracedata_hdr_t *)(ptr - sizeof(esp_tracedata_hdr_t));
    // update written size
    hdr->wr_sz = hdr->block_sz;
}

uint8_t *esp_apptrace_membufs_up_buffer_get(esp_apptrace_membufs_proto_data_t *proto, uint32_t size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *buf_ptr = NULL;

    if (size > ESP_APPTRACE_USR_DATA_LEN_MAX(proto)) {
        ESP_APPTRACE_LOGE("Too large user data size %" PRIu32 "!", size);
        return NULL;
    }

    // check for data in the pending buffer
#if CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    if (esp_apptrace_rb_read_size_get(&proto->rb_pend) > 0) {
        // if we have buffered data try to switch  block
        esp_apptrace_membufs_swap(proto);
        // if switch was successful, part or all pended data have been copied to  block
    }
    if (esp_apptrace_rb_read_size_get(&proto->rb_pend) > 0) {
        // if we have buffered data alloc new pending buffer
        ESP_APPTRACE_LOGD("Get %d bytes from PEND buffer", size);
        buf_ptr = esp_apptrace_rb_produce(&proto->rb_pend, ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
        if (buf_ptr == NULL) {
            int pended_buf;
            buf_ptr = esp_apptrace_membufs_wait4buf(proto, ESP_APPTRACE_USR_BLOCK_RAW_SZ(size), tmo, &pended_buf);
            if (buf_ptr && !pended_buf) {
                ESP_APPTRACE_LOGD("Get %d bytes from block", size);
                // update cur block marker
                ESP_APPTRACE_INBLOCK_MARKER_UPD(proto, ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
            }
        }
    } else {
#else
    if (1) {
#endif
        if (ESP_APPTRACE_INBLOCK_MARKER(proto) + ESP_APPTRACE_USR_BLOCK_RAW_SZ(size) > ESP_APPTRACE_INBLOCK(proto)->sz) {
            #if CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX > 0
            ESP_APPTRACE_LOGD("Block full. Get %" PRIu32 " bytes from PEND buffer", size);
            buf_ptr = esp_apptrace_rb_produce(&proto->rb_pend, ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
            #endif
            if (buf_ptr == NULL) {
                int pended_buf;
                ESP_APPTRACE_LOGD(" full. Get %" PRIu32 " bytes from pend buffer", size);
                buf_ptr = esp_apptrace_membufs_wait4buf(proto, ESP_APPTRACE_USR_BLOCK_RAW_SZ(size), tmo, &pended_buf);
                if (buf_ptr && !pended_buf) {
                    ESP_APPTRACE_LOGD("Got %" PRIu32 " bytes from block", size);
                    // update cur block marker
                    ESP_APPTRACE_INBLOCK_MARKER_UPD(proto, ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
                }
            }
        } else {
            ESP_APPTRACE_LOGD("Get %" PRIu32 " bytes from  buffer", size);
            // fit to curr  nlock
            buf_ptr = ESP_APPTRACE_INBLOCK(proto)->start + ESP_APPTRACE_INBLOCK_MARKER(proto);
            // update cur block marker
            ESP_APPTRACE_INBLOCK_MARKER_UPD(proto, ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
        }
    }
    if (buf_ptr) {
        buf_ptr = esp_apptrace_membufs_pkt_start(buf_ptr, size);
    }

    return buf_ptr;
}

esp_err_t esp_apptrace_membufs_up_buffer_put(esp_apptrace_membufs_proto_data_t *proto, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_membufs_pkt_end(ptr);
    // TODO: mark block as busy in order not to re-use it for other tracing calls until it is completely written
    // TODO: avoid potential situation when all memory is consumed by low prio tasks which can not complete writing due to
    // higher prio tasks and the latter can not allocate buffers at all
    // this is abnormal situation can be detected on host which will receive only uncompleted buffers
    // workaround: use own memcpy which will kick-off dead tracing calls
    return ESP_OK;
}

esp_err_t esp_apptrace_membufs_flush_nolock(esp_apptrace_membufs_proto_data_t *proto, uint32_t min_sz, esp_apptrace_tmo_t *tmo)
{
    int res = ESP_OK;

    if (ESP_APPTRACE_INBLOCK_MARKER(proto) < min_sz) {
        ESP_APPTRACE_LOGI("Ignore flush request for min %" PRIu32 " bytes. Bytes in  block: %" PRIu32, min_sz, ESP_APPTRACE_INBLOCK_MARKER(proto));
        return ESP_OK;
    }
    // switch  block while size of data (including that in pending buffer) is more than min size
    while (ESP_APPTRACE_INBLOCK_MARKER(proto) > min_sz) {
        ESP_APPTRACE_LOGD("Try to flush %" PRIu32 " bytes. Wait until block switch for %" PRIi64 " us", ESP_APPTRACE_INBLOCK_MARKER(proto), tmo->tmo);
        res = esp_apptrace_membufs_swap_waitus(proto, tmo);
        if (res != ESP_OK) {
            if (tmo->tmo != ESP_APPTRACE_TMO_INFINITE)
                ESP_APPTRACE_LOGW("Failed to switch to another block in %lld us!", tmo->tmo);
            else
                ESP_APPTRACE_LOGE("Failed to switch to another block in %lld us!", tmo->tmo);
            return res;
        }
    }

    return res;
}
