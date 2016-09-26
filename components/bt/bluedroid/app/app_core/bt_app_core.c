#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "fixed_queue.h"
#include "gki.h"
#include "bt_defs.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "allocator.h"

#include "bta_api.h"
#include "bta_gatt_api.h"
#include "bt_app_common.h"

#include "controller.h"

#include "hash_map.h"
#include "hash_functions.h"
#include "alarm.h"
#include "app_button.h"
#include "button_pro.h"
#include "thread.h"
#include "bt_app_common.h"
#include "gatt_profile.h"
#include "gattc_profile.h"
#include "smp_int.h"
#include "smp_api.h"

static fixed_queue_t *bta_app1_msg_queue;
fixed_queue_t *bt_app1_general_alarm_queue;
hash_map_t *bt_app1_general_alarm_hash_map;
pthread_mutex_t bt_app1_general_alarm_lock;
static const size_t BT_APP1_GENERAL_ALARM_HASH_MAP_SIZE = 10;

xQueueHandle xBtaApp1Queue;
xTaskHandle xBtaApp1TaskHandle;

#define BT_APP1_TTYPE_MAIN_ENTRY   (1)
static TIMER_LIST_ENT main_boot_tle;

tSMP_CB smp_cmd;

static void bt_app1_context_switched(void *p_msg);
static void bt_app1_send_msg(void *p_msg);
static void bt_app1_task_handler(void *arg);
static void bta_app1_msg_ready(fixed_queue_t *queue);
static void bt_app1_task_shut_down(void);

static void bt_app1_general_alarm_ready(fixed_queue_t *queue);
static void bt_app1_general_alarm_process(TIMER_LIST_ENT *p_tle);
void bt_app1_start_timer(TIMER_LIST_ENT *p_tle, UINT16 type, UINT32 timeout_sec);

//extern void ble_test_conn(void);
//extern void bt_test_start_inquiry(void);
extern void ble_server_test(void);

static void bt_app1_task_handler(void *arg)
{
    TaskEvt_t *e;
    UINT8 button_msg[2] = {0x01,0x00};
    for (;;) {
        if (pdTRUE == xQueueReceive(xBtaApp1Queue, &e, (portTickType)portMAX_DELAY)) {
            if (e->sig == 0xff) {
                fixed_queue_process(bta_app1_msg_queue);
                fixed_queue_process(bt_app1_general_alarm_queue);
            }else if(e->sig == BUTTON_PRESS_EVT){
			LOG_ERROR("button_press_event come in,button_value=%x\n",e->par);
		  button_msg[1] = e->par;
          button_msg_notify(2,button_msg);		
}
        }
        osi_free(e);
    }
}

static void bt_app1_task_post(void)
{

     TaskEvt_t *evt = (TaskEvt_t *)osi_malloc(sizeof(TaskEvt_t));
     if (evt == NULL)
        return;

     evt->sig = 0xff;
     evt->par = 0;

     if (xQueueSend(xBtaApp1Queue, &evt, 10/portTICK_RATE_MS) != pdTRUE) {
         ets_printf("btdm_post failed\n");
     }

}


static void bta_app1_msg_ready(fixed_queue_t *queue) {
    BT_HDR *p_msg;
    while (!fixed_queue_is_empty(queue)) {
        p_msg = (BT_HDR *)fixed_queue_dequeue(queue);
        LOG_ERROR("bta_app1_msg_ready, evt: %d\n", p_msg->event);
        switch (p_msg->event) {
        case BT_EVT_APP1_CONTEXT_SWITCH:
            bt_app1_context_switched(p_msg);
            break;
        default:
            LOG_ERROR("unhandled BT_APP1 event (%d)\n", p_msg->event & BT_EVT_MASK);
            break;
        }
        GKI_freebuf(p_msg);
    }
}

static void bt_app1_context_switched(void *p_msg)
{
    tBTAPP1_CONTEXT_SWITCH_CBACK *p = (tBTAPP1_CONTEXT_SWITCH_CBACK *) p_msg;
    
    if (p->p_cb)
        p->p_cb(p->event, p->p_param);
}

static void bt_app1_send_msg(void *p_msg)
{
    if (bta_app1_msg_queue) {
        fixed_queue_enqueue(bta_app1_msg_queue, p_msg);
        //ke_event_set(KE_EVENT_BT_APP1_TASK);
        bt_app1_task_post();
    }
}

