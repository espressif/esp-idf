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
#include "blehr_sens.h"

static const char *manuf_name = "Apache Mynewt ESP32 devkitC";
static const char *model_num = "Mynewt RAS Service";

static uint8_t ras_feat_val;
static uint16_t ras_feat_val_handle;

static int
gatt_svr_chr_access_ras_val(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Service: Ranging Data Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                /* Characteristic: Feature Value */
                .uuid = BLE_UUID16_DECLARE(BLE_UUID_RAS_FEATURES_VAL),
                .access_cb = gatt_svr_chr_access_ras_val,
                .val_handle = &ras_feat_val_handle,
                .flags = BLE_GATT_CHR_F_READ|BLE_GATT_CHR_F_READ_ENC,
           },{
                /* Characteristic: On demand ranging data */
                .uuid = BLE_UUID16_DECLARE(BLE_UUID_RAS_ONDEMAND_RD_VAL),
                .access_cb = gatt_svr_chr_access_ras_val,
                .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE |BLE_GATT_CHR_F_READ_ENC
                ,
           },{
               /* Characteristic: RAS Control Point */
               .uuid = BLE_UUID16_DECLARE(BLE_UUID_RAS_CP_VAL),
               .access_cb = gatt_svr_chr_access_ras_val,
               .flags = BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_INDICATE |BLE_GATT_CHR_F_READ_ENC ,
           },{
               /* Characteristic: RAS Data Ready */
               .uuid = BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_READY_VAL),
               .access_cb = gatt_svr_chr_access_ras_val,
              .flags = BLE_GATT_CHR_F_INDICATE | BLE_GATT_CHR_F_READ_ENC,
           },{
               /* Characteristic: RAS data overwritten */
                .uuid = BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_OVERWRITTEN_VAL),
                .access_cb = gatt_svr_chr_access_ras_val,
                .flags = BLE_GATT_CHR_F_INDICATE | BLE_GATT_CHR_F_READ_ENC,
           },{
                 0, /* No more characteristics in this service */
           },
         }
    },
    {
        0, /* No more services */
    },
};

static int
gatt_svr_chr_access_ras_val(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
   const ble_uuid_t *uuid;
   int rc;

   switch (ctxt->op) {
   case BLE_GATT_ACCESS_OP_READ_CHR:
       if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
           MODLOG_DFLT(INFO, "Characteristic read; conn_handle=%d attr_handle=%d\n",
                       conn_handle, attr_handle);
       } else {
           MODLOG_DFLT(INFO, "Characteristic read by NimBLE stack; attr_handle=%d\n",
                       attr_handle);
       }
       uuid = ctxt->chr->uuid;
       if(uuid == BLE_UUID_RAS_FEATURES_VAL){
            if (attr_handle == ras_feat_val_handle) {
               rc = os_mbuf_append(ctxt->om,
                                   &ras_feat_val,
                                   sizeof(ras_feat_val));
               return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
           }
       }
       goto unknown;

   case BLE_GATT_ACCESS_OP_WRITE_CHR:
       if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
           MODLOG_DFLT(INFO, "Characteristic write; conn_handle=%d attr_handle=%d",
                       conn_handle, attr_handle);
       } else {
           MODLOG_DFLT(INFO, "Characteristic write by NimBLE stack; attr_handle=%d",
                       attr_handle);
       }
      return 0;

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

int
gatt_svr_init(void)
{
    int rc;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
