/*
*	WeChat mpbledemo2 
*
*	author by anqiren  2014/12/02  V1.0bat
*
**/

#include "prf_defs.h"

#if	(WX_AIRSYNC_CFG)

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "app_airsync_prf.h"
#include "wechat_aes.h"
#include "epb_MmBp.h"
#include "wechar_crc.h"
#include "app_wechat_util.h"
#include "app_airsync_md5.h"
#include "mpbledemo2.h"
#include "gki.h"


mpbledemo2_info m_info = {CMD_NULL, {NULL, 0}};

#if defined EAM_md5AndNoEnrypt || EAM_md5AndAesEnrypt
	uint8_t md5_type_and_id[16];
#endif

uint8_t challeange[CHALLENAGE_LENGTH] = {0x11,0x22,0x33,0x44}; //为了方便这里填了一组确定的数组，使用过程中请自行生成随机数

mpbledemo2_state mpbledemo2Sta = {false, false, false, false, false, false,0,0,0};
const uint8_t key[16] = DEVICE_KEY;
uint8_t session_key[32] = {0};
data_handler *m_mpbledemo2_handler = NULL;
uint8_t isLightOn;
/**** NOTIFICATION BITS ***********/
uint8_t FirstNotificationBit ;
uint8_t SecondNotificationBit;


//static void mpbledemo2_handleCmdFromServer(BleDemo2CmdID cmd, uint8_t *ptrData, uint32_t lengthInByte);

/**@brief   Function for the light initialization.
 *
 * @details Initializes all lights used by this application.
 */

int32_t mpbledemo2_get_md5(void)
{
	int32_t error_code = 0;
#if defined EAM_md5AndNoEnrypt || EAM_md5AndAesEnrypt
	char device_type[] = DEVICE_TYPE;
	char device_id[] = DEVICE_ID;
	char argv[sizeof(DEVICE_TYPE) + sizeof(DEVICE_ID) - 1];
	memcpy(argv,device_type,sizeof(DEVICE_TYPE));
/*when add the DEVICE_ID to DEVICE_TYPE, the offset shuld -1 to overwrite '\0'  at the end of DEVICE_TYPE */
	memcpy(argv + sizeof(DEVICE_TYPE)-1,device_id,sizeof(DEVICE_ID));

	error_code = wechat_md5(argv, md5_type_and_id);

#endif
	return error_code;
}

void mpbledemo2_reset()
{
	mpbledemo2Sta.auth_send = false;
	mpbledemo2Sta.auth_state = false;
	mpbledemo2Sta.indication_state = false;
	mpbledemo2Sta.init_send = false;
	mpbledemo2Sta.init_state = false;
	mpbledemo2Sta.send_data_seq = 0;
	mpbledemo2Sta.push_data_seq = 0;
	mpbledemo2Sta.seq = 0;
}
    
int32_t mpbledemo2_init(void)
{
	mpbledemo2_reset();
	return (mpbledemo2_get_md5());
}


void mpbledemo2_indication_state(bool isEnable)
{
    mpbledemo2Sta.indication_state = isEnable;
    if (false == isEnable)
    {
        mpbledemo2Sta.auth_send = false;
    }
}

int32_t device_auth()
{	
	if (m_mpbledemo2_handler == NULL) {
			m_mpbledemo2_handler = get_handler_by_type(PRODUCT_TYPE_MPBLEDEMO2);
		}
		uint8_t *data = NULL;
		uint32_t len = 0;
		
		ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_AUTH);	
		m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
		if(data == NULL)
		{
			return errorCodeProduce;
		}
        
		//sent data
		ble_wechat_indicate_data(data, len);
		return 0;
	}
	

int32_t	device_init()
	{
		uint8_t *data = NULL;
		uint32_t len = 0;
		ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_INIT);
		m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
		if(data == NULL)
		{
			return errorCodeProduce;
		}
        
		//sent data
		ble_wechat_indicate_data(data, len);
	
		return 0;
}

