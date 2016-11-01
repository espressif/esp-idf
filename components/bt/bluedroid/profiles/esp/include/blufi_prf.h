#include "bt_target.h"
#include "gatt_api.h"
#include "gattdefs.h"

//define the blufi serivce uuid
#define SVC_BLUFI_UUID	0xFFFF
//define the blufi Char uuid
#define CHAR_BLUFI_UUID	0xFF01

#define BLUFI_HDL_NUM	4

#define BLUFI_VAL_MAX_LEN	(20)

#define BLUFI_MAX_STRING_DATA     33

typedef void (tBLUFI_CBACK)(UINT8 app_id, UINT8 event, UINT8 len, UINT8 *data);

typedef enum
{
	RECV_ACT_PASSWD = 0,
	RECV_ACT_SSID,
	RECV_ACT_MAX
} RECV_ACTION;

typedef struct
{
    BD_ADDR remote_bda;
    BOOLEAN need_rsp;
    UINT16  clt_cfg;
} tBLUFI_WRITE_DATA;

typedef struct
{
    BOOLEAN         in_use;
	BOOLEAN			congest;
    UINT16          conn_id;
    BOOLEAN         connected;
    BD_ADDR         remote_bda;
    UINT32          trans_id;
    UINT8           cur_srvc_id;

} tBLUFI_CLCB;


typedef struct
{
    UINT8           app_id;
    UINT16          blufi_hdl;
    tBLUFI_CBACK*	blufi_cback;

}tBLUFI_INST;


/* service engine control block */
typedef struct
{
    BOOLEAN                 enabled;
	BOOLEAN					is_primery;
    UINT8                   inst_id;
    tGATT_IF                gatt_if;
    tBLUFI_CLCB            	clcb; 			/* connection link*/
	tBLUFI_INST				blufi_inst;
} tBLUFI_CB_ENV;

void blufi_create_service(void);

tBLUFI_CLCB *blufi_env_clcb_alloc(UINT16 conn_id, BD_ADDR bda);

UINT16 blufi_env_find_conn_id_by_bd_adddr(BD_ADDR bda);

BOOLEAN blufi_env_clcb_dealloc(UINT16 conn_id);

tGATT_STATUS blufi_profile_init(tBLUFI_CBACK *call_back);

void blufi_msg_notify(UINT8 *blufi_msg, UINT8 len);

