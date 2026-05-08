/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/autoconf.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/common/bt_str.h>

#include <../host/conn_internal.h>

#include "host/ble_hs.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"

#include "common/host.h"

LOG_MODULE_REGISTER(ISO_NGDB, CONFIG_BT_ISO_LOG_LEVEL);

static int gattc_db_find_inc_svcs_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          const struct ble_gatt_svc *svc,
                                          void *arg);

static int gattc_db_disc_all_inc_chrs_cb_safe(uint16_t conn_handle,
                                              const struct ble_gatt_error *error,
                                              const struct ble_gatt_chr *chrc,
                                              void *arg);

static int gattc_db_disc_all_chrs_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          const struct ble_gatt_chr *chrc,
                                          void *arg);

static int gattc_db_disc_all_inc_dscs_cb_safe(uint16_t conn_handle,
                                              const struct ble_gatt_error *error,
                                              uint16_t chr_val_handle,
                                              const struct ble_gatt_dsc *dsc,
                                              void *arg);

static int gattc_db_disc_all_dscs_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          uint16_t chr_val_handle,
                                          const struct ble_gatt_dsc *dsc,
                                          void *arg);

static int gattc_db_enable_notify_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          struct ble_gatt_attr *attr,
                                          void *arg);

struct gattc_db_svc {
    struct ble_gatt_svc svc;    /* Information of this service */

    uint8_t inc_svc_disc : 1;
    uint8_t chrc_disc : 1;
    uint8_t dsc_disc : 1;
    uint8_t cccd_write : 1;

    size_t inc_svc_num;         /* Number of included services */

    sys_slist_t inc_svc_list;   /* List of the included services of this service */
    sys_slist_t chrc_list;      /* List of the characteristics of this service */

    sys_snode_t node;
};

struct gattc_db_inc_svc {
    uint16_t attr_handle;       /* Attribute handle of Include declaration */

    struct ble_gatt_svc svc;    /* Information of this included service */

    uint8_t chrc_disc : 1;
    uint8_t dsc_disc : 1;
    uint8_t is_primary : 1;

    sys_slist_t chrc_list;      /* List of the characteristics of this included service */

    sys_snode_t node;
};

struct gattc_db_chrc {
    struct ble_gatt_chr chrc;   /* Information of this characteristic */

    uint8_t cccd_write : 1;

    struct ble_gatt_dsc cccd;   /* For each characteristic, only store the CCCD information */

    sys_snode_t node;
};

struct gattc_db_dsc {
    struct ble_gatt_dsc dsc;    /* Information of this descriptor */

    sys_snode_t node;
};

static struct gattc_db {
    uint16_t conn_handle;
    enum {
        DISC_IDLE = 0,
        DISC_IN_PROGRESS,
        DISC_SUCCESS,
        DISC_FAIL,
    } status;
    sys_slist_t svc_list;       /* List of the GATT server database of the connection */
} gattc_db[CONFIG_BT_MAX_CONN] = {
    [0 ...(CONFIG_BT_MAX_CONN - 1)] = {
        .conn_handle = UINT16_MAX,
        .status = DISC_IDLE,
    },
};

static struct gattc_db *gattc_db_add(uint16_t conn_handle)
{
    for (size_t i = 0; i < ARRAY_SIZE(gattc_db); i++) {
        if (gattc_db[i].conn_handle == UINT16_MAX) {
            gattc_db[i].conn_handle = conn_handle;
            sys_slist_init(&gattc_db[i].svc_list);

            return &gattc_db[i];
        }
    }

    return NULL;
}

static struct gattc_db *gattc_db_find(uint16_t conn_handle)
{
    for (size_t i = 0; i < ARRAY_SIZE(gattc_db); i++) {
        if (gattc_db[i].conn_handle == conn_handle) {
            return &gattc_db[i];
        }
    }

    return NULL;
}

static void gattc_db_chrc_clear(sys_slist_t *chrc_list)
{
    struct gattc_db_chrc *achrc;
    sys_snode_t *node;

    while ((node = sys_slist_get(chrc_list)) != NULL) {
        achrc = SYS_SLIST_CONTAINER(node, achrc, node);
        free(achrc);
    }
}

static void gattc_db_del(struct gattc_db *adb)
{
    struct gattc_db_inc_svc *ainc_svc;
    struct gattc_db_svc *asvc;
    sys_snode_t *node;

    assert(adb);

    while ((node = sys_slist_get(&adb->svc_list)) != NULL) {
        asvc = SYS_SLIST_CONTAINER(node, asvc, node);

        while ((node = sys_slist_get(&asvc->inc_svc_list)) != NULL) {
            ainc_svc = SYS_SLIST_CONTAINER(node, ainc_svc, node);
            gattc_db_chrc_clear(&ainc_svc->chrc_list);
            free(ainc_svc);
        }

        gattc_db_chrc_clear(&asvc->chrc_list);
        free(asvc);
    }

    adb->conn_handle = UINT16_MAX;
    adb->status = DISC_IDLE;
    sys_slist_init(&adb->svc_list);
}

static void gattc_db_svc_insert(struct gattc_db *adb, const struct ble_gatt_svc *svc)
{
    struct gattc_db_svc *asvc;

    asvc = calloc(1, sizeof(*asvc));
    assert(asvc);

    memcpy(&asvc->svc, svc, sizeof(asvc->svc));
    sys_slist_init(&asvc->inc_svc_list);
    sys_slist_init(&asvc->chrc_list);

    sys_slist_append(&adb->svc_list, &asvc->node);

    LOG_DBG("[N]GattcDbSvcInsert");
}

