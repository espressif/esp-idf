/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
# include <stdlib.h>
#include <string.h>
#include "esp_spp_api.h"
#include "esp_gap_bt_api.h"
#include "app_spp_msg_set.h"

extern esp_bd_addr_t peer_bd_addr;

void spp_msg_show_usage(void)
{
    printf("########################################################################\n");
    printf("Supported commands are as follows, arguments are embraced with < and >\n");
    printf("spp h;                -- show command manual\n\n");
    printf("Use this cmmand table if the IO Capability of local device set as IO_CAP_IO.\n");
    printf("spp ok;               -- manual Numeric Confirmation.\n\n");
    printf("Use this cmmand table if the IO Capability of local device set as IO_CAP_IN.\n");
    printf("spp key <auth key>;   -- manual Passkey. (e.g. spp key 136245;)\n\n");
    printf("########################################################################\n");
}

#define SPP_CMD_HANDLER(cmd)    static void spp_##cmd##_handler(int argn, char **argv)


SPP_CMD_HANDLER(help)
{
    spp_msg_show_usage();
}

SPP_CMD_HANDLER(ok)
{
    esp_bt_gap_ssp_confirm_reply(peer_bd_addr, true);
}

SPP_CMD_HANDLER(key)
{
    if (argn != 2) {
        printf("Insufficient number of arguments");
    } else {
        printf("Input Paring Key: %s\n", argv[1]);
        int passkey = atoi(argv[1]);
        esp_bt_gap_ssp_passkey_reply(peer_bd_addr, true, passkey);
    }
}

static spp_msg_hdl_t spp_cmd_tbl[] = {
    {0,    "h",   spp_help_handler},
    {5,    "ok",  spp_ok_handler},
    {10,   "key", spp_key_handler},
};

spp_msg_hdl_t *spp_get_cmd_tbl(void)
{
    return spp_cmd_tbl;
}

size_t spp_get_cmd_tbl_size(void)
{
    return sizeof(spp_cmd_tbl) / sizeof(spp_msg_hdl_t);
}
