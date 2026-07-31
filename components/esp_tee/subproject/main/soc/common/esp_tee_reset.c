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
#if SOC_RNG_SUPPORTED
#include "hal/rng_ll.h"
#endif

#include "hal/wdt_hal.h"

#include "esp_attr.h"
#include "esp_macros.h"
#include "esp_rom_serial_output.h"
#include "esp_rom_sys.h"

#include "esp_tee.h"
#include "esp_tee_rv_utils.h"

#include "sdkconfig.h"

void IRAM_ATTR esp_tee_soc_reset_crypto_peripherals(void)
{
    /* Reset the crypto peripherals to a clean state and leave their clocks disabled; drivers re-enable on demand */
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

#if SOC_HMAC_SUPPORTED
    hmac_ll_enable_bus_clock(true);
    hmac_ll_reset_register();
    hmac_ll_clean();
    hmac_ll_enable_bus_clock(false);
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

#if SOC_RNG_SUPPORTED
    rng_ll_enable();
#if RNG_LL_NEEDS_RESET_WHEN_WAKEUP
    rng_ll_reset();
#endif
#endif
}

void IRAM_ATTR esp_tee_system_reset(void)
{
    rv_utils_tee_intr_global_disable();

    // Make sure all the pending output is sent from the UART FIFO
    if (CONFIG_ESP_CONSOLE_UART_NUM >= 0) {
        esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    }

    // Leave the crypto peripherals in a clean state for the next boot
    esp_tee_soc_reset_crypto_peripherals();

    // Generate core reset alongwith the RTC domain
    // NOTE: The RTC WDT is the only SW-triggerable reset that covers the RTC domain
    wdt_hal_context_t rwdt_ctx;
    wdt_hal_init(&rwdt_ctx, WDT_RWDT, 0, false);
    wdt_hal_write_protect_disable(&rwdt_ctx);
    wdt_hal_config_stage(&rwdt_ctx, WDT_STAGE0, 32, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_enable(&rwdt_ctx);
    wdt_hal_write_protect_enable(&rwdt_ctx);

    // Give the RTC_WDT time to fire; if it does not, reset the HP_SYS directly
    esp_rom_delay_us(1000);
    esp_rom_software_reset_system();
    ESP_INFINITE_LOOP();
}
