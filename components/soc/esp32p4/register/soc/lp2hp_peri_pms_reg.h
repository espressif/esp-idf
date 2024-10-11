/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** PMS_LP2HP_PERI_PMS_DATE_REG register
 *  Version control register
 */
#define PMS_LP2HP_PERI_PMS_DATE_REG (DR_REG_LP2HP_PERI_PMS_BASE + 0x0)
/** PMS_LP2HP_PERI_PMS_DATE : R/W; bitpos: [31:0]; default: 2294790;
 *  Version control register.
 */
#define PMS_LP2HP_PERI_PMS_DATE    0xFFFFFFFFU
#define PMS_LP2HP_PERI_PMS_DATE_M  (PMS_LP2HP_PERI_PMS_DATE_V << PMS_LP2HP_PERI_PMS_DATE_S)
#define PMS_LP2HP_PERI_PMS_DATE_V  0xFFFFFFFFU
#define PMS_LP2HP_PERI_PMS_DATE_S  0

/** PMS_LP2HP_PERI_PMS_CLK_EN_REG register
 *  Clock gating register
 */
#define PMS_LP2HP_PERI_PMS_CLK_EN_REG (DR_REG_LP2HP_PERI_PMS_BASE + 0x4)
/** PMS_LP2HP_PERI_PMS_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: Enable automatic clock gating.
 *  1: Keep the clock always on.
 */
#define PMS_LP2HP_PERI_PMS_CLK_EN    (BIT(0))
#define PMS_LP2HP_PERI_PMS_CLK_EN_M  (PMS_LP2HP_PERI_PMS_CLK_EN_V << PMS_LP2HP_PERI_PMS_CLK_EN_S)
#define PMS_LP2HP_PERI_PMS_CLK_EN_V  0x00000001U
#define PMS_LP2HP_PERI_PMS_CLK_EN_S  0

/** PMS_LP_MM_PMS_REG0_REG register
 *  Permission control register0 for the LP CPU in machine mode
 */
#define PMS_LP_MM_PMS_REG0_REG (DR_REG_LP2HP_PERI_PMS_BASE + 0x8)
/** PMS_LP_MM_PSRAM_ALLOW : R/W; bitpos: [0]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access external RAM
 *  without going through cache.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_PSRAM_ALLOW    (BIT(0))
#define PMS_LP_MM_PSRAM_ALLOW_M  (PMS_LP_MM_PSRAM_ALLOW_V << PMS_LP_MM_PSRAM_ALLOW_S)
#define PMS_LP_MM_PSRAM_ALLOW_V  0x00000001U
#define PMS_LP_MM_PSRAM_ALLOW_S  0
/** PMS_LP_MM_FLASH_ALLOW : R/W; bitpos: [1]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access external
 *  flash without going through cache.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_FLASH_ALLOW    (BIT(1))
#define PMS_LP_MM_FLASH_ALLOW_M  (PMS_LP_MM_FLASH_ALLOW_V << PMS_LP_MM_FLASH_ALLOW_S)
#define PMS_LP_MM_FLASH_ALLOW_V  0x00000001U
#define PMS_LP_MM_FLASH_ALLOW_S  1
/** PMS_LP_MM_L2MEM_ALLOW : R/W; bitpos: [2]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP L2M2M
 *  without going through cache.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_L2MEM_ALLOW    (BIT(2))
#define PMS_LP_MM_L2MEM_ALLOW_M  (PMS_LP_MM_L2MEM_ALLOW_V << PMS_LP_MM_L2MEM_ALLOW_S)
#define PMS_LP_MM_L2MEM_ALLOW_V  0x00000001U
#define PMS_LP_MM_L2MEM_ALLOW_S  2
/** PMS_LP_MM_L2ROM_ALLOW : R/W; bitpos: [3]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP ROM
 *  without going through cache.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_L2ROM_ALLOW    (BIT(3))
#define PMS_LP_MM_L2ROM_ALLOW_M  (PMS_LP_MM_L2ROM_ALLOW_V << PMS_LP_MM_L2ROM_ALLOW_S)
#define PMS_LP_MM_L2ROM_ALLOW_V  0x00000001U
#define PMS_LP_MM_L2ROM_ALLOW_S  3
/** PMS_LP_MM_TRACE0_ALLOW : R/W; bitpos: [6]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access TRACE0.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_TRACE0_ALLOW    (BIT(6))
#define PMS_LP_MM_TRACE0_ALLOW_M  (PMS_LP_MM_TRACE0_ALLOW_V << PMS_LP_MM_TRACE0_ALLOW_S)
#define PMS_LP_MM_TRACE0_ALLOW_V  0x00000001U
#define PMS_LP_MM_TRACE0_ALLOW_S  6
/** PMS_LP_MM_TRACE1_ALLOW : R/W; bitpos: [7]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access TRACE1.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_TRACE1_ALLOW    (BIT(7))
#define PMS_LP_MM_TRACE1_ALLOW_M  (PMS_LP_MM_TRACE1_ALLOW_V << PMS_LP_MM_TRACE1_ALLOW_S)
#define PMS_LP_MM_TRACE1_ALLOW_V  0x00000001U
#define PMS_LP_MM_TRACE1_ALLOW_S  7
/** PMS_LP_MM_CPU_BUS_MON_ALLOW : R/W; bitpos: [8]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access CPU bus
 *  monitor.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_CPU_BUS_MON_ALLOW    (BIT(8))
#define PMS_LP_MM_CPU_BUS_MON_ALLOW_M  (PMS_LP_MM_CPU_BUS_MON_ALLOW_V << PMS_LP_MM_CPU_BUS_MON_ALLOW_S)
#define PMS_LP_MM_CPU_BUS_MON_ALLOW_V  0x00000001U
#define PMS_LP_MM_CPU_BUS_MON_ALLOW_S  8
/** PMS_LP_MM_L2MEM_MON_ALLOW : R/W; bitpos: [9]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access L2MEM
 *  monitor.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_L2MEM_MON_ALLOW    (BIT(9))
#define PMS_LP_MM_L2MEM_MON_ALLOW_M  (PMS_LP_MM_L2MEM_MON_ALLOW_V << PMS_LP_MM_L2MEM_MON_ALLOW_S)
#define PMS_LP_MM_L2MEM_MON_ALLOW_V  0x00000001U
#define PMS_LP_MM_L2MEM_MON_ALLOW_S  9
/** PMS_LP_MM_TCM_MON_ALLOW : R/W; bitpos: [10]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access TCM monitor.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_TCM_MON_ALLOW    (BIT(10))
#define PMS_LP_MM_TCM_MON_ALLOW_M  (PMS_LP_MM_TCM_MON_ALLOW_V << PMS_LP_MM_TCM_MON_ALLOW_S)
#define PMS_LP_MM_TCM_MON_ALLOW_V  0x00000001U
#define PMS_LP_MM_TCM_MON_ALLOW_S  10
/** PMS_LP_MM_CACHE_ALLOW : R/W; bitpos: [11]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access cache.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_CACHE_ALLOW    (BIT(11))
#define PMS_LP_MM_CACHE_ALLOW_M  (PMS_LP_MM_CACHE_ALLOW_V << PMS_LP_MM_CACHE_ALLOW_S)
#define PMS_LP_MM_CACHE_ALLOW_V  0x00000001U
#define PMS_LP_MM_CACHE_ALLOW_S  11

/** PMS_LP_MM_PMS_REG1_REG register
 *  Permission control register1 for the LP CPU in machine mode
 */