static void gattc_db_inc_svc_insert(struct gattc_db_svc *asvc, const struct ble_gatt_svc *inc_svc)
{
    struct gattc_db_inc_svc *ainc_svc;

    ainc_svc = calloc(1, sizeof(*ainc_svc));
    assert(ainc_svc);

    memcpy(&ainc_svc->svc, inc_svc, sizeof(ainc_svc->svc));
    sys_slist_init(&ainc_svc->chrc_list);

    sys_slist_append(&asvc->inc_svc_list, &ainc_svc->node);
    asvc->inc_svc_num += 1;

    /* Record the attribute handle of Include declaration.
     * TODO:
     * Update attr_handle properly when CONFIG_BT_NIMBLE_INCL_SVC_DISCOVERY is enabled.
     */
    ainc_svc->attr_handle = asvc->svc.start_handle + asvc->inc_svc_num;

    LOG_DBG("[N]GattcDbIncSvcInsert[%u][%u]", asvc->svc.start_handle, asvc->inc_svc_num);
}

static void gattc_db_chrc_insert(sys_slist_t *chrc_list, const struct ble_gatt_chr *chrc)
{
    struct gattc_db_chrc *achrc;

    achrc = calloc(1, sizeof(*achrc));
    assert(achrc);

    memcpy(&achrc->chrc, chrc, sizeof(achrc->chrc));

    sys_slist_append(chrc_list, &achrc->node);

    LOG_DBG("[N]GattcDbChrcInsert");
}

static void gattc_db_dsc_cccd_store(sys_slist_t *chrc_list,
                                    uint16_t chr_val_handle,
                                    const struct ble_gatt_dsc *dsc)
{
    struct gattc_db_chrc *achrc;

    /* LOG_DBG("[N]GattcDbDscCccdStore[%u]", chr_val_handle); */

    SYS_SLIST_FOR_EACH_CONTAINER(chrc_list, achrc, node) {
        if (achrc->chrc.val_handle == dsc->handle - 1) {
            if (achrc->cccd.handle) {
                LOG_WRN("[N]GattcDbCccAlreadyUpd[%u][%u]", chr_val_handle, achrc->cccd.handle);
                return;
            }

            /* LOG_DBG("[N]GattcDbCccUpd[%u][%u]", achrc->chrc.val_handle, dsc->handle); */

            memcpy(&achrc->cccd, dsc, sizeof(achrc->cccd));
            return;
        }
    }
}

static struct gattc_db_svc *gattc_db_disc_find(struct gattc_db *adb)
{
    struct gattc_db_svc *asvc;

    /* LOG_DBG("[N]GattcDbDiscFind"); */

    SYS_SLIST_FOR_EACH_CONTAINER(&adb->svc_list, asvc, node) {
        /* LOG_DBG("[N]GattcDbSvc[%u][%u][%u][%u]", */
        /*         asvc->inc_svc_disc, asvc->chrc_disc, */
        /*         asvc->dsc_disc, asvc->cccd_write); */

        if (asvc->inc_svc_disc == 0 ||
                asvc->chrc_disc == 0 ||
                asvc->dsc_disc == 0 ||
                asvc->cccd_write == 0) {
            return asvc;
        }
    }

    return NULL;
}

