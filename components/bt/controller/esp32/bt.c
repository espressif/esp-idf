/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
#include "xtensa_api.h" // Replace with interrupt allocator API (IDF-3891)
#include "xtensa/core-macros.h"
#include "esp_types.h"
#include "esp_mac.h"
#include "esp_random.h"
#include "esp_task.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#ifdef CONFIG_ESP_PHY_ENABLED
#include "esp_phy_init.h"
#endif
#include "esp_bt.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_private/esp_clk.h"
#include "esp_private/periph_ctrl.h"
#include "soc/rtc.h"
#include "soc/soc_memory_layout.h"
#include "soc/dport_reg.h"
#ifdef CONFIG_ESP_COEX_ENABLED
#include "private/esp_coexist_internal.h"
#endif
#include "esp_timer.h"
#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#endif

#include "esp_rom_sys.h"
#include "hli_api.h"

#if CONFIG_BT_ENABLED

/* Macro definition
 ************************************************************************
 */

#define UNUSED(x)                           (void)(x)
#define BTDM_LOG_TAG                        "BTDM_INIT"

#define BTDM_INIT_PERIOD                    (5000)    /* ms */

/* Bluetooth system and controller config */
#define BTDM_CFG_BT_DATA_RELEASE            (1<<0)
#define BTDM_CFG_HCI_UART                   (1<<1)
#define BTDM_CFG_CONTROLLER_RUN_APP_CPU     (1<<2)
#define BTDM_CFG_SCAN_DUPLICATE_OPTIONS     (1<<3)
#define BTDM_CFG_SEND_ADV_RESERVED_SIZE     (1<<4)
#define BTDM_CFG_BLE_FULL_SCAN_SUPPORTED    (1<<5)

/* Sleep mode */
#define BTDM_MODEM_SLEEP_MODE_NONE          (0)
#define BTDM_MODEM_SLEEP_MODE_ORIG          (1)
#define BTDM_MODEM_SLEEP_MODE_EVED          (2)  // sleep mode for BLE controller, used only for internal test.

/* Low Power Clock Selection */
#define BTDM_LPCLK_SEL_XTAL      (0)
#define BTDM_LPCLK_SEL_XTAL32K   (1)
#define BTDM_LPCLK_SEL_RTC_SLOW  (2)
#define BTDM_LPCLK_SEL_8M        (3)

/* Sleep and wakeup interval control */
#define BTDM_MIN_SLEEP_DURATION          (12) // threshold of interval in slots to allow to fall into modem sleep
#define BTDM_MODEM_WAKE_UP_DELAY         (4)  // delay in slots of modem wake up procedure, including re-enable PHY/RF

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
#define OSI_VERSION              0x00010005
#define OSI_MAGIC_VALUE          0xFADEBEAD

#define BLE_CONTROLLER_MALLOC_CAPS        (MALLOC_CAP_8BIT|MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL)
/* Types definition
 ************************************************************************
 */

/* VHCI function interface */
typedef struct vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} vhci_host_callback_t;

/* Dram region */
typedef struct {
    esp_bt_mode_t mode;
    intptr_t start;
    intptr_t end;
} btdm_dram_available_region_t;

typedef struct {
    void *handle;
} btdm_queue_item_t;

