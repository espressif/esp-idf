/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"

#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS || CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS

#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_private/esp_wifi_he_private.h"

/*******************************************************
 *                Macros
 *******************************************************/
/*
 * enable/disable rx/tx statistics after Wi-Fi started:
 * (1) esp_wifi_enable_rx_statistics(true, true);            //rx_stats=true, rx_mu_stats=true
 * (2) esp_wifi_enable_tx_statistics(ESP_WIFI_ACI_BE, true); //aci=ESP_WIFI_ACI_BE, tx_stats=true
 */

/*******************************************************
 *                Constants
 *******************************************************/
static const char *TAG = "stats";

/*******************************************************
 *                Structures
 *******************************************************/

/*******************************************************
 *                Variable Definitions
 *******************************************************/
#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_MU_STATS
esp_test_rx_mu_statistics_t rx_mu_stats = { 0, }; //10932 bytes
#endif

/*******************************************************
 *                Function Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/
const char *tx_fail_error2str(esp_test_tx_fail_error_t error)
{
    switch (error) {
    case TEST_TX_FAIL_ERROR_H00:
        return "0x00";
    case TEST_TX_FAIL_ERROR_H53:
        return "0x53";
    case TEST_TX_FAIL_ERROR_H63:
        return "0x63";
    case TEST_TX_FAIL_ERROR_H75:
        return "0x75";
    case TEST_TX_FAIL_ERROR_H41:
        return "0x41";
    case TEST_TX_FAIL_ERROR_H42:
        return "0x42";
    case TEST_TX_FAIL_ERROR_H47:
        return "0x47";
    case TEST_TX_FAIL_ERROR_H80:
        return "0x80";
    case TEST_TX_FAIL_ERROR_H5A:
        return "0x5A";
    case TEST_TX_FAIL_ERROR_HXX:
        return "Others";
    case TEST_TX_FAIL_ERROR_MAX:
        return "Undefined";
    }
    return "Undefined";
}

const char *tx_fail_match2str(esp_test_tx_fail_match_t match)
{
    switch (match) {
    case TEST_TX_WAIT_MATCH:
        return "MATCH";
    case TEST_TX_WAIT_NOT2SELF:
        return "NOT2SELF";
    case TEST_TX_MISMATCH:
        return "MISMATCH";
    case TEST_TX_WAIT_TIMEOUT:
        return "TIMEOUT";
    case TEST_TX_WAIT_MAX:
        return "Undefined";
    }
    return "Undefined";
}

const char *tx_fail_state2str(esp_test_tx_fail_state_t state)
{
    switch (state) {
    case TEST_TX_SUCCESS:
        return "TX Success";
    case TEST_TX_FAIL_RTS:
        return "TX RTS";
    case TEST_TX_WAIT_CTS:    //RX
        return "Wait CTS";
    case TEST_TX_FAIL_CTS:
        return "TX RTS";
    case TEST_TX_FAIL_DATA:
        return "TX DATA";
    case TEST_TX_WAIT_ACK:    //RX
        return "Wait ACK/BA";
    case TEST_TX_FAIL_MAX:
        return "Undefined";
    }
    return "Undefined";
}

int wifi_cmd_clr_tx_statistics(int argc, char **argv)
{
    ESP_LOGW(TAG, "Clear tx statistics");
    int i;
    for (i = 0; i < 3; i++) {
        esp_wifi_clr_tx_statistics(i); //BE
        esp_wifi_clr_tx_tb_statistics(i);
    }
    esp_test_clr_hw_statistics();
    return 0;
}

void print_hw_tb_statistics(void)
{
    esp_test_hw_tb_statistics_t hw_tb_stats = { 0, };
    esp_test_get_hw_tb_statistics(&hw_tb_stats);
    printf("(test)rx_trig:%d, tx_bfrpt:%d, tb_times:%d, tb_qos_null:%d, tb_qos_data:%d, tb_cca_cancel:%d, tb_sifs_abort:%d, tb_pwr_outof_range:%d\n",
           hw_tb_stats.rx_trig,
           hw_tb_stats.tx_bfrpt, //including TB and Non-TB
           hw_tb_stats.tb_times,
           hw_tb_stats.tb_qos_null,
           hw_tb_stats.tb_times - hw_tb_stats.tb_qos_null,
           hw_tb_stats.tb_cca_cancel,
           hw_tb_stats.tb_sifs_abort,
           hw_tb_stats.tb_pwr_outof_range);
}

int wifi_cmd_get_tx_statistics(int argc, char **argv)
{
    uint8_t i, h, j, k;

    ESP_LOGW(TAG, "Get tx statistics");
    esp_test_tx_tb_statistics_t tb_stats = { 0, };               //32 bytes
    esp_test_tx_statistics_t tx_stats = { 0, };                  //136 bytes
    esp_test_tx_fail_statistics_t tx_fail[TEST_TX_FAIL_MAX] = { 0, }; //TEST_TX_FAIL_MAX * 164 bytes

    print_hw_tb_statistics();
    //only check BE
    for (i = 2; i < 3; i++) {
        esp_wifi_get_tx_tb_statistics(i, &tb_stats);
        /* TB */
        printf("(test)aci:%" PRIu8 ", tb(suc:%" PRIu32 ", ack:%" PRIu32 ", err:%" PRIu32 "), "
               "count(suc:%" PRIu32 ", ack:%" PRIu32 ", err:%" PRIu32 ", tot:%" PRIu32 ", max_sent:%" PRIu32 ")\n",
               i,
               tb_stats.complete_suc_tb,
               tb_stats.complete_ack_tb,
               tb_stats.complete_err_tb,
               tb_stats.complete_tb_suc_count,
               tb_stats.complete_tb_ack_count,
               tb_stats.complete_tb_err_count,
               tb_stats.complete_tb_tot_count,
               tb_stats.complete_tb_pack_sent);

        esp_wifi_get_tx_statistics(i, &tx_stats, (esp_test_tx_fail_statistics_t *) &tx_fail);
        int tot_tx_times = tx_stats.tb_times + (tx_stats.tx_enable - tx_stats.tb_last); //TB + EDCA
        int tot_fail = tx_fail[1].count + tx_fail[2].count + tx_fail[3].count + tx_fail[4].count + tx_fail[5].count;
        printf("(test)aci:%" PRIu8 ", enable:%" PRIu32 ", complete:%" PRIu32 ", tb_times:%" PRIu32 ", tb_last:%" PRIu32 ", edca:%" PRIu32 ", "
               "succ:%" PRIu32 ", fail(%" PRIu32 ",%" PRIu32 ",%" PRIu32 ", cts:%" PRIu32 "/%2.2f%%, ack:%" PRIu32 "/%2.2f%%, tot:%d, %.2f%%), "
               "edca(ack:%" PRIu32 ", ba:%" PRIu32 "), tb(hw-ba:%" PRIu32 ", sw-ba:%" PRIu32 ")\n",
               i, tx_stats.tx_enable,
               tx_stats.tx_complete,
               tx_stats.tb_times,
               tx_stats.tb_last,
               tx_stats.tx_enable - tx_stats.tb_last,
               tx_fail[0].count,
               tx_fail[1].count,
               tx_fail[3].count,
               tx_fail[4].count,
               tx_fail[2].count,
               (float) ((float) tx_fail[2].count / (float) tot_tx_times) * 100, //rx cts
               tx_fail[5].count, (float) ((float) tx_fail[5].count / (float) tot_tx_times) * 100, //rx ack
               tot_fail,
               (float) ((float) tot_fail / (float) tot_tx_times) * 100,
               tx_stats.rx_ack,
               tx_stats.rx_ba,
               tx_stats.tb_rx_ba, //including ACKs
               tx_stats.rx_dump_ba);

        printf("(test)aci:%" PRIu8 ", txFrames:%" PRIu32 ", s-mpdu:%" PRIu32 "(%.2f%%), "
               "bitmap(max:%d, min:%d, tot:%" PRIu32 ", avg:%.2f), "
               "retry(edca:%" PRIu32 ", tb:%" PRIu32 ", %.2f%%), collision:%" PRIu32 ", timeout:%" PRIu32 "\n",
               i,
               tx_stats.tx_succ,
               tx_stats.rx_ack,
               ((float) (tx_stats.rx_ack) / (float) tot_tx_times) * 100,
               tx_stats.rx_max_bitmap,
               tx_stats.rx_min_bitmap,
               tx_stats.rx_tot_bitmap,
               (float) tx_stats.rx_tot_bitmap / (float) (tx_stats.tb_rx_ba + tx_stats.rx_ba),
               tx_stats.retry_edca, tx_stats.retry_tb, (float) (tx_stats.retry_edca + tx_stats.retry_tb) / (float) tx_stats.tx_succ * 100,
               tx_stats.collision, tx_stats.timeout);

        float tot_rtt_ms = (float) tx_stats.tx_tot_rtt / (float) 1000;
        printf("(test)aci:%" PRIu8 ", seqno_rtt[%" PRIu32 ",%" PRIu32 "], hw_rtt[%" PRIu32 ", %" PRIu32 "], muedca[enable:%" PRIu32 ", times:%" PRIu32 ", %.2f, %.2f, tot:%.2f], avg:%.3f ms, tot:%.3f secs\n",
               i,
               tx_stats.tx_seq_min_rtt,
               tx_stats.tx_seq_max_rtt,
               tx_stats.tx_min_rtt,
               tx_stats.tx_max_rtt,
               tx_stats.tx_muedca_enable,
               tx_stats.muedca_times,
               (float) tx_stats.tx_min_muedca_time / (float) 1000,
               (float) tx_stats.tx_max_muedca_time / (float) 1000,
               (float) tx_stats.tx_tot_muedca_time / (float) 1000, //ms
               (float) tot_rtt_ms / (float) tot_tx_times, //ms
               (float) tot_rtt_ms / (float) 1000); //seconds
        /* fail state */
        for (h = 1; h < TEST_TX_FAIL_MAX; h++) { //state
            for (j = 0; j < TEST_TX_WAIT_MAX; j++) { //match
                for (k = 0; k < TEST_TX_FAIL_ERROR_MAX; k++) { //error
                    if (tx_fail[h].match[j][k]) {
                        printf("(test)[%d][%d][%d](%16s + %16s + %16s)%3" PRIu32 "/%3" PRIu32 "(%.2f%%)\n", h, j, k, tx_fail_state2str(h),
                               tx_fail_match2str(j), tx_fail_error2str(k),
                               tx_fail[h].match[j][k], tx_fail[h].count,
                               ((float) tx_fail[h].match[j][k] / (float) tx_fail[h].count) * 100);
                    }
                }
            }
        }
        printf("\n");
    }
    wifi_cmd_clr_tx_statistics(0, 0);
    return 0;
}

