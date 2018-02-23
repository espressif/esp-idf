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
#include <string.h>
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "mdns.h"

static const char * if_str[] = {"STA", "AP", "ETH", "MAX"};
static const char * ip_protocol_str[] = {"V4", "V6", "MAX"};

static void mdns_print_results(mdns_result_t * results)
{
    mdns_result_t * r = results;
    mdns_ip_addr_t * a = NULL;
    int i = 1, t;
    while (r) {
        printf("%d: Interface: %s, Type: %s\n", i++, if_str[r->tcpip_if], ip_protocol_str[r->ip_protocol]);
        if (r->instance_name) {
            printf("  PTR : %s\n", r->instance_name);
        }
        if (r->hostname) {
            printf("  SRV : %s.local:%u\n", r->hostname, r->port);
        }
        if (r->txt_count) {
            printf("  TXT : [%u] ", r->txt_count);
            for (t=0; t<r->txt_count; t++) {
                printf("%s=%s; ", r->txt[t].key, r->txt[t].value);
            }
            printf("\n");
        }
        a = r->addr;
        while (a) {
            if (a->addr.type == IPADDR_TYPE_V6) {
                printf("  AAAA: " IPV6STR "\n", IPV62STR(a->addr.u_addr.ip6));
            } else {
                printf("  A   : " IPSTR "\n", IP2STR(&(a->addr.u_addr.ip4)));
            }
            a = a->next;
        }
        r = r->next;
    }
}

static struct {
    struct arg_str *hostname;
    struct arg_int *timeout;
    struct arg_end *end;
} mdns_query_a_args;

static int cmd_mdns_query_a(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_query_a_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_query_a_args.end, argv[0]);
        return 1;
    }

    const char * hostname = mdns_query_a_args.hostname->sval[0];
    int timeout = mdns_query_a_args.timeout->ival[0];

    if (!hostname || !hostname[0]) {
        printf("ERROR: Hostname not supplied\n");
        return 1;
    }

    if (timeout <= 0) {
        timeout = 1000;
    }

    printf("Query A: %s.local, Timeout: %d\n", hostname, timeout);

    struct ip4_addr addr;
    addr.addr = 0;

    esp_err_t err = mdns_query_a(hostname, timeout,  &addr);
    if (err) {
        if (err == ESP_ERR_NOT_FOUND) {
            printf("ERROR: Host was not found!\n");
            return 0;
        }
        printf("ERROR: Query Failed\n");
        return 1;
    }

    printf(IPSTR "\n", IP2STR(&addr));

    return 0;
}

static void register_mdns_query_a()
{
    mdns_query_a_args.hostname = arg_str1(NULL, NULL, "<hostname>", "Hostname that is searched for");
    mdns_query_a_args.timeout = arg_int0("t", "timeout", "<timeout>", "Timeout for this query");
    mdns_query_a_args.end = arg_end(2);

    const esp_console_cmd_t cmd_init = {
        .command = "mdns_query_a",
        .help = "Query MDNS for IPv4",
        .hint = NULL,
        .func = &cmd_mdns_query_a,
        .argtable = &mdns_query_a_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_init) );
}

static int cmd_mdns_query_aaaa(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_query_a_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_query_a_args.end, argv[0]);
        return 1;
    }

    const char * hostname = mdns_query_a_args.hostname->sval[0];
    int timeout = mdns_query_a_args.timeout->ival[0];

    if (!hostname || !hostname[0]) {
        printf("ERROR: Hostname not supplied\n");
        return 1;
    }

    if (timeout <= 0) {
        timeout = 1000;
    }

    printf("Query AAAA: %s.local, Timeout: %d\n", hostname, timeout);

    struct ip6_addr addr;
    memset(addr.addr, 0, 16);

    esp_err_t err = mdns_query_aaaa(hostname, timeout,  &addr);
    if (err) {
        if (err == ESP_ERR_NOT_FOUND) {
            printf("Host was not found!\n");
            return 0;
        }
        printf("ERROR: Query Failed\n");
        return 1;
    }

    printf(IPV6STR "\n", IPV62STR(addr));

    return 0;
}

