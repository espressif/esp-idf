/* OpenThread Command Line Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_ot_tcp_socket.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#define TAG "ot_socket"

static void tcp_socket_server_task(void *pvParameters)
{
    char addr_str[128];
    char rx_buffer[128];
    esp_err_t ret = ESP_OK;
    int err = 0;
    int len = 0;
    int listen_sock;
    int opt = 1;
    int port = CONFIG_OPENTHREAD_CLI_TCP_SERVER_PORT;
    int client_sock = 0;
    struct timeval timeout = { 0 };
    struct sockaddr_storage source_addr; // Large enough for both IPv6
    struct sockaddr_in6 listen_addr = { 0 };
    // The TCP server listen at the address "::", which means all addresses can be listened to.
    inet6_aton("::", &listen_addr.sin6_addr);
    listen_addr.sin6_family = AF_INET6;
    listen_addr.sin6_port = htons(port);

    listen_sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6);
    ESP_GOTO_ON_FALSE((listen_sock >= 0), ESP_OK, exit, TAG, "Unable to create socket: errno %d", errno);

    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // Note that by default IPV6 binds to both protocols, it is must be disabled
    // if both protocols used at the same time (used in CI)
    setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    err = bind(listen_sock, (struct sockaddr *)&listen_addr, sizeof(struct sockaddr_in6) );
    ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Socket unable to bind: errno %d, IPPROTO: %d", errno, AF_INET6);
    ESP_LOGI(TAG, "Socket bound, port %d", port);

    err = listen(listen_sock, 1);
    ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Error occurred during listen: errno %d", errno);

    //blocking-mode accept, set timeout 30 seconds
    timeout.tv_sec = 30;
    setsockopt(listen_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    ESP_LOGI(TAG, "Socket listening, timeout is 30 seconds");

    socklen_t addr_len = sizeof(source_addr);
    client_sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
    ESP_GOTO_ON_FALSE((client_sock >= 0), ESP_FAIL, exit, TAG, "Unable to accept connection: errno %d", errno);

    ESP_GOTO_ON_FALSE((err >= 0), ESP_FAIL, exit, TAG, "Error occurred during sending: errno %d", errno);

    //blocking-mode receive, set timeout 30 seconds
    timeout.tv_sec = 30;
    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    len = recv(client_sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    // Error occurred during receiving
    ESP_GOTO_ON_FALSE((len >= 0), ESP_FAIL, exit, TAG, "recv failed: errno %d", errno);
    // Data received
    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
    ESP_LOGI(TAG, "Received %d bytes from client:", len);
    ESP_LOGI(TAG, "%s", rx_buffer);

    // Convert ip address to string
    inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
    ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);
exit:
    if (ret != ESP_OK || listen_sock != -1) {
        shutdown(listen_sock, 0);
        close(listen_sock);
    }
    if (client_sock != -1) {
        shutdown(client_sock, 0);
        close(client_sock);
    }
    ESP_LOGI(TAG, "Socket server is closed.");
    vTaskDelete(NULL);
}

static void tcp_socket_client_task(void *pvParameters)
{
    char *host_ip = (char *)pvParameters;
    char *payload = "This message is from client\n";
    char rx_buffer[128];
    esp_err_t ret = ESP_OK;
    int client_sock;
    int err = 0;
    int len = 0;
    int port = CONFIG_OPENTHREAD_CLI_TCP_SERVER_PORT;
    struct sockaddr_in6 dest_addr = { 0 };

    inet6_aton(host_ip, &dest_addr.sin6_addr);
    dest_addr.sin6_family = AF_INET6;
    dest_addr.sin6_port = htons(port);

    client_sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6);
    ESP_GOTO_ON_FALSE((client_sock >= 0), ESP_OK, exit, TAG, "Unable to create socket: errno %d", errno);

    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, port);

    err = connect(client_sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
    ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Socket unable to connect: errno %d", errno);
    ESP_LOGI(TAG, "Successfully connected");

    len = send(client_sock, payload, strlen(payload), 0);
    ESP_GOTO_ON_FALSE((len >= 0), ESP_FAIL, exit, TAG, "Error occurred during sending: errno %d", errno);

    len = recv(client_sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    // Error occurred during receiving
    ESP_GOTO_ON_FALSE((len >= 0), ESP_FAIL, exit, TAG, "recv failed: errno %d", errno);
    // Data received
    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
    ESP_LOGI(TAG, "Received %d bytes from %s", len, host_ip);
    ESP_LOGI(TAG, "%s", rx_buffer);

exit:
    if (ret != ESP_OK || client_sock != -1) {
        shutdown(client_sock, 0);
        close(client_sock);
    }
    ESP_LOGI(TAG, "Socket client is closed.");
    vTaskDelete(NULL);
}

void esp_ot_process_tcp_server(void *aContext, uint8_t aArgsLength, char *aArgs[])
{
    (void)(aContext);
    (void)(aArgsLength);
    (void)(*aArgs);
    xTaskCreate(tcp_socket_server_task, "ot_tcp_scoket_server", 4096, xTaskGetCurrentTaskHandle(), 4, NULL);
}

void esp_ot_process_tcp_client(void *aContext, uint8_t aArgsLength, char *aArgs[])
{
    (void)(aContext);
    (void)(aArgsLength);
    if (aArgsLength == 0) {
        ESP_LOGE(TAG, "Invalid arguments.");
    } else {
        xTaskCreate(tcp_socket_client_task, "ot_tcp_socket_client", 4096, aArgs[0], 4, NULL);
    }
}
