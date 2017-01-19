/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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


/*****************************************************************************
 *
 *  Filename:      btif_rc.c
 *
 *  Description:   Bluetooth AVRC implementation
 *
 *****************************************************************************/
// #include <hardware/bluetooth.h>
// #include <fcntl.h>
#include <string.h>
#include "bta_api.h"
#include "bta_av_api.h"
#include "avrc_defs.h"
#include "gki.h"

#define LOG_TAG "bt_btif_avrc"
#include "btif_common.h"
#include "btif_util.h"
#include "btif_av.h"
#include "bt_rc.h"
#include "uinput.h"
#include "esp_avrc_api.h"

/*****************************************************************************
**  Constants & Macros
******************************************************************************/

/* cod value for Headsets */
#define COD_AV_HEADSETS        0x0404
/* for AVRC 1.4 need to change this */
#define MAX_RC_NOTIFICATIONS AVRC_EVT_APP_SETTING_CHANGE

#define IDX_GET_PLAY_STATUS_RSP   0
#define IDX_LIST_APP_ATTR_RSP     1
#define IDX_LIST_APP_VALUE_RSP    2
#define IDX_GET_CURR_APP_VAL_RSP  3
#define IDX_SET_APP_VAL_RSP       4
#define IDX_GET_APP_ATTR_TXT_RSP  5
#define IDX_GET_APP_VAL_TXT_RSP   6
#define IDX_GET_ELEMENT_ATTR_RSP  7
#define MAX_VOLUME 128
#define MAX_LABEL 16
#define MAX_TRANSACTIONS_PER_SESSION 16
#define MAX_CMD_QUEUE_LEN 8
#define PLAY_STATUS_PLAYING 1

#define CHECK_RC_CONNECTED                                                                  \
    BTIF_TRACE_DEBUG("## %s ##", __FUNCTION__);                                            \
    if(btif_rc_cb.rc_connected == FALSE)                                                    \
    {                                                                                       \
        BTIF_TRACE_WARNING("Function %s() called when RC is not connected", __FUNCTION__); \
        return BT_STATUS_NOT_READY;                                                         \
    }

#define CHECK_ESP_RC_CONNECTED       do { \
        BTIF_TRACE_DEBUG("## %s ##", __FUNCTION__); \
        if (btif_rc_cb.rc_connected == FALSE) { \
            BTIF_TRACE_WARNING("Function %s() called when RC is not connected", __FUNCTION__); \
	    return ESP_ERR_INVALID_STATE; \
        } \
    } while (0)

#define FILL_PDU_QUEUE(index, ctype, label, pending)        \
{                                                           \
    btif_rc_cb.rc_pdu_info[index].ctype = ctype;            \
    btif_rc_cb.rc_pdu_info[index].label = label;            \
    btif_rc_cb.rc_pdu_info[index].is_rsp_pending = pending; \
}

#define SEND_METAMSG_RSP(index, avrc_rsp)                                                      \
{                                                                                              \
    if(btif_rc_cb.rc_pdu_info[index].is_rsp_pending == FALSE)                                  \
    {                                                                                          \
        BTIF_TRACE_WARNING("%s Not sending response as no PDU was registered", __FUNCTION__); \
        return BT_STATUS_UNHANDLED;                                                            \
    }                                                                                          \
    send_metamsg_rsp(btif_rc_cb.rc_handle, btif_rc_cb.rc_pdu_info[index].label,                \
        btif_rc_cb.rc_pdu_info[index].ctype, avrc_rsp);                                        \
    btif_rc_cb.rc_pdu_info[index].ctype = 0;                                                   \
    btif_rc_cb.rc_pdu_info[index].label = 0;                                                   \
    btif_rc_cb.rc_pdu_info[index].is_rsp_pending = FALSE;                                      \
}

/*****************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct {
    UINT8 bNotify;
    UINT8 label;
} btif_rc_reg_notifications_t;

typedef struct
{
    UINT8   label;
    UINT8   ctype;
    BOOLEAN is_rsp_pending;
} btif_rc_cmd_ctxt_t;

/* TODO : Merge btif_rc_reg_notifications_t and btif_rc_cmd_ctxt_t to a single struct */
typedef struct {
    BOOLEAN                     rc_connected;
    UINT8                       rc_handle;
    tBTA_AV_FEAT                rc_features;
    BD_ADDR                     rc_addr;
    UINT16                      rc_pending_play;
    btif_rc_cmd_ctxt_t          rc_pdu_info[MAX_CMD_QUEUE_LEN];
    btif_rc_reg_notifications_t rc_notif[MAX_RC_NOTIFICATIONS];
    unsigned int                rc_volume;
    uint8_t                     rc_vol_label;
} btif_rc_cb_t;

typedef struct {
    BOOLEAN in_use;
    UINT8 lbl;
    UINT8 handle;
} rc_transaction_t;

typedef struct
{
    pthread_mutex_t lbllock;
    rc_transaction_t transaction[MAX_TRANSACTIONS_PER_SESSION];
} rc_device_t;


rc_device_t device;

#if UINPUT_INCLUDED
#define MAX_UINPUT_PATHS 3
static const char* uinput_dev_path[] =
                       {"/dev/uinput", "/dev/input/uinput", "/dev/misc/uinput" };
#endif /* UINPUT_INCLUDED */
static int uinput_fd = -1;

static void send_key (int fd, uint16_t key, int pressed);
#if UINPUT_INCLUDED
static int  send_event (int fd, uint16_t type, uint16_t code, int32_t value);
static int  uinput_driver_check();
static int  uinput_create(char *name);
static int  init_uinput (void);
static void close_uinput (void);
#endif /* UINPUT_INCLUDED */

static const struct {
    const char *name;
    uint8_t avrcp;
    uint16_t mapped_id;
    uint8_t release_quirk;
} key_map[] = {
    { "PLAY",         AVRC_ID_PLAY,     KEY_PLAYCD,       1 },
    { "STOP",         AVRC_ID_STOP,     KEY_STOPCD,       0 },
    { "PAUSE",        AVRC_ID_PAUSE,    KEY_PAUSECD,      1 },
    { "FORWARD",      AVRC_ID_FORWARD,  KEY_NEXTSONG,     0 },
    { "BACKWARD",     AVRC_ID_BACKWARD, KEY_PREVIOUSSONG, 0 },
    { "REWIND",       AVRC_ID_REWIND,   KEY_REWIND,       0 },
    { "FAST FORWARD", AVRC_ID_FAST_FOR, KEY_FAST_FORWARD, 0 },
    { NULL,           0,                0,                0 }
};

static void send_reject_response (UINT8 rc_handle, UINT8 label,
    UINT8 pdu, UINT8 status);
static UINT8 opcode_from_pdu(UINT8 pdu);
static void send_metamsg_rsp (UINT8 rc_handle, UINT8 label,
    tBTA_AV_CODE code, tAVRC_RESPONSE *pmetamsg_resp);
static void register_volumechange(UINT8 label);
static void lbl_init();
static void lbl_destroy();
static void init_all_transactions();
static bt_status_t  get_transaction(rc_transaction_t **ptransaction);
static void release_transaction(UINT8 label);
static rc_transaction_t* get_transaction_by_lbl(UINT8 label);
static void handle_rc_metamsg_rsp(tBTA_AV_META_MSG *pmeta_msg);
static void btif_rc_upstreams_evt(UINT16 event, tAVRC_COMMAND* p_param, UINT8 ctype, UINT8 label);
static void btif_rc_upstreams_rsp_evt(UINT16 event, tAVRC_RESPONSE *pavrc_resp, UINT8 ctype, UINT8 label);

/*****************************************************************************
**  Static variables
******************************************************************************/
static btif_rc_cb_t btif_rc_cb;
static btrc_callbacks_t *bt_rc_callbacks = NULL;
// static btrc_ctrl_callbacks_t *bt_rc_ctrl_callbacks = NULL;
static esp_avrc_ct_cb_t bt_rc_ctrl_callback = NULL;

// TODO: need protection against race
#define BTIF_AVRC_CT_CB_TO_APP(_event, _param)    do { \
	if (bt_rc_ctrl_callback) { \
	    bt_rc_ctrl_callback(_event, _param); \
	} \
    } while (0)

/*****************************************************************************
**  Static functions
******************************************************************************/

/*****************************************************************************
**  Externs
******************************************************************************/
#if BTIF_HF_INCLUDED
extern BOOLEAN btif_hf_call_terminated_recently();
#else
BOOLEAN btif_hf_call_terminated_recently()
{
    return FALSE;
}
#endif

#if 0
extern BOOLEAN check_cod(const bt_bdaddr_t *remote_bdaddr, uint32_t cod);
#endif 

/*****************************************************************************
**  Functions
******************************************************************************/

/*****************************************************************************
**   Local uinput helper functions
******************************************************************************/
#if UINPUT_INCLUDED
int send_event (int fd, uint16_t type, uint16_t code, int32_t value)
{
    struct uinput_event event;
    BTIF_TRACE_DEBUG("%s type:%u code:%u value:%d", __FUNCTION__,
        type, code, value);
    memset(&event, 0, sizeof(event));
    event.type  = type;
    event.code  = code;
    event.value = value;

    return write(fd, &event, sizeof(event));
}

