/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

//
// How It Works
// ************

// 1. Components Overview
// ======================

// Xtensa has useful feature: TRAX debug module. It allows recording program execution flow at run-time without disturbing CPU.
// Exectution flow data are written to configurable Trace RAM block. Besides accessing Trace RAM itself TRAX module also allows to read/write
// trace memory via its registers by means of JTAG, APB or ERI transactions.
// ESP32 has two Xtensa cores with separate TRAX modules on them and provides two special memory regions to be used as trace memory.
// Chip allows muxing access to those trace memory blocks in such a way that while one block is accessed by CPUs another one can be accessed by host
// by means of reading/writing TRAX registers via JTAG. Blocks muxing is configurable at run-time and allows switching trace memory blocks between
// accessors in round-robin fashion so they can read/write separate memory blocks without disturbing each other.
// This module implements application tracing feature based on above mechanisms. It allows to transfer arbitrary user data to/from
// host via JTAG with minimal impact on system performance. This module is implied to be used in the following tracing scheme.

//                                                        ------>------                                         ----- (host components) -----
//                                                        |           |                                         |                           |
// -------------------   -----------------------     -----------------------     ----------------    ------     ---------   -----------------
// |trace data source|-->|target tracing module|<--->|TRAX_MEM0 | TRAX_MEM1|---->|TRAX_DATA_REGS|<-->|JTAG|<--->|OpenOCD|-->|trace data sink|
// -------------------   -----------------------     -----------------------     ----------------    ------     ---------   -----------------
//                                 |                      |           |                                |
//                                 |                      ------<------          ----------------      |
//                                 |<------------------------------------------->|TRAX_CTRL_REGS|<---->|
//                                                                               ----------------

// In general tracing goes in the following way. User application requests tracing module to send some data by calling esp_apptrace_buffer_get(),
// module allocates necessary buffer in current input trace block. Then user fills received buffer with data and calls esp_apptrace_buffer_put().
// When current input trace block is filled with app data it is exposed to host and the second block becomes input one and buffer filling restarts.
// While target application fills one TRAX block host reads another one via JTAG.
// This module also allows communication in the opposite direction: from host to target. As it was said ESP32 and host can access different TRAX blocks
// simultaneously, so while target writes trace data to one block host can write its own data (e.g. tracing commands) to another one then when
// blocks are switched host receives trace data and target receives data written by host application. Target user application can read host data
// by calling esp_apptrace_read() API.
// To control buffer switching and for other communication purposes this implementation uses some TRAX registers. It is safe since HW TRAX tracing
// can not be used along with application tracing feature so these registers are freely readable/writeable via JTAG from host and via ERI from ESP32 cores.
// Overhead of this implementation on target CPU is produced only by allocating/managing buffers and copying of data.
// On the host side special OpenOCD command must be used to read trace data.

// 2. TRAX Registers layout
// ========================

// This module uses two TRAX HW registers to communicate with host SW (OpenOCD).
//  - Control register uses TRAX_DELAYCNT as storage. Only lower 24 bits of TRAX_DELAYCNT are writable. Control register has the following bitfields:
//   | 31..XXXXXX..24 | 23 .(host_connect). 23| 22..(block_id)..15 | 14..(block_len)..0 |
//    14..0  bits - actual length of user data in trace memory block. Target updates it every time it fills memory block and exposes it to host.
//                  Host writes zero to this field when it finishes reading exposed block;
//    21..15 bits - trace memory block transfer ID. Block counter. It can overflow. Updated by target, host should not modify it. Actually can be 2 bits;
//    22     bit  - 'host data present' flag. If set to one there is data from host, otherwise - no host data;
//    23     bit  - 'host connected' flag. If zero then host is not connected and tracing module works in post-mortem mode, otherwise in streaming mode;
// - Status register uses TRAX_TRIGGERPC as storage. If this register is not zero then current CPU is changing TRAX registers and
//   this register holds address of the instruction which application will execute when it finishes with those registers modifications.
//   See 'Targets Connection' setion for details.

// 3. Modes of operation
// =====================

