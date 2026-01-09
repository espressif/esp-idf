/*
 * SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "esp_attr.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#include "esp_event.h"
#include "esp_event_internal.h"
#include "esp_event_private.h"

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
#include "esp_timer.h"
#endif

/* ---------------------------- Definitions --------------------------------- */

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
// LOOP @<address, name> rx:<received events no.> dr:<dropped events no.>
#define LOOP_DUMP_FORMAT              "LOOP @%p,%s rx:%" PRIu32 " dr:%" PRIu32 "\n"
// handler @<address> ev:<base, id> inv:<times invoked> time:<runtime>
#define HANDLER_DUMP_FORMAT           "  HANDLER @%p ev:%s,%s inv:%" PRIu32 " time:%lld us\n"

#define PRINT_DUMP_INFO(dst, sz, ...)  do { \
                                            int cb = snprintf(dst, sz, __VA_ARGS__); \
                                            dst += cb; \
                                            sz -= cb; \
                                        } while(0);
#endif

/* ------------------------- Static Variables ------------------------------- */

static const char* TAG = "event";
static const char* esp_event_any_base = "any";
static const char* esp_event_handler_cleanup = "cleanup";

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
static SLIST_HEAD(esp_event_loop_instance_list_t, esp_event_loop_instance) s_event_loops =
    SLIST_HEAD_INITIALIZER(s_event_loops);

static portMUX_TYPE s_event_loops_spinlock = portMUX_INITIALIZER_UNLOCKED;
#endif

/* ------------------------- Static Functions ------------------------------- */

FORCE_INLINE_ATTR void *esp_event_calloc(size_t n, size_t size)
{
    uint32_t caps = MALLOC_CAP_DEFAULT;
    /* to enable this config, SPIRAM has to be enabled too,
     * which ensure that a heap in external RAM exists */
#if CONFIG_ESP_EVENT_LOOP_IN_EXT_RAM
    caps = MALLOC_CAP_SPIRAM;
#endif
    return heap_caps_calloc(n, size, caps);
}

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING

static int esp_event_dump_prepare(void)
{
    esp_event_loop_instance_t* loop_it;
    esp_event_loop_node_t *loop_node_it;
    esp_event_base_node_t* base_node_it;
    esp_event_id_node_t* id_node_it;
    esp_event_handler_node_t* handler_it;

    // Count the number of items to be printed. This is needed to compute how much memory to reserve.
    int loops = 0, handlers = 0;

    portENTER_CRITICAL(&s_event_loops_spinlock);

    SLIST_FOREACH(loop_it, &s_event_loops, next) {
        SLIST_FOREACH(loop_node_it, &(loop_it->loop_nodes), next) {
            SLIST_FOREACH(handler_it, &(loop_node_it->handlers), next) {
                handlers++;
            }

            SLIST_FOREACH(base_node_it, &(loop_node_it->base_nodes), next) {
                SLIST_FOREACH(handler_it, &(base_node_it->handlers), next) {
                    handlers++;
                }
                SLIST_FOREACH(id_node_it, &(base_node_it->id_nodes), next) {
                    SLIST_FOREACH(handler_it, &(id_node_it->handlers), next) {
                        handlers++;
                    }
                }
            }
        }
        loops++;
    }

    portEXIT_CRITICAL(&s_event_loops_spinlock);

    // Reserve slightly more memory than computed
    int allowance = 3;
    int size = (((loops + allowance) * (sizeof(LOOP_DUMP_FORMAT) + 10 + 20 + 2 * 11)) +
                ((handlers + allowance) * (sizeof(HANDLER_DUMP_FORMAT) + 10 + 2 * 20 + 11 + 20)));

    return size;
}
#endif

static void esp_event_loop_run_task(void* args)
{
    esp_err_t err;
    esp_event_loop_handle_t event_loop = (esp_event_loop_handle_t) args;

    ESP_LOGD(TAG, "running task for loop %p", event_loop);

    while (1) {
        err = esp_event_loop_run(event_loop, portMAX_DELAY);
        if (err != ESP_OK) {
            break;
        }
    }

    ESP_LOGE(TAG, "suspended task for loop %p", event_loop);
    vTaskSuspend(NULL);
}

static void handler_execute(esp_event_loop_instance_t* loop, esp_event_handler_node_t *handler, esp_event_post_instance_t post)
{
    ESP_LOGD(TAG, "running post %s:%"PRIu32" with handler %p and context %p on loop %p", post.base, post.id, handler->handler_ctx->handler, &handler->handler_ctx, loop);

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
    int64_t start, diff;
    start = esp_timer_get_time();
#endif
    // Execute the handler
#if CONFIG_ESP_EVENT_POST_FROM_ISR
    void* data_ptr = NULL;

    if (post.data_set) {
        if (post.data_allocated) {
            data_ptr = post.data.ptr;
        } else {
            data_ptr = &post.data.val;
        }
    }

    (*(handler->handler_ctx->handler))(handler->handler_ctx->arg, post.base, post.id, data_ptr);
#else
    (*(handler->handler_ctx->handler))(handler->handler_ctx->arg, post.base, post.id, post.data.ptr);
#endif

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
    diff = esp_timer_get_time() - start;

    handler->invoked++;
    handler->time += diff;
#endif
}

