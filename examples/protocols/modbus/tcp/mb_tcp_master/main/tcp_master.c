// Copyright 2016-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "string.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "mdns.h"
#include "protocol_examples_common.h"

#include "modbus_params.h"  // for modbus parameters structures
#include "mbcontroller.h"
#include "sdkconfig.h"

#define MB_TCP_PORT                     (CONFIG_FMB_TCP_PORT_DEFAULT)   // TCP port used by example

// The number of parameters that intended to be used in the particular control process
#define MASTER_MAX_CIDS num_device_parameters

// Number of reading of parameters from slave
#define MASTER_MAX_RETRY                (30)

// Timeout to update cid over Modbus
#define UPDATE_CIDS_TIMEOUT_MS          (500)
#define UPDATE_CIDS_TIMEOUT_TICS        (UPDATE_CIDS_TIMEOUT_MS / portTICK_RATE_MS)

// Timeout between polls
#define POLL_TIMEOUT_MS                 (1)
#define POLL_TIMEOUT_TICS               (POLL_TIMEOUT_MS / portTICK_RATE_MS)
#define MB_MDNS_PORT                    (502)

#define MASTER_TAG "MASTER_TEST"

// The macro to get offset for parameter in the appropriate structure
#define HOLD_OFFSET(field) ((uint16_t)(offsetof(holding_reg_params_t, field) + 1))
#define INPUT_OFFSET(field) ((uint16_t)(offsetof(input_reg_params_t, field) + 1))
#define COIL_OFFSET(field) ((uint16_t)(offsetof(coil_reg_params_t, field) + 1))
#define DISCR_OFFSET(field) ((uint16_t)(offsetof(discrete_reg_params_t, field) + 1))
#define STR(fieldname) ((const char*)( fieldname ))

// Options can be used as bit masks or parameter limits
#define OPTS(min_val, max_val, step_val) { .opt1 = min_val, .opt2 = max_val, .opt3 = step_val }

#define MB_ID_BYTE0(id) ((uint8_t)(id))
#define MB_ID_BYTE1(id) ((uint8_t)(((uint16_t)(id) >> 8) & 0xFF))
#define MB_ID_BYTE2(id) ((uint8_t)(((uint32_t)(id) >> 16) & 0xFF))
#define MB_ID_BYTE3(id) ((uint8_t)(((uint32_t)(id) >> 24) & 0xFF))

#define MB_ID2STR(id) MB_ID_BYTE0(id), MB_ID_BYTE1(id), MB_ID_BYTE2(id), MB_ID_BYTE3(id)

#if CONFIG_FMB_CONTROLLER_SLAVE_ID_SUPPORT
#define MB_DEVICE_ID (uint32_t)CONFIG_FMB_CONTROLLER_SLAVE_ID
#else
#define MB_DEVICE_ID (uint32_t)0x00112233
#endif

#define MB_MDNS_INSTANCE(pref) pref"mb_master_tcp"

// Enumeration of modbus device addresses accessed by master device
// Each address in the table is a index of TCP slave ip address in mb_communication_info_t::tcp_ip_addr table
enum {
    MB_DEVICE_ADDR1 = 1, // Slave address 1
    MB_DEVICE_COUNT
};

// Enumeration of all supported CIDs for device (used in parameter definition table)
enum {
    CID_INP_DATA_0 = 0,
    CID_HOLD_DATA_0,
    CID_INP_DATA_1,
    CID_HOLD_DATA_1,
    CID_INP_DATA_2,
    CID_HOLD_DATA_2,
    CID_RELAY_P1,
    CID_RELAY_P2,
    CID_COUNT
};

