/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for SPI Flash Encryption

#include "hal/spi_flash_encrypted_ll.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND
/**
 * @brief Default pseudo rounds configs of the XTS-AES accelerator
 */
typedef enum {
    ESP_XTS_AES_PSEUDO_ROUNDS_DISABLE = 0,
    ESP_XTS_AES_PSEUDO_ROUNDS_LOW,
    ESP_XTS_AES_PSEUDO_ROUNDS_MEDIUM,
    ESP_XTS_AES_PSEUDO_ROUNDS_HIGH,
} esp_xts_aes_psuedo_rounds_state_t;

/* The total number of pseudo-rounds randomly inserted in an XTS-AES operation are controlled by
 * configuring the PSEUDO_MODE, PSEUDO_BASE, PSEUDO_INC parameters.
 * Users can also set the frequency of random key updates by configuring the PSEUDO_RNG_CNT.
 * Here, we would be using some pre-decided values for these parameters corresponding to the security needed.
 * For more information regarding these parameters please refer the TRM.
 */
#define XTS_AES_PSEUDO_ROUNDS_BASE      4
#define XTS_AES_PSEUDO_ROUNDS_INC       2
#define XTS_AES_PSEUDO_ROUNDS_RNG_CNT   7

#endif /* SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND */

/**
 * @brief Enable the flash encryption
 */
void spi_flash_encryption_hal_enable(void);

/**
 * @brief Disable the flash encryption
 */
void spi_flash_encryption_hal_disable(void);

/**
 * Prepare flash encryption before operation.
 *
 * @param address The destination address in flash for the write operation.
 * @param buffer Data for programming
 * @param size Size to program.
 *
 * @note address and buffer must be 8-word aligned.
 */
void spi_flash_encryption_hal_prepare(uint32_t address, const uint32_t* buffer, uint32_t size);

/**
 * @brief flash data encryption operation is done.
 */
void spi_flash_encryption_hal_done(void);

/**
 * Destroy encrypted result
 */
void spi_flash_encryption_hal_destroy(void);

/**
 * Check if is qualified to encrypt the buffer
 *
 * @param address the address of written flash partition.
 * @param length Buffer size.
 */
bool spi_flash_encryption_hal_check(uint32_t address, uint32_t length);

#ifdef SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND
/**
 * @brief Enable the pseudo-round function during XTS-AES operations
 *
 * @param mode set the mode for pseudo rounds, zero to disable, with increasing security upto three.
 * @param base basic number of pseudo rounds, zero if disable
 * @param increment increment number of pseudo rounds, zero if disable
 * @param key_rng_cnt update frequency of the pseudo-key, zero if disable
 */
void spi_flash_encryption_hal_enable_pseudo_rounds(uint8_t mode, uint8_t base, uint8_t increment, uint8_t key_rng_cnt);
#endif /* SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND */

#ifdef __cplusplus
}
#endif