static int gattc_db_disc_start(struct gattc_db *adb, struct gattc_db_svc *asvc)
{
    struct gattc_db_inc_svc *ainc_svc;
    struct gattc_db_chrc *achrc;
    uint16_t value = 0;
    int rc;

    /* LOG_DBG("[N]GattcDbDiscStart[%u]", adb->conn_handle); */

    if (asvc->inc_svc_disc == 0) {
        LOG_DBG("[N]GattcDbIncSvcDisc[%u][%u]", asvc->svc.start_handle, asvc->svc.end_handle);

        /* TODO:
         * Enable CONFIG_BT_NIMBLE_INCL_SVC_DISCOVERY for discovering included services.
         */
        rc = ble_gattc_find_inc_svcs(adb->conn_handle,
                                     asvc->svc.start_handle,
                                     asvc->svc.end_handle,
                                     gattc_db_find_inc_svcs_cb_safe,
                                     asvc);
        if (rc) {
            LOG_ERR("[N]GattcDbFindIncSvcsFail[%d]", rc);
            adb->status = DISC_FAIL;
        } else {
            adb->status = DISC_IN_PROGRESS;
        }
        return rc;
    }

    /* Start characteristic discovery, the included service if existed
     * will be discovered firstly.
     */
    SYS_SLIST_FOR_EACH_CONTAINER(&asvc->inc_svc_list, ainc_svc, node) {
        if (ainc_svc->chrc_disc == 0) {
            LOG_DBG("[N]GattcDbIncChrcDisc[%u][%u]", ainc_svc->svc.start_handle, ainc_svc->svc.end_handle);

            rc = ble_gattc_disc_all_chrs(adb->conn_handle,
                                         ainc_svc->svc.start_handle,
                                         ainc_svc->svc.end_handle,
                                         gattc_db_disc_all_inc_chrs_cb_safe,
                                         ainc_svc);
            if (rc) {
                LOG_ERR("[N]GattcDbDiscAllChrsFail[%d]", rc);
                adb->status = DISC_FAIL;
            } else {
                adb->status = DISC_IN_PROGRESS;
            }
            return rc;
        }
    }

    /* Discover the characteristics of this service (all the included services are done) */
    if (asvc->chrc_disc == 0) {
        LOG_DBG("[N]GattcDbChrcDisc[%u][%u]", asvc->svc.start_handle, asvc->svc.end_handle);

        rc = ble_gattc_disc_all_chrs(adb->conn_handle,
                                     asvc->svc.start_handle,
                                     asvc->svc.end_handle,
                                     gattc_db_disc_all_chrs_cb_safe,
                                     asvc);
        if (rc) {
            LOG_ERR("[N]GattcDbDiscAllChrsFail[%d]", rc);
            adb->status = DISC_FAIL;
        } else {
            adb->status = DISC_IN_PROGRESS;
        }
        return rc;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&asvc->inc_svc_list, ainc_svc, node) {
        if (ainc_svc->dsc_disc == 0) {
            /* If trying to discover CCCD of a characteristic, then the service
             * end_handle must be not smaller than start_handle plus 4, i.e. at
             * least 4 attributes shall exist in the service. They are:
             * - Service declaration
             * - Characteristic declaration
             * - Characteristic value
             * - Characteristic CCCD
             */
            LOG_DBG("[N]GattcDbIncDscDisc[%u][%u]", ainc_svc->svc.start_handle, ainc_svc->svc.end_handle);

            if (ainc_svc->svc.end_handle >= ainc_svc->svc.start_handle + 4) {
                rc = ble_gattc_disc_all_dscs(adb->conn_handle,
                                             ainc_svc->svc.start_handle,
                                             ainc_svc->svc.end_handle,
                                             gattc_db_disc_all_inc_dscs_cb_safe,
                                             ainc_svc);
                if (rc) {
                    LOG_ERR("[N]GattcDbDiscAllDscsFail[%d]", rc);
                    adb->status = DISC_FAIL;
                } else {
                    adb->status = DISC_IN_PROGRESS;
                }
                return rc;
            }

            ainc_svc->dsc_disc = 1;
        }
    }

    if (asvc->dsc_disc == 0) {
        /* If trying to discover CCCD of a characteristic, then the service
         * end_handle must be not smaller than start_handle plus 4, i.e. at
         * least 4 attributes shall exist in the service. They are:
         * - Service declaration
         * - Characteristic declaration
         * - Characteristic value
         * - Characteristic CCCD
         */
        LOG_DBG("[N]GattcDbDscDisc[%u][%u]", asvc->svc.start_handle, asvc->svc.end_handle);

        if (asvc->svc.end_handle >= asvc->svc.start_handle + 4) {
            rc = ble_gattc_disc_all_dscs(adb->conn_handle,
                                         asvc->svc.start_handle,
                                         asvc->svc.end_handle,
                                         gattc_db_disc_all_dscs_cb_safe,
                                         asvc);
            if (rc) {
                LOG_ERR("[N]GattcDbDiscAllDscsFail[%d]", rc);
                adb->status = DISC_FAIL;
            } else {
                adb->status = DISC_IN_PROGRESS;
            }
            return rc;
        }

        asvc->dsc_disc = 1;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&asvc->inc_svc_list, ainc_svc, node) {
        SYS_SLIST_FOR_EACH_CONTAINER(&ainc_svc->chrc_list, achrc, node) {
            LOG_DBG("[N]GattcDbIncCccWr[%u][%u]", achrc->cccd.handle, achrc->cccd_write);

            if (achrc->cccd.handle && achrc->cccd_write == 0) {
                /* Reset value for each characteristic */
                value = 0;

                if (achrc->chrc.properties & BLE_GATT_CHR_PROP_NOTIFY) {
                    value |= BT_GATT_CCC_NOTIFY;
                }
                if (achrc->chrc.properties & BLE_GATT_CHR_PROP_INDICATE) {
                    value |= BT_GATT_CCC_INDICATE;
                }

                rc = ble_gattc_write_flat(adb->conn_handle,
                                          achrc->cccd.handle,
                                          &value, sizeof(value),
                                          gattc_db_enable_notify_cb_safe,
                                          achrc);
                if (rc) {
                    LOG_ERR("[N]GattcDbWrFlatFail[%d]", rc);
                    adb->status = DISC_FAIL;
                } else {
                    adb->status = DISC_IN_PROGRESS;
                }
                return rc;
            }
        }
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&asvc->chrc_list, achrc, node) {
        /* LOG_DBG("[N]GattcDbCccWr[%u][%u]", achrc->cccd.handle, achrc->cccd_write); */

        if (achrc->cccd.handle && achrc->cccd_write == 0) {
            /* Reset value for each characteristic */
            value = 0;

            if (achrc->chrc.properties & BLE_GATT_CHR_PROP_NOTIFY) {
                value |= BT_GATT_CCC_NOTIFY;
            }
            if (achrc->chrc.properties & BLE_GATT_CHR_PROP_INDICATE) {
                value |= BT_GATT_CCC_INDICATE;
            }

            rc = ble_gattc_write_flat(adb->conn_handle,
                                      achrc->cccd.handle,
                                      &value, sizeof(value),
                                      gattc_db_enable_notify_cb_safe,
                                      achrc);
            if (rc) {
                LOG_ERR("[N]GattcDbWrFlatFail[%d]", rc);
                adb->status = DISC_FAIL;
            } else {
                adb->status = DISC_IN_PROGRESS;
            }
            return rc;
        }
    }

    LOG_DBG("[N]GattcDbDiscEnd");

    asvc->cccd_write = 1;
    adb->status = DISC_SUCCESS;

    return 0;
}

static bool gattc_inc_svc_is_primary(struct gattc_db *adb,
                                     struct gattc_db_inc_svc *ainc_svc)
{
    struct gattc_db_svc *asvc;

    LOG_DBG("[N]GattcDbIncSvcIsPrimary");

    SYS_SLIST_FOR_EACH_CONTAINER(&adb->svc_list, asvc, node) {
        if (ainc_svc->svc.start_handle == asvc->svc.start_handle &&
                ainc_svc->svc.end_handle == asvc->svc.end_handle) {
            if (ble_uuid_cmp(&ainc_svc->svc.uuid.u, &asvc->svc.uuid.u)) {
                LOG_WRN("[N]GattcDbInvSvcMismatch[%u][%u][%u][%u][%04x][%04x]",
                        ainc_svc->svc.start_handle, ainc_svc->svc.end_handle,
                        asvc->svc.start_handle, asvc->svc.end_handle,
                        ainc_svc->svc.uuid.u16.value, asvc->svc.uuid.u16.value);
                return false;
            }

            LOG_DBG("[N]GattcDbIncSvcPrimary[%u][%u][%04x]",
                    ainc_svc->svc.start_handle, ainc_svc->svc.end_handle,
                    ainc_svc->svc.uuid.u16.value);
            return true;
        }
    }