static void register_mdns_query_aaaa()
{
    mdns_query_a_args.hostname = arg_str1(NULL, NULL, "<hostname>", "Hostname that is searched for");
    mdns_query_a_args.timeout = arg_int0("t", "timeout", "<timeout>", "Timeout for this query");
    mdns_query_a_args.end = arg_end(2);

    const esp_console_cmd_t cmd_init = {
        .command = "mdns_query_aaaa",
        .help = "Query MDNS for IPv6",
        .hint = NULL,
        .func = &cmd_mdns_query_aaaa,
        .argtable = &mdns_query_a_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_init) );
}

static struct {
    struct arg_str *instance;
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_int *timeout;
    struct arg_end *end;
} mdns_query_srv_args;

static int cmd_mdns_query_srv(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_query_srv_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_query_srv_args.end, argv[0]);
        return 1;
    }

    const char * instance = mdns_query_srv_args.instance->sval[0];
    const char * service = mdns_query_srv_args.service->sval[0];
    const char * proto = mdns_query_srv_args.proto->sval[0];
    int timeout = mdns_query_srv_args.timeout->ival[0];

    if (timeout <= 0) {
        timeout = 1000;
    }

    printf("Query SRV: %s.%s.%s.local, Timeout: %d\n", instance, service, proto, timeout);

    mdns_result_t * results = NULL;
    esp_err_t err = mdns_query_srv(instance, service, proto, timeout,  &results);
    if (err) {
        printf("ERROR: Query Failed\n");
        return 1;
    }
    if (!results) {
        printf("No results found!\n");
        return 0;
    }
    mdns_print_results(results);
    mdns_query_results_free(results);
    return 0;
}

static void register_mdns_query_srv()
{
    mdns_query_srv_args.instance = arg_str1(NULL, NULL, "<instance>", "Instance to search for");
    mdns_query_srv_args.service = arg_str1(NULL, NULL, "<service>", "Service to search for (ex. _http, _smb, etc.)");
    mdns_query_srv_args.proto = arg_str1(NULL, NULL, "<proto>", "Protocol to search for (_tcp, _udp, etc.)");
    mdns_query_srv_args.timeout = arg_int0("t", "timeout", "<timeout>", "Timeout for this query");
    mdns_query_srv_args.end = arg_end(2);

    const esp_console_cmd_t cmd_init = {
        .command = "mdns_query_srv",
        .help = "Query MDNS for Service SRV",
        .hint = NULL,
        .func = &cmd_mdns_query_srv,
        .argtable = &mdns_query_srv_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_init) );
}

static struct {
    struct arg_str *instance;
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_int *timeout;
    struct arg_end *end;
} mdns_query_txt_args;

static int cmd_mdns_query_txt(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_query_txt_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_query_txt_args.end, argv[0]);
        return 1;
    }

    const char * instance = mdns_query_txt_args.instance->sval[0];
    const char * service = mdns_query_txt_args.service->sval[0];
    const char * proto = mdns_query_txt_args.proto->sval[0];
    int timeout = mdns_query_txt_args.timeout->ival[0];

    printf("Query TXT: %s.%s.%s.local, Timeout: %d\n", instance, service, proto, timeout);

    if (timeout <= 0) {
        timeout = 5000;
    }

    mdns_result_t * results = NULL;
    esp_err_t err = mdns_query_txt(instance, service, proto, timeout,  &results);
    if (err) {
        printf("ERROR: Query Failed\n");
        return 1;
    }
    if (!results) {
        printf("No results found!\n");
        return 0;
    }

    mdns_print_results(results);
    mdns_query_results_free(results);
    return 0;
}

