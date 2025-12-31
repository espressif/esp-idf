/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>

#include "soc/soc_caps.h"
#include "esp_err.h"
#include "esp_random.h"

#include "hal/mmu_types.h"
#include "hal/wdt_hal.h"
#include "hal/spi_flash_hal.h"
#include "hal/spi_flash_types.h"
#include "esp_private/mspi_timing_tuning.h"
#if SOC_AES_SUPPORTED
#include "aes/esp_aes.h"
#endif
#if SOC_SHA_SUPPORTED
#include "hal/sha_types.h"
#include "psa_crypto_driver_esp_sha_contexts.h"
#endif
#if SOC_ECC_SUPPORTED
#include "ecc_impl.h"
#endif
#if SOC_HMAC_SUPPORTED
#include "esp_hmac.h"
#endif
#if SOC_DIG_SIGN_SUPPORTED
#include "esp_ds.h"
#include "rom/digital_signature.h"
#endif
#include "psa/crypto.h"
#include "esp_crypto_lock.h"
#include "esp_flash.h"

#include "esp_tee.h"
#include "secure_service_num.h"
#include "sdkconfig.h"

/* ---------------------------------------------- Interrupts ------------------------------------------------- */

void IRAM_ATTR __wrap_esp_rom_route_intr_matrix(int cpu_no, uint32_t model_num, uint32_t intr_num)
{
    esp_tee_service_call(4, SS_ESP_ROM_ROUTE_INTR_MATRIX, cpu_no, model_num, intr_num);
}

#if SOC_INT_CLIC_SUPPORTED
void IRAM_ATTR __wrap_esprv_int_set_vectored(int rv_int_num, bool vectored)
{
    esp_tee_service_call(3, SS_ESPRV_INT_SET_VECTORED, rv_int_num, vectored);
}
#endif

/* ---------------------------------------------- RTC_WDT ------------------------------------------------- */

void IRAM_ATTR __wrap_wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr)
{
    esp_tee_service_call(5, SS_WDT_HAL_INIT, hal, wdt_inst, prescaler, enable_intr);
}

void IRAM_ATTR __wrap_wdt_hal_deinit(wdt_hal_context_t *hal)
{
    esp_tee_service_call(2, SS_WDT_HAL_DEINIT, hal);
}

/* ---------------------------------------------- AES ------------------------------------------------- */

#if SOC_AES_SUPPORTED
int __wrap_esp_aes_intr_alloc(void)
{
    return esp_tee_service_call(1, SS_ESP_AES_INTR_ALLOC);
}

int __wrap_esp_aes_crypt_cbc(esp_aes_context *ctx,
                             int mode,
                             size_t length,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output)
{
    esp_crypto_sha_aes_lock_acquire();
    esp_err_t err = esp_tee_service_call(7, SS_ESP_AES_CRYPT_CBC, ctx, mode, length, iv, input, output);
    esp_crypto_sha_aes_lock_release();
    return err;
}

int __wrap_esp_aes_crypt_cfb128(esp_aes_context *ctx,
                                int mode,
                                size_t length,
                                size_t *iv_off,
                                unsigned char iv[16],
                                const unsigned char *input,
                                unsigned char *output)
{
    esp_crypto_sha_aes_lock_acquire();
    esp_err_t err = esp_tee_service_call(8, SS_ESP_AES_CRYPT_CFB128, (uint32_t)ctx,
                                         mode, length, iv_off, iv, (uint32_t)input, (uint32_t)output);
    esp_crypto_sha_aes_lock_release();
    return err;
}

int __wrap_esp_aes_crypt_cfb8(esp_aes_context *ctx,
                              int mode,
                              size_t length,
                              unsigned char iv[16],
                              const unsigned char *input,
                              unsigned char *output)
{
    esp_crypto_sha_aes_lock_acquire();
    esp_err_t err = esp_tee_service_call(7, SS_ESP_AES_CRYPT_CFB8, ctx,
                                         mode, length, iv, input, output);
    esp_crypto_sha_aes_lock_release();
    return err;
}