/* OSI function */
struct osi_funcs_t {
    uint32_t _version;
    xt_handler (*_set_isr)(int n, xt_handler f, void *arg);
    void (*_ints_on)(unsigned int mask);
    void (*_interrupt_disable)(void);
    void (*_interrupt_restore)(void);
    void (*_task_yield)(void);
    void (*_task_yield_from_isr)(void);
    void *(*_semphr_create)(uint32_t max, uint32_t init);
    void (*_semphr_delete)(void *semphr);
    int32_t (*_semphr_take_from_isr)(void *semphr, void *hptw);
    int32_t (*_semphr_give_from_isr)(void *semphr, void *hptw);
    int32_t (*_semphr_take)(void *semphr, uint32_t block_time_ms);
    int32_t (*_semphr_give)(void *semphr);
    void *(*_mutex_create)(void);
    void (*_mutex_delete)(void *mutex);
    int32_t (*_mutex_lock)(void *mutex);
    int32_t (*_mutex_unlock)(void *mutex);
    void *(* _queue_create)(uint32_t queue_len, uint32_t item_size);
    void (* _queue_delete)(void *queue);
    int32_t (* _queue_send)(void *queue, void *item, uint32_t block_time_ms);
    int32_t (* _queue_send_from_isr)(void *queue, void *item, void *hptw);
    int32_t (* _queue_recv)(void *queue, void *item, uint32_t block_time_ms);
    int32_t (* _queue_recv_from_isr)(void *queue, void *item, void *hptw);
    int32_t (* _task_create)(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
    void (* _task_delete)(void *task_handle);
    bool (* _is_in_isr)(void);
    int (* _cause_sw_intr_to_core)(int core_id, int intr_no);
    void *(* _malloc)(size_t size);
    void *(* _malloc_internal)(size_t size);
    void (* _free)(void *p);
    int32_t (* _read_efuse_mac)(uint8_t mac[6]);
    void (* _srand)(unsigned int seed);
    int (* _rand)(void);
    uint32_t (* _btdm_lpcycles_2_us)(uint32_t cycles);
    uint32_t (* _btdm_us_2_lpcycles)(uint32_t us);
    bool (* _btdm_sleep_check_duration)(uint32_t *slot_cnt);
    void (* _btdm_sleep_enter_phase1)(uint32_t lpcycles);  /* called when interrupt is disabled */
    void (* _btdm_sleep_enter_phase2)(void);
    void (* _btdm_sleep_exit_phase1)(void);  /* called from ISR */
    void (* _btdm_sleep_exit_phase2)(void);  /* called from ISR */
    void (* _btdm_sleep_exit_phase3)(void);  /* called from task */
    bool (* _coex_bt_wakeup_request)(void);
    void (* _coex_bt_wakeup_request_end)(void);
    int (* _coex_bt_request)(uint32_t event, uint32_t latency, uint32_t duration);
    int (* _coex_bt_release)(uint32_t event);
    int (* _coex_register_bt_cb)(coex_func_cb_t cb);
    uint32_t (* _coex_bb_reset_lock)(void);
    void (* _coex_bb_reset_unlock)(uint32_t restore);
    int (* _coex_schm_register_btdm_callback)(void *callback);
    void (* _coex_schm_status_bit_clear)(uint32_t type, uint32_t status);
    void (* _coex_schm_status_bit_set)(uint32_t type, uint32_t status);
    uint32_t (* _coex_schm_interval_get)(void);
    uint8_t (* _coex_schm_curr_period_get)(void);
    void *(* _coex_schm_curr_phase_get)(void);
    int (* _coex_wifi_channel_get)(uint8_t *primary, uint8_t *secondary);
    int (* _coex_register_wifi_channel_change_callback)(void *cb);
    xt_handler (*_set_isr_l3)(int n, xt_handler f, void *arg);
    void (*_interrupt_l3_disable)(void);
    void (*_interrupt_l3_restore)(void);
    void *(* _customer_queue_create)(uint32_t queue_len, uint32_t item_size);
    int (* _coex_version_get)(unsigned int *major, unsigned int *minor, unsigned int *patch);
    void (* _patch_apply)(void);
    uint32_t _magic;
};

typedef void (*workitem_handler_t)(void* arg);

/* External functions or values
 ************************************************************************
 */

/* not for user call, so don't put to include file */
/* OSI */
extern int btdm_osi_funcs_register(void *osi_funcs);
/* Initialise and De-initialise */
extern int btdm_controller_init(uint32_t config_mask, esp_bt_controller_config_t *config_opts);
extern void btdm_controller_deinit(void);
extern int btdm_controller_enable(esp_bt_mode_t mode);
extern void btdm_controller_disable(void);
extern uint8_t btdm_controller_get_mode(void);
extern const char *btdm_controller_get_compile_version(void);
extern void btdm_rf_bb_init_phase2(void); // shall be called after PHY/RF is enabled
extern int btdm_dispatch_work_to_controller(workitem_handler_t callback, void *arg, bool blocking);
/* Sleep */
extern void btdm_controller_enable_sleep(bool enable);
extern void btdm_controller_set_sleep_mode(uint8_t mode);
extern uint8_t btdm_controller_get_sleep_mode(void);
extern bool btdm_power_state_active(void);
extern void btdm_wakeup_request(void);
extern void btdm_in_wakeup_requesting_set(bool in_wakeup_requesting);
/* Low Power Clock */
extern bool btdm_lpclk_select_src(uint32_t sel);
extern bool btdm_lpclk_set_div(uint32_t div);
/* VHCI */
extern bool API_vhci_host_check_send_available(void);
extern void API_vhci_host_send_packet(uint8_t *data, uint16_t len);
extern int API_vhci_host_register_callback(const vhci_host_callback_t *callback);
/* TX power */
extern int ble_txpwr_set(int power_type, int power_level);
extern int ble_txpwr_get(int power_type);
extern int bredr_txpwr_set(int min_power_level, int max_power_level);
extern int bredr_txpwr_get(int *min_power_level, int *max_power_level);
extern void bredr_sco_datapath_set(uint8_t data_path);
extern void btdm_controller_scan_duplicate_list_clear(void);
/* Shutdown */
extern void esp_bt_controller_shutdown(void);
extern void sdk_config_set_bt_pll_track_enable(bool enable);
extern void sdk_config_set_uart_flow_ctrl_enable(bool enable);

extern char _bss_start_btdm;
extern char _bss_end_btdm;
extern char _data_start_btdm;
extern char _data_end_btdm;
extern uint32_t _data_start_btdm_rom;
extern uint32_t _data_end_btdm_rom;

extern uint32_t _bt_bss_start;
extern uint32_t _bt_bss_end;
extern uint32_t _bt_controller_bss_start;
extern uint32_t _bt_controller_bss_end;
extern uint32_t _bt_data_start;
extern uint32_t _bt_data_end;
extern uint32_t _bt_controller_data_start;
extern uint32_t _bt_controller_data_end;

extern void config_bt_funcs_reset(void);
extern void config_ble_funcs_reset(void);
extern void config_btdm_funcs_reset(void);

/* Local Function Declare
 *********************************************************************
 */
#if CONFIG_BTDM_CTRL_HLI
static xt_handler set_isr_hlevel_wrapper(int n, xt_handler f, void *arg);
static void interrupt_hlevel_disable(void);
static void interrupt_hlevel_restore(void);
#endif /* CONFIG_BTDM_CTRL_HLI */
static void task_yield(void);
static void task_yield_from_isr(void);
static void *semphr_create_wrapper(uint32_t max, uint32_t init);
static void semphr_delete_wrapper(void *semphr);
static int32_t semphr_take_from_isr_wrapper(void *semphr, void *hptw);
static int32_t semphr_give_from_isr_wrapper(void *semphr, void *hptw);
static int32_t  semphr_take_wrapper(void *semphr, uint32_t block_time_ms);
static int32_t  semphr_give_wrapper(void *semphr);
static void *mutex_create_wrapper(void);
static void mutex_delete_wrapper(void *mutex);
static int32_t mutex_lock_wrapper(void *mutex);
static int32_t mutex_unlock_wrapper(void *mutex);
#if CONFIG_BTDM_CTRL_HLI
static void *queue_create_hlevel_wrapper(uint32_t queue_len, uint32_t item_size);
static void queue_delete_hlevel_wrapper(void *queue);
static int32_t queue_send_hlevel_wrapper(void *queue, void *item, uint32_t block_time_ms);
static int32_t queue_send_from_isr_hlevel_wrapper(void *queue, void *item, void *hptw);
static int32_t queue_recv_hlevel_wrapper(void *queue, void *item, uint32_t block_time_ms);
static int32_t queue_recv_from_isr_hlevel_wrapper(void *queue, void *item, void *hptw);
#else
static void *queue_create_wrapper(uint32_t queue_len, uint32_t item_size);
static void queue_delete_wrapper(void *queue);
static int32_t queue_send_wrapper(void *queue, void *item, uint32_t block_time_ms);
static int32_t queue_send_from_isr_wrapper(void *queue, void *item, void *hptw);
static int32_t queue_recv_wrapper(void *queue, void *item, uint32_t block_time_ms);
static int32_t queue_recv_from_isr_wrapper(void *queue, void *item, void *hptw);
#endif /* CONFIG_BTDM_CTRL_HLI */
static int32_t task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
static void task_delete_wrapper(void *task_handle);
static bool is_in_isr_wrapper(void);
static void cause_sw_intr(void *arg);
static int cause_sw_intr_to_core_wrapper(int core_id, int intr_no);
static void *malloc_internal_wrapper(size_t size);
static int32_t read_mac_wrapper(uint8_t mac[6]);
static void srand_wrapper(unsigned int seed);
static int rand_wrapper(void);
static uint32_t btdm_lpcycles_2_us(uint32_t cycles);
static uint32_t btdm_us_2_lpcycles(uint32_t us);
static bool btdm_sleep_check_duration(uint32_t *slot_cnt);
static void btdm_sleep_enter_phase1_wrapper(uint32_t lpcycles);
static void btdm_sleep_enter_phase2_wrapper(void);
static void btdm_sleep_exit_phase3_wrapper(void);
static bool coex_bt_wakeup_request(void);
static void coex_bt_wakeup_request_end(void);
static int coex_bt_request_wrapper(uint32_t event, uint32_t latency, uint32_t duration);
static int coex_bt_release_wrapper(uint32_t event);
static int coex_register_bt_cb_wrapper(coex_func_cb_t cb);
static uint32_t coex_bb_reset_lock_wrapper(void);
static void coex_bb_reset_unlock_wrapper(uint32_t restore);
static int coex_schm_register_btdm_callback_wrapper(void *callback);
static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status);
static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status);
static uint32_t coex_schm_interval_get_wrapper(void);
static uint8_t coex_schm_curr_period_get_wrapper(void);
static void * coex_schm_curr_phase_get_wrapper(void);
static int coex_wifi_channel_get_wrapper(uint8_t *primary, uint8_t *secondary);
static int coex_register_wifi_channel_change_callback_wrapper(void *cb);
static int coex_version_get_wrapper(unsigned int *major, unsigned int *minor, unsigned int *patch);
#if CONFIG_BTDM_CTRL_HLI
static void *customer_queue_create_hlevel_wrapper(uint32_t queue_len, uint32_t item_size);
#endif /* CONFIG_BTDM_CTRL_HLI */
static void interrupt_l3_disable(void);
static void interrupt_l3_restore(void);
static void bt_controller_deinit_internal(void);
static void patch_apply(void);

/* Local variable definition
 ***************************************************************************
 */
