/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLE_MESH_DEBUG_ACCESS)

#include "mesh.h"
#include "adv.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "mesh_main.h"
#include "mesh_common.h"
#include "provisioner_main.h"

#include "generic_client.h"
#include "sensor_client.h"
#include "time_scene_client.h"
#include "lighting_client.h"

#include "generic_server.h"
#include "sensor_server.h"
#include "time_scene_server.h"
#include "lighting_server.h"

#define BLE_MESH_SDU_MAX_LEN    384

static const struct bt_mesh_comp *dev_comp;
static u16_t dev_primary_addr;

static const struct {
    const u16_t id;
    int (*const init)(struct bt_mesh_model *model, bool primary);
} model_init[] = {
    { BLE_MESH_MODEL_ID_CFG_SRV,                   bt_mesh_cfg_srv_init                   },
    { BLE_MESH_MODEL_ID_HEALTH_SRV,                bt_mesh_health_srv_init                },
#if defined(CONFIG_BLE_MESH_CFG_CLI)
    { BLE_MESH_MODEL_ID_CFG_CLI,                   bt_mesh_cfg_cli_init                   },
#endif
#if defined(CONFIG_BLE_MESH_HEALTH_CLI)
    { BLE_MESH_MODEL_ID_HEALTH_CLI,                bt_mesh_health_cli_init                },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
    { BLE_MESH_MODEL_ID_GEN_ONOFF_CLI,             bt_mesh_gen_onoff_cli_init             },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_LEVEL_CLI)
    { BLE_MESH_MODEL_ID_GEN_LEVEL_CLI,             bt_mesh_gen_level_cli_init             },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_DEF_TRANS_TIME_CLI)
    { BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI,    bt_mesh_gen_def_trans_time_cli_init    },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_POWER_ONOFF_CLI)
    { BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI,       bt_mesh_gen_pwr_onoff_cli_init         },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_POWER_LEVEL_CLI)
    { BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI,       bt_mesh_gen_pwr_level_cli_init         },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_BATTERY_CLI)
    { BLE_MESH_MODEL_ID_GEN_BATTERY_CLI,           bt_mesh_gen_battery_cli_init           },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_LOCATION_CLI)
    { BLE_MESH_MODEL_ID_GEN_LOCATION_CLI,          bt_mesh_gen_location_cli_init          },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_PROPERTY_CLI)
    { BLE_MESH_MODEL_ID_GEN_PROP_CLI,              bt_mesh_gen_property_cli_init          },
#endif
#if defined(CONFIG_BLE_MESH_SENSOR_CLI)
    { BLE_MESH_MODEL_ID_SENSOR_CLI,                bt_mesh_sensor_cli_init                },
#endif
#if defined(CONFIG_BLE_MESH_TIME_CLI)
    { BLE_MESH_MODEL_ID_TIME_CLI,                  bt_mesh_time_cli_init                  },
#endif
#if defined(CONFIG_BLE_MESH_SCENE_CLI)
    { BLE_MESH_MODEL_ID_SCENE_CLI,                 bt_mesh_scene_cli_init                 },
#endif
#if defined(CONFIG_BLE_MESH_SCHEDULER_CLI)
    { BLE_MESH_MODEL_ID_SCHEDULER_CLI,             bt_mesh_scheduler_cli_init             },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_LIGHTNESS_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,       bt_mesh_light_lightness_cli_init       },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_CTL_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,             bt_mesh_light_ctl_cli_init             },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_HSL_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,             bt_mesh_light_hsl_cli_init             },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_XYL_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,             bt_mesh_light_xyl_cli_init             },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_LC_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_LC_CLI,              bt_mesh_light_lc_cli_init              },
#endif
    { BLE_MESH_MODEL_ID_GEN_ONOFF_SRV,             bt_mesh_gen_onoff_srv_init             },
    { BLE_MESH_MODEL_ID_GEN_LEVEL_SRV,             bt_mesh_gen_level_srv_init             },
    { BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV,    bt_mesh_gen_def_trans_time_srv_init    },
    { BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV,       bt_mesh_gen_power_onoff_srv_init       },
    { BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV, bt_mesh_gen_power_onoff_setup_srv_init },
    { BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV,       bt_mesh_gen_power_level_srv_init       },
    { BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV, bt_mesh_gen_power_level_setup_srv_init },
    { BLE_MESH_MODEL_ID_GEN_BATTERY_SRV,           bt_mesh_gen_battery_srv_init           },
    { BLE_MESH_MODEL_ID_GEN_LOCATION_SRV,          bt_mesh_gen_location_srv_init          },
    { BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV,    bt_mesh_gen_location_setup_srv_init    },
    { BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV,         bt_mesh_gen_user_prop_srv_init         },
    { BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV,        bt_mesh_gen_admin_prop_srv_init        },
    { BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV, bt_mesh_gen_manu_prop_srv_init         },
    { BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV,       bt_mesh_gen_client_prop_srv_init       },
    { BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV,       bt_mesh_light_lightness_srv_init       },
    { BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, bt_mesh_light_lightness_setup_srv_init },
    { BLE_MESH_MODEL_ID_LIGHT_CTL_SRV,             bt_mesh_light_ctl_srv_init             },
    { BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV,       bt_mesh_light_ctl_setup_srv_init       },
    { BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV,        bt_mesh_light_ctl_temp_srv_init        },
    { BLE_MESH_MODEL_ID_LIGHT_HSL_SRV,             bt_mesh_light_hsl_srv_init             },
    { BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV,         bt_mesh_light_hsl_hue_srv_init         },
    { BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV,         bt_mesh_light_hsl_sat_srv_init         },
    { BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV,       bt_mesh_light_hsl_setup_srv_init       },
    { BLE_MESH_MODEL_ID_LIGHT_XYL_SRV,             bt_mesh_light_xyl_srv_init             },
    { BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV,       bt_mesh_light_xyl_setup_srv_init       },
    { BLE_MESH_MODEL_ID_LIGHT_LC_SRV,              bt_mesh_light_lc_srv_init              },
    { BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV,        bt_mesh_light_lc_setup_srv_init        },
    { BLE_MESH_MODEL_ID_TIME_SRV,                  bt_mesh_time_srv_init                  },
    { BLE_MESH_MODEL_ID_TIME_SETUP_SRV,            bt_mesh_time_setup_srv_init            },
    { BLE_MESH_MODEL_ID_SCENE_SRV,                 bt_mesh_scene_srv_init                 },
    { BLE_MESH_MODEL_ID_SCENE_SETUP_SRV,           bt_mesh_scene_setup_srv_init           },
    { BLE_MESH_MODEL_ID_SCHEDULER_SRV,             bt_mesh_scheduler_srv_init             },
    { BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV,       bt_mesh_scheduler_setup_srv_init       },
    { BLE_MESH_MODEL_ID_SENSOR_SRV,                bt_mesh_sensor_srv_init                },
    { BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV,          bt_mesh_sensor_setup_srv_init          },
};

