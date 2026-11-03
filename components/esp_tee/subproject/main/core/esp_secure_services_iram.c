/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include <string.h>
#include <sys/param.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_macros.h"
#include "esp_fault.h"

#include "hal/mmu_types.h"
#include "hal/mmu_hal.h"
#include "hal/wdt_hal.h"
#include "rom/cache.h"

#include "riscv/rv_utils.h"
#include "soc/soc.h"

#include "esp_tee.h"
#include "esp_tee_memory_utils.h"
#include "esp_tee_intr.h"
#include "esp_tee_rv_utils.h"

#include "nvs.h"
#include "esp_tee_flash.h"
#include "esp_tee_sec_storage.h"

static __attribute__((unused)) const char *TAG = "esp_tee_sec_srv_iram";

/* ---------------------------------------------- Interrupts ------------------------------------------------- */

#if SOC_INT_CLIC_SUPPORTED
#define TEE_RESERVED_INTR_MASK   ((1U << TEE_SECURE_INUM) | (1U << TEE_PASS_INUM))
#else
#define TEE_RESERVED_INTR_MASK   ((1U << TEE_SECURE_INUM))
#endif

static inline bool is_intr_num_invalid(uint32_t intr_num)
{
    return (intr_num >= (uint32_t)SOC_CPU_INTR_NUM) ||
           (((1U << intr_num) & TEE_RESERVED_INTR_MASK) != 0U);
}

void _ss_esp_rom_route_intr_matrix(int cpu_no, uint32_t model_num, uint32_t intr_num)
{
    if (is_intr_num_invalid(intr_num)) {
        return;
    }
    return esp_tee_route_intr_matrix(cpu_no, model_num, intr_num);
}

void _ss_rv_utils_intr_enable(uint32_t intr_mask)
{
    rv_utils_tee_intr_enable(intr_mask & ~TEE_RESERVED_INTR_MASK);
}

void _ss_rv_utils_intr_disable(uint32_t intr_mask)
{
    rv_utils_tee_intr_disable(intr_mask & ~TEE_RESERVED_INTR_MASK);
}

void _ss_rv_utils_intr_set_priority(int rv_int_num, int priority)
{
    if (is_intr_num_invalid((uint32_t)rv_int_num)) {
        return;
    }
    rv_utils_tee_intr_set_priority(rv_int_num, priority);
}

void _ss_rv_utils_intr_set_type(int intr_num, enum intr_type type)
{
    if (is_intr_num_invalid((uint32_t)intr_num)) {
        return;
    }
    rv_utils_tee_intr_set_type(intr_num, type);
}

void _ss_rv_utils_intr_set_threshold(int priority_threshold)
{
    rv_utils_tee_intr_set_threshold(priority_threshold);
}

void _ss_rv_utils_intr_edge_ack(uint32_t intr_num)
{
    if (is_intr_num_invalid(intr_num)) {
        return;
    }
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
    if (is_intr_num_invalid((uint32_t)rv_int_num)) {
        return;
    }
    esprv_int_set_vectored(rv_int_num, vectored);
}
#endif

/* ---------------------------------------------- RTC_WDT ------------------------------------------------- */

static bool is_wdt_dev_valid(const void *dev)
{
    return (dev == (const void *)&TIMERG0)
#if TIMG_LL_GET(INST_NUM) >= 2
           || (dev == (const void *)&TIMERG1)
#endif
           || (dev == (const void *)RWDT_DEV_GET());
}

void _ss_wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr)
{
    bool valid_addr = esp_tee_buf_in_ree(hal, sizeof(wdt_hal_context_t));

    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    wdt_hal_context_t hal_local;
    wdt_hal_init(&hal_local, wdt_inst, prescaler, enable_intr);

    *hal = hal_local;
}

void _ss_wdt_hal_deinit(wdt_hal_context_t *hal)
{
    if (!esp_tee_buf_in_ree(hal, sizeof(wdt_hal_context_t))) {
        return;
    }

    wdt_hal_context_t hal_snap = *hal;

    bool valid_addr = is_wdt_dev_valid(hal_snap.mwdt_dev);
    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    wdt_hal_deinit(&hal_snap);
}

/* ---------------------------------------------- Secure Storage ------------------------------------------------- */

