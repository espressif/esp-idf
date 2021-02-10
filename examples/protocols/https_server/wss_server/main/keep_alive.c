/* Keep Alive engine for wss server example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "keep_alive.h"
#include "esp_timer.h"

typedef enum {
    NO_CLIENT = 0,
    CLIENT_FD_ADD,
    CLIENT_FD_REMOVE,
    CLIENT_UPDATE,
    CLIENT_ACTIVE,
    STOP_TASK,
} client_fd_action_type_t;

typedef struct {
    client_fd_action_type_t type;
    int fd;
    uint64_t last_seen;
} client_fd_action_t;

typedef struct wss_keep_alive_storage {
    size_t max_clients;
    wss_check_client_alive_cb_t check_client_alive_cb;
    wss_check_client_alive_cb_t client_not_alive_cb;
    size_t keep_alive_period_ms;
    size_t not_alive_after_ms;
    void * user_ctx;
    QueueHandle_t q;
    client_fd_action_t clients[];
} wss_keep_alive_storage_t;

typedef struct wss_keep_alive_storage* wss_keep_alive_t;

static const char *TAG = "wss_keep_alive";

static uint64_t _tick_get_ms(void)
{
    return esp_timer_get_time()/1000;
}

// Goes over active clients to find out how long we could sleep before checking who's alive
static uint64_t get_max_delay(wss_keep_alive_t h)
{
    int64_t check_after_ms = 30000; // max delay, no need to check anyone
    for (int i=0; i<h->max_clients; ++i) {
        if (h->clients[i].type == CLIENT_ACTIVE) {
            uint64_t check_this_client_at = h->clients[i].last_seen + h->keep_alive_period_ms;
            if (check_this_client_at < check_after_ms + _tick_get_ms()) {
                check_after_ms = check_this_client_at - _tick_get_ms();
                if (check_after_ms < 0) {
                    check_after_ms = 1000; // min delay, some client(s) not responding already
                }
            }
        }
    }
    return check_after_ms;
}


static bool update_client(wss_keep_alive_t h, int sockfd, uint64_t timestamp)
{
    for (int i=0; i<h->max_clients; ++i) {
        if (h->clients[i].type == CLIENT_ACTIVE && h->clients[i].fd == sockfd) {
            h->clients[i].last_seen = timestamp;
            return true;
        }
    }
    return false;
}

static bool remove_client(wss_keep_alive_t h, int sockfd)
{
    for (int i=0; i<h->max_clients; ++i) {
        if (h->clients[i].type == CLIENT_ACTIVE && h->clients[i].fd == sockfd) {
            h->clients[i].type = NO_CLIENT;
            h->clients[i].fd = -1;
            return true;
        }
    }
    return false;
}
static bool add_new_client(wss_keep_alive_t h,int sockfd)
{
    for (int i=0; i<h->max_clients; ++i) {
        if (h->clients[i].type == NO_CLIENT) {
            h->clients[i].type = CLIENT_ACTIVE;
            h->clients[i].fd = sockfd;
            h->clients[i].last_seen = _tick_get_ms();
            return true; // success
        }
    }
    return false;
}

static void keep_alive_task(void* arg)
{
    wss_keep_alive_storage_t *keep_alive_storage = arg;
    bool run_task = true;
    client_fd_action_t client_action;
    while (run_task) {
        if (xQueueReceive(keep_alive_storage->q, (void *) &client_action,
                get_max_delay(keep_alive_storage) / portTICK_PERIOD_MS) == pdTRUE) {
            switch (client_action.type) {
                case CLIENT_FD_ADD:
                    if (!add_new_client(keep_alive_storage, client_action.fd)) {
                        ESP_LOGE(TAG, "Cannot add new client");
                    }
                    break;
                case CLIENT_FD_REMOVE:
                    if (!remove_client(keep_alive_storage, client_action.fd)) {
                        ESP_LOGE(TAG, "Cannot remove client fd:%d", client_action.fd);
                    }
                    break;
                case CLIENT_UPDATE:
                    if (!update_client(keep_alive_storage, client_action.fd, client_action.last_seen)) {
                        ESP_LOGE(TAG, "Cannot find client fd:%d", client_action.fd);
                    }
                    break;
                case STOP_TASK:
                    run_task = false;
                    break;
                default:
                    ESP_LOGE(TAG, "Unexpected client action");
                    break;
            }
        } else {
                // timeout: check if PING message needed
                for (int i=0; i<keep_alive_storage->max_clients; ++i) {
                    if (keep_alive_storage->clients[i].type == CLIENT_ACTIVE) {
                        if (keep_alive_storage->clients[i].last_seen + keep_alive_storage->keep_alive_period_ms <= _tick_get_ms()) {
                            ESP_LOGD(TAG, "Haven't seen the client (fd=%d) for a while", keep_alive_storage->clients[i].fd);
                            if (keep_alive_storage->clients[i].last_seen + keep_alive_storage->not_alive_after_ms <= _tick_get_ms()) {
                                ESP_LOGE(TAG, "Client (fd=%d) not alive!",  keep_alive_storage->clients[i].fd);
                                keep_alive_storage->client_not_alive_cb(keep_alive_storage, keep_alive_storage->clients[i].fd);
                            } else {
                                keep_alive_storage->check_client_alive_cb(keep_alive_storage, keep_alive_storage->clients[i].fd);
                            }
                        }
                    }
                }
            }
    }
    vQueueDelete(keep_alive_storage->q);
    free(keep_alive_storage);

    vTaskDelete(NULL);
}

wss_keep_alive_t wss_keep_alive_start(wss_keep_alive_config_t *config)
{
    size_t queue_size = config->max_clients/2;
    size_t client_list_size = config->max_clients + queue_size;
    wss_keep_alive_t keep_alive_storage = calloc(1,
            sizeof(wss_keep_alive_storage_t) + client_list_size* sizeof(client_fd_action_t));
    if (keep_alive_storage == NULL) {
        return false;
    }
    keep_alive_storage->check_client_alive_cb = config->check_client_alive_cb;
    keep_alive_storage->client_not_alive_cb = config->client_not_alive_cb;
    keep_alive_storage->max_clients = config->max_clients;
    keep_alive_storage->not_alive_after_ms = config->not_alive_after_ms;
    keep_alive_storage->keep_alive_period_ms = config->keep_alive_period_ms;
    keep_alive_storage->user_ctx = config->user_ctx;
    keep_alive_storage->q =  xQueueCreate(queue_size, sizeof(client_fd_action_t));
    if (xTaskCreate(keep_alive_task, "keep_alive_task", config->task_stack_size,
                    keep_alive_storage, config->task_prio, NULL) != pdTRUE) {
        wss_keep_alive_stop(keep_alive_storage);
        return false;
    }
    return keep_alive_storage;
}

void wss_keep_alive_stop(wss_keep_alive_t h)
{
    client_fd_action_t stop = { .type = STOP_TASK };
    xQueueSendToBack(h->q, &stop, 0);
    // internal structs will be de-allocated in the task
}

esp_err_t wss_keep_alive_add_client(wss_keep_alive_t h, int fd)
{
    client_fd_action_t client_fd_action = { .fd = fd, .type = CLIENT_FD_ADD};
    if (xQueueSendToBack(h->q, &client_fd_action, 0) == pdTRUE) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t wss_keep_alive_remove_client(wss_keep_alive_t h, int fd)
{
    client_fd_action_t client_fd_action = { .fd = fd, .type = CLIENT_FD_REMOVE};
    if (xQueueSendToBack(h->q, &client_fd_action, 0) == pdTRUE) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t wss_keep_alive_client_is_active(wss_keep_alive_t h, int fd)
{
    client_fd_action_t client_fd_action = { .fd = fd, .type = CLIENT_UPDATE,
                                            .last_seen = _tick_get_ms()};
    if (xQueueSendToBack(h->q, &client_fd_action, 0) == pdTRUE) {
        return ESP_OK;
    }
    return ESP_FAIL;

}

void wss_keep_alive_set_user_ctx(wss_keep_alive_t h, void *ctx)
{
    h->user_ctx = ctx;
}

void* wss_keep_alive_get_user_ctx(wss_keep_alive_t h)
{
    return h->user_ctx;
}