static const struct {
    const u16_t id;
    int (*const deinit)(struct bt_mesh_model *model, bool primary);
} model_deinit[] = {
    { BLE_MESH_MODEL_ID_CFG_SRV,                   bt_mesh_cfg_srv_deinit                   },
    { BLE_MESH_MODEL_ID_HEALTH_SRV,                bt_mesh_health_srv_deinit                },
#if defined(CONFIG_BLE_MESH_CFG_CLI)
    { BLE_MESH_MODEL_ID_CFG_CLI,                   bt_mesh_cfg_cli_deinit                   },
#endif
#if defined(CONFIG_BLE_MESH_HEALTH_CLI)
    { BLE_MESH_MODEL_ID_HEALTH_CLI,                bt_mesh_health_cli_deinit                },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
    { BLE_MESH_MODEL_ID_GEN_ONOFF_CLI,             bt_mesh_gen_onoff_cli_deinit             },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_LEVEL_CLI)
    { BLE_MESH_MODEL_ID_GEN_LEVEL_CLI,             bt_mesh_gen_level_cli_deinit             },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_DEF_TRANS_TIME_CLI)
    { BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI,    bt_mesh_gen_def_trans_time_cli_deinit    },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_POWER_ONOFF_CLI)
    { BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI,       bt_mesh_gen_pwr_onoff_cli_deinit         },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_POWER_LEVEL_CLI)
    { BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI,       bt_mesh_gen_pwr_level_cli_deinit         },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_BATTERY_CLI)
    { BLE_MESH_MODEL_ID_GEN_BATTERY_CLI,           bt_mesh_gen_battery_cli_deinit           },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_LOCATION_CLI)
    { BLE_MESH_MODEL_ID_GEN_LOCATION_CLI,          bt_mesh_gen_location_cli_deinit          },
#endif
#if defined(CONFIG_BLE_MESH_GENERIC_PROPERTY_CLI)
    { BLE_MESH_MODEL_ID_GEN_PROP_CLI,              bt_mesh_gen_property_cli_deinit          },
#endif
#if defined(CONFIG_BLE_MESH_SENSOR_CLI)
    { BLE_MESH_MODEL_ID_SENSOR_CLI,                bt_mesh_sensor_cli_deinit                },
#endif
#if defined(CONFIG_BLE_MESH_TIME_CLI)
    { BLE_MESH_MODEL_ID_TIME_CLI,                  bt_mesh_time_cli_deinit                  },
#endif
#if defined(CONFIG_BLE_MESH_SCENE_CLI)
    { BLE_MESH_MODEL_ID_SCENE_CLI,                 bt_mesh_scene_cli_deinit                 },
#endif
#if defined(CONFIG_BLE_MESH_SCHEDULER_CLI)
    { BLE_MESH_MODEL_ID_SCHEDULER_CLI,             bt_mesh_scheduler_cli_deinit             },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_LIGHTNESS_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,       bt_mesh_light_lightness_cli_deinit       },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_CTL_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,             bt_mesh_light_ctl_cli_deinit             },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_HSL_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,             bt_mesh_light_hsl_cli_deinit             },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_XYL_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,             bt_mesh_light_xyl_cli_deinit             },
#endif
#if defined(CONFIG_BLE_MESH_LIGHT_LC_CLI)
    { BLE_MESH_MODEL_ID_LIGHT_LC_CLI,              bt_mesh_light_lc_cli_deinit              },
