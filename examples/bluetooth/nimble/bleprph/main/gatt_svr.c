/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "bleprph.h"
#include "services/ans/ble_svc_ans.h"

/*** Maximum number of characteristics with the notify flag ***/
#define MAX_NOTIFY 5

static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12,
                     0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59);

/* A characteristic that can be subscribed to */
static uint8_t gatt_svr_chr_val;
static uint16_t gatt_svr_chr_val_handle;
static const ble_uuid128_t gatt_svr_chr_uuid =
    BLE_UUID128_INIT(0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11,
                     0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33);

/* A custom descriptor */
static uint8_t gatt_svr_dsc_val;
static const ble_uuid128_t gatt_svr_dsc_uuid =
    BLE_UUID128_INIT(0x01, 0x01, 0x01, 0x01, 0x12, 0x12, 0x12, 0x12,
                     0x23, 0x23, 0x23, 0x23, 0x34, 0x34, 0x34, 0x34);

static void* subscription_list_buffer;
static struct os_mempool subscription_list_mempool;
struct subscription {
    SLIST_ENTRY(subscription) next;
    uint16_t val_handle;
};
SLIST_HEAD(subscription_list, subscription);
/*** SLIST that contains the handles of all the characteristics that are subscribed ***/
static struct subscription_list slist;

static int
gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                struct ble_gatt_access_ctxt *ctxt,
                void *arg);

static bool
gatt_svr_is_subscribed(uint16_t val_handle);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /*** Service ***/
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                /*** This characteristic can be subscribed to by writing 0x00 and 0x01 to the CCCD ***/
                .uuid = &gatt_svr_chr_uuid.u,
                .access_cb = gatt_svc_access,
#if CONFIG_EXAMPLE_ENCRYPTION
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE |
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC |
                BLE_GATT_CHR_F_NOTIFY,
#else
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_NOTIFY,
#endif
                .val_handle = &gatt_svr_chr_val_handle,
                .descriptors = (struct ble_gatt_dsc_def[])
                { {
                      .uuid = &gatt_svr_dsc_uuid.u,
#if CONFIG_EXAMPLE_ENCRYPTION
                      .att_flags = BLE_ATT_F_READ | BLE_ATT_F_READ_ENC,
#else
                      .att_flags = BLE_ATT_F_READ,
#endif
                      .access_cb = gatt_svc_access,
                    }, {
                      0, /* No more descriptors in this characteristic */
                    }
                },
            }, {
                0, /* No more characteristics in this service. */
            }
        },
    },

    {
        0, /* No more services. */
    },
};