// This module supports two modes of operation:
//  - Post-mortem mode. This is the default mode. In this mode application tracing module does not check whether host has read all the data from block
//    exposed to it and switches block in any case. The mode does not need host interaction for operation and so can be useful when only the latest
//    trace data are necessary, e.g. for analyzing crashes. On panic the latest data from current input block are exposed to host and host can read them.
//    It can happen that system panic occurs when there are very small amount of data which are not exposed to host yet (e.g. crash just after the
//    TRAX block switch). In this case the previous 16KB of collected data will be dropped and host will see the latest, but very small piece of trace.
//    It can be insufficient to diagnose the problem. To avoid such situations there is menuconfig option
//    CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH
//    which controls the threshold for flushing data in case of panic.
//  - Streaming mode. Tracing module enters this mode when host connects to target and sets respective bits in control registers (per core).
//    In this mode before switching the block tracing module waits for the host to read all the data from the previously exposed block.
//    On panic tracing module also waits (timeout is configured via menuconfig via CONFIG_APPTRACE_ONPANIC_HOST_FLUSH_TMO) for the host to read all data.

// 4. Communication Protocol
// =========================

// 4.1 Trace Memory Blocks
// -----------------------

// Communication is controlled via special register. Host periodically polls control register on each core to find out if there are any data available.
// When current input memory block is filled it is exposed to host and 'block_len' and 'block_id' fields are updated in the control register.
// Host reads new register value and according to it's value starts reading data from exposed block. Meanwhile target starts filling another trace block.
// When host finishes reading the block it clears 'block_len' field in control register indicating to the target that it is ready to accept the next one.
// If the host has some data to transfer to the target it writes them to trace memory block before clearing 'block_len' field. Then it sets
// 'host_data_present' bit and clears 'block_len' field in control register. Upon every block switch target checks 'host_data_present' bit and if it is set
// reads them to down buffer before writing any trace data to switched TRAX block.

// 4.2 User Data Chunks Level
// --------------------------

// Since trace memory block is shared between user data chunks and data copying is performed on behalf of the API user (in its normal context) in
// multithreading environment it can happen that task/ISR which copies data is preempted by another high prio task/ISR. So it is possible situation
// that task/ISR will fail to complete filling its data chunk before the whole trace block is exposed to the host. To handle such conditions tracing
// module prepends all user data chunks with header which contains allocated buffer size and actual data length within it. OpenOCD command
// which reads application traces reports error when it reads incomplete user data block.
// Data which are transffered from host to target are also prepended with a header. Down channel data header is simple and consists of one two bytes field
// containing length of host data following the header.

// 4.3 Data Buffering
// ------------------

// It takes some time for the host to read TRAX memory block via JTAG. In streaming mode it can happen that target has filled its TRAX block, but host
// has not completed reading of the previous one yet. So in this case time critical tracing calls (which can not be delayed for too long time due to
// the lack of free memory in TRAX block) can be dropped. To avoid such scenarios tracing module implements data buffering. Buffered data will be sent
// to the host later when TRAX block switch occurs. The maximum size of the buffered data is controlled by menuconfig option
// CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX.

// 4.4 Target Connection/Disconnection
// -----------------------------------

// When host is going to start tracing in streaming mode it needs to put both ESP32 cores into initial state when 'host connected' bit is set
// on both cores. To accomplish this host halts both cores and sets this bit in TRAX registers. But target code can be halted in state when it has read control
// register but has not updated its value. To handle such situations target code indicates to the host that it is updating control register by writing
// non-zero value to status register. Actually it writes address of the instruction which it will execute when it finishes with
// the registers update. When target is halted during control register update host sets breakpoint at the address from status register and resumes CPU.
// After target code finishes with register update it is halted on breakpoint, host detects it and safely sets 'host connected' bit. When both cores
// are set up they are resumed. Tracing starts without further intrusion into CPUs work.
// When host is going to stop tracing in streaming mode it needs to disconnect targets. Disconnection process is done using the same algorithm
// as for connecting, but 'host connected' bits are cleared on ESP32 cores.

// 5. Module Access Synchronization
// ================================

