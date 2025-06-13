/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "esp_heap_caps_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"
#include "esp_types.h"
#include "esp_mac.h"
#include "esp_random.h"
#include "esp_task.h"
#include "esp_attr.h"
#include "esp_phy_init.h"
#include "esp_bt.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_ipc.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc_memory_layout.h"
#include "private/esp_coexist_internal.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_rom_sys.h"
#include "esp_private/phy.h"
#if CONFIG_IDF_TARGET_ESP32C3
#include "riscv/interrupt.h"
#include "esp32c3/rom/rom_layout.h"
#else //CONFIG_IDF_TARGET_ESP32S3
#include "freertos/xtensa_api.h"
#include "xtensa/core-macros.h"
#include "esp32s3/rom/rom_layout.h"
#endif
#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
#include "ble_log/ble_log_spi_out.h"
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
#if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
#include "esp_partition.h"
#include "hal/wdt_hal.h"
#endif // CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
#if CONFIG_BT_ENABLED

/* Macro definition
 ************************************************************************
 */

#define BT_LOG_TAG                          "BLE_INIT"

#define BTDM_INIT_PERIOD                    (5000)    /* ms */

/* Low Power Clock Selection */
#define BTDM_LPCLK_SEL_XTAL      (0)
#define BTDM_LPCLK_SEL_XTAL32K   (1)
#define BTDM_LPCLK_SEL_RTC_SLOW  (2)
#define BTDM_LPCLK_SEL_8M        (3)

// wakeup request sources
enum {
    BTDM_ASYNC_WAKEUP_SRC_VHCI = 0,
    BTDM_ASYNC_WAKEUP_REQ_COEX,
    BTDM_ASYNC_WAKEUP_SRC_DISA,
    BTDM_ASYNC_WAKEUP_SRC_TMR,
    BTDM_ASYNC_WAKEUP_SRC_MAX,
};

// low power control struct
typedef union {
    struct {
        uint32_t enable                  :  1; // whether low power mode is required
        uint32_t lpclk_sel               :  3; // low power clock source
        uint32_t mac_bb_pd               :  1; // whether hardware(MAC, BB) force-power-down is required during sleep
        uint32_t wakeup_timer_required   :  1; // whether system timer is needed
        uint32_t no_light_sleep          :  1; // do not allow system to enter light sleep after bluetooth is enabled
        uint32_t main_xtal_pu            :  1; // power up main XTAL
        uint32_t reserved                : 24; // reserved
    };
    uint32_t val;
} btdm_lpcntl_t;

// low power control status
typedef union {
    struct {
        uint32_t pm_lock_released        :  1; // whether power management lock is released
        uint32_t mac_bb_pd               :  1; // whether hardware(MAC, BB) is powered down
        uint32_t phy_enabled             :  1; // whether phy is switched on
        uint32_t wakeup_timer_started    :  1; // whether wakeup timer is started
        uint32_t reserved                : 28; // reserved
    };
    uint32_t val;
} btdm_lpstat_t;

/* Sleep and wakeup interval control */
#define BTDM_MIN_SLEEP_DURATION          (24) // threshold of interval in half slots to allow to fall into modem sleep
#define BTDM_MODEM_WAKE_UP_DELAY         (8)  // delay in half slots of modem wake up procedure, including re-enable PHY/RF

#define BT_DEBUG(...)
#define BT_API_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        BT_DEBUG("%s %d %s ret=0x%X\n", __FUNCTION__, __LINE__, (info), __err);\
        return __err;\
    }\
} while(0)

#define OSI_FUNCS_TIME_BLOCKING  0xffffffff
#define OSI_VERSION              0x0001000A
#define OSI_MAGIC_VALUE          0xFADEBEAD

#define BLE_PWR_HDL_INVL 0xFFFF

#define BLE_CONTROLLER_MALLOC_CAPS        (MALLOC_CAP_INTERNAL|MALLOC_CAP_DMA)

#if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
#define MAX_STORAGE_SIZE          (CONFIG_BT_CTRL_LE_LOG_PARTITION_SIZE)
#define BLOCK_SIZE                (4096)
#define THRESHOLD                 (3072)
#define PARTITION_NAME            "bt_ctrl_log"
#endif

/* Types definition
 ************************************************************************
 */
/* vendor dependent signals to be posted to controller task */
typedef enum {
    BTDM_VND_OL_SIG_WAKEUP_TMR = 0,
    BTDM_VND_OL_SIG_NUM,
} btdm_vnd_ol_sig_t;

/* prototype of function to handle vendor dependent signals */
typedef void (* btdm_vnd_ol_task_func_t)(void *param);

/* VHCI function interface */
typedef struct vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} vhci_host_callback_t;

typedef struct {
    void *handle;
} btdm_queue_item_t;

typedef void (* osi_intr_handler)(void);

typedef struct {
    int source;               /*!< ISR source */
    int flags;                /*!< ISR alloc flag */
    void (*fn)(void *);       /*!< ISR function */
    void *arg;                /*!< ISR function args*/
    intr_handle_t *handle;    /*!< ISR handle */
    esp_err_t ret;
} btdm_isr_alloc_t;