void print_rx_statistics_nonmimo(const esp_test_rx_mu_statistics_t *mu_stats)
{
    if (!mu_stats->nonmimo_rx) {
        return;
    }

    int i, j;
    int tot_rx_nonmimo = 0;
    ESP_LOGW(TAG, "(nonmimo)dut rx:%" PRIu32, mu_stats->nonmimo_rx);
    ESP_LOGW(TAG, "(nonmimo)ru_alloc_96_num_2046:%" PRIu32 ", ru_alloc_112_num_2046:%" PRIu32, mu_stats->ru_alloc_96_num_2046, mu_stats->ru_alloc_112_num_2046);
    ESP_LOGW(TAG, "(nonmimo)sigb, mcs0:%" PRIu32 "(%2.2f%%), mcs1:%" PRIu32 "(%2.2f%%), mcs2:%" PRIu32 "(%2.2f%%), mcs3:%" PRIu32 "(%2.2f%%), mcs4:%" PRIu32 "(%2.2f%%), mcs5:%" PRIu32 "(%2.2f%%)",
             mu_stats->nonmimo_sigb_mcs[0], ((float) mu_stats->nonmimo_sigb_mcs[0] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_sigb_mcs[1], ((float) mu_stats->nonmimo_sigb_mcs[1] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_sigb_mcs[2], ((float) mu_stats->nonmimo_sigb_mcs[2] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_sigb_mcs[3], ((float) mu_stats->nonmimo_sigb_mcs[3] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_sigb_mcs[4], ((float) mu_stats->nonmimo_sigb_mcs[4] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_sigb_mcs[5], ((float) mu_stats->nonmimo_sigb_mcs[5] / (float) mu_stats->nonmimo_rx) * 100);
    ESP_LOGW(TAG, "(nonmimo)users, num1:%" PRIu32 "(%2.2f%%), num2:%" PRIu32 "(%2.2f%%), num3:%" PRIu32 "(%2.2f%%), num4:%" PRIu32 "(%2.2f%%), num5:%" PRIu32 "(%2.2f%%), num6:%" PRIu32 "(%2.2f%%), num7:%" PRIu32 "(%2.2f%%), num8:%" PRIu32 "(%2.2f%%), num9:%" PRIu32 "(%2.2f%%)",
             mu_stats->nonmimo_user_num_occu[0], ((float) mu_stats->nonmimo_user_num_occu[0] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_user_num_occu[1], ((float) mu_stats->nonmimo_user_num_occu[1] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_user_num_occu[2], ((float) mu_stats->nonmimo_user_num_occu[2] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_user_num_occu[3], ((float) mu_stats->nonmimo_user_num_occu[3] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_user_num_occu[4], ((float) mu_stats->nonmimo_user_num_occu[4] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_user_num_occu[5], ((float) mu_stats->nonmimo_user_num_occu[5] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_user_num_occu[6], ((float) mu_stats->nonmimo_user_num_occu[6] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_user_num_occu[7], ((float) mu_stats->nonmimo_user_num_occu[7] / (float) mu_stats->nonmimo_rx) * 100,
             mu_stats->nonmimo_user_num_occu[8], ((float) mu_stats->nonmimo_user_num_occu[8] / (float) mu_stats->nonmimo_rx) * 100);
    for (i = 0; i < 256; i++) {
        for (j = 0; j < 9; j++) {
            if (!mu_stats->nonmimo_ru_alloc[i][j]) {
                continue;
            }
            ESP_LOGI(TAG, "(nonmimo)ru_allocation:0x%2x(%3" PRIu8 "), position:%" PRIu8 ", %5" PRIu32 "(%2.2f%%)", i, i, j + 1, mu_stats->nonmimo_ru_alloc[i][j],
                     ((float) mu_stats->nonmimo_ru_alloc[i][j] / (float) mu_stats->nonmimo_rx) * 100);
        }
    }
    for (i = 0; i < ESP_TEST_RX_MU_USER_NUM; i++) {
        if (!mu_stats->nonmimo[i].aid) {
            continue;
        }
        if (mu_stats->aid != mu_stats->nonmimo[i].aid) {
            continue;
        }
        tot_rx_nonmimo = mu_stats->nonmimo[i].occu_nsts[0] + mu_stats->nonmimo[i].occu_nsts[1] + mu_stats->nonmimo[i].occu_nsts[2] + mu_stats->nonmimo[i].occu_nsts[3];
        printf("[%" PRIu8 "]%said:0x%x, txbf:%" PRIu32 ", dcm:%" PRIu32 "\n", i, (mu_stats->aid == mu_stats->nonmimo[i].aid) ? "#" : " ", mu_stats->nonmimo[i].aid,
               mu_stats->nonmimo[i].txbf, mu_stats->nonmimo[i].dcm);
        printf("[%d]%said:0x%x, "
               "mcs0:%" PRIu32 "(%2.2f%%), mcs1:%" PRIu32 "(%2.2f%%), mcs2:%" PRIu32 "(%2.2f%%), mcs3:%" PRIu32 "(%2.2f%%), mcs4:%" PRIu32 "(%2.2f%%), "
               "mcs5:%" PRIu32 "(%2.2f%%), mcs6:%" PRIu32 "(%2.2f%%), mcs7:%" PRIu32 "(%2.2f%%), mcs8:%" PRIu32 "(%2.2f%%), mcs9:%" PRIu32 "(%2.2f%%), "
               "mcs10:%" PRIu32 "(%2.2f%%), mcs11:%" PRIu32 "(%2.2f%%)\n",
               i, (mu_stats->aid == mu_stats->nonmimo[i].aid) ? "#" : " ", mu_stats->nonmimo[i].aid,
               mu_stats->nonmimo[i].occu_mcs[0], ((float) mu_stats->nonmimo[i].occu_mcs[0] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[1], ((float) mu_stats->nonmimo[i].occu_mcs[1] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[2], ((float) mu_stats->nonmimo[i].occu_mcs[2] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[3], ((float) mu_stats->nonmimo[i].occu_mcs[3] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[4], ((float) mu_stats->nonmimo[i].occu_mcs[4] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[5], ((float) mu_stats->nonmimo[i].occu_mcs[5] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[6], ((float) mu_stats->nonmimo[i].occu_mcs[6] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[7], ((float) mu_stats->nonmimo[i].occu_mcs[7] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[8], ((float) mu_stats->nonmimo[i].occu_mcs[8] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[9], ((float) mu_stats->nonmimo[i].occu_mcs[9] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[10], ((float) mu_stats->nonmimo[i].occu_mcs[10] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_mcs[11], ((float) mu_stats->nonmimo[i].occu_mcs[11] / (float) tot_rx_nonmimo) * 100);
        printf("[%" PRIu8 "]%said:0x%x, "
               "nsts0:%" PRIu32 "(%2.2f%%), nsts1:%" PRIu32 "(%2.2f%%), nsts2:%" PRIu32 "(%2.2f%%), nsts3:%" PRIu32 "(%2.2f%%)\n",
               i, (mu_stats->aid == mu_stats->nonmimo[i].aid) ? "#" : " ", mu_stats->nonmimo[i].aid,
               mu_stats->nonmimo[i].occu_nsts[0], ((float) mu_stats->nonmimo[i].occu_nsts[0] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_nsts[1], ((float) mu_stats->nonmimo[i].occu_nsts[1] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_nsts[2], ((float) mu_stats->nonmimo[i].occu_nsts[2] / (float) tot_rx_nonmimo) * 100,
               mu_stats->nonmimo[i].occu_nsts[3], ((float) mu_stats->nonmimo[i].occu_nsts[3] / (float) tot_rx_nonmimo) * 100);
        printf("[%" PRIu8 "]%said:0x%x, "
               "tot_rx_nonmimo:%8d, sta/dut:%2.2f%%\n",
               i, (mu_stats->aid == mu_stats->nonmimo[i].aid) ? "#" : " ", mu_stats->nonmimo[i].aid,
               tot_rx_nonmimo, ((float) tot_rx_nonmimo / (float) mu_stats->nonmimo_rx) * 100);
    }
}

void print_rx_statistics_mimo(const esp_test_rx_mu_statistics_t *mu_stats)
{
    if (!mu_stats->mimo_rx) {
        return;
    }

    int i;
    int tot_rx_mimo = 0;
    ESP_LOGW(TAG, "(mimo)dut rx:%" PRIu32 "", mu_stats->mimo_rx);
    ESP_LOGW(TAG, "(mimo)sigb, mcs0:%" PRIu32 "(%2.2f%%), mcs1:%" PRIu32 "(%2.2f%%), mcs2:%" PRIu32 "(%2.2f%%), mcs3:%" PRIu32 "(%2.2f%%), mcs4:%" PRIu32 "(%2.2f%%), mcs5:%" PRIu32 "(%2.2f%%)",
             mu_stats->mimo_sigb_mcs[0], ((float) mu_stats->mimo_sigb_mcs[0] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_sigb_mcs[1], ((float) mu_stats->mimo_sigb_mcs[1] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_sigb_mcs[2], ((float) mu_stats->mimo_sigb_mcs[2] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_sigb_mcs[3], ((float) mu_stats->mimo_sigb_mcs[3] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_sigb_mcs[4], ((float) mu_stats->mimo_sigb_mcs[4] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_sigb_mcs[5], ((float) mu_stats->mimo_sigb_mcs[5] / (float) mu_stats->mimo_rx) * 100);
    ESP_LOGW(TAG, "(mimo)users num2:%" PRIu32 "(%2.2f%%), num3:%" PRIu32 "(%2.2f%%), num4:%" PRIu32 "(%2.2f%%), num5:%" PRIu32 "(%2.2f%%), num6:%" PRIu32 "(%2.2f%%), num7:%" PRIu32 "(%2.2f%%), num8:%" PRIu32 "(%2.2f%%)",
             mu_stats->mimo_user_num_occu[0], ((float) mu_stats->mimo_user_num_occu[0] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_user_num_occu[1], ((float) mu_stats->mimo_user_num_occu[1] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_user_num_occu[2], ((float) mu_stats->mimo_user_num_occu[2] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_user_num_occu[3], ((float) mu_stats->mimo_user_num_occu[3] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_user_num_occu[4], ((float) mu_stats->mimo_user_num_occu[4] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_user_num_occu[5], ((float) mu_stats->mimo_user_num_occu[5] / (float) mu_stats->mimo_rx) * 100,
             mu_stats->mimo_user_num_occu[6], ((float) mu_stats->mimo_user_num_occu[6] / (float) mu_stats->mimo_rx) * 100);
    for (i = 0; i < ESP_TEST_RX_MU_USER_NUM; i++) {
        if (!mu_stats->mimo[i].aid) {
            continue;
        }
        tot_rx_mimo = mu_stats->mimo[i].occu_ss[0] + mu_stats->mimo[i].occu_ss[1] + mu_stats->mimo[i].occu_ss[2] + mu_stats->mimo[i].occu_ss[3];
        printf("[%" PRIu8 "]%said:0x%x, "
               "mcs0:%" PRIu32 "(%2.2f%%), mcs1:%" PRIu32 "(%2.2f%%), mcs2:%" PRIu32 "(%2.2f%%), mcs3:%" PRIu32 "(%2.2f%%), mcs4:%" PRIu32 "(%2.2f%%), "
               "mcs5:%" PRIu32 "(%2.2f%%), mcs6:%" PRIu32 "(%2.2f%%), mcs7:%" PRIu32 "(%2.2f%%), mcs8:%" PRIu32 "(%2.2f%%), mcs9:%" PRIu32 "(%2.2f%%), "
               "mcs10:%" PRIu32 "(%2.2f%%), mcs11:%" PRIu32 "(%2.2f%%)\n",
               i, (mu_stats->aid == mu_stats->mimo[i].aid) ? "#" : " ", mu_stats->mimo[i].aid,
               mu_stats->mimo[i].occu_mcs[0], ((float) mu_stats->mimo[i].occu_mcs[0] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[1], ((float) mu_stats->mimo[i].occu_mcs[1] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[2], ((float) mu_stats->mimo[i].occu_mcs[2] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[3], ((float) mu_stats->mimo[i].occu_mcs[3] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[4], ((float) mu_stats->mimo[i].occu_mcs[4] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[5], ((float) mu_stats->mimo[i].occu_mcs[5] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[6], ((float) mu_stats->mimo[i].occu_mcs[6] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[7], ((float) mu_stats->mimo[i].occu_mcs[7] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[8], ((float) mu_stats->mimo[i].occu_mcs[8] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[9], ((float) mu_stats->mimo[i].occu_mcs[9] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[10], ((float) mu_stats->mimo[i].occu_mcs[10] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_mcs[11], ((float) mu_stats->mimo[i].occu_mcs[11] / (float) tot_rx_mimo) * 100);
        printf("[%" PRIu8 "]%said:0x%x, "
               "ss0:%" PRIu32 "(%2.2f%%), ss1:%" PRIu32 "(%2.2f%%), ss2:%" PRIu32 "(%2.2f%%), ss3:%" PRIu32 "(%2.2f%%)\n",
               i, (mu_stats->aid == mu_stats->mimo[i].aid) ? "#" : " ", mu_stats->mimo[i].aid,
               mu_stats->mimo[i].occu_ss[0], ((float) mu_stats->mimo[i].occu_ss[0] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_ss[1], ((float) mu_stats->mimo[i].occu_ss[1] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_ss[2], ((float) mu_stats->mimo[i].occu_ss[2] / (float) tot_rx_mimo) * 100,
               mu_stats->mimo[i].occu_ss[3], ((float) mu_stats->mimo[i].occu_ss[3] / (float) tot_rx_mimo) * 100);
        printf("[%" PRIu8 "]%said:0x%x, "
               "tot_rx_mimo:%8d, sta/dut:%2.2f%%\n",
               i, (mu_stats->aid == mu_stats->mimo[i].aid) ? "#" : " ", mu_stats->mimo[i].aid,
               tot_rx_mimo, ((float) tot_rx_mimo / (float) mu_stats->mimo_rx) * 100);
    }
}

void print_hw_rx_statistics(void)
{
    esp_test_hw_rx_statistics_t hw_rx_stats = { 0, };
    esp_test_get_hw_rx_statistics(&hw_rx_stats);
    printf(
        "WDEVRX_FCS_ERR          :%d\n"
        "WDEVRX_ABORT            :%d\n"
        "WDEVRX_ABORT_FCS_PASS   :%d\n"
        "NRX_ERR_PWRDROP         :%d\n"
        "NRX_HESIGB_ERR          :%d\n"
        "WDEVRX_SAMEBM_ERRCNT    :%d\n"
        "WDEVRX_MPDU             :%d\n"
        "WDEVRX_END_CNT          :%d\n"
        "WDEVRX_DATASUC          :%d\n"
        "WDEVRX_LASTUNMATCH_ERR  :%d\n"
        "RXHUNG_STATIS           :%d\n"
        "TXHUNG_STATIS           :%d\n"
        "RXTXHUNG                :%" PRIu32 "\n"
        "WDEVRX_CFO              :%d\n"
        "WDEVRX_SF               :%d\n"
        "WDEVRX_OTHER_UCAST      :%d\n"
        "WDEVRX_BUF_FULLCNT      :%d\n"
        "WDEVRX_FIFO_OVFCNT      :%d\n"
        "WDEVRX_TKIP_ERRCNT      :%d\n"
        "WDEVRX_BTBLOCK_ERR      :%d\n"
        "WDEVRX_FREQHOP_ERR      :%d\n"
        "WDEVRX_ACK_INT_CNT      :%d\n"
        "WDEVRX_RTS_INT_CNT      :%d\n"
        "BRX_ERR_AGC             :%d\n"
        "BRX_ERR                 :%d\n"
        "NRX_ERR                 :%d\n"
        "NRX_ERR_ABORT           :%d\n"
        "NRX_ERR_AGCEXIT         :%d\n"
        "NRX_ERR_BBOFF           :%d\n"
        "NRX_ERR_FDM_WDG         :%d\n"
        "NRX_ERR_RESTART         :%d\n"
        "NRX_ERR_SERV            :%d\n"
        "NRX_ERR_TXOVER          :%d\n"
        "NRX_HE_UNSUPPORT        :%d\n"
        "NRX_HTSIG_ERR           :%d\n"
        "NRX_HEUNSUPPORT         :%d\n"
        "NRX_HESIGA_CRC          :%d\n",
        hw_rx_stats.rx_fcs_err,
        hw_rx_stats.rx_abort,
        hw_rx_stats.rx_abort_fcs_pass,
        hw_rx_stats.nrx_err_pwrdrop,
        hw_rx_stats.nrx_hesigb_err,
        hw_rx_stats.rx_samebm_errcnt,
        hw_rx_stats.rx_mpdu,
        hw_rx_stats.rx_end_cnt,
        hw_rx_stats.rx_datasuc,
        hw_rx_stats.rx_lastunmatch_err,
        hw_rx_stats.rxhung_statis,
        hw_rx_stats.txhung_statis,
        hw_rx_stats.rxtxhung,
        hw_rx_stats.rx_cfo_hz,
        hw_rx_stats.rx_sf,
        hw_rx_stats.rx_other_ucast,
        hw_rx_stats.rx_buf_fullcnt,
        hw_rx_stats.rx_fifo_ovfcnt,
        hw_rx_stats.rx_tkip_errcnt,
        hw_rx_stats.rx_btblock_err,
        hw_rx_stats.rx_freqhop_err,
        hw_rx_stats.rx_ack_int_cnt,
        hw_rx_stats.rx_rts_int_cnt,
        hw_rx_stats.brx_err_agc,
        hw_rx_stats.brx_err,
        hw_rx_stats.nrx_err,
        hw_rx_stats.nrx_err_abort,
        hw_rx_stats.nrx_err_agcexit,
        hw_rx_stats.nrx_err_bboff,
        hw_rx_stats.nrx_err_fdm_wdg,
        hw_rx_stats.nrx_err_restart,
        hw_rx_stats.nrx_err_serv,
        hw_rx_stats.nrx_err_txover,
        hw_rx_stats.nrx_err_unsupport,
        hw_rx_stats.nrx_htsig_err,
        hw_rx_stats.nrx_heunsupport,
        hw_rx_stats.nrx_hesiga_crc
    );
}

int wifi_cmd_clr_rx_statistics(int argc, char **argv)
{
    ESP_LOGW(TAG, "Clear rx statistics");
    esp_wifi_clr_rx_statistics(0);
    esp_wifi_clr_rx_statistics(7);
#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_MU_STATS
    esp_test_clr_rx_error_occurs();
    esp_wifi_clr_rx_mu_statistics();
#endif
    esp_test_clr_hw_statistics();
    return 0;
}

void print_rx_mu_statistics(void)
{
    /* mu */
    esp_wifi_get_rx_mu_statistics(&rx_mu_stats);
    /* MIMO */
    print_rx_statistics_mimo(&rx_mu_stats);
    /* non-MIMO */
    print_rx_statistics_nonmimo(&rx_mu_stats);
}

int wifi_cmd_get_rx_statistics(int argc, char **argv)
{
    ESP_LOGW(TAG, "Get rx statistics");
    esp_test_rx_statistics_t rx_stats = { 0, };
    esp_test_rx_error_occurs_t rx_error_occurs = { 0, };

    esp_wifi_get_rx_statistics(0, &rx_stats); //tid=0
    print_hw_tb_statistics();

    ESP_LOGW(TAG, "(0)legacy:%" PRIu32 ", ht(ht:%" PRIu32 ", ht_retry:%" PRIu32 "/%2.2f%%, ht_noeb:%" PRIu32 "/%2.2f%%)",
             rx_stats.legacy,
             rx_stats.ht, rx_stats.ht_retry,
             rx_stats.ht_retry ? ((float) ((float) rx_stats.ht_retry / (float) rx_stats.ht) * 100) : 0,
             rx_stats.ht_noeb, rx_stats.ht_noeb ? ((float) ((float) rx_stats.ht_noeb / (float) rx_stats.ht) * 100) : 0);
    ESP_LOGW(TAG, "(0)su(su:%" PRIu32 ", su_txbf:%" PRIu32 ", su_stbc:%" PRIu32 ", su_retry:%" PRIu32 "/%2.2f%%, ersu:%" PRIu32 ", ersu_dcm:%" PRIu32 ", su_noeb:%" PRIu32 "/%2.2f%%)",
             rx_stats.su,
             rx_stats.su_txbf, rx_stats.su_stbc,
             rx_stats.su_retry,
             rx_stats.su_retry ? ((float) ((float) rx_stats.su_retry / (float) rx_stats.su) * 100) : 0,
             rx_stats.ersu,
             rx_stats.ersu_dcm,
             rx_stats.su_noeb, rx_stats.su_noeb ? ((float) ((float) rx_stats.su_noeb / (float) rx_stats.su) * 100) : 0);
    ESP_LOGW(TAG, "(0)mu(mu:%" PRIu32 ", mimo:%" PRIu32 ", non-mimo:%" PRIu32 ", txbf:%" PRIu32 ", stbc:%" PRIu32 ", mu_retry:%" PRIu32 "/%2.2f%%, mu_noeb:%" PRIu32 "/%2.2f%%)",
             rx_stats.mu,
             rx_stats.mu_mimo,
             rx_stats.mu_ofdma, rx_stats.mu_txbf, rx_stats.mu_stbc,
             rx_stats.mu_retry,
             rx_stats.mu_retry ? ((float) ((float) rx_stats.mu_retry / (float) rx_stats.mu) * 100) : 0,
             rx_stats.mu_noeb, rx_stats.mu_noeb ? ((float) ((float) rx_stats.mu_noeb / (float) rx_stats.mu) * 100) : 0);

    memset(&rx_stats, 0, sizeof(rx_stats));
    esp_wifi_get_rx_statistics(7, &rx_stats); //tid=7
    ESP_LOGW(TAG, "(7)legacy:%" PRIu32 ", ht:%" PRIu32 ", su:%" PRIu32 ", su_txbf:%" PRIu32 ", ersu:%" PRIu32 ", mu:%" PRIu32, rx_stats.legacy,
             rx_stats.ht, rx_stats.su, rx_stats.su_txbf, rx_stats.ersu, rx_stats.mu);
    ESP_LOGW(TAG, "(hw)isr:%" PRIu32 ", nblks:%" PRIu32, rx_stats.rx_isr, rx_stats.rx_nblks);
    /* hw rx statistics */
    print_hw_rx_statistics();
#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_MU_STATS
    print_rx_mu_statistics();
#endif
    esp_test_get_rx_error_occurs(&rx_error_occurs);
    ESP_LOGW(TAG, "(rx)tot_errors:%" PRIu32, rx_error_occurs.tot);
    int known_errors = 0; //rx error: 0x40-0xff
    int i;
    for (i = 0; i < 2; i++) {
        if (rx_error_occurs.occurs[i]) {
            known_errors += rx_error_occurs.occurs[i];
            printf("[%3d]  0x%x, %8" PRIu32 ", %2.2f%%\n", i, (i ? 0xf5 : 0xc6), rx_error_occurs.occurs[i],  ((float) rx_error_occurs.occurs[i] / (float) rx_error_occurs.tot) * 100);
        }
    }
    if (rx_error_occurs.tot - known_errors) {
        printf("[%3d]others, %8" PRIu32 ", %2.2f%%\n\n", i, rx_error_occurs.tot - known_errors,  ((float) known_errors / (float) rx_error_occurs.tot) * 100);
    }
    wifi_cmd_clr_rx_statistics(0, 0);
    return 0;
}

#endif /* CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS || CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS */

void register_wifi_stats(void)
{
#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS
    /* get tx statistics */
    const esp_console_cmd_t tx_stats_cmd = {
        .command = "tx",
        .help = "get tx statistics",
        .hint = NULL,
        .func = &wifi_cmd_get_tx_statistics,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&tx_stats_cmd));
    /* clear tx statistics */
    const esp_console_cmd_t clr_cmd = {
        .command = "clrtx",
        .help = "clear tx statistics",
        .hint = NULL,
        .func = &wifi_cmd_clr_tx_statistics,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&clr_cmd));
#endif

#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS
    /* get rx statistics */
    const esp_console_cmd_t rx_stats_cmd = {
        .command = "rx",
        .help = "get rx statistics",
        .hint = NULL,
        .func = &wifi_cmd_get_rx_statistics,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&rx_stats_cmd));
    /* clear rx statistics */
    const esp_console_cmd_t clr_rx_cmd = {
        .command = "clrrx",
        .help = "clear rx statistics",
        .hint = NULL,
        .func = &wifi_cmd_clr_rx_statistics,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&clr_rx_cmd));
#endif
}
