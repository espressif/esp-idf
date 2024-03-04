/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/lock.h>
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/idf_additions.h"
#include "esp_heap_caps.h"
#include "esp_cache.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "driver/ppa.h"
#include "esp_private/dma2d.h"
#include "hal/dma2d_ll.h"
#include "soc/dma2d_channel.h"
#include "hal/ppa_ll.h"
#include "hal/ppa_types.h"
#include "hal/color_types.h"
#include "hal/color_hal.h"
#include "esp_private/periph_ctrl.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

static const char *TAG = "ppa";

typedef struct ppa_dev_t *ppa_soc_handle_t; // PPA SOC layer handle

typedef struct {
    ppa_soc_handle_t dev;
} ppa_hal_context_t;

void ppa_hal_init(ppa_hal_context_t *hal)
{
    hal->dev = PPA_LL_GET_HW;
}

void ppa_hal_deinit(ppa_hal_context_t *hal)
{
    hal->dev = NULL;
}

// PPA module contains SRM engine and Blending engine

typedef struct ppa_engine_t ppa_engine_t;

typedef struct ppa_invoker_t ppa_invoker_t;

typedef struct {
    PPA_SRM_OPERATION_CONFIG;
    uint32_t scale_x_int;
    uint32_t scale_x_frag;
    uint32_t scale_y_int;
    uint32_t scale_y_frag;
} ppa_srm_oper_t;

typedef ppa_blend_operation_config_t ppa_blend_oper_t;

typedef ppa_fill_operation_config_t ppa_fill_oper_t;

typedef struct ppa_trans_s {
    STAILQ_ENTRY(ppa_trans_s) entry; // link entry
    dma2d_trans_config_t *trans_desc;
    dma2d_trans_t *dma_trans_placeholder;
    SemaphoreHandle_t sem;
    ppa_invoker_t *invoker;
} ppa_trans_t;

typedef struct {
    union {
        ppa_srm_oper_t *srm_desc;
        ppa_blend_oper_t *blend_desc;
        ppa_fill_oper_t *fill_desc;
        void *op_desc;
    };
    ppa_engine_t *ppa_engine;
    ppa_trans_t *trans_elm;
    dma2d_trigger_peripheral_t trigger_periph;
} ppa_dma2d_trans_on_picked_config_t;

struct ppa_engine_t {
    ppa_engine_type_t type;
    portMUX_TYPE spinlock;
    SemaphoreHandle_t sem;
    // bool in_accepting_trans_state;
    STAILQ_HEAD(trans, ppa_trans_s) trans_stailq; // link head of pending transactions for the PPA engine
    // callback func? Here or in the struct above?
    // dma2d_rx_event_callbacks_t event_cbs;
};

typedef struct ppa_srm_engine_t {
    ppa_engine_t base;
    dma2d_descriptor_t *dma_tx_desc;
    dma2d_descriptor_t *dma_rx_desc;
} ppa_srm_engine_t;

typedef struct ppa_blend_engine_t {
    ppa_engine_t base;
    dma2d_descriptor_t *dma_tx_bg_desc;
    dma2d_descriptor_t *dma_tx_fg_desc;
    dma2d_descriptor_t *dma_rx_desc;
} ppa_blend_engine_t;

struct ppa_invoker_t {
    ppa_engine_t *srm_engine;
    ppa_engine_t *blending_engine;
    uint32_t srm_trans_cnt;
    uint32_t blending_trans_cnt;
    portMUX_TYPE spinlock;
    bool in_accepting_trans_state;
    // done_cb
    // user_data
};

typedef enum {
    PPA_OPERATION_SRM,
    PPA_OPERATION_BLEND,
    PPA_OPERATION_FILL,
    PPA_OPERATION_NUM,
} ppa_operation_t;

typedef struct ppa_platform_t {
    _lock_t mutex;                              // platform level mutex lock to protect the ppa_module_acquire/ppa_module_release process
    portMUX_TYPE spinlock;                      // platform level spinlock
    ppa_hal_context_t hal;
    dma2d_pool_handle_t dma2d_pool_handle;
    ppa_srm_engine_t *srm;
    ppa_blend_engine_t *blending;
    uint32_t srm_engine_ref_count;
    uint32_t blend_engine_ref_count;
    uint32_t dma_desc_mem_size;
} ppa_platform_t;

// PPA driver platform
static ppa_platform_t s_platform = {
    .spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
};

#define PPA_MEM_ALLOC_CAPS (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT) // TODO...

typedef struct {
    ppa_engine_type_t engine;
} ppa_engine_config_t;

static esp_err_t ppa_engine_acquire(const ppa_engine_config_t *config, ppa_engine_t **ret_engine);
static esp_err_t ppa_engine_release(ppa_engine_t *ppa_engine);
static bool ppa_srm_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);
static bool ppa_blend_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);
static bool ppa_fill_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);

const dma2d_trans_on_picked_callback_t ppa_oper_trans_on_picked_func[PPA_OPERATION_NUM] = {
    ppa_srm_transaction_on_picked,
    ppa_blend_transaction_on_picked,
    ppa_fill_transaction_on_picked,
};

// extern uint32_t dma2d_tx_channel_reserved_mask[SOC_DMA2D_GROUPS];
// extern uint32_t dma2d_rx_channel_reserved_mask[SOC_DMA2D_GROUPS];
// static uint32_t ppa_specified_tx_channel_mask = 0;
// static uint32_t ppa_specified_rx_channel_mask = 0;
// static __attribute__((constructor)) void ppa_reserve_dma2d_channels(void)
// {
//     // ppa_specified_tx_channel_mask = 1 << 0;
//     // ppa_specified_rx_channel_mask = 1 << 0;
//     dma2d_tx_channel_reserved_mask[0] |= ppa_specified_tx_channel_mask;
//     dma2d_rx_channel_reserved_mask[0] |= ppa_specified_rx_channel_mask;
// }

