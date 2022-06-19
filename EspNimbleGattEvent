#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define DEVICE_NAME "MY BLE DEVICE"
uint8_t ble_addr_type;
void ble_app_advertise(void);

#define DEVICE_INFO_SERVICE 0x180A
#define MANUFACTURER_NAME 0x2A29

#define BATTRY_SERVICE 0x180F
#define BATTRY_LEVEL_CHAR 0x2A19

#define BATTRY_CLIENT_CONFIG_DESCRIPTOR 0x2902

uint16_t batt_char_attr_hdl;
uint16_t conn_hdl;

static xTimerHandle timer_handler;

static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    printf("incoming message: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}

static int device_info(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "manufacturer name", strlen("manufacturer name"));
    return 0;
}

static int battry_level(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    static uint8_t level = 50;
    os_mbuf_append(ctxt->om, &level, sizeof(level));
    return 0;
}


//see https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Descriptors/org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
static uint8_t config[2] = {0x01, 0x00};

static int battry_level_descriptor(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if(ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC)
    {
        os_mbuf_append(ctxt->om, &config, sizeof(config));
    }
    else{
        memcpy(config, ctxt->om->om_data, ctxt->om->om_len);
    }
    if(config[0] == 0x01) {
        xTimerStart(timer_handler, 0);
    }
    else {
         xTimerStop(timer_handler, 0);
    } 
    return 0;
}

static const struct ble_gatt_svc_def gat_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(DEVICE_INFO_SERVICE),
     .characteristics = (struct ble_gatt_chr_def[]){
         {.uuid = BLE_UUID16_DECLARE(MANUFACTURER_NAME),
          .flags = BLE_GATT_CHR_F_READ,
          .access_cb = device_info},
         {.uuid = BLE_UUID128_DECLARE(0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff),
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_write},
         {0}}},
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY, 
        .uuid = BLE_UUID16_DECLARE(BATTRY_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = BLE_UUID16_DECLARE(BATTRY_LEVEL_CHAR), 
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .access_cb = battry_level,
                .val_handle = &batt_char_attr_hdl,
                .descriptors = (struct ble_gatt_dsc_def[]){
                    {
                        .uuid = BLE_UUID16_DECLARE(BATTRY_CLIENT_CONFIG_DESCRIPTOR), 
                        .att_flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                        .access_cb = battry_level_descriptor
                    },
                    {0}
                }
            },
            {0}  
        }
    },
{0}};

static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_CONNECT %s", event->connect.status == 0 ? "OK" : "Failed");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        conn_hdl = event->connect.conn_handle;
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_DISCONNECT");
        ble_app_advertise();
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_ADV_COMPLETE");
        ble_app_advertise();
        break;
    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_SUBSCRIBE");
        if(event->subscribe.attr_handle == batt_char_attr_hdl)
        {
            xTimerStart(timer_handler, 0);
        }
        break;
    default:
        break;
    }
    return 0;
}

void ble_app_advertise(void)
{
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_DISC_LTD;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)ble_svc_gap_device_name();
    fields.name_len = strlen(ble_svc_gap_device_name());
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_advertise();
}

void host_task(void *param)
{
    nimble_port_run();
}

uint8_t battry = 100;
void update_battery_timer()
{
    if(battry-- == 0) 
    {
        battry = 100;
    }
    printf("reporting battry level %d\n",battry);
     struct os_mbuf *om = ble_hs_mbuf_from_flat(&battry, sizeof(battry));
    ble_gattc_notify_custom(conn_hdl, batt_char_attr_hdl, om);

}

void app_main(void)
{
    nvs_flash_init();

    esp_nimble_hci_and_controller_init();
    nimble_port_init();

    ble_svc_gap_device_name_set(DEVICE_NAME);
    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_gatts_count_cfg(gat_svcs);
    ble_gatts_add_svcs(gat_svcs);

    ble_hs_cfg.sync_cb = ble_app_on_sync;

    timer_handler = xTimerCreate("update_battery_timer", pdMS_TO_TICKS(1000), pdTRUE, NULL, update_battery_timer);
    
    printf("got here\n");

    nimble_port_freertos_init(host_task);
}