static void register_mdns_query_txt()
{
    mdns_query_txt_args.instance = arg_str1(NULL, NULL, "<instance>", "Instance to search for");
    mdns_query_txt_args.service = arg_str1(NULL, NULL, "<service>", "Service to search for (ex. _http, _smb, etc.)");
    mdns_query_txt_args.proto = arg_str1(NULL, NULL, "<proto>", "Protocol to search for (_tcp, _udp, etc.)");
    mdns_query_txt_args.timeout = arg_int0("t", "timeout", "<timeout>", "Timeout for this query");
    mdns_query_txt_args.end = arg_end(2);

    const esp_console_cmd_t cmd_init = {
        .command = "mdns_query_txt",
        .help = "Query MDNS for Service TXT",
        .hint = NULL,
        .func = &cmd_mdns_query_txt,
        .argtable = &mdns_query_txt_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_init) );
}

static struct {
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_int *timeout;
    struct arg_int *max_results;
    struct arg_end *end;
} mdns_query_ptr_args;

static int cmd_mdns_query_ptr(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_query_ptr_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_query_ptr_args.end, argv[0]);
        return 1;
    }

    const char * service = mdns_query_ptr_args.service->sval[0];
    const char * proto = mdns_query_ptr_args.proto->sval[0];
    int timeout = mdns_query_ptr_args.timeout->ival[0];
    int max_results = mdns_query_ptr_args.max_results->ival[0];

    if (timeout <= 0) {
        timeout = 5000;
    }

    if (max_results <= 0 || max_results > 255) {
        max_results = 255;
    }

    printf("Query PTR: %s.%s.local, Timeout: %d, Max Results: %d\n", service, proto, timeout, max_results);

    mdns_result_t * results = NULL;
    esp_err_t err = mdns_query_ptr(service, proto, timeout, max_results,  &results);
    if (err) {
        printf("ERROR: Query Failed\n");
        return 1;
    }
    if (!results) {
        printf("No results found!\n");
        return 0;
    }

    mdns_print_results(results);
    mdns_query_results_free(results);
    return 0;
}

static void register_mdns_query_ptr()
{
    mdns_query_ptr_args.service = arg_str1(NULL, NULL, "<service>", "Service to search for (ex. _http, _smb, etc.)");
    mdns_query_ptr_args.proto = arg_str1(NULL, NULL, "<proto>", "Protocol to search for (_tcp, _udp, etc.)");
    mdns_query_ptr_args.timeout = arg_int0("t", "timeout", "<timeout>", "Timeout for this query");
    mdns_query_ptr_args.max_results = arg_int0("m", "max_results", "<max_results>", "Maximum results returned");
    mdns_query_ptr_args.end = arg_end(2);

    const esp_console_cmd_t cmd_init = {
        .command = "mdns_query_ptr",
        .help = "Query MDNS for Service",
        .hint = NULL,
        .func = &cmd_mdns_query_ptr,
        .argtable = &mdns_query_ptr_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_init) );
}

static struct {
    struct arg_str *hostname;
    struct arg_int *timeout;
    struct arg_int *max_results;
    struct arg_end *end;
} mdns_query_ip_args;

static int cmd_mdns_query_ip(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_query_ip_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_query_ip_args.end, argv[0]);
        return 1;
    }

    const char * hostname = mdns_query_ip_args.hostname->sval[0];
    int timeout = mdns_query_ip_args.timeout->ival[0];
    int max_results = mdns_query_ip_args.max_results->ival[0];

    if (!hostname || !hostname[0]) {
        printf("ERROR: Hostname not supplied\n");
        return 1;
    }

    if (timeout <= 0) {
        timeout = 1000;
    }

    if (max_results < 0 || max_results > 255) {
        max_results = 255;
    }

    printf("Query IP: %s.local, Timeout: %d, Max Results: %d\n", hostname, timeout, max_results);

    mdns_result_t * results = NULL;
    esp_err_t err = mdns_query(hostname, NULL, NULL, MDNS_TYPE_ANY, timeout, max_results, &results);
    if (err) {
        printf("ERROR: Query Failed\n");
        return 1;
    }
    if (!results) {
        printf("No results found!\n");
        return 0;
    }
    mdns_print_results(results);
    mdns_query_results_free(results);

    return 0;
}