int __wrap_esp_aes_crypt_ctr(esp_aes_context *ctx,
                             size_t length,
                             size_t *nc_off,
                             unsigned char nonce_counter[16],
                             unsigned char stream_block[16],
                             const unsigned char *input,
                             unsigned char *output)
{
    esp_crypto_sha_aes_lock_acquire();
    esp_err_t err = esp_tee_service_call(8, SS_ESP_AES_CRYPT_CTR, ctx, length, nc_off, nonce_counter, stream_block, input, output);
    esp_crypto_sha_aes_lock_release();
    return err;
}

int __wrap_esp_aes_crypt_ecb(esp_aes_context *ctx,
                             int mode,
                             const unsigned char input[16],
                             unsigned char output[16])
{
    esp_crypto_sha_aes_lock_acquire();
    esp_err_t err = esp_tee_service_call(5, SS_ESP_AES_CRYPT_ECB,
                                         (uint32_t)ctx, (uint32_t)mode,
                                         (uint32_t)input, (uint32_t)output);
    esp_crypto_sha_aes_lock_release();
    return err;
}

int __wrap_esp_aes_crypt_ofb(esp_aes_context *ctx,
                             size_t length,
                             size_t *iv_off,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output)
{
    esp_crypto_sha_aes_lock_acquire();
    esp_err_t err = esp_tee_service_call(7, SS_ESP_AES_CRYPT_OFB, (uint32_t)ctx, length,
                                         iv_off, iv, (uint32_t)input, (uint32_t)output);
    esp_crypto_sha_aes_lock_release();
    return err;
}
#endif
/* ---------------------------------------------- SHA ------------------------------------------------- */

#if SOC_SHA_SUPPORTED
void __wrap_esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{
    esp_tee_service_call(5, SS_ESP_SHA,
                         (uint32_t)sha_type, (uint32_t)input,
                         (uint32_t)ilen, (uint32_t)output);
}

int __wrap_esp_sha_dma(esp_sha_type sha_type, const void *input, uint32_t ilen,
                       const void *buf, uint32_t buf_len, bool is_first_block)
{
    return esp_tee_service_call(7, SS_ESP_SHA_DMA, sha_type, input, ilen, buf, buf_len, is_first_block);
}

int __wrap_esp_sha_block(esp_sha_type sha_type, const void *data_block, bool is_first_block)
{
    return esp_tee_service_call(4, SS_ESP_SHA_BLOCK, sha_type, data_block, is_first_block);
}

void __wrap_esp_sha_set_mode(esp_sha_type sha_type)
{
    esp_tee_service_call(2, SS_ESP_SHA_SET_MODE, sha_type);
}

void __wrap_esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    esp_tee_service_call(3, SS_ESP_SHA_READ_DIGEST_STATE, sha_type, digest_state);
}

void __wrap_esp_sha_write_digest_state(esp_sha_type sha_type, void *digest_state)
{
    esp_tee_service_call(3, SS_ESP_SHA_WRITE_DIGEST_STATE, sha_type, digest_state);
}

void __wrap_esp_crypto_sha_enable_periph_clk(bool enable)
{
    esp_tee_service_call(2, SS_ESP_CRYPTO_SHA_ENABLE_PERIPH_CLK, enable);
}

#if SOC_SHA_SUPPORT_SHA512_T
int __wrap_esp_sha_512_t_init_hash(uint16_t t)
{
    return esp_tee_service_call(2, SS_ESP_SHA_512_T_INIT_HASH, t);
}
#endif
#endif

/* ---------------------------------------------- HMAC ------------------------------------------------- */

#if SOC_HMAC_SUPPORTED
esp_err_t __wrap_esp_hmac_calculate(hmac_key_id_t key_id, const void *message, size_t message_len, uint8_t *hmac)
{
    esp_crypto_hmac_lock_acquire();
    esp_err_t err = esp_tee_service_call(5, SS_ESP_HMAC_CALCULATE, key_id, message, message_len, hmac);
    esp_crypto_hmac_lock_release();
    return err;
}

esp_err_t __wrap_esp_hmac_jtag_enable(hmac_key_id_t key_id, const uint8_t *token)
{
    esp_crypto_hmac_lock_acquire();
    esp_err_t err = esp_tee_service_call(3, SS_ESP_HMAC_JTAG_ENABLE, key_id, token);
    esp_crypto_hmac_lock_release();
    return err;
}

esp_err_t __wrap_esp_hmac_jtag_disable(void)
{
    esp_crypto_hmac_lock_acquire();
    esp_err_t err = esp_tee_service_call(1, SS_ESP_HMAC_JTAG_DISABLE);
    esp_crypto_hmac_lock_release();
    return err;
}
#endif

