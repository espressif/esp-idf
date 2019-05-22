/******************************************************************************
 *
 *  Copyright (C) 2016 The Android Open Source Project
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *  Copyright (C) 2019 Blake Felt
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
/************************************************************************************
 *
 *  Filename:      btc_hd.cc
 *
 *  Description:   HID Device Profile Bluetooth Interface
 *
 *
 ***********************************************************************************/
//#include <errno.h>
//#include <hardware/bluetooth.h>
//#include <hardware/bt_hd.h>
#include "esp_hidd_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bta/bta_api.h"
#include "bta/bta_hd_api.h"
#include "bta/bta_hh_api.h"
#include "btc/btc_storage.h"
#include "btc/btc_util.h"
#include "btc/btc_manage.h"
#include "btc_hd.h"
#include "osi/allocator.h"

#if HID_DEV_INCLUDED == TRUE

#define BTC_HD_APP_NAME_LEN 50
#define BTC_HD_APP_DESCRIPTION_LEN 50
#define BTC_HD_APP_PROVIDER_LEN 50
#define BTC_HD_APP_DESCRIPTOR_LEN 2048
#define COD_HID_KEYBOARD 0x0540
#define COD_HID_POINTING 0x0580
#define COD_HID_COMBO 0x05C0
#define COD_HID_MAJOR 0x0500
extern "C" bool bta_dm_check_if_only_hd_connected(BD_ADDR peer_addr);
//extern bool check_cod_hid(const bt_bdaddr_t* remote_bdaddr);
/* HD request events */
typedef enum { BTC_HD_DUMMY_REQ_EVT = 0 } btc_hd_req_evt_t;
btc_hd_cb_t btc_hd_cb;
static esp_hidd_callbacks_t* bt_hd_callbacks = NULL;
static tBTA_HD_APP_INFO app_info;
static tBTA_HD_QOS_INFO in_qos;
static tBTA_HD_QOS_INFO out_qos;
typedef void (bt_hid_copy_cb_t) (btc_msg_t* msg, void *p_dest, void *p_src);
static void intr_data_copy_cb(btc_msg_t* msg, void* p_dst, void* p_src) {
  uint16_t event = msg->act;
  tBTA_HD_INTR_DATA* p_dst_data = (tBTA_HD_INTR_DATA*)p_dst;
  tBTA_HD_INTR_DATA* p_src_data = (tBTA_HD_INTR_DATA*)p_src;
  uint8_t* p_data;
  if (!p_src) return;
  if (event != BTA_HD_INTR_DATA_EVT) return;
  memcpy(p_dst, p_src, sizeof(tBTA_HD_INTR_DATA));
  p_data = ((uint8_t*)p_dst_data) + sizeof(tBTA_HD_INTR_DATA);
  memcpy(p_data, p_src_data->p_data, p_src_data->len);
  p_dst_data->p_data = p_data;
}
static void set_report_copy_cb(btc_msg_t* msg, void* p_dst, void* p_src) {
  uint16_t event = msg->act;
  tBTA_HD_SET_REPORT* p_dst_data = (tBTA_HD_SET_REPORT*)p_dst;
  tBTA_HD_SET_REPORT* p_src_data = (tBTA_HD_SET_REPORT*)p_src;
  uint8_t* p_data;
  if (!p_src) return;
  if (event != BTA_HD_SET_REPORT_EVT) return;
  memcpy(p_dst, p_src, sizeof(tBTA_HD_SET_REPORT));
  p_data = ((uint8_t*)p_dst_data) + sizeof(tBTA_HD_SET_REPORT);
  memcpy(p_data, p_src_data->p_data, p_src_data->len);
  p_dst_data->p_data = p_data;
}
static void btc_hd_free_buf() {
  if (app_info.descriptor.dsc_list) osi_free(app_info.descriptor.dsc_list);
  if (app_info.p_description) osi_free(app_info.p_description);
  if (app_info.p_name) osi_free(app_info.p_name);
  if (app_info.p_provider) osi_free(app_info.p_provider);
  app_info.descriptor.dsc_list = NULL;
  app_info.p_description = NULL;
  app_info.p_name = NULL;
  app_info.p_provider = NULL;
}

#define HAL_CBACK(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }
    
