/*
 * SPDX-FileCopyrightText: 2017-2021 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/conn.h>

#include <../host/hci_core.h>

#include "common/host.h"

static struct bt_le_ext_adv ext_adv_pool[CONFIG_BT_EXT_ADV_MAX_ADV_SET];

static struct bt_le_ext_adv *ext_adv_find(uint8_t adv_handle)
{
    struct bt_le_ext_adv *adv = NULL;

    for (size_t i = 0; i < ARRAY_SIZE(ext_adv_pool); i++) {
        if (atomic_test_bit(ext_adv_pool[i].flags,
                            BT_PER_ADV_PARAMS_SET) &&
                ext_adv_pool[i].handle == adv_handle) {
            LOG_DBG("ExtAdvFound[%u][%u]", i, adv_handle);
            adv = &ext_adv_pool[i];
            break;
        }
    }

    return adv;
}

static struct bt_le_ext_adv *ext_adv_new(void)
{
    struct bt_le_ext_adv *adv = NULL;

    for (size_t i = 0; i < ARRAY_SIZE(ext_adv_pool); i++) {
        if (atomic_test_bit(ext_adv_pool[i].flags,
                            BT_PER_ADV_PARAMS_SET) == false) {
            adv = &ext_adv_pool[i];

            memset(adv, 0, sizeof(*adv));
            atomic_set_bit(adv->flags, BT_PER_ADV_PARAMS_SET);
            break;
        }
    }

    return adv;
}

static void ext_adv_delete(struct bt_le_ext_adv *adv)
{
    atomic_clear_bit(adv->flags, BT_PER_ADV_PARAMS_SET);
}

_IDF_ONLY
struct bt_le_ext_adv *bt_le_ext_adv_find(uint8_t adv_handle)
{
    LOG_DBG("ExtAdvFind[%u]", adv_handle);
    return ext_adv_find(adv_handle);
}

_IDF_ONLY
int bt_le_ext_adv_new_safe(uint8_t adv_handle)
{
    struct bt_le_ext_adv *adv = NULL;
    int err = 0;

    bt_le_host_lock();

    adv = ext_adv_find(adv_handle);
    if (adv) {
        LOG_WRN("ExtAdvExist[%u]", adv_handle);
        err = -EEXIST;
        goto end;
    }

    adv = ext_adv_new();
    if (adv == NULL) {
        LOG_ERR("NoFreeExtAdv[%u]", adv_handle);
        err = -ENOMEM;
        goto end;
    }

    adv->handle = adv_handle;

end:
    bt_le_host_unlock();

    return err;
}

_IDF_ONLY
int bt_le_ext_adv_delete_safe(uint8_t adv_handle)
{
    struct bt_le_ext_adv *adv = NULL;
    int err = 0;

    bt_le_host_lock();

    adv = ext_adv_find(adv_handle);
    if (adv == NULL) {
        LOG_ERR("ExtAdvNotFound[%u]", adv_handle);
        err = -ENODEV;
        goto end;
    }

    ext_adv_delete(adv);

end:
    bt_le_host_unlock();

    return err;
}

_LIB_ONLY
int bt_le_ext_adv_get_info(const struct bt_le_ext_adv *adv,
                           struct bt_le_ext_adv_info *info)
{
    if (adv == NULL || info == NULL) {
        LOG_ERR("ParamNull[%p][%p]", adv, info);
        return -EINVAL;
    }

    if (!IS_ARRAY_ELEMENT(ext_adv_pool, adv)) {
        LOG_ERR("ExtAdvNotInPool[%p]", adv);
        return -EINVAL;
    }

    /* Force to use ENABLED state for LIB usage */
    info->ext_adv_state = BT_LE_EXT_ADV_STATE_ENABLED;
    info->per_adv_state = BT_LE_PER_ADV_STATE_ENABLED;

    return 0;
}