static void register_mdns_query_ip()
{
    mdns_query_ip_args.hostname = arg_str1(NULL, NULL, "<hostname>", "Hostname that is searched for");
    mdns_query_ip_args.timeout = arg_int0("t", "timeout", "<timeout>", "Timeout for this query");
    mdns_query_ip_args.max_results = arg_int0("m", "max_results", "<max_results>", "Maximum results returned");
    mdns_query_ip_args.end = arg_end(2);

    const esp_console_cmd_t cmd_init = {
        .command = "mdns_query_ip",
        .help = "Query MDNS for IP",
        .hint = NULL,
        .func = &cmd_mdns_query_ip,
        .argtable = &mdns_query_ip_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_init) );
}

static struct {
    struct arg_str *instance;
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_int *timeout;
    struct arg_int *max_results;
    struct arg_end *end;
} mdns_query_svc_args;

static int cmd_mdns_query_svc(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_query_svc_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_query_svc_args.end, argv[0]);
        return 1;
    }

    const char * instance = mdns_query_svc_args.instance->sval[0];
    const char * service = mdns_query_svc_args.service->sval[0];
    const char * proto = mdns_query_svc_args.proto->sval[0];
    int timeout = mdns_query_svc_args.timeout->ival[0];
    int max_results = mdns_query_svc_args.max_results->ival[0];

    if (timeout <= 0) {
        timeout = 5000;
    }

    if (max_results < 0 || max_results > 255) {
        max_results = 255;
    }

    printf("Query SVC: %s.%s.%s.local, Timeout: %d, Max Results: %d\n", instance, service, proto, timeout, max_results);

    mdns_result_t * results = NULL;
    esp_err_t err = mdns_query(instance, service, proto, MDNS_TYPE_ANY, timeout, max_results,  &results);
    if (err) {
        printf("ERROR: Query Failed\n");
        return 1;
    }
    if (!results) {
        printf("No results found!\n");
        return 0;
    }

    mdns_print_results(results);
    mdns_query_results_free(results);
    return 0;
}

static void register_mdns_query_svc()
{
    mdns_query_svc_args.instance = arg_str1(NULL, NULL, "<instance>", "Instance to search for");
    mdns_query_svc_args.service = arg_str1(NULL, NULL, "<service>", "Service to search for (ex. _http, _smb, etc.)");
    mdns_query_svc_args.proto = arg_str1(NULL, NULL, "<proto>", "Protocol to search for (_tcp, _udp, etc.)");
    mdns_query_svc_args.timeout = arg_int0("t", "timeout", "<timeout>", "Timeout for this query");
    mdns_query_svc_args.max_results = arg_int0("m", "max_results", "<max_results>", "Maximum results returned");
    mdns_query_svc_args.end = arg_end(2);

    const esp_console_cmd_t cmd_init = {
        .command = "mdns_query_svc",
        .help = "Query MDNS for Service TXT & SRV",
        .hint = NULL,
        .func = &cmd_mdns_query_svc,
        .argtable = &mdns_query_svc_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_init) );
}

static struct {
    struct arg_str *hostname;
    struct arg_str *instance;
    struct arg_end *end;
} mdns_init_args;

static int cmd_mdns_init(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_init_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_init_args.end, argv[0]);
        return 1;
    }

    ESP_ERROR_CHECK( mdns_init() );

    if (mdns_init_args.hostname->sval[0]) {
        ESP_ERROR_CHECK( mdns_hostname_set(mdns_init_args.hostname->sval[0]) );
        printf("MDNS: Hostname: %s\n", mdns_init_args.hostname->sval[0]);
    }

    if (mdns_init_args.instance->sval[0]) {
        ESP_ERROR_CHECK( mdns_instance_name_set(mdns_init_args.instance->sval[0]) );
        printf("MDNS: Instance: %s\n", mdns_init_args.instance->sval[0]);
    }

    return 0;
}

static void register_mdns_init()
{
    mdns_init_args.hostname = arg_str0("h", "hostname", "<hostname>", "Hostname that the server will advertise");
    mdns_init_args.instance = arg_str0("i", "instance", "<instance>", "Default instance name for services");
    mdns_init_args.end = arg_end(2);

    const esp_console_cmd_t cmd_init = {
        .command = "mdns_init",
        .help = "Start MDNS Server",
        .hint = NULL,
        .func = &cmd_mdns_init,
        .argtable = &mdns_init_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_init) );
}