/*******************************************************************************
 *
 * Function         btc_hd_remove_device
 *
 * Description      Removes plugged device
 *
 * Returns          void
 *
 ******************************************************************************/
void btc_hd_remove_device(bt_bdaddr_t bd_addr) {
  BTA_HdRemoveDevice((uint8_t*)&bd_addr);
  btc_storage_remove_bonded_device(&bd_addr);
}
/*******************************************************************************
 *
 * Function         btc_hd_upstreams_evt
 *
 * Description      Executes events in btc context
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hd_upstreams_evt(uint16_t event, char* p_param) {
  tBTA_HD* p_data = (tBTA_HD*)p_param;

  BTC_TRACE_API("%s: event=%s", __func__, dump_hd_event(event));

  switch (event) {
    case BTA_HD_ENABLE_EVT:
      BTC_TRACE_DEBUG("%s: status=%d", __func__, p_data->status);
      if (p_data->status == BTA_HD_OK) {
        //btif_storage_load_hidd();
        btc_hd_cb.status = BTC_HD_ENABLED;
        /* Register the app if not yet registered */
        if (!btc_hd_cb.app_registered) {
          BTA_HdRegisterApp(&app_info, &in_qos, &out_qos);
          btc_hd_free_buf();
        }
      } else {
        btc_hd_cb.status = BTC_HD_DISABLED;
        BTC_TRACE_WARNING("Failed to enable BT-HD, status=%d", p_data->status);
      }
      break;
    case BTA_HD_DISABLE_EVT:
      BTC_TRACE_DEBUG("%s: status=%d", __func__, p_data->status);
      btc_hd_cb.status = BTC_HD_DISABLED;
      if (btc_hd_cb.service_dereg_active) {
        BTC_TRACE_WARNING("registering hid host now");
        //btif_hh_service_registration(TRUE);
        btc_hd_cb.service_dereg_active = FALSE;
      }
      btc_hd_free_buf();
      if (p_data->status == BTA_HD_OK)
        memset(&btc_hd_cb, 0, sizeof(btc_hd_cb));
      else
        BTC_TRACE_WARNING("Failed to disable BT-HD, status=%d",
                           p_data->status);
      break;
    case BTA_HD_REGISTER_APP_EVT: {
      bt_bdaddr_t* addr = (bt_bdaddr_t*)&p_data->reg_status.bda;
      
      if (!p_data->reg_status.in_use) {
        addr = NULL;
      }
      btc_hd_cb.app_registered = TRUE;
      
      HAL_CBACK(bt_hd_callbacks, application_state_cb, addr->address,
                ESP_HIDD_APP_STATE_REGISTERED);
    } break;
    case BTA_HD_UNREGISTER_APP_EVT:
      btc_hd_cb.app_registered = FALSE;
      HAL_CBACK(bt_hd_callbacks, application_state_cb, NULL,
                ESP_HIDD_APP_STATE_NOT_REGISTERED);
      if (btc_hd_cb.service_dereg_active) {
        BTC_TRACE_WARNING("disabling hid device service now");
        btc_hd_free_buf();
        BTA_HdDisable();
      }
      break;
    case BTA_HD_OPEN_EVT: {
      bt_bdaddr_t* addr = (bt_bdaddr_t*)&p_data->conn.bda;
      BTC_TRACE_EVENT(
          "BTA_HD_OPEN_EVT, address (%02x:%02x:%02x:%02x:%02x:%02x)",
          addr->address[0], addr->address[1], addr->address[2],
          addr->address[3], addr->address[4], addr->address[5]);
      BTC_TRACE_DEBUG("not checking if the connection is from hid host or hid device");
      /* Check if the connection is from hid host and not hid device */
    //   if (check_cod_hid(addr)) {
    //     /* Incoming connection from hid device, reject it */
    //     BTC_TRACE_WARNING("remote device is not hid host, disconnecting");
    //     btc_hd_cb.forced_disc = TRUE;
    //     BTA_HdDisconnect();
    //     break;
    //   }
      BTC_TRACE_DEBUG("not placing hidd into nvram");
    //   btif_storage_set_hidd((bt_bdaddr_t*)&p_data->conn.bda);
      HAL_CBACK(bt_hd_callbacks, connection_state_cb,
                addr->address, ESP_HIDD_CONN_STATE_CONNECTED);
    } break;
    case BTA_HD_CLOSE_EVT: {
      bt_bdaddr_t* addr = (bt_bdaddr_t*)&p_data->conn.bda;

      
      if (btc_hd_cb.forced_disc) {
        
        BTC_TRACE_WARNING("remote device was forcefully disconnected");
        btc_hd_remove_device(*addr);
        btc_hd_cb.forced_disc = FALSE;
        HID_DevUnplugDevice(addr->address);
        break;
      }
      HID_DevUnplugDevice(addr->address);
      HAL_CBACK(bt_hd_callbacks, connection_state_cb,
                addr->address, ESP_HIDD_CONN_STATE_DISCONNECTED);
    } break;
    case BTA_HD_GET_REPORT_EVT:
      HAL_CBACK(bt_hd_callbacks, get_report_cb, p_data->get_report.report_type,
                p_data->get_report.report_id, p_data->get_report.buffer_size);
      break;
    case BTA_HD_SET_REPORT_EVT:
      HAL_CBACK(bt_hd_callbacks, set_report_cb, p_data->set_report.report_type,
                p_data->set_report.report_id, p_data->set_report.len,
                p_data->set_report.p_data);
      break;
    case BTA_HD_SET_PROTOCOL_EVT:
      HAL_CBACK(bt_hd_callbacks, set_protocol_cb, p_data->set_protocol);
      break;
    case BTA_HD_INTR_DATA_EVT:
      HAL_CBACK(bt_hd_callbacks, intr_data_cb, p_data->intr_data.report_id,
                p_data->intr_data.len, p_data->intr_data.p_data);
      break;
    case BTA_HD_VC_UNPLUG_EVT: {
      bt_bdaddr_t* bd_addr = (bt_bdaddr_t*)&p_data->conn.bda;
      HAL_CBACK(bt_hd_callbacks, connection_state_cb,
                bd_addr->address, ESP_HIDD_CONN_STATE_DISCONNECTED);
      
      if (bta_dm_check_if_only_hd_connected(p_data->conn.bda)) {
        BTC_TRACE_DEBUG("%s: Removing bonding as only HID profile connected",
                         __func__);
        BTA_DmRemoveDevice((uint8_t*)&p_data->conn.bda, BT_TRANSPORT_BR_EDR);
      } else {
        BTC_TRACE_DEBUG(
            "%s: Only removing HID data as some other profiles "
            "connected",
            __func__);
        btc_hd_remove_device(*bd_addr);
      }
      HAL_CBACK(bt_hd_callbacks, vc_unplug_cb);
      } break;
    default:
      BTC_TRACE_WARNING("%s: unknown event (%d)", __func__, event);
      break;
  }
}
/*******************************************************************************
 *
 * Function         bte_hd_evt
 *
 * Description      Switches context from BTE to BTC for all BT-HD events
 *
 * Returns          void
 *
 ******************************************************************************/
