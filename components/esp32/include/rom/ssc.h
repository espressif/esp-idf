// Copyright 2011-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ROM_SSC_H_
#define _ROM_SSC_H_

#include "esp_types.h"

#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cmd_s {
    char *cmd_str;
#define CMD_T_ASYNC   0x01
#define CMD_T_SYNC    0x02
    uint8_t flag;
    uint8_t id;
    void    (* cmd_func)(void);
    void    (* cmd_callback)(void *arg);
} ssc_cmd_t;

#define ssc_printf ets_printf
#define SSC_CMD_N  10   //since the command not added in ssc_cmd.c
#define MAX_LINE_N  40
#define PROMPT  ":>"
#define SSC_EVT_N   4


#define SSC_PRIO   30
enum {
    CMD_SET_SSID = 0,
    CMD_SCAN,
    CMD_CONNECT,
    CMD_DISCONNECT,
    CMD_SET_MACADDR,
    CMD_PING,
    CMD_PING_COUNT,
    CMD_PING_LENGTH,
    CMD_SET_IP,
//    CMD_RD_I2C, 
//    CMD_SET_NULL, it's just for solving the old rom bug 
//    CMD_SET_I2C,
//    CMD_RD_I2CM,
//    CMD_SET_I2CM,
//    CMD_SET_PBUS,
//    CMD_SET_TXTONE,
//    CMD_SET_STOPTONE,
    CMD_END,
};

enum {
    SIG_SSC_RUNCMD,
    SIG_SSC_CMDDONE,
    SIG_SSC_RESTART,
    SIG_SSC_UART_RX_CHAR,
};

void ssc_attach(void) ROMFN_ATTR;
void ssc_cmd_done(int cmd_id, STATUS status) ROMFN_ATTR;
int ssc_param_len(void) ROMFN_ATTR;
char * ssc_param_str(void) ROMFN_ATTR;
void ssc_register(ssc_cmd_t *cmdset, uint8_t cmdnum, void (* help)(void)) ROMFN_ATTR;

extern ssc_cmd_t sscCmdSet[];
void ssc_help(void);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_SSC_H_ */