void send_key (int fd, uint16_t key, int pressed)
{
    BTIF_TRACE_DEBUG("%s fd:%d key:%u pressed:%d", __FUNCTION__,
        fd, key, pressed);

    if (fd < 0)
    {
        return;
    }

    BTIF_TRACE_DEBUG("AVRCP: Send key %d (%d) fd=%d", key, pressed, fd);
    send_event(fd, EV_KEY, key, pressed);
    send_event(fd, EV_SYN, SYN_REPORT, 0);
}

/************** uinput related functions **************/
int uinput_driver_check()
{
    uint32_t i;
    for (i=0; i < MAX_UINPUT_PATHS; i++)
    {
        if (access(uinput_dev_path[i], O_RDWR) == 0) {
           return 0;
        }
    }
    BTIF_TRACE_ERROR("%s ERROR: uinput device is not in the system", __FUNCTION__);
    return -1;
}

int uinput_create(char *name)
{
    struct uinput_dev dev;
    int fd, x = 0;

    for(x=0; x < MAX_UINPUT_PATHS; x++)
    {
        fd = open(uinput_dev_path[x], O_RDWR);
        if (fd < 0)
            continue;
        break;
    }
    if (x == MAX_UINPUT_PATHS) {
        BTIF_TRACE_ERROR("%s ERROR: uinput device open failed", __FUNCTION__);
        return -1;
    }
    memset(&dev, 0, sizeof(dev));
    if (name)
        strncpy(dev.name, name, UINPUT_MAX_NAME_SIZE-1);

    dev.id.bustype = BUS_BLUETOOTH;
    dev.id.vendor  = 0x0000;
    dev.id.product = 0x0000;
    dev.id.version = 0x0000;

    if (write(fd, &dev, sizeof(dev)) < 0) {
        BTIF_TRACE_ERROR("%s Unable to write device information", __FUNCTION__);
        close(fd);
        return -1;
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    for (x = 0; key_map[x].name != NULL; x++)
        ioctl(fd, UI_SET_KEYBIT, key_map[x].mapped_id);

    if (ioctl(fd, UI_DEV_CREATE, NULL) < 0) {
        BTIF_TRACE_ERROR("%s Unable to create uinput device", __FUNCTION__);
        close(fd);
        return -1;
    }
    return fd;
}

int init_uinput (void)
{
    char *name = "AVRCP";

    BTIF_TRACE_DEBUG("%s", __FUNCTION__);
    uinput_fd = uinput_create(name);
    if (uinput_fd < 0) {
        BTIF_TRACE_ERROR("%s AVRCP: Failed to initialize uinput for %s (%d)",
                          __FUNCTION__, name, uinput_fd);
    } else {
        BTIF_TRACE_DEBUG("%s AVRCP: Initialized uinput for %s (fd=%d)",
                          __FUNCTION__, name, uinput_fd);
    }
    return uinput_fd;
}

void close_uinput (void)
{
    BTIF_TRACE_DEBUG("%s", __FUNCTION__);
    if (uinput_fd > 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);

        close(uinput_fd);
        uinput_fd = -1;
    }
}

#else /* UINPUT_INCLUDED */
void send_key (int fd, uint16_t key, int pressed)
{
    BTIF_TRACE_DEBUG("%s fd:%d key:%u pressed:%d, func not implemented", __FUNCTION__,
        fd, key, pressed);

    return;
}

#endif /* UINPUT_INCLUDED */

void handle_rc_features()
{
    btrc_remote_features_t rc_features = BTRC_FEAT_NONE;
    bt_bdaddr_t rc_addr;
    bdcpy(rc_addr.address, btif_rc_cb.rc_addr);

    // TODO(eisenbach): If devices need to be blacklisted for absolute
    // volume, it should be added to device/include/interop_database.h
    // For now, everything goes... If blacklisting is necessary, exclude
    // the following bit here:
    //    btif_rc_cb.rc_features &= ~BTA_AV_FEAT_ADV_CTRL;

    if (btif_rc_cb.rc_features & BTA_AV_FEAT_BROWSE)
    {
        rc_features |= BTRC_FEAT_BROWSE;
    }

    if ( (btif_rc_cb.rc_features & BTA_AV_FEAT_ADV_CTRL) &&
         (btif_rc_cb.rc_features & BTA_AV_FEAT_RCTG))
    {
        rc_features |= BTRC_FEAT_ABSOLUTE_VOLUME;
    }

    if (btif_rc_cb.rc_features & BTA_AV_FEAT_METADATA)
    {
        rc_features |= BTRC_FEAT_METADATA;
    }

    BTIF_TRACE_DEBUG("%s: rc_features=0x%x", __FUNCTION__, rc_features);
    // todo: uncomment the following line when added the AVRC target role
    // HAL_CBACK(bt_rc_callbacks, remote_features_cb, &rc_addr, rc_features)
    
#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
     BTIF_TRACE_DEBUG("Checking for feature flags in btif_rc_handler with label %d",
                        btif_rc_cb.rc_vol_label);
     // Register for volume change on connect
     if(btif_rc_cb.rc_features & BTA_AV_FEAT_ADV_CTRL &&
	btif_rc_cb.rc_features & BTA_AV_FEAT_RCTG)
     {
         rc_transaction_t *p_transaction=NULL;
         bt_status_t status = BT_STATUS_NOT_READY;
         if(MAX_LABEL==btif_rc_cb.rc_vol_label)
         {
            status=get_transaction(&p_transaction);
         }
         else
         {
            p_transaction=get_transaction_by_lbl(btif_rc_cb.rc_vol_label);
            if(NULL!=p_transaction)
            {
               BTIF_TRACE_DEBUG("register_volumechange already in progress for label %d",
                                  btif_rc_cb.rc_vol_label);
               return;
            }
            else
              status=get_transaction(&p_transaction);
         }

         if(BT_STATUS_SUCCESS == status && NULL!=p_transaction)
         {
            btif_rc_cb.rc_vol_label=p_transaction->lbl;
            register_volumechange(btif_rc_cb.rc_vol_label);
         }
    }
#endif
}


/***************************************************************************
 *  Function       handle_rc_connect
 *
 *  - Argument:    tBTA_AV_RC_OPEN  RC open data structure
 *
 *  - Description: RC connection event handler
 *
 ***************************************************************************/
void handle_rc_connect (tBTA_AV_RC_OPEN *p_rc_open)
{
    BTIF_TRACE_DEBUG("%s: rc_handle: %d", __FUNCTION__, p_rc_open->rc_handle);
#if (AVRC_CTLR_INCLUDED == TRUE)
    bt_bdaddr_t rc_addr;
#endif

    if(p_rc_open->status == BTA_AV_SUCCESS)
    {
        //check if already some RC is connected
        if (btif_rc_cb.rc_connected)
        {
            BTIF_TRACE_ERROR("Got RC OPEN in connected state, Connected RC: %d \
                and Current RC: %d", btif_rc_cb.rc_handle,p_rc_open->rc_handle );
            if ((btif_rc_cb.rc_handle != p_rc_open->rc_handle)
                && (bdcmp(btif_rc_cb.rc_addr, p_rc_open->peer_addr)))
            {
                BTIF_TRACE_DEBUG("Got RC connected for some other handle");
                BTA_AvCloseRc(p_rc_open->rc_handle);
                return;
            }
        }
        memcpy(btif_rc_cb.rc_addr, p_rc_open->peer_addr, sizeof(BD_ADDR));
        btif_rc_cb.rc_features = p_rc_open->peer_features;
        btif_rc_cb.rc_vol_label=MAX_LABEL;
        btif_rc_cb.rc_volume=MAX_VOLUME;

        btif_rc_cb.rc_connected = TRUE;
        btif_rc_cb.rc_handle = p_rc_open->rc_handle;

        /* on locally initiated connection we will get remote features as part of connect */
        if (btif_rc_cb.rc_features != 0)
            handle_rc_features();
#if UINPUT_INCLUDED
        if(uinput_driver_check() == BT_STATUS_SUCCESS)
        {
            init_uinput();
        }
#endif /* UINPUT_INCLUDED */
#if (AVRC_CTLR_INCLUDED == TRUE)
        bdcpy(rc_addr.address, btif_rc_cb.rc_addr);
        /* report connection state if device is AVRCP target */
        if (btif_rc_cb.rc_features & BTA_AV_FEAT_RCTG) {
	    esp_avrc_ct_cb_param_t param;
	    memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
	    param.conn_stat.connected = true;
	    param.conn_stat.feat_mask = btif_rc_cb.rc_features;
	    memcpy(param.conn_stat.remote_bda, &rc_addr, sizeof(esp_bd_addr_t));
	    BTIF_AVRC_CT_CB_TO_APP(ESP_AVRC_CT_CONNECTION_STATE_EVT, &param);
        }
#endif
    }
    else
    {
        BTIF_TRACE_ERROR("%s Connect failed with error code: %d",
            __FUNCTION__, p_rc_open->status);
        btif_rc_cb.rc_connected = FALSE;
    }
}

/***************************************************************************
 *  Function       handle_rc_disconnect
 *
 *  - Argument:    tBTA_AV_RC_CLOSE     RC close data structure
 *
 *  - Description: RC disconnection event handler
 *
 ***************************************************************************/
