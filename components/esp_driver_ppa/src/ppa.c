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
#include "soc/dma2d_channel.h"
#include "hal/ppa_hal.h"
#include "hal/ppa_ll.h"
#include "hal/ppa_types.h"
#include "hal/color_types.h"
#include "hal/color_hal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_memory_utils.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

static const char *TAG = "ppa";

typedef struct ppa_platform_t {
    _lock_t mutex;                              // Platform level mutex lock to protect the ppa_engine_acquire/ppa_engine_release process
    portMUX_TYPE spinlock;                      // Platform level spinlock
    ppa_hal_context_t hal;                      // PPA HAL context
    dma2d_pool_handle_t dma2d_pool_handle;      // Pointer to the acquired 2D-DMA pool
    ppa_srm_engine_t *srm;                      // Pointer to the PPA SRM engine
    ppa_blend_engine_t *blending;               // Pointer to the PPA blending engine
    uint32_t srm_engine_ref_count;              // Reference count used to protect PPA SRM engine acquire and release
    uint32_t blend_engine_ref_count;            // Reference count used to protect PPA blending engine acquire and release
    size_t buf_alignment_size;                  // Alignment requirement for the outgoing buffer addr and size to satisfy cache line size
    uint32_t dma_desc_mem_size;                 // Alignment requirement for the 2D-DMA descriptor to satisfy cache line size
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;               // Power management lock
#endif
} ppa_platform_t;

// PPA driver platform
static ppa_platform_t s_platform = {
    .spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
};

static esp_err_t ppa_engine_acquire(const ppa_engine_config_t *config, ppa_engine_t **ret_engine);
static esp_err_t ppa_engine_release(ppa_engine_t *ppa_engine);
static bool ppa_malloc_transaction(QueueHandle_t trans_elm_ptr_queue, uint32_t trans_elm_num, ppa_operation_t oper_type);
static void ppa_free_transaction(ppa_trans_t *trans_elm);
static bool ppa_srm_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);
static bool ppa_blend_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);
static bool ppa_fill_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);

const dma2d_trans_on_picked_callback_t ppa_oper_trans_on_picked_func[PPA_OPERATION_NUM] = {
    ppa_srm_transaction_on_picked,
    ppa_blend_transaction_on_picked,
    ppa_fill_transaction_on_picked,
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
        s_platform.dma_desc_mem_size = ALIGN_UP(sizeof(dma2d_descriptor_align8_t), alignment);
    }
    if (s_platform.buf_alignment_size == 0) {
        esp_cache_get_alignment(ESP_CACHE_MALLOC_FLAG_PSRAM | ESP_CACHE_MALLOC_FLAG_DMA, &s_platform.buf_alignment_size);
    }

    if (config->engine == PPA_ENGINE_TYPE_SRM) {
        if (!s_platform.srm) {
            ppa_srm_engine_t *srm_engine = heap_caps_calloc(1, sizeof(ppa_srm_engine_t), PPA_MEM_ALLOC_CAPS);
            SemaphoreHandle_t srm_sem = xSemaphoreCreateBinaryWithCaps(PPA_MEM_ALLOC_CAPS);
            dma2d_descriptor_t *srm_tx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | PPA_MEM_ALLOC_CAPS);
            dma2d_descriptor_t *srm_rx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | PPA_MEM_ALLOC_CAPS);
            if (srm_engine && srm_sem && srm_tx_dma_desc && srm_rx_dma_desc) {
                srm_engine->dma_tx_desc = srm_tx_dma_desc;
                srm_engine->dma_rx_desc = srm_rx_dma_desc;
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
            ret = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "ppa_srm", &srm_engine->base.pm_lock);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "create pm lock failed");
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
            dma2d_descriptor_t *blending_tx_bg_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | PPA_MEM_ALLOC_CAPS);
            dma2d_descriptor_t *blending_tx_fg_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | PPA_MEM_ALLOC_CAPS);
            dma2d_descriptor_t *blending_rx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(alignment, 1, s_platform.dma_desc_mem_size, MALLOC_CAP_DMA | PPA_MEM_ALLOC_CAPS);
            if (blending_engine && blending_sem && blending_tx_bg_dma_desc && blending_tx_fg_dma_desc && blending_rx_dma_desc) {
                blending_engine->dma_tx_bg_desc = blending_tx_bg_dma_desc;
                blending_engine->dma_tx_fg_desc = blending_tx_fg_dma_desc;
                blending_engine->dma_rx_desc = blending_rx_dma_desc;
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
            ret = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "ppa_blending", &blending_engine->base.pm_lock);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "create pm lock failed");
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

#if CONFIG_PM_ENABLE
            assert(!s_platform.pm_lock);
            // Create and acquire the PM lock
            ret = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "ppa", &s_platform.pm_lock);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "create pm lock failed");
                goto wrap_up;
            }
#endif
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
                assert(esp_pm_lock_delete(srm_engine->base.pm_lock) == ESP_OK);
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
                assert(esp_pm_lock_delete(blending_engine->base.pm_lock) == ESP_OK);
            }