// Example Data (Object) Dictionary for Modbus parameters:
// The CID field in the table must be unique.
// Modbus Slave Addr field defines slave address of the device with correspond parameter.
// Modbus Reg Type - Type of Modbus register area (Holding register, Input Register and such).
// Reg Start field defines the start Modbus register number and Reg Size defines the number of registers for the characteristic accordingly.
// The Instance Offset defines offset in the appropriate parameter structure that will be used as instance to save parameter value.
// Data Type, Data Size specify type of the characteristic and its data size.
// Parameter Options field specifies the options that can be used to process parameter value (limits or masks).
// Access Mode - can be used to implement custom options for processing of characteristic (Read/Write restrictions, factory mode values and etc).
const mb_parameter_descriptor_t device_parameters[] = {
    // { CID, Param Name, Units, Modbus Slave Addr, Modbus Reg Type, Reg Start, Reg Size, Instance Offset, Data Type, Data Size, Parameter Options, Access Mode}
    { CID_INP_DATA_0, STR("Data_channel_0"), STR("Volts"), MB_DEVICE_ADDR1, MB_PARAM_INPUT, 0, 2,
                    INPUT_OFFSET(input_data0), PARAM_TYPE_FLOAT, 4, OPTS( -10, 10, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    { CID_HOLD_DATA_0, STR("Humidity_1"), STR("%rH"), MB_DEVICE_ADDR1, MB_PARAM_HOLDING, 0, 2,
            HOLD_OFFSET(holding_data0), PARAM_TYPE_FLOAT, 4, OPTS( 0, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    { CID_INP_DATA_1, STR("Temperature_1"), STR("C"), MB_DEVICE_ADDR1, MB_PARAM_INPUT, 2, 2,
            INPUT_OFFSET(input_data1), PARAM_TYPE_FLOAT, 4, OPTS( -40, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    { CID_HOLD_DATA_1, STR("Humidity_2"), STR("%rH"), MB_DEVICE_ADDR1, MB_PARAM_HOLDING, 2, 2,
            HOLD_OFFSET(holding_data1), PARAM_TYPE_FLOAT, 4, OPTS( 0, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    { CID_INP_DATA_2, STR("Temperature_2"), STR("C"), MB_DEVICE_ADDR1, MB_PARAM_INPUT, 4, 2,
            INPUT_OFFSET(input_data2), PARAM_TYPE_FLOAT, 4, OPTS( -40, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    { CID_HOLD_DATA_2, STR("Humidity_3"), STR("%rH"), MB_DEVICE_ADDR1, MB_PARAM_HOLDING, 4, 2,
            HOLD_OFFSET(holding_data2), PARAM_TYPE_FLOAT, 4, OPTS( 0, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    { CID_RELAY_P1, STR("RelayP1"), STR("on/off"), MB_DEVICE_ADDR1, MB_PARAM_COIL, 0, 8,
            COIL_OFFSET(coils_port0), PARAM_TYPE_U16, 2, OPTS( BIT1, 0, 0 ), PAR_PERMS_READ_WRITE_TRIGGER },
    { CID_RELAY_P2, STR("RelayP2"), STR("on/off"), MB_DEVICE_ADDR1, MB_PARAM_COIL, 8, 8,
            COIL_OFFSET(coils_port1), PARAM_TYPE_U16, 2, OPTS( BIT0, 0, 0 ), PAR_PERMS_READ_WRITE_TRIGGER }
};

// Calculate number of parameters in the table
const uint16_t num_device_parameters = (sizeof(device_parameters)/sizeof(device_parameters[0]));

// This table represents slave IP addresses that correspond to the short address field of the slave in device_parameters structure
// Modbus TCP stack shall use these addresses to be able to connect and read parameters from slave
char* slave_ip_address_table[MB_DEVICE_COUNT] = {
#if CONFIG_MB_SLAVE_IP_FROM_STDIN
    "FROM_STDIN",     // Address corresponds to MB_DEVICE_ADDR1 and set to predefined value by user
    NULL
#elif CONFIG_MB_MDNS_IP_RESOLVER
    NULL,
    NULL
#endif
};

#if CONFIG_MB_SLAVE_IP_FROM_STDIN

// Scan IP address according to IPV settings
char* master_scan_addr(int* index, char* buffer)
{
    char* ip_str = NULL;
    unsigned int a[8] = {0};
    int buf_cnt = 0;
#if !CONFIG_EXAMPLE_CONNECT_IPV6
    buf_cnt = sscanf(buffer, "IP%d="IPSTR, index, &a[0], &a[1], &a[2], &a[3]);
    if (buf_cnt == 5) {
        if (-1 == asprintf(&ip_str, IPSTR, a[0], a[1], a[2], a[3])) {
            abort();
        }
    }
#else
    buf_cnt = sscanf(buffer, "IP%d="IPV6STR, index, &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6], &a[7]);
    if (buf_cnt == 9) {
        if (-1 == asprintf(&ip_str, IPV6STR, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7])) {
            abort();
        }
    }
#endif
    return ip_str;
}

static int master_get_slave_ip_stdin(char** addr_table)
{
    char buf[128];
    int index;
    char* ip_str = NULL;
    int buf_cnt = 0;
    int ip_cnt = 0;

    if (!addr_table) {
        return 0;
    }

    ESP_ERROR_CHECK(example_configure_stdin_stdout());
    while(1) {
        if (addr_table[ip_cnt] && strcmp(addr_table[ip_cnt], "FROM_STDIN") == 0) {
            printf("Waiting IP%d from stdin:\r\n", ip_cnt);
            while (fgets(buf, sizeof(buf), stdin) == NULL) {
                fputs(buf, stdout);
            }
            buf_cnt = strlen(buf);
            buf[buf_cnt - 1] = '\0';
            fputc('\n', stdout);
            ip_str = master_scan_addr(&index, buf);
            if (ip_str != NULL) {
                ESP_LOGI(MASTER_TAG, "IP(%d) = [%s] set from stdin.", ip_cnt, ip_str);
                if ((ip_cnt >= MB_DEVICE_COUNT) || (index != ip_cnt)) {
                    addr_table[ip_cnt] = NULL;
                    break;
                }
                addr_table[ip_cnt++] = ip_str;
            } else {
                // End of configuration
                addr_table[ip_cnt++] = NULL;
                break;
            }
        } else {
            if (addr_table[ip_cnt]) {
                ESP_LOGI(MASTER_TAG, "Leave IP(%d) = [%s] set manually.", ip_cnt, addr_table[ip_cnt]);
                ip_cnt++;
            } else {
                ESP_LOGI(MASTER_TAG, "IP(%d) is not set in the table.", ip_cnt);
                break;
            }
        }
    }
    return ip_cnt;
}

#elif CONFIG_MB_MDNS_IP_RESOLVER

// convert MAC from binary format to string
static inline char* gen_mac_str(const uint8_t* mac, char* pref, char* mac_str)
{
    sprintf(mac_str, "%s%02X%02X%02X%02X%02X%02X", pref, MAC2STR(mac));
    return mac_str;
}

static inline char* gen_id_str(char* service_name, char* slave_id_str)
{
    sprintf(slave_id_str, "%s%02X%02X%02X%02X", service_name, MB_ID2STR(MB_DEVICE_ID));
    return slave_id_str;
}

static void master_start_mdns_service()
{
    char temp_str[32] = {0};
    uint8_t sta_mac[6] = {0};
    ESP_ERROR_CHECK(esp_read_mac(sta_mac, ESP_MAC_WIFI_STA));
    char* hostname = gen_mac_str(sta_mac, MB_MDNS_INSTANCE("")"_", temp_str);
    // initialize mDNS
    ESP_ERROR_CHECK(mdns_init());
    // set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    ESP_LOGI(MASTER_TAG, "mdns hostname set to: [%s]", hostname);

    // set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set(MB_MDNS_INSTANCE("esp32_")));

    // structure with TXT records
    mdns_txt_item_t serviceTxtData[] = {
        {"board","esp32"}
    };

    // initialize service
    ESP_ERROR_CHECK(mdns_service_add(MB_MDNS_INSTANCE(""), "_modbus", "_tcp", MB_MDNS_PORT, serviceTxtData, 1));
    // add mac key string text item
    ESP_ERROR_CHECK(mdns_service_txt_item_set("_modbus", "_tcp", "mac", gen_mac_str(sta_mac, "\0", temp_str)));
    // add slave id key txt item
    ESP_ERROR_CHECK( mdns_service_txt_item_set("_modbus", "_tcp", "mb_id", gen_id_str("\0", temp_str)));
}

static char* master_get_slave_ip_str(mdns_ip_addr_t* address, mb_tcp_addr_type_t addr_type)
{
    mdns_ip_addr_t* a = address;
    char* slave_ip_str = NULL;

    while (a) {
        if ((a->addr.type == ESP_IPADDR_TYPE_V6) && (addr_type == MB_IPV6)) {
            if (-1 == asprintf(&slave_ip_str, IPV6STR, IPV62STR(a->addr.u_addr.ip6))) {
                abort();
            }
        } else if ((a->addr.type == ESP_IPADDR_TYPE_V4) && (addr_type == MB_IPV4)) {
            if (-1 == asprintf(&slave_ip_str, IPSTR, IP2STR(&(a->addr.u_addr.ip4)))) {
                abort();
            }
        }
        if (slave_ip_str) {
            break;
        }
        a = a->next;
    }
    return slave_ip_str;
}

static esp_err_t master_resolve_slave(const char* name, mdns_result_t* result, char** resolved_ip,
                                        mb_tcp_addr_type_t addr_type)
{
    if (!name || !result) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_result_t* r = result;
    int t;
    char* slave_ip = NULL;
    for (; r ; r = r->next) {
        if ((r->ip_protocol == MDNS_IP_PROTOCOL_V4) && (addr_type == MB_IPV6)) {
            continue;
        } else if ((r->ip_protocol == MDNS_IP_PROTOCOL_V6) && (addr_type == MB_IPV4)) {
            continue;
        }
        // Check host name for Modbus short address and
        // append it into slave ip address table
        if ((strcmp(r->instance_name, name) == 0) && (r->port == CONFIG_FMB_TCP_PORT_DEFAULT)) {
            printf("  PTR : %s\n", r->instance_name);
            if (r->txt_count) {
                printf("  TXT : [%u] ", r->txt_count);
                for ( t = 0; t < r->txt_count; t++) {
                    printf("%s=%s; ", r->txt[t].key, r->txt[t].value?r->txt[t].value:"NULL");
                }
                printf("\n");
            }
            slave_ip = master_get_slave_ip_str(r->addr, addr_type);
            if (slave_ip) {
                ESP_LOGI(MASTER_TAG, "Resolved slave %s[%s]:%u", r->hostname, slave_ip, r->port);
                *resolved_ip = slave_ip;
                return ESP_OK;
            }
        }
    }
    *resolved_ip = NULL;
    ESP_LOGD(MASTER_TAG, "Fail to resolve slave: %s", name);
    return ESP_ERR_NOT_FOUND;
}

static int master_create_slave_list(mdns_result_t* results, char** addr_table,
                                        mb_tcp_addr_type_t addr_type)
{
    if (!results) {
        return -1;
    }
    int i, addr, resolved = 0;
    const mb_parameter_descriptor_t* pdescr = &device_parameters[0];
    char** ip_table = addr_table;
    char slave_name[22] = {0};
    char* slave_ip = NULL;

    for (i = 0; (i < num_device_parameters && pdescr); i++, pdescr++) {
        addr = pdescr->mb_slave_addr;
        if (-1 == sprintf(slave_name, "mb_slave_tcp_%02X", addr)) {
            ESP_LOGI(MASTER_TAG, "Fail to create instance name for index: %d", addr);
            abort();
        }
        if (!ip_table[addr - 1]) {
            esp_err_t err = master_resolve_slave(slave_name, results, &slave_ip, addr_type);
            if (err != ESP_OK) {
                ESP_LOGE(MASTER_TAG, "Index: %d, sl_addr: %d, name:%s, failed to resolve!",
                                        i, addr, slave_name);
                // Set correspond index to NULL indicate host not resolved
                ip_table[addr - 1] = NULL;
                continue;
            }
            ip_table[addr - 1] = slave_ip; //slave_name;
            ESP_LOGI(MASTER_TAG, "Index: %d, sl_addr: %d, name:%s, resolve to IP: [%s]",
                                    i, addr, slave_name, slave_ip);
            resolved++;
        } else {
            ESP_LOGI(MASTER_TAG, "Index: %d, sl_addr: %d, name:%s, set to IP: [%s]",
                                    i, addr, slave_name, ip_table[addr - 1]);
            resolved++;
        }
    }
    return resolved;
}

static void master_destroy_slave_list(char** table)
{
    for (int i = 0; ((i < MB_DEVICE_COUNT) && table[i] != NULL); i++) {
        if (table[i]) {
            free(table[i]);
            table[i] = NULL;
        }
    }
}

static int master_query_slave_service(const char * service_name, const char * proto,
                                        mb_tcp_addr_type_t addr_type)
{
    ESP_LOGI(MASTER_TAG, "Query PTR: %s.%s.local", service_name, proto);

    mdns_result_t* results = NULL;
    int count = 0;

    esp_err_t err = mdns_query_ptr(service_name, proto, 3000, 20, &results);
    if(err){
        ESP_LOGE(MASTER_TAG, "Query Failed: %s", esp_err_to_name(err));
        return count;
    }
    if(!results){
        ESP_LOGW(MASTER_TAG, "No results found!");
        return count;
    }

    count = master_create_slave_list(results, slave_ip_address_table, addr_type);

    mdns_query_results_free(results);
    return count;
}
#endif

// The function to get pointer to parameter storage (instance) according to parameter description table
static void* master_get_param_data(const mb_parameter_descriptor_t* param_descriptor)
{
    assert(param_descriptor != NULL);
    void* instance_ptr = NULL;
    if (param_descriptor->param_offset != 0) {
       switch(param_descriptor->mb_param_type)
       {
           case MB_PARAM_HOLDING:
               instance_ptr = ((void*)&holding_reg_params + param_descriptor->param_offset - 1);
               break;
           case MB_PARAM_INPUT:
               instance_ptr = ((void*)&input_reg_params + param_descriptor->param_offset - 1);
               break;
           case MB_PARAM_COIL:
               instance_ptr = ((void*)&coil_reg_params + param_descriptor->param_offset - 1);
               break;
           case MB_PARAM_DISCRETE:
               instance_ptr = ((void*)&discrete_reg_params + param_descriptor->param_offset - 1);
               break;
           default:
               instance_ptr = NULL;
               break;
       }
    } else {
        ESP_LOGE(MASTER_TAG, "Wrong parameter offset for CID #%d", param_descriptor->cid);
        assert(instance_ptr != NULL);
    }
    return instance_ptr;
}

// User operation function to read slave values and check alarm
static void master_operation_func(void *arg)
{
    esp_err_t err = ESP_OK;
    float value = 0;
    bool alarm_state = false;
    const mb_parameter_descriptor_t* param_descriptor = NULL;

    ESP_LOGI(MASTER_TAG, "Start modbus test...");

    for(uint16_t retry = 0; retry <= MASTER_MAX_RETRY && (!alarm_state); retry++) {
        // Read all found characteristics from slave(s)
        for (uint16_t cid = 0; (err != ESP_ERR_NOT_FOUND) && cid < MASTER_MAX_CIDS; cid++)
        {
            // Get data from parameters description table
            // and use this information to fill the characteristics description table
            // and having all required fields in just one table
            err = mbc_master_get_cid_info(cid, &param_descriptor);
            if ((err != ESP_ERR_NOT_FOUND) && (param_descriptor != NULL)) {
                void* temp_data_ptr = master_get_param_data(param_descriptor);
                assert(temp_data_ptr);
                uint8_t type = 0;
                err = mbc_master_get_parameter(cid, (char*)param_descriptor->param_key,
                                                    (uint8_t*)&value, &type);
                if (err == ESP_OK) {
                    *(float*)temp_data_ptr = value;
                    if ((param_descriptor->mb_param_type == MB_PARAM_HOLDING) ||
                        (param_descriptor->mb_param_type == MB_PARAM_INPUT)) {
                        ESP_LOGI(MASTER_TAG, "Characteristic #%d %s (%s) value = %f (0x%x) read successful.",
                                        param_descriptor->cid,
                                        (char*)param_descriptor->param_key,
                                        (char*)param_descriptor->param_units,
                                        value,
                                        *(uint32_t*)temp_data_ptr);
                        if (((value > param_descriptor->param_opts.max) ||
                            (value < param_descriptor->param_opts.min))) {
                                alarm_state = true;
                                break;
                        }
                    } else {
                        uint16_t state = *(uint16_t*)temp_data_ptr;
                        const char* rw_str = (state & param_descriptor->param_opts.opt1) ? "ON" : "OFF";
                        ESP_LOGI(MASTER_TAG, "Characteristic #%d %s (%s) value = %s (0x%x) read successful.",
                                        param_descriptor->cid,
                                        (char*)param_descriptor->param_key,
                                        (char*)param_descriptor->param_units,
                                        (const char*)rw_str,
                                        *(uint16_t*)temp_data_ptr);
                        if (state & param_descriptor->param_opts.opt1) {
                            alarm_state = true;
                            break;
                        }
                    }
                } else {
                    ESP_LOGE(MASTER_TAG, "Characteristic #%d (%s) read fail, err = %d (%s).",
                                        param_descriptor->cid,
                                        (char*)param_descriptor->param_key,
                                        (int)err,
                                        (char*)esp_err_to_name(err));
                }
                vTaskDelay(POLL_TIMEOUT_TICS); // timeout between polls
            }
        }
        vTaskDelay(UPDATE_CIDS_TIMEOUT_TICS);
    }

    if (alarm_state) {
        ESP_LOGI(MASTER_TAG, "Alarm triggered by cid #%d.",
                                        param_descriptor->cid);
    } else {
        ESP_LOGE(MASTER_TAG, "Alarm is not triggered after %d retries.",
                                        MASTER_MAX_RETRY);
    }
    ESP_LOGI(MASTER_TAG, "Destroy master...");
    vTaskDelay(100);
}

static esp_err_t init_services(mb_tcp_addr_type_t ip_addr_type)
{
    esp_err_t result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      result = nvs_flash_init();
    }
    ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
                            MASTER_TAG,
                            "nvs_flash_init fail, returns(0x%x).",
                            (uint32_t)result);
    result = esp_netif_init();
    ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
                            MASTER_TAG,
                            "esp_netif_init fail, returns(0x%x).",
                            (uint32_t)result);
    result = esp_event_loop_create_default();
    ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
                            MASTER_TAG,
                            "esp_event_loop_create_default fail, returns(0x%x).",
                            (uint32_t)result);
#if CONFIG_MB_MDNS_IP_RESOLVER
    // Start mdns service and register device
    master_start_mdns_service();
#endif
    // This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    // Read "Establishing Wi-Fi or Ethernet Connection" section in
    // examples/protocols/README.md for more information about this function.
    result = example_connect();
    ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
                                MASTER_TAG,
                                "example_connect fail, returns(0x%x).",
                                (uint32_t)result);
#if CONFIG_EXAMPLE_CONNECT_WIFI
   result = esp_wifi_set_ps(WIFI_PS_NONE);
   ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
                                   MASTER_TAG,
                                   "esp_wifi_set_ps fail, returns(0x%x).",
                                   (uint32_t)result);
#endif
#if CONFIG_MB_MDNS_IP_RESOLVER
    int res = 0;
    for (int retry = 0; (res < num_device_parameters) && (retry < 10); retry++) {
        res = master_query_slave_service("_modbus", "_tcp", ip_addr_type);
    }
    if (res < num_device_parameters) {
        ESP_LOGE(MASTER_TAG, "Could not resolve one or more slave IP addresses, resolved: %d out of %d.", res, num_device_parameters );
        ESP_LOGE(MASTER_TAG, "Make sure you configured all slaves according to device parameter table and they alive in the network.");
        return ESP_ERR_NOT_FOUND;
    }
    mdns_free();
#elif CONFIG_MB_SLAVE_IP_FROM_STDIN
    int ip_cnt = master_get_slave_ip_stdin(slave_ip_address_table);
    if (ip_cnt) {
        ESP_LOGI(MASTER_TAG, "Configured %d IP addresse(s).", ip_cnt);
    } else {
        ESP_LOGE(MASTER_TAG, "Fail to get IP address from stdin. Continue.");
        return ESP_ERR_NOT_FOUND;
    }
#endif
    return ESP_OK;
}

static esp_err_t destroy_services(void)
{
    esp_err_t err = ESP_OK;
#if CONFIG_MB_MDNS_IP_RESOLVER
    master_destroy_slave_list(slave_ip_address_table);
#endif
    err = example_disconnect();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                   MASTER_TAG,
                                   "example_disconnect fail, returns(0x%x).",
                                   (uint32_t)err);
    err = esp_event_loop_delete_default();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                       MASTER_TAG,
                                       "esp_event_loop_delete_default fail, returns(0x%x).",
                                       (uint32_t)err);
    err = esp_netif_deinit();
    ESP_RETURN_ON_FALSE((err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED), ESP_ERR_INVALID_STATE,
                                        MASTER_TAG,
                                        "esp_netif_deinit fail, returns(0x%x).",
                                        (uint32_t)err);
    err = nvs_flash_deinit();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                MASTER_TAG,
                                "nvs_flash_deinit fail, returns(0x%x).",
                                (uint32_t)err);
    return err;
}

// Modbus master initialization
static esp_err_t master_init(mb_communication_info_t* comm_info)
{
    void* master_handler = NULL;

    esp_err_t err = mbc_master_init_tcp(&master_handler);
    ESP_RETURN_ON_FALSE((master_handler != NULL), ESP_ERR_INVALID_STATE,
                                MASTER_TAG,
                                "mb controller initialization fail.");
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                            MASTER_TAG,
                            "mb controller initialization fail, returns(0x%x).",
                            (uint32_t)err);

    err = mbc_master_setup((void*)comm_info);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                            MASTER_TAG,
                            "mb controller setup fail, returns(0x%x).",
                            (uint32_t)err);

    err = mbc_master_set_descriptor(&device_parameters[0], num_device_parameters);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                MASTER_TAG,
                                "mb controller set descriptor fail, returns(0x%x).",
                                (uint32_t)err);
    ESP_LOGI(MASTER_TAG, "Modbus master stack initialized...");

    err = mbc_master_start();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                            MASTER_TAG,
                            "mb controller start fail, returns(0x%x).",
                            (uint32_t)err);
    vTaskDelay(5);
    return err;
}

static esp_err_t master_destroy(void)
{
    esp_err_t err = mbc_master_destroy();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                MASTER_TAG,
                                "mbc_master_destroy fail, returns(0x%x).",
                                (uint32_t)err);
    ESP_LOGI(MASTER_TAG, "Modbus master stack destroy...");
    return err;
}

void app_main(void)
{
    mb_tcp_addr_type_t ip_addr_type;
#if !CONFIG_EXAMPLE_CONNECT_IPV6
    ip_addr_type = MB_IPV4;
#else
    ip_addr_type = MB_IPV6;
#endif
    ESP_ERROR_CHECK(init_services(ip_addr_type));

    mb_communication_info_t comm_info = { 0 };
    comm_info.ip_port = MB_TCP_PORT;
    comm_info.ip_addr_type = ip_addr_type;
    comm_info.ip_mode = MB_MODE_TCP;
    comm_info.ip_addr = (void*)slave_ip_address_table;
    comm_info.ip_netif_ptr = (void*)get_example_netif();

    ESP_ERROR_CHECK(master_init(&comm_info));
    vTaskDelay(50);

    master_operation_func(NULL);
    ESP_ERROR_CHECK(master_destroy());
    ESP_ERROR_CHECK(destroy_services());
}