#define PMS_LP_MM_PMS_REG1_REG (DR_REG_LP2HP_PERI_PMS_BASE + 0x30)
/** PMS_LP_MM_HP_USBOTG_ALLOW : R/W; bitpos: [0]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP
 *  high-speed USB 2.0 OTG.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_USBOTG_ALLOW    (BIT(0))
#define PMS_LP_MM_HP_USBOTG_ALLOW_M  (PMS_LP_MM_HP_USBOTG_ALLOW_V << PMS_LP_MM_HP_USBOTG_ALLOW_S)
#define PMS_LP_MM_HP_USBOTG_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_USBOTG_ALLOW_S  0
/** PMS_LP_MM_HP_USBOTG11_ALLOW : R/W; bitpos: [1]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP
 *  full-speed USB 2.0 OTG.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_USBOTG11_ALLOW    (BIT(1))
#define PMS_LP_MM_HP_USBOTG11_ALLOW_M  (PMS_LP_MM_HP_USBOTG11_ALLOW_V << PMS_LP_MM_HP_USBOTG11_ALLOW_S)
#define PMS_LP_MM_HP_USBOTG11_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_USBOTG11_ALLOW_S  1
/** PMS_LP_MM_HP_USBOTG11_WRAP_ALLOW : R/W; bitpos: [2]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP
 *  full-speed USB 2.0 OTG's wrap.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_USBOTG11_WRAP_ALLOW    (BIT(2))
#define PMS_LP_MM_HP_USBOTG11_WRAP_ALLOW_M  (PMS_LP_MM_HP_USBOTG11_WRAP_ALLOW_V << PMS_LP_MM_HP_USBOTG11_WRAP_ALLOW_S)
#define PMS_LP_MM_HP_USBOTG11_WRAP_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_USBOTG11_WRAP_ALLOW_S  2
/** PMS_LP_MM_HP_GDMA_ALLOW : R/W; bitpos: [3]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP DW-GDMA.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_GDMA_ALLOW    (BIT(3))
#define PMS_LP_MM_HP_GDMA_ALLOW_M  (PMS_LP_MM_HP_GDMA_ALLOW_V << PMS_LP_MM_HP_GDMA_ALLOW_S)
#define PMS_LP_MM_HP_GDMA_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_GDMA_ALLOW_S  3
/** PMS_LP_MM_HP_REGDMA_ALLOW : R/W; bitpos: [4]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP GDMA (DW
 *  GDMA).
 *  0: Not allowed
 *  1: Allow
 */