#endif
            free(blending_engine);
        }
    }

    if (!s_platform.srm && !s_platform.blending) {
        assert(s_platform.srm_engine_ref_count == 0 && s_platform.blend_engine_ref_count == 0);

#if CONFIG_PM_ENABLE
        if (s_platform.pm_lock) {
            assert(esp_pm_lock_delete(s_platform.pm_lock) == ESP_OK);
        }
#endif

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
    ESP_RETURN_ON_FALSE(config->oper_type < PPA_OPERATION_NUM, ESP_ERR_INVALID_ARG, TAG, "unknown operation");

    ppa_client_t *client = (ppa_client_t *)heap_caps_calloc(1, sizeof(ppa_client_t), PPA_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(client, ESP_ERR_NO_MEM, TAG, "no mem to register client");

    uint32_t ring_buf_size = MAX(1, config->max_pending_trans_num);
    client->trans_elm_ptr_queue = xQueueCreateWithCaps(ring_buf_size, sizeof(uint32_t), PPA_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(client->trans_elm_ptr_queue && ppa_malloc_transaction(client->trans_elm_ptr_queue, ring_buf_size, config->oper_type),
                      ESP_ERR_NO_MEM, err, TAG, "no mem for transaction storage");

    client->oper_type = config->oper_type;
    client->done_cb = config->done_cb;
    client->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    client->in_accepting_trans_state = true;
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
        ppa_client->in_accepting_trans_state = false;
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
        dma_trans_desc->on_job_picked = ppa_oper_trans_on_picked_func[oper_type]; // TODO: This maybe better to be in the ppa_do_xxx function
        new_trans_elm->trans_desc = dma_trans_desc;
        new_trans_elm->dma_trans_placeholder = dma_trans_elm;
        new_trans_elm->sem = ppa_trans_sem;

        // Fill the ring buffer with allocated transaction element pointer
        assert(xQueueSend(trans_elm_ptr_queue, &new_trans_elm, 0));
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

static bool ppa_recycle_transaction(ppa_client_handle_t ppa_client, ppa_trans_t *trans_elm)
{
    // Reset transaction and send back to client's trans_elm_ptr_queue
    // TODO: To be very safe, we shall memset all to 0, and reconnect necessary pointers？
    BaseType_t HPTaskAwoken;
    assert(xQueueSendFromISR(ppa_client->trans_elm_ptr_queue, &trans_elm, &HPTaskAwoken));
    return HPTaskAwoken;
}

static esp_err_t ppa_do_operation(ppa_client_handle_t ppa_client, ppa_engine_t *ppa_engine_base, ppa_trans_t *trans_elm, ppa_trans_mode_t mode)
{
    esp_err_t ret = ESP_OK;

#if CONFIG_PM_ENABLE
    assert((esp_pm_lock_acquire(s_platform.pm_lock) == ESP_OK) && "acquire pm_lock failed");
#endif

    portENTER_CRITICAL(&ppa_client->spinlock);
    // TODO: Check whether trans_cnt and trans_elm_ptr_queue need in one spinlock!!!
    if (ppa_client->in_accepting_trans_state) {
        // Send transaction into PPA engine queue
        STAILQ_INSERT_TAIL(&ppa_engine_base->trans_stailq, trans_elm, entry);
        ppa_client->trans_cnt++;
    } else {
        ret = ESP_FAIL;
    }
    portEXIT_CRITICAL(&ppa_client->spinlock);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "The client cannot accept transaction now");
        goto err;
    }

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
            assert((esp_pm_lock_acquire(ppa_engine_base->pm_lock) == ESP_OK) && "acquire pm_lock failed");
#endif
            ret = ppa_dma2d_enqueue(trans_elm);
            if (ret != ESP_OK) {
                portENTER_CRITICAL(&ppa_engine_base->spinlock);
                STAILQ_REMOVE(&ppa_engine_base->trans_stailq, trans_elm, ppa_trans_s, entry);
                portEXIT_CRITICAL(&ppa_engine_base->spinlock);
#if CONFIG_PM_ENABLE
                assert((esp_pm_lock_release(ppa_engine_base->pm_lock) == ESP_OK) && "release pm_lock failed");
#endif
                xSemaphoreGive(ppa_engine_base->sem);
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
        // while (1) {
        //     printf("ppa intr: %ld\n", PPA.int_raw.val);
        // }
        xSemaphoreTake(trans_elm->sem, portMAX_DELAY); // Given in the ISR
        // Sanity check new_trans_elm not in trans_stailq anymore? (loop takes time tho)
        // ppa_recycle_transaction(ppa_client, trans_elm); // TODO: Do we need it to be here or can be at the end of done_cb?
    }

#if CONFIG_PM_ENABLE
    assert((esp_pm_lock_release(s_platform.pm_lock) == ESP_OK) && "release pm_lock failed");
#endif

err:
    return ret;
}

static bool ppa_transaction_done_cb(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    BaseType_t HPTaskAwoken;
    ppa_trans_t *trans_elm = (ppa_trans_t *)user_data;
    ppa_client_t *client = trans_elm->client;
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)trans_elm->trans_desc->user_config;
    ppa_engine_t *engine_base = trans_on_picked_desc->ppa_engine;

    if (client->done_cb) {
        ppa_event_data_t edata = {};
        need_yield |= client->done_cb(client, &edata, trans_elm->user_data);
    }

    ppa_trans_t *next_start_trans = NULL;
    portENTER_CRITICAL_ISR(&engine_base->spinlock);
    // Remove this transaction from transaction queue
    STAILQ_REMOVE(&engine_base->trans_stailq, trans_elm, ppa_trans_s, entry);
    next_start_trans = STAILQ_FIRST(&engine_base->trans_stailq);
    portEXIT_CRITICAL_ISR(&engine_base->spinlock);

    // If there is next trans in PPA engine queue, send it to DMA queue; otherwise, release the engine semaphore
    if (next_start_trans) {
        ppa_dma2d_enqueue(next_start_trans);
    } else {
#if CONFIG_PM_ENABLE
        assert((esp_pm_lock_release(engine_base->pm_lock) == ESP_OK));
#endif
        xSemaphoreGiveFromISR(engine_base->sem, &HPTaskAwoken);
        need_yield |= (HPTaskAwoken == pdTRUE);
    }
    // Recycle transaction and release transaction semaphore
    // if (trans_elm->sem != NULL) {
    xSemaphoreGiveFromISR(trans_elm->sem, &HPTaskAwoken);
    need_yield |= (HPTaskAwoken == pdTRUE);
    // }

    // TODO: Check whether trans_cnt and trans_elm_ptr_queue need in one spinlock!!!
    portENTER_CRITICAL_ISR(&client->spinlock);
    need_yield |= ppa_recycle_transaction(client, trans_elm);
    client->trans_cnt--;
    portEXIT_CRITICAL_ISR(&client->spinlock);

    return need_yield;
}