// Access to internal module's data is synchronized with custom mutex. Mutex is a wrapper for portMUX_TYPE and uses almost the same sync mechanism as in
// vPortCPUAcquireMutex/vPortCPUReleaseMutex. The mechanism uses S32C1I Xtensa instruction to implement exclusive access to module's data from tasks and
// ISRs running on both cores. Also custom mutex allows specifying timeout for locking operation. Locking routine checks underlaying mutex in cycle until
// it gets its ownership or timeout expires. The differences of application tracing module's mutex implementation from vPortCPUAcquireMutex/vPortCPUReleaseMutex are:
// - Support for timeouts.
// - Local IRQs for CPU which owns the mutex are disabled till the call to unlocking routine. This is made to avoid possible task's prio inversion.
//   When low prio task takes mutex and enables local IRQs gets preempted by high prio task which in its turn can try to acquire mutex using infinite timeout.
//   So no local task switch occurs when mutex is locked. But this does not apply to tasks on another CPU.
//   WARNING: Priority inversion can happen when low prio task works on one CPU and medium and high prio tasks work on another.
// WARNING: Care must be taken when selecting timeout values for trace calls from ISRs. Tracing module does not care about watchdogs when waiting
// on internal locks and for host to complete previous block reading, so if timeout value exceeds watchdog's one it can lead to the system reboot.

// 6. Timeouts
// ===========

// Timeout mechanism is based on xthal_get_ccount() routine and supports timeout values in microseconds.
// There are two situations when task/ISR can be delayed by tracing API call. Timeout mechanism takes into account both conditions:
// - Trace data are locked by another task/ISR. When wating on trace data lock.
// - Current TRAX memory input block is full when working in streaming mode (host is connected). When waiting for host to complete previous block reading.
// When wating for any of above conditions xthal_get_ccount() is called periodically to calculate time elapsed from trace API routine entry. When elapsed
// time exceeds specified timeout value operation is canceled and ESP_ERR_TIMEOUT code is returned.
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "soc/tracemem_config.h"
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#include "soc/sensitive_reg.h"
#endif
#include "eri.h"
#include "esp_private/trax.h"
#include "esp_cpu.h"
#include "esp_log.h"
#include "esp_app_trace_membufs_proto.h"
#include "esp_app_trace_port.h"

// TRAX is disabled, so we use its registers for our own purposes
// | 31..XXXXXX..24 | 23 .(host_connect). 23 | 22 .(host_data). 22| 21..(block_id)..15 | 14..(block_len)..0 |
#define ESP_APPTRACE_TRAX_CTRL_REG              ERI_TRAX_DELAYCNT
#define ESP_APPTRACE_TRAX_STAT_REG              ERI_TRAX_TRIGGERPC

#define ESP_APPTRACE_TRAX_BLOCK_LEN_MSK         0x7FFFUL
#define ESP_APPTRACE_TRAX_BLOCK_LEN(_l_)        ((_l_) & ESP_APPTRACE_TRAX_BLOCK_LEN_MSK)
#define ESP_APPTRACE_TRAX_BLOCK_LEN_GET(_v_)    ((_v_) & ESP_APPTRACE_TRAX_BLOCK_LEN_MSK)
#define ESP_APPTRACE_TRAX_BLOCK_ID_MSK          0x7FUL
#define ESP_APPTRACE_TRAX_BLOCK_ID(_id_)        (((_id_) & ESP_APPTRACE_TRAX_BLOCK_ID_MSK) << 15)
#define ESP_APPTRACE_TRAX_BLOCK_ID_GET(_v_)     (((_v_) >> 15) & ESP_APPTRACE_TRAX_BLOCK_ID_MSK)
#define ESP_APPTRACE_TRAX_HOST_DATA             (1 << 22)
#define ESP_APPTRACE_TRAX_HOST_CONNECT          (1 << 23)

#define ESP_APPTRACE_TRAX_INITED(_hw_)          ((_hw_)->inited & (1 << esp_cpu_get_core_id()))

#define ESP_APPTRACE_TRAX_BLOCK_SIZE            (0x4000UL)

/** TRAX HW transport data */
typedef struct {
    uint8_t                             inited;
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_apptrace_lock_t                 lock;   // sync lock
#endif
    esp_apptrace_membufs_proto_data_t   membufs;
} esp_apptrace_trax_data_t;


