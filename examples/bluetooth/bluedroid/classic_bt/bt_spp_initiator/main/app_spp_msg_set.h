/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __APP_SPP_MSG_SET_H__
#define __APP_SPP_MSG_SET_H__

#define SPP_MSG_ARGS_MAX             (5)

typedef void (* spp_cmd_handler)(int argn, char **argv);

typedef struct {
    uint16_t opcode;
    const char *str;
    spp_cmd_handler handler;
} spp_msg_hdl_t;

extern spp_msg_hdl_t *spp_get_cmd_tbl(void);
extern size_t spp_get_cmd_tbl_size(void);

void spp_msg_show_usage(void);
#endif /* __APP_SPP_MSG_SET_H__*/
