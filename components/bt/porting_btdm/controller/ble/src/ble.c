/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
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

// #include "nimble/nimble_port_freertos.h"
#include "esp_private/esp_modem_clock.h"

#ifdef ESP_PLATFORM
#include "esp_log.h"
#endif // ESP_PLATFORM

#if CONFIG_SW_COEXIST_ENABLE
#include "private/esp_coexist_internal.h"
#endif // CONFIG_SW_COEXIST_ENABLE

// #include "nimble/nimble_npl_os.h"
#include "btdm_endian.h"

#include "esp_bt.h"
#include "ble_priv.h"
#include "esp_intr_alloc.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp_phy_init.h"
#include "esp_private/periph_ctrl.h"
#include "esp_rom_sys.h"

#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "hci/hci_hal.h"
#endif // CONFIG_BT_BLUEDROID_ENABLED
#include "esp_private/sleep_retention.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_private/periph_ctrl.h"
#include "esp_sleep.h"
#include "soc/rtc.h"


#include "ble_msys.h"
#include "btdm_osal.h"


#if CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2
#include "ble_log.h"
#else /* !CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 */
#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
#include "ble_log/ble_log_spi_out.h"
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

#if CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED
#include "ble_log/ble_log_uhci_out.h"
#endif // CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED
#endif /* CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 */



#if UC_BT_CTRL_BLE_IS_ENABLE

/* Macro definition
 ************************************************************************
 */
#define NIMBLE_PORT_LOG_TAG          "BLE_INIT"

#define EXT_FUNC_VERSION             0x20250819
#define EXT_FUNC_MAGIC_VALUE         0xA5A5A5A5

#define BT_ASSERT_PRINT              esp_rom_printf

#ifdef CONFIG_BT_BLUEDROID_ENABLED
/* ACL_DATA_MBUF_LEADINGSPCAE: The leadingspace in user info header for ACL data */
#define ACL_DATA_MBUF_LEADINGSPCAE    4
#endif // CONFIG_BT_BLUEDROID_ENABLED

/* Types definition
 ************************************************************************
 */