/* OSI function */
struct osi_funcs_t {
    uint32_t _magic;
    uint32_t _version;
    int (* _interrupt_alloc)(int cpu_id, int source, intr_handler_t handler, void *arg, void **ret_handle);
    int (* _interrupt_free)(void *handle);
    void (*_interrupt_handler_set_rsv)(int interrupt_no, intr_handler_t fn, void *arg);
    void (*_global_intr_disable)(void);
    void (*_global_intr_restore)(void);
    void (*_task_yield)(void);
    void (*_task_yield_from_isr)(void);
    void *(*_semphr_create)(uint32_t max, uint32_t init);
    void (*_semphr_delete)(void *semphr);
    int (*_semphr_take_from_isr)(void *semphr, void *hptw);
    int (*_semphr_give_from_isr)(void *semphr, void *hptw);
    int (*_semphr_take)(void *semphr, uint32_t block_time_ms);
    int (*_semphr_give)(void *semphr);
    void *(*_mutex_create)(void);
    void (*_mutex_delete)(void *mutex);
    int (*_mutex_lock)(void *mutex);
    int (*_mutex_unlock)(void *mutex);
    void *(* _queue_create)(uint32_t queue_len, uint32_t item_size);
    void (* _queue_delete)(void *queue);
    int (* _queue_send)(void *queue, void *item, uint32_t block_time_ms);
    int (* _queue_send_from_isr)(void *queue, void *item, void *hptw);
    int (* _queue_recv)(void *queue, void *item, uint32_t block_time_ms);
    int (* _queue_recv_from_isr)(void *queue, void *item, void *hptw);
    int (* _task_create)(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
    void (* _task_delete)(void *task_handle);
    bool (* _is_in_isr)(void);
    int (* _cause_sw_intr_to_core)(int core_id, int intr_no);
    void *(* _malloc)(size_t size);
    void *(* _malloc_internal)(size_t size);
    void (* _free)(void *p);
    int (* _read_efuse_mac)(uint8_t mac[6]);
    void (* _srand)(unsigned int seed);
    int (* _rand)(void);
    uint32_t (* _btdm_lpcycles_2_hus)(uint32_t cycles, uint32_t *error_corr);
    uint32_t (* _btdm_hus_2_lpcycles)(uint32_t hus);
    bool (* _btdm_sleep_check_duration)(int32_t *slot_cnt);
    void (* _btdm_sleep_enter_phase1)(uint32_t lpcycles);  /* called when interrupt is disabled */
    void (* _btdm_sleep_enter_phase2)(void);
    void (* _btdm_sleep_exit_phase1)(void);  /* called from ISR */
    void (* _btdm_sleep_exit_phase2)(void);  /* called from ISR */
    void (* _btdm_sleep_exit_phase3)(void);  /* called from task */
    void (* _coex_wifi_sleep_set)(bool sleep);
    int (* _coex_core_ble_conn_dyn_prio_get)(bool *low, bool *high);
    int (* _coex_schm_register_btdm_callback)(void *callback);
    void (* _coex_schm_status_bit_set)(uint32_t type, uint32_t status);
    void (* _coex_schm_status_bit_clear)(uint32_t type, uint32_t status);
    uint32_t (* _coex_schm_interval_get)(void);
    uint8_t (* _coex_schm_curr_period_get)(void);
    void *(* _coex_schm_curr_phase_get)(void);
    int (* _interrupt_enable)(void *handle);
    int (* _interrupt_disable)(void *handle);
    void (* _esp_hw_power_down)(void);
    void (* _esp_hw_power_up)(void);
    void (* _ets_backup_dma_copy)(uint32_t reg, uint32_t mem_addr, uint32_t num, bool to_rem);
    void (* _ets_delay_us)(uint32_t us);
    void (* _btdm_rom_table_ready)(void);
    bool (* _coex_bt_wakeup_request)(void);
    void (* _coex_bt_wakeup_request_end)(void);
    int64_t (*_get_time_us)(void);
    void (* _assert)(void);
};

#if CONFIG_BT_CTRL_LE_LOG_EN
typedef void (*interface_func_t) (uint32_t len, const uint8_t*addr, bool end);
#endif // CONFIG_BT_CTRL_LE_LOG_EN

/* External functions or values
 ************************************************************************
 */

/* not for user call, so don't put to include file */
/* OSI */
extern int btdm_osi_funcs_register(void *osi_funcs);
/* Initialise and De-initialise */
extern int btdm_controller_init(esp_bt_controller_config_t *config_opts);
extern void btdm_controller_deinit(void);
extern int btdm_controller_enable(esp_bt_mode_t mode);
extern void btdm_controller_disable(void);
extern uint8_t btdm_controller_get_mode(void);
extern const char *btdm_controller_get_compile_version(void);
extern void btdm_rf_bb_init_phase2(void); // shall be called after PHY/RF is enabled
/* Sleep */
extern void btdm_controller_enable_sleep(bool enable);
extern uint8_t btdm_controller_get_sleep_mode(void);
extern bool btdm_power_state_active(void);
extern void btdm_wakeup_request(void);
extern void btdm_in_wakeup_requesting_set(bool in_wakeup_requesting);

/* vendor dependent tasks to be posted and handled by controller task*/
extern int btdm_vnd_offload_task_register(btdm_vnd_ol_sig_t sig, btdm_vnd_ol_task_func_t func);
extern int btdm_vnd_offload_task_deregister(btdm_vnd_ol_sig_t sig);
extern int r_btdm_vnd_offload_post_from_isr(btdm_vnd_ol_sig_t sig, void *param, bool need_yield);
extern int r_btdm_vnd_offload_post(btdm_vnd_ol_sig_t sig, void *param);

/* Low Power Clock */
extern bool btdm_lpclk_select_src(uint32_t sel);
extern bool btdm_lpclk_set_div(uint32_t div);
extern int btdm_hci_tl_io_event_post(int event);

/* VHCI */
extern bool API_vhci_host_check_send_available(void);
extern void API_vhci_host_send_packet(uint8_t *data, uint16_t len);
extern int API_vhci_host_register_callback(const vhci_host_callback_t *callback);
/* TX power */
extern int ble_txpwr_set(int power_type, uint16_t handle, int power_level);
extern int ble_txpwr_get(int power_type, uint16_t handle);

extern void coex_pti_v2(void);

extern bool btdm_deep_sleep_mem_init(void);
extern void btdm_deep_sleep_mem_deinit(void);
extern void btdm_ble_power_down_dma_copy(bool copy);
extern uint8_t btdm_sleep_clock_sync(void);
extern void sdk_config_extend_set_pll_track(bool enable);

#if CONFIG_MAC_BB_PD
extern void esp_mac_bb_power_down(void);
extern void esp_mac_bb_power_up(void);
extern void ets_backup_dma_copy(uint32_t reg, uint32_t mem_addr, uint32_t num, bool to_mem);
#endif

extern void btdm_cca_feature_enable(void);
extern void btdm_aa_check_enhance_enable(void);

/* BLE Log module */
#if CONFIG_BT_CTRL_LE_LOG_EN
extern int r_ble_log_init_async(interface_func_t bt_controller_log_interface, bool task_create, uint8_t buffers, uint32_t *bufs_size);
extern int r_ble_log_deinit_async(void);
extern void r_ble_log_async_select_dump_buffers(uint8_t buffers);
extern void r_ble_log_async_output_dump_all(bool output);
extern void esp_panic_handler_feed_wdts(void);
#endif // CONFIG_BT_CTRL_LE_LOG_EN
#if (CONFIG_BT_BLUEDROID_ENABLED || CONFIG_BT_NIMBLE_ENABLED)
extern void scan_stack_enableAdvFlowCtrlVsCmd(bool en);
extern void adv_stack_enableClearLegacyAdvVsCmd(bool en);
extern void advFilter_stack_enableDupExcListVsCmd(bool en);
extern void chanSel_stack_enableSetCsaVsCmd(bool en);
#endif // (CONFIG_BT_BLUEDROID_ENABLED || CONFIG_BT_NIMBLE_ENABLED)

extern void ble_dtm_funcs_reset(void);
extern void ble_scan_funcs_reset(void);
extern void ble_42_adv_funcs_reset(void);
extern void ble_init_funcs_reset(void);
extern void ble_con_funcs_reset(void);
extern void ble_cca_funcs_reset(void);
extern void ble_ext_adv_funcs_reset(void);
extern void ble_ext_scan_funcs_reset(void);
extern void ble_base_funcs_reset(void);
extern void ble_enc_funcs_reset(void);

extern uint32_t _bt_bss_start;
extern uint32_t _bt_bss_end;
extern uint32_t _bt_controller_bss_start;
extern uint32_t _bt_controller_bss_end;
extern uint32_t _bt_data_start;
extern uint32_t _bt_data_end;
extern uint32_t _bt_controller_data_start;
extern uint32_t _bt_controller_data_end;

/* Local Function Declare
 *********************************************************************
 */
static int interrupt_alloc_wrapper(int cpu_id, int source, intr_handler_t handler, void *arg, void **ret_handle);
static int interrupt_free_wrapper(void *handle);
static void global_interrupt_disable(void);
static void global_interrupt_restore(void);
static void task_yield_from_isr(void);
static void *semphr_create_wrapper(uint32_t max, uint32_t init);
static void semphr_delete_wrapper(void *semphr);
static int semphr_take_from_isr_wrapper(void *semphr, void *hptw);
static int semphr_give_from_isr_wrapper(void *semphr, void *hptw);
static int  semphr_take_wrapper(void *semphr, uint32_t block_time_ms);
static int  semphr_give_wrapper(void *semphr);
static void *mutex_create_wrapper(void);
static void mutex_delete_wrapper(void *mutex);
static int mutex_lock_wrapper(void *mutex);
static int mutex_unlock_wrapper(void *mutex);
static void *queue_create_wrapper(uint32_t queue_len, uint32_t item_size);
static void queue_delete_wrapper(void *queue);
static int queue_send_wrapper(void *queue, void *item, uint32_t block_time_ms);
static int queue_send_from_isr_wrapper(void *queue, void *item, void *hptw);
static int queue_recv_wrapper(void *queue, void *item, uint32_t block_time_ms);
static int queue_recv_from_isr_wrapper(void *queue, void *item, void *hptw);
static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
static void task_delete_wrapper(void *task_handle);
static bool is_in_isr_wrapper(void);
static void *malloc_internal_wrapper(size_t size);
static int read_mac_wrapper(uint8_t mac[6]);
static void srand_wrapper(unsigned int seed);
static int rand_wrapper(void);
static uint32_t btdm_lpcycles_2_hus(uint32_t cycles, uint32_t *error_corr);
static uint32_t btdm_hus_2_lpcycles(uint32_t hus);
static bool btdm_sleep_check_duration(int32_t *slot_cnt);
static void btdm_sleep_enter_phase1_wrapper(uint32_t lpcycles);
static void btdm_sleep_enter_phase2_wrapper(void);
static void btdm_sleep_exit_phase3_wrapper(void);
static void coex_wifi_sleep_set_hook(bool sleep);
static int coex_schm_register_btdm_callback_wrapper(void *callback);
static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status);
static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status);
static uint32_t coex_schm_interval_get_wrapper(void);
static uint8_t coex_schm_curr_period_get_wrapper(void);
static void * coex_schm_curr_phase_get_wrapper(void);
static int interrupt_enable_wrapper(void *handle);
static int interrupt_disable_wrapper(void *handle);
static void btdm_hw_mac_power_up_wrapper(void);
static void btdm_hw_mac_power_down_wrapper(void);
static void btdm_backup_dma_copy_wrapper(uint32_t reg, uint32_t mem_addr, uint32_t num,  bool to_mem);
static void btdm_funcs_table_ready_wrapper(void);
static bool coex_bt_wakeup_request(void);
static void coex_bt_wakeup_request_end(void);
static int64_t get_time_us_wrapper(void);
static void assert_wrapper(void);

static void btdm_slp_tmr_callback(void *arg);

static esp_err_t try_heap_caps_add_region(intptr_t start, intptr_t end);

static void bt_controller_deinit_internal(void);

#if CONFIG_BT_CTRL_LE_LOG_EN
static void esp_bt_controller_log_interface(uint32_t len, const uint8_t *addr, bool end);
#if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
void esp_bt_read_ctrl_log_from_flash(bool output);
static int esp_bt_controller_log_storage(uint32_t len, const uint8_t *addr, bool end);
static void esp_bt_ctrl_log_partition_get_and_erase_first_block(void);
#endif // #if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
#endif // CONFIG_BT_CTRL_LE_LOG_EN

/* Local variable definition
 ***************************************************************************
 */
/* OSI funcs */
static const struct osi_funcs_t osi_funcs_ro = {
    ._magic = OSI_MAGIC_VALUE,
    ._version = OSI_VERSION,
    ._interrupt_alloc = interrupt_alloc_wrapper,
    ._interrupt_free = interrupt_free_wrapper,
    ._interrupt_handler_set_rsv = NULL,
    ._global_intr_disable = global_interrupt_disable,
    ._global_intr_restore = global_interrupt_restore,
    ._task_yield = vPortYield,
    ._task_yield_from_isr = task_yield_from_isr,
    ._semphr_create = semphr_create_wrapper,
    ._semphr_delete = semphr_delete_wrapper,
    ._semphr_take_from_isr = semphr_take_from_isr_wrapper,
    ._semphr_give_from_isr = semphr_give_from_isr_wrapper,
    ._semphr_take = semphr_take_wrapper,
    ._semphr_give = semphr_give_wrapper,
    ._mutex_create = mutex_create_wrapper,
    ._mutex_delete = mutex_delete_wrapper,
    ._mutex_lock = mutex_lock_wrapper,
    ._mutex_unlock = mutex_unlock_wrapper,
    ._queue_create = queue_create_wrapper,
    ._queue_delete = queue_delete_wrapper,
    ._queue_send = queue_send_wrapper,
    ._queue_send_from_isr = queue_send_from_isr_wrapper,
    ._queue_recv = queue_recv_wrapper,
    ._queue_recv_from_isr = queue_recv_from_isr_wrapper,
    ._task_create = task_create_wrapper,
    ._task_delete = task_delete_wrapper,
    ._is_in_isr = is_in_isr_wrapper,
    ._cause_sw_intr_to_core = NULL,
    ._malloc = malloc,
    ._malloc_internal = malloc_internal_wrapper,
    ._free = free,
    ._read_efuse_mac = read_mac_wrapper,
    ._srand = srand_wrapper,
    ._rand = rand_wrapper,
    ._btdm_lpcycles_2_hus = btdm_lpcycles_2_hus,
    ._btdm_hus_2_lpcycles = btdm_hus_2_lpcycles,
    ._btdm_sleep_check_duration = btdm_sleep_check_duration,
    ._btdm_sleep_enter_phase1 = btdm_sleep_enter_phase1_wrapper,
    ._btdm_sleep_enter_phase2 = btdm_sleep_enter_phase2_wrapper,
    ._btdm_sleep_exit_phase1 = NULL,
    ._btdm_sleep_exit_phase2 = NULL,
    ._btdm_sleep_exit_phase3 = btdm_sleep_exit_phase3_wrapper,
    ._coex_wifi_sleep_set = coex_wifi_sleep_set_hook,
    ._coex_core_ble_conn_dyn_prio_get = NULL,
    ._coex_schm_register_btdm_callback = coex_schm_register_btdm_callback_wrapper,
    ._coex_schm_status_bit_set = coex_schm_status_bit_set_wrapper,
    ._coex_schm_status_bit_clear = coex_schm_status_bit_clear_wrapper,
    ._coex_schm_interval_get = coex_schm_interval_get_wrapper,
    ._coex_schm_curr_period_get = coex_schm_curr_period_get_wrapper,
    ._coex_schm_curr_phase_get = coex_schm_curr_phase_get_wrapper,
    ._interrupt_enable = interrupt_enable_wrapper,
    ._interrupt_disable = interrupt_disable_wrapper,
    ._esp_hw_power_down = btdm_hw_mac_power_down_wrapper,
    ._esp_hw_power_up = btdm_hw_mac_power_up_wrapper,
    ._ets_backup_dma_copy = btdm_backup_dma_copy_wrapper,
    ._ets_delay_us = esp_rom_delay_us,
    ._btdm_rom_table_ready = btdm_funcs_table_ready_wrapper,
    ._coex_bt_wakeup_request = coex_bt_wakeup_request,
    ._coex_bt_wakeup_request_end = coex_bt_wakeup_request_end,
    ._get_time_us = get_time_us_wrapper,
    ._assert = assert_wrapper,
};

