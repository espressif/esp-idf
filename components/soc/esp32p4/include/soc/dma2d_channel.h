/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// The following macros are matched with the 2D-DMA peri_sel field peripheral selection ID
#define SOC_DMA2D_TRIG_PERIPH_JPEG_RX          (0)
#define SOC_DMA2D_TRIG_PERIPH_PPA_SR_RX        (1)
#define SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_RX     (2)
#define SOC_DMA2D_TRIG_PERIPH_M2M_RX           (-1) // Any value of 3 ~ 7, TX and RX do not have to use same ID value for M2M

#define SOC_DMA2D_TRIG_PERIPH_JPEG_TX          (0)
#define SOC_DMA2D_TRIG_PERIPH_PPA_SR_TX        (1)
#define SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_FG_TX  (2)
#define SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_BG_TX  (3)
#define SOC_DMA2D_TRIG_PERIPH_M2M_TX           (-1) // Any value of 4 ~ 7, TX and RX do not have to use same ID value for M2M
