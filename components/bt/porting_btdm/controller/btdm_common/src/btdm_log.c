/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include "btdm_user_cfg.h"
#include "btdm_osal.h"
#include "sdkconfig.h"
#include "assert.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "btdm_osal.h"
#include "esp_err.h"
#include "esp_log.h"

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

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
typedef void (*interface_func_t) (uint32_t len, const uint8_t *addr, uint32_t len_append, const uint8_t *addr_append, uint32_t flag);

enum {
    BLE_LOG_INTERFACE_FLAG_CONTINUE = 0,
    BLE_LOG_INTERFACE_FLAG_END,
};

/**
 * @brief Select buffers
*/
typedef enum {
    ESP_BLE_LOG_BUF_HCI         = 0x02,
    ESP_BLE_LOG_BUF_CONTROLLER  = 0x05,
} esp_ble_log_buf_t;

#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

/*
 ***************************************************************************************************
 * External Functions
 ***************************************************************************************************
 */
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
void r_btdm_sched_list_details_dump(void);
void api_base_ll_dumpDebugStatus(void);
void api_sched_txn_dumpListDetails(void);
void r_ble_phy_hw_state_dump(uint8_t);
void esp_ble_controller_log_dump_all(bool);
void esp_panic_handler_feed_wdts(void);
void r_ble_log_async_output_dump_all(bool output);

int r_ble_log_init_async(interface_func_t interface, bool task_create, uint8_t buffers, uint32_t *bufs_size);
int r_ble_log_deinit_async(void);
int r_ble_log_init_simple(interface_func_t interface, void *handler);
void r_ble_log_deinit_simple(void);
void r_ble_log_async_select_dump_buffers(uint8_t buffers);
int r_ble_log_ctrl_level_and_mod(uint8_t log_level, uint32_t mod_switch);
int r_ble_ctrl_mod_type(uint16_t mod, uint32_t mod_type_switch);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

/*
 ***************************************************************************************************
 * Static Function Definitions
 ***************************************************************************************************
 */
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

#if CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
#error "This function has been deprecated."

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

#if !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED && !CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED
static void esp_bt_controller_log_interface(uint32_t len, const uint8_t *addr, uint32_t len_append, const uint8_t *addr_append, uint32_t flag)
{
    bool end = (flag & (1 << BLE_LOG_INTERFACE_FLAG_END));
#if CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
    esp_bt_controller_log_storage(len, addr, end);
#else // !CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
    btdm_osal_hw_enter_critical();
    esp_panic_handler_feed_wdts();

    if (len && addr) {
      for (int i = 0; i < len; i++) {
        esp_rom_printf("%02x ", addr[i]);
      }
    }
    if (len_append && addr_append) {
      for (int i = 0; i < len_append; i++) {
        esp_rom_printf("%02x ", addr_append[i]);
      }
    }
    if (end) {
      esp_rom_printf("\n");
    }

    btdm_osal_hw_exit_critical(0);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
}
#endif // !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED && !CONFIG_BT_LE_CONTROLLER_LOG_UHCI_OUT_ENABLED

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
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

static void IRAM_ATTR
btdm_log_exception_dump(void)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    api_base_ll_dumpDebugStatus();
    r_btdm_sched_list_details_dump();
    api_sched_txn_dumpListDetails();
    esp_ble_controller_log_dump_all(true);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
}

/*
 ***************************************************************************************************
 * Public Function Definitions
 ***************************************************************************************************
 */
