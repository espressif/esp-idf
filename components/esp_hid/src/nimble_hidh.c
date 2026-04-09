/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "ble_hidh.h"
#include "esp_private/esp_hidh_private.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"

#include "esp_hid_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include <assert.h>
#include <errno.h>
#include "nimble/nimble_opt.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_hs_adv.h"
#include "host/ble_hs_hci.h"
#include "host/ble_att.h"
#include "host/ble_sm.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "services/bas/ble_svc_bas.h"
#include "services/hid/ble_svc_hid.h"
#include "services/dis/ble_svc_dis.h"

#if CONFIG_BT_NIMBLE_HID_SERVICE

#define ESP_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define ESP_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

static const char *TAG = "NIMBLE_HIDH";
static SemaphoreHandle_t s_ble_hidh_cb_semaphore = NULL;

static SemaphoreHandle_t s_ble_hidh_op_mutex = NULL;
static void (*s_prev_reset_cb)(int reason) = NULL;
static void (*s_prev_sync_cb)(void) = NULL;

#define HIDH_MAX_SVCS   10
#define HIDH_MAX_CHRS   20
#define HIDH_MAX_DSCS   20

/* variables used for attribute discovery */
static int services_discovered;
static int chrs_discovered;
static int dscs_discovered;
static int status = 0;

static int s_gatt_op_status;

static inline void WAIT_CB(void)
{
    xSemaphoreTake(s_ble_hidh_cb_semaphore, portMAX_DELAY);
}

static inline void SEND_CB(void)
{
    xSemaphoreGive(s_ble_hidh_cb_semaphore);
}

static inline void LOCK_OPS(void)
{
    if (s_ble_hidh_op_mutex) {
        xSemaphoreTake(s_ble_hidh_op_mutex, portMAX_DELAY);
    }
}

static inline void UNLOCK_OPS(void)
{
    if (s_ble_hidh_op_mutex) {
        xSemaphoreGive(s_ble_hidh_op_mutex);
    }
}

static esp_event_loop_handle_t event_loop_handle;
static uint8_t *s_read_data_val = NULL;
static uint16_t s_read_data_len = 0;
static int s_read_status = 0;

static void
print_mbuf(const struct os_mbuf *om)
{
    while (om != NULL) {
        ESP_LOG_BUFFER_HEXDUMP(TAG, om->om_data, om->om_len, ESP_LOG_DEBUG);
        om = SLIST_NEXT(om, om_next);
    }
}

static char *nimble_hidh_dup_cstr(const uint8_t *data, size_t len)
{
    char *p;

    if (data == NULL || len == 0) {
        return NULL;
    }
    p = (char *)malloc(len + 1);
    if (p == NULL) {
        return NULL;
    }
    memcpy(p, data, len);
    p[len] = '\0';
    return p;
}

static uint8_t *nimble_hidh_dup_bytes(const uint8_t *data, size_t len)
{
    uint8_t *p;

    if (data == NULL || len == 0) {
        return NULL;
    }
    p = (uint8_t *)malloc(len);
    if (p == NULL) {
        return NULL;
    }
    memcpy(p, data, len);
    return p;
}

static int
nimble_on_read(uint16_t conn_handle,
               const struct ble_gatt_error *error,
               struct ble_gatt_attr *attr,
               void *arg)
{
    int old_offset;

    (void)arg;
    MODLOG_DFLT(INFO, "Read complete; status=%d conn_handle=%d", error->status,
                conn_handle);
    s_read_status = error->status;
    switch (s_read_status) {
    case 0:
        MODLOG_DFLT(DEBUG, " attr_handle=%d value=", attr->handle);
        if ((uint32_t)s_read_data_len + OS_MBUF_PKTLEN(attr->om) > 4096) { // max allowed hid report size
            ESP_LOGE(TAG, "Read data too large, aborting");
            free(s_read_data_val);
            s_read_data_val = NULL;
            s_read_data_len = 0;
            s_read_status = BLE_HS_ENOMEM;
            SEND_CB();
            return -1;
        }
        old_offset = s_read_data_len;
        s_read_data_len += OS_MBUF_PKTLEN(attr->om);
        uint8_t *tmp = realloc(s_read_data_val, s_read_data_len + 1);
        if (tmp == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for read data");
            free(s_read_data_val);
            s_read_data_val = NULL;
            s_read_data_len = 0;
            s_read_status = BLE_HS_ENOMEM;
            SEND_CB();
            return -1;
        }
        s_read_data_val = tmp;
        ble_hs_mbuf_to_flat(attr->om, s_read_data_val + old_offset, OS_MBUF_PKTLEN(attr->om), NULL);
        print_mbuf(attr->om);
        return 0;
    case BLE_HS_EDONE:
        if (s_read_data_val) {
            s_read_data_val[s_read_data_len] = 0; // to ensure strings are ended with \0
        }
        s_read_status = 0;
        SEND_CB();
        return 0;
    default:
        SEND_CB();
        return 0;
    }
}

static int read_char(uint16_t conn_handle, uint16_t handle, uint8_t **out, uint16_t *out_len)
{
    int rc;

    LOCK_OPS();
    s_read_data_val = NULL;
    s_read_data_len = 0;
    s_read_status = 0;

    /* read long because the server may not support the large enough mtu */
    rc = ble_gattc_read_long(conn_handle, handle, 0, nimble_on_read, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "read_char failed");
        UNLOCK_OPS();
        return rc;
    }
    WAIT_CB();
    if (s_read_status == 0) {
        *out = s_read_data_val;
        *out_len = s_read_data_len;
    } else {
        free(s_read_data_val);
        s_read_data_val = NULL;
    }
    rc = s_read_status;
    UNLOCK_OPS();
    return rc;
}

