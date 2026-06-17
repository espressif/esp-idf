/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"

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
#if SOC_HMAC_SUPPORTED
#include "hal/hmac_ll.h"
#endif
#if SOC_DIG_SIGN_SUPPORTED
#include "hal/ds_ll.h"
#endif
#if SOC_ECDSA_SUPPORTED
#include "hal/ecdsa_ll.h"
#endif

#include "esp_tee.h"

void esp_tee_soc_reset_crypto_peripherals(void)
{
    /* Reset the crypto peripherals to a clean state and leave their clocks disabled; drivers re-enable on demand */
#if SOC_AES_SUPPORTED
    aes_ll_enable_bus_clock(true);
    aes_ll_reset_register();
    aes_ll_enable_bus_clock(false);
#endif

#if SOC_SHA_SUPPORTED
    sha_ll_enable_bus_clock(true);
    sha_ll_reset_register();
    sha_ll_enable_bus_clock(false);
#endif

#if SOC_MPI_SUPPORTED
    mpi_ll_enable_bus_clock(true);
    mpi_ll_reset_register();
    mpi_ll_enable_bus_clock(false);
#endif

#if SOC_ECC_SUPPORTED
    ecc_ll_enable_bus_clock(true);
    ecc_ll_reset_register();
    ecc_ll_power_up();
    ecc_ll_enable_bus_clock(false);
#endif

#if SOC_HMAC_SUPPORTED
    hmac_ll_enable_bus_clock(true);
    hmac_ll_reset_register();
    hmac_ll_enable_bus_clock(false);
#endif

#if SOC_DIG_SIGN_SUPPORTED
    ds_ll_enable_bus_clock(true);
    ds_ll_reset_register();
    ds_ll_enable_bus_clock(false);
#endif

#if SOC_ECDSA_SUPPORTED
    ecdsa_ll_enable_bus_clock(true);
    ecdsa_ll_reset_register();
    ecdsa_ll_enable_bus_clock(false);
#endif
}
