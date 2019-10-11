/* Net-suite test code

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "linenoise/linenoise.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
void nettestif_input(void *buffer, u16_t len);

/* these data configures ARP cache so the test IPs are knows */
static char arp1[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 
    0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a, 0x00, 0x00, 0x01
};

/* Test data (ICMP packet) for verification of tcp ip test netif
    00-00-00-00-00-01-00-00-00-00-00-02-08-00-45-00-00-1c-00-00-00-00-ff-01-a7-de-0a-00-00-02-0a-00-00-01-08-00-f7-fd-00-01-00-01
*/

/* creating test pcb */
static struct tcp_pcb *test_pcb;

err_t test_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    return ERR_OK;
}

void test_error(void *arg, err_t err)
{
    printf("Error CB from pcb %d\n", err);
}

err_t test_poll(void *arg, struct tcp_pcb *tpcb)
{
    return ERR_OK;
}

err_t test_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

    tcp_setprio(newpcb, TCP_PRIO_MIN);
    tcp_arg(newpcb, NULL);
    tcp_recv(newpcb, test_recv);
    tcp_err(newpcb, test_error);
    tcp_poll(newpcb, test_poll, 0);

    return ERR_OK;
}

void test_tcp_init(void)
{
    test_pcb = tcp_new();
    if (test_pcb != NULL) {
        err_t err;
        /* Binding this test_pcb to 4242 to accept connections on this port
         * - this has to be configured as DUT endpoint
         * - all network traffic from and to network stack is tracked in nettestif
         */
        err = tcp_bind(test_pcb, IP_ADDR_ANY, 4242);
        if (err == ERR_OK) {
            test_pcb = tcp_listen(test_pcb);
            tcp_accept(test_pcb, test_accept);
        } else  {
            printf("cannot bind test_pcb\n");
            abort();
        }
    } else  {
        printf("cannot create test_pcb\n");
        abort();
    }
}

/**
 * @brief Process line read from serial input, character by character
 *
 * Converts from hex string to byte stream, so it can be processed
 * in test network interface
 *
 * @param line
 * @param packet
 *
 * @return size of packet
 */
static size_t process_line(char* line, char* packet)
{
    size_t count = 0;
    size_t i;

    for (i=0; i< strlen(line); i++) {
        char c = line[i];
        // accept both separators between bytes
        if (c == '-' || c == ' ') {
            ++count;
        // Processing numeric characters
        } else if (c >= '0' && c <= '9') {
            packet[count] *= 16;
            packet[count] += c - '0';
        // Processing alpha-numeric hex characters
        } else if (c >= 'a' && c <= 'f') {
            packet[count] *= 16;
            packet[count] += c - 'a' + 10;
        }
    }

    if (i>0 && strlen(line)>0) {
        count++;
    }
    return count;
}

void app_main(void)
{
    char packet[128];

    tcpip_adapter_ip_info_t ip_info;

    uint8_t ap_mac[6] = { 0,0,0,0,0,1};
    IP4_ADDR(&ip_info.ip, 10, 0 , 0, 1);
    IP4_ADDR(&ip_info.gw, 10, 0 , 0, 1);
    IP4_ADDR(&ip_info.netmask, 255, 255 , 255, 0);    

    tcpip_adapter_init();

    tcpip_adapter_test_start(ap_mac, &ip_info);

    // initializes TCP endpoint on DUT per https://github.com/intel/net-test-suites#21-endpoints
    test_tcp_init();
    // Inject ARP packet to let the network stack know about IP/MAC of the counterpart
    nettestif_input(arp1, sizeof(arp1));
    // Initialize VFS & UART so we can use std::cout/cin
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install( (uart_port_t)CONFIG_CONSOLE_UART_NUM,
            256, 0, 0, NULL, 0) );
    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_CONSOLE_UART_NUM);
    esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);
    linenoiseSetDumbMode(1);

    /* Now read from stdin and pass the data to test netif */
    while (1) {
        size_t size;
        char* line = linenoise("");
        if (!line) {
            continue;
        }

        size = process_line(line, packet);

        nettestif_input(packet, size);

        linenoiseFree(line);
    }

}
