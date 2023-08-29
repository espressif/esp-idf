/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/allocator.h"
#include "esp_bt_main.h"

#include "config/stack_config.h"

struct stack_config_env_tag {
    esp_bluedroid_config_t cfg;
    struct bluedroid_config interface;
};

static struct stack_config_env_tag *s_stack_config_env = NULL;

static bool get_ssp_enabled(void)
{
    assert(s_stack_config_env);
    esp_bluedroid_config_t *cfg = &s_stack_config_env->cfg;
    return cfg->ssp_en;
}

bt_status_t bluedriod_config_init(esp_bluedroid_config_t *cfg)
{
    s_stack_config_env = osi_calloc(sizeof(struct stack_config_env_tag));
    if (!s_stack_config_env) {
        return BT_STATUS_NOMEM;
    }

    memcpy(&s_stack_config_env->cfg, cfg, sizeof(esp_bluedroid_config_t));

    struct bluedroid_config *interface = &s_stack_config_env->interface;
    interface->get_ssp_enabled = get_ssp_enabled;

    return BT_STATUS_SUCCESS;
}

void bluedriod_config_deinit(void)
{
    if (s_stack_config_env) {
        osi_free(s_stack_config_env);
        s_stack_config_env = NULL;
    }
}

const struct bluedroid_config *bluedriod_config_get(void)
{
    assert(s_stack_config_env);
    return &s_stack_config_env->interface;
}