#endif
    { BLE_MESH_MODEL_ID_GEN_ONOFF_SRV,             bt_mesh_gen_onoff_srv_deinit             },
    { BLE_MESH_MODEL_ID_GEN_LEVEL_SRV,             bt_mesh_gen_level_srv_deinit             },
    { BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV,    bt_mesh_gen_def_trans_time_srv_deinit    },
    { BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV,       bt_mesh_gen_power_onoff_srv_deinit       },
    { BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV, bt_mesh_gen_power_onoff_setup_srv_deinit },
    { BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV,       bt_mesh_gen_power_level_srv_deinit       },
    { BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV, bt_mesh_gen_power_level_setup_srv_deinit },
    { BLE_MESH_MODEL_ID_GEN_BATTERY_SRV,           bt_mesh_gen_battery_srv_deinit           },
    { BLE_MESH_MODEL_ID_GEN_LOCATION_SRV,          bt_mesh_gen_location_srv_deinit          },
    { BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV,    bt_mesh_gen_location_setup_srv_deinit    },
    { BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV,         bt_mesh_gen_user_prop_srv_deinit         },
    { BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV,        bt_mesh_gen_admin_prop_srv_deinit        },
    { BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV, bt_mesh_gen_manu_prop_srv_deinit         },
    { BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV,       bt_mesh_gen_client_prop_srv_deinit       },
    { BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV,       bt_mesh_light_lightness_srv_deinit       },
    { BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, bt_mesh_light_lightness_setup_srv_deinit },
    { BLE_MESH_MODEL_ID_LIGHT_CTL_SRV,             bt_mesh_light_ctl_srv_deinit             },
    { BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV,       bt_mesh_light_ctl_setup_srv_deinit       },
    { BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV,        bt_mesh_light_ctl_temp_srv_deinit        },
    { BLE_MESH_MODEL_ID_LIGHT_HSL_SRV,             bt_mesh_light_hsl_srv_deinit             },
    { BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV,         bt_mesh_light_hsl_hue_srv_deinit         },
    { BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV,         bt_mesh_light_hsl_sat_srv_deinit         },
    { BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV,       bt_mesh_light_hsl_setup_srv_deinit       },
    { BLE_MESH_MODEL_ID_LIGHT_XYL_SRV,             bt_mesh_light_xyl_srv_deinit             },
    { BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV,       bt_mesh_light_xyl_setup_srv_deinit       },
    { BLE_MESH_MODEL_ID_LIGHT_LC_SRV,              bt_mesh_light_lc_srv_deinit              },
    { BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV,        bt_mesh_light_lc_setup_srv_deinit        },
    { BLE_MESH_MODEL_ID_TIME_SRV,                  bt_mesh_time_srv_deinit                  },
    { BLE_MESH_MODEL_ID_TIME_SETUP_SRV,            bt_mesh_time_setup_srv_deinit            },
    { BLE_MESH_MODEL_ID_SCENE_SRV,                 bt_mesh_scene_srv_deinit                 },
    { BLE_MESH_MODEL_ID_SCENE_SETUP_SRV,           bt_mesh_scene_setup_srv_deinit           },
    { BLE_MESH_MODEL_ID_SCHEDULER_SRV,             bt_mesh_scheduler_srv_deinit             },
    { BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV,       bt_mesh_scheduler_setup_srv_deinit       },
    { BLE_MESH_MODEL_ID_SENSOR_SRV,                bt_mesh_sensor_srv_deinit                },
    { BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV,          bt_mesh_sensor_setup_srv_deinit          },
};

void bt_mesh_model_foreach(void (*func)(struct bt_mesh_model *mod,
                                        struct bt_mesh_elem *elem,
                                        bool vnd, bool primary,
                                        void *user_data),
                           void *user_data)
{
    int i, j;

    for (i = 0; i < dev_comp->elem_count; i++) {
        struct bt_mesh_elem *elem = &dev_comp->elem[i];

        for (j = 0; j < elem->model_count; j++) {
            struct bt_mesh_model *model = &elem->models[j];

            func(model, elem, false, i == 0, user_data);
        }

        for (j = 0; j < elem->vnd_model_count; j++) {
            struct bt_mesh_model *model = &elem->vnd_models[j];

            func(model, elem, true, i == 0, user_data);
        }
    }
}

s32_t bt_mesh_model_pub_period_get(struct bt_mesh_model *mod)
{
    int period = 0;

    if (!mod->pub) {
        BT_ERR("%s, Model has no publication support", __func__);
        return 0;
    }

    switch (mod->pub->period >> 6) {
    case 0x00:
        /* 1 step is 100 ms */
        period = K_MSEC((mod->pub->period & BIT_MASK(6)) * 100U);
        break;
    case 0x01:
        /* 1 step is 1 second */
        period = K_SECONDS(mod->pub->period & BIT_MASK(6));
        break;
    case 0x02:
        /* 1 step is 10 seconds */
        period = K_SECONDS((mod->pub->period & BIT_MASK(6)) * 10U);
        break;
    case 0x03:
        /* 1 step is 10 minutes */
        period = K_MINUTES((mod->pub->period & BIT_MASK(6)) * 10U);
        break;
    default:
        BT_ERR("%s, Unknown model publication period", __func__);
        return 0;
    }

    if (mod->pub->fast_period) {
        return period >> mod->pub->period_div;
    } else {
        return period;
    }
}

static s32_t next_period(struct bt_mesh_model *mod)
{
    struct bt_mesh_model_pub *pub = mod->pub;
    u32_t elapsed = 0U, period = 0U;

    if (!pub) {
        BT_ERR("%s, Model has no publication support", __func__);
        return -ENOTSUP;
    }

    period = bt_mesh_model_pub_period_get(mod);
    if (!period) {
        return 0;
    }

    elapsed = k_uptime_get_32() - pub->period_start;

    BT_INFO("Publishing took %ums", elapsed);

    if (elapsed >= period) {
        BT_WARN("Publication sending took longer than the period");
        /* Return smallest positive number since 0 means disabled */
        return K_MSEC(1);
    }

    return period - elapsed;
}

