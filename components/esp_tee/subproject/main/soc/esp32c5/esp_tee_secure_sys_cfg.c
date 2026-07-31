/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include <stdint.h>

#include "riscv/rv_utils.h"
#include "riscv/encoding.h"

#include "hal/apm_hal.h"
#include "hal/efuse_hal.h"
#include "hal/key_mgr_ll.h"
#include "hal/key_mgr_types.h"

#include "soc/clic_reg.h"
#include "soc/interrupts.h"

#include "esp_tee.h"
#include "esp_tee_intr.h"
#include "esp_tee_rv_utils.h"

#include "esp_cpu.h"
#include "esp_log.h"

#define CSR_MCOUNTEREN  0x306

#define _m2u_switch(arg0, arg1)                             \
  ({                                                        \
    register uintptr_t ra asm("ra") = (uintptr_t)(arg0);    \
    register uintptr_t a1 asm("a1") = (uintptr_t)(arg1);    \
    asm volatile("ecall" : :"r"(ra), "r"(a1) : );           \
  })

#define SET_BIT(t, n)  (t |= (1UL << (n)))
#define CLR_BIT(t, n)  (t &= ~(1UL << (n)))

static const char *TAG = "esp_tee_secure_sys_cfg";

extern uint32_t _vector_table;
extern uint32_t _mtvt_table;

/* NOTE: The ECDSA/HMAC/DS key selectors live in the Key Manager, so it stays clocked */
static void tee_init_key_mgr(void)
{
    /* NOTE: With flash encryption enabled, the KM is already initialized by the ROM/bootloader */
    if (!efuse_hal_flash_encryption_enabled()) {
        key_mgr_ll_power_up();
        key_mgr_ll_enable_bus_clock(true);
        key_mgr_ll_enable_peripheral_clock(true);
        key_mgr_ll_reset_register();

        while (key_mgr_ll_get_state() != ESP_KEY_MGR_STATE_IDLE) {
        }
    }

    key_mgr_ll_set_key_usage(ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);
    key_mgr_ll_set_key_usage(ESP_KEY_MGR_FLASH_XTS_AES_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);
    key_mgr_ll_set_key_usage(ESP_KEY_MGR_HMAC_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);
    key_mgr_ll_set_key_usage(ESP_KEY_MGR_DS_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);

    ESP_LOGI(TAG, "Key Manager: Not supported with ESP-TEE - REE access disabled");
}