static DRAM_ATTR struct osi_funcs_t *osi_funcs_p;

/* Static variable declare */
static DRAM_ATTR esp_bt_controller_status_t btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

static DRAM_ATTR portMUX_TYPE global_int_mux = portMUX_INITIALIZER_UNLOCKED;

// low power control struct
static DRAM_ATTR btdm_lpcntl_t s_lp_cntl;
// low power status struct
static DRAM_ATTR btdm_lpstat_t s_lp_stat;
// measured average low power clock period in micro seconds
static DRAM_ATTR uint32_t btdm_lpcycle_us = 0;
// number of fractional bit for btdm_lpcycle_us
static DRAM_ATTR uint8_t btdm_lpcycle_us_frac = 0;
// semaphore used for blocking VHCI API to wait for controller to wake up
static DRAM_ATTR QueueHandle_t s_wakeup_req_sem = NULL;
// wakeup timer
static DRAM_ATTR esp_timer_handle_t s_btdm_slp_tmr = NULL;

#ifdef CONFIG_PM_ENABLE
static DRAM_ATTR esp_pm_lock_handle_t s_pm_lock;
// pm_lock to prevent light sleep due to incompatibility currently
static DRAM_ATTR esp_pm_lock_handle_t s_light_sleep_pm_lock;
#endif

#if CONFIG_BT_CTRL_LE_LOG_EN
enum log_out_mode {
    LOG_DUMP_MEMORY,
    LOG_ASYNC_OUT,
    LOG_STORAGE_TO_FLASH,
    LOG_SPI_OUT,
};

const static uint32_t log_bufs_size[] = {CONFIG_BT_CTRL_LE_LOG_BUF1_SIZE, CONFIG_BT_CTRL_LE_LOG_HCI_BUF_SIZE, CONFIG_BT_CTRL_LE_LOG_BUF2_SIZE};
bool log_is_inited = false;
#if CONFIG_BT_CTRL_LE_LOG_DUMP_ONLY
uint8_t log_output_mode = LOG_DUMP_MEMORY;
#else
#if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
uint8_t log_output_mode = LOG_STORAGE_TO_FLASH;
#elif CONFIG_BT_CTRL_LE_LOG_SPI_OUT_EN
uint8_t log_output_mode = LOG_SPI_OUT;
#else
uint8_t log_output_mode = LOG_ASYNC_OUT;
#endif // CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
#endif // CONFIG_BT_CTRL_LE_LOG_DUMP_ONLY
#if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
static const esp_partition_t *log_partition;
static uint32_t write_index = 0;
static uint32_t next_erase_index = BLOCK_SIZE;
static bool block_erased = false;
static bool stop_write = false;
static bool is_filled = false;
#endif // CONFIG_BT_CTRL_LE_LOG_STORAGE_EN

static void esp_bt_controller_log_interface(uint32_t len, const uint8_t *addr, bool end)
{
    if (log_output_mode == LOG_STORAGE_TO_FLASH) {
#if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
        esp_bt_controller_log_storage(len, addr, end);
#endif //CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
    } else {
        portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
        portENTER_CRITICAL_SAFE(&spinlock);
        esp_panic_handler_feed_wdts();
        for (int i = 0; i < len; i++) {
            esp_rom_printf("%02x ", addr[i]);
        }

        if (end) {
            esp_rom_printf("\n");
        }
        portEXIT_CRITICAL_SAFE(&spinlock);
    }
}

#if CONFIG_BT_CTRL_LE_LOG_SPI_OUT_EN
static IRAM_ATTR void esp_bt_controller_spi_log_interface(uint32_t len, const uint8_t *addr, bool end)
{
    ble_log_spi_out_write(BLE_LOG_SPI_OUT_SOURCE_ESP_LEGACY, addr, len);
}
#endif // CONFIG_BT_CTRL_LE_LOG_SPI_OUT_EN

void esp_ble_controller_log_dump_all(bool output)
{
    if (log_output_mode == LOG_STORAGE_TO_FLASH) {
#if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
        esp_bt_read_ctrl_log_from_flash(output);
#endif // CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
    } else {
        portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
        portENTER_CRITICAL_SAFE(&spinlock);
        esp_panic_handler_feed_wdts();
        esp_rom_printf("\r\n[DUMP_START:");
        r_ble_log_async_output_dump_all(output);
        esp_rom_printf(":DUMP_END]\r\n");
        portEXIT_CRITICAL_SAFE(&spinlock);
    }
}

void esp_bt_log_output_mode_set(uint8_t output_mode)
{
    log_output_mode = output_mode;
}

uint8_t esp_bt_log_output_mode_get(void)
{
    return log_output_mode;
}

esp_err_t esp_bt_controller_log_init(uint8_t log_output_mode)
{
    esp_err_t ret = ESP_OK;
    interface_func_t bt_controller_log_interface;
    bt_controller_log_interface = esp_bt_controller_log_interface;
    bool task_create;
    uint8_t buffers = 0;

    if (log_is_inited) {
        return ret;
    }

#if CONFIG_BT_CTRL_LE_LOG_EN
    buffers |= ESP_BLE_LOG_BUF_CONTROLLER;
#endif // CONFIG_BT_CTRL_LE_LOG_EN
#if CONFIG_BT_CTRL_LE_HCI_LOG_EN
    buffers |= ESP_BLE_LOG_BUF_HCI;
#endif // CONFIG_BT_CTRL_LE_HCI_LOG_EN

    switch (log_output_mode) {
        case LOG_DUMP_MEMORY:
            task_create = false;
            break;
        case LOG_ASYNC_OUT:
        case LOG_STORAGE_TO_FLASH:
            task_create = true;
#if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
            if (log_output_mode == LOG_STORAGE_TO_FLASH) {
                esp_bt_ctrl_log_partition_get_and_erase_first_block();
            }
#endif // CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
            break;
        case LOG_SPI_OUT:
            task_create = true;
#if CONFIG_BT_CTRL_LE_LOG_SPI_OUT_EN
            bt_controller_log_interface = esp_bt_controller_spi_log_interface;
#endif // CONFIG_BT_CTRL_LE_LOG_SPI_OUT_EN
            break;
        default:
            assert(0);
    }

    ret = r_ble_log_init_async(bt_controller_log_interface, task_create, buffers, (uint32_t *)log_bufs_size);
    if (ret == ESP_OK) {
        log_is_inited = true;
    }

    return ret;
}

void esp_bt_ontroller_log_deinit(void)
{
    r_ble_log_deinit_async();
    log_is_inited = false;
}

#if CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
static void esp_bt_ctrl_log_partition_get_and_erase_first_block(void)
{
    log_partition = NULL;
    assert(MAX_STORAGE_SIZE % BLOCK_SIZE == 0);
    // Find the partition map in the partition table
    log_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, PARTITION_NAME);
    assert(log_partition != NULL);
    // Prepare data to be read later using the mapped address
    ESP_ERROR_CHECK(esp_partition_erase_range(log_partition, 0, BLOCK_SIZE));
    write_index = 0;
    next_erase_index = BLOCK_SIZE;
    block_erased = false;
    is_filled = false;
    stop_write = false;
}

static int esp_bt_controller_log_storage(uint32_t len, const uint8_t *addr, bool end)
{
    if (len > MAX_STORAGE_SIZE) {
        return -1;
    }

    if (stop_write) {
        return 0;
    }

    if (((write_index) % BLOCK_SIZE) >= THRESHOLD && !block_erased) {
        // esp_rom_printf("Ers nxt: %d,%d\n", next_erase_index, write_index);
        esp_partition_erase_range(log_partition, next_erase_index, BLOCK_SIZE);
        next_erase_index = (next_erase_index + BLOCK_SIZE) % MAX_STORAGE_SIZE;
        block_erased = true;
    }

    if (((write_index + len) / BLOCK_SIZE) >  (write_index / BLOCK_SIZE)) {
        block_erased = false;
    }

    if (write_index + len <= MAX_STORAGE_SIZE) {
        esp_partition_write(log_partition, write_index, addr, len);
        write_index = (write_index + len) % MAX_STORAGE_SIZE;
    } else {
        uint32_t first_part_len = MAX_STORAGE_SIZE - write_index;
        esp_partition_write(log_partition, write_index, addr, first_part_len);
        esp_partition_write(log_partition, 0, addr + first_part_len, len - first_part_len);
        write_index = len - first_part_len;
        is_filled = true;
        // esp_rom_printf("old idx: %d,%d\n",next_erase_index, write_index);
    }

    return 0;
}

void esp_bt_read_ctrl_log_from_flash(bool output)
{
    esp_partition_mmap_handle_t mmap_handle;
    uint32_t read_index;
    const void *mapped_ptr;
    const uint8_t *buffer;
    uint32_t print_len;
    uint32_t max_print_len;
    esp_err_t err;

    print_len = 0;
    max_print_len = 4096;
    err = esp_partition_mmap(log_partition, 0, MAX_STORAGE_SIZE, ESP_PARTITION_MMAP_DATA, &mapped_ptr, &mmap_handle);
    if (err != ESP_OK) {
        ESP_LOGE("FLASH", "Mmap failed: %s", esp_err_to_name(err));
        return;
    }

    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);
    esp_panic_handler_feed_wdts();
    r_ble_log_async_output_dump_all(true);
    esp_bt_ontroller_log_deinit();
    stop_write = true;

    buffer = (const uint8_t *)mapped_ptr;
    esp_panic_handler_feed_wdts();
    if (is_filled) {
        read_index = next_erase_index;
    } else {
        read_index = 0;
    }

    esp_rom_printf("\r\nREAD_CHECK:%ld,%ld,%d\r\n",read_index, write_index, is_filled);
    esp_rom_printf("\r\n[DUMP_START:");
    while (read_index != write_index) {
        esp_rom_printf("%02x ", buffer[read_index]);
        if (print_len > max_print_len) {
            esp_panic_handler_feed_wdts();
            print_len = 0;
        }

        print_len++;
        read_index = (read_index + 1) % MAX_STORAGE_SIZE;
    }

    esp_rom_printf(":DUMP_END]\r\n");
    portEXIT_CRITICAL_SAFE(&spinlock);
    esp_partition_munmap(mmap_handle);
    err = esp_bt_controller_log_init(log_output_mode);
    assert(err == ESP_OK);
}
#endif // CONFIG_BT_CTRL_LE_LOG_STORAGE_EN
#endif // CONFIG_BT_CTRL_LE_LOG_EN

