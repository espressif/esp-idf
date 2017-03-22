/* OpenSSL client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef _OPENSSL_EXAMPLE_H_
#define _OPENSSL_EXAMPLE_H_

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID               CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS               CONFIG_WIFI_PASSWORD

/* The examples use domain of "www.baidu.com" and port number of 433 that 
   you can set via 'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define OPENSSL_EXAMPLE_TARGET_NAME "www.baidu.com"
   and ie #define OPENSSL_EXAMPLE_TARGET_TCP_PORT 433 
*/
#define OPENSSL_EXAMPLE_TARGET_NAME        CONFIG_TARGET_DOMAIN
#define OPENSSL_EXAMPLE_TARGET_TCP_PORT    CONFIG_TARGET_PORT_NUMBER

#define OPENSSL_EXAMPLE_REQUEST            "{\"path\": \"/v1/ping/\", \"method\": \"GET\"}\r\n"

#define OPENSSL_EXAMPLE_TASK_NAME        "openssl_example"
#define OPENSSL_EXAMPLE_TASK_STACK_WORDS 10240
#define OPENSSL_EXAMPLE_TASK_PRORIOTY    8

#define OPENSSL_EXAMPLE_RECV_BUF_LEN       1024

#define OPENSSL_EXAMPLE_LOCAL_TCP_PORT     443

#endif

