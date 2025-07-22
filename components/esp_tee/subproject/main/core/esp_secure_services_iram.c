/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_fault.h"

#include "hal/mmu_types.h"
#include "hal/mmu_hal.h"
#include "hal/wdt_hal.h"

#include "hal/spi_flash_hal.h"
#include "hal/spi_flash_types.h"
#include "spi_flash_chip_generic.h"
#include "memspi_host_driver.h"
#include "esp_flash.h"

#include "esp_tee.h"
#include "esp_tee_memory_utils.h"
#include "esp_tee_intr.h"
#include "esp_tee_rv_utils.h"

#include "esp_tee_flash.h"
#include "esp_tee_sec_storage.h"

static __attribute__((unused)) const char *TAG = "esp_tee_sec_srv_iram";

/* ---------------------------------------------- Interrupts ------------------------------------------------- */

void _ss_esp_rom_route_intr_matrix(int cpu_no, uint32_t model_num, uint32_t intr_num)
{
    return esp_tee_route_intr_matrix(cpu_no, model_num, intr_num);
}

void _ss_rv_utils_intr_enable(uint32_t intr_mask)
{
    rv_utils_tee_intr_enable(intr_mask);
}

void _ss_rv_utils_intr_disable(uint32_t intr_mask)
{
    rv_utils_tee_intr_disable(intr_mask);
}

void _ss_rv_utils_intr_set_priority(int rv_int_num, int priority)
{
    rv_utils_tee_intr_set_priority(rv_int_num, priority);
}

void _ss_rv_utils_intr_set_type(int intr_num, enum intr_type type)
{
    rv_utils_tee_intr_set_type(intr_num, type);
}

void _ss_rv_utils_intr_set_threshold(int priority_threshold)
{
    rv_utils_tee_intr_set_threshold(priority_threshold);
}

void _ss_rv_utils_intr_edge_ack(uint32_t intr_num)
{
    rv_utils_tee_intr_edge_ack(intr_num);
}

void _ss_rv_utils_intr_global_enable(void)
{
    rv_utils_tee_intr_global_enable();
}

/* ---------------------------------------------- RTC_WDT ------------------------------------------------- */

void _ss_wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr)
{
    wdt_hal_init(hal, wdt_inst, prescaler, enable_intr);
}

void _ss_wdt_hal_deinit(wdt_hal_context_t *hal)
{
    wdt_hal_deinit(hal);
}

/* ---------------------------------------------- Secure Storage ------------------------------------------------- */

esp_err_t _ss_esp_tee_sec_storage_ecdsa_sign(const esp_tee_sec_storage_key_cfg_t *cfg, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)hash) && esp_tee_ptr_in_ree((void *)out_sign)) &&
                       (esp_tee_ptr_in_ree((void *)(hash + hlen)) &&
                        esp_tee_ptr_in_ree((void *)((char *)out_sign + sizeof(esp_tee_sec_storage_ecdsa_sign_t)))));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_ecdsa_sign(cfg, hash, hlen, out_sign);
}

esp_err_t _ss_esp_tee_sec_storage_ecdsa_get_pubkey(const esp_tee_sec_storage_key_cfg_t *cfg, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)out_pubkey)) &&
                       (esp_tee_ptr_in_ree((void *)((char *)out_pubkey + sizeof(esp_tee_sec_storage_ecdsa_pubkey_t)))));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_ecdsa_get_pubkey(cfg, out_pubkey);
}

esp_err_t _ss_esp_tee_sec_storage_aead_encrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, uint8_t *tag, size_t tag_len, uint8_t *output)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)ctx->input) &&
                       esp_tee_ptr_in_ree((void *)tag) &&
                       esp_tee_ptr_in_ree((void *)output));

    valid_addr &= (esp_tee_ptr_in_ree((void *)(ctx->input + ctx->input_len)) &&
                   esp_tee_ptr_in_ree((void *)(tag + tag_len)) &&
                   esp_tee_ptr_in_ree((void *)(output + ctx->input_len)));

    if (ctx->aad) {
        valid_addr &= (esp_tee_ptr_in_ree((void *)ctx->aad) && esp_tee_ptr_in_ree((void *)(ctx->aad + ctx->aad_len)));
    }

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_aead_encrypt(ctx, tag, tag_len, output);
}