static int read_descr(uint16_t conn_handle, uint16_t handle, uint8_t **out, uint16_t *out_len)
{
    int rc;

    LOCK_OPS();
    s_read_data_val = NULL;
    s_read_data_len = 0;
    s_read_status = 0;

    rc = ble_gattc_read_long(conn_handle, handle, 0, nimble_on_read, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "read_descr failed");
        UNLOCK_OPS();
        return rc;
    }
    WAIT_CB();
    if (s_read_status == 0) {
        *out = s_read_data_val;
        *out_len = s_read_data_len;
    } else {
        free(s_read_data_val);
        s_read_data_val = NULL;
    }
    rc = s_read_status;
    UNLOCK_OPS();
    return rc;
}

static int
svc_disced(uint16_t conn_handle, const struct ble_gatt_error *error,
           const struct ble_gatt_svc *service, void *arg)
{
    int rc;
    struct ble_gatt_svc *service_result;
    uint16_t uuid16;
    esp_hidh_dev_t *dev;

    service_result = arg;
    rc = 0;
    status = error->status;
    switch (error->status) {
    case 0:
        if (services_discovered >= 10) {
            ESP_LOGE(TAG, "Too many services, ignoring");
            break;
        }
        memcpy(service_result + services_discovered, service, sizeof(struct ble_gatt_svc));
        services_discovered++;
        uuid16 = ble_uuid_u16(&service->uuid.u);
        dev = esp_hidh_dev_get_by_conn_id(conn_handle);
        if (!dev) {
            ESP_LOGE(TAG, "Service discovery received for unknown device");
            break;
        }
        if (uuid16 == BLE_SVC_HID_UUID16) {
            dev->status = 0;
            dev->config.report_maps_len++;
            ESP_LOGD(TAG, "HID Service is Discovered");
        }
        break;

    case BLE_HS_EDONE:
        rc = 0;
        status = 0;
        /* release the sem now */
        SEND_CB();
        break;

    default:
        rc = error->status;
        break;
    }

    if (rc != 0) {
        /* Error; abort discovery. */
        SEND_CB();
        ESP_LOGE(TAG, "Error in service discovery %d\n", rc);
    }

    return rc;
}

static int
chr_disced(uint16_t conn_handle, const struct ble_gatt_error *error,
           const struct ble_gatt_chr *chr, void *arg)
{
    struct ble_gatt_chr *chrs;
    int rc;
    rc = 0;
    chrs = arg;

    status = error->status;
    switch (error->status) {
    case 0:
        ESP_LOGD(TAG, "Char discovered : def handle : %04x, val_handle : %04x, properties : %02x\n, uuid : %04x",
                 chr->def_handle, chr->val_handle, chr->properties, ble_uuid_u16(&chr->uuid.u));
        if (chrs_discovered >= 20) {
            ESP_LOGE(TAG, "Too many characteristics, ignoring");
            break;
        }
        memcpy(chrs + chrs_discovered, chr, sizeof(struct ble_gatt_chr));
        chrs_discovered++;
        break;

    case BLE_HS_EDONE:
        status = 0;
        /* release when discovery is complete */
        SEND_CB();
        rc = 0;
        break;

    default:
        rc = error->status;
        break;
    }

    if (rc != 0) {
        /* Error; abort discovery. */
        SEND_CB();
    }

    return rc;
}

static int
desc_disced(uint16_t conn_handle, const struct ble_gatt_error *error,
            uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
            void *arg)
{
    int rc;
    struct ble_gatt_dsc *dscr;
    dscr = arg;

    rc = 0;
    status = error->status;
    switch (error->status) {
    case 0:
        ESP_LOGD(TAG, "DISC discovered : handle : %04x, uuid : %04x",
                 dsc->handle, ble_uuid_u16(&dsc->uuid.u));
        if (dscs_discovered >= 20) {
            ESP_LOGE(TAG, "Too many descriptors, ignoring");
            break;
        }
        memcpy(dscr + dscs_discovered, dsc, sizeof(struct ble_gatt_dsc));
        dscs_discovered++;
        break;

    case BLE_HS_EDONE:
        /* All descriptors in this characteristic discovered */
        rc = 0;
        status = 0;
        /*  release the sem as the discovery is complete */
        SEND_CB();
        break;

    default:
        /* Error; abort discovery. */
        rc = error->status;
        break;
    }

    if (rc != 0) {
        /* Error; abort discovery. */
        SEND_CB();
    }

    return rc;
}