static bool ppa_srm_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(num_chans == 2 && dma2d_chans && user_config);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)user_config;
    assert(trans_on_picked_desc->trigger_periph == DMA2D_TRIG_PERIPH_PPA_SRM && trans_on_picked_desc->srm_desc && trans_on_picked_desc->ppa_engine);

    ppa_srm_oper_t *srm_trans_desc = (ppa_srm_oper_t *)trans_on_picked_desc->srm_desc;
    ppa_srm_engine_t *srm_engine = __containerof(trans_on_picked_desc->ppa_engine, ppa_srm_engine_t, base);

    // Reset SRM engine
    ppa_ll_srm_reset(s_platform.hal.dev);

    // Get the required 2D-DMA channel handles
    dma2d_channel_handle_t dma2d_tx_chan = NULL;
    dma2d_channel_handle_t dma2d_rx_chan = NULL;
    for (uint32_t i = 0; i < num_chans; i++) {
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_TX) {
            dma2d_tx_chan = dma2d_chans[i].chan;
        }
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_RX) {
            dma2d_rx_chan = dma2d_chans[i].chan;
        }
    }
    assert(dma2d_tx_chan && dma2d_rx_chan);

    color_space_pixel_format_t in_pixel_format = {
        .color_type_id = srm_trans_desc->in.srm_cm,
    };

    // Fill 2D-DMA descriptors
    srm_engine->dma_tx_desc->vb_size = srm_trans_desc->in.block_h;
    srm_engine->dma_tx_desc->hb_length = srm_trans_desc->in.block_w;
    srm_engine->dma_tx_desc->err_eof = 0;
    srm_engine->dma_tx_desc->dma2d_en = 1;
    srm_engine->dma_tx_desc->suc_eof = 1;
    srm_engine->dma_tx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    srm_engine->dma_tx_desc->va_size = srm_trans_desc->in.pic_h;
    srm_engine->dma_tx_desc->ha_length = srm_trans_desc->in.pic_w;
    srm_engine->dma_tx_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(in_pixel_format);
    srm_engine->dma_tx_desc->y = srm_trans_desc->in.block_offset_y;
    srm_engine->dma_tx_desc->x = srm_trans_desc->in.block_offset_x;
    srm_engine->dma_tx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    srm_engine->dma_tx_desc->buffer = (void *)srm_trans_desc->in.buffer;
    srm_engine->dma_tx_desc->next = NULL;

    // vb_size, hb_length can be any value (auto writeback). However, if vb_size/hb_length is 0, it triggers 2D-DMA DESC_ERROR interrupt, and dma2d driver will automatically ends the transaction. Therefore, to avoid this, we set them to 1.
    srm_engine->dma_rx_desc->vb_size = 1;
    srm_engine->dma_rx_desc->hb_length = 1;
    srm_engine->dma_rx_desc->err_eof = 0;
    srm_engine->dma_rx_desc->dma2d_en = 1;
    srm_engine->dma_rx_desc->suc_eof = 1;
    srm_engine->dma_rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    srm_engine->dma_rx_desc->va_size = srm_trans_desc->out.pic_h;
    srm_engine->dma_rx_desc->ha_length = srm_trans_desc->out.pic_w;
    // pbyte can be any value
    srm_engine->dma_rx_desc->y = srm_trans_desc->out.block_offset_y;
    srm_engine->dma_rx_desc->x = srm_trans_desc->out.block_offset_x;
    srm_engine->dma_rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    srm_engine->dma_rx_desc->buffer = (void *)srm_trans_desc->out.buffer;
    srm_engine->dma_rx_desc->next = NULL;

    esp_cache_msync((void *)srm_engine->dma_tx_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)srm_engine->dma_rx_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    // Configure 2D-DMA channels
    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_PPA_SRM,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_SRM_TX,
    };
    dma2d_connect(dma2d_tx_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_SRM_RX;
    dma2d_connect(dma2d_rx_chan, &trig_periph);

    dma2d_transfer_ability_t dma_transfer_ability = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_128,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
        // Configure the block size to be received by the SRM engine, which is passed from the 2D-DMA TX channel (i.e. 2D-DMA dscr-port mode)
        .dscr_port_block_h = (srm_trans_desc->in.srm_cm == PPA_SRM_COLOR_MODE_YUV420) ? PPA_LL_SRM_YUV420_BLOCK_SIZE : PPA_LL_SRM_DEFAULT_BLOCK_SIZE,
        .dscr_port_block_v = (srm_trans_desc->in.srm_cm == PPA_SRM_COLOR_MODE_YUV420) ? PPA_LL_SRM_YUV420_BLOCK_SIZE : PPA_LL_SRM_DEFAULT_BLOCK_SIZE,
    };
    dma2d_set_transfer_ability(dma2d_tx_chan, &dma_transfer_ability);
    dma2d_set_transfer_ability(dma2d_rx_chan, &dma_transfer_ability);

    // YUV444 and YUV422 are not supported by PPA module, need to utilize 2D-DMA color space conversion feature to do a conversion
    ppa_srm_color_mode_t ppa_in_color_mode = srm_trans_desc->in.srm_cm;
    if (ppa_in_color_mode == PPA_SRM_COLOR_MODE_YUV444) {
        ppa_in_color_mode = PPA_SRM_COLOR_MODE_RGB888;
        dma2d_csc_config_t dma_tx_csc = {0};
        if (srm_trans_desc->in.yuv_std == COLOR_CONV_STD_RGB_YUV_BT601) {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV444_TO_RGB888_601;
        } else {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV444_TO_RGB888_709;
        }
        dma2d_configure_color_space_conversion(dma2d_tx_chan, &dma_tx_csc);
    } else if (ppa_in_color_mode == PPA_SRM_COLOR_MODE_YUV422) {
        ppa_in_color_mode = PPA_SRM_COLOR_MODE_RGB888;
        dma2d_csc_config_t dma_tx_csc = {0};
        if (srm_trans_desc->in.yuv_std == COLOR_CONV_STD_RGB_YUV_BT601) {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV422_TO_RGB888_601;
        } else {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV422_TO_RGB888_709;
        }
        dma2d_configure_color_space_conversion(dma2d_tx_chan, &dma_tx_csc);
    }

    ppa_srm_color_mode_t ppa_out_color_mode = srm_trans_desc->out.srm_cm;
    if (ppa_out_color_mode == PPA_SRM_COLOR_MODE_YUV444) {
        ppa_out_color_mode = PPA_SRM_COLOR_MODE_YUV420;
        dma2d_csc_config_t dma_rx_csc = {
            .rx_csc_option = DMA2D_CSC_RX_YUV420_TO_YUV444,
        };
        dma2d_configure_color_space_conversion(dma2d_rx_chan, &dma_rx_csc);
    }

    dma2d_rx_event_callbacks_t dma_event_cbs = {
        .on_recv_eof = ppa_transaction_done_cb,
    };
    dma2d_register_rx_event_callbacks(dma2d_rx_chan, &dma_event_cbs, (void *)trans_on_picked_desc->trans_elm);

    dma2d_set_desc_addr(dma2d_tx_chan, (intptr_t)srm_engine->dma_tx_desc);
    dma2d_set_desc_addr(dma2d_rx_chan, (intptr_t)srm_engine->dma_rx_desc);
    dma2d_start(dma2d_tx_chan);
    dma2d_start(dma2d_rx_chan);

    // Configure PPA SRM engine
    ppa_ll_srm_set_rx_color_mode(s_platform.hal.dev, ppa_in_color_mode);
    if (COLOR_SPACE_TYPE((uint32_t)ppa_in_color_mode) == COLOR_SPACE_YUV) {
        ppa_ll_srm_set_rx_yuv_range(s_platform.hal.dev, srm_trans_desc->in.yuv_range);
        ppa_ll_srm_set_rx_yuv2rgb_std(s_platform.hal.dev, srm_trans_desc->in.yuv_std);
    }
    ppa_ll_srm_enable_rx_byte_swap(s_platform.hal.dev, srm_trans_desc->byte_swap);
    ppa_ll_srm_enable_rx_rgb_swap(s_platform.hal.dev, srm_trans_desc->rgb_swap);
    ppa_ll_srm_configure_rx_alpha(s_platform.hal.dev, srm_trans_desc->alpha_update_mode, srm_trans_desc->alpha_value);

    ppa_ll_srm_set_tx_color_mode(s_platform.hal.dev, ppa_out_color_mode);
    if (COLOR_SPACE_TYPE((uint32_t)ppa_out_color_mode) == COLOR_SPACE_YUV) {
        ppa_ll_srm_set_tx_yuv_range(s_platform.hal.dev, srm_trans_desc->out.yuv_range);
        ppa_ll_srm_set_tx_rgb2yuv_std(s_platform.hal.dev, srm_trans_desc->out.yuv_std);
    }

    ppa_ll_srm_set_rotation_angle(s_platform.hal.dev, srm_trans_desc->rotation_angle);
    ppa_ll_srm_set_scaling_x(s_platform.hal.dev, srm_trans_desc->scale_x_int, srm_trans_desc->scale_x_frag);
    ppa_ll_srm_set_scaling_y(s_platform.hal.dev, srm_trans_desc->scale_y_int, srm_trans_desc->scale_y_frag);
    ppa_ll_srm_enable_mirror_x(s_platform.hal.dev, srm_trans_desc->mirror_x);
    ppa_ll_srm_enable_mirror_y(s_platform.hal.dev, srm_trans_desc->mirror_y);

    ppa_ll_srm_start(s_platform.hal.dev);

    // No need to yield
    return false;
}