void esp_tee_soc_secure_sys_init(void)
{
    ESP_LOGI(TAG, "Current privilege level - 0x%x", esp_cpu_get_curr_privilege_level());

    /* Setting the M-mode vector table */
    rv_utils_set_mtvec((uint32_t)&_vector_table);
    rv_utils_set_mtvt((uint32_t)&_mtvt_table);

    /* Disable global interrupts */
    RV_CLEAR_CSR(mstatus, MSTATUS_UIE);
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);

    /* Enabled support for U-mode interrupts */
    REG_SET_FIELD(CLIC_INT_CONFIG_REG, CLIC_INT_CONFIG_NMBITS, 0x01);
    REG_SET_FIELD(CLIC_INT_CONFIG_REG, CLIC_INT_CONFIG_UNLBITS, NLBITS);
    REG_SET_FIELD(CLIC_INT_CONFIG_REG, CLIC_INT_CONFIG_MNLBITS, NLBITS);

    /* Allow reading the cycle counter CSRs from U-mode */
    RV_WRITE_CSR(CSR_MCOUNTEREN, 0x07);

    /* Clearing all interrupt configurations */
    uint32_t core_id = esp_cpu_get_core_id();
    for (int i = 0; i < ETS_MAX_INTR_SOURCE; i++) {
        interrupt_clic_ll_route(core_id, i, ETS_INVALID_INUM);
        REG_CLR_BIT(DR_REG_INTMTX_BASE + 4 * i, BIT(8));
    }

    esprv_int_set_threshold(0);

    esprv_int_set_priority(TEE_SECURE_INUM, TEE_SECURE_INUM_PRIO);
    esprv_int_set_type(TEE_SECURE_INUM, ESP_CPU_INTR_TYPE_LEVEL);
    esprv_int_enable(BIT(TEE_SECURE_INUM));
    esprv_int_set_vectored(TEE_SECURE_INUM, true);
    rv_utils_tee_intr_set_mode(TEE_SECURE_INUM, PRV_M);

    esprv_int_set_priority(TEE_PASS_INUM, 1);
    esprv_int_set_type(TEE_PASS_INUM, ESP_CPU_INTR_TYPE_LEVEL);
    esprv_int_enable(BIT(TEE_PASS_INUM));
    esprv_int_set_vectored(TEE_PASS_INUM, true);
    rv_utils_tee_intr_set_mode(TEE_PASS_INUM, PRV_M);

    ESP_LOGD(TAG, "Initial interrupt config -");
    ESP_LOGD(TAG, "mtvec: 0x%08x", RV_READ_CSR(mtvec));
    ESP_LOGD(TAG, "mtvt: 0x%08x", RV_READ_CSR(MTVT_CSR));
    ESP_LOGD(TAG, "mstatus: 0x%08x", RV_READ_CSR(mstatus));
    ESP_LOGD(TAG, "mcounteren: 0x%08x", RV_READ_CSR(CSR_MCOUNTEREN));

    /* PMP, PMA and APM configuration to isolate the resources between TEE and REE. */
    esp_tee_configure_region_protection();
    esp_tee_configure_apm_protection();

    /* Protect secure interrupt sources */
    esp_tee_protect_intr_src(ETS_LP_APM_M0_INTR_SOURCE);    // LP_APM_M0
    esp_tee_protect_intr_src(ETS_LP_APM_M1_INTR_SOURCE);    // LP_APM_M1
    esp_tee_protect_intr_src(ETS_HP_APM_M0_INTR_SOURCE);    // HP_APM_M0
    esp_tee_protect_intr_src(ETS_HP_APM_M1_INTR_SOURCE);    // HP_APM_M1
    esp_tee_protect_intr_src(ETS_HP_APM_M2_INTR_SOURCE);    // HP_APM_M2
    esp_tee_protect_intr_src(ETS_HP_APM_M3_INTR_SOURCE);    // HP_APM_M3
    esp_tee_protect_intr_src(ETS_HP_APM_M4_INTR_SOURCE);    // HP_APM_M4
    esp_tee_protect_intr_src(ETS_LP_APM0_INTR_SOURCE);      // LP_APM0
    esp_tee_protect_intr_src(ETS_CPU_APM_M0_INTR_SOURCE);   // CPU_APM_M0
    esp_tee_protect_intr_src(ETS_CPU_APM_M1_INTR_SOURCE);   // CPU_APM_M1
    esp_tee_protect_intr_src(ETS_AES_INTR_SOURCE);          // AES
    esp_tee_protect_intr_src(ETS_SHA_INTR_SOURCE);          // SHA
    esp_tee_protect_intr_src(ETS_ECC_INTR_SOURCE);          // ECC

    /* Set the key usage for the ECDSA/XTS-AES/HMAC/DS peripherals to eFuse */
    tee_init_key_mgr();
    /* Reset the protected crypto peripherals and leave their clocks disabled */
    esp_tee_soc_reset_crypto_peripherals();
}

IRAM_ATTR inline void esp_tee_switch_to_ree(uint32_t ns_entry_addr)
{
    /* 2nd argument is used as magic value to detect very first M2U switch */
    /* TBD: clean this up and use proper temporary register instead of a1 */
    /* Switch to non-secure world and launch App. */
    _m2u_switch(ns_entry_addr, ESP_TEE_M2U_SWITCH_MAGIC << 12);
}
