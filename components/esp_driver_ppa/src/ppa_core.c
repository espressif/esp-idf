/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/idf_additions.h"
#include "freertos/queue.h"
#include "esp_heap_caps.h"
#include "esp_cache.h"
#include "esp_private/esp_cache_private.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "driver/ppa.h"
#include "ppa_priv.h"
#include "esp_private/dma2d.h"
#include "hal/dma2d_ll.h"
#include "hal/ppa_hal.h"
#include "hal/ppa_ll.h"
#include "hal/ppa_types.h"
#include "esp_private/periph_ctrl.h"

static const char *TAG = "ppa_core";

// PPA driver platform
static ppa_platform_t s_platform = {
    .spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
};

static esp_err_t ppa_engine_acquire(const ppa_engine_config_t *config, ppa_engine_t **ret_engine);
static esp_err_t ppa_engine_release(ppa_engine_t *ppa_engine);
static bool ppa_malloc_transaction(QueueHandle_t trans_elm_ptr_queue, uint32_t trans_elm_num, ppa_operation_t oper_type);
static void ppa_free_transaction(ppa_trans_t *trans_elm);

const dma2d_trans_on_picked_callback_t ppa_oper_trans_on_picked_func[PPA_OPERATION_INVALID] = {
    [PPA_OPERATION_SRM] = ppa_srm_transaction_on_picked,
    [PPA_OPERATION_BLEND] = ppa_blend_transaction_on_picked,
    [PPA_OPERATION_FILL] = ppa_fill_transaction_on_picked,
};

static esp_err_t ppa_engine_acquire(const ppa_engine_config_t *config, ppa_engine_t **ret_engine)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && ret_engine, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->engine == PPA_ENGINE_TYPE_SRM || config->engine == PPA_ENGINE_TYPE_BLEND, ESP_ERR_INVALID_ARG, TAG, "invalid engine");

    *ret_engine = NULL;

    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    size_t alignment = MAX(DMA2D_LL_DESC_ALIGNMENT, data_cache_line_size);

    _lock_acquire(&s_platform.mutex);
    if (s_platform.dma_desc_mem_size == 0) {
        s_platform.dma_desc_mem_size = PPA_ALIGN_UP(sizeof(dma2d_descriptor_align8_t), alignment);
    }
    if (s_platform.buf_alignment_size == 0) {
        esp_cache_get_alignment(MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA, &s_platform.buf_alignment_size);
    }

    if (config->engine == PPA_ENGINE_TYPE_SRM) {
        if (!s_platform.srm) {
            ppa_srm_engine_t *srm_engine = heap_caps_calloc(1, sizeof(ppa_srm_engine_t), PPA_MEM_ALLOC_CAPS);
            SemaphoreHandle_t srm_sem = xSemaphoreCreateBinaryWithCaps(PPA_MEM_ALLOC_CAPS);
            dma2d_descriptor_t *srm_tx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            dma2d_descriptor_t *srm_rx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (srm_engine && srm_sem && srm_tx_dma_desc && srm_rx_dma_desc) {
                srm_engine->dma_tx_desc = srm_tx_dma_desc;
                srm_engine->dma_rx_desc = srm_rx_dma_desc;
                srm_engine->base.platform = &s_platform;
                srm_engine->base.type = PPA_ENGINE_TYPE_SRM;
                srm_engine->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
                srm_engine->base.sem = srm_sem;
                xSemaphoreGive(srm_engine->base.sem);
                STAILQ_INIT(&srm_engine->base.trans_stailq);
                s_platform.srm = srm_engine;
                s_platform.srm_engine_ref_count++;
                *ret_engine = &srm_engine->base;

                // TODO: Register PPA interrupt? Useful for SRM parameter error. If SRM parameter error, blocks at 2D-DMA, transaction can never finish, stuck...
                // need a way to force end
            } else {
                ret = ESP_ERR_NO_MEM;
                ESP_LOGE(TAG, "no mem to register PPA SRM engine");
                free(srm_engine);
                if (srm_sem) {
                    vSemaphoreDeleteWithCaps(srm_sem);
                }
                free(srm_tx_dma_desc);
                free(srm_rx_dma_desc);
            }

#if CONFIG_PM_ENABLE
            if (ret == ESP_OK) {
                ret = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "ppa_srm", &srm_engine->base.pm_lock);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "create pm lock failed");
                }
            }
