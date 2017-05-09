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

// Xtensa has useful feature: TRAX debug module. It allows recording program execution flow during run-time without disturbing CPU commands flow.
// Exectution flow data are written to configurable Trace RAM block. Besides accessing Trace RAM itself TRAX module also allows to read/write
// trace memory via its registers by means of JTAG, APB or ERI transactions.
// ESP32 has two Xtensa cores with separate TRAX modules on them and provides two special memory regions to be used as trace memory.
// ESP32 allows muxing access to trace memory blocks in such a way that while one block is accessed by CPUs another can be accessed via JTAG by host
// via reading/writing TRAX registers. Block muxing is configurable at run-time and allows switching trace memory blocks between
// accessors in round-robin fashion so they can read/write separate memory blocks without disturbing each other.
// This moduile implements application tracing feature based on above mechanisms. This feature allows to transfer arbitrary user data to
// host via JTAG with minimal impact on system performance. This module is implied to be used in the following tracing scheme.

//                                                    ------>------                                         ----- (host components) -----
//                                                    |           |                                         |                           |
// ---------------   -----------------------     -----------------------     ----------------    ------     ---------   -----------------
// |apptrace user|-->|target tracing module|<--->|TRAX_MEM0 | TRAX_MEM1|---->|TRAX_DATA_REGS|<-->|JTAG|<--->|OpenOCD|-->|trace data file|
// ---------------   -----------------------     -----------------------     ----------------    ------     ---------   -----------------
//                             |                      |           |                                |
//                             |                      ------<------          ----------------      |
//                             |<------------------------------------------->|TRAX_CTRL_REGS|<---->|
//                                                                           ----------------

// In general tracing happens in the following way. User aplication requests tracing module to send some data by calling esp_apptrace_buffer_get(),
// moduile allocates necessary buffer in current input trace block. Then user fills received buffer with data and calls esp_apptrace_buffer_put().
// When current input trace block is filled with app data it is exposed to host and the second block becomes input one and buffer filling restarts.
// While target application fills one memory block host reads another block via JTAG.
// To control buffer switching and for other communication purposes this implementation uses some TRAX registers. It is safe since HW TRAX tracing
// can not be used along with application tracing feature so these registers are freely readable/writeable via JTAG from host and via ERI from ESP32 cores.
// So this implementation's target CPU overhead is produced only by calls to allocate/manage buffers and data copying.
// On host special OpenOCD command must be used to read trace data.

// 2.1.1.1 TRAX Registers layout
// =============================

// This module uses two TRAX HW registers to communicate with host SW (OpenOCD).
//  - Control register uses TRAX_DELAYCNT as storage. Only lower 24 bits of TRAX_DELAYCNT are writable. Control register has the following bitfields:
//   | 31..XXXXXX..24 | 23 .(host_connect). 23| 22..(block_id)..15 | 14..(block_len)..0 |
//    14..0  bits - actual length of user data in trace memory block. Target updates it every time it fills memory block and exposes it to host.
//                  Host writes zero to this field when it finishes reading exposed block;
//    22..15 bits - trace memory block transfer ID. Block counter. It can overflow. Updated by target, host should not modify it. Actually can be 1-2 bits;
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
//    There is menuconfig option CONFIG_ESP32_APPTRACE_ONPANIC_HOST_FLUSH_TRAX_THRESH which control the threshold for flushing data on panic.
//  - Streaming mode. Tracing module enters this mode when host connects to targets and sets respective bit in control register. In this mode tracing
//    module waits for specified time until host read all the data from exposed block.
//    On panic tracing module waits (timeout is configured via menuconfig via ESP32_APPTRACE_ONPANIC_HOST_FLUSH_TMO) for the host to read all data
//    from the previously exposed block.

// 4. Communication Protocol
// =========================

// 4.1 Trace Memory Blocks
// ^^^^^^^^^^^^^^^^^^^^^^^^^

// Communication is controlled via special register. Host periodically polls control register on each core to find out if there are any data avalable.
// When current input trace memory block is filled tracing module exposes block to host and updates block_len and block_id fields in control register.
// Host reads new register value and according to it starts reading data from exposed block. Meanwhile target starts filling another trace block.
// When host finishes reading the block it clears block_len field in control register indicating to target that it is ready to accept the next block.

// 4.2 User Data Chunks Level
// --------------------------