    return false;
}

static void gattc_inc_svc_extra_check(struct gattc_db *adb)
{
    struct gattc_db_inc_svc *ainc_svc;
    struct gattc_db_svc *asvc;

    LOG_DBG("[N]GattcDbIncSvcExtraCheck");

    SYS_SLIST_FOR_EACH_CONTAINER(&adb->svc_list, asvc, node) {
        SYS_SLIST_FOR_EACH_CONTAINER(&asvc->inc_svc_list, ainc_svc, node) {
            ainc_svc->is_primary = gattc_inc_svc_is_primary(adb, ainc_svc);
        }
    }
}

static void gattc_db_disc(struct gattc_db *adb, uint8_t status)
{
    struct gattc_db_svc *asvc = NULL;
    int rc;

    /* LOG_DBG("[N]GattcDbDisc"); */

    /* Delete the gatt client database and notify the discovery failure
     * to the upper layer.
     */
    if (status == DISC_FAIL) {
        gattc_db_del(adb);

        bt_le_nimble_gatt_post_disc_cmpl_event(adb->conn_handle, 0x01);
        return;
    }

    while (1) {
        asvc = gattc_db_disc_find(adb);
        if (asvc == NULL) {
            /* All the GATT server database is discovered */

            /* Check if any included service is a primary service */
            gattc_inc_svc_extra_check(adb);

            bt_le_nimble_gatt_post_disc_cmpl_event(adb->conn_handle, 0x00);
            break;
        }

        rc = gattc_db_disc_start(adb, asvc);
        if (rc) {
            /* If the discovery of this gatt client database fails,
             * we will delete the gatt client database and post the
             * discovery completion event.
             */
            gattc_db_del(adb);

            bt_le_nimble_gatt_post_disc_cmpl_event(adb->conn_handle, 0x01);
            break;
        }

        if (adb->status == DISC_IN_PROGRESS) {
            break;
        }

        /* If the discovery of this gatt client database is completed,
         * we will continue and try to find the next to-be-discovered
         * gatt client database.
         */
    }
}

static int gattc_db_disc_all_svcs_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          const struct ble_gatt_svc *svc,
                                          void *arg)
{
    struct gattc_db *adb;
    int rc = 0;

    /* LOG_DBG("[N]GattcDbDiscAllSvcsCb[%u][%04x]", conn_handle, error->status); */

    bt_le_host_lock();

    adb = arg;
    assert(adb);
    assert(adb->conn_handle == conn_handle);

    switch (error->status) {
    case 0:
        assert(svc);

        LOG_DBG("[N]GattcDbDiscAllSvcs[%s][%u][%u]",
                audio_svc_uuid_to_str(svc->uuid.u16.value),
                svc->start_handle, svc->end_handle);

        gattc_db_svc_insert(adb, svc);
        break;

    case BLE_HS_EDONE:
        break;

    default:
        LOG_WRN("[N]GattcDbDiscAllSvcsStatus[%04x]", error->status);
        rc = error->status;
        break;
    }

    if (error->status) {
        if (error->status == BLE_HS_EDONE) {
            gattc_db_disc(adb, DISC_SUCCESS);
        } else {
            gattc_db_disc(adb, DISC_FAIL);
        }
    }

    bt_le_host_unlock();

    return rc;
}