void handle_rc_disconnect (tBTA_AV_RC_CLOSE *p_rc_close)
{
#if (AVRC_CTLR_INCLUDED == TRUE)
    bt_bdaddr_t rc_addr;
    tBTA_AV_FEAT features;
#endif
    BTIF_TRACE_DEBUG("%s: rc_handle: %d", __FUNCTION__, p_rc_close->rc_handle);
    if ((p_rc_close->rc_handle != btif_rc_cb.rc_handle)
        && (bdcmp(btif_rc_cb.rc_addr, p_rc_close->peer_addr)))
    {
        BTIF_TRACE_ERROR("Got disconnect of unknown device");
        return;
    }

    btif_rc_cb.rc_handle = 0;
    btif_rc_cb.rc_connected = FALSE;
    memset(btif_rc_cb.rc_addr, 0, sizeof(BD_ADDR));
    memset(btif_rc_cb.rc_notif, 0, sizeof(btif_rc_cb.rc_notif));
#if (AVRC_CTLR_INCLUDED == TRUE)
    features = btif_rc_cb.rc_features;
#endif
    btif_rc_cb.rc_features = 0;
    btif_rc_cb.rc_vol_label=MAX_LABEL;
    btif_rc_cb.rc_volume=MAX_VOLUME;
    init_all_transactions();
#if UINPUT_INCLUDED
    close_uinput();
#endif /* UINPUT_INCLUDED */
#if (AVRC_CTLR_INCLUDED == TRUE)
    bdcpy(rc_addr.address, btif_rc_cb.rc_addr);
#endif
    memset(btif_rc_cb.rc_addr, 0, sizeof(BD_ADDR));
#if (AVRC_CTLR_INCLUDED == TRUE)
    /* report connection state if device is AVRCP target */
    if (features & BTA_AV_FEAT_RCTG) {
	esp_avrc_ct_cb_param_t param;
	memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
	param.conn_stat.connected = false;
	memcpy(param.conn_stat.remote_bda, &rc_addr, sizeof(esp_bd_addr_t));
	BTIF_AVRC_CT_CB_TO_APP(ESP_AVRC_CT_CONNECTION_STATE_EVT, &param);
    }
#endif
}

/***************************************************************************
 *  Function       handle_rc_passthrough_cmd
 *
 *  - Argument:    tBTA_AV_RC rc_id   remote control command ID
 *                 tBTA_AV_STATE key_state status of key press
 *
 *  - Description: Remote control command handler
 *
 ***************************************************************************/
void handle_rc_passthrough_cmd ( tBTA_AV_REMOTE_CMD *p_remote_cmd)
{
    const char *status;
    int pressed, i;

    BTIF_TRACE_DEBUG("%s: p_remote_cmd->rc_id=%d", __FUNCTION__, p_remote_cmd->rc_id);

    /* If AVRC is open and peer sends PLAY but there is no AVDT, then we queue-up this PLAY */
    if (p_remote_cmd)
    {
        /* queue AVRC PLAY if GAVDTP Open notification to app is pending (2 second timer) */
        if ((p_remote_cmd->rc_id == BTA_AV_RC_PLAY) && (!btif_av_is_connected()))
        {
            if (p_remote_cmd->key_state == AVRC_STATE_PRESS)
            {
                APPL_TRACE_WARNING("%s: AVDT not open, queuing the PLAY command", __FUNCTION__);
                btif_rc_cb.rc_pending_play = TRUE;
            }
            return;
        }

        if ((p_remote_cmd->rc_id == BTA_AV_RC_PAUSE) && (btif_rc_cb.rc_pending_play))
        {
            APPL_TRACE_WARNING("%s: Clear the pending PLAY on PAUSE received", __FUNCTION__);
            btif_rc_cb.rc_pending_play = FALSE;
            return;
        }
    }

    if ((p_remote_cmd->rc_id == BTA_AV_RC_STOP) && (!btif_av_stream_started_ready()))
    {
        APPL_TRACE_WARNING("%s: Stream suspended, ignore STOP cmd",__FUNCTION__);
        return;
    }

    if (p_remote_cmd->key_state == AVRC_STATE_RELEASE) {
        status = "released";
        pressed = 0;
    } else {
        status = "pressed";
        pressed = 1;
    }

    /* If this is Play/Pause command (press or release)  before processing, check the following
     * a voice call has ended recently
     * the remote device is not of type headset
     * If the above conditions meet, drop the Play/Pause command
     * This fix is to interop with certain carkits which sends an automatic  PLAY  or PAUSE
     * commands right after call ends
     */
    if((p_remote_cmd->rc_id == BTA_AV_RC_PLAY || p_remote_cmd->rc_id == BTA_AV_RC_PAUSE)&&
       (btif_hf_call_terminated_recently() == TRUE)
#if 0 // temporary hack since no btif_storage module is not ported for now
       && (check_cod( (const bt_bdaddr_t*)&(btif_rc_cb.rc_addr), COD_AV_HEADSETS) != TRUE)
#endif
       )
    {
        BTIF_TRACE_DEBUG("%s:Dropping the play/Pause command received right after call end cmd:%d",
                           __FUNCTION__,p_remote_cmd->rc_id);
        return;
    }

    if (p_remote_cmd->rc_id == BTA_AV_RC_FAST_FOR || p_remote_cmd->rc_id == BTA_AV_RC_REWIND) {
        HAL_CBACK(bt_rc_callbacks, passthrough_cmd_cb, p_remote_cmd->rc_id, pressed);
        return;
    }

    for (i = 0; key_map[i].name != NULL; i++) {
        if (p_remote_cmd->rc_id == key_map[i].avrcp) {
            BTIF_TRACE_DEBUG("%s: %s %s", __FUNCTION__, key_map[i].name, status);

           /* MusicPlayer uses a long_press_timeout of 1 second for PLAYPAUSE button
            * and maps that to autoshuffle. So if for some reason release for PLAY/PAUSE
            * comes 1 second after the press, the MediaPlayer UI goes into a bad state.
            * The reason for the delay could be sniff mode exit or some AVDTP procedure etc.
            * The fix is to generate a release right after the press and drown the 'actual'
            * release.
            */
            if ((key_map[i].release_quirk == 1) && (pressed == 0))
            {
                BTIF_TRACE_DEBUG("%s: AVRC %s Release Faked earlier, drowned now",
                                  __FUNCTION__, key_map[i].name);
                return;
            }
            send_key(uinput_fd, key_map[i].mapped_id, pressed);
            if ((key_map[i].release_quirk == 1) && (pressed == 1))
            {
                GKI_delay(30); // 30ms
                BTIF_TRACE_DEBUG("%s: AVRC %s Release quirk enabled, send release now",
                                  __FUNCTION__, key_map[i].name);
                send_key(uinput_fd, key_map[i].mapped_id, 0);
            }
            break;
        }
    }

    if (key_map[i].name == NULL)
        BTIF_TRACE_ERROR("%s AVRCP: unknown button 0x%02X %s", __FUNCTION__,
                        p_remote_cmd->rc_id, status);
}

/***************************************************************************
 *  Function       handle_rc_passthrough_rsp
 *
 *  - Argument:    tBTA_AV_REMOTE_RSP passthrough command response
 *
 *  - Description: Remote control passthrough response handler
 *
 ***************************************************************************/
void handle_rc_passthrough_rsp ( tBTA_AV_REMOTE_RSP *p_remote_rsp)
{
#if (AVRC_CTLR_INCLUDED == TRUE)
    const char *status;
    if (btif_rc_cb.rc_features & BTA_AV_FEAT_RCTG)
    {
        int key_state;
        if (p_remote_rsp->key_state == AVRC_STATE_RELEASE)
        {
            status = "released";
            key_state = 1;
        }
        else
        {
            status = "pressed";
            key_state = 0;
        }

        BTIF_TRACE_DEBUG("%s: rc_id=%d status=%s", __FUNCTION__, p_remote_rsp->rc_id, status);

	do {
	    esp_avrc_ct_cb_param_t param;
	    memset(&param, 0, sizeof(esp_avrc_ct_cb_param_t));
            param.psth_rsp.tl = p_remote_rsp->label;
	    param.psth_rsp.key_code = p_remote_rsp->rc_id;
	    param.psth_rsp.key_state = key_state;
	    BTIF_AVRC_CT_CB_TO_APP(ESP_AVRC_CT_PASSTHROUGH_RSP_EVT, &param);
	} while (0);
    }
    else
    {
        BTIF_TRACE_ERROR("%s DUT does not support AVRCP controller role", __FUNCTION__);
    }
#else
    BTIF_TRACE_ERROR("%s AVRCP controller role is not enabled", __FUNCTION__);
#endif
}

void handle_uid_changed_notification(tBTA_AV_META_MSG *pmeta_msg, tAVRC_COMMAND *pavrc_command)
{
    tAVRC_RESPONSE avrc_rsp = {0};
    avrc_rsp.rsp.pdu = pavrc_command->pdu;
    avrc_rsp.rsp.status = AVRC_STS_NO_ERROR;
    avrc_rsp.rsp.opcode = pavrc_command->cmd.opcode;

    avrc_rsp.reg_notif.event_id = pavrc_command->reg_notif.event_id;
    avrc_rsp.reg_notif.param.uid_counter = 0;

    send_metamsg_rsp(pmeta_msg->rc_handle, pmeta_msg->label, AVRC_RSP_INTERIM, &avrc_rsp);
    send_metamsg_rsp(pmeta_msg->rc_handle, pmeta_msg->label, AVRC_RSP_CHANGED, &avrc_rsp);

}