// TODO: acquire pm_lock per transaction?
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
                // srm_engine->base.in_accepting_trans_state = true;
                STAILQ_INIT(&srm_engine->base.trans_stailq);
                // srm_engine->base.event_cbs
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
                // blending_engine->base.in_accepting_trans_state = true;
                STAILQ_INIT(&blending_engine->base.trans_stailq);
                // blending_engine->base.event_cbs
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
            }
        }
    }
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
            // // Stop accepting new transactions to SRM engine
            // portENTER_CRITICAL(&srm_engine->base.spinlock);
            // srm_engine->base.in_accepting_trans_state = false;
            // portEXIT_CRITICAL(&srm_engine->base.spinlock);
            // // Wait until all transactions get processed
            // while (!STAILQ_EMPTY(&srm_engine->base.trans_stailq)); // TODO: Think twice, looks like I am not able to use engine semaphore to decide
            assert(STAILQ_EMPTY(&srm_engine->base.trans_stailq));
            // Now, time to free
            s_platform.srm = NULL;
            free(srm_engine->dma_tx_desc);
            free(srm_engine->dma_rx_desc);
            vSemaphoreDeleteWithCaps(srm_engine->base.sem);
            free(srm_engine);
        }
    } else if (ppa_engine->type == PPA_ENGINE_TYPE_BLEND) {
        ppa_blend_engine_t *blending_engine = __containerof(ppa_engine, ppa_blend_engine_t, base);
        s_platform.blend_engine_ref_count--;
        if (s_platform.blend_engine_ref_count == 0) {
            // // Stop accepting new transactions to blending engine
            // portENTER_CRITICAL(&blending_engine->base.spinlock);
            // blending_engine->base.in_accepting_trans_state = false;
            // portEXIT_CRITICAL(&blending_engine->base.spinlock);
            // // Wait until all transactions get processed
            // while (!STAILQ_EMPTY(&blending_engine->base.trans_stailq)); // TODO: Think twice, looks like I am not able to use engine semaphore to decide
            assert(STAILQ_EMPTY(&blending_engine->base.trans_stailq));
            // Now, time to free
            s_platform.blending = NULL;
            free(blending_engine->dma_tx_bg_desc);
            free(blending_engine->dma_tx_fg_desc);
            free(blending_engine->dma_rx_desc);
            vSemaphoreDeleteWithCaps(blending_engine->base.sem);
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

esp_err_t ppa_register_invoker(const ppa_invoker_config_t *config, ppa_invoker_handle_t *ret_invoker)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && ret_invoker, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    ppa_invoker_t *invoker = (ppa_invoker_t *)heap_caps_calloc(1, sizeof(ppa_invoker_t), PPA_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(invoker, ESP_ERR_NO_MEM, TAG, "no mem to register invoker");

    invoker->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    invoker->in_accepting_trans_state = true;
    if (config->operation_flag & PPA_OPERATION_FLAG_SRM) {
        ppa_engine_config_t engine_config = {
            .engine = PPA_ENGINE_TYPE_SRM,
        };
        ESP_GOTO_ON_ERROR(ppa_engine_acquire(&engine_config, &invoker->srm_engine), err, TAG, "unable to acquire SRM engine");
    }
    if (config->operation_flag & PPA_OPERATION_FLAG_BLEND || config->operation_flag & PPA_OPERATION_FLAG_FILL) {
        ppa_engine_config_t engine_config = {
            .engine = PPA_ENGINE_TYPE_BLEND,
        };
        ESP_GOTO_ON_ERROR(ppa_engine_acquire(&engine_config, &invoker->blending_engine), err, TAG, "unable to acquire Blending engine");
    }
    *ret_invoker = invoker;

err:
    if (ret != ESP_OK) {
        ppa_unregister_invoker(invoker);
    }
    return ret;
}

esp_err_t ppa_unregister_invoker(ppa_invoker_handle_t ppa_invoker)
{
    ESP_RETURN_ON_FALSE(ppa_invoker, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    bool do_unregister = false;
    portENTER_CRITICAL(&ppa_invoker->spinlock);
    if (ppa_invoker->srm_trans_cnt == 0 && ppa_invoker->blending_trans_cnt == 0) {
        ppa_invoker->in_accepting_trans_state = false;
        do_unregister = true;
    }
    portEXIT_CRITICAL(&ppa_invoker->spinlock);
    ESP_RETURN_ON_FALSE(do_unregister, ESP_ERR_INVALID_STATE, TAG, "invoker still has unprocessed trans");

    if (ppa_invoker->srm_engine) {
        ppa_engine_release(ppa_invoker->srm_engine);
    }
    if (ppa_invoker->blending_engine) {
        ppa_engine_release(ppa_invoker->blending_engine);
    }
    free(ppa_invoker);
    return ESP_OK;
}

// // TODO: pm lock?
// esp_err_t ppa_module_acquire(const ppa_group_alloc_config_t *config, ppa_group_handle_t *ret_group)
// {
//     esp_err_t ret = ESP_OK;
//     ESP_RETURN_ON_FALSE(config && ret_group, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
//     ESP_RETURN_ON_FALSE(config->group_id < PPA_LL_GROUPS, ESP_ERR_INVALID_ARG, TAG, "invalid group_id");

//     int group_id = config->group_id;

//     // _lock_acquire(&s_platform.mutex);
//     // if (!s_platform.group[group_id]) {
//     //     ppa_group_t *pre_alloc_group = heap_caps_calloc(1, sizeof(ppa_group_t), PPA_MEM_ALLOC_CAPS);
//     //     if (pre_alloc_group) {
//     //         ppa_hal_init(&pre_alloc_group->hal, group_id); // initialize HAL context
//     //         pre_alloc_group->group_id = group_id;
//     //         pre_alloc_group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
//     //         s_platform.group[group_id] = pre_alloc_group; // register to platform
//     //         // Enable the bus clock to access PPA registers
//     //         PERIPH_RCC_ATOMIC() {
//     //             ppa_ll_enable_bus_clock(group_id, true);
//     //             ppa_ll_reset_register(group_id);
//     //         }

//     //         // Get 2D-DMA pool handle
//     //         dma2d_pool_config_t dma2d_config = {
//     //             .pool_id = 0,
//     //         };
//     //         ret = dma2d_acquire_pool(&dma2d_config, &s_platform.group[group_id]->dma2d_pool_handle);
//     //         if (ret != ESP_OK) {
//     //             ESP_LOGE(TAG, "install 2D-DMA failed");
//     //             // free(pre_alloc_group);
//     //             // s_platform.group[group_id] = NULL;
//     //         }
//     //     } else {
//     //         ret = ESP_ERR_NO_MEM;
//     //     }
//     // }

//     // // Register PPA SRM engine
//     // if (ret == ESP_OK && config->srm_engine_en && !s_platform.group[group_id]->srm) {
//     //     ppa_srm_engine_t *srm_engine = heap_caps_calloc(1, sizeof(ppa_srm_engine_t), PPA_MEM_ALLOC_CAPS);
//     //     SemaphoreHandle_t srm_sem = xSemaphoreCreateBinary();
//     //     dma2d_descriptor_t *srm_tx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS); // TODO: get cache line size by API
//     //     dma2d_descriptor_t *srm_rx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS);
//     //     if (srm_engine && srm_sem && srm_tx_dma_desc && srm_rx_dma_desc) {
//     //         srm_engine->dma_tx_desc = srm_tx_dma_desc;
//     //         srm_engine->dma_rx_desc = srm_rx_dma_desc;
//     //         srm_engine->base.group = s_platform.group[group_id];
//     //         srm_engine->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
//     //         srm_engine->base.sem = srm_sem;
//     //         xSemaphoreGive(srm_engine->base.sem);
//     //         srm_engine->base.in_accepting_trans_state = true;
//     //         STAILQ_INIT(&srm_engine->base.trans_stailq);
//     //         // srm_engine->base.event_cbs
//     //         s_platform.group[group_id]->srm = srm_engine;
//     //     } else {
//     //         ret = ESP_ERR_NO_MEM;
//     //         ESP_LOGE(TAG, "no mem to register PPA SRM engine");
//     //         free(srm_engine);
//     //         if (srm_sem) vSemaphoreDelete(srm_sem);
//     //         free(srm_tx_dma_desc);
//     //         free(srm_rx_dma_desc);
//     //     }
//     // }

//     // // Register PPA Blending engine
//     // if (ret == ESP_OK && config->blending_engine_en && !s_platform.group[group_id]->blending) {
//     //     ppa_blending_engine_t *blending_engine = heap_caps_calloc(1, sizeof(ppa_blending_engine_t), PPA_MEM_ALLOC_CAPS);
//     //     SemaphoreHandle_t blending_sem = xSemaphoreCreateBinary();
//     //     dma2d_descriptor_t *blending_tx_bg_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS); // TODO: get cache line size by API
//     //     dma2d_descriptor_t *blending_tx_fg_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS);
//     //     dma2d_descriptor_t *blending_rx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS);
//     //     if (blending_engine && blending_sem && blending_tx_bg_dma_desc && blending_tx_fg_dma_desc && blending_rx_dma_desc) {
//     //         blending_engine->dma_tx_bg_desc = blending_tx_bg_dma_desc;
//     //         blending_engine->dma_tx_fg_desc = blending_tx_fg_dma_desc;
//     //         blending_engine->dma_rx_desc = blending_rx_dma_desc;
//     //         blending_engine->base.group = s_platform.group[group_id];
//     //         blending_engine->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
//     //         blending_engine->base.sem = blending_sem;
//     //         xSemaphoreGive(blending_engine->base.sem);
//     //         blending_engine->base.in_accepting_trans_state = true;
//     //         STAILQ_INIT(&blending_engine->base.trans_stailq);
//     //         // blending_engine->base.event_cbs
//     //         s_platform.group[group_id]->blending = blending_engine;
//     //     } else {
//     //         ret = ESP_ERR_NO_MEM;
//     //         ESP_LOGE(TAG, "no mem to register PPA Blending engine");
//     //         free(blending_engine);
//     //         if (blending_sem) vSemaphoreDelete(blending_sem);
//     //         free(blending_tx_bg_dma_desc);
//     //         free(blending_tx_fg_dma_desc);
//     //         free(blending_rx_dma_desc);
//     //     }
//     // }
//     // _lock_release(&s_platform.mutex);

//     // ppa_module_release

//     bool new_group = false;
//     bool new_srm_engine = false;
//     bool new_blending_engine = false;
//     ppa_group_t *pre_alloc_group = heap_caps_calloc(1, sizeof(ppa_group_t), PPA_MEM_ALLOC_CAPS);
//     ppa_srm_engine_t *srm_engine = NULL;
//     ppa_blend_engine_t *blending_engine = NULL;
//     SemaphoreHandle_t srm_sem = NULL, blending_sem = NULL;

//     // portENTER_CRITICAL(&s_platform.spinlock);
//     if (!s_platform.group[group_id]) {
//         if (pre_alloc_group) {
//             new_group = true;
//             ppa_hal_init(&pre_alloc_group->hal, group_id); // initialize HAL context
//             pre_alloc_group->group_id = group_id;
//             pre_alloc_group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
//             s_platform.group[group_id] = pre_alloc_group; // register to platform
//             // Enable the bus clock to access PPA registers
//             PERIPH_RCC_ATOMIC() {
//                 ppa_ll_enable_bus_clock(group_id, true);
//                 ppa_ll_reset_register(group_id);
//             }
//         } else {
//             ret = ESP_ERR_NO_MEM;
//         }
//     }
//     // portEXIT_CRITICAL(&s_platform.spinlock);

//     if (new_group) {
//         // Get 2D-DMA pool handle
//         dma2d_pool_config_t dma2d_config = {
//             .pool_id = 0,
//         };
//         ret = dma2d_acquire_pool(&dma2d_config, &s_platform.group[group_id]->dma2d_pool_handle);
//         if (ret != ESP_OK) {
//             ESP_LOGE(TAG, "install 2D-DMA failed");
//             goto err;
//         }
//     }

//     if (ret == ESP_OK && config->srm_engine_en) {
//         srm_engine = heap_caps_calloc(1, sizeof(ppa_srm_engine_t), PPA_MEM_ALLOC_CAPS);
//         srm_sem = xSemaphoreCreateBinary();
//         dma2d_descriptor_t *srm_tx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS); // TODO: get cache line size by API
//         dma2d_descriptor_t *srm_rx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS);
//          // Register PPA SRM engine
//         portENTER_CRITICAL(&s_platform.group[group_id]->spinlock);
//         if (!s_platform.group[group_id]->srm) {
//             if (srm_engine && srm_sem && srm_tx_dma_desc && srm_rx_dma_desc) {
//                 new_srm_engine = true;
//                 srm_engine->dma_tx_desc = srm_tx_dma_desc;
//                 srm_engine->dma_rx_desc = srm_rx_dma_desc;
//                 srm_engine->base.group = s_platform.group[group_id];
//                 srm_engine->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
//                 srm_engine->base.sem = srm_sem;
//                 xSemaphoreGive(srm_engine->base.sem);
//                 srm_engine->base.in_accepting_trans_state = true;
//                 STAILQ_INIT(&srm_engine->base.trans_stailq);
//                 // srm_engine->base.event_cbs
//                 s_platform.group[group_id]->srm = srm_engine;
//             } else {
//                 ret = ESP_ERR_NO_MEM;
//             }
//         }
//         portEXIT_CRITICAL(&s_platform.group[group_id]->spinlock);
//         if (ret == ESP_ERR_NO_MEM) {
//             ESP_LOGE(TAG, "no mem to register PPA SRM engine");
//         }
//     }

//     if (ret == ESP_OK && config->blending_engine_en) {
//         blending_engine = heap_caps_calloc(1, sizeof(ppa_blend_engine_t), PPA_MEM_ALLOC_CAPS);
//         blending_sem = xSemaphoreCreateBinary();
//         dma2d_descriptor_t *blending_tx_bg_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS); // TODO: get cache line size by API
//         dma2d_descriptor_t *blending_tx_fg_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS);
//         dma2d_descriptor_t *blending_rx_dma_desc = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, 1, 64, PPA_MEM_ALLOC_CAPS);
//         // Register PPA Blending engine
//         portENTER_CRITICAL(&s_platform.group[group_id]->spinlock);
//         if (!s_platform.group[group_id]->blending) {
//             if (blending_engine && blending_sem && blending_tx_bg_dma_desc && blending_tx_fg_dma_desc && blending_rx_dma_desc) {
//                 new_blending_engine = true;
//                 blending_engine->dma_tx_bg_desc = blending_tx_bg_dma_desc;
//                 blending_engine->dma_tx_fg_desc = blending_tx_fg_dma_desc;
//                 blending_engine->dma_rx_desc = blending_rx_dma_desc;
//                 blending_engine->base.group = s_platform.group[group_id];
//                 blending_engine->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
//                 blending_engine->base.sem = blending_sem;
//                 xSemaphoreGive(blending_engine->base.sem);
//                 blending_engine->base.in_accepting_trans_state = true;
//                 STAILQ_INIT(&blending_engine->base.trans_stailq);
//                 // blending_engine->base.event_cbs
//                 s_platform.group[group_id]->blending = blending_engine;
//             } else {
//                 ret = ESP_ERR_NO_MEM;
//             }
//         }
//         portEXIT_CRITICAL(&s_platform.group[group_id]->spinlock);
//         if (ret == ESP_ERR_NO_MEM) {
//             ESP_LOGE(TAG, "no mem to register PPA Blending engine");
//         }
//     }

//     if (!new_srm_engine) {
//         free(srm_engine);
//         if (srm_sem) vSemaphoreDelete(srm_sem);
//         // TODO: free desc
//     }
//     if (!new_blending_engine) {
//         free(blending_engine);
//         if (blending_sem) vSemaphoreDelete(blending_sem);
//         // TODO: free desc
//     }
// err:
//     if (ret != ESP_OK) {
//         if (new_group) {
//             ppa_module_release(s_platform.group[group_id]);
//         }
//     }
//     if (!new_group) {
//         free(pre_alloc_group);
//     }
//     *ret_group = s_platform.group[group_id];
//     return ret;
// }

// esp_err_t ppa_module_release(ppa_group_handle_t ppa_group)
// {
//     esp_err_t ret = ESP_OK;
//     ESP_RETURN_ON_FALSE(ppa_group, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

//     bool do_deinitialize = false;
//     int group_id = ppa_group->group_id;
//     ppa_srm_engine_t *srm_engine = ppa_group->srm;
//     ppa_blend_engine_t *blending_engine = ppa_group->blending;
//     bool srm_no_waiting_trans = true;
//     bool blending_no_waiting_trans = true;

//     // portENTER_CRITICAL(&s_platform.spinlock);
//     portENTER_CRITICAL(&ppa_group->spinlock);
//     if (srm_engine) {
//         srm_engine->base.in_accepting_trans_state = false;
//         portENTER_CRITICAL(&srm_engine->base.spinlock);
//         srm_no_waiting_trans = STAILQ_EMPTY(&srm_engine->base.trans_stailq);
//         portEXIT_CRITICAL(&srm_engine->base.spinlock);
//     }
//     if (blending_engine) {
//         blending_engine->base.in_accepting_trans_state = false;
//         portENTER_CRITICAL(&blending_engine->base.spinlock);
//         blending_no_waiting_trans = STAILQ_EMPTY(&blending_engine->base.trans_stailq);
//         portEXIT_CRITICAL(&blending_engine->base.spinlock);
//     }
//     portEXIT_CRITICAL(&ppa_group->spinlock);
//     if (srm_no_waiting_trans && blending_no_waiting_trans) {
//         do_deinitialize = true;
//         ppa_group->srm = NULL;
//         ppa_group->blending = NULL;
//         s_platform.group[group_id] = NULL;
//     } else {
//         ret = ESP_FAIL;
//     }
//     // portEXIT_CRITICAL(&s_platform.spinlock);

//     if (do_deinitialize) {
//         if (srm_engine) {
//             free(srm_engine->dma_tx_desc);
//             free(srm_engine->dma_rx_desc);
//             vSemaphoreDelete(srm_engine->base.sem);
//             free(srm_engine);
//         }
//         if (blending_engine) {
//             free(blending_engine->dma_tx_bg_desc);
//             free(blending_engine->dma_tx_fg_desc);
//             free(blending_engine->dma_rx_desc);
//             vSemaphoreDelete(blending_engine->base.sem);
//             free(blending_engine);
//         }
//         dma2d_release_pool(ppa_group->dma2d_pool_handle);
//         // Disable the bus clock to access PPA registers
//         PERIPH_RCC_ATOMIC() {
//             ppa_ll_enable_bus_clock(group_id, false);
//         }
//         free(ppa_group);
//     }
//     return ret;
// }

// Each PPA engine should only have one transaction being pushed to 2D-DMA queue, the rest transactions should stay in engine's own transaction queue.
// This is to avoid 2D-DMA channels being hold, but not actually being used (waiting for PPA engine to be free)
static esp_err_t ppa_dma2d_enqueue(const ppa_trans_t *trans_elm)
{
    return dma2d_enqueue(s_platform.dma2d_pool_handle, trans_elm->trans_desc, trans_elm->dma_trans_placeholder);
}

static void ppa_recycle_transaction(ppa_trans_t *trans_elm)
{
    if (trans_elm) {
        if (trans_elm->trans_desc) {
            ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)trans_elm->trans_desc->user_config;
            if (trans_on_picked_desc) {
                free(trans_on_picked_desc->op_desc);
                free(trans_on_picked_desc);
            }
            free(trans_elm->trans_desc);
        }
        if (trans_elm->sem) {
            vSemaphoreDeleteWithCaps(trans_elm->sem);
        }
        free(trans_elm);
    }
}

static esp_err_t ppa_prepare_trans_elm(ppa_invoker_handle_t ppa_invoker, ppa_engine_t *ppa_engine_base, ppa_operation_t ppa_operation, const void *oper_config, ppa_trans_mode_t mode, ppa_trans_t **trans_elm)
{
    esp_err_t ret = ESP_OK;
    ppa_engine_type_t engine_type = ppa_engine_base->type;

    ppa_trans_t *new_trans_elm = (ppa_trans_t *)heap_caps_calloc(1, sizeof(ppa_trans_t), PPA_MEM_ALLOC_CAPS);
    dma2d_trans_t *dma_trans_elm = (dma2d_trans_t *)heap_caps_calloc(1, SIZEOF_DMA2D_TRANS_T, PPA_MEM_ALLOC_CAPS);
    dma2d_trans_config_t *dma_trans_desc = (dma2d_trans_config_t *)heap_caps_calloc(1, sizeof(dma2d_trans_config_t), PPA_MEM_ALLOC_CAPS);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)heap_caps_calloc(1, sizeof(ppa_dma2d_trans_on_picked_config_t), PPA_MEM_ALLOC_CAPS);
    size_t ppa_trans_desc_size = (ppa_operation == PPA_OPERATION_SRM) ? sizeof(ppa_srm_oper_t) :
                                 (ppa_operation == PPA_OPERATION_BLEND) ? sizeof(ppa_blend_oper_t) :
                                 (ppa_operation == PPA_OPERATION_FILL) ? sizeof(ppa_fill_oper_t) : 0;
    assert(ppa_trans_desc_size != 0);
    void *ppa_trans_desc = heap_caps_calloc(1, ppa_trans_desc_size, PPA_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(new_trans_elm && dma_trans_elm && dma_trans_desc && trans_on_picked_desc && ppa_trans_desc, ESP_ERR_NO_MEM, err, TAG, "no mem for transaction storage");
    if (mode == PPA_TRANS_MODE_BLOCKING) {
        new_trans_elm->sem = xSemaphoreCreateBinaryWithCaps(PPA_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(new_trans_elm->sem, ESP_ERR_NO_MEM, err, TAG, "no mem for transaction storage");
    }

    size_t cpy_size = (ppa_operation == PPA_OPERATION_SRM) ? sizeof(ppa_srm_operation_config_t) :
                      (ppa_operation == PPA_OPERATION_BLEND) ? sizeof(ppa_blend_operation_config_t) :
                      (ppa_operation == PPA_OPERATION_FILL) ? sizeof(ppa_fill_operation_config_t) : 0;
    memcpy(ppa_trans_desc, oper_config, cpy_size);

    trans_on_picked_desc->op_desc = ppa_trans_desc;
    trans_on_picked_desc->ppa_engine = ppa_engine_base;
    trans_on_picked_desc->trans_elm = new_trans_elm;
    trans_on_picked_desc->trigger_periph = (engine_type == PPA_ENGINE_TYPE_SRM) ? DMA2D_TRIG_PERIPH_PPA_SRM : DMA2D_TRIG_PERIPH_PPA_BLEND;

    dma_trans_desc->tx_channel_num = (ppa_operation == PPA_OPERATION_SRM) ? 1 :
                                     (ppa_operation == PPA_OPERATION_BLEND) ? 2 : 0; // PPA_OPERATION_FILL does not have data input
    dma_trans_desc->rx_channel_num = 1;

    // dma_trans_desc->specified_tx_channel_mask = ppa_specified_tx_channel_mask;
    // dma_trans_desc->specified_rx_channel_mask = ppa_specified_rx_channel_mask;

    dma_trans_desc->user_config = (void *)trans_on_picked_desc;
    dma_trans_desc->on_job_picked = ppa_oper_trans_on_picked_func[ppa_operation];

    new_trans_elm->trans_desc = dma_trans_desc;
    new_trans_elm->dma_trans_placeholder = dma_trans_elm;
    new_trans_elm->invoker = ppa_invoker;

    *trans_elm = new_trans_elm;
err:
    if (ret != ESP_OK) {
        ppa_recycle_transaction(new_trans_elm);
        *trans_elm = NULL;
    }
    return ret;
}

static esp_err_t ppa_do_operation(ppa_invoker_handle_t ppa_invoker, ppa_engine_t *ppa_engine_base, ppa_trans_t *trans_elm, ppa_trans_mode_t mode)
{
    esp_err_t ret = ESP_OK;
    ppa_engine_type_t engine_type = ppa_engine_base->type;

    portENTER_CRITICAL(&ppa_invoker->spinlock);
    if (ppa_invoker->in_accepting_trans_state) {
        // Send transaction into PPA engine queue
        STAILQ_INSERT_TAIL(&ppa_engine_base->trans_stailq, trans_elm, entry);
        if (engine_type == PPA_ENGINE_TYPE_SRM) {
            ppa_invoker->srm_trans_cnt++;
        } else {
            ppa_invoker->blending_trans_cnt++;
        }
    } else {
        ret = ESP_FAIL;
    }
    portEXIT_CRITICAL(&ppa_invoker->spinlock);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "The invoker cannot accept transaction now");
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
            ret = ppa_dma2d_enqueue(trans_elm);
            if (ret != ESP_OK) {
                portENTER_CRITICAL(&ppa_engine_base->spinlock);
                STAILQ_REMOVE(&ppa_engine_base->trans_stailq, trans_elm, ppa_trans_s, entry);
                portEXIT_CRITICAL(&ppa_engine_base->spinlock);
                xSemaphoreGive(ppa_engine_base->sem);
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
        ppa_recycle_transaction(trans_elm);
    }

err:
    return ret;
}

static bool ppa_transaction_done_cb(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    BaseType_t HPTaskAwoken;
    ppa_trans_t *trans_elm = (ppa_trans_t *)user_data;
    ppa_invoker_t *invoker = trans_elm->invoker;
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)trans_elm->trans_desc->user_config;
    ppa_engine_t *engine_base = trans_on_picked_desc->ppa_engine;
    ppa_engine_type_t engine_type = engine_base->type;

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
        xSemaphoreGiveFromISR(engine_base->sem, &HPTaskAwoken);
        need_yield |= (HPTaskAwoken == pdTRUE);
    }
    // Recycle transaction or give transaction semaphore
    if (trans_elm->sem != NULL) {
        xSemaphoreGiveFromISR(trans_elm->sem, &HPTaskAwoken);
        need_yield |= (HPTaskAwoken == pdTRUE);
    } else {
        ppa_recycle_transaction(trans_elm);
    }

    portENTER_CRITICAL_ISR(&invoker->spinlock);
    if (engine_type == PPA_ENGINE_TYPE_SRM) {
        invoker->srm_trans_cnt--;
    } else {
        invoker->blending_trans_cnt--;
    }
    portEXIT_CRITICAL_ISR(&invoker->spinlock);

    // TODO: how to notify non-blocking transaction

    return need_yield;
}

