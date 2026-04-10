/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "sdkconfig.h"
#include "esp_crypto_periph_clk.h"

#if SOC_AES_SUPPORTED
#include "hal/aes_ll.h"
#endif
#if SOC_SHA_SUPPORTED
#include "hal/sha_ll.h"
#endif
#if SOC_MPI_SUPPORTED
#include "hal/mpi_ll.h"
#endif
#if SOC_ECC_SUPPORTED
#include "hal/ecc_ll.h"
#endif
/* NOTE: For ESP32-S2, the HMAC and DS are implemented in the ROM */
#if SOC_HMAC_SUPPORTED && !CONFIG_IDF_TARGET_ESP32S2
#include "hal/hmac_ll.h"
#endif
#if SOC_DIG_SIGN_SUPPORTED && !CONFIG_IDF_TARGET_ESP32S2
#include "hal/ds_ll.h"
#endif
#if SOC_ECDSA_SUPPORTED
#include "hal/ecdsa_ll.h"
#endif
#if SOC_KEY_MANAGER_SUPPORTED
#include "hal/key_mgr_ll.h"
#endif
/* Crypto DMA, shared between AES and SHA */
#if SOC_AES_CRYPTO_DMA && SOC_SHA_CRYPTO_DMA
#include "hal/crypto_dma_ll.h"
#endif

#if NON_OS_BUILD
// To suppress build errors about spinlock's __DECLARE_RCC_ATOMIC_ENV
int __DECLARE_RCC_ATOMIC_ENV __attribute__((unused));
#endif

#if SOC_AES_SUPPORTED
void esp_crypto_aes_enable_periph_clk(bool enable)
{
    AES_RCC_ATOMIC() {
        aes_ll_enable_bus_clock(enable);
        if (enable) {
            aes_ll_reset_register();
        }
#if SOC_AES_CRYPTO_DMA
        crypto_dma_ll_enable_bus_clock(enable);
        if (enable) {
            crypto_dma_ll_reset_register();
        }
#endif
    }
}
#endif

#if SOC_SHA_SUPPORTED
void esp_crypto_sha_enable_periph_clk(bool enable)
{
    SHA_RCC_ATOMIC() {
        sha_ll_enable_bus_clock(enable);
        if (enable) {
            sha_ll_reset_register();
        }
#if SOC_SHA_CRYPTO_DMA
        crypto_dma_ll_enable_bus_clock(enable);
        if (enable) {
            crypto_dma_ll_reset_register();
        }
#endif
    }
}
#endif

#if SOC_MPI_SUPPORTED
void esp_crypto_mpi_enable_periph_clk(bool enable)
{
    MPI_RCC_ATOMIC() {
        mpi_ll_enable_bus_clock(enable);
        if (enable) {
            mpi_ll_power_up();
            mpi_ll_reset_register();
        } else {
            mpi_ll_power_down();
        }
    }
}
#endif

#if SOC_ECC_SUPPORTED
void esp_crypto_ecc_enable_periph_clk(bool enable)
{
    ECC_RCC_ATOMIC() {
        ecc_ll_enable_bus_clock(enable);
        if (enable) {
            ecc_ll_power_up();
            ecc_ll_reset_register();
        } else {
            ecc_ll_power_down();
        }
    }
}
#endif

#if SOC_HMAC_SUPPORTED && !CONFIG_IDF_TARGET_ESP32S2
void esp_crypto_hmac_enable_periph_clk(bool enable)
{
    HMAC_RCC_ATOMIC() {
        hmac_ll_enable_bus_clock(enable);
        if (enable) {
            hmac_ll_reset_register();
        }
    }
}
#endif

#if SOC_DIG_SIGN_SUPPORTED && !CONFIG_IDF_TARGET_ESP32S2
void esp_crypto_ds_enable_periph_clk(bool enable)
{
    DS_RCC_ATOMIC() {
        ds_ll_enable_bus_clock(enable);
        if (enable) {
            ds_ll_reset_register();
        }
    }
}
#endif

#if SOC_ECDSA_SUPPORTED
void esp_crypto_ecdsa_enable_periph_clk(bool enable)
{
    ECDSA_RCC_ATOMIC() {
        ecdsa_ll_enable_bus_clock(enable);
        if (enable) {
            ecdsa_ll_reset_register();
        }
    }
}
#endif

#if SOC_KEY_MANAGER_SUPPORTED
void esp_crypto_key_mgr_enable_periph_clk(bool enable)
{
    KEY_MANAGER_RCC_ATOMIC() {
        key_mgr_ll_power_up();
        key_mgr_ll_enable_bus_clock(enable);
        key_mgr_ll_enable_peripheral_clock(enable);
        if (enable) {
            key_mgr_ll_reset_register();
        }
    }
}
#endif
