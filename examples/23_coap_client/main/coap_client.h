/* CoAP client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef _COAP_CLIENT_H_
#define _COAP_CLIENT_H_

#include <sdkconfig.h>

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID            CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS            CONFIG_WIFI_PASSWORD

#define COAP_DEMO_THREAD_NAME        "CoAP_demo"
#define COAP_DEMO_THREAD_STACK_WORDS 10240
#define COAP_DEMO_THREAD_PRORIOTY    8

#define COAP_DEFAULT_TIME_SEC 5
#define COAP_DEFAULT_TIME_USEC 0

/* The examples use domain of "californium.eclipse.org",uri "coap://californium.eclipse.org" and port number of 5683 that
   you can set via 'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define COAP_DEFAULT_DEMO_ADDR "californium.eclipse.org"
   , ie #define COAP_DEFAULT_DEMO_URI "coap://californium.eclipse.org" and ie #define COAP_DEFAULT_PORT 5683
*/
#define COAP_DEFAULT_PORT CONFIG_TARGET_PORT_NUMBER
#define COAP_DEFAULT_DEMO_ADDR CONFIG_TARGET_DOMAIN
#define COAP_DEFAULT_DEMO_URI CONFIG_TARGET_DOMAIN_URI

#endif