static bool ppa_srm_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(num_chans == 2 && dma2d_chans && user_config);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)user_config;
    assert(trans_on_picked_desc->trigger_periph == DMA2D_TRIG_PERIPH_PPA_SRM && trans_on_picked_desc->srm_desc && trans_on_picked_desc->ppa_engine);

    ppa_srm_oper_t *srm_trans_desc = trans_on_picked_desc->srm_desc;
    ppa_srm_engine_t *srm_engine = __containerof(trans_on_picked_desc->ppa_engine, ppa_srm_engine_t, base);

    // Free 2D-DMA transaction placeholder (transaction has already been moved out from 2D-DMA queue)
    free(trans_on_picked_desc->trans_elm->dma_trans_placeholder);

    // Get the required 2D-DMA channel handles
    uint32_t dma2d_tx_chan_idx = 0;
    uint32_t dma2d_rx_chan_idx = 1;
    if (dma2d_chans[0].dir == DMA2D_CHANNEL_DIRECTION_RX) {
        dma2d_tx_chan_idx = 1;
        dma2d_rx_chan_idx = 0;
    }
    dma2d_channel_handle_t dma2d_tx_chan = dma2d_chans[dma2d_tx_chan_idx].chan;
    dma2d_channel_handle_t dma2d_rx_chan = dma2d_chans[dma2d_rx_chan_idx].chan;

    // Write back and invalidate are performed on the entire picture (the window content is not continuous in the buffer)
    // Write back in_buffer
    color_space_pixel_format_t in_pixel_format = {
        .color_type_id = srm_trans_desc->in_color.mode,
    };
    uint32_t in_buffer_len = srm_trans_desc->in_pic_w * srm_trans_desc->in_pic_h * color_hal_pixel_format_get_bit_depth(in_pixel_format) / 8;
    esp_cache_msync(srm_trans_desc->in_buffer, in_buffer_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    // Invalidate out_buffer
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = srm_trans_desc->out_color.mode,
    };
    uint32_t out_buffer_len = srm_trans_desc->out_pic_w * srm_trans_desc->out_pic_h * color_hal_pixel_format_get_bit_depth(out_pixel_format) / 8;
    esp_cache_msync(srm_trans_desc->out_buffer, out_buffer_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

    // Fill 2D-DMA descriptors
    srm_engine->dma_tx_desc->vb_size = srm_trans_desc->in_block_h;
    srm_engine->dma_tx_desc->hb_length = srm_trans_desc->in_block_w;
    srm_engine->dma_tx_desc->err_eof = 0;
    srm_engine->dma_tx_desc->dma2d_en = 1;
    srm_engine->dma_tx_desc->suc_eof = 1;
    srm_engine->dma_tx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    srm_engine->dma_tx_desc->va_size = srm_trans_desc->in_pic_h;
    srm_engine->dma_tx_desc->ha_length = srm_trans_desc->in_pic_w;
    srm_engine->dma_tx_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(in_pixel_format);
    srm_engine->dma_tx_desc->y = srm_trans_desc->in_block_offset_y;
    srm_engine->dma_tx_desc->x = srm_trans_desc->in_block_offset_x;
    srm_engine->dma_tx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    srm_engine->dma_tx_desc->buffer = (void *)srm_trans_desc->in_buffer;
    srm_engine->dma_tx_desc->next = NULL;

    // vb_size, hb_length can be any value (auto writeback). However, if vb_size/hb_length is 0, it triggers 2D-DMA DESC_ERROR interrupt, and dma2d driver will automatically ends the transaction. Therefore, to avoid this, we set them to 1.
    srm_engine->dma_rx_desc->vb_size = 1;
    srm_engine->dma_rx_desc->hb_length = 1;
    srm_engine->dma_rx_desc->err_eof = 0;
    srm_engine->dma_rx_desc->dma2d_en = 1;
    srm_engine->dma_rx_desc->suc_eof = 1;
    srm_engine->dma_rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    srm_engine->dma_rx_desc->va_size = srm_trans_desc->out_pic_h;
    srm_engine->dma_rx_desc->ha_length = srm_trans_desc->out_pic_w;
    // pbyte can be any value
    srm_engine->dma_rx_desc->y = srm_trans_desc->out_block_offset_y;
    srm_engine->dma_rx_desc->x = srm_trans_desc->out_block_offset_x;
    srm_engine->dma_rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    srm_engine->dma_rx_desc->buffer = (void *)srm_trans_desc->out_buffer;
    srm_engine->dma_rx_desc->next = NULL;

    esp_cache_msync((void *)srm_engine->dma_tx_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)srm_engine->dma_rx_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    // printf("desc addr: %p\n", srm_engine->dma_rx_desc);
    // printf("desc content: %08lX, %08lX, %08lX, %08lX, %08lX\n", *(uint32_t *)srm_engine->dma_rx_desc, *(uint32_t *)((uint32_t)srm_engine->dma_rx_desc + 4), *(uint32_t *)((uint32_t)srm_engine->dma_rx_desc + 8), *(uint32_t *)((uint32_t)srm_engine->dma_rx_desc + 12), *(uint32_t *)((uint32_t)srm_engine->dma_rx_desc + 16));

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
    };
    dma2d_set_transfer_ability(dma2d_tx_chan, &dma_transfer_ability);
    dma2d_set_transfer_ability(dma2d_rx_chan, &dma_transfer_ability);

    // YUV444 and YUV422 are not supported by PPA module, need to utilize 2D-DMA color space conversion feature to do a conversion
    ppa_srm_color_mode_t ppa_in_color_mode = srm_trans_desc->in_color.mode;
    if (ppa_in_color_mode == PPA_SRM_COLOR_MODE_YUV444) {
        ppa_in_color_mode = PPA_SRM_COLOR_MODE_RGB888;
        dma2d_csc_config_t dma_tx_csc = {0};
        if (srm_trans_desc->in_color.yuv_std == COLOR_CONV_STD_RGB_YUV_BT601) {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV444_TO_RGB888_601;
        } else {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV444_TO_RGB888_709;
        }
        dma2d_configure_color_space_conversion(dma2d_tx_chan, &dma_tx_csc);
    } else if (ppa_in_color_mode == PPA_SRM_COLOR_MODE_YUV422) {
        ppa_in_color_mode = PPA_SRM_COLOR_MODE_RGB888;
        dma2d_csc_config_t dma_tx_csc = {0};
        if (srm_trans_desc->in_color.yuv_std == COLOR_CONV_STD_RGB_YUV_BT601) {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV422_TO_RGB888_601;
        } else {
            dma_tx_csc.tx_csc_option = DMA2D_CSC_TX_YUV422_TO_RGB888_709;
        }
        dma2d_configure_color_space_conversion(dma2d_tx_chan, &dma_tx_csc);
    }

    ppa_srm_color_mode_t ppa_out_color_mode = srm_trans_desc->out_color.mode;
    if (ppa_out_color_mode == PPA_SRM_COLOR_MODE_YUV444) {
        ppa_out_color_mode = PPA_SRM_COLOR_MODE_YUV420;
        dma2d_csc_config_t dma_rx_csc = {
            .rx_csc_option = DMA2D_CSC_RX_YUV420_TO_YUV444,
        };
        dma2d_configure_color_space_conversion(dma2d_rx_chan, &dma_rx_csc); // TODO: RX CSC only on channel 0
    }

    dma2d_rx_event_callbacks_t dma_event_cbs = {
        .on_recv_eof = ppa_transaction_done_cb,
    };
    dma2d_register_rx_event_callbacks(dma2d_rx_chan, &dma_event_cbs, (void *)trans_on_picked_desc->trans_elm);

    ppa_ll_srm_reset(s_platform.hal.dev);

    dma2d_set_desc_addr(dma2d_tx_chan, (intptr_t)srm_engine->dma_tx_desc);
    dma2d_set_desc_addr(dma2d_rx_chan, (intptr_t)srm_engine->dma_rx_desc);
    dma2d_start(dma2d_tx_chan);
    dma2d_start(dma2d_rx_chan);

    // Configure PPA SRM engine
    ppa_ll_srm_set_rx_color_mode(s_platform.hal.dev, ppa_in_color_mode);
    if (COLOR_SPACE_TYPE(ppa_in_color_mode) == COLOR_SPACE_YUV) {
        ppa_ll_srm_set_rx_yuv_range(s_platform.hal.dev, srm_trans_desc->in_color.yuv_range);
        ppa_ll_srm_set_yuv2rgb_std(s_platform.hal.dev, srm_trans_desc->in_color.yuv_std);
    }
    ppa_ll_srm_enable_rx_byte_swap(s_platform.hal.dev, srm_trans_desc->in_color.byte_swap);
    ppa_ll_srm_enable_rx_rgb_swap(s_platform.hal.dev, srm_trans_desc->in_color.rgb_swap);
    ppa_ll_srm_configure_rx_alpha(s_platform.hal.dev, srm_trans_desc->in_color.alpha_mode, srm_trans_desc->in_color.alpha_value);

    ppa_ll_srm_set_tx_color_mode(s_platform.hal.dev, ppa_out_color_mode);
    if (COLOR_SPACE_TYPE(ppa_out_color_mode) == COLOR_SPACE_YUV) {
        ppa_ll_srm_set_rx_yuv_range(s_platform.hal.dev, srm_trans_desc->out_color.yuv_range);
        ppa_ll_srm_set_yuv2rgb_std(s_platform.hal.dev, srm_trans_desc->out_color.yuv_std);
    }

    // TODO: sr_macro_bk_ro_bypass
    // PPA.sr_byte_order.sr_macro_bk_ro_bypass = 1;
    ppa_ll_srm_set_rotation_angle(s_platform.hal.dev, srm_trans_desc->rotation_angle);
    ppa_ll_srm_set_scaling_x(s_platform.hal.dev, srm_trans_desc->scale_x_int, srm_trans_desc->scale_x_frag);
    ppa_ll_srm_set_scaling_y(s_platform.hal.dev, srm_trans_desc->scale_y_int, srm_trans_desc->scale_y_frag);
    ppa_ll_srm_enable_mirror_x(s_platform.hal.dev, srm_trans_desc->mirror_x);
    ppa_ll_srm_enable_mirror_y(s_platform.hal.dev, srm_trans_desc->mirror_y);

    ppa_ll_srm_start(s_platform.hal.dev);

    // No need to yield
    return false;
}