struct ext_funcs_t {
    uint32_t ext_version;
    void *(* _malloc)(uint32_t size, uint32_t flags);
    void (*_free)(void *p);
    void (*_osi_assert)(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
    uint32_t (* _os_random)(void);
    int (* _ecc_gen_key_pair)(uint8_t *public, uint8_t *priv);
    int (* _ecc_gen_dh_key)(const uint8_t *remote_pub_key_x, const uint8_t *remote_pub_key_y,
                            const uint8_t *local_priv_key, uint8_t *dhkey);
    void (* _esp_reset_rpa_moudle)(void);
    int (*_ecc_aes_cmac)(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out);
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
extern void btdm_get_default_mac(uint8_t *mac);

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
extern int ble_controller_enable(uint8_t mode);
extern int ble_controller_disable(void);
extern int esp_register_ext_funcs (struct ext_funcs_t *);
extern void esp_unregister_ext_funcs (void);
extern int r_esp_ble_ll_set_public_addr(const uint8_t *addr);
extern uint32_t r_os_cputime_get32(void);
extern uint32_t r_os_cputime_ticks_to_usecs(uint32_t ticks);
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
extern const sleep_retention_entries_config_t *r_esp_ble_mac_retention_link_get(uint8_t *size, uint8_t extra);
extern void esp_ble_set_wakeup_overhead(uint32_t overhead);
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
extern void esp_ble_change_rtc_freq(uint32_t freq);
extern void btdm_sleep_set_sleep_cb(void *s_cb, void *w_cb, void *s_arg,
                                        void *w_arg, uint32_t us_to_enabled);
// extern void r_ble_rtc_wake_up_state_clr(void);

extern int  r_esp_ble_msys_init(uint16_t msys_size1, uint16_t msys_size2, uint16_t msys_cnt1, uint16_t msys_cnt2, uint8_t from_heap);
extern void r_esp_ble_msys_deinit(void);

extern int ble_sm_alg_gen_dhkey(const uint8_t *peer_pub_key_x,
                                const uint8_t *peer_pub_key_y,
                                const uint8_t *our_priv_key, uint8_t *out_dhkey);
extern int ble_sm_alg_gen_key_pair(uint8_t *pub, uint8_t *priv);
extern int r_ble_txpwr_set(esp_ble_enhanced_power_type_t power_type, uint16_t handle, int power_level);
extern int r_ble_txpwr_get(esp_ble_enhanced_power_type_t power_type, uint16_t handle);
extern uint32_t _bt_bss_start;
extern uint32_t _bt_bss_end;
extern uint32_t _bt_controller_bss_start;
extern uint32_t _bt_controller_bss_end;
extern uint32_t _bt_data_start;
extern uint32_t _bt_data_end;
extern uint32_t _bt_controller_data_start;
extern uint32_t _bt_controller_data_end;

int ble_sm_ecc_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out);
static void osi_assert_wrapper(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
static uint32_t osi_random_wrapper(void);
static void esp_reset_rpa_moudle(void);
static int esp_ecc_gen_key_pair(uint8_t *pub, uint8_t *priv);
static int esp_ecc_gen_dh_key(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                              const uint8_t *our_priv_key, uint8_t *out_dhkey);
static int esp_ecc_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out);
/* Local variable definition
 ***************************************************************************
 */
/* Static variable declare */
static DRAM_ATTR esp_bt_controller_status_t ble_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
#if !CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2
#if !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED && !CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED
static void esp_bt_controller_log_interface(uint32_t len, const uint8_t *addr, uint32_t len_append, const uint8_t *addr_append, uint32_t flag);
#endif // !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED && !CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED
#if CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
static void esp_bt_ctrl_log_partition_get_and_erase_first_block(void);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
#endif /* !CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 */
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
static bool log_is_inited = false;

#if CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2
/* TODO: Remove event handler dependency in lib */
static void void_handler(void) {}

/* TODO: Declare public interfaces in a public header */
void esp_bt_controller_log_deinit(void)
{
    log_is_inited = false;
    r_ble_log_deinit_simple();
    ble_log_deinit();
}

esp_err_t esp_bt_controller_log_init(void)
{
    if (log_is_inited) {
        return ESP_OK;
    }

    if (!ble_log_init()) {
        goto exit;
    }

    if (r_ble_log_init_simple(ble_log_write_hex_ll, void_handler) != 0) {
        goto exit;
    }

    if (r_ble_log_ctrl_level_and_mod(CONFIG_BT_LE_CONTROLLER_LOG_OUTPUT_LEVEL,
                                     CONFIG_BT_LE_CONTROLLER_LOG_MOD_OUTPUT_SWITCH) != ESP_OK) {
        goto exit;
    }
    log_is_inited = true;
    return ESP_OK;

exit:
    esp_bt_controller_log_deinit();
    return ESP_FAIL;
}
#else /* !CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 */
const static uint32_t log_bufs_size[] = {CONFIG_BT_LE_LOG_CTRL_BUF1_SIZE, CONFIG_BT_LE_LOG_HCI_BUF_SIZE, CONFIG_BT_LE_LOG_CTRL_BUF2_SIZE};
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

#if CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED
    if (ble_log_uhci_out_init() != 0) {
        goto uhci_out_init_failed;
    }
#endif // CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED

#if CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    if (r_ble_log_init_simple(ble_log_spi_out_ll_write, ble_log_spi_out_ll_log_ev_proc) != 0) {
        goto log_init_failed;
    }
#elif CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED
    if (r_ble_log_init_simple(ble_log_uhci_out_ll_write, ble_log_uhci_out_ll_log_ev_proc) != 0) {
        goto log_init_failed;
    }
#else
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
#endif

    if (r_ble_log_init_async(esp_bt_controller_log_interface, task_create, buffers, (uint32_t *)log_bufs_size) != 0) {
        goto log_init_failed;
    }
#endif

    if (r_ble_log_ctrl_level_and_mod(CONFIG_BT_LE_CONTROLLER_LOG_OUTPUT_LEVEL, CONFIG_BT_LE_CONTROLLER_LOG_MOD_OUTPUT_SWITCH) != ESP_OK) {
        goto ctrl_level_init_failed;
    }
    log_is_inited = true;
    return ESP_OK;

ctrl_level_init_failed:
#if CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    r_ble_log_deinit_simple();
#elif CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED
    r_ble_log_deinit_simple();
#else
    r_ble_log_deinit_async();
#endif
log_init_failed:
#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    ble_log_spi_out_deinit();
spi_out_init_failed:
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
#if CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED
    ble_log_uhci_out_deinit();
uhci_out_init_failed:
#endif // CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED
    return ESP_FAIL;
}