// Since trace memory block is shared between user data chunks and data copying is performed on behalf of the API user (in its normal context) in
// multithreading environment it can happen that task/ISR which copies data is preempted by another high prio task/ISR. So it is possible situation
// that task/ISR will fail to complete filling its data chunk before the whole trace block is exposed to the host. To handle such conditions tracing
// module prepends all user data chunks with 4 bytes header which contains allocated buffer size and actual data length within it. OpenOCD command
// which reads application traces will report error when it will read incompleted user data block.

// 4.3 Targets Connection/Disconnection
// ------------------------------------

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
// There are some differences how mutex behaves when it is used from task and ISR context when timeout is non-zero:
// - In task context when mutex can not be locked portYIELD() is called before check for timeout condition to alow othet tasks work on the same CPU.
// - In ISR context when mutex can not be locked nothing is done before expired time check.
// WARNING: Care must be taken when selecting timeout values for trace calls from ISRs. Tracing module does not care about watchdogs when waiting on internal locks
// and when waiting for host to complete previous block reading, so if wating timeout value exceedes watchdog's one it can lead to system reboot.

// 6. Timeouts
// ------------

// Timeout mechanism is based on xthal_get_ccount() routine and supports timeout values in micorseconds.
// There are two situations when task/ISR can be delayed by tracing API call. Timeout mechanism takes into account both conditions:
// - Trace data are locked by another task/ISR. When wating on trace data lock.
// - Current TRAX memory input block is full when working in streaming mode (host is connected). When waiting for host to complete previous block reading.
// When wating for any of above conditions xthal_get_ccount() is called periodically to calculate time elapsed from trace API routine entry. When elapsed
// time exceeds specified timeout value operation is canceled and ESP_ERR_TIMEOUT code is returned.

// ALSO SEE example usage of application tracing module in 'components/log/README.rst'

#include <string.h>
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "eri.h"
#include "trax.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "esp_app_trace.h"

#if CONFIG_ESP32_APPTRACE_ENABLE
#define ESP_APPTRACE_DEBUG_STATS_ENABLE         0
#define ESP_APPTRACE_BUF_HISTORY_DEPTH          (16*100)

#define ESP_APPTRACE_MAX_VPRINTF_ARGS           256

#define ESP_APPTRACE_PRINT_LOCK_NONE            0
#define ESP_APPTRACE_PRINT_LOCK_SEM             1
#define ESP_APPTRACE_PRINT_LOCK_MUX             2
#define ESP_APPTRACE_PRINT_LOCK                 ESP_APPTRACE_PRINT_LOCK_NONE//ESP_APPTRACE_PRINT_LOCK_SEM

#define ESP_APPTRACE_USE_LOCK_SEM               0 // 1 - semaphore (now may be broken), 0 - portMUX_TYPE

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
const static char *TAG = "esp_apptrace";

#if ESP_APPTRACE_PRINT_LOCK != ESP_APPTRACE_PRINT_LOCK_NONE
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
// | 31..XXXXXX..24 | 23 .(host_connect). 23| 22..(block_id)..15 | 14..(block_len)..0 |
#define ESP_APPTRACE_TRAX_CTRL_REG              ERI_TRAX_DELAYCNT
#define ESP_APPTRACE_TRAX_STAT_REG              ERI_TRAX_TRIGGERPC

#define ESP_APPTRACE_TRAX_BLOCK_LEN_MSK         0x7FFFUL
#define ESP_APPTRACE_TRAX_BLOCK_LEN(_l_)        ((_l_) & ESP_APPTRACE_TRAX_BLOCK_LEN_MSK)
#define ESP_APPTRACE_TRAX_BLOCK_LEN_GET(_v_)    ((_v_) & ESP_APPTRACE_TRAX_BLOCK_LEN_MSK)
#define ESP_APPTRACE_TRAX_BLOCK_ID_MSK          0xFFUL
#define ESP_APPTRACE_TRAX_BLOCK_ID(_id_)        (((_id_) & ESP_APPTRACE_TRAX_BLOCK_ID_MSK) << 15)
#define ESP_APPTRACE_TRAX_BLOCK_ID_GET(_v_)     (((_v_) >> 15) & ESP_APPTRACE_TRAX_BLOCK_ID_MSK)
#define ESP_APPTRACE_TRAX_HOST_CONNECT          (1 << 23)

