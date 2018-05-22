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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "mdns.h"
#include "mdns_private.h"

//
// Global stuctures containing packet payload, search
mdns_rx_packet_t g_packet;
struct pbuf mypbuf;
mdns_search_once_t * search = NULL;

//
// Dependency injected test functions
void mdns_test_execute_action(void * action);
mdns_srv_item_t * mdns_test_mdns_get_service_item(const char * service, const char * proto);
mdns_search_once_t * mdns_test_search_init(const char * name, const char * service, const char * proto, uint16_t type, uint32_t timeout, uint8_t max_results);
esp_err_t mdns_test_send_search_action(mdns_action_type_t type, mdns_search_once_t * search);
void mdns_test_search_free(mdns_search_once_t * search);
void mdns_test_init_di();

//
// mdns function wrappers for mdns setup in test mode
static int mdns_test_hostname_set(const char * mdns_hostname)
{
    int ret = mdns_hostname_set(mdns_hostname);
    mdns_action_t * a = NULL;
    GetLastItem(&a);
    mdns_test_execute_action(a);
    return ret;
}

static int mdns_test_service_instance_name_set(const char * service, const char * proto, const char * instance)
{
    int ret = mdns_service_instance_name_set(service, proto, instance);
    mdns_action_t * a = NULL;
    GetLastItem(&a);
    mdns_test_execute_action(a);
    return ret;
}

static int mdns_test_service_txt_set(const char * service, const char * proto,  uint8_t num_items, mdns_txt_item_t txt[])
{
    int ret = mdns_service_txt_set(service, proto, txt, num_items);
    mdns_action_t * a = NULL;
    GetLastItem(&a);
    mdns_test_execute_action(a);
    return ret;
}

static int mdns_test_service_add(const char * service_name, const char * proto, uint32_t port)
{
    if (mdns_service_add(NULL, service_name, proto, port, NULL, 0)) {
        // This is expected failure as the service thread is not running
    }
    mdns_action_t * a = NULL;
    GetLastItem(&a);
    mdns_test_execute_action(a);

    if (mdns_test_mdns_get_service_item(service_name, proto)==NULL) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static mdns_result_t* mdns_test_query(const char * service_name, const char * proto)
{
    search = mdns_test_search_init(NULL, service_name, proto, MDNS_TYPE_PTR, 3000, 20);
    if (!search) {
        abort();
    }

    if (mdns_test_send_search_action(ACTION_SEARCH_ADD, search)) {
        mdns_test_search_free(search);
        abort();
    }

    mdns_action_t * a = NULL;
    GetLastItem(&a);
    mdns_test_execute_action(a);
    return NULL;
}

static void mdns_test_query_free()
{
    mdns_test_search_free(search);
}

//
// function "under test" where afl-mangled packets passed
//
void mdns_parse_packet(mdns_rx_packet_t * packet);

//
// Test starts here
//
int main(int argc, char** argv)
{
    int i;
    const char * mdns_hostname = "minifritz";
    const char * mdns_instance = "Hristo's Time Capsule";
    mdns_txt_item_t arduTxtData[4] = {
        {"board","esp32"},
        {"tcp_check","no"},
        {"ssh_upload","no"},
        {"auth_upload","no"}
    };

    const uint8_t mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x32};
    
    uint8_t buf[1460];
    char winstance[21+strlen(mdns_hostname)];

    sprintf(winstance, "%s [%02x:%02x:%02x:%02x:%02x:%02x]", mdns_hostname, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Init depencency injected methods
    mdns_test_init_di();

    if (mdns_init()) {
        abort();
    }

    if (mdns_test_hostname_set(mdns_hostname)) {
        abort();
    }

    if (mdns_test_service_add("_workstation", "_tcp", 9)) {
        abort();
    }
    if (mdns_test_service_instance_name_set("_workstation", "_tcp", winstance)) {
        abort();
    }

    if (mdns_test_service_add("_arduino", "_tcp", 3232)) {
        abort();
    }

    if (mdns_test_service_txt_set("_arduino", "_tcp", 4, arduTxtData)) {
        abort();
    }

    if (mdns_test_service_add("_http", "_tcp", 80)) {
        abort();
    }

    if (mdns_test_service_instance_name_set("_http", "_tcp", "ESP WebServer")) {
        abort();
    }

    if (
           mdns_test_service_add("_afpovertcp", "_tcp", 548)
        || mdns_test_service_add("_rfb", "_tcp", 885)
        || mdns_test_service_add("_smb", "_tcp", 885)
        || mdns_test_service_add("_adisk", "_tcp", 885)
        || mdns_test_service_add("_airport", "_tcp", 885)
        || mdns_test_service_add("_printer", "_tcp", 885)
        || mdns_test_service_add("_airplay", "_tcp", 885)
        || mdns_test_service_add("_raop", "_tcp", 885)
        || mdns_test_service_add("_uscan", "_tcp", 885)
        || mdns_test_service_add("_uscans", "_tcp", 885)
        || mdns_test_service_add("_ippusb", "_tcp", 885)
        || mdns_test_service_add("_scanner", "_tcp", 885)
        || mdns_test_service_add("_ipp", "_tcp", 885)
        || mdns_test_service_add("_ipps", "_tcp", 885)
        || mdns_test_service_add("_pdl-datastream", "_tcp", 885)
        || mdns_test_service_add("_ptp", "_tcp", 885)
        || mdns_test_service_add("_sleep-proxy", "_udp", 885))
    {
        abort(); 
    }
        
    mdns_result_t * results = NULL;
    FILE *file;
    size_t nread;

#ifdef INSTR_IS_OFF
    size_t len = 1460;
    memset(buf, 0, 1460);

    if (argc != 2)
    {
        printf("Non-instrumentation mode: please supply a file name created by AFL to reproduce crash\n");
        return 1;
    }
    else
    {
        //
        // Note: parameter1 is a file (mangled packet) which caused the crash
        file = fopen(argv[1], "r");
        if (file) {
            len = fread(buf, 1, 1460, file);
        }
        fclose(file);
    }

    for (i=0; i<1; i++) {
#else
    while (__AFL_LOOP(1000)) {
        memset(buf, 0, 1460);
        size_t len = read(0, buf, 1460);
#endif
        mypbuf.payload = buf;
        mypbuf.len = len;
        g_packet.pb = &mypbuf;
        mdns_test_query("_afpovertcp", "_tcp");
        mdns_parse_packet(&g_packet);
    }
    ForceTaskDelete();
    mdns_free();
    return 0;
}
