/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_hf_ag_api.h"
#include "app_hf_msg_set.h"
#include "bt_app_hf.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"

// if you want to connect a specific device, add it's bda here
esp_bd_addr_t hf_peer_addr = {0xB0, 0xF1, 0xA3, 0x01, 0x2D,0x2E};


#define HF_CMD_HANDLER(cmd)    static int hf_##cmd##_handler(int argn, char **argv)

HF_CMD_HANDLER(conn)
{
    printf("Connect.\n");
    esp_hf_ag_slc_connect(hf_peer_addr);
    return 0;
}

HF_CMD_HANDLER(disc)
{
    printf("Disconnect\n");
    esp_hf_ag_slc_disconnect(hf_peer_addr);
    return 0;
}

HF_CMD_HANDLER(conn_audio)
{
    printf("Connect Audio\n");
    esp_hf_ag_audio_connect(hf_peer_addr);
    return 0;
}

HF_CMD_HANDLER(disc_audio)
{
    printf("Disconnect Audio\n");
    esp_hf_ag_audio_disconnect(hf_peer_addr);
    return 0;
}

//AT+BVRA
HF_CMD_HANDLER(vra_on)
{
    printf("Start Voice Recognition.\n");
    esp_hf_ag_vra_control(hf_peer_addr,1);
    return 0;
}
//AT+BVRA
HF_CMD_HANDLER(vra_off)
{
    printf("Stop Voicer Recognition.\n");
    esp_hf_ag_vra_control(hf_peer_addr,0);
    return 0;
}

//AT+VGS or AT+VGM
HF_CMD_HANDLER(volume_control)
{
    if (argn != 3) {
        printf("Insufficient number of arguments");
        return 1;
    }
    int target, volume;
    if (sscanf(argv[1], "%d", &target) != 1 ||
        (target != ESP_HF_VOLUME_CONTROL_TARGET_SPK &&
        target != ESP_HF_VOLUME_CONTROL_TARGET_MIC)) {
        printf("Invalid argument for target %s\n", argv[1]);
        return 1;
    }
    if (sscanf(argv[2], "%d", &volume) != 1 ||
            (volume < 0 || volume > 15)) {
        printf("Invalid argument for volume %s\n", argv[2]);
        return 1;
    }
    printf("Volume Update\n");
    esp_hf_ag_volume_control(hf_peer_addr, target, volume);
    return 0;
}

//+CIEV
HF_CMD_HANDLER(ciev_report)
{
    if (argn != 3) {
        printf("Insufficient number of arguments");
        return 1;
    }

    int ind_type, value;

    sscanf(argv[1], "%d", &ind_type);
    sscanf(argv[2], "%d", &value);

    if (ind_type > ESP_HF_IND_TYPE_CALLHELD) {
        printf("Invalid argument for status type %s\n", argv[1]);
        return 1;
    }
    if ((ind_type == ESP_HF_IND_TYPE_CALL) &&
        (value != ESP_HF_CALL_STATUS_NO_CALLS &&
        value != ESP_HF_CALL_STATUS_CALL_IN_PROGRESS)) {
        printf("Invalid argument for callsetup state %s\n", argv[2]);
        return 1;
    }
    if ((ind_type == ESP_HF_IND_TYPE_CALLSETUP) &&
        (value < ESP_HF_CALL_SETUP_STATUS_IDLE ||
        value > ESP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING)) {
        printf("Invalid argument for call state %s\n", argv[2]);
        return 1;
    }
    if ((ind_type == ESP_HF_IND_TYPE_SERVICE) &&
        (value != ESP_HF_NETWORK_STATE_NOT_AVAILABLE &&
        value != ESP_HF_NETWORK_STATE_AVAILABLE)) {
        printf("Invalid argument for network state %s\n", argv[2]);
        return 1;
    }
    if ((ind_type == ESP_HF_IND_TYPE_SIGNAL &&
        (value < 0 || value > 5))) {
        printf("Invalid argument for signal %s\n", argv[2]);
        return 1;
    }
    if ((ind_type == ESP_HF_IND_TYPE_ROAM &&
        (value != ESP_HF_ROAMING_STATUS_INACTIVE &&
        value != ESP_HF_ROAMING_STATUS_ACTIVE))) {
        printf("Invalid argument for roaming state %s\n", argv[2]);
        return 1;
    }
    if ((ind_type == ESP_HF_IND_TYPE_BATTCHG &&
        (value < 0 || value > 5))) {
        printf("Invalid argument for battery %s\n", argv[2]);
        return 1;
    }
    if ((ind_type == ESP_HF_IND_TYPE_CALLHELD) &&
        (value < ESP_HF_CALL_HELD_STATUS_NONE ||
        value > ESP_HF_CALL_HELD_STATUS_HELD)) {
        printf("Invalid argument for call held state %s\n", argv[2]);
        return 1;
    }

    printf("Device Indicator Changed!\n");
    esp_hf_ag_ciev_report(hf_peer_addr, ind_type, value);
    return 0;
}

