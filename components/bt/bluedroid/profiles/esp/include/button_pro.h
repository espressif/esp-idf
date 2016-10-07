/**
 ****************************************************************************************
 *
 * @file button_pro.h
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/8/18
 *
 *
 ****************************************************************************************
 */

#include "bt_target.h"
#include "gatt_api.h"
#include "gattdefs.h"

#define KEY_SUCCESS             GATT_SUCCESS
#define KEY_ILLEGAL_PARAM       GATT_ILLEGAL_PARAMETER
#define KEY_NO_RESOURCES        GATT_NO_RESOURCES

//define the key serivce uuid
#define ATT_SVC_BUTTON	0xFFFF
//define the key Char uuid
#define ATT_CHAR_BUTTON_WIT	0xFF01
#define ATT_CHAR_BUTTON_NTF	0xFF02

#define	BUTTON_PRESS_NTF_CFG	0x01

#define BUTTON_VAL_MAX_LEN	(10)

#define BUTT_MAX_APPS                  GATT_CL_MAX_LCB

#define BUT_MAX_STRING_DATA     7

typedef void (tBU_CBACK)(UINT8 app_id, UINT8 event, UINT8 len, UINT8 *data);

#ifndef BUT_MAX_INT_NUM
#define BUT_MAX_INT_NUM     4
#endif

enum
{
	RECEIVE_NET_PASSWD_EVT,
	RECEIVE_NET_SSD_EVT,
	RECEIVE_EVT_MAX
};

/// button Service Attributes Indexes
enum
{
    KEY_IDX_SVC,
	KEY_IDX_BUTTON_WIT_CHAR,
    KEY_IDX_BUTTON_WIT_VAL,
    KEY_IDX_BUTTON_NTF_CHAR,
    KEY_IDX_BUTTON_NTF_VAL,
    KEY_IDX_BUTTON_NTF_CFG,

    KEY_IDX_NB,
};

typedef struct
{
    BD_ADDR remote_bda;
    BOOLEAN need_rsp;
    UINT16  clt_cfg;
}tBUT_WRITE_DATA;

typedef struct
{
    BOOLEAN         in_use;
	BOOLEAN			congest;
    UINT16          conn_id;
    BOOLEAN         connected;
    BD_ADDR         remote_bda;
    UINT32          trans_id;
    UINT8           cur_srvc_id;

}tBUT_CLCB;


typedef struct
{
    UINT8           app_id;
    UINT16          but_wirt_hdl;
    UINT16          but_ntf_hdl;
    UINT16          but_cfg_hdl;
 
    tBU_CBACK       *p_cback;

}tBUT_INST;


/* service engine control block */
typedef struct
{
    tBUT_CLCB             	clcb; 			/* connection link*/
    tGATT_IF                gatt_if;
    BOOLEAN                 enabled;
	BOOLEAN					is_primery;
	tBUT_INST               button_inst;
    UINT8                   inst_id;
}tBUTTON_CB_ENV;

void Button_CreateService(void);

tBUT_CLCB *button_env_clcb_alloc(UINT16 conn_id, BD_ADDR bda);

UINT16 button_env_find_conn_id_by_bd_adddr(BD_ADDR bda);

BOOLEAN button_env_clcb_dealloc(UINT16 conn_id);

tGATT_STATUS button_init(tBU_CBACK *call_back);

void button_msg_notify(UINT8 len, UINT8 *button_msg);

extern tBUTTON_CB_ENV button_cb_env;