esp_err_t ppa_do_scale_rotate_mirror(ppa_invoker_handle_t ppa_invoker, const ppa_srm_operation_config_t *oper_config, const ppa_trans_config_t *trans_config)
{
    ESP_RETURN_ON_FALSE(ppa_invoker && oper_config && trans_config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ppa_invoker->srm_engine, ESP_ERR_INVALID_ARG, TAG, "invoker did not register to SRM engine");
    ESP_RETURN_ON_FALSE(trans_config->mode <= PPA_TRANS_MODE_NON_BLOCKING, ESP_ERR_INVALID_ARG, TAG, "invalid mode");
    // Any restrictions on in/out buffer address? alignment? alignment restriction comes from cache, its addr and size need to be aligned to cache line size on 912!
    // buffer on stack/heap
    // ESP_RETURN_ON_FALSE(config->rotation_angle)
    // ESP_RETURN_ON_FALSE(config->in/out_color_mode)
    // what if in_color is YUV420, out is RGB, what is out RGB range? Full range?
    ESP_RETURN_ON_FALSE(oper_config->scale_x < (PPA_LL_SRM_SCALING_INT_MAX + 1) && oper_config->scale_x >= (1.0 / PPA_LL_SRM_SCALING_FRAG_MAX) &&
                        oper_config->scale_y < (PPA_LL_SRM_SCALING_INT_MAX + 1) && oper_config->scale_y >= (1.0 / PPA_LL_SRM_SCALING_FRAG_MAX),
                        ESP_ERR_INVALID_ARG, TAG, "invalid scale");
    // byte/rgb swap with color mode only to (A)RGB color space?

    // YUV420: in desc, ha/hb/va/vb/x/y must be even number

    // TODO: Maybe do buffer writeback and invalidation here, instead of in on_picked?

    ppa_trans_t *trans_elm = NULL;
    esp_err_t ret = ppa_prepare_trans_elm(ppa_invoker, ppa_invoker->srm_engine, PPA_OPERATION_SRM, (void *)oper_config, trans_config->mode, &trans_elm);
    if (ret == ESP_OK) {
        assert(trans_elm);

        // Pre-process some data
        ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = trans_elm->trans_desc->user_config;
        ppa_srm_oper_t *srm_trans_desc = trans_on_picked_desc->srm_desc;
        srm_trans_desc->scale_x_int = (uint32_t)srm_trans_desc->scale_x;
        srm_trans_desc->scale_x_frag = (uint32_t)(srm_trans_desc->scale_x * (PPA_LL_SRM_SCALING_FRAG_MAX + 1)) & PPA_LL_SRM_SCALING_FRAG_MAX;
        srm_trans_desc->scale_y_int = (uint32_t)srm_trans_desc->scale_y;
        srm_trans_desc->scale_y_frag = (uint32_t)(srm_trans_desc->scale_y * (PPA_LL_SRM_SCALING_FRAG_MAX + 1)) & PPA_LL_SRM_SCALING_FRAG_MAX;

        ret = ppa_do_operation(ppa_invoker, ppa_invoker->srm_engine, trans_elm, trans_config->mode);
        if (ret != ESP_OK) {
            ppa_recycle_transaction(trans_elm);
        }
    }
    return ret;
}

