// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Hot It Works
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

// In general tracing goes in the following way. User aplication requests tracing module to send some data by calling esp_apptrace_buffer_get(),
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
// - Status register uses TRAX_TRIGGERPC as storage. If this register is not zero then currentlly CPU is changing TRAX registers and
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
//    It can be insufficient to diagnose the problem. To avoid such situations there is menuconfig option CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_TRAX_THRESH
//    which controls the threshold for flushing data in case of panic.
//  - Streaming mode. Tracing module enters this mode when host connects to target and sets respective bits in control registers (per core).
//    In this mode before switching the block tracing module waits for the host to read all the data from the previously exposed block.
//    On panic tracing module also waits (timeout is configured via menuconfig via CONFIG_ESP32_APPTRACE_ONPANIC_HOST_FLUSH_TMO) for the host to read all data.

// 4. Communication Protocol
// =========================

// 4.1 Trace Memory Blocks
// -----------------------

// Communication is controlled via special register. Host periodically polls control register on each core to find out if there are any data avalable.
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
// which reads application traces reports error when it reads incompleted user data block.
// Data which are transfered from host to target are also prepended with a header. Down channel data header is simple and consists of one two bytes field
// containing length of host data following the heder.

// 4.3 Data Buffering
// ------------------

// It takes some time for the host to read TRAX memory block via JTAG. In streaming mode it can happen that target has filled its TRAX block, but host
// has not completed reading of the previous one yet. So in this case time critical tracing calls (which can not be delayed for too long time due to
// the lack of free memory in TRAX block) can be dropped. To avoid such scenarios tracing module implements data buffering. Buffered data will be sent
// to the host later when TRAX block switch occurs. The maximum size of the buffered data is controlled by menuconfig option
// CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX.

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
// on internal locks and for host to complete previous block reading, so if timeout value exceedes watchdog's one it can lead to the system reboot.

// 6. Timeouts
// ===========

// Timeout mechanism is based on xthal_get_ccount() routine and supports timeout values in micorseconds.
// There are two situations when task/ISR can be delayed by tracing API call. Timeout mechanism takes into account both conditions:
// - Trace data are locked by another task/ISR. When wating on trace data lock.
// - Current TRAX memory input block is full when working in streaming mode (host is connected). When waiting for host to complete previous block reading.
// When wating for any of above conditions xthal_get_ccount() is called periodically to calculate time elapsed from trace API routine entry. When elapsed
// time exceeds specified timeout value operation is canceled and ESP_ERR_TIMEOUT code is returned.

// ALSO SEE example usage of application tracing module in 'components/app_trace/README.rst'

#include <string.h>
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "eri.h"
#include "trax.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "freertos/FreeRTOS.h"
#include "esp_app_trace.h"

#if CONFIG_ESP32_APPTRACE_ENABLE
#define ESP_APPTRACE_MAX_VPRINTF_ARGS           256
#define ESP_APPTRACE_HOST_BUF_SIZE              256

#define ESP_APPTRACE_PRINT_LOCK                 0

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "esp_apptrace";

