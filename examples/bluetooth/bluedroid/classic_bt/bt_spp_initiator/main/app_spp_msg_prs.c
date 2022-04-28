/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "app_spp_msg_prs.h"
#include "app_spp_msg_set.h"

#define SPP_MSG_HDR_LEN        (4)
const static char spp_msg_hdr[SPP_MSG_HDR_LEN] = {'s', 'p', 'p', ' '};
#define SPP_MSG_TAIL_LEN       (1)
const static char spp_msg_tail[SPP_MSG_TAIL_LEN] = {';'};

void spp_msg_parser_reset_state(spp_msg_prs_cb_t *prs)
{
    prs->state = SPP_MSG_PRS_IDLE;
    prs->cnt = 0;
    prs->h_idx = 0;
    prs->t_idx = 0;
}

void spp_msg_parser_register_callback(spp_msg_prs_cb_t *prs, spp_msg_callback cb)
{
    prs->callback = cb;
}

spp_msg_prs_err_t spp_msg_parse(char c, spp_msg_prs_cb_t *prs)
{
    spp_msg_prs_err_t err = SPP_MSG_PRS_ERR_IN_PROGRESS;
    switch (prs->state) {
        case SPP_MSG_PRS_IDLE:
        {
            if (c == spp_msg_hdr[0]) {
                prs->state = SPP_MSG_PRS_HDR;
                prs->buf[0] = c;
                prs->cnt = 1;
                prs->h_idx = 1;
            } else {
                err = SPP_MSG_PRS_ERR_HDR_UNDETECTED;
            }
            break;
        }

        case SPP_MSG_PRS_HDR:
        {
            if (c == spp_msg_hdr[prs->h_idx]) {
                prs->buf[prs->cnt++] = c;
                if (++(prs->h_idx) == SPP_MSG_HDR_LEN) {
                    prs->state = SPP_MSG_PRS_PAYL;
                    prs->t_idx = 0;
                }
            } else {
                spp_msg_parser_reset_state(prs);
                err = SPP_MSG_PRS_ERR_HDR_SYNC_FAILED;
            }
            break;
        }

        case SPP_MSG_PRS_PAYL:
        {
            prs->buf[prs->cnt++] = c;
            if (c == spp_msg_tail[prs->t_idx]) {
                if (++(prs->t_idx) == SPP_MSG_TAIL_LEN) {
                    prs->buf[prs->cnt] = '\0';
                    prs->callback(prs->buf, prs->cnt);
                    spp_msg_parser_reset_state(prs);
                    err = SPP_MSG_PRS_ERR_OK;
                    break;
                }
            } else {
                prs->t_idx = 0;
            }

            if (prs->cnt >= SPP_MSG_LEN_MAX-1) {
                spp_msg_parser_reset_state(prs);
                err = SPP_MSG_PRS_ERR_BUF_OVERFLOW;
            }
            break;
        }
    }
    return err;
}


void spp_msg_split_args(char *start, char *end, char **argv, int *argn)
{
    if (argn == NULL || *argn == 0) {
        return;
    }

    memset(argv, 0, (*argn) * sizeof(void *));

    int max_argn = *argn;
    *argn = 0;

    char *p = start;
    for (int i = 0; i < max_argn; ++i) {
        while (isspace((int)*p) && p != end) {
            ++p;
        }
        if (p == end) {
            return;
        }

        argv[i] = p++;
        ++(*argn);

        while (!isspace((int)*p) && p != end) {
            ++p;
        }

        if (p == end) {
            return;
        } else {
            *p = '\0';
            ++p;
        }
    }
}

void spp_msg_args_parser(char *buf, int len)
{
    char *argv[SPP_MSG_ARGS_MAX];
    int argn = SPP_MSG_ARGS_MAX;
    char *start = buf + SPP_MSG_HDR_LEN;
    // set the command terminator to '\0'
    char *end = buf + len - SPP_MSG_TAIL_LEN;
    *end = '\0';

    spp_msg_split_args(start, end, argv, &argn);

    if (argn == 0) {
        return;
    }

    bool cmd_supported = false;

    spp_msg_hdl_t *cmd_tbl = spp_get_cmd_tbl();
    size_t cmd_tbl_size = spp_get_cmd_tbl_size();
    for (int i = 0; i < cmd_tbl_size; ++i) {
        spp_msg_hdl_t *hdl = &cmd_tbl[i];
        if (strcmp(argv[0], hdl->str) == 0) {
            if (hdl->handler) {
                hdl->handler(argn, argv);
                cmd_supported = true;
                break;
            }
        }
    }
    if (!cmd_supported) {
        printf("unsupported command\n");
        spp_msg_show_usage();
    }
    return;
}
