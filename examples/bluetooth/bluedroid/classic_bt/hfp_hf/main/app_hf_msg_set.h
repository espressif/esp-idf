/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __APP_HF_MSG_SET_H__
#define __APP_HF_MSG_SET_H__

#define HF_MSG_ARGS_MAX             (5)

typedef void (* hf_cmd_handler)(int argn, char **argv);

typedef struct {
    uint16_t opcode;
    const char *str;
    hf_cmd_handler handler;
} hf_msg_hdl_t;

extern hf_msg_hdl_t *hf_get_cmd_tbl(void);
extern size_t hf_get_cmd_tbl_size(void);

void hf_msg_show_usage(void);
#endif /* __APP_HF_MSG_SET_H__*/