static int cmd_mdns_free(int argc, char** argv)
{
    mdns_free();
    return 0;
}

static void register_mdns_free()
{
    const esp_console_cmd_t cmd_free = {
        .command = "mdns_free",
        .help = "Stop MDNS Server",
        .hint = NULL,
        .func = &cmd_mdns_free,
        .argtable = NULL
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_free) );
}

static struct {
    struct arg_str *hostname;
    struct arg_end *end;
} mdns_set_hostname_args;

static int cmd_mdns_set_hostname(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_set_hostname_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_set_hostname_args.end, argv[0]);
        return 1;
    }

    if (mdns_set_hostname_args.hostname->sval[0] == NULL) {
        printf("ERROR: Bad arguments!\n");
        return 1;
    }

    ESP_ERROR_CHECK( mdns_hostname_set(mdns_set_hostname_args.hostname->sval[0]) );
    return 0;
}

static void register_mdns_set_hostname()
{
    mdns_set_hostname_args.hostname = arg_str1(NULL, NULL, "<hostname>", "Hostname that the server will advertise");
    mdns_set_hostname_args.end = arg_end(2);

    const esp_console_cmd_t cmd_set_hostname = {
        .command = "mdns_set_hostname",
        .help = "Set MDNS Server hostname",
        .hint = NULL,
        .func = &cmd_mdns_set_hostname,
        .argtable = &mdns_set_hostname_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_set_hostname) );
}

static struct {
    struct arg_str *instance;
    struct arg_end *end;
} mdns_set_instance_args;

static int cmd_mdns_set_instance(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_set_instance_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_set_instance_args.end, argv[0]);
        return 1;
    }

    if (mdns_set_instance_args.instance->sval[0] == NULL) {
        printf("ERROR: Bad arguments!\n");
        return 1;
    }

    ESP_ERROR_CHECK( mdns_instance_name_set(mdns_set_instance_args.instance->sval[0]) );
    return 0;
}

static void register_mdns_set_instance()
{
    mdns_set_instance_args.instance = arg_str1(NULL, NULL, "<instance>", "Default instance name for services");
    mdns_set_instance_args.end = arg_end(2);

    const esp_console_cmd_t cmd_set_instance = {
        .command = "mdns_set_instance",
        .help = "Set MDNS Server Istance Name",
        .hint = NULL,
        .func = &cmd_mdns_set_instance,
        .argtable = &mdns_set_instance_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_set_instance) );
}

static mdns_txt_item_t * _convert_items(const char **values, int count)
{
    int i=0,e;
    const char * value = NULL;
    mdns_txt_item_t * items = (mdns_txt_item_t*) malloc(sizeof(mdns_txt_item_t) * count);
    if (!items) {
        printf("ERROR: No Memory!\n");
        goto fail;

    }
    memset(items, 0, sizeof(mdns_txt_item_t) * count);

    for (i=0; i<count; i++) {
        value = values[i];
        char * esign = strchr(value, '=');
        if (!esign) {
            printf("ERROR: Equal sign not found in '%s'!\n", value);
            goto fail;
        }
        int var_len = esign - value;
        int val_len = strlen(value) - var_len - 1;
        char * var = (char*)malloc(var_len+1);
        if (var == NULL) {
            printf("ERROR: No Memory!\n");
            goto fail;
        }
        char * val = (char*)malloc(val_len+1);
        if (val == NULL) {
            printf("ERROR: No Memory!\n");
            free(var);
            goto fail;
        }
        memcpy(var, value, var_len);
        var[var_len] = 0;
        memcpy(val, esign+1, val_len);
        val[val_len] = 0;

        items[i].key = var;
        items[i].value = val;
    }

    return items;

fail:
    for (e=0;e<i;e++) {
        free((char *)items[e].key);
        free((char *)items[e].value);
    }
    free(items);
    return NULL;
}