/* ---------------------------------------------- DS ------------------------------------------------- */

#if SOC_DIG_SIGN_SUPPORTED
esp_err_t __wrap_esp_ds_sign(const void *message,
                             const esp_ds_data_t *data,
                             hmac_key_id_t key_id,
                             void *signature)
{
    esp_crypto_ds_lock_acquire();
    esp_err_t err = esp_tee_service_call(5, SS_ESP_DS_SIGN, message, data, key_id, signature);
    esp_crypto_ds_lock_release();
    return err;
}

esp_err_t __wrap_esp_ds_start_sign(const void *message,
                                   const esp_ds_data_t *data,
                                   hmac_key_id_t key_id,
                                   esp_ds_context_t **esp_ds_ctx)
{
    esp_crypto_ds_lock_acquire();
    if (esp_ds_ctx != NULL) {
        *esp_ds_ctx = malloc(sizeof(esp_ds_context_t));
        if (!*esp_ds_ctx) {
            return ESP_ERR_NO_MEM;
        }
    }
    return esp_tee_service_call(5, SS_ESP_DS_START_SIGN, message, data, key_id, esp_ds_ctx);
}

bool __wrap_esp_ds_is_busy(void)
{
    return esp_tee_service_call(1, SS_ESP_DS_IS_BUSY);
}

esp_err_t __wrap_esp_ds_finish_sign(void *signature, esp_ds_context_t *esp_ds_ctx)
{
    esp_err_t err = esp_tee_service_call(3, SS_ESP_DS_FINISH_SIGN, signature, esp_ds_ctx);
    if (err != ESP_ERR_INVALID_ARG) {
        free(esp_ds_ctx);
    }
    esp_crypto_ds_lock_release();
    return err;
}

esp_err_t __wrap_esp_ds_encrypt_params(esp_ds_data_t *data,
                                       const void *iv,
                                       const esp_ds_p_data_t *p_data,
                                       const void *key)
{
    esp_crypto_sha_aes_lock_acquire();
    esp_err_t err = esp_tee_service_call(5, SS_ESP_DS_ENCRYPT_PARAMS, data, iv, p_data, key);
    esp_crypto_sha_aes_lock_release();
    return err;
}
#endif

/* ---------------------------------------------- MPI ------------------------------------------------- */

#if SOC_MPI_SUPPORTED
void __wrap_esp_crypto_mpi_enable_periph_clk(bool enable)
{
    esp_tee_service_call(2, SS_ESP_CRYPTO_MPI_ENABLE_PERIPH_CLK, enable);
}
#endif

/* ---------------------------------------------- ECC ------------------------------------------------- */

#if SOC_ECC_SUPPORTED
int __wrap_esp_ecc_point_multiply(const ecc_point_t *point, const uint8_t *scalar, ecc_point_t *result, bool verify_first)
{
    esp_crypto_ecc_lock_acquire();
    esp_err_t err = esp_tee_service_call(5, SS_ESP_ECC_POINT_MULTIPLY, point, scalar, result, verify_first);
    esp_crypto_ecc_lock_release();
    return err;
}

int __wrap_esp_ecc_point_verify(const ecc_point_t *point)
{
    esp_crypto_ecc_lock_acquire();
    esp_err_t err = esp_tee_service_call(2, SS_ESP_ECC_POINT_VERIFY, point);
    esp_crypto_ecc_lock_release();
    return err;
}
#endif

#if SOC_ECC_SUPPORTED && SOC_ECDSA_SUPPORTED
void __wrap_esp_crypto_ecc_enable_periph_clk(bool enable)
{
    esp_tee_service_call(2, SS_ESP_CRYPTO_ECC_ENABLE_PERIPH_CLK, enable);
}
#endif

/* ---------------------------------------------- MMU HAL ------------------------------------------------- */

void IRAM_ATTR __wrap_mmu_hal_map_region(uint32_t mmu_id, mmu_target_t mem_type, uint32_t vaddr, uint32_t paddr, uint32_t len, uint32_t *out_len)
{
    esp_tee_service_call(7, SS_MMU_HAL_MAP_REGION, mmu_id, mem_type, vaddr, paddr, len, out_len);
}