static esp_err_t handler_instances_add(esp_event_handler_nodes_t* handlers, esp_event_handler_t event_handler, void* event_handler_arg, esp_event_handler_instance_context_t **handler_ctx, bool legacy)
{
    esp_event_handler_node_t *handler_instance = esp_event_calloc(1, sizeof(*handler_instance));

    if (!handler_instance) {
        return ESP_ERR_NO_MEM;
    }

    esp_event_handler_instance_context_t *context = esp_event_calloc(1, sizeof(*context));

    if (!context) {
        free(handler_instance);
        return ESP_ERR_NO_MEM;
    }

    context->handler = event_handler;
    context->arg = event_handler_arg;
    handler_instance->handler_ctx = context;

    if (SLIST_EMPTY(handlers)) {
        SLIST_INSERT_HEAD(handlers, handler_instance, next);
    } else {
        esp_event_handler_node_t *it = NULL, *last = NULL;

        SLIST_FOREACH(it, handlers, next) {
            if (legacy) {
                if (event_handler == it->handler_ctx->handler) {
                    it->handler_ctx->arg = event_handler_arg;
                    ESP_LOGW(TAG, "handler already registered, overwriting");
                    free(handler_instance);
                    free(context);
                    return ESP_OK;
                }
            }
            last = it;
        }

        SLIST_INSERT_AFTER(last, handler_instance, next);
    }

    // If the caller didn't provide the handler instance context, don't set it.
    // It will be removed once the event loop is deleted.
    if (handler_ctx) {
        *handler_ctx = context;
    }

    return ESP_OK;
}

static esp_err_t base_node_add_handler(esp_event_base_node_t* base_node,
                                       int32_t id,
                                       esp_event_handler_t event_handler,
                                       void *event_handler_arg,
                                       esp_event_handler_instance_context_t **handler_ctx,
                                       bool legacy)
{
    if (id == ESP_EVENT_ANY_ID) {
        return handler_instances_add(&(base_node->handlers), event_handler, event_handler_arg, handler_ctx, legacy);
    } else {
        esp_err_t err = ESP_OK;
        esp_event_id_node_t *it = NULL, *id_node = NULL, *last_id_node = NULL;

        SLIST_FOREACH(it, &(base_node->id_nodes), next) {
            if (it->id == id) {
                id_node = it;
            }
            last_id_node = it;
        }

        if (!last_id_node || !id_node) {
            id_node = (esp_event_id_node_t*) esp_event_calloc(1, sizeof(*id_node));

            if (!id_node) {
                ESP_LOGE(TAG, "alloc for new id node failed");
                return ESP_ERR_NO_MEM;
            }

            id_node->id = id;

            SLIST_INIT(&(id_node->handlers));

            err = handler_instances_add(&(id_node->handlers), event_handler, event_handler_arg, handler_ctx, legacy);

            if (err == ESP_OK) {
                if (!last_id_node) {
                    SLIST_INSERT_HEAD(&(base_node->id_nodes), id_node, next);
                } else {
                    SLIST_INSERT_AFTER(last_id_node, id_node, next);
                }
            } else {
                free(id_node);
            }

            return err;
        } else {
            return handler_instances_add(&(id_node->handlers), event_handler, event_handler_arg, handler_ctx, legacy);
        }
    }
}

static esp_err_t loop_node_add_handler(esp_event_loop_node_t* loop_node,
                                       esp_event_base_t base,
                                       int32_t id,
                                       esp_event_handler_t event_handler,
                                       void *event_handler_arg,
                                       esp_event_handler_instance_context_t **handler_ctx,
                                       bool legacy)
{
    if (base == esp_event_any_base && id == ESP_EVENT_ANY_ID) {
        return handler_instances_add(&(loop_node->handlers), event_handler, event_handler_arg, handler_ctx, legacy);
    } else {
        esp_err_t err = ESP_OK;
        esp_event_base_node_t *it = NULL, *base_node = NULL, *last_base_node = NULL;

        SLIST_FOREACH(it, &(loop_node->base_nodes), next) {
            if (it->base == base) {
                base_node = it;
            }
            last_base_node = it;
        }

        if (!last_base_node ||
                !base_node ||
                (base_node && !SLIST_EMPTY(&(base_node->id_nodes)) && id == ESP_EVENT_ANY_ID) ||
                (last_base_node && last_base_node->base != base && !SLIST_EMPTY(&(last_base_node->id_nodes)) && id == ESP_EVENT_ANY_ID)) {
            base_node = (esp_event_base_node_t*) esp_event_calloc(1, sizeof(*base_node));

            if (!base_node) {
                ESP_LOGE(TAG, "alloc mem for new base node failed");
                return ESP_ERR_NO_MEM;
            }

            base_node->base = base;

            SLIST_INIT(&(base_node->handlers));
            SLIST_INIT(&(base_node->id_nodes));

            err = base_node_add_handler(base_node, id, event_handler, event_handler_arg, handler_ctx, legacy);

            if (err == ESP_OK) {
                if (!last_base_node) {
                    SLIST_INSERT_HEAD(&(loop_node->base_nodes), base_node, next);
                } else {
                    SLIST_INSERT_AFTER(last_base_node, base_node, next);
                }
            } else {
                free(base_node);
            }

            return err;
        } else {
            return base_node_add_handler(base_node, id, event_handler, event_handler_arg, handler_ctx, legacy);
        }
    }
}