void esp_bt_controller_log_deinit(void)
{
#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    ble_log_spi_out_deinit();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

#if CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED
    ble_log_uhci_out_deinit();
#endif // CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED

#if CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    r_ble_log_deinit_simple();
#elif CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED
    r_ble_log_deinit_simple();
#else
    r_ble_log_deinit_async();
#endif

    log_is_inited = false;
}
#endif /* CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 */

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
    err = esp_bt_controller_log_init(log_output_mode);
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

struct ext_funcs_t ext_funcs_ro = {
    .ext_version = EXT_FUNC_VERSION,
    ._malloc = (void *(*)(uint32_t, uint32_t))btdm_osal_malloc,
    ._free = btdm_osal_free,
    ._osi_assert = osi_assert_wrapper,
    ._os_random = osi_random_wrapper,
    ._ecc_gen_key_pair = esp_ecc_gen_key_pair,
    ._ecc_gen_dh_key = esp_ecc_gen_dh_key,
    ._esp_reset_rpa_moudle = esp_reset_rpa_moudle,
    ._ecc_aes_cmac  = esp_ecc_aes_cmac,
    .magic = EXT_FUNC_MAGIC_VALUE,
};

static void IRAM_ATTR esp_reset_rpa_moudle(void)
{

}

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
    //TODO: use esp_ramdom
    // return esp_ramdom();

    return btdm_osal_rand();
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

static int esp_ecc_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out)
{
    return ble_sm_ecc_aes_cmac(key, in, len, out);
}

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
esp_err_t sleep_modem_ble_mac_modem_state_init(void)
{
    uint8_t size;
    const sleep_retention_entries_config_t *ble_mac_modem_config = r_esp_ble_mac_retention_link_get(&size, 0);
    esp_err_t err = sleep_retention_entries_create(ble_mac_modem_config, size, REGDMA_LINK_PRI_5, SLEEP_RETENTION_MODULE_BLE_MAC);
    if (err == ESP_OK) {
        ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Modem BLE MAC retention initialization");
    }
    return err;
}
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */

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
    cache_size = CONFIG_BT_LE_SCAN_DUPL_CACHE_SIZE;
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

esp_err_t esp_ble_controller_init(esp_bt_controller_config_t *cfg)
{
    uint8_t mac[6];
    esp_err_t ret = ESP_OK;
    // uint32_t slow_clk_freq = 0;

    extern const char *ble_controller_get_compile_version(void);
    ESP_LOGI(NIMBLE_PORT_LOG_TAG, "BLE controller compile version [%s]\n", ble_controller_get_compile_version());

    if (!cfg) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "cfg is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    ret = esp_register_ext_funcs(&ext_funcs_ro);
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "register extend functions failed");
        return ret;
    }

#if CONFIG_BT_NIMBLE_ENABLED
    /* ble_npl_eventq_init() needs to use npl functions in rom and
     * must be called after esp_ble_controller_init().
     */
    ble_npl_eventq_init(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLE

    /* Enable BT-related clocks */
    // modem_clock_module_mac_reset(PERIPH_BT_MODULE);
    // modem_clock_module_enable(PERIPH_BT_MODULE);
// #if CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL
//    esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL);
//    slow_clk_freq = 100000;
// #else
// #if CONFIG_RTC_CLK_SRC_INT_RC
//     esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_RC_SLOW);
//     slow_clk_freq = 30000;
// #elif CONFIG_RTC_CLK_SRC_EXT_CRYS
//     if (rtc_clk_slow_src_get() == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
//         esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_XTAL32K);
//         slow_clk_freq = 32768;
//     } else {
//         ESP_LOGW(NIMBLE_PORT_LOG_TAG, "32.768kHz XTAL not detected, fall back to main XTAL as Bluetooth sleep clock");
//         esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL);
//         slow_clk_freq = 100000;
//     }
// #elif CONFIG_RTC_CLK_SRC_INT_RC32K
//     esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_RC32K);
//     slow_clk_freq = 32000;
// #elif CONFIG_RTC_CLK_SRC_EXT_OSC
//     esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_EXT32K);
//     slow_clk_freq = 32000;
// #else
//     ESP_LOGE(NIMBLE_PORT_LOG_TAG, "Unsupported clock source");
//     assert(0);
// #endif
// #endif /* CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL */

// #if CONFIG_SW_COEXIST_ENABLE
//     if (ble_osi_coex_funcs_register((struct osi_coex_funcs_t *)&s_osi_coex_funcs_ro) != 0) {
//         ESP_LOGW(NIMBLE_PORT_LOG_TAG, "osi coex funcs reg failed");
//         ret = ESP_ERR_INVALID_ARG;
//         goto modem_deint;
//     }

//     coex_init();
// #endif // CONFIG_SW_COEXIST_ENABLE
    cfg->ble.rtc_freq = 32000;

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    ret = esp_bt_controller_log_init();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "ble_controller_log_init failed %d", ret);
        goto modem_deint;
    }
