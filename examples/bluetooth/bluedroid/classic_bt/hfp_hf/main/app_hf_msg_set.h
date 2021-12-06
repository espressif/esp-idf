/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __APP_HF_MSG_SET_H__
#define __APP_HF_MSG_SET_H__

#define HF_MSG_ARGS_MAX             (5)

typedef int (* hf_cmd_handler)(int argn, char **argv);

typedef struct {
    uint16_t opcode;
    const char *str;
    hf_cmd_handler handler;
} hf_msg_hdl_t;

extern hf_msg_hdl_t *hf_get_cmd_tbl(void);
extern size_t hf_get_cmd_tbl_size(void);

void hf_msg_show_usage(void);

void register_hfp_hf(void);
#endif /* __APP_HF_MSG_SET_H__*/
