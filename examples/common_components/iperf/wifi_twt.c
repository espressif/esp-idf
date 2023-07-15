/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#if CONFIG_SOC_WIFI_HE_SUPPORT

#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_he.h"

/*******************************************************
 *                Constants
 *******************************************************/
static const char *TAG = "twt";

/*******************************************************
 *                Structures
 *******************************************************/
typedef struct {
    struct arg_int *setup;
    struct arg_int *teardown;
    struct arg_int *suspend;
    struct arg_int *trigger;      //1-trigger-enabled, 0-non-trigger-enabled, setup
    struct arg_int *flowtype;     //1-unannounced, 0-announced, setup
    struct arg_int *negtype;
    struct arg_int *wakeinvlexp;  //setup
    struct arg_int *wakeduraunit; //1-TU, 0-256us
    struct arg_int *wakeinvlman;  //setup
    struct arg_int *minwakedur;   //setup
    struct arg_int *flowid;
    struct arg_int *twtid;
    struct arg_int *setup_timeout_time_ms;
    struct arg_int *suspend_time_ms;
    struct arg_int *all_twt;
    struct arg_end *end;
} wifi_itwt_args_t;

typedef struct {
    struct arg_int *timeout;
    struct arg_end *end;
} wifi_itwt_send_probereq_t;

/*******************************************************
 *                Variable Definitions
 *******************************************************/
static wifi_itwt_args_t itwt_args;
static wifi_itwt_send_probereq_t itwt_probe_args;