#if ESP_APPTRACE_PRINT_LOCK
#define ESP_APPTRACE_LOG( format, ... )   \
    do { \
        esp_apptrace_log_lock(); \
        ets_printf(format, ##__VA_ARGS__); \
        esp_apptrace_log_unlock(); \
    } while(0)
#else
#define ESP_APPTRACE_LOG( format, ... )   \
    do { \
        ets_printf(format, ##__VA_ARGS__); \
    } while(0)
#endif

#define ESP_APPTRACE_LOG_LEV( _L_, level, format, ... )   \
    do { \
        if (LOG_LOCAL_LEVEL >= level) { \
            ESP_APPTRACE_LOG(LOG_FORMAT(_L_, format), esp_log_early_timestamp(), TAG, ##__VA_ARGS__); \
        } \
    } while(0)

#define ESP_APPTRACE_LOGE( format, ... )  ESP_APPTRACE_LOG_LEV(E, ESP_LOG_ERROR, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGW( format, ... )  ESP_APPTRACE_LOG_LEV(W, ESP_LOG_WARN, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGI( format, ... )  ESP_APPTRACE_LOG_LEV(I, ESP_LOG_INFO, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGD( format, ... )  ESP_APPTRACE_LOG_LEV(D, ESP_LOG_DEBUG, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGV( format, ... )  ESP_APPTRACE_LOG_LEV(V, ESP_LOG_VERBOSE, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGO( format, ... )  ESP_APPTRACE_LOG_LEV(E, ESP_LOG_NONE, format, ##__VA_ARGS__)

#define ESP_APPTRACE_CPUTICKS2US(_t_)       ((_t_)/(XT_CLOCK_FREQ/1000000))

// TODO: move these (and same definitions in trax.c to dport_reg.h)
#define TRACEMEM_MUX_PROBLK0_APPBLK1            0
#define TRACEMEM_MUX_BLK0_ONLY                  1
#define TRACEMEM_MUX_BLK1_ONLY                  2
#define TRACEMEM_MUX_PROBLK1_APPBLK0            3

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

#if CONFIG_SYSVIEW_ENABLE
#define ESP_APPTRACE_USR_BLOCK_CORE(_cid_)      (0)
#define ESP_APPTRACE_USR_BLOCK_LEN(_v_)         (_v_)
#else
#define ESP_APPTRACE_USR_BLOCK_CORE(_cid_)      ((_cid_) << 15)
#define ESP_APPTRACE_USR_BLOCK_LEN(_v_)         (~(1 << 15) & (_v_))
#endif
#define ESP_APPTRACE_USR_BLOCK_RAW_SZ(_s_)     ((_s_) + sizeof(esp_tracedata_hdr_t))

static volatile uint8_t *s_trax_blocks[] = {
    (volatile uint8_t *) 0x3FFFC000,
    (volatile uint8_t *) 0x3FFF8000
};

#define ESP_APPTRACE_TRAX_BLOCKS_NUM            (sizeof(s_trax_blocks)/sizeof(s_trax_blocks[0]))

#define ESP_APPTRACE_TRAX_INBLOCK_START         0

#define ESP_APPTRACE_TRAX_INBLOCK_MARKER()          (s_trace_buf.trax.state.markers[s_trace_buf.trax.state.in_block % 2])
#define ESP_APPTRACE_TRAX_INBLOCK_MARKER_UPD(_v_)   do {s_trace_buf.trax.state.markers[s_trace_buf.trax.state.in_block % 2] += (_v_);}while(0)
#define ESP_APPTRACE_TRAX_INBLOCK_GET()             (&s_trace_buf.trax.blocks[s_trace_buf.trax.state.in_block % 2])

#define ESP_APPTRACE_TRAX_BLOCK_SIZE            (0x4000UL)
#if CONFIG_SYSVIEW_ENABLE
#define ESP_APPTRACE_USR_DATA_LEN_MAX           255UL
#else
#define ESP_APPTRACE_USR_DATA_LEN_MAX           (ESP_APPTRACE_TRAX_BLOCK_SIZE - sizeof(esp_tracedata_hdr_t))
#endif

/** Trace data header. Every user data chunk is prepended with this header.
 * User allocates block with esp_apptrace_buffer_get and then fills it with data,
 * in multithreading environment it can happen that tasks gets buffer and then gets interrupted,
 * so it is possible that user data are incomplete when TRAX memory block is exposed to the host.
 * In this case host SW will see that wr_sz < block_sz and will report error.
 */
typedef struct {
#if CONFIG_SYSVIEW_ENABLE
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

/** TRAX HW transport state */
typedef struct {
    uint32_t                   in_block;                                // input block ID
    // TODO: change to uint16_t
    uint32_t                   markers[ESP_APPTRACE_TRAX_BLOCKS_NUM];   // block filling level markers
} esp_apptrace_trax_state_t;

/** memory block parameters */
typedef struct {
    uint8_t   *start;   // start address
    uint16_t   sz;      // size
} esp_apptrace_mem_block_t;

/** TRAX HW transport data */
typedef struct {
    volatile esp_apptrace_trax_state_t  state;                                  // state
    esp_apptrace_mem_block_t            blocks[ESP_APPTRACE_TRAX_BLOCKS_NUM];   // memory blocks
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    // ring buffer control struct for pending user blocks
    esp_apptrace_rb_t                   rb_pend;
    // storage for pending user blocks
    uint8_t                             pending_data[CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX + 1];
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > ESP_APPTRACE_TRAX_BLOCK_SIZE
    // ring buffer control struct for pending user data chunks sizes,
    // every chunk contains whole number of user blocks and fit into TRAX memory block
    esp_apptrace_rb_t                   rb_pend_chunk_sz;
    // storage for above ring buffer data
    uint16_t                            pending_chunk_sz[CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX/ESP_APPTRACE_TRAX_BLOCK_SIZE + 2];
    // current (accumulated) pending user data chunk size
    uint16_t                            cur_pending_chunk_sz;
#endif
#endif
} esp_apptrace_trax_data_t;

/** tracing module internal data */
typedef struct {
    esp_apptrace_lock_t         lock;   // sync lock
    uint8_t                     inited; // module initialization state flag
    // ring buffer control struct for data from host (down buffer)
    esp_apptrace_rb_t           rb_down;
    // storage for above ring buffer data
    esp_apptrace_trax_data_t    trax;   // TRAX HW transport data
} esp_apptrace_buffer_t;

static esp_apptrace_buffer_t    s_trace_buf;

#if ESP_APPTRACE_PRINT_LOCK
static esp_apptrace_lock_t s_log_lock = {.irq_stat = 0, .portmux = portMUX_INITIALIZER_UNLOCKED};
#endif

static uint32_t esp_apptrace_trax_down_buffer_write_nolock(uint8_t *data, uint32_t size);
static esp_err_t esp_apptrace_trax_flush(uint32_t min_sz, esp_apptrace_tmo_t *tmo);

static inline int esp_apptrace_log_lock()
{
#if ESP_APPTRACE_PRINT_LOCK
    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, ESP_APPTRACE_TMO_INFINITE);
    int ret = esp_apptrace_lock_take(&s_log_lock, &tmo);
    return ret;
#else
    return 0;
#endif
}

static inline void esp_apptrace_log_unlock()
{
 #if ESP_APPTRACE_PRINT_LOCK
    esp_apptrace_lock_give(&s_log_lock);
#endif
}

static inline esp_err_t esp_apptrace_lock_initialize()
{
#if CONFIG_ESP32_APPTRACE_LOCK_ENABLE
    esp_apptrace_lock_init(&s_trace_buf.lock);
#endif
    return ESP_OK;
}

static inline esp_err_t esp_apptrace_lock_cleanup()
{
    return ESP_OK;
}

esp_err_t esp_apptrace_lock(esp_apptrace_tmo_t *tmo)
{
#if CONFIG_ESP32_APPTRACE_LOCK_ENABLE
    esp_err_t ret = esp_apptrace_lock_take(&s_trace_buf.lock, tmo);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
#endif
    return ESP_OK;
}

esp_err_t esp_apptrace_unlock()
{
    esp_err_t ret = ESP_OK;
#if CONFIG_ESP32_APPTRACE_LOCK_ENABLE
    ret = esp_apptrace_lock_give(&s_trace_buf.lock);
#endif
    return ret;
}

#if CONFIG_ESP32_APPTRACE_DEST_TRAX
static void esp_apptrace_trax_init()
{
    // Stop trace, if any (on the current CPU)
    eri_write(ERI_TRAX_TRAXCTRL, TRAXCTRL_TRSTP);
    eri_write(ERI_TRAX_TRAXCTRL, TRAXCTRL_TMEN);
    eri_write(ESP_APPTRACE_TRAX_CTRL_REG, ESP_APPTRACE_TRAX_BLOCK_ID(ESP_APPTRACE_TRAX_INBLOCK_START));
    eri_write(ESP_APPTRACE_TRAX_STAT_REG, 0);

    ESP_APPTRACE_LOGI("Initialized TRAX on CPU%d", xPortGetCoreID());
}

#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > ESP_APPTRACE_TRAX_BLOCK_SIZE
// keep the size of buffered data for copying to TRAX mem block.
// Only whole user blocks should be copied from buffer to TRAX block upon the switch
static void esp_apptrace_trax_pend_chunk_sz_update(uint16_t size)
{
    ESP_APPTRACE_LOGD("Update chunk enter %d/%d  w-r-s %d-%d-%d", s_trace_buf.trax.cur_pending_chunk_sz, size,
        s_trace_buf.trax.rb_pend_chunk_sz.wr, s_trace_buf.trax.rb_pend_chunk_sz.rd, s_trace_buf.trax.rb_pend_chunk_sz.cur_size);

    if ((uint32_t)s_trace_buf.trax.cur_pending_chunk_sz + (uint32_t)size <= ESP_APPTRACE_TRAX_BLOCK_SIZE) {
        ESP_APPTRACE_LOGD("Update chunk %d/%d", s_trace_buf.trax.cur_pending_chunk_sz, size);
        s_trace_buf.trax.cur_pending_chunk_sz += size;
    } else {
        uint16_t *chunk_sz = (uint16_t *)esp_apptrace_rb_produce(&s_trace_buf.trax.rb_pend_chunk_sz, sizeof(uint16_t));
        if (!chunk_sz) {
            assert(false && "Failed to alloc pended chunk sz slot!");
        } else {
            ESP_APPTRACE_LOGD("Update new chunk %d/%d", s_trace_buf.trax.cur_pending_chunk_sz, size);
            *chunk_sz = s_trace_buf.trax.cur_pending_chunk_sz;
            s_trace_buf.trax.cur_pending_chunk_sz = size;
        }
    }
}

static uint16_t esp_apptrace_trax_pend_chunk_sz_get()
{
    uint16_t ch_sz;
    ESP_APPTRACE_LOGD("Get chunk enter %d w-r-s %d-%d-%d", s_trace_buf.trax.cur_pending_chunk_sz,
        s_trace_buf.trax.rb_pend_chunk_sz.wr, s_trace_buf.trax.rb_pend_chunk_sz.rd, s_trace_buf.trax.rb_pend_chunk_sz.cur_size);

    uint16_t *chunk_sz = (uint16_t *)esp_apptrace_rb_consume(&s_trace_buf.trax.rb_pend_chunk_sz, sizeof(uint16_t));
    if (!chunk_sz) {
        ch_sz = s_trace_buf.trax.cur_pending_chunk_sz;
        s_trace_buf.trax.cur_pending_chunk_sz = 0;
    } else {
        ch_sz = *chunk_sz;
    }
    return ch_sz;
}
#endif

// assumed to be protected by caller from multi-core/thread access
static esp_err_t esp_apptrace_trax_block_switch()
{
    int prev_block_num = s_trace_buf.trax.state.in_block % 2;
    int new_block_num = prev_block_num ? (0) : (1);
    int res = ESP_OK;
    extern uint32_t __esp_apptrace_trax_eri_updated;

    // indicate to host that we are about to update.
    // this is used only to place CPU into streaming mode at tracing startup
    // before starting streaming host can halt us after we read  ESP_APPTRACE_TRAX_CTRL_REG and before we updated it
    // HACK: in this case host will set breakpoint just after ESP_APPTRACE_TRAX_CTRL_REG update,
    // here we set address to set bp at
    // enter ERI update critical section
    eri_write(ESP_APPTRACE_TRAX_STAT_REG, (uint32_t)&__esp_apptrace_trax_eri_updated);

    uint32_t ctrl_reg = eri_read(ESP_APPTRACE_TRAX_CTRL_REG);
    uint32_t host_connected = ESP_APPTRACE_TRAX_HOST_CONNECT & ctrl_reg;
    if (host_connected) {
        uint32_t acked_block = ESP_APPTRACE_TRAX_BLOCK_ID_GET(ctrl_reg);
        uint32_t host_to_read = ESP_APPTRACE_TRAX_BLOCK_LEN_GET(ctrl_reg);
        if (host_to_read != 0 || acked_block != (s_trace_buf.trax.state.in_block & ESP_APPTRACE_TRAX_BLOCK_ID_MSK)) {
            ESP_APPTRACE_LOGD("HC[%d]: Can not switch %x %d %x %x/%lx, m %d", xPortGetCoreID(), ctrl_reg, host_to_read, acked_block,
                s_trace_buf.trax.state.in_block & ESP_APPTRACE_TRAX_BLOCK_ID_MSK, s_trace_buf.trax.state.in_block,
                s_trace_buf.trax.state.markers[prev_block_num]);
            res = ESP_ERR_NO_MEM;
            goto _on_func_exit;
        }
    }
    s_trace_buf.trax.state.markers[new_block_num] = 0;
    // switch to new block
    s_trace_buf.trax.state.in_block++;

    DPORT_WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, new_block_num ? TRACEMEM_MUX_BLK0_ONLY : TRACEMEM_MUX_BLK1_ONLY);
    // handle data from host
    esp_hostdata_hdr_t *hdr = (esp_hostdata_hdr_t *)s_trace_buf.trax.blocks[new_block_num].start;
    if (ctrl_reg & ESP_APPTRACE_TRAX_HOST_DATA && hdr->block_sz > 0) {
        // TODO: add support for multiple blocks from host, currently there is no need for that
        uint8_t *p = s_trace_buf.trax.blocks[new_block_num].start + s_trace_buf.trax.blocks[new_block_num].sz;
        ESP_APPTRACE_LOGD("Recvd %d bytes from host [%x %x %x %x %x %x %x %x .. %x %x %x %x %x %x %x %x]", hdr->block_sz,
            *(s_trace_buf.trax.blocks[new_block_num].start+0), *(s_trace_buf.trax.blocks[new_block_num].start+1),
            *(s_trace_buf.trax.blocks[new_block_num].start+2), *(s_trace_buf.trax.blocks[new_block_num].start+3),
            *(s_trace_buf.trax.blocks[new_block_num].start+4), *(s_trace_buf.trax.blocks[new_block_num].start+5),
            *(s_trace_buf.trax.blocks[new_block_num].start+6), *(s_trace_buf.trax.blocks[new_block_num].start+7),
            *(p-8), *(p-7), *(p-6), *(p-5), *(p-4), *(p-3), *(p-2), *(p-1));
        uint32_t sz = esp_apptrace_trax_down_buffer_write_nolock((uint8_t *)(hdr+1), hdr->block_sz);
        if (sz != hdr->block_sz) {
            ESP_APPTRACE_LOGE("Failed to write %d bytes to down buffer (%d %d)!", hdr->block_sz - sz, hdr->block_sz, sz);
        }
        hdr->block_sz = 0;
    }
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    // copy pending data to TRAX block if any
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > ESP_APPTRACE_TRAX_BLOCK_SIZE
    uint16_t max_chunk_sz = esp_apptrace_trax_pend_chunk_sz_get();
#else
    uint16_t max_chunk_sz = s_trace_buf.trax.blocks[new_block_num].sz;
#endif
    while (s_trace_buf.trax.state.markers[new_block_num] < max_chunk_sz) {
        uint32_t read_sz = esp_apptrace_rb_read_size_get(&s_trace_buf.trax.rb_pend);
        if (read_sz == 0) {
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > ESP_APPTRACE_TRAX_BLOCK_SIZE
            /* theres is a bug: esp_apptrace_trax_pend_chunk_sz_get returned wrong value,
               it must be greater or equal to one returned by esp_apptrace_rb_read_size_get */
            ESP_APPTRACE_LOGE("No pended bytes, must be > 0 and <= %d!", max_chunk_sz);
#endif
            break;
        }
        if (read_sz > max_chunk_sz - s_trace_buf.trax.state.markers[new_block_num]) {
            read_sz = max_chunk_sz - s_trace_buf.trax.state.markers[new_block_num];
        }
        uint8_t *ptr = esp_apptrace_rb_consume(&s_trace_buf.trax.rb_pend, read_sz);
        if (!ptr) {
            assert(false && "Failed to consume pended bytes!!");
            break;
        }
        if (host_connected) {
            ESP_APPTRACE_LOGD("Pump %d pend bytes [%x %x %x %x : %x %x %x %x : %x %x %x %x : %x %x...%x %x]",
                read_sz, *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4),
                *(ptr+5), *(ptr+6), *(ptr+7), *(ptr+8), *(ptr+9), *(ptr+10), *(ptr+11), *(ptr+12), *(ptr+13), *(ptr+read_sz-2), *(ptr+read_sz-1));
        }
        memcpy(s_trace_buf.trax.blocks[new_block_num].start + s_trace_buf.trax.state.markers[new_block_num], ptr, read_sz);
        s_trace_buf.trax.state.markers[new_block_num] += read_sz;
    }
#endif
    eri_write(ESP_APPTRACE_TRAX_CTRL_REG, ESP_APPTRACE_TRAX_BLOCK_ID(s_trace_buf.trax.state.in_block) |
              host_connected | ESP_APPTRACE_TRAX_BLOCK_LEN(s_trace_buf.trax.state.markers[prev_block_num]));

_on_func_exit:
    // exit ERI update critical section
    eri_write(ESP_APPTRACE_TRAX_STAT_REG, 0x0);
    // TODO: currently host sets breakpoint, use break instruction to stop;
    // it will allow to use ESP_APPTRACE_TRAX_STAT_REG for other purposes
    asm volatile (
        "    .global     __esp_apptrace_trax_eri_updated\n"
        "__esp_apptrace_trax_eri_updated:\n"); // host will set bp here to resolve collision at streaming start
    return res;
}

static esp_err_t esp_apptrace_trax_block_switch_waitus(esp_apptrace_tmo_t *tmo)
{
    int res;

    while ((res = esp_apptrace_trax_block_switch()) != ESP_OK) {
        res = esp_apptrace_tmo_check(tmo);
        if (res != ESP_OK) {
            break;
        }
    }
    return res;
}

static uint8_t *esp_apptrace_trax_down_buffer_get(uint32_t *size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *ptr = NULL;

    int res = esp_apptrace_lock(tmo);
    if (res != ESP_OK) {
        return NULL;
    }
    while (1) {
        uint32_t sz = esp_apptrace_rb_read_size_get(&s_trace_buf.rb_down);
        if (sz != 0) {
            ptr = esp_apptrace_rb_consume(&s_trace_buf.rb_down, sz > *size ? *size : sz);
            if (!ptr) {
                assert(false && "Failed to consume bytes from down buffer!");
            }
            *size = sz;
            break;
        }
        // may need to flush
        uint32_t ctrl_reg = eri_read(ESP_APPTRACE_TRAX_CTRL_REG);
        if (ctrl_reg & ESP_APPTRACE_TRAX_HOST_DATA) {
            ESP_APPTRACE_LOGD("force flush");
            res = esp_apptrace_trax_block_switch_waitus(tmo);
            if (res != ESP_OK) {
                ESP_APPTRACE_LOGE("Failed to switch to another block to recv data from host!");
                /*do not return error because data can be in down buffer already*/
            }
        } else {
            // check tmo only if there is no data from host
            res = esp_apptrace_tmo_check(tmo);
            if (res != ESP_OK) {
                return NULL;
            }
        }
    }
    if (esp_apptrace_unlock() != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return ptr;
}

static inline esp_err_t esp_apptrace_trax_down_buffer_put(uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    /* nothing todo */
    return ESP_OK;
}

static uint32_t esp_apptrace_trax_down_buffer_write_nolock(uint8_t *data, uint32_t size)
{
    uint32_t total_sz = 0;

    while (total_sz < size) {
        // ESP_APPTRACE_LOGE("esp_apptrace_trax_down_buffer_write_nolock WRS %d-%d-%d %d", s_trace_buf.rb_down.wr, s_trace_buf.rb_down.rd,
        //     s_trace_buf.rb_down.cur_size, size);
        uint32_t wr_sz = esp_apptrace_rb_write_size_get(&s_trace_buf.rb_down);
        if (wr_sz == 0) {
            break;
        }

        if (wr_sz > size - total_sz) {
            wr_sz = size - total_sz;
        }
        // ESP_APPTRACE_LOGE("esp_apptrace_trax_down_buffer_write_nolock wr %d", wr_sz);
        uint8_t *ptr = esp_apptrace_rb_produce(&s_trace_buf.rb_down, wr_sz);
        if (!ptr) {
            assert(false && "Failed to produce bytes to down buffer!");
        }
        // ESP_APPTRACE_LOGE("esp_apptrace_trax_down_buffer_write_nolock wr %d to 0x%x from 0x%x", wr_sz, ptr, data + total_sz + wr_sz);
        memcpy(ptr, data + total_sz, wr_sz);
        total_sz += wr_sz;
        // ESP_APPTRACE_LOGE("esp_apptrace_trax_down_buffer_write_nolock wr %d/%d", wr_sz, total_sz);
    }
    return total_sz;
}

static inline uint8_t *esp_apptrace_data_header_init(uint8_t *ptr, uint16_t usr_size)
{
    // it is safe to use xPortGetCoreID() in macro call because arg is used only once inside it
    ((esp_tracedata_hdr_t *)ptr)->block_sz = ESP_APPTRACE_USR_BLOCK_CORE(xPortGetCoreID()) | usr_size;
    ((esp_tracedata_hdr_t *)ptr)->wr_sz = 0;
    return ptr + sizeof(esp_tracedata_hdr_t);
}

static inline uint8_t *esp_apptrace_trax_wait4buf(uint16_t size, esp_apptrace_tmo_t *tmo, int *pended)
{
    uint8_t *ptr = NULL;

    int res = esp_apptrace_trax_block_switch_waitus(tmo);
    if (res != ESP_OK) {
        return NULL;
    }
    // check if we still have pending data
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    if (esp_apptrace_rb_read_size_get(&s_trace_buf.trax.rb_pend) > 0) {
        // if after TRAX block switch still have pending data (not all pending data have been pumped to TRAX block)
        // alloc new pending buffer
        *pended = 1;
        ptr = esp_apptrace_rb_produce(&s_trace_buf.trax.rb_pend, size);
        if (!ptr) {
            ESP_APPTRACE_LOGE("Failed to alloc pend buf 1: w-r-s %d-%d-%d!", s_trace_buf.trax.rb_pend.wr, s_trace_buf.trax.rb_pend.rd, s_trace_buf.trax.rb_pend.cur_size);
        }
    } else
#endif
    {
        // update block pointers
        if (ESP_APPTRACE_TRAX_INBLOCK_MARKER() + size > ESP_APPTRACE_TRAX_INBLOCK_GET()->sz) {
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > 0
            *pended = 1;
            ptr = esp_apptrace_rb_produce(&s_trace_buf.trax.rb_pend, size);
            if (ptr == NULL) {
                ESP_APPTRACE_LOGE("Failed to alloc pend buf 2: w-r-s %d-%d-%d!", s_trace_buf.trax.rb_pend.wr, s_trace_buf.trax.rb_pend.rd, s_trace_buf.trax.rb_pend.cur_size);
            }
#endif
        } else {
            *pended = 0;
            ptr = ESP_APPTRACE_TRAX_INBLOCK_GET()->start + ESP_APPTRACE_TRAX_INBLOCK_MARKER();
        }
    }

    return ptr;
}

static uint8_t *esp_apptrace_trax_get_buffer(uint32_t size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *buf_ptr = NULL;

    if (size > ESP_APPTRACE_USR_DATA_LEN_MAX) {
        ESP_APPTRACE_LOGE("Too large user data size %d!", size);
        return NULL;
    }

    int res = esp_apptrace_lock(tmo);
    if (res != ESP_OK) {
        return NULL;
    }
    // check for data in the pending buffer
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    if (esp_apptrace_rb_read_size_get(&s_trace_buf.trax.rb_pend) > 0) {
        // if we have buffered data try to switch TRAX block
        esp_apptrace_trax_block_switch();
        // if switch was successful, part or all pended data have been copied to TRAX block
    }
    if (esp_apptrace_rb_read_size_get(&s_trace_buf.trax.rb_pend) > 0) {
        // if we have buffered data alloc new pending buffer
        ESP_APPTRACE_LOGD("Get %d bytes from PEND buffer", size);
        buf_ptr = esp_apptrace_rb_produce(&s_trace_buf.trax.rb_pend, ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
        if (buf_ptr == NULL) {
            int pended_buf;
            buf_ptr = esp_apptrace_trax_wait4buf(ESP_APPTRACE_USR_BLOCK_RAW_SZ(size), tmo, &pended_buf);
            if (buf_ptr) {
                if (pended_buf) {
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > ESP_APPTRACE_TRAX_BLOCK_SIZE
                    esp_apptrace_trax_pend_chunk_sz_update(ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
#endif
                } else {
                    ESP_APPTRACE_LOGD("Get %d bytes from TRAX buffer", size);
                    // update cur block marker
                    ESP_APPTRACE_TRAX_INBLOCK_MARKER_UPD(ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
                }
            }
        } else {
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > ESP_APPTRACE_TRAX_BLOCK_SIZE
            esp_apptrace_trax_pend_chunk_sz_update(ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
#endif
        }
    } else
#endif
    if (ESP_APPTRACE_TRAX_INBLOCK_MARKER() + ESP_APPTRACE_USR_BLOCK_RAW_SZ(size) > ESP_APPTRACE_TRAX_INBLOCK_GET()->sz) {
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > 0
        ESP_APPTRACE_LOGD("TRAX full. Get %d bytes from PEND buffer", size);
        buf_ptr = esp_apptrace_rb_produce(&s_trace_buf.trax.rb_pend, ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
        if (buf_ptr) {
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > ESP_APPTRACE_TRAX_BLOCK_SIZE
            esp_apptrace_trax_pend_chunk_sz_update(ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
#endif
        }
#endif
        if (buf_ptr == NULL) {
            int pended_buf;
            ESP_APPTRACE_LOGD("TRAX full. Get %d bytes from pend buffer", size);
            buf_ptr = esp_apptrace_trax_wait4buf(ESP_APPTRACE_USR_BLOCK_RAW_SZ(size), tmo, &pended_buf);
            if (buf_ptr) {
                if (pended_buf) {
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > ESP_APPTRACE_TRAX_BLOCK_SIZE
                    esp_apptrace_trax_pend_chunk_sz_update(ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
#endif
                } else {
                    ESP_APPTRACE_LOGD("Got %d bytes from TRAX buffer", size);
                    // update cur block marker
                    ESP_APPTRACE_TRAX_INBLOCK_MARKER_UPD(ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
                }
            }
        }
    } else {
        ESP_APPTRACE_LOGD("Get %d bytes from TRAX buffer", size);
        // fit to curr TRAX nlock
        buf_ptr = ESP_APPTRACE_TRAX_INBLOCK_GET()->start + ESP_APPTRACE_TRAX_INBLOCK_MARKER();
        // update cur block marker
        ESP_APPTRACE_TRAX_INBLOCK_MARKER_UPD(ESP_APPTRACE_USR_BLOCK_RAW_SZ(size));
    }
    if (buf_ptr) {
        buf_ptr = esp_apptrace_data_header_init(buf_ptr, size);
    }

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_unlock() != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }

    return buf_ptr;
}

static esp_err_t esp_apptrace_trax_put_buffer(uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    int res = ESP_OK;
    esp_tracedata_hdr_t *hdr = (esp_tracedata_hdr_t *)(ptr - sizeof(esp_tracedata_hdr_t));

    // update written size
    hdr->wr_sz = hdr->block_sz;

    // TODO: mark block as busy in order not to re-use it for other tracing calls until it is completely written
    // TODO: avoid potential situation when all memory is consumed by low prio tasks which can not complete writing due to
    // higher prio tasks and the latter can not allocate buffers at all
    // this is abnormal situation can be detected on host which will receive only uncompleted buffers
    // workaround: use own memcpy which will kick-off dead tracing calls

    return res;
}

static esp_err_t esp_apptrace_trax_flush(uint32_t min_sz, esp_apptrace_tmo_t *tmo)
{
    int res = ESP_OK;

    if (ESP_APPTRACE_TRAX_INBLOCK_MARKER() < min_sz) {
        ESP_APPTRACE_LOGI("Ignore flush request for min %d bytes. Bytes in TRAX block: %d.", min_sz, ESP_APPTRACE_TRAX_INBLOCK_MARKER());
        return ESP_OK;
    }
    // switch TRAX block while size of data is more than min size
    while (ESP_APPTRACE_TRAX_INBLOCK_MARKER() > 0) {
        ESP_APPTRACE_LOGD("Try to flush %d bytes. Wait until block switch for %u us", ESP_APPTRACE_TRAX_INBLOCK_MARKER(), tmo->tmo);
        res = esp_apptrace_trax_block_switch_waitus(tmo);
        if (res != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to switch to another block!");
            return res;
        }
    }

    return res;
}

static esp_err_t esp_apptrace_trax_dest_init()
{
    for (int i = 0; i < ESP_APPTRACE_TRAX_BLOCKS_NUM; i++) {
        s_trace_buf.trax.blocks[i].start = (uint8_t *)s_trax_blocks[i];
        s_trace_buf.trax.blocks[i].sz = ESP_APPTRACE_TRAX_BLOCK_SIZE;
        s_trace_buf.trax.state.markers[i] = 0;
    }
    s_trace_buf.trax.state.in_block = ESP_APPTRACE_TRAX_INBLOCK_START;
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    esp_apptrace_rb_init(&s_trace_buf.trax.rb_pend, s_trace_buf.trax.pending_data,
                        sizeof(s_trace_buf.trax.pending_data));
#if CONFIG_ESP32_APPTRACE_PENDING_DATA_SIZE_MAX > ESP_APPTRACE_TRAX_BLOCK_SIZE
    s_trace_buf.trax.cur_pending_chunk_sz = 0;
    esp_apptrace_rb_init(&s_trace_buf.trax.rb_pend_chunk_sz, (uint8_t *)s_trace_buf.trax.pending_chunk_sz,
                        sizeof(s_trace_buf.trax.pending_chunk_sz));
#endif
#endif

    DPORT_WRITE_PERI_REG(DPORT_PRO_TRACEMEM_ENA_REG, DPORT_PRO_TRACEMEM_ENA_M);
#if CONFIG_FREERTOS_UNICORE == 0
    DPORT_WRITE_PERI_REG(DPORT_APP_TRACEMEM_ENA_REG, DPORT_APP_TRACEMEM_ENA_M);
#endif
    // Expose block 1 to host, block 0 is current trace input buffer
    DPORT_WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, TRACEMEM_MUX_BLK1_ONLY);

    return ESP_OK;
}
#endif

esp_err_t esp_apptrace_init()
{
    int res;

    if (!s_trace_buf.inited) {
        memset(&s_trace_buf, 0, sizeof(s_trace_buf));
        res = esp_apptrace_lock_initialize(&s_trace_buf.lock);
        if (res != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to init log lock (%d)!", res);
            return res;
        }
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        res = esp_apptrace_trax_dest_init();
        if (res != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to init TRAX dest data (%d)!", res);
            esp_apptrace_lock_cleanup();
            return res;
        }
#endif
    }

#if CONFIG_ESP32_APPTRACE_DEST_TRAX
    // init TRAX on this CPU
    esp_apptrace_trax_init();
#endif

    // disabled by default
    esp_apptrace_rb_init(&s_trace_buf.rb_down, NULL, 0);

    s_trace_buf.inited |= 1 << xPortGetCoreID(); // global and this CPU-specific data are inited

    return ESP_OK;
}

void esp_apptrace_down_buffer_config(uint8_t *buf, uint32_t size)
{
    esp_apptrace_rb_init(&s_trace_buf.rb_down, buf, size);
}

esp_err_t esp_apptrace_read(esp_apptrace_dest_t dest, void *buf, uint32_t *size, uint32_t user_tmo)
{
    int res = ESP_OK;
    esp_apptrace_tmo_t tmo;
    //TODO: use ptr to HW transport iface struct
    uint8_t *(*apptrace_get_down_buffer)(uint32_t *, esp_apptrace_tmo_t *);
    esp_err_t (*apptrace_put_down_buffer)(uint8_t *, esp_apptrace_tmo_t *);

    if (dest == ESP_APPTRACE_DEST_TRAX) {
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        apptrace_get_down_buffer = esp_apptrace_trax_down_buffer_get;
        apptrace_put_down_buffer = esp_apptrace_trax_down_buffer_put;
#else
        ESP_APPTRACE_LOGE("Application tracing via TRAX is disabled in menuconfig!");
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else {
        ESP_APPTRACE_LOGE("Trace destinations other then TRAX are not supported yet!");
        return ESP_ERR_NOT_SUPPORTED;
    }

    //TODO: callback system
    esp_apptrace_tmo_init(&tmo, user_tmo);
    uint32_t act_sz = *size;
    *size = 0;
    uint8_t * ptr = apptrace_get_down_buffer(&act_sz, &tmo);
    if (ptr && act_sz > 0) {
        ESP_APPTRACE_LOGD("Read %d bytes from host", act_sz);
        memcpy(buf, ptr, act_sz);
        res = apptrace_put_down_buffer(ptr, &tmo);
        *size = act_sz;
    }

    return res;
}

uint8_t *esp_apptrace_down_buffer_get(esp_apptrace_dest_t dest, uint32_t *size, uint32_t user_tmo)
{
    esp_apptrace_tmo_t tmo;
    //TODO: use ptr to HW transport iface struct
    uint8_t *(*apptrace_get_down_buffer)(uint32_t *, esp_apptrace_tmo_t *);

    if (dest == ESP_APPTRACE_DEST_TRAX) {
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        apptrace_get_down_buffer = esp_apptrace_trax_down_buffer_get;
#else
        ESP_APPTRACE_LOGE("Application tracing via TRAX is disabled in menuconfig!");
        return NULL;
#endif
    } else {
        ESP_APPTRACE_LOGE("Trace destinations other then TRAX are not supported yet!");
        return NULL;
    }

    // ESP_APPTRACE_LOGE("esp_apptrace_down_buffer_get %d", *size);
    esp_apptrace_tmo_init(&tmo, user_tmo);
    return apptrace_get_down_buffer(size, &tmo);
}

esp_err_t esp_apptrace_down_buffer_put(esp_apptrace_dest_t dest, uint8_t *ptr, uint32_t user_tmo)
{
    esp_apptrace_tmo_t tmo;
    //TODO: use ptr to HW transport iface struct
    esp_err_t (*apptrace_put_down_buffer)(uint8_t *, esp_apptrace_tmo_t *);

    if (dest == ESP_APPTRACE_DEST_TRAX) {
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        apptrace_put_down_buffer = esp_apptrace_trax_down_buffer_put;
#else
        ESP_APPTRACE_LOGE("Application tracing via TRAX is disabled in menuconfig!");
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else {
        ESP_APPTRACE_LOGE("Trace destinations other then TRAX are not supported yet!");
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    return apptrace_put_down_buffer(ptr, &tmo);
}

esp_err_t esp_apptrace_write(esp_apptrace_dest_t dest, const void *data, uint32_t size, uint32_t user_tmo)
{
    uint8_t *ptr = NULL;
    esp_apptrace_tmo_t tmo;
    //TODO: use ptr to HW transport iface struct
    uint8_t *(*apptrace_get_buffer)(uint32_t, esp_apptrace_tmo_t *);
    esp_err_t (*apptrace_put_buffer)(uint8_t *, esp_apptrace_tmo_t *);

    if (dest == ESP_APPTRACE_DEST_TRAX) {
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        apptrace_get_buffer = esp_apptrace_trax_get_buffer;
        apptrace_put_buffer = esp_apptrace_trax_put_buffer;
#else
        ESP_APPTRACE_LOGE("Application tracing via TRAX is disabled in menuconfig!");
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else {
        ESP_APPTRACE_LOGE("Trace destinations other then TRAX are not supported yet!");
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    ptr = apptrace_get_buffer(size, &tmo);
    if (ptr == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // actually can be suspended here by higher prio tasks/ISRs
    //TODO: use own memcpy with dead trace calls kick-off algo and tmo expiration check
    memcpy(ptr, data, size);

    // now indicate that this buffer is ready to be sent off to host
    return apptrace_put_buffer(ptr, &tmo);
}

int esp_apptrace_vprintf_to(esp_apptrace_dest_t dest, uint32_t user_tmo, const char *fmt, va_list ap)
{
    uint16_t nargs = 0;
    uint8_t *pout, *p = (uint8_t *)fmt;
    esp_apptrace_tmo_t tmo;
    //TODO: use ptr to HW transport iface struct
    uint8_t *(*apptrace_get_buffer)(uint32_t, esp_apptrace_tmo_t *);
    esp_err_t (*apptrace_put_buffer)(uint8_t *, esp_apptrace_tmo_t *);

    if (dest == ESP_APPTRACE_DEST_TRAX) {
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        apptrace_get_buffer = esp_apptrace_trax_get_buffer;
        apptrace_put_buffer = esp_apptrace_trax_put_buffer;
#else
        ESP_APPTRACE_LOGE("Application tracing via TRAX is disabled in menuconfig!");
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else {
        ESP_APPTRACE_LOGE("Trace destinations other then TRAX are not supported yet!");
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    ESP_APPTRACE_LOGD("fmt %x", fmt);
    while ((p = (uint8_t *)strchr((char *)p, '%')) && nargs < ESP_APPTRACE_MAX_VPRINTF_ARGS) {
        p++;
        if (*p != '%' && *p != 0) {
            nargs++;
        }
    }
    ESP_APPTRACE_LOGD("nargs = %d", nargs);
    if (p) {
        ESP_APPTRACE_LOGE("Failed to store all printf args!");
    }

    pout = apptrace_get_buffer(1 + sizeof(char *) + nargs * sizeof(uint32_t), &tmo);
    if (pout == NULL) {
        ESP_APPTRACE_LOGE("Failed to get buffer!");
        return -1;
    }
    p = pout;
    *pout = nargs;
    pout++;
    *(const char **)pout = fmt;
    pout += sizeof(char *);
    while (nargs-- > 0) {
        uint32_t arg = va_arg(ap, uint32_t);
        *(uint32_t *)pout = arg;
        pout += sizeof(uint32_t);
        ESP_APPTRACE_LOGD("arg %x", arg);
    }

    int ret = apptrace_put_buffer(p, &tmo);
    if (ret != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to put printf buf (%d)!", ret);
        return -1;
    }

    return (pout - p);
}

int esp_apptrace_vprintf(const char *fmt, va_list ap)
{
    return esp_apptrace_vprintf_to(ESP_APPTRACE_DEST_TRAX, /*ESP_APPTRACE_TMO_INFINITE*/0, fmt, ap);
}

uint8_t *esp_apptrace_buffer_get(esp_apptrace_dest_t dest, uint32_t size, uint32_t user_tmo)
{
    esp_apptrace_tmo_t tmo;
    //TODO: use ptr to HW transport iface struct
    uint8_t *(*apptrace_get_buffer)(uint32_t, esp_apptrace_tmo_t *);

    if (dest == ESP_APPTRACE_DEST_TRAX) {
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        apptrace_get_buffer = esp_apptrace_trax_get_buffer;
#else
        ESP_APPTRACE_LOGE("Application tracing via TRAX is disabled in menuconfig!");
        return NULL;
#endif
    } else {
        ESP_APPTRACE_LOGE("Trace destinations other then TRAX are not supported yet!");
        return NULL;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    return apptrace_get_buffer(size, &tmo);
}

esp_err_t esp_apptrace_buffer_put(esp_apptrace_dest_t dest, uint8_t *ptr, uint32_t user_tmo)
{
    esp_apptrace_tmo_t tmo;
    //TODO: use ptr to HW transport iface struct
    esp_err_t (*apptrace_put_buffer)(uint8_t *, esp_apptrace_tmo_t *);

    if (dest == ESP_APPTRACE_DEST_TRAX) {
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        apptrace_put_buffer = esp_apptrace_trax_put_buffer;
#else
        ESP_APPTRACE_LOGE("Application tracing via TRAX is disabled in menuconfig!");
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else {
        ESP_APPTRACE_LOGE("Trace destinations other then TRAX are not supported yet!");
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    return apptrace_put_buffer(ptr, &tmo);
}

esp_err_t esp_apptrace_flush_nolock(esp_apptrace_dest_t dest, uint32_t min_sz, uint32_t usr_tmo)
{
    esp_apptrace_tmo_t tmo;
    //TODO: use ptr to HW transport iface struct
    esp_err_t (*apptrace_flush)(uint32_t, esp_apptrace_tmo_t *);

    if (dest == ESP_APPTRACE_DEST_TRAX) {
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        apptrace_flush = esp_apptrace_trax_flush;
#else
        ESP_APPTRACE_LOGE("Application tracing via TRAX is disabled in menuconfig!");
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else {
        ESP_APPTRACE_LOGE("Trace destinations other then TRAX are not supported yet!");
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, usr_tmo);
    return apptrace_flush(min_sz, &tmo);
}

esp_err_t esp_apptrace_flush(esp_apptrace_dest_t dest, uint32_t usr_tmo)
{
    int res;
    esp_apptrace_tmo_t tmo;

    esp_apptrace_tmo_init(&tmo, usr_tmo);
    res = esp_apptrace_lock(&tmo);
    if (res != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to lock apptrace data (%d)!", res);
        return res;
    }

    res = esp_apptrace_flush_nolock(dest, 0, esp_apptrace_tmo_remaining_us(&tmo));
    if (res != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to flush apptrace data (%d)!", res);
    }

    if (esp_apptrace_unlock() != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }

    return res;
}
#endif