esp_err_t _ss_esp_tee_sec_storage_ecdsa_sign(const esp_tee_sec_storage_key_cfg_t *cfg, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign)
{
    if (!esp_tee_buf_in_ree(cfg, sizeof(esp_tee_sec_storage_key_cfg_t))) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_tee_sec_storage_key_cfg_t cfg_local = *cfg;
    char id_buf[NVS_KEY_NAME_MAX_SIZE];
    if (!tee_snapshot_ree_str(&cfg_local.id, id_buf, sizeof(id_buf))) {
        return ESP_ERR_INVALID_ARG;
    }

    bool valid_arg = (esp_tee_buf_in_ree(hash, hlen) &&
                      esp_tee_buf_in_ree(out_sign, sizeof(esp_tee_sec_storage_ecdsa_sign_t)) &&
                      !esp_tee_sec_storage_is_key_tee_owned(cfg_local.id));
    if (!valid_arg) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_arg);

    return esp_tee_sec_storage_ecdsa_sign(&cfg_local, hash, hlen, out_sign);
}

esp_err_t _ss_esp_tee_sec_storage_ecdsa_get_pubkey(const esp_tee_sec_storage_key_cfg_t *cfg, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey)
{
    if (!esp_tee_buf_in_ree(cfg, sizeof(esp_tee_sec_storage_key_cfg_t))) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_tee_sec_storage_key_cfg_t cfg_local = *cfg;
    char id_buf[NVS_KEY_NAME_MAX_SIZE];
    if (!tee_snapshot_ree_str(&cfg_local.id, id_buf, sizeof(id_buf))) {
        return ESP_ERR_INVALID_ARG;
    }

    bool valid_arg = (esp_tee_buf_in_ree(out_pubkey, sizeof(esp_tee_sec_storage_ecdsa_pubkey_t)) &&
                      !esp_tee_sec_storage_is_key_tee_owned(cfg_local.id));
    if (!valid_arg) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_arg);

    return esp_tee_sec_storage_ecdsa_get_pubkey(&cfg_local, out_pubkey);
}

esp_err_t _ss_esp_tee_sec_storage_aead_encrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, uint8_t *iv, size_t iv_len, uint8_t *tag, size_t tag_len, uint8_t *output)
{
    if (!esp_tee_buf_in_ree(ctx, sizeof(esp_tee_sec_storage_aead_ctx_t))) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_tee_sec_storage_aead_ctx_t ctx_local = *ctx;
    char id_buf[NVS_KEY_NAME_MAX_SIZE];
    if (!tee_snapshot_ree_str(&ctx_local.key_id, id_buf, sizeof(id_buf))) {
        return ESP_ERR_INVALID_ARG;
    }

    bool valid_arg = (esp_tee_buf_in_ree(ctx_local.input, ctx_local.input_len) &&
                      esp_tee_buf_in_ree(iv, iv_len) &&
                      esp_tee_buf_in_ree(tag, tag_len) &&
                      esp_tee_buf_in_ree(output, ctx_local.input_len) &&
                      !esp_tee_sec_storage_is_key_tee_owned(ctx_local.key_id));

    if (ctx_local.aad_len != 0) {
        valid_arg &= esp_tee_buf_in_ree(ctx_local.aad, ctx_local.aad_len);
    }

    if (!valid_arg) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_arg);

    return esp_tee_sec_storage_aead_encrypt(&ctx_local, iv, iv_len, tag, tag_len, output);
}

esp_err_t _ss_esp_tee_sec_storage_aead_decrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, const uint8_t *iv, size_t iv_len, const uint8_t *tag, size_t tag_len, uint8_t *output)
{
    if (!esp_tee_buf_in_ree(ctx, sizeof(esp_tee_sec_storage_aead_ctx_t))) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_tee_sec_storage_aead_ctx_t ctx_local = *ctx;
    char id_buf[NVS_KEY_NAME_MAX_SIZE];
    if (!tee_snapshot_ree_str(&ctx_local.key_id, id_buf, sizeof(id_buf))) {
        return ESP_ERR_INVALID_ARG;
    }

    bool valid_arg = (esp_tee_buf_in_ree(ctx_local.input, ctx_local.input_len) &&
                      esp_tee_buf_in_ree(iv, iv_len) &&
                      esp_tee_buf_in_ree(tag, tag_len) &&
                      esp_tee_buf_in_ree(output, ctx_local.input_len) &&
                      !esp_tee_sec_storage_is_key_tee_owned(ctx_local.key_id));

    if (ctx_local.aad_len != 0) {
        valid_arg &= esp_tee_buf_in_ree(ctx_local.aad, ctx_local.aad_len);
    }

    if (!valid_arg) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_arg);

    return esp_tee_sec_storage_aead_decrypt(&ctx_local, iv, iv_len, tag, tag_len, output);
}

