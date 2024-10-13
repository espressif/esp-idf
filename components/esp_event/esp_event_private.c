/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_event_private.h"
#include "esp_event_internal.h"

#include "esp_log.h"

bool esp_event_is_handler_registered(esp_event_loop_handle_t event_loop, esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler)
{
    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    bool result = false;

    esp_event_loop_node_t* loop_node;
    esp_event_base_node_t* base_node;
    esp_event_id_node_t* id_node;
    esp_event_handler_node_t* handler;

    SLIST_FOREACH(loop_node, &(loop->loop_nodes), next) {
        SLIST_FOREACH(handler, &(loop_node->handlers), next) {
            if (event_base == ESP_EVENT_ANY_BASE && event_id == ESP_EVENT_ANY_ID && handler->handler_ctx->handler == event_handler) {
                result = true;
                goto out;
            }
        }

        SLIST_FOREACH(base_node, &(loop_node->base_nodes), next) {
            if (base_node->base == event_base) {
                SLIST_FOREACH(handler, &(base_node->handlers), next) {
                    if (event_id == ESP_EVENT_ANY_ID && handler->handler_ctx->handler == event_handler) {
                        result = true;
                        goto out;
                    }
                }

                SLIST_FOREACH(id_node, &(base_node->id_nodes), next) {
                    if (id_node->id == event_id) {
                        SLIST_FOREACH(handler, &(id_node->handlers), next) {
                            if (handler->handler_ctx->handler == event_handler) {
                                result = true;
                                goto out;
                            }
                        }
                    }
                }
            }
        }
    }

out:
    xSemaphoreGive(loop->mutex);
    return result;
}
