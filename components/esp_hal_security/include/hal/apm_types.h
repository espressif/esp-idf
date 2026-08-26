/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "esp_bit_defs.h"
#include "hal/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_APM_CTRL_FILTER_SUPPORTED
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
 * @brief APM controller exception info structure
 */
typedef struct {
    uint32_t regn;  /*!< Region number */
    uint32_t mode;  /*!< Security mode */
    uint32_t id;    /*!< Master ID */
    uint32_t type;  /*!< Exception type */
    uint32_t addr;  /*!< Exception address */
} apm_ctrl_exception_info_t;
#else
/**
 * @brief Security modes for APM
 */
typedef enum {
    APM_SEC_MODE_TEE  = 0,    /*!< Trusted Execution Environment mode (M-mode) */
    APM_SEC_MODE_REE  = 1,    /*!< Rich Execution Environment mode (U-mode) */
} apm_security_mode_t;

/**
 * @brief APM controller modules
 */
typedef enum {
    APM_CTRL_DMA_PMS = 0,       /*!< DMA APM controller */
    APM_CTRL_HP_PERI_PMS,       /*!< High-performance APM controller */
    APM_CTRL_HP2LP_PERI_PMS,    /*!< High-performance to Low-power APM controller */
    APM_CTRL_LP_PERI_PMS,       /*!< Low-power APM controller */
    APM_CTRL_LP2HP_PERI_PMS,    /*!< Low-power to High-performance APM controller */
} apm_ctrl_module_t;

/**
 * @brief Master IDs for APM
 */
typedef enum {
    APM_MASTER_HPCPU0 = 0,    /*!< High-performance CPU core 0 */
    APM_MASTER_HPCPU1,        /*!< High-performance CPU core 1 */
    APM_MASTER_LPCPU,         /*!< Low-power CPU */
    APM_MASTER_DMA,           /*!< DMA masters */
} apm_master_id_t;

/**
 * @brief DMA master IDs for APM
 */
typedef enum {
    APM_MASTER_DMA_GDMA_CH0 = 0,
    APM_MASTER_DMA_GDMA_CH1,
    APM_MASTER_DMA_GDMA_CH2,
    APM_MASTER_DMA_GDMA_CH3,
    APM_MASTER_DMA_AHB_PDMA_ADC,
    APM_MASTER_DMA_AHB_PDMA_I2S0,
    APM_MASTER_DMA_AHB_PDMA_I2S1,
    APM_MASTER_DMA_AHB_PDMA_I2S2,
    APM_MASTER_DMA_AHB_PDMA_I3C_MST,
    APM_MASTER_DMA_AHB_PDMA_UHCI0,
    APM_MASTER_DMA_AHB_PDMA_RMT,
    APM_MASTER_DMA_AXI_PDMA_LCDCAM = 13,
    APM_MASTER_DMA_AXI_PDMA_GPSPI2,
    APM_MASTER_DMA_AXI_PDMA_GPSPI3,
    APM_MASTER_DMA_AXI_PDMA_PARLIO,
    APM_MASTER_DMA_AXI_PDMA_AES,
    APM_MASTER_DMA_AXI_PDMA_SHA,
    APM_MASTER_DMA_DMA2D_JPEG,
    APM_MASTER_DMA_USBOTG_HS,
    APM_MASTER_DMA_GMAC,
    APM_MASTER_DMA_SDMMC,
    APM_MASTER_DMA_USBOTG_FS,
    APM_MASTER_DMA_TRACE0,
    APM_MASTER_DMA_TRACE1,
    APM_MASTER_DMA_L2MEM_MON,
    APM_MASTER_DMA_SPM_MON,
    APM_MASTER_DMA_H264 = 30,
    APM_MASTER_DMA_DMA2D_PPA,
    APM_MASTER_DMA_DMA2D_DUMMY,
    APM_MASTER_DMA_AHB_PDMA_DUMMY,
    APM_MASTER_DMA_AXI_PDMA_DUMMY,
    APM_MASTER_DMA_MAX,
} apm_master_dma_id_t;

/**
 * @brief HP peripheral slave IDs for APM
 */
typedef enum {
    APM_SLAVE_PSRAM = 0,
    APM_SLAVE_FLASH,
    APM_SLAVE_L2MEM,
    APM_SLAVE_L2ROM,
    APM_SLAVE_TRACE0 = 6,
    APM_SLAVE_TRACE1,
    APM_SLAVE_CPU_BUS_MON,
    APM_SLAVE_L2MEM_MON,
    APM_SLAVE_SPM_MON,
    APM_SLAVE_CACHE,
    APM_SLAVE_CPU_PERIPH_MAX,
} apm_slave_cpu_periph_id_t;