void IRAM_ATTR __wrap_mmu_hal_unmap_region(uint32_t mmu_id, uint32_t vaddr, uint32_t len)
{
    esp_tee_service_call(4, SS_MMU_HAL_UNMAP_REGION, mmu_id, vaddr, len);
}

bool IRAM_ATTR __wrap_mmu_hal_vaddr_to_paddr(uint32_t mmu_id, uint32_t vaddr, uint32_t *out_paddr, mmu_target_t *out_target)
{
    return esp_tee_service_call(5, SS_MMU_HAL_VADDR_TO_PADDR, mmu_id, vaddr, out_paddr, out_target);
}

bool IRAM_ATTR __wrap_mmu_hal_paddr_to_vaddr(uint32_t mmu_id, uint32_t paddr, mmu_target_t target, mmu_vaddr_t type, uint32_t *out_vaddr)
{
    return esp_tee_service_call(6, SS_MMU_HAL_PADDR_TO_VADDR, mmu_id, paddr, target, type, out_vaddr);
}

/**
 * NOTE: This ROM-provided API is intended to configure the Cache MMU size for
 * instruction (irom) and rodata (drom) sections in flash.
 *
 * On ESP32-C5, it also sets the start pages for flash irom and drom sections,
 * which involves accessing MMU registers directly.
 *
 * However, these MMU registers are protected by the APM and direct access
 * from the REE results in a fault.
 *
 * To prevent this, we wrap this function to be routed as a TEE service call.
 */
#if CONFIG_IDF_TARGET_ESP32C5
void IRAM_ATTR __wrap_Cache_Set_IDROM_MMU_Size(uint32_t irom_size, uint32_t drom_size)
{
    esp_tee_service_call(3, SS_CACHE_SET_IDROM_MMU_SIZE, irom_size, drom_size);
}
#endif

#if CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1
/* ---------------------------------------------- SPI Flash HAL ------------------------------------------------- */

uint32_t IRAM_ATTR __wrap_spi_flash_hal_check_status(spi_flash_host_inst_t *host)
{
    return esp_tee_service_call(2, SS_SPI_FLASH_HAL_CHECK_STATUS, host);
}

esp_err_t IRAM_ATTR __wrap_spi_flash_hal_common_command(spi_flash_host_inst_t *host, spi_flash_trans_t *trans)
{
    return esp_tee_service_call(3, SS_SPI_FLASH_HAL_COMMON_COMMAND, host, trans);
}

esp_err_t IRAM_ATTR __wrap_spi_flash_hal_device_config(spi_flash_host_inst_t *host)
{
    return esp_tee_service_call(2, SS_SPI_FLASH_HAL_DEVICE_CONFIG, host);
}

void IRAM_ATTR __wrap_spi_flash_hal_erase_block(spi_flash_host_inst_t *host, uint32_t start_address)
{
    esp_tee_service_call(3, SS_SPI_FLASH_HAL_ERASE_BLOCK, host, start_address);
}

void IRAM_ATTR __wrap_spi_flash_hal_erase_sector(spi_flash_host_inst_t *host, uint32_t start_address)
{
    esp_tee_service_call(3, SS_SPI_FLASH_HAL_ERASE_SECTOR, host, start_address);
}

void IRAM_ATTR __wrap_spi_flash_hal_program_page(spi_flash_host_inst_t *host, const void *buffer, uint32_t address, uint32_t length)
{
    esp_tee_service_call(5, SS_SPI_FLASH_HAL_PROGRAM_PAGE, host, buffer, address, length);
}

esp_err_t IRAM_ATTR __wrap_spi_flash_hal_read(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len)
{
    return esp_tee_service_call(5, SS_SPI_FLASH_HAL_READ, host, buffer, address, read_len);
}

void IRAM_ATTR __wrap_spi_flash_hal_resume(spi_flash_host_inst_t *host)
{
    esp_tee_service_call(2, SS_SPI_FLASH_HAL_RESUME, host);
}

esp_err_t IRAM_ATTR __wrap_spi_flash_hal_set_write_protect(spi_flash_host_inst_t *host, bool wp)
{
    return esp_tee_service_call(3, SS_SPI_FLASH_HAL_SET_WRITE_PROTECT, host, wp);
}