/***************************************************************************
 *  Function       handle_rc_metamsg_cmd
 *
 *  - Argument:    tBTA_AV_VENDOR Structure containing the received
 *                          metamsg command
 *
 *  - Description: Remote control metamsg command handler (AVRCP 1.3)
 *
 ***************************************************************************/
void handle_rc_metamsg_cmd (tBTA_AV_META_MSG *pmeta_msg)
{
    /* Parse the metamsg command and pass it on to BTL-IFS */
    UINT8             scratch_buf[512] = {0};
    tAVRC_COMMAND    avrc_command = {0};
    tAVRC_STS status;

    BTIF_TRACE_EVENT("+ %s", __FUNCTION__);

    if (pmeta_msg->p_msg->hdr.opcode != AVRC_OP_VENDOR)
    {
        BTIF_TRACE_WARNING("Invalid opcode: %x", pmeta_msg->p_msg->hdr.opcode);
        return;
    }
    if (pmeta_msg->len < 3)
    {
        BTIF_TRACE_WARNING("Invalid length.Opcode: 0x%x, len: 0x%x", pmeta_msg->p_msg->hdr.opcode,
            pmeta_msg->len);
        return;
    }

    if (pmeta_msg->code >= AVRC_RSP_NOT_IMPL)
    {
#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
{
     rc_transaction_t *transaction=NULL;
     transaction=get_transaction_by_lbl(pmeta_msg->label);
     if(NULL!=transaction)
     {
        handle_rc_metamsg_rsp(pmeta_msg);
     }
     else
     {
         BTIF_TRACE_DEBUG("%s:Discard vendor dependent rsp. code: %d label:%d.",
             __FUNCTION__, pmeta_msg->code, pmeta_msg->label);
     }
     return;
}
#else
{
        BTIF_TRACE_DEBUG("%s:Received vendor dependent rsp. code: %d len: %d. Not processing it.",
            __FUNCTION__, pmeta_msg->code, pmeta_msg->len);
        return;
}
#endif
      }

    status=AVRC_ParsCommand(pmeta_msg->p_msg, &avrc_command, scratch_buf, sizeof(scratch_buf));
    BTIF_TRACE_DEBUG("Received vendor command.code,PDU and label: %d, %d,%d",pmeta_msg->code,
                       avrc_command.cmd.pdu, pmeta_msg->label);

    if (status != AVRC_STS_NO_ERROR)
    {
        /* return error */
        BTIF_TRACE_WARNING("%s: Error in parsing received metamsg command. status: 0x%02x",
            __FUNCTION__, status);
        send_reject_response(pmeta_msg->rc_handle, pmeta_msg->label, avrc_command.pdu, status);
    }
    else
    {
        /* if RegisterNotification, add it to our registered queue */

        if (avrc_command.cmd.pdu == AVRC_PDU_REGISTER_NOTIFICATION)
        {
            UINT8 event_id = avrc_command.reg_notif.event_id;
            BTIF_TRACE_EVENT("%s:New register notification received.event_id:%s,label:0x%x,code:%x",
            __FUNCTION__,dump_rc_notification_event_id(event_id), pmeta_msg->label,pmeta_msg->code);
            btif_rc_cb.rc_notif[event_id-1].bNotify = TRUE;
            btif_rc_cb.rc_notif[event_id-1].label = pmeta_msg->label;

            if(event_id == AVRC_EVT_UIDS_CHANGE)
            {
                handle_uid_changed_notification(pmeta_msg, &avrc_command);
                return;
            }

        }

        BTIF_TRACE_EVENT("%s: Passing received metamsg command to app. pdu: %s",
            __FUNCTION__, dump_rc_pdu(avrc_command.cmd.pdu));

        /* Since handle_rc_metamsg_cmd() itself is called from
         * btif context, no context switching is required. Invoke
         * btif_rc_upstreams_evt directly from here. */
        btif_rc_upstreams_evt((uint16_t)avrc_command.cmd.pdu, &avrc_command, pmeta_msg->code,
                               pmeta_msg->label);
    }
}

/***************************************************************************
 **
 ** Function       btif_rc_handler
 **
 ** Description    RC event handler
 **
 ***************************************************************************/
void btif_rc_handler(tBTA_AV_EVT event, tBTA_AV *p_data)
{
    BTIF_TRACE_DEBUG ("%s event:%s", __FUNCTION__, dump_rc_event(event));
    switch (event)
    {
        case BTA_AV_RC_OPEN_EVT:
        {
            BTIF_TRACE_DEBUG("Peer_features:%x", p_data->rc_open.peer_features);
            handle_rc_connect( &(p_data->rc_open) );
        }break;

        case BTA_AV_RC_CLOSE_EVT:
        {
            handle_rc_disconnect( &(p_data->rc_close) );
        }break;

        case BTA_AV_REMOTE_CMD_EVT:
        {
            BTIF_TRACE_DEBUG("rc_id:0x%x key_state:%d", p_data->remote_cmd.rc_id,
                               p_data->remote_cmd.key_state);
            handle_rc_passthrough_cmd( (&p_data->remote_cmd) );
        }
        break;
#if (AVRC_CTLR_INCLUDED == TRUE)
        case BTA_AV_REMOTE_RSP_EVT:
        {
            BTIF_TRACE_DEBUG("RSP: rc_id:0x%x key_state:%d", p_data->remote_rsp.rc_id,
                               p_data->remote_rsp.key_state);
            handle_rc_passthrough_rsp( (&p_data->remote_rsp) );
        }
        break;
#endif
        case BTA_AV_RC_FEAT_EVT:
        {
            BTIF_TRACE_DEBUG("Peer_features:%x", p_data->rc_feat.peer_features);
            btif_rc_cb.rc_features = p_data->rc_feat.peer_features;
            handle_rc_features();
        }
        break;
        case BTA_AV_META_MSG_EVT:
        {
            BTIF_TRACE_DEBUG("BTA_AV_META_MSG_EVT  code:%d label:%d", p_data->meta_msg.code,
                p_data->meta_msg.label);
            BTIF_TRACE_DEBUG("  company_id:0x%x len:%d handle:%d", p_data->meta_msg.company_id,
                p_data->meta_msg.len, p_data->meta_msg.rc_handle);
            /* handle the metamsg command */
            handle_rc_metamsg_cmd(&(p_data->meta_msg));
        }
        break;
        default:
            BTIF_TRACE_DEBUG("Unhandled RC event : 0x%x", event);
    }
}

/***************************************************************************
 **
 ** Function       btif_rc_get_connected_peer
 **
 ** Description    Fetches the connected headset's BD_ADDR if any
 **
 ***************************************************************************/
BOOLEAN btif_rc_get_connected_peer(BD_ADDR peer_addr)
{
    if (btif_rc_cb.rc_connected == TRUE) {
        bdcpy(peer_addr, btif_rc_cb.rc_addr);
        return TRUE;
    }
    return FALSE;
}

/***************************************************************************
 **
 ** Function       btif_rc_check_handle_pending_play
 **
 ** Description    Clears the queued PLAY command. if bSend is TRUE, forwards to app
 **
 ***************************************************************************/

/* clear the queued PLAY command. if bSend is TRUE, forward to app */
void btif_rc_check_handle_pending_play (BD_ADDR peer_addr, BOOLEAN bSendToApp)
{
    UNUSED(peer_addr);

    BTIF_TRACE_DEBUG("%s: bSendToApp=%d", __FUNCTION__, bSendToApp);
    if (btif_rc_cb.rc_pending_play)
    {
        if (bSendToApp)
        {
            tBTA_AV_REMOTE_CMD remote_cmd;
            APPL_TRACE_DEBUG("%s: Sending queued PLAYED event to app", __FUNCTION__);

            memset (&remote_cmd, 0, sizeof(tBTA_AV_REMOTE_CMD));
            remote_cmd.rc_handle  = btif_rc_cb.rc_handle;
            remote_cmd.rc_id      = AVRC_ID_PLAY;
            remote_cmd.hdr.ctype  = AVRC_CMD_CTRL;
            remote_cmd.hdr.opcode = AVRC_OP_PASS_THRU;

            /* delay sending to app, else there is a timing issue in the framework,
             ** which causes the audio to be on th device's speaker. Delay between
             ** OPEN & RC_PLAYs
            */
            GKI_delay (200);
            /* send to app - both PRESSED & RELEASED */
            remote_cmd.key_state  = AVRC_STATE_PRESS;
            handle_rc_passthrough_cmd( &remote_cmd );

            GKI_delay (100);

            remote_cmd.key_state  = AVRC_STATE_RELEASE;
            handle_rc_passthrough_cmd( &remote_cmd );
        }
        btif_rc_cb.rc_pending_play = FALSE;
    }
}