esp_err_t _ss_esp_tee_sec_storage_ecdsa_sign_pbkdf2(const esp_tee_sec_storage_pbkdf2_ctx_t *ctx, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey)
{
    if (!esp_tee_buf_in_ree(ctx, sizeof(esp_tee_sec_storage_pbkdf2_ctx_t))) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_tee_sec_storage_pbkdf2_ctx_t ctx_local = *ctx;

    bool valid_addr = (esp_tee_buf_in_ree(hash, hlen) &&
                       esp_tee_buf_in_ree(out_sign, sizeof(esp_tee_sec_storage_ecdsa_sign_t)) &&
                       esp_tee_buf_in_ree(out_pubkey, sizeof(esp_tee_sec_storage_ecdsa_pubkey_t)) &&
                       esp_tee_buf_in_ree(ctx_local.salt, ctx_local.salt_len));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_sec_storage_ecdsa_sign_pbkdf2(&ctx_local, hash, hlen, out_sign, out_pubkey);
}

/* ---------------------------------------------- MMU HAL ------------------------------------------------- */

static bool tee_ree_ext_vaddr_ok(uint32_t mmu_id, uint32_t vaddr, uint32_t len)
{
    uint32_t page = mmu_hal_pages_to_bytes(mmu_id, 1);
    uint32_t map_len = ESP_ALIGN_UP(len, page);

    return (len != 0 && map_len >= len && (vaddr % page == 0) &&
            mmu_hal_check_valid_ext_vaddr_region(mmu_id, vaddr, map_len,
                                                 MMU_VADDR_DATA | MMU_VADDR_INSTRUCTION) &&
            !esp_tee_flash_check_vrange_in_tee_region(vaddr, map_len));
}

static bool tee_ree_ext_paddr_ok(uint32_t mmu_id, uint32_t paddr, uint32_t len)
{
    uint32_t page = mmu_hal_pages_to_bytes(mmu_id, 1);
    uint32_t map_len = ESP_ALIGN_UP(len, page);

    return (len != 0 && map_len >= len && (paddr % page == 0) &&
            mmu_hal_check_valid_paddr_region(mmu_id, paddr, map_len) &&
            !esp_tee_flash_check_prange_in_tee_region(paddr, map_len));
}

void _ss_mmu_hal_map_region(uint32_t mmu_id, mmu_target_t mem_type, uint32_t vaddr,
                            uint32_t paddr, uint32_t len, uint32_t *out_len)
{
    bool valid_addr = (tee_ree_ext_vaddr_ok(mmu_id, vaddr, len) &&
                       tee_ree_ext_paddr_ok(mmu_id, paddr, len) &&
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
    bool valid_addr = tee_ree_ext_vaddr_ok(mmu_id, vaddr, len);

    if (!valid_addr) {
        ESP_LOGD(TAG, "[%s] Illegal flash access at 0x%08x", __func__, vaddr);
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    mmu_hal_unmap_region(mmu_id, vaddr, len);
}

bool _ss_mmu_hal_vaddr_to_paddr(uint32_t mmu_id, uint32_t vaddr, uint32_t *out_paddr, mmu_target_t *out_target)
{
    uint32_t page = mmu_hal_pages_to_bytes(mmu_id, 1);
    bool valid_addr = (tee_ree_ext_vaddr_ok(mmu_id, ESP_ALIGN_DOWN(vaddr, page), 1) &&
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
    bool valid_addr = (mmu_hal_check_valid_paddr_region(mmu_id, paddr, 1) &&
                       !esp_tee_flash_check_paddr_in_tee_region(paddr) &&
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