#define PMS_LP_MM_HP_REGDMA_ALLOW    (BIT(4))
#define PMS_LP_MM_HP_REGDMA_ALLOW_M  (PMS_LP_MM_HP_REGDMA_ALLOW_V << PMS_LP_MM_HP_REGDMA_ALLOW_S)
#define PMS_LP_MM_HP_REGDMA_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_REGDMA_ALLOW_S  4
/** PMS_LP_MM_HP_SDMMC_ALLOW : R/W; bitpos: [5]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP SDMMC.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_SDMMC_ALLOW    (BIT(5))
#define PMS_LP_MM_HP_SDMMC_ALLOW_M  (PMS_LP_MM_HP_SDMMC_ALLOW_V << PMS_LP_MM_HP_SDMMC_ALLOW_S)
#define PMS_LP_MM_HP_SDMMC_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_SDMMC_ALLOW_S  5
/** PMS_LP_MM_HP_AHB_PDMA_ALLOW : R/W; bitpos: [6]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access GDMA-AHB.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_AHB_PDMA_ALLOW    (BIT(6))
#define PMS_LP_MM_HP_AHB_PDMA_ALLOW_M  (PMS_LP_MM_HP_AHB_PDMA_ALLOW_V << PMS_LP_MM_HP_AHB_PDMA_ALLOW_S)
#define PMS_LP_MM_HP_AHB_PDMA_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_AHB_PDMA_ALLOW_S  6
/** PMS_LP_MM_HP_JPEG_ALLOW : R/W; bitpos: [7]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP JPEG.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_JPEG_ALLOW    (BIT(7))
#define PMS_LP_MM_HP_JPEG_ALLOW_M  (PMS_LP_MM_HP_JPEG_ALLOW_V << PMS_LP_MM_HP_JPEG_ALLOW_S)
#define PMS_LP_MM_HP_JPEG_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_JPEG_ALLOW_S  7
/** PMS_LP_MM_HP_PPA_ALLOW : R/W; bitpos: [8]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP PPA.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_PPA_ALLOW    (BIT(8))
#define PMS_LP_MM_HP_PPA_ALLOW_M  (PMS_LP_MM_HP_PPA_ALLOW_V << PMS_LP_MM_HP_PPA_ALLOW_S)
#define PMS_LP_MM_HP_PPA_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_PPA_ALLOW_S  8
/** PMS_LP_MM_HP_DMA2D_ALLOW : R/W; bitpos: [9]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP 2D-DMA.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_DMA2D_ALLOW    (BIT(9))
#define PMS_LP_MM_HP_DMA2D_ALLOW_M  (PMS_LP_MM_HP_DMA2D_ALLOW_V << PMS_LP_MM_HP_DMA2D_ALLOW_S)
#define PMS_LP_MM_HP_DMA2D_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_DMA2D_ALLOW_S  9
/** PMS_LP_MM_HP_KEY_MANAGER_ALLOW : R/W; bitpos: [10]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP key
 *  manager.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_KEY_MANAGER_ALLOW    (BIT(10))
#define PMS_LP_MM_HP_KEY_MANAGER_ALLOW_M  (PMS_LP_MM_HP_KEY_MANAGER_ALLOW_V << PMS_LP_MM_HP_KEY_MANAGER_ALLOW_S)
#define PMS_LP_MM_HP_KEY_MANAGER_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_KEY_MANAGER_ALLOW_S  10
/** PMS_LP_MM_HP_AXI_PDMA_ALLOW : R/W; bitpos: [11]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP GDMA-AXI.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_AXI_PDMA_ALLOW    (BIT(11))
#define PMS_LP_MM_HP_AXI_PDMA_ALLOW_M  (PMS_LP_MM_HP_AXI_PDMA_ALLOW_V << PMS_LP_MM_HP_AXI_PDMA_ALLOW_S)
#define PMS_LP_MM_HP_AXI_PDMA_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_AXI_PDMA_ALLOW_S  11
/** PMS_LP_MM_HP_FLASH_ALLOW : R/W; bitpos: [12]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP flash
 *  MSPI controller.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_FLASH_ALLOW    (BIT(12))
#define PMS_LP_MM_HP_FLASH_ALLOW_M  (PMS_LP_MM_HP_FLASH_ALLOW_V << PMS_LP_MM_HP_FLASH_ALLOW_S)
#define PMS_LP_MM_HP_FLASH_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_FLASH_ALLOW_S  12
/** PMS_LP_MM_HP_PSRAM_ALLOW : R/W; bitpos: [13]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP PSRAM
 *  MSPI controller.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_PSRAM_ALLOW    (BIT(13))
#define PMS_LP_MM_HP_PSRAM_ALLOW_M  (PMS_LP_MM_HP_PSRAM_ALLOW_V << PMS_LP_MM_HP_PSRAM_ALLOW_S)
#define PMS_LP_MM_HP_PSRAM_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_PSRAM_ALLOW_S  13
/** PMS_LP_MM_HP_CRYPTO_ALLOW : R/W; bitpos: [14]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP CRYPTO.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_CRYPTO_ALLOW    (BIT(14))
#define PMS_LP_MM_HP_CRYPTO_ALLOW_M  (PMS_LP_MM_HP_CRYPTO_ALLOW_V << PMS_LP_MM_HP_CRYPTO_ALLOW_S)
#define PMS_LP_MM_HP_CRYPTO_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_CRYPTO_ALLOW_S  14
/** PMS_LP_MM_HP_GMAC_ALLOW : R/W; bitpos: [15]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP EMAC.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_GMAC_ALLOW    (BIT(15))
#define PMS_LP_MM_HP_GMAC_ALLOW_M  (PMS_LP_MM_HP_GMAC_ALLOW_V << PMS_LP_MM_HP_GMAC_ALLOW_S)
#define PMS_LP_MM_HP_GMAC_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_GMAC_ALLOW_S  15
/** PMS_LP_MM_HP_USB_PHY_ALLOW : R/W; bitpos: [16]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP
 *  high-speed USB 2.0 OTG PHY.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_USB_PHY_ALLOW    (BIT(16))
#define PMS_LP_MM_HP_USB_PHY_ALLOW_M  (PMS_LP_MM_HP_USB_PHY_ALLOW_V << PMS_LP_MM_HP_USB_PHY_ALLOW_S)
#define PMS_LP_MM_HP_USB_PHY_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_USB_PHY_ALLOW_S  16
/** PMS_LP_MM_HP_PVT_ALLOW : R/W; bitpos: [17]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP PVT.
 *  0: Not allowed
 *  1: Allow
 */