static esp_err_t handler_instances_remove(esp_event_handler_nodes_t* handlers, esp_event_handler_instance_context_t* handler_ctx, bool legacy)
{
    esp_event_handler_node_t *it, *temp;

    SLIST_FOREACH_SAFE(it, handlers, next, temp) {
        if (legacy) {
            if (it->handler_ctx->handler == handler_ctx->handler) {
                SLIST_REMOVE(handlers, it, esp_event_handler_node, next);
                free(it->handler_ctx);
                free(it);
                return ESP_OK;
            }
        } else {
            if (it->handler_ctx == handler_ctx) {
                SLIST_REMOVE(handlers, it, esp_event_handler_node, next);
                free(it->handler_ctx);
                free(it);
                return ESP_OK;
            }
        }
    }

    return ESP_ERR_NOT_FOUND;
}

static esp_err_t base_node_remove_handler(esp_event_base_node_t* base_node, int32_t id, esp_event_handler_instance_context_t* handler_ctx, bool legacy)
{
    if (id == ESP_EVENT_ANY_ID) {
        return handler_instances_remove(&(base_node->handlers), handler_ctx, legacy);
    } else {
        esp_event_id_node_t *it, *temp;
        SLIST_FOREACH_SAFE(it, &(base_node->id_nodes), next, temp) {
            if (it->id == id) {
                esp_err_t res = handler_instances_remove(&(it->handlers), handler_ctx, legacy);

                if (res == ESP_OK) {
                    if (SLIST_EMPTY(&(it->handlers))) {
                        SLIST_REMOVE(&(base_node->id_nodes), it, esp_event_id_node, next);
                        free(it);
                    }
                    return ESP_OK;
                }
            }
        }
    }

    return ESP_ERR_NOT_FOUND;
}

static esp_err_t loop_node_remove_handler(esp_event_loop_node_t* loop_node, esp_event_base_t base, int32_t id, esp_event_handler_instance_context_t* handler_ctx, bool legacy)
{
    if (base == esp_event_any_base && id == ESP_EVENT_ANY_ID) {
        return handler_instances_remove(&(loop_node->handlers), handler_ctx, legacy);
    } else {
        esp_event_base_node_t *it, *temp;
        SLIST_FOREACH_SAFE(it, &(loop_node->base_nodes), next, temp) {
            if (it->base == base) {
                esp_err_t res = base_node_remove_handler(it, id, handler_ctx, legacy);

                if (res == ESP_OK) {
                    if (SLIST_EMPTY(&(it->handlers)) && SLIST_EMPTY(&(it->id_nodes))) {
                        SLIST_REMOVE(&(loop_node->base_nodes), it, esp_event_base_node, next);
                        free(it);
                    }
                    return ESP_OK;
                }
            }
        }
    }

    return ESP_ERR_NOT_FOUND;
}

