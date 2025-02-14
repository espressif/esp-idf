/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>

#include "esp_cpu.h"
#include "esp_efuse.h"
#include "esp_fault.h"
#include "esp_flash.h"
#include "esp_flash_encrypt.h"
#include "esp_rom_efuse.h"

#include "hal/efuse_hal.h"
#include "hal/mmu_types.h"
#include "hal/mmu_hal.h"
#include "hal/wdt_hal.h"
#include "hal/sha_hal.h"

#include "hal/spi_flash_hal.h"
#include "hal/spi_flash_types.h"
#include "spi_flash_chip_generic.h"
#include "memspi_host_driver.h"

#include "soc/soc_caps.h"
#include "aes/esp_aes.h"
#include "sha/sha_core.h"

#include "esp_tee.h"
#include "esp_tee_memory_utils.h"
#include "esp_tee_intr.h"
#include "esp_tee_aes_intr.h"
#include "esp_tee_rv_utils.h"

#include "esp_tee_flash.h"
#include "esp_tee_sec_storage.h"
#include "esp_tee_ota_ops.h"
#include "esp_attestation.h"

static __attribute__((unused)) const char *TAG = "esp_tee_sec_srv";

void _ss_invalid_secure_service(void)
{
    assert(0);
}

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

/* ---------------------------------------------- eFuse ------------------------------------------------- */

uint32_t _ss_efuse_hal_chip_revision(void)
{
    return efuse_hal_chip_revision();
}

uint32_t _ss_efuse_hal_get_chip_ver_pkg(void)
{
    return efuse_hal_get_chip_ver_pkg();
}

bool _ss_efuse_hal_get_disable_wafer_version_major(void)
{
    return efuse_hal_get_disable_wafer_version_major();
}

void _ss_efuse_hal_get_mac(uint8_t *mac)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)mac)) &
                       (esp_tee_ptr_in_ree((void *)(mac + 6))));

    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    efuse_hal_get_mac(mac);
}

bool _ss_esp_efuse_check_secure_version(uint32_t secure_version)
{
    return esp_efuse_check_secure_version(secure_version);
}

esp_err_t _ss_esp_efuse_read_field_blob(const esp_efuse_desc_t *field[], void *dst, size_t dst_size_bits)
{
    if ((field != NULL) && (field[0]->efuse_block >= EFUSE_BLK4)) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_efuse_read_field_blob(field, dst, dst_size_bits);
}

bool _ss_esp_flash_encryption_enabled(void)
{
    uint32_t flash_crypt_cnt = 0;
#ifndef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    flash_crypt_cnt = efuse_ll_get_flash_crypt_cnt();
#else
    esp_efuse_read_field_blob(ESP_EFUSE_SPI_BOOT_CRYPT_CNT, &flash_crypt_cnt, ESP_EFUSE_SPI_BOOT_CRYPT_CNT[0]->bit_count)    ;
#endif
    /* __builtin_parity is in flash, so we calculate parity inline */
    bool enabled = false;
    while (flash_crypt_cnt) {
        if (flash_crypt_cnt & 1) {
            enabled = !enabled;
        }
        flash_crypt_cnt >>= 1;
    }
    return enabled;
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

/* ---------------------------------------------- AES ------------------------------------------------- */

void _ss_esp_aes_intr_alloc(void)
{
    esp_tee_aes_intr_alloc();
}

int _ss_esp_aes_crypt_cbc(esp_aes_context *ctx,
                          int mode,
                          size_t length,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_cbc(ctx, mode, length, iv, input, output);
}

int _ss_esp_aes_crypt_cfb128(esp_aes_context *ctx,
                             int mode,
                             size_t length,
                             size_t *iv_off,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_cfb128(ctx, mode, length, iv_off, iv, input, output);
}

int _ss_esp_aes_crypt_cfb8(esp_aes_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_cfb8(ctx, mode, length, iv, input, output);
}

int _ss_esp_aes_crypt_ctr(esp_aes_context *ctx,
                          size_t length,
                          size_t *nc_off,
                          unsigned char nonce_counter[16],
                          unsigned char stream_block[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_ctr(ctx, length, nc_off, nonce_counter, stream_block, input, output);
}

int _ss_esp_aes_crypt_ecb(esp_aes_context *ctx,
                          int mode,
                          const unsigned char input[16],
                          unsigned char output[16])
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &
                       (esp_tee_ptr_in_ree((void *)(input + 16)) && esp_tee_ptr_in_ree((void *)(output + 16))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_ecb(ctx, mode, input, output);
}

int _ss_esp_aes_crypt_ofb(esp_aes_context *ctx,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_ofb(ctx, length, iv_off, iv, input, output);
}

/* ---------------------------------------------- SHA ------------------------------------------------- */

void _ss_esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &
                       (esp_tee_ptr_in_ree((void *)(input + ilen))));

    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    esp_sha(sha_type, input, ilen, output);
}