esp_err_t _ss_esp_tee_sec_storage_aead_decrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, const uint8_t *tag, size_t tag_len, uint8_t *output)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)ctx->input) &&
                       esp_tee_ptr_in_ree((void *)tag) &&
                       esp_tee_ptr_in_ree((void *)output));

    valid_addr &= (esp_tee_ptr_in_ree((void *)(ctx->input + ctx->input_len)) &&
                   esp_tee_ptr_in_ree((void *)(tag + tag_len)) &&
                   esp_tee_ptr_in_ree((void *)(output + ctx->input_len)));

    if (ctx->aad) {
        valid_addr &= (esp_tee_ptr_in_ree((void *)ctx->aad) && esp_tee_ptr_in_ree((void *)(ctx->aad + ctx->aad_len)));
    }

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_aead_decrypt(ctx, tag, tag_len, output);
}

/* ---------------------------------------------- MMU HAL ------------------------------------------------- */

void _ss_mmu_hal_map_region(uint32_t mmu_id, mmu_target_t mem_type, uint32_t vaddr,
                            uint32_t paddr, uint32_t len, uint32_t *out_len)
{
    bool vaddr_chk = esp_tee_flash_check_vaddr_in_tee_region(vaddr);
    bool paddr_chk = esp_tee_flash_check_paddr_in_tee_region(paddr);
    if (vaddr_chk || paddr_chk) {
        return;
    }
    ESP_FAULT_ASSERT(!vaddr_chk && !paddr_chk);

    mmu_hal_map_region(mmu_id, mem_type, vaddr, paddr, len, out_len);
}

void _ss_mmu_hal_unmap_region(uint32_t mmu_id, uint32_t vaddr, uint32_t len)
{
    bool vaddr_chk = esp_tee_flash_check_vaddr_in_tee_region(vaddr);
    if (vaddr_chk) {
        return;
    }
    ESP_FAULT_ASSERT(!vaddr_chk);

    mmu_hal_unmap_region(mmu_id, vaddr, len);
}

bool _ss_mmu_hal_vaddr_to_paddr(uint32_t mmu_id, uint32_t vaddr, uint32_t *out_paddr, mmu_target_t *out_target)
{
    bool vaddr_chk = esp_tee_flash_check_vaddr_in_tee_region(vaddr);
    if (vaddr_chk) {
        return false;
    }
    ESP_FAULT_ASSERT(!vaddr_chk);
    return mmu_hal_vaddr_to_paddr(mmu_id, vaddr, out_paddr, out_target);
}

bool _ss_mmu_hal_paddr_to_vaddr(uint32_t mmu_id, uint32_t paddr, mmu_target_t target, mmu_vaddr_t type, uint32_t *out_vaddr)
{
    bool paddr_chk = esp_tee_flash_check_paddr_in_tee_region(paddr);
    if (paddr_chk) {
        return false;
    }
    ESP_FAULT_ASSERT(!paddr_chk);
    return mmu_hal_paddr_to_vaddr(mmu_id, paddr, target, type, out_vaddr);
}

#if CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1
/* ---------------------------------------------- SPI Flash HAL ------------------------------------------------- */

uint32_t _ss_spi_flash_hal_check_status(spi_flash_host_inst_t *host)
{
    return spi_flash_hal_check_status(host);
}

esp_err_t _ss_spi_flash_hal_common_command(spi_flash_host_inst_t *host, spi_flash_trans_t *trans)
{
    bool paddr_chk = esp_tee_flash_check_paddr_in_tee_region(trans->address);
    if (paddr_chk) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, trans->address);
        return ESP_FAIL;
    }
    return spi_flash_hal_common_command(host, trans);
}

esp_err_t _ss_spi_flash_hal_device_config(spi_flash_host_inst_t *host)
{
    return spi_flash_hal_device_config(host);
}

void _ss_spi_flash_hal_erase_block(spi_flash_host_inst_t *host, uint32_t start_address)
{
    bool paddr_chk = esp_tee_flash_check_paddr_in_tee_region(start_address);
    if (paddr_chk) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, start_address);
        return;
    }
    ESP_FAULT_ASSERT(!paddr_chk);
    spi_flash_hal_erase_block(host, start_address);
}