/* this api does the following things :
** does service, characteristic and descriptor discovery and
** fills the hid device information accordingly in dev */
static void read_device_services(esp_hidh_dev_t *dev)
{
    uint16_t suuid, cuuid, duuid;
    uint16_t chandle, dhandle;
    esp_hidh_dev_report_t *report = NULL;
    uint8_t *rdata = NULL;
    uint16_t rlen = 0;
    esp_hid_report_item_t *r;
    esp_hid_report_map_t *map;
    struct ble_gatt_svc service_result[HIDH_MAX_SVCS];
    uint16_t svc_count;
    uint8_t hidindex = 0;
    int rc;

    services_discovered = 0;
    esp_hidh_dev_lock(dev);
    rc = ble_gattc_disc_all_svcs(dev->ble.conn_id, svc_disced, service_result);
    if (rc != 0) {
        ESP_LOGD(TAG, "Error discovering services : %d", rc);
        goto done;
    }
    WAIT_CB();
    if (status != 0) {
        ESP_LOGE(TAG, "failed to find services");
        goto done;
    }

    svc_count = services_discovered;
    if (svc_count > HIDH_MAX_SVCS) {
        ESP_LOGE(TAG, "Too many services %u (max %d)", svc_count, HIDH_MAX_SVCS);
        goto done;
    }

    ESP_LOGD(TAG, "Found %u HID Services", dev->config.report_maps_len);
    if (dev->config.report_maps_len > 0) {
        dev->config.report_maps = (esp_hid_raw_report_map_t *)calloc(dev->config.report_maps_len,
                                                                     sizeof(esp_hid_raw_report_map_t));
        if (dev->config.report_maps == NULL) {
            ESP_LOGE(TAG, "malloc report maps failed");
            goto done;
        }
        dev->protocol_mode = (uint8_t *)calloc(dev->config.report_maps_len, sizeof(uint8_t));
        if (dev->protocol_mode == NULL) {
            ESP_LOGE(TAG, "malloc protocol_mode failed");
            free(dev->config.report_maps);
            dev->config.report_maps = NULL;
            goto done;
        }
    }

    for (uint16_t s = 0; s < svc_count; s++) {
        suuid = ble_uuid_u16(&service_result[s].uuid.u);
        ESP_LOGD(TAG, "Service discovered : start_handle : %d, end handle : %d, uuid: 0x%04x",
                 service_result[s].start_handle, service_result[s].end_handle, suuid);

        if (suuid != BLE_SVC_BAS_UUID16
                && suuid != BLE_SVC_DIS_UUID16
                && suuid != BLE_SVC_HID_UUID16
                && suuid != 0x1800) { /* device name? */
            continue;
        }

        struct ble_gatt_chr char_result[HIDH_MAX_CHRS];
        uint16_t ccount = HIDH_MAX_CHRS;
        rc = ble_gattc_disc_all_chrs(dev->ble.conn_id, service_result[s].start_handle,
                                     service_result[s].end_handle, chr_disced, char_result);
        if (rc != 0) {
            ESP_LOGE(TAG, "failed to start char discovery for service : %d, rc=%d", s, rc);
            chrs_discovered = 0;
            continue;
        }
        WAIT_CB();
        if (status != 0) {
            ESP_LOGE(TAG, "failed to find chars for service : %u", s);
            chrs_discovered = 0;
            continue;
        }
        ccount = chrs_discovered;
        if (ccount > HIDH_MAX_CHRS) {
            ESP_LOGE(TAG, "Too many characteristics %u (max %d)", ccount, HIDH_MAX_CHRS);
            chrs_discovered = 0;
            continue;
        }
        if (rc == 0) {
            for (uint16_t c = 0; c < ccount; c++) {
                cuuid = ble_uuid_u16(&char_result[c].uuid.u);
                chandle = char_result[c].val_handle;
                ESP_LOGD(TAG, "  CHAR:(%d), handle: %d, perm: 0x%02x, uuid: 0x%04x",
                         c + 1, chandle, char_result[c].properties, cuuid);
                if (suuid == BLE_SVC_GAP_UUID16) {
                    if (dev->config.device_name == NULL && cuuid == BLE_SVC_GAP_CHR_UUID16_DEVICE_NAME
                            && (char_result[c].properties & BLE_GATT_CHR_PROP_READ) != 0) {
                        if (read_char(dev->ble.conn_id, chandle, &rdata, &rlen) == 0 && rlen) {
                            char *dn = nimble_hidh_dup_cstr(rdata, rlen);
                            if (dn) {
                                free((void *)dev->config.device_name);
                                dev->config.device_name = dn;
                            }
                        }
                    } else {
                        continue;
                    }
                } else if (suuid == BLE_SVC_BAS_UUID16) {
                    if (cuuid == BLE_SVC_BAS_CHR_UUID16_BATTERY_LEVEL &&
                            (char_result[c].properties & BLE_GATT_CHR_PROP_READ) != 0) {
                        dev->ble.battery_handle = chandle;
                    } else {
                        continue;
                    }
                } else if (suuid == BLE_SVC_DIS_UUID16) {
                    if (char_result[c].properties & BLE_GATT_CHR_PROP_READ) {
                        if (cuuid == BLE_SVC_DIS_CHR_UUID16_PNP_ID) {
                            if (read_char(dev->ble.conn_id, chandle, &rdata, &rlen) == 0 && rlen == 7) {
                                dev->config.vendor_id = *((uint16_t *)&rdata[1]);
                                dev->config.product_id = *((uint16_t *)&rdata[3]);
                                dev->config.version = *((uint16_t *)&rdata[5]);
                            }
                        } else if (cuuid == BLE_SVC_DIS_CHR_UUID16_MANUFACTURER_NAME) {
                            if (read_char(dev->ble.conn_id, chandle, &rdata, &rlen) == 0 && rlen) {
                                char *mn = nimble_hidh_dup_cstr(rdata, rlen);
                                if (mn) {
                                    free((void *)dev->config.manufacturer_name);
                                    dev->config.manufacturer_name = mn;
                                }
                            }
                        } else if (cuuid == BLE_SVC_DIS_CHR_UUID16_SERIAL_NUMBER) {
                            if (read_char(dev->ble.conn_id, chandle, &rdata, &rlen) == 0 && rlen) {
                                char *sn = nimble_hidh_dup_cstr(rdata, rlen);
                                if (sn) {
                                    free((void *)dev->config.serial_number);
                                    dev->config.serial_number = sn;
                                }
                            }
                        }
                        continue;
                    } else {
                        if (cuuid == BLE_SVC_HID_CHR_UUID16_PROTOCOL_MODE) {
                            if (char_result[c].properties & BLE_GATT_CHR_PROP_READ) {
                                if (read_char(dev->ble.conn_id, chandle, &rdata, &rlen) == 0 && rlen) {
                                    dev->protocol_mode[hidindex] = *((uint8_t *)rdata);
                                    free(rdata);
                                    rdata = NULL;
                                }
                            }
                        }
                        continue;
                    }
                    if (cuuid == BLE_SVC_HID_CHR_UUID16_REPORT_MAP) {
                        if (char_result[c].properties & BLE_GATT_CHR_PROP_READ) {
                            if (read_char(dev->ble.conn_id, chandle, &rdata, &rlen) == 0 && rlen) {
                                uint8_t *copy = nimble_hidh_dup_bytes(rdata, rlen);
                                if (copy) {
                                    free((void *)dev->config.report_maps[hidindex].data);
                                    dev->config.report_maps[hidindex].data = copy;
                                    dev->config.report_maps[hidindex].len = rlen;
                                }
                            }
                            continue;
                        } else if (cuuid == BLE_SVC_HID_CHR_UUID16_BOOT_KBD_INP || cuuid == BLE_SVC_HID_CHR_UUID16_BOOT_KBD_OUT
                                   || cuuid == BLE_SVC_HID_CHR_UUID16_BOOT_MOUSE_INP || cuuid == BLE_SVC_HID_CHR_UUID16_RPT) {
                            report = (esp_hidh_dev_report_t *)malloc(sizeof(esp_hidh_dev_report_t));
                            if (report == NULL) {
                                ESP_LOGE(TAG, "malloc esp_hidh_dev_report_t failed");
                                goto done;
                            }
                            report->next = NULL;
                            report->permissions = char_result[c].properties;
                            report->handle = chandle;
                            report->ccc_handle = 0;
                            report->report_id = 0;
                            report->map_index = hidindex;
                            if (cuuid == BLE_SVC_HID_CHR_UUID16_BOOT_KBD_INP) {
                                report->protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
                                report->report_type = ESP_HID_REPORT_TYPE_INPUT;
                                report->usage = ESP_HID_USAGE_KEYBOARD;
                                report->value_len = 8;
                            } else if (cuuid == BLE_SVC_HID_CHR_UUID16_BOOT_KBD_OUT) {
                                report->protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
                                report->report_type = ESP_HID_REPORT_TYPE_OUTPUT;
                                report->usage = ESP_HID_USAGE_KEYBOARD;
                                report->value_len = 8;
                            } else if (cuuid == BLE_SVC_HID_CHR_UUID16_BOOT_MOUSE_INP) {
                                report->protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
                                report->report_type = ESP_HID_REPORT_TYPE_INPUT;
                                report->usage = ESP_HID_USAGE_MOUSE;
                                report->value_len = 8;
                            } else {
                                report->protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT;
                                report->report_type = 0;
                                report->usage = ESP_HID_USAGE_GENERIC;
                                report->value_len = 0;
                            }
                        } else {
                            report->protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT;
                            report->report_type = 0;
                            report->usage = ESP_HID_USAGE_GENERIC;
                            report->value_len = 0;
                        }
                    } else {
                        continue;
                    }
                }
                struct ble_gatt_dsc descr_result[HIDH_MAX_DSCS];
                uint16_t num_dsc = HIDH_MAX_DSCS;
                uint16_t chr_end_handle;
                if (c + 1 < ccount) {
                    chr_end_handle = char_result[c + 1].def_handle - 1;
                } else {
                    chr_end_handle = service_result[s].end_handle;
                }
                if (chr_end_handle <= char_result[c].val_handle) {
                    ESP_LOGD(TAG, "skip descriptor discovery for val_handle=%u end_handle=%u",
                             char_result[c].val_handle, chr_end_handle);
                    if (suuid == BLE_SVC_HID_UUID16 && report != NULL) {
                        report->next = dev->reports;
                        dev->reports = report;
                        dev->reports_len++;
                    }
                    dscs_discovered = 0;
                    continue;
                }

                /*
                 * NimBLE expects the characteristic value handle here; it
                 * discovers descriptors in the range (chr_val_handle, end].
                 */
                rc = ble_gattc_disc_all_dscs(dev->ble.conn_id, char_result[c].val_handle,
                                             chr_end_handle, desc_disced, descr_result);
                if (rc != 0) {
                    ESP_LOGD(TAG, "descriptor discovery not started for characteristic : %d, rc=%d", c, rc);
                    if (suuid == BLE_SVC_HID_UUID16 && report != NULL) {
                        report->next = dev->reports;
                        dev->reports = report;
                        dev->reports_len++;
                    }
                    dscs_discovered = 0;
                    continue;
                }
                WAIT_CB();
                if (status != 0) {
                    ESP_LOGE(TAG, "failed to find descriptors for characteristic : %u", c);
                    if (suuid == BLE_SVC_HID_UUID16 && report != NULL) {
                        report->next = dev->reports;
                        dev->reports = report;
                        dev->reports_len++;
                    }
                    dscs_discovered = 0;
                    continue;
                }
                num_dsc = dscs_discovered;
                if (num_dsc > HIDH_MAX_DSCS) {
                    ESP_LOGE(TAG, "Too many descriptors %u (max %d)", num_dsc, HIDH_MAX_DSCS);
                    if (suuid == BLE_SVC_HID_UUID16 && report != NULL) {
                        report->next = dev->reports;
                        dev->reports = report;
                        dev->reports_len++;
                    }
                    dscs_discovered = 0;
                    continue;
                }
                if (rc == ESP_OK) {
                    for (uint16_t d = 0; d < num_dsc; d++) {
                        duuid = ble_uuid_u16(&descr_result[d].uuid.u);
                        dhandle = descr_result[d].handle;
                        ESP_LOGD(TAG, "    DESCR:(%d), handle: %d, uuid: 0x%04x", d + 1, dhandle, duuid);

                        if (suuid == BLE_SVC_BAS_UUID16) {
                            if (duuid == BLE_GATT_DSC_CLT_CFG_UUID16 &&
                                    (char_result[c].properties & BLE_GATT_CHR_PROP_NOTIFY) != 0) {
                                dev->ble.battery_ccc_handle = dhandle;
                            }
                        } else if (suuid == BLE_SVC_HID_UUID16 && report != NULL) {
                            if (duuid == BLE_GATT_DSC_CLT_CFG_UUID16 && (report->permissions & BLE_GATT_CHR_PROP_NOTIFY) != 0) {
                                report->ccc_handle = dhandle;
                            } else if (duuid == BLE_SVC_HID_DSC_UUID16_RPT_REF) {
                                if (read_descr(dev->ble.conn_id, dhandle, &rdata, &rlen) == 0 && rlen >= 2) {
                                    report->report_id = rdata[0];
                                    report->report_type = rdata[1];
                                    free(rdata);
                                }
                            }
                        }
                    }
                }
                if (suuid == BLE_SVC_HID_UUID16 && report != NULL) {
                    report->next = dev->reports;
                    dev->reports = report;
                    dev->reports_len++;
                }
                dscs_discovered = 0;
            }
            if (suuid == BLE_SVC_HID_UUID16) {
                hidindex++;
            }
        }
        chrs_discovered = 0;
    }

    for (uint8_t d = 0; d < dev->config.report_maps_len; d++) {
        if (dev->reports_len && dev->config.report_maps[d].len && dev->config.report_maps[d].data) {
            map = esp_hid_parse_report_map(dev->config.report_maps[d].data, dev->config.report_maps[d].len);
            if (map) {
                if (dev->ble.appearance == 0) {
                    dev->ble.appearance = map->appearance;
                }
                report = dev->reports;

                while (report) {
                    if (report->map_index == d) {
                        for (uint8_t i = 0; i < map->reports_len; i++) {
                            r = &map->reports[i];
                            if (report->protocol_mode == ESP_HID_PROTOCOL_MODE_BOOT
                                    && report->protocol_mode == r->protocol_mode
                                    && report->report_type == r->report_type
                                    && report->usage == r->usage) {
                                report->report_id = r->report_id;
                                report->value_len = r->value_len;
                            } else if (report->protocol_mode == r->protocol_mode
                                       && report->report_type == r->report_type
                                       && report->report_id == r->report_id) {
                                report->usage = r->usage;
                                report->value_len = r->value_len;
                            }
                        }
                    }
                    report = report->next;
                }
                free(map->reports);
                free(map);
                map = NULL;
            }
        }
    }
done:
    esp_hidh_dev_unlock(dev);
}