#define PMS_LP_MM_HP_PVT_ALLOW    (BIT(17))
#define PMS_LP_MM_HP_PVT_ALLOW_M  (PMS_LP_MM_HP_PVT_ALLOW_V << PMS_LP_MM_HP_PVT_ALLOW_S)
#define PMS_LP_MM_HP_PVT_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_PVT_ALLOW_S  17
/** PMS_LP_MM_HP_CSI_HOST_ALLOW : R/W; bitpos: [18]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP MIPI CSI
 *  host.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_CSI_HOST_ALLOW    (BIT(18))
#define PMS_LP_MM_HP_CSI_HOST_ALLOW_M  (PMS_LP_MM_HP_CSI_HOST_ALLOW_V << PMS_LP_MM_HP_CSI_HOST_ALLOW_S)
#define PMS_LP_MM_HP_CSI_HOST_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_CSI_HOST_ALLOW_S  18
/** PMS_LP_MM_HP_DSI_HOST_ALLOW : R/W; bitpos: [19]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP MIPI DSI
 *  host.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_DSI_HOST_ALLOW    (BIT(19))
#define PMS_LP_MM_HP_DSI_HOST_ALLOW_M  (PMS_LP_MM_HP_DSI_HOST_ALLOW_V << PMS_LP_MM_HP_DSI_HOST_ALLOW_S)
#define PMS_LP_MM_HP_DSI_HOST_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_DSI_HOST_ALLOW_S  19
/** PMS_LP_MM_HP_ISP_ALLOW : R/W; bitpos: [20]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP ISP.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_ISP_ALLOW    (BIT(20))
#define PMS_LP_MM_HP_ISP_ALLOW_M  (PMS_LP_MM_HP_ISP_ALLOW_V << PMS_LP_MM_HP_ISP_ALLOW_S)
#define PMS_LP_MM_HP_ISP_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_ISP_ALLOW_S  20
/** PMS_LP_MM_HP_H264_CORE_ALLOW : R/W; bitpos: [21]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP H264
 *  Encoder.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_H264_CORE_ALLOW    (BIT(21))
#define PMS_LP_MM_HP_H264_CORE_ALLOW_M  (PMS_LP_MM_HP_H264_CORE_ALLOW_V << PMS_LP_MM_HP_H264_CORE_ALLOW_S)
#define PMS_LP_MM_HP_H264_CORE_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_H264_CORE_ALLOW_S  21
/** PMS_LP_MM_HP_RMT_ALLOW : R/W; bitpos: [22]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP RMT.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_RMT_ALLOW    (BIT(22))
#define PMS_LP_MM_HP_RMT_ALLOW_M  (PMS_LP_MM_HP_RMT_ALLOW_V << PMS_LP_MM_HP_RMT_ALLOW_S)
#define PMS_LP_MM_HP_RMT_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_RMT_ALLOW_S  22
/** PMS_LP_MM_HP_BITSRAMBLER_ALLOW : R/W; bitpos: [23]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP bit
 *  scrambler.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_BITSRAMBLER_ALLOW    (BIT(23))
#define PMS_LP_MM_HP_BITSRAMBLER_ALLOW_M  (PMS_LP_MM_HP_BITSRAMBLER_ALLOW_V << PMS_LP_MM_HP_BITSRAMBLER_ALLOW_S)
#define PMS_LP_MM_HP_BITSRAMBLER_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_BITSRAMBLER_ALLOW_S  23
/** PMS_LP_MM_HP_AXI_ICM_ALLOW : R/W; bitpos: [24]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP AXI ICM.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_AXI_ICM_ALLOW    (BIT(24))
#define PMS_LP_MM_HP_AXI_ICM_ALLOW_M  (PMS_LP_MM_HP_AXI_ICM_ALLOW_V << PMS_LP_MM_HP_AXI_ICM_ALLOW_S)
#define PMS_LP_MM_HP_AXI_ICM_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_AXI_ICM_ALLOW_S  24
/** PMS_LP_MM_HP_PERI_PMS_ALLOW : R/W; bitpos: [25]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access
 *  HP_PERI_PMS_REG.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_PERI_PMS_ALLOW    (BIT(25))
#define PMS_LP_MM_HP_PERI_PMS_ALLOW_M  (PMS_LP_MM_HP_PERI_PMS_ALLOW_V << PMS_LP_MM_HP_PERI_PMS_ALLOW_S)
#define PMS_LP_MM_HP_PERI_PMS_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_PERI_PMS_ALLOW_S  25
/** PMS_LP_MM_LP2HP_PERI_PMS_ALLOW : R/W; bitpos: [26]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access
 *  LP2HP_PERI_PMS_REG.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP2HP_PERI_PMS_ALLOW    (BIT(26))
#define PMS_LP_MM_LP2HP_PERI_PMS_ALLOW_M  (PMS_LP_MM_LP2HP_PERI_PMS_ALLOW_V << PMS_LP_MM_LP2HP_PERI_PMS_ALLOW_S)
#define PMS_LP_MM_LP2HP_PERI_PMS_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP2HP_PERI_PMS_ALLOW_S  26
/** PMS_LP_MM_DMA_PMS_ALLOW : R/W; bitpos: [27]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access
 *  HP_DMA_PMS_REG.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_DMA_PMS_ALLOW    (BIT(27))
#define PMS_LP_MM_DMA_PMS_ALLOW_M  (PMS_LP_MM_DMA_PMS_ALLOW_V << PMS_LP_MM_DMA_PMS_ALLOW_S)
#define PMS_LP_MM_DMA_PMS_ALLOW_V  0x00000001U
#define PMS_LP_MM_DMA_PMS_ALLOW_S  27
/** PMS_LP_MM_HP_H264_DMA2D_ALLOW : R/W; bitpos: [28]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access 2D-DMA.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_H264_DMA2D_ALLOW    (BIT(28))
#define PMS_LP_MM_HP_H264_DMA2D_ALLOW_M  (PMS_LP_MM_HP_H264_DMA2D_ALLOW_V << PMS_LP_MM_HP_H264_DMA2D_ALLOW_S)
#define PMS_LP_MM_HP_H264_DMA2D_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_H264_DMA2D_ALLOW_S  28

/** PMS_LP_MM_PMS_REG2_REG register
 *  Permission control register2 for the LP CPU in machine mode
 */