static int gattc_db_find_inc_svcs_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          const struct ble_gatt_svc *svc,
                                          void *arg)
{
    struct gattc_db_svc *asvc;
    struct gattc_db *adb;
    int rc = 0;

    asvc = arg;
    assert(asvc);

    /* LOG_DBG("[N]GattcDbFindIncSvcsCb[%u][%04x]", conn_handle, error->status); */

    bt_le_host_lock();

    adb = gattc_db_find(conn_handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        rc = -ENODEV;
        goto end;
    }

    switch (error->status) {
    case 0:
        assert(svc);

        LOG_DBG("[N]GattcDbFindIncSvcs[%s][%u][%u]",
                audio_svc_uuid_to_str(svc->uuid.u16.value),
                svc->start_handle, svc->end_handle);

        gattc_db_inc_svc_insert(asvc, svc);
        break;

    case BLE_HS_EDONE:
        break;

    default:
        rc = error->status;
        break;
    }

    if (error->status) {
        if (error->status == BLE_HS_EDONE ||
            error->status == BLE_HS_EINVAL) {
            /* BLE_HS_EINVAL here is benign noise from NimBLE iteration,
             * not a real failure. Root cause:
             *
             * In ble_gattc_find_inc_svcs_rx_complete (ble_gattc.c), the
             * end-of-procedure check is `prev_handle == 0xffff` only. If
             * the last include attribute lands on the parent service's
             * end_handle (e.g. a service with just decl + include), then
             * prev_handle is bumped to end_handle and the resume path
             * calls ble_att_clt_tx_read_type with [end+1, end] -- which
             * fails its `start > end` guard with BLE_HS_EINVAL and prints
             *   E NimBLE: ble_att_clt_tx_read_type rc=3 <end+1> <end>
             *
             * The proper upstream fix would be in ble_gattc.c:
             *
             *   -    if (proc->find_inc_svcs.prev_handle == 0xffff) {
             *   +    if (proc->find_inc_svcs.prev_handle == 0xffff ||
             *   +        proc->find_inc_svcs.prev_handle >=
             *   +            proc->find_inc_svcs.end_handle) {
             *            // Procedure complete.
             *            ble_gattc_find_inc_svcs_cb(proc, BLE_HS_EDONE, 0, NULL);
             *            return BLE_HS_EDONE;
             *        }
             *
             * We keep NimBLE untouched and absorb the EINVAL here.
             *
             * Mark the included service discovery as completed so the
             * state machine moves on instead of stalling.
             */
            asvc->inc_svc_disc = 1;

            gattc_db_disc(adb, DISC_SUCCESS);
        } else {
            LOG_WRN("[N]GattcDbFindIncSvcsStatus[%04x]", error->status);
            gattc_db_disc(adb, DISC_FAIL);
        }
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_db_disc_all_inc_chrs_cb_safe(uint16_t conn_handle,
                                              const struct ble_gatt_error *error,
                                              const struct ble_gatt_chr *chrc,
                                              void *arg)
{
    struct gattc_db_inc_svc *ainc_svc;
    struct gattc_db *adb;
    int rc = 0;

    ainc_svc = arg;
    assert(ainc_svc);

    /* LOG_DBG("[N]GattcDbDiscAllIncChrsCb[%u][%04x]", conn_handle, error->status); */

    bt_le_host_lock();

    adb = gattc_db_find(conn_handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        rc = -ENODEV;
        goto end;
    }

    switch (error->status) {
    case 0:
        assert(chrc);

        LOG_DBG("[N]GattcDbDiscAllIncChrs[%u][%u]", chrc->def_handle, chrc->val_handle);

        gattc_db_chrc_insert(&ainc_svc->chrc_list, chrc);
        break;

    case BLE_HS_EDONE:
        break;

    default:
        LOG_WRN("[N]GattcDbDiscAllIncChrsStatus[%04x]", error->status);
        rc = error->status;
        break;
    }

    if (error->status) {
        if (error->status == BLE_HS_EDONE) {
            /* Mark the characteristic discovery as completed */
            ainc_svc->chrc_disc = 1;

            gattc_db_disc(adb, DISC_SUCCESS);
        } else {
            gattc_db_disc(adb, DISC_FAIL);
        }
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_db_disc_all_chrs_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          const struct ble_gatt_chr *chrc,
                                          void *arg)
{
    struct gattc_db_svc *asvc;
    struct gattc_db *adb;
    int rc = 0;

    asvc = arg;
    assert(asvc);

    /* LOG_DBG("[N]GattcDbDiscAllChrsCb[%u][%04x]", conn_handle, error->status); */

    bt_le_host_lock();

    adb = gattc_db_find(conn_handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        rc = -ENODEV;
        goto end;
    }

    switch (error->status) {
    case 0:
        assert(chrc);

        LOG_DBG("[N]GattcDbDiscAllChrs[%u][%u]", chrc->def_handle, chrc->val_handle);

        gattc_db_chrc_insert(&asvc->chrc_list, chrc);
        break;

    case BLE_HS_EDONE:
        break;

    default:
        LOG_WRN("[N]GattcDbDiscAllChrsStatus[%04x]", error->status);
        rc = error->status;
        break;
    }

    if (error->status) {
        if (error->status == BLE_HS_EDONE) {
            /* Mark the characteristic discovery as completed */
            asvc->chrc_disc = 1;

            gattc_db_disc(adb, DISC_SUCCESS);
        } else {
            gattc_db_disc(adb, DISC_FAIL);
        }
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_db_disc_all_inc_dscs_cb_safe(uint16_t conn_handle,
                                              const struct ble_gatt_error *error,
                                              uint16_t chr_val_handle,
                                              const struct ble_gatt_dsc *dsc,
                                              void *arg)
{
    struct gattc_db_inc_svc *ainc_svc;
    struct gattc_db *adb;
    int rc = 0;

    ainc_svc = arg;
    assert(ainc_svc);

    LOG_DBG("[N]GattcDbDiscAllIncDscsCb[%u][%04x][%u]",
            conn_handle, error->status, chr_val_handle);

    bt_le_host_lock();

    adb = gattc_db_find(conn_handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        rc = -ENODEV;
        goto end;
    }

    switch (error->status) {
    case 0:
        assert(dsc);

        if (dsc->uuid.u.type == BLE_UUID_TYPE_16 &&
                dsc->uuid.u16.value == BT_UUID_GATT_CCC_VAL) {
            LOG_DBG("[N]GattcDbDiscAllIncDscs[%u][%u]", chr_val_handle, dsc->handle);

            gattc_db_dsc_cccd_store(&ainc_svc->chrc_list, chr_val_handle, dsc);
        }
        break;

    case BLE_HS_EDONE:
        break;

    default:
        LOG_WRN("[N]GattcDbDiscAllIncDscsStatus[%04x]", error->status);
        rc = error->status;
        break;
    }

    if (error->status) {
        if (error->status == BLE_HS_EDONE) {
            /* Mark the descriptor discovery as completed */
            ainc_svc->dsc_disc = 1;

            gattc_db_disc(adb, DISC_SUCCESS);
        } else {
            gattc_db_disc(adb, DISC_FAIL);
        }
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_db_disc_all_dscs_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          uint16_t chr_val_handle,
                                          const struct ble_gatt_dsc *dsc,
                                          void *arg)
{
    struct gattc_db_svc *asvc;
    struct gattc_db *adb;
    int rc = 0;

    asvc = arg;
    assert(asvc);

    /* LOG_DBG("[N]GattcDbDiscAllDscsCb[%u][%04x][%u]", */
    /*         conn_handle, error->status, chr_val_handle); */

    bt_le_host_lock();

    adb = gattc_db_find(conn_handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        rc = -ENODEV;
        goto end;
    }

    switch (error->status) {
    case 0:
        assert(dsc);

        if (dsc->uuid.u.type == BLE_UUID_TYPE_16 &&
                dsc->uuid.u16.value == BT_UUID_GATT_CCC_VAL) {
            LOG_DBG("[N]GattcDbDiscAllDscs[%u]", dsc->handle);

            gattc_db_dsc_cccd_store(&asvc->chrc_list, chr_val_handle, dsc);
        }
        break;

    case BLE_HS_EDONE:
        break;

    default:
        LOG_WRN("[N]GattcDbDiscAllDscsStatus[%04x]", error->status);
        rc = error->status;
        break;
    }

    if (error->status) {
        if (error->status == BLE_HS_EDONE) {
            /* Mark the descriptor discovery as completed */
            asvc->dsc_disc = 1;

            gattc_db_disc(adb, DISC_SUCCESS);
        } else {
            gattc_db_disc(adb, DISC_FAIL);
        }
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_db_enable_notify_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          struct ble_gatt_attr *attr,
                                          void *arg)
{
    struct gattc_db_chrc *achrc;
    struct gattc_db *adb;
    int rc = 0;

    achrc = arg;
    assert(achrc);

    /* LOG_DBG("[N]GattcDbEnableNtfCb[%u][%04x]", conn_handle, error->status); */

    bt_le_host_lock();

    adb = gattc_db_find(conn_handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        rc = -ENODEV;
        goto end;
    }

    switch (error->status) {
    case 0:
        assert(attr);
        LOG_DBG("[N]GattcDbEnableNotify[%u]", attr->handle);
        break;

    case BLE_HS_EDONE:
        break;

    default:
        LOG_WRN("[N]GattcDbEnableNotifyStatus[%04x]", error->status);
        rc = error->status;
        break;
    }

    /* Mark the CCCD write as completed */
    achrc->cccd_write = 1;

    gattc_db_disc(adb, DISC_SUCCESS);

end:
    bt_le_host_unlock();

    return rc;
}

static int handle_gattc_disc_svc_by_uuid(struct bt_conn *conn, ble_uuid16_t *uuid,
                                         struct bt_gatt_discover_params *params)
{
    struct bt_gatt_service_val svc = {0};
    struct bt_uuid_16 svc_uuid = {0};
    struct bt_gatt_attr attr = {0};
    struct gattc_db_svc *asvc;
    struct gattc_db *adb;
    uint8_t ret;

    LOG_DBG("[N]GattcDbHdlDiscSvcByUuid[%u]", conn->handle);

    ret = BT_GATT_ITER_CONTINUE;

    adb = gattc_db_find(conn->handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        goto end;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&adb->svc_list, asvc, node) {
        if (asvc->svc.uuid.u.type != uuid->u.type ||
                asvc->svc.uuid.u16.value != uuid->value) {
            continue;
        }

        svc_uuid.uuid.type = BT_UUID_TYPE_16;
        svc_uuid.val = asvc->svc.uuid.u16.value;

        svc.uuid = &svc_uuid.uuid;
        svc.end_handle = asvc->svc.end_handle;

        attr.user_data = &svc;
        attr.handle = asvc->svc.start_handle;
        attr.uuid = params->uuid;

        ret = params->func(conn, &attr, params);

        /* LOG_DBG("[N]GattcDbSvcIter%s", ret == BT_GATT_ITER_CONTINUE ? "Cont" : "Stop"); */

        if (ret == BT_GATT_ITER_STOP) {
            goto end;
        }
    }

end:
    if (ret == BT_GATT_ITER_CONTINUE) {
        params->func(conn, NULL, params);
    }

    return 0;
}

static int handle_gattc_find_inc_svcs(struct bt_conn *conn,
                                      struct bt_gatt_discover_params *params)
{
    struct bt_gatt_include inc_svc = {0};
    struct gattc_db_inc_svc *ainc_svc;
    struct bt_uuid_16 svc_uuid = {0};
    struct bt_gatt_attr attr = {0};
    struct gattc_db_svc *asvc;
    struct gattc_db *adb;
    uint8_t ret;

    LOG_DBG("[N]GattcDbHdlFindIncSvcs[%u]", conn->handle);

    ret = BT_GATT_ITER_CONTINUE;

    adb = gattc_db_find(conn->handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        goto end;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&adb->svc_list, asvc, node) {
        SYS_SLIST_FOR_EACH_CONTAINER(&asvc->inc_svc_list, ainc_svc, node) {
            /* Make sure the attribute handle of Include declaration is within range */
            if (ainc_svc->attr_handle < params->start_handle ||
                    ainc_svc->attr_handle > params->end_handle) {
                continue;
            }

            svc_uuid.uuid.type = BT_UUID_TYPE_16;
            svc_uuid.val = ainc_svc->svc.uuid.u16.value;

            inc_svc.uuid = &svc_uuid.uuid;
            inc_svc.start_handle = ainc_svc->svc.start_handle;
            inc_svc.end_handle = ainc_svc->svc.end_handle;

            attr.user_data = &inc_svc;
            /* TODO:
             * When CONFIG_BT_NIMBLE_INCL_SVC_DISCOVERY is enabled, use ainc_svc->attr_handle here.
             */
            attr.handle = params->start_handle; /* Include declaration handle */

            ret = params->func(conn, &attr, params);

            /* LOG_DBG("[N]GattcDbSvcIter%s", ret == BT_GATT_ITER_CONTINUE ? "Cont" : "Stop"); */

            if (ret == BT_GATT_ITER_STOP) {
                goto end;
            }
        }
    }

end:
    if (ret == BT_GATT_ITER_CONTINUE) {
        params->func(conn, NULL, params);
    }

    return 0;
}

static uint8_t gattc_disc_chr_each(struct bt_conn *conn,
                                   struct gattc_db_chrc *achrc,
                                   struct bt_gatt_discover_params *params)
{
    struct bt_uuid_16 chrc_uuid = {0};
    struct bt_gatt_attr attr = {0};
    struct bt_gatt_chrc chrc = {0};

    /* Copy the full UUID based on actual type */
    switch (achrc->chrc.uuid.u.type) {
    case BLE_UUID_TYPE_16:
        chrc_uuid.uuid.type = BT_UUID_TYPE_16;
        chrc_uuid.val = achrc->chrc.uuid.u16.value;
        break;
    default:
        return BT_GATT_ITER_STOP;
    }

    chrc.uuid = &chrc_uuid.uuid;
    chrc.value_handle = achrc->chrc.val_handle;
    chrc.properties = achrc->chrc.properties;

    attr.user_data = &chrc;
    attr.handle = achrc->chrc.def_handle;

    /* LOG_DBG("[N]GattcDbDiscChrEach[%u][%s][%u][%u][%02x]", */
    /*         conn->handle, bt_uuid_str(chrc.uuid), attr.handle, */
    /*         chrc.value_handle, chrc.properties); */

    return params->func(conn, &attr, params);
}

static int handle_gattc_disc_chrs(struct bt_conn *conn, ble_uuid16_t *uuid,
                                  struct bt_gatt_discover_params *params)
{
    struct gattc_db_inc_svc *ainc_svc;
    struct gattc_db_chrc *achrc;
    struct gattc_db_svc *asvc;
    struct gattc_db *adb;
    uint8_t ret;

    LOG_DBG("[N]GattcDbHdlDiscChrs[%u]", conn->handle);

    ret = BT_GATT_ITER_CONTINUE;

    adb = gattc_db_find(conn->handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        goto end;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&adb->svc_list, asvc, node) {
        SYS_SLIST_FOR_EACH_CONTAINER(&asvc->inc_svc_list, ainc_svc, node) {
            /* If the included service is a primary service, which means it still exists
             * in the adb->svc_list, hence no need to find the characteristics here.
             */
            if (ainc_svc->is_primary) {
                continue;
            }

            SYS_SLIST_FOR_EACH_CONTAINER(&ainc_svc->chrc_list, achrc, node) {
                if (uuid && (achrc->chrc.uuid.u.type != uuid->u.type ||
                             achrc->chrc.uuid.u16.value != uuid->value)) {
                    continue;
                }

                /* Check if the characteristic value handle is within range */
                if (achrc->chrc.val_handle < params->start_handle ||
                        achrc->chrc.val_handle > params->end_handle) {
                    continue;
                }

                ret = gattc_disc_chr_each(conn, achrc, params);

                /* LOG_DBG("[N]GattcDbIncSvcIter%s", ret == BT_GATT_ITER_CONTINUE ? "Cont" : "Stop"); */

                if (ret == BT_GATT_ITER_STOP) {
                    goto end;
                }
            }
        }

        SYS_SLIST_FOR_EACH_CONTAINER(&asvc->chrc_list, achrc, node) {
            if (uuid && (achrc->chrc.uuid.u.type != uuid->u.type ||
                         achrc->chrc.uuid.u16.value != uuid->value)) {
                continue;
            }

            /* Check if the characteristic value handle is within range */
            if (achrc->chrc.val_handle < params->start_handle ||
                    achrc->chrc.val_handle > params->end_handle) {
                continue;
            }

            ret = gattc_disc_chr_each(conn, achrc, params);

            /* LOG_DBG("[N]GattcDbSvcIter%s", ret == BT_GATT_ITER_CONTINUE ? "Cont" : "Stop"); */

            if (ret == BT_GATT_ITER_STOP) {
                goto end;
            }
        }
    }

end:
    if (ret == BT_GATT_ITER_CONTINUE) {
        params->func(conn, NULL, params);
    }

    return 0;
}

void handle_gattc_db_disc_event_safe(struct bt_le_gattc_discover_event *event)
{
    struct bt_conn *conn;
    ble_uuid16_t uuid;

    LOG_DBG("[N]GattcDbHdlDiscEvt[%u][%u]", event->conn_handle, event->type);

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(event->conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("[N]GattcDbNotConn[%d]", __LINE__);
        goto end;
    }

    assert(event->params);

    switch (event->type) {
    case GATTC_DISC_TYPE_SVC_BY_UUID:
        uuid.u.type = BLE_UUID_TYPE_16;
        uuid.value = event->uuid;

        handle_gattc_disc_svc_by_uuid(conn, &uuid, event->params);
        break;

    case GATTC_DISC_TYPE_INC_SVCS:
        handle_gattc_find_inc_svcs(conn, event->params);
        break;

    case GATTC_DISC_TYPE_CHRS_BY_UUID:
        uuid.u.type = BLE_UUID_TYPE_16;
        uuid.value = event->uuid;

        handle_gattc_disc_chrs(conn, &uuid, event->params);
        break;

    case GATTC_DISC_TYPE_ALL_CHRS:
        handle_gattc_disc_chrs(conn, NULL, event->params);
        break;

    default:
        assert(0);
        break;
    }

end:
    bt_le_host_unlock();
}

int bt_le_nimble_gattc_db_disc_svc_by_uuid(struct bt_conn *conn, ble_uuid16_t *uuid,
                                           struct bt_gatt_discover_params *params)
{
    LOG_DBG("[N]GattcDbDiscSvcByUuid[%u]", conn->handle);

    return bt_le_nimble_gatt_post_disc_event(conn->handle, uuid, params, GATTC_DISC_TYPE_SVC_BY_UUID);
}

int bt_le_nimble_gattc_db_find_inc_svcs(struct bt_conn *conn,
                                        struct bt_gatt_discover_params *params)
{
    LOG_DBG("[N]GattcDbFindIncSvcs[%u]", conn->handle);

    return bt_le_nimble_gatt_post_disc_event(conn->handle, NULL, params, GATTC_DISC_TYPE_INC_SVCS);
}

int bt_le_nimble_gattc_db_disc_chrs_by_uuid(struct bt_conn *conn, ble_uuid16_t *uuid,
                                            struct bt_gatt_discover_params *params)
{
    LOG_DBG("[N]GattcDbDiscChrsByUuid[%u]", conn->handle);

    return bt_le_nimble_gatt_post_disc_event(conn->handle, uuid, params, GATTC_DISC_TYPE_CHRS_BY_UUID);
}

int bt_le_nimble_gattc_db_disc_all_chrs(struct bt_conn *conn,
                                        struct bt_gatt_discover_params *params)
{
    LOG_DBG("[N]GattcDbDiscAllChrs[%u]", conn->handle);

    return bt_le_nimble_gatt_post_disc_event(conn->handle, NULL, params, GATTC_DISC_TYPE_ALL_CHRS);
}

static int handle_gattc_disc_all_dscs(struct bt_conn *conn,
                                      struct bt_gatt_discover_params *params)
{
    struct bt_gatt_subscribe_params *sub_params;
    struct gattc_db_inc_svc *ainc_svc;
    struct bt_gatt_attr *found = NULL;
    struct bt_gatt_attr attr = {0};
    struct gattc_db_chrc *achrc;
    struct gattc_db_svc *asvc;
    struct gattc_db *adb;
    int rc = 0;

    /* Note:
     * For MCC, the procedures will be:
     * - Find GMCS;
     * - Find all characteristics of GMCS;
     * - Enable notification of the characteristics;
     * - Find included service (e.g. OTS);
     *
     * In this case, we will post events to the ISO task for finding GMCS
     * and all its characteristics.
     * But for enabling notification, if auto discovery is enabled (which
     * is by default), the only thing needs to be done is updating the
     * ccc_handle of each characteristic. In this case, no event will be
     * posted to the ISO task, instead after finding all characteristics,
     * we will directly do subscribe.
     * And after all the subscription are done, we will post an event for
     * finding the included service.
     */

    sub_params = params->sub_params;
    assert(sub_params);

    LOG_DBG("[N]GattcDbHdlDiscAllDscs[%u][%u]", conn->handle, sub_params->value_handle);

    adb = gattc_db_find(conn->handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbNotFound");
        rc = -ENODEV;
        goto end;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&adb->svc_list, asvc, node) {
        SYS_SLIST_FOR_EACH_CONTAINER(&asvc->inc_svc_list, ainc_svc, node) {
            if (sub_params->value_handle < ainc_svc->svc.start_handle ||
                    sub_params->value_handle > ainc_svc->svc.end_handle) {
                continue;
            }

            SYS_SLIST_FOR_EACH_CONTAINER(&ainc_svc->chrc_list, achrc, node) {
                if (achrc->chrc.val_handle == sub_params->value_handle) {
                    if (achrc->cccd.handle) {
                        attr.handle = achrc->cccd.handle;
                        found = &attr;
                    }

                    LOG_DBG("[N]GattcDbIncCccFound[%u]", achrc->cccd.handle);
                    goto end;
                }
            }
        }

        if (sub_params->value_handle < asvc->svc.start_handle ||
                sub_params->value_handle > asvc->svc.end_handle) {
            continue;
        }

        SYS_SLIST_FOR_EACH_CONTAINER(&asvc->chrc_list, achrc, node) {
            if (achrc->chrc.val_handle == sub_params->value_handle) {
                if (achrc->cccd.handle) {
                    attr.handle = achrc->cccd.handle;
                    found = &attr;
                }

                LOG_DBG("[N]GattcDbCccFound[%u]", achrc->cccd.handle);
                goto end;
            }
        }
    }

end:
    params->func(conn, found, params);

    return rc;
}

int bt_le_nimble_gattc_db_disc_all_dscs(struct bt_conn *conn,
                                        struct bt_gatt_discover_params *params)
{
    return handle_gattc_disc_all_dscs(conn, params);
}

int bt_le_nimble_gattc_db_auto_disc(uint16_t conn_handle)
{
    /* The GATTC procedures will be:
     * - Find all the primary services;
     * - Find if any service is included by each primary service;
     * - Find the characteristics of each Primary or Secondary service;
     * - Find the descriptor of each characteristic of each service.
     */
    struct gattc_db *adb;
    int rc = 0;

    LOG_DBG("[N]GattcDbAutoDisc[%u]", conn_handle);

    adb = gattc_db_find(conn_handle);
    if (adb) {
        LOG_WRN("[N]GattcDbExist");
        return -EALREADY;
    }

    adb = gattc_db_add(conn_handle);
    if (adb == NULL) {
        LOG_ERR("[N]GattcDbFull");
        return -ENOMEM;
    }

    rc = ble_gattc_disc_all_svcs(conn_handle, gattc_db_disc_all_svcs_cb_safe, adb);
    if (rc) {
        LOG_ERR("[N]GattcDbDiscAllSvcsFail[%d]", rc);

        /* Revert the previous addition of the GATT client database */
        gattc_db_del(adb);
    }

    return rc;
}

void bt_le_nimble_gattc_db_remove(uint16_t conn_handle)
{
    /* Note:
     * Even if the connection is disconnected, the GATT client database should
     * not be removed. And after the connection is re-established, no need to
     * re-discover the GATT client database which can save time.
     *
     * TODO:
     * If Service Change Indication is received, the GATT client database should
     * be removed and re-discovered.
     */
    struct gattc_db *adb = NULL;

    LOG_DBG("[N]GattcDbRemove[%u]", conn_handle);

    adb = gattc_db_find(conn_handle);
    if (adb == NULL) {
        /* Legitimate when the conn never ran auto-disc (e.g. pairing failed
         * or the app doesn't use GATT). Idempotent remove, not an error.
         */
        LOG_DBG("[N]GattcDbNotFoundToRemove[%u]", conn_handle);
        return;
    }

    gattc_db_del(adb);
}