/* Generic reject response */
static void send_reject_response (UINT8 rc_handle, UINT8 label, UINT8 pdu, UINT8 status)
{
    UINT8 ctype = AVRC_RSP_REJ;
    tAVRC_RESPONSE avrc_rsp;
    BT_HDR *p_msg = NULL;
    memset (&avrc_rsp, 0, sizeof(tAVRC_RESPONSE));

    avrc_rsp.rsp.opcode = opcode_from_pdu(pdu);
    avrc_rsp.rsp.pdu    = pdu;
    avrc_rsp.rsp.status = status;

    if (AVRC_STS_NO_ERROR == (status = AVRC_BldResponse(rc_handle, &avrc_rsp, &p_msg)) )
    {
        BTIF_TRACE_DEBUG("%s:Sending error notification to handle:%d. pdu:%s,status:0x%02x",
            __FUNCTION__, rc_handle, dump_rc_pdu(pdu), status);
        BTA_AvMetaRsp(rc_handle, label, ctype, p_msg);
    }
}

/***************************************************************************
 *  Function       send_metamsg_rsp
 *
 *  - Argument:
 *                  rc_handle     RC handle corresponding to the connected RC
 *                  label            Label of the RC response
 *                  code            Response type
 *                  pmetamsg_resp    Vendor response
 *
 *  - Description: Remote control metamsg response handler (AVRCP 1.3)
 *
 ***************************************************************************/
static void send_metamsg_rsp (UINT8 rc_handle, UINT8 label, tBTA_AV_CODE code,
    tAVRC_RESPONSE *pmetamsg_resp)
{
    UINT8 ctype;

    if (!pmetamsg_resp)
    {
        BTIF_TRACE_WARNING("%s: Invalid response received from application", __FUNCTION__);
        return;
    }

    BTIF_TRACE_EVENT("+%s: rc_handle: %d, label: %d, code: 0x%02x, pdu: %s", __FUNCTION__,
        rc_handle, label, code, dump_rc_pdu(pmetamsg_resp->rsp.pdu));

    if (pmetamsg_resp->rsp.status != AVRC_STS_NO_ERROR)
    {
        ctype = AVRC_RSP_REJ;
    }
    else
    {
        if ( code < AVRC_RSP_NOT_IMPL)
        {
            if (code == AVRC_CMD_NOTIF)
            {
               ctype = AVRC_RSP_INTERIM;
            }
            else if (code == AVRC_CMD_STATUS)
            {
               ctype = AVRC_RSP_IMPL_STBL;
            }
            else
            {
               ctype = AVRC_RSP_ACCEPT;
            }
        }
        else
        {
            ctype = code;
        }
    }
    /* if response is for register_notification, make sure the rc has
    actually registered for this */
    if((pmetamsg_resp->rsp.pdu == AVRC_PDU_REGISTER_NOTIFICATION) && (code == AVRC_RSP_CHANGED))
    {
        BOOLEAN bSent = FALSE;
        UINT8   event_id = pmetamsg_resp->reg_notif.event_id;
        BOOLEAN bNotify = (btif_rc_cb.rc_connected) && (btif_rc_cb.rc_notif[event_id-1].bNotify);

        /* de-register this notification for a CHANGED response */
        btif_rc_cb.rc_notif[event_id-1].bNotify = FALSE;
        BTIF_TRACE_DEBUG("%s rc_handle: %d. event_id: 0x%02d bNotify:%u", __FUNCTION__,
             btif_rc_cb.rc_handle, event_id, bNotify);
        if (bNotify)
        {
            BT_HDR *p_msg = NULL;
            tAVRC_STS status;

            if (AVRC_STS_NO_ERROR == (status = AVRC_BldResponse(btif_rc_cb.rc_handle,
                pmetamsg_resp, &p_msg)) )
            {
                BTIF_TRACE_DEBUG("%s Sending notification to rc_handle: %d. event_id: 0x%02d",
                     __FUNCTION__, btif_rc_cb.rc_handle, event_id);
                bSent = TRUE;
                BTA_AvMetaRsp(btif_rc_cb.rc_handle, btif_rc_cb.rc_notif[event_id-1].label,
                    ctype, p_msg);
            }
            else
            {
                BTIF_TRACE_WARNING("%s failed to build metamsg response. status: 0x%02x",
                    __FUNCTION__, status);
            }

        }

        if (!bSent)
        {
            BTIF_TRACE_DEBUG("%s: Notification not sent, as there are no RC connections or the \
                CT has not subscribed for event_id: %s", __FUNCTION__, dump_rc_notification_event_id(event_id));
        }
    }
    else
    {
        /* All other commands go here */

        BT_HDR *p_msg = NULL;
        tAVRC_STS status;

        status = AVRC_BldResponse(rc_handle, pmetamsg_resp, &p_msg);

        if (status == AVRC_STS_NO_ERROR)
        {
            BTA_AvMetaRsp(rc_handle, label, ctype, p_msg);
        }
        else
        {
            BTIF_TRACE_ERROR("%s: failed to build metamsg response. status: 0x%02x",
                __FUNCTION__, status);
        }
    }
}

static UINT8 opcode_from_pdu(UINT8 pdu)
{
    UINT8 opcode = 0;

    switch (pdu)
    {
    case AVRC_PDU_NEXT_GROUP:
    case AVRC_PDU_PREV_GROUP: /* pass thru */
        opcode  = AVRC_OP_PASS_THRU;
        break;

    default: /* vendor */
        opcode  = AVRC_OP_VENDOR;
        break;
    }

    return opcode;
}

/*******************************************************************************
**
** Function         btif_rc_upstreams_evt
**
** Description      Executes AVRC UPSTREAMS events in btif context.
**
** Returns          void
**
*******************************************************************************/
static void btif_rc_upstreams_evt(UINT16 event, tAVRC_COMMAND *pavrc_cmd, UINT8 ctype, UINT8 label)
{
    BTIF_TRACE_EVENT("%s pdu: %s handle: 0x%x ctype:%x label:%x", __FUNCTION__,
        dump_rc_pdu(pavrc_cmd->pdu), btif_rc_cb.rc_handle, ctype, label);

    switch (event)
    {
        case AVRC_PDU_GET_PLAY_STATUS:
        {
            FILL_PDU_QUEUE(IDX_GET_PLAY_STATUS_RSP, ctype, label, TRUE)
            HAL_CBACK(bt_rc_callbacks, get_play_status_cb);
        }
        break;
        case AVRC_PDU_LIST_PLAYER_APP_ATTR:
        case AVRC_PDU_LIST_PLAYER_APP_VALUES:
        case AVRC_PDU_GET_CUR_PLAYER_APP_VALUE:
        case AVRC_PDU_SET_PLAYER_APP_VALUE:
        case AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT:
        case AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT:
        {
            /* TODO: Add support for Application Settings */
            send_reject_response (btif_rc_cb.rc_handle, label, pavrc_cmd->pdu, AVRC_STS_BAD_CMD);
        }
        break;
        case AVRC_PDU_GET_ELEMENT_ATTR:
        {
            btrc_media_attr_t element_attrs[BTRC_MAX_ELEM_ATTR_SIZE];
            UINT8 num_attr;
             memset(&element_attrs, 0, sizeof(element_attrs));
            if (pavrc_cmd->get_elem_attrs.num_attr == 0)
            {
                /* CT requests for all attributes */
                int attr_cnt;
                num_attr = BTRC_MAX_ELEM_ATTR_SIZE;
                for (attr_cnt = 0; attr_cnt < BTRC_MAX_ELEM_ATTR_SIZE; attr_cnt++)
                {
                    element_attrs[attr_cnt] = attr_cnt + 1;
                }
            }
            else if (pavrc_cmd->get_elem_attrs.num_attr == 0xFF)
            {
                /* 0xff indicates, no attributes requested - reject */
                send_reject_response (btif_rc_cb.rc_handle, label, pavrc_cmd->pdu,
                    AVRC_STS_BAD_PARAM);
                return;
            }
            else
            {
                int attr_cnt, filled_attr_count;

                num_attr = 0;
                /* Attribute IDs from 1 to AVRC_MAX_NUM_MEDIA_ATTR_ID are only valid,
                 * hence HAL definition limits the attributes to AVRC_MAX_NUM_MEDIA_ATTR_ID.
                 * Fill only valid entries.
                 */
                for (attr_cnt = 0; (attr_cnt < pavrc_cmd->get_elem_attrs.num_attr) &&
                    (num_attr < AVRC_MAX_NUM_MEDIA_ATTR_ID); attr_cnt++)
                {
                    if ((pavrc_cmd->get_elem_attrs.attrs[attr_cnt] > 0) &&
                        (pavrc_cmd->get_elem_attrs.attrs[attr_cnt] <= AVRC_MAX_NUM_MEDIA_ATTR_ID))
                    {
                        /* Skip the duplicate entries : PTS sends duplicate entries for Fragment cases
                         */
                        for (filled_attr_count = 0; filled_attr_count < num_attr; filled_attr_count++)
                        {
                            if (element_attrs[filled_attr_count] == pavrc_cmd->get_elem_attrs.attrs[attr_cnt])
                                break;
                        }
                        if (filled_attr_count == num_attr)
                        {
                            element_attrs[num_attr] = pavrc_cmd->get_elem_attrs.attrs[attr_cnt];
                            num_attr++;
                        }
                    }
                }
            }
            FILL_PDU_QUEUE(IDX_GET_ELEMENT_ATTR_RSP, ctype, label, TRUE);
            HAL_CBACK(bt_rc_callbacks, get_element_attr_cb, num_attr, element_attrs);
        }
        break;
        case AVRC_PDU_REGISTER_NOTIFICATION:
        {
            if(pavrc_cmd->reg_notif.event_id == BTRC_EVT_PLAY_POS_CHANGED &&
                pavrc_cmd->reg_notif.param == 0)
            {
                BTIF_TRACE_WARNING("%s Device registering position changed with illegal param 0.",
                    __FUNCTION__);
                send_reject_response (btif_rc_cb.rc_handle, label, pavrc_cmd->pdu, AVRC_STS_BAD_PARAM);
                /* de-register this notification for a rejected response */
                btif_rc_cb.rc_notif[BTRC_EVT_PLAY_POS_CHANGED - 1].bNotify = FALSE;
                return;
            }
            HAL_CBACK(bt_rc_callbacks, register_notification_cb, pavrc_cmd->reg_notif.event_id,
                pavrc_cmd->reg_notif.param);
        }
        break;
        case AVRC_PDU_INFORM_DISPLAY_CHARSET:
        {
            tAVRC_RESPONSE avrc_rsp;
            BTIF_TRACE_EVENT("%s() AVRC_PDU_INFORM_DISPLAY_CHARSET", __FUNCTION__);
            if(btif_rc_cb.rc_connected == TRUE)
            {
                memset(&(avrc_rsp.inform_charset), 0, sizeof(tAVRC_RSP));
                avrc_rsp.inform_charset.opcode=opcode_from_pdu(AVRC_PDU_INFORM_DISPLAY_CHARSET);
                avrc_rsp.inform_charset.pdu=AVRC_PDU_INFORM_DISPLAY_CHARSET;
                avrc_rsp.inform_charset.status=AVRC_STS_NO_ERROR;
                send_metamsg_rsp(btif_rc_cb.rc_handle, label, ctype, &avrc_rsp);
            }
        }
        break;
        default:
        {
        send_reject_response (btif_rc_cb.rc_handle, label, pavrc_cmd->pdu,
            (pavrc_cmd->pdu == AVRC_PDU_SEARCH)?AVRC_STS_SEARCH_NOT_SUP:AVRC_STS_BAD_CMD);
        return;
        }
        break;
    }

}


