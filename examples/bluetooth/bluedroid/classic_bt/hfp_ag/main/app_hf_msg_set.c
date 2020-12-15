/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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

void hf_msg_show_usage(void)
{
    printf("########################################################################\n");
    printf("HFP AG command usage manual\n");
    printf("HFP AG commands begins with \"hf\" and end with \";\"\n");
    printf("Supported commands are as follows, arguments are embraced with < and >\n\n");
    printf("hf con;                   -- set up connection with peer device\n");
    printf("hf dis;                   -- disconnection with peer device\n");
    printf("hf cona;                  -- set up audio connection with peer device\n");
    printf("hf disa;                  -- release audio connection with peer device\n");
    printf("hf vron;                  -- start voice recognition\n");
    printf("hf vroff;                 -- stop voice recognition\n");
    printf("hf vu <tgt> <vol>;        -- volume update\n");
    printf("     tgt: 0-speaker, 1-microphone\n");
    printf("     vol: volume gain ranges from 0 to 15\n");
    printf("hf ind <call> <ntk> <callsetup> <sig>;       -- unsolicited indication device status to HF Client\n");
    printf("     call: call status [0,1]\n");
    printf("     callsetup: call setup status [0,3]\n");
    printf("     ntk: network status [0,1]\n");
    printf("     sig: signal strength value from 0~5\n");
    printf("hf ate <rep> <err>;       -- send extended at error code\n");
    printf("     rep: response code from 0 to 7\n");
    printf("     err: error code from 0 to 32\n");
    printf("hf iron;                  -- in-band ring tone provided\n");
    printf("hf iroff;                 -- in-band ring tone not provided\n");
    printf("hf ac;                    -- Answer Incoming Call from AG\n");
    printf("hf rc;                    -- Reject Incoming Call from AG\n");
    printf("hf d <num>;               -- Dial Number by AG, e.g. hf d 11223344\n");
    printf("hf end;                   -- End up a call by AG\n");
    printf("hf h;                     -- to see the command for HFP AG\n");
    printf("########################################################################\n");
}

#define HF_CMD_HANDLER(cmd)    static int hf_##cmd##_handler(int argn, char **argv)

HF_CMD_HANDLER(help)
{
    hf_msg_show_usage();
    return 0;
}

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
HF_CMD_HANDLER(ind_change)
{
    if (argn != 5) {
        printf("Insufficient number of arguments");
        return 1;
    }

    int call_state, ntk_state, call_setup_state, signal;

    if (sscanf(argv[1], "%d", &call_state) != 1 ||
        (call_state != ESP_HF_CALL_STATUS_NO_CALLS &&
        call_state != ESP_HF_CALL_STATUS_CALL_IN_PROGRESS)) {
        printf("Invalid argument for call state %s\n", argv[1]);
        return 1;
    }
    if (sscanf(argv[2], "%d", &call_setup_state) != 1 ||
        (call_setup_state < ESP_HF_CALL_SETUP_STATUS_IDLE || call_setup_state > ESP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING)) {
        printf("Invalid argument for callsetup state %s\n", argv[2]);
        return 1;
    }
    if (sscanf(argv[3], "%d", &ntk_state) != 1 ||
        (ntk_state != ESP_HF_NETWORK_STATE_NOT_AVAILABLE &&
        ntk_state != ESP_HF_NETWORK_STATE_AVAILABLE)) {
        printf("Invalid argument for netwrok state %s\n", argv[3]);
        return 1;
    }
    if (sscanf(argv[4], "%d", &signal) != 1 ||
            (signal < 0 || signal > 5)) {
        printf("Invalid argument for signal %s\n", argv[4]);
        return 1;
    }
    printf("Device Indicator Changed!\n");
    esp_hf_ag_devices_status_indchange(hf_peer_addr, call_state, call_setup_state, ntk_state, signal);
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
HF_CMD_HANDLER(d)
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
    {0,    "h",            hf_help_handler},
    {5,    "con",          hf_conn_handler},
    {10,   "dis",          hf_disc_handler},
    {20,   "cona",         hf_conn_audio_handler},
    {30,   "disa",         hf_disc_audio_handler},
    {40,   "vu",           hf_volume_control_handler},
    {50,   "ind",          hf_ind_change_handler},
    {60,   "vron",         hf_vra_on_handler},
    {70,   "vroff",        hf_vra_off_handler},
    {80,   "ate",          hf_cme_err_handler},
    {90,   "iron",         hf_ir_on_handler},
    {100,  "iroff",        hf_ir_off_handler},
    {110,  "ac",           hf_ac_handler},
    {120,  "rc",           hf_rc_handler},
    {130,  "end",          hf_end_handler},
    {140,  "d",            hf_d_handler},
};

hf_msg_hdl_t *hf_get_cmd_tbl(void)
{
    return hf_cmd_tbl;
}

size_t hf_get_cmd_tbl_size(void)
{
    return sizeof(hf_cmd_tbl) / sizeof(hf_msg_hdl_t);
}

