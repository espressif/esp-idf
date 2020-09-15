/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
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