void IRAM_ATTR btdm_hw_mac_power_down_wrapper(void)
{
#if CONFIG_MAC_BB_PD
#if SOC_PM_SUPPORT_BT_PD
    // Bluetooth module power down
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_BT_FORCE_ISO);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_FORCE_PD);
#endif
    esp_mac_bb_power_down();
#endif
}

void IRAM_ATTR btdm_hw_mac_power_up_wrapper(void)
{
#if CONFIG_MAC_BB_PD
#if SOC_PM_SUPPORT_BT_PD
    // Bluetooth module power up
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_FORCE_PD);
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_BT_FORCE_ISO);
#endif
    esp_mac_bb_power_up();
#endif
}

void IRAM_ATTR btdm_backup_dma_copy_wrapper(uint32_t reg, uint32_t mem_addr, uint32_t num,  bool to_mem)
{
#if CONFIG_MAC_BB_PD
    ets_backup_dma_copy(reg, mem_addr, num, to_mem);
#endif
}

static inline void esp_bt_power_domain_on(void)
{
    // Bluetooth module power up
#if SOC_PM_SUPPORT_BT_PD
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_FORCE_PD);
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_BT_FORCE_ISO);
#endif
    esp_wifi_bt_power_domain_on();
}

static inline void esp_bt_power_domain_off(void)
{
    // Bluetooth module power down
#if SOC_PM_SUPPORT_BT_PD
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_BT_FORCE_ISO);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_FORCE_PD);
#endif
    esp_wifi_bt_power_domain_off();
}

static void btdm_intr_alloc(void *arg)
{
    btdm_isr_alloc_t *p = arg;
    p->ret = esp_intr_alloc(p->source, p->flags, p->fn, p->arg, p->handle);
}

static int interrupt_alloc_wrapper(int cpu_id, int source, intr_handler_t handler, void *arg, void **ret_handle)
{
    btdm_isr_alloc_t p;
    p.source = source;
#if CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY
    p.flags = ESP_INTR_FLAG_LEVEL3;
#else
    p.flags = ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM;
#endif
    p.fn = handler;
    p.arg = arg;
    p.handle = (intr_handle_t *)ret_handle;
#if CONFIG_FREERTOS_UNICORE
    btdm_intr_alloc(&p);
#else
    esp_ipc_call_blocking(cpu_id, btdm_intr_alloc, &p);
#endif
    return p.ret;
}

static int interrupt_free_wrapper(void *handle)
{
    return esp_intr_free((intr_handle_t)handle);
}

static int interrupt_enable_wrapper(void *handle)
{
    return esp_intr_enable((intr_handle_t)handle);
}

static int interrupt_disable_wrapper(void *handle)
{
    return esp_intr_disable((intr_handle_t)handle);
}

static void IRAM_ATTR global_interrupt_disable(void)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(&global_int_mux);
    } else {
        portENTER_CRITICAL(&global_int_mux);
    }
}

static void IRAM_ATTR global_interrupt_restore(void)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(&global_int_mux);
    } else {
        portEXIT_CRITICAL(&global_int_mux);
    }
}

static void IRAM_ATTR task_yield_from_isr(void)
{
    portYIELD_FROM_ISR();
}

static void *semphr_create_wrapper(uint32_t max, uint32_t init)
{
    btdm_queue_item_t *semphr = heap_caps_calloc(1, sizeof(btdm_queue_item_t), MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    assert(semphr);

    /* IDF FreeRTOS guarantees that all dynamic memory allocation goes to internal RAM. */
    semphr->handle = (void *)xSemaphoreCreateCounting(max, init);
    assert(semphr->handle);

    return semphr;
}

static void semphr_delete_wrapper(void *semphr)
{
    if (semphr == NULL) {
        return;
    }

    btdm_queue_item_t *semphr_item = (btdm_queue_item_t *)semphr;

    if (semphr_item->handle) {
        vSemaphoreDelete(semphr_item->handle);
    }

    free(semphr);
}

static int IRAM_ATTR semphr_take_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int)xSemaphoreTakeFromISR(((btdm_queue_item_t *)semphr)->handle, hptw);
}

static int IRAM_ATTR semphr_give_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int)xSemaphoreGiveFromISR(((btdm_queue_item_t *)semphr)->handle, hptw);
}

static int semphr_take_wrapper(void *semphr, uint32_t block_time_ms)
{
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        return (int)xSemaphoreTake(((btdm_queue_item_t *)semphr)->handle, portMAX_DELAY);
    } else {
        return (int)xSemaphoreTake(((btdm_queue_item_t *)semphr)->handle, block_time_ms / portTICK_PERIOD_MS);
    }
}

static int semphr_give_wrapper(void *semphr)
{
    return (int)xSemaphoreGive(((btdm_queue_item_t *)semphr)->handle);
}

static void *mutex_create_wrapper(void)
{
    return (void *)xSemaphoreCreateMutex();
}

static void mutex_delete_wrapper(void *mutex)
{
    vSemaphoreDelete(mutex);
}

static int mutex_lock_wrapper(void *mutex)
{
    return (int)xSemaphoreTake(mutex, portMAX_DELAY);
}

static int mutex_unlock_wrapper(void *mutex)
{
    return (int)xSemaphoreGive(mutex);
}

static void *queue_create_wrapper(uint32_t queue_len, uint32_t item_size)
{
    btdm_queue_item_t *queue = NULL;

    queue = (btdm_queue_item_t*)heap_caps_malloc(sizeof(btdm_queue_item_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    assert(queue);

    /* IDF FreeRTOS guarantees that all dynamic memory allocation goes to internal RAM. */
    queue->handle = xQueueCreate( queue_len, item_size);
    assert(queue->handle);

    return queue;
}

static void queue_delete_wrapper(void *queue)
{
    btdm_queue_item_t *queue_item = (btdm_queue_item_t *)queue;
    if (queue_item) {
        if(queue_item->handle){
            vQueueDelete(queue_item->handle);
        }
        free(queue_item);
    }
}

static int queue_send_wrapper(void *queue, void *item, uint32_t block_time_ms)
{
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        return (int)xQueueSend(((btdm_queue_item_t*)queue)->handle, item, portMAX_DELAY);
    } else {
        return (int)xQueueSend(((btdm_queue_item_t*)queue)->handle, item, block_time_ms / portTICK_PERIOD_MS);
    }
}

static int IRAM_ATTR queue_send_from_isr_wrapper(void *queue, void *item, void *hptw)
{
    return (int)xQueueSendFromISR(((btdm_queue_item_t*)queue)->handle, item, hptw);
}

static int queue_recv_wrapper(void *queue, void *item, uint32_t block_time_ms)
{
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        return (int)xQueueReceive(((btdm_queue_item_t*)queue)->handle, item, portMAX_DELAY);
    } else {
        return (int)xQueueReceive(((btdm_queue_item_t*)queue)->handle, item, block_time_ms / portTICK_PERIOD_MS);
    }
}

static int IRAM_ATTR queue_recv_from_isr_wrapper(void *queue, void *item, void *hptw)
{
    return (int)xQueueReceiveFromISR(((btdm_queue_item_t*)queue)->handle, item, hptw);
}

static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id)
{
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle, (core_id < portNUM_PROCESSORS ? core_id : tskNO_AFFINITY));
}

static void task_delete_wrapper(void *task_handle)
{
    vTaskDelete(task_handle);
}

static bool IRAM_ATTR is_in_isr_wrapper(void)
{
    return (bool)xPortInIsrContext();
}

static void *malloc_internal_wrapper(size_t size)
{
    void *p = heap_caps_malloc(size, BLE_CONTROLLER_MALLOC_CAPS);
    if(p == NULL) {
        ESP_LOGE(BT_LOG_TAG, "Malloc failed");
    }
    return p;
}

void *malloc_ble_controller_mem(size_t size)
{
    void *p = heap_caps_malloc(size, BLE_CONTROLLER_MALLOC_CAPS);
    if(p == NULL) {
        ESP_LOGE(BT_LOG_TAG, "Malloc failed");
    }
    return p;
}

uint32_t get_ble_controller_free_heap_size(void)
{
    return heap_caps_get_free_size(BLE_CONTROLLER_MALLOC_CAPS);
}