#define HF_ORDER(name)   name##_cmd
enum hf_cmd_name {
    h = 0,      /*show command manual*/
    con,        /*set up connection with peer device*/
    dis,        /*disconnection with peer device*/
    cona,       /*set up audio connection with peer device*/
    disa,       /*release connection with peer device*/
    vu,         /*volume update*/
    ind,        /*unsolicited indication device status to HF Client*/
    vron,       /*start voice recognition*/
    vroff,      /*stop voice recognition*/
    ate,        /*send extended at error code*/
    iron,       /*in-band ring tone provided*/
    iroff,      /*in-band ring tone not provided*/
    ac,         /*Answer Incoming Call from AG*/
    rc,         /*Reject Incoming Call from AG*/
    end,        /*End up a call by AG*/
    d           /*Dial Number by AG, e.g. d 11223344*/
};
static char *hf_cmd_explain[] = {
    "show command manual",
    "set up connection with peer device",
    "disconnection with peer device",
    "set up audio connection with peer device",
    "release connection with peer device",
    "volume update",
    "unsolicited indication device status to HF Client",
    "start voice recognition",
    "stop voice recognition",
    "send extended at error code",
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
    struct arg_str *call;
    struct arg_str *ntk;
    struct arg_str *callsetup;
    struct arg_str *sig;
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
            .help = hf_cmd_explain[con],
            .hint = NULL,
            .func = hf_cmd_tbl[con].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(con)));

        const esp_console_cmd_t HF_ORDER(dis) = {
            .command = "dis",
            .help = hf_cmd_explain[dis],
            .hint = NULL,
            .func = hf_cmd_tbl[dis].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(dis)));

        const esp_console_cmd_t HF_ORDER(cona) = {
            .command = "cona",
            .help = hf_cmd_explain[cona],
            .hint = NULL,
            .func = hf_cmd_tbl[cona].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(cona)));

        const esp_console_cmd_t HF_ORDER(disa) = {
            .command = "disa",
            .help = hf_cmd_explain[disa],
            .hint = NULL,
            .func = hf_cmd_tbl[disa].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(disa)));



        const esp_console_cmd_t HF_ORDER(ac) = {
            .command = "ac",
            .help = hf_cmd_explain[ac],
            .hint = NULL,
            .func = hf_cmd_tbl[ac].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(ac)));

        const esp_console_cmd_t HF_ORDER(rc) = {
            .command = "rc",
            .help = hf_cmd_explain[rc],
            .hint = NULL,
            .func = hf_cmd_tbl[rc].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(rc)));

        const esp_console_cmd_t HF_ORDER(d) = {
            .command = "d",
            .help = hf_cmd_explain[d],
            .hint = "<num>",
            .func = hf_cmd_tbl[d].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(d)));



        const esp_console_cmd_t HF_ORDER(vron) = {
            .command = "vron",
            .help = hf_cmd_explain[vron],
            .hint = NULL,
            .func = hf_cmd_tbl[vron].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(vron)));

        const esp_console_cmd_t HF_ORDER(vroff) = {
            .command = "vroff",
            .help = hf_cmd_explain[vroff],
            .hint = NULL,
            .func = hf_cmd_tbl[vroff].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(vroff)));

        vu_args.tgt = arg_str1(NULL, NULL, "<tgt>", "\n        0-speaker\n        1-microphone");
        vu_args.vol = arg_str1(NULL, NULL, "<vol>", "volume gain ranges from 0 to 15");
        vu_args.end = arg_end(1);
        const esp_console_cmd_t HF_ORDER(vu) = {
            .command = "vu",
            .help = hf_cmd_explain[vu],
            .hint = NULL,
            .func = hf_cmd_tbl[vu].handler,
            .argtable = &vu_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(vu)));

        const esp_console_cmd_t HF_ORDER(end) = {
            .command = "end",
            .help = hf_cmd_explain[end],
            .hint = NULL,
            .func = hf_cmd_tbl[end].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(end)));

        const esp_console_cmd_t HF_ORDER(iron) = {
            .command = "iron",
            .help = hf_cmd_explain[iron],
            .hint = NULL,
            .func = hf_cmd_tbl[iron].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(iron)));

        const esp_console_cmd_t HF_ORDER(iroff) = {
            .command = "iroff",
            .help = hf_cmd_explain[iroff],
            .hint = NULL,
            .func = hf_cmd_tbl[iroff].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(iroff)));

        ind_args.call = arg_str1(NULL, NULL, "<call>", "call status [0,1]");
        ind_args.callsetup = arg_str1(NULL, NULL, "<callsetup>", "call setup status [0,3]");
        ind_args.ntk = arg_str1(NULL, NULL, "<ntk>", "network status [0,1]");
        ind_args.sig = arg_str1(NULL, NULL, "<sig>", "signal strength value from 0~5");
        ind_args.end = arg_end(1);
        const esp_console_cmd_t HF_ORDER(ind) = {
            .command = "ind",
            .help = hf_cmd_explain[ind],
            .hint = NULL,
            .func = hf_cmd_tbl[ind].handler,
            .argtable = &ind_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(ind)));

        ate_args.err = arg_str1(NULL, NULL, "<err>", "error code from 0 to 32");
        ate_args.rep = arg_str1(NULL, NULL, "<rep>", "response code from 0 to 7");
        ate_args.end = arg_end(1);
        const esp_console_cmd_t HF_ORDER(ate) = {
            .command = "ate",
            .help = hf_cmd_explain[ate],
            .hint = NULL,
            .func = hf_cmd_tbl[ate].handler,
            .argtable = &ate_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&HF_ORDER(ate)));
}