void mpbledemo2_main_process()
{
	int error_code = 0;
	if((mpbledemo2Sta.indication_state) && (!mpbledemo2Sta.auth_state) && (!mpbledemo2Sta.auth_send) )
	{
		error_code = device_auth();
        if (0 == error_code)
        {
            mpbledemo2Sta.auth_send = true;	
        }
	}
	if((mpbledemo2Sta.auth_state) && (!mpbledemo2Sta.init_state) && (!mpbledemo2Sta.init_send))
	{
		error_code = device_init();
        if (0 == error_code)
        {
            mpbledemo2Sta.init_send = true;            
        }
	}
}

void mpbledemo2_data_free_func(uint8_t *data, uint32_t len)
{
	if(data)
    {
		GKI_freebuf(data);
		data = NULL;
    }
}

void mpbledemo2_data_produce_func(void *args, uint8_t **r_data, uint32_t *r_len)
{	
		static uint16_t bleDemoHeadLen = sizeof(BlueDemoHead);
		mpbledemo2_info *info = (mpbledemo2_info *)args;
		BaseRequest basReq = {NULL};
		static uint8_t fix_head_len = sizeof(BpFixHead);
		BpFixHead fix_head = {0xFE, 1, 0, htons(ECI_req_auth), 0};
		mpbledemo2Sta.seq++;
		switch (info->cmd)
		{
		case CMD_AUTH:
			{
			#if defined EAM_md5AndAesEnrypt
				uint8_t deviceid[] = DEVICE_ID;
				static uint32_t seq = 0x00000001;		//
				uint32_t ran = 0x11223344;		//为了方便起见这里放了一个固定值做为随机数，在使用时请自行生成随机数。
				ran = t_htonl(ran);
				seq = t_htonl(seq);
				uint8_t id_len = strlen(DEVICE_ID);
				uint8_t* data = GKI_getbuf(id_len+8);
				if(!data){return;}
				memcpy(data,deviceid,id_len);
				memcpy(data+id_len,(uint8_t*)&ran,4);
				memcpy(data+id_len+4,(uint8_t*)&seq,4);
				uint32_t crc = crc32(0, data, id_len+8);
				crc = t_htonl(crc);
				memset(data,0x00,id_len+8);
				memcpy(data,(uint8_t*)&ran,4);
				memcpy(data+4,(uint8_t*)&seq,4);
				memcpy(data+8,(uint8_t*)&crc,4);	
				uint8_t CipherText[16];
				AES_Init(key);
				AES_Encrypt_PKCS7 (data, CipherText, 12, key);
				if(data){GKI_freebuf(data);data = NULL;}
				AuthRequest authReq = {&basReq, true,{md5_type_and_id, MD5_TYPE_AND_ID_LENGTH}, 
                PROTO_VERSION, AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, true ,
                {CipherText, CIPHER_TEXT_LENGTH}, false, {NULL, 0}, false, {NULL, 0}, false, 
                {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
				seq++;
			#endif
				
			#if defined EAM_macNoEncrypt
				static uint8_t mac_address[MAC_ADDRESS_LENGTH];
				//get_mac_addr(mac_address);
				AuthRequest authReq = {&basReq, false,{NULL, 0}, PROTO_VERSION, AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, false,{NULL, 0}, true, {mac_address, MAC_ADDRESS_LENGTH}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
			#endif
				
			#if defined EAM_md5AndNoEnrypt
				AuthRequest authReq = {&basReq, true,{md5_type_and_id, MD5_TYPE_AND_ID_LENGTH}, PROTO_VERSION, (EmAuthMethod)AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, false ,{NULL, 0}, false, {NULL, 0}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
			#endif
				*r_len = epb_auth_request_pack_size(&authReq) + fix_head_len;
				*r_data = (uint8_t *)GKI_getbuf(*r_len);
				if(!(*r_data)){return;}
				if(epb_pack_auth_request(&authReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{
					*r_data = NULL;
					return;
				}
				fix_head.nCmdId = htons(ECI_req_auth);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(mpbledemo2Sta.seq);
				memcpy(*r_data, &fix_head, fix_head_len);                
				return ;
		}
		case CMD_INIT:
			{
				//has challeange
				InitRequest initReq = {&basReq,false, {NULL, 0},true, {challeange, CHALLENAGE_LENGTH}};
				*r_len = epb_init_request_pack_size(&initReq) + fix_head_len;
			#if defined EAM_md5AndAesEnrypt
				uint8_t length = *r_len;				
				uint8_t *p = GKI_getbuf(AES_get_length( *r_len-fix_head_len));
				if(!p){return;}
				*r_len = AES_get_length( *r_len-fix_head_len)+fix_head_len;
			#endif
			//pack data
				*r_data = (uint8_t *)GKI_getbuf(*r_len);
				if(!(*r_data)){return;}
				if(epb_pack_init_request(&initReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{*r_data = NULL;return;}
				//encrypt body
			#if defined EAM_md5AndAesEnrypt
				AES_Init(session_key);
				AES_Encrypt_PKCS7(*r_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度
				memcpy(*r_data + fix_head_len, p, *r_len-fix_head_len);
				if(p)GKI_freebuf(p);
			#endif
				fix_head.nCmdId = htons(ECI_req_init);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(mpbledemo2Sta.seq);
				memcpy(*r_data, &fix_head, fix_head_len);
				return  ;
		}
		case CMD_SENDDAT:
			{
			
				BlueDemoHead  *bleDemoHead = (BlueDemoHead*)GKI_getbuf(bleDemoHeadLen+info->send_msg.len);
				if (!bleDemoHead)
                {
                    return;
                }
                
                // header of sent data
				bleDemoHead->m_magicCode[0] = MPBLEDEMO2_MAGICCODE_H;
				bleDemoHead->m_magicCode[1] = MPBLEDEMO2_MAGICCODE_L;
				bleDemoHead->m_version      = htons(MPBLEDEMO2_VERSION);
				bleDemoHead->m_totalLength  = htons(bleDemoHeadLen + info->send_msg.len);
				bleDemoHead->m_cmdid        = htons(sendTextReq);
				bleDemoHead->m_seq          = htons(mpbledemo2Sta.seq);
				bleDemoHead->m_errorCode    = 0;	
                
				/*connect body and head.*/
				/*turn to uint8_t* befort offset.*/
				memcpy((uint8_t*)bleDemoHead + bleDemoHeadLen, info->send_msg.str, info->send_msg.len);		
                
				SendDataRequest sendDatReq = 
                    {
                        &basReq, 
                        { (uint8_t*) bleDemoHead, (bleDemoHeadLen + info->send_msg.len)},  // define the data content wrapped in epb
                        false,      // no type, the data is directly sent to vendor server
                        (EmDeviceDataType)NULL
                    };
				*r_len = epb_send_data_request_pack_size(&sendDatReq) + fix_head_len;
                    
			#if defined EAM_md5AndAesEnrypt
				uint16_t length = *r_len;
				uint8_t *p = GKI_getbuf(AES_get_length( *r_len-fix_head_len));
				if(!p){return;}
				*r_len = AES_get_length( *r_len-fix_head_len)+fix_head_len;
			#endif
                
				*r_data = (uint8_t *)GKI_getbuf(*r_len);
				if(!(*r_data)){return;}
				if(epb_pack_send_data_request(&sendDatReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{
					*r_data = NULL;
				#if defined EAM_md5AndAesEnrypt
					if(p){GKI_freebuf(p);
					p = NULL;}
				#endif
					return;
				}
                
			#if defined EAM_md5AndAesEnrypt
				//encrypt body
				AES_Init(session_key);
				AES_Encrypt_PKCS7(*r_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度
				memcpy(*r_data + fix_head_len, p, *r_len-fix_head_len);
				if(p){GKI_freebuf(p); p = NULL;}
			#endif
                
                // header of epb
				fix_head.nCmdId = htons(ECI_req_sendData);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(mpbledemo2Sta.seq);
                
				memcpy(*r_data, &fix_head, fix_head_len);
				if(bleDemoHead){GKI_freebuf(bleDemoHead);bleDemoHead = NULL;}
                
                // increase sequence by 1
                mpbledemo2Sta.send_data_seq++;				
				return ;
		}
	}	
}

int mpbledemo2_data_consume_func(uint8_t *data, uint32_t len)
{
		BpFixHead *fix_head = (BpFixHead *)data;
		uint8_t fix_head_len = sizeof(BpFixHead);
		switch(ntohs(fix_head->nCmdId))
		{
			case ECI_none:
			{


			}
				break;
			case ECI_resp_auth:
				{
					AuthResponse* authResp;
					authResp = epb_unpack_auth_response(data+fix_head_len,len-fix_head_len);
					if(authResp->base_response)
					{
						if(authResp->base_response->err_code == 0)
						{
							mpbledemo2Sta.auth_state = true;
						}
						else
						{
                            int32_t returnedErrCode = authResp->base_response->err_code;
							epb_unpack_auth_response_free(authResp);
							return returnedErrCode;
						}
					}
				#if defined EAM_md5AndAesEnrypt		// get sessionkey
					if(authResp->aes_session_key.len)
					{
						AES_Init(key);
						AES_Decrypt(session_key,authResp->aes_session_key.data,authResp->aes_session_key.len,key);
					}
				#endif
					epb_unpack_auth_response_free(authResp);
				}
				break;
			case ECI_resp_sendData:
				{
				
			#if defined EAM_md5AndAesEnrypt		
					uint32_t length = len- fix_head_len;//加密后数据长度
					uint8_t *p = GKI_getbuf (length);
					if(!p){ if(data)GKI_freebuf(data);data = NULL; return 0;}
					AES_Init(session_key);
					//解密数据
					AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
					
					uint8_t temp;
					temp = p[length - 1];//算出填充长度
					len = len - temp;//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);//把明文放回
					if(p){GKI_freebuf(p);p = NULL;}
			#endif	
				SendDataResponse *sendDataResp;
				sendDataResp = epb_unpack_send_data_response(data+fix_head_len,len-fix_head_len);
				if (!sendDataResp)
				{
					return errorCodeUnpackSendDataResp;
				}

					if(sendDataResp->base_response->err_code)
					{
						epb_unpack_send_data_response_free(sendDataResp);
						return sendDataResp->base_response->err_code;
					}
					epb_unpack_send_data_response_free(sendDataResp);
			}
				break;
			case ECI_resp_init:
				{
				FirstNotificationBit = 0;
				SecondNotificationBit = 0; 
		
			#if defined EAM_md5AndAesEnrypt		
					uint32_t length = len- fix_head_len;		//加密后数据长度
					uint8_t *p = GKI_getbuf (length);
					if(!p){if(data)GKI_freebuf(data);data = NULL; return 0;}
					AES_Init(session_key);
					//解密数据
					AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
					
					uint8_t temp;
					temp = p[length - 1];			//算出填充长度
					len = len - temp;				//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);		//把明文放回
					if(p){GKI_freebuf(p);p = NULL;}
			#endif		
				InitResponse *initResp = epb_unpack_init_response(data+fix_head_len, len-fix_head_len);
				if(!initResp)
				{
					return errorCodeUnpackInitResp;
				}
			
					if(initResp->base_response)
					{
						if(initResp->base_response->err_code == 0)
						{
							if(initResp->has_challeange_answer)
							{
								if(crc32(0,challeange,CHALLENAGE_LENGTH) == initResp->challeange_answer)
								{
									mpbledemo2Sta.init_state = true;
								}
							}
							else mpbledemo2Sta.init_state = true;
							mpbledemo2Sta.wechats_switch_state = true;
						}
						else
						{
							epb_unpack_init_response_free(initResp);
							return initResp->base_response->err_code;
						}
					}
				epb_unpack_init_response_free(initResp);
			}
				break;
			case ECI_push_recvData:
				{
				#if defined EAM_md5AndAesEnrypt
					uint32_t length = len- fix_head_len;//加密后数据长度
					uint8_t *p = GKI_getbuf (length);
					if(!p){if(data)GKI_freebuf(data); data =NULL; return 0;}
					AES_Init(session_key);
					//解密数据
					AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
					
					uint8_t temp;
					temp = p[length - 1];//算出填充长度
					len = len - temp;//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);//把明文放回
					if(p){GKI_freebuf(p);p = NULL;}
				#endif
			RecvDataPush *recvDatPush;
			recvDatPush = epb_unpack_recv_data_push(data+fix_head_len, len-fix_head_len);
		
			if(!recvDatPush)
			{
				return errorCodeUnpackRecvDataPush;
			}
		
			BlueDemoHead *bledemohead = (BlueDemoHead*)recvDatPush->data.data;
		
            
            // ble demo command handle
  
			epb_unpack_recv_data_push_free(recvDatPush);
			mpbledemo2Sta.push_data_seq++;
		}
				break;
			case ECI_push_switchView:
				{
			
					mpbledemo2Sta.wechats_switch_state = !mpbledemo2Sta.wechats_switch_state;
			
			#if defined EAM_md5AndAesEnrypt		
					uint32_t length = len- fix_head_len;//加密后数据长度
					uint8_t *p = GKI_getbuf (length);
					if(!p){if(data)GKI_freebuf(data);data = NULL; return 0;}
					AES_Init(session_key);
					//解密数据
					AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
					
					uint8_t temp;
					temp = p[length - 1];//算出填充长度
					len = len - temp;//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);//把明文放回
					if(p){GKI_freebuf(p);p = NULL;}
			#endif		
				SwitchViewPush *swichViewPush;
				swichViewPush = epb_unpack_switch_view_push(data+fix_head_len,len-fix_head_len);
				if(!swichViewPush)
				{
					return errorCodeUnpackSwitchViewPush;
				}
				epb_unpack_switch_view_push_free(swichViewPush);
			}
				break;
			case ECI_push_switchBackgroud:
				{
			
			#if defined EAM_md5AndAesEnrypt
					uint32_t length = len- fix_head_len;//加密后数据长度
					uint8_t *p = GKI_getbuf (length);
					if(!p){if(data)GKI_freebuf(data);data = NULL;  return 0;}
					AES_Init(session_key);
					//解密数据
					AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
					uint8_t temp;
					temp = p[length - 1];//算出填充长度
					len = len - temp;//取加密前数据总长度
					memcpy(data + fix_head_len, p ,length -temp);//把明文放回
					if(data){GKI_freebuf(p);p = NULL;}
			#endif
				SwitchBackgroudPush *switchBackgroundPush = epb_unpack_switch_backgroud_push(data+fix_head_len,len-fix_head_len);
				if(! switchBackgroundPush)
				{
					return errorCodeUnpackSwitchBackgroundPush;
				}	
				epb_unpack_switch_backgroud_push_free(switchBackgroundPush);
			}
				break;
			case ECI_err_decode:
				break;
			default:
				{
	
			}
				break;
		}
		return 0;
}
void mpbledemo2_data_error_func(int error_code)
{
	if(error_code)
	{
        
		//NVIC_SystemReset();
	}
}

data_handler mpbledemo2_data_handler = {
		.m_product_type                 = PRODUCT_TYPE_MPBLEDEMO2,
		.m_data_produce_func            = &mpbledemo2_data_produce_func,
		.m_data_free_func               = &mpbledemo2_data_free_func,
		.m_data_consume_func            = &mpbledemo2_data_consume_func,
		.m_data_error_func              = &mpbledemo2_data_error_func,
        .m_data_init_peripheral_func    = NULL,
		.m_data_init_func               = &mpbledemo2_init,
		.m_data_main_process_func       = &mpbledemo2_main_process,
		.m_data_button_handler_func     = NULL,
		.m_data_produce_args            = &m_info,
		.next                           = NULL
};

// send data to wechat server
int32_t mpbledemo2_sendData(uint8_t* ptrData, uint32_t lengthInByte)
{
    uint8_t *data = NULL;
	uint32_t len = 0;
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_SENDDAT);   
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, send_msg.len, lengthInByte);
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, send_msg.str, (const char *)ptrData);    
    
	m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
	if(data == NULL)
	{
		return errorCodeProduce;
	}
	ble_wechat_indicate_data(data, len);
    return 0;
}

#endif	///WX_AIRSYNC_CFG