#endif
        } else {
            // SRM engine already registered
            s_platform.srm_engine_ref_count++;
            *ret_engine = &s_platform.srm->base;
        }
    } else if (config->engine == PPA_ENGINE_TYPE_BLEND) {
        if (!s_platform.blending) {
            ppa_blend_engine_t *blending_engine = heap_caps_calloc(1, sizeof(ppa_blend_engine_t), PPA_MEM_ALLOC_CAPS);
            SemaphoreHandle_t blending_sem = xSemaphoreCreateBinaryWithCaps(PPA_MEM_ALLOC_CAPS);
            dma2d_descriptor_t *blending_tx_bg_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            dma2d_descriptor_t *blending_tx_fg_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            dma2d_descriptor_t *blending_rx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (blending_engine && blending_sem && blending_tx_bg_dma_desc && blending_tx_fg_dma_desc && blending_rx_dma_desc) {
                blending_engine->dma_tx_bg_desc = blending_tx_bg_dma_desc;
                blending_engine->dma_tx_fg_desc = blending_tx_fg_dma_desc;
                blending_engine->dma_rx_desc = blending_rx_dma_desc;
                blending_engine->base.platform = &s_platform;
                blending_engine->base.type = PPA_ENGINE_TYPE_BLEND;
                blending_engine->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
                blending_engine->base.sem = blending_sem;
                xSemaphoreGive(blending_engine->base.sem);
                STAILQ_INIT(&blending_engine->base.trans_stailq);
                s_platform.blending = blending_engine;
                s_platform.blend_engine_ref_count++;
                *ret_engine = &blending_engine->base;
            } else {
                ret = ESP_ERR_NO_MEM;
                ESP_LOGE(TAG, "no mem to register PPA Blending engine");
                free(blending_engine);
                if (blending_sem) {
                    vSemaphoreDeleteWithCaps(blending_sem);
                }
                free(blending_tx_bg_dma_desc);
                free(blending_tx_fg_dma_desc);
                free(blending_rx_dma_desc);
            }

#if CONFIG_PM_ENABLE
            if (ret == ESP_OK) {
                ret = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "ppa_blending", &blending_engine->base.pm_lock);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "create pm lock failed");
                }
            }
#endif
        } else {
            // Blending engine already registered
            s_platform.blend_engine_ref_count++;
            *ret_engine = &s_platform.blending->base;
        }
    }

    if (ret == ESP_OK) {
        if (!s_platform.hal.dev) {
            assert(!s_platform.dma2d_pool_handle);

            // Enable the bus clock to access PPA registers
            PERIPH_RCC_ATOMIC() {
                ppa_ll_enable_bus_clock(true);
                ppa_ll_reset_register();
            }

            ppa_hal_init(&s_platform.hal); // initialize HAL context

            // Get 2D-DMA pool handle
            dma2d_pool_config_t dma2d_config = {
                .pool_id = 0,
            };
            ret = dma2d_acquire_pool(&dma2d_config, &s_platform.dma2d_pool_handle);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "install 2D-DMA failed");
                goto wrap_up;
            }
        }
    }
wrap_up:
    _lock_release(&s_platform.mutex);

    if (ret != ESP_OK && *ret_engine != NULL) {
        ppa_engine_release(*ret_engine);
    }

    return ret;
}

static esp_err_t ppa_engine_release(ppa_engine_t *ppa_engine)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(ppa_engine, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    _lock_acquire(&s_platform.mutex);
    if (ppa_engine->type == PPA_ENGINE_TYPE_SRM) {
        ppa_srm_engine_t *srm_engine = __containerof(ppa_engine, ppa_srm_engine_t, base);
        s_platform.srm_engine_ref_count--;
        if (s_platform.srm_engine_ref_count == 0) {
            assert(STAILQ_EMPTY(&srm_engine->base.trans_stailq));
            // Now, time to free
            s_platform.srm = NULL;
            free(srm_engine->dma_tx_desc);
            free(srm_engine->dma_rx_desc);
            vSemaphoreDeleteWithCaps(srm_engine->base.sem);
#if CONFIG_PM_ENABLE
            if (srm_engine->base.pm_lock) {
                ret = esp_pm_lock_delete(srm_engine->base.pm_lock);
                assert(ret == ESP_OK);
            }
#endif
            free(srm_engine);
        }
    } else if (ppa_engine->type == PPA_ENGINE_TYPE_BLEND) {
        ppa_blend_engine_t *blending_engine = __containerof(ppa_engine, ppa_blend_engine_t, base);
        s_platform.blend_engine_ref_count--;
        if (s_platform.blend_engine_ref_count == 0) {
            assert(STAILQ_EMPTY(&blending_engine->base.trans_stailq));
            // Now, time to free
            s_platform.blending = NULL;
            free(blending_engine->dma_tx_bg_desc);
            free(blending_engine->dma_tx_fg_desc);
            free(blending_engine->dma_rx_desc);
            vSemaphoreDeleteWithCaps(blending_engine->base.sem);
#if CONFIG_PM_ENABLE
            if (blending_engine->base.pm_lock) {
                ret = esp_pm_lock_delete(blending_engine->base.pm_lock);
                assert(ret == ESP_OK);
            }
#endif
            free(blending_engine);
        }
    }

    if (!s_platform.srm && !s_platform.blending) {
        assert(s_platform.srm_engine_ref_count == 0 && s_platform.blend_engine_ref_count == 0);

        if (s_platform.dma2d_pool_handle) {
            dma2d_release_pool(s_platform.dma2d_pool_handle); // TODO: check return value. If not ESP_OK, then must be error on other 2D-DMA clients :( Give a warning log?
            s_platform.dma2d_pool_handle = NULL;
        }

        ppa_hal_deinit(&s_platform.hal); // De-initialize HAL context

        // Disable the bus clock to access PPA registers
        PERIPH_RCC_ATOMIC() {
            ppa_ll_enable_bus_clock(false);
        }
    }
    _lock_release(&s_platform.mutex);
    return ret;
}