void _ss_spi_flash_hal_erase_chip(spi_flash_host_inst_t *host)
{
    spi_flash_hal_erase_chip(host);
}

void _ss_spi_flash_hal_erase_sector(spi_flash_host_inst_t *host, uint32_t start_address)
{
    bool paddr_chk = esp_tee_flash_check_paddr_in_tee_region(start_address);
    if (paddr_chk) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, start_address);
        return;
    }
    ESP_FAULT_ASSERT(!paddr_chk);
    spi_flash_hal_erase_sector(host, start_address);
}

void _ss_spi_flash_hal_program_page(spi_flash_host_inst_t *host, const void *buffer, uint32_t address, uint32_t length)
{
    bool paddr_chk = esp_tee_flash_check_paddr_in_tee_region(address);
    if (paddr_chk) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, address);
        return;
    }

    bool buf_addr_chk = ((esp_tee_ptr_in_ree((void *)buffer) && esp_tee_ptr_in_ree((void *)(buffer + length))));
    if (!buf_addr_chk) {
        return;
    }

    ESP_FAULT_ASSERT(!paddr_chk && buf_addr_chk);
    spi_flash_hal_program_page(host, buffer, address, length);
}

esp_err_t _ss_spi_flash_hal_read(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len)
{
    bool paddr_chk = esp_tee_flash_check_paddr_in_tee_region(address);
    if (paddr_chk) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, address);
        return ESP_FAIL;
    }

    bool buf_addr_chk = ((esp_tee_ptr_in_ree((void *)buffer) && esp_tee_ptr_in_ree((void *)(buffer + read_len))));
    if (!buf_addr_chk) {
        return ESP_FAIL;
    }

    ESP_FAULT_ASSERT(!paddr_chk && buf_addr_chk);
    return spi_flash_hal_read(host, buffer, address, read_len);
}

void _ss_spi_flash_hal_resume(spi_flash_host_inst_t *host)
{
    spi_flash_hal_resume(host);
}

esp_err_t _ss_spi_flash_hal_set_write_protect(spi_flash_host_inst_t *host, bool wp)
{
    return spi_flash_hal_set_write_protect(host, wp);
}

esp_err_t _ss_spi_flash_hal_setup_read_suspend(spi_flash_host_inst_t *host, const spi_flash_sus_cmd_conf *sus_conf)
{
    return spi_flash_hal_setup_read_suspend(host, sus_conf);
}

bool _ss_spi_flash_hal_supports_direct_read(spi_flash_host_inst_t *host, const void *p)
{
    return spi_flash_hal_supports_direct_read(host, p);
}

bool _ss_spi_flash_hal_supports_direct_write(spi_flash_host_inst_t *host, const void *p)
{
    return spi_flash_hal_supports_direct_write(host, p);
}

void _ss_spi_flash_hal_suspend(spi_flash_host_inst_t *host)
{
    spi_flash_hal_suspend(host);
}

/* ---------------------------------------------- SPI Flash Extras ------------------------------------------------- */

extern uint32_t bootloader_flash_execute_command_common(uint8_t command, uint32_t addr_len, uint32_t address,
                                                        uint8_t dummy_len, uint8_t mosi_len, uint32_t mosi_data,
                                                        uint8_t miso_len);

uint32_t _ss_bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len)
{
    bool paddr_chk = esp_tee_flash_check_paddr_in_tee_region(address);
    if (paddr_chk) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, address);
        return ESP_FAIL;
    }
    ESP_FAULT_ASSERT(!paddr_chk);
    return bootloader_flash_execute_command_common(command, addr_len, address, dummy_len,
                                                   mosi_len, mosi_data, miso_len);
}

esp_err_t _ss_memspi_host_flush_cache(spi_flash_host_inst_t *host, uint32_t addr, uint32_t size)
{
    bool paddr_chk = esp_tee_flash_check_paddr_in_tee_region(addr);
    if (paddr_chk) {
        return ESP_FAIL;
    }
    ESP_FAULT_ASSERT(!paddr_chk);
    return memspi_host_flush_cache(host, addr, size);
}

esp_err_t _ss_spi_flash_chip_generic_config_host_io_mode(esp_flash_t *chip, uint32_t flags)
{
    return spi_flash_chip_generic_config_host_io_mode(chip, flags);
}
#endif
