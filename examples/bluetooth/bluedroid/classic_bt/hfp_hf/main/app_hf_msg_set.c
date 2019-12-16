/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "esp_hf_client_api.h"
#include "app_hf_msg_set.h"

extern esp_bd_addr_t peer_addr;

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
    printf("hf h;              -- show command manual\n");
    printf("########################################################################\n");
}

#define HF_CMD_HANDLER(cmd)    static void hf_##cmd##_handler(int argn, char **argv)


HF_CMD_HANDLER(help)
{
    hf_msg_show_usage();
}

HF_CMD_HANDLER(conn)
{
    printf("connect\n");
    esp_hf_client_connect(peer_addr);
}

HF_CMD_HANDLER(disc)
{
    printf("disconnect\n");
    esp_hf_client_disconnect(peer_addr);
}

HF_CMD_HANDLER(conn_audio)
{
    printf("connect audio\n");
    esp_hf_client_connect_audio(peer_addr);
}

HF_CMD_HANDLER(disc_audio)
{
    printf("disconnect audio\n");
    esp_hf_client_disconnect_audio(peer_addr);
}

HF_CMD_HANDLER(query_op)
{
    printf("Query operator\n");
    esp_hf_client_query_current_operator_name();
}

HF_CMD_HANDLER(answer)
{
    printf("Answer call\n");
    esp_hf_client_answer_call();
}
HF_CMD_HANDLER(reject)
{
    printf("Reject call\n");
    esp_hf_client_reject_call();
}

HF_CMD_HANDLER(dial)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
    } else {
        printf("Dial number %s\n", argv[1]);
        esp_hf_client_dial(argv[1]);
    }
}

HF_CMD_HANDLER(redial)
{
    printf("Dial number\n");
    esp_hf_client_dial(NULL);
}

HF_CMD_HANDLER(dial_mem)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
        return;
    }
    int index;
    if (sscanf(argv[1], "%d", &index) != 1) {
        printf("Invalid argument %s\n", argv[1]);
        return;
    }

    printf("Dial memory %d\n", index);
    esp_hf_client_dial_memory(index);
}


HF_CMD_HANDLER(start_vr)
{
    printf("Start voice recognition\n");
    esp_hf_client_start_voice_recognition();
}

HF_CMD_HANDLER(stop_vr)
{
    printf("Stop voice recognition\n");
    esp_hf_client_stop_voice_recognition();
}

HF_CMD_HANDLER(volume_update)
{
    if (argn != 3) {
        printf("Insufficient number of arguments");
        return;
    }
    int target, volume;
    if (sscanf(argv[1], "%d", &target) != 1 ||
            (target != ESP_HF_VOLUME_CONTROL_TARGET_SPK &&
             target != ESP_HF_VOLUME_CONTROL_TARGET_MIC)) {
        printf("Invalid argument for target %s\n", argv[1]);
        return;
    }

    if (sscanf(argv[2], "%d", &volume) != 1 ||
            (volume < 0 || volume > 15)) {
        printf("Invalid argument for volume %s\n", argv[2]);
        return;
    }

    printf("volume update\n");
    esp_hf_client_volume_update(target, volume);
}

HF_CMD_HANDLER(query_call)
{
    printf("Query current call status\n");
    esp_hf_client_query_current_calls();
}

HF_CMD_HANDLER(retrieve_subscriber)
{
    printf("Retrieve subscriber information\n");
    esp_hf_client_retrieve_subscriber_info();
}

HF_CMD_HANDLER(request_last_voice_tag)
{
    printf("Request last voice tag\n");
    esp_hf_client_request_last_voice_tag_number();
}

HF_CMD_HANDLER(btrh)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
        return;
    }

    int btrh;
    if (sscanf(argv[1], "%d", &btrh) != 1) {
        printf("Invalid argument %s\n", argv[1]);
        return;
    }
    if (btrh < ESP_HF_BTRH_CMD_HOLD || btrh > ESP_HF_BTRH_CMD_REJECT) {
        printf("Invalid argument %s\n", argv[1]);
        return;
    }
    printf("respond and hold command: %d\n", btrh);
    esp_hf_client_send_btrh_cmd(btrh);
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
        return;
    }

    if (strlen(argv[1]) != 1 || !is_dtmf_code(argv[1][0])) {
        printf("Invalid argument %s\n", argv[1]);
        return;
    }

    printf("send dtmf code: %s\n", argv[1]);
    esp_hf_client_send_dtmf(argv[1][0]);
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
};

hf_msg_hdl_t *hf_get_cmd_tbl(void)
{
    return hf_cmd_tbl;
}

size_t hf_get_cmd_tbl_size(void)
{
    return sizeof(hf_cmd_tbl) / sizeof(hf_msg_hdl_t);
}
