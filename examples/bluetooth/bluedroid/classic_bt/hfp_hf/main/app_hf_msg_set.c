/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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

void hf_msg_show_usage(void)
{
    printf("########################################################################\n");
    printf("HF client command usage manual\n");
    printf("HF client commands begin with \"hf\" and end with \";\"\n");
    printf("Supported commands are as follows, arguments are embraced with < and >\n");
    printf("hf con;   -- setup connection with peer device\n");
    printf("hf dis;   -- release connection with peer device\n");
    printf("hf cona;  -- setup audio connection with peer device\n");
    printf("hf disa;  -- release connection with peer device\n");
    printf("hf qop;   -- query current operator name\n");
    printf("hf qc;    -- query current call status\n");
    printf("hf ac;    -- answer incoming call\n");
    printf("hf rc;    -- reject incoming call\n");
    printf("hf d <num>;        -- dial <num>, e.g. hf d 11223344\n");
    printf("hf rd;             -- redial\n");
    printf("hf dm <index>;     -- dial memory\n");
    printf("hf vron;           -- start voice recognition\n");
    printf("hf vroff;          -- stop voice recognition\n");
    printf("hf vu <tgt> <vol>; -- volume update\n");
    printf("     tgt: 0-speaker, 1-microphone\n");
    printf("     vol: volume gain ranges from 0 to 15\n");
    printf("hf rs;             -- retrieve subscriber information\n");
    printf("hf rv;             -- retrieve last voice tag number\n");
    printf("hf rh <btrh>;      -- response and hold\n");
    printf("     btrh:\n");
    printf("        0 - put call on hold,\n");
    printf("        1 - accept the held call,\n");
    printf("        2 -reject the held call\n");
    printf("hf k <dtmf>;       -- send dtmf code.\n");
    printf("     dtmf: single character in set 0-9, *, #, A-D\n");
    printf("hf xp;             -- Enable Vendor specific feature for battery status\n");
    printf("hf bat;            -- Send battery status\n");
    printf("hf h;              -- show command manual\n");
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
    {0,    "h",            hf_help_handler},
    {5,    "con",          hf_conn_handler},
    {10,   "dis",          hf_disc_handler},
    {20,   "cona",         hf_conn_audio_handler},
    {30,   "disa",         hf_disc_audio_handler},
    {40,   "qop",          hf_query_op_handler},
    {120,  "qc",           hf_query_call_handler},
    {50,   "ac",           hf_answer_handler},
    {60,   "rc",           hf_reject_handler},
    {70,   "d",            hf_dial_handler},
    {80,   "rd",           hf_redial_handler},
    {85,   "dm",           hf_dial_mem_handler},
    {90,   "vron",         hf_start_vr_handler},
    {100,  "vroff",        hf_stop_vr_handler},
    {110,  "vu",           hf_volume_update_handler},
    {130,  "rs",           hf_retrieve_subscriber_handler},
    {140,  "rv",           hf_request_last_voice_tag_handler},
    {150,  "rh",           hf_btrh_handler},
    {160,  "k",            hf_dtmf_handler},
    {170,  "xp",           hf_xapl_handler},
    {180,  "bat",          hf_iphoneaccev_handler},
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
    dis,        /*release connection with peer device*/
    cona,       /*setup audio connection with peer device*/
    disa,       /*setup audio connection with peer device*/
    qop,        /*query current operator name*/
    qc,         /*query current call status*/
    ac,         /*answer incoming call*/
    rc,         /*reject incoming call*/
    d,          /*dial <num>, e.g. d 11223344*/
    rd,         /*redial*/
    dm,         /*dial memory*/
    vron,       /*start voice recognition*/
    vroff,      /*stop voice recognition*/
    vu,         /*volume update*/
    rs,         /*retrieve subscriber information*/
    rv,         /*retrieve last voice tag number*/
    rh,         /*response and hold*/
    k,          /*send dtmf code*/
    xp,         /*send XAPL feature enable command to indicate battery level*/
    bat,        /*send battery level and docker status*/
};
static char *hf_cmd_explain[] = {
    "show command manual",
    "set up connection with peer device",
    "release connection with peer device",
    "setup audio connection with peer device",
    "release connection with peer device",
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
    "send battery level and docker status.",
};

