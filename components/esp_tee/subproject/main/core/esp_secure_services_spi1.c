/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_fault.h"

#include "hal/spi_flash_hal.h"
#include "hal/spi_flash_types.h"
#include "esp_flash_chips/spi_flash_chip_generic.h"
#include "esp_flash_chips/spi_flash_defs.h"
#include "esp_private/memspi_host_driver.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_private/spi_flash_os.h"
#include "esp_flash.h"
#include "esp_flash_chips/esp_flash_types.h"

#include "esp_tee.h"
#include "esp_tee_memory_utils.h"
#include "esp_tee_flash.h"

static __attribute__((unused)) const char *TAG = "esp_tee_sec_srv_spi1";

/* ---------------------------------------------- SPI Flash HAL ------------------------------------------------- */

/* Bound accesses to the build-time flash capacity, not the REE-provided chip->size */
#if CONFIG_ESPTOOLPY_FLASHSIZE_1MB
#define FLASH_CHIP_CAPACITY    (1U << 20)
#elif CONFIG_ESPTOOLPY_FLASHSIZE_2MB
#define FLASH_CHIP_CAPACITY    (2U << 20)
#elif CONFIG_ESPTOOLPY_FLASHSIZE_4MB
#define FLASH_CHIP_CAPACITY    (4U << 20)
#elif CONFIG_ESPTOOLPY_FLASHSIZE_8MB
#define FLASH_CHIP_CAPACITY    (8U << 20)
#elif CONFIG_ESPTOOLPY_FLASHSIZE_16MB
#define FLASH_CHIP_CAPACITY    (16U << 20)
#elif CONFIG_ESPTOOLPY_FLASHSIZE_32MB
#define FLASH_CHIP_CAPACITY    (32U << 20)
#elif CONFIG_ESPTOOLPY_FLASHSIZE_64MB
#define FLASH_CHIP_CAPACITY    (64U << 20)
#elif CONFIG_ESPTOOLPY_FLASHSIZE_128MB
#define FLASH_CHIP_CAPACITY    (128U << 20)
#else
#error "Unknown flash size for the TEE SPI1 flash protection capacity bound"
#endif

#define FLASH_ADDR_24BIT_LIMIT (1U << 24)

/* `limit` caps the range to what the address phase can encode, so the flash decodes the validated address */
static bool is_flash_range_in_bounds(uint32_t paddr, uint32_t len, uint32_t limit)
{
    limit = MIN(limit, FLASH_CHIP_CAPACITY);
    return (len <= limit) && (paddr <= limit - len);
}

static bool is_flash_addr_writable(uint32_t paddr, uint32_t len, uint32_t limit)
{
    return is_flash_range_in_bounds(paddr, len, limit) &&
           !esp_tee_flash_check_prange_in_tee_region(paddr, len) &&
           !esp_tee_flash_check_prange_write_protected(paddr, len);
}

static bool is_flash_addr_readable(uint32_t paddr, uint32_t len, uint32_t limit)
{
    return is_flash_range_in_bounds(paddr, len, limit) &&
           !esp_tee_flash_check_prange_in_tee_region(paddr, len);
}

/* Validate the whole aligned sector/block that the erase opcode clears */
static bool is_flash_erase_ok(uint32_t paddr, uint32_t size, uint32_t limit)
{
    return is_flash_addr_writable(paddr & ~(size - 1U), size, limit);
}

/* NOTE: only a configured read opcode with a matching address phase gets a non-zero limit */
static uint32_t spi1_read_addr_limit(void)
{
    uint32_t opcode_bitlen;

    /* NOTE: command phase off would make the flash decode the address byte as the opcode */
    if (!spimem_flash_ll_get_hw(SPI1_HOST)->user.usr_command) {
        return 0;
    }

    switch (spimem_flash_ll_get_hw(SPI1_HOST)->user2.usr_command_value) {
    case CMD_FASTRD_QIO:
    case CMD_FASTRD_QUAD:
    case CMD_FASTRD_DIO:
    case CMD_FASTRD_DUAL:
    case CMD_FASTRD:
    case CMD_READ:
        opcode_bitlen = 24U;
        break;
    case CMD_FASTRD_QIO_4B:
    case CMD_FASTRD_QUAD_4B:
    case CMD_FASTRD_DIO_4B:
    case CMD_FASTRD_DUAL_4B:
    case CMD_FASTRD_4B:
    case CMD_READ_4B:
        opcode_bitlen = 32U;
        break;
    default:
        return 0;
    }

    /* NOTE: address phase must match the opcode width (+8 for QIO/DIO mode bits) so the flash decodes the validated address */
    const uint32_t addr_bitlen = (uint32_t)spi_flash_ll_get_addr_bitlen(spi_flash_ll_get_hw(SPI1_HOST)) & ~7U;
    if (addr_bitlen != opcode_bitlen && addr_bitlen != opcode_bitlen + 8U) {
        return 0;
    }

    return (opcode_bitlen == 24U) ? FLASH_ADDR_24BIT_LIMIT : UINT32_MAX;
}

