/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "esp_random.h"
#include "esp_heap_caps.h"
#include "esp_heap_caps_init.h"
#include "esp_mac.h"

#include "sdkconfig.h"

#if CONFIG_BT_NIMBLE_ENABLED
#include "nimble/nimble_port.h"
#endif // CONFIG_BT_NIMBLE_ENABLED
#include "nimble/nimble_port_freertos.h"
#include "esp_private/esp_modem_clock.h"

#ifdef ESP_PLATFORM
#include "esp_log.h"
#endif // ESP_PLATFORM

#if CONFIG_SW_COEXIST_ENABLE
#include "private/esp_coexist_internal.h"
#endif // CONFIG_SW_COEXIST_ENABLE

#include "nimble/nimble_npl_os.h"
#include "esp_hci_transport.h"
#include "os/endian.h"

#include "esp_bt.h"
#include "ble_priv.h"
#include "esp_intr_alloc.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp_phy_init.h"
#include "esp_private/periph_ctrl.h"
#include "bt_osi_mem.h"

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#include "esp_private/sleep_modem.h"
#include "esp_private/sleep_retention.h"
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_sleep.h"
#include "soc/rtc.h"

#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
#include "ble_log/ble_log_spi_out.h"
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

/* Macro definition
 ************************************************************************
 */
#define NIMBLE_PORT_LOG_TAG          "BLE_INIT"
#define OSI_COEX_VERSION              0x00010006
#define OSI_COEX_MAGIC_VALUE          0xFADEBEAD

#define EXT_FUNC_VERSION             0x20250415
#define EXT_FUNC_MAGIC_VALUE         0xA5A5A5A5

#define BT_ASSERT_PRINT              ets_printf

/* Types definition
 ************************************************************************
 */
struct osi_coex_funcs_t {
    uint32_t _magic;
    uint32_t _version;
    void (* _coex_wifi_sleep_set)(bool sleep);
    int (* _coex_core_ble_conn_dyn_prio_get)(bool *low, bool *high);
    void (* _coex_schm_status_bit_set)(uint32_t type, uint32_t status);
    void (* _coex_schm_status_bit_clear)(uint32_t type, uint32_t status);
};

struct ext_funcs_t {
    uint32_t ext_version;
    int (*_esp_intr_alloc)(int source, int flags, intr_handler_t handler, void *arg, void **ret_handle);
    int (*_esp_intr_free)(void **ret_handle);
    void *(* _malloc)(size_t size);
    void (*_free)(void *p);
    int (* _task_create)(void *task_func, const char *name, uint32_t stack_depth, void *param,
                         uint32_t prio, void *task_handle, uint32_t core_id);
    void (* _task_delete)(void *task_handle);
    void (*_osi_assert)(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
    uint32_t (* _os_random)(void);
    int (* _ecc_gen_key_pair)(uint8_t *public, uint8_t *priv);
    int (* _ecc_gen_dh_key)(const uint8_t *remote_pub_key_x, const uint8_t *remote_pub_key_y,
                            const uint8_t *local_priv_key, uint8_t *dhkey);
    uint32_t magic;
};

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
typedef void (*interface_func_t) (uint32_t len, const uint8_t *addr, uint32_t len_append, const uint8_t *addr_append, uint32_t flag);

enum {
    BLE_LOG_INTERFACE_FLAG_CONTINUE = 0,
    BLE_LOG_INTERFACE_FLAG_END,
};
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
/* External functions or variables
 ************************************************************************
 */
extern int ble_osi_coex_funcs_register(struct osi_coex_funcs_t *coex_funcs);
extern int r_ble_controller_init(esp_bt_controller_config_t *cfg);
extern void esp_ble_controller_info_capture(uint32_t cycle_times);
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
extern int r_ble_log_init_async(interface_func_t interface, bool task_create, uint8_t buffers, uint32_t *bufs_size);
extern int r_ble_log_deinit_async(void);
extern int r_ble_log_init_simple(interface_func_t interface, void *handler);
extern void r_ble_log_deinit_simple(void);
extern void r_ble_log_async_select_dump_buffers(uint8_t buffers);
extern void r_ble_log_async_output_dump_all(bool output);
extern void esp_panic_handler_feed_wdts(void);
extern int r_ble_log_ctrl_level_and_mod(uint8_t log_level, uint32_t mod_switch);
extern int r_ble_ctrl_mod_type(uint16_t mod, uint32_t mod_type_switch);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
extern int r_ble_controller_deinit(void);
extern int r_ble_controller_enable(uint8_t mode);
extern int r_ble_controller_disable(void);
extern int esp_register_ext_funcs (struct ext_funcs_t *);
extern void esp_unregister_ext_funcs (void);
extern int r_esp_ble_ll_set_public_addr(const uint8_t *addr);
extern int esp_register_npl_funcs (struct npl_funcs_t *p_npl_func);
extern void esp_unregister_npl_funcs (void);
extern void npl_freertos_mempool_deinit(void);
extern uint32_t r_os_cputime_get32(void);
extern uint32_t r_os_cputime_ticks_to_usecs(uint32_t ticks);
extern void r_ble_lll_rfmgmt_set_sleep_cb(void *s_cb, void *w_cb, void *s_arg,
                                          void *w_arg, uint32_t us_to_enabled);
extern void r_ble_rtc_wake_up_state_clr(void);
extern int os_msys_init(void);
extern void os_msys_deinit(void);
#if CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY
extern void r_ble_ll_scan_start_time_init_compensation(uint32_t init_compensation);
extern void r_priv_sdk_config_insert_proc_time_set(uint16_t insert_proc_time);
#endif // CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
extern sleep_retention_entries_config_t *r_esp_ble_mac_retention_link_get(uint8_t *size, uint8_t extra);
extern void r_esp_ble_set_wakeup_overhead(uint32_t overhead);
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
#if CONFIG_PM_ENABLE
extern void r_esp_ble_stop_wakeup_timing(void);
#endif // CONFIG_PM_ENABLE
#if CONFIG_BT_LE_LL_PEER_SCA_SET_ENABLE
extern void r_ble_ll_customize_peer_sca_set(uint16_t peer_sca);
#endif  // CONFIG_BT_LE_LL_PEER_SCA_SET_ENABLE
extern int ble_sm_alg_gen_dhkey(const uint8_t *peer_pub_key_x,
                                const uint8_t *peer_pub_key_y,
                                const uint8_t *our_priv_key, uint8_t *out_dhkey);
extern int ble_sm_alg_gen_key_pair(uint8_t *pub, uint8_t *priv);
extern int r_ble_txpwr_set(esp_ble_enhanced_power_type_t power_type, uint16_t handle, int power_level);
extern int r_ble_txpwr_get(esp_ble_enhanced_power_type_t power_type, uint16_t handle);
extern int r_ble_get_npl_element_info(esp_bt_controller_config_t *cfg, ble_npl_count_info_t * npl_info);
extern char *ble_controller_get_compile_version(void);
extern int esp_ble_register_bb_funcs(void);
extern void esp_ble_unregister_bb_funcs(void);
extern uint32_t _bt_bss_start;
extern uint32_t _bt_bss_end;
extern uint32_t _bt_controller_bss_start;
extern uint32_t _bt_controller_bss_end;
extern uint32_t _bt_data_start;
extern uint32_t _bt_data_end;
extern uint32_t _bt_controller_data_start;
extern uint32_t _bt_controller_data_end;

/* Local Function Declaration
 *********************************************************************
 */
static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status);
static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status);
static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth,
                               void *param, uint32_t prio, void *task_handle, uint32_t core_id);
