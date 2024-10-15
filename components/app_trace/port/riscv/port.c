/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include "sdkconfig.h"
#include "esp_cpu.h"
#include "esp_log.h"
#include "esp_app_trace_membufs_proto.h"
#include "esp_app_trace_port.h"
#include "riscv/semihosting.h"

/** RISCV HW transport data */
typedef struct {
    uint8_t                             inited; // initialization state flags for every core
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_apptrace_lock_t                 lock;   // sync lock
#endif
    esp_apptrace_membufs_proto_data_t   membufs;
} esp_apptrace_riscv_data_t;

/** RISCV memory host iface control block */
typedef struct {
    uint32_t                    ctrl;
    // - Guard field. If this register is not zero then CPU is changing this struct and
    //   this guard field holds address of the instruction which application will execute when CPU finishes with those modifications.
    uint32_t                    stat;
    esp_apptrace_mem_block_t *  mem_blocks;
} esp_apptrace_riscv_ctrl_block_t;

#define ESP_APPTRACE_RISCV_BLOCK_LEN_MSK         0x7FFFUL
#define ESP_APPTRACE_RISCV_BLOCK_LEN(_l_)        ((_l_) & ESP_APPTRACE_RISCV_BLOCK_LEN_MSK)
#define ESP_APPTRACE_RISCV_BLOCK_LEN_GET(_v_)    ((_v_) & ESP_APPTRACE_RISCV_BLOCK_LEN_MSK)
#define ESP_APPTRACE_RISCV_BLOCK_ID_MSK          0x7FUL
#define ESP_APPTRACE_RISCV_BLOCK_ID(_id_)        (((_id_) & ESP_APPTRACE_RISCV_BLOCK_ID_MSK) << 15)
#define ESP_APPTRACE_RISCV_BLOCK_ID_GET(_v_)     (((_v_) >> 15) & ESP_APPTRACE_RISCV_BLOCK_ID_MSK)
#define ESP_APPTRACE_RISCV_HOST_DATA             (1 << 22)
#define ESP_APPTRACE_RISCV_HOST_CONNECT          (1 << 23)

#define ESP_APPTRACE_RISCV_INITED(_hw_)          ((_hw_)->inited & (1 << 0/*esp_cpu_get_core_id()*/))