bt_status_t bt_app1_transfer_context (tBTAPP1_CBACK *p_cback, UINT16 event, char* p_params, int param_len, tBTAPP1_COPY_CBACK *p_copy_cback)
{
    tBTAPP1_CONTEXT_SWITCH_CBACK *p_msg;

    LOG_ERROR("btapp1_transfer_context evt %d, len %d", event, param_len);

    /* allocate and send message that will be executed in btif context */
    if ((p_msg = (tBTAPP1_CONTEXT_SWITCH_CBACK *) GKI_getbuf(sizeof(tBTAPP1_CONTEXT_SWITCH_CBACK) + param_len)) != NULL)
    {
        p_msg->hdr.event = BT_EVT_APP1_CONTEXT_SWITCH; /* internal event */
        p_msg->p_cb = p_cback;

        p_msg->event = event;                         /* callback event */

        /* check if caller has provided a copy callback to do the deep copy */
        if (p_copy_cback)
        {
            p_copy_cback(event, p_msg->p_param, p_params);
        }
        else if (p_params)
        {
            memcpy(p_msg->p_param, p_params, param_len);  /* callback parameter data */
        }

        bt_app1_send_msg(p_msg);
        return BT_STATUS_SUCCESS;
    }
    else
    {
        /* let caller deal with a failed allocation */
        return BT_STATUS_NOMEM;
    }
}

void bt_app1_task_start_up(void)
{
    bta_app1_msg_queue = fixed_queue_new(SIZE_MAX);
    if (bta_app1_msg_queue == NULL)
        goto error_exit;
    //ke_event_callback_set(KE_EVENT_BT_APP1_TASK, &bt_app1_task_handler);

    xBtaApp1Queue = xQueueCreate(3, sizeof(void *));
    xTaskCreate(bt_app1_task_handler, "BtaApp1T", 8192, NULL, configMAX_PRIORITIES - 3, xBtaApp1TaskHandle);

    fixed_queue_register_dequeue(bta_app1_msg_queue, bta_app1_msg_ready);

    bt_app1_general_alarm_hash_map = hash_map_new(BT_APP1_GENERAL_ALARM_HASH_MAP_SIZE,
            hash_function_pointer, NULL, (data_free_fn)osi_alarm_free, NULL);
    if (bt_app1_general_alarm_hash_map == NULL)
        goto error_exit;

    pthread_mutex_init(&bt_app1_general_alarm_lock, NULL);

    bt_app1_general_alarm_queue = fixed_queue_new(SIZE_MAX);
    if (bt_app1_general_alarm_queue == NULL)
        goto error_exit;
    fixed_queue_register_dequeue(bt_app1_general_alarm_queue, bt_app1_general_alarm_ready);

    memset(&main_boot_tle, 0, sizeof(TIMER_LIST_ENT));
    return;

error_exit:
    LOG_ERROR("%s Unable to allocate resources for bt_app1", __func__);
    bt_app1_task_shut_down();
}

static void bt_app1_task_shut_down(void)
{
    fixed_queue_unregister_dequeue(bta_app1_msg_queue);
    fixed_queue_free(bta_app1_msg_queue, NULL);
    bta_app1_msg_queue = NULL;

    // todo: hash map, pthread_mutex...
    fixed_queue_unregister_dequeue(bt_app1_general_alarm_queue);

    vTaskDelete(xBtaApp1TaskHandle);
    vTaskDelete(xBtaApp1Queue);
}


static void bt_app1_dm_data_copy(uint16_t event, char *dst, char *src)
{
    tBTA_DM_SEC *dst_dm_sec = (tBTA_DM_SEC*)dst;
    tBTA_DM_SEC *src_dm_sec = (tBTA_DM_SEC*)src;

    if (!src_dm_sec)
        return;

    assert(dst_dm_sec);
    memcpy(dst_dm_sec, src_dm_sec, sizeof(tBTA_DM_SEC));

    if (event == BTA_DM_BLE_KEY_EVT)
    {
        dst_dm_sec->ble_key.p_key_value = osi_malloc(sizeof(tBTM_LE_KEY_VALUE));
        assert(src_dm_sec->ble_key.p_key_value);
        assert(dst_dm_sec->ble_key.p_key_value);
        memcpy(dst_dm_sec->ble_key.p_key_value, src_dm_sec->ble_key.p_key_value, sizeof(tBTM_LE_KEY_VALUE));
    }
}

static void bt_app1_dm_data_free(uint16_t event, tBTA_DM_SEC *dm_sec)
{
    if (event == BTA_DM_BLE_KEY_EVT)
        osi_free(dm_sec->ble_key.p_key_value);
}