static void task_delete_wrapper(void *task_handle);
static int esp_intr_alloc_wrapper(int source, int flags, intr_handler_t handler,
                                  void *arg, void **ret_handle_in);
static int esp_intr_free_wrapper(void **ret_handle);
static void osi_assert_wrapper(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
static uint32_t osi_random_wrapper(void);
static int esp_ecc_gen_key_pair(uint8_t *pub, uint8_t *priv);
static int esp_ecc_gen_dh_key(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                              const uint8_t *our_priv_key, uint8_t *out_dhkey);
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
#if !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
static void esp_bt_controller_log_interface(uint32_t len, const uint8_t *addr, uint32_t len_append, const uint8_t *addr_append, uint32_t flag);
#endif // !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
#if CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
static void esp_bt_ctrl_log_partition_get_and_erase_first_block(void);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
/* Local variable definition
 ***************************************************************************
 */
/* Static variable declare */
static DRAM_ATTR esp_bt_controller_status_t ble_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
const static uint32_t log_bufs_size[] = {CONFIG_BT_LE_LOG_CTRL_BUF1_SIZE, CONFIG_BT_LE_LOG_HCI_BUF_SIZE, CONFIG_BT_LE_LOG_CTRL_BUF2_SIZE};
static bool log_is_inited = false;

esp_err_t esp_bt_controller_log_init(void)
{
    if (log_is_inited) {
        return ESP_OK;
    }

#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    if (ble_log_spi_out_init() != 0) {
        goto spi_out_init_failed;
    }
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

#if CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    if (r_ble_log_init_simple(ble_log_spi_out_ll_write, ble_log_spi_out_ll_log_ev_proc) != 0) {
        goto log_init_failed;
    }
#else // !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    uint8_t buffers = 0;
#if CONFIG_BT_LE_CONTROLLER_LOG_CTRL_ENABLED
    buffers |= ESP_BLE_LOG_BUF_CONTROLLER;
#endif // CONFIG_BT_LE_CONTROLLER_LOG_CTRL_ENABLED
#if CONFIG_BT_LE_CONTROLLER_LOG_HCI_ENABLED
    buffers |= ESP_BLE_LOG_BUF_HCI;
#endif // CONFIG_BT_LE_CONTROLLER_LOG_HCI_ENABLED

    bool task_create = true;
#if CONFIG_BT_LE_CONTROLLER_LOG_DUMP_ONLY
    task_create = false;
#elif CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
    esp_bt_ctrl_log_partition_get_and_erase_first_block();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED

    if (r_ble_log_init_async(esp_bt_controller_log_interface, task_create, buffers, (uint32_t *)log_bufs_size) != 0) {
        goto log_init_failed;
    }
#endif // CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED

    if (r_ble_log_ctrl_level_and_mod(CONFIG_BT_LE_CONTROLLER_LOG_OUTPUT_LEVEL, CONFIG_BT_LE_CONTROLLER_LOG_MOD_OUTPUT_SWITCH) != ESP_OK) {
        goto ctrl_level_init_failed;
    }
    log_is_inited = true;
    return ESP_OK;

ctrl_level_init_failed:
#if CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    r_ble_log_deinit_simple();
#else // !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    r_ble_log_deinit_async();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
log_init_failed:
#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    ble_log_spi_out_deinit();
spi_out_init_failed:
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    return ESP_FAIL;
}

void esp_bt_controller_log_deinit(void)
{
#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    ble_log_spi_out_deinit();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

#if CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    r_ble_log_deinit_simple();
#else // !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    r_ble_log_deinit_async();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED

    log_is_inited = false;
}

#if CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
#include "esp_partition.h"
#include "hal/wdt_hal.h"

#define MAX_STORAGE_SIZE          (CONFIG_BT_LE_CONTROLLER_LOG_PARTITION_SIZE)
#define BLOCK_SIZE                (4096)
#define THRESHOLD                 (3072)
#define PARTITION_NAME            "bt_ctrl_log"

static const esp_partition_t *log_partition;
static uint32_t write_index = 0;
static uint32_t next_erase_index = BLOCK_SIZE;
static bool block_erased = false;
static bool stop_write = false;
static bool is_filled = false;

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
    esp_bt_controller_log_deinit();
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
    err = esp_bt_controller_log_init();
    assert(err == ESP_OK);
}
#endif // CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE

#if CONFIG_BT_LE_CONTROLLER_LOG_TASK_WDT_USER_HANDLER_ENABLE
void esp_task_wdt_isr_user_handler(void)
{
    esp_ble_controller_log_dump_all(true);
}
#endif // CONFIG_BT_LE_CONTROLLER_LOG_TASK_WDT_USER_HANDLER_ENABLE

#if CONFIG_BT_LE_CONTROLLER_LOG_WRAP_PANIC_HANDLER_ENABLE
void __real_esp_panic_handler(void *info);
void __wrap_esp_panic_handler (void *info)
{
    esp_ble_controller_log_dump_all(true);
    __real_esp_panic_handler(info);
}
#endif // CONFIG_BT_LE_CONTROLLER_LOG_WRAP_PANIC_HANDLER_ENABLE
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

/* This variable tells if BLE is running */
static bool s_ble_active = false;
#ifdef CONFIG_PM_ENABLE
static DRAM_ATTR esp_pm_lock_handle_t s_pm_lock = NULL;
#endif // CONFIG_PM_ENABLE
#define MAIN_XTAL_FREQ_HZ                 (32000000)
static DRAM_ATTR modem_clock_lpclk_src_t s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_INVALID;
static DRAM_ATTR uint32_t s_bt_lpclk_freq = 100000;