#endif // CONFIG_BT_CONTROLLER_LOG_ENABLED

    extern int esp_ble_register_bb_funcs(void);
    ret = esp_ble_register_bb_funcs();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "esp_ble_register_bb_funcs failed %d", ret);
        goto modem_deint;
    }

    extern int r_ble_controller_init(void *cfg);
    ret = r_ble_controller_init(&cfg->ble);
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "ble_controller_init failed %d", ret);
        goto modem_deint;
    }

    // esp_ble_change_rtc_freq(slow_clk_freq);

    ble_controller_scan_duplicate_config();

    ret = ble_msys_init();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "ble_msys_init failed %d", ret);
        goto free_controller;
    }

    // ESP_ERROR_CHECK(esp_read_mac((uint8_t *)mac, ESP_MAC_BT));
    btdm_osal_read_efuse_mac(mac);
    ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Bluetooth MAC: %02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    btdm_swap_in_place(mac, 6);
    r_esp_ble_ll_set_public_addr(mac);
    ble_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    return ESP_OK;

free_controller:
    ble_msys_deinit();
    r_ble_controller_deinit();
modem_deint:
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    esp_bt_controller_log_deinit();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    // modem_clock_deselect_lp_clock_source(PERIPH_BT_MODULE);
    // modem_clock_module_disable(PERIPH_BT_MODULE);
#if CONFIG_BT_NIMBLE_ENABLED
    ble_npl_eventq_deinit(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLED

    esp_unregister_ext_funcs();
    return ret;
}

esp_err_t esp_ble_controller_deinit(void)
{
    if ((ble_controller_status < ESP_BT_CONTROLLER_STATUS_INITED) ||
        (ble_controller_status >= ESP_BT_CONTROLLER_STATUS_ENABLED)) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }

    ble_msys_deinit();

    // modem_clock_deselect_lp_clock_source(PERIPH_BT_MODULE);
    // modem_clock_module_disable(PERIPH_BT_MODULE);

    r_ble_controller_deinit();

    extern int esp_ble_unregister_bb_funcs(void);
    esp_ble_unregister_bb_funcs();

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    esp_bt_controller_log_deinit();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

#if CONFIG_BT_NIMBLE_ENABLED
    /* De-initialize default event queue */
    ble_npl_eventq_deinit(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLED

    esp_unregister_ext_funcs();

    ble_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

    return ESP_OK;
}

esp_err_t esp_ble_controller_disable(void)
{
    if (ble_controller_status < ESP_BT_CONTROLLER_STATUS_ENABLED) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }
    if (ble_controller_disable() != 0) {
        return ESP_FAIL;
    }
    ble_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    return ESP_OK;
}

esp_err_t esp_ble_controller_mem_release(esp_bt_mode_t mode)
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