static void bte_hd_evt(tBTA_HD_EVT event, tBTA_HD* p_data) {
  bt_status_t status;
  int param_len = 0;
  bt_hid_copy_cb_t* p_copy_cback = NULL;
  BTC_TRACE_API("%s event=%d", __func__, event);
  switch (event) {
    case BTA_HD_ENABLE_EVT:
    case BTA_HD_DISABLE_EVT:
    case BTA_HD_UNREGISTER_APP_EVT:
      param_len = sizeof(tBTA_HD_STATUS);
      break;
    case BTA_HD_REGISTER_APP_EVT:
      param_len = sizeof(tBTA_HD_REG_STATUS);
      break;
    case BTA_HD_OPEN_EVT:
    case BTA_HD_CLOSE_EVT:
    case BTA_HD_VC_UNPLUG_EVT:
      param_len = sizeof(tBTA_HD_CONN);
      break;
    case BTA_HD_GET_REPORT_EVT:
      param_len += sizeof(tBTA_HD_GET_REPORT);
      break;
    case BTA_HD_SET_REPORT_EVT:
      param_len = sizeof(tBTA_HD_SET_REPORT) + p_data->set_report.len;
      p_copy_cback = set_report_copy_cb;
      break;
    case BTA_HD_SET_PROTOCOL_EVT:
      param_len += sizeof(p_data->set_protocol);
      break;
    case BTA_HD_INTR_DATA_EVT:
      param_len = sizeof(tBTA_HD_INTR_DATA) + p_data->intr_data.len;
      p_copy_cback = intr_data_copy_cb;
      break;
  }
  btc_msg_t msg;
  msg.sig = BTC_SIG_API_CB;
  msg.pid = BTC_PID_HD;
  msg.act = event;

  status = btc_transfer_context(&msg, p_data, param_len, p_copy_cback);
  if(status != BT_STATUS_SUCCESS) {
      BTC_TRACE_ERROR("context transfer failed");
  }
}
/*******************************************************************************
 *
 * Function        init
 *
 * Description     Initializes BT-HD interface
 *
 * Returns         BT_STATUS_SUCCESS
 *
 ******************************************************************************/
