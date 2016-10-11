/**
 ****************************************************************************************
 *
 * @file app_airsync_prf.h
 *
 * @brief Application Security Entry Point
 *
 * Copyright (C) ESPRESSIF 2016
 * Created by Yulong at 2016/9/30
 *
 ****************************************************************************************
 */
#ifndef _APP_AIRSYNC_PRF_H__
#define _APP_AIRSYNC_PRF_H__

#include "app_wechat_util.h"
#include <stdint.h>
#include <stdbool.h>

#define MAGIC_NUMBER	0xFE

/**
 ****************************************************************************************
 * @brief Create Wechat Database
 *
 ****************************************************************************************
 */
void app_wechat_create_db(void);

void app_wechat_SetDatahandler(data_handler* pHandler);
int ble_wechat_indicate_data(uint8_t *data, int len);
int ble_wechat_indicate_data_chunk(void);
void ble_wechat_process_received_data(uint8_t* pData, uint32_t length);
data_handler* app_wechat_datahandler(void);
bool ble_wechat_is_last_data_sent_out(void);


#endif	///_APP_AIRSYNC_PRF_H__



