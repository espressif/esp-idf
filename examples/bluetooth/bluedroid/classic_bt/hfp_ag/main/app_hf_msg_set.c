/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "esp_hf_ag_api.h"
#include "app_hf_msg_set.h"
#include "bt_app_hf.h"

// if you want to connect a specific device, add it's bda here
// esp_bd_addr_t hf_peer_addr = {0x70,0x26,0x05,0xca,0xeb,0x21};

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

#define HF_CMD_HANDLER(cmd)    static void hf_##cmd##_handler(int argn, char **argv)

HF_CMD_HANDLER(help)
{
    hf_msg_show_usage();
}

HF_CMD_HANDLER(conn)
{
    printf("Connect.\n");
    esp_bt_hf_connect(hf_peer_addr);
}

HF_CMD_HANDLER(disc)
{
    printf("Disconnect\n");
    esp_bt_hf_disconnect(hf_peer_addr);
}

HF_CMD_HANDLER(conn_audio)
{
    printf("Connect Audio\n");
    esp_bt_hf_connect_audio(hf_peer_addr);
}

HF_CMD_HANDLER(disc_audio)
{
    printf("Disconnect Audio\n");
    esp_bt_hf_disconnect_audio(hf_peer_addr);
}

//AT+BVRA
HF_CMD_HANDLER(vra_on)
{
    printf("Start Voice Recognition.\n");
    esp_bt_hf_vra(hf_peer_addr,1);
}
//AT+BVRA
HF_CMD_HANDLER(vra_off)
{
    printf("Stop Voicer Recognition.\n");
    esp_bt_hf_vra(hf_peer_addr,0);
}

//AT+VGS or AT+VGM
HF_CMD_HANDLER(volume_control)
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
    printf("Volume Update\n");
    esp_bt_hf_volume_control(hf_peer_addr, target, volume);
}

//+CIEV
HF_CMD_HANDLER(ind_change)
{
    if (argn != 5) {
        printf("Insufficient number of arguments");
        return;
    }

    int call_state, ntk_state, call_setup_state, signal;

    if (sscanf(argv[1], "%d", &call_state) != 1 ||
        (call_state != ESP_HF_CALL_STATUS_NO_CALLS &&
        call_state != ESP_HF_CALL_STATUS_CALL_IN_PROGRESS)) {
        printf("Invalid argument for call state %s\n", argv[1]);
        return;
    }
    if (sscanf(argv[2], "%d", &call_setup_state) != 1 ||
        (call_setup_state < ESP_HF_CALL_SETUP_STATUS_IDLE || call_setup_state > ESP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING)) {
        printf("Invalid argument for callsetup state %s\n", argv[2]);
        return;
    }
    if (sscanf(argv[3], "%d", &ntk_state) != 1 ||
        (ntk_state != ESP_HF_NETWORK_STATE_NOT_AVAILABLE &&
        ntk_state != ESP_HF_NETWORK_STATE_AVAILABLE)) {
        printf("Invalid argument for netwrok state %s\n", argv[3]);
        return;
    }
    if (sscanf(argv[4], "%d", &signal) != 1 ||
            (signal < 0 || signal > 5)) {
        printf("Invalid argument for signal %s\n", argv[4]);
        return;
    }
    printf("Device Indicator Changed!\n");
    esp_bt_hf_indchange_notification(hf_peer_addr, call_state, call_setup_state, ntk_state, signal);
}

//AT+CMEE
HF_CMD_HANDLER(cme_err)
{
    if (argn != 3) {
        printf("Insufficient number of arguments");
        return;
    }

    int response_code, error_code;
    if (sscanf(argv[1], "%d", &response_code) != 1 ||
        (response_code < ESP_HF_AT_RESPONSE_CODE_OK && response_code > ESP_HF_AT_RESPONSE_CODE_CME)) {
        printf("Invalid argument for response_code %s\n", argv[1]);
        return;
    }

    if (sscanf(argv[2], "%d", &error_code) != 1 ||
            (error_code < ESP_HF_CME_AG_FAILURE || error_code > ESP_HF_CME_NETWORK_NOT_ALLOWED)) {
        printf("Invalid argument for volume %s\n", argv[2]);
        return;
    }

    printf("Send CME Error.\n");
    esp_bt_hf_cmee_response(hf_peer_addr,response_code,error_code);
}

//+BSIR:1
HF_CMD_HANDLER(ir_on)
{
    printf("Enable Voicer Recognition.\n");
    esp_bt_hf_bsir(hf_peer_addr,1);
}

//+BSIR:0
HF_CMD_HANDLER(ir_off)
{
    printf("Disable Voicer Recognition.\n");
    esp_bt_hf_bsir(hf_peer_addr,0);
}

//Answer Call from AG
HF_CMD_HANDLER(ac)
{
    printf("Answer Call from AG.\n");
    char *number = {"123456"};
    esp_bt_hf_answer_call(hf_peer_addr,1,0,1,1,number,0);
}

//Reject Call from AG
HF_CMD_HANDLER(rc)
{
    printf("Reject Call from AG.\n");
    char *number = {"123456"};
    esp_bt_hf_reject_call(hf_peer_addr,0,0,0,0,number,0);
}

//End Call from AG
HF_CMD_HANDLER(end)
{
    printf("End Call from AG.\n");
    char *number = {"123456"};
    esp_bt_hf_end_call(hf_peer_addr,0,0,0,0,number,0);
}

//Dial Call from AG
HF_CMD_HANDLER(d)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
    } else {
        printf("Dial number %s\n", argv[1]);
        esp_bt_hf_out_call(hf_peer_addr,1,0,1,2,argv[1],0);
    }
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