//AT+CMEE
HF_CMD_HANDLER(cme_err)
{
    if (argn != 3) {
        printf("Insufficient number of arguments");
        return 1;
    }

    int response_code, error_code;
    if (sscanf(argv[1], "%d", &response_code) != 1 ||
        (response_code < ESP_HF_AT_RESPONSE_CODE_OK && response_code > ESP_HF_AT_RESPONSE_CODE_CME)) {
        printf("Invalid argument for response_code %s\n", argv[1]);
        return 1;
    }

    if (sscanf(argv[2], "%d", &error_code) != 1 ||
            (error_code < ESP_HF_CME_AG_FAILURE || error_code > ESP_HF_CME_NETWORK_NOT_ALLOWED)) {
        printf("Invalid argument for volume %s\n", argv[2]);
        return 1;
    }

    printf("Send CME Error.\n");
    esp_hf_ag_cmee_send(hf_peer_addr,response_code,error_code);
    return 0;
}

//+BSIR:1
HF_CMD_HANDLER(ir_on)
{
    printf("Enable Voicer Recognition.\n");
    esp_hf_ag_bsir(hf_peer_addr,1);
    return 0;
}

//+BSIR:0
HF_CMD_HANDLER(ir_off)
{
    printf("Disable Voicer Recognition.\n");
    esp_hf_ag_bsir(hf_peer_addr,0);
    return 0;
}

//Answer Call from AG
HF_CMD_HANDLER(ac)
{
    printf("Answer Call from AG.\n");
    char *number = {"123456"};
    esp_hf_ag_answer_call(hf_peer_addr,1,0,1,1,number,0);
    return 0;
}

//Reject Call from AG
HF_CMD_HANDLER(rc)
{
    printf("Reject Call from AG.\n");
    char *number = {"123456"};
    esp_hf_ag_reject_call(hf_peer_addr,0,0,0,0,number,0);
    return 0;
}

//End Call from AG
HF_CMD_HANDLER(end)
{
    printf("End Call from AG.\n");
    char *number = {"123456"};
    esp_hf_ag_end_call(hf_peer_addr,0,0,0,0,number,0);
    return 0;
}

//Dial Call from AG
HF_CMD_HANDLER(dn)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
    } else {
        printf("Dial number %s\n", argv[1]);
        esp_hf_ag_out_call(hf_peer_addr,1,0,1,2,argv[1],0);
    }
    return 0;
}

static hf_msg_hdl_t hf_cmd_tbl[] = {
    {"con",          hf_conn_handler},
    {"dis",          hf_disc_handler},
    {"cona",         hf_conn_audio_handler},
    {"disa",         hf_disc_audio_handler},
    {"vu",           hf_volume_control_handler},
    {"ciev",         hf_ciev_report_handler},
    {"vron",         hf_vra_on_handler},
    {"vroff",        hf_vra_off_handler},
    {"ate",          hf_cme_err_handler},
    {"iron",         hf_ir_on_handler},
    {"iroff",        hf_ir_off_handler},
    {"ac",           hf_ac_handler},
    {"rc",           hf_rc_handler},
    {"end",          hf_end_handler},
    {"dn",           hf_dn_handler},
};

#define HF_ORDER(name)   name##_cmd
enum hf_cmd_idx {
    HF_CMD_IDX_CON = 0,       /*set up connection with peer device*/
    HF_CMD_IDX_DIS,           /*disconnection with peer device*/
    HF_CMD_IDX_CONA,          /*set up audio connection with peer device*/
    HF_CMD_IDX_DISA,          /*release audio connection with peer device*/
    HF_CMD_IDX_VU,            /*volume update*/
    HF_CMD_IDX_CIEV,          /*unsolicited indication device status to HF Client*/
    HF_CMD_IDX_VRON,          /*start voice recognition*/
    HF_CMD_IDX_VROFF,         /*stop voice recognition*/
    HF_CMD_IDX_ATE,           /*send extended AT error code*/
    HF_CMD_IDX_IRON,          /*in-band ring tone provided*/
    HF_CMD_IDX_IROFF,         /*in-band ring tone not provided*/
    HF_CMD_IDX_AC,            /*Answer Incoming Call from AG*/
    HF_CMD_IDX_RC,            /*Reject Incoming Call from AG*/
    HF_CMD_IDX_END,           /*End up a call by AG*/
    HF_CMD_IDX_DN             /*Dial Number by AG, e.g. d 11223344*/
};

