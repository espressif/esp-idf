/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_private/mipi_csi_share_hw_ctrl.h"

TEST_CASE("ISP CSI share usage test", "[isp]")
{
    int csi_brg_id = 0;
    TEST_ESP_OK(mipi_csi_brg_claim(MIPI_CSI_BRG_USER_CSI, &csi_brg_id));
    TEST_ESP_OK(mipi_csi_brg_claim(MIPI_CSI_BRG_USER_SHARE, &csi_brg_id));
    TEST_ESP_OK(mipi_csi_brg_declaim(csi_brg_id));
    TEST_ESP_OK(mipi_csi_brg_declaim(csi_brg_id));

    TEST_ESP_OK(mipi_csi_brg_claim(MIPI_CSI_BRG_USER_ISP_DVP, &csi_brg_id));
    TEST_ESP_OK(mipi_csi_brg_claim(MIPI_CSI_BRG_USER_SHARE, &csi_brg_id));
    TEST_ESP_OK(mipi_csi_brg_declaim(csi_brg_id));
    TEST_ESP_OK(mipi_csi_brg_declaim(csi_brg_id));

    TEST_ESP_OK(mipi_csi_brg_claim(MIPI_CSI_BRG_USER_ISP_DVP, &csi_brg_id));
    TEST_ASSERT(mipi_csi_brg_claim(MIPI_CSI_BRG_USER_CSI, &csi_brg_id) == ESP_ERR_NOT_FOUND);
    TEST_ESP_OK(mipi_csi_brg_declaim(csi_brg_id));
    TEST_ASSERT(mipi_csi_brg_declaim(csi_brg_id) == ESP_ERR_INVALID_STATE);
}