static void publish_sent(int err, void *user_data)
{
    struct bt_mesh_model *mod = user_data;
    s32_t delay = 0;

    BT_DBG("err %d", err);

    if (!mod->pub) {
        BT_ERR("%s, Model has no publication support", __func__);
        return;
    }

    if (mod->pub->count) {
        delay = BLE_MESH_PUB_TRANSMIT_INT(mod->pub->retransmit);
    } else {
        delay = next_period(mod);
    }

    if (delay) {
        BT_INFO("Publishing next time in %dms", delay);
        k_delayed_work_submit(&mod->pub->timer, delay);
    }
}

static void publish_start(u16_t duration, int err, void *user_data)
{
    struct bt_mesh_model *mod = user_data;
    struct bt_mesh_model_pub *pub = mod->pub;

    if (err) {
        BT_ERR("Failed to publish: err %d", err);
        return;
    }

    /* Initialize the timestamp for the beginning of a new period */
    if (pub->count == BLE_MESH_PUB_TRANSMIT_COUNT(pub->retransmit)) {
        pub->period_start = k_uptime_get_32();
    }
}

static const struct bt_mesh_send_cb pub_sent_cb = {
    .start = publish_start,
    .end = publish_sent,
};

static int publish_retransmit(struct bt_mesh_model *mod)
{
    struct bt_mesh_model_pub *pub = mod->pub;
    if (!pub) {
        BT_ERR("%s, Model has no publication support", __func__);
        return -ENOTSUP;
    }

    struct bt_mesh_app_key *key = NULL;
    struct net_buf_simple *sdu = NULL;
    struct bt_mesh_msg_ctx ctx = {
        .addr = pub->addr,
        .send_ttl = pub->ttl,
        .model = mod,
        .srv_send = (pub->dev_role == NODE ? true : false),
    };
    struct bt_mesh_net_tx tx = {
        .ctx = &ctx,
        .src = bt_mesh_model_elem(mod)->addr,
        .xmit = bt_mesh_net_transmit_get(),
        .friend_cred = pub->cred,
    };
    int err = 0;

    key = bt_mesh_tx_appkey_get(pub->dev_role, pub->key);
    if (!key) {
        BT_ERR("%s, Failed to find AppKey", __func__);
        return -EADDRNOTAVAIL;
    }

    tx.sub = bt_mesh_tx_netkey_get(pub->dev_role, key->net_idx);
    if (!tx.sub) {
        BT_ERR("%s, Failed to get subnet", __func__);
        return -EADDRNOTAVAIL;
    }

    ctx.net_idx = key->net_idx;
    ctx.app_idx = key->app_idx;

    sdu = bt_mesh_alloc_buf(pub->msg->len + 4);
    if (!sdu) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    net_buf_simple_add_mem(sdu, pub->msg->data, pub->msg->len);

    pub->count--;

    err = bt_mesh_trans_send(&tx, sdu, &pub_sent_cb, mod);

    bt_mesh_free_buf(sdu);
    return err;
}

static void mod_publish(struct k_work *work)
{
    struct bt_mesh_model_pub *pub = CONTAINER_OF(work,
                                    struct bt_mesh_model_pub,
                                    timer.work);
    s32_t period_ms = 0;
    int err = 0;

    BT_DBG("%s", __func__);

    period_ms = bt_mesh_model_pub_period_get(pub->mod);
    BT_INFO("period %u ms", period_ms);

    if (pub->count) {
        err = publish_retransmit(pub->mod);
        if (err) {
            BT_ERR("%s, Failed to retransmit (err %d)", __func__, err);

            pub->count = 0U;

            /* Continue with normal publication */
            if (period_ms) {
                k_delayed_work_submit(&pub->timer, period_ms);
            }
        }

        return;
    }

    if (!period_ms) {
        return;
    }

    __ASSERT_NO_MSG(pub->update != NULL);

    /* Callback the model publish update event to the application layer.
     * In the event, users can update the context of the publish message
     * which will be published in the next period.
     */
    err = pub->update(pub->mod);
    if (err) {
        BT_ERR("%s, Failed to update publication message", __func__);
        return;
    }

    err = bt_mesh_model_publish(pub->mod);
    if (err) {
        BT_ERR("%s, Publishing failed (err %d)", __func__, err);
    }
}

struct bt_mesh_elem *bt_mesh_model_elem(struct bt_mesh_model *mod)
{
    return &dev_comp->elem[mod->elem_idx];
}

struct bt_mesh_model *bt_mesh_model_get(bool vnd, u8_t elem_idx, u8_t mod_idx)
{
    struct bt_mesh_elem *elem = NULL;

    if (!dev_comp) {
        BT_ERR("%s, dev_comp is not initialized", __func__);
        return NULL;
    }

    if (elem_idx >= dev_comp->elem_count) {
        BT_ERR("%s, Invalid element index %u", __func__, elem_idx);
        return NULL;
    }

    elem = &dev_comp->elem[elem_idx];

    if (vnd) {
        if (mod_idx >= elem->vnd_model_count) {
            BT_ERR("%s, Invalid vendor model index %u", __func__, mod_idx);
            return NULL;
        }

        return &elem->vnd_models[mod_idx];
    } else {
        if (mod_idx >= elem->model_count) {
            BT_ERR("%s, Invalid SIG model index %u", __func__, mod_idx);
            return NULL;
        }

        return &elem->models[mod_idx];
    }
}

