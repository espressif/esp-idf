/* ASIO chat server client example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "protocol_examples_common.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "server.hpp"
#include "client.hpp"
#include <thread>
#include <pthread.h>

using asio::ip::tcp;

static const char *TAG = "asio-chat";

// This variable is necessary for `python test` execution, it provides synchronisation between server/client(as server should be started before client)
std::mutex server_ready;

#ifdef CONFIG_EXAMPLE_CHAT_CLIENT
static void get_string(char *line, size_t size)
{
    int count = 0;
    while (count < size) {
        int c = fgetc(stdin);
        if (c == '\n') {
            line[count] = '\0';
            break;
        } else if (c > 0 && c < 127) {
            line[count] = c;
            ++count;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void start_client(void)
{
    const std::string port(CONFIG_EXAMPLE_CHAT_CLIENT_CONNECT_PORT);
    const std::string name(CONFIG_EXAMPLE_CHAT_CLIENT_CONNECT_ADDRESS);
    asio::io_context io_context;
    char line[128];

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(name, port);
    chat_client c(io_context, endpoints);
#ifdef CONFIG_EXAMPLE_CHAT_SERVER
    std::lock_guard<std::mutex> guard(server_ready);
#endif
    std::thread t([&io_context]() { try {
                                        io_context.run();
                                  } catch (const std::exception &e) {
                                        ESP_LOGE(TAG, "Exception occured during client thread execution %s", e.what());
                                  }
                                    catch (...) {
                                         ESP_LOGE(TAG, "Unknown exception");
                                   }});
    do {
        ESP_LOGI(TAG, "CLIENT: Waiting for input");
        get_string(line, sizeof(line));

        chat_message msg;
        msg.body_length(std::strlen(line));
        std::memcpy(msg.body(), line, msg.body_length());
        msg.encode_header();
        c.write(msg);
        sleep(1);
    } while (strcmp(line, "exit") != 0);

    c.close();
    t.join();
}
#endif // CONFIG_EXAMPLE_CHAT_CLIENT

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    try {
#ifdef CONFIG_EXAMPLE_CHAT_SERVER
        asio::io_context io_context;
        chat_server server(io_context, tcp::endpoint(tcp::v4(), std::atoi(CONFIG_EXAMPLE_CHAT_SERVER_BIND_PORT)));
        std::thread t =  std::thread([&io_context]() { // Chat server starting here
                                     try {
                                           io_context.run();
                                     } catch (const std::exception &e) {
                                           ESP_LOGE(TAG, "Exception occured during server thread execution %s", e.what());
                                     }
                                       catch (...) {
                                           ESP_LOGE(TAG, "Unknown exception");
                                     }});;
#endif
#ifdef CONFIG_EXAMPLE_CHAT_CLIENT
        start_client();
#endif
#ifdef CONFIG_EXAMPLE_CHAT_SERVER
        t.join();
#endif
    } catch (const std::exception &e) {
        ESP_LOGE(TAG, "Exception occured during run %s", e.what());
    } catch (...) {
        ESP_LOGE(TAG, "Unknown exception");
    }
    ESP_ERROR_CHECK(example_disconnect());
}