static volatile uint8_t *s_trax_blocks[] = {
    (volatile uint8_t *) 0x3FFFC000,
    (volatile uint8_t *) 0x3FFF8000
};

#define ESP_APPTRACE_TRAX_BLOCKS_NUM            (sizeof(s_trax_blocks)/sizeof(s_trax_blocks[0]))

//#define ESP_APPTRACE_TRAX_BUFFER_SIZE           (ESP_APPTRACE_TRAX_BLOCK_SIZE/4)

#define ESP_APPTRACE_TRAX_INBLOCK_START         0//(ESP_APPTRACE_TRAX_BLOCK_ID_MSK - 4)


#define ESP_APPTRACE_TRAX_INBLOCK_MARKER_PTR_GET()  (&s_trace_buf.trax.state.markers[s_trace_buf.trax.state.in_block % 2])
#define ESP_APPTRACE_TRAX_INBLOCK_GET()             (&s_trace_buf.trax.blocks[s_trace_buf.trax.state.in_block % 2])

#if ESP_APPTRACE_DEBUG_STATS_ENABLE == 1
/** keeps info about apptrace API (write/get buffer) caller and internal module's data related to that call
 * NOTE: used for module debug purposes, currently this functionality is partially broken,
 * but can be useful in future
 */
typedef struct {
    uint32_t    hnd;        // task/ISR handle
    uint32_t    ts;         // timestamp
    uint32_t    stamp;      // test (user) trace buffer stamp
    uint32_t    in_block;   // TRAX input block ID
    uint32_t    eri_len[2]; // contents of ERI control register upon entry to / exit from API routine
    uint32_t    wr_err;     // number of trace write errors
} esp_trace_buffer_wr_hitem_t;

/** apptrace API calls history. History is organized as ring buffer*/
typedef struct {
    uint32_t                    hist_rd; // the first history entry index
    uint32_t                    hist_wr; // the last history entry index
    esp_trace_buffer_wr_hitem_t hist[ESP_APPTRACE_BUF_HISTORY_DEPTH]; // history data
} esp_trace_buffer_wr_stats_t;

/** trace module stats */
typedef struct {
    esp_trace_buffer_wr_stats_t wr;
} esp_trace_buffer_stats_t;
#endif

/** Trace data header. Every user data chunk is prepended with this header.
 * User allocates block with esp_apptrace_buffer_get and then fills it with data,
 * in multithreading environment it can happen that tasks gets buffer and then gets interrupted,
 * so it is possible that user data are incomplete when TRAX memory block is exposed to the host.
 * In this case host SW will see that wr_sz < block_sz and will report error.
 */
typedef struct {
    uint16_t   block_sz; // size of allocated block for user data
    uint16_t   wr_sz;    // size of actually written data
} esp_tracedata_hdr_t;

/** TRAX HW transport state */
typedef struct {
    uint32_t                   in_block;                                // input block ID
    uint32_t                   markers[ESP_APPTRACE_TRAX_BLOCKS_NUM];   // block filling level markers
#if ESP_APPTRACE_DEBUG_STATS_ENABLE == 1
    esp_trace_buffer_stats_t   stats;                                   // stats
#endif
} esp_apptrace_trax_state_t;

/** memory block parameters */
typedef struct {
    uint8_t   *start;   // start address
    uint32_t   sz;      // size
} esp_apptrace_mem_block_t;

/** TRAX HW transport data */
typedef struct {
    volatile esp_apptrace_trax_state_t  state;                                  // state
    esp_apptrace_mem_block_t            blocks[ESP_APPTRACE_TRAX_BLOCKS_NUM];   // memory blocks
} esp_apptrace_trax_data_t;

/** tracing module synchronization lock */
typedef struct {
    volatile unsigned int   irq_stat;   // local (on 1 CPU) IRQ state
    portMUX_TYPE            portmux;    // mux for synchronization
} esp_apptrace_lock_t;

#define ESP_APPTRACE_MUX_GET(_m_)   (&(_m_)->portmux)

/** tracing module internal data */
typedef struct {
#if ESP_APPTRACE_USE_LOCK_SEM == 1
    SemaphoreHandle_t           lock;
#else
    esp_apptrace_lock_t         lock;   // sync lock
#endif
    uint8_t                     inited; // module initialization state flag
    esp_apptrace_trax_data_t    trax;   // TRAX HW transport data
} esp_apptrace_buffer_t;