static void mod_init(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                     bool vnd, bool primary, void *user_data)
{
    int i;

    mod->elem = elem;

    if (mod->pub) {
        mod->pub->mod = mod;
        k_delayed_work_init(&mod->pub->timer, mod_publish);
    }

    for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
        mod->keys[i] = BLE_MESH_KEY_UNUSED;
    }

    mod->flags = 0;
    mod->elem_idx = elem - dev_comp->elem;
    if (vnd) {
        mod->model_idx = mod - elem->vnd_models;
    } else {
        mod->model_idx = mod - elem->models;
    }

    if (vnd) {
        return;
    }

    for (i = 0; i < ARRAY_SIZE(model_init); i++) {
        if (model_init[i].id == mod->id) {
            model_init[i].init(mod, primary);
        }
    }
}

static void mod_deinit(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                       bool vnd, bool primary, void *user_data)
{
    int i;

    mod->elem = NULL;

    if (mod->pub) {
        mod->pub->mod = NULL;
        k_delayed_work_free(&mod->pub->timer);
    }

    for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
        mod->keys[i] = BLE_MESH_KEY_UNUSED;
    }

    mod->flags = 0U;
    mod->elem_idx = 0U;
    mod->model_idx = 0U;

    if (vnd) {
        return;
    }

    for (i = 0; i < ARRAY_SIZE(model_deinit); i++) {
        if (model_deinit[i].id == mod->id) {
            model_deinit[i].deinit(mod, primary);
        }
    }
}

int bt_mesh_comp_register(const struct bt_mesh_comp *comp)
{
    /* There must be at least one element */
    if (!comp->elem_count) {
        return -EINVAL;
    }

    dev_comp = comp;

    bt_mesh_model_foreach(mod_init, NULL);

    return 0;
}

int bt_mesh_comp_deregister(void)
{
    if (dev_comp == NULL) {
        return -EINVAL;
    }

    bt_mesh_model_foreach(mod_deinit, NULL);

    dev_primary_addr = BLE_MESH_ADDR_UNASSIGNED;
    dev_comp = NULL;

    return 0;
}

void bt_mesh_comp_provision(u16_t addr)
{
    int i;

    dev_primary_addr = addr;

    BT_INFO("addr 0x%04x elem_count %u", addr, dev_comp->elem_count);

    for (i = 0; i < dev_comp->elem_count; i++) {
        struct bt_mesh_elem *elem = &dev_comp->elem[i];

        elem->addr = addr++;

        BT_DBG("addr 0x%04x mod_count %u vnd_mod_count %u",
               elem->addr, elem->model_count, elem->vnd_model_count);
    }
}

void bt_mesh_comp_unprovision(void)
{
    BT_DBG("%s", __func__);

    dev_primary_addr = BLE_MESH_ADDR_UNASSIGNED;

    bt_mesh_model_foreach(mod_init, NULL);
}

u16_t bt_mesh_primary_addr(void)
{
    return dev_primary_addr;
}

u16_t *bt_mesh_model_find_group(struct bt_mesh_model *mod, u16_t addr)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] == addr) {
            return &mod->groups[i];
        }
    }

    return NULL;
}

static struct bt_mesh_model *bt_mesh_elem_find_group(struct bt_mesh_elem *elem,
        u16_t group_addr)
{
    struct bt_mesh_model *model = NULL;
    u16_t *match = NULL;
    int i;

    for (i = 0; i < elem->model_count; i++) {
        model = &elem->models[i];

        match = bt_mesh_model_find_group(model, group_addr);
        if (match) {
            return model;
        }
    }

    for (i = 0; i < elem->vnd_model_count; i++) {
        model = &elem->vnd_models[i];

        match = bt_mesh_model_find_group(model, group_addr);
        if (match) {
            return model;
        }
    }

    return NULL;
}

struct bt_mesh_elem *bt_mesh_elem_find(u16_t addr)
{
    u16_t index = 0U;

    if (BLE_MESH_ADDR_IS_UNICAST(addr)) {
        index = (addr - dev_comp->elem[0].addr);
        if (index < dev_comp->elem_count) {
            return &dev_comp->elem[index];
        } else {
            return NULL;
        }
    }

    for (index = 0; index < dev_comp->elem_count; index++) {
        struct bt_mesh_elem *elem = &dev_comp->elem[index];

        if (bt_mesh_elem_find_group(elem, addr)) {
            return elem;
        }
    }

    return NULL;
}

u8_t bt_mesh_elem_count(void)
{
    return dev_comp->elem_count;
}

static bool model_has_key(struct bt_mesh_model *mod, u16_t key)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
        if (mod->keys[i] == key) {
            return true;
        }
    }

    return false;
}

static const struct bt_mesh_model_op *find_op(struct bt_mesh_model *models,
        u8_t model_count, u16_t dst,
        u16_t app_idx, u32_t opcode,
        struct bt_mesh_model **model)
{
    int i;

    for (i = 0; i < model_count; i++) {
        const struct bt_mesh_model_op *op;

        *model = &models[i];

        if (BLE_MESH_ADDR_IS_GROUP(dst) ||
                BLE_MESH_ADDR_IS_VIRTUAL(dst)) {
            if (!bt_mesh_model_find_group(*model, dst)) {
                continue;
            }
        }

        if (!model_has_key(*model, app_idx)) {
            continue;
        }

        for (op = (*model)->op; op->func; op++) {
            if (op->opcode == opcode) {
                return op;
            }
        }
    }