static int
gatt_svr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
               void *dst, uint16_t *len)
{
    uint16_t om_len;
    int rc;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

/**
 * Access callback whenever a characteristic/descriptor is read or written to.
 * Here reads and writes need to be handled.
 * ctxt->op tells weather the operation is read or write and
 * weather it is on a characteristic or descriptor,
 * ctxt->dsc->uuid tells which characteristic/descriptor is accessed.
 * Accordingly do:
 *     Append the value to ctxt->om if the operation is READ
 *     Write ctxt->om to the value if the operation is WRITE
 **/
static int
gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    const ble_uuid_t *uuid;
    int rc;

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        uuid = ctxt->chr->uuid;
        if (ble_uuid_cmp(uuid, &gatt_svr_chr_uuid.u) == 0) {
            rc = os_mbuf_append(ctxt->om,
                                &gatt_svr_chr_val,
                                sizeof(gatt_svr_chr_val));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        goto unknown;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        uuid = ctxt->chr->uuid;
        if (ble_uuid_cmp(uuid, &gatt_svr_chr_uuid.u) == 0) {
            rc = gatt_svr_write(ctxt->om,
                                sizeof(gatt_svr_chr_val),
                                sizeof(gatt_svr_chr_val),
                                &gatt_svr_chr_val, NULL);
            if (gatt_svr_is_subscribed(attr_handle)) {
                struct os_mbuf *txom;
                uint8_t notification[] = {0x00, 0x01, 0x02, 0x03};
                txom = ble_hs_mbuf_from_flat(notification, sizeof(notification));
                /* No need to free txom as it is consumed by ble_gattc_notify_custom */
                rc = ble_gattc_notify_custom(conn_handle,
                                             gatt_svr_chr_val_handle, txom);
                if (rc == 0) {
                    MODLOG_DFLT(INFO, "Notification sent succesfully\n");
                } else {
                    MODLOG_DFLT(INFO, "Error in sending notification rc = %d\n", rc);
                }
            }
            return rc;
        }
        goto unknown;

    case BLE_GATT_ACCESS_OP_READ_DSC:
        uuid = ctxt->dsc->uuid;
        if (ble_uuid_cmp(uuid, &gatt_svr_dsc_uuid.u) == 0) {
            rc = os_mbuf_append(ctxt->om,
                                &gatt_svr_dsc_val,
                                sizeof(gatt_svr_chr_val));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        goto unknown;

    case BLE_GATT_ACCESS_OP_WRITE_DSC:
        goto unknown;

    default:
        goto unknown;
    }

unknown:
    /* Unknown characteristic/descriptor;
     * The NimBLE host should not have called this function;
     */
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                    ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                    "def_handle=%d val_handle=%d\n",
                    ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                    ctxt->chr.def_handle,
                    ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                    ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

/**
 * Tells if a characteristic has been subscribed to
 * based on it's value handle.
 * All subscribed characteristic value handles are stored
 * in an SLIST.
 **/
static bool
gatt_svr_is_subscribed(uint16_t val_handle)
{
    struct subscription *sub;
    SLIST_FOREACH(sub, &slist, next) {
        if (sub->val_handle == val_handle) {
            return true;
        }
    }
    return false;
}

/*
 * Call this with the value handle of the characteristic
 * that needs to be subscribed to, to add the handle in the
 * list of subscribed characteristics
 */
int
gatt_svr_subscribe(uint16_t val_handle) {
    struct subscription *sub;

    SLIST_FOREACH(sub, &slist, next) {
        if (sub->val_handle == val_handle) {
            return BLE_ATT_ERR_INVALID_HANDLE;
        }
    }
    sub = os_memblock_get(&subscription_list_mempool);
    if (sub == NULL) {
        /* Out of memory */
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    memset(sub, 0, sizeof(struct subscription));
    SLIST_NEXT(sub, next) = NULL;
    sub->val_handle = val_handle;

    SLIST_INSERT_HEAD(&slist, sub, next);

    return 0;
}

/**
 * Removes all subscriptions from the list
 * of subscribed characteristics.
 * Called upon disconnect
 **/
void
gatt_svr_subscription_delete()
{
    struct subscription *sub;

    while(true) {
        sub = SLIST_FIRST(&slist);
        if (sub == NULL) {
            break;
        }
        SLIST_REMOVE_HEAD(&slist, next);
        os_memblock_put(&subscription_list_mempool, sub);
        sub = NULL;
    }

    os_mempool_clear(&subscription_list_mempool);
    free(subscription_list_buffer);
    subscription_list_buffer = NULL;
}

int
gatt_svr_init(void)
{
    int rc;

    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_svc_ans_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    /* Setting a value for the read-only descriptor */
    gatt_svr_dsc_val = 0x99;

    /* Allocating mempool for the SLIST */
    subscription_list_buffer = malloc(OS_MEMPOOL_BYTES(MAX_NOTIFY, sizeof(struct subscription)));
    if (subscription_list_buffer == NULL) {
        rc = BLE_HS_ENOMEM;
        return rc;
    }
    rc = os_mempool_init(&subscription_list_mempool, MAX_NOTIFY,
                         sizeof(struct subscription), subscription_list_buffer,
                         "subscription_list_mempool");
    if (rc != 0) {
        rc = BLE_HS_EOS;
        free(subscription_list_buffer);
        subscription_list_buffer = NULL;
        MODLOG_DFLT(ERROR, "Error while allocating memory\n");
        return rc;
    }

    SLIST_INIT(&slist);
    assert(SLIST_EMPTY(&slist));

    return 0;
}
