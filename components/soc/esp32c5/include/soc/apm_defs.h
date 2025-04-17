/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Number of paths for each supported APM controller */
#define APM_CTRL_HP_APM_PATH_NUM          (5)
#define APM_CTRL_LP_APM0_PATH_NUM         (1)
#define APM_CTRL_LP_APM_PATH_NUM          (2)
#define APM_CTRL_CPU_APM_PATH_NUM         (2)
/* Number of regions for each supported APM controller */
#define APM_CTRL_HP_APM_REGION_NUM        (16)
#define APM_CTRL_LP_APM0_REGION_NUM       (8)
#define APM_CTRL_LP_APM_REGION_NUM        (8)
#define APM_CTRL_CPU_APM_REGION_NUM       (8)

/* Register offset for TEE mode control */
#define APM_TEE_MODE_CTRL_OFFSET        (0x04)
/* Register offset between TEE ctrl periph registers */
#define APM_TEE_PERI_CTRL_OFFSET        (0x04)
/* Register offset between region bound address registers */
#define APM_REGION_ADDR_OFFSET          (0x0C)
/* Register offset between region pms attribute registers */
#define APM_REGION_ATTR_OFFSET          (0x0C)
/* Register offset between exception info registers */
#define APM_EXCP_INFO_OFFSET            (0x10)

/* Bit to clear exception status */
#define APM_EXCP_STATUS_CLR_BIT         (BIT(0))
/* Bit to lock TEE mode */
#define APM_TEE_MODE_LOCK_BIT           (BIT(2))
/* Bit to lock region pms attributes */
#define APM_REGION_LOCK_BIT             (BIT(11))

/* APM controller masters mask */
#define APM_MASTER_MASK_ALL                 (0x03CE017FU)
/* HP-TEE controller peripherals mask */
#define APM_TEE_HP_PERIPH_MASK_ALL          (0x0017FF7FFC7DFEEFULL)
/* LP-TEE controller peripherals mask */
#define APM_TEE_LP_PERIPH_MASK_ALL          (0x0007B4FFULL)

/**
 * @brief HP-TEE Controller Peripherals
 */