void register_hfp_hf(void)
{

        const esp_console_cmd_t con_cmd = {
            .command = "con",
            .help = hf_cmd_explain[con],
            .hint = NULL,
            .func = hf_cmd_tbl[con].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&con_cmd));

        const esp_console_cmd_t dis_cmd = {
            .command = "dis",
            .help = hf_cmd_explain[dis],
            .hint = NULL,
            .func = hf_cmd_tbl[dis].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&dis_cmd));

        const esp_console_cmd_t cona_cmd = {
            .command = "cona",
            .help = hf_cmd_explain[cona],
            .hint = NULL,
            .func = hf_cmd_tbl[cona].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&cona_cmd));

        const esp_console_cmd_t disa_cmd = {
            .command = "disa",
            .help = hf_cmd_explain[disa],
            .hint = NULL,
            .func = hf_cmd_tbl[disa].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&disa_cmd));

        const esp_console_cmd_t qop_cmd = {
            .command = "qop",
            .help = hf_cmd_explain[qop],
            .hint = NULL,
            .func = hf_cmd_tbl[qop].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&qop_cmd));

        const esp_console_cmd_t qc_cmd = {
            .command = "qc",
            .help = hf_cmd_explain[qc],
            .hint = NULL,
            .func = hf_cmd_tbl[qc].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&qc_cmd));

        const esp_console_cmd_t ac_cmd = {
            .command = "ac",
            .help = hf_cmd_explain[ac],
            .hint = NULL,
            .func = hf_cmd_tbl[ac].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&ac_cmd));

        const esp_console_cmd_t rc_cmd = {
            .command = "rc",
            .help = hf_cmd_explain[rc],
            .hint = NULL,
            .func = hf_cmd_tbl[rc].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rc_cmd));

        const esp_console_cmd_t d_cmd = {
            .command = "d",
            .help = hf_cmd_explain[d],
            .hint = "<num>",
            .func = hf_cmd_tbl[d].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&d_cmd));

        const esp_console_cmd_t rd_cmd = {
            .command = "rd",
            .help = hf_cmd_explain[rd],
            .hint = NULL,
            .func = hf_cmd_tbl[rd].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rd_cmd));

        const esp_console_cmd_t dm_cmd = {
            .command = "dm",
            .help = hf_cmd_explain[dm],
            .hint = "<index>",
            .func = hf_cmd_tbl[dm].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&dm_cmd));

        const esp_console_cmd_t vron_cmd = {
            .command = "vron",
            .help = hf_cmd_explain[vron],
            .hint = NULL,
            .func = hf_cmd_tbl[vron].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&vron_cmd));

        const esp_console_cmd_t vroff_cmd = {
            .command = "vroff",
            .help = hf_cmd_explain[vroff],
            .hint = NULL,
            .func = hf_cmd_tbl[vroff].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&vroff_cmd));

        vu_args.tgt = arg_str1(NULL, NULL, "<tgt>", "\n        0-speaker\n        1-microphone");
        vu_args.vol = arg_str1(NULL, NULL, "<vol>", "volume gain ranges from 0 to 15");
        vu_args.end = arg_end(1);
        const esp_console_cmd_t vu_cmd = {
            .command = "vu",
            .help = hf_cmd_explain[vu],
            .hint = NULL,
            .func = hf_cmd_tbl[vu].handler,
            .argtable = &vu_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&vu_cmd));

        const esp_console_cmd_t rs_cmd = {
            .command = "rs",
            .help = hf_cmd_explain[rs],
            .hint = NULL,
            .func = hf_cmd_tbl[rs].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rs_cmd));

        const esp_console_cmd_t rv_cmd = {
            .command = "rv",
            .help = hf_cmd_explain[rv],
            .hint = NULL,
            .func = hf_cmd_tbl[rv].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rv_cmd));

        rh_args.btrh = arg_str1(NULL, NULL, "<btrh>", "\n        0 - put call on hold,\n        1 - accept the held call,\n        2 -reject the held call");
        rh_args.end = arg_end(1);
        const esp_console_cmd_t rh_cmd = {
            .command = "rh",
            .help = hf_cmd_explain[rh],
            .hint = NULL,
            .func = hf_cmd_tbl[rh].handler,
            .argtable = &rh_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&rh_cmd));

        const esp_console_cmd_t k_cmd = {
            .command = "k",
            .help = hf_cmd_explain[k],
            .hint = "<dtmf>",
            .func = hf_cmd_tbl[k].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&k_cmd));

        const esp_console_cmd_t xp_cmd = {
            .command = "xp",
            .help = hf_cmd_explain[xp],
            .hint = NULL,
            .func = hf_cmd_tbl[xp].handler,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&xp_cmd));

        bat_args.bat_level = arg_int0(NULL, NULL, "<bat_level>", "battery level ranges from 0 to 9");
        bat_args.docked = arg_int0(NULL, NULL, "<docked>", "0 - undocked; 1 - docked");
        bat_args.end = arg_end(1);
        const esp_console_cmd_t bat_cmd = {
            .command = "bat",
            .help = hf_cmd_explain[bat],
            .hint = "<bat_level> <docked>",
            .func = hf_cmd_tbl[bat].handler,
            .argtable = &bat_args,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&bat_cmd));
}