/*******************************************************************************
**
** Function         btif_rc_upstreams_rsp_evt
**
** Description      Executes AVRC UPSTREAMS response events in btif context.
**
** Returns          void
**
*******************************************************************************/
static void btif_rc_upstreams_rsp_evt(UINT16 event, tAVRC_RESPONSE *pavrc_resp, UINT8 ctype, UINT8 label)
{
    BTIF_TRACE_EVENT("%s pdu: %s handle: 0x%x ctype:%x label:%x", __FUNCTION__,
        dump_rc_pdu(pavrc_resp->pdu), btif_rc_cb.rc_handle, ctype, label);

#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
    switch (event)
    {
        case AVRC_PDU_REGISTER_NOTIFICATION:
        {
             if(AVRC_RSP_CHANGED==ctype)
                 btif_rc_cb.rc_volume=pavrc_resp->reg_notif.param.volume;
             HAL_CBACK(bt_rc_callbacks, volume_change_cb, pavrc_resp->reg_notif.param.volume,ctype)
        }
        break;

        case AVRC_PDU_SET_ABSOLUTE_VOLUME:
        {
            BTIF_TRACE_DEBUG("Set absolute volume change event received: volume %d,ctype %d",
                pavrc_resp->volume.volume,ctype);
            if(AVRC_RSP_ACCEPT==ctype)
                btif_rc_cb.rc_volume=pavrc_resp->volume.volume;
            HAL_CBACK(bt_rc_callbacks,volume_change_cb,pavrc_resp->volume.volume,ctype)
        }
        break;

        default:
            return;
    }
#endif
}

/************************************************************************************
**  AVRCP API Functions
************************************************************************************/

/*******************************************************************************
**
** Function         init
**
** Description      Initializes the AVRC interface
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t init(btrc_callbacks_t* callbacks )
{
    BTIF_TRACE_EVENT("## %s ##", __FUNCTION__);
    bt_status_t result = BT_STATUS_SUCCESS;

    if (bt_rc_callbacks)
        return BT_STATUS_DONE;

    bt_rc_callbacks = callbacks;
    memset (&btif_rc_cb, 0, sizeof(btif_rc_cb));
    btif_rc_cb.rc_vol_label=MAX_LABEL;
    btif_rc_cb.rc_volume=MAX_VOLUME;
    lbl_init();

    return result;
}


/*******************************************************************************
**
** Function         esp_avrc_ct_register_callback
**
** Description      Register AVRCP controller callback function
**
** Returns          esp_err_t
**
*******************************************************************************/
esp_err_t esp_avrc_ct_register_callback(esp_avrc_ct_cb_t callback)
{
    if (bt_rc_ctrl_callback)
	return ESP_FAIL;

    bt_rc_ctrl_callback = callback;
    return ESP_OK;
}


/*******************************************************************************
**
** Function         esp_avrc_ct_init
**
** Description      Initializes the AVRC interface
**
** Returns          esp_err_t
**
*******************************************************************************/
esp_err_t esp_avrc_ct_init(void)
{
    BTIF_TRACE_EVENT("## %s ##", __FUNCTION__);
    esp_err_t result = ESP_OK;

    memset (&btif_rc_cb, 0, sizeof(btif_rc_cb));
    btif_rc_cb.rc_vol_label=MAX_LABEL;
    btif_rc_cb.rc_volume=MAX_VOLUME;
    lbl_init();

    return result;
}

/***************************************************************************
**
** Function         get_play_status_rsp
**
** Description      Returns the current play status.
**                      This method is called in response to
**                      GetPlayStatus request.
**
** Returns          bt_status_t
**
***************************************************************************/
static bt_status_t get_play_status_rsp(btrc_play_status_t play_status, uint32_t song_len,
    uint32_t song_pos)
{
    tAVRC_RESPONSE avrc_rsp;
    CHECK_RC_CONNECTED
    memset(&(avrc_rsp.get_play_status), 0, sizeof(tAVRC_GET_PLAY_STATUS_RSP));
    avrc_rsp.get_play_status.song_len = song_len;
    avrc_rsp.get_play_status.song_pos = song_pos;
    avrc_rsp.get_play_status.play_status = play_status;

    avrc_rsp.get_play_status.pdu = AVRC_PDU_GET_PLAY_STATUS;
    avrc_rsp.get_play_status.opcode = opcode_from_pdu(AVRC_PDU_GET_PLAY_STATUS);
    avrc_rsp.get_play_status.status = AVRC_STS_NO_ERROR;
    /* Send the response */
    SEND_METAMSG_RSP(IDX_GET_PLAY_STATUS_RSP, &avrc_rsp);
    return BT_STATUS_SUCCESS;
}

/***************************************************************************
**
** Function         get_element_attr_rsp
**
** Description      Returns the current songs' element attributes
**                      in text.
**
** Returns          bt_status_t
**
***************************************************************************/
static bt_status_t get_element_attr_rsp(uint8_t num_attr, btrc_element_attr_val_t *p_attrs)
{
    tAVRC_RESPONSE avrc_rsp;
    UINT32 i;
    tAVRC_ATTR_ENTRY element_attrs[BTRC_MAX_ELEM_ATTR_SIZE];
    CHECK_RC_CONNECTED
    memset(element_attrs, 0, sizeof(tAVRC_ATTR_ENTRY) * num_attr);

    if (num_attr == 0)
    {
        avrc_rsp.get_play_status.status = AVRC_STS_BAD_PARAM;
    }
    else
    {
        for (i=0; i<num_attr; i++) {
            element_attrs[i].attr_id = p_attrs[i].attr_id;
            element_attrs[i].name.charset_id = AVRC_CHARSET_ID_UTF8;
            element_attrs[i].name.str_len = (UINT16)strlen((char *)p_attrs[i].text);
            element_attrs[i].name.p_str = p_attrs[i].text;
            BTIF_TRACE_DEBUG("%s attr_id:0x%x, charset_id:0x%x, str_len:%d, str:%s",
                __FUNCTION__, (unsigned int)element_attrs[i].attr_id,
                element_attrs[i].name.charset_id, element_attrs[i].name.str_len,
                element_attrs[i].name.p_str);
        }
        avrc_rsp.get_play_status.status = AVRC_STS_NO_ERROR;
    }
    avrc_rsp.get_elem_attrs.num_attr = num_attr;
    avrc_rsp.get_elem_attrs.p_attrs = element_attrs;
    avrc_rsp.get_elem_attrs.pdu = AVRC_PDU_GET_ELEMENT_ATTR;
    avrc_rsp.get_elem_attrs.opcode = opcode_from_pdu(AVRC_PDU_GET_ELEMENT_ATTR);
    /* Send the response */
    SEND_METAMSG_RSP(IDX_GET_ELEMENT_ATTR_RSP, &avrc_rsp);
    return BT_STATUS_SUCCESS;
}