static char *hf_cmd_explain[] = {
    "set up connection with peer device",
    "disconnection with peer device",
    "set up audio connection with peer device",
    "release audio connection with peer device",
    "volume update",
    "unsolicited indication device status to HF Client",
    "start voice recognition",
    "stop voice recognition",
    "send extended AT error code",
    "in-band ring tone provided",
    "in-band ring tone not provided",
    "Answer Incoming Call from AG",
    "Reject Incoming Call from AG",
    "End up a call by AG",
    "Dial Number by AG, e.g. d 11223344",
};
typedef struct {
    struct arg_str *tgt;
    struct arg_str *vol;
    struct arg_end *end;
} vu_args_t;

typedef struct {
    struct arg_str *ind_type;
    struct arg_str *value;
    struct arg_end *end;
} ind_args_t;

typedef struct {
    struct arg_str *rep;
    struct arg_str *err;
    struct arg_end *end;
} ate_args_t;

static vu_args_t vu_args;
static ind_args_t ind_args;
static ate_args_t ate_args;

void register_hfp_ag(void)
{

        const esp_console_cmd_t HF_ORDER(con) = {
            .command = "con",
            .help = hf_cmd_explain[HF_CMD_IDX_CON],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_CON].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(con)));

        const esp_console_cmd_t HF_ORDER(dis) = {
            .command = "dis",
            .help = hf_cmd_explain[HF_CMD_IDX_DIS],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_DIS].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(dis)));

        const esp_console_cmd_t HF_ORDER(cona) = {
            .command = "cona",
            .help = hf_cmd_explain[HF_CMD_IDX_CONA],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_CONA].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(cona)));

        const esp_console_cmd_t HF_ORDER(disa) = {
            .command = "disa",
            .help = hf_cmd_explain[HF_CMD_IDX_DISA],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_DISA].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(disa)));

        const esp_console_cmd_t HF_ORDER(ac) = {
            .command = "ac",
            .help = hf_cmd_explain[HF_CMD_IDX_AC],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_AC].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(ac)));

        const esp_console_cmd_t HF_ORDER(rc) = {
            .command = "rc",
            .help = hf_cmd_explain[HF_CMD_IDX_RC],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_RC].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(rc)));

        const esp_console_cmd_t HF_ORDER(dn) = {
            .command = "dn",
            .help = hf_cmd_explain[HF_CMD_IDX_DN],
            .hint = "<num>",
            .func = hf_cmd_tbl[HF_CMD_IDX_DN].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(dn)));

        const esp_console_cmd_t HF_ORDER(vron) = {
            .command = "vron",
            .help = hf_cmd_explain[HF_CMD_IDX_VRON],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_VRON].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(vron)));

        const esp_console_cmd_t HF_ORDER(vroff) = {
            .command = "vroff",
            .help = hf_cmd_explain[HF_CMD_IDX_VROFF],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_VROFF].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(vroff)));

        vu_args.tgt = arg_str1(NULL, NULL, "<tgt>", "\n        0-speaker\n        1-microphone");
        vu_args.vol = arg_str1(NULL, NULL, "<vol>", "volume gain ranges from 0 to 15");
        vu_args.end = arg_end(1);
        const esp_console_cmd_t HF_ORDER(vu) = {
            .command = "vu",
            .help = hf_cmd_explain[HF_CMD_IDX_VU],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_VU].handler,
            .argtable = &vu_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(vu)));

        const esp_console_cmd_t HF_ORDER(end) = {
            .command = "end",
            .help = hf_cmd_explain[HF_CMD_IDX_END],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_END].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(end)));

        const esp_console_cmd_t HF_ORDER(iron) = {
            .command = "iron",
            .help = hf_cmd_explain[HF_CMD_IDX_IRON],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_IRON].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(iron)));

        const esp_console_cmd_t HF_ORDER(iroff) = {
            .command = "iroff",
            .help = hf_cmd_explain[HF_CMD_IDX_IROFF],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_IROFF].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(iroff)));

        ind_args.ind_type = arg_str1(NULL, NULL, "<ind_type>", "\n    1-call\n    2-callsetup\n    3-serval\n \
   4-signal\n    5-roam\n    6-battery\n    7-callheld");
        ind_args.value = arg_str1(NULL, NULL, "<value>", "value of indicator type");
        ind_args.end = arg_end(1);
        const esp_console_cmd_t HF_ORDER(ciev) = {
            .command = "ciev",
            .help = hf_cmd_explain[HF_CMD_IDX_CIEV],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_CIEV].handler,
            .argtable = &ind_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(ciev)));

        ate_args.err = arg_str1(NULL, NULL, "<err>", "error code from 0 to 32");
        ate_args.rep = arg_str1(NULL, NULL, "<rep>", "response code from 0 to 7");
        ate_args.end = arg_end(1);
        const esp_console_cmd_t HF_ORDER(ate) = {
            .command = "ate",
            .help = hf_cmd_explain[HF_CMD_IDX_ATE],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_ATE].handler,
            .argtable = &ate_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(ate)));
}