/* OSI funcs */
static const struct osi_funcs_t osi_funcs_ro = {
    ._version = OSI_VERSION,
#if CONFIG_BTDM_CTRL_HLI
    ._set_isr = set_isr_hlevel_wrapper,
    ._ints_on = xt_ints_on,
    ._interrupt_disable = interrupt_hlevel_disable,
    ._interrupt_restore = interrupt_hlevel_restore,
#else
    ._set_isr = xt_set_interrupt_handler,
    ._ints_on = xt_ints_on,
    ._interrupt_disable = interrupt_l3_disable,
    ._interrupt_restore = interrupt_l3_restore,
#endif /* CONFIG_BTDM_CTRL_HLI */
    ._task_yield = task_yield,
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
#if CONFIG_BTDM_CTRL_HLI
    ._queue_create = queue_create_hlevel_wrapper,
    ._queue_delete = queue_delete_hlevel_wrapper,
    ._queue_send = queue_send_hlevel_wrapper,
    ._queue_send_from_isr = queue_send_from_isr_hlevel_wrapper,
    ._queue_recv = queue_recv_hlevel_wrapper,
    ._queue_recv_from_isr = queue_recv_from_isr_hlevel_wrapper,
#else
    ._queue_create = queue_create_wrapper,
    ._queue_delete = queue_delete_wrapper,
    ._queue_send = queue_send_wrapper,
    ._queue_send_from_isr = queue_send_from_isr_wrapper,
    ._queue_recv = queue_recv_wrapper,
    ._queue_recv_from_isr = queue_recv_from_isr_wrapper,
#endif /* CONFIG_BTDM_CTRL_HLI */
    ._task_create = task_create_wrapper,
    ._task_delete = task_delete_wrapper,
    ._is_in_isr = is_in_isr_wrapper,
    ._cause_sw_intr_to_core = cause_sw_intr_to_core_wrapper,
    ._malloc = malloc,
    ._malloc_internal = malloc_internal_wrapper,
    ._free = free,
    ._read_efuse_mac = read_mac_wrapper,
    ._srand = srand_wrapper,
    ._rand = rand_wrapper,
    ._btdm_lpcycles_2_us = btdm_lpcycles_2_us,
    ._btdm_us_2_lpcycles = btdm_us_2_lpcycles,
    ._btdm_sleep_check_duration = btdm_sleep_check_duration,
    ._btdm_sleep_enter_phase1 = btdm_sleep_enter_phase1_wrapper,
    ._btdm_sleep_enter_phase2 = btdm_sleep_enter_phase2_wrapper,
    ._btdm_sleep_exit_phase1 = NULL,
    ._btdm_sleep_exit_phase2 = NULL,
    ._btdm_sleep_exit_phase3 = btdm_sleep_exit_phase3_wrapper,
    ._coex_bt_wakeup_request = coex_bt_wakeup_request,
    ._coex_bt_wakeup_request_end = coex_bt_wakeup_request_end,
    ._coex_bt_request = coex_bt_request_wrapper,
    ._coex_bt_release = coex_bt_release_wrapper,
    ._coex_register_bt_cb = coex_register_bt_cb_wrapper,
    ._coex_bb_reset_lock = coex_bb_reset_lock_wrapper,
    ._coex_bb_reset_unlock = coex_bb_reset_unlock_wrapper,
    ._coex_schm_register_btdm_callback = coex_schm_register_btdm_callback_wrapper,
    ._coex_schm_status_bit_clear = coex_schm_status_bit_clear_wrapper,
    ._coex_schm_status_bit_set = coex_schm_status_bit_set_wrapper,
    ._coex_schm_interval_get = coex_schm_interval_get_wrapper,
    ._coex_schm_curr_period_get = coex_schm_curr_period_get_wrapper,
    ._coex_schm_curr_phase_get = coex_schm_curr_phase_get_wrapper,
    ._coex_wifi_channel_get = coex_wifi_channel_get_wrapper,
    ._coex_register_wifi_channel_change_callback = coex_register_wifi_channel_change_callback_wrapper,
    ._set_isr_l3 = xt_set_interrupt_handler,
    ._interrupt_l3_disable = interrupt_l3_disable,
    ._interrupt_l3_restore = interrupt_l3_restore,
#if CONFIG_BTDM_CTRL_HLI
    ._customer_queue_create = customer_queue_create_hlevel_wrapper,
#else
    ._customer_queue_create = NULL,
#endif /* CONFIG_BTDM_CTRL_HLI */
    ._coex_version_get = coex_version_get_wrapper,
    ._patch_apply = patch_apply,
    ._magic = OSI_MAGIC_VALUE,
};

/* the mode column will be modified by release function to indicate the available region */
static btdm_dram_available_region_t btdm_dram_available_region[] = {
    //following is .data
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_DATA_START,      SOC_MEM_BT_DATA_END         },
    //following is memory which HW will use
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_EM_BTDM0_START,  SOC_MEM_BT_EM_BTDM0_END     },
    {ESP_BT_MODE_BLE,           SOC_MEM_BT_EM_BLE_START,    SOC_MEM_BT_EM_BLE_END      },
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_EM_BTDM1_START,  SOC_MEM_BT_EM_BTDM1_END     },
    {ESP_BT_MODE_CLASSIC_BT,    SOC_MEM_BT_EM_BREDR_START,  SOC_MEM_BT_EM_BREDR_REAL_END},
    //following is .bss
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_BSS_START,       SOC_MEM_BT_BSS_END          },
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_MISC_START,      SOC_MEM_BT_MISC_END         },
};

/* Reserve the full memory region used by Bluetooth Controller,
 *    some may be released later at runtime. */
SOC_RESERVE_MEMORY_REGION(SOC_MEM_BT_EM_START,   SOC_MEM_BT_EM_BREDR_REAL_END,  rom_bt_em);
SOC_RESERVE_MEMORY_REGION(SOC_MEM_BT_BSS_START,  SOC_MEM_BT_BSS_END,            rom_bt_bss);
SOC_RESERVE_MEMORY_REGION(SOC_MEM_BT_MISC_START, SOC_MEM_BT_MISC_END,           rom_bt_misc);
SOC_RESERVE_MEMORY_REGION(SOC_MEM_BT_DATA_START, SOC_MEM_BT_DATA_END,           rom_bt_data);

static DRAM_ATTR struct osi_funcs_t *osi_funcs_p;

/* Static variable declare */
// timestamp when PHY/RF was switched on
static DRAM_ATTR int64_t s_time_phy_rf_just_enabled = 0;
static DRAM_ATTR esp_bt_controller_status_t btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

static DRAM_ATTR portMUX_TYPE global_int_mux = portMUX_INITIALIZER_UNLOCKED;

// measured average low power clock period in micro seconds
static DRAM_ATTR uint32_t btdm_lpcycle_us = 0;
static DRAM_ATTR uint8_t btdm_lpcycle_us_frac = 0; // number of fractional bit for btdm_lpcycle_us

#if CONFIG_BTDM_CTRL_MODEM_SLEEP_MODE_ORIG
// used low power clock
static DRAM_ATTR uint8_t btdm_lpclk_sel;
#endif /* #ifdef CONFIG_BTDM_CTRL_MODEM_SLEEP_MODE_ORIG */

static DRAM_ATTR QueueHandle_t s_wakeup_req_sem = NULL;
#ifdef CONFIG_PM_ENABLE
static DRAM_ATTR esp_timer_handle_t s_btdm_slp_tmr;
static DRAM_ATTR esp_pm_lock_handle_t s_pm_lock;
static bool s_pm_lock_acquired = true;
static DRAM_ATTR bool s_btdm_allow_light_sleep;
// pm_lock to prevent light sleep when using main crystal as Bluetooth low power clock
static DRAM_ATTR esp_pm_lock_handle_t s_light_sleep_pm_lock;
static void btdm_slp_tmr_callback(void *arg);
#endif /* #ifdef CONFIG_PM_ENABLE */


static inline void esp_bt_power_domain_on(void)
{
    // Bluetooth module power up
    esp_wifi_bt_power_domain_on();
}

static inline void esp_bt_power_domain_off(void)
{
    // Bluetooth module power down
    esp_wifi_bt_power_domain_off();
}

static inline void btdm_check_and_init_bb(void)
{
    /* init BT-BB if PHY/RF has been switched off since last BT-BB init */
    int64_t latest_ts = esp_phy_rf_get_on_ts();
    if (latest_ts != s_time_phy_rf_just_enabled ||
        s_time_phy_rf_just_enabled == 0) {
        btdm_rf_bb_init_phase2();
        s_time_phy_rf_just_enabled = latest_ts;
    }
}

#if CONFIG_BTDM_CTRL_HLI
struct interrupt_hlevel_cb{
    uint32_t status;
    uint8_t nested;
};

static DRAM_ATTR struct interrupt_hlevel_cb hli_cb = {
    .status = 0,
    .nested = 0,
};