static esp_err_t esp_apptrace_trax_init(esp_apptrace_trax_data_t *hw_data);
static esp_err_t esp_apptrace_trax_flush(esp_apptrace_trax_data_t *hw_data, esp_apptrace_tmo_t *tmo);
static esp_err_t esp_apptrace_trax_flush_nolock(esp_apptrace_trax_data_t *hw_data, uint32_t min_sz, esp_apptrace_tmo_t *tmo);
static uint8_t *esp_apptrace_trax_up_buffer_get(esp_apptrace_trax_data_t *hw_data, uint32_t size, esp_apptrace_tmo_t *tmo);
static esp_err_t esp_apptrace_trax_up_buffer_put(esp_apptrace_trax_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
static void esp_apptrace_trax_down_buffer_config(esp_apptrace_trax_data_t *hw_data, uint8_t *buf, uint32_t size);
static uint8_t *esp_apptrace_trax_down_buffer_get(esp_apptrace_trax_data_t *hw_data, uint32_t *size, esp_apptrace_tmo_t *tmo);
static esp_err_t esp_apptrace_trax_down_buffer_put(esp_apptrace_trax_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
static bool esp_apptrace_trax_host_is_connected(esp_apptrace_trax_data_t *hw_data);
static esp_err_t esp_apptrace_trax_buffer_swap_start(uint32_t curr_block_id);
static esp_err_t esp_apptrace_trax_buffer_swap(uint32_t new_block_id);
static esp_err_t esp_apptrace_trax_buffer_swap_end(uint32_t new_block_id, uint32_t prev_block_len);
static bool esp_apptrace_trax_host_data_pending(void);


const static char *TAG = "esp_apptrace";

static uint8_t * const s_trax_blocks[] = {
    (uint8_t *)TRACEMEM_BLK0_ADDR,
    (uint8_t *)TRACEMEM_BLK1_ADDR
};

esp_apptrace_hw_t *esp_apptrace_jtag_hw_get(void **data)
{
#if CONFIG_APPTRACE_DEST_JTAG
    static esp_apptrace_membufs_proto_hw_t s_trax_proto_hw = {
        .swap_start = esp_apptrace_trax_buffer_swap_start,
        .swap = esp_apptrace_trax_buffer_swap,
        .swap_end = esp_apptrace_trax_buffer_swap_end,
        .host_data_pending = esp_apptrace_trax_host_data_pending,
    };
    static esp_apptrace_trax_data_t s_trax_hw_data = {
        .membufs = {
            .hw = &s_trax_proto_hw,
        },
    };
    static esp_apptrace_hw_t s_trax_hw = {
        .init = (esp_err_t (*)(void *))esp_apptrace_trax_init,
        .get_up_buffer = (uint8_t *(*)(void *, uint32_t, esp_apptrace_tmo_t *))esp_apptrace_trax_up_buffer_get,
        .put_up_buffer = (esp_err_t (*)(void *, uint8_t *, esp_apptrace_tmo_t *))esp_apptrace_trax_up_buffer_put,
        .flush_up_buffer_nolock = (esp_err_t (*)(void *, uint32_t, esp_apptrace_tmo_t *))esp_apptrace_trax_flush_nolock,
        .flush_up_buffer = (esp_err_t (*)(void *, esp_apptrace_tmo_t *))esp_apptrace_trax_flush,
        .down_buffer_config = (void (*)(void *, uint8_t *, uint32_t ))esp_apptrace_trax_down_buffer_config,
        .get_down_buffer = (uint8_t *(*)(void *, uint32_t *, esp_apptrace_tmo_t *))esp_apptrace_trax_down_buffer_get,
        .put_down_buffer = (esp_err_t (*)(void *, uint8_t *, esp_apptrace_tmo_t *))esp_apptrace_trax_down_buffer_put,
        .host_is_connected = (bool (*)(void *))esp_apptrace_trax_host_is_connected,
    };
    *data = &s_trax_hw_data;
    return &s_trax_hw;
#else
    return NULL;
#endif
}

static esp_err_t esp_apptrace_trax_lock(esp_apptrace_trax_data_t *hw_data, esp_apptrace_tmo_t *tmo)
{
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_err_t ret = esp_apptrace_lock_take(&hw_data->lock, tmo);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
#endif
    return ESP_OK;
}

static esp_err_t esp_apptrace_trax_unlock(esp_apptrace_trax_data_t *hw_data)
{
    esp_err_t ret = ESP_OK;
#if CONFIG_APPTRACE_LOCK_ENABLE
    ret = esp_apptrace_lock_give(&hw_data->lock);
#endif
    return ret;
}

static inline void esp_apptrace_trax_hw_init(void)
{
    // Stop trace, if any (on the current CPU)
    eri_write(ERI_TRAX_TRAXCTRL, TRAXCTRL_TRSTP);
    eri_write(ERI_TRAX_TRAXCTRL, TRAXCTRL_TMEN);
    eri_write(ESP_APPTRACE_TRAX_CTRL_REG, ESP_APPTRACE_TRAX_BLOCK_ID(0));
    // this is for OpenOCD to let him know where stub entries vector is resided
    // must be read by host before any transfer using TRAX
    eri_write(ESP_APPTRACE_TRAX_STAT_REG, 0);

    ESP_APPTRACE_LOGI("Initialized TRAX on CPU%d", esp_cpu_get_core_id());
}

static inline void esp_apptrace_trax_select_memory_block(int block_num)
{
    // select memory block to be exposed to the TRAX module (accessed by host)
#if CONFIG_IDF_TARGET_ESP32
    DPORT_WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, block_num ? TRACEMEM_MUX_BLK0_ONLY : TRACEMEM_MUX_BLK1_ONLY);
#elif CONFIG_IDF_TARGET_ESP32S2
    WRITE_PERI_REG(DPORT_PMS_OCCUPY_3_REG, block_num ? BIT(TRACEMEM_MUX_BLK0_NUM-4) : BIT(TRACEMEM_MUX_BLK1_NUM-4));
#elif CONFIG_IDF_TARGET_ESP32S3
    // select memory block to be exposed to the TRAX module (accessed by host)
    uint32_t block_bits = block_num ? TRACEMEM_CORE0_MUX_BLK_BITS(TRACEMEM_MUX_BLK0_NUM)
                        : TRACEMEM_CORE0_MUX_BLK_BITS(TRACEMEM_MUX_BLK1_NUM);
    block_bits |= block_num ? TRACEMEM_CORE1_MUX_BLK_BITS(TRACEMEM_MUX_BLK0_NUM)
                        : TRACEMEM_CORE1_MUX_BLK_BITS(TRACEMEM_MUX_BLK1_NUM);
    ESP_EARLY_LOGV(TAG, "Select block %d @ %p (bits 0x%" PRIx32 ")", block_num, s_trax_blocks[block_num], block_bits);
    DPORT_WRITE_PERI_REG(SENSITIVE_INTERNAL_SRAM_USAGE_2_REG, block_bits);
#endif
}

static inline void esp_apptrace_trax_memory_enable(void)
{
#if CONFIG_IDF_TARGET_ESP32
        /* Enable trace memory on PRO CPU */
        DPORT_WRITE_PERI_REG(DPORT_PRO_TRACEMEM_ENA_REG, DPORT_PRO_TRACEMEM_ENA_M);
#if CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE == 0
        /* Enable trace memory on APP CPU */
        DPORT_WRITE_PERI_REG(DPORT_APP_TRACEMEM_ENA_REG, DPORT_APP_TRACEMEM_ENA_M);
#endif
#endif
}

/*****************************************************************************************/
/***************************** Apptrace HW iface *****************************************/
/*****************************************************************************************/

static esp_err_t esp_apptrace_trax_init(esp_apptrace_trax_data_t *hw_data)
{
    int core_id = esp_cpu_get_core_id();

    // 'esp_apptrace_trax_init()' is called on every core, so ensure to do main initialization only once
    if (core_id == 0) {
        esp_apptrace_mem_block_t mem_blocks_cfg[2] = {
            {
                .start = s_trax_blocks[0],
                .sz = ESP_APPTRACE_TRAX_BLOCK_SIZE
            },
            {
                .start = s_trax_blocks[1],
                .sz = ESP_APPTRACE_TRAX_BLOCK_SIZE
            },
        };
        esp_err_t res = esp_apptrace_membufs_init(&hw_data->membufs, mem_blocks_cfg);
        if (res != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to init membufs proto (%d)!", res);
            return res;
        }
#if CONFIG_APPTRACE_LOCK_ENABLE
        esp_apptrace_lock_init(&hw_data->lock);
#endif
        esp_apptrace_trax_memory_enable();
        esp_apptrace_trax_select_memory_block(0);
    }
    // init TRAX on this CPU
    esp_apptrace_trax_hw_init();
    hw_data->inited |= 1 << core_id;

    return ESP_OK;
}

static uint8_t *esp_apptrace_trax_up_buffer_get(esp_apptrace_trax_data_t *hw_data, uint32_t size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *ptr;

    if (!ESP_APPTRACE_TRAX_INITED(hw_data)) {
        return NULL;
    }
    esp_err_t res = esp_apptrace_trax_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return NULL;
    }

    ptr = esp_apptrace_membufs_up_buffer_get(&hw_data->membufs, size, tmo);

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_trax_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return ptr;
}

