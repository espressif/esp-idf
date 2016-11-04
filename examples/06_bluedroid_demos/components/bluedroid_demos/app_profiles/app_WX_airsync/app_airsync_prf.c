// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "prf_defs.h"
#if (WX_AIRSYNC_CFG)

#include "app_airsync_prf.h"
#include "wx_airsync_prf.h"
#include "app_wechat_util.h"
#include "gki.h"
#include "mpbledemo2.h"

/*****************************************************************************
* data handle
*****************************************************************************/
data_info g_send_data;

static data_info g_rcv_data;
data_handler *pDataHandler;


static void airsync_process_msgreceive_cb(UINT8 app_id,
											UINT8 conn_id,uint8_t *pData, uint16_t length);

///function for register all products
static void register_all_products(void)
{
	REGISTER(mpbledemo2);
	// could register more products if defined
}

void app_wechat_init(void)
{
	register_all_products();

	data_handler_init(&m_mpbledemo2_handler,PRODUCT_TYPE_MPBLEDEMO2);
	m_mpbledemo2_handler->m_data_init_func();
	m_mpbledemo2_handler->m_data_init_peripheral_func();

	app_wechat_SetDatahandler(m_mpbledemo2_handler);
}

int ble_wechat_indicate_data_chunk(void)
{
	uint16_t chunk_len = 0;
	chunk_len = g_send_data.len - g_send_data.offset;
	chunk_len = chunk_len > BLE_WECHAT_MAX_DATA_LEN?BLE_WECHAT_MAX_DATA_LEN:chunk_len;

	if(chunk_len == 0)
	{
		app_wechat_datahandler()->m_data_free_func(g_send_data.data,g_send_data.len);
		g_send_data.data = NULL;
		g_send_data.len = 0;
		g_send_data.offset = 0;
		return 0;
	}

	g_send_data.offset += chunk_len;
	return 1;
}


 /*******************************************************************************
**
** Function         airsync_msgreceive_cb
**
** Description      the callback function after the airsync profile have been 
**					receive the data from the peer device
** parms			appid:the appid have been register to the gatt database
**					conn_id:the current connection index
**					msg_val:the airsync value receive from peer device
** Returns          NULL 
**
*******************************************************************************/
static void airsync_process_msgreceive_cb(UINT8 app_id,
											UINT8 conn_id,uint8_t *pData, uint16_t length)
{
	int error_code;
	int chunk_size = 0;
	if(length <= BLE_WECHAT_MAX_DATA_LEN)
	{
		if(g_rcv_data.len == 0)
		{
			BpFixHead *fix_head = (BpFixHead *)pData;
			g_rcv_data.len = ntohs(fix_head->nLength);
			g_rcv_data.offset = 0;
			g_rcv_data.data = (uint8_t *)GKI_getbuf(g_rcv_data.len);
		}

		chunk_size = g_rcv_data.len - g_rcv_data.offset;
		chunk_size = chunk_size < length ? chunk_size : length;
		memcpy(g_rcv_data.data + g_rcv_data.offset, pData, chunk_size);
		g_rcv_data.offset += chunk_size;

		if (g_rcv_data.len <= g_rcv_data.offset) 
		{
			error_code = app_wechat_datahandler()->m_data_consume_func(g_rcv_data.data, g_rcv_data.len);
			app_wechat_datahandler()->m_data_free_func(g_rcv_data.data,g_rcv_data.len);
			wechat_error_chack(app_wechat_datahandler(), error_code);
			g_rcv_data.len = 0;
			g_rcv_data.offset = 0;	

            app_wechat_datahandler()->m_data_main_process_func();
		}	

		
	}
	
	
}

 void app_wechat_SetDatahandler(data_handler* pHandler)
{
    pDataHandler = pHandler;
}

bool ble_wechat_is_last_data_sent_out(void)
{
	return (g_send_data.len == 0 && g_send_data.offset == 0 ) || \
		(g_send_data.len != 0 && g_send_data.len == g_send_data.offset);
}

//device sent data on the indicate characteristic
int ble_wechat_indicate_data(uint8_t *data, int len)
{
	if(data == NULL || len == 0)
	{
		return 0;
	}

	if(!ble_wechat_is_last_data_sent_out())
	{
		return 0;
	}

	g_send_data.data = data;
	g_send_data.len = len;
	g_send_data.offset = 0;

	return (ble_wechat_indicate_data_chunk());
}

data_handler *app_wechat_datahandler(void)
{
	return pDataHandler;
}


#endif	///WX_AIRSYNC_CFG

