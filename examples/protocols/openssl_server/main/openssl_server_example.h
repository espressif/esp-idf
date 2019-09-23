/* OpenSSL server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef _OPENSSL_SERVER_H_
#define _OPENSSL_SERVER_H_

#include "sdkconfig.h"

#define OPENSSL_EXAMPLE_TASK_NAME        "openssl_example"
#define OPENSSL_EXAMPLE_TASK_STACK_WORDS 10240
#define OPENSSL_EXAMPLE_TASK_PRIORITY    8

#define OPENSSL_EXAMPLE_RECV_BUF_LEN       1024

#define OPENSSL_EXAMPLE_LOCAL_TCP_PORT     443

#endif