static esp_err_t esp_apptrace_trax_up_buffer_put(esp_apptrace_trax_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    if (!ESP_APPTRACE_TRAX_INITED(hw_data)) {
        return ESP_ERR_INVALID_STATE;
    }
    // Can avoid locking because esp_apptrace_membufs_up_buffer_put() just modifies buffer's header
    esp_err_t res = esp_apptrace_membufs_up_buffer_put(&hw_data->membufs, ptr, tmo);
    return res;
}

static void esp_apptrace_trax_down_buffer_config(esp_apptrace_trax_data_t *hw_data, uint8_t *buf, uint32_t size)
{
    if (!ESP_APPTRACE_TRAX_INITED(hw_data)) {
        return;
    }
    esp_apptrace_membufs_down_buffer_config(&hw_data->membufs, buf, size);
}

static uint8_t *esp_apptrace_trax_down_buffer_get(esp_apptrace_trax_data_t *hw_data, uint32_t *size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *ptr;

    if (!ESP_APPTRACE_TRAX_INITED(hw_data)) {
        return NULL;
    }
    esp_err_t res = esp_apptrace_trax_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return NULL;
    }

    ptr = esp_apptrace_membufs_down_buffer_get(&hw_data->membufs, size, tmo);

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_trax_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return ptr;
}

