/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define COUNT_EFUSE_REG_PER_BLOCK 8      /* The number of registers per block. */

#define ESP_EFUSE_SECURE_VERSION_NUM_BLOCK EFUSE_BLK0

#define ESP_EFUSE_FIELD_CORRESPONDS_CODING_SCHEME(scheme, max_num_bit)

/*-------------------------- I2S CAPS ----------------------------------------*/
#define SOC_I2S_NUM                 (1)
#define SOC_I2S_SUPPORTS_PCM        (1)
#define SOC_I2S_SUPPORTS_PDM_TX     (1)
#define SOC_I2S_SUPPORTS_PDM_RX     (0)
#define SOC_I2S_SUPPORTS_PDM        (1) //(SOC_I2S_SUPPORTS_PDM_RX | SOC_I2S_SUPPORTS_PDM_TX)
#define SOC_I2S_SUPPORTS_TDM        (1)
#define SOC_GDMA_I2S0_DMA_CHANNEL   (0)

#ifdef __cplusplus
}
#endif