static int IRAM_ATTR read_mac_wrapper(uint8_t mac[6])
{
    int ret = esp_read_mac(mac, ESP_MAC_BT);
    ESP_LOGI(BT_LOG_TAG, "Bluetooth MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return ret;
}

static void IRAM_ATTR srand_wrapper(unsigned int seed)
{
    /* empty function */
}

static int IRAM_ATTR rand_wrapper(void)
{
    return (int)esp_random();
}

static uint32_t IRAM_ATTR btdm_lpcycles_2_hus(uint32_t cycles, uint32_t *error_corr)
{
    uint64_t local_error_corr = (error_corr == NULL) ? 0 : (uint64_t)(*error_corr);
    uint64_t res = (uint64_t)btdm_lpcycle_us * cycles * 2;
    local_error_corr += res;
    res = (local_error_corr >> btdm_lpcycle_us_frac);
    local_error_corr -= (res << btdm_lpcycle_us_frac);
    if (error_corr) {
        *error_corr = (uint32_t) local_error_corr;
    }
    return (uint32_t)res;
}

/*
 * @brief Converts a duration in half us into a number of low power clock cycles.
 */
static uint32_t IRAM_ATTR btdm_hus_2_lpcycles(uint32_t hus)
{
    // The number of sleep duration(us) should not lead to overflow. Thrs: 100s
    // Compute the sleep duration in us to low power clock cycles, with calibration result applied
    // clock measurement is conducted
    uint64_t cycles = ((uint64_t)(hus) << btdm_lpcycle_us_frac) / btdm_lpcycle_us;
    cycles >>= 1;

    return (uint32_t)cycles;
}

static bool IRAM_ATTR btdm_sleep_check_duration(int32_t *half_slot_cnt)
{
    if (*half_slot_cnt < BTDM_MIN_SLEEP_DURATION) {
        return false;
    }
    /* wake up in advance considering the delay in enabling PHY/RF */
    *half_slot_cnt -= BTDM_MODEM_WAKE_UP_DELAY;
    return true;
}

static void btdm_sleep_enter_phase1_wrapper(uint32_t lpcycles)
{
    if (s_lp_cntl.wakeup_timer_required == 0) {
        return;
    }

    uint32_t us_to_sleep = btdm_lpcycles_2_hus(lpcycles, NULL) >> 1;

#define BTDM_MIN_TIMER_UNCERTAINTY_US      (1800)
#define BTDM_RTC_SLOW_CLK_RC_DRIFT_PERCENT 7
    assert(us_to_sleep > BTDM_MIN_TIMER_UNCERTAINTY_US);
    // allow a maximum time uncertainty to be about 488ppm(1/2048) at least as clock drift
    // and set the timer in advance
    uint32_t uncertainty = (us_to_sleep >> 11);
#if CONFIG_BT_CTRL_MAIN_XTAL_PU_DURING_LIGHT_SLEEP
    // recalculate clock drift when Bluetooth using main XTAL during light sleep
    if (rtc_clk_slow_src_get() == SOC_RTC_SLOW_CLK_SRC_RC_SLOW) {
        uncertainty = us_to_sleep * BTDM_RTC_SLOW_CLK_RC_DRIFT_PERCENT / 100;
    }
#endif

    if (uncertainty < BTDM_MIN_TIMER_UNCERTAINTY_US) {
        uncertainty = BTDM_MIN_TIMER_UNCERTAINTY_US;
    }

    assert (s_lp_stat.wakeup_timer_started == 0);
    // start a timer to wake up and acquire the pm_lock before modem_sleep awakes
    if (esp_timer_start_once(s_btdm_slp_tmr, us_to_sleep - uncertainty) == ESP_OK) {
        s_lp_stat.wakeup_timer_started = 1;
    } else {
        ESP_LOGE(BT_LOG_TAG, "timer start failed");
        assert(0);
    }
}

static void btdm_sleep_enter_phase2_wrapper(void)
{
    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
        if (s_lp_stat.phy_enabled) {
            esp_phy_disable(PHY_MODEM_BT);
            s_lp_stat.phy_enabled = 0;
        } else {
            assert(0);
        }

#ifdef CONFIG_PM_ENABLE
        if (s_lp_stat.pm_lock_released == 0) {
            esp_pm_lock_release(s_pm_lock);
            s_lp_stat.pm_lock_released = 1;
        }
#endif
    }
}

static void btdm_sleep_exit_phase3_wrapper(void)
{
#ifdef CONFIG_PM_ENABLE
    // If BT wakeup before esp timer coming due to timer task have no chance to run.
    // Then we will not run into `btdm_sleep_exit_phase0` and acquire PM lock,
    // Do it again here to fix this issue.
    if (s_lp_stat.pm_lock_released) {
        esp_pm_lock_acquire(s_pm_lock);
        s_lp_stat.pm_lock_released = 0;
    }
#endif

    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
        if (s_lp_stat.phy_enabled == 0) {
            esp_phy_enable(PHY_MODEM_BT);
            s_lp_stat.phy_enabled = 1;
        }
    }

    // If BT wakeup before esp timer coming due to timer task have no chance to run.
    // Then we will not run into `btdm_sleep_exit_phase0` and stop esp timer,
    // Do it again here to fix this issue.
    if (s_lp_cntl.wakeup_timer_required && s_lp_stat.wakeup_timer_started) {
        esp_timer_stop(s_btdm_slp_tmr);
        s_lp_stat.wakeup_timer_started = 0;
    }

    // wait for the sleep state to change
    // the procedure duration is at micro-second level or less
    while (btdm_sleep_clock_sync()) {
        ;
    }
}

static void IRAM_ATTR btdm_sleep_exit_phase0(void *param)
{
    assert(s_lp_cntl.enable == 1);

#ifdef CONFIG_PM_ENABLE
    if (s_lp_stat.pm_lock_released) {
        esp_pm_lock_acquire(s_pm_lock);
        s_lp_stat.pm_lock_released = 0;
    }
#endif

    int event = (int) param;
    if (event == BTDM_ASYNC_WAKEUP_SRC_VHCI || event == BTDM_ASYNC_WAKEUP_SRC_DISA) {
        btdm_wakeup_request();
    }

    if (s_lp_cntl.wakeup_timer_required && s_lp_stat.wakeup_timer_started) {
        esp_timer_stop(s_btdm_slp_tmr);
        s_lp_stat.wakeup_timer_started = 0;
    }

    if (event == BTDM_ASYNC_WAKEUP_SRC_VHCI || event == BTDM_ASYNC_WAKEUP_SRC_DISA) {
        semphr_give_wrapper(s_wakeup_req_sem);
    }
}

static void IRAM_ATTR btdm_slp_tmr_callback(void *arg)
{
#ifdef CONFIG_PM_ENABLE
    r_btdm_vnd_offload_post(BTDM_VND_OL_SIG_WAKEUP_TMR, (void *)BTDM_ASYNC_WAKEUP_SRC_TMR);
#endif
}


static bool async_wakeup_request(int event)
{
    if (s_lp_cntl.enable == 0) {
        return false;
    }

    bool do_wakeup_request = false;
    switch (event) {
        case BTDM_ASYNC_WAKEUP_SRC_VHCI:
        case BTDM_ASYNC_WAKEUP_SRC_DISA:
            btdm_in_wakeup_requesting_set(true);
            if (!btdm_power_state_active()) {
                r_btdm_vnd_offload_post(BTDM_VND_OL_SIG_WAKEUP_TMR, (void *)event);
                do_wakeup_request = true;
                semphr_take_wrapper(s_wakeup_req_sem, OSI_FUNCS_TIME_BLOCKING);
            }
            break;
        case BTDM_ASYNC_WAKEUP_REQ_COEX:
            if (!btdm_power_state_active()) {
                do_wakeup_request = true;
#if CONFIG_PM_ENABLE
                if (s_lp_stat.pm_lock_released) {
                    esp_pm_lock_acquire(s_pm_lock);
                    s_lp_stat.pm_lock_released = 0;
                }
#endif
                btdm_wakeup_request();

                if (s_lp_cntl.wakeup_timer_required && s_lp_stat.wakeup_timer_started) {
                    esp_timer_stop(s_btdm_slp_tmr);
                    s_lp_stat.wakeup_timer_started = 0;
                }
            }
        default:
            break;
    }

    return do_wakeup_request;
}

static void async_wakeup_request_end(int event)
{
    if (s_lp_cntl.enable == 0) {
        return;
    }

    bool allow_to_sleep;
    switch (event) {
        case BTDM_ASYNC_WAKEUP_SRC_VHCI:
        case BTDM_ASYNC_WAKEUP_SRC_DISA:
            allow_to_sleep = true;
            break;
        case BTDM_ASYNC_WAKEUP_REQ_COEX:
            allow_to_sleep = false;
            break;
        default:
            allow_to_sleep = true;
            break;
    }

    if (allow_to_sleep) {
        btdm_in_wakeup_requesting_set(false);
    }

    return;
}

static void btdm_funcs_table_ready_wrapper(void)
{
#if BT_BLE_CCA_MODE == 2
    btdm_cca_feature_enable();
#endif
#if BLE_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS_ENABLED
    btdm_aa_check_enhance_enable();
#endif
#if CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY
    // do nothing
#else
    ESP_LOGI(BT_LOG_TAG, "Feature Config, ADV:%d, BLE_50:%d, DTM:%d, SCAN:%d, CCA:%d, SMP:%d, CONNECT:%d",
                        BT_CTRL_BLE_ADV, BT_CTRL_50_FEATURE_SUPPORT, BT_CTRL_DTM_ENABLE, BT_CTRL_BLE_SCAN,
                        BT_BLE_CCA_MODE, BLE_SECURITY_ENABLE, BT_CTRL_BLE_MASTER);

    ble_base_funcs_reset();
#if CONFIG_BT_CTRL_BLE_ADV
    ble_42_adv_funcs_reset();
#if (BT_CTRL_50_FEATURE_SUPPORT == 1)
    ble_ext_adv_funcs_reset();
#endif //
#endif // CONFIG_BT_CTRL_BLE_ADV

#if CONFIG_BT_CTRL_DTM_ENABLE
    ble_dtm_funcs_reset();
#endif // CONFIG_BT_CTRL_DTM_ENABLE

#if CONFIG_BT_CTRL_BLE_SCAN
    ble_scan_funcs_reset();
#if (BT_CTRL_50_FEATURE_SUPPORT == 1)
    ble_ext_scan_funcs_reset();
#endif // (BT_CTRL_50_FEATURE_SUPPORT == 1)
#endif // CONFIG_BT_CTRL_BLE_SCAN

#if (BT_BLE_CCA_MODE != 0)
    ble_cca_funcs_reset();
#endif // (BT_BLE_CCA_MODE != 0)

#if CONFIG_BT_CTRL_BLE_SECURITY_ENABLE
    ble_enc_funcs_reset();
#endif // CONFIG_BT_CTRL_BLE_SECURITY_ENABLE

#if CONFIG_BT_CTRL_BLE_MASTER
    ble_init_funcs_reset();
    ble_con_funcs_reset();
#endif // CONFIG_BT_CTRL_BLE_MASTER

#endif // CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY
}

bool bt_async_wakeup_request(void)
{
    return async_wakeup_request(BTDM_ASYNC_WAKEUP_SRC_VHCI);
}

void bt_wakeup_request_end(void)
{
    async_wakeup_request_end(BTDM_ASYNC_WAKEUP_SRC_VHCI);
}

static bool coex_bt_wakeup_request(void)
{
    return async_wakeup_request(BTDM_ASYNC_WAKEUP_REQ_COEX);
}

static void coex_bt_wakeup_request_end(void)
{
    async_wakeup_request_end(BTDM_ASYNC_WAKEUP_REQ_COEX);
    return;
}

static IRAM_ATTR int64_t get_time_us_wrapper(void)
{
    return esp_timer_get_time();
}

