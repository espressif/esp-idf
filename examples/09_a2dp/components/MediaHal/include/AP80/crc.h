/**
 **************************************************************************************
 * @file    crc.h
 * @brief   Calculate CRC Value
 * 
 * @author  Aissen Li
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __CRC_H__
#define __CRC_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include <stdint.h>

uint16_t GetCRC16NBS(uint8_t* data, uint32_t length);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__CRC_H__