static struct {
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_int *port;
    struct arg_str *instance;
    struct arg_str *txt;
    struct arg_end *end;
} mdns_add_args;

static int cmd_mdns_service_add(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_add_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_add_args.end, argv[0]);
        return 1;
    }

    if (!mdns_add_args.service->sval[0] || !mdns_add_args.proto->sval[0] || !mdns_add_args.port->ival[0]) {
        printf("ERROR: Bad arguments!\n");
        return 1;
    }
    const char * instance = NULL;
    if (mdns_add_args.instance->sval[0] && mdns_add_args.instance->sval[0][0]) {
        instance = mdns_add_args.instance->sval[0];
        printf("MDNS: Service Instance: %s\n", instance);
    }
    mdns_txt_item_t * items = NULL;
    if (mdns_add_args.txt->count) {
        items = _convert_items(mdns_add_args.txt->sval, mdns_add_args.txt->count);
        if (!items) {
            printf("ERROR: No Memory!\n");
            return 1;

        }
    }

    ESP_ERROR_CHECK( mdns_service_add(instance, mdns_add_args.service->sval[0], mdns_add_args.proto->sval[0], mdns_add_args.port->ival[0], items, mdns_add_args.txt->count) );
    free(items);
    return 0;
}

static void register_mdns_service_add()
{
    mdns_add_args.service = arg_str1(NULL, NULL, "<service>", "MDNS Service");
    mdns_add_args.proto = arg_str1(NULL, NULL, "<proto>", "IP Protocol");
    mdns_add_args.port = arg_int1(NULL, NULL, "<port>", "Service Port");
    mdns_add_args.instance = arg_str0("i", "instance", "<instance>", "Instance name");
    mdns_add_args.txt = arg_strn(NULL, NULL, "item", 0, 30, "TXT Items (name=value)");
    mdns_add_args.end = arg_end(2);

    const esp_console_cmd_t cmd_add = {
        .command = "mdns_service_add",
        .help = "Add service to MDNS",
        .hint = NULL,
        .func = &cmd_mdns_service_add,
        .argtable = &mdns_add_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_add) );
}

static struct {
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_end *end;
} mdns_remove_args;

static int cmd_mdns_service_remove(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_remove_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_remove_args.end, argv[0]);
        return 1;
    }

    if (!mdns_remove_args.service->sval[0] || !mdns_remove_args.proto->sval[0]) {
        printf("ERROR: Bad arguments!\n");
        return 1;
    }

    ESP_ERROR_CHECK( mdns_service_remove(mdns_remove_args.service->sval[0], mdns_remove_args.proto->sval[0]) );
    return 0;
}

static void register_mdns_service_remove()
{
    mdns_remove_args.service = arg_str1(NULL, NULL, "<service>", "MDNS Service");
    mdns_remove_args.proto = arg_str1(NULL, NULL, "<proto>", "IP Protocol");
    mdns_remove_args.end = arg_end(2);

    const esp_console_cmd_t cmd_remove = {
        .command = "mdns_service_remove",
        .help = "Remove service from MDNS",
        .hint = NULL,
        .func = &cmd_mdns_service_remove,
        .argtable = &mdns_remove_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_remove) );
}

static struct {
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_str *instance;
    struct arg_end *end;
} mdns_service_instance_set_args;

static int cmd_mdns_service_instance_set(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_service_instance_set_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_service_instance_set_args.end, argv[0]);
        return 1;
    }

    if (!mdns_service_instance_set_args.service->sval[0] || !mdns_service_instance_set_args.proto->sval[0] || !mdns_service_instance_set_args.instance->sval[0]) {
        printf("ERROR: Bad arguments!\n");
        return 1;
    }

    ESP_ERROR_CHECK( mdns_service_instance_name_set(mdns_service_instance_set_args.service->sval[0], mdns_service_instance_set_args.proto->sval[0], mdns_service_instance_set_args.instance->sval[0]) );
    return 0;
}

