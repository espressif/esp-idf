/* SLIP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_slip.h"

#include "lwip/sockets.h"

#include "slip_modem.h"

static const char *TAG = "SLIP_EXAMPLE";

#define STACK_SIZE (10 * 1024)
#define PRIORITY   10

static void udp_rx_tx_task(void *arg)
{
    char addr_str[128];
    uint8_t rx_buff[1024];

    int sock = (int)arg;

    struct sockaddr_storage source_addr;
    socklen_t socklen = sizeof(source_addr);


    ESP_LOGI(TAG, "Starting node manager UDP task");

    while (1) {
        // Receive data
        int len = recvfrom(sock, rx_buff, sizeof(rx_buff) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
        if (len < 0) {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            break;
        }

        // Parse out address to string
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
        } else if (source_addr.ss_family == PF_INET6) {
            inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
        }

        // Force null termination of received data and print
        rx_buff[len] = 0;
        ESP_LOGI(TAG, "Received '%s' from '%s'", rx_buff, addr_str);

        // Send data back
        int err = sendto(sock, rx_buff, len, 0, (struct sockaddr *)&source_addr, socklen);
        if (err < 0) {
            ESP_LOGE(TAG, "sendto failed: errno %d", errno);
            break;
        }
    }

    vTaskDelete(NULL);
}

esp_err_t udp_rx_tx_init(void)
{
    // Setup bind address
    struct sockaddr_in6 dest_addr;
#if CONFIG_EXAMPLE_IPV4
    sa_family_t family = AF_INET;
    int ip_protocol = IPPROTO_IP;
    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(CONFIG_EXAMPLE_UDP_PORT);
    ip_protocol = IPPROTO_IP;
#else
    sa_family_t family = AF_INET6;
    int ip_protocol = IPPROTO_IPV6;
    bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
    dest_addr.sin6_family = family;
    dest_addr.sin6_port = htons(CONFIG_EXAMPLE_UDP_PORT);
#endif

    // Create socket
    int sock = socket(family, SOCK_DGRAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return ESP_FAIL;
    }

    // Disable IPv4 and reuse address
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#if !CONFIG_EXAMPLE_IPV4
    setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
#endif

    // Bind socket
    int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", CONFIG_EXAMPLE_UDP_PORT);


    // Start UDP rx thread
    xTaskCreate(udp_rx_tx_task, "udp_rx_tx", STACK_SIZE, (void *)sock, PRIORITY, NULL);

    return ESP_OK;
}

// Write a prefix to the contiki slip device
static void slip_set_prefix(esp_netif_t *slip_netif)
{
    uint8_t buff[10] = {0};

    // Fetch the slip interface IP
    const esp_ip6_addr_t *addr = esp_slip_get_ip6(slip_netif);

    ESP_LOGI(TAG, "%s: prefix set (%08x:%08x)", __func__,
             lwip_ntohl(addr->addr[0]), lwip_ntohl(addr->addr[1]));

    // Build slip set message
    buff[0] = '!';
    buff[1] = 'P';
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            buff[2 + i * 4 + j] = addr->addr[i] >> (j * 8);
        }
    }

    // Write raw data out the slip interface
    esp_netif_lwip_slip_raw_output(slip_netif, buff, 2 + 8);
}

// slip_rx_filter filters incoming commands from the slip interface
// this implementation is designed for use with contiki slip devices
bool slip_rx_filter(void *ctx, uint8_t *data, uint32_t len)
{

    esp_netif_t *slip_netif = (esp_netif_t *)ctx;

    if (data[1] == '?') {
        switch (data[2]) {
        case 'P':
            ESP_LOGI(TAG, "Prefix request");
            slip_set_prefix(slip_netif);

            return true;

        default:
            ESP_LOGI(TAG, "Unhandled request '%c'", data[2]);
            break;
        }

        return true;

    } else if (data[1] == '!') {
        switch (data[2]) {
        default:
            ESP_LOGI(TAG, "Unhandled command '%c'", data[2]);
            break;
        }
    }

    return false;
}

#if CONFIG_EXAMPLE_IPV4
static const esp_netif_ip_info_t s_slip_ip4 = {
        .ip = { .addr = ESP_IP4TOADDR( 10, 0, 0, 2) },
};
#endif

// Initialise the SLIP interface
esp_netif_t *slip_if_init(void)
{
    ESP_LOGI(TAG, "Initialising SLIP interface");

    esp_netif_inherent_config_t base_cfg = ESP_NETIF_INHERENT_DEFAULT_SLIP()
#if CONFIG_EXAMPLE_IPV4
    base_cfg.ip_info = &s_slip_ip4;
#endif
    esp_netif_config_t cfg = {          .base = &base_cfg,
                                        .driver = NULL,
                                        .stack = ESP_NETIF_NETSTACK_DEFAULT_SLIP };

    esp_netif_t *slip_netif = esp_netif_new(&cfg);

    esp_netif_slip_config_t slip_config;

    IP6_ADDR(&slip_config.ip6_addr,
             lwip_htonl(0xfd0000),
             lwip_htonl(0x00000000),
             lwip_htonl(0x00000000),
             lwip_htonl(0x00000001)
            );

    esp_netif_slip_set_params(slip_netif, &slip_config);

    ESP_LOGI(TAG, "Initialising SLIP modem");

    esp_slip_modem_config_t modem_cfg = {
        .uart_dev = UART_NUM_1,

        .uart_tx_pin = CONFIG_EXAMPLE_UART_TX_PIN,
        .uart_rx_pin = CONFIG_EXAMPLE_UART_RX_PIN,
        .uart_baud = CONFIG_EXAMPLE_UART_BAUD,

        .rx_buffer_len = 1024,

        .rx_filter = slip_rx_filter,
        .rx_filter_ctx = slip_netif,
    };

    void *slip_modem = esp_slip_modem_create(slip_netif, &modem_cfg);
    ESP_ERROR_CHECK(esp_netif_attach(slip_netif, slip_modem));

    ESP_LOGI(TAG, "SLIP init complete");

    return slip_netif;
}

void app_main(void)
{
    // Setup networking
    esp_netif_init();

    esp_log_level_set("*", ESP_LOG_DEBUG);

    // Create event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Setup slip interface
    slip_if_init();

    // Setup UDP loopback service
    udp_rx_tx_init();
}