    *model = NULL;
    return NULL;
}

static int get_opcode(struct net_buf_simple *buf, u32_t *opcode)
{
    switch (buf->data[0] >> 6) {
    case 0x00:
    case 0x01:
        if (buf->data[0] == 0x7f) {
            BT_ERR("%s, Ignoring RFU OpCode", __func__);
            return -EINVAL;
        }

        *opcode = net_buf_simple_pull_u8(buf);
        return 0;
    case 0x02:
        if (buf->len < 2) {
            BT_ERR("%s, Too short payload for 2-octet OpCode", __func__);
            return -EINVAL;
        }

        *opcode = net_buf_simple_pull_be16(buf);
        return 0;
    case 0x03:
        if (buf->len < 3) {
            BT_ERR("%s, Too short payload for 3-octet OpCode", __func__);
            return -EINVAL;
        }

        *opcode = net_buf_simple_pull_u8(buf) << 16;
        *opcode |= net_buf_simple_pull_le16(buf);
        return 0;
    }

    return -EINVAL;
}

bool bt_mesh_fixed_group_match(u16_t addr)
{
    /* Check for fixed group addresses */
    switch (addr) {
    case BLE_MESH_ADDR_ALL_NODES:
        return true;
    case BLE_MESH_ADDR_PROXIES:
        return (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED);
    case BLE_MESH_ADDR_FRIENDS:
        return (bt_mesh_friend_get() == BLE_MESH_FRIEND_ENABLED);
    case BLE_MESH_ADDR_RELAYS:
        return (bt_mesh_relay_get() == BLE_MESH_RELAY_ENABLED);
    default:
        return false;
    }
}

void bt_mesh_model_recv(struct bt_mesh_net_rx *rx, struct net_buf_simple *buf)
{
    struct bt_mesh_model *models = NULL, *model = NULL;
    const struct bt_mesh_model_op *op = NULL;
    u32_t opcode = 0U;
    u8_t count = 0U;
    int i;

    BT_INFO("app_idx 0x%04x src 0x%04x dst 0x%04x", rx->ctx.app_idx,
           rx->ctx.addr, rx->ctx.recv_dst);
    BT_INFO("len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    if (get_opcode(buf, &opcode) < 0) {
        BT_WARN("%s, Unable to decode OpCode", __func__);
        return;
    }

    BT_INFO("OpCode 0x%08x", opcode);

    for (i = 0; i < dev_comp->elem_count; i++) {
        struct bt_mesh_elem *elem = &dev_comp->elem[i];

        if (BLE_MESH_ADDR_IS_UNICAST(rx->ctx.recv_dst)) {
            if (elem->addr != rx->ctx.recv_dst) {
                continue;
            }
        } else if (BLE_MESH_ADDR_IS_GROUP(rx->ctx.recv_dst) ||
                   BLE_MESH_ADDR_IS_VIRTUAL(rx->ctx.recv_dst)) {
            /* find_op() will do proper model/group matching */
        } else if (i != 0 ||
                   !bt_mesh_fixed_group_match(rx->ctx.recv_dst)) {
            continue;
        }

        /* SIG models cannot contain 3-byte (vendor) OpCodes, and
         * vendor models cannot contain SIG (1- or 2-byte) OpCodes, so
         * we only need to do the lookup in one of the model lists.
         */
        if (opcode < 0x10000) {
            models = elem->models;
            count = elem->model_count;
        } else {
            models = elem->vnd_models;
            count = elem->vnd_model_count;
        }

        op = find_op(models, count, rx->ctx.recv_dst, rx->ctx.app_idx,
                     opcode, &model);
        if (op) {
            struct net_buf_simple_state state;

            if (buf->len < op->min_len) {
                BT_ERR("%s, Too short message for OpCode 0x%08x",
                       __func__, opcode);
                continue;
            }

            /* The callback will likely parse the buffer, so
             * store the parsing state in case multiple models
             * receive the message.
             */
            net_buf_simple_save(buf, &state);

            /** Changed by Espressif, here we update recv_op with the
             *  value opcode got from the buf.
             */
            rx->ctx.recv_op = opcode;
            /** Changed by Espressif, we update the model pointer to the
             *  found model when we received a message.
             */
            rx->ctx.model = model;
            /** Changed by Espressif, we update the srv_send flag to be
             *  true when we received a message. This flag will be used
             *  when a server model sends a status message and will
             *  have no impact on the client sent messages.
             */
            rx->ctx.srv_send = true;

            op->func(model, &rx->ctx, buf);
            net_buf_simple_restore(buf, &state);

        } else {
            BT_DBG("No OpCode 0x%08x for elem %d", opcode, i);
        }
    }
}

void bt_mesh_model_msg_init(struct net_buf_simple *msg, u32_t opcode)
{
    net_buf_simple_init(msg, 0);

    switch (BLE_MESH_MODEL_OP_LEN(opcode)) {
    case 1:
        net_buf_simple_add_u8(msg, opcode);
        break;
    case 2:
        net_buf_simple_add_be16(msg, opcode);
        break;
    case 3:
        net_buf_simple_add_u8(msg, ((opcode >> 16) & 0xff));
        net_buf_simple_add_le16(msg, opcode & 0xffff);
        break;
    default:
        BT_WARN("Unknown opcode format");
        break;
    }
}