/* Allowlist of REE flash opcodes, each bound to its address width and the flash range it touches */
static bool is_common_trans_ok(const spi_flash_trans_t *trans)
{
    /* NOTE: keep transfers within the 64-byte SPI1 data buffer */
    if (trans->mosi_len > SPI_FLASH_HAL_MAX_WRITE_BYTES || trans->miso_len > SPI_FLASH_HAL_MAX_READ_BYTES) {
        return false;
    }
    if (trans->mosi_len != 0 && !esp_tee_buf_in_ree(trans->mosi_data, trans->mosi_len)) {
        return false;
    }
    if (trans->miso_len != 0 && !esp_tee_buf_in_ree(trans->miso_data, trans->miso_len)) {
        return false;
    }
    /* NOTE: enforce the TEE-configured line mode so the flash decodes the validated address */
    if (trans->flags & SPI_FLASH_TRANS_FLAG_IGNORE_BASEIO) {
        return false;
    }

    const uint32_t addr = trans->address;
    uint32_t addr_bitlen = 24U;

    switch (trans->command) {
    case CMD_WREN:                  /* 0x06 - write enable (set the write-enable latch) */
    case CMD_WRDI:                  /* 0x04 - write disable (clear the write-enable latch) */
    case CMD_RDSR:                  /* 0x05 - read status register 1 */
    case CMD_RDSR2:                 /* 0x35 - read status register 2 */
    case CMD_WRSR:                  /* 0x01 - write status register 1 */
    case CMD_WRSR2:                 /* 0x31 - write status register 2 */
    case CMD_RDID:                  /* 0x9F - read JEDEC manufacturer/device ID */
    case CMD_RDUID:                 /* 0x4B - read the factory-programmed unique ID */
        return trans->address_bitlen == 0U;
    case CMD_RDSFDP:                /* 0x5A - read SFDP table (not the flash array) */
        return trans->address_bitlen == 24U;
    case CMD_READ:                  /* 0x03 - read data */
        /* NOTE: plain read, without a MOSI or dummy phase */
        if (trans->mosi_len != 0 || trans->dummy_bitlen != 0) {
            return false;
        }
        return trans->address_bitlen == 24U &&
               is_flash_addr_readable(addr, MAX(1U, trans->miso_len), FLASH_ADDR_24BIT_LIMIT);
    case CMD_PROGRAM_PAGE:          /* 0x02 - page program */
        return trans->address_bitlen == 24U &&
               is_flash_addr_writable(addr, MAX(1U, trans->mosi_len), FLASH_ADDR_24BIT_LIMIT);
    case CMD_PROGRAM_PAGE_4B:       /* 0x12 - page program, 4-byte address */
        return trans->address_bitlen == 32U &&
               is_flash_addr_writable(addr, MAX(1U, trans->mosi_len), UINT32_MAX);
    case CMD_SECTOR_ERASE_4B:       /* 0x21 - 4KB sector erase, 4-byte address */
        addr_bitlen = 32U;
    /* fall through */
    case CMD_SECTOR_ERASE:          /* 0x20 - 4KB sector erase */
        return trans->address_bitlen == addr_bitlen &&
               is_flash_erase_ok(addr, FLASH_SECTOR_SIZE, (addr_bitlen == 24U) ? FLASH_ADDR_24BIT_LIMIT : UINT32_MAX);
    case CMD_LARGE_BLOCK_ERASE_4B:  /* 0xDC - 64KB block erase, 4-byte address */
        addr_bitlen = 32U;
    /* fall through */
    case CMD_LARGE_BLOCK_ERASE:     /* 0xD8 - 64KB block erase */
        return trans->address_bitlen == addr_bitlen &&
               is_flash_erase_ok(addr, FLASH_BLOCK_SIZE, (addr_bitlen == 24U) ? FLASH_ADDR_24BIT_LIMIT : UINT32_MAX);
    default:
        return false;
    }
}

extern void spi_flash_hal_poll_cmd_done(spi_flash_host_inst_t *host);
extern esp_err_t spi_flash_hal_configure_host_io_mode(spi_flash_host_inst_t *host, uint32_t command,
                                                      uint32_t addr_bitlen, int dummy_cyclelen_base,
                                                      esp_flash_io_mode_t io_mode);