#define BLE_CONTROLLER_MALLOC_CAPS          (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT|MALLOC_CAP_DMA)
void *malloc_ble_controller_mem(size_t size)
{
    return heap_caps_malloc(size, BLE_CONTROLLER_MALLOC_CAPS);
}

uint32_t get_ble_controller_free_heap_size(void)
{
    return heap_caps_get_free_size(BLE_CONTROLLER_MALLOC_CAPS);
}

#define BLE_RTC_DELAY_US_LIGHT_SLEEP                    (5100)
#define BLE_RTC_DELAY_US_MODEM_SLEEP                    (1500)

static const struct osi_coex_funcs_t s_osi_coex_funcs_ro = {
    ._magic = OSI_COEX_MAGIC_VALUE,
    ._version = OSI_COEX_VERSION,
    ._coex_wifi_sleep_set = NULL,
    ._coex_core_ble_conn_dyn_prio_get = NULL,
    ._coex_schm_status_bit_set = coex_schm_status_bit_set_wrapper,
    ._coex_schm_status_bit_clear = coex_schm_status_bit_clear_wrapper,
};

struct ext_funcs_t ext_funcs_ro = {
    .ext_version = EXT_FUNC_VERSION,
    ._esp_intr_alloc = esp_intr_alloc_wrapper,
    ._esp_intr_free = esp_intr_free_wrapper,
    ._malloc = bt_osi_mem_malloc_internal,
    ._free = bt_osi_mem_free,
    ._task_create = task_create_wrapper,
    ._task_delete = task_delete_wrapper,
    ._osi_assert = osi_assert_wrapper,
    ._os_random = osi_random_wrapper,
    ._ecc_gen_key_pair = esp_ecc_gen_key_pair,
    ._ecc_gen_dh_key = esp_ecc_gen_dh_key,
    .magic = EXT_FUNC_MAGIC_VALUE,
};

static void IRAM_ATTR osi_assert_wrapper(const uint32_t ln, const char *fn,
                                         uint32_t param1, uint32_t param2)
{
    BT_ASSERT_PRINT("BLE assert: line %d in function %s, param: 0x%x, 0x%x", ln, fn, param1, param2);
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    esp_ble_controller_log_dump_all(true);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    assert(0);
}

static uint32_t IRAM_ATTR osi_random_wrapper(void)
{
    return esp_random();
}

static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_schm_status_bit_set(type, status);
#endif // CONFIG_SW_COEXIST_ENABLE
}

static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_schm_status_bit_clear(type, status);
#endif // CONFIG_SW_COEXIST_ENABLE
}

static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth,
                                void *param, uint32_t prio, void *task_handle, uint32_t core_id)
{
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle,
                                             (core_id < portNUM_PROCESSORS ? core_id : tskNO_AFFINITY));
}

static void task_delete_wrapper(void *task_handle)
{
    vTaskDelete(task_handle);
}

static int esp_ecc_gen_key_pair(uint8_t *pub, uint8_t *priv)
{
    int rc = -1;
#if CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
    rc = ble_sm_alg_gen_key_pair(pub, priv);
#endif // CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
    return rc;
}

static int esp_ecc_gen_dh_key(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                              const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
    int rc = -1;
#if CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
    rc = ble_sm_alg_gen_dhkey(peer_pub_key_x, peer_pub_key_y, our_priv_key, out_dhkey);
#endif // CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
    return rc;
}

static int esp_intr_alloc_wrapper(int source, int flags, intr_handler_t handler,
                                  void *arg, void **ret_handle_in)
{
#if CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY
    int rc = esp_intr_alloc(source, flags, handler, arg, (intr_handle_t *)ret_handle_in);
#else
    int rc = esp_intr_alloc(source, flags | ESP_INTR_FLAG_IRAM, handler, arg, (intr_handle_t *)ret_handle_in);
#endif
    return rc;
}

static int esp_intr_free_wrapper(void **ret_handle)
{
    int rc = 0;
    rc = esp_intr_free((intr_handle_t) * ret_handle);
    *ret_handle = NULL;
    return rc;
}

void esp_bt_rtc_slow_clk_select(uint8_t slow_clk_src)
{
    /* Select slow clock source for BT momdule */
    switch (slow_clk_src) {
        case MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL:
            ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Using main XTAL as clock source");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (MAIN_XTAL_FREQ_HZ/s_bt_lpclk_freq - 1));
            break;
        case MODEM_CLOCK_LPCLK_SRC_RC_SLOW:
            ESP_LOGW(NIMBLE_PORT_LOG_TAG, "Using 136 kHz RC as clock source, use with caution as it may not maintain ACL or Sync process due to low clock accuracy!");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (5 - 1));
            break;
        case MODEM_CLOCK_LPCLK_SRC_XTAL32K:
            ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Using external 32.768 kHz XTAL as clock source");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (1 - 1));
            break;
        case MODEM_CLOCK_LPCLK_SRC_RC32K:
            ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Using 32 kHz RC as clock source, can only run legacy ADV or SCAN due to low clock accuracy!");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (1 - 1));
            break;
        case MODEM_CLOCK_LPCLK_SRC_EXT32K:
            ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Using 32 kHz oscillator as clock source, can only run legacy ADV or SCAN due to low clock accuracy!");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (1 - 1));
            break;
        default:
    }
}

modem_clock_lpclk_src_t esp_bt_get_lpclk_src(void)
{
    return s_bt_lpclk_src;
}

void esp_bt_set_lpclk_src(modem_clock_lpclk_src_t clk_src)
{
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return;
    }

    if (clk_src >= MODEM_CLOCK_LPCLK_SRC_MAX) {
        return;
    }

    s_bt_lpclk_src = clk_src;
}

uint32_t esp_bt_get_lpclk_freq(void)
{
    return s_bt_lpclk_freq;
}

void esp_bt_set_lpclk_freq(uint32_t clk_freq)
{
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return;
    }

    if (!clk_freq) {
        return;
    }

    if (MAIN_XTAL_FREQ_HZ % clk_freq) {
        return;
    }

    s_bt_lpclk_freq = clk_freq;
}

IRAM_ATTR void controller_sleep_cb(uint32_t enable_tick, void *arg)
{
    if (!s_ble_active) {
        return;
    }
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
    r_ble_rtc_wake_up_state_clr();
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
    esp_phy_disable(PHY_MODEM_BT);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(s_pm_lock);
#endif // CONFIG_PM_ENABLE
    s_ble_active = false;
}