static void register_for_notify(uint16_t conn_handle, uint16_t handle)
{
    /* NimBLE notifications are enabled via CCC descriptor writes. */
    (void)conn_handle;
    (void)handle;
}

static int
on_write(uint16_t conn_handle,
         const struct ble_gatt_error *error,
         struct ble_gatt_attr *attr,
         void *arg)
{
    esp_hidh_dev_t *dev = (esp_hidh_dev_t *)arg;

    if (dev == NULL) {
        s_gatt_op_status = BLE_HS_EAPP;
        SEND_CB();
        return 0;
    }
    if (esp_hidh_dev_get_by_conn_id(conn_handle) != dev) {
        s_gatt_op_status = BLE_HS_ENOTCONN;
        SEND_CB();
        return 0;
    }

    MODLOG_DFLT(DEBUG, "write complete; status=%d conn_handle=%d "
                "attr_handle=%d\n",
                error->status, conn_handle, attr->handle);
    s_gatt_op_status = error->status;
    SEND_CB();

    return 0;
}

static void write_char_descr(esp_hidh_dev_t *dev, uint16_t handle, uint16_t value_len, uint8_t *value)
{
    int rc;

    if (dev == NULL || dev->ble.conn_id < 0) {
        return;
    }
    s_gatt_op_status = 0;
    rc = ble_gattc_write_flat(dev->ble.conn_id, handle, value, value_len, on_write, dev);
    if (rc != 0) {
        ESP_LOGE(TAG, "write_char_descr failed rc=%d", rc);
        return;
    }
    WAIT_CB();
    if (s_gatt_op_status != 0) {
        ESP_LOGE(TAG, "GATT write failed status=%d", s_gatt_op_status);
    }
}