/***************************************************************************
**
** Function         register_notification_rsp
**
** Description      Response to the register notification request.
**                      in text.
**
** Returns          bt_status_t
**
***************************************************************************/
static bt_status_t register_notification_rsp(btrc_event_id_t event_id,
    btrc_notification_type_t type, btrc_register_notification_t *p_param)
{
    tAVRC_RESPONSE avrc_rsp;
    CHECK_RC_CONNECTED
    BTIF_TRACE_EVENT("## %s ## event_id:%s", __FUNCTION__, dump_rc_notification_event_id(event_id));
    if (btif_rc_cb.rc_notif[event_id-1].bNotify == FALSE)
    {
        BTIF_TRACE_ERROR("Avrcp Event id not registered: event_id = %x", event_id);
        return BT_STATUS_NOT_READY;
    }
    memset(&(avrc_rsp.reg_notif), 0, sizeof(tAVRC_REG_NOTIF_RSP));
    avrc_rsp.reg_notif.event_id = event_id;

    switch(event_id)
    {
        case BTRC_EVT_PLAY_STATUS_CHANGED:
            avrc_rsp.reg_notif.param.play_status = p_param->play_status;
            if (avrc_rsp.reg_notif.param.play_status == PLAY_STATUS_PLAYING)
                btif_av_clear_remote_suspend_flag();
            break;
        case BTRC_EVT_TRACK_CHANGE:
            memcpy(&(avrc_rsp.reg_notif.param.track), &(p_param->track), sizeof(btrc_uid_t));
            break;
        case BTRC_EVT_PLAY_POS_CHANGED:
            avrc_rsp.reg_notif.param.play_pos = p_param->song_pos;
            break;
        default:
            BTIF_TRACE_WARNING("%s : Unhandled event ID : 0x%x", __FUNCTION__, event_id);
            return BT_STATUS_UNHANDLED;
    }

    avrc_rsp.reg_notif.pdu = AVRC_PDU_REGISTER_NOTIFICATION;
    avrc_rsp.reg_notif.opcode = opcode_from_pdu(AVRC_PDU_REGISTER_NOTIFICATION);
    avrc_rsp.get_play_status.status = AVRC_STS_NO_ERROR;

    /* Send the response. */
    send_metamsg_rsp(btif_rc_cb.rc_handle, btif_rc_cb.rc_notif[event_id-1].label,
        ((type == BTRC_NOTIFICATION_TYPE_INTERIM)?AVRC_CMD_NOTIF:AVRC_RSP_CHANGED), &avrc_rsp);
    return BT_STATUS_SUCCESS;
}

/***************************************************************************
**
** Function         set_volume
**
** Description      Send current volume setting to remote side.
**                  Support limited to SetAbsoluteVolume
**                  This can be enhanced to support Relative Volume (AVRCP 1.0).
**                  With RelateVolume, we will send VOLUME_UP/VOLUME_DOWN
**                  as opposed to absolute volume level
** volume: Should be in the range 0-127. bit7 is reseved and cannot be set
**
** Returns          bt_status_t
**
***************************************************************************/
static bt_status_t set_volume(uint8_t volume)
{
    BTIF_TRACE_DEBUG("%s", __FUNCTION__);
    CHECK_RC_CONNECTED
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;
    rc_transaction_t *p_transaction=NULL;

    if(btif_rc_cb.rc_volume==volume)
    {
        status=BT_STATUS_DONE;
        BTIF_TRACE_ERROR("%s: volume value already set earlier: 0x%02x",__FUNCTION__, volume);
        return status;
    }

    if ((btif_rc_cb.rc_features & BTA_AV_FEAT_RCTG) &&
        (btif_rc_cb.rc_features & BTA_AV_FEAT_ADV_CTRL))
    {
        tAVRC_COMMAND avrc_cmd = {0};
        BT_HDR *p_msg = NULL;

        BTIF_TRACE_DEBUG("%s: Peer supports absolute volume. newVolume=%d", __FUNCTION__, volume);
        avrc_cmd.volume.opcode = AVRC_OP_VENDOR;
        avrc_cmd.volume.pdu = AVRC_PDU_SET_ABSOLUTE_VOLUME;
        avrc_cmd.volume.status = AVRC_STS_NO_ERROR;
        avrc_cmd.volume.volume = volume;

        if (AVRC_BldCommand(&avrc_cmd, &p_msg) == AVRC_STS_NO_ERROR)
        {
            bt_status_t tran_status=get_transaction(&p_transaction);
            if(BT_STATUS_SUCCESS == tran_status && NULL!=p_transaction)
            {
                BTIF_TRACE_DEBUG("%s msgreq being sent out with label %d",
                                   __FUNCTION__,p_transaction->lbl);
                BTA_AvMetaCmd(btif_rc_cb.rc_handle,p_transaction->lbl, AVRC_CMD_CTRL, p_msg);
                status =  BT_STATUS_SUCCESS;
            }
            else
            {
                if(NULL!=p_msg)
                   GKI_freebuf(p_msg);
                BTIF_TRACE_ERROR("%s: failed to obtain transaction details. status: 0x%02x",
                                    __FUNCTION__, tran_status);
                status = BT_STATUS_FAIL;
            }
        }
        else
        {
            BTIF_TRACE_ERROR("%s: failed to build absolute volume command. status: 0x%02x",
                                __FUNCTION__, status);
            status = BT_STATUS_FAIL;
        }
    }
    else
        status=BT_STATUS_NOT_READY;
    return status;
}


/***************************************************************************
**
** Function         register_volumechange
**
** Description     Register for volume change notification from remote side.
**
** Returns          void
**
***************************************************************************/

static void register_volumechange (UINT8 lbl)
{
    tAVRC_COMMAND avrc_cmd = {0};
    BT_HDR *p_msg = NULL;
    tAVRC_STS BldResp=AVRC_STS_BAD_CMD;
    rc_transaction_t *p_transaction=NULL;

    BTIF_TRACE_DEBUG("%s called with label:%d",__FUNCTION__,lbl);

    avrc_cmd.cmd.opcode=0x00;
    avrc_cmd.pdu = AVRC_PDU_REGISTER_NOTIFICATION;
    avrc_cmd.reg_notif.event_id = AVRC_EVT_VOLUME_CHANGE;
    avrc_cmd.reg_notif.status = AVRC_STS_NO_ERROR;

    BldResp=AVRC_BldCommand(&avrc_cmd, &p_msg);
    if(AVRC_STS_NO_ERROR==BldResp && p_msg)
    {
         p_transaction=get_transaction_by_lbl(lbl);
         if(NULL!=p_transaction)
         {
             BTA_AvMetaCmd(btif_rc_cb.rc_handle,p_transaction->lbl, AVRC_CMD_NOTIF, p_msg);
             BTIF_TRACE_DEBUG("%s:BTA_AvMetaCmd called",__FUNCTION__);
         }
         else
         {
            if(NULL!=p_msg)
               GKI_freebuf(p_msg);
            BTIF_TRACE_ERROR("%s transaction not obtained with label: %d",__FUNCTION__,lbl);
         }
    }
    else
        BTIF_TRACE_ERROR("%s failed to build command:%d",__FUNCTION__,BldResp);
}


/***************************************************************************
**
** Function         handle_rc_metamsg_rsp
**
** Description      Handle RC metamessage response
**
** Returns          void
**
***************************************************************************/
static void handle_rc_metamsg_rsp(tBTA_AV_META_MSG *pmeta_msg)
{
    tAVRC_RESPONSE    avrc_response = {0};
    UINT8             scratch_buf[512] = {0};
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;

    if(AVRC_OP_VENDOR==pmeta_msg->p_msg->hdr.opcode &&(AVRC_RSP_CHANGED==pmeta_msg->code
      || AVRC_RSP_INTERIM==pmeta_msg->code || AVRC_RSP_ACCEPT==pmeta_msg->code
      || AVRC_RSP_REJ==pmeta_msg->code || AVRC_RSP_NOT_IMPL==pmeta_msg->code))
    {
        status=AVRC_ParsResponse(pmeta_msg->p_msg, &avrc_response, scratch_buf, sizeof(scratch_buf));
        BTIF_TRACE_DEBUG("%s: code %d,event ID %d,PDU %x,parsing status %d, label:%d",
          __FUNCTION__,pmeta_msg->code,avrc_response.reg_notif.event_id,avrc_response.reg_notif.pdu,
          status, pmeta_msg->label);

        if (status != AVRC_STS_NO_ERROR)
        {
            if(AVRC_PDU_REGISTER_NOTIFICATION==avrc_response.rsp.pdu
                && AVRC_EVT_VOLUME_CHANGE==avrc_response.reg_notif.event_id
                && btif_rc_cb.rc_vol_label==pmeta_msg->label)
            {
                btif_rc_cb.rc_vol_label=MAX_LABEL;
                release_transaction(btif_rc_cb.rc_vol_label);
            }
            else if(AVRC_PDU_SET_ABSOLUTE_VOLUME==avrc_response.rsp.pdu)
            {
                release_transaction(pmeta_msg->label);
            }
            return;
        }
        else if(AVRC_PDU_REGISTER_NOTIFICATION==avrc_response.rsp.pdu
            && AVRC_EVT_VOLUME_CHANGE==avrc_response.reg_notif.event_id
            && btif_rc_cb.rc_vol_label!=pmeta_msg->label)
            {
                // Just discard the message, if the device sends back with an incorrect label
                BTIF_TRACE_DEBUG("%s:Discarding register notfn in rsp.code: %d and label %d",
                __FUNCTION__, pmeta_msg->code, pmeta_msg->label);
                return;
            }
    }
    else
    {
        BTIF_TRACE_DEBUG("%s:Received vendor dependent in adv ctrl rsp. code: %d len: %d. Not processing it.",
        __FUNCTION__, pmeta_msg->code, pmeta_msg->len);
        return;
    }

    if(AVRC_PDU_REGISTER_NOTIFICATION==avrc_response.rsp.pdu
        && AVRC_EVT_VOLUME_CHANGE==avrc_response.reg_notif.event_id
        && AVRC_RSP_CHANGED==pmeta_msg->code)
     {
         /* re-register for volume change notification */
         // Do not re-register for rejected case, as it might get into endless loop
         register_volumechange(btif_rc_cb.rc_vol_label);
     }
     else if(AVRC_PDU_SET_ABSOLUTE_VOLUME==avrc_response.rsp.pdu)
     {
          /* free up the label here */
          release_transaction(pmeta_msg->label);
     }

     BTIF_TRACE_EVENT("%s: Passing received metamsg response to app. pdu: %s",
             __FUNCTION__, dump_rc_pdu(avrc_response.pdu));
     btif_rc_upstreams_rsp_evt((uint16_t)avrc_response.rsp.pdu, &avrc_response, pmeta_msg->code,
                                pmeta_msg->label);
}