static const spi_flash_host_driver_t tee_host_driver = {
    .poll_cmd_done          = spi_flash_hal_poll_cmd_done,
    .configure_host_io_mode = spi_flash_hal_configure_host_io_mode,
};

static spi_flash_host_inst_t *tee_own_host(const spi_flash_host_inst_t *host, spi_flash_hal_context_t *snap)
{
    if (!esp_tee_buf_in_ree(host, sizeof(spi_flash_hal_context_t))) {
        return NULL;
    }

    *snap = *(const spi_flash_hal_context_t *)host;

    /* Reject a host aimed at another peripheral rather than silently retargeting it */
    if (snap->spi != spi_flash_ll_get_hw(SPI1_HOST)) {
        return NULL;
    }

    snap->inst.driver  = &tee_host_driver;
    snap->spi          = spi_flash_ll_get_hw(SPI1_HOST);
    snap->base_io_mode = SPI_FLASH_SLOWRD;
    /* NOTE: the SPI1 main flash runs without extra dummy cycles */
    snap->extra_dummy  = 0;

    return &snap->inst;
}

uint32_t _ss_spi_flash_hal_check_status(spi_flash_host_inst_t *host)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return 0;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    return spi_flash_hal_check_status(tee_host);
}

esp_err_t _ss_spi_flash_hal_common_command(spi_flash_host_inst_t *host, spi_flash_trans_t *trans)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    if (!esp_tee_buf_in_ree(trans, sizeof(spi_flash_trans_t))) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_flash_trans_t trans_snap = *trans;

    bool trans_valid = is_common_trans_ok(&trans_snap);
    if (!trans_valid) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, trans_snap.address);
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(trans_valid);

    return spi_flash_hal_common_command(tee_host, &trans_snap);
}

esp_err_t _ss_spi_flash_hal_device_config(spi_flash_host_inst_t *host)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    return spi_flash_hal_device_config(tee_host);
}

void _ss_spi_flash_hal_erase_block(spi_flash_host_inst_t *host, uint32_t start_address)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    bool valid_addr = is_flash_erase_ok(start_address, FLASH_BLOCK_SIZE, FLASH_ADDR_24BIT_LIMIT);
    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, start_address);
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    spi_flash_hal_erase_block(tee_host, start_address);
}

void _ss_spi_flash_hal_erase_sector(spi_flash_host_inst_t *host, uint32_t start_address)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    bool valid_addr = is_flash_erase_ok(start_address, FLASH_SECTOR_SIZE, FLASH_ADDR_24BIT_LIMIT);
    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, start_address);
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    spi_flash_hal_erase_sector(tee_host, start_address);
}

void _ss_spi_flash_hal_program_page(spi_flash_host_inst_t *host, const void *buffer, uint32_t address, uint32_t length)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    bool valid_addr = (length <= SPI_FLASH_HAL_MAX_WRITE_BYTES &&
                       is_flash_addr_writable(address, length, FLASH_ADDR_24BIT_LIMIT) &&
                       esp_tee_buf_in_ree(buffer, length));
    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, address);
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    spi_flash_hal_program_page(tee_host, buffer, address, length);
}

esp_err_t _ss_spi_flash_hal_read(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    uint32_t addr_limit = spi1_read_addr_limit();
    bool valid_addr = (addr_limit != 0 && read_len <= SPI_FLASH_HAL_MAX_READ_BYTES &&
                       is_flash_addr_readable(address, read_len, addr_limit) &&
                       esp_tee_buf_in_ree(buffer, read_len));
    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, address);
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_hal_read(tee_host, buffer, address, read_len);
}

void _ss_spi_flash_hal_resume(spi_flash_host_inst_t *host)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    spi_flash_hal_resume(tee_host);
}

esp_err_t _ss_spi_flash_hal_set_write_protect(spi_flash_host_inst_t *host, bool wp)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    return spi_flash_hal_set_write_protect(tee_host, wp);
}

esp_err_t _ss_spi_flash_hal_setup_read_suspend(spi_flash_host_inst_t *host, const spi_flash_sus_cmd_conf *sus_conf)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    if (!esp_tee_buf_in_ree(sus_conf, sizeof(spi_flash_sus_cmd_conf))) {
        return ESP_ERR_INVALID_ARG;
    }

    const spi_flash_sus_cmd_conf sus_snap = *sus_conf;
    return spi_flash_hal_setup_read_suspend(tee_host, &sus_snap);
}