int _ss_esp_sha_dma(esp_sha_type sha_type, const void *input, uint32_t ilen,
                    const void *buf, uint32_t buf_len, bool is_first_block)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)input) &&
                       esp_tee_ptr_in_ree((void *)((char *)input + ilen)));
    if (buf_len) {
        valid_addr &= (esp_tee_ptr_in_ree((void *)buf) &&
                       esp_tee_ptr_in_ree((void *)((char *)buf + buf_len)));
    }

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_sha_dma(sha_type, input, ilen, buf, buf_len, is_first_block);
}

void _ss_esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    sha_hal_read_digest(sha_type, digest_state);
}

void _ss_esp_sha_write_digest_state(esp_sha_type sha_type, void *digest_state)
{
    sha_hal_write_digest(sha_type, digest_state);
}

void _ss_esp_sha_block(esp_sha_type sha_type, const void *data_block, bool is_first_block)
{
    esp_sha_block(sha_type, data_block, is_first_block);
}

/* ---------------------------------------------- OTA ------------------------------------------------- */

int _ss_esp_tee_ota_begin(void)
{
    return esp_tee_ota_begin();
}

int _ss_esp_tee_ota_write(uint32_t rel_offset, void *data, size_t size)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)data)) &
                       (esp_tee_ptr_in_ree((void *)((char *)data + size))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_ota_write(rel_offset, data, size);
}

int _ss_esp_tee_ota_end(void)
{
    return esp_tee_ota_end();
}

/* ---------------------------------------------- Secure Storage ------------------------------------------------- */

esp_err_t _ss_esp_tee_sec_storage_init(void)
{
    return esp_tee_sec_storage_init();
}

esp_err_t _ss_esp_tee_sec_storage_gen_key(uint16_t slot_id, uint8_t key_type)
{
    return esp_tee_sec_storage_gen_key(slot_id, key_type);
}

esp_err_t _ss_esp_tee_sec_storage_get_signature(uint16_t slot_id, uint8_t *hash, size_t hlen, esp_tee_sec_storage_sign_t *out_sign)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)hash) && esp_tee_ptr_in_ree((void *)out_sign)) &
                       (esp_tee_ptr_in_ree((void *)(hash + hlen)) &&
                        esp_tee_ptr_in_ree((void *)((char *)out_sign + sizeof(esp_tee_sec_storage_sign_t)))));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_get_signature(slot_id, hash, hlen, out_sign);
}

esp_err_t _ss_esp_tee_sec_storage_get_pubkey(uint16_t slot_id, esp_tee_sec_storage_pubkey_t *pubkey)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)pubkey)) &
                       (esp_tee_ptr_in_ree((void *)((char *)pubkey + sizeof(esp_tee_sec_storage_pubkey_t)))));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_get_pubkey(slot_id, pubkey);
}

esp_err_t _ss_esp_tee_sec_storage_encrypt(uint16_t slot_id, uint8_t *input, uint8_t len, uint8_t *aad,
                                          uint16_t aad_len, uint8_t *tag, uint16_t tag_len, uint8_t *output)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)input) &&
                       esp_tee_ptr_in_ree((void *)tag) &&
                       esp_tee_ptr_in_ree((void *)output));

    valid_addr &= (esp_tee_ptr_in_ree((void *)(input + len)) &&
                   esp_tee_ptr_in_ree((void *)(tag + tag_len)) &&
                   esp_tee_ptr_in_ree((void *)(output + len)));

    if (aad) {
        valid_addr &= (esp_tee_ptr_in_ree((void *)aad) && esp_tee_ptr_in_ree((void *)(aad + aad_len)));
    }

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_encrypt(slot_id, input, len, aad, aad_len, tag, tag_len, output);
}

esp_err_t _ss_esp_tee_sec_storage_decrypt(uint16_t slot_id, uint8_t *input, uint8_t len, uint8_t *aad,
                                          uint16_t aad_len, uint8_t *tag, uint16_t tag_len, uint8_t *output)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)input) &&
                       esp_tee_ptr_in_ree((void *)tag) &&
                       esp_tee_ptr_in_ree((void *)output));

    valid_addr &= (esp_tee_ptr_in_ree((void *)(input + len)) &&
                   esp_tee_ptr_in_ree((void *)(tag + tag_len)) &&
                   esp_tee_ptr_in_ree((void *)(output + len)));

    if (aad) {
        valid_addr &= (esp_tee_ptr_in_ree((void *)aad) && esp_tee_ptr_in_ree((void *)(aad + aad_len)));
    }

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_decrypt(slot_id, input, len, aad, aad_len, tag, tag_len, output);
}

bool _ss_esp_tee_sec_storage_is_slot_empty(uint16_t slot_id)
{
    return esp_tee_sec_storage_is_slot_empty(slot_id);
}

esp_err_t _ss_esp_tee_sec_storage_clear_slot(uint16_t slot_id)
{
    return esp_tee_sec_storage_clear_slot(slot_id);
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
