/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
#include "rom/cache.h"

#include "hal/spi_flash_hal.h"
#include "hal/spi_flash_types.h"
#include "esp_flash_chips/spi_flash_chip_generic.h"
#include "esp_private/memspi_host_driver.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_flash.h"
#include "esp_flash_chips/esp_flash_types.h"
#include "riscv/rv_utils.h"

#include "esp_tee.h"
#include "esp_tee_memory_utils.h"
#include "esp_tee_intr.h"
#include "esp_tee_rv_utils.h"

#include "nvs.h"
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
    rv_utils_intr_edge_ack(intr_num);
}

void _ss_rv_utils_intr_global_enable(void)
{
    rv_utils_tee_intr_global_enable();
}

uint32_t _ss_rv_utils_intr_get_enabled_mask(void)
{
    return rv_utils_intr_get_enabled_mask();
}

void _ss_rv_utils_set_cycle_count(uint32_t ccount)
{
    rv_utils_set_cycle_count(ccount);
}

#if SOC_BRANCH_PREDICTOR_SUPPORTED
void _ss_rv_utils_en_branch_predictor(void)
{
    rv_utils_en_branch_predictor();
}

void _ss_rv_utils_dis_branch_predictor(void)
{
    rv_utils_dis_branch_predictor();
}
#endif

#if SOC_CPU_SUPPORT_WFE
void _ss_rv_utils_wfe_mode_enable(bool en)
{
    rv_utils_wfe_mode_enable(en);
}
#endif

#if SOC_INT_CLIC_SUPPORTED
void _ss_esprv_int_set_vectored(int rv_int_num, bool vectored)
{
    esprv_int_set_vectored(rv_int_num, vectored);
}
#endif

/* ---------------------------------------------- RTC_WDT ------------------------------------------------- */

void _ss_wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr)
{
    bool valid_addr = esp_tee_buf_in_ree(hal, sizeof(wdt_hal_context_t));

    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    wdt_hal_init(hal, wdt_inst, prescaler, enable_intr);
}

void _ss_wdt_hal_deinit(wdt_hal_context_t *hal)
{
    bool valid_addr = esp_tee_buf_in_ree(hal, sizeof(wdt_hal_context_t));

    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    wdt_hal_deinit(hal);
}

/* ---------------------------------------------- Secure Storage ------------------------------------------------- */

esp_err_t _ss_esp_tee_sec_storage_ecdsa_sign(const esp_tee_sec_storage_key_cfg_t *cfg, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign)
{
    bool valid_addr = (esp_tee_buf_in_ree(cfg, sizeof(esp_tee_sec_storage_key_cfg_t)) &&
                       esp_tee_buf_in_ree(hash, hlen) &&
                       esp_tee_buf_in_ree(out_sign, sizeof(esp_tee_sec_storage_ecdsa_sign_t)));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_ecdsa_sign(cfg, hash, hlen, out_sign);
}

esp_err_t _ss_esp_tee_sec_storage_ecdsa_get_pubkey(const esp_tee_sec_storage_key_cfg_t *cfg, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey)
{
    bool valid_addr = (esp_tee_buf_in_ree(cfg, sizeof(esp_tee_sec_storage_key_cfg_t)) &&
                       esp_tee_buf_in_ree(out_pubkey, sizeof(esp_tee_sec_storage_ecdsa_pubkey_t)));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_ecdsa_get_pubkey(cfg, out_pubkey);
}

esp_err_t _ss_esp_tee_sec_storage_aead_encrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, uint8_t *iv, size_t iv_len, uint8_t *tag, size_t tag_len, uint8_t *output)
{
    bool valid_addr = (esp_tee_buf_in_ree(ctx, sizeof(esp_tee_sec_storage_aead_ctx_t)) &&
                       esp_tee_buf_in_ree(ctx->input, ctx->input_len) &&
                       esp_tee_buf_in_ree(iv, iv_len) &&
                       esp_tee_buf_in_ree(tag, tag_len) &&
                       esp_tee_buf_in_ree(output, ctx->input_len));

    if (ctx->aad_len != 0) {
        valid_addr &= esp_tee_buf_in_ree(ctx->aad, ctx->aad_len);
    }

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_aead_encrypt(ctx, iv, iv_len, tag, tag_len, output);
}

esp_err_t _ss_esp_tee_sec_storage_aead_decrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, const uint8_t *iv, size_t iv_len, const uint8_t *tag, size_t tag_len, uint8_t *output)
{
    bool valid_addr = (esp_tee_buf_in_ree(ctx, sizeof(esp_tee_sec_storage_aead_ctx_t)) &&
                       esp_tee_buf_in_ree(ctx->input, ctx->input_len) &&
                       esp_tee_buf_in_ree(iv, iv_len) &&
                       esp_tee_buf_in_ree(tag, tag_len) &&
                       esp_tee_buf_in_ree(output, ctx->input_len));

    if (ctx->aad_len != 0) {
        valid_addr &= esp_tee_buf_in_ree(ctx->aad, ctx->aad_len);
    }

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_aead_decrypt(ctx, iv, iv_len, tag, tag_len, output);
}