/***************************************************************************
**
** Function         cleanup
**
** Description      Closes the AVRC interface
**
** Returns          void
**
***************************************************************************/
static void cleanup()
{
    BTIF_TRACE_EVENT("## %s ##", __FUNCTION__);
#if UINPUT_INCLUDED
    close_uinput();
#endif /* UINPUT_INCLUDED */
    if (bt_rc_callbacks)
    {
        bt_rc_callbacks = NULL;
    }
    memset(&btif_rc_cb, 0, sizeof(btif_rc_cb_t));
    lbl_destroy();
    BTIF_TRACE_EVENT("## %s ## completed", __FUNCTION__);
}

/***************************************************************************
**
** Function         cleanup_ctrl
**
** Description      Closes the AVRC Controller interface
**
** Returns          void
**
***************************************************************************/
void esp_avrc_ct_deinit(void)
{
    BTIF_TRACE_EVENT("## %s ##", __FUNCTION__);

    if (bt_rc_ctrl_callback)
    {
        bt_rc_ctrl_callback = NULL;
    }
    memset(&btif_rc_cb, 0, sizeof(btif_rc_cb_t));
    lbl_destroy();
    BTIF_TRACE_EVENT("## %s ## completed", __FUNCTION__);
}

esp_err_t esp_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state)
{
    tAVRC_STS status = BT_STATUS_UNSUPPORTED;
    if (tl >= 16 ||
        key_state > ESP_AVRC_PT_CMD_STATE_RELEASED) {
        return ESP_ERR_INVALID_ARG;
    }
#if (AVRC_CTLR_INCLUDED == TRUE)
    CHECK_ESP_RC_CONNECTED;
    BTIF_TRACE_DEBUG("%s: key-code: %d, key-state: %d", __FUNCTION__,
                                                    key_code, key_state);
    if (btif_rc_cb.rc_features & BTA_AV_FEAT_RCTG)
    {
        BTA_AvRemoteCmd(btif_rc_cb.rc_handle, tl,
                        (tBTA_AV_RC)key_code, (tBTA_AV_STATE)key_state);
        status =  BT_STATUS_SUCCESS;
        BTIF_TRACE_EVENT("%s: succesfully sent passthrough command to BTA", __FUNCTION__);
    }
    else
    {
        status = BT_STATUS_FAIL;
        BTIF_TRACE_DEBUG("%s: feature not supported", __FUNCTION__);
    }
#else
    BTIF_TRACE_DEBUG("%s: feature not enabled", __FUNCTION__);
#endif

    switch (status) {
    case BT_STATUS_SUCCESS: return ESP_OK;
    case BT_STATUS_UNSUPPORTED: return ESP_ERR_NOT_SUPPORTED;
    default: return ESP_FAIL;
    }
}

#if 0
static const btrc_interface_t bt_rc_interface = {
    sizeof(bt_rc_interface),
    init,
    get_play_status_rsp,
    NULL, /* list_player_app_attr_rsp */
    NULL, /* list_player_app_value_rsp */
    NULL, /* get_player_app_value_rsp */
    NULL, /* get_player_app_attr_text_rsp */
    NULL, /* get_player_app_value_text_rsp */
    get_element_attr_rsp,
    NULL, /* set_player_app_value_rsp */
    register_notification_rsp,
    set_volume,
    cleanup,
};

/*******************************************************************************
**
** Function         btif_rc_get_interface
**
** Description      Get the AVRCP Target callback interface
**
** Returns          btav_interface_t
**
*******************************************************************************/
const btrc_interface_t *btif_rc_get_interface(void)
{
    BTIF_TRACE_EVENT("%s", __FUNCTION__);
    return &bt_rc_interface;
}

#endif /* #if 0*/

/*******************************************************************************
**      Function         initialize_transaction
**
**      Description    Initializes fields of the transaction structure
**
**      Returns          void
*******************************************************************************/
static void initialize_transaction(int lbl)
{
    pthread_mutex_lock(&device.lbllock);
    if(lbl < MAX_TRANSACTIONS_PER_SESSION)
    {
       device.transaction[lbl].lbl = lbl;
       device.transaction[lbl].in_use=FALSE;
       device.transaction[lbl].handle=0;
    }
    pthread_mutex_unlock(&device.lbllock);
}

/*******************************************************************************
**      Function         lbl_init
**
**      Description    Initializes label structures and mutexes.
**
**      Returns         void
*******************************************************************************/
void lbl_init()
{
    memset(&device,0,sizeof(rc_device_t));
    // pthread_mutexattr_t attr;
    // pthread_mutexattr_init(&attr);
    // pthread_mutex_init(&(device.lbllock), &attr);
    // pthread_mutexattr_destroy(&attr);
    pthread_mutex_init(&(device.lbllock), NULL);
    
    init_all_transactions();
}

/*******************************************************************************
**
** Function         init_all_transactions
**
** Description    Initializes all transactions
**
** Returns          void
*******************************************************************************/
void init_all_transactions()
{
    UINT8 txn_indx=0;
    for(txn_indx=0; txn_indx < MAX_TRANSACTIONS_PER_SESSION; txn_indx++)
    {
        initialize_transaction(txn_indx);
    }
}

/*******************************************************************************
**
** Function         get_transaction_by_lbl
**
** Description    Will return a transaction based on the label. If not inuse
**                     will return an error.
**
** Returns          bt_status_t
*******************************************************************************/
rc_transaction_t *get_transaction_by_lbl(UINT8 lbl)
{
    rc_transaction_t *transaction = NULL;
    pthread_mutex_lock(&device.lbllock);

    /* Determine if this is a valid label */
    if (lbl < MAX_TRANSACTIONS_PER_SESSION)
    {
        if (FALSE==device.transaction[lbl].in_use)
        {
            transaction = NULL;
        }
        else
        {
            transaction = &(device.transaction[lbl]);
            BTIF_TRACE_DEBUG("%s: Got transaction.label: %d",__FUNCTION__,lbl);
        }
    }

    pthread_mutex_unlock(&device.lbllock);
    return transaction;
}

/*******************************************************************************
**
** Function         get_transaction
**
** Description    Obtains the transaction details.
**
** Returns          bt_status_t
*******************************************************************************/

bt_status_t  get_transaction(rc_transaction_t **ptransaction)
{
    bt_status_t result = BT_STATUS_NOMEM;
    UINT8 i=0;
    pthread_mutex_lock(&device.lbllock);

    // Check for unused transactions
    for (i=0; i<MAX_TRANSACTIONS_PER_SESSION; i++)
    {
        if (FALSE==device.transaction[i].in_use)
        {
            BTIF_TRACE_DEBUG("%s:Got transaction.label: %d",__FUNCTION__,device.transaction[i].lbl);
            device.transaction[i].in_use = TRUE;
            *ptransaction = &(device.transaction[i]);
            result = BT_STATUS_SUCCESS;
            break;
        }
    }

    pthread_mutex_unlock(&device.lbllock);
    return result;
}


/*******************************************************************************
**
** Function         release_transaction
**
** Description    Will release a transaction for reuse
**
** Returns          bt_status_t
*******************************************************************************/
void release_transaction(UINT8 lbl)
{
    rc_transaction_t *transaction = get_transaction_by_lbl(lbl);

    /* If the transaction is in use... */
    if (transaction != NULL)
    {
        BTIF_TRACE_DEBUG("%s: lbl: %d", __FUNCTION__, lbl);
        initialize_transaction(lbl);
    }
}

/*******************************************************************************
**
** Function         lbl_destroy
**
** Description    Cleanup of the mutex
**
** Returns          void
*******************************************************************************/
void lbl_destroy()
{
    pthread_mutex_destroy(&(device.lbllock));
}