static esp_err_t esp_apptrace_trax_down_buffer_put(esp_apptrace_trax_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    if (!ESP_APPTRACE_TRAX_INITED(hw_data)) {
        return ESP_ERR_INVALID_STATE;
    }
    // Can avoid locking because esp_apptrace_membufs_down_buffer_put() does nothing
    /*esp_err_t res = esp_apptrace_trax_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return res;
    }*/

    esp_err_t res = esp_apptrace_membufs_down_buffer_put(&hw_data->membufs, ptr, tmo);

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    /*if (esp_apptrace_trax_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }*/
    return res;
}

static bool esp_apptrace_trax_host_is_connected(esp_apptrace_trax_data_t *hw_data)
{
    if (!ESP_APPTRACE_TRAX_INITED(hw_data)) {
        return false;
    }
    return eri_read(ESP_APPTRACE_TRAX_CTRL_REG) & ESP_APPTRACE_TRAX_HOST_CONNECT ? true : false;
}

static esp_err_t esp_apptrace_trax_flush_nolock(esp_apptrace_trax_data_t *hw_data, uint32_t min_sz, esp_apptrace_tmo_t *tmo)
{
    if (!ESP_APPTRACE_TRAX_INITED(hw_data)) {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_apptrace_membufs_flush_nolock(&hw_data->membufs, min_sz, tmo);
}

static esp_err_t esp_apptrace_trax_flush(esp_apptrace_trax_data_t *hw_data, esp_apptrace_tmo_t *tmo)
{
    if (!ESP_APPTRACE_TRAX_INITED(hw_data)) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t res = esp_apptrace_trax_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return res;
    }

    res = esp_apptrace_membufs_flush_nolock(&hw_data->membufs, 0, tmo);

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_trax_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return res;
}