static esp_err_t loop_remove_handler(esp_event_remove_handler_context_t* ctx)
{
    esp_event_loop_node_t *it, *temp;
    SLIST_FOREACH_SAFE(it, &(ctx->loop->loop_nodes), next, temp) {
        esp_err_t res = loop_node_remove_handler(it, ctx->event_base, ctx->event_id, ctx->handler_ctx, ctx->legacy);

        if (res == ESP_OK) {
            if (SLIST_EMPTY(&(it->base_nodes)) && SLIST_EMPTY(&(it->handlers))) {
                SLIST_REMOVE(&(ctx->loop->loop_nodes), it, esp_event_loop_node, next);
                free(it);
            }
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

static void handler_instances_remove_all(esp_event_handler_nodes_t* handlers)
{
    esp_event_handler_node_t *it, *temp;
    SLIST_FOREACH_SAFE(it, handlers, next, temp) {
        SLIST_REMOVE(handlers, it, esp_event_handler_node, next);
        free(it->handler_ctx);
        free(it);
    }
}

static void base_node_remove_all_handler(esp_event_base_node_t* base_node)
{
    handler_instances_remove_all(&(base_node->handlers));

    esp_event_id_node_t *it, *temp;
    SLIST_FOREACH_SAFE(it, &(base_node->id_nodes), next, temp) {
        handler_instances_remove_all(&(it->handlers));
        SLIST_REMOVE(&(base_node->id_nodes), it, esp_event_id_node, next);
        free(it);
    }
}

static void loop_node_remove_all_handler(esp_event_loop_node_t* loop_node)
{
    handler_instances_remove_all(&(loop_node->handlers));

    esp_event_base_node_t *it, *temp;
    SLIST_FOREACH_SAFE(it, &(loop_node->base_nodes), next, temp) {
        base_node_remove_all_handler(it);
        SLIST_REMOVE(&(loop_node->base_nodes), it, esp_event_base_node, next);
        free(it);
    }
}

static void inline __attribute__((always_inline)) post_instance_delete(esp_event_post_instance_t* post)
{
#if CONFIG_ESP_EVENT_POST_FROM_ISR
    if (post->data_allocated)
#endif
    {
        free(post->data.ptr);
    }
    memset(post, 0, sizeof(*post));
}

static esp_err_t find_and_unregister_handler(esp_event_remove_handler_context_t* ctx)
{
    esp_event_handler_node_t *handler_to_unregister = NULL;
    esp_event_handler_node_t *handler;
    esp_event_loop_node_t *loop_node;
    esp_event_base_node_t *base_node;
    esp_event_id_node_t *id_node;

    SLIST_FOREACH(loop_node, &(ctx->loop->loop_nodes), next) {
        // Execute loop level handlers
        SLIST_FOREACH(handler, &(loop_node->handlers), next) {
            if (ctx->legacy) {
                if (handler->handler_ctx->handler == ctx->handler_ctx->handler) {
                    handler_to_unregister = handler;
                    break;
                }
            } else {
                if (handler->handler_ctx == ctx->handler_ctx) {
                    handler_to_unregister = handler;
                    break;
                }
            }
        }

        if (handler_to_unregister != NULL) {
            break;
        }

        SLIST_FOREACH(base_node, &(loop_node->base_nodes), next) {
            if (base_node->base == ctx->event_base) {
                // Execute base level handlers
                SLIST_FOREACH(handler, &(base_node->handlers), next) {
                    if (ctx->legacy) {
                        if (handler->handler_ctx->handler == ctx->handler_ctx->handler) {
                            handler_to_unregister = handler;
                            break;
                        }
                    } else {
                        if (handler->handler_ctx == ctx->handler_ctx) {
                            handler_to_unregister = handler;
                            break;
                        }
                    }
                }

                if (handler_to_unregister != NULL) {
                    break;
                }

                SLIST_FOREACH(id_node, &(base_node->id_nodes), next) {
                    if (id_node->id == ctx->event_id) {
                        // Execute id level handlers
                        SLIST_FOREACH(handler, &(id_node->handlers), next) {
                            if (ctx->legacy) {
                                if (handler->handler_ctx->handler == ctx->handler_ctx->handler) {
                                    handler_to_unregister = handler;
                                    break;
                                }
                            } else {
                                if (handler->handler_ctx == ctx->handler_ctx) {
                                    handler_to_unregister = handler;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (handler_to_unregister == NULL) {
        /* handler not found in the lists, return */
        return ESP_ERR_NOT_FOUND;
    }

    if (handler_to_unregister->unregistered) {
        /* the handler was found in a list but has already be marked
         * as unregistered. It means an event was already created to
         * remove from the list. return OK but do nothing */
        return ESP_OK;
    }
    /* handler found in the lists and not already marked as unregistered. Mark it as unregistered
     * and post an event to remove it from the lists */
    handler_to_unregister->unregistered = true;
    if (ctx->legacy) {
        /* in case of legacy code, we have to copy the handler_ctx content since it was created in the calling function */
        esp_event_handler_instance_context_t *handler_ctx_copy = esp_event_calloc(1, sizeof(esp_event_handler_instance_context_t));
        if (!handler_ctx_copy) {
            return ESP_ERR_NO_MEM;
        }
        handler_ctx_copy->arg = ctx->handler_ctx->arg;
        handler_ctx_copy->handler = ctx->handler_ctx->handler;
        ctx->handler_ctx = handler_ctx_copy;
    }
    return esp_event_post_to(ctx->loop, esp_event_handler_cleanup, 0, ctx, sizeof(esp_event_remove_handler_context_t), portMAX_DELAY);
}

/* ---------------------------- Public API --------------------------------- */

esp_err_t esp_event_loop_create(const esp_event_loop_args_t* event_loop_args, esp_event_loop_handle_t* event_loop)
{
    if (event_loop_args == NULL) {
        ESP_LOGE(TAG, "event_loop_args was NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (event_loop == NULL) {
        ESP_LOGE(TAG, "event_loop was NULL");
        return ESP_ERR_INVALID_ARG;
    }

    esp_event_loop_instance_t* loop;
    esp_err_t err = ESP_ERR_NO_MEM; // most likely error

    loop = esp_event_calloc(1, sizeof(*loop));
    if (loop == NULL) {
        ESP_LOGE(TAG, "alloc for event loop failed");
        return err;
    }

#if CONFIG_ESP_EVENT_LOOP_IN_EXT_RAM && !CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR
    loop->queue = xQueueCreateWithCaps(event_loop_args->queue_size, sizeof(esp_event_post_instance_t), MALLOC_CAP_SPIRAM);
#else
    loop->queue = xQueueCreate(event_loop_args->queue_size, sizeof(esp_event_post_instance_t));
#endif // CONFIG_ESP_EVENT_LOOP_IN_EXT_RAM && !CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR
    if (loop->queue == NULL) {
        ESP_LOGE(TAG, "create event loop queue failed");
        goto on_err;
    }

    loop->mutex = xSemaphoreCreateRecursiveMutex();
    if (loop->mutex == NULL) {
        ESP_LOGE(TAG, "create event loop mutex failed");
        goto on_err;
    }

    SLIST_INIT(&(loop->loop_nodes));

    // Create the loop task if requested
    if (event_loop_args->task_name != NULL) {
#if !CONFIG_ESP_EVENT_LOOP_IN_EXT_RAM
        BaseType_t task_created = xTaskCreatePinnedToCore(esp_event_loop_run_task,
                                                          event_loop_args->task_name,
                                                          event_loop_args->task_stack_size,
                                                          (void*) loop,
                                                          event_loop_args->task_priority,
                                                          &(loop->task),
                                                          event_loop_args->task_core_id);
#else
        BaseType_t task_created = xTaskCreatePinnedToCoreWithCaps(esp_event_loop_run_task,
                                                                  event_loop_args->task_name,
                                                                  event_loop_args->task_stack_size,
                                                                  (void*) loop,
                                                                  event_loop_args->task_priority,
                                                                  &(loop->task),
                                                                  event_loop_args->task_core_id,
                                                                  MALLOC_CAP_SPIRAM);
#endif // !CONFIG_ESP_EVENT_LOOP_IN_EXT_RAM

        if (task_created != pdPASS) {
            ESP_LOGE(TAG, "create task for loop failed");
            err = ESP_FAIL;
            goto on_err;
        }

        loop->name = event_loop_args->task_name;

        ESP_LOGD(TAG, "created task for loop %p", loop);
    } else {
        loop->name = "";
        loop->task = NULL;
    }

    loop->running_task = NULL;

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
    portENTER_CRITICAL(&s_event_loops_spinlock);
    SLIST_INSERT_HEAD(&s_event_loops, loop, next);
    portEXIT_CRITICAL(&s_event_loops_spinlock);
#endif

    *event_loop = (esp_event_loop_handle_t) loop;

    ESP_LOGD(TAG, "created event loop %p", loop);

    return ESP_OK;

on_err:
    if (loop->queue != NULL) {
#if CONFIG_ESP_EVENT_LOOP_IN_EXT_RAM && !CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR
        vQueueDeleteWithCaps(loop->queue);
#else
        vQueueDelete(loop->queue);
#endif
    }

    if (loop->mutex != NULL) {
        vSemaphoreDelete(loop->mutex);
    }

    free(loop);

    return err;
}

// On event lookup performance: The library implements the event list as a linked list, which results to O(n)
// lookup time. The test comparing this implementation to the O(lg n) performance of rbtrees
// (https://github.com/freebsd/freebsd/blob/master/sys/sys/tree.h)
// indicate that the difference is not that substantial, especially considering the additional
// pointers per node of rbtrees. Code for the rbtree implementation of the event loop library is archived
// in feature/esp_event_loop_library_rbtrees if needed.
esp_err_t esp_event_loop_run(esp_event_loop_handle_t event_loop, TickType_t ticks_to_run)
{
    assert(event_loop);

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;
    esp_event_post_instance_t post;
    TickType_t marker = xTaskGetTickCount();
    TickType_t end = 0;

#if (configUSE_16_BIT_TICKS == 1)
    int32_t remaining_ticks = ticks_to_run;
#else
    int64_t remaining_ticks = ticks_to_run;
#endif

    while (xQueueReceive(loop->queue, &post, remaining_ticks) == pdTRUE) {
        // The event has already been unqueued, so ensure it gets executed.
        xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);

        // check if the event retrieve from the queue is the internal event that is
        // triggered when a handler needs to be removed..
        if (post.base == esp_event_handler_cleanup) {
            assert(post.data.ptr != NULL);
            esp_event_remove_handler_context_t* ctx = (esp_event_remove_handler_context_t*)post.data.ptr;
            loop_remove_handler(ctx);

            // if the handler unregistration request came from legacy code,
            // we have to free handler_ctx pointer since it points to memory
            // allocated by esp_event_handler_unregister_with_internal
            if (ctx->legacy) {
                free(ctx->handler_ctx);
            }
        }

        loop->running_task = xTaskGetCurrentTaskHandle();

        bool exec = false;

        esp_event_handler_node_t *handler, *temp_handler;
        esp_event_loop_node_t *loop_node, *temp_node;
        esp_event_base_node_t *base_node, *temp_base;
        esp_event_id_node_t *id_node, *temp_id_node;

        SLIST_FOREACH_SAFE(loop_node, &(loop->loop_nodes), next, temp_node) {
            // Execute loop level handlers
            SLIST_FOREACH_SAFE(handler, &(loop_node->handlers), next, temp_handler) {
                if (!handler->unregistered) {
                    handler_execute(loop, handler, post);
                    exec |= true;
                }
            }

            SLIST_FOREACH_SAFE(base_node, &(loop_node->base_nodes), next, temp_base) {
                if (base_node->base == post.base) {
                    // Execute base level handlers
                    SLIST_FOREACH_SAFE(handler, &(base_node->handlers), next, temp_handler) {
                        if (!handler->unregistered) {
                            handler_execute(loop, handler, post);
                            exec |= true;
                        }
                    }

                    SLIST_FOREACH_SAFE(id_node, &(base_node->id_nodes), next, temp_id_node) {
                        if (id_node->id == post.id) {
                            // Execute id level handlers
                            SLIST_FOREACH_SAFE(handler, &(id_node->handlers), next, temp_handler) {
                                if (!handler->unregistered) {
                                    handler_execute(loop, handler, post);
                                    exec |= true;
                                }
                            }
                            // Skip to next base node
                            break;
                        }
                    }
                }
            }
        }

        esp_event_base_t base = post.base;
        int32_t id = post.id;

        post_instance_delete(&post);

        if (ticks_to_run != portMAX_DELAY) {
            end = xTaskGetTickCount();
            remaining_ticks -= end - marker;
            // If the ticks to run expired, return to the caller
            if (remaining_ticks <= 0) {
                xSemaphoreGiveRecursive(loop->mutex);
                break;
            } else {
                marker = end;
            }
        }

        loop->running_task = NULL;

        xSemaphoreGiveRecursive(loop->mutex);

        if (!exec) {
            // No handlers were registered, not even loop/base level handlers
            ESP_LOGD(TAG, "no handlers have been registered for event %s:%"PRIu32" posted to loop %p", base, id, event_loop);
        }
    }

    return ESP_OK;
}

esp_err_t esp_event_loop_delete(esp_event_loop_handle_t event_loop)
{
    assert(event_loop);
    ESP_LOGD(TAG, "deleting loop %p", (void*) event_loop);

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;
    SemaphoreHandle_t loop_mutex = loop->mutex;

    xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
    portENTER_CRITICAL(&s_event_loops_spinlock);
    SLIST_REMOVE(&s_event_loops, loop, esp_event_loop_instance, next);
    portEXIT_CRITICAL(&s_event_loops_spinlock);
#endif

    // Delete the task if it was created
    if (loop->task != NULL) {
#if CONFIG_ESP_EVENT_LOOP_IN_EXT_RAM
        vTaskDeleteWithCaps(loop->task);
#else
        vTaskDelete(loop->task);
#endif
        loop->task = NULL;
    }

    // Remove all registered events and handlers in the loop
    esp_event_loop_node_t *it, *temp;
    SLIST_FOREACH_SAFE(it, &(loop->loop_nodes), next, temp) {
        loop_node_remove_all_handler(it);
        SLIST_REMOVE(&(loop->loop_nodes), it, esp_event_loop_node, next);
        free(it);
    }

    // Drop existing posts on the queue
    esp_event_post_instance_t post;
    while (xQueueReceive(loop->queue, &post, 0) == pdTRUE) {
        post_instance_delete(&post);
    }

    // Cleanup loop
#if CONFIG_ESP_EVENT_LOOP_IN_EXT_RAM && !CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR
    vQueueDeleteWithCaps(loop->queue);
#else
    vQueueDelete(loop->queue);
#endif
    free(loop);
    // Free loop mutex before deleting
    xSemaphoreGiveRecursive(loop_mutex);
    vSemaphoreDelete(loop_mutex);

    return ESP_OK;
}

esp_err_t esp_event_handler_register_with_internal(esp_event_loop_handle_t event_loop, esp_event_base_t event_base,
                                                   int32_t event_id, esp_event_handler_t event_handler, void* event_handler_arg,
                                                   esp_event_handler_instance_context_t** handler_ctx_arg, bool legacy)
{
    assert(event_loop);
    assert(event_handler);

    if (event_base == ESP_EVENT_ANY_BASE && event_id != ESP_EVENT_ANY_ID) {
        ESP_LOGE(TAG, "registering to any event base with specific id unsupported");
        return ESP_ERR_INVALID_ARG;
    }

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    if (event_base == ESP_EVENT_ANY_BASE) {
        event_base = esp_event_any_base;
    }

    esp_err_t err = ESP_OK;

    xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);

    esp_event_loop_node_t *loop_node = NULL, *last_loop_node = NULL;

    SLIST_FOREACH(loop_node, &(loop->loop_nodes), next) {
        last_loop_node = loop_node;
    }

    bool is_loop_level_handler = (event_base == esp_event_any_base) && (event_id == ESP_EVENT_ANY_ID);

    if (!last_loop_node ||
            (last_loop_node && !SLIST_EMPTY(&(last_loop_node->base_nodes)) && is_loop_level_handler)) {
        loop_node = (esp_event_loop_node_t*) esp_event_calloc(1, sizeof(*loop_node));

        if (!loop_node) {
            ESP_LOGE(TAG, "alloc for new loop node failed");
            err = ESP_ERR_NO_MEM;
            goto on_err;
        }

        SLIST_INIT(&(loop_node->handlers));
        SLIST_INIT(&(loop_node->base_nodes));

        err = loop_node_add_handler(loop_node, event_base, event_id, event_handler, event_handler_arg, handler_ctx_arg, legacy);

        if (err == ESP_OK) {
            if (!last_loop_node) {
                SLIST_INSERT_HEAD(&(loop->loop_nodes), loop_node, next);
            } else {
                SLIST_INSERT_AFTER(last_loop_node, loop_node, next);
            }
        } else {
            free(loop_node);
        }
    } else {
        err = loop_node_add_handler(last_loop_node, event_base, event_id, event_handler, event_handler_arg, handler_ctx_arg, legacy);
    }

on_err:
    xSemaphoreGiveRecursive(loop->mutex);
    return err;
}

esp_err_t esp_event_handler_register_with(esp_event_loop_handle_t event_loop, esp_event_base_t event_base,
                                          int32_t event_id, esp_event_handler_t event_handler, void* event_handler_arg)
{
    return esp_event_handler_register_with_internal(event_loop, event_base, event_id, event_handler, event_handler_arg, NULL, true);
}

esp_err_t esp_event_handler_instance_register_with(esp_event_loop_handle_t event_loop, esp_event_base_t event_base,
                                                   int32_t event_id, esp_event_handler_t event_handler, void* event_handler_arg,
                                                   esp_event_handler_instance_t* handler_ctx_arg)
{
    return esp_event_handler_register_with_internal(event_loop, event_base, event_id, event_handler, event_handler_arg, (esp_event_handler_instance_context_t**) handler_ctx_arg, false);
}

esp_err_t esp_event_handler_unregister_with_internal(esp_event_loop_handle_t event_loop, esp_event_base_t event_base,
                                                     int32_t event_id, esp_event_handler_instance_context_t* handler_ctx, bool legacy)
{
    assert(event_loop);
    assert(handler_ctx);

    if (event_base == ESP_EVENT_ANY_BASE && event_id != ESP_EVENT_ANY_ID) {
        ESP_LOGE(TAG, "unregistering to any event base with specific id unsupported");
        return ESP_ERR_INVALID_ARG;
    }

    if (event_base == ESP_EVENT_ANY_BASE) {
        event_base = esp_event_any_base;
    }

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;
    esp_event_remove_handler_context_t remove_handler_ctx = {loop, event_base, event_id, handler_ctx, legacy};

    /* remove the handler if the mutex is taken successfully.
     * otherwise it will be removed from the list later */
    esp_err_t res = ESP_FAIL;
    if (xSemaphoreTake(loop->mutex, 0) == pdTRUE) {
        res = loop_remove_handler(&remove_handler_ctx);
        xSemaphoreGive(loop->mutex);
    } else {
        xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);
        res = find_and_unregister_handler(&remove_handler_ctx);
        xSemaphoreGiveRecursive(loop->mutex);
    }

    return res;
}

esp_err_t esp_event_handler_unregister_with(esp_event_loop_handle_t event_loop, esp_event_base_t event_base,
                                            int32_t event_id, esp_event_handler_t event_handler)
{
    esp_event_handler_instance_context_t local_handler_ctx;
    local_handler_ctx.handler = event_handler;
    local_handler_ctx.arg = NULL;

    return esp_event_handler_unregister_with_internal(event_loop, event_base, event_id, &local_handler_ctx, true);
}

esp_err_t esp_event_handler_instance_unregister_with(esp_event_loop_handle_t event_loop, esp_event_base_t event_base,
                                                     int32_t event_id, esp_event_handler_instance_t handler_ctx_arg)
{
    if (!handler_ctx_arg) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_event_handler_unregister_with_internal(event_loop, event_base, event_id, (esp_event_handler_instance_context_t*) handler_ctx_arg, false);
}

esp_err_t esp_event_post_to(esp_event_loop_handle_t event_loop, esp_event_base_t event_base, int32_t event_id,
                            const void* event_data, size_t event_data_size, TickType_t ticks_to_wait)
{
    assert(event_loop);

    if (event_base == ESP_EVENT_ANY_BASE || event_id == ESP_EVENT_ANY_ID) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    esp_event_post_instance_t post;
    memset((void*)(&post), 0, sizeof(post));

    if (event_data != NULL && event_data_size != 0) {
#if CONFIG_ESP_EVENT_POST_FROM_ISR
        if (event_data_size > sizeof(post.data.val)) {
            post.data.ptr = calloc(1, event_data_size);
            if (post.data.ptr == NULL) {
                return ESP_ERR_NO_MEM;
            }
            post.data_allocated = true;
            memcpy(post.data.ptr, event_data, event_data_size);
        } else {
            memcpy((void*)(&(post.data.val)), event_data, event_data_size);
        }
        post.data_set = true;
#else // !CONFIG_ESP_EVENT_POST_FROM_ISR
        // Make persistent copy of event data on heap.
        void* event_data_copy = esp_event_calloc(1, event_data_size);

        if (event_data_copy == NULL) {
            return ESP_ERR_NO_MEM;
        }

        memcpy(event_data_copy, event_data, event_data_size);
        post.data.ptr = event_data_copy;
#endif // !CONFIG_ESP_EVENT_POST_FROM_ISR
    }
    post.base = event_base;
    post.id = event_id;

    BaseType_t result = pdFALSE;

    // Find the task that currently executes the loop. It is safe to query loop->task since it is
    // not mutated since loop creation. ENSURE THIS REMAINS TRUE.
    if (loop->task == NULL) {
        // The loop has no dedicated task. Find out what task is currently running it.
        result = xSemaphoreTakeRecursive(loop->mutex, ticks_to_wait);

        if (result == pdTRUE) {
            if (loop->running_task != xTaskGetCurrentTaskHandle()) {
                xSemaphoreGiveRecursive(loop->mutex);
                result = xQueueSendToBack(loop->queue, &post, ticks_to_wait);
            } else {
                xSemaphoreGiveRecursive(loop->mutex);
                result = xQueueSendToBack(loop->queue, &post, 0);
            }
        }
    } else {
        // The loop has a dedicated task.
        if (loop->task != xTaskGetCurrentTaskHandle()) {
            result = xQueueSendToBack(loop->queue, &post, ticks_to_wait);
        } else {
            result = xQueueSendToBack(loop->queue, &post, 0);
        }
    }

    if (result != pdTRUE) {
        post_instance_delete(&post);

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
        atomic_fetch_add(&loop->events_dropped, 1);
#endif
        return ESP_ERR_TIMEOUT;
    }

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
    atomic_fetch_add(&loop->events_received, 1);
#endif

    return ESP_OK;
}

#if CONFIG_ESP_EVENT_POST_FROM_ISR
esp_err_t esp_event_isr_post_to(esp_event_loop_handle_t event_loop, esp_event_base_t event_base, int32_t event_id,
                                const void* event_data, size_t event_data_size, BaseType_t* task_unblocked)
{
    assert(event_loop);

    if (event_base == ESP_EVENT_ANY_BASE || event_id == ESP_EVENT_ANY_ID) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    esp_event_post_instance_t post;
    memset((void*)(&post), 0, sizeof(post));

    if (event_data_size > sizeof(post.data.val)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (event_data != NULL && event_data_size != 0) {
        memcpy((void*)(&(post.data.val)), event_data, event_data_size);
        post.data_allocated = false;
        post.data_set = true;
    }
    post.base = event_base;
    post.id = event_id;

    BaseType_t result = pdFALSE;

    // Post the event from an ISR,
    result = xQueueSendToBackFromISR(loop->queue, &post, task_unblocked);

    if (result != pdTRUE) {
        post_instance_delete(&post);

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
        atomic_fetch_add(&loop->events_dropped, 1);
#endif
        return ESP_FAIL;
    }

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
    atomic_fetch_add(&loop->events_received, 1);
#endif

    return ESP_OK;
}
#endif

esp_err_t esp_event_dump(FILE* file)
{
#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
    assert(file);

    esp_event_loop_instance_t* loop_it;
    esp_event_loop_node_t *loop_node_it;
    esp_event_base_node_t* base_node_it;
    esp_event_id_node_t* id_node_it;
    esp_event_handler_node_t* handler_it;

    // Allocate memory for printing
    int sz = esp_event_dump_prepare();
    char* buf = esp_event_calloc(sz, sizeof(char));
    char* dst = buf;

    char id_str_buf[20];

    // Print info to buffer
    portENTER_CRITICAL(&s_event_loops_spinlock);

    SLIST_FOREACH(loop_it, &s_event_loops, next) {
        uint32_t events_received, events_dropped;

        events_received = atomic_load(&loop_it->events_received);
        events_dropped = atomic_load(&loop_it->events_dropped);

        PRINT_DUMP_INFO(dst, sz, LOOP_DUMP_FORMAT, loop_it, loop_it->task != NULL ? loop_it->name : "none",
                        events_received, events_dropped);

        int sz_bak = sz;

        SLIST_FOREACH(loop_node_it, &(loop_it->loop_nodes), next) {
            SLIST_FOREACH(handler_it, &(loop_node_it->handlers), next) {
                PRINT_DUMP_INFO(dst, sz, HANDLER_DUMP_FORMAT, handler_it->handler_ctx->handler, "ESP_EVENT_ANY_BASE",
                                "ESP_EVENT_ANY_ID", handler_it->invoked, handler_it->time);
            }

            SLIST_FOREACH(base_node_it, &(loop_node_it->base_nodes), next) {
                SLIST_FOREACH(handler_it, &(base_node_it->handlers), next) {
                    PRINT_DUMP_INFO(dst, sz, HANDLER_DUMP_FORMAT, handler_it->handler_ctx->handler, base_node_it->base,
                                    "ESP_EVENT_ANY_ID", handler_it->invoked, handler_it->time);
                }

                SLIST_FOREACH(id_node_it, &(base_node_it->id_nodes), next) {
                    SLIST_FOREACH(handler_it, &(id_node_it->handlers), next) {
                        memset(id_str_buf, 0, sizeof(id_str_buf));
                        snprintf(id_str_buf, sizeof(id_str_buf), "%" PRIi32, id_node_it->id);

                        PRINT_DUMP_INFO(dst, sz, HANDLER_DUMP_FORMAT, handler_it->handler_ctx->handler, base_node_it->base,
                                        id_str_buf, handler_it->invoked, handler_it->time);
                    }
                }
            }
        }

        // No handlers registered for this loop
        if (sz == sz_bak) {
            PRINT_DUMP_INFO(dst, sz, "  NO HANDLERS REGISTERED\n");
        }
    }

    portEXIT_CRITICAL(&s_event_loops_spinlock);

    // Print the contents of the buffer to the file
    fprintf(file, buf);

    // Free the allocated buffer
    free(buf);
#endif
    return ESP_OK;
}