/*******************************************************
 *                Function Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/
static int wifi_cmd_itwt(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &itwt_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, itwt_args.end, argv[0]);
        return 1;
    }

    esp_err_t err = ESP_OK;
    if (itwt_args.setup->count) {
        if (itwt_args.wakeinvlman->count) {
            if (itwt_args.wakeinvlman->ival[0] < 0 || itwt_args.wakeinvlman->ival[0] > 65535) {
                ESP_LOGE(TAG, "(itwt)expect [0, 65535], wake_invl_mant: %d", itwt_args.wakeinvlman->ival[0]);
                return 1;
            }
        }
        if (itwt_args.wakeinvlexp->count) {
            if (itwt_args.wakeinvlexp->ival[0] < 0 || itwt_args.wakeinvlexp->ival[0] > 31) {
                ESP_LOGE(TAG, "(itwt)expect [0, 31], wake_invl_expn: %d", itwt_args.wakeinvlexp->ival[0]);
                return 1;
            }
        }
        if (itwt_args.minwakedur->count) {
            if (itwt_args.minwakedur->ival[0] < 0 || itwt_args.minwakedur->ival[0] > 255) {
                ESP_LOGE(TAG, "(itwt)expect [0, 255], min_wake_dura: %d", itwt_args.minwakedur->ival[0]);
                return 1;
            }
        }
        if (itwt_args.wakeduraunit->count) {
            if (itwt_args.wakeduraunit->ival[0] < 0 || itwt_args.wakeduraunit->ival[0] > 1) {
                ESP_LOGE(TAG, "(itwt)expect [0, 1], wake duration unit: %d", itwt_args.wakeduraunit->ival[0]);
                return 1;
            }
        }
        if (itwt_args.twtid->count) {
            if (itwt_args.twtid->ival[0] < 0 || itwt_args.twtid->ival[0] > 32767) {
                ESP_LOGE(TAG, "(itwt)expect [0, 32767], twt id: %d", itwt_args.twtid->ival[0]);
                return 1;
            }
        }
        if (itwt_args.setup_timeout_time_ms->count) {
            if (itwt_args.setup_timeout_time_ms->ival[0] < 0 || itwt_args.setup_timeout_time_ms->ival[0] > 65535) {
                ESP_LOGE(TAG, "(itwt)expect [0, 65535], setup timeout time: %d", itwt_args.setup_timeout_time_ms->ival[0]);
                return 1;
            }
        }
        wifi_twt_setup_config_t setup_config = {
            .setup_cmd = (itwt_args.setup->ival[0] <= TWT_DEMAND) ? itwt_args.setup->ival[0] : TWT_REQUEST,
            .flow_id = 0,
            .twt_id = itwt_args.twtid->count ? itwt_args.twtid->ival[0] : 0,
            .flow_type = itwt_args.flowtype->count ? ((itwt_args.flowtype->ival[0] == 0) ? 0 : 1) : 0,
            .min_wake_dura = itwt_args.minwakedur->count ? itwt_args.minwakedur->ival[0] : 255,
            .wake_duration_unit = itwt_args.wakeduraunit->count ? itwt_args.wakeduraunit->ival[0] : 0,
            .wake_invl_expn = itwt_args.wakeinvlexp->count ? itwt_args.wakeinvlexp->ival[0] : 10,
            .wake_invl_mant = itwt_args.wakeinvlman->count ? itwt_args.wakeinvlman->ival[0] : 512,
            .trigger = itwt_args.trigger->count ? (itwt_args.trigger->ival[0] ? 1 : 0) : 1,
            .timeout_time_ms =  itwt_args.setup_timeout_time_ms->count ? itwt_args.setup_timeout_time_ms->ival[0] : 5000,
        };
        err = esp_wifi_sta_itwt_setup(&setup_config);
        ESP_LOGI(TAG, "(itwt)setup, trigger:%d, %s, flow_id:%d, err:0x%x",
                 setup_config.trigger, setup_config.flow_type ? "unannounce" : "announced", setup_config.flow_id, err);
    }
    if (itwt_args.teardown->count) {
        // teardown a given flow id, all_twt has a high priority
        int flow_id = itwt_args.flowid->count ?  itwt_args.flowid->ival[0] : (-1);
        bool all_twt = itwt_args.all_twt->count ? ((itwt_args.all_twt->ival[0] == 1) ? true : false) : false;
        flow_id = (all_twt == true) ? FLOW_ID_ALL : flow_id;
        if (flow_id >= 0) {
            err = esp_wifi_sta_itwt_teardown(flow_id);
            ESP_LOGI(TAG, "(itwt)teardown, flow_id:%d, all_twt:%d, err:0x%x", flow_id, all_twt, err);
        } else {
            ESP_LOGE(TAG, "(itwt)teardown, should specify an existing flow id");
        }
    }
    if (itwt_args.suspend->count) {
        // suspend a given flow id
        int flow_id = itwt_args.flowid->count ?  itwt_args.flowid->ival[0] : (-1);
        bool all_twt = itwt_args.all_twt->count ? (itwt_args.all_twt->ival[0] ? true : false) : false;
        flow_id = (all_twt == true) ? FLOW_ID_ALL : flow_id;
        int suspend_time_ms = itwt_args.suspend_time_ms->count ? itwt_args.suspend_time_ms->ival[0] : 0;
        if (flow_id > 0) {
            err = esp_wifi_sta_itwt_suspend(flow_id, suspend_time_ms);
            ESP_LOGI(TAG, "(itwt)suspend, flow_id:%d, all_twt:%d, suspend:%d ms, err:0x%x", flow_id, all_twt, suspend_time_ms, err);
        } else {
            ESP_LOGE(TAG, "(itwt)suspend, should specify an existing flow id");
        }
    }

    return 0;
}

static int wifi_cmd_itwt_probe(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &itwt_probe_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, itwt_probe_args.end, argv[0]);
        return 1;
    }

    esp_err_t err = ESP_OK;
    if (itwt_probe_args.timeout->count) {
        if (itwt_probe_args.timeout->ival[0] > 0) {
            ESP_LOGI(TAG, "(itwt)send probe req, timeout:%d ms", itwt_probe_args.timeout->ival[0]);
            err = esp_wifi_sta_itwt_send_probe_req(itwt_probe_args.timeout->ival[0]);
        } else {
            ESP_LOGE(TAG, "(itwt)invalid input, timeout:%d ms", itwt_probe_args.timeout->ival[0]);
        }
    }
    ESP_LOGI(TAG, "err:0x%x", err);
    return err;
}

void register_wifi_itwt(void)
{
    /* itwt setup/teardown */
    itwt_args.setup = arg_int0(NULL, "setup", "<setup>", "twt setup/teardown an individual flow id");
    itwt_args.teardown = arg_int0(NULL, "teardown", "<setup>", "twt setup/teardown an individual flow id");
    itwt_args.suspend = arg_int0(NULL, "suspend", "<setup>", "twt setup/teardown an individual flow id");
    itwt_args.trigger = arg_int0("t", NULL, "<trigger>", "trigger");
    itwt_args.flowtype = arg_int0("f", NULL, "<flow_type>", "flow type: 0-announced, 1-unannounced");
    itwt_args.negtype = arg_int0("n", NULL, "<neg_type>", "negotiate type");
    itwt_args.minwakedur = arg_int0("d", NULL, "<minwakedur>", "Norminal Min. Wake Duration");
    itwt_args.wakeduraunit = arg_int0("u", NULL, "<wakeduraunit>", "wake duration unit 0-256us, 1-TU (TU = 1024us)");
    itwt_args.wakeinvlexp = arg_int0("e", NULL, "<wakeinvlexp>", "Wake Interval Exponent");
    itwt_args.wakeinvlman = arg_int0("m", NULL, "<wakeinvlman>", "Wake Interval Mantissa");
    itwt_args.flowid = arg_int0("i", NULL, "<flow_id>", "Flow ID");
    itwt_args.suspend_time_ms = arg_int0("s", NULL, "<suspend_time_ms>", "time of suspending iTWT agreements, unit ms");
    itwt_args.twtid = arg_int0("w", NULL, "<twt_id>", "TWT ID");
    itwt_args.setup_timeout_time_ms = arg_int0("u", NULL, "<setup_timeout_time_ms>", "iTWT setup timeout time, unit ms");
    itwt_args.all_twt = arg_int0("a", NULL, "<all_twt>", "All TWT");
    itwt_args.end = arg_end(1);
    const esp_console_cmd_t itwt_cmd = {
        .command = "itwt",
        .help = "itwt setup, teardown or suspend",
        .hint = NULL,
        .func = &wifi_cmd_itwt,
        .argtable = &itwt_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&itwt_cmd));

    /* itwt probe */
    itwt_probe_args.timeout = arg_int0("t", NULL, "[timeout]", "time of sending a probe request frame and receiving a probe response frame from ap, unit ms");
    itwt_probe_args.end = arg_end(1);
    const esp_console_cmd_t itwt_probe_cmd = {
        .command = "probe",
        .help = "send probe request for TSF update when at lease one itwt agreement setup",
        .hint = NULL,
        .func = &wifi_cmd_itwt_probe,
        .argtable = &itwt_probe_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&itwt_probe_cmd));
}

#else

void register_wifi_itwt(void)
{
    ;
}

#endif /* CONFIG_SOC_WIFI_HE_SUPPORT */