static void attach_report_listeners(esp_hidh_dev_t *dev)
{
    uint16_t ccc_data = 1;
    esp_hidh_dev_report_t *report;

    if (dev == NULL) {
        return;
    }

    LOCK_OPS();
    if (dev->ble.conn_id < 0 || !dev->connected) {
        UNLOCK_OPS();
        return;
    }

    report = dev->reports;

    if (dev->ble.battery_handle) {
        register_for_notify(dev->ble.conn_id, dev->ble.battery_handle);
        if (dev->ble.battery_ccc_handle && dev->ble.conn_id >= 0 && dev->connected) {
            write_char_descr(dev, dev->ble.battery_ccc_handle, 2, (uint8_t *)&ccc_data);
        }
    }

    while (report) {
        if (dev->ble.conn_id < 0 || !dev->connected) {
            break;
        }
        if ((report->permissions & BLE_GATT_CHR_PROP_NOTIFY) != 0 && report->protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {
            register_for_notify(dev->ble.conn_id, report->handle);
            if (report->ccc_handle && dev->ble.conn_id >= 0 && dev->connected) {
                write_char_descr(dev, report->ccc_handle, 2, (uint8_t *)&ccc_data);
            }
        }
        report = report->next;
    }
    UNLOCK_OPS();
}

static int
esp_hidh_gattc_event_handler(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;
    esp_hidh_dev_t *dev = NULL;
    esp_hidh_dev_report_t *report = NULL;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        dev = (esp_hidh_dev_t *)arg;
        if (event->connect.status == 0) {
            /* Connection successfully established. */
            MODLOG_DFLT(INFO, "Connection established ");

            if (dev == NULL) {
                ESP_LOGE(TAG, "Connect success with NULL device context");
                SEND_CB();
                return 0;
            }

            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            if (rc != 0) {
                ESP_LOGE(TAG, "ble_gap_conn_find failed; rc=%d", rc);
                dev->status = rc;
                dev->ble.conn_id = -1;
                SEND_CB();
                return 0;
            }

            if (memcmp(desc.peer_ota_addr.val, dev->addr.bda, sizeof(dev->addr.bda)) != 0 &&
                    memcmp(desc.peer_id_addr.val, dev->addr.bda, sizeof(dev->addr.bda)) != 0) {
                ESP_LOGW(TAG, "Connect address mismatch vs open request");
            }

            dev->status = -1; /* not found until HID service is discovered */
            dev->ble.conn_id = event->connect.conn_handle;

            /* Try to set the mtu to the max value */
            rc = ble_att_set_preferred_mtu(BLE_ATT_MTU_MAX);
            if (rc != 0) {
                ESP_LOGE(TAG, "att preferred mtu set failed");
            }
            rc = ble_gattc_exchange_mtu(event->connect.conn_handle, NULL, NULL);
            if (rc != 0) {
                ESP_LOGE(TAG, "Failed to negotiate MTU; rc = %d", rc);
            }
            rc = ble_gap_security_initiate(event->connect.conn_handle);
            if (rc != 0) {
                ESP_LOGW(TAG, "Failed to initiate security; rc = %d", rc);
            }
            SEND_CB();
        } else {
            MODLOG_DFLT(ERROR, "Error: Connection failed; status=%d\n",
                        event->connect.status);
            if (dev == NULL) {
                ESP_LOGE(TAG, "Connect failed with NULL device context");
                SEND_CB();
                return 0;
            }
            dev->status = event->connect.status;
            dev->ble.conn_id = -1;
            SEND_CB();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        /* Connection terminated. */
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        dev = esp_hidh_dev_get_by_conn_id(event->disconnect.conn.conn_handle);
        if (!dev) {
            ESP_LOGE(TAG, "CLOSE received for unknown device");
            break;
        }
        if (!dev->connected) {
            dev->status = event->disconnect.reason;
            dev->ble.conn_id = -1;
        } else {
            dev->connected = false;
            dev->status = event->disconnect.reason;
            // free the device in the wrapper event handler
            dev->in_use = false;
            if (event_loop_handle) {
                esp_hidh_event_data_t p = {0};
                p.close.dev = dev;
                p.close.reason = event->disconnect.reason;
                p.close.status = ESP_OK;
                esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_CLOSE_EVENT, &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
            } else {
                esp_hidh_dev_free_inner(dev);
            }
        }

        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        /* Peer sent us a notification or indication. */
        MODLOG_DFLT(DEBUG, "received %s; conn_handle=%d attr_handle=%d "
                    "attr_len=%d\n",
                    event->notify_rx.indication ?
                    "indication" :
                    "notification",
                    event->notify_rx.conn_handle,
                    event->notify_rx.attr_handle,
                    OS_MBUF_PKTLEN(event->notify_rx.om));

        /* Attribute data is contained in event->notify_rx.om. Use
         * `os_mbuf_copydata` to copy the data received in notification mbuf */

        dev = esp_hidh_dev_get_by_conn_id(event->notify_rx.conn_handle);
        if (!dev) {
            ESP_LOGE(TAG, "NOTIFY received for unknown device");
            break;
        }
        rc = ble_gap_conn_find(event->notify_rx.conn_handle, &desc);
        if (rc != 0) {
            ESP_LOGW(TAG, "Dropping notification: unknown link");
            return 0;
        }
        /* Enforce encrypted notifications only for strict security policy. */
        if ((ble_hs_cfg.sm_mitm || ble_hs_cfg.sm_sc_only) && !desc.sec_state.encrypted) {
            ESP_LOGW(TAG, "Dropping notification on unencrypted link");
            return 0;
        }
        if (event_loop_handle) {
            esp_hidh_event_data_t p = {0};
            if (event->notify_rx.attr_handle == dev->ble.battery_handle) {
                p.battery.dev = dev;
                ble_hs_mbuf_to_flat(event->notify_rx.om, &p.battery.level, sizeof(p.battery.level), NULL);
                esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_BATTERY_EVENT,
                                  &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
            } else {
                report = esp_hidh_dev_get_report_by_handle(dev, event->notify_rx.attr_handle);
                if (report) {
                    esp_hidh_event_data_t *p_param = NULL;
                    size_t event_data_size = sizeof(esp_hidh_event_data_t);

                    if (dev->protocol_mode == NULL ||
                            report->protocol_mode != dev->protocol_mode[report->map_index]) {
                        /* only pass the notifications in the current protocol mode */
                        ESP_LOGD(TAG, "Wrong protocol mode, dropping notification");
                        return 0;
                    }
                    if (OS_MBUF_PKTLEN(event->notify_rx.om)) {
                        event_data_size += OS_MBUF_PKTLEN(event->notify_rx.om);
                    }

                    if ((p_param = (esp_hidh_event_data_t *)malloc(event_data_size)) == NULL) {
                        ESP_LOGE(TAG, "%s malloc event data failed!", __func__);
                        break;
                    }
                    memset(p_param, 0, event_data_size);
                    if (OS_MBUF_PKTLEN(event->notify_rx.om) && event->notify_rx.om) {
                        ble_hs_mbuf_to_flat(event->notify_rx.om, ((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t),
                                            OS_MBUF_PKTLEN(event->notify_rx.om), NULL);
                    }

                    if (report->report_type == ESP_HID_REPORT_TYPE_FEATURE) {
                        p_param->feature.dev = dev;
                        p_param->feature.map_index = report->map_index;
                        p_param->feature.report_id = report->report_id;
                        p_param->feature.usage = report->usage;
                        p_param->feature.length = OS_MBUF_PKTLEN(event->notify_rx.om);
                        p_param->feature.data = ((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t);
                        esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_FEATURE_EVENT, p_param, event_data_size, portMAX_DELAY);
                    } else {
                        p_param->input.dev = dev;
                        p_param->input.map_index = report->map_index;
                        p_param->input.report_id = report->report_id;
                        p_param->input.usage = report->usage;
                        p_param->input.length = OS_MBUF_PKTLEN(event->notify_rx.om);
                        p_param->input.data = ((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t);
                        esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_INPUT_EVENT, p_param, event_data_size, portMAX_DELAY);
                    }

                    if (p_param) {
                        free(p_param);
                        p_param = NULL;
                    }
                }
            }
        }
        break;
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        if (rc != 0) {
            ESP_LOGW(TAG, "ENC_CHANGE for unknown connection");
            return 0;
        }
        ESP_LOGI(TAG, "ENC_CHANGE status=%d encrypted=%d authenticated=%d bonded=%d",
                 event->enc_change.status, desc.sec_state.encrypted,
                 desc.sec_state.authenticated, desc.sec_state.bonded);
        return 0;

    case BLE_GAP_EVENT_PASSKEY_ACTION:
        switch (event->passkey.params.action) {
        case BLE_SM_IOACT_NUMCMP: {
            struct ble_sm_io pkey = {
                .action = BLE_SM_IOACT_NUMCMP,
                .numcmp_accept = 1,
            };
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(TAG, "PASSKEY NUMCMP auto-accept rc=%d", rc);
            return 0;
        }
        case BLE_SM_IOACT_DISP:
            ESP_LOGI(TAG, "PASSKEY display requested");
            return 0;
        case BLE_SM_IOACT_INPUT: {
            struct ble_sm_io pkey = {
                .action = BLE_SM_IOACT_INPUT,
                .passkey = 123456,
            };
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(TAG, "PASSKEY INPUT injected rc=%d", rc);
            return 0;
        }
        default:
            ESP_LOGW(TAG, "PASSKEY action=%u", event->passkey.params.action);
            return 0;
        }

    case BLE_GAP_EVENT_MTU:
        MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.channel_id,
                    event->mtu.value);
        return 0;
    default:
        return 0;
    }
    return 0;
}

/*
 * Public Functions
 * */

static esp_err_t esp_ble_hidh_dev_close(esp_hidh_dev_t *dev)
{
    if (dev == NULL || dev->ble.conn_id < 0) {
        return ESP_FAIL;
    }
    return ble_gap_terminate(dev->ble.conn_id, BLE_ERR_REM_USER_CONN_TERM);
}

static esp_err_t esp_ble_hidh_dev_report_write(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, uint8_t *value, size_t value_len)
{
    int rc;
    esp_hidh_dev_report_t *report = esp_hidh_dev_get_report_by_id_and_type(dev, map_index, report_id, report_type);
    if (!report) {
        ESP_LOGE(TAG, "%s report %d not found", esp_hid_report_type_str(report_type), report_id);
        return ESP_FAIL;
    }
    if (value_len > report->value_len) {
        ESP_LOGE(TAG, "%s report %d takes maximum %d bytes. you have provided %d", esp_hid_report_type_str(report_type), report_id, report->value_len, value_len);
        return ESP_FAIL;
    }
    LOCK_OPS();
    s_gatt_op_status = 0;
    rc = ble_gattc_write_flat(dev->ble.conn_id, report->handle, value, value_len, on_write, dev);
    if (rc != 0) {
        WAIT_CB();
        rc = s_gatt_op_status;
    }
    WAIT_CB();
    rc = s_gatt_op_status;
    UNLOCK_OPS();
    if (rc != 0) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t esp_ble_hidh_dev_report_read(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, size_t max_length, uint8_t *value, size_t *value_len)
{
    esp_hidh_dev_report_t *report = esp_hidh_dev_get_report_by_id_and_type(dev, map_index, report_id, report_type);
    if (!report) {
        ESP_LOGE(TAG, "%s report %d not found", esp_hid_report_type_str(report_type), report_id);
        return ESP_FAIL;
    }
    uint16_t len = max_length;
    uint8_t *v = NULL;
    LOCK_OPS();
    int s = read_char(dev->ble.conn_id, report->handle, &v, &len);
    UNLOCK_OPS();
    if (s == 0) {
        if (len > max_length) {
            len = max_length;
        }
        *value_len = len;
        memcpy(value, v, len);
        free(v);
        return ESP_OK;
    }
    ESP_LOGE(TAG, "%s report %d read failed: 0x%x", esp_hid_report_type_str(report_type), report_id, s);
    return ESP_FAIL;
}

static void esp_ble_hidh_dev_dump(esp_hidh_dev_t *dev, FILE *fp)
{
    fprintf(fp, "BDA:" ESP_BD_ADDR_STR ", Appearance: 0x%04x, Connection ID: %d\n", ESP_BD_ADDR_HEX(dev->addr.bda),
            dev->ble.appearance, dev->ble.conn_id);
    fprintf(fp, "Name: %s, Manufacturer: %s, Serial Number: %s\n", dev->config.device_name ? dev->config.device_name : "",
            dev->config.manufacturer_name ? dev->config.manufacturer_name : "",
            dev->config.serial_number ? dev->config.serial_number : "");
    fprintf(fp, "PID: 0x%04x, VID: 0x%04x, VERSION: 0x%04x\n", dev->config.product_id, dev->config.vendor_id, dev->config.version);
    fprintf(fp, "Battery: Handle: %u, CCC Handle: %u\n", dev->ble.battery_handle, dev->ble.battery_ccc_handle);
    fprintf(fp, "Report Maps: %d\n", dev->config.report_maps_len);
    if (dev->config.report_maps_len > 0 && dev->config.report_maps == NULL) {
        fprintf(fp, "  (report_maps allocation missing)\n");
        return;
    }
    for (uint8_t d = 0; d < dev->config.report_maps_len; d++) {
        fprintf(fp, "  Report Map Length: %d\n", dev->config.report_maps[d].len);
        esp_hidh_dev_report_t *report = dev->reports;
        while (report) {
            if (report->map_index == d) {
                fprintf(fp, "    %8s %7s %6s, ID: %2u, Length: %3u, Permissions: 0x%02x, Handle: %3u, CCC Handle: %3u\n",
                        esp_hid_usage_str(report->usage), esp_hid_report_type_str(report->report_type),
                        esp_hid_protocol_mode_str(report->protocol_mode), report->report_id, report->value_len,
                        report->permissions, report->handle, report->ccc_handle);
            }
            report = report->next;
        }
    }

}

static void nimble_host_synced(void)
{
    if (s_prev_sync_cb && s_prev_sync_cb != nimble_host_synced) {
        s_prev_sync_cb();
    }
    /*
        no need to perform anything here
    */
}

static void nimble_host_reset(int reason)
{
    if (s_prev_reset_cb && s_prev_reset_cb != nimble_host_reset) {
        s_prev_reset_cb(reason);
    }
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);

    if (s_ble_hidh_cb_semaphore) {
        xSemaphoreGive(s_ble_hidh_cb_semaphore);
    }
}

esp_err_t esp_ble_hidh_init(const esp_hidh_config_t *config)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "Config is NULL");
    ESP_RETURN_ON_FALSE(!s_ble_hidh_cb_semaphore, ESP_ERR_INVALID_STATE, TAG, "Already initialized");

    event_loop_handle = esp_hidh_get_event_loop();
    s_ble_hidh_cb_semaphore = xSemaphoreCreateBinary();
    ESP_RETURN_ON_FALSE(s_ble_hidh_cb_semaphore,
                        ESP_ERR_NO_MEM, TAG, "Allocation failed");
    s_ble_hidh_op_mutex = xSemaphoreCreateMutex();
    if (s_ble_hidh_op_mutex == NULL) {
        vSemaphoreDelete(s_ble_hidh_cb_semaphore);
        s_ble_hidh_cb_semaphore = NULL;
        return ESP_ERR_NO_MEM;
    }

    s_prev_reset_cb = ble_hs_cfg.reset_cb;
    s_prev_sync_cb = ble_hs_cfg.sync_cb;
    /* Enforce secure pairing defaults for HID traffic. */
    ble_hs_cfg.sm_io_cap = BLE_HS_IO_KEYBOARD_ONLY;
    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_sc = 1;
    ble_hs_cfg.reset_cb = nimble_host_reset;
    ble_hs_cfg.sync_cb = nimble_host_synced;
    return ret;
}