esp_err_t ppa_do_scale_rotate_mirror(ppa_client_handle_t ppa_client, const ppa_srm_oper_trans_config_t *config)
{
    ESP_RETURN_ON_FALSE(ppa_client && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ppa_client->oper_type == PPA_OPERATION_SRM, ESP_ERR_INVALID_ARG, TAG, "client is not for SRM operations");
    ESP_RETURN_ON_FALSE(config->mode <= PPA_TRANS_MODE_NON_BLOCKING, ESP_ERR_INVALID_ARG, TAG, "invalid mode");
    // in_buffer could be anywhere (ram, flash, psram), out_buffer ptr cannot in flash region
    ESP_RETURN_ON_FALSE(esp_ptr_internal(config->out.buffer) || esp_ptr_external_ram(config->out.buffer), ESP_ERR_INVALID_ARG, TAG, "invalid out.buffer addr");
    ESP_RETURN_ON_FALSE((uintptr_t)config->out.buffer % s_platform.buf_alignment_size == 0 && config->out.buffer_size % s_platform.buf_alignment_size == 0,
                        ESP_ERR_INVALID_ARG, TAG, "out.buffer addr or out.buffer_size not aligned to cache line size");
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = config->out.srm_cm,
    };
    uint32_t out_pic_len = config->out.pic_w * config->out.pic_h * color_hal_pixel_format_get_bit_depth(out_pixel_format) / 8;
    ESP_RETURN_ON_FALSE(out_pic_len <= config->out.buffer_size, ESP_ERR_INVALID_ARG, TAG, "out.pic_w/h mismatch with out.buffer_size");
    ESP_RETURN_ON_FALSE(config->scale_x < (PPA_LL_SRM_SCALING_INT_MAX + 1) && config->scale_x >= (1.0 / PPA_LL_SRM_SCALING_FRAG_MAX) &&
                        config->scale_y < (PPA_LL_SRM_SCALING_INT_MAX + 1) && config->scale_y >= (1.0 / PPA_LL_SRM_SCALING_FRAG_MAX),
                        ESP_ERR_INVALID_ARG, TAG, "invalid scale");
    uint32_t new_block_w = 0;
    uint32_t new_block_h = 0;
    if (config->rotation_angle == PPA_SRM_ROTATION_ANGLE_0 || config->rotation_angle == PPA_SRM_ROTATION_ANGLE_180) {
        new_block_w = (uint32_t)(config->scale_x * config->in.block_w);
        new_block_h = (uint32_t)(config->scale_y * config->in.block_h);
    } else {
        new_block_w = (uint32_t)(config->scale_y * config->in.block_h);
        new_block_h = (uint32_t)(config->scale_x * config->in.block_w);
    }
    ESP_RETURN_ON_FALSE(new_block_w <= (config->out.pic_w - config->out.block_offset_x) &&
                        new_block_h <= (config->out.pic_h - config->out.block_offset_y),
                        ESP_ERR_INVALID_ARG, TAG, "scale does not fit in the out pic");
    if (config->byte_swap) {
        PPA_CHECK_CM_SUPPORT_BYTE_SWAP("in.srm", (uint32_t)config->in.srm_cm);
    }
    if (config->rgb_swap) {
        PPA_CHECK_CM_SUPPORT_RGB_SWAP("in.srm", (uint32_t)config->in.srm_cm);
    }
    ESP_RETURN_ON_FALSE(config->alpha_value <= 0xFF, ESP_ERR_INVALID_ARG, TAG, "invalid alpha_value");
    // To reduce complexity, rotation_angle, color_mode, alpha_update_mode correctness are checked in their corresponding LL functions
    // TODO:
    // YUV420: in desc, ha/hb/va/vb/x/y must be even number
    // What for YUV444/YUV422

    // Write back and invalidate are performed on the entire picture (the window content is not continuous in the buffer)
    // Write back in_buffer
    color_space_pixel_format_t in_pixel_format = {
        .color_type_id = config->in.srm_cm,
    };
    uint32_t in_pic_len = config->in.pic_w * config->in.pic_h * color_hal_pixel_format_get_bit_depth(in_pixel_format) / 8;
    esp_cache_msync(config->in.buffer, in_pic_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    // Invalidate out_buffer
    esp_cache_msync(config->out.buffer, config->out.buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

    esp_err_t ret = ESP_OK;
    ppa_trans_t *trans_elm = NULL;
    if (xQueueReceive(ppa_client->trans_elm_ptr_queue, (void *)&trans_elm, 0)) {
        dma2d_trans_config_t *dma_trans_desc = trans_elm->trans_desc;

        ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = dma_trans_desc->user_config;

        ppa_srm_oper_t *srm_trans_desc = (ppa_srm_oper_t *)trans_on_picked_desc->srm_desc;
        memcpy(srm_trans_desc, config, sizeof(ppa_srm_oper_trans_config_t));
        srm_trans_desc->scale_x_int = (uint32_t)srm_trans_desc->scale_x;
        srm_trans_desc->scale_x_frag = (uint32_t)(srm_trans_desc->scale_x * (PPA_LL_SRM_SCALING_FRAG_MAX + 1)) & PPA_LL_SRM_SCALING_FRAG_MAX;
        srm_trans_desc->scale_y_int = (uint32_t)srm_trans_desc->scale_y;
        srm_trans_desc->scale_y_frag = (uint32_t)(srm_trans_desc->scale_y * (PPA_LL_SRM_SCALING_FRAG_MAX + 1)) & PPA_LL_SRM_SCALING_FRAG_MAX;

        trans_on_picked_desc->ppa_engine = ppa_client->engine;
        trans_on_picked_desc->trigger_periph = DMA2D_TRIG_PERIPH_PPA_SRM;

        dma_trans_desc->tx_channel_num = 1;
        dma_trans_desc->rx_channel_num = 1;
        dma_trans_desc->channel_flags = 0;
        if (config->in.srm_cm == PPA_SRM_COLOR_MODE_YUV422 || config->in.srm_cm == PPA_SRM_COLOR_MODE_YUV444) {
            dma_trans_desc->channel_flags |= DMA2D_CHANNEL_FUNCTION_FLAG_TX_CSC;
        }
        if (config->out.srm_cm == PPA_SRM_COLOR_MODE_YUV444) {
            dma_trans_desc->channel_flags |= DMA2D_CHANNEL_FUNCTION_FLAG_RX_CSC;
        }
        dma_trans_desc->specified_tx_channel_mask = 0;
        dma_trans_desc->specified_rx_channel_mask = 0;

        trans_elm->client = ppa_client;
        trans_elm->user_data = config->user_data;
        xSemaphoreTake(trans_elm->sem, 0); // Ensure no transaction semaphore before transaction starts

        ret = ppa_do_operation(ppa_client, ppa_client->engine, trans_elm, config->mode);
        if (ret != ESP_OK) {
            ppa_recycle_transaction(ppa_client, trans_elm);
        }
    } else {
        ret = ESP_FAIL;
        ESP_LOGE(TAG, "exceed maximum pending transactions for the client, consider increase max_pending_trans_num");
    }
    return ret;
}

static bool ppa_blend_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(num_chans == 3 && dma2d_chans && user_config);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)user_config;
    assert(trans_on_picked_desc->trigger_periph == DMA2D_TRIG_PERIPH_PPA_BLEND && trans_on_picked_desc->blend_desc && trans_on_picked_desc->ppa_engine);

    ppa_blend_oper_t *blend_trans_desc = (ppa_blend_oper_t *)trans_on_picked_desc->blend_desc;
    ppa_blend_engine_t *blend_engine = __containerof(trans_on_picked_desc->ppa_engine, ppa_blend_engine_t, base);

    // Reset blending engine
    ppa_ll_blend_reset(s_platform.hal.dev);

    // Get the required 2D-DMA channel handles
    dma2d_channel_handle_t dma2d_tx_bg_chan = NULL;
    dma2d_channel_handle_t dma2d_tx_fg_chan = NULL;
    dma2d_channel_handle_t dma2d_rx_chan = NULL;
    for (uint32_t i = 0; i < num_chans; i++) {
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_TX) {
            if (!dma2d_tx_bg_chan) {
                dma2d_tx_bg_chan = dma2d_chans[i].chan;
            } else {
                dma2d_tx_fg_chan = dma2d_chans[i].chan;
            }
        }
        if (dma2d_chans[i].dir == DMA2D_CHANNEL_DIRECTION_RX) {
            dma2d_rx_chan = dma2d_chans[i].chan;
        }
    }
    assert(dma2d_tx_bg_chan && dma2d_tx_fg_chan && dma2d_rx_chan);

    color_space_pixel_format_t in_bg_pixel_format = {
        .color_type_id = blend_trans_desc->in_bg.blend_cm,
    };
    color_space_pixel_format_t in_fg_pixel_format = {
        .color_type_id = blend_trans_desc->in_fg.blend_cm,
    };
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = blend_trans_desc->out.blend_cm,
    };

    // Fill 2D-DMA descriptors
    blend_engine->dma_tx_bg_desc->vb_size = blend_trans_desc->in_bg.block_h;
    blend_engine->dma_tx_bg_desc->hb_length = blend_trans_desc->in_bg.block_w;
    blend_engine->dma_tx_bg_desc->err_eof = 0;
    blend_engine->dma_tx_bg_desc->dma2d_en = 1;
    blend_engine->dma_tx_bg_desc->suc_eof = 1;
    blend_engine->dma_tx_bg_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_tx_bg_desc->va_size = blend_trans_desc->in_bg.pic_h;
    blend_engine->dma_tx_bg_desc->ha_length = blend_trans_desc->in_bg.pic_w;
    blend_engine->dma_tx_bg_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(in_bg_pixel_format);
    blend_engine->dma_tx_bg_desc->y = blend_trans_desc->in_bg.block_offset_y;
    blend_engine->dma_tx_bg_desc->x = blend_trans_desc->in_bg.block_offset_x;
    blend_engine->dma_tx_bg_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_tx_bg_desc->buffer = (void *)blend_trans_desc->in_bg.buffer;
    blend_engine->dma_tx_bg_desc->next = NULL;

    blend_engine->dma_tx_fg_desc->vb_size = blend_trans_desc->in_fg.block_h;
    blend_engine->dma_tx_fg_desc->hb_length = blend_trans_desc->in_fg.block_w;
    blend_engine->dma_tx_fg_desc->err_eof = 0;
    blend_engine->dma_tx_fg_desc->dma2d_en = 1;
    blend_engine->dma_tx_fg_desc->suc_eof = 1;
    blend_engine->dma_tx_fg_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_tx_fg_desc->va_size = blend_trans_desc->in_fg.pic_h;
    blend_engine->dma_tx_fg_desc->ha_length = blend_trans_desc->in_fg.pic_w;
    blend_engine->dma_tx_fg_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(in_fg_pixel_format);
    blend_engine->dma_tx_fg_desc->y = blend_trans_desc->in_fg.block_offset_y;
    blend_engine->dma_tx_fg_desc->x = blend_trans_desc->in_fg.block_offset_x;
    blend_engine->dma_tx_fg_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_tx_fg_desc->buffer = (void *)blend_trans_desc->in_fg.buffer;
    blend_engine->dma_tx_fg_desc->next = NULL;

    blend_engine->dma_rx_desc->vb_size = blend_trans_desc->in_fg.block_h; // in_bg.block_h == in_fg.block_h
    blend_engine->dma_rx_desc->hb_length = blend_trans_desc->in_fg.block_w; // in_bg.block_w == in_fg.block_w
    blend_engine->dma_rx_desc->err_eof = 0;
    blend_engine->dma_rx_desc->dma2d_en = 1;
    blend_engine->dma_rx_desc->suc_eof = 1;
    blend_engine->dma_rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_rx_desc->va_size = blend_trans_desc->out.pic_h;
    blend_engine->dma_rx_desc->ha_length = blend_trans_desc->out.pic_w;
    blend_engine->dma_rx_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(out_pixel_format);
    blend_engine->dma_rx_desc->y = blend_trans_desc->out.block_offset_y;
    blend_engine->dma_rx_desc->x = blend_trans_desc->out.block_offset_x;
    blend_engine->dma_rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_rx_desc->buffer = (void *)blend_trans_desc->out.buffer;
    blend_engine->dma_rx_desc->next = NULL;

    esp_cache_msync((void *)blend_engine->dma_tx_bg_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)blend_engine->dma_tx_fg_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)blend_engine->dma_rx_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    // Configure 2D-DMA channels
    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_PPA_BLEND,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_BG_TX,
    };
    dma2d_connect(dma2d_tx_bg_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_FG_TX;
    dma2d_connect(dma2d_tx_fg_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_RX;
    dma2d_connect(dma2d_rx_chan, &trig_periph);

    dma2d_transfer_ability_t dma_transfer_ability = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_128,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };
    dma2d_set_transfer_ability(dma2d_tx_bg_chan, &dma_transfer_ability);
    dma2d_set_transfer_ability(dma2d_tx_fg_chan, &dma_transfer_ability);
    dma2d_set_transfer_ability(dma2d_rx_chan, &dma_transfer_ability);

    dma2d_rx_event_callbacks_t dma_event_cbs = {
        .on_recv_eof = ppa_transaction_done_cb,
    };
    dma2d_register_rx_event_callbacks(dma2d_rx_chan, &dma_event_cbs, (void *)trans_on_picked_desc->trans_elm);

    dma2d_set_desc_addr(dma2d_tx_bg_chan, (intptr_t)blend_engine->dma_tx_bg_desc);
    dma2d_set_desc_addr(dma2d_tx_fg_chan, (intptr_t)blend_engine->dma_tx_fg_desc);
    dma2d_set_desc_addr(dma2d_rx_chan, (intptr_t)blend_engine->dma_rx_desc);
    dma2d_start(dma2d_tx_bg_chan);
    dma2d_start(dma2d_tx_fg_chan);
    dma2d_start(dma2d_rx_chan);

    // Configure PPA Blending engine
    ppa_ll_blend_set_rx_bg_color_mode(s_platform.hal.dev, blend_trans_desc->in_bg.blend_cm);
    ppa_ll_blend_enable_rx_bg_byte_swap(s_platform.hal.dev, blend_trans_desc->bg_byte_swap);
    ppa_ll_blend_enable_rx_bg_rgb_swap(s_platform.hal.dev, blend_trans_desc->bg_rgb_swap);
    ppa_ll_blend_configure_rx_bg_alpha(s_platform.hal.dev, blend_trans_desc->bg_alpha_update_mode, blend_trans_desc->bg_alpha_value);

    ppa_ll_blend_set_rx_fg_color_mode(s_platform.hal.dev, blend_trans_desc->in_fg.blend_cm);
    if (COLOR_SPACE_TYPE((uint32_t)blend_trans_desc->in_fg.blend_cm) == COLOR_SPACE_ALPHA) {
        ppa_ll_blend_set_rx_fg_fix_rgb(s_platform.hal.dev, blend_trans_desc->fg_fix_rgb_val);
    }
    ppa_ll_blend_enable_rx_fg_byte_swap(s_platform.hal.dev, blend_trans_desc->fg_byte_swap);
    ppa_ll_blend_enable_rx_fg_rgb_swap(s_platform.hal.dev, blend_trans_desc->fg_rgb_swap);
    ppa_ll_blend_configure_rx_fg_alpha(s_platform.hal.dev, blend_trans_desc->fg_alpha_update_mode, blend_trans_desc->fg_alpha_value);

    ppa_ll_blend_set_tx_color_mode(s_platform.hal.dev, blend_trans_desc->out.blend_cm);

    // Color keying
    ppa_ll_blend_configure_rx_bg_ck_range(s_platform.hal.dev,
                                          blend_trans_desc->bg_ck_en ? blend_trans_desc->bg_ck_rgb_low_thres : 0xFFFFFF,
                                          blend_trans_desc->bg_ck_en ? blend_trans_desc->bg_ck_rgb_high_thres : 0);
    ppa_ll_blend_configure_rx_fg_ck_range(s_platform.hal.dev,
                                          blend_trans_desc->fg_ck_en ? blend_trans_desc->fg_ck_rgb_low_thres : 0xFFFFFF,
                                          blend_trans_desc->fg_ck_en ? blend_trans_desc->fg_ck_rgb_high_thres : 0);
    ppa_ll_blend_set_ck_default_rgb(s_platform.hal.dev, (blend_trans_desc->bg_ck_en && blend_trans_desc->fg_ck_en) ? blend_trans_desc->ck_rgb_default_val : 0);
    ppa_ll_blend_enable_ck_fg_bg_reverse(s_platform.hal.dev, blend_trans_desc->ck_reverse_bg2fg);

    ppa_ll_blend_start(s_platform.hal.dev, PPA_LL_BLEND_TRANS_MODE_BLEND);

    // No need to yield
    return false;
}