esp_err_t ppa_register_client(const ppa_client_config_t *config, ppa_client_handle_t *ret_client)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && ret_client, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->oper_type < PPA_OPERATION_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown operation");

    ppa_client_t *client = (ppa_client_t *)heap_caps_calloc(1, sizeof(ppa_client_t), PPA_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(client, ESP_ERR_NO_MEM, TAG, "no mem to register client");

    // Allocate memory for storing transaction contexts and create a queue to save these trans_elm_ptr
    uint32_t queue_size = MAX(1, config->max_pending_trans_num);
    client->trans_elm_ptr_queue = xQueueCreateWithCaps(queue_size, sizeof(uint32_t), PPA_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(client->trans_elm_ptr_queue && ppa_malloc_transaction(client->trans_elm_ptr_queue, queue_size, config->oper_type),
                      ESP_ERR_NO_MEM, err, TAG, "no mem for transaction storage");

    client->oper_type = config->oper_type;
    client->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    client->data_burst_length = config->data_burst_length ? config->data_burst_length : PPA_DATA_BURST_LENGTH_128;
    if (config->oper_type == PPA_OPERATION_SRM) {
        ppa_engine_config_t engine_config = {
            .engine = PPA_ENGINE_TYPE_SRM,
        };
        ESP_GOTO_ON_ERROR(ppa_engine_acquire(&engine_config, &client->engine), err, TAG, "unable to acquire SRM engine");
    } else if (config->oper_type == PPA_OPERATION_BLEND || config->oper_type == PPA_OPERATION_FILL) {
        ppa_engine_config_t engine_config = {
            .engine = PPA_ENGINE_TYPE_BLEND,
        };
        ESP_GOTO_ON_ERROR(ppa_engine_acquire(&engine_config, &client->engine), err, TAG, "unable to acquire Blending engine");
    }
    *ret_client = client;

err:
    if (ret != ESP_OK) {
        ppa_unregister_client(client);
    }
    return ret;
}

esp_err_t ppa_unregister_client(ppa_client_handle_t ppa_client)
{
    ESP_RETURN_ON_FALSE(ppa_client, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    bool do_unregister = false;
    portENTER_CRITICAL(&ppa_client->spinlock);
    if (ppa_client->trans_cnt == 0) {
        do_unregister = true;
    }
    portEXIT_CRITICAL(&ppa_client->spinlock);
    ESP_RETURN_ON_FALSE(do_unregister, ESP_ERR_INVALID_STATE, TAG, "client still has unprocessed trans");

    if (ppa_client->engine) {
        ppa_engine_release(ppa_client->engine);
    }

    if (ppa_client->trans_elm_ptr_queue) {
        ppa_trans_t *trans_elm = NULL;
        while (xQueueReceive(ppa_client->trans_elm_ptr_queue, (void *)&trans_elm, 0)) {
            ppa_free_transaction(trans_elm);
        }
        vQueueDeleteWithCaps(ppa_client->trans_elm_ptr_queue);
    }
    free(ppa_client);
    return ESP_OK;
}

esp_err_t ppa_client_register_event_callbacks(ppa_client_handle_t ppa_client, const ppa_event_callbacks_t *cbs)
{
    ESP_RETURN_ON_FALSE(ppa_client && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    ppa_client->done_cb = cbs->on_trans_done;
    return ESP_OK;
}

// Each PPA engine should only have one transaction being pushed to 2D-DMA queue, the rest transactions should stay in engine's own transaction queue.
// This is to avoid 2D-DMA channels being hold, but not actually being used (waiting for PPA engine to be free)
static esp_err_t ppa_dma2d_enqueue(const ppa_trans_t *trans_elm)
{
    return dma2d_enqueue(s_platform.dma2d_pool_handle, trans_elm->trans_desc, trans_elm->dma_trans_placeholder);
}

static bool ppa_malloc_transaction(QueueHandle_t trans_elm_ptr_queue, uint32_t trans_elm_num, ppa_operation_t oper_type)
{
    bool res = true;
    size_t ppa_trans_desc_size = (oper_type == PPA_OPERATION_SRM) ? sizeof(ppa_srm_oper_t) :
                                 (oper_type == PPA_OPERATION_BLEND) ? sizeof(ppa_blend_oper_t) :
                                 (oper_type == PPA_OPERATION_FILL) ? sizeof(ppa_fill_oper_t) : 0;
    assert(ppa_trans_desc_size != 0);
    size_t trans_elm_storage_size = sizeof(ppa_trans_t) + SIZEOF_DMA2D_TRANS_T + sizeof(dma2d_trans_config_t) + sizeof(ppa_dma2d_trans_on_picked_config_t) + ppa_trans_desc_size;
    for (int i = 0; i < trans_elm_num; i++) {
        void *trans_elm_storage = heap_caps_calloc(1, trans_elm_storage_size, PPA_MEM_ALLOC_CAPS);
        SemaphoreHandle_t ppa_trans_sem = xSemaphoreCreateBinaryWithCaps(PPA_MEM_ALLOC_CAPS);

        if (!trans_elm_storage || !ppa_trans_sem) {
            if (trans_elm_storage) {
                free(trans_elm_storage);
            }
            if (ppa_trans_sem) {
                vSemaphoreDeleteWithCaps(ppa_trans_sem);
            }
            res = false;
            break;
        }

        // Construct trans_elm
        ppa_trans_t *new_trans_elm = (ppa_trans_t *)trans_elm_storage;
        dma2d_trans_t *dma_trans_elm = (dma2d_trans_t *)((uint32_t)trans_elm_storage + sizeof(ppa_trans_t));
        dma2d_trans_config_t *dma_trans_desc = (dma2d_trans_config_t *)((uint32_t)dma_trans_elm + SIZEOF_DMA2D_TRANS_T);
        ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)((uint32_t)dma_trans_desc + sizeof(dma2d_trans_config_t));
        void *ppa_trans_desc = (void *)((uint32_t)trans_on_picked_desc + sizeof(ppa_dma2d_trans_on_picked_config_t));

        trans_on_picked_desc->op_desc = ppa_trans_desc;
        trans_on_picked_desc->trans_elm = new_trans_elm;
        dma_trans_desc->user_config = (void *)trans_on_picked_desc;
        dma_trans_desc->on_job_picked = ppa_oper_trans_on_picked_func[oper_type];
        new_trans_elm->trans_desc = dma_trans_desc;
        new_trans_elm->dma_trans_placeholder = dma_trans_elm;
        new_trans_elm->sem = ppa_trans_sem;

        // Fill the queue with allocated transaction element pointer
        BaseType_t sent = xQueueSend(trans_elm_ptr_queue, &new_trans_elm, 0);
        assert(sent);
    }
    return res;
}

static void ppa_free_transaction(ppa_trans_t *trans_elm)
{
    if (trans_elm) {
        if (trans_elm->sem) {
            vSemaphoreDeleteWithCaps(trans_elm->sem);
        }
        free(trans_elm);
    }
}

bool ppa_recycle_transaction(ppa_client_handle_t ppa_client, ppa_trans_t *trans_elm)
{
    // Reset transaction and send back to client's trans_elm_ptr_queue
    // TODO: To be very safe, we shall memset all to 0, and reconnect necessary pointers?
    BaseType_t HPTaskAwoken;
    BaseType_t sent = xQueueSendFromISR(ppa_client->trans_elm_ptr_queue, &trans_elm, &HPTaskAwoken);
    assert(sent);
    return HPTaskAwoken;
}

esp_err_t ppa_do_operation(ppa_client_handle_t ppa_client, ppa_engine_t *ppa_engine_base, ppa_trans_t *trans_elm, ppa_trans_mode_t mode)
{
    esp_err_t ret = ESP_OK;
    esp_err_t pm_lock_ret __attribute__((unused));

    portENTER_CRITICAL(&ppa_client->spinlock);
    // Send transaction into PPA engine queue
    portENTER_CRITICAL(&ppa_engine_base->spinlock);
    STAILQ_INSERT_TAIL(&ppa_engine_base->trans_stailq, trans_elm, entry);
    portEXIT_CRITICAL(&ppa_engine_base->spinlock);
    ppa_client->trans_cnt++;
    portEXIT_CRITICAL(&ppa_client->spinlock);

    TickType_t ticks_to_wait = (mode == PPA_TRANS_MODE_NON_BLOCKING) ? 0 : portMAX_DELAY;
    if (xSemaphoreTake(ppa_engine_base->sem, ticks_to_wait) == pdTRUE) {
        // Check if the transaction has already been started from the ISR
        // If so, then the transaction should have been removed from queue at this moment (transaction completed)
        bool found = false;
        ppa_trans_t *temp = NULL;
        portENTER_CRITICAL(&ppa_engine_base->spinlock);
        STAILQ_FOREACH(temp, &ppa_engine_base->trans_stailq, entry) {
            if (temp == trans_elm) {
                found = true;
                break;
            }
        }
        portEXIT_CRITICAL(&ppa_engine_base->spinlock);
        if (found) {
#if CONFIG_PM_ENABLE
            pm_lock_ret = esp_pm_lock_acquire(ppa_engine_base->pm_lock);
            assert((pm_lock_ret == ESP_OK) && "acquire pm_lock failed");
#endif
            ret = ppa_dma2d_enqueue(trans_elm);
            if (ret != ESP_OK) {
                portENTER_CRITICAL(&ppa_engine_base->spinlock);
                STAILQ_REMOVE(&ppa_engine_base->trans_stailq, trans_elm, ppa_trans_s, entry);
                portEXIT_CRITICAL(&ppa_engine_base->spinlock);
                xSemaphoreGive(ppa_engine_base->sem);
#if CONFIG_PM_ENABLE
                pm_lock_ret = esp_pm_lock_release(ppa_engine_base->pm_lock);
                assert((pm_lock_ret == ESP_OK) && "release pm_lock failed");
#endif
                portENTER_CRITICAL(&ppa_client->spinlock);
                ppa_client->trans_cnt--;
                portEXIT_CRITICAL(&ppa_client->spinlock);
                goto err;
            }
        } else {
            xSemaphoreGive(ppa_engine_base->sem);
        }
    }

    if (mode == PPA_TRANS_MODE_BLOCKING) {
        xSemaphoreTake(trans_elm->sem, portMAX_DELAY); // Given in the ISR
    }

err:
    return ret;
}

bool ppa_transaction_done_cb(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    BaseType_t HPTaskAwoken;
    ppa_trans_t *trans_elm = (ppa_trans_t *)user_data;
    ppa_client_t *client = trans_elm->client;
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)trans_elm->trans_desc->user_config;
    ppa_engine_t *engine_base = trans_on_picked_desc->ppa_engine;
    // Save callback contexts
    ppa_event_callback_t done_cb = client->done_cb;
    void *trans_elm_user_data = trans_elm->user_data;

    ppa_trans_t *next_start_trans = NULL;
    portENTER_CRITICAL_ISR(&engine_base->spinlock);
    // Remove this transaction from transaction queue
    STAILQ_REMOVE(&engine_base->trans_stailq, trans_elm, ppa_trans_s, entry);
    next_start_trans = STAILQ_FIRST(&engine_base->trans_stailq);
    portEXIT_CRITICAL_ISR(&engine_base->spinlock);

    portENTER_CRITICAL_ISR(&client->spinlock);
    // Release transaction semaphore to unblock ppa_do_operation
    xSemaphoreGiveFromISR(trans_elm->sem, &HPTaskAwoken);
    need_yield |= (HPTaskAwoken == pdTRUE);

    // Then recycle transaction elm
    need_yield |= ppa_recycle_transaction(client, trans_elm);

    client->trans_cnt--;
    portEXIT_CRITICAL_ISR(&client->spinlock);

    // If there is next trans in PPA engine queue, send it to DMA queue; otherwise, release the engine semaphore
    if (next_start_trans) {
        ppa_dma2d_enqueue(next_start_trans);
    } else {
        xSemaphoreGiveFromISR(engine_base->sem, &HPTaskAwoken);
        need_yield |= (HPTaskAwoken == pdTRUE);
#if CONFIG_PM_ENABLE
        esp_err_t pm_lock_ret = esp_pm_lock_release(engine_base->pm_lock);
        assert(pm_lock_ret == ESP_OK);
#endif
    }

    // Process last transaction's callback
    if (done_cb) {
        ppa_event_data_t edata = {};
        need_yield |= done_cb(client, &edata, trans_elm_user_data);
    }

    return need_yield;
}