esp_err_t _ss_esp_tee_sec_storage_ecdsa_sign_pbkdf2(const esp_tee_sec_storage_pbkdf2_ctx_t *ctx, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey)
{
    bool valid_addr = (esp_tee_buf_in_ree(ctx, sizeof(esp_tee_sec_storage_pbkdf2_ctx_t)) &&
                       esp_tee_buf_in_ree(hash, hlen) &&
                       esp_tee_buf_in_ree(out_sign, sizeof(esp_tee_sec_storage_ecdsa_sign_t)) &&
                       esp_tee_buf_in_ree(out_pubkey, sizeof(esp_tee_sec_storage_ecdsa_pubkey_t)) &&
                       esp_tee_buf_in_ree(ctx->salt, ctx->salt_len));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_ecdsa_sign_pbkdf2(ctx, hash, hlen, out_sign, out_pubkey);
}

/* ---------------------------------------------- MMU HAL ------------------------------------------------- */

void _ss_mmu_hal_map_region(uint32_t mmu_id, mmu_target_t mem_type, uint32_t vaddr,
                            uint32_t paddr, uint32_t len, uint32_t *out_len)
{
    bool valid_addr = (!esp_tee_flash_check_vrange_in_tee_region(vaddr, len) &&
                       !esp_tee_flash_check_prange_in_tee_region(paddr, len) &&
                       esp_tee_buf_in_ree(out_len, sizeof(uint32_t)));

    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x | 0x%08x", __func__, vaddr, paddr);
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    mmu_hal_map_region(mmu_id, mem_type, vaddr, paddr, len, out_len);
}

void _ss_mmu_hal_unmap_region(uint32_t mmu_id, uint32_t vaddr, uint32_t len)
{
    bool vaddr_chk = esp_tee_flash_check_vrange_in_tee_region(vaddr, len);

    if (vaddr_chk) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, vaddr);
        return;
    }
    ESP_FAULT_ASSERT(!vaddr_chk);

    mmu_hal_unmap_region(mmu_id, vaddr, len);
}

bool _ss_mmu_hal_vaddr_to_paddr(uint32_t mmu_id, uint32_t vaddr, uint32_t *out_paddr, mmu_target_t *out_target)
{
    bool valid_addr = (!esp_tee_flash_check_vaddr_in_tee_region(vaddr) &&
                       esp_tee_buf_in_ree(out_paddr, sizeof(uint32_t)) &&
                       esp_tee_buf_in_ree(out_target, sizeof(mmu_target_t)));

    if (!valid_addr) {
        return false;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return mmu_hal_vaddr_to_paddr(mmu_id, vaddr, out_paddr, out_target);
}

bool _ss_mmu_hal_paddr_to_vaddr(uint32_t mmu_id, uint32_t paddr, mmu_target_t target, mmu_vaddr_t type, uint32_t *out_vaddr)
{
    bool valid_addr = (!esp_tee_flash_check_paddr_in_tee_region(paddr) &&
                       esp_tee_buf_in_ree(out_vaddr, sizeof(uint32_t)));

    if (!valid_addr) {
        return false;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return mmu_hal_paddr_to_vaddr(mmu_id, paddr, target, type, out_vaddr);
}

#if CONFIG_IDF_TARGET_ESP32C5
void _ss_Cache_Set_IDROM_MMU_Size(uint32_t irom_size, uint32_t drom_size)
{
    Cache_Set_IDROM_MMU_Size(irom_size, drom_size);
}
#endif

#if CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1
/* ---------------------------------------------- SPI Flash HAL ------------------------------------------------- */

static bool is_spi_host_in_ree(spi_flash_host_inst_t *host)
{
    return esp_tee_buf_in_ree(host, sizeof(spi_flash_host_inst_t));
}

static bool is_spi_trans_valid(spi_flash_host_inst_t *host, spi_flash_trans_t *trans)
{
    bool valid_addr = (is_spi_host_in_ree(host) &&
                       esp_tee_buf_in_ree(trans, sizeof(spi_flash_trans_t)));

    if (trans->mosi_len != 0) {
        valid_addr &= esp_tee_buf_in_ree(trans->mosi_data, trans->mosi_len);
    }
    if (trans->miso_len != 0) {
        valid_addr &= esp_tee_buf_in_ree(trans->miso_data, trans->miso_len);
    }
    return valid_addr;
}

uint32_t _ss_spi_flash_hal_check_status(spi_flash_host_inst_t *host)
{
    bool valid_addr = is_spi_host_in_ree(host);

    if (!valid_addr) {
        return 0;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_hal_check_status(host);
}

esp_err_t _ss_spi_flash_hal_common_command(spi_flash_host_inst_t *host, spi_flash_trans_t *trans)
{
    bool valid_addr = (is_spi_trans_valid(host, trans) &&
                       !esp_tee_flash_check_prange_in_tee_region(trans->address, trans->mosi_len) &&
                       !esp_tee_flash_check_prange_in_tee_region(trans->address, trans->miso_len));

    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, trans->address);
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_hal_common_command(host, trans);
}

esp_err_t _ss_spi_flash_hal_device_config(spi_flash_host_inst_t *host)
{
    bool valid_addr = is_spi_host_in_ree(host);

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_hal_device_config(host);
}

void _ss_spi_flash_hal_erase_block(spi_flash_host_inst_t *host, uint32_t start_address)
{
    bool valid_addr = (is_spi_host_in_ree(host) &&
                       !esp_tee_flash_check_paddr_in_tee_region(start_address));

    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, start_address);
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    spi_flash_hal_erase_block(host, start_address);
}

void _ss_spi_flash_hal_erase_sector(spi_flash_host_inst_t *host, uint32_t start_address)
{
    bool valid_addr = (is_spi_host_in_ree(host) &&
                       !esp_tee_flash_check_prange_in_tee_region(start_address, FLASH_SECTOR_SIZE));

    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, start_address);
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    spi_flash_hal_erase_sector(host, start_address);
}

