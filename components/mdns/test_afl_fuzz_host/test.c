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

#ifdef MDNS_TEST_MODE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "mdns.h"

void mdns_parse_packet(mdns_server_t * server, const uint8_t * data, size_t len);

int main(int argc, char** argv)
{
    const char * mdns_hostname = "minifritz";
    const char * mdns_instance = "Hristo's Time Capsule";
    const char * arduTxtData[4] = {
        "board=esp32",
        "tcp_check=no",
        "ssh_upload=no",
        "auth_upload=no"
    };
    const uint8_t mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x32};
    
    mdns_server_t * mdns = NULL;
    uint8_t buf[1460];
    char winstance[21+strlen(mdns_hostname)];

    sprintf(winstance, "%s [%02x:%02x:%02x:%02x:%02x:%02x]", mdns_hostname, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if (mdns_init(TCPIP_ADAPTER_IF_ETH, &mdns)) {
        abort();
    }

    if (mdns_set_hostname(mdns, mdns_hostname)) {
        abort();
    }

    if (mdns_set_instance(mdns, mdns_instance)) {
        abort();
    }

    if (mdns_service_add(mdns, "_workstation", "_tcp", 9)) {
        abort();
    }

    if (mdns_service_instance_set(mdns, "_workstation", "_tcp", winstance)) {
        abort();
    }

    if (mdns_service_add(mdns, "_arduino", "_tcp", 3232)) {
        abort();
    }

    if (mdns_service_txt_set(mdns, "_arduino", "_tcp", 4, arduTxtData)) {
        abort();
    }

    if (mdns_service_add(mdns, "_http", "_tcp", 80)) {
        abort();
    }

    if (mdns_service_instance_set(mdns, "_http", "_tcp", "ESP WebServer")) {
        abort();
    }

    if (
           mdns_service_add(mdns, "_afpovertcp", "_tcp", 548)
        || mdns_service_add(mdns, "_rfb", "_tcp", 885)
        || mdns_service_add(mdns, "_smb", "_tcp", 885)
        || mdns_service_add(mdns, "_adisk", "_tcp", 885)
        || mdns_service_add(mdns, "_airport", "_tcp", 885)
        || mdns_service_add(mdns, "_printer", "_tcp", 885)
        || mdns_service_add(mdns, "_airplay", "_tcp", 885)
        || mdns_service_add(mdns, "_raop", "_tcp", 885)
        || mdns_service_add(mdns, "_uscan", "_tcp", 885)
        || mdns_service_add(mdns, "_uscans", "_tcp", 885)
        || mdns_service_add(mdns, "_ippusb", "_tcp", 885)
        || mdns_service_add(mdns, "_scanner", "_tcp", 885)
        || mdns_service_add(mdns, "_ipp", "_tcp", 885)
        || mdns_service_add(mdns, "_ipps", "_tcp", 885)
        || mdns_service_add(mdns, "_pdl-datastream", "_tcp", 885)
        || mdns_service_add(mdns, "_ptp", "_tcp", 885)
        || mdns_service_add(mdns, "_sleep-proxy", "_udp", 885))
    {
        abort();
    }

    while (__AFL_LOOP(1000)) {
        memset(buf, 0, 1460);
        size_t len = read(0, buf, 1460);
        mdns_query(mdns, "_afpovertcp", "_tcp", 0);
        mdns_parse_packet(mdns, buf, len);
        mdns_query_end(mdns);
    }
    return 0;
}

#endif
