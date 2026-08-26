/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * pthread port for Linux build
 */

#include <pthread.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_pthread.h"
#include "esp_heap_caps.h"

#include <string.h>
#include "freertos/FreeRTOS.h"

static pthread_key_t s_pthread_cfg_key;
static void esp_pthread_cfg_key_destructor(void *value)
{
    free(value);
}

static int get_default_pthread_core(void)
{
    return CONFIG_PTHREAD_TASK_CORE_DEFAULT == -1 ? tskNO_AFFINITY : CONFIG_PTHREAD_TASK_CORE_DEFAULT;
}

/**
 * @brief Creates a default pthread configuration based
 * on the values set via menuconfig.
 *
 * @return
 *      A default configuration structure.
 */
esp_pthread_cfg_t esp_pthread_get_default_config(void)
{
    esp_pthread_cfg_t cfg = {
        .stack_size = CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT,
        .prio = CONFIG_PTHREAD_TASK_PRIO_DEFAULT,
        .inherit_cfg = false,
        .thread_name = NULL,
        .pin_to_core = get_default_pthread_core(),
        .stack_alloc_caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT,
    };

    return cfg;
}

esp_err_t esp_pthread_set_cfg(const esp_pthread_cfg_t *cfg)
{
    // Not checking the stack size here since PTHREAD_STACK_MIN has two conflicting declarations on Linux

    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // 0 is treated as default value, hence change caps to MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL in that case
    int heap_caps;
    if (cfg->stack_alloc_caps == 0) {
        heap_caps = MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL;
    } else {
        // Check that memory is 8-bit capable
        if (!(cfg->stack_alloc_caps & MALLOC_CAP_8BIT)) {
            return ESP_ERR_INVALID_ARG;
        }

        heap_caps = cfg->stack_alloc_caps;
    }

    /* If a value is already set, update that value */
    esp_pthread_cfg_t *p = pthread_getspecific(s_pthread_cfg_key);
    if (!p) {
        p = malloc(sizeof(esp_pthread_cfg_t));
        if (!p) {
            return ESP_ERR_NO_MEM;
        }
    }
    *p = *cfg;
    p->stack_alloc_caps = heap_caps;
    p->stack_size = MAX(p->stack_size, 0x4000); // make sure Linux minimal stack size is respected

    int __attribute((unused)) res = pthread_setspecific(s_pthread_cfg_key, p);

    assert(res == 0);

    return ESP_OK;
}

esp_err_t esp_pthread_get_cfg(esp_pthread_cfg_t *p)
{
    if (p == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_pthread_cfg_t *cfg = pthread_getspecific(s_pthread_cfg_key);
    if (cfg) {
        *p = *cfg;
        return ESP_OK;
    }
    memset(p, 0, sizeof(*p));
    return ESP_ERR_NOT_FOUND;
}

__attribute__((constructor)) esp_err_t esp_pthread_init(void)
{
    if (pthread_key_create(&s_pthread_cfg_key, esp_pthread_cfg_key_destructor) != 0) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}
