// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SMARTCONFIG_ACK_H
#define SMARTCONFIG_ACK_H

#ifdef __cplusplus
extern "C" {
#endif

#define SC_ACK_TASK_PRIORITY             2          /*!< Priority of sending smartconfig ACK task */
#define SC_ACK_TASK_STACK_SIZE           2048       /*!< Stack size of sending smartconfig ACK task */

#define SC_ACK_TOUCH_SERVER_PORT         18266      /*!< ESP touch UDP port of server on cellphone */
#define SC_ACK_AIRKISS_SERVER_PORT       10000      /*!< Airkiss UDP port of server on cellphone */
#define SC_ACK_AIRKISS_DEVICE_PORT       10001      /*!< Airkiss UDP port of server on device */
#define SC_ACK_AIRKISS_TIMEOUT           1500       /*!< Airkiss read data timeout millisecond */

#define SC_ACK_TOUCH_LEN                 11         /*!< Length of ESP touch ACK context */
#define SC_ACK_AIRKISS_LEN               7          /*!< Length of Airkiss ACK context */

#define SC_ACK_MAX_COUNT                 30         /*!< Maximum count of sending smartconfig ACK */

/**
 * @brief Smartconfig ACK type.
 */
typedef enum {
    SC_ACK_TYPE_ESPTOUCH = 0,                       /*!< ESP touch ACK type */
    SC_ACK_TYPE_AIRKISS,                            /*!< Airkiss ACK type */
} sc_ack_type_t;

/**
 * @brief Smartconfig parameters passed to sc_ack_send call.
 */
typedef struct sc_ack {
    sc_ack_type_t type;           /*!< Smartconfig ACK type */
    uint8_t *link_flag;           /*!< Smartconfig link flag */
    sc_callback_t cb;             /*!< Smartconfig callback function */
    struct {
        uint8_t token;            /*!< Smartconfig token to be sent */
        uint8_t mac[6];           /*!< MAC address of station */
        uint8_t ip[4];            /*!< IP address of cellphone */
    } ctx;
} sc_ack_t;

/**
  * @brief  Send smartconfig ACK to cellphone.
  *
  * @attention The API is only used in libsmartconfig.a.
  *
  * @param  param: smartconfig parameters;
  */
void sc_ack_send(sc_ack_t *param);

/**
  * @brief  Stop sending smartconfig ACK to cellphone.
  *
  * @attention The API is only used in libsmartconfig.a.
  */
void sc_ack_send_stop(void);

#ifdef __cplusplus
}
#endif
#endif