esp_err_t ppa_do_blend(ppa_client_handle_t ppa_client, const ppa_blend_oper_trans_config_t *config)
{
    ESP_RETURN_ON_FALSE(ppa_client && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ppa_client->oper_type == PPA_OPERATION_BLEND, ESP_ERR_INVALID_ARG, TAG, "client is not for blend operations");
    ESP_RETURN_ON_FALSE(config->mode <= PPA_TRANS_MODE_NON_BLOCKING, ESP_ERR_INVALID_ARG, TAG, "invalid mode");
    // in_buffer could be anywhere (ram, flash, psram), out_buffer ptr cannot in flash region
    ESP_RETURN_ON_FALSE(esp_ptr_internal(config->out.buffer) || esp_ptr_external_ram(config->out.buffer), ESP_ERR_INVALID_ARG, TAG, "invalid out.buffer addr");
    ESP_RETURN_ON_FALSE((uintptr_t)config->out.buffer % s_platform.buf_alignment_size == 0 && config->out.buffer_size % s_platform.buf_alignment_size == 0,
                        ESP_ERR_INVALID_ARG, TAG, "out.buffer addr or out.buffer_size not aligned to cache line size");
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = config->out.blend_cm,
    };
    uint32_t out_pic_len = config->out.pic_w * config->out.pic_h * color_hal_pixel_format_get_bit_depth(out_pixel_format) / 8;
    ESP_RETURN_ON_FALSE(out_pic_len <= config->out.buffer_size, ESP_ERR_INVALID_ARG, TAG, "out.pic_w/h mismatch with out.buffer_size");
    ESP_RETURN_ON_FALSE(config->in_bg.block_w == config->in_fg.block_w && config->in_bg.block_h == config->in_fg.block_h,
                        ESP_ERR_INVALID_ARG, TAG, "in_bg.block_w/h must be equal to in_fg.block_w/h");
    if (config->bg_byte_swap) {
        PPA_CHECK_CM_SUPPORT_BYTE_SWAP("in_bg.blend", (uint32_t)config->in_bg.blend_cm);
    }
    if (config->bg_rgb_swap) {
        PPA_CHECK_CM_SUPPORT_RGB_SWAP("in_bg.blend", (uint32_t)config->in_bg.blend_cm);
    }
    if (config->fg_byte_swap) {
        PPA_CHECK_CM_SUPPORT_BYTE_SWAP("in_fg.blend", (uint32_t)config->in_fg.blend_cm);
    }
    if (config->fg_rgb_swap) {
        PPA_CHECK_CM_SUPPORT_RGB_SWAP("in_fg.blend", (uint32_t)config->in_fg.blend_cm);
    }
    ESP_RETURN_ON_FALSE(config->bg_alpha_value <= 0xFF && config->fg_alpha_value <= 0xFF, ESP_ERR_INVALID_ARG, TAG, "invalid bg/fg_alpha_value");
    if (config->in_bg.blend_cm == PPA_BLEND_COLOR_MODE_L4) {
        ESP_RETURN_ON_FALSE(config->in_bg.block_w % 2 == 0 && config->in_bg.block_offset_x % 2 == 0,
                            ESP_ERR_INVALID_ARG, TAG, "in_bg.block_w and in_bg.block_offset_x must be even");
    }
    if (config->in_fg.blend_cm == PPA_BLEND_COLOR_MODE_A4 || config->in_fg.blend_cm == PPA_BLEND_COLOR_MODE_L4) {
        ESP_RETURN_ON_FALSE(config->in_fg.block_w % 2 == 0 && config->in_fg.block_offset_x % 2 == 0,
                            ESP_ERR_INVALID_ARG, TAG, "in_fg.block_w and in_fg.block_offset_x must be even");
    }
    // To reduce complexity, color_mode, alpha_update_mode correctness are checked in their corresponding LL functions

    // Write back and invalidate are performed on the entire picture (the window content is not continuous in the buffer)
    // Write back in_bg_buffer, in_fg_buffer
    color_space_pixel_format_t in_bg_pixel_format = {
        .color_type_id = config->in_bg.blend_cm,
    };
    uint32_t in_bg_pic_len = config->in_bg.pic_w * config->in_bg.pic_h * color_hal_pixel_format_get_bit_depth(in_bg_pixel_format) / 8;
    esp_cache_msync(config->in_bg.buffer, in_bg_pic_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    color_space_pixel_format_t in_fg_pixel_format = {
        .color_type_id = config->in_fg.blend_cm,
    };
    uint32_t in_fg_pic_len = config->in_fg.pic_w * config->in_fg.pic_h * color_hal_pixel_format_get_bit_depth(in_fg_pixel_format) / 8;
    esp_cache_msync(config->in_fg.buffer, in_fg_pic_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    // Invalidate out_buffer
    esp_cache_msync(config->out.buffer, config->out.buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

    esp_err_t ret = ESP_OK;
    ppa_trans_t *trans_elm = NULL;
    if (xQueueReceive(ppa_client->trans_elm_ptr_queue, (void *)&trans_elm, 0)) {
        dma2d_trans_config_t *dma_trans_desc = trans_elm->trans_desc;

        ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = dma_trans_desc->user_config;

        ppa_blend_oper_t *blend_trans_desc = (ppa_blend_oper_t *)trans_on_picked_desc->blend_desc;
        memcpy(blend_trans_desc, config, sizeof(ppa_blend_oper_trans_config_t));

        trans_on_picked_desc->ppa_engine = ppa_client->engine;
        trans_on_picked_desc->trigger_periph = DMA2D_TRIG_PERIPH_PPA_BLEND;

        dma_trans_desc->tx_channel_num = 2;
        dma_trans_desc->rx_channel_num = 1;
        dma_trans_desc->channel_flags = 0;
        dma_trans_desc->specified_tx_channel_mask = 0;
        dma_trans_desc->specified_rx_channel_mask = 0;

        trans_elm->client = ppa_client;
        trans_elm->user_data = config->user_data;
        xSemaphoreTake(trans_elm->sem, 0); // Ensure no transaction semaphore before transaction starts

        ret = ppa_do_operation(ppa_client, ppa_client->engine, trans_elm, config->mode);
        if (ret != ESP_OK) {
            ppa_recycle_transaction(ppa_client, trans_elm);
        }
    } else {
        ret = ESP_FAIL;
        ESP_LOGE(TAG, "exceed maximum pending transactions for the client, consider increase max_pending_trans_num");
    }
    return ret;
}

static bool ppa_fill_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(num_chans == 1 && dma2d_chans && user_config);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)user_config;
    assert(trans_on_picked_desc->trigger_periph == DMA2D_TRIG_PERIPH_PPA_BLEND && trans_on_picked_desc->fill_desc && trans_on_picked_desc->ppa_engine);

    ppa_fill_oper_t *fill_trans_desc = (ppa_fill_oper_t *)trans_on_picked_desc->fill_desc;
    ppa_blend_engine_t *blend_engine = __containerof(trans_on_picked_desc->ppa_engine, ppa_blend_engine_t, base);

    // Reset blending engine
    ppa_ll_blend_reset(s_platform.hal.dev);

    // Get the required 2D-DMA channel handles
    assert(dma2d_chans[0].dir == DMA2D_CHANNEL_DIRECTION_RX);
    dma2d_channel_handle_t dma2d_rx_chan = dma2d_chans[0].chan;

    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = fill_trans_desc->out.fill_cm,
    };

    // Fill 2D-DMA descriptors
    blend_engine->dma_rx_desc->vb_size = fill_trans_desc->fill_block_h;
    blend_engine->dma_rx_desc->hb_length = fill_trans_desc->fill_block_w;
    blend_engine->dma_rx_desc->err_eof = 0;
    blend_engine->dma_rx_desc->dma2d_en = 1;
    blend_engine->dma_rx_desc->suc_eof = 1;
    blend_engine->dma_rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_rx_desc->va_size = fill_trans_desc->out.pic_h;
    blend_engine->dma_rx_desc->ha_length = fill_trans_desc->out.pic_w;
    blend_engine->dma_rx_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(out_pixel_format);
    blend_engine->dma_rx_desc->y = fill_trans_desc->out.block_offset_y;
    blend_engine->dma_rx_desc->x = fill_trans_desc->out.block_offset_x;
    blend_engine->dma_rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_rx_desc->buffer = (void *)fill_trans_desc->out.buffer;
    blend_engine->dma_rx_desc->next = NULL;

    esp_cache_msync((void *)blend_engine->dma_rx_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    // Configure 2D-DMA channels
    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_PPA_BLEND,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_PPA_BLEND_RX,
    };
    dma2d_connect(dma2d_rx_chan, &trig_periph);

    dma2d_transfer_ability_t dma_transfer_ability = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_128,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };
    dma2d_set_transfer_ability(dma2d_rx_chan, &dma_transfer_ability);

    dma2d_rx_event_callbacks_t dma_event_cbs = {
        .on_recv_eof = ppa_transaction_done_cb,
    };
    dma2d_register_rx_event_callbacks(dma2d_rx_chan, &dma_event_cbs, (void *)trans_on_picked_desc->trans_elm);

    dma2d_set_desc_addr(dma2d_rx_chan, (intptr_t)blend_engine->dma_rx_desc);
    dma2d_start(dma2d_rx_chan);

    // Configure PPA Blending engine
    ppa_ll_blend_configure_filling_block(s_platform.hal.dev, fill_trans_desc->fill_argb_color, fill_trans_desc->fill_block_w, fill_trans_desc->fill_block_h);
    ppa_ll_blend_set_tx_color_mode(s_platform.hal.dev, fill_trans_desc->out.fill_cm);

    ppa_ll_blend_start(s_platform.hal.dev, PPA_LL_BLEND_TRANS_MODE_FILL);

    // No need to yield
    return false;
}