typedef enum {
    APM_TEE_HP_PERIPH_UART0 = 0,            /*!< UART0 */
    APM_TEE_HP_PERIPH_UART1,                /*!< UART1 */
    APM_TEE_HP_PERIPH_UHCI0,                /*!< UHCI0 */
    APM_TEE_HP_PERIPH_I2C_EXT0,             /*!< I2C0 */
    APM_TEE_HP_PERIPH_I2S = 5,              /*!< I2S */
    APM_TEE_HP_PERIPH_PARL_IO,              /*!< Parallel IO */
    APM_TEE_HP_PERIPH_PWM,                  /*!< PWM */
    APM_TEE_HP_PERIPH_LEDC = 9,             /*!< LED Control */
    APM_TEE_HP_PERIPH_TWAI0,                /*!< TWAI0 */
    APM_TEE_HP_PERIPH_USB_SJ,               /*!< USB Serial JTAG */
    APM_TEE_HP_PERIPH_RMT,                  /*!< RMT */
    APM_TEE_HP_PERIPH_GDMA,                 /*!< GDMA */
    APM_TEE_HP_PERIPH_REGDMA,               /*!< REGDMA */
    APM_TEE_HP_PERIPH_ETM,                  /*!< Event Task Matrix */
    APM_TEE_HP_PERIPH_INTMTX,               /*!< Interrupt Matrix */
    APM_TEE_HP_PERIPH_APB_ADC = 18,         /*!< ADC */
    APM_TEE_HP_PERIPH_TG0,                  /*!< Timer Group 0 */
    APM_TEE_HP_PERIPH_TG1,                  /*!< Timer Group 1 */
    APM_TEE_HP_PERIPH_SYSTIMER,             /*!< Systimer */
    APM_TEE_HP_PERIPH_MISC,                 /*!< Misc */
    APM_TEE_HP_PERIPH_PVT_MON = 26,         /*!< PVT Monitor */
    APM_TEE_HP_PERIPH_PCNT,                 /*!< Pulse Counter */
    APM_TEE_HP_PERIPH_IOMUX,                /*!< IO MUX */
    APM_TEE_HP_PERIPH_PSRAM_MON,            /*!< PSRAM Monitor */
    APM_TEE_HP_PERIPH_MEM_MON,              /*!< Memory Monitor */
    APM_TEE_HP_PERIPH_SYSTEM_REG,           /*!< System registers */
    APM_TEE_HP_PERIPH_PCR_REG,              /*!< PCR registers */
    APM_TEE_HP_PERIPH_MSPI,                 /*!< MSPI */
    APM_TEE_HP_PERIPH_HP_APM,               /*!< HP APM */
    APM_TEE_HP_PERIPH_CPU_APM,              /*!< CPU APM */
    APM_TEE_HP_PERIPH_TEE,                  /*!< TEE */
    APM_TEE_HP_PERIPH_CRYPTO,               /*!< Crypto */
    APM_TEE_HP_PERIPH_TRACE,                /*!< Trace */
    APM_TEE_HP_PERIPH_CPU_BUS_MON = 40,     /*!< CPU Bus Monitor */
    APM_TEE_HP_PERIPH_INTPRI_REG,           /*!< Interrupt Priority registers */
    APM_TEE_HP_PERIPH_CACHE_CFG,            /*!< Cache */
    APM_TEE_HP_PERIPH_MODEM,                /*!< Modem */
    APM_TEE_HP_PERIPH_TWAI1,                /*!< TWAI1 */
    APM_TEE_HP_PERIPH_SPI2,                 /*!< SPI2 */
    APM_TEE_HP_PERIPH_BIT_SCRAMBLER,        /*!< Bit Scrambler */
    APM_TEE_HP_PERIPH_KEY_MANAGER,          /*!< Key Manager */
    APM_TEE_HP_PERIPH_MODEM_PWR,            /*!< Modem Power */
    APM_TEE_HP_PERIPH_HINF,                 /*!< Host Interface */
    APM_TEE_HP_PERIPH_SLC,                  /*!< SLC */
    APM_TEE_HP_PERIPH_SLC_HOST = 52,        /*!< SLC Host */
    APM_TEE_HP_PERIPH_MAX                   /*!< Maximum ID */
} apm_tee_hp_periph_t;

/**
 * @brief LP-TEE Controller Peripherals
 */
typedef enum {
    APM_TEE_LP_PERIPH_EFUSE = 0,           /*!< EFUSE */
    APM_TEE_LP_PERIPH_PMU,                 /*!< PMU */
    APM_TEE_LP_PERIPH_CLKRST,              /*!< CLKRST */
    APM_TEE_LP_PERIPH_LP_AON_CTRL,         /*!< LP_AON_CTRL */
    APM_TEE_LP_PERIPH_LP_TIMER,            /*!< LP_TIMER */
    APM_TEE_LP_PERIPH_LP_WDT,              /*!< LP_WDT */
    APM_TEE_LP_PERIPH_LP_PERI,             /*!< LP_PERI */
    APM_TEE_LP_PERIPH_LP_ANA_PERI,         /*!< LP_ANA_PERI */
    APM_TEE_LP_PERIPH_LP_IO = 10,          /*!< LP_IO */
    APM_TEE_LP_PERIPH_LP_TEE = 12,         /*!< LP_TEE */
    APM_TEE_LP_PERIPH_UART,                /*!< UART */
    APM_TEE_LP_PERIPH_I2C_EXT = 15,        /*!< I2C_EXT */
    APM_TEE_LP_PERIPH_I2C_ANA_MST,         /*!< I2C_ANA_MST */
    APM_TEE_LP_PERIPH_HUK,                 /*!< HUK */
    APM_TEE_LP_PERIPH_LP_APM,              /*!< LP_APM */
    APM_TEE_LP_PERIPH_MAX                  /*!< Max ID */
} apm_tee_lp_periph_t;

#ifdef __cplusplus
}
#endif
