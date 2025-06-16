/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>

#include "esp_err.h"
#include "esp_random.h"

#include "hal/sha_types.h"
#include "hal/sha_hal.h"
#include "rom/digital_signature.h"
#include "hal/mmu_types.h"
#include "hal/wdt_hal.h"
#include "hal/spi_flash_types.h"
#include "esp_hmac.h"
#include "esp_ds.h"
#include "esp_crypto_lock.h"
#include "esp_flash.h"

#include "soc/soc_caps.h"

#include "esp_tee.h"
#include "secure_service_num.h"

/* ---------------------------------------------- Interrupts ------------------------------------------------- */

void IRAM_ATTR __wrap_esp_rom_route_intr_matrix(int cpu_no, uint32_t model_num, uint32_t intr_num)
{
    esp_tee_service_call(4, SS_ESP_ROM_ROUTE_INTR_MATRIX, cpu_no, model_num, intr_num);
}

/* ---------------------------------------------- RTC_WDT ------------------------------------------------- */

void __wrap_wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr)
{
    esp_tee_service_call(5, SS_WDT_HAL_INIT, hal, wdt_inst, prescaler, enable_intr);
}

void __wrap_wdt_hal_deinit(wdt_hal_context_t *hal)
{
    esp_tee_service_call(2, SS_WDT_HAL_DEINIT, hal);
}

/* ---------------------------------------------- AES ------------------------------------------------- */

typedef struct {
    uint8_t key_bytes;
    volatile uint8_t key_in_hardware; /* This variable is used for fault injection checks, so marked volatile to avoid optimisation */
    uint8_t key[32];
} esp_aes_context;

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

/* ---------------------------------------------- SHA ------------------------------------------------- */

typedef enum {
    ESP_SHA1_STATE_INIT,
    ESP_SHA1_STATE_IN_PROCESS
} esp_sha1_state;

typedef enum {
    ESP_SHA256_STATE_INIT,
    ESP_SHA256_STATE_IN_PROCESS
} esp_sha256_state;

typedef enum {
    ESP_SHA512_STATE_INIT,
    ESP_SHA512_STATE_IN_PROCESS
} esp_sha512_state;

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int first_block;            /*!< if first then true else false */
    esp_sha_type mode;
    esp_sha1_state sha_state;
} esp_sha1_context;

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int first_block;           /*!< if first then true, else false */
    esp_sha_type mode;
    esp_sha256_state sha_state;
} esp_sha256_context;

typedef struct {
    uint64_t total[2];          /*!< number of bytes processed  */
    uint64_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[128];  /*!< data block being processed */
    int first_block;
    esp_sha_type mode;
    uint32_t t_val;             /*!< t_val for 512/t mode */
    esp_sha512_state sha_state;
} esp_sha512_context;

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

/* ---------------------------------------------- HMAC ------------------------------------------------- */

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

/* ---------------------------------------------- DS ------------------------------------------------- */

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

/* ---------------------------------------------- MPI ------------------------------------------------- */

void __wrap_esp_crypto_mpi_enable_periph_clk(bool enable)
{
    esp_tee_service_call(2, SS_ESP_CRYPTO_MPI_ENABLE_PERIPH_CLK, enable);
}

/* ---------------------------------------------- ECC ------------------------------------------------- */

#define P256_LEN        (256/8)
#define P192_LEN        (192/8)

typedef struct {
    uint8_t x[P256_LEN]; /* Little endian order */
    uint8_t y[P256_LEN]; /* Little endian order */
    unsigned len;        /* P192_LEN or P256_LEN */
} ecc_point_t;

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

#if SOC_ECDSA_SUPPORTED
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

void IRAM_ATTR __wrap_spi_flash_hal_erase_chip(spi_flash_host_inst_t *host)
{
    esp_tee_service_call(2, SS_SPI_FLASH_HAL_ERASE_CHIP, host);
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
#endif