static esp_err_t esp_apptrace_riscv_init(esp_apptrace_riscv_data_t *hw_data);
static esp_err_t esp_apptrace_riscv_flush(esp_apptrace_riscv_data_t *hw_data, esp_apptrace_tmo_t *tmo);
static esp_err_t esp_apptrace_riscv_flush_nolock(esp_apptrace_riscv_data_t *hw_data, uint32_t min_sz, esp_apptrace_tmo_t *tmo);
static uint8_t *esp_apptrace_riscv_up_buffer_get(esp_apptrace_riscv_data_t *hw_data, uint32_t size, esp_apptrace_tmo_t *tmo);
static esp_err_t esp_apptrace_riscv_up_buffer_put(esp_apptrace_riscv_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
static void esp_apptrace_riscv_down_buffer_config(esp_apptrace_riscv_data_t *hw_data, uint8_t *buf, uint32_t size);
static uint8_t *esp_apptrace_riscv_down_buffer_get(esp_apptrace_riscv_data_t *hw_data, uint32_t *size, esp_apptrace_tmo_t *tmo);
static esp_err_t esp_apptrace_riscv_down_buffer_put(esp_apptrace_riscv_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
static bool esp_apptrace_riscv_host_is_connected(esp_apptrace_riscv_data_t *hw_data);
static esp_err_t esp_apptrace_riscv_buffer_swap_start(uint32_t curr_block_id);
static esp_err_t esp_apptrace_riscv_buffer_swap(uint32_t new_block_id);
static esp_err_t esp_apptrace_riscv_buffer_swap_end(uint32_t new_block_id, uint32_t prev_block_len);
static bool esp_apptrace_riscv_host_data_pending(void);


const static char *TAG = "esp_apptrace";

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#define APPTRACE_DRAM_ATTR TCM_DRAM_ATTR
#else
#define APPTRACE_DRAM_ATTR
#endif

static APPTRACE_DRAM_ATTR esp_apptrace_riscv_ctrl_block_t s_tracing_ctrl[CONFIG_FREERTOS_NUMBER_OF_CORES];

esp_apptrace_hw_t *esp_apptrace_jtag_hw_get(void **data)
{
#if CONFIG_APPTRACE_DEST_JTAG
    static esp_apptrace_membufs_proto_hw_t s_trace_proto_hw = {
        .swap_start = esp_apptrace_riscv_buffer_swap_start,
        .swap = esp_apptrace_riscv_buffer_swap,
        .swap_end = esp_apptrace_riscv_buffer_swap_end,
        .host_data_pending = esp_apptrace_riscv_host_data_pending,
    };
    static esp_apptrace_riscv_data_t s_trace_hw_data = {
        .membufs = {
            .hw = &s_trace_proto_hw,
        },
    };
    static esp_apptrace_hw_t s_trace_hw = {
        .init = (esp_err_t (*)(void *))esp_apptrace_riscv_init,
        .get_up_buffer = (uint8_t *(*)(void *, uint32_t, esp_apptrace_tmo_t *))esp_apptrace_riscv_up_buffer_get,
        .put_up_buffer = (esp_err_t (*)(void *, uint8_t *, esp_apptrace_tmo_t *))esp_apptrace_riscv_up_buffer_put,
        .flush_up_buffer_nolock = (esp_err_t (*)(void *, uint32_t, esp_apptrace_tmo_t *))esp_apptrace_riscv_flush_nolock,
        .flush_up_buffer = (esp_err_t (*)(void *, esp_apptrace_tmo_t *))esp_apptrace_riscv_flush,
        .down_buffer_config = (void (*)(void *, uint8_t *, uint32_t ))esp_apptrace_riscv_down_buffer_config,
        .get_down_buffer = (uint8_t *(*)(void *, uint32_t *, esp_apptrace_tmo_t *))esp_apptrace_riscv_down_buffer_get,
        .put_down_buffer = (esp_err_t (*)(void *, uint8_t *, esp_apptrace_tmo_t *))esp_apptrace_riscv_down_buffer_put,
        .host_is_connected = (bool (*)(void *))esp_apptrace_riscv_host_is_connected,
    };
    *data = &s_trace_hw_data;
    return &s_trace_hw;
#else
    return NULL;
#endif
}

/* Advertises apptrace control block address to host.
   This function can be overridden with custom implementation,
   e.g. OpenOCD flasher stub use own implementation of it. */
__attribute__((weak)) int esp_apptrace_advertise_ctrl_block(void *ctrl_block_addr)
{
    if (!esp_cpu_dbgr_is_attached()) {
        return 0;
    }
    return (int) semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_APPTRACE_INIT, (long*)ctrl_block_addr);
}

/* Returns up buffers config.
   This function can be overridden with custom implementation,
   e.g. OpenOCD flasher stub use own implementation of it. */
__attribute__((weak)) void esp_apptrace_get_up_buffers(esp_apptrace_mem_block_t mem_blocks_cfg[2])
{
    static uint8_t s_mem_blocks[2][CONFIG_APPTRACE_BUF_SIZE];

    mem_blocks_cfg[0].start = s_mem_blocks[0];
    mem_blocks_cfg[0].sz = CONFIG_APPTRACE_BUF_SIZE;
    mem_blocks_cfg[1].start = s_mem_blocks[1];
    mem_blocks_cfg[1].sz = CONFIG_APPTRACE_BUF_SIZE;
}

static esp_err_t esp_apptrace_riscv_lock(esp_apptrace_riscv_data_t *hw_data, esp_apptrace_tmo_t *tmo)
{
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_err_t ret = esp_apptrace_lock_take(&hw_data->lock, tmo);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
#endif
    return ESP_OK;
}

static esp_err_t esp_apptrace_riscv_unlock(esp_apptrace_riscv_data_t *hw_data)
{
    esp_err_t ret = ESP_OK;
#if CONFIG_APPTRACE_LOCK_ENABLE
    ret = esp_apptrace_lock_give(&hw_data->lock);
#endif
    return ret;
}

/*****************************************************************************************/
/***************************** Apptrace HW iface *****************************************/
/*****************************************************************************************/

static esp_err_t esp_apptrace_riscv_init(esp_apptrace_riscv_data_t *hw_data)
{
    int core_id = esp_cpu_get_core_id();

    if (hw_data->inited == 0) {
        esp_apptrace_mem_block_t mem_blocks_cfg[2];
        esp_apptrace_get_up_buffers(mem_blocks_cfg);
        esp_err_t res = esp_apptrace_membufs_init(&hw_data->membufs, mem_blocks_cfg);
        if (res != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to init membufs proto (%d)!", res);
            return res;
        }
#if CONFIG_APPTRACE_LOCK_ENABLE
        esp_apptrace_lock_init(&hw_data->lock);
#endif
    }
    hw_data->inited |= 1 << core_id;
    ESP_APPTRACE_LOGI("Apptrace initialized on CPU%d. Tracing control block @ %p.", core_id, &s_tracing_ctrl[core_id]);
    s_tracing_ctrl[core_id].mem_blocks = hw_data->membufs.blocks;
    for (int i = 0; i < 2; i++) {
        ESP_APPTRACE_LOGD("Mem buf[%d] %" PRIu32 " bytes @ %p (%p/%p)", i,
            s_tracing_ctrl[core_id].mem_blocks[i].sz, s_tracing_ctrl[core_id].mem_blocks[i].start,
            &(s_tracing_ctrl[core_id].mem_blocks[i].start), &(s_tracing_ctrl[core_id].mem_blocks[i].sz));
    }
    // notify host about control block address
    int res = esp_apptrace_advertise_ctrl_block(&s_tracing_ctrl[core_id]);
    assert(res == 0 && "Failed to send config to host!");

    return ESP_OK;
}

static uint8_t *esp_apptrace_riscv_up_buffer_get(esp_apptrace_riscv_data_t *hw_data, uint32_t size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *ptr;

    if (!ESP_APPTRACE_RISCV_INITED(hw_data)) {
        return NULL;
    }
    esp_err_t res = esp_apptrace_riscv_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return NULL;
    }

    ptr = esp_apptrace_membufs_up_buffer_get(&hw_data->membufs, size, tmo);

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_riscv_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return ptr;
}