IRAM_ATTR void controller_wakeup_cb(void *arg)
{
    if (s_ble_active) {
        return;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_config_t pm_config;
    esp_pm_lock_acquire(s_pm_lock);
    esp_pm_get_configuration(&pm_config);
    assert(esp_rom_get_cpu_ticks_per_us() == pm_config.max_freq_mhz);
    r_ble_rtc_wake_up_state_clr();
#endif //CONFIG_PM_ENABLE
    esp_phy_enable(PHY_MODEM_BT);
    if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_RC_SLOW) {
        uint32_t *clk_freq = (uint32_t *)arg;
        *clk_freq = esp_clk_tree_lp_slow_get_freq_hz(ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED) / 5;
    }
    s_ble_active = true;
}

#ifdef CONFIG_FREERTOS_USE_TICKLESS_IDLE
static esp_err_t sleep_modem_ble_mac_retention_init(void *arg)
{
    uint8_t size;
    int extra = *(int *)arg;
    sleep_retention_entries_config_t *ble_mac_modem_config = r_esp_ble_mac_retention_link_get(&size, extra);
    esp_err_t err = sleep_retention_entries_create(ble_mac_modem_config, size, REGDMA_LINK_PRI_BT_MAC_BB, SLEEP_RETENTION_MODULE_BLE_MAC);
    if (err == ESP_OK) {
        ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Modem BLE MAC retention initialization");
    }
    return err;
}

static esp_err_t sleep_modem_ble_mac_modem_state_init(uint8_t extra)
{
    int retention_args = extra;
    sleep_retention_module_init_param_t init_param = {
        .cbs     = { .create = { .handle = sleep_modem_ble_mac_retention_init, .arg = &retention_args } },
        .depends = BIT(SLEEP_RETENTION_MODULE_BT_BB)
    };
    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_BLE_MAC, &init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate(SLEEP_RETENTION_MODULE_BLE_MAC);
    }
    return err;
}

static void sleep_modem_ble_mac_modem_state_deinit(void)
{
    esp_err_t err = sleep_retention_module_free(SLEEP_RETENTION_MODULE_BLE_MAC);
    if (err == ESP_OK) {
        err = sleep_retention_module_deinit(SLEEP_RETENTION_MODULE_BLE_MAC);
        assert(err == ESP_OK);
    }
}

void IRAM_ATTR sleep_modem_light_sleep_overhead_set(uint32_t overhead)
{
    r_esp_ble_set_wakeup_overhead(overhead);
}
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */


esp_err_t controller_sleep_init(void)
{
    esp_err_t rc = 0;

#ifdef CONFIG_BT_LE_SLEEP_ENABLE
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "BLE modem sleep is enabled");
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
    r_ble_lll_rfmgmt_set_sleep_cb(controller_sleep_cb, controller_wakeup_cb, 0, 0,
                                BLE_RTC_DELAY_US_LIGHT_SLEEP);
#else
    r_ble_lll_rfmgmt_set_sleep_cb(controller_sleep_cb, controller_wakeup_cb, 0, 0,
                                BLE_RTC_DELAY_US_MODEM_SLEEP);
#endif /* FREERTOS_USE_TICKLESS_IDLE */
#endif // CONFIG_BT_LE_SLEEP_ENABLE

#ifdef CONFIG_PM_ENABLE
    rc = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "bt", &s_pm_lock);
    if (rc != ESP_OK) {
        goto error;
    }
    rc = esp_deep_sleep_register_hook(&r_esp_ble_stop_wakeup_timing);
    if (rc != ESP_OK) {
        goto error;
    }
#endif //CONFIG_PM_ENABLE
#if CONFIG_BT_LE_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE
    /* Create a new regdma link for BLE related register restoration */
    rc = sleep_modem_ble_mac_modem_state_init(0);
    if (rc != ESP_OK) {
        goto error;
    }
    esp_sleep_enable_bt_wakeup();
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "Enable light sleep, the wake up source is BLE timer");

    rc = esp_pm_register_inform_out_light_sleep_overhead_callback(sleep_modem_light_sleep_overhead_set);
    if (rc != ESP_OK) {
        goto error;
    }
#endif /* CONFIG_BT_LE_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE */
    return rc;

#ifdef CONFIG_PM_ENABLE
error:
#endif // CONFIG_PM_ENABLE
#if CONFIG_BT_LE_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE
    esp_sleep_disable_bt_wakeup();
    esp_pm_unregister_inform_out_light_sleep_overhead_callback(sleep_modem_light_sleep_overhead_set);
#endif /* CONFIG_BT_LE_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE */
#ifdef CONFIG_PM_ENABLE
    esp_deep_sleep_deregister_hook(&r_esp_ble_stop_wakeup_timing);
    /*lock should release first and then delete*/
    if (s_pm_lock != NULL) {
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
    }
#endif // CONFIG_PM_ENABLE

    return rc;
}

void controller_sleep_deinit(void)
{
#if CONFIG_BT_LE_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE
    r_ble_rtc_wake_up_state_clr();
    esp_sleep_disable_bt_wakeup();
    sleep_modem_ble_mac_modem_state_deinit();
    esp_pm_unregister_inform_out_light_sleep_overhead_callback(sleep_modem_light_sleep_overhead_set);
#endif /* CONFIG_BT_LE_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE */
#ifdef CONFIG_PM_ENABLE
    esp_deep_sleep_deregister_hook(&r_esp_ble_stop_wakeup_timing);
    /* lock should be released first */
    esp_pm_lock_delete(s_pm_lock);
    s_pm_lock = NULL;
#endif //CONFIG_PM_ENABLE
}

typedef enum {
    FILTER_DUPLICATE_PDUTYPE = BIT(0),
    FILTER_DUPLICATE_LENGTH  = BIT(1),
    FILTER_DUPLICATE_ADDRESS = BIT(2),
    FILTER_DUPLICATE_ADVDATA = BIT(3),
    FILTER_DUPLICATE_DEFAULT = FILTER_DUPLICATE_PDUTYPE | FILTER_DUPLICATE_ADDRESS,
    FILTER_DUPLICATE_PDU_ALL = 0xF,
    FILTER_DUPLICATE_EXCEPTION_FOR_MESH = BIT(4),
    FILTER_DUPLICATE_AD_TYPE = BIT(5),
}disc_duplicate_mode_t;


extern void r_filter_duplicate_mode_enable(disc_duplicate_mode_t mode);
extern void r_filter_duplicate_mode_disable(disc_duplicate_mode_t mode);
extern void r_filter_duplicate_set_ring_list_max_num(uint32_t max_num);
extern void r_scan_duplicate_cache_refresh_set_time(uint32_t period_time);