static IRAM_ATTR void assert_wrapper(void)
{
#if CONFIG_BT_CTRL_LE_LOG_EN
    esp_ble_controller_log_dump_all(true);
#endif // CONFIG_BT_CTRL_LE_LOG_EN
}

bool esp_vhci_host_check_send_available(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return false;
    }
    return API_vhci_host_check_send_available();
}

void esp_vhci_host_send_packet(uint8_t *data, uint16_t len)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return;
    }
    async_wakeup_request(BTDM_ASYNC_WAKEUP_SRC_VHCI);

    API_vhci_host_send_packet(data, len);

    async_wakeup_request_end(BTDM_ASYNC_WAKEUP_SRC_VHCI);
}

esp_err_t esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_FAIL;
    }
    return API_vhci_host_register_callback((const vhci_host_callback_t *)callback) == 0 ? ESP_OK : ESP_FAIL;
}

static void btdm_controller_mem_init(void)
{
    extern void btdm_controller_rom_data_init(void );
    btdm_controller_rom_data_init();
}

/**
 * Release two memory areas to the heap. If both areas are consecutive, they will be released as
 * a single area.
 */
typedef struct {
    intptr_t start;
    intptr_t end;
    const char* name;
} bt_area_t;

static esp_err_t esp_bt_mem_release_area(const bt_area_t *area)
{
    esp_err_t ret = ESP_OK;
    intptr_t mem_start = area->start;
    intptr_t mem_end = area->end;
    if (mem_start != mem_end) {
        ESP_LOGD(BT_LOG_TAG, "Release %s [0x%08x] - [0x%08x], len %d", area->name, mem_start, mem_end, mem_end - mem_start);
        ret = try_heap_caps_add_region(mem_start, mem_end);
    }
    return ret;
}

static esp_err_t esp_bt_mem_release_areas(const bt_area_t *area1, const bt_area_t *area2)
{
    esp_err_t ret = ESP_OK;

    if (area1->end == area2->start) {
        bt_area_t merged_area = {
            .start = area1->start,
            .end = area2->end,
            .name = area1->name
        };
        ret = esp_bt_mem_release_area(&merged_area);
    } else {
        esp_bt_mem_release_area(area1);
        ret = esp_bt_mem_release_area(area2);
    }

    return ret;
}

esp_err_t esp_bt_controller_rom_mem_release(esp_bt_mode_t mode)
{
    esp_err_t ret = ESP_OK;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_area_t rom_btdm_data = {
        .start = (intptr_t) ets_rom_layout_p->data_start_btdm,
        .end   = (intptr_t) ets_rom_layout_p->data_end_btdm,
        .name  = "ROM btdm data",
    };
    bt_area_t rom_btdm_bss = {
        .start = (intptr_t)ets_rom_layout_p->bss_start_btdm,
        .end   = (intptr_t)ets_rom_layout_p->bss_end_btdm,
        .name  = "ROM btdm BSS",
    };
    bt_area_t rom_btdm_inter_data = {
        .start = (intptr_t) ets_rom_layout_p->data_start_interface_btdm,
        .end   = (intptr_t) ets_rom_layout_p->data_end_interface_btdm,
        .name  = "ROM interface btdm data",
    };
    bt_area_t rom_btdm_inter_bss = {
        .start = (intptr_t)ets_rom_layout_p->bss_start_interface_btdm,
        .end   = (intptr_t)ets_rom_layout_p->bss_end_interface_btdm,
        .name  = "ROM interface btdm BSS",
    };

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (mode & ESP_BT_MODE_BLE) {
        /* Free BTDM memory used by the ROM */
        if (ret == ESP_OK) {
            ret = esp_bt_mem_release_areas(&rom_btdm_data, &rom_btdm_bss);
        }

        if (ret == ESP_OK) {
            ret = esp_bt_mem_release_areas(&rom_btdm_inter_data, &rom_btdm_inter_bss);
        }
    }

    return ret;
}

esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode)
{
    esp_err_t ret = ESP_OK;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_area_t cont_bss = {
        .start = (intptr_t)&_bt_controller_bss_start,
        .end   = (intptr_t)&_bt_controller_bss_end,
        .name  = "BT Controller BSS",
    };

    bt_area_t cont_data = {
        .start = (intptr_t)&_bt_controller_data_start,
        .end   = (intptr_t)&_bt_controller_data_end,
        .name  = "BT Controller Data"
    };

    if (mode & ESP_BT_MODE_BLE) {
        /* free data and BSS section for libbtdm_app.a */
        if (ret == ESP_OK) {
            ret = esp_bt_mem_release_areas(&cont_data, &cont_bss);
        }
        /* free data and BSS section for Bluetooth controller ROM code */
        if (ret == ESP_OK) {
            ret = esp_bt_controller_rom_mem_release(mode);
        }
    }

    return ret;
}

esp_err_t esp_bt_mem_release(esp_bt_mode_t mode)
{
    esp_err_t ret = ESP_OK;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_area_t bss = {
        .start = (intptr_t)&_bt_bss_start,
        .end   = (intptr_t)&_bt_bss_end,
        .name  = "BT BSS",
    };
    bt_area_t cont_bss = {
        .start = (intptr_t)&_bt_controller_bss_start,
        .end   = (intptr_t)&_bt_controller_bss_end,
        .name  = "BT Controller BSS",
    };
    bt_area_t data = {
        .start = (intptr_t)&_bt_data_start,
        .end   = (intptr_t)&_bt_data_end,
        .name  = "BT Data",
    };
    bt_area_t cont_data = {
        .start = (intptr_t)&_bt_controller_data_start,
        .end   = (intptr_t)&_bt_controller_data_end,
        .name  = "BT Controller Data"
    };

    if (mode & ESP_BT_MODE_BLE) {
        /* Start by freeing Bluetooth BSS section */
        if (ret == ESP_OK) {
            ret = esp_bt_mem_release_areas(&bss, &cont_bss);
        }

        /* Do the same thing with the Bluetooth data section */
        if (ret == ESP_OK) {
            ret = esp_bt_mem_release_areas(&data, &cont_data);
        }

        /* free data and BSS section for Bluetooth controller ROM code */
        if (ret == ESP_OK) {
            ret = esp_bt_controller_rom_mem_release(mode);
        }
    }

    return ret;
}

static esp_err_t try_heap_caps_add_region(intptr_t start, intptr_t end)
{
    int ret = heap_caps_add_region(start, end);
    /* heap_caps_add_region() returns ESP_ERR_INVALID_SIZE if the memory region is
     * is too small to fit a heap. This cannot be termed as a fatal error and hence
     * we replace it by ESP_OK
     */

    if (ret == ESP_ERR_INVALID_SIZE) {
        return ESP_OK;
    }
    return ret;
}

#if CONFIG_MAC_BB_PD
static void IRAM_ATTR btdm_mac_bb_power_down_cb(void)
{
    if (s_lp_cntl.mac_bb_pd && s_lp_stat.mac_bb_pd == 0) {
        btdm_ble_power_down_dma_copy(true);
        s_lp_stat.mac_bb_pd = 1;
    }
}

static void IRAM_ATTR btdm_mac_bb_power_up_cb(void)
{
    if (s_lp_cntl.mac_bb_pd && s_lp_stat.mac_bb_pd) {
        btdm_ble_power_down_dma_copy(false);
        s_lp_stat.mac_bb_pd = 0;
    }
}
#endif

// init low-power control resources
static esp_err_t btdm_low_power_mode_init(esp_bt_controller_config_t *cfg)
{
    esp_err_t err = ESP_OK;

    do {
        // set default values for global states or resources
        s_lp_stat.val = 0;
        s_lp_cntl.val = 0;
        s_lp_cntl.main_xtal_pu = 0;
        s_wakeup_req_sem = NULL;
        s_btdm_slp_tmr = NULL;

        // configure and initialize resources
        s_lp_cntl.enable = (cfg->sleep_mode == ESP_BT_SLEEP_MODE_1) ? 1 : 0;
        s_lp_cntl.lpclk_sel = (cfg->sleep_mode == ESP_BT_SLEEP_MODE_1) ? cfg->sleep_clock : ESP_BT_SLEEP_CLOCK_MAIN_XTAL;
        s_lp_cntl.no_light_sleep = 0;

        if (s_lp_cntl.enable) {
#if CONFIG_MAC_BB_PD
            if (!btdm_deep_sleep_mem_init()) {
                err = ESP_ERR_NO_MEM;
                break;
            }
            s_lp_cntl.mac_bb_pd = 1;
#endif
#ifdef CONFIG_PM_ENABLE
            s_lp_cntl.wakeup_timer_required = 1;
#endif
            // async wakeup semaphore for VHCI
            s_wakeup_req_sem = semphr_create_wrapper(1, 0);
            if (s_wakeup_req_sem == NULL) {
                err = ESP_ERR_NO_MEM;
                break;
            }
            btdm_vnd_offload_task_register(BTDM_VND_OL_SIG_WAKEUP_TMR, btdm_sleep_exit_phase0);

            if (s_lp_cntl.wakeup_timer_required) {
                esp_timer_create_args_t create_args = {
                    .callback = btdm_slp_tmr_callback,
                    .arg = NULL,
                    .name = "btSlp",
                };
                if ((err = esp_timer_create(&create_args, &s_btdm_slp_tmr)) != ESP_OK) {
                    break;
                }
            }

            // set default bluetooth sleep clock cycle and its fractional bits
            btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
            btdm_lpcycle_us = 2 << (btdm_lpcycle_us_frac);

            if (s_lp_cntl.lpclk_sel == ESP_BT_SLEEP_CLOCK_EXT_32K_XTAL) { // External 32 kHz XTAL
                // check whether or not EXT_CRYS is working
                if (rtc_clk_slow_src_get() != SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
                    ESP_LOGW(BT_LOG_TAG, "32.768kHz XTAL not detected, fall back to main XTAL as Bluetooth sleep clock");
                    s_lp_cntl.lpclk_sel = ESP_BT_SLEEP_CLOCK_MAIN_XTAL;
#if !CONFIG_BT_CTRL_MAIN_XTAL_PU_DURING_LIGHT_SLEEP
                    s_lp_cntl.no_light_sleep = 1;
#endif
                }
            } else if (s_lp_cntl.lpclk_sel == ESP_BT_SLEEP_CLOCK_RTC_SLOW) {  // Internal 136kHz RC oscillator
                if (rtc_clk_slow_src_get() == SOC_RTC_SLOW_CLK_SRC_RC_SLOW) {
                    ESP_LOGW(BT_LOG_TAG, "Internal 136kHz RC oscillator. The accuracy of this clock is a lot larger than 500ppm which is "
                                "required in Bluetooth communication, so don't select this option in scenarios such as BLE connection state.");
                } else {
                    ESP_LOGW(BT_LOG_TAG, "Internal 136kHz RC oscillator not detected.");
                    assert(0);
                }
            } else if (s_lp_cntl.lpclk_sel == ESP_BT_SLEEP_CLOCK_MAIN_XTAL) {
                ESP_LOGI(BT_LOG_TAG, "Bluetooth will use main XTAL as Bluetooth sleep clock.");
#if !CONFIG_BT_CTRL_MAIN_XTAL_PU_DURING_LIGHT_SLEEP
                s_lp_cntl.no_light_sleep = 1;
#endif
            }
        } else {
            s_lp_cntl.no_light_sleep = 1;
        }

        bool select_src_ret __attribute__((unused));
        bool set_div_ret __attribute__((unused));
        if (s_lp_cntl.lpclk_sel == ESP_BT_SLEEP_CLOCK_MAIN_XTAL) {
#ifdef CONFIG_BT_CTRL_MAIN_XTAL_PU_DURING_LIGHT_SLEEP
            ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON));
            s_lp_cntl.main_xtal_pu = 1;
#endif
            select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL);
            set_div_ret = btdm_lpclk_set_div(esp_clk_xtal_freq() / MHZ);
            assert(select_src_ret && set_div_ret);
            btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
            btdm_lpcycle_us = 1 << (btdm_lpcycle_us_frac);
        } else if (s_lp_cntl.lpclk_sel == ESP_BT_SLEEP_CLOCK_EXT_32K_XTAL) {
            select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL32K);
            set_div_ret = btdm_lpclk_set_div(0);
            assert(select_src_ret && set_div_ret);
            btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
            btdm_lpcycle_us = (RTC_CLK_CAL_FRACT > 15) ? (1000000 << (RTC_CLK_CAL_FRACT - 15)) :
                (1000000 >> (15 - RTC_CLK_CAL_FRACT));
            assert(btdm_lpcycle_us != 0);
        } else if (s_lp_cntl.lpclk_sel == ESP_BT_SLEEP_CLOCK_RTC_SLOW) {
            select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_RTC_SLOW);
            set_div_ret = btdm_lpclk_set_div(0);
            assert(select_src_ret && set_div_ret);
            btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
            btdm_lpcycle_us = esp_clk_slowclk_cal_get();
        } else {
            err = ESP_ERR_INVALID_ARG;
            break;
        }