esp_err_t esp_ble_mem_release(esp_bt_mode_t mode)
{
    intptr_t mem_start, mem_end;

    if (mode & ESP_BT_MODE_BLE) {
        /* If the addresses of btdm .bss and bt .bss are consecutive,
         * they are registered in the system heap as a piece of memory
         */
        if(_bt_bss_end == _bt_controller_bss_start) {
            mem_start = (intptr_t)&_bt_bss_start;
            mem_end = (intptr_t)&_bt_controller_bss_end;
            if (mem_start != mem_end) {
                ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release BSS [0x%08x] - [0x%08x], len %d",
                         mem_start, mem_end, mem_end - mem_start);
                ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
            }
        } else {
            mem_start = (intptr_t)&_bt_bss_start;
            mem_end = (intptr_t)&_bt_bss_end;
            if (mem_start != mem_end) {
                ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release BT BSS [0x%08x] - [0x%08x], len %d",
                         mem_start, mem_end, mem_end - mem_start);
                ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
            }

            mem_start = (intptr_t)&_bt_controller_bss_start;
            mem_end = (intptr_t)&_bt_controller_bss_end;
            if (mem_start != mem_end) {
                ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release Controller BSS [0x%08x] - [0x%08x], len %d",
                         mem_start, mem_end, mem_end - mem_start);
                ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
            }
        }
        /* If the addresses of btdm .data and bt .data are consecutive,
         * they are registered in the system heap as a piece of memory
         */
        if(_bt_data_end == _bt_controller_data_start) {
            mem_start = (intptr_t)&_bt_data_start;
            mem_end = (intptr_t)&_bt_controller_data_end;
            if (mem_start != mem_end) {
                ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release data [0x%08x] - [0x%08x], len %d",
                         mem_start, mem_end, mem_end - mem_start);
                ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
            }
        } else {
            mem_start = (intptr_t)&_bt_data_start;
            mem_end = (intptr_t)&_bt_data_end;
            if (mem_start != mem_end) {
                ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release BT Data [0x%08x] - [0x%08x], len %d",
                         mem_start, mem_end, mem_end - mem_start);
                ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
            }

            mem_start = (intptr_t)&_bt_controller_data_start;
            mem_end = (intptr_t)&_bt_controller_data_end;
            if (mem_start != mem_end) {
                ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release Controller Data [0x%08x] - [0x%08x], len %d",
                         mem_start, mem_end, mem_end - mem_start);
                ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
            }
        }
    }

    return ESP_OK;
}


esp_bt_controller_status_t esp_ble_controller_get_status(void)
{
    return ble_controller_status;
}

esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level)
{
    esp_err_t stat = ESP_FAIL;

    switch (power_type) {
    case ESP_BLE_PWR_TYPE_DEFAULT:
    case ESP_BLE_PWR_TYPE_ADV:
    case ESP_BLE_PWR_TYPE_SCAN:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, power_level) == 0) {
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
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, power_level) == 0) {
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
    case ESP_BLE_PWR_TYPE_ADV:
    case ESP_BLE_PWR_TYPE_SCAN:
    case ESP_BLE_PWR_TYPE_DEFAULT:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
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
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
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
#if CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2
void esp_ble_controller_log_dump_all(bool output)
{
    ble_log_dump_to_console();
}
#else /* !CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 */
#if !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED && !CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED
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
#endif // !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED && !CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED

void esp_ble_controller_log_dump_all(bool output)
{
#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    ble_log_spi_out_dump_all();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

#if CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED
    ble_log_uhci_out_dump_all();
#endif // CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED

#if CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
    esp_bt_read_ctrl_log_from_flash(output);
#elif !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED && !CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);
    esp_panic_handler_feed_wdts();
    BT_ASSERT_PRINT("\r\n[DUMP_START:");
    r_ble_log_async_output_dump_all(output);
    BT_ASSERT_PRINT(":DUMP_END]\r\n");
    portEXIT_CRITICAL_SAFE(&spinlock);
#endif
}
#endif /* CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 */
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

/* Adapt to the aes cmac function under Nimble Host  */
#if CONFIG_BT_NIMBLE_ENABLED
#if CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS
#include "mbedtls/aes.h"
#include "mbedtls/cipher.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/cmac.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"
#else
#include "tinycrypt/aes.h"
#include "tinycrypt/constants.h"
#include "tinycrypt/utils.h"
#include "tinycrypt/cmac_mode.h"
#include "tinycrypt/ecc_dh.h"
#endif // CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS

int ble_sm_ecc_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out)
{
#if CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS
    int ret = 0;
    mbedtls_cipher_context_t ctx;
    const mbedtls_cipher_info_t *cipher_info;

    mbedtls_cipher_init(&ctx);

    // Choose AES-128 cipher info (or AES-256 if using 32-byte key)
    cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
    if (cipher_info == NULL) {
        ret = -1;
        goto exit;
    }

    if ((ret = mbedtls_cipher_setup(&ctx, cipher_info)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_starts(&ctx, key, 128)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_update(&ctx, in, len)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_finish(&ctx, out)) != 0) {
        goto exit;
    }

    ret = 0;

exit:
    mbedtls_cipher_free(&ctx);
    return ret;
#else
    // TinyCrypt implementation
    struct tc_aes_key_sched_struct sched;
    struct tc_cmac_struct state;
    // Initialize CMAC context, set key and scheduling information
    // If initialization fails, return -1
    if (tc_cmac_setup(&state, key, &sched) == TC_CRYPTO_FAIL) {
        return -1;
    }

    if (tc_cmac_update(&state, in, len) == TC_CRYPTO_FAIL) {
        return -1;
    }

    if (tc_cmac_final(out, &state) == TC_CRYPTO_FAIL) {
        return -1;
    }

    return 0;
#endif // CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS
}