int
ble_vhci_disc_duplicate_mode_enable(int mode)
{
    // TODO: use vendor hci to update
    r_filter_duplicate_mode_enable(mode);
    return true;
}

int
ble_vhci_disc_duplicate_mode_disable(int mode)
{
    // TODO: use vendor hci to update
    r_filter_duplicate_mode_disable(mode);
    return true;
}

int ble_vhci_disc_duplicate_set_max_cache_size(int max_cache_size){
    // TODO: use vendor hci to update
    r_filter_duplicate_set_ring_list_max_num(max_cache_size);
    return true;
}

int ble_vhci_disc_duplicate_set_period_refresh_time(int refresh_period_time){
    // TODO: use vendor hci to update
    r_scan_duplicate_cache_refresh_set_time(refresh_period_time);
    return true;
}

/**
 * @brief Config scan duplicate option mode from menuconfig (Adapt to the old configuration method.)
 */
void ble_controller_scan_duplicate_config(void)
{
    uint32_t duplicate_mode = FILTER_DUPLICATE_DEFAULT;
    uint32_t cache_size = 100;
#if CONFIG_BT_LE_SCAN_DUPL == true
    cache_size = CONFIG_BT_LE_LL_DUP_SCAN_LIST_COUNT;
    if (CONFIG_BT_LE_SCAN_DUPL_TYPE == 0) {
        duplicate_mode = FILTER_DUPLICATE_ADDRESS | FILTER_DUPLICATE_PDUTYPE;
    } else if (CONFIG_BT_LE_SCAN_DUPL_TYPE == 1) {
        duplicate_mode = FILTER_DUPLICATE_ADVDATA;
    } else if (CONFIG_BT_LE_SCAN_DUPL_TYPE == 2) {
        duplicate_mode = FILTER_DUPLICATE_ADDRESS | FILTER_DUPLICATE_ADVDATA;
    }
    duplicate_mode |= FILTER_DUPLICATE_EXCEPTION_FOR_MESH;

    ble_vhci_disc_duplicate_set_period_refresh_time(CONFIG_BT_LE_SCAN_DUPL_CACHE_REFRESH_PERIOD);
#endif

    ble_vhci_disc_duplicate_mode_disable(0xFFFFFFFF);
    ble_vhci_disc_duplicate_mode_enable(duplicate_mode);
    ble_vhci_disc_duplicate_set_max_cache_size(cache_size);
}

static void ble_rtc_clk_init(esp_bt_controller_config_t *cfg)
{
    if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_INVALID) {
#if CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL
        s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL;
#else
#if CONFIG_RTC_CLK_SRC_INT_RC
        s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_RC_SLOW;
#elif CONFIG_RTC_CLK_SRC_EXT_CRYS
        if (rtc_clk_slow_src_get() == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
            s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_XTAL32K;
        } else {
            ESP_LOGW(NIMBLE_PORT_LOG_TAG, "32.768kHz XTAL not detected, fall back to main XTAL as Bluetooth sleep clock");
            s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL;
        }
#elif CONFIG_RTC_CLK_SRC_INT_RC32K
        s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_RC32K;
#elif CONFIG_RTC_CLK_SRC_EXT_OSC
        s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_EXT32K;
#else
        ESP_LOGE(NIMBLE_PORT_LOG_TAG, "Unsupported clock source");
        assert(0);
#endif
#endif /* CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL */
    }

    if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) {
        cfg->rtc_freq = s_bt_lpclk_freq;
    } else if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_XTAL32K) {
        cfg->rtc_freq = 32768;
    } else if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_RC_SLOW) {
        cfg->rtc_freq = esp_clk_tree_lp_slow_get_freq_hz(ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED) / 5;
        cfg->ble_ll_sca = 3000;
    } else if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_RC32K) {
        cfg->rtc_freq = 32000;
    } else if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_EXT32K) {
        cfg->rtc_freq = 32000;
    }
    esp_bt_rtc_slow_clk_select(s_bt_lpclk_src);
}

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    uint8_t mac[6];
    esp_err_t ret = ESP_OK;
    ble_npl_count_info_t npl_info;
    uint8_t hci_transport_mode;

    memset(&npl_info, 0, sizeof(ble_npl_count_info_t));
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_ERR_INVALID_STATE;
    }

    if (!cfg) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "cfg is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    ret = esp_register_ext_funcs(&ext_funcs_ro);
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "register extend functions failed");
        return ret;
    }

    /* Initialize the function pointers for OS porting */
    npl_freertos_funcs_init();
    struct npl_funcs_t *p_npl_funcs = npl_freertos_funcs_get();
    if (!p_npl_funcs) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "npl functions get failed");
        return ESP_ERR_INVALID_ARG;
    }

    ret = esp_register_npl_funcs(p_npl_funcs);
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "npl functions register failed");
        goto free_mem;
    }

    r_ble_get_npl_element_info(cfg, &npl_info);
    npl_freertos_set_controller_npl_info(&npl_info);
    if (npl_freertos_mempool_init() != 0) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "npl mempool init failed");
        ret = ESP_ERR_INVALID_ARG;
        goto free_mem;
    }

#if CONFIG_BT_NIMBLE_ENABLED
    /* ble_npl_eventq_init() needs to use npl functions in rom and
     * must be called after esp_bt_controller_init().
     */
    ble_npl_eventq_init(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLED
    /* Enable BT-related clocks */
    modem_clock_module_enable(PERIPH_BT_MODULE);
    modem_clock_module_mac_reset(PERIPH_BT_MODULE);
    /* Select slow clock source for BT momdule */
    ble_rtc_clk_init(cfg);

    if (ble_osi_coex_funcs_register((struct osi_coex_funcs_t *)&s_osi_coex_funcs_ro) != 0) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "osi coex funcs reg failed");
        ret = ESP_ERR_INVALID_ARG;
        goto modem_deint;
    }

#if CONFIG_SW_COEXIST_ENABLE
    coex_init();
#endif // CONFIG_SW_COEXIST_ENABLE

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    ret = esp_bt_controller_log_init();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "ble_controller_log_init failed %d", ret);
        goto modem_deint;
    }
#endif // CONFIG_BT_CONTROLLER_LOG_ENABLED
    ret = esp_ble_register_bb_funcs();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "esp_ble_register_bb_funcs failed %d", ret);
        goto modem_deint;
    }

    ESP_LOGI(NIMBLE_PORT_LOG_TAG, "ble controller commit:[%s]", ble_controller_get_compile_version());

    ret = r_ble_controller_init(cfg);
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "r_ble_controller_init failed %d", ret);
        goto modem_deint;
    }

