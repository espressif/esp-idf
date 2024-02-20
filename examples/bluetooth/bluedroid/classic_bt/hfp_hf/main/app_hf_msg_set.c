/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_hf_client_api.h"
#include "app_hf_msg_set.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"

extern esp_bd_addr_t peer_addr;

typedef struct {
    struct arg_str *tgt;
    struct arg_str *vol;
    struct arg_end *end;
} vu_args_t;

typedef struct {
    struct arg_str *btrh;
    struct arg_end *end;
} rh_args_t;

typedef struct {
    struct arg_int *bat_level;
    struct arg_int *docked;
    struct arg_end *end;
} bat_args_t;

static vu_args_t vu_args;
static rh_args_t rh_args;
static bat_args_t bat_args;

#define HF_CMD_HANDLER(cmd)    static int hf_##cmd##_handler(int argn, char **argv)

HF_CMD_HANDLER(conn)
{
    printf("connect\n");
    esp_hf_client_connect(peer_addr);
    return 0;
}

HF_CMD_HANDLER(disc)
{
    printf("disconnect\n");
    esp_hf_client_disconnect(peer_addr);
    return 0;
}

HF_CMD_HANDLER(conn_audio)
{
    printf("connect audio\n");
    esp_hf_client_connect_audio(peer_addr);
    return 0;
}

HF_CMD_HANDLER(disc_audio)
{
    printf("disconnect audio\n");
    esp_hf_client_disconnect_audio(peer_addr);
    return 0;
}

HF_CMD_HANDLER(query_op)
{
    printf("Query operator\n");
    esp_hf_client_query_current_operator_name();
    return 0;
}

HF_CMD_HANDLER(answer)
{
    printf("Answer call\n");
    esp_hf_client_answer_call();
    return 0;
}
HF_CMD_HANDLER(reject)
{
    printf("Reject call\n");
    esp_hf_client_reject_call();
    return 0;
}

HF_CMD_HANDLER(dial)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
    } else {
        printf("Dial number %s\n", argv[1]);
        esp_hf_client_dial(argv[1]);
    }
    return 0;
}

HF_CMD_HANDLER(redial)
{
    printf("Dial number\n");
    esp_hf_client_dial(NULL);
    return 0;
}

HF_CMD_HANDLER(dial_mem)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
        return 1;
    }
    int index;
    if (sscanf(argv[1], "%d", &index) != 1) {
        printf("Invalid argument %s\n", argv[1]);
        return 1;
    }

    printf("Dial memory %d\n", index);
    esp_hf_client_dial_memory(index);
    return 0;
}


HF_CMD_HANDLER(start_vr)
{
    printf("Start voice recognition\n");
    esp_hf_client_start_voice_recognition();
    return 0;
}

HF_CMD_HANDLER(stop_vr)
{
    printf("Stop voice recognition\n");
    esp_hf_client_stop_voice_recognition();
    return 0;
}

HF_CMD_HANDLER(volume_update)
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

    printf("volume update\n");
    esp_hf_client_volume_update(target, volume);
    return 0;
}

HF_CMD_HANDLER(query_call)
{
    printf("Query current call status\n");
    esp_hf_client_query_current_calls();
    return 0;
}

HF_CMD_HANDLER(retrieve_subscriber)
{
    printf("Retrieve subscriber information\n");
    esp_hf_client_retrieve_subscriber_info();
    return 0;
}

HF_CMD_HANDLER(request_last_voice_tag)
{
    printf("Request last voice tag\n");
    esp_hf_client_request_last_voice_tag_number();
    return 0;
}

HF_CMD_HANDLER(btrh)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
        return 1;
    }

    int btrh;
    if (sscanf(argv[1], "%d", &btrh) != 1) {
        printf("Invalid argument %s\n", argv[1]);
        return 1;
    }
    if (btrh < ESP_HF_BTRH_CMD_HOLD || btrh > ESP_HF_BTRH_CMD_REJECT) {
        printf("Invalid argument %s\n", argv[1]);
        return 1;
    }
    printf("respond and hold command: %d\n", btrh);
    esp_hf_client_send_btrh_cmd(btrh);
    return 0;
}

static bool is_dtmf_code(char c)
{
    if (c >= '0' && c <= '9') {
        return true;
    }
    if (c == '#' || c == '*') {
        return true;
    }
    if (c >= 'A' && c <= 'D') {
        return true;
    }
    return false;
}

HF_CMD_HANDLER(dtmf)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
        return 1;
    }

    if (strlen(argv[1]) != 1 || !is_dtmf_code(argv[1][0])) {
        printf("Invalid argument %s\n", argv[1]);
        return 1;
    }

    printf("send dtmf code: %s\n", argv[1]);
    esp_hf_client_send_dtmf(argv[1][0]);
    return 0;
}