static bool ppa_blend_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(num_chans == 3 && dma2d_chans && user_config);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)user_config;
    assert(trans_on_picked_desc->trigger_periph == DMA2D_TRIG_PERIPH_PPA_BLEND && trans_on_picked_desc->blend_desc && trans_on_picked_desc->ppa_engine);

    ppa_blend_oper_t *blend_trans_desc = trans_on_picked_desc->blend_desc;
    ppa_blend_engine_t *blend_engine = __containerof(trans_on_picked_desc->ppa_engine, ppa_blend_engine_t, base);

    // Free 2D-DMA transaction placeholder (transaction has already been moved out from 2D-DMA queue)
    free(trans_on_picked_desc->trans_elm->dma_trans_placeholder);

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

    // Write back and invalidate are performed on the entire picture (the window content is not continuous in the buffer)
    // Write back in_bg_buffer, in_fg_buffer
    color_space_pixel_format_t in_bg_pixel_format = {
        .color_type_id = blend_trans_desc->in_bg_color.mode,
    };
    uint32_t in_bg_buffer_len = blend_trans_desc->in_bg_pic_w * blend_trans_desc->in_bg_pic_h * color_hal_pixel_format_get_bit_depth(in_bg_pixel_format) / 8;
    esp_cache_msync(blend_trans_desc->in_bg_buffer, in_bg_buffer_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    color_space_pixel_format_t in_fg_pixel_format = {
        .color_type_id = blend_trans_desc->in_fg_color.mode,
    };
    uint32_t in_fg_buffer_len = blend_trans_desc->in_fg_pic_w * blend_trans_desc->in_fg_pic_h * color_hal_pixel_format_get_bit_depth(in_fg_pixel_format) / 8;
    esp_cache_msync(blend_trans_desc->in_fg_buffer, in_fg_buffer_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    // Invalidate out_buffer
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = blend_trans_desc->out_color.mode,
    };
    uint32_t out_buffer_len = blend_trans_desc->out_pic_w * blend_trans_desc->out_pic_h * color_hal_pixel_format_get_bit_depth(out_pixel_format) / 8;
    esp_cache_msync(blend_trans_desc->out_buffer, out_buffer_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

    // Fill 2D-DMA descriptors
    blend_engine->dma_tx_bg_desc->vb_size = blend_trans_desc->in_bg_fg_block_h;
    blend_engine->dma_tx_bg_desc->hb_length = blend_trans_desc->in_bg_fg_block_w;
    blend_engine->dma_tx_bg_desc->err_eof = 0;
    blend_engine->dma_tx_bg_desc->dma2d_en = 1;
    blend_engine->dma_tx_bg_desc->suc_eof = 1;
    blend_engine->dma_tx_bg_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_tx_bg_desc->va_size = blend_trans_desc->in_bg_pic_h;
    blend_engine->dma_tx_bg_desc->ha_length = blend_trans_desc->in_bg_pic_w;
    blend_engine->dma_tx_bg_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(in_bg_pixel_format);
    blend_engine->dma_tx_bg_desc->y = blend_trans_desc->in_bg_block_offset_y;
    blend_engine->dma_tx_bg_desc->x = blend_trans_desc->in_bg_block_offset_x;
    blend_engine->dma_tx_bg_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_tx_bg_desc->buffer = (void *)blend_trans_desc->in_bg_buffer;
    blend_engine->dma_tx_bg_desc->next = NULL;

    blend_engine->dma_tx_fg_desc->vb_size = blend_trans_desc->in_bg_fg_block_h;
    blend_engine->dma_tx_fg_desc->hb_length = blend_trans_desc->in_bg_fg_block_w;
    blend_engine->dma_tx_fg_desc->err_eof = 0;
    blend_engine->dma_tx_fg_desc->dma2d_en = 1;
    blend_engine->dma_tx_fg_desc->suc_eof = 1;
    blend_engine->dma_tx_fg_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_tx_fg_desc->va_size = blend_trans_desc->in_fg_pic_h;
    blend_engine->dma_tx_fg_desc->ha_length = blend_trans_desc->in_fg_pic_w;
    blend_engine->dma_tx_fg_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(in_fg_pixel_format);
    blend_engine->dma_tx_fg_desc->y = blend_trans_desc->in_fg_block_offset_y;
    blend_engine->dma_tx_fg_desc->x = blend_trans_desc->in_fg_block_offset_x;
    blend_engine->dma_tx_fg_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_tx_fg_desc->buffer = (void *)blend_trans_desc->in_fg_buffer;
    blend_engine->dma_tx_fg_desc->next = NULL;

    blend_engine->dma_rx_desc->vb_size = blend_trans_desc->in_bg_fg_block_h;
    blend_engine->dma_rx_desc->hb_length = blend_trans_desc->in_bg_fg_block_w;
    blend_engine->dma_rx_desc->err_eof = 0;
    blend_engine->dma_rx_desc->dma2d_en = 1;
    blend_engine->dma_rx_desc->suc_eof = 1;
    blend_engine->dma_rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_rx_desc->va_size = blend_trans_desc->out_pic_h;
    blend_engine->dma_rx_desc->ha_length = blend_trans_desc->out_pic_w;
    blend_engine->dma_rx_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(out_pixel_format);
    blend_engine->dma_rx_desc->y = blend_trans_desc->out_block_offset_y;
    blend_engine->dma_rx_desc->x = blend_trans_desc->out_block_offset_x;
    blend_engine->dma_rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_rx_desc->buffer = (void *)blend_trans_desc->out_buffer;
    blend_engine->dma_rx_desc->next = NULL;

    esp_cache_msync((void *)blend_engine->dma_tx_bg_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)blend_engine->dma_tx_fg_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)blend_engine->dma_rx_desc, s_platform.dma_desc_mem_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    // printf("tx bg desc content: %08lX, %08lX, %08lX, %08lX, %08lX\n", *(uint32_t *)blend_engine->dma_tx_bg_desc, *(uint32_t *)((uint32_t)blend_engine->dma_tx_bg_desc + 4), *(uint32_t *)((uint32_t)blend_engine->dma_tx_bg_desc + 8), *(uint32_t *)((uint32_t)blend_engine->dma_tx_bg_desc + 12), *(uint32_t *)((uint32_t)blend_engine->dma_tx_bg_desc + 16));
    // printf("tx fg desc content: %08lX, %08lX, %08lX, %08lX, %08lX\n", *(uint32_t *)blend_engine->dma_tx_fg_desc, *(uint32_t *)((uint32_t)blend_engine->dma_tx_fg_desc + 4), *(uint32_t *)((uint32_t)blend_engine->dma_tx_fg_desc + 8), *(uint32_t *)((uint32_t)blend_engine->dma_tx_fg_desc + 12), *(uint32_t *)((uint32_t)blend_engine->dma_tx_fg_desc + 16));
    // printf("rx desc content: %08lX, %08lX, %08lX, %08lX, %08lX\n", *(uint32_t *)blend_engine->dma_rx_desc, *(uint32_t *)((uint32_t)blend_engine->dma_rx_desc + 4), *(uint32_t *)((uint32_t)blend_engine->dma_rx_desc + 8), *(uint32_t *)((uint32_t)blend_engine->dma_rx_desc + 12), *(uint32_t *)((uint32_t)blend_engine->dma_rx_desc + 16));

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

    ppa_ll_blend_reset(s_platform.hal.dev);

    dma2d_set_desc_addr(dma2d_tx_bg_chan, (intptr_t)blend_engine->dma_tx_bg_desc);
    dma2d_set_desc_addr(dma2d_tx_fg_chan, (intptr_t)blend_engine->dma_tx_fg_desc);
    dma2d_set_desc_addr(dma2d_rx_chan, (intptr_t)blend_engine->dma_rx_desc);
    dma2d_start(dma2d_tx_bg_chan);
    dma2d_start(dma2d_tx_fg_chan);
    dma2d_start(dma2d_rx_chan);

    // Configure PPA Blending engine
    ppa_ll_blend_set_rx_bg_color_mode(s_platform.hal.dev, blend_trans_desc->in_bg_color.mode);
    ppa_ll_blend_enable_rx_bg_byte_swap(s_platform.hal.dev, blend_trans_desc->in_bg_color.byte_swap);
    ppa_ll_blend_enable_rx_bg_rgb_swap(s_platform.hal.dev, blend_trans_desc->in_bg_color.rgb_swap);
    ppa_ll_blend_configure_rx_bg_alpha(s_platform.hal.dev, blend_trans_desc->in_bg_color.alpha_mode, blend_trans_desc->in_bg_color.alpha_value);

    ppa_ll_blend_set_rx_fg_color_mode(s_platform.hal.dev, blend_trans_desc->in_fg_color.mode);
    if (COLOR_SPACE_TYPE(blend_trans_desc->in_fg_color.mode) == COLOR_SPACE_ALPHA) {
        ppa_ll_blend_set_rx_fg_fix_rgb(s_platform.hal.dev, blend_trans_desc->in_fg_color.fix_rgb_val);
    }
    ppa_ll_blend_enable_rx_fg_byte_swap(s_platform.hal.dev, blend_trans_desc->in_fg_color.byte_swap);
    ppa_ll_blend_enable_rx_fg_rgb_swap(s_platform.hal.dev, blend_trans_desc->in_fg_color.rgb_swap);
    ppa_ll_blend_configure_rx_fg_alpha(s_platform.hal.dev, blend_trans_desc->in_fg_color.alpha_mode, blend_trans_desc->in_fg_color.alpha_value);

    ppa_ll_blend_set_tx_color_mode(s_platform.hal.dev, blend_trans_desc->out_color.mode);

    ppa_ll_blend_start(s_platform.hal.dev, PPA_LL_BLEND_TRANS_MODE_BLEND);

    // No need to yield
    return false;
}