esp_err_t IRAM_ATTR __wrap_spi_flash_hal_setup_read_suspend(spi_flash_host_inst_t *host, const spi_flash_sus_cmd_conf *sus_conf)
{
    return esp_tee_service_call(6, SS_SPI_FLASH_HAL_SETUP_READ_SUSPEND, host, sus_conf);
}

bool IRAM_ATTR __wrap_spi_flash_hal_supports_direct_read(spi_flash_host_inst_t *host, const void *p)
{
    return esp_tee_service_call(3, SS_SPI_FLASH_HAL_SUPPORTS_DIRECT_READ, host, p);
}

bool IRAM_ATTR __wrap_spi_flash_hal_supports_direct_write(spi_flash_host_inst_t *host, const void *p)
{
    return esp_tee_service_call(3, SS_SPI_FLASH_HAL_SUPPORTS_DIRECT_WRITE, host, p);
}

void IRAM_ATTR __wrap_spi_flash_hal_suspend(spi_flash_host_inst_t *host)
{
    esp_tee_service_call(2, SS_SPI_FLASH_HAL_SUSPEND, host);
}

/* ---------------------------------------------- SPI Flash Extras ------------------------------------------------- */

uint32_t IRAM_ATTR __wrap_bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len)
{
    return esp_tee_service_call(8, SS_BOOTLOADER_FLASH_EXECUTE_COMMAND_COMMON,
                                command, addr_len, address, dummy_len, mosi_len,
                                mosi_data, miso_len);
}

esp_err_t IRAM_ATTR __wrap_memspi_host_flush_cache(spi_flash_host_inst_t *host, uint32_t addr, uint32_t size)
{
    return esp_tee_service_call(4, SS_MEMSPI_HOST_FLUSH_CACHE, host, addr, size);
}

esp_err_t IRAM_ATTR __wrap_spi_flash_chip_generic_config_host_io_mode(esp_flash_t *chip, uint32_t flags)
{
    return esp_tee_service_call(3, SS_SPI_FLASH_CHIP_GENERIC_CONFIG_HOST_IO_MODE, chip, flags);
}

#if CONFIG_IDF_TARGET_ESP32C5
void IRAM_ATTR __wrap_mspi_timing_flash_tuning(void)
{
    esp_tee_service_call(1, SS_MSPI_TIMING_FLASH_TUNING);
}

void IRAM_ATTR __wrap_mspi_timing_psram_tuning(void)
{
    esp_tee_service_call(1, SS_MSPI_TIMING_PSRAM_TUNING);
}

void IRAM_ATTR __wrap_mspi_timing_enter_low_speed_mode(bool control_spi1)
{
    esp_tee_service_call(2, SS_MSPI_TIMING_ENTER_LOW_SPEED_MODE, control_spi1);
}

void IRAM_ATTR __wrap_mspi_timing_enter_high_speed_mode(bool control_spi1)
{
    esp_tee_service_call(2, SS_MSPI_TIMING_ENTER_HIGH_SPEED_MODE, control_spi1);
}

void IRAM_ATTR __wrap_mspi_timing_change_speed_mode_cache_safe(bool switch_down)
{
    esp_tee_service_call(2, SS_MSPI_TIMING_CHANGE_SPEED_MODE_CACHE_SAFE, switch_down);
}

void IRAM_ATTR __wrap_spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config)
{
    esp_tee_service_call(2, SS_SPI_TIMING_GET_FLASH_TIMING_PARAM, out_timing_config);
}
#endif
#endif

#if CONFIG_SECURE_TEE_ATTESTATION
psa_status_t __wrap_psa_initial_attest_get_token(const uint8_t *auth_challenge, size_t challenge_size,
                                                 uint8_t *token_buf, size_t token_buf_size, size_t *token_size)
{
    return (esp_err_t)esp_tee_service_call_with_noniram_intr_disabled(6, SS_PSA_INITIAL_ATTEST_GET_TOKEN, auth_challenge, challenge_size,
                                                                      token_buf, token_buf_size, token_size);
}

psa_status_t __wrap_psa_initial_attest_get_token_size(size_t challenge_size, size_t *token_size)
{
    return (esp_err_t)esp_tee_service_call_with_noniram_intr_disabled(3, SS_PSA_INITIAL_ATTEST_GET_TOKEN_SIZE, challenge_size, token_size);
}
#endif
