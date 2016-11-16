#ifndef __BLUFI_INT_H__
#define __BLUFI_INT_H__

//define the blufi serivce uuid
#define SVC_BLUFI_UUID	0xFFFF
//define the blufi Char uuid
#define CHAR_BLUFI_UUID	0xFF01

#define BLUFI_HDL_NUM	4

#define BLUFI_VAL_MAX_LEN	(128)

#define BLUFI_MAX_STRING_DATA     128


typedef struct
{
    UINT8           app_id;
    UINT16          blufi_hdl;
}tBLUFI_INST;


/* service engine control block */
typedef struct
{
    BOOLEAN                 enabled;
	BOOLEAN					is_primery;
    UINT8                   inst_id;
    tGATT_IF                gatt_if;
	tBLUFI_INST				blufi_inst;
    BOOLEAN         		in_use;
	BOOLEAN					congest;
    UINT16					conn_id;
    BOOLEAN					connected;
	BD_ADDR					remote_bda;
    UINT32					trans_id;
    UINT8					cur_srvc_id;
} tBLUFI_CB_ENV;

#endif /* __BLUFI_INT_H__ */