static bool ready_to_send(u8_t role, u16_t dst)
{
    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned() && role == NODE) {
        return true;
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en() && role == PROVISIONER) {
        if (!bt_mesh_provisioner_check_msg_dst(dst)) {
            BT_ERR("%s, Failed to find DST 0x%04x", __func__, dst);
            return false;
        }
        return true;
    } else if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && bt_mesh_is_provisioned() && role == FAST_PROV) {
        return true;
    }

    return false;
}

static int model_send(struct bt_mesh_model *model,
                      struct bt_mesh_net_tx *tx, bool implicit_bind,
                      struct net_buf_simple *msg,
                      const struct bt_mesh_send_cb *cb, void *cb_data)
{
    u8_t role = 0U;

    role = bt_mesh_get_device_role(model, tx->ctx->srv_send);
    if (role == ROLE_NVAL) {
        BT_ERR("%s, Failed to get model role", __func__);
        return -EINVAL;
    }

    BT_INFO("net_idx 0x%04x app_idx 0x%04x dst 0x%04x", tx->ctx->net_idx,
           tx->ctx->app_idx, tx->ctx->addr);
    BT_INFO("len %u: %s", msg->len, bt_hex(msg->data, msg->len));

    if (!ready_to_send(role, tx->ctx->addr)) {
        BT_ERR("%s, fail", __func__);
        return -EINVAL;
    }

    if (net_buf_simple_tailroom(msg) < 4) {
        BT_ERR("%s, Not enough tailroom for TransMIC", __func__);
        return -EINVAL;
    }

    if (msg->len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SDU_MAX_LEN) - 4) {
        BT_ERR("%s, Too big message", __func__);
        return -EMSGSIZE;
    }

    if (!implicit_bind && !model_has_key(model, tx->ctx->app_idx)) {
        BT_ERR("%s, Model not bound to AppKey 0x%04x", __func__, tx->ctx->app_idx);
        return -EINVAL;
    }

    return bt_mesh_trans_send(tx, msg, cb, cb_data);
}

int bt_mesh_model_send(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *msg,
                       const struct bt_mesh_send_cb *cb, void *cb_data)
{
    struct bt_mesh_subnet *sub = NULL;
    u8_t role = 0U;

    role = bt_mesh_get_device_role(model, ctx->srv_send);
    if (role == ROLE_NVAL) {
        BT_ERR("%s, Failed to get model role", __func__);
        return -EINVAL;
    }

    sub = bt_mesh_tx_netkey_get(role, ctx->net_idx);
    if (!sub) {
        BT_ERR("%s, Failed to get subnet", __func__);
        return -EINVAL;
    }

    ctx->model = model;

    struct bt_mesh_net_tx tx = {
        .sub = sub,
        .ctx = ctx,
        .src = bt_mesh_model_elem(model)->addr,
        .xmit = bt_mesh_net_transmit_get(),
        .friend_cred = 0,
    };

    return model_send(model, &tx, false, msg, cb, cb_data);
}

int bt_mesh_model_publish(struct bt_mesh_model *model)
{
    struct bt_mesh_model_pub *pub = model->pub;
    struct bt_mesh_app_key *key = NULL;
    struct net_buf_simple *sdu = NULL;
    struct bt_mesh_msg_ctx ctx = {0};
    struct bt_mesh_net_tx tx = {
        .sub = NULL,
        .ctx = &ctx,
        .src = bt_mesh_model_elem(model)->addr,
        .xmit = bt_mesh_net_transmit_get(),
    };
    int err = 0;

    BT_DBG("%s", __func__);

    if (!pub || !pub->msg) {
        BT_ERR("%s, Model has no publication support", __func__);
        return -ENOTSUP;
    }

    if (pub->addr == BLE_MESH_ADDR_UNASSIGNED) {
        BT_WARN("%s, Unassigned model publish address", __func__);
        return -EADDRNOTAVAIL;
    }

    key = bt_mesh_tx_appkey_get(pub->dev_role, pub->key);
    if (!key) {
        BT_ERR("%s, Failed to get AppKey", __func__);
        return -EADDRNOTAVAIL;
    }

    if (pub->msg->len + 4 > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SDU_MAX_LEN)) {
        BT_ERR("%s, Message does not fit maximum SDU size", __func__);
        return -EMSGSIZE;
    }

    if (pub->count) {
        BT_WARN("%s, Clearing publish retransmit timer", __func__);
        k_delayed_work_cancel(&pub->timer);
    }

    ctx.addr = pub->addr;
    ctx.send_ttl = pub->ttl;
    ctx.net_idx = key->net_idx;
    ctx.app_idx = key->app_idx;
    ctx.srv_send = pub->dev_role == NODE ? true : false;

    tx.friend_cred = pub->cred;

    tx.sub = bt_mesh_tx_netkey_get(pub->dev_role, ctx.net_idx);
    if (!tx.sub) {
        BT_ERR("%s, Failed to get subnet", __func__);
        return -EADDRNOTAVAIL;
    }

    pub->count = BLE_MESH_PUB_TRANSMIT_COUNT(pub->retransmit);

    BT_INFO("Publish Retransmit Count %u Interval %ums", pub->count,
           BLE_MESH_PUB_TRANSMIT_INT(pub->retransmit));

    sdu = bt_mesh_alloc_buf(pub->msg->len + 4);
    if (!sdu) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    net_buf_simple_add_mem(sdu, pub->msg->data, pub->msg->len);

    err = model_send(model, &tx, true, sdu, &pub_sent_cb, model);
    if (err) {
        /* Don't try retransmissions for this publish attempt */
        pub->count = 0U;
        /* Make sure the publish timer gets reset */
        publish_sent(err, model);
    }

    bt_mesh_free_buf(sdu);
    return err;
}

