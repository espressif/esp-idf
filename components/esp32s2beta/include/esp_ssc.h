// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_SSC_H__
#define __ESP_SSC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_T_ASYNC   0x01
#define CMD_T_SYNC    0x02

typedef struct cmd_s {
    char *cmd_str;
    uint8_t flag;
    uint8_t id;
    void (* cmd_func)(void);
    void (* cmd_callback)(void *arg);
} ssc_cmd_t;

#define MAX_LINE_N  127

typedef enum {
    SSC_BR_9600   = 9600,
    SSC_BR_19200  = 19200,
    SSC_BR_38400  = 38400,
    SSC_BR_57600  = 57600,
    SSC_BR_74880  = 74880,
    SSC_BR_115200 = 115200,
    SSC_BR_230400 = 230400,
    SSC_BR_460800 = 460800,
    SSC_BR_921600 = 921600
} SscBaudRate;

/** \defgroup SSC_APIs SSC APIs
  * @brief SSC APIs
  *
  * SSC means simple serial command.
  * SSC APIs allows users to define their own command, users can refer to spiffs_test/test_main.c.
  *
  */

/** @addtogroup SSC_APIs
  * @{
  */

/**
  * @brief  Initial the ssc function.
  *
  * @attention param is no use, just compatible with ESP8266, default bandrate is 115200
  *
  * @param  SscBaudRate bandrate : baud rate
  *
  * @return null
  */
void ssc_attach(SscBaudRate bandrate);

/**
  * @brief   Get the length of the simple serial command.
  *
  * @param   null
  *
  * @return  length of the command.
  */
int ssc_param_len(void);

/**
  * @brief   Get the simple serial command string.
  *
  * @param   null
  *
  * @return  the command.
  */
char *ssc_param_str(void);

/**
  * @brief   Parse the simple serial command (ssc).
  *
  * @param   char *pLine  : [input] the ssc string
  * @param   char *argv[] : [output] parameters of the ssc
  *
  * @return  the number of parameters.
  */
int ssc_parse_param(char *pLine, char *argv[]);

/**
  * @brief  Register the user-defined simple serial command (ssc) set.
  *
  * @param  ssc_cmd_t *cmdset   : the ssc set
  * @param  uint8 cmdnum        : number of commands
  * @param  void (* help)(void) : callback of user-guide
  *
  * @return  null
  */
void ssc_register(ssc_cmd_t *cmdset, uint8_t cmdnum, void (* help)(void));

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SSC_H__ */