void IRAM_ATTR
wr_btdm_log_set_flags(uint8_t p0)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_set_buf_index_flag(uint32_t p0);
    r_ble_log_set_buf_index_flag(p0);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_reset_flags(uint8_t p0)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_reset_buf_index_flag(uint32_t p0);
    r_ble_log_reset_buf_index_flag(p0);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_x0(uint32_t p0)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_x0(uint32_t p0);
    r_ble_log_internal_x0(p0);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_x1(uint32_t p0, uint32_t p1)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_x1(uint32_t p0, uint32_t p1);
    r_ble_log_internal_x1(p0, p1);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_x2(uint32_t p0, uint32_t p1, uint32_t p2)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_x2(uint32_t p0, uint32_t p1, uint32_t p2);
    r_ble_log_internal_x2(p0, p1, p2);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_x3(uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_x3(uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3);
    r_ble_log_internal_x3(p0, p1, p2, p3);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_hex(uint32_t p0, uint32_t p1, uint32_t p2)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_hex(uint32_t p0, uint32_t p1, uint32_t p2);
    r_ble_log_internal_hex(p0, p1, p2);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_raw_export(uint16_t p0, uint8_t * addr, uint32_t len)
{
    esp_rom_printf("ID: 0x%04x, params: ", p0);
    for (int i = 0; i < len; i++) {
        esp_rom_printf("0x%02x, ", addr[i]);
    }
    esp_rom_printf("\n");
}

void IRAM_ATTR
wr_btdm_assert(const char *file, int line, int p0, int p1)
{
    btdm_log_exception_dump();
    esp_rom_printf("assert %s,%d, param:0x%x,0x%x\n", file, line, p0, p1);
    assert(0);
}

void IRAM_ATTR
wr_btdm_compressed_assert_x0(uint32_t cond, uint32_t id)
{
    if (!cond) {
        btdm_log_exception_dump();
        esp_rom_printf("bt assertion id:%d\n", id);
        assert(0);
    }
}

void IRAM_ATTR
wr_btdm_compressed_assert_x1(uint32_t cond, uint32_t id, uint32_t p1)
{
    if (!cond) {
        btdm_log_exception_dump();
        esp_rom_printf("bt assertion id:%d, params: 0x%x\n", id, p1);
        assert(0);
    }
}

void IRAM_ATTR
wr_btdm_compressed_assert_x2(uint32_t cond, uint32_t id, uint32_t p1, uint32_t p2)
{
    if (!cond) {
        btdm_log_exception_dump();
        esp_rom_printf("bt assertion id:%d, params: 0x%x, 0x%x\n", id, p1, p2);
        assert(0);
    }
}

#if CONFIG_BT_LE_CONTROLLER_LOG_WRAP_PANIC_HANDLER_ENABLE
void __real_esp_panic_handler(void *info);
void __wrap_esp_panic_handler (void *info)
{
    esp_ble_controller_log_dump_all(true);
    __real_esp_panic_handler(info);
}
#endif // CONFIG_BT_LE_CONTROLLER_LOG_WRAP_PANIC_HANDLER_ENABLE

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
#if CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2
void esp_ble_controller_log_dump_all(bool output)
{
    ble_log_dump_to_console();
}
#else /* !CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 */
void esp_ble_controller_log_dump_all(bool output)
{
#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
    ble_log_spi_out_dump_all();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

#if CONFIG_BT_LE_CONTROLLER_LOG_STORAGE_ENABLE
    esp_bt_read_ctrl_log_from_flash(output);
#elif !CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_ENABLED
    btdm_osal_hw_enter_critical();
    esp_panic_handler_feed_wdts();
    esp_rom_printf("\r\n[DUMP_START:");
    r_ble_log_async_output_dump_all(output);
    esp_rom_printf(":DUMP_END]\r\n");
    btdm_osal_hw_exit_critical(0);
#endif
}
#endif // CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

#if CONFIG_BT_LE_CONTROLLER_LOG_TASK_WDT_USER_HANDLER_ENABLE
void esp_task_wdt_isr_user_handler(void)
{
    esp_ble_controller_log_dump_all(true);
}
#endif // CONFIG_BT_LE_CONTROLLER_LOG_TASK_WDT_USER_HANDLER_ENABLE


/*
 ***************************************************************************************************
 * BTDM LOG Initialization
 ***************************************************************************************************
 */
int
btdm_log_init(void)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    esp_err_t ret;
    ret = esp_bt_controller_log_init();
    if (ret != ESP_OK) {
        return ret;
    }
#endif // CONFIG_BT_CONTROLLER_LOG_ENABLED

    return 0;
}

void
btdm_log_deinit(void)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    esp_bt_controller_log_deinit();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
}