void btc_hd_service_registration(void);
static bt_status_t init(esp_hidd_callbacks_t* callbacks) {
  HID_DevSetTraceLevel(HIDD_INITIAL_TRACE_LEVEL);
  HID_DevInit(); // initialize!!!
  btc_hd_service_registration();

  BTC_TRACE_API("%s", __func__);
  bt_hd_callbacks = callbacks;
  memset(&btc_hd_cb, 0, sizeof(btc_hd_cb));
  return BT_STATUS_SUCCESS;
}
/*******************************************************************************
 *
 * Function         cleanup
 *
 * Description      Cleans up BT-HD interface
 *
 * Returns          none
 *
 ******************************************************************************/
static void cleanup(void) {
  BTC_TRACE_API("hd:%s", __func__);
  if (bt_hd_callbacks) {
    /* update flag, not to enable hid host service now as BT is switching off */
    btc_hd_cb.service_dereg_active = FALSE;
    //btif_disable_service(BTA_HIDD_SERVICE_ID);
    bt_hd_callbacks = NULL;
  }
}
/*******************************************************************************
 *
 * Function         register_app
 *
 * Description      Registers HID Device application
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t register_app(esp_hidd_app_param_t* p_app_param,
                                esp_hidd_qos_param_t* p_in_qos,
                                esp_hidd_qos_param_t* p_out_qos) {
  BTC_TRACE_API("%s", __func__);
  if (btc_hd_cb.app_registered) {
    BTC_TRACE_WARNING("%s: application already registered", __func__);
    return BT_STATUS_BUSY;
  }
  app_info.p_name = (char*)osi_malloc(BTC_HD_APP_NAME_LEN);
  memcpy(app_info.p_name, p_app_param->name, BTC_HD_APP_NAME_LEN);
  app_info.p_description = (char*)osi_malloc(BTC_HD_APP_DESCRIPTION_LEN);
  memcpy(app_info.p_description, p_app_param->description,
         BTC_HD_APP_DESCRIPTION_LEN);
  app_info.p_provider = (char*)osi_malloc(BTC_HD_APP_PROVIDER_LEN);
  memcpy(app_info.p_provider, p_app_param->provider, BTC_HD_APP_PROVIDER_LEN);
  app_info.subclass = p_app_param->subclass;
  app_info.descriptor.dl_len = p_app_param->desc_list_len;
  app_info.descriptor.dsc_list =
      (uint8_t*)osi_malloc(app_info.descriptor.dl_len);
  memcpy(app_info.descriptor.dsc_list, p_app_param->desc_list,
         p_app_param->desc_list_len);
  in_qos.service_type = p_in_qos->service_type;
  in_qos.token_rate = p_in_qos->token_rate;
  in_qos.token_bucket_size = p_in_qos->token_bucket_size;
  in_qos.peak_bandwidth = p_in_qos->peak_bandwidth;
  in_qos.access_latency = p_in_qos->access_latency;
  in_qos.delay_variation = p_in_qos->delay_variation;
  out_qos.service_type = p_out_qos->service_type;
  out_qos.token_rate = p_out_qos->token_rate;
  out_qos.token_bucket_size = p_out_qos->token_bucket_size;
  out_qos.peak_bandwidth = p_out_qos->peak_bandwidth;
  out_qos.access_latency = p_out_qos->access_latency;
  out_qos.delay_variation = p_out_qos->delay_variation;
  /* register HID Device with L2CAP and unregister HID Host with L2CAP */
  /* Disable HH */
  //btif_hh_service_registration(FALSE);
  return BT_STATUS_SUCCESS;
}
/*******************************************************************************
 *
 * Function         unregister_app
 *
 * Description      Unregisters HID Device application
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t unregister_app(void) {
  BTC_TRACE_API("%s", __func__);
  if (!btc_hd_cb.app_registered) {
    BTC_TRACE_WARNING("%s: application not yet registered", __func__);
    return BT_STATUS_NOT_READY;
  }
  if (btc_hd_cb.status != BTC_HD_ENABLED) {
    BTC_TRACE_WARNING("%s: BT-HD not enabled, status=%d", __func__,
                       btc_hd_cb.status);
    return BT_STATUS_NOT_READY;
  }
  if (btc_hd_cb.service_dereg_active) {
    BTC_TRACE_WARNING("%s: BT-HD deregistering in progress", __func__);
    return BT_STATUS_BUSY;
  }
  btc_hd_cb.service_dereg_active = TRUE;
  BTA_HdUnregisterApp();
  return BT_STATUS_SUCCESS;
}
/*******************************************************************************
 *
 * Function         connect
 *
 * Description      Connects to host
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t connect(void) {
  BTC_TRACE_API("%s", __func__);
  if (!btc_hd_cb.app_registered) {
    BTC_TRACE_WARNING("%s: application not yet registered", __func__);
    return BT_STATUS_NOT_READY;
  }
  if (btc_hd_cb.status != BTC_HD_ENABLED) {
    BTC_TRACE_WARNING("%s: BT-HD not enabled, status=%d", __func__,
                       btc_hd_cb.status);
    return BT_STATUS_NOT_READY;
  }
  BTA_HdConnect();
  return BT_STATUS_SUCCESS;
}
/*******************************************************************************
 *
 * Function         disconnect
 *
 * Description      Disconnects from host
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t disconnect(void) {
  BTC_TRACE_API("%s", __func__);
  if (!btc_hd_cb.app_registered) {
    BTC_TRACE_WARNING("%s: application not yet registered", __func__);
    return BT_STATUS_NOT_READY;
  }
  if (btc_hd_cb.status != BTC_HD_ENABLED) {
    BTC_TRACE_WARNING("%s: BT-HD not enabled, status=%d", __func__,
                       btc_hd_cb.status);
    return BT_STATUS_NOT_READY;
  }
  BTA_HdDisconnect();
  return BT_STATUS_SUCCESS;
}
/*******************************************************************************
 *
 * Function         send_report
 *
 * Description      Sends Reports to hid host
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t send_report(esp_hidd_report_type_t type, uint8_t id,
                               uint16_t len, uint8_t* p_data) {
  tBTA_HD_REPORT report;
  APPL_TRACE_VERBOSE("%s: type=%d id=%d len=%d", __func__, type, id, len);
  if (!btc_hd_cb.app_registered) {
    BTC_TRACE_WARNING("%s: application not yet registered", __func__);
    return BT_STATUS_NOT_READY;
  }
  if (btc_hd_cb.status != BTC_HD_ENABLED) {
    BTC_TRACE_WARNING("%s: BT-HD not enabled, status=%d", __func__,
                       btc_hd_cb.status);
    return BT_STATUS_NOT_READY;
  }
  if (type == ESP_HIDD_REPORT_TYPE_INTRDATA) {
    report.type = ESP_HIDD_REPORT_TYPE_INPUT;
    report.use_intr = TRUE;
  } else {
    report.type = (type & 0x03);
    report.use_intr = FALSE;
  }
  report.id = id;
  report.len = len;
  report.p_data = p_data;
  BTA_HdSendReport(&report);
  return BT_STATUS_SUCCESS;
}
/*******************************************************************************
 *
 * Function         report_error
 *
 * Description      Sends HANDSHAKE with error info for invalid SET_REPORT
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t report_error(uint8_t error) {
  BTC_TRACE_API("%s", __func__);
  if (!btc_hd_cb.app_registered) {
    BTC_TRACE_WARNING("%s: application not yet registered", __func__);
    return BT_STATUS_NOT_READY;
  }
  if (btc_hd_cb.status != BTC_HD_ENABLED) {
    BTC_TRACE_WARNING("%s: BT-HD not enabled, status=%d", __func__,
                       btc_hd_cb.status);
    return BT_STATUS_NOT_READY;
  }
  BTA_HdReportError(error);
  return BT_STATUS_SUCCESS;
}
/*******************************************************************************
 *
 * Function         virtual_cable_unplug
 *
 * Description      Sends Virtual Cable Unplug to host
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t virtual_cable_unplug(void) {
  BTC_TRACE_API("%s", __func__);
  if (!btc_hd_cb.app_registered) {
    BTC_TRACE_WARNING("%s: application not yet registered", __func__);
    return BT_STATUS_NOT_READY;
  }
  if (btc_hd_cb.status != BTC_HD_ENABLED) {
    BTC_TRACE_WARNING("%s: BT-HD not enabled, status=%d", __func__,
                       btc_hd_cb.status);
    return BT_STATUS_NOT_READY;
  }
  BTA_HdVirtualCableUnplug();
  return BT_STATUS_SUCCESS;
}
// static const bthd_interface_t bthdInterface = {
//     sizeof(bthdInterface),
//     init,
//     cleanup,
//     register_app,
//     unregister_app,
//     connect,
//     disconnect,
//     send_report,
//     report_error,
//     virtual_cable_unplug,
// };
/*******************************************************************************
 *
 * Function         btif_hd_execute_service
 *
 * Description      Enabled/disables BT-HD service
 *
 * Returns          BT_STATUS_SUCCESS
 *
 ******************************************************************************/