struct bt_mesh_model *bt_mesh_model_find_vnd(struct bt_mesh_elem *elem,
        u16_t company, u16_t id)
{
    int i;

    for (i = 0; i < elem->vnd_model_count; i++) {
        if (elem->vnd_models[i].vnd.company == company &&
                elem->vnd_models[i].vnd.id == id) {
            return &elem->vnd_models[i];
        }
    }

    return NULL;
}

struct bt_mesh_model *bt_mesh_model_find(struct bt_mesh_elem *elem,
        u16_t id)
{
    int i;

    for (i = 0; i < elem->model_count; i++) {
        if (elem->models[i].id == id) {
            return &elem->models[i];
        }
    }

    return NULL;
}

const struct bt_mesh_comp *bt_mesh_comp_get(void)
{
    return dev_comp;
}

/* APIs used by messages encryption in upper transport layer & network layer */
struct bt_mesh_subnet *bt_mesh_tx_netkey_get(u8_t role, u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned() && role == NODE) {
        sub = bt_mesh_subnet_get(net_idx);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en() && role == PROVISIONER) {
        sub = bt_mesh_provisioner_subnet_get(net_idx);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && bt_mesh_is_provisioned() && role == FAST_PROV) {
        sub = bt_mesh_fast_prov_subnet_get(net_idx);
    }

    return sub;
}

const u8_t *bt_mesh_tx_devkey_get(u8_t role, u16_t dst)
{
    const u8_t *key = NULL;

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned() && role == NODE) {
        key = bt_mesh.dev_key;
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en() && role == PROVISIONER) {
        key = bt_mesh_provisioner_dev_key_get(dst);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && bt_mesh_is_provisioned() && role == FAST_PROV) {
        key = bt_mesh_fast_prov_dev_key_get(dst);
    }

    return key;
}

struct bt_mesh_app_key *bt_mesh_tx_appkey_get(u8_t role, u16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned() && role == NODE) {
        key = bt_mesh_app_key_find(app_idx);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en() && role == PROVISIONER) {
        key = bt_mesh_provisioner_app_key_find(app_idx);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && bt_mesh_is_provisioned() && role == FAST_PROV) {
        key = bt_mesh_fast_prov_app_key_find(app_idx);
    }

    return key;
}

/* APIs used by messages decryption in network layer & upper transport layer */
size_t bt_mesh_rx_netkey_size(void)
{
    size_t size = 0U;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        size = ARRAY_SIZE(bt_mesh.sub);
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        size = ARRAY_SIZE(bt_mesh.p_sub);
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    size = ARRAY_SIZE(bt_mesh.sub);
    if (bt_mesh_is_provisioner_en()) {
        size += ARRAY_SIZE(bt_mesh.p_sub);
    }
#endif

    return size;
}

struct bt_mesh_subnet *bt_mesh_rx_netkey_get(size_t index)
{
    struct bt_mesh_subnet *sub = NULL;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        sub = &bt_mesh.sub[index];
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        sub = bt_mesh.p_sub[index];
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (index < ARRAY_SIZE(bt_mesh.sub)) {
        sub = &bt_mesh.sub[index];
    } else {
        sub = bt_mesh.p_sub[index - ARRAY_SIZE(bt_mesh.sub)];
    }
#endif

    return sub;
}

size_t bt_mesh_rx_devkey_size(void)
{
    size_t size = 0U;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        size = 1;
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        size = 1;
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    size = 1;
    if (bt_mesh_is_provisioner_en()) {
        size += 1;
    }
#endif

    return size;
}

const u8_t *bt_mesh_rx_devkey_get(size_t index, u16_t src)
{
    const u8_t *key = NULL;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        key = bt_mesh.dev_key;
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        key = bt_mesh_provisioner_dev_key_get(src);
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (index < 1) {
        key = bt_mesh.dev_key;
    } else {
        key = bt_mesh_provisioner_dev_key_get(src);
    }
#endif

    return key;
}

size_t bt_mesh_rx_appkey_size(void)
{
    size_t size = 0U;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        size = ARRAY_SIZE(bt_mesh.app_keys);
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        size = ARRAY_SIZE(bt_mesh.p_app_keys);
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    size = ARRAY_SIZE(bt_mesh.app_keys);
    if (bt_mesh_is_provisioner_en()) {
        size += ARRAY_SIZE(bt_mesh.p_app_keys);
    }
#endif

    return size;
}

struct bt_mesh_app_key *bt_mesh_rx_appkey_get(size_t index)
{
    struct bt_mesh_app_key *key = NULL;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        key = &bt_mesh.app_keys[index];
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        key = bt_mesh.p_app_keys[index];
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (index < ARRAY_SIZE(bt_mesh.app_keys)) {
        key = &bt_mesh.app_keys[index];
    } else {
        key = bt_mesh.p_app_keys[index - ARRAY_SIZE(bt_mesh.app_keys)];
    }
#endif

    return key;
}