static xt_handler set_isr_hlevel_wrapper(int mask, xt_handler f, void *arg)
{
    esp_err_t err = hli_intr_register((intr_handler_t) f, arg, DPORT_PRO_INTR_STATUS_0_REG, mask);
    if (err == ESP_OK) {
        return f;
    } else {
        return 0;
    }
 }

static void IRAM_ATTR interrupt_hlevel_disable(void)
{
    assert(xPortGetCoreID() == CONFIG_BTDM_CTRL_PINNED_TO_CORE);
    assert(hli_cb.nested != UCHAR_MAX);
    uint32_t status = hli_intr_disable();
    if (hli_cb.nested++ == 0) {
        hli_cb.status = status;
    }
}

static void IRAM_ATTR interrupt_hlevel_restore(void)
{
    assert(xPortGetCoreID() == CONFIG_BTDM_CTRL_PINNED_TO_CORE);
    assert(hli_cb.nested > 0);
    if (--hli_cb.nested == 0) {
        hli_intr_restore(hli_cb.status);
    }
}
#endif /* CONFIG_BTDM_CTRL_HLI */

static void IRAM_ATTR interrupt_l3_disable(void)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(&global_int_mux);
    } else {
        portENTER_CRITICAL(&global_int_mux);
    }
}

static void IRAM_ATTR interrupt_l3_restore(void)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(&global_int_mux);
    } else {
        portEXIT_CRITICAL(&global_int_mux);
    }
}

static void IRAM_ATTR task_yield(void)
{
    vPortYield();
}


static void IRAM_ATTR task_yield_from_isr(void)
{
    portYIELD_FROM_ISR();
}

static void *semphr_create_wrapper(uint32_t max, uint32_t init)
{
    btdm_queue_item_t *semphr = heap_caps_calloc(1, sizeof(btdm_queue_item_t), MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    assert(semphr);

    void *handle = NULL;

    /* IDF FreeRTOS guarantees that all dynamic memory allocation goes to internal RAM. */
    handle = (void *)xSemaphoreCreateCounting(max, init);
    assert(handle);

#if CONFIG_BTDM_CTRL_HLI
    SemaphoreHandle_t downstream_semaphore = handle;
    assert(downstream_semaphore);
    hli_queue_handle_t s_semaphore = hli_semaphore_create(max, downstream_semaphore);
    assert(s_semaphore);
    semphr->handle = (void *)s_semaphore;
#else
    semphr->handle =  handle;
#endif /* CONFIG_BTDM_CTRL_HLI */

    return semphr;
}

static void semphr_delete_wrapper(void *semphr)
{
    if (semphr == NULL) {
        return;
    }

    btdm_queue_item_t *semphr_item = (btdm_queue_item_t *)semphr;
    void *handle = NULL;
#if CONFIG_BTDM_CTRL_HLI
    if (semphr_item->handle) {
        handle = ((hli_queue_handle_t)(semphr_item->handle))->downstream;
        hli_queue_delete((hli_queue_handle_t)(semphr_item->handle));
    }
#else
    handle = semphr_item->handle;
#endif /* CONFIG_BTDM_CTRL_HLI */

    if (handle) {
        vSemaphoreDelete(handle);
    }

    free(semphr);
}

static int32_t IRAM_ATTR semphr_take_from_isr_wrapper(void *semphr, void *hptw)
{
#if CONFIG_BTDM_CTRL_HLI
    // Not support it
    assert(0);
    return 0;
#else
    void *handle = ((btdm_queue_item_t *)semphr)->handle;
    return (int32_t)xSemaphoreTakeFromISR(handle, hptw);
#endif /* CONFIG_BTDM_CTRL_HLI */
}

static int32_t IRAM_ATTR semphr_give_from_isr_wrapper(void *semphr, void *hptw)
{
    void *handle = ((btdm_queue_item_t *)semphr)->handle;
#if CONFIG_BTDM_CTRL_HLI
    UNUSED(hptw);
    assert(xPortGetCoreID() == CONFIG_BTDM_CTRL_PINNED_TO_CORE);
    return hli_semaphore_give(handle);
#else
    return (int32_t)xSemaphoreGiveFromISR(handle, hptw);
#endif /* CONFIG_BTDM_CTRL_HLI */
}

static int32_t semphr_take_wrapper(void *semphr, uint32_t block_time_ms)
{
    bool ret;
    void *handle = ((btdm_queue_item_t *)semphr)->handle;
#if CONFIG_BTDM_CTRL_HLI
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        ret = xSemaphoreTake(((hli_queue_handle_t)handle)->downstream, portMAX_DELAY);
    } else {
        ret = xSemaphoreTake(((hli_queue_handle_t)handle)->downstream, block_time_ms / portTICK_PERIOD_MS);
    }
#else
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        ret = xSemaphoreTake(handle, portMAX_DELAY);
    } else {
        ret = xSemaphoreTake(handle, block_time_ms / portTICK_PERIOD_MS);
    }
#endif /* CONFIG_BTDM_CTRL_HLI */
    return (int32_t)ret;
}

static int32_t semphr_give_wrapper(void *semphr)
{
    void *handle = ((btdm_queue_item_t *)semphr)->handle;
#if CONFIG_BTDM_CTRL_HLI
    return (int32_t)xSemaphoreGive(((hli_queue_handle_t)handle)->downstream);
#else
    return (int32_t)xSemaphoreGive(handle);
#endif /* CONFIG_BTDM_CTRL_HLI */
}

static void *mutex_create_wrapper(void)
{
    return (void *)xSemaphoreCreateMutex();
}

static void mutex_delete_wrapper(void *mutex)
{
    vSemaphoreDelete(mutex);
}

static int32_t mutex_lock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreTake(mutex, portMAX_DELAY);
}

static int32_t mutex_unlock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreGive(mutex);
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

#if CONFIG_BTDM_CTRL_HLI
static void *queue_create_hlevel_wrapper(uint32_t queue_len, uint32_t item_size)
{
    btdm_queue_item_t *queue_item = queue_create_wrapper(queue_len, item_size);
    assert(queue_item);
    QueueHandle_t downstream_queue = queue_item->handle;
    assert(queue_item->handle);
    hli_queue_handle_t queue = hli_queue_create(queue_len, item_size, downstream_queue);
    assert(queue);
    queue_item->handle = queue;
    return (void *)queue_item;
}

static void *customer_queue_create_hlevel_wrapper(uint32_t queue_len, uint32_t item_size)
{
    btdm_queue_item_t *queue_item = queue_create_wrapper(queue_len, item_size);
    assert(queue_item);
    QueueHandle_t downstream_queue = queue_item->handle;
    assert(queue_item->handle);
    hli_queue_handle_t queue = hli_customer_queue_create(queue_len, item_size, downstream_queue);
    assert(queue);
    queue_item->handle = queue;
    return (void *)queue_item;
}

static void queue_delete_hlevel_wrapper(void *queue)
{
    if (queue == NULL) {
        return;
    }

    btdm_queue_item_t *queue_item = (btdm_queue_item_t *)queue;

    if (queue_item->handle) {
        void *handle = ((hli_queue_handle_t)(queue_item->handle))->downstream;
        hli_queue_delete(queue_item->handle);
        queue_item->handle = handle;
        queue_delete_wrapper(queue_item);
    }
}

static int32_t queue_send_hlevel_wrapper(void *queue, void *item, uint32_t block_time_ms)
{
    void *handle = ((btdm_queue_item_t *)queue)->handle;
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xQueueSend(((hli_queue_handle_t)handle)->downstream, item, portMAX_DELAY);
    } else {
        return (int32_t)xQueueSend(((hli_queue_handle_t)handle)->downstream, item, block_time_ms / portTICK_PERIOD_MS);
    }
}

/**
 * Queue send from isr
 * @param  queue The queue which will send to
 * @param  item  The message which will be send
 * @param  hptw  need do task yield or not
 * @return       send success or not
 *               There is an issue here:  When the queue is full, it may return true but it send fail to the queue, sometimes.
 *               But in Bluetooth controller's isr, We don't care about the return value.
 *               It only required tp send success when the queue is empty all the time.
 *               So, this function meets the requirement.
 */