static void bt_app1_dm_upstreams_evt(UINT16 event, char *p_param)
{
    tBTA_DM_SEC *p_data = (tBTA_DM_SEC*)p_param;
    switch (event) {
    case BTA_DM_ENABLE_EVT: {

//        BTA_DmSetDeviceName("ijiazu");

  


        /*set connectable,discoverable, pairable and paired only modes of local device*/
        tBTA_DM_DISC disc_mode = BTA_DM_GENERAL_DISC | BTA_DM_BLE_GENERAL_DISCOVERABLE;
        tBTA_DM_CONN conn_mode = BTA_DM_CONN | BTA_DM_BLE_CONNECTABLE;
        BTA_DmSetVisibility(disc_mode, conn_mode, BTA_DM_IGNORE, BTA_DM_IGNORE);

#if (defined(BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
        /* Enable local privacy */
        BTA_DmBleConfigLocalPrivacy(BLE_LOCAL_PRIVACY_ENABLED);
        do {
            const controller_t *controller = controller_get_interface();
            char bdstr[18];
            bdaddr_to_string(controller->get_address(), bdstr, sizeof(bdstr));
            LOG_ERROR("BDA is: %s\n", bdstr);
        } while (0);
#endif        
    }
	break;
	case BTA_DM_BLE_SEC_REQ_EVT:
		
		smp_cmd.local_io_capability = 0x03;		//no input no output
		smp_cmd.loc_oob_flag = 0x00;		//oob data not present
		smp_cmd.loc_auth_req = 0x05;
		smp_cmd.loc_enc_size = 0x10;
		smp_cmd.local_i_key = 0x07;
		smp_cmd.local_r_key = 0x07;
		memcpy(smp_cmd.pairing_bda,p_data->ble_req.bd_addr,0x06);
		smp_send_cmd(SMP_OPCODE_PAIRING_RSP,&smp_cmd);
		smp_set_state(SMP_STATE_WAIT_CONFIRM);
		//BTA_DmConfirm(p_data->ble_req.bd_addr,true);
			break;
	default:
		break;
    }

    bt_app1_dm_data_free(event, p_data);
}

static void bte_dm_evt(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* p_data)
{
    LOG_ERROR("bte_dm_evt: %d\n", (uint16_t)event);
    bt_app1_transfer_context(bt_app1_dm_upstreams_evt, (uint16_t)event, 
           (void *)p_data, sizeof(tBTA_DM_SEC), bt_app1_dm_data_copy);
}

void bt_app1_init_ok(UNUSED_ATTR uint16_t event, UNUSED_ATTR char *p_param)
{
    BTA_EnableBluetooth(bte_dm_evt);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    bt_app1_start_timer(&main_boot_tle, BT_APP1_TTYPE_MAIN_ENTRY, 8);
}

/* Alarm timer */
static void bt_app1_general_alarm_cb(void *data) {
    assert(data != NULL);
    TIMER_LIST_ENT *p_tle = (TIMER_LIST_ENT *)data;

    fixed_queue_enqueue(bt_app1_general_alarm_queue, p_tle);
    //ke_event_set(KE_EVENT_BT_APP1_TASK);
    bt_app1_task_post();
}

void bt_app1_start_timer(TIMER_LIST_ENT *p_tle, UINT16 type, UINT32 timeout_sec) {
   osi_alarm_t *alarm = NULL;

    assert(p_tle != NULL);

    // Get the alarm for the timer list entry.
    pthread_mutex_lock(&bt_app1_general_alarm_lock);
    if (!hash_map_has_key(bt_app1_general_alarm_hash_map, p_tle)) {
        alarm = osi_alarm_new("bt_app1", bt_app1_general_alarm_cb, (void *)p_tle, 0);
        hash_map_set(bt_app1_general_alarm_hash_map, p_tle, alarm);
    }
    pthread_mutex_unlock(&bt_app1_general_alarm_lock);
    
	pthread_mutex_lock(&bt_app1_general_alarm_lock);
    alarm = hash_map_get(bt_app1_general_alarm_hash_map, p_tle);
    pthread_mutex_unlock(&bt_app1_general_alarm_lock);
    if (alarm == NULL) {
        LOG_ERROR("%s Unable to create alarm", __func__);

        return;
    }

    osi_alarm_cancel(alarm);

    p_tle->event = type;
    // NOTE: This value is in seconds but stored in a ticks field.
    p_tle->ticks = timeout_sec;
    p_tle->in_use = TRUE;
    osi_alarm_set(alarm, (period_ms_t)(timeout_sec * 1000));
}

void bt_app1_stop_timer(TIMER_LIST_ENT *p_tle)
{
    assert(p_tle != NULL);

    if (p_tle->in_use == FALSE)
        return;
    p_tle->in_use = FALSE;

    // Get the alarm for the timer list entry.
   osi_alarm_t *alarm = hash_map_get(bt_app1_general_alarm_hash_map, p_tle);
    if (alarm == NULL) {
        LOG_WARN("%s Unable to find expected alarm in hashmap", __func__);
        return;
    }
    osi_alarm_cancel(alarm);
}

static void bt_app1_general_alarm_process(TIMER_LIST_ENT *p_tle)
{
    assert(p_tle != NULL);
    LOG_ERROR("general_alarm_process\n");
    switch (p_tle->event) {
    case BT_APP1_TTYPE_MAIN_ENTRY:
        LOG_ERROR("BT_APP1 main boot**********\n");

      //  ble_test_conn();
  //      ble_server_test();
 

  //     bt_test_start_inquiry();

       gatts_server_test();
       //gattc_client_test();
        break;
    }

}

static void bt_app1_general_alarm_ready(fixed_queue_t *queue)
{
    TIMER_LIST_ENT *p_tle;

    while (!fixed_queue_is_empty(queue)) {
        p_tle = (TIMER_LIST_ENT *)fixed_queue_dequeue(queue);
        bt_app1_general_alarm_process(p_tle);
    }
}