static void register_mdns_service_instance_set()
{
    mdns_service_instance_set_args.service = arg_str1(NULL, NULL, "<service>", "MDNS Service");
    mdns_service_instance_set_args.proto = arg_str1(NULL, NULL, "<proto>", "IP Protocol");
    mdns_service_instance_set_args.instance = arg_str1(NULL, NULL, "<instance>", "Instance name");
    mdns_service_instance_set_args.end = arg_end(2);

    const esp_console_cmd_t cmd_add = {
        .command = "mdns_service_instance_set",
        .help = "Set MDNS Service Instance Name",
        .hint = NULL,
        .func = &cmd_mdns_service_instance_set,
        .argtable = &mdns_service_instance_set_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_add) );
}

static struct {
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_int *port;
    struct arg_end *end;
} mdns_service_port_set_args;

static int cmd_mdns_service_port_set(int argc, char** argv) {
    int nerrors = arg_parse(argc, argv, (void**) &mdns_service_port_set_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_service_port_set_args.end, argv[0]);
        return 1;
    }

    if (!mdns_service_port_set_args.service->sval[0] || !mdns_service_port_set_args.proto->sval[0] || !mdns_service_port_set_args.port->ival[0]) {
        printf("ERROR: Bad arguments!\n");
        return 1;
    }

    ESP_ERROR_CHECK( mdns_service_port_set(mdns_service_port_set_args.service->sval[0], mdns_service_port_set_args.proto->sval[0], mdns_service_port_set_args.port->ival[0]) );
    return 0;
}

static void register_mdns_service_port_set()
{
    mdns_service_port_set_args.service = arg_str1(NULL, NULL, "<service>", "MDNS Service");
    mdns_service_port_set_args.proto = arg_str1(NULL, NULL, "<proto>", "IP Protocol");
    mdns_service_port_set_args.port = arg_int1(NULL, NULL, "<port>", "Service Port");
    mdns_service_port_set_args.end = arg_end(2);

    const esp_console_cmd_t cmd_add = {
        .command = "mdns_service_port_set",
        .help = "Set MDNS Service port",
        .hint = NULL,
        .func = &cmd_mdns_service_port_set,
        .argtable = &mdns_service_port_set_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_add) );
}

static struct {
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_str *txt;
    struct arg_end *end;
} mdns_txt_replace_args;

static int cmd_mdns_service_txt_replace(int argc, char** argv)
{
    mdns_txt_item_t * items = NULL;
    int nerrors = arg_parse(argc, argv, (void**) &mdns_txt_replace_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_txt_replace_args.end, argv[0]);
        return 1;
    }

    if (!mdns_txt_replace_args.service->sval[0] || !mdns_txt_replace_args.proto->sval[0]) {
        printf("ERROR: Bad arguments!\n");
        return 1;
    }

    if (mdns_txt_replace_args.txt->count) {
        items = _convert_items(mdns_txt_replace_args.txt->sval, mdns_txt_replace_args.txt->count);
        if (!items) {
            printf("ERROR: No Memory!\n");
            return 1;

        }
    }
    ESP_ERROR_CHECK( mdns_service_txt_set(mdns_txt_replace_args.service->sval[0], mdns_txt_replace_args.proto->sval[0], items, mdns_txt_replace_args.txt->count) );
    free(items);
    return 0;
}

static void register_mdns_service_txt_replace()
{
    mdns_txt_replace_args.service = arg_str1(NULL, NULL, "<service>", "MDNS Service");
    mdns_txt_replace_args.proto = arg_str1(NULL, NULL, "<proto>", "IP Protocol");
    mdns_txt_replace_args.txt = arg_strn(NULL, NULL, "item", 0, 30, "TXT Items (name=value)");
    mdns_txt_replace_args.end = arg_end(2);

    const esp_console_cmd_t cmd_txt_set = {
        .command = "mdns_service_txt_replace",
        .help = "Replace MDNS service TXT items",
        .hint = NULL,
        .func = &cmd_mdns_service_txt_replace,
        .argtable = &mdns_txt_replace_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_txt_set) );
}

static struct {
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_str *var;
    struct arg_str *value;
    struct arg_end *end;
} mdns_txt_set_args;

