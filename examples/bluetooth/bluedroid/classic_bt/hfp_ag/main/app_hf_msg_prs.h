/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __APP_HF_MSG_PRS_H__
#define __APP_HF_MSG_PRS_H__

typedef enum {
    HF_MSG_PRS_ERR_OK = 0,          // a complete message is finished
    HF_MSG_PRS_ERR_IN_PROGRESS,     // message parsing is in progress
    HF_MSG_PRS_ERR_HDR_UNDETECTED,  // header not detected
    HF_MSG_PRS_ERR_HDR_SYNC_FAILED, // failed to sync header
    HF_MSG_PRS_ERR_BUF_OVERFLOW,    // exceeds the buffer size: HF_MSG_LEN_MAX
} hf_msg_prs_err_t;

typedef enum {
    HF_MSG_PRS_IDLE = 0,
    HF_MSG_PRS_HDR,
    HF_MSG_PRS_PAYL,
} hf_msg_prs_state_t;

typedef void (*hf_msg_callback)(char *buf, int len);

#define HF_MSG_LEN_MAX        (128)

typedef struct {
    hf_msg_prs_state_t state;
    char buf[HF_MSG_LEN_MAX + 1];
    int cnt;
    int h_idx;
    int t_idx;
    hf_msg_callback callback;
} hf_msg_prs_cb_t;

void hf_msg_parser_reset_state(hf_msg_prs_cb_t *prs);

void hf_msg_parser_register_callback(hf_msg_prs_cb_t *prs, hf_msg_callback cb);

hf_msg_prs_err_t hf_msg_parse(char c, hf_msg_prs_cb_t *prs);

void hf_msg_show_usage(void);

#endif /* __APP_HF_MSG_PRS_H__*/

