/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#if CONFIG_SOC_WIFI_HE_SUPPORT

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "esp_wifi_he_types.h"
#include "esp_wifi_he_types_private.h"

#ifdef __cplusplus
extern "C" {
#endif

float esp_test_get_bfr_avgsnr(void);

void esp_test_enable_edca_tx(esp_wifi_aci_t aci);
void esp_test_disable_edca_tx(esp_wifi_aci_t aci);

esp_err_t esp_wifi_enable_htc_uph(bool enable);
esp_err_t esp_wifi_set_htc_omc(const esp_wifi_htc_omc_t *om);
void esp_wifi_enable_rx_stbc(bool enable);
void esp_wifi_enable_su_bmfmee(bool enable);
esp_err_t esp_wifi_set_tf_padding_duration(int tf_padding_duration);

void hal_he_set_ul_mu(bool ul_mu_disable, bool ul_mu_data_disable);
void hal_he_set_bf_report_rate(sig_mode_t sig_mode, wifi_phy_rate_t rate, bool ersu, bool dcm);

void dbg_read_muedca_timer(uint8_t aci);
void dbg_read_axtb_diag(void);
void dbg_read_ax_diag(bool verbose);
void dbg_read_tx_mplen(const void*, uint8_t ac);
void dbg_tb_ignore_cca_enable(bool enable);

esp_err_t esp_wifi_sta_report_bsscolor_collision(void);
esp_err_t esp_wifi_sta_report_bsscolor_inuse(void);

/* RX */
esp_err_t esp_test_clr_rx_error_occurs(void);
esp_err_t esp_test_get_rx_error_occurs(esp_test_rx_error_occurs_t *err_occurs);

/* HW */
void esp_test_clr_hw_statistics(void);
esp_err_t esp_test_get_hw_rx_statistics(esp_test_hw_rx_statistics_t *hw_rx_stats);
esp_err_t esp_test_get_hw_tb_statistics(esp_test_hw_tb_statistics_t *hw_tb_stats);

/**
  * @brief     Get tx stats enabled ACI bitmap
  *
  * @return
  *    - acibitmap
  */
uint8_t esp_wifi_get_tx_statistics_ena_acibitmap(void);

/**
  * @brief     Clear DL MU-MIMO and DL OFDMA reception statistics.
  *
  * @return
  *    - ESP_OK
  *    - ESP_FAIL
  */
esp_err_t esp_wifi_clr_rx_mu_statistics(void);

/**
  * @brief     Get the DL MU-MIMO and DL OFDMA reception statistics.
  *
  * @param[in]    mu_stats the DL MU-MIMO and DL OFDMA reception statistics
  *
  * @return
  *    - ESP_OK
  *    - ESP_FAIL
  */
esp_err_t esp_wifi_get_rx_mu_statistics(esp_test_rx_mu_statistics_t *mu_stats);

/**
  * @brief     Clear the reception statistics excluding DL MU-MIMO and DL OFDMA.
  *
  * @param[in]    tid the traffic id, accept tid = 0, tid = 7 and tid = 8. tid = 8 will clear reception statistics for both tid = 0 and tid = 7
  *
  * @return
  *    - ESP_OK
  *    - ESP_FAIL
  */
esp_err_t esp_wifi_clr_rx_statistics(uint8_t tid);

/**
  * @brief     Get the reception statistics excluding DL MU-MIMO and DL OFDMA.
  *
  * @param[in]    tid the traffic id, only accept tid = 0 or tid = 7
  * @param[in]    rx_stats the reception statistics
  *
  * @return
  *    - ESP_OK
  *    - ESP_FAIL
  */
esp_err_t esp_wifi_get_rx_statistics(uint8_t tid, esp_test_rx_statistics_t *rx_stats);

/**
  * @brief     Clear the transmission statistics.
  *
  * @param[in]    aci access category id.
  *                   Generally, for data frames, aci = ESP_WIFI_ACI_BE; for management frames, aci = ESP_WIFI_ACI_VO.
  *
  * @return
  *    - ESP_OK
  *    - ESP_FAIL
  */
esp_err_t esp_wifi_clr_tx_statistics(esp_wifi_aci_t aci);

/**
  * @brief     Get the transmission statistics.
  *
  * @param[in]    aci access category id.
  * @param[in]    tx_stats the transmission statistics
  * @param[in]    tx_fail the common failure state and reason
  *
  * @return
  *    - ESP_OK
  *    - ESP_FAIL
  */
esp_err_t esp_wifi_get_tx_statistics(esp_wifi_aci_t aci, esp_test_tx_statistics_t *tx_stats, esp_test_tx_fail_statistics_t *tx_fail);

/**
  * @brief     Clear the TB PPDU transmission statistics.
  *
  * @param[in]    aci access category id.
  *                   Generally, for data frames, aci = ESP_WIFI_ACI_BE; for management frames, aci = ESP_WIFI_ACI_VO.
  *
  * @return
  *    - ESP_OK
  *    - ESP_FAIL
  */
esp_err_t esp_wifi_clr_tx_tb_statistics(esp_wifi_aci_t aci);

/**
  * @brief     Get the TB PPDU transmission statistics.
  *
  * @param[in]    aci access category id.
  * @param[in]    tb_stats TB PPDU statistics.
  *
  * @return
  *    - ESP_OK
  *    - ESP_FAIL
  */
esp_err_t esp_wifi_get_tx_tb_statistics(esp_wifi_aci_t aci, esp_test_tx_tb_statistics_t *tb_stats);

/**
* @brief Add BSS color change announcement IE
*
* @attention This API should be called after esp_wifi_start().
*
* @param color new bss color, 0 means disable.
*
* @return
*  - ESP_OK: succeed
*  - others: failed
*/
esp_err_t esp_wifi_softap_add_color_change_announcement(uint8_t color);

/**
* @brief Add bss max idle ie
*
* @attention This API should be called after esp_wifi_start().
*
* @param[in] bss_max_idle_enable enable bss max idle
* @param[in] bss_max_idle_period_secs bss max idle period, unit seconds
* @param[in] protected_keep_alive using protected/unprotected frame to keep alive
*
* @return
*  - ESP_OK: succeed
*  - others: failed
*/
esp_err_t esp_wifi_softap_set_bss_max_idle(bool bss_max_idle_enable, uint16_t bss_max_idle_period_secs, bool protected_keep_alive);

/**
* @brief Reset MU EDCA Timer
*
* @attention This API should be called after esp_wifi_start().
*
* @param aci_bitmap bit0: BK
*                   bit1: BE
*                   bit2: VI
*                   bit3: VO
*
* @return
*  - ESP_OK: succeed
*  - others: failed
*/
esp_err_t esp_wifi_sta_reset_muedca_timer(uint8_t aci_bitmap);

/**
  * @brief     Set bss color collision detection duration and frame threshold.
  *
  * @param[in] threshold the number of HE frames with the same BSS color as STA but in different BSSs.
  * @param[in] duration duration of the detection. If the number of frames that STA detects reaches threshold,
  *            STA will report BSS Color Collision to the associated AP. Unit seconds.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_sta_set_bss_color_collision_detection(int threshold, int duration);

esp_err_t esp_test_clr_rx_ctrls(void);
esp_err_t esp_test_get_rx_ctrls(esp_test_rx_ctrl_t* rx);

void hal_set_tx_pwr(wifi_phy_rate_t rate,  int8_t max_pwr);
int8_t hal_get_tx_pwr(wifi_phy_rate_t rate);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_SOC_WIFI_HE_SUPPORT