static int cmd_mdns_service_txt_set(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_txt_set_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_txt_set_args.end, argv[0]);
        return 1;
    }

    if (!mdns_txt_set_args.service->sval[0] || !mdns_txt_set_args.proto->sval[0] || !mdns_txt_set_args.var->sval[0]) {
        printf("ERROR: Bad arguments!\n");
        return 1;
    }

    ESP_ERROR_CHECK( mdns_service_txt_item_set(mdns_txt_set_args.service->sval[0], mdns_txt_set_args.proto->sval[0], mdns_txt_set_args.var->sval[0], mdns_txt_set_args.value->sval[0]) );
    return 0;
}

static void register_mdns_service_txt_set()
{
    mdns_txt_set_args.service = arg_str1(NULL, NULL, "<service>", "MDNS Service");
    mdns_txt_set_args.proto = arg_str1(NULL, NULL, "<proto>", "IP Protocol");
    mdns_txt_set_args.var = arg_str1(NULL, NULL, "<var>", "Item Name");
    mdns_txt_set_args.value = arg_str1(NULL, NULL, "<value>", "Item Value");
    mdns_txt_set_args.end = arg_end(2);

    const esp_console_cmd_t cmd_txt_set = {
        .command = "mdns_service_txt_set",
        .help = "Add/Set MDNS service TXT item",
        .hint = NULL,
        .func = &cmd_mdns_service_txt_set,
        .argtable = &mdns_txt_set_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_txt_set) );
}

static struct {
    struct arg_str *service;
    struct arg_str *proto;
    struct arg_str *var;
    struct arg_end *end;
} mdns_txt_remove_args;

static int cmd_mdns_service_txt_remove(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &mdns_txt_remove_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mdns_txt_remove_args.end, argv[0]);
        return 1;
    }

    if (!mdns_txt_remove_args.service->sval[0] || !mdns_txt_remove_args.proto->sval[0] || !mdns_txt_remove_args.var->sval[0]) {
        printf("ERROR: Bad arguments!\n");
        return 1;
    }

    ESP_ERROR_CHECK( mdns_service_txt_item_remove(mdns_txt_remove_args.service->sval[0], mdns_txt_remove_args.proto->sval[0], mdns_txt_remove_args.var->sval[0]) );
    return 0;
}

static void register_mdns_service_txt_remove()
{
    mdns_txt_remove_args.service = arg_str1(NULL, NULL, "<service>", "MDNS Service");
    mdns_txt_remove_args.proto = arg_str1(NULL, NULL, "<proto>", "IP Protocol");
    mdns_txt_remove_args.var = arg_str1(NULL, NULL, "<var>", "Item Name");
    mdns_txt_remove_args.end = arg_end(2);

    const esp_console_cmd_t cmd_txt_remove = {
        .command = "mdns_service_txt_remove",
        .help = "Remove MDNS service TXT item",
        .hint = NULL,
        .func = &cmd_mdns_service_txt_remove,
        .argtable = &mdns_txt_remove_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_txt_remove) );
}

static int cmd_mdns_service_remove_all(int argc, char** argv)
{
    mdns_service_remove_all();
    return 0;
}

static void register_mdns_service_remove_all()
{
    const esp_console_cmd_t cmd_free = {
        .command = "mdns_service_remove_all",
        .help = "Remove all MDNS services",
        .hint = NULL,
        .func = &cmd_mdns_service_remove_all,
        .argtable = NULL
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_free) );
}

void mdns_console_register()
{
    register_mdns_init();
    register_mdns_free();
    register_mdns_set_hostname();
    register_mdns_set_instance();
    register_mdns_service_add();
    register_mdns_service_remove();
    register_mdns_service_instance_set();
    register_mdns_service_port_set();
    register_mdns_service_txt_replace();
    register_mdns_service_txt_set();
    register_mdns_service_txt_remove();
    register_mdns_service_remove_all();

    register_mdns_query_a();
    register_mdns_query_aaaa();
    register_mdns_query_txt();
    register_mdns_query_srv();
    register_mdns_query_ptr();

    register_mdns_query_ip();
    register_mdns_query_svc();
}

