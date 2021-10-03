/* OpenThread Command Line Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_ot_udp_socket.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#define TAG "ot_socket"

static void udp_socket_server_task(void *pvParameters)
{
    char addr_str[128];
    char *payload = "This message is from server\n";
    char rx_buffer[128];
    esp_err_t ret = ESP_OK;
    int err = 0;
    int len;
    int listen_sock;

    int port = CONFIG_OPENTHREAD_CLI_UDP_SERVER_PORT;
    struct timeval timeout = { 0 };
    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
    struct sockaddr_in6 listen_addr;

    inet6_aton("::", &listen_addr.sin6_addr);
    listen_addr.sin6_family = AF_INET6;
    listen_addr.sin6_port = htons(port);

    listen_sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    ESP_GOTO_ON_FALSE((listen_sock >= 0), ESP_OK, exit, TAG, "Unable to create socket: errno %d", errno);
    ESP_LOGI(TAG, "Socket created");

    // Note that by default IPV6 binds to both protocols, it is must be disabled
    // if both protocols used at the same time (used in CI)
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));

    err = bind(listen_sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr));
    ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Socket unable to bind: errno %d", errno);
    ESP_LOGI(TAG, "Socket bound, port %d", port);

    timeout.tv_sec = 30;
    setsockopt(listen_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    ESP_LOGI(TAG, "Waiting for data, timeout is 30 seconds");
    socklen_t socklen = sizeof(source_addr);
    len = recvfrom(listen_sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

    // Error occurred during receiving
    ESP_GOTO_ON_FALSE((len >= 0), ESP_FAIL, exit, TAG, "recvfrom failed: errno %d", errno);
    // Data received
    // Get the sender's ip address as string
    inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);

    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
    ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
    ESP_LOGI(TAG, "%s", rx_buffer);

    err = sendto(listen_sock, payload, strlen(payload), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
    ESP_GOTO_ON_FALSE((err >= 0), ESP_FAIL, exit, TAG, "Error occurred during sending: errno %d", errno);
exit:
    if (ret != ESP_OK || listen_sock != -1) {
        shutdown(listen_sock, 0);
        close(listen_sock);
    }
    ESP_LOGI(TAG, "Socket server is closed.");
    vTaskDelete(NULL);
}

static void udp_socket_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char *host_ip = (char *)pvParameters;
    char *payload = "This message is from client\n";
    int client_sock;
    int port = CONFIG_OPENTHREAD_CLI_UDP_SERVER_PORT;
    int err = 0;
    int len;
    esp_err_t ret = ESP_OK;
    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
    struct sockaddr_in6 dest_addr = { 0 };

    inet6_aton(host_ip, &dest_addr.sin6_addr);
    dest_addr.sin6_family = AF_INET6;
    dest_addr.sin6_port = htons(port);

    client_sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    ESP_GOTO_ON_FALSE((client_sock >= 0), ESP_OK, exit, TAG, "Unable to create socket: errno %d", errno);
    ESP_LOGI(TAG, "Socket created, sending to %s:%d", host_ip, port);

    err = sendto(client_sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    ESP_GOTO_ON_FALSE((err >= 0), ESP_FAIL, exit, TAG, "Error occurred during sending: errno %d", errno);
    ESP_LOGI(TAG, "Message sent");

    socklen_t socklen = sizeof(source_addr);
    len = recvfrom(client_sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

    // Error occurred during receiving
    ESP_GOTO_ON_FALSE((len >= 0), ESP_FAIL, exit, TAG, "recvfrom failed: errno %d", errno);
    // Data received
    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
    ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
    ESP_LOGI(TAG, "%s", rx_buffer);
exit:
    if (ret != ESP_OK || client_sock != -1) {
        shutdown(client_sock, 0);
        close(client_sock);
    }
    ESP_LOGI(TAG, "Socket client is closed");
    vTaskDelete(NULL);
}

void esp_ot_process_udp_server(void *aContext, uint8_t aArgsLength, char *aArgs[])
{
    (void)(aContext);
    (void)(aArgsLength);
    xTaskCreate(udp_socket_server_task, "ot_udp_scoket_server", 4096, xTaskGetCurrentTaskHandle(), 4, NULL);
}

void esp_ot_process_udp_client(void *aContext, uint8_t aArgsLength, char *aArgs[])
{
    (void)(aContext);
    (void)(aArgsLength);
    if (aArgsLength == 0) {
        ESP_LOGE(TAG, "Invalid arguments.");
    } else {
        xTaskCreate(udp_socket_client_task, "ot_udp_socket_client", 4096, aArgs[0], 4, NULL);
    }
}