esp_err_t ppa_do_blend(ppa_invoker_handle_t ppa_invoker, const ppa_blend_operation_config_t *oper_config, const ppa_trans_config_t *trans_config)
{
    ESP_RETURN_ON_FALSE(ppa_invoker && oper_config && trans_config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ppa_invoker->blending_engine, ESP_ERR_INVALID_ARG, TAG, "invoker did not register to Blending engine");
    ESP_RETURN_ON_FALSE(trans_config->mode <= PPA_TRANS_MODE_NON_BLOCKING, ESP_ERR_INVALID_ARG, TAG, "invalid mode");
    // TODO: ARG CHECK
    // 当输入类型为 L4、A4 时，图像块的尺寸 hb 以及在图像中的偏移 x 必须为偶数

    // TODO: Maybe do buffer writeback and invalidation here, instead of in on_picked?

    ppa_trans_t *trans_elm = NULL;
    esp_err_t ret = ppa_prepare_trans_elm(ppa_invoker, ppa_invoker->blending_engine, PPA_OPERATION_BLEND, (void *)oper_config, trans_config->mode, &trans_elm);
    if (ret == ESP_OK) {
        assert(trans_elm);

        ret = ppa_do_operation(ppa_invoker, ppa_invoker->blending_engine, trans_elm, trans_config->mode);
        if (ret != ESP_OK) {
            ppa_recycle_transaction(trans_elm);
        }
    }
    return ret;
}

