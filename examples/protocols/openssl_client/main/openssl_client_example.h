/* OpenSSL client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef _OPENSSL_EXAMPLE_H_
#define _OPENSSL_EXAMPLE_H_

/* The examples use domain of "www.baidu.com" and port number of 433 that
   you can set via the project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define OPENSSL_EXAMPLE_TARGET_NAME "www.baidu.com"
   and ie #define OPENSSL_EXAMPLE_TARGET_TCP_PORT 433
*/
#define EXAMPLE_OPENSSL_TARGET_DOMAIN CONFIG_EXAMPLE_OPENSSL_CLIENT_TARGET_DOMAIN
#define EXAMPLE_OPENSSL_TARGET_PORT    CONFIG_EXAMPLE_OPENSSL_CLIENT_TARGET_PORT

#define EXAMPLE_OPENSSL_REQUEST            "{\"path\": \"/v1/ping/\", \"method\": \"GET\"}\r\n"

#define EXAMPLE_OPENSSL_TASK_NAME        "openssl_example"
#define EXAMPLE_OPENSSL_TASK_STACK_WORDS 10240
#define EXAMPLE_OPENSSL_TASK_PRIORITY    8

#define EXAMPLE_OPENSSL_RECV_BUF_LEN       1024

#endif