HF_CMD_HANDLER(xapl)
{
    printf("send XAPL feature enable command to indicate battery level\n");
    esp_hf_client_send_xapl("0505-1995-0610", ESP_HF_CLIENT_XAPL_FEAT_BATTERY_REPORT | ESP_HF_CLIENT_XAPL_FEAT_DOCKED);
    return 0;
}

HF_CMD_HANDLER(iphoneaccev)
{
    int nerrors = arg_parse(argn, argv, (void**) &bat_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, bat_args.end, argv[0]);
        return 1;
    }

    int bat_level = bat_args.bat_level->ival[0];
    bool docked = bat_args.docked->ival[0] == 0 ? false : true;

    if (bat_level > 9 || bat_level < 0) {
        printf("Invalid argument for battery level %d\n", bat_level);
        return 1;
    }

    printf("send battery level and docker status\n");
    esp_hf_client_send_iphoneaccev(bat_level, docked);
    return 0;
}


static hf_msg_hdl_t hf_cmd_tbl[] = {
    {"con",          hf_conn_handler},
    {"dis",          hf_disc_handler},
    {"cona",         hf_conn_audio_handler},
    {"disa",         hf_disc_audio_handler},
    {"qop",          hf_query_op_handler},
    {"qc",           hf_query_call_handler},
    {"ac",           hf_answer_handler},
    {"rc",           hf_reject_handler},
    {"d",            hf_dial_handler},
    {"rd",           hf_redial_handler},
    {"dm",           hf_dial_mem_handler},
    {"vron",         hf_start_vr_handler},
    {"vroff",        hf_stop_vr_handler},
    {"vu",           hf_volume_update_handler},
    {"rs",           hf_retrieve_subscriber_handler},
    {"rv",           hf_request_last_voice_tag_handler},
    {"rh",           hf_btrh_handler},
    {"k",            hf_dtmf_handler},
    {"xp",           hf_xapl_handler},
    {"bat",          hf_iphoneaccev_handler},
};

#define HF_ORDER(name)   name##_cmd
enum hf_cmd_idx {
    HF_CMD_IDX_CON = 0,       /*set up connection with peer device*/
    HF_CMD_IDX_DIS,           /*disconnection with peer device*/
    HF_CMD_IDX_CONA,          /*set up audio connection with peer device*/
    HF_CMD_IDX_DISA,          /*release audio connection with peer device*/
    HF_CMD_IDX_QOP,        /*query current operator name*/
    HF_CMD_IDX_QC,         /*query current call status*/
    HF_CMD_IDX_AC,         /*answer incoming call*/
    HF_CMD_IDX_RC,         /*reject incoming call*/
    HF_CMD_IDX_D,          /*dial <num>, e.g. d 11223344*/
    HF_CMD_IDX_RD,         /*redial*/
    HF_CMD_IDX_DM,         /*dial memory*/
    HF_CMD_IDX_VRON,       /*start voice recognition*/
    HF_CMD_IDX_VROFF,      /*stop voice recognition*/
    HF_CMD_IDX_VU,         /*volume update*/
    HF_CMD_IDX_RS,         /*retrieve subscriber information*/
    HF_CMD_IDX_RV,         /*retrieve last voice tag number*/
    HF_CMD_IDX_RH,         /*response and hold*/
    HF_CMD_IDX_K,          /*send dtmf code*/
    HF_CMD_IDX_XP,         /*send XAPL feature enable command to indicate battery level*/
    HF_CMD_IDX_BAT,        /*send battery level and docker status*/
};

static char *hf_cmd_explain[] = {
    "set up connection with peer device",
    "release connection with peer device",
    "setup audio connection with peer device",
    "release audio connection with peer device",
    "query current operator name",
    "query current call status",
    "answer incoming call",
    "reject incoming call",
    "dial <num>, e.g. d 11223344",
    "redial",
    "dial memory",
    "start voice recognition",
    "stop voice recognition",
    "volume update",
    "retrieve subscriber information",
    "retrieve last voice tag number",
    "response and hold",
    "send dtmf code.\n        <dtmf>  single character in set 0-9, *, #, A-D",
    "send XAPL feature enable command to indicate battery level",
    "send battery level and docker status",
};