static bool ppa_fill_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(num_chans == 1 && dma2d_chans && user_config);
    ppa_dma2d_trans_on_picked_config_t *trans_on_picked_desc = (ppa_dma2d_trans_on_picked_config_t *)user_config;
    assert(trans_on_picked_desc->trigger_periph == DMA2D_TRIG_PERIPH_PPA_BLEND && trans_on_picked_desc->fill_desc && trans_on_picked_desc->ppa_engine);

    ppa_fill_oper_t *fill_trans_desc = trans_on_picked_desc->fill_desc;
    ppa_blend_engine_t *blend_engine = __containerof(trans_on_picked_desc->ppa_engine, ppa_blend_engine_t, base);

    // Free 2D-DMA transaction placeholder (transaction has already been moved out from 2D-DMA queue)
    free(trans_on_picked_desc->trans_elm->dma_trans_placeholder);

    // Get the required 2D-DMA channel handles
    assert(dma2d_chans[0].dir == DMA2D_CHANNEL_DIRECTION_RX);
    dma2d_channel_handle_t dma2d_rx_chan = dma2d_chans[0].chan;

    // Invalidate is performed on the entire picture (the window content is not continuous in the buffer)
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = fill_trans_desc->out_color.mode,
    };
    uint32_t out_buffer_len = fill_trans_desc->out_pic_w * fill_trans_desc->out_pic_h * color_hal_pixel_format_get_bit_depth(out_pixel_format) / 8;
    esp_cache_msync(fill_trans_desc->out_buffer, out_buffer_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

    // Fill 2D-DMA descriptors
    blend_engine->dma_rx_desc->vb_size = fill_trans_desc->fill_block_h;
    blend_engine->dma_rx_desc->hb_length = fill_trans_desc->fill_block_w;
    blend_engine->dma_rx_desc->err_eof = 0;
    blend_engine->dma_rx_desc->dma2d_en = 1;
    blend_engine->dma_rx_desc->suc_eof = 1;
    blend_engine->dma_rx_desc->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    blend_engine->dma_rx_desc->va_size = fill_trans_desc->out_pic_h;
    blend_engine->dma_rx_desc->ha_length = fill_trans_desc->out_pic_w;
    blend_engine->dma_rx_desc->pbyte = dma2d_desc_pixel_format_to_pbyte_value(out_pixel_format);
    blend_engine->dma_rx_desc->y = fill_trans_desc->out_block_offset_y;
    blend_engine->dma_rx_desc->x = fill_trans_desc->out_block_offset_x;
    blend_engine->dma_rx_desc->mode = DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE;
    blend_engine->dma_rx_desc->buffer = (void *)fill_trans_desc->out_buffer;
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

    ppa_ll_blend_reset(s_platform.hal.dev);

    dma2d_set_desc_addr(dma2d_rx_chan, (intptr_t)blend_engine->dma_rx_desc);
    dma2d_start(dma2d_rx_chan);

    // Configure PPA Blending engine
    ppa_ll_blend_configure_filling_block(s_platform.hal.dev, fill_trans_desc->fill_argb_color, fill_trans_desc->fill_block_w, fill_trans_desc->fill_block_h);
    ppa_ll_blend_set_tx_color_mode(s_platform.hal.dev, fill_trans_desc->out_color.mode);

    ppa_ll_blend_start(s_platform.hal.dev, PPA_LL_BLEND_TRANS_MODE_FILL);

    // No need to yield
    return false;
}