#if CONFIG_SW_COEXIST_ENABLE
        coex_update_lpclk_interval();
#endif

#ifdef CONFIG_PM_ENABLE
        if (s_lp_cntl.no_light_sleep) {
            if ((err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "btLS", &s_light_sleep_pm_lock)) != ESP_OK) {
                break;
            }
            ESP_LOGW(BT_LOG_TAG, "light sleep mode will not be able to apply when bluetooth is enabled.");
        }
        if ((err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "bt", &s_pm_lock)) != ESP_OK) {
            break;
        } else {
            s_lp_stat.pm_lock_released = 1;
        }
#endif
    } while (0);

    return err;
}

esp_bt_sleep_clock_t esp_bt_get_lpclk_src(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED &&
            btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_BT_SLEEP_CLOCK_NONE;
    }

    return s_lp_cntl.lpclk_sel;
}

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    esp_err_t err = ESP_FAIL;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (cfg->controller_task_prio != ESP_TASK_BT_CONTROLLER_PRIO
        || cfg->controller_task_stack_size < ESP_TASK_BT_CONTROLLER_STACK) {
        ESP_LOGE(BT_LOG_TAG, "Invalid controller task prioriy or stack size");
        return ESP_ERR_INVALID_ARG;
    }

    if (cfg->bluetooth_mode != ESP_BT_MODE_BLE) {
        ESP_LOGE(BT_LOG_TAG, "%s controller only support BLE only mode", __func__);
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (cfg->bluetooth_mode & ESP_BT_MODE_BLE) {
        if ((cfg->ble_max_act <= 0) || (cfg->ble_max_act > BT_CTRL_BLE_MAX_ACT_LIMIT)) {
            ESP_LOGE(BT_LOG_TAG, "Invalid value of ble_max_act");
            return ESP_ERR_INVALID_ARG;
        }
    }

    if (cfg->sleep_mode == ESP_BT_SLEEP_MODE_1) {
        if (cfg->sleep_clock == ESP_BT_SLEEP_CLOCK_NONE) {
            ESP_LOGE(BT_LOG_TAG, "SLEEP_MODE_1 enabled but sleep clock not configured");
            return ESP_ERR_INVALID_ARG;
        }
        if (cfg->sleep_clock > ESP_BT_SLEEP_CLOCK_RTC_SLOW) {
            ESP_LOGE(BT_LOG_TAG, "SLEEP_MODE_1 is enabled but this sleep clock is not supported");
            return ESP_ERR_INVALID_ARG;
        }
    }

    // overwrite some parameters
    cfg->magic = ESP_BT_CTRL_CONFIG_MAGIC_VAL;

#if CONFIG_MAC_BB_PD
    esp_mac_bb_pd_mem_init();
#endif
    esp_phy_modem_init();
    esp_bt_power_domain_on();

    btdm_controller_mem_init();

    osi_funcs_p = (struct osi_funcs_t *)malloc_internal_wrapper(sizeof(struct osi_funcs_t));
    if (osi_funcs_p == NULL) {
        return ESP_ERR_NO_MEM;
    }

    memcpy(osi_funcs_p, &osi_funcs_ro, sizeof(struct osi_funcs_t));
    if (btdm_osi_funcs_register(osi_funcs_p) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(BT_LOG_TAG, "BT controller compile version [%s]", btdm_controller_get_compile_version());

#if (CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY)
    ESP_LOGI(BT_LOG_TAG,"Put all controller code in flash");
#endif

    if ((err = btdm_low_power_mode_init(cfg)) != ESP_OK) {
        ESP_LOGE(BT_LOG_TAG, "Low power module initialization failed");
        goto error;
    }

#if CONFIG_SW_COEXIST_ENABLE
    coex_init();
#endif

#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    if (ble_log_spi_out_init() != 0) {
        ESP_LOGE(BT_LOG_TAG, "BLE Log SPI output init failed");
        goto error;
    }
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

    periph_module_enable(PERIPH_BT_MODULE);
    periph_module_reset(PERIPH_BT_MODULE);

#if CONFIG_BT_CTRL_LE_LOG_EN
    err = esp_bt_controller_log_init(log_output_mode);
    if (err != ESP_OK) {
        ESP_LOGW(BT_LOG_TAG, "ble_controller_log_init failed %d", err);
        goto error;
    }
#endif // CONFIG_BT_CTRL_LE_LOG_EN

    err = btdm_controller_init(cfg);

    if (err != 0) {
        ESP_LOGE(BT_LOG_TAG, "%s %d\n",__func__,err);
        err = ESP_ERR_NO_MEM;
        goto error;
    }

#if (CONFIG_BT_BLUEDROID_ENABLED || CONFIG_BT_NIMBLE_ENABLED)
    scan_stack_enableAdvFlowCtrlVsCmd(true);
    adv_stack_enableClearLegacyAdvVsCmd(true);
    advFilter_stack_enableDupExcListVsCmd(true);
    chanSel_stack_enableSetCsaVsCmd(true);
#endif // (CONFIG_BT_BLUEDROID_ENABLED || CONFIG_BT_NIMBLE_ENABLED)

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    return ESP_OK;

error:

#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    ble_log_spi_out_deinit();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

    bt_controller_deinit_internal();

    return err;
}

esp_err_t esp_bt_controller_deinit(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    ble_log_spi_out_deinit();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

#if (CONFIG_BT_BLUEDROID_ENABLED || CONFIG_BT_NIMBLE_ENABLED)
    scan_stack_enableAdvFlowCtrlVsCmd(false);
    adv_stack_enableClearLegacyAdvVsCmd(false);
    advFilter_stack_enableDupExcListVsCmd(false);
    chanSel_stack_enableSetCsaVsCmd(false);
#endif // (CONFIG_BT_BLUEDROID_ENABLED || CONFIG_BT_NIMBLE_ENABLED)

    btdm_controller_deinit();

    bt_controller_deinit_internal();

    return ESP_OK;
}

// deinit low power control resources
static void btdm_low_power_mode_deinit(void)
{
#if CONFIG_MAC_BB_PD
    if (s_lp_cntl.mac_bb_pd) {
        btdm_deep_sleep_mem_deinit();
        s_lp_cntl.mac_bb_pd = 0;
    }
#endif

#ifdef CONFIG_PM_ENABLE
    if (s_lp_cntl.no_light_sleep) {
        if (s_light_sleep_pm_lock != NULL) {
            esp_pm_lock_delete(s_light_sleep_pm_lock);
            s_light_sleep_pm_lock = NULL;
        }
    }

    if (s_pm_lock != NULL) {
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
        s_lp_stat.pm_lock_released = 0;
    }
#endif

    if (s_lp_cntl.wakeup_timer_required && s_btdm_slp_tmr != NULL) {
        if (s_lp_stat.wakeup_timer_started) {
            esp_timer_stop(s_btdm_slp_tmr);
        }
        s_lp_stat.wakeup_timer_started = 0;
        esp_timer_delete(s_btdm_slp_tmr);
        s_btdm_slp_tmr = NULL;
    }

    if (s_lp_cntl.enable) {
        btdm_vnd_offload_task_deregister(BTDM_VND_OL_SIG_WAKEUP_TMR);
        if (s_wakeup_req_sem != NULL) {
            semphr_delete_wrapper(s_wakeup_req_sem);
            s_wakeup_req_sem = NULL;
        }
    }

    if (s_lp_cntl.lpclk_sel == ESP_BT_SLEEP_CLOCK_MAIN_XTAL) {
#ifdef CONFIG_BT_CTRL_MAIN_XTAL_PU_DURING_LIGHT_SLEEP
        if (s_lp_cntl.main_xtal_pu) {
            ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF));
            s_lp_cntl.main_xtal_pu = 0;
        }
#endif
        btdm_lpclk_select_src(BTDM_LPCLK_SEL_RTC_SLOW);
        btdm_lpclk_set_div(0);