// bt_status_t btif_hd_execute_service(bool b_enable) {
//   BTC_TRACE_API("%s: b_enable=%d", __func__, b_enable);
//   if (!b_enable) BTA_HdDisable();
//   return BT_STATUS_SUCCESS;
// }
/*******************************************************************************
 *
 * Function         btif_hd_get_interface
 *
 * Description      Gets BT-HD interface
 *
 * Returns          bthd_interface_t
 *
 ******************************************************************************/
// const bthd_interface_t* btif_hd_get_interface() {
//   BTC_TRACE_API("%s", __func__);
//   return &bthdInterface;
// }
/*******************************************************************************
 *
 * Function         btc_hd_service_registration
 *
 * Description      Registers hid device service
 *
 * Returns          none
 *
 ******************************************************************************/
void btc_hd_service_registration() {
   BTC_TRACE_API("%s", __func__);
   /* enable HD */
   BTA_HdEnable(bte_hd_evt);
}

void btc_hd_call_handler(btc_msg_t *msg) {
    esp_hidd_args_t *arg = (esp_hidd_args_t*) (msg->arg);
    switch(msg->act) {
        case BTC_HD_INIT_EVT:
            init(arg->callbacks);
            break;
        case BTC_HD_CLEANUP_EVT:
            cleanup();
            break;
        case BTC_HD_REGISTER_EVT:
            register_app(arg->register_app.app_param, arg->register_app.in_qos, arg->register_app.out_qos);
            break;
        case BTC_HD_UNREGISTER_EVT:
            unregister_app();
            break;
        case BTC_HD_CONNECT_EVT:
            connect();
            break;
        case BTC_HD_DISCONNECT_EVT:
            disconnect();
            break;
        case BTC_HD_SEND_REPORT_EVT:
            send_report(arg->send_report.type, arg->send_report.id, arg->send_report.len, arg->send_report.p_data);
            break;
        case BTC_HD_REPORT_ERROR_EVT:
            report_error(arg->error);
            break;
        case BTC_HD_UNPLUG_EVT:
            virtual_cable_unplug();
            break;
        default:
            BTC_TRACE_WARNING("unknown hidd action %i", msg->act);
            break;
    }
}

void btc_hd_cb_handler(btc_msg_t *msg) {
    uint16_t event = msg->act;
    char* p_param = (char*)msg->arg;
    btc_hd_upstreams_evt(event, p_param);
}

#endif // HID_DEV_INCLUDED==TRUE