/** waiting timeout data */
typedef struct {
    uint32_t   start;   // waiting start (in ticks)
    uint32_t   tmo;     // timeout (in us)
} esp_apptrace_tmo_t;

static esp_apptrace_buffer_t s_trace_buf;

#if ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_SEM
static SemaphoreHandle_t    s_log_lock;
#elif ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_MUX
static esp_apptrace_lock_t   s_log_lock;
#endif

static inline void esp_apptrace_tmo_init(esp_apptrace_tmo_t *tmo, uint32_t user_tmo)
{
    tmo->start = xthal_get_ccount();
    tmo->tmo = user_tmo;
}

static esp_err_t esp_apptrace_tmo_check(esp_apptrace_tmo_t *tmo)
{
    unsigned cur, elapsed;

    if (tmo->tmo != ESP_APPTRACE_TMO_INFINITE) {
        cur = xthal_get_ccount();
        if (tmo->start <= cur) {
            elapsed = cur - tmo->start;
        } else {
            elapsed = 0xFFFFFFFF - tmo->start + cur;
        }
        if (ESP_APPTRACE_CPUTICKS2US(elapsed) >= tmo->tmo) {
            return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

#if ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_MUX || ESP_APPTRACE_USE_LOCK_SEM == 0
static inline void esp_apptrace_mux_init(esp_apptrace_lock_t *mux)
{
    ESP_APPTRACE_MUX_GET(mux)->mux = portMUX_FREE_VAL;
    mux->irq_stat = 0;
}

static esp_err_t esp_apptrace_lock_take(esp_apptrace_lock_t *mux, uint32_t tmo)
{
    uint32_t res = ~portMUX_FREE_VAL;
    esp_apptrace_tmo_t sleeping_tmo;

    esp_apptrace_tmo_init(&sleeping_tmo, tmo);
    while (1) {
        res = (xPortGetCoreID() << portMUX_VAL_SHIFT) | portMUX_MAGIC_VAL;
        // first disable IRQs on this CPU, this will prevent current task from been
        // preempted by higher prio tasks, otherwise deadlock can happen:
        // when lower prio task took mux and then preempted by higher prio one which also tries to
        // get mux with INFINITE timeout
        unsigned int irq_stat = portENTER_CRITICAL_NESTED();
        // Now try to lock mux
        uxPortCompareSet(&ESP_APPTRACE_MUX_GET(mux)->mux, portMUX_FREE_VAL, &res);
        if (res == portMUX_FREE_VAL) {
            // do not enable IRQs, we will held them disabled until mux is unlocked
            // we do not need to flush cache region for mux->irq_stat because it is used
            // to hold and restore IRQ state only for CPU which took mux, other CPUs will not use this value
            mux->irq_stat = irq_stat;
            break;
        }
        // if mux is locked by other task/ISR enable IRQs and let other guys work
        portEXIT_CRITICAL_NESTED(irq_stat);

        if (!xPortInIsrContext()) {
            portYIELD();
        }

        int err = esp_apptrace_tmo_check(&sleeping_tmo);
        if (err != ESP_OK) {
            return err;
        }
    }

    return ESP_OK;
}

esp_err_t esp_apptrace_mux_give(esp_apptrace_lock_t *mux)
{
    esp_err_t ret = ESP_OK;
    uint32_t res = 0;
    unsigned int irq_stat;

    res = portMUX_FREE_VAL;

    // first of all save a copy of IRQ status for this locker because uxPortCompareSet will unlock mux and tasks/ISRs
    // from other core can overwrite mux->irq_stat
    irq_stat = mux->irq_stat;
    uxPortCompareSet(&ESP_APPTRACE_MUX_GET(mux)->mux, (xPortGetCoreID() << portMUX_VAL_SHIFT) | portMUX_MAGIC_VAL, &res);
    // enable local interrupts
    portEXIT_CRITICAL_NESTED(irq_stat);

    if ( ((res & portMUX_VAL_MASK) >> portMUX_VAL_SHIFT) == xPortGetCoreID() ) {
        // nothing to do
    } else if ( res == portMUX_FREE_VAL ) {
        ret = ESP_FAIL; // should never get here
    } else {
        ret = ESP_FAIL;  // should never get here
    }
    return ret;
}
#endif

static inline esp_err_t esp_apptrace_log_init()
{
#if ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_SEM
    s_log_lock = xSemaphoreCreateBinary();
    if (!s_log_lock) {
        ets_printf("%s: Failed to create print lock sem!", TAG);
        return ESP_FAIL;
    }
    xSemaphoreGive(s_log_lock);
#elif ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_MUX
    esp_apptrace_mux_init(&s_log_lock);
#endif
    return ESP_OK;
}

static inline void esp_apptrace_log_cleanup()
{
#if ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_SEM
    vSemaphoreDelete(s_log_lock);
#endif
}

static inline int esp_apptrace_log_lock()
{
#if ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_SEM
    BaseType_t ret;
    if (xPortInIsrContext()) {
        ret = xSemaphoreTakeFromISR(s_print_lock, NULL);
    } else {
        ret = xSemaphoreTake(s_print_lock, portMAX_DELAY);
    }
    return ret;
#elif ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_MUX
    int ret = esp_apptrace_lock_take(&s_log_lock, ESP_APPTRACE_TMO_INFINITE);
    return ret;
#endif
    return 0;
}

static inline void esp_apptrace_log_unlock()
{
#if ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_SEM
    if (xPortInIsrContext()) {
        xSemaphoreGiveFromISR(s_log_lock, NULL);
    } else {
        xSemaphoreGive(s_log_lock);
    }
#elif ESP_APPTRACE_PRINT_LOCK == ESP_APPTRACE_PRINT_LOCK_MUX
    esp_apptrace_mux_give(&s_log_lock);
#endif
}

esp_err_t esp_apptrace_lock_init()
{
#if ESP_APPTRACE_USE_LOCK_SEM == 1
    s_trace_buf.lock = xSemaphoreCreateBinary();
    if (!s_trace_buf.lock) {
        ESP_APPTRACE_LOGE("Failed to create lock!");
        return ESP_FAIL;
    }
    xSemaphoreGive(s_trace_buf.lock);
#else
    esp_apptrace_mux_init(&s_trace_buf.lock);
#endif
    return ESP_OK;
}

esp_err_t esp_apptrace_lock_cleanup()
{
#if ESP_APPTRACE_USE_LOCK_SEM == 1
    vSemaphoreDelete(s_trace_buf.lock);
#endif
    return ESP_OK;
}

esp_err_t esp_apptrace_lock(uint32_t *tmo)
{
    unsigned cur, elapsed, start = xthal_get_ccount();

#if ESP_APPTRACE_USE_LOCK_SEM == 1
    BaseType_t ret;
    if (xPortInIsrContext()) {
        ret = xSemaphoreTakeFromISR(s_trace_buf.lock, NULL);
    } else {
        ret = xSemaphoreTake(s_trace_buf.lock, portTICK_PERIOD_MS * (*tmo) / 1000);
    }
    if (ret != pdTRUE) {
        return ESP_FAIL;
    }
#else
    esp_err_t ret = esp_apptrace_lock_take(&s_trace_buf.lock, *tmo);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
#endif
    // decrease tmo by actual waiting time
    cur = xthal_get_ccount();
    if (start <= cur) {
        elapsed = cur - start;
    } else {
        elapsed = ULONG_MAX - start + cur;
    }
    if (ESP_APPTRACE_CPUTICKS2US(elapsed) > *tmo) {
        *tmo = 0;
    } else {
        *tmo -= ESP_APPTRACE_CPUTICKS2US(elapsed);
    }

    return ESP_OK;
}

esp_err_t esp_apptrace_unlock()
{
    esp_err_t ret = ESP_OK;
#if ESP_APPTRACE_USE_LOCK_SEM == 1
    if (xPortInIsrContext()) {
        xSemaphoreGiveFromISR(s_trace_buf.lock, NULL);
    } else {
        xSemaphoreGive(s_trace_buf.lock);
    }
#else
    ret = esp_apptrace_mux_give(&s_trace_buf.lock);
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
#if ESP_APPTRACE_DEBUG_STATS_ENABLE == 1
    if (s_trace_buf.state.stats.wr.hist_wr < ESP_APPTRACE_BUF_HISTORY_DEPTH) {
        esp_trace_buffer_wr_hitem_t *hi = (esp_trace_buffer_wr_hitem_t *)&s_trace_buf.state.stats.wr.hist[s_trace_buf.state.stats.wr.hist_wr - 1];
        hi->eri_len[1] = ctrl_reg;
    }
#endif
    uint32_t host_connected = ESP_APPTRACE_TRAX_HOST_CONNECT & ctrl_reg;
    if (host_connected) {
        uint32_t acked_block = ESP_APPTRACE_TRAX_BLOCK_ID_GET(ctrl_reg);
        uint32_t host_to_read = ESP_APPTRACE_TRAX_BLOCK_LEN_GET(ctrl_reg);
        if (host_to_read != 0 || acked_block != (s_trace_buf.trax.state.in_block & ESP_APPTRACE_TRAX_BLOCK_ID_MSK)) {
            // ESP_APPTRACE_LOGE("HC[%d]: Can not switch %x %d %x %x/%lx", xPortGetCoreID(), ctrl_reg, host_to_read, acked_block,
            //     s_trace_buf.trax.state.in_block & ESP_APPTRACE_TRAX_BLOCK_ID_MSK, s_trace_buf.trax.state.in_block);
            res = ESP_ERR_NO_MEM;
            goto _on_func_exit;
        }
    }
    s_trace_buf.trax.state.markers[new_block_num] = 0;
    // switch to new block
    s_trace_buf.trax.state.in_block++;

    DPORT_WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, new_block_num ? TRACEMEM_MUX_BLK0_ONLY : TRACEMEM_MUX_BLK1_ONLY);
    eri_write(ESP_APPTRACE_TRAX_CTRL_REG, ESP_APPTRACE_TRAX_BLOCK_ID(s_trace_buf.trax.state.in_block) |
              host_connected | ESP_APPTRACE_TRAX_BLOCK_LEN(s_trace_buf.trax.state.markers[prev_block_num]));

_on_func_exit:
    // exit ERI update critical section
    eri_write(ESP_APPTRACE_TRAX_STAT_REG, 0x0);
    asm volatile (
        "    .global     __esp_apptrace_trax_eri_updated\n"
        "__esp_apptrace_trax_eri_updated:\n"); // host will set bp here to resolve collision at streaming start
    return res;
}

static esp_err_t esp_apptrace_trax_block_switch_waitus(uint32_t tmo)
{
    int res;
    esp_apptrace_tmo_t sleeping_tmo;

    esp_apptrace_tmo_init(&sleeping_tmo, tmo);

    while ((res = esp_apptrace_trax_block_switch()) != ESP_OK) {
        res = esp_apptrace_tmo_check(&sleeping_tmo);
        if (res != ESP_OK) {
            break;
        }
    }
    return res;
}

static uint8_t *esp_apptrace_trax_get_buffer(size_t size, uint32_t *tmo)
{
    uint8_t *buf_ptr = NULL;
    volatile uint32_t *cur_block_marker;
    esp_apptrace_mem_block_t *cur_block;

    int res = esp_apptrace_lock(tmo);
    if (res != ESP_OK) {
        return NULL;
    }

#if ESP_APPTRACE_DEBUG_STATS_ENABLE == 1
    esp_trace_buffer_wr_hitem_t *hi = NULL;
    if (s_trace_buf.state.stats.wr.hist_wr < ESP_APPTRACE_BUF_HISTORY_DEPTH) {
        hi = (esp_trace_buffer_wr_hitem_t *)&s_trace_buf.state.stats.wr.hist[s_trace_buf.state.stats.wr.hist_wr++];
        hi->hnd = *(uint32_t *)(buf + 0);
        hi->ts = *(uint32_t *)(buf + sizeof(uint32_t));
        hi->stamp = *(buf + 2 * sizeof(uint32_t));
        hi->in_block = s_trace_buf.state.in_block;
        hi->wr_err = 0;
        hi->eri_len[0] = eri_read(ESP_APPTRACE_TRAX_CTRL_REG);
        if (s_trace_buf.state.stats.wr.hist_wr == ESP_APPTRACE_BUF_HISTORY_DEPTH) {
            s_trace_buf.state.stats.wr.hist_wr = 0;
        }
        if (s_trace_buf.state.stats.wr.hist_wr == s_trace_buf.state.stats.wr.hist_rd) {
            s_trace_buf.state.stats.wr.hist_rd++;
            if (s_trace_buf.state.stats.wr.hist_rd == ESP_APPTRACE_BUF_HISTORY_DEPTH) {
                s_trace_buf.state.stats.wr.hist_rd = 0;
            }
        }
    }
#endif

    cur_block_marker = ESP_APPTRACE_TRAX_INBLOCK_MARKER_PTR_GET();
    cur_block = ESP_APPTRACE_TRAX_INBLOCK_GET();

    if (*cur_block_marker + size + sizeof(esp_tracedata_hdr_t) >= cur_block->sz) {
        // flush data, we can not unlock apptrace until we have buffer for all user data
        // otherwise other tasks/ISRs can get control and write their data between chunks of this data
        res = esp_apptrace_trax_block_switch_waitus(/*size + sizeof(esp_tracedata_hdr_t),*/*tmo);
        if (res != ESP_OK) {
            if (esp_apptrace_unlock() != ESP_OK) {
                ESP_APPTRACE_LOGE("Failed to unlock apptrace data!");
                // there is a bug, should never get here
            }
            return NULL;
        }
        // we switched to new block, update TRAX block pointers
        cur_block_marker = ESP_APPTRACE_TRAX_INBLOCK_MARKER_PTR_GET();
        cur_block = ESP_APPTRACE_TRAX_INBLOCK_GET();
    }

    buf_ptr = cur_block->start + *cur_block_marker;
    ((esp_tracedata_hdr_t *)buf_ptr)->block_sz = size;
    ((esp_tracedata_hdr_t *)buf_ptr)->wr_sz = 0;

    *cur_block_marker += size + sizeof(esp_tracedata_hdr_t);

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_unlock() != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to unlock apptrace data!");
        // there is a bug, should never get here
    }

    return buf_ptr + sizeof(esp_tracedata_hdr_t);
}

static esp_err_t esp_apptrace_trax_put_buffer(uint8_t *ptr, uint32_t *tmo)
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

static esp_err_t esp_apptrace_trax_flush(uint32_t min_sz, uint32_t tmo)
{
    volatile uint32_t *in_block_marker;
    int res = ESP_OK;

    in_block_marker = ESP_APPTRACE_TRAX_INBLOCK_MARKER_PTR_GET();
    if (*in_block_marker > min_sz) {
        ESP_APPTRACE_LOGD("Wait until block switch for %u us", tmo);
        res = esp_apptrace_trax_block_switch_waitus(/*0 query any size,*/tmo);
        if (res != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to switch to another block");
            return res;
        }
        ESP_APPTRACE_LOGD("Flushed last block %u bytes", *in_block_marker);
        *in_block_marker = 0;
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
        res = esp_apptrace_log_init();
        if (res != ESP_OK) {
            ets_printf("%s: Failed to init log lock (%d)!", TAG, res);
            return res;
        }
        //memset(&s_trace_buf, 0, sizeof(s_trace_buf));
        res = esp_apptrace_lock_init(&s_trace_buf.lock);
        if (res != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to init log lock (%d)!", res);
            esp_apptrace_log_cleanup();
            return res;
        }
#if CONFIG_ESP32_APPTRACE_DEST_TRAX
        res = esp_apptrace_trax_dest_init();
        if (res != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to init TRAX dest data (%d)!", res);
            esp_apptrace_lock_cleanup();
            esp_apptrace_log_cleanup();
            return res;
        }
#endif
    }

#if CONFIG_ESP32_APPTRACE_DEST_TRAX
    // init TRAX on this CPU
    esp_apptrace_trax_init();
#endif

    s_trace_buf.inited |= 1 << xPortGetCoreID(); // global and this CPU-specific data are inited

    return ESP_OK;
}

esp_err_t esp_apptrace_write(esp_apptrace_dest_t dest, void *data, size_t size, uint32_t user_tmo)
{
    uint8_t *ptr = NULL;
    uint32_t tmo = user_tmo;
    //TODO: use ptr to HW transport iface struct
    uint8_t *(*apptrace_get_buffer)(size_t, uint32_t *);
    esp_err_t (*apptrace_put_buffer)(uint8_t *, uint32_t *);

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

    ptr = apptrace_get_buffer(size, &tmo);
    if (ptr == NULL) {
        //ESP_APPTRACE_LOGE("Failed to get buffer!");
        return ESP_ERR_NO_MEM;
    }

    // actually can be suspended here by higher prio tasks/ISRs
    //TODO: use own memcpy with dead trace calls kick-off algo, and tmo expiration check
    memcpy(ptr, data, size);

    // now indicate that this buffer is ready to be sent off to host
    return apptrace_put_buffer(ptr, &tmo);
}

int esp_apptrace_vprintf_to(esp_apptrace_dest_t dest, uint32_t user_tmo, const char *fmt, va_list ap)
{
    uint16_t nargs = 0;
    uint8_t *pout, *p = (uint8_t *)fmt;
    uint32_t tmo = user_tmo;
    //TODO: use ptr to HW transport iface struct
    uint8_t *(*apptrace_get_buffer)(size_t, uint32_t *);
    esp_err_t (*apptrace_put_buffer)(uint8_t *, uint32_t *);

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

    // ESP_APPTRACE_LOGI("fmt %x", fmt);
    while ((p = (uint8_t *)strchr((char *)p, '%')) && nargs < ESP_APPTRACE_MAX_VPRINTF_ARGS) {
        p++;
        if (*p != '%' && *p != 0) {
            nargs++;
        }
    }
    // ESP_APPTRACE_LOGI("nargs = %d", nargs);
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
        // ESP_APPTRACE_LOGI("arg %x", arg);
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

uint8_t *esp_apptrace_buffer_get(esp_apptrace_dest_t dest, size_t size, uint32_t user_tmo)
{
    uint32_t tmo = user_tmo;
    //TODO: use ptr to HW transport iface struct
    uint8_t *(*apptrace_get_buffer)(size_t, uint32_t *);

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

    return apptrace_get_buffer(size, &tmo);
}

esp_err_t esp_apptrace_buffer_put(esp_apptrace_dest_t dest, uint8_t *ptr, uint32_t user_tmo)
{
    uint32_t tmo = user_tmo;
    //TODO: use ptr to HW transport iface struct
    esp_err_t (*apptrace_put_buffer)(uint8_t *, uint32_t *);

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

    return apptrace_put_buffer(ptr, &tmo);
}

esp_err_t esp_apptrace_flush_nolock(esp_apptrace_dest_t dest, uint32_t min_sz, uint32_t tmo)
{
    //TODO: use ptr to HW transport iface struct
    esp_err_t (*apptrace_flush)(uint32_t, uint32_t);

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

    return apptrace_flush(min_sz, tmo);
}

esp_err_t esp_apptrace_flush(esp_apptrace_dest_t dest, uint32_t tmo)
{
    int res;

    res = esp_apptrace_lock(&tmo);
    if (res != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to lock apptrace data (%d)!", res);
        return res;
    }

    res = esp_apptrace_flush_nolock(dest, 0, tmo);
    if (res != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to fluch apptrace data (%d)!", res);
    }

    if (esp_apptrace_unlock() != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to unlock apptrace data (%d)!", res);
    }

    return res;
}

#if ESP_APPTRACE_DEBUG_STATS_ENABLE == 1
void esp_apptrace_print_stats()
{
    uint32_t i;
    uint32_t tmo = ESP_APPTRACE_TMO_INFINITE;

    esp_apptrace_lock(&tmo);

    for (i = s_trace_buf.state.stats.wr.hist_rd; (i < s_trace_buf.state.stats.wr.hist_wr) && (i < ESP_APPTRACE_BUF_HISTORY_DEPTH); i++) {
        esp_trace_buffer_wr_hitem_t *hi = (esp_trace_buffer_wr_hitem_t *)&s_trace_buf.state.stats.wr.hist[i];
        ESP_APPTRACE_LOGO("hist[%u] = {%x, %x}", i, hi->hnd, hi->ts);
    }
    if (i == ESP_APPTRACE_BUF_HISTORY_DEPTH) {
        for (i = 0; i < s_trace_buf.state.stats.wr.hist_wr; i++) {
            esp_trace_buffer_wr_hitem_t *hi = (esp_trace_buffer_wr_hitem_t *)&s_trace_buf.state.stats.wr.hist[i];
            ESP_APPTRACE_LOGO("hist[%u] = {%x, %x}", i, hi->hnd, hi->ts);
        }
    }

    esp_apptrace_unlock();
}
#endif
#endif
