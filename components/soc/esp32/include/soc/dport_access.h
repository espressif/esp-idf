// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _DPORT_ACCESS_H_
#define _DPORT_ACCESS_H_

#include <stdint.h>
#include "esp_attr.h"
#include "esp_dport_access.h"

#ifdef __cplusplus
extern "C" {
#endif

//Registers Operation {{

// The _DPORT_xxx register read macros access DPORT memory directly (as opposed to
// DPORT_REG_READ which applies SMP-safe protections).
//
// Use DPORT_REG_READ versions to be SMP-safe in IDF apps. If you want to
// make a sequence of DPORT reads, use DPORT_STALL_OTHER_CPU_START() macro
// explicitly and then use _DPORT_REG_READ macro while other CPU is stalled.
//
// _DPORT_REG_WRITE & DPORT_REG_WRITE are equivalent.
#define _DPORT_REG_READ(_r)        (*(volatile uint32_t *)(_r))
#define _DPORT_REG_WRITE(_r, _v)   (*(volatile uint32_t *)(_r)) = (_v)

// Write value to DPORT register (does not require protecting)
#define DPORT_REG_WRITE(_r, _v)   _DPORT_REG_WRITE((_r), (_v))

// Read value from register, SMP-safe version.
static inline uint32_t IRAM_ATTR DPORT_REG_READ(uint32_t reg)
{
    uint32_t val;

    DPORT_STALL_OTHER_CPU_START();
    val = _DPORT_REG_READ(reg);
    DPORT_STALL_OTHER_CPU_END();

    return val;
}

//get bit or get bits from register
#define DPORT_REG_GET_BIT(_r, _b)  (DPORT_REG_READ(_r) & (_b))

//set bit or set bits to register
#define DPORT_REG_SET_BIT(_r, _b)  DPORT_REG_WRITE((_r), (DPORT_REG_READ(_r)|(_b)))

//clear bit or clear bits of register
#define DPORT_REG_CLR_BIT(_r, _b)  DPORT_REG_WRITE((_r), (DPORT_REG_READ(_r) & (~(_b))))

//set bits of register controlled by mask
#define DPORT_REG_SET_BITS(_r, _b, _m) DPORT_REG_WRITE((_r), ((DPORT_REG_READ(_r) & (~(_m))) | ((_b) & (_m))))

//get field from register, uses field _S & _V to determine mask
#define DPORT_REG_GET_FIELD(_r, _f) ((DPORT_REG_READ(_r) >> (_f##_S)) & (_f##_V))

//set field to register, used when _f is not left shifted by _f##_S
#define DPORT_REG_SET_FIELD(_r, _f, _v) DPORT_REG_WRITE((_r), ((DPORT_REG_READ(_r) & (~((_f##_V) << (_f##_S))))|(((_v) & (_f##_V))<<(_f##_S))))

//get field value from a variable, used when _f is not left shifted by _f##_S
#define DPORT_VALUE_GET_FIELD(_r, _f) (((_r) >> (_f##_S)) & (_f))

//get field value from a variable, used when _f is left shifted by _f##_S
#define DPORT_VALUE_GET_FIELD2(_r, _f) (((_r) & (_f))>> (_f##_S))

//set field value to a variable, used when _f is not left shifted by _f##_S
#define DPORT_VALUE_SET_FIELD(_r, _f, _v) ((_r)=(((_r) & ~((_f) << (_f##_S)))|((_v)<<(_f##_S))))

//set field value to a variable, used when _f is left shifted by _f##_S
#define DPORT_VALUE_SET_FIELD2(_r, _f, _v) ((_r)=(((_r) & ~(_f))|((_v)<<(_f##_S))))

//generate a value from a field value, used when _f is not left shifted by _f##_S
#define DPORT_FIELD_TO_VALUE(_f, _v) (((_v)&(_f))<<_f##_S)

//generate a value from a field value, used when _f is left shifted by _f##_S
#define DPORT_FIELD_TO_VALUE2(_f, _v) (((_v)<<_f##_S) & (_f))

//Register read macros with an underscore prefix access DPORT memory directly. In IDF apps, use the non-underscore versions to be SMP-safe.
#define _DPORT_READ_PERI_REG(addr) (*((volatile uint32_t *)(addr)))
#define _DPORT_WRITE_PERI_REG(addr, val) (*((volatile uint32_t *)(addr))) = (uint32_t)(val)
#define _DPORT_REG_SET_BIT(_r, _b)  _DPORT_REG_WRITE((_r), (_DPORT_REG_READ(_r)|(_b)))
#define _DPORT_REG_CLR_BIT(_r, _b)  _DPORT_REG_WRITE((_r), (_DPORT_REG_READ(_r) & (~(_b))))

//read value from register
static inline uint32_t IRAM_ATTR DPORT_READ_PERI_REG(uint32_t addr) 
{
    uint32_t val;

    DPORT_STALL_OTHER_CPU_START();
    val = _DPORT_READ_PERI_REG(addr);
    DPORT_STALL_OTHER_CPU_END();
    
    return val;
}

//write value to register
#define DPORT_WRITE_PERI_REG(addr, val) _DPORT_WRITE_PERI_REG((addr), (val))

//clear bits of register controlled by mask
#define DPORT_CLEAR_PERI_REG_MASK(reg, mask) DPORT_WRITE_PERI_REG((reg), (DPORT_READ_PERI_REG(reg)&(~(mask))))

//set bits of register controlled by mask
#define DPORT_SET_PERI_REG_MASK(reg, mask)   DPORT_WRITE_PERI_REG((reg), (DPORT_READ_PERI_REG(reg)|(mask)))

//get bits of register controlled by mask
#define DPORT_GET_PERI_REG_MASK(reg, mask)   (DPORT_READ_PERI_REG(reg) & (mask))

//get bits of register controlled by highest bit and lowest bit
#define DPORT_GET_PERI_REG_BITS(reg, hipos,lowpos)     ((DPORT_READ_PERI_REG(reg)>>(lowpos))&((1<<((hipos)-(lowpos)+1))-1))

//set bits of register controlled by mask and shift
#define DPORT_SET_PERI_REG_BITS(reg,bit_map,value,shift) DPORT_WRITE_PERI_REG((reg), ((DPORT_READ_PERI_REG(reg)&(~((bit_map)<<(shift))))|(((value) & bit_map)<<(shift))))

//get field of register
#define DPORT_GET_PERI_REG_BITS2(reg, mask,shift)      ((DPORT_READ_PERI_REG(reg)>>(shift))&(mask))
//}}

#ifdef __cplusplus
}
#endif

#endif /* _DPORT_ACCESS_H_ */