esp_err_t ppa_do_fill(ppa_client_handle_t ppa_client, const ppa_fill_oper_trans_config_t *config)
{
    ESP_RETURN_ON_FALSE(ppa_client && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ppa_client->oper_type == PPA_OPERATION_FILL, ESP_ERR_INVALID_ARG, TAG, "client is not for fill operations");
    ESP_RETURN_ON_FALSE(config->mode <= PPA_TRANS_MODE_NON_BLOCKING, ESP_ERR_INVALID_ARG, TAG, "invalid mode");
    // out_buffer ptr cannot in flash region
    ESP_RETURN_ON_FALSE(esp_ptr_internal(config->out.buffer) || esp_ptr_external_ram(config->out.buffer), ESP_ERR_INVALID_ARG, TAG, "invalid out.buffer addr");
    ESP_RETURN_ON_FALSE((uintptr_t)config->out.buffer % s_platform.buf_alignment_size == 0 && config->out.buffer_size % s_platform.buf_alignment_size == 0,
                        ESP_ERR_INVALID_ARG, TAG, "out.buffer addr or out.buffer_size not aligned to cache line size");
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = config->out.fill_cm,
    };
    uint32_t out_pic_len = config->out.pic_w * config->out.pic_h * color_hal_pixel_format_get_bit_depth(out_pixel_format) / 8;
    ESP_RETURN_ON_FALSE(out_pic_len <= config->out.buffer_size, ESP_ERR_INVALID_ARG, TAG, "out.pic_w/h mismatch with out.buffer_size");
    // To reduce complexity, color_mode, fill_block_w/h correctness are checked in their corresponding LL functions

    // Write back and invalidate are performed on the entire picture (the window content is not continuous in the buffer)
    esp_cache_msync(config->out.buffer, config->out.buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE);

    esp_err_t ret = ESP_OK;
    ppa_trans_t *trans_elm = NULL;
    if (xQueueReceive(ppa_client->trans_elm_ptr_queue, (void *)&trans_elm, 0)) {
        dma2d_trans_config_t *dma_trans_desc = trans_elm->trans_desc;

        ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = dma_trans_desc->user_config;

        ppa_fill_oper_t *fill_trans_desc = (ppa_fill_oper_t *)trans_on_picked_desc->fill_desc;
        memcpy(fill_trans_desc, config, sizeof(ppa_fill_oper_trans_config_t));

        trans_on_picked_desc->ppa_engine = ppa_client->engine;
        trans_on_picked_desc->trigger_periph = DMA2D_TRIG_PERIPH_PPA_BLEND;

        dma_trans_desc->tx_channel_num = 0;
        dma_trans_desc->rx_channel_num = 1;
        dma_trans_desc->channel_flags = 0;
        dma_trans_desc->specified_tx_channel_mask = 0;
        dma_trans_desc->specified_rx_channel_mask = 0;

        trans_elm->client = ppa_client;
        trans_elm->user_data = config->user_data;
        xSemaphoreTake(trans_elm->sem, 0); // Ensure no transaction semaphore before transaction starts

        ret = ppa_do_operation(ppa_client, ppa_client->engine, trans_elm, config->mode);
        if (ret != ESP_OK) {
            ppa_recycle_transaction(ppa_client, trans_elm);
        }
    } else {
        ret = ESP_FAIL;
        ESP_LOGE(TAG, "exceed maximum pending transactions for the client, consider increase max_pending_trans_num");
    }
    return ret;
}