static esp_err_t esp_apptrace_riscv_up_buffer_put(esp_apptrace_riscv_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    if (!ESP_APPTRACE_RISCV_INITED(hw_data)) {
        return ESP_ERR_INVALID_STATE;
    }
    // Can avoid locking because esp_apptrace_membufs_up_buffer_put() just modifies buffer's header
    esp_err_t res = esp_apptrace_membufs_up_buffer_put(&hw_data->membufs, ptr, tmo);
    return res;
}

static void esp_apptrace_riscv_down_buffer_config(esp_apptrace_riscv_data_t *hw_data, uint8_t *buf, uint32_t size)
{
    if (!ESP_APPTRACE_RISCV_INITED(hw_data)) {
        return;
    }
    esp_apptrace_membufs_down_buffer_config(&hw_data->membufs, buf, size);
}

static uint8_t *esp_apptrace_riscv_down_buffer_get(esp_apptrace_riscv_data_t *hw_data, uint32_t *size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *ptr;

    if (!ESP_APPTRACE_RISCV_INITED(hw_data)) {
        return NULL;
    }
    esp_err_t res = esp_apptrace_riscv_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return NULL;
    }

    ptr = esp_apptrace_membufs_down_buffer_get(&hw_data->membufs, size, tmo);

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_riscv_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return ptr;
}

static esp_err_t esp_apptrace_riscv_down_buffer_put(esp_apptrace_riscv_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    if (!ESP_APPTRACE_RISCV_INITED(hw_data)) {
        return ESP_ERR_INVALID_STATE;
    }
    // Can avoid locking because esp_apptrace_membufs_down_buffer_put() does nothing
    /*esp_err_t res = esp_apptrace_riscv_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return res;
    }*/

    esp_err_t res = esp_apptrace_membufs_down_buffer_put(&hw_data->membufs, ptr, tmo);

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    /*if (esp_apptrace_riscv_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }*/
    return res;
}

static bool esp_apptrace_riscv_host_is_connected(esp_apptrace_riscv_data_t *hw_data)
{
    if (!ESP_APPTRACE_RISCV_INITED(hw_data)) {
        return false;
    }
    return s_tracing_ctrl[esp_cpu_get_core_id()].ctrl & ESP_APPTRACE_RISCV_HOST_CONNECT ? true : false;
}

static esp_err_t esp_apptrace_riscv_flush_nolock(esp_apptrace_riscv_data_t *hw_data, uint32_t min_sz, esp_apptrace_tmo_t *tmo)
{
    if (!ESP_APPTRACE_RISCV_INITED(hw_data)) {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_apptrace_membufs_flush_nolock(&hw_data->membufs, min_sz, tmo);
}

static esp_err_t esp_apptrace_riscv_flush(esp_apptrace_riscv_data_t *hw_data, esp_apptrace_tmo_t *tmo)
{
    if (!ESP_APPTRACE_RISCV_INITED(hw_data)) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t res = esp_apptrace_riscv_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return res;
    }

    res = esp_apptrace_membufs_flush_nolock(&hw_data->membufs, 0, tmo);

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_riscv_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return res;
}