/*****************************************************************************************/
/************************** Membufs proto HW iface ***************************************/
/*****************************************************************************************/

static inline void esp_apptrace_trax_buffer_swap_lock(void)
{
    extern uint32_t __esp_apptrace_trax_eri_updated;

    // indicate to host that we are about to update.
    // this is used only to place CPU into streaming mode at tracing startup
    // before starting streaming host can halt us after we read  ESP_APPTRACE_TRAX_CTRL_REG and before we updated it
    // HACK: in this case host will set breakpoint just after ESP_APPTRACE_TRAX_CTRL_REG update,
    // here we set address to set bp at
    // enter ERI update critical section
    eri_write(ESP_APPTRACE_TRAX_STAT_REG, (uint32_t)&__esp_apptrace_trax_eri_updated);
}

static __attribute__((noinline)) void esp_apptrace_trax_buffer_swap_unlock(void)
{
    // exit ERI update critical section
    eri_write(ESP_APPTRACE_TRAX_STAT_REG, 0x0);
    // TODO: currently host sets breakpoint, use break instruction to stop;
    // it will allow to use ESP_APPTRACE_TRAX_STAT_REG for other purposes
    asm volatile (
        "    .global     __esp_apptrace_trax_eri_updated\n"
        "__esp_apptrace_trax_eri_updated:\n"); // host will set bp here to resolve collision at streaming start
}

static esp_err_t esp_apptrace_trax_buffer_swap_start(uint32_t curr_block_id)
{
    esp_err_t res = ESP_OK;

    esp_apptrace_trax_buffer_swap_lock();

    uint32_t ctrl_reg = eri_read(ESP_APPTRACE_TRAX_CTRL_REG);
    uint32_t host_connected = ESP_APPTRACE_TRAX_HOST_CONNECT & ctrl_reg;
    if (host_connected) {
        uint32_t acked_block = ESP_APPTRACE_TRAX_BLOCK_ID_GET(ctrl_reg);
        uint32_t host_to_read = ESP_APPTRACE_TRAX_BLOCK_LEN_GET(ctrl_reg);
        if (host_to_read != 0 || acked_block != (curr_block_id & ESP_APPTRACE_TRAX_BLOCK_ID_MSK)) {
            ESP_APPTRACE_LOGD("HC[%d]: Can not switch %" PRIx32 " %" PRIu32 " %" PRIx32 " %" PRIx32 "/%" PRIx32, esp_cpu_get_core_id(), ctrl_reg, host_to_read, acked_block,
                curr_block_id & ESP_APPTRACE_TRAX_BLOCK_ID_MSK, curr_block_id);
            res = ESP_ERR_NO_MEM;
            goto _on_err;
        }
    }
    return ESP_OK;
_on_err:
    esp_apptrace_trax_buffer_swap_unlock();
    return res;
}

static esp_err_t esp_apptrace_trax_buffer_swap_end(uint32_t new_block_id, uint32_t prev_block_len)
{
    uint32_t ctrl_reg = eri_read(ESP_APPTRACE_TRAX_CTRL_REG);
    uint32_t host_connected = ESP_APPTRACE_TRAX_HOST_CONNECT & ctrl_reg;
    eri_write(ESP_APPTRACE_TRAX_CTRL_REG, ESP_APPTRACE_TRAX_BLOCK_ID(new_block_id) |
              host_connected | ESP_APPTRACE_TRAX_BLOCK_LEN(prev_block_len));
    esp_apptrace_trax_buffer_swap_unlock();
    return ESP_OK;
}

static esp_err_t esp_apptrace_trax_buffer_swap(uint32_t new_block_id)
{
    esp_apptrace_trax_select_memory_block(new_block_id);
    return ESP_OK;
}

static bool esp_apptrace_trax_host_data_pending(void)
{
    uint32_t ctrl_reg = eri_read(ESP_APPTRACE_TRAX_CTRL_REG);
    return (ctrl_reg & ESP_APPTRACE_TRAX_HOST_DATA) ? true : false;
}