static int32_t IRAM_ATTR queue_send_from_isr_hlevel_wrapper(void *queue, void *item, void *hptw)
{
    UNUSED(hptw);
    assert(xPortGetCoreID() == CONFIG_BTDM_CTRL_PINNED_TO_CORE);
    void *handle = ((btdm_queue_item_t *)queue)->handle;
    return hli_queue_put(handle, item);
}

static int32_t queue_recv_hlevel_wrapper(void *queue, void *item, uint32_t block_time_ms)
{
    bool ret;
    void *handle = ((btdm_queue_item_t *)queue)->handle;
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        ret = xQueueReceive(((hli_queue_handle_t)handle)->downstream, item, portMAX_DELAY);
    } else {
        ret = xQueueReceive(((hli_queue_handle_t)handle)->downstream, item, block_time_ms / portTICK_PERIOD_MS);
    }

    return (int32_t)ret;
}

static int32_t IRAM_ATTR queue_recv_from_isr_hlevel_wrapper(void *queue, void *item, void *hptw)
{
    // Not support it
    assert(0);
    return 0;
}

#else

static int32_t queue_send_wrapper(void *queue, void *item, uint32_t block_time_ms)
{
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xQueueSend(((btdm_queue_item_t*)queue)->handle, item, portMAX_DELAY);
    } else {
        return (int32_t)xQueueSend(((btdm_queue_item_t*)queue)->handle, item, block_time_ms / portTICK_PERIOD_MS);
    }
}

static int32_t IRAM_ATTR queue_send_from_isr_wrapper(void *queue, void *item, void *hptw)
{
    return (int32_t)xQueueSendFromISR(((btdm_queue_item_t*)queue)->handle, item, hptw);
}

static int32_t queue_recv_wrapper(void *queue, void *item, uint32_t block_time_ms)
 {
    bool ret;
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        ret = xQueueReceive(((btdm_queue_item_t*)queue)->handle, item, portMAX_DELAY);
    } else {
        ret = xQueueReceive(((btdm_queue_item_t*)queue)->handle, item, block_time_ms / portTICK_PERIOD_MS);
    }

    return (int32_t)ret;
 }

static int32_t IRAM_ATTR queue_recv_from_isr_wrapper(void *queue, void *item, void *hptw)
{
    return (int32_t)xQueueReceiveFromISR(((btdm_queue_item_t*)queue)->handle, item, hptw);
}
#endif /* CONFIG_BTDM_CTRL_HLI */


static int32_t task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id)
{
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle, (core_id < CONFIG_FREERTOS_NUMBER_OF_CORES ? core_id : tskNO_AFFINITY));
}

static void task_delete_wrapper(void *task_handle)
{
    vTaskDelete(task_handle);
}

static bool IRAM_ATTR is_in_isr_wrapper(void)
{
    return !xPortCanYield();
}

static void IRAM_ATTR cause_sw_intr(void *arg)
{
    /* just convert void * to int, because the width is the same */
    uint32_t intr_no = (uint32_t)arg;
    XTHAL_SET_INTSET((1<<intr_no));
}

static int IRAM_ATTR cause_sw_intr_to_core_wrapper(int core_id, int intr_no)
{
    esp_err_t err = ESP_OK;

#if CONFIG_FREERTOS_UNICORE
    cause_sw_intr((void *)intr_no);
#else /* CONFIG_FREERTOS_UNICORE */
    if (xPortGetCoreID() == core_id) {
        cause_sw_intr((void *)intr_no);
    } else {
        err = esp_ipc_call(core_id, cause_sw_intr, (void *)intr_no);
    }
#endif /* !CONFIG_FREERTOS_UNICORE */
    return err;
}

static void *malloc_internal_wrapper(size_t size)
{
    return heap_caps_malloc(size, BLE_CONTROLLER_MALLOC_CAPS);
}

void *malloc_ble_controller_mem(size_t size)
{
    void *p = heap_caps_malloc(size, BLE_CONTROLLER_MALLOC_CAPS);
    if(p == NULL) {
        ESP_LOGE(BTDM_LOG_TAG, "Malloc failed");
    }
    return p;
}

uint32_t get_ble_controller_free_heap_size(void)
{
    return heap_caps_get_free_size(BLE_CONTROLLER_MALLOC_CAPS);
}

static int32_t IRAM_ATTR read_mac_wrapper(uint8_t mac[6])
{
    int ret = esp_read_mac(mac, ESP_MAC_BT);
    ESP_LOGI(BTDM_LOG_TAG, "Bluetooth MAC: %02x:%02x:%02x:%02x:%02x:%02x",
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

static uint32_t IRAM_ATTR btdm_lpcycles_2_us(uint32_t cycles)
{
    // The number of lp cycles should not lead to overflow. Thrs: 100s
    // clock measurement is conducted
    uint64_t us = (uint64_t)btdm_lpcycle_us * cycles;
    us = (us + (1 << (btdm_lpcycle_us_frac - 1))) >> btdm_lpcycle_us_frac;
    return (uint32_t)us;
}

/*
 * @brief Converts a duration in slots into a number of low power clock cycles.
 */
static uint32_t IRAM_ATTR btdm_us_2_lpcycles(uint32_t us)
{
    // The number of sleep duration(us) should not lead to overflow. Thrs: 100s
    // Compute the sleep duration in us to low power clock cycles, with calibration result applied
    // clock measurement is conducted
    uint64_t cycles = ((uint64_t)(us) << btdm_lpcycle_us_frac) / btdm_lpcycle_us;

    return (uint32_t)cycles;
}

static bool IRAM_ATTR btdm_sleep_check_duration(uint32_t *slot_cnt)
{
    if (*slot_cnt < BTDM_MIN_SLEEP_DURATION) {
        return false;
    }
    /* wake up in advance considering the delay in enabling PHY/RF */
    *slot_cnt -= BTDM_MODEM_WAKE_UP_DELAY;
    return true;
}

static void btdm_sleep_enter_phase1_wrapper(uint32_t lpcycles)
{
#ifdef CONFIG_PM_ENABLE
    // start a timer to wake up and acquire the pm_lock before modem_sleep awakes
    uint32_t us_to_sleep = btdm_lpcycles_2_us(lpcycles);

#define BTDM_MIN_TIMER_UNCERTAINTY_US      (500)
    assert(us_to_sleep > BTDM_MIN_TIMER_UNCERTAINTY_US);
    // allow a maximum time uncertainty to be about 488ppm(1/2048) at least as clock drift
    // and set the timer in advance
    uint32_t uncertainty = (us_to_sleep >> 11);
    if (uncertainty < BTDM_MIN_TIMER_UNCERTAINTY_US) {
        uncertainty = BTDM_MIN_TIMER_UNCERTAINTY_US;
    }

    if (esp_timer_start_once(s_btdm_slp_tmr, us_to_sleep - uncertainty) != ESP_OK) {
        ESP_LOGW(BTDM_LOG_TAG, "timer start failed");
    }
#endif
}

static void btdm_sleep_enter_phase2_wrapper(void)
{
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_phy_disable(PHY_MODEM_BT);
#ifdef CONFIG_PM_ENABLE
        if (s_pm_lock_acquired) {
            esp_pm_lock_release(s_pm_lock);
            s_pm_lock_acquired = false;
        }
#endif
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        esp_phy_disable(PHY_MODEM_BT);
        // pause bluetooth baseband
        periph_module_disable(PERIPH_BT_BASEBAND_MODULE);
    }
}

static void btdm_sleep_exit_phase3_wrapper(void)
{
#ifdef CONFIG_PM_ENABLE
    if (!s_pm_lock_acquired) {
        s_pm_lock_acquired = true;
        esp_pm_lock_acquire(s_pm_lock);
    }
#endif

    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_phy_enable(PHY_MODEM_BT);
        btdm_check_and_init_bb();
#ifdef CONFIG_PM_ENABLE
        esp_timer_stop(s_btdm_slp_tmr);
#endif
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        // resume bluetooth baseband
        periph_module_enable(PERIPH_BT_BASEBAND_MODULE);
        esp_phy_enable(PHY_MODEM_BT);
    }
}