#define PMS_LP_MM_PMS_REG2_REG (DR_REG_LP2HP_PERI_PMS_BASE + 0xa4)
/** PMS_LP_MM_HP_MCPWM0_ALLOW : R/W; bitpos: [0]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP MCPWM0.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_MCPWM0_ALLOW    (BIT(0))
#define PMS_LP_MM_HP_MCPWM0_ALLOW_M  (PMS_LP_MM_HP_MCPWM0_ALLOW_V << PMS_LP_MM_HP_MCPWM0_ALLOW_S)
#define PMS_LP_MM_HP_MCPWM0_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_MCPWM0_ALLOW_S  0
/** PMS_LP_MM_HP_MCPWM1_ALLOW : R/W; bitpos: [1]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP MCPWM1.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_MCPWM1_ALLOW    (BIT(1))
#define PMS_LP_MM_HP_MCPWM1_ALLOW_M  (PMS_LP_MM_HP_MCPWM1_ALLOW_V << PMS_LP_MM_HP_MCPWM1_ALLOW_S)
#define PMS_LP_MM_HP_MCPWM1_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_MCPWM1_ALLOW_S  1
/** PMS_LP_MM_HP_TIMER_GROUP0_ALLOW : R/W; bitpos: [2]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP timer
 *  group0.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_TIMER_GROUP0_ALLOW    (BIT(2))
#define PMS_LP_MM_HP_TIMER_GROUP0_ALLOW_M  (PMS_LP_MM_HP_TIMER_GROUP0_ALLOW_V << PMS_LP_MM_HP_TIMER_GROUP0_ALLOW_S)
#define PMS_LP_MM_HP_TIMER_GROUP0_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_TIMER_GROUP0_ALLOW_S  2
/** PMS_LP_MM_HP_TIMER_GROUP1_ALLOW : R/W; bitpos: [3]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP timer
 *  group1.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_TIMER_GROUP1_ALLOW    (BIT(3))
#define PMS_LP_MM_HP_TIMER_GROUP1_ALLOW_M  (PMS_LP_MM_HP_TIMER_GROUP1_ALLOW_V << PMS_LP_MM_HP_TIMER_GROUP1_ALLOW_S)
#define PMS_LP_MM_HP_TIMER_GROUP1_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_TIMER_GROUP1_ALLOW_S  3
/** PMS_LP_MM_HP_I2C0_ALLOW : R/W; bitpos: [4]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP I2C0.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_I2C0_ALLOW    (BIT(4))
#define PMS_LP_MM_HP_I2C0_ALLOW_M  (PMS_LP_MM_HP_I2C0_ALLOW_V << PMS_LP_MM_HP_I2C0_ALLOW_S)
#define PMS_LP_MM_HP_I2C0_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_I2C0_ALLOW_S  4
/** PMS_LP_MM_HP_I2C1_ALLOW : R/W; bitpos: [5]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP I2C1.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_I2C1_ALLOW    (BIT(5))
#define PMS_LP_MM_HP_I2C1_ALLOW_M  (PMS_LP_MM_HP_I2C1_ALLOW_V << PMS_LP_MM_HP_I2C1_ALLOW_S)
#define PMS_LP_MM_HP_I2C1_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_I2C1_ALLOW_S  5
/** PMS_LP_MM_HP_I2S0_ALLOW : R/W; bitpos: [6]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP I2S0.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_I2S0_ALLOW    (BIT(6))
#define PMS_LP_MM_HP_I2S0_ALLOW_M  (PMS_LP_MM_HP_I2S0_ALLOW_V << PMS_LP_MM_HP_I2S0_ALLOW_S)
#define PMS_LP_MM_HP_I2S0_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_I2S0_ALLOW_S  6
/** PMS_LP_MM_HP_I2S1_ALLOW : R/W; bitpos: [7]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP I2S1.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_I2S1_ALLOW    (BIT(7))
#define PMS_LP_MM_HP_I2S1_ALLOW_M  (PMS_LP_MM_HP_I2S1_ALLOW_V << PMS_LP_MM_HP_I2S1_ALLOW_S)
#define PMS_LP_MM_HP_I2S1_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_I2S1_ALLOW_S  7
/** PMS_LP_MM_HP_I2S2_ALLOW : R/W; bitpos: [8]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP I2S2.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_I2S2_ALLOW    (BIT(8))
#define PMS_LP_MM_HP_I2S2_ALLOW_M  (PMS_LP_MM_HP_I2S2_ALLOW_V << PMS_LP_MM_HP_I2S2_ALLOW_S)
#define PMS_LP_MM_HP_I2S2_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_I2S2_ALLOW_S  8
/** PMS_LP_MM_HP_PCNT_ALLOW : R/W; bitpos: [9]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP PCNT.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_PCNT_ALLOW    (BIT(9))
#define PMS_LP_MM_HP_PCNT_ALLOW_M  (PMS_LP_MM_HP_PCNT_ALLOW_V << PMS_LP_MM_HP_PCNT_ALLOW_S)
#define PMS_LP_MM_HP_PCNT_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_PCNT_ALLOW_S  9
/** PMS_LP_MM_HP_UART0_ALLOW : R/W; bitpos: [10]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP UART0.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_UART0_ALLOW    (BIT(10))
#define PMS_LP_MM_HP_UART0_ALLOW_M  (PMS_LP_MM_HP_UART0_ALLOW_V << PMS_LP_MM_HP_UART0_ALLOW_S)
#define PMS_LP_MM_HP_UART0_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_UART0_ALLOW_S  10
/** PMS_LP_MM_HP_UART1_ALLOW : R/W; bitpos: [11]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP UART1.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_UART1_ALLOW    (BIT(11))
#define PMS_LP_MM_HP_UART1_ALLOW_M  (PMS_LP_MM_HP_UART1_ALLOW_V << PMS_LP_MM_HP_UART1_ALLOW_S)
#define PMS_LP_MM_HP_UART1_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_UART1_ALLOW_S  11
/** PMS_LP_MM_HP_UART2_ALLOW : R/W; bitpos: [12]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP UART2.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_UART2_ALLOW    (BIT(12))
#define PMS_LP_MM_HP_UART2_ALLOW_M  (PMS_LP_MM_HP_UART2_ALLOW_V << PMS_LP_MM_HP_UART2_ALLOW_S)
#define PMS_LP_MM_HP_UART2_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_UART2_ALLOW_S  12
/** PMS_LP_MM_HP_UART3_ALLOW : R/W; bitpos: [13]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP UART3.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_UART3_ALLOW    (BIT(13))
#define PMS_LP_MM_HP_UART3_ALLOW_M  (PMS_LP_MM_HP_UART3_ALLOW_V << PMS_LP_MM_HP_UART3_ALLOW_S)
#define PMS_LP_MM_HP_UART3_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_UART3_ALLOW_S  13
/** PMS_LP_MM_HP_UART4_ALLOW : R/W; bitpos: [14]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP UART4.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_UART4_ALLOW    (BIT(14))
#define PMS_LP_MM_HP_UART4_ALLOW_M  (PMS_LP_MM_HP_UART4_ALLOW_V << PMS_LP_MM_HP_UART4_ALLOW_S)
#define PMS_LP_MM_HP_UART4_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_UART4_ALLOW_S  14
/** PMS_LP_MM_HP_PARLIO_ALLOW : R/W; bitpos: [15]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP PARLIO.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_PARLIO_ALLOW    (BIT(15))
#define PMS_LP_MM_HP_PARLIO_ALLOW_M  (PMS_LP_MM_HP_PARLIO_ALLOW_V << PMS_LP_MM_HP_PARLIO_ALLOW_S)
#define PMS_LP_MM_HP_PARLIO_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_PARLIO_ALLOW_S  15
/** PMS_LP_MM_HP_GPSPI2_ALLOW : R/W; bitpos: [16]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP GP-SPI2.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_GPSPI2_ALLOW    (BIT(16))
#define PMS_LP_MM_HP_GPSPI2_ALLOW_M  (PMS_LP_MM_HP_GPSPI2_ALLOW_V << PMS_LP_MM_HP_GPSPI2_ALLOW_S)
#define PMS_LP_MM_HP_GPSPI2_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_GPSPI2_ALLOW_S  16
/** PMS_LP_MM_HP_GPSPI3_ALLOW : R/W; bitpos: [17]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP GP-SPI3.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_GPSPI3_ALLOW    (BIT(17))
#define PMS_LP_MM_HP_GPSPI3_ALLOW_M  (PMS_LP_MM_HP_GPSPI3_ALLOW_V << PMS_LP_MM_HP_GPSPI3_ALLOW_S)
#define PMS_LP_MM_HP_GPSPI3_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_GPSPI3_ALLOW_S  17
/** PMS_LP_MM_HP_USBDEVICE_ALLOW : R/W; bitpos: [18]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP
 *  USB/Serial JTAG Controller.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_USBDEVICE_ALLOW    (BIT(18))
#define PMS_LP_MM_HP_USBDEVICE_ALLOW_M  (PMS_LP_MM_HP_USBDEVICE_ALLOW_V << PMS_LP_MM_HP_USBDEVICE_ALLOW_S)
#define PMS_LP_MM_HP_USBDEVICE_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_USBDEVICE_ALLOW_S  18
/** PMS_LP_MM_HP_LEDC_ALLOW : R/W; bitpos: [19]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP LEDC.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_LEDC_ALLOW    (BIT(19))
#define PMS_LP_MM_HP_LEDC_ALLOW_M  (PMS_LP_MM_HP_LEDC_ALLOW_V << PMS_LP_MM_HP_LEDC_ALLOW_S)
#define PMS_LP_MM_HP_LEDC_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_LEDC_ALLOW_S  19
/** PMS_LP_MM_HP_ETM_ALLOW : R/W; bitpos: [21]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP ETM.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_ETM_ALLOW    (BIT(21))
#define PMS_LP_MM_HP_ETM_ALLOW_M  (PMS_LP_MM_HP_ETM_ALLOW_V << PMS_LP_MM_HP_ETM_ALLOW_S)
#define PMS_LP_MM_HP_ETM_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_ETM_ALLOW_S  21
/** PMS_LP_MM_HP_INTRMTX_ALLOW : R/W; bitpos: [22]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP interrupt
 *  matrix.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_INTRMTX_ALLOW    (BIT(22))
#define PMS_LP_MM_HP_INTRMTX_ALLOW_M  (PMS_LP_MM_HP_INTRMTX_ALLOW_V << PMS_LP_MM_HP_INTRMTX_ALLOW_S)
#define PMS_LP_MM_HP_INTRMTX_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_INTRMTX_ALLOW_S  22
/** PMS_LP_MM_HP_TWAI0_ALLOW : R/W; bitpos: [23]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP TWAI0.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_TWAI0_ALLOW    (BIT(23))
#define PMS_LP_MM_HP_TWAI0_ALLOW_M  (PMS_LP_MM_HP_TWAI0_ALLOW_V << PMS_LP_MM_HP_TWAI0_ALLOW_S)
#define PMS_LP_MM_HP_TWAI0_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_TWAI0_ALLOW_S  23
/** PMS_LP_MM_HP_TWAI1_ALLOW : R/W; bitpos: [24]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP TWAI1.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_TWAI1_ALLOW    (BIT(24))
#define PMS_LP_MM_HP_TWAI1_ALLOW_M  (PMS_LP_MM_HP_TWAI1_ALLOW_V << PMS_LP_MM_HP_TWAI1_ALLOW_S)
#define PMS_LP_MM_HP_TWAI1_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_TWAI1_ALLOW_S  24
/** PMS_LP_MM_HP_TWAI2_ALLOW : R/W; bitpos: [25]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP TWAI2.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_TWAI2_ALLOW    (BIT(25))
#define PMS_LP_MM_HP_TWAI2_ALLOW_M  (PMS_LP_MM_HP_TWAI2_ALLOW_V << PMS_LP_MM_HP_TWAI2_ALLOW_S)
#define PMS_LP_MM_HP_TWAI2_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_TWAI2_ALLOW_S  25
/** PMS_LP_MM_HP_I3C_MST_ALLOW : R/W; bitpos: [26]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP I3C
 *  master controller.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_I3C_MST_ALLOW    (BIT(26))
#define PMS_LP_MM_HP_I3C_MST_ALLOW_M  (PMS_LP_MM_HP_I3C_MST_ALLOW_V << PMS_LP_MM_HP_I3C_MST_ALLOW_S)
#define PMS_LP_MM_HP_I3C_MST_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_I3C_MST_ALLOW_S  26
/** PMS_LP_MM_HP_I3C_SLV_ALLOW : R/W; bitpos: [27]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP I3C slave
 *  controller.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_I3C_SLV_ALLOW    (BIT(27))
#define PMS_LP_MM_HP_I3C_SLV_ALLOW_M  (PMS_LP_MM_HP_I3C_SLV_ALLOW_V << PMS_LP_MM_HP_I3C_SLV_ALLOW_S)
#define PMS_LP_MM_HP_I3C_SLV_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_I3C_SLV_ALLOW_S  27
/** PMS_LP_MM_HP_LCDCAM_ALLOW : R/W; bitpos: [28]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP LCD_CAM.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_LCDCAM_ALLOW    (BIT(28))
#define PMS_LP_MM_HP_LCDCAM_ALLOW_M  (PMS_LP_MM_HP_LCDCAM_ALLOW_V << PMS_LP_MM_HP_LCDCAM_ALLOW_S)
#define PMS_LP_MM_HP_LCDCAM_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_LCDCAM_ALLOW_S  28
/** PMS_LP_MM_HP_ADC_ALLOW : R/W; bitpos: [30]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP ADC.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_ADC_ALLOW    (BIT(30))
#define PMS_LP_MM_HP_ADC_ALLOW_M  (PMS_LP_MM_HP_ADC_ALLOW_V << PMS_LP_MM_HP_ADC_ALLOW_S)
#define PMS_LP_MM_HP_ADC_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_ADC_ALLOW_S  30
/** PMS_LP_MM_HP_UHCI_ALLOW : R/W; bitpos: [31]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP UHCI.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_UHCI_ALLOW    (BIT(31))
#define PMS_LP_MM_HP_UHCI_ALLOW_M  (PMS_LP_MM_HP_UHCI_ALLOW_V << PMS_LP_MM_HP_UHCI_ALLOW_S)
#define PMS_LP_MM_HP_UHCI_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_UHCI_ALLOW_S  31

/** PMS_LP_MM_PMS_REG3_REG register
 *  Permission control register3 for the LP CPU in machine mode
 */