esp_err_t esp_ble_hidh_deinit(void)
{
    ESP_RETURN_ON_FALSE(s_ble_hidh_cb_semaphore, ESP_ERR_INVALID_STATE, TAG, "Already deinitialized");

    free(s_read_data_val);
    s_read_data_val = NULL;
    s_read_data_len = 0;
    s_read_status = 0;

    if (s_ble_hidh_cb_semaphore) {
        vSemaphoreDelete(s_ble_hidh_cb_semaphore);
        s_ble_hidh_cb_semaphore = NULL;
    }
    if (s_ble_hidh_op_mutex) {
        vSemaphoreDelete(s_ble_hidh_op_mutex);
        s_ble_hidh_op_mutex = NULL;
    }
    if (ble_hs_cfg.reset_cb == nimble_host_reset) {
        ble_hs_cfg.reset_cb = s_prev_reset_cb;
    }
    if (ble_hs_cfg.sync_cb == nimble_host_synced) {
        ble_hs_cfg.sync_cb = s_prev_sync_cb;
    }

    return ESP_OK;
}

esp_hidh_dev_t *esp_ble_hidh_dev_open(uint8_t *bda, uint8_t address_type)
{
    esp_err_t ret;
    ble_addr_t addr;
    uint8_t own_addr_type;

    own_addr_type = 0; // set to public for now
    esp_hidh_dev_t *dev = esp_hidh_dev_malloc();
    if (dev == NULL) {
        ESP_LOGE(TAG, "malloc esp_hidh_dev_t failed");
        return NULL;
    }

    dev->in_use = true;
    dev->transport = ESP_HID_TRANSPORT_BLE;
    memcpy(dev->addr.bda, bda, sizeof(dev->addr.bda));
    dev->ble.address_type = address_type;
    dev->ble.appearance = ESP_HID_APPEARANCE_GENERIC;

    memcpy(addr.val, bda, sizeof(addr.val));
    addr.type = address_type;

    LOCK_OPS();
    ret = ble_gap_connect(own_addr_type, &addr, 30000, NULL, esp_hidh_gattc_event_handler, dev);
    if (ret) {
        esp_hidh_dev_free_inner(dev);
        ESP_LOGE(TAG, "esp_ble_gattc_open failed: %d", ret);
        UNLOCK_OPS();
        return NULL;
    }
    WAIT_CB();
    if (dev->ble.conn_id < 0) {
        ret = dev->status;
        ESP_LOGE(TAG, "dev open failed! status: 0x%x", dev->status);
        esp_hidh_dev_free_inner(dev);
        UNLOCK_OPS();
        return NULL;
    }

    dev->close = esp_ble_hidh_dev_close;
    dev->report_write = esp_ble_hidh_dev_report_write;
    dev->report_read = esp_ble_hidh_dev_report_read;
    dev->dump = esp_ble_hidh_dev_dump;

    /* perform service discovery and fill the report maps */
    read_device_services(dev);
    dev->connected = true;

    if (event_loop_handle) {
        esp_hidh_event_data_t p = {0};
        p.open.status = ESP_OK;
        p.open.dev = dev;
        esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_OPEN_EVENT, &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
    }

    attach_report_listeners(dev);
    UNLOCK_OPS();
    return dev;
}
#endif // CONFIG_BT_NIMBLE_HID_SERVICE
