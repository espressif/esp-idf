/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/mipi_csi_share_hw_ctrl.h"
#include "esp_private/critical_section.h"
#include "soc/soc_caps.h"
#include "hal/mipi_csi_ll.h"

#define MIPI_CSI_BRG_USER_NO_USER    99

typedef struct {
    mipi_csi_brg_user_t user[MIPI_CSI_BRG_LL_BRG_NUMS];
    int ref_cnt[MIPI_CSI_BRG_LL_BRG_NUMS];
    portMUX_TYPE spinlock;
} csi_brg_share_ctx_t;

static csi_brg_share_ctx_t s_ctx = {
    .user[0 ...(MIPI_CSI_BRG_LL_BRG_NUMS - 1)] = MIPI_CSI_BRG_USER_NO_USER,
    .ref_cnt[0 ...(MIPI_CSI_BRG_LL_BRG_NUMS - 1)] = 0,
    .spinlock = portMUX_INITIALIZER_UNLOCKED,
};

ESP_LOG_ATTR_TAG(TAG, "CSI_SHARE");

esp_err_t mipi_csi_brg_claim(mipi_csi_brg_user_t user, int *out_id)
{
    bool found = true;
    esp_os_enter_critical(&s_ctx.spinlock);
    for (int i = 0; i < MIPI_CSI_BRG_LL_BRG_NUMS; i ++) {
        bool user_is_shared = (s_ctx.user[i] == MIPI_CSI_BRG_USER_SHARE);
        bool to_share = (user == MIPI_CSI_BRG_USER_SHARE);
        bool duplicate_user = (s_ctx.user[i] == user);

        if (s_ctx.ref_cnt[i] == 0) {
            PERIPH_RCC_ATOMIC() {
                mipi_csi_ll_enable_brg_module_clock(i, true);
                mipi_csi_ll_reset_brg_module_clock(i);
                mipi_csi_brg_ll_enable_clock(MIPI_CSI_BRG_LL_GET_HW(i), true);
            }
            s_ctx.user[i] = user;
        } else {
            if (!user_is_shared && !to_share && !duplicate_user) {
                found = false;
            }

            if (user_is_shared) {
                s_ctx.user[i] = user;
            }
        }

        if (found) {
            s_ctx.ref_cnt[i]++;
            *out_id = i;
            break;
        }
    }
    esp_os_exit_critical(&s_ctx.spinlock);

    if (!found) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t mipi_csi_brg_declaim(int id)
{
    esp_os_enter_critical(&s_ctx.spinlock);

    s_ctx.ref_cnt[id]--;
    if (s_ctx.ref_cnt[id] < 0) {
        s_ctx.ref_cnt[id] = 0;
        esp_os_exit_critical(&s_ctx.spinlock);
        ESP_LOGE(TAG, "%s called, but s_ctx.ref_cnt[%d] == 0", __func__, id);
        return ESP_ERR_INVALID_STATE;
    } else if (s_ctx.ref_cnt[id] == 0) {
        PERIPH_RCC_ATOMIC() {
            mipi_csi_ll_enable_brg_module_clock(id, false);
            mipi_csi_brg_ll_enable_clock(MIPI_CSI_BRG_LL_GET_HW(id), false);
        }
        s_ctx.user[id] = MIPI_CSI_BRG_USER_NO_USER;
    }
    esp_os_exit_critical(&s_ctx.spinlock);

    return ESP_OK;
}
