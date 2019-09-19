/* Console example — WiFi commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "cmd_decl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "tcpip_adapter.h"
#include "esp_event.h"
#include "cmd_wifi.h"
#include "cmd_ping.h"
#include "lwip/inet.h"
#include <netinet/in.h>
#include "esp_ping.h"
#include "ping/ping.h"

#define DEFAULT_COUNT (5)
#define DEFAULT_TIMEOUT (1000)    // unit = ms
#define DEFAULT_DELAY (500)       // unit = ms
#define DEFAULT_DATA_SIZE (32)	// bytes
#define FAILURE (-1)

extern volatile bool is_connected;

/** Arguments used by 'send_icmp' function */
static struct {
    struct arg_str *ip_address;
    struct arg_int *count;
    struct arg_int *timeout;
    struct arg_int *delay;
    struct arg_int *packet_size;
    struct arg_int *tos;
    struct arg_end *end;
} ping_args;

static bool parse_ip_address(const char *address, ip4_addr_t *ipv4, ip6_addr_t *ipv6)
{
    // Determine if the address is IPv4 or IPv6
    char *indicator = (char *) malloc(strlen(address));
    strcpy(indicator, address);
    while(*indicator++)
    {
        if (*indicator == '.') {
			return inet_aton(address, ipv4);
        }
        else if (*indicator == ':') {
            return inet6_aton(address, &ipv6);
        }
    }
    return false;
}

static esp_err_t parse_args(ip4_addr_t *ip4, ip6_addr_t *ip6, uint32_t *count, uint32_t *timeout,
		uint32_t *delay, uint32_t *packet_size, uint32_t *tos)
{
    if (ping_args.ip_address->count > 0)
    {
        // Parse IP address (Handle v4 and v6)
        bool rc = parse_ip_address(ping_args.ip_address->sval[0], ip4, ip6);
		if (!rc) {
			ESP_LOGE("ping","Error parsing provided IP address...aborting!\n");
			return ESP_FAIL;
		}
    }

    if (ping_args.count->count > 0)
        *count= *ping_args.count->ival;

    if (ping_args.timeout->count > 0)
        *timeout = *ping_args.timeout->ival;

    if (ping_args.delay->count > 0)
		*delay = *ping_args.delay->ival;

	if (ping_args.packet_size->count > 0)
		*packet_size = *ping_args.packet_size->ival;
	if (ping_args.tos->count > 0)
		*tos = *ping_args.tos->ival;

	return ESP_OK;
}

esp_err_t ping_results(ping_target_id_t message_type, esp_ping_found *found_val)
{
    static int ctr;
    ip4_addr_t target_ipv4;
	uint32_t ping_count = 0;
	esp_ping_get_target(PING_TARGET_IP_ADDRESS, &target_ipv4.addr, sizeof(uint32_t));
	esp_ping_get_target(PING_TARGET_IP_ADDRESS_COUNT, &ping_count, sizeof(uint32_t));

	if (found_val->ping_err != 2)
	{
		fprintf(stdout, "%d bytes from %s: icmp_seq=%d time=%d ms\n",
				found_val->bytes, inet_ntoa(target_ipv4), found_val->ping_seqno, found_val->resp_time);
	} else {
		fprintf(stdout, ".");
		fflush(stdout);
	}


    if (++ctr >= ping_count) {
		int32_t packet_loss = ( 1-((found_val->recv_count * 1.0) / found_val->send_count)) * 100;
		fprintf(stdout, "\n--- %s ping statistics ---\n", inet_ntoa(target_ipv4));
		fprintf(stdout, "%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
				found_val->send_count, found_val->recv_count, packet_loss, found_val->total_time);
		fprintf(stdout, "rtt min/avg/max = %d/%1.2f/%d ms\n", found_val->min_time,
				(found_val->total_time * 1.0) / found_val->recv_count, found_val->max_time);
		// clean up time
		memset(found_val, 0, sizeof(esp_ping_found));
		ctr = 0;
	}
	ping_deinit();

    return ESP_OK;
}
static esp_err_t send_icmp(int argc, char **argv)
{
	if (!is_connected)
	{
		ESP_LOGE("ping", "Device is not connected to any AP. Cannot ping host without an IP address\n"
				"Please associate to an SSID and then use ping!\n");
		return ESP_FAIL;
	}
	esp_err_t ret;
    int counter;
    ip4_addr_t target_ipv4;
    // initialize optional arguments to default values
    uint32_t ping_count     = DEFAULT_COUNT;
    uint32_t ping_timeout   = DEFAULT_TIMEOUT;
    uint32_t ping_delay     = DEFAULT_DELAY;
    uint32_t packet_size    = DEFAULT_DATA_SIZE;
    uint32_t ping_tos       = -1;
    // TO DO: IPv6
    ip6_addr_t target_ipv6;

    int nerrors = arg_parse(argc, argv, (void **) &ping_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, ping_args.end, argv[0]);
        return FAILURE;
    }

    ret = parse_args(&target_ipv4, &target_ipv6, &ping_count, &ping_timeout, &ping_delay,
			&packet_size, &ping_tos);

	if (ret == ESP_OK) {
		printf("PING %s (%s) %d(%d) bytes of data.\n", inet_ntoa(target_ipv4),
				inet_ntoa(target_ipv4), packet_size, (packet_size + 28)); // ICMP header length is 28 bytes
		for (counter = 0; counter < ping_count; counter++)
		{
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			esp_ping_set_target(PING_TARGET_IP_ADDRESS_COUNT, &ping_count, sizeof(uint32_t));
			esp_ping_set_target(PING_TARGET_RCV_TIMEO, &ping_timeout, sizeof(uint32_t));
			esp_ping_set_target(PING_TARGET_DELAY_TIME, &ping_delay, sizeof(uint32_t));
			esp_ping_set_target(PING_TARGET_IP_ADDRESS, &target_ipv4.addr, sizeof(uint32_t));
			esp_ping_set_target(PING_TARGET_RES_FN, &ping_results, sizeof(ping_results));
			esp_ping_set_target(PING_TARGET_DATA_LEN, &packet_size, sizeof(uint32_t));
			if (ping_tos != -1)
				esp_ping_set_target(PING_TARGET_IP_TOS, &ping_tos, sizeof(uint32_t));
			ping_init();
		}
	} else
		return ESP_FAIL;
    return ESP_OK;
}

void register_ping(void)
{
    ping_args.ip_address = arg_str0(NULL, NULL, "<IPv4/IPv6>", "Target IP Address");
    ping_args.timeout = arg_int0("t", "timeout", "<t>", "Connection timeout, ms");
    ping_args.count = arg_int0("c", "count", "<n>", "Number of messages");
    ping_args.delay= arg_int0("d", "delay", "<t>", "Delay between messges, ms");
    ping_args.packet_size = arg_int0("s", "size", "<n>", "Packet data size, bytes");
    ping_args.tos = arg_int0(NULL, "tos", "<n>", "Type of service");
    ping_args.end = arg_end(1);
    const esp_console_cmd_t ping_cmd = {
        .command = "ping",
        .help = "Send an ICMP message to an IPv4/IPv6 address",
        .hint = "<IP address>",
        .func = &send_icmp,
        .argtable = &ping_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&ping_cmd) );
}