#if CONFIG_BT_LE_LL_PEER_SCA_SET_ENABLE
    r_ble_ll_customize_peer_sca_set(CONFIG_BT_LE_LL_PEER_SCA);
#endif // CONFIG_BT_LE_LL_PEER_SCA_SET_ENABLE

    ret = ble_stack_initEnv();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "ble_stack_initEnv failed %d", ret);
        goto free_controller;
    }

    ble_controller_scan_duplicate_config();

    ret = os_msys_init();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "msys_init failed %d", ret);
        goto free_controller;
    }

    ret = controller_sleep_init();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "controller_sleep_init failed %d", ret);
        goto free_controller;
    }

    ESP_ERROR_CHECK(esp_read_mac((uint8_t *)mac, ESP_MAC_BT));
    ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Bluetooth MAC: %02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    swap_in_place(mac, 6);
    r_esp_ble_ll_set_public_addr(mac);

    ble_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

#if CONFIG_BT_LE_HCI_INTERFACE_USE_RAM
    hci_transport_mode = HCI_TRANSPORT_VHCI;
#elif CONFIG_BT_LE_HCI_INTERFACE_USE_UART
    hci_transport_mode = HCI_TRANSPORT_UART_NO_DMA;
#if CONFIG_BT_LE_UART_HCI_DMA_MODE
    hci_transport_mode = HCI_TRANSPORT_UART_UHCI;
#endif // CONFIG_BT_LE_UART_HCI_DMA_MODE
#endif // CONFIG_BT_LE_HCI_INTERFACE_USE_RAM
    ret = hci_transport_init(hci_transport_mode);
    if (ret) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "hci transport init failed %d", ret);
        goto free_controller;
    }

    return ESP_OK;
free_controller:
    hci_transport_deinit();
    controller_sleep_deinit();
    os_msys_deinit();
    ble_stack_deinitEnv();
    r_ble_controller_deinit();
modem_deint:
    esp_ble_unregister_bb_funcs();
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    esp_bt_controller_log_deinit();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    modem_clock_deselect_lp_clock_source(PERIPH_BT_MODULE);
    modem_clock_module_disable(PERIPH_BT_MODULE);
#if CONFIG_BT_NIMBLE_ENABLED
    ble_npl_eventq_deinit(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLED
free_mem:
    npl_freertos_mempool_deinit();
    esp_unregister_npl_funcs();
    npl_freertos_funcs_deinit();
    esp_unregister_ext_funcs();
    return ret;
}

esp_err_t esp_bt_controller_deinit(void)
{
    if ((ble_controller_status < ESP_BT_CONTROLLER_STATUS_INITED) ||
        (ble_controller_status >= ESP_BT_CONTROLLER_STATUS_ENABLED)) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }

    hci_transport_deinit();
    controller_sleep_deinit();

    os_msys_deinit();

    modem_clock_deselect_lp_clock_source(PERIPH_BT_MODULE);
    modem_clock_module_disable(PERIPH_BT_MODULE);

    ble_stack_deinitEnv();
    r_ble_controller_deinit();
    esp_ble_unregister_bb_funcs();
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    esp_bt_controller_log_deinit();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

#if CONFIG_BT_NIMBLE_ENABLED
    /* De-initialize default event queue */
    ble_npl_eventq_deinit(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLED

    esp_unregister_npl_funcs();

    esp_unregister_ext_funcs();

    /* De-initialize npl functions */
    npl_freertos_funcs_deinit();

    npl_freertos_mempool_deinit();

    ble_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

    return ESP_OK;
}

esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode)
{
    esp_err_t ret = ESP_OK;

    if (mode != ESP_BT_MODE_BLE) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller mode");
        return ESP_FAIL;
    }
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }
    if (!s_ble_active) {
#if CONFIG_PM_ENABLE
        esp_pm_lock_acquire(s_pm_lock);
#endif  // CONFIG_PM_ENABLE
        esp_phy_enable(PHY_MODEM_BT);
        s_ble_active = true;
    }
    esp_btbb_enable();
#if CONFIG_SW_COEXIST_ENABLE
    coex_enable();
#endif // CONFIG_SW_COEXIST_ENABLE

#if CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY
    r_ble_ll_scan_start_time_init_compensation(500);
    r_priv_sdk_config_insert_proc_time_set(500);
#endif // CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY

    if (ble_stack_enable() != 0) {
        ret = ESP_FAIL;
        goto error;
    }

    if (r_ble_controller_enable(mode) != 0) {
        ret = ESP_FAIL;
        goto error;
    }
    ble_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;
    return ESP_OK;

error:
    ble_stack_disable();
#if CONFIG_SW_COEXIST_ENABLE
    coex_disable();
#endif
    esp_btbb_disable();
    if (s_ble_active) {
        esp_phy_disable(PHY_MODEM_BT);
#if CONFIG_PM_ENABLE
        esp_pm_lock_release(s_pm_lock);
#endif  // CONFIG_PM_ENABLE
        s_ble_active = false;
    }
    return ret;
}

esp_err_t esp_bt_controller_disable(void)
{
    if (ble_controller_status < ESP_BT_CONTROLLER_STATUS_ENABLED) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }
    if (r_ble_controller_disable() != 0) {
        return ESP_FAIL;
    }
    ble_stack_disable();
#if CONFIG_SW_COEXIST_ENABLE
    coex_disable();
#endif
    esp_btbb_disable();
    if (s_ble_active) {
        esp_phy_disable(PHY_MODEM_BT);
#if CONFIG_PM_ENABLE
        esp_pm_lock_release(s_pm_lock);
#endif  // CONFIG_PM_ENABLE
        s_ble_active = false;
    }
    ble_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    return ESP_OK;
}

esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode)
{
    ESP_LOGD(NIMBLE_PORT_LOG_TAG, "%s not implemented, return OK", __func__);
    return ESP_OK;
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
        ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release %s [0x%08x] - [0x%08x], len %d", area->name, mem_start, mem_end, mem_end - mem_start);
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

esp_err_t esp_bt_mem_release(esp_bt_mode_t mode)
{
    esp_err_t ret = ESP_OK;

    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
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
    }

    return ret;
}

esp_bt_controller_status_t esp_bt_controller_get_status(void)
{
    return ble_controller_status;
}

esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level)
{
    esp_err_t stat = ESP_FAIL;

    switch (power_type) {
    case ESP_BLE_PWR_TYPE_DEFAULT:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_PWR_TYPE_ADV:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_ADV, 0xFF, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_PWR_TYPE_SCAN:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_SCAN, 0, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_PWR_TYPE_CONN_HDL0:
    case ESP_BLE_PWR_TYPE_CONN_HDL1:
    case ESP_BLE_PWR_TYPE_CONN_HDL2:
    case ESP_BLE_PWR_TYPE_CONN_HDL3:
    case ESP_BLE_PWR_TYPE_CONN_HDL4:
    case ESP_BLE_PWR_TYPE_CONN_HDL5:
    case ESP_BLE_PWR_TYPE_CONN_HDL6:
    case ESP_BLE_PWR_TYPE_CONN_HDL7:
    case ESP_BLE_PWR_TYPE_CONN_HDL8:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_CONN, power_type, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    default:
        stat = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return stat;
}

esp_err_t esp_ble_tx_power_set_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle,
                                        esp_power_level_t power_level)
{
    esp_err_t stat = ESP_FAIL;
    switch (power_type) {
    case ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_SCAN, 0, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_ADV:
    case ESP_BLE_ENHANCED_PWR_TYPE_CONN:
        if (r_ble_txpwr_set(power_type, handle, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    default:
        stat = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return stat;
}

esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type)
{
    int tx_level = 0;

    switch (power_type) {
    case ESP_BLE_PWR_TYPE_DEFAULT:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
        break;
    case ESP_BLE_PWR_TYPE_ADV:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_ADV, 0);
        break;
    case ESP_BLE_PWR_TYPE_SCAN:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_SCAN, 0);
        break;
    case ESP_BLE_PWR_TYPE_CONN_HDL0:
    case ESP_BLE_PWR_TYPE_CONN_HDL1:
    case ESP_BLE_PWR_TYPE_CONN_HDL2:
    case ESP_BLE_PWR_TYPE_CONN_HDL3:
    case ESP_BLE_PWR_TYPE_CONN_HDL4:
    case ESP_BLE_PWR_TYPE_CONN_HDL5:
    case ESP_BLE_PWR_TYPE_CONN_HDL6:
    case ESP_BLE_PWR_TYPE_CONN_HDL7:
    case ESP_BLE_PWR_TYPE_CONN_HDL8:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_CONN, power_type);
        break;
    default:
        return ESP_PWR_LVL_INVALID;
    }

    if (tx_level < 0) {
        return ESP_PWR_LVL_INVALID;
    }

    return (esp_power_level_t)tx_level;
}

esp_power_level_t esp_ble_tx_power_get_enhanced(esp_ble_enhanced_power_type_t power_type,
                                                uint16_t handle)
{
    int tx_level = 0;

    switch (power_type) {
    case ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_SCAN, 0);
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_ADV:
    case ESP_BLE_ENHANCED_PWR_TYPE_CONN:
        tx_level = r_ble_txpwr_get(power_type, handle);
        break;
    default:
        return ESP_PWR_LVL_INVALID;
    }

    if (tx_level < 0) {
       return ESP_PWR_LVL_INVALID;
    }

    return (esp_power_level_t)tx_level;
}

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
#if !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
static void esp_bt_controller_log_interface(uint32_t len, const uint8_t *addr, uint32_t len_append, const uint8_t *addr_append, uint32_t flag)
{
    bool end = (flag & BIT(BLE_LOG_INTERFACE_FLAG_END));
#if CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
    esp_bt_controller_log_storage(len, addr, end);
#else // !CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);
    esp_panic_handler_feed_wdts();

    if (len && addr) {
        for (int i = 0; i < len; i++) { esp_rom_printf("%02x ", addr[i]); }
    }
    if (len_append && addr_append) {
        for (int i = 0; i < len_append; i++) { esp_rom_printf("%02x ", addr_append[i]); }
    }
    if (end) { esp_rom_printf("\n"); }

    portEXIT_CRITICAL_SAFE(&spinlock);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
}
#endif // !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED

void esp_ble_controller_log_dump_all(bool output)
{
#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    ble_log_spi_out_dump_all();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

#if CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
    esp_bt_read_ctrl_log_from_flash(output);
#elif !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);
    esp_panic_handler_feed_wdts();
    BT_ASSERT_PRINT("\r\n[DUMP_START:");
    r_ble_log_async_output_dump_all(output);
    BT_ASSERT_PRINT(":DUMP_END]\r\n");
    portEXIT_CRITICAL_SAFE(&spinlock);
#endif
}
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

#if (!CONFIG_BT_NIMBLE_ENABLED) && (CONFIG_BT_CONTROLLER_ENABLED)
#if CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
#define BLE_SM_KEY_ERR 0x17
#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
#include "mbedtls/aes.h"
#if CONFIG_BT_LE_SM_SC
#include "mbedtls/cipher.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/cmac.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"

static mbedtls_ecp_keypair keypair;
#endif // CONFIG_BT_LE_SM_SC

#else
#include "tinycrypt/aes.h"
#include "tinycrypt/constants.h"
#include "tinycrypt/utils.h"

#if CONFIG_BT_LE_SM_SC
#include "tinycrypt/cmac_mode.h"
#include "tinycrypt/ecc_dh.h"
#endif // CONFIG_BT_LE_SM_SC
#endif // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS

/* Based on Core Specification 4.2 Vol 3. Part H 2.3.5.6.1 */
static const uint8_t ble_sm_alg_dbg_priv_key[32] = {
    0x3f, 0x49, 0xf6, 0xd4, 0xa3, 0xc5, 0x5f, 0x38, 0x74, 0xc9, 0xb3, 0xe3,
    0xd2, 0x10, 0x3f, 0x50, 0x4a, 0xff, 0x60, 0x7b, 0xeb, 0x40, 0xb7, 0x99,
    0x58, 0x99, 0xb8, 0xa6, 0xcd, 0x3c, 0x1a, 0xbd
};