typedef enum {
    APM_SLAVE_HP_USBOTG_HS = 0,
    APM_SLAVE_HP_USBOTG_FS,
    APM_SLAVE_HP_USBOTG_FS_WRAP,
    APM_SLAVE_HP_GDMA,
    APM_SLAVE_HP_SDMMC = 5,
    APM_SLAVE_HP_AHB_PDMA,
    APM_SLAVE_HP_JPEG,
    APM_SLAVE_HP_PPA,
    APM_SLAVE_HP_DMA2D,
    APM_SLAVE_HP_KEY_MANAGER,
    APM_SLAVE_HP_AXI_PDMA,
    APM_SLAVE_HP_FLASH_MSPI,
    APM_SLAVE_HP_PSRAM_MSPI,
    APM_SLAVE_HP_CRYPTO,
    APM_SLAVE_HP_GMAC,
    APM_SLAVE_HP_USB_PHY,
    APM_SLAVE_HP_PVT,
    APM_SLAVE_HP_CSI_HOST,
    APM_SLAVE_HP_DSI_HOST,
    APM_SLAVE_HP_ISP,
    APM_SLAVE_HP_H264_CORE,
    APM_SLAVE_HP_RMT,
    APM_SLAVE_HP_BITSCRAMBLER,
    APM_SLAVE_HP_AXI_ICM,
    APM_SLAVE_HP_PERI_PMS,
    APM_SLAVE_LP2HP_PERI_PMS,
    APM_SLAVE_DMA_PMS,
    APM_SLAVE_HP_H264_DMA2D,
    APM_SLAVE_AXI_PERF_MON,
    APM_SLAVE_HP_PERIPH0_MAX,
} apm_slave_hp_periph0_id_t;

typedef enum {
    APM_SLAVE_HP_MCPWM0,
    APM_SLAVE_HP_MCPWM1,
    APM_SLAVE_HP_TG0,
    APM_SLAVE_HP_TG1,
    APM_SLAVE_HP_I2C0,
    APM_SLAVE_HP_I2C1,
    APM_SLAVE_HP_I2S0,
    APM_SLAVE_HP_I2S1,
    APM_SLAVE_HP_I2S2,
    APM_SLAVE_HP_PCNT,
    APM_SLAVE_HP_UART0,
    APM_SLAVE_HP_UART1,
    APM_SLAVE_HP_UART2,
    APM_SLAVE_HP_UART3,
    APM_SLAVE_HP_UART4,
    APM_SLAVE_HP_PARLIO,
    APM_SLAVE_HP_GPSPI2,
    APM_SLAVE_HP_GPSPI3,
    APM_SLAVE_HP_USBDEVICE,
    APM_SLAVE_HP_LEDC,
    APM_SLAVE_HP_ETM = 21,
    APM_SLAVE_HP_INTMTX,
    APM_SLAVE_HP_TWAI0,
    APM_SLAVE_HP_TWAI1,
    APM_SLAVE_HP_TWAI2,
    APM_SLAVE_HP_I3C_MST,
    APM_SLAVE_HP_I3C_SLV,
    APM_SLAVE_HP_LCDCAM,
    APM_SLAVE_HP_ADC = 30,
    APM_SLAVE_HP_UHCI,
    APM_SLAVE_HP_GPIO = 32,
    APM_SLAVE_HP_IOMUX,
    APM_SLAVE_HP_SYSTIMER,
    APM_SLAVE_HP_SYS_REG,
    APM_SLAVE_HP_CLKRST,
    APM_SLAVE_HP_PERIPH1_MAX,
} apm_slave_hp_periph1_id_t;

/**
 * @brief LP peripheral slave IDs for APM
 */
typedef enum {
    APM_SLAVE_LP_SYSREG = 0,
    APM_SLAVE_LP_AONCLKRST,
    APM_SLAVE_LP_TIMER,
    APM_SLAVE_LP_ANAPERI,
    APM_SLAVE_LP_PMU,
    APM_SLAVE_LP_WDT,
    APM_SLAVE_LP_MAILBOX,
    APM_SLAVE_LP_RTC,
    APM_SLAVE_LP_PERICLKRST,
    APM_SLAVE_LP_UART,
    APM_SLAVE_LP_I2C,
    APM_SLAVE_LP_SPI,
    APM_SLAVE_LP_I2CMST,
    APM_SLAVE_LP_I2S,
    APM_SLAVE_LP_ADC,
    APM_SLAVE_LP_TOUCH,
    APM_SLAVE_LP_IOMUX,
    APM_SLAVE_LP_INTR,
    APM_SLAVE_LP_EFUSE,
    APM_SLAVE_LP_PERI_PMS,
    APM_SLAVE_HP2LP_PERI_PMS,
    APM_SLAVE_LP_TSENS,
    APM_SLAVE_LP_HUK,
    APM_SLAVE_LP_SRAM,
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    APM_SLAVE_LP_TRNG,
#endif
    APM_SLAVE_LP_PERIPH_MAX,
} apm_slave_lp_periph_t;

/**
 * @brief APM ADDRHOLE exception types
 */
typedef enum {
    APM_EXCP_NONE     = 0,       /*!< No exception */
    APM_EXCP_HP_AXI   = BIT(0),  /*!< Exception from illegal access in HP AXI matrix */
    APM_EXCP_HP_AHB   = BIT(1),  /*!< Exception from illegal access in HP CPU and AHB matrix */
    APM_EXCP_LP_AHB   = BIT(2),  /*!< Exception from illegal access in LP AHB matrix */
    APM_EXCP_LP_IDBUS = BIT(3),  /*!< Exception from illegal access on LP CPU IBUS or DBUS */
} apm_ctrl_exception_type;

/**
 * @brief Exception info structure
 */
typedef struct {
    uint32_t addr;              /*!< Faulting address */
    uint8_t id;                 /*!< Master ID */
    bool is_wr;                 /*!< True indicates Write transaction, else Read transaction */
    bool is_secure;             /*!< True indicates illegal address access (space exception), else access without permission (authority exception) */
} apm_ctrl_exception_info_t;
#endif

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

#ifdef __cplusplus
}
#endif