#define PMS_LP_MM_PMS_REG3_REG (DR_REG_LP2HP_PERI_PMS_BASE + 0x11c)
/** PMS_LP_MM_HP_GPIO_ALLOW : R/W; bitpos: [0]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP GPIO
 *  Matrix.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_GPIO_ALLOW    (BIT(0))
#define PMS_LP_MM_HP_GPIO_ALLOW_M  (PMS_LP_MM_HP_GPIO_ALLOW_V << PMS_LP_MM_HP_GPIO_ALLOW_S)
#define PMS_LP_MM_HP_GPIO_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_GPIO_ALLOW_S  0
/** PMS_LP_MM_HP_IOMUX_ALLOW : R/W; bitpos: [1]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP IO MUX.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_IOMUX_ALLOW    (BIT(1))
#define PMS_LP_MM_HP_IOMUX_ALLOW_M  (PMS_LP_MM_HP_IOMUX_ALLOW_V << PMS_LP_MM_HP_IOMUX_ALLOW_S)
#define PMS_LP_MM_HP_IOMUX_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_IOMUX_ALLOW_S  1
/** PMS_LP_MM_HP_SYSTIMER_ALLOW : R/W; bitpos: [2]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP system
 *  timer.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_SYSTIMER_ALLOW    (BIT(2))
#define PMS_LP_MM_HP_SYSTIMER_ALLOW_M  (PMS_LP_MM_HP_SYSTIMER_ALLOW_V << PMS_LP_MM_HP_SYSTIMER_ALLOW_S)
#define PMS_LP_MM_HP_SYSTIMER_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_SYSTIMER_ALLOW_S  2
/** PMS_LP_MM_HP_SYS_REG_ALLOW : R/W; bitpos: [3]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access HP system
 *  register.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_SYS_REG_ALLOW    (BIT(3))
#define PMS_LP_MM_HP_SYS_REG_ALLOW_M  (PMS_LP_MM_HP_SYS_REG_ALLOW_V << PMS_LP_MM_HP_SYS_REG_ALLOW_S)
#define PMS_LP_MM_HP_SYS_REG_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_SYS_REG_ALLOW_S  3
/** PMS_LP_MM_HP_CLKRST_ALLOW : R/W; bitpos: [4]; default: 1;
 *  Configures whether the LP CPU in machine mode has permission to access
 *  HP_SYS_CLKRST.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP_CLKRST_ALLOW    (BIT(4))
#define PMS_LP_MM_HP_CLKRST_ALLOW_M  (PMS_LP_MM_HP_CLKRST_ALLOW_V << PMS_LP_MM_HP_CLKRST_ALLOW_S)
#define PMS_LP_MM_HP_CLKRST_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP_CLKRST_ALLOW_S  4

#ifdef __cplusplus
}
#endif