#if CONFIG_SW_COEXIST_ENABLE
        coex_update_lpclk_interval();
#endif
    }

    btdm_lpcycle_us = 0;
}

static void bt_controller_deinit_internal(void)
{
    periph_module_disable(PERIPH_BT_MODULE);

    btdm_low_power_mode_deinit();

    esp_bt_power_domain_off();
#if CONFIG_MAC_BB_PD
    esp_mac_bb_pd_mem_deinit();
#endif
    esp_phy_modem_deinit();

#if CONFIG_BT_CTRL_LE_LOG_EN
    esp_bt_ontroller_log_deinit();
#endif // CONFIG_BT_CTRL_LE_LOG_EN

    if (osi_funcs_p != NULL) {
        free(osi_funcs_p);
        osi_funcs_p = NULL;
    }

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;
}

esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode)
{
    esp_err_t ret = ESP_OK;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

    //As the history reason, mode should be equal to the mode which set in esp_bt_controller_init()
    if (mode != btdm_controller_get_mode()) {
        ESP_LOGE(BT_LOG_TAG, "invalid mode %d, controller support mode is %d", mode, btdm_controller_get_mode());
        return ESP_ERR_INVALID_ARG;
    }

    /* Enable PHY when enabling controller to reduce power dissipation after controller init
     * Notice the init order: esp_phy_enable() -> bt_bb_v2_init_cmplx() -> coex_pti_v2()
     */
    esp_phy_enable(PHY_MODEM_BT);
    s_lp_stat.phy_enabled = 1;

#if CONFIG_SW_COEXIST_ENABLE
    coex_enable();
#endif

    // enable low power mode
    do {
#ifdef CONFIG_PM_ENABLE
        if (s_lp_cntl.no_light_sleep) {
            esp_pm_lock_acquire(s_light_sleep_pm_lock);
        }
        esp_pm_lock_acquire(s_pm_lock);
        s_lp_stat.pm_lock_released = 0;
#endif

#if CONFIG_MAC_BB_PD
        if (esp_register_mac_bb_pd_callback(btdm_mac_bb_power_down_cb) != 0) {
            ret = ESP_ERR_INVALID_ARG;
            goto error;
        }

        if (esp_register_mac_bb_pu_callback(btdm_mac_bb_power_up_cb) != 0) {
            ret = ESP_ERR_INVALID_ARG;
            goto error;
        }
#endif

        if (s_lp_cntl.enable) {
            btdm_controller_enable_sleep(true);
        }
    } while (0);

    // Disable pll track by default in BLE controller on ESP32-C3 and ESP32-S3
    sdk_config_extend_set_pll_track(false);

    if (btdm_controller_enable(mode) != 0) {
        ret = ESP_ERR_INVALID_STATE;
        goto error;
    }

    coex_pti_v2();

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;

    return ret;

error:
    // disable low power mode
    do {
#if CONFIG_MAC_BB_PD
        esp_unregister_mac_bb_pd_callback(btdm_mac_bb_power_down_cb);
        esp_unregister_mac_bb_pu_callback(btdm_mac_bb_power_up_cb);
#endif

        btdm_controller_enable_sleep(false);
#ifdef CONFIG_PM_ENABLE
        if (s_lp_cntl.no_light_sleep) {
            esp_pm_lock_release(s_light_sleep_pm_lock);
        }
        if (s_lp_stat.pm_lock_released == 0) {
            esp_pm_lock_release(s_pm_lock);
            s_lp_stat.pm_lock_released = 1;
        }
#endif
    } while (0);

#if CONFIG_SW_COEXIST_ENABLE
    coex_disable();
#endif
    if (s_lp_stat.phy_enabled) {
        esp_phy_disable(PHY_MODEM_BT);
        s_lp_stat.phy_enabled = 0;
    }
    return ret;
}

esp_err_t esp_bt_controller_disable(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    async_wakeup_request(BTDM_ASYNC_WAKEUP_SRC_DISA);
    while (!btdm_power_state_active()){}
    btdm_controller_disable();

    async_wakeup_request_end(BTDM_ASYNC_WAKEUP_SRC_DISA);

#if CONFIG_SW_COEXIST_ENABLE
    coex_disable();
#endif
    if (s_lp_stat.phy_enabled) {
        esp_phy_disable(PHY_MODEM_BT);
        s_lp_stat.phy_enabled = 0;
    }

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    // disable low power mode
    do {
#if CONFIG_MAC_BB_PD
        esp_unregister_mac_bb_pd_callback(btdm_mac_bb_power_down_cb);
        esp_unregister_mac_bb_pu_callback(btdm_mac_bb_power_up_cb);
#endif

#ifdef CONFIG_PM_ENABLE
        if (s_lp_cntl.no_light_sleep) {
            esp_pm_lock_release(s_light_sleep_pm_lock);
        }

        if (s_lp_stat.pm_lock_released == 0) {
            esp_pm_lock_release(s_pm_lock);
            s_lp_stat.pm_lock_released = 1;
        } else {
            assert(0);
        }
#endif
    } while (0);

    return ESP_OK;
}

esp_bt_controller_status_t esp_bt_controller_get_status(void)
{
    return btdm_controller_status;
}

static int enh_power_type_get(esp_ble_power_type_t power_type)
{
    switch (power_type) {
    case ESP_BLE_PWR_TYPE_ADV:
        return ESP_BLE_ENHANCED_PWR_TYPE_ADV;
    case ESP_BLE_PWR_TYPE_SCAN:
        return ESP_BLE_ENHANCED_PWR_TYPE_SCAN;
    case ESP_BLE_PWR_TYPE_CONN_HDL0:
    case ESP_BLE_PWR_TYPE_CONN_HDL1:
    case ESP_BLE_PWR_TYPE_CONN_HDL2:
    case ESP_BLE_PWR_TYPE_CONN_HDL3:
    case ESP_BLE_PWR_TYPE_CONN_HDL4:
    case ESP_BLE_PWR_TYPE_CONN_HDL5:
    case ESP_BLE_PWR_TYPE_CONN_HDL6:
    case ESP_BLE_PWR_TYPE_CONN_HDL7:
    case ESP_BLE_PWR_TYPE_CONN_HDL8:
        return ESP_BLE_ENHANCED_PWR_TYPE_CONN;
    case ESP_BLE_PWR_TYPE_DEFAULT:
        return ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT;
    default:
        break;
    }

    return power_type;
}

/* extra functions */
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level)
{
    esp_err_t stat = ESP_FAIL;
    uint16_t handle = BLE_PWR_HDL_INVL;
    int enh_pwr_type = enh_power_type_get(power_type);

    if (power_type > ESP_BLE_PWR_TYPE_DEFAULT) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (enh_pwr_type == ESP_BLE_ENHANCED_PWR_TYPE_CONN) {
        handle = power_type;
    }

    if (ble_txpwr_set(enh_pwr_type, handle, power_level) == 0) {
        stat = ESP_OK;
    }

    return stat;
}

esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type)
{
    esp_power_level_t lvl;
    uint16_t handle = BLE_PWR_HDL_INVL;
    int enh_pwr_type = enh_power_type_get(power_type);

    if (power_type > ESP_BLE_PWR_TYPE_DEFAULT) {
        return ESP_PWR_LVL_INVALID;
    }

    if (enh_pwr_type == ESP_BLE_ENHANCED_PWR_TYPE_CONN) {
        handle = power_type;
    }

    lvl = (esp_power_level_t)ble_txpwr_get(enh_pwr_type, handle);

    return lvl;
}

esp_err_t esp_ble_tx_power_set_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle,
                                        esp_power_level_t power_level)
{
    esp_err_t stat = ESP_FAIL;

    switch (power_type) {
    case ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT:
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        if (ble_txpwr_set(power_type, BLE_PWR_HDL_INVL, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_ADV:
    case ESP_BLE_ENHANCED_PWR_TYPE_CONN:
        if (ble_txpwr_set(power_type, handle, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    default:
        stat = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return stat;
}

esp_power_level_t esp_ble_tx_power_get_enhanced(esp_ble_enhanced_power_type_t power_type,
                                                uint16_t handle)
{
    int tx_level = 0;

    switch (power_type) {
    case ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT:
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        tx_level = ble_txpwr_get(power_type, BLE_PWR_HDL_INVL);
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_ADV:
    case ESP_BLE_ENHANCED_PWR_TYPE_CONN:
        tx_level = ble_txpwr_get(power_type, handle);
        break;
    default:
        return ESP_PWR_LVL_INVALID;
    }

    return (esp_power_level_t)tx_level;
}

esp_err_t esp_bt_sleep_enable (void)
{
    esp_err_t status;
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
        btdm_controller_enable_sleep (true);
        status = ESP_OK;
    } else {
        status = ESP_ERR_NOT_SUPPORTED;
    }

    return status;
}

esp_err_t esp_bt_sleep_disable (void)
{
    esp_err_t status;
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
        btdm_controller_enable_sleep (false);
        status = ESP_OK;
    } else {
        status = ESP_ERR_NOT_SUPPORTED;
    }

    return status;
}

bool esp_bt_controller_is_sleeping(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED ||
        btdm_controller_get_sleep_mode() != ESP_BT_SLEEP_MODE_1) {
        return false;
    }

    return !btdm_power_state_active();
}

void esp_bt_controller_wakeup_request(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED ||
        btdm_controller_get_sleep_mode() != ESP_BT_SLEEP_MODE_1) {
        return;
    }

    btdm_wakeup_request();

}

int IRAM_ATTR esp_bt_h4tl_eif_io_event_notify(int event)
{
    return btdm_hci_tl_io_event_post(event);
}

static void coex_wifi_sleep_set_hook(bool sleep)
{

}

static int coex_schm_register_btdm_callback_wrapper(void *callback)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_schm_register_callback(COEX_SCHM_CALLBACK_TYPE_BT, callback);
#else
    return 0;
#endif
}

static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_schm_status_bit_clear(type, status);
#endif
}

static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_schm_status_bit_set(type, status);
#endif
}

static uint32_t coex_schm_interval_get_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_schm_interval_get();
#else
    return 0;
#endif
}

static uint8_t coex_schm_curr_period_get_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_schm_curr_period_get();
#else
    return 1;
#endif
}

static void * coex_schm_curr_phase_get_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_schm_curr_phase_get();
#else
    return NULL;
#endif
}

#endif /*  CONFIG_BT_ENABLED */