#ifdef CONFIG_PM_ENABLE
static void btdm_slp_tmr_customer_callback(void * arg)
{
    (void)(arg);

    if (!s_pm_lock_acquired) {
        s_pm_lock_acquired = true;
        esp_pm_lock_acquire(s_pm_lock);
    }
}

static void IRAM_ATTR btdm_slp_tmr_callback(void *arg)
{
    (void)(arg);
    btdm_dispatch_work_to_controller(btdm_slp_tmr_customer_callback, NULL, true);
}
#endif

#define BTDM_ASYNC_WAKEUP_REQ_HCI       0
#define BTDM_ASYNC_WAKEUP_REQ_COEX      1
#define BTDM_ASYNC_WAKEUP_REQ_CTRL_DISA 2
#define BTDM_ASYNC_WAKEUP_REQMAX        3

static void btdm_wakeup_request_callback(void * arg)
{
    (void)(arg);

#if CONFIG_PM_ENABLE
    if (!s_pm_lock_acquired) {
        s_pm_lock_acquired = true;
        esp_pm_lock_acquire(s_pm_lock);
    }
    esp_timer_stop(s_btdm_slp_tmr);
#endif
    btdm_wakeup_request();

    semphr_give_wrapper(s_wakeup_req_sem);
}

static bool async_wakeup_request(int event)
{
    bool do_wakeup_request = false;

    switch (event) {
        case BTDM_ASYNC_WAKEUP_REQ_HCI:
            btdm_in_wakeup_requesting_set(true);
            // NO break
        case BTDM_ASYNC_WAKEUP_REQ_CTRL_DISA:
            if (!btdm_power_state_active()) {
                do_wakeup_request = true;

                btdm_dispatch_work_to_controller(btdm_wakeup_request_callback, NULL, true);
                semphr_take_wrapper(s_wakeup_req_sem, OSI_FUNCS_TIME_BLOCKING);
            }
            break;
        case BTDM_ASYNC_WAKEUP_REQ_COEX:
            if (!btdm_power_state_active()) {
                do_wakeup_request = true;
#if CONFIG_PM_ENABLE
                if (!s_pm_lock_acquired) {
                    s_pm_lock_acquired = true;
                    esp_pm_lock_acquire(s_pm_lock);
                }
                esp_timer_stop(s_btdm_slp_tmr);
#endif
                btdm_wakeup_request();
            }
            break;
        default:
            return false;
    }

    return do_wakeup_request;
}

static void async_wakeup_request_end(int event)
{
    bool request_lock = false;
    switch (event) {
        case BTDM_ASYNC_WAKEUP_REQ_HCI:
            request_lock = true;
            break;
        case BTDM_ASYNC_WAKEUP_REQ_COEX:
        case BTDM_ASYNC_WAKEUP_REQ_CTRL_DISA:
            request_lock = false;
            break;
        default:
            return;
    }

    if (request_lock) {
        btdm_in_wakeup_requesting_set(false);
    }

    return;
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

static int IRAM_ATTR coex_bt_request_wrapper(uint32_t event, uint32_t latency, uint32_t duration)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_bt_request(event, latency, duration);
#else
    return 0;
#endif
}

static int IRAM_ATTR coex_bt_release_wrapper(uint32_t event)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_bt_release(event);
#else
    return 0;
#endif
}

static int coex_register_bt_cb_wrapper(coex_func_cb_t cb)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_register_bt_cb(cb);
#else
    return 0;
#endif
}

static uint32_t IRAM_ATTR coex_bb_reset_lock_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_bb_reset_lock();
#else
    return 0;
#endif
}

static void IRAM_ATTR coex_bb_reset_unlock_wrapper(uint32_t restore)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_bb_reset_unlock(restore);
#endif
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

static int coex_wifi_channel_get_wrapper(uint8_t *primary, uint8_t *secondary)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_wifi_channel_get(primary, secondary);
#else
    return -1;
#endif
}

static int coex_register_wifi_channel_change_callback_wrapper(void *cb)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_register_wifi_channel_change_callback(cb);
#else
    return -1;
#endif
}

static int coex_version_get_wrapper(unsigned int *major, unsigned int *minor, unsigned int *patch)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_version_t version;
    ESP_ERROR_CHECK(coex_version_get_value(&version));
    *major = (unsigned int)version.major;
    *minor = (unsigned int)version.minor;
    *patch = (unsigned int)version.patch;
    return 0;
#endif
    return -1;
}

bool esp_vhci_host_check_send_available(void)
{
    return API_vhci_host_check_send_available();
}

void esp_vhci_host_send_packet(uint8_t *data, uint16_t len)
{
    async_wakeup_request(BTDM_ASYNC_WAKEUP_REQ_HCI);

    API_vhci_host_send_packet(data, len);

    async_wakeup_request_end(BTDM_ASYNC_WAKEUP_REQ_HCI);
}

esp_err_t esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback)
{
    return API_vhci_host_register_callback((const vhci_host_callback_t *)callback) == 0 ? ESP_OK : ESP_FAIL;
}

static uint32_t btdm_config_mask_load(void)
{
    uint32_t mask = 0x0;

#if CONFIG_BTDM_CTRL_HCI_MODE_UART_H4
    mask |= BTDM_CFG_HCI_UART;
#endif
#if CONFIG_BTDM_CTRL_PINNED_TO_CORE == 1
    mask |= BTDM_CFG_CONTROLLER_RUN_APP_CPU;
#endif
#if CONFIG_BTDM_CTRL_FULL_SCAN_SUPPORTED
    mask |= BTDM_CFG_BLE_FULL_SCAN_SUPPORTED;
#endif /* CONFIG_BTDM_CTRL_FULL_SCAN_SUPPORTED */
    mask |= BTDM_CFG_SCAN_DUPLICATE_OPTIONS;

    mask |= BTDM_CFG_SEND_ADV_RESERVED_SIZE;

    return mask;
}