/*****************************************************************************************/
/************************** Membufs proto HW iface ***************************************/
/*****************************************************************************************/

static inline void esp_apptrace_riscv_buffer_swap_lock(void)
{
    extern uint32_t __esp_apptrace_riscv_updated;

    // indicate to host that we are about to update.
    // this is used only to place CPU into streaming mode at tracing startup
    // before starting streaming host can halt us after we read  ESP_APPTRACE_RISCV_CTRL_REG and before we updated it
    // HACK: in this case host will set breakpoint just after ESP_APPTRACE_RISCV_CTRL_REG update,
    // here we set address to set bp at
    // enter ERI update critical section
    s_tracing_ctrl[esp_cpu_get_core_id()].stat = (uint32_t)&__esp_apptrace_riscv_updated;
}

static __attribute__((noinline)) void esp_apptrace_riscv_buffer_swap_unlock(void)
{
    // exit ERI update critical section
    s_tracing_ctrl[esp_cpu_get_core_id()].stat = 0;
    // TODO: currently host sets breakpoint, use break instruction to stop;
    // it will allow to use ESP_APPTRACE_RISCV_STAT_REG for other purposes
    asm volatile (
        "    .global     __esp_apptrace_riscv_updated\n"
        "__esp_apptrace_riscv_updated:\n"); // host will set bp here to resolve collision at streaming start
}

static esp_err_t esp_apptrace_riscv_buffer_swap_start(uint32_t curr_block_id)
{
    esp_err_t res = ESP_OK;

    esp_apptrace_riscv_buffer_swap_lock();

    uint32_t ctrl_reg = s_tracing_ctrl[esp_cpu_get_core_id()].ctrl;
    uint32_t host_connected = ESP_APPTRACE_RISCV_HOST_CONNECT & ctrl_reg;
    if (host_connected) {
        uint32_t acked_block = ESP_APPTRACE_RISCV_BLOCK_ID_GET(ctrl_reg);
        uint32_t host_to_read = ESP_APPTRACE_RISCV_BLOCK_LEN_GET(ctrl_reg);
        if (host_to_read != 0 || acked_block != (curr_block_id & ESP_APPTRACE_RISCV_BLOCK_ID_MSK)) {
            ESP_APPTRACE_LOGD("[%d]: Can not switch %" PRIx32 " %" PRIu32 " %" PRIx32 " %" PRIx32 "/%" PRIx32, esp_cpu_get_core_id(), ctrl_reg, host_to_read, acked_block,
                curr_block_id & ESP_APPTRACE_RISCV_BLOCK_ID_MSK, curr_block_id);
            res = ESP_ERR_NO_MEM;
            goto _on_err;
        }
    }
    return ESP_OK;
_on_err:
    esp_apptrace_riscv_buffer_swap_unlock();
    return res;
}

static esp_err_t esp_apptrace_riscv_buffer_swap_end(uint32_t new_block_id, uint32_t prev_block_len)
{
    uint32_t ctrl_reg = s_tracing_ctrl[esp_cpu_get_core_id()].ctrl;
    uint32_t host_connected = ESP_APPTRACE_RISCV_HOST_CONNECT & ctrl_reg;
    s_tracing_ctrl[esp_cpu_get_core_id()].ctrl = ESP_APPTRACE_RISCV_BLOCK_ID(new_block_id) |
              host_connected | ESP_APPTRACE_RISCV_BLOCK_LEN(prev_block_len);
    esp_apptrace_riscv_buffer_swap_unlock();
    return ESP_OK;
}

static esp_err_t esp_apptrace_riscv_buffer_swap(uint32_t new_block_id)
{
    /* do nothing */
    return ESP_OK;
}

static bool esp_apptrace_riscv_host_data_pending(void)
{
    uint32_t ctrl_reg = s_tracing_ctrl[esp_cpu_get_core_id()].ctrl;
    // ESP_APPTRACE_LOGV("%s() 0x%x", __func__, ctrl_reg);
    return (ctrl_reg & ESP_APPTRACE_RISCV_HOST_DATA) ? true : false;
}
