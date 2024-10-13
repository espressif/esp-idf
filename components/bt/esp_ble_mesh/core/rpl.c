/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "mesh/config.h"
#include "mesh/trace.h"
#include "mesh.h"
#include "settings.h"

void bt_mesh_update_rpl(struct bt_mesh_rpl *rpl, struct bt_mesh_net_rx *rx)
{
    rpl->src = rx->ctx.addr;
    rpl->seq = rx->seq;
    rpl->old_iv = rx->old_iv;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_rpl(rpl);
    }
}

/* Check the Replay Protection List for a replay attempt. If non-NULL match
 * parameter is given the RPL slot is returned but it is not immediately
 * updated (needed for segmented messages), whereas if a NULL match is given
 * the RPL is immediately updated (used for unsegmented messages).
 */
static bool rpl_check_and_store(struct bt_mesh_net_rx *rx, struct bt_mesh_rpl **match)
{
    for (size_t i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];

        /* Empty slot */
        if (rpl->src == BLE_MESH_ADDR_UNASSIGNED) {
            if (match) {
                *match = rpl;
            } else {
                bt_mesh_update_rpl(rpl, rx);
            }

            return false;
        }

        /* Existing slot for given address */
        if (rpl->src == rx->ctx.addr) {
            if (rx->old_iv && !rpl->old_iv) {
                return true;
            }

            if ((!rx->old_iv && rpl->old_iv) ||
                rpl->seq < rx->seq) {
                if (match) {
                    *match = rpl;
                } else {
                    bt_mesh_update_rpl(rpl, rx);
                }

                return false;
            }

#if CONFIG_BLE_MESH_NOT_RELAY_REPLAY_MSG
            rx->replay_msg = 1;
#endif

            return true;
        }
    }

    BT_ERR("RPL is full!");
    return true;
}

bool bt_mesh_rpl_check(struct bt_mesh_net_rx *rx, struct bt_mesh_rpl **match)
{
    /* Don't bother checking messages from ourselves */
    if (rx->net_if == BLE_MESH_NET_IF_LOCAL) {
        return false;
    }

    /* The RPL is used only for the local node */
    if (!rx->local_match) {
        return false;
    }

    return rpl_check_and_store(rx, match);
}

void bt_mesh_rpl_update(void)
{
    /* Discard "old old" IV Index entries from RPL and flag
     * any other ones (which are valid) as old.
     */
    for (size_t i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];

        if (rpl->src) {
            if (rpl->old_iv) {
                (void)memset(rpl, 0, sizeof(*rpl));
            } else {
                rpl->old_iv = true;
            }

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_rpl(rpl);
            }
        }
    }
}

void bt_mesh_rpl_reset_single(uint16_t src, bool erase)
{
    if (!BLE_MESH_ADDR_IS_UNICAST(src)) {
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        if (src == bt_mesh.rpl[i].src) {
            memset(&bt_mesh.rpl[i], 0, sizeof(struct bt_mesh_rpl));

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && erase) {
                bt_mesh_clear_rpl_single(src);
            }
        }
    }
}

void bt_mesh_rpl_reset(bool erase)
{
    (void)memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && erase) {
        bt_mesh_clear_rpl();
    }
}
