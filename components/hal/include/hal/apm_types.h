/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Master IDs for APM
 */
typedef enum {
    APM_MASTER_HPCORE            = 0,    /*!< HP CPU */
#if SOC_LP_CORE_SUPPORTED
    APM_MASTER_LPCORE            = 1,    /*!< LP CPU */
#endif /* SOC_LP_CORE_SUPPORTED */
    APM_MASTER_REGDMA            = 2,
#if SOC_SDIO_SLAVE_SUPPORTED
    APM_MASTER_SDIOSLV           = 3,    /*!< SDIO slave */
#endif /* SOC_SDIO_SLAVE_SUPPORTED */
    APM_MASTER_MODEM             = 4,    /*!< Modem */
    APM_MASTER_MEM_MON           = 5,    /*!< Memory monitor */
    APM_MASTER_TRACE             = 6,    /*!< Trace */
#if SOC_SPIRAM_SUPPORTED
    APM_MASTER_PSRAM_MEM_MON     = 8,    /*!< PSRAM memory monitor */
#endif /* SOC_SPIRAM_SUPPORTED */
#if SOC_GPSPI_SUPPORTED && SOC_AHB_GDMA_VERSION == 1
    APM_MASTER_GDMA_GPSPI        = 16,   /*!< GDMA for GPSPI/SPI2 */
#elif SOC_GPSPI_SUPPORTED && SOC_AHB_GDMA_VERSION == 2
    APM_MASTER_GDMA_GPSPI        = 17,
#endif /* SOC_GPSPI_SUPPORTED */
#if SOC_UHCI_SUPPORTED
    APM_MASTER_GDMA_UHCI         = 18,   /*!< GDMA for UHCI */
#endif /* SOC_UHCI_SUPPORTED */
    APM_MASTER_GDMA_I2S          = 19,   /*!< GDMA for I2S */
#if SOC_AES_SUPPORTED
    APM_MASTER_GDMA_AES          = 22,   /*!< GDMA for AES */
#endif /* SOC_AES_SUPPORTED */
    APM_MASTER_GDMA_SHA          = 23,   /*!< GDMA for SHA */
    APM_MASTER_GDMA_ADC          = 24,   /*!< GDMA for ADC */
#if SOC_PARLIO_SUPPORTED
    APM_MASTER_GDMA_PARLIO       = 25,   /*!< GDMA for PARLIO */
#endif /* SOC_PARLIO_SUPPORTED */
    APM_MASTER_MAX               = 32,   /*!< Maximum master ID */
} apm_master_id_t;

/**
 * @brief APM controller modules
 */
typedef enum {
    APM_CTRL_HP_APM  = 0,  /*!< High-performance APM controller */
#if SOC_APM_LP_APM0_SUPPORTED
    APM_CTRL_LP_APM0,      /*!< Low-power APM0 controller */
#endif
    APM_CTRL_LP_APM,       /*!< Low-power APM controller */
#if SOC_APM_CPU_APM_SUPPORTED
    APM_CTRL_CPU_APM,      /*!< CPU APM controller */
#endif
} apm_ctrl_module_t;

/**
 * @brief TEE controller modules
 */
typedef enum {
    APM_TEE_CTRL_HP = 0,   /*!< High-performance TEE controller */
#if SOC_APM_SUPPORT_LP_TEE_CTRL
    APM_TEE_CTRL_LP = 1,   /*!< Low-power TEE controller */
#endif
} apm_tee_ctrl_module_t;

/**
 * @brief APM controller access paths
 */
typedef enum {
    APM_CTRL_ACCESS_PATH_M0 = 0,    /*!< Access path M0 */
    APM_CTRL_ACCESS_PATH_M1 = 1,    /*!< Access path M1 */
    APM_CTRL_ACCESS_PATH_M2 = 2,    /*!< Access path M2 */
    APM_CTRL_ACCESS_PATH_M3 = 3,    /*!< Access path M3 */
    APM_CTRL_ACCESS_PATH_M4 = 4,    /*!< Access path M4 */
} apm_ctrl_access_path_t;

/**
 * @brief Security modes for APM
 */
typedef enum {
    APM_SEC_MODE_TEE  = 0,  /*!< Trusted Execution Environment mode */
    APM_SEC_MODE_REE0 = 1,  /*!< Rich Execution Environment mode 0 */
    APM_SEC_MODE_REE1 = 2,  /*!< Rich Execution Environment mode 1 */
    APM_SEC_MODE_REE2 = 3,  /*!< Rich Execution Environment mode 2 */
} apm_security_mode_t;

/**
 * @brief Access permission flags
 */
typedef enum {
    APM_PERM_NONE = 0,    /*!< No permissions */
    APM_PERM_X    = 1,    /*!< Execute */
    APM_PERM_W    = 2,    /*!< Write */
    APM_PERM_R    = 4,    /*!< Read */
    APM_PERM_ALL  = 7,    /*!< All access */
} apm_perm_t;

/**
 * @brief APM controller exception info structure
 */
typedef struct {
    uint32_t regn;  /*!< Region number */
    uint32_t mode;  /*!< Security mode */
    uint32_t id;    /*!< Master ID */
    uint32_t type;  /*!< Exception type */
    uint32_t addr;  /*!< Exception address */
} apm_ctrl_exception_info_t;

#ifdef __cplusplus
}
#endif
