//  crc32.h
//  WeChat Embedded
//
//  Created by harlliu on 14-03-03.
//  Copyright 2014 Tencent. All rights reserved.
//

#ifndef __CRC32_H__
#define __CRC32_H__


#include "prf_defs.h"
#if (WX_AIRSYNC_CFG)

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
uint32_t crc32(uint32_t crc, const uint8_t *buf, int len);

#ifdef __cplusplus
}
#endif		///__cplusplus

#endif		///WX_AIRSYNC_CFG

#endif	///__CRC32_H__