esp_err_t ppa_do_fill(ppa_invoker_handle_t ppa_invoker, const ppa_fill_operation_config_t *oper_config, const ppa_trans_config_t *trans_config)
{
    ESP_RETURN_ON_FALSE(ppa_invoker && oper_config && trans_config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ppa_invoker->blending_engine, ESP_ERR_INVALID_ARG, TAG, "invoker did not register to Blending engine");
    ESP_RETURN_ON_FALSE(trans_config->mode <= PPA_TRANS_MODE_NON_BLOCKING, ESP_ERR_INVALID_ARG, TAG, "invalid mode");
    // TODO: ARG CHECK
    // fill_block_w <= PPA_BLEND_HB_V, fill_block_h <= PPA_BLEND_VB_V

    // TODO: Maybe do buffer invalidation here, instead of in on_picked?

    ppa_trans_t *trans_elm = NULL;
    esp_err_t ret = ppa_prepare_trans_elm(ppa_invoker, ppa_invoker->blending_engine, PPA_OPERATION_FILL, (void *)oper_config, trans_config->mode, &trans_elm);
    if (ret == ESP_OK) {
        assert(trans_elm);

        ret = ppa_do_operation(ppa_invoker, ppa_invoker->blending_engine, trans_elm, trans_config->mode);
        if (ret != ESP_OK) {
            ppa_recycle_transaction(trans_elm);
        }
    }
    return ret;
}