#endif // CONFIG_BT_NIMBLE_ENABLED

#if  (!CONFIG_BT_NIMBLE_ENABLED) && (CONFIG_BT_CONTROLLER_ENABLED == true)
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

#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
#if CONFIG_BT_LE_SM_SC
static mbedtls_ecp_keypair keypair;
#endif // CONFIG_BT_LE_SM_SC
#endif// CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS

int ble_sm_alg_gen_dhkey(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                         const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
    uint8_t dh[32];
    uint8_t pk[64];
    uint8_t priv[32];
    int rc = BLE_SM_KEY_ERR;

    btdm_swap_buf(pk, peer_pub_key_x, 32);
    btdm_swap_buf(&pk[32], peer_pub_key_y, 32);
    btdm_swap_buf(priv, our_priv_key, 32);

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
    if ((rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0)) != 0) {
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

    btdm_swap_buf(out_dhkey, dh, 32);
    return 0;
}

/* based on Core Specification 4.2 Vol 3. Part H 2.3.5.6.1 */
static const uint8_t ble_sm_alg_dbg_priv_key[32] = {
    0x3f, 0x49, 0xf6, 0xd4, 0xa3, 0xc5, 0x5f, 0x38, 0x74, 0xc9, 0xb3, 0xe3,
    0xd2, 0x10, 0x3f, 0x50, 0x4a, 0xff, 0x60, 0x7b, 0xeb, 0x40, 0xb7, 0x99,
    0x58, 0x99, 0xb8, 0xa6, 0xcd, 0x3c, 0x1a, 0xbd
};