bool _ss_spi_flash_hal_supports_direct_read(spi_flash_host_inst_t *host, const void *p)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return false;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    if (!esp_tee_ptr_in_ree(p)) {
        return false;
    }

    return spi_flash_hal_supports_direct_read(tee_host, p);
}

bool _ss_spi_flash_hal_supports_direct_write(spi_flash_host_inst_t *host, const void *p)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return false;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    if (!esp_tee_ptr_in_ree(p)) {
        return false;
    }

    return spi_flash_hal_supports_direct_write(tee_host, p);
}

void _ss_spi_flash_hal_suspend(spi_flash_host_inst_t *host)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    spi_flash_hal_suspend(tee_host);
}

/* ---------------------------------------------- SPI Flash Extras ------------------------------------------------- */

extern uint32_t bootloader_flash_execute_command_common(uint8_t command, uint32_t addr_len, uint32_t address,
                                                        uint8_t dummy_len, uint8_t mosi_len, uint32_t mosi_data,
                                                        uint8_t miso_len);

static inline bool is_bootloader_cmd_allowed(uint8_t command)
{
    switch (command) {
    case CMD_WRSR3:      /* 0x11 - write status register 3 */
    case CMD_RDSR3:      /* 0x15 - read status register 3 */
    case CMD_WRENVSR:    /* 0x50 - write enable for volatile status-register bits */
    case CMD_WRAP:       /* 0x77 - set burst-with-wrap configuration */
    case CMD_RDID:       /* 0x9F - read JEDEC manufacturer/device ID */
    case CMD_HPMEN:      /* 0xA3 - enable high-performance mode */
    case CMD_BURST_RD:   /* 0xC0 - set read-burst parameters */
        return true;
    default:
        return false;
    }
}

uint32_t _ss_bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len)
{
    if (!is_bootloader_cmd_allowed(command)) {
        ESP_LOGD(TAG, "[%s] Disallowed flash command 0x%02x from REE", __func__, command);
        return 0;
    }
    ESP_FAULT_ASSERT(is_bootloader_cmd_allowed(command));

    /* NOTE: allowed opcodes take no address; lengths are in bits, capped to the callee's 4-byte buffers */
    if (addr_len != 0U || mosi_len > 32U || miso_len > 32U) {
        return 0;
    }

    return bootloader_flash_execute_command_common(command, addr_len, address, dummy_len,
                                                   mosi_len, mosi_data, miso_len);
}

esp_err_t _ss_memspi_host_flush_cache(spi_flash_host_inst_t *host, uint32_t addr, uint32_t size)
{
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    if (esp_tee_flash_check_prange_in_tee_region(addr, size)) {
        return ESP_ERR_INVALID_ARG;
    }

    return memspi_host_flush_cache(tee_host, addr, size);
}

esp_err_t _ss_spi_flash_chip_generic_config_host_io_mode(esp_flash_t *chip, uint32_t flags)
{
    if (!esp_tee_buf_in_ree(chip, sizeof(struct esp_flash_t))) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_flash_host_inst_t *const host = chip->host;
    spi_flash_hal_context_t host_snap;
    spi_flash_host_inst_t *tee_host = tee_own_host(host, &host_snap);
    if (tee_host == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(tee_host != NULL);

    esp_flash_t chip_snap = {
        .host          = tee_host,
        .read_mode     = chip->read_mode,
        .hpm_dummy_ena = chip->hpm_dummy_ena,
    };

    return spi_flash_chip_generic_config_host_io_mode(&chip_snap, flags);
}

#if CONFIG_IDF_TARGET_ESP32C5
void _ss_mspi_timing_psram_tuning(void)
{
    mspi_timing_psram_tuning();
}

void _ss_mspi_timing_flash_tuning(void)
{
    mspi_timing_flash_tuning();
}

void _ss_mspi_timing_enter_low_speed_mode(bool control_spi1)
{
    mspi_timing_enter_low_speed_mode(control_spi1);
}

void _ss_mspi_timing_enter_high_speed_mode(bool control_spi1)
{
    mspi_timing_enter_high_speed_mode(control_spi1);
}

void _ss_mspi_timing_enter_low_speed_early(void)
{
    mspi_timing_enter_low_speed_early();
}

void _ss_mspi_timing_enter_high_speed_early(void)
{
    mspi_timing_enter_high_speed_early();
}

void _ss_mspi_timing_change_speed_mode_cache_safe(bool switch_down)
{
    mspi_timing_change_speed_mode_cache_safe(switch_down);
}

void _ss_spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config)
{
    bool valid_addr = esp_tee_buf_in_ree(out_timing_config, sizeof(spi_flash_hal_timing_config_t));

    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    spi_timing_get_flash_timing_param(out_timing_config);
}
#endif