static void btdm_controller_mem_init(void)
{
    /* initialise .data section */
    memcpy(&_data_start_btdm, (void *)_data_start_btdm_rom, &_data_end_btdm - &_data_start_btdm);
    ESP_LOGD(BTDM_LOG_TAG, ".data initialise [0x%08x] <== [0x%08x]", (uint32_t)&_data_start_btdm, _data_start_btdm_rom);

    //initial em, .bss section
    for (int i = 1; i < sizeof(btdm_dram_available_region)/sizeof(btdm_dram_available_region_t); i++) {
        if (btdm_dram_available_region[i].mode != ESP_BT_MODE_IDLE) {
            memset((void *)btdm_dram_available_region[i].start, 0x0, btdm_dram_available_region[i].end - btdm_dram_available_region[i].start);
            ESP_LOGD(BTDM_LOG_TAG, ".bss initialise [0x%08x] - [0x%08x]", btdm_dram_available_region[i].start, btdm_dram_available_region[i].end);
        }
    }
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
        ESP_LOGD(BTDM_LOG_TAG, "Release %s [0x%08x] - [0x%08x], len %d", area->name, mem_start, mem_end, mem_end - mem_start);
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

static esp_err_t esp_bt_controller_rom_mem_release(esp_bt_mode_t mode)
{
    bool update = true;
    intptr_t mem_start=(intptr_t) NULL, mem_end=(intptr_t) NULL;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    //already released
    if (!(mode & btdm_dram_available_region[0].mode)) {
        return ESP_ERR_INVALID_STATE;
    }

    for (int i = 0; i < sizeof(btdm_dram_available_region)/sizeof(btdm_dram_available_region_t); i++) {
        //skip the share mode, idle mode and other mode
        if (btdm_dram_available_region[i].mode == ESP_BT_MODE_IDLE
                || (mode & btdm_dram_available_region[i].mode) != btdm_dram_available_region[i].mode) {
            //clear the bit of the mode which will be released
            btdm_dram_available_region[i].mode &= ~mode;
            continue;
        } else {
            //clear the bit of the mode which will be released
            btdm_dram_available_region[i].mode &= ~mode;
        }

        if (update) {
            mem_start = btdm_dram_available_region[i].start;
            mem_end = btdm_dram_available_region[i].end;
            update = false;
        }

        if (i < sizeof(btdm_dram_available_region)/sizeof(btdm_dram_available_region_t) - 1) {
            mem_end = btdm_dram_available_region[i].end;
            if (btdm_dram_available_region[i+1].mode != ESP_BT_MODE_IDLE
                    && (mode & btdm_dram_available_region[i+1].mode) == btdm_dram_available_region[i+1].mode
                    && mem_end == btdm_dram_available_region[i+1].start) {
                continue;
            } else {
                ESP_LOGD(BTDM_LOG_TAG, "Release DRAM [0x%08x] - [0x%08x]", mem_start, mem_end);
                ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
                update = true;
            }
        } else {
            mem_end = btdm_dram_available_region[i].end;
            ESP_LOGD(BTDM_LOG_TAG, "Release DRAM [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
            update = true;
        }
    }

    return ESP_OK;
}

esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode)
{
    esp_err_t ret = ESP_OK;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    if (mode == ESP_BT_MODE_BTDM) {
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

        ret = esp_bt_mem_release_areas(&cont_data, &cont_bss);
    }

    if (ret == ESP_OK) {
        ret = esp_bt_controller_rom_mem_release(mode);
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

    /*
     * Free data and BSS section for Bluetooth controller ROM code.
     * Note that rom mem release must be performed before section _bt_data_start to _bt_data_end is released,
     * otherwise `btdm_dram_available_region` will no longer be available when performing rom mem release and
     * thus causing heap corruption.
     */
    ret = esp_bt_controller_rom_mem_release(mode);

    if (mode == ESP_BT_MODE_BTDM) {
        /* Start by freeing Bluetooth BSS section */
        if (ret == ESP_OK) {
            ret = esp_bt_mem_release_areas(&bss, &cont_bss);
        }

        /* Do the same thing with the Bluetooth data section */
        if (ret == ESP_OK) {
            ret = esp_bt_mem_release_areas(&data, &cont_data);
        }
    }

    return ret;
}

#if CONFIG_BTDM_CTRL_HLI
static void hli_queue_setup_cb(void* arg)
{
    hli_queue_setup();
}

static void hli_queue_setup_pinned_to_core(int core_id)
{
#if CONFIG_FREERTOS_UNICORE
    hli_queue_setup_cb(NULL);
#else /* CONFIG_FREERTOS_UNICORE */
    if (xPortGetCoreID() == core_id) {
        hli_queue_setup_cb(NULL);
    } else {
        esp_ipc_call(core_id, hli_queue_setup_cb, NULL);
    }
#endif /* !CONFIG_FREERTOS_UNICORE */
}
#endif /* CONFIG_BTDM_CTRL_HLI */

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    esp_err_t err;
    uint32_t btdm_cfg_mask = 0;

#if CONFIG_BTDM_CTRL_HLI
    hli_queue_setup_pinned_to_core(CONFIG_BTDM_CTRL_PINNED_TO_CORE);
#endif /* CONFIG_BTDM_CTRL_HLI */

    //if all the bt available memory was already released, cannot initialize bluetooth controller
    if (btdm_dram_available_region[0].mode == ESP_BT_MODE_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    osi_funcs_p = (struct osi_funcs_t *)malloc_internal_wrapper(sizeof(struct osi_funcs_t));
    if (osi_funcs_p == NULL) {
        return ESP_ERR_NO_MEM;
    }

    memcpy(osi_funcs_p, &osi_funcs_ro, sizeof(struct osi_funcs_t));
    if (btdm_osi_funcs_register(osi_funcs_p) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (cfg->controller_task_prio != ESP_TASK_BT_CONTROLLER_PRIO
            || cfg->controller_task_stack_size < ESP_TASK_BT_CONTROLLER_STACK) {
        return ESP_ERR_INVALID_ARG;
    }

    //overwrite some parameters
    cfg->bt_max_sync_conn = CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN_EFF;
    cfg->magic  = ESP_BT_CONTROLLER_CONFIG_MAGIC_VAL;

    if (((cfg->mode & ESP_BT_MODE_BLE) && (cfg->ble_max_conn <= 0 || cfg->ble_max_conn > BTDM_CONTROLLER_BLE_MAX_CONN_LIMIT))
            || ((cfg->mode & ESP_BT_MODE_CLASSIC_BT) && (cfg->bt_max_acl_conn <= 0 || cfg->bt_max_acl_conn > BTDM_CONTROLLER_BR_EDR_MAX_ACL_CONN_LIMIT))
            || ((cfg->mode & ESP_BT_MODE_CLASSIC_BT) && (cfg->bt_max_sync_conn > BTDM_CONTROLLER_BR_EDR_MAX_SYNC_CONN_LIMIT))) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(BTDM_LOG_TAG, "BT controller compile version [%s]", btdm_controller_get_compile_version());

    s_wakeup_req_sem = semphr_create_wrapper(1, 0);
    if (s_wakeup_req_sem == NULL) {
        err = ESP_ERR_NO_MEM;
        goto error;
    }

    esp_phy_modem_init();

    esp_bt_power_domain_on();

    btdm_controller_mem_init();

    periph_module_enable(PERIPH_BT_MODULE);

#ifdef CONFIG_PM_ENABLE
    s_btdm_allow_light_sleep = false;
#endif

    // set default sleep clock cycle and its fractional bits
    btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
    btdm_lpcycle_us = 2 << (btdm_lpcycle_us_frac);

#if CONFIG_BTDM_CTRL_MODEM_SLEEP_MODE_ORIG

    btdm_lpclk_sel = BTDM_LPCLK_SEL_XTAL; // set default value
#if CONFIG_BTDM_CTRL_LPCLK_SEL_EXT_32K_XTAL
    // check whether or not EXT_CRYS is working
    if (rtc_clk_slow_src_get() == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        btdm_lpclk_sel = BTDM_LPCLK_SEL_XTAL32K; // External 32kHz XTAL
#ifdef CONFIG_PM_ENABLE
        s_btdm_allow_light_sleep = true;
#endif
    } else {
        ESP_LOGW(BTDM_LOG_TAG, "32.768kHz XTAL not detected, fall back to main XTAL as Bluetooth sleep clock\n"
                 "light sleep mode will not be able to apply when bluetooth is enabled");
        btdm_lpclk_sel = BTDM_LPCLK_SEL_XTAL; // set default value
    }
#else
    btdm_lpclk_sel = BTDM_LPCLK_SEL_XTAL; // set default value
#endif

    bool select_src_ret __attribute__((unused));
    bool set_div_ret __attribute__((unused));
    if (btdm_lpclk_sel == BTDM_LPCLK_SEL_XTAL) {
        select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL);
        set_div_ret = btdm_lpclk_set_div(esp_clk_xtal_freq() * 2 / MHZ - 1);
        assert(select_src_ret && set_div_ret);
        btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
        btdm_lpcycle_us = 2 << (btdm_lpcycle_us_frac);
    } else { // btdm_lpclk_sel == BTDM_LPCLK_SEL_XTAL32K
        select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL32K);
        set_div_ret = btdm_lpclk_set_div(0);
        assert(select_src_ret && set_div_ret);
        btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
        btdm_lpcycle_us = (RTC_CLK_CAL_FRACT > 15) ? (1000000 << (RTC_CLK_CAL_FRACT - 15)) :
            (1000000 >> (15 - RTC_CLK_CAL_FRACT));
        assert(btdm_lpcycle_us != 0);
    }
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_ORIG);

#elif CONFIG_BTDM_CTRL_MODEM_SLEEP_MODE_EVED
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_EVED);
#else
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_NONE);
#endif