#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
static int mbedtls_gen_keypair(uint8_t *public_key, uint8_t *private_key)
{
    int rc = BLE_SM_KEY_ERR;
    size_t olen = 0;
    uint8_t pub[65] = {0};
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


    if ((rc = mbedtls_ecp_point_write_binary(&keypair.MBEDTLS_PRIVATE(grp),
                                             &keypair.MBEDTLS_PRIVATE(Q),
                                             MBEDTLS_ECP_PF_UNCOMPRESSED,
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
#endif // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS

/**
 * pub: 64 bytes
 * priv: 32 bytes
 */
int ble_sm_alg_gen_key_pair(uint8_t *pub, uint8_t *priv)
{
#if CONFIG_BT_LE_SM_SC_DEBUG_KEYS
    btdm_swap_buf(pub, ble_sm_alg_dbg_pub_key, 32);
    btdm_swap_buf(&pub[32], &ble_sm_alg_dbg_pub_key[32], 32);
    btdm_swap_buf(priv, ble_sm_alg_dbg_priv_key, 32);
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
#endif // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
        /* Make sure generated key isn't debug key. */
    } while (memcmp(priv, ble_sm_alg_dbg_priv_key, 32) == 0);

    btdm_swap_buf(pub, pk, 32);
    btdm_swap_buf(&pub[32], &pk[32], 32);
    btdm_swap_in_place(priv, 32);
#endif // CONFIG_BT_LE_SM_SC_DEBUG_KEYS

    return 0;
}

int ble_sm_ecc_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out)
{
#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
    int ret = 0;
    mbedtls_cipher_context_t ctx;
    const mbedtls_cipher_info_t *cipher_info;

    mbedtls_cipher_init(&ctx);

    // Choose AES-128 cipher info (or AES-256 if using 32-byte key)
    cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
    if (cipher_info == NULL) {
        ret = -1;
        goto exit;
    }

    if ((ret = mbedtls_cipher_setup(&ctx, cipher_info)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_starts(&ctx, key, 128)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_update(&ctx, in, len)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_finish(&ctx, out)) != 0) {
        goto exit;
    }

    ret = 0;

exit:
    mbedtls_cipher_free(&ctx);
    return ret;

#else
    // TinyCrypt implementation
    struct tc_aes_key_sched_struct sched;
    struct tc_cmac_struct state;
    // Initialize CMAC context, set key and scheduling information
    // If initialization fails, return -1
    if (tc_cmac_setup(&state, key, &sched) == TC_CRYPTO_FAIL) {
        return -1;
    }

    if (tc_cmac_update(&state, in, len) == TC_CRYPTO_FAIL) {
        return -1;
    }

    if (tc_cmac_final(out, &state) == TC_CRYPTO_FAIL) {
        return -1;
    }

    return 0;
#endif
    // Calculation successful, return 0
    return 0;
}
#endif // (!CONFIG_BT_NIMBLE_ENABLED) && (CONFIG_BT_CONTROLLER_ENABLED == true)

/*
* Bluetooth LE stack functions
**************************************************************************************************
*/
extern int r_base_stack_initEnv(void);
extern int r_base_stack_deinitEnv(void);
extern void r_base_stack_init(void);
extern int r_base_stack_enable(void);
extern void r_base_stack_disable(void);

extern int r_conn_stack_initEnv(void);
extern void r_conn_stack_deinitEnv(void);
extern int r_conn_stack_enable(void);
extern void r_conn_stack_disable(void);
extern int r_gapFinder_stack_enable(void);
extern void r_gapFinder_stack_disable(void);

#define base_stack_initEnv       r_base_stack_initEnv
#define base_stack_deinitEnv     r_base_stack_deinitEnv
#define base_stack_enable        r_base_stack_enable
#define base_stack_disable       r_base_stack_disable

#define conn_stack_initEnv       r_conn_stack_initEnv
#define conn_stack_deinitEnv     r_conn_stack_deinitEnv
#define conn_stack_enable        r_conn_stack_enable
#define conn_stack_disable       r_conn_stack_disable
#define gapFinder_stack_enable   r_gapFinder_stack_enable
#define gapFinder_stack_disable  r_gapFinder_stack_disable

#if CONFIG_BT_LE_ERROR_SIM_ENABLED
extern int r_conn_errorSim_initEnv(void);
extern void r_conn_errorSim_deinitEnv(void);
extern int r_conn_errorSim_enable(void);
extern void r_conn_errorSim_disable(void);


#define conn_errorSim_initEnv        r_conn_errorSim_initEnv
#define conn_errorSim_deinitEnv      r_conn_errorSim_deinitEnv
#define conn_errorSim_enable         r_conn_errorSim_enable
#define conn_errorSim_disable        r_conn_errorSim_disable
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED

#if CONFIG_BT_LE_ISO_SUPPORT
extern int r_iso_stack_initEnv(void);
extern void r_iso_stack_deinitEnv(void);
extern int r_iso_stack_enable(void);
extern void r_iso_stack_disable(void);
#define iso_stack_enable         r_iso_stack_enable
#define iso_stack_disable        r_iso_stack_disable
#define iso_stack_initEnv   r_iso_stack_initEnv
#define iso_stack_deinitEnv r_iso_stack_deinitEnv
#endif // CONFIG_BT_LE_ISO_SUPPORT


int ble_stack_init(esp_bt_controller_config_t *cfg)
{
    int rc;

    rc = esp_ble_controller_init(cfg);
    if (rc != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "esp_ble_controller_init failed %d", rc);
        return rc;
    }

    rc = base_stack_initEnv();
    if (rc != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "base_stack_initEnv failed %d", rc);
        return rc;
    }

#if DEFAULT_BT_LE_MAX_CONNECTIONS
    rc = conn_stack_initEnv();
    if (rc) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "conn_stack_initEnv failed %d", rc);
        return rc;
    }
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    rc = conn_errorSim_initEnv();
    if (rc) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "conn_errorSim_initEnv failed %d", rc);
        return rc;
    }
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

#if CONFIG_BT_LE_ISO_SUPPORT
    rc = iso_stack_initEnv();
    if (rc) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "iso_stack_initEnv failed %d", rc);
        return rc;
    }
#endif // CONFIG_BT_LE_ISO_SUPPORT

#if CONFIG_SW_COEXIST_ENABLE
    // Should be invoked in ble ?
    extern int r_bt_rf_coex_env_init(void);
    r_bt_rf_coex_env_init();
#endif /* CONFIG_SW_COEXIST_ENABLE */
    return 0;
}