void register_hfp_hf(void)
{

        const esp_console_cmd_t con_cmd = {
            .command = "con",
            .help = hf_cmd_explain[HF_CMD_IDX_CON],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_CON].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&con_cmd));

        const esp_console_cmd_t dis_cmd = {
            .command = "dis",
            .help = hf_cmd_explain[HF_CMD_IDX_DIS],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_DIS].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&dis_cmd));

        const esp_console_cmd_t cona_cmd = {
            .command = "cona",
            .help = hf_cmd_explain[HF_CMD_IDX_CONA],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_CONA].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&cona_cmd));

        const esp_console_cmd_t disa_cmd = {
            .command = "disa",
            .help = hf_cmd_explain[HF_CMD_IDX_DISA],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_DISA].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&disa_cmd));

        const esp_console_cmd_t qop_cmd = {
            .command = "qop",
            .help = hf_cmd_explain[HF_CMD_IDX_QOP],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_QOP].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&qop_cmd));

        const esp_console_cmd_t qc_cmd = {
            .command = "qc",
            .help = hf_cmd_explain[HF_CMD_IDX_QC],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_QC].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&qc_cmd));

        const esp_console_cmd_t ac_cmd = {
            .command = "ac",
            .help = hf_cmd_explain[HF_CMD_IDX_AC],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_AC].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&ac_cmd));

        const esp_console_cmd_t rc_cmd = {
            .command = "rc",
            .help = hf_cmd_explain[HF_CMD_IDX_RC],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_RC].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rc_cmd));

        const esp_console_cmd_t d_cmd = {
            .command = "d",
            .help = hf_cmd_explain[HF_CMD_IDX_D],
            .hint = "<num>",
            .func = hf_cmd_tbl[HF_CMD_IDX_D].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&d_cmd));

        const esp_console_cmd_t rd_cmd = {
            .command = "rd",
            .help = hf_cmd_explain[HF_CMD_IDX_RD],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_RD].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rd_cmd));

        const esp_console_cmd_t dm_cmd = {
            .command = "dm",
            .help = hf_cmd_explain[HF_CMD_IDX_DM],
            .hint = "<index>",
            .func = hf_cmd_tbl[HF_CMD_IDX_DM].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&dm_cmd));

        const esp_console_cmd_t vron_cmd = {
            .command = "vron",
            .help = hf_cmd_explain[HF_CMD_IDX_VRON],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_VRON].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&vron_cmd));

        const esp_console_cmd_t vroff_cmd = {
            .command = "vroff",
            .help = hf_cmd_explain[HF_CMD_IDX_VROFF],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_VROFF].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&vroff_cmd));

        vu_args.tgt = arg_str1(NULL, NULL, "<tgt>", "\n        0-speaker\n        1-microphone");
        vu_args.vol = arg_str1(NULL, NULL, "<vol>", "volume gain ranges from 0 to 15");
        vu_args.end = arg_end(1);
        const esp_console_cmd_t vu_cmd = {
            .command = "vu",
            .help = hf_cmd_explain[HF_CMD_IDX_VU],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_VU].handler,
            .argtable = &vu_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&vu_cmd));

        const esp_console_cmd_t rs_cmd = {
            .command = "rs",
            .help = hf_cmd_explain[HF_CMD_IDX_RS],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_RS].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rs_cmd));

        const esp_console_cmd_t rv_cmd = {
            .command = "rv",
            .help = hf_cmd_explain[HF_CMD_IDX_RV],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_RV].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rv_cmd));

        rh_args.btrh = arg_str1(NULL, NULL, "<btrh>", "\n        0 - put call on hold,\n        1 - accept the held call,\n        2 -reject the held call");
        rh_args.end = arg_end(1);
        const esp_console_cmd_t rh_cmd = {
            .command = "rh",
            .help = hf_cmd_explain[HF_CMD_IDX_RH],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_RH].handler,
            .argtable = &rh_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rh_cmd));

        const esp_console_cmd_t k_cmd = {
            .command = "k",
            .help = hf_cmd_explain[HF_CMD_IDX_K],
            .hint = "<dtmf>",
            .func = hf_cmd_tbl[HF_CMD_IDX_K].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&k_cmd));

        const esp_console_cmd_t xp_cmd = {
            .command = "xp",
            .help = hf_cmd_explain[HF_CMD_IDX_XP],
            .hint = NULL,
            .func = hf_cmd_tbl[HF_CMD_IDX_XP].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&xp_cmd));

        bat_args.bat_level = arg_int0(NULL, NULL, "<bat_level>", "battery level ranges from 0 to 9");
        bat_args.docked = arg_int0(NULL, NULL, "<docked>", "0 - undocked; 1 - docked");
        bat_args.end = arg_end(1);
        const esp_console_cmd_t bat_cmd = {
            .command = "bat",
            .help = hf_cmd_explain[HF_CMD_IDX_BAT],
            .hint = "<bat_level> <docked>",
            .func = hf_cmd_tbl[HF_CMD_IDX_BAT].handler,
            .argtable = &bat_args,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&bat_cmd));
}