#if CONFIG_BTDM_CTRL_HCI_UART_FLOW_CTRL_EN
    sdk_config_set_uart_flow_ctrl_enable(true);
#else
    sdk_config_set_uart_flow_ctrl_enable(false);
#endif

#ifdef CONFIG_PM_ENABLE
    if (!s_btdm_allow_light_sleep) {
        if ((err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "btLS", &s_light_sleep_pm_lock)) != ESP_OK) {
            goto error;
        }
    }
    if ((err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "bt", &s_pm_lock)) != ESP_OK) {
        goto error;
    }
    esp_timer_create_args_t create_args = {
        .callback = btdm_slp_tmr_callback,
        .arg = NULL,
        .name = "btSlp"
    };
    if ((err = esp_timer_create(&create_args, &s_btdm_slp_tmr)) != ESP_OK) {
        goto error;
    }

    s_pm_lock_acquired = true;
#endif

#if CONFIG_SW_COEXIST_ENABLE
    coex_init();
#endif

    btdm_cfg_mask = btdm_config_mask_load();

    err = btdm_controller_init(btdm_cfg_mask, cfg);

    if (err != 0) {
        ESP_LOGE(BTDM_LOG_TAG, "%s %d\n",__func__,err);
        err = ESP_ERR_NO_MEM;
        goto error;
    }

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    return ESP_OK;

error:

    bt_controller_deinit_internal();

    return err;
}

esp_err_t esp_bt_controller_deinit(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

    btdm_controller_deinit();

    bt_controller_deinit_internal();

    return ESP_OK;
}

static void bt_controller_deinit_internal(void)
{
    periph_module_disable(PERIPH_BT_MODULE);

#ifdef CONFIG_PM_ENABLE
    if (!s_btdm_allow_light_sleep) {
        esp_pm_lock_delete(s_light_sleep_pm_lock);
        s_light_sleep_pm_lock = NULL;
    }

    if (s_pm_lock != NULL) {
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
    }

    if (s_btdm_slp_tmr != NULL) {
        esp_timer_stop(s_btdm_slp_tmr);
        esp_timer_delete(s_btdm_slp_tmr);
        s_btdm_slp_tmr = NULL;
    }

    s_pm_lock_acquired = false;
#endif

    if (s_wakeup_req_sem) {
        semphr_delete_wrapper(s_wakeup_req_sem);
        s_wakeup_req_sem = NULL;
    }

    if (osi_funcs_p) {
        free(osi_funcs_p);
        osi_funcs_p = NULL;
    }

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

    btdm_lpcycle_us = 0;
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_NONE);

    esp_bt_power_domain_off();

    esp_phy_modem_deinit();
}

static void bt_controller_shutdown(void* arg)
{
    esp_bt_controller_shutdown();
}

static void bt_shutdown(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return;
    }
#if !CONFIG_FREERTOS_UNICORE
    esp_ipc_call_blocking(CONFIG_BTDM_CTRL_PINNED_TO_CORE, bt_controller_shutdown, NULL);
#else
    bt_controller_shutdown(NULL);
#endif
    esp_phy_disable(PHY_MODEM_BT);

    return;
}

static void patch_apply(void)
{
    config_btdm_funcs_reset();

#ifndef CONFIG_BTDM_CTRL_MODE_BLE_ONLY
    config_bt_funcs_reset();
#endif

#ifndef CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY
    config_ble_funcs_reset();
#endif
}

esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode)
{
    int ret;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

    //As the history reason, mode should be equal to the mode which set in esp_bt_controller_init()
    if (mode != btdm_controller_get_mode()) {
        return ESP_ERR_INVALID_ARG;
    }

#ifdef CONFIG_PM_ENABLE
    if (!s_btdm_allow_light_sleep) {
        esp_pm_lock_acquire(s_light_sleep_pm_lock);
    }
    esp_pm_lock_acquire(s_pm_lock);
#endif

    esp_phy_enable(PHY_MODEM_BT);

#if CONFIG_SW_COEXIST_ENABLE
    coex_enable();
#endif

    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        btdm_controller_enable_sleep(true);
    }

    sdk_config_set_bt_pll_track_enable(true);

    // initialize bluetooth baseband
    btdm_check_and_init_bb();

    ret = btdm_controller_enable(mode);
    if (ret != 0) {
#if CONFIG_SW_COEXIST_ENABLE
        coex_disable();
#endif
        esp_phy_disable(PHY_MODEM_BT);
#ifdef CONFIG_PM_ENABLE
        if (!s_btdm_allow_light_sleep) {
            esp_pm_lock_release(s_light_sleep_pm_lock);
        }
        esp_pm_lock_release(s_pm_lock);
#endif
        return ESP_ERR_INVALID_STATE;
    }

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;
    ret = esp_register_shutdown_handler(bt_shutdown);
    if (ret != ESP_OK) {
        ESP_LOGW(BTDM_LOG_TAG, "Register shutdown handler failed, ret = 0x%x", ret);
    }

    return ESP_OK;
}

esp_err_t esp_bt_controller_disable(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    // disable modem sleep and wake up from sleep mode
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        btdm_controller_enable_sleep(false);
        async_wakeup_request(BTDM_ASYNC_WAKEUP_REQ_CTRL_DISA);
        while (!btdm_power_state_active()) {
            esp_rom_delay_us(1000);
        }
    }

    btdm_controller_disable();

#if CONFIG_SW_COEXIST_ENABLE
    coex_disable();
#endif

    esp_phy_disable(PHY_MODEM_BT);
    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    esp_unregister_shutdown_handler(bt_shutdown);

#ifdef CONFIG_PM_ENABLE
    if (!s_btdm_allow_light_sleep) {
        esp_pm_lock_release(s_light_sleep_pm_lock);
    }
    esp_pm_lock_release(s_pm_lock);
#endif

    return ESP_OK;
}

esp_bt_controller_status_t esp_bt_controller_get_status(void)
{
    return btdm_controller_status;
}

/* extra functions */
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level)
{
    if (ble_txpwr_set(power_type, power_level) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type)
{
    return (esp_power_level_t)ble_txpwr_get(power_type);
}

esp_err_t esp_bredr_tx_power_set(esp_power_level_t min_power_level, esp_power_level_t max_power_level)
{
    esp_err_t err;
    int ret;

    ret = bredr_txpwr_set(min_power_level, max_power_level);

    if (ret == 0) {
        err = ESP_OK;
    } else if (ret == -1) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        err = ESP_ERR_INVALID_STATE;
    }

    return err;
}

esp_err_t esp_bredr_tx_power_get(esp_power_level_t *min_power_level, esp_power_level_t *max_power_level)
{
    if (bredr_txpwr_get((int *)min_power_level, (int *)max_power_level) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t esp_bt_sleep_enable (void)
{
    esp_err_t status;
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG ||
            btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
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
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG ||
            btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        btdm_controller_enable_sleep (false);
        status = ESP_OK;
    } else {
        status = ESP_ERR_NOT_SUPPORTED;
    }

    return status;
}

esp_err_t esp_bredr_sco_datapath_set(esp_sco_data_path_t data_path)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    bredr_sco_datapath_set(data_path);
    return ESP_OK;
}

esp_err_t esp_ble_scan_dupilcate_list_flush(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btdm_controller_scan_duplicate_list_clear();
    return ESP_OK;
}

/**
 * This function re-write controller's function,
 * As coredump can not show parameters in function which is in a .a file.
 *
 * After coredump fixing this issue, just delete this function.
 */
void IRAM_ATTR r_assert(const char *condition, int param0, int param1, const char *file, int line)
{
    __asm__ __volatile__("ill\n");
}

#endif /*  CONFIG_BT_ENABLED */