void ble_stack_deinit(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    extern void r_bt_rf_coex_env_deinit(void);
    r_bt_rf_coex_env_deinit();
#endif /* CONFIG_SW_COEXIST_ENABLE */

#if CONFIG_BT_LE_ISO_SUPPORT
    iso_stack_deinitEnv();
#endif // CONFIG_BT_LE_ISO_SUPPORT
#if DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_errorSim_deinitEnv();
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_stack_deinitEnv();
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

    base_stack_deinitEnv();

    esp_ble_controller_deinit();
}

int ble_stack_enable(void)
{
    int rc;

#if UC_BT_CTRL_BR_EDR_IS_ENABLE && CONFIG_BT_CTRL_MULTI_LINK_ENABLED
    extern void r_sched_txn_bredrIsEnabled(bool);
    r_sched_txn_bredrIsEnabled(true);
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE && CONFIG_BT_CTRL_MULTI_LINK_ENABLED

    rc = base_stack_enable();
    if (rc != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "base_stack_enable failed %d", rc);
        return rc;
    }

#if DEFAULT_BT_LE_MAX_CONNECTIONS
    rc = conn_stack_enable();
    if (rc) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "conn_stack_enable failed %d", rc);
        return rc;
    }
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    rc = conn_errorSim_enable();
    if (rc) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "conn_errorSim_enable failed %d", rc);
        return rc;
    }
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

#if CONFIG_BT_LE_ISO_SUPPORT

    rc = iso_stack_enable();
    if (rc != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "iso_stack_enable failed %d", rc);
        return rc;
    }

    rc = gapFinder_stack_enable();
    if (rc != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "gapFinder_stack_enable failed %d", rc);
        return rc;
    }

#endif // BT_LE_ISO_SUPPORT

#if CONFIG_SW_COEXIST_ENABLE
    extern int r_bt_rf_coex_env_enable(void);
    r_bt_rf_coex_env_enable();
#endif /* CONFIG_SW_COEXIST_ENABLE */
    return 0;
}

void ble_stack_disable(void)
{
#if CONFIG_BT_LE_ISO_SUPPORT
    gapFinder_stack_disable();

    iso_stack_disable();
#endif // BT_LE_ISO_SUPPORT

#if DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_errorSim_disable();
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_stack_disable();
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

    base_stack_disable();
}
#endif // UC_BT_CTRL_BLE_IS_ENABLE


#if CONFIG_BT_NIMBLE_ENABLED
#define BLE_HOST_CO_COUNT (8)
#define BLE_HOST_EV_COUNT (11 + BLE_HOST_CO_COUNT)
#define BLE_HOST_EVQ_COUNT (3)
#define BLE_HOST_SEM_COUNT (10)
#define BLE_HOST_MUTEX_COUNT (4)
#else
#define BLE_HOST_CO_COUNT (0)
#define BLE_HOST_EV_COUNT (0)
#define BLE_HOST_EVQ_COUNT (0)
#define BLE_HOST_SEM_COUNT (0)
#define BLE_HOST_MUTEX_COUNT (0)
#endif

void
ble_osal_elem_calc(esp_bt_controller_config_t *cfg, btdm_osal_elem_num_t *elem)
{
    btdm_osal_elem_num_t ble_osal_elem;

    extern int r_ble_ll_get_npl_element_info(void *cfg, btdm_osal_elem_num_t *elem);
    r_ble_ll_get_npl_element_info(cfg, &ble_osal_elem);

    // elem->evt_count += ble_osal_elem.evt_count + BLE_HOST_CO_COUNT;
    // elem->evtq_count += ble_osal_elem.evtq_count + BLE_HOST_EV_COUNT;
    // elem->co_count += ble_osal_elem.co_count + BLE_HOST_EVQ_COUNT;
    // elem->sem_count += ble_osal_elem.sem_count + BLE_HOST_SEM_COUNT;
    // elem->mutex_count += ble_osal_elem.mutex_count + BLE_HOST_MUTEX_COUNT;

    // TODO
    elem->evt_count += ble_osal_elem.evt_count + BLE_HOST_CO_COUNT + 10;
    elem->evtq_count += ble_osal_elem.evtq_count + BLE_HOST_EV_COUNT + 10;
    elem->co_count += ble_osal_elem.co_count + BLE_HOST_EVQ_COUNT + 10;
    elem->sem_count += ble_osal_elem.sem_count + BLE_HOST_SEM_COUNT + 10;
    elem->mutex_count += ble_osal_elem.mutex_count + BLE_HOST_MUTEX_COUNT + 10;
}