int ble_sm_alg_gen_dhkey(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                         const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
    uint8_t dh[32];
    uint8_t pk[64];
    uint8_t priv[32];
    int rc = BLE_SM_KEY_ERR;

    swap_buf(pk, peer_pub_key_x, 32);
    swap_buf(&pk[32], peer_pub_key_y, 32);
    swap_buf(priv, our_priv_key, 32);

#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
    struct mbedtls_ecp_point pt = {0}, Q = {0};
    mbedtls_mpi z = {0}, d = {0};
    mbedtls_ctr_drbg_context ctr_drbg = {0};
    mbedtls_entropy_context entropy = {0};

    uint8_t pub[65] = {0};
    /* Hardcoded first byte of pub key for MBEDTLS_ECP_PF_UNCOMPRESSED */
    pub[0] = 0x04;
    memcpy(&pub[1], pk, 64);

    /* Initialize the required structures here */
    mbedtls_ecp_point_init(&pt);
    mbedtls_ecp_point_init(&Q);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    mbedtls_mpi_init(&d);
    mbedtls_mpi_init(&z);

    /* Below 3 steps are to validate public key on curve secp256r1 */
    if (mbedtls_ecp_group_load(&keypair.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1) != 0) {
        goto exit;
    }

    if (mbedtls_ecp_point_read_binary(&keypair.MBEDTLS_PRIVATE(grp), &pt, pub, 65) != 0) {
        goto exit;
    }

    if (mbedtls_ecp_check_pubkey(&keypair.MBEDTLS_PRIVATE(grp), &pt) != 0) {
        goto exit;
    }

    /* Set PRNG */
    if ((rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0)) != 0) {
        goto exit;
    }

    /* Prepare point Q from pub key */
    if (mbedtls_ecp_point_read_binary(&keypair.MBEDTLS_PRIVATE(grp), &Q, pub, 65) != 0) {
        goto exit;
    }

    if (mbedtls_mpi_read_binary(&d, priv, 32) != 0) {
        goto exit;
    }

    rc = mbedtls_ecdh_compute_shared(&keypair.MBEDTLS_PRIVATE(grp), &z, &Q, &d,
                                     mbedtls_ctr_drbg_random, &ctr_drbg);
    if (rc != 0) {
        goto exit;
    }

    rc = mbedtls_mpi_write_binary(&z, dh, 32);
    if (rc != 0) {
        goto exit;
    }

exit:
    mbedtls_ecp_point_free(&pt);
    mbedtls_mpi_free(&z);
    mbedtls_mpi_free(&d);
    mbedtls_ecp_point_free(&Q);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    if (rc != 0) {
        return BLE_SM_KEY_ERR;
    }

#else
    if (uECC_valid_public_key(pk, &curve_secp256r1) < 0) {
        return BLE_SM_KEY_ERR;
    }

    rc = uECC_shared_secret(pk, priv, dh, &curve_secp256r1);
    if (rc == TC_CRYPTO_FAIL) {
        return BLE_SM_KEY_ERR;
    }
#endif // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS

    swap_buf(out_dhkey, dh, 32);
    return 0;
}

#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
static int mbedtls_gen_keypair(uint8_t *public_key, uint8_t *private_key)
{
    int rc = BLE_SM_KEY_ERR;
    mbedtls_entropy_context entropy = {0};
    mbedtls_ctr_drbg_context ctr_drbg = {0};

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ecp_keypair_init(&keypair);

    if ((rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0)) != 0) {
        goto exit;
    }

    if ((rc = mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, &keypair,
                                  mbedtls_ctr_drbg_random, &ctr_drbg)) != 0) {
        goto exit;
    }

    if ((rc = mbedtls_mpi_write_binary(&keypair.MBEDTLS_PRIVATE(d), private_key, 32)) != 0) {
        goto exit;
    }

    size_t olen = 0;
    uint8_t pub[65] = {0};

    if ((rc = mbedtls_ecp_point_write_binary(&keypair.MBEDTLS_PRIVATE(grp), &keypair.MBEDTLS_PRIVATE(Q), MBEDTLS_ECP_PF_UNCOMPRESSED,
              &olen, pub, 65)) != 0) {
        goto exit;
    }

    memcpy(public_key, &pub[1], 64);

exit:
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    if (rc != 0) {
        mbedtls_ecp_keypair_free(&keypair);
        return BLE_SM_KEY_ERR;
    }

    return 0;
}
#endif  // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS

/**
 * pub: 64 bytes
 * priv: 32 bytes
 */
int ble_sm_alg_gen_key_pair(uint8_t *pub, uint8_t *priv)
{
#if CONFIG_BT_LE_SM_SC_DEBUG_KEYS
    swap_buf(pub, ble_sm_alg_dbg_pub_key, 32);
    swap_buf(&pub[32], &ble_sm_alg_dbg_pub_key[32], 32);
    swap_buf(priv, ble_sm_alg_dbg_priv_key, 32);
#else
    uint8_t pk[64];

    do {
#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
        if (mbedtls_gen_keypair(pk, priv) != 0) {
            return BLE_SM_KEY_ERR;
        }
#else
        if (uECC_make_key(pk, priv, &curve_secp256r1) != TC_CRYPTO_SUCCESS) {
            return BLE_SM_KEY_ERR;
        }
#endif  // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
        /* Make sure generated key isn't debug key. */
    } while (memcmp(priv, ble_sm_alg_dbg_priv_key, 32) == 0);

    swap_buf(pub, pk, 32);
    swap_buf(&pub[32], &pk[32], 32);
    swap_in_place(priv, 32);
#endif // CONFIG_BT_LE_SM_SC_DEBUG_KEYS
    return 0;
}

#endif // CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
#endif // (!CONFIG_BT_NIMBLE_ENABLED) && (CONFIG_BT_CONTROLLER_ENABLED)
#if CONFIG_BT_LE_DEBUG_REMAIN_SCENE_ENABLED
#include "esp_gdbstub.h"
#endif // CONFIG_BT_LE_DEBUG_REMAIN_SCENE_ENABLED

int IRAM_ATTR
ble_capture_info_user_handler(uint8_t type, uint32_t reason, uint32_t param1, uint32_t param2)
{
    int i;

    switch(type) {
        case 0:
            for (i = 0; i < 2; i++) {
                esp_ble_controller_info_capture(0x010101);
            }
#if CONFIG_BT_LE_DEBUG_REMAIN_SCENE_ENABLED
            uintptr_t sp;
            __asm__ volatile ("mv %0, sp" : "=r" (sp));
            esp_gdbstub_panic_handler(&sp);
#endif // CONFIG_BT_LE_DEBUG_REMAIN_SCENE_ENABLED
            break;
#if CONFIG_BT_LE_ASSERT_WHEN_ABNORMAL_DISCONN_ENABLED
        case 1:
            if ((reason == 0x08) || (reason == 0x3d) || (reason == 0x28)) {
                osi_assert_wrapper(__LINE__,__func__, type, reason);
            }
            break;
#endif // CONFIG_BT_LE_ASSERT_WHEN_ABNORMAL_DISCONN_ENABLED
        default:
            break;
    }
    return 0;
}
