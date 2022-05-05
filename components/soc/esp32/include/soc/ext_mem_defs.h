/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _CACHE_MEMORY_H_
#define _CACHE_MEMORY_H_

#include "esp_bit_defs.h"


#ifdef __cplusplus
extern "C" {
#endif


#define IRAM0_CACHE_ADDRESS_LOW     0x400D0000
#define IRAM0_CACHE_ADDRESS_HIGH    0x40400000

#define IRAM1_CACHE_ADDRESS_LOW     0x40400000
#define IRAM1_CACHE_ADDRESS_HIGH    0x40800000

#define IROM0_CACHE_ADDRESS_LOW     0x40800000
#define IROM0_CACHE_ADDRESS_HIGH    0x40C00000

#define DRAM1_CACHE_ADDRESS_LOW     0x3F800000
#define DRAM1_CACHE_ADDRESS_HIGH    0x3FC00000

#define DROM0_CACHE_ADDRESS_LOW     0x3F400000
#define DROM0_CACHE_ADDRESS_HIGH    0x3F800000


#define ADDRESS_IN_BUS(bus_name, vaddr)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)
#define ADDRESS_IN_IRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM0_CACHE, vaddr)
#define ADDRESS_IN_IRAM1_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM1_CACHE, vaddr)
#define ADDRESS_IN_IROM0_CACHE(vaddr)      ADDRESS_IN_BUS(IROM0_CACHE, vaddr)
#define ADDRESS_IN_DRAM1_CACHE(vaddr)      ADDRESS_IN_BUS(DRAM1_CACHE, vaddr)
#define ADDRESS_IN_DROM0_CACHE(vaddr)      ADDRESS_IN_BUS(DROM0_CACHE, vaddr)


#ifdef __cplusplus
}
#endif

#endif /*_CACHE_MEMORY_H_ */