void _ss_spi_flash_hal_program_page(spi_flash_host_inst_t *host, const void *buffer, uint32_t address, uint32_t length)
{
    bool valid_addr = (is_spi_host_in_ree(host) &&
                       !esp_tee_flash_check_prange_in_tee_region(address, length) &&
                       esp_tee_buf_in_ree(buffer, length));

    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, address);
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    spi_flash_hal_program_page(host, buffer, address, length);
}

esp_err_t _ss_spi_flash_hal_read(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len)
{
    bool valid_addr = (is_spi_host_in_ree(host) &&
                       !esp_tee_flash_check_prange_in_tee_region(address, read_len) &&
                       esp_tee_buf_in_ree(buffer, read_len));

    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, address);
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_hal_read(host, buffer, address, read_len);
}

void _ss_spi_flash_hal_resume(spi_flash_host_inst_t *host)
{
    bool valid_addr = is_spi_host_in_ree(host);

    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    spi_flash_hal_resume(host);
}

esp_err_t _ss_spi_flash_hal_set_write_protect(spi_flash_host_inst_t *host, bool wp)
{
    bool valid_addr = is_spi_host_in_ree(host);

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_hal_set_write_protect(host, wp);
}

esp_err_t _ss_spi_flash_hal_setup_read_suspend(spi_flash_host_inst_t *host, const spi_flash_sus_cmd_conf *sus_conf)
{
    bool valid_addr = (is_spi_host_in_ree(host) &&
                       esp_tee_buf_in_ree(sus_conf, sizeof(spi_flash_sus_cmd_conf)));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_hal_setup_read_suspend(host, sus_conf);
}

bool _ss_spi_flash_hal_supports_direct_read(spi_flash_host_inst_t *host, const void *p)
{
    bool valid_addr = (is_spi_host_in_ree(host) && esp_tee_ptr_in_ree(p));

    if (!valid_addr) {
        return false;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_hal_supports_direct_read(host, p);
}

bool _ss_spi_flash_hal_supports_direct_write(spi_flash_host_inst_t *host, const void *p)
{
    bool valid_addr = (is_spi_host_in_ree(host) && esp_tee_ptr_in_ree(p));

    if (!valid_addr) {
        return false;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_hal_supports_direct_write(host, p);
}

void _ss_spi_flash_hal_suspend(spi_flash_host_inst_t *host)
{
    bool valid_addr = is_spi_host_in_ree(host);

    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

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
    bool valid_addr = (!esp_tee_flash_check_prange_in_tee_region(address, mosi_len) &&
                       !esp_tee_flash_check_prange_in_tee_region(address, miso_len));

    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, address);
        return 0;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return bootloader_flash_execute_command_common(command, addr_len, address, dummy_len,
                                                   mosi_len, mosi_data, miso_len);
}

esp_err_t _ss_memspi_host_flush_cache(spi_flash_host_inst_t *host, uint32_t addr, uint32_t size)
{
    bool valid_addr = (is_spi_host_in_ree(host) &&
                       !esp_tee_flash_check_prange_in_tee_region(addr, size));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return memspi_host_flush_cache(host, addr, size);
}

esp_err_t _ss_spi_flash_chip_generic_config_host_io_mode(esp_flash_t *chip, uint32_t flags)
{
    bool valid_addr = esp_tee_buf_in_ree(chip, sizeof(struct esp_flash_t));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return spi_flash_chip_generic_config_host_io_mode(chip, flags);
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
#endif
