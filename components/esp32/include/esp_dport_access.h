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

#ifndef _ESP_DPORT_ACCESS_H_
#define _ESP_DPORT_ACCESS_H_

#include <stdint.h>

void esp_dport_access_stall_other_cpu_start(void);
void esp_dport_access_stall_other_cpu_end(void);
void esp_dport_access_int_init(void);

#if defined(BOOTLOADER_BUILD) || defined(CONFIG_FREERTOS_UNICORE)
#define DPORT_STAL_OTHER_CPU_START()
#define DPORT_STAL_OTHER_CPU_END()
#else
#define DPORT_STAL_OTHER_CPU_START()   esp_dport_access_stall_other_cpu_start()
#define DPORT_STAL_OTHER_CPU_END()     esp_dport_access_stall_other_cpu_end()
#endif

#define IS_DPORT_REG(_r) (((_r) >= DR_REG_DPORT_BASE) && (_r) <= DPORT_DATE_REG)

//Registers Operation {{

#define _REG_WRITE(_r, _v)   (*(volatile uint32_t *)(_r)) = (_v)
#define _REG_READ(_r)        (*(volatile uint32_t *)(_r))
//write value to register
#define DPORT_REG_WRITE(_r, _v)    {DPORT_STAL_OTHER_CPU_START(); (*(volatile uint32_t *)(_r)) = (_v); DPORT_STAL_OTHER_CPU_END();}

//read value from register
#define DPORT_REG_READ(_r) ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = (*(volatile uint32_t *)(_r)); DPORT_STAL_OTHER_CPU_END(); val;})

//get bit or get bits from register
#define DPORT_REG_GET_BIT(_r, _b)  ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = (*(volatile uint32_t*)(_r) & (_b)); DPORT_STAL_OTHER_CPU_END(); val;})

//set bit or set bits to register
#define DPORT_REG_SET_BIT(_r, _b)  {DPORT_STAL_OTHER_CPU_START(); (*(volatile uint32_t*)(_r) |= (_b)); DPORT_STAL_OTHER_CPU_END();}

//clear bit or clear bits of register
#define DPORT_REG_CLR_BIT(_r, _b)  {DPORT_STAL_OTHER_CPU_START(); (*(volatile uint32_t*)(_r) &= ~(_b)); DPORT_STAL_OTHER_CPU_END();}

//set bits of register controlled by mask
#define DPORT_REG_SET_BITS(_r, _b, _m) {DPORT_STAL_OTHER_CPU_START(); (*(volatile uint32_t*)(_r) = (*(volatile uint32_t*)(_r) & ~(_m)) | ((_b) & (_m))); DPORT_STAL_OTHER_CPU_END();}

//get field from register, uses field _S & _V to determine mask
#define DPORT_REG_GET_FIELD(_r, _f) ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = ((_REG_READ(_r) >> (_f##_S)) & (_f##_V)); DPORT_STAL_OTHER_CPU_END(); val;})

//set field to register, used when _f is not left shifted by _f##_S
#define DPORT_REG_SET_FIELD(_r, _f, _v) {DPORT_STAL_OTHER_CPU_START(); (_REG_WRITE((_r),((_REG_READ(_r) & ~((_f) << (_f##_S)))|(((_v) & (_f))<<(_f##_S))))); DPORT_STAL_OTHER_CPU_END();}

//get field value from a variable, used when _f is not left shifted by _f##_S
#define DPORT_VALUE_GET_FIELD(_r, _f) ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = (((_r) >> (_f##_S)) & (_f)); DPORT_STAL_OTHER_CPU_END(); val;})

//get field value from a variable, used when _f is left shifted by _f##_S
#define DPORT_VALUE_GET_FIELD2(_r, _f) ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = (((_r) & (_f))>> (_f##_S)); DPORT_STAL_OTHER_CPU_END(); val;})

//set field value to a variable, used when _f is not left shifted by _f##_S
#define DPORT_VALUE_SET_FIELD(_r, _f, _v) {DPORT_STAL_OTHER_CPU_START(); ((_r)=(((_r) & ~((_f) << (_f##_S)))|((_v)<<(_f##_S)))); DPORT_STAL_OTHER_CPU_END();}

//set field value to a variable, used when _f is left shifted by _f##_S
#define DPORT_VALUE_SET_FIELD2(_r, _f, _v) {DPORT_STAL_OTHER_CPU_START(); ((_r)=(((_r) & ~(_f))|((_v)<<(_f##_S)))); DPORT_STAL_OTHER_CPU_END();}

//generate a value from a field value, used when _f is not left shifted by _f##_S
#define DPORT_FIELD_TO_VALUE(_f, _v) ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = (((_v)&(_f))<<_f##_S); DPORT_STAL_OTHER_CPU_END(); val;})

//generate a value from a field value, used when _f is left shifted by _f##_S
#define DPORT_FIELD_TO_VALUE2(_f, _v) ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = (((_v)<<_f##_S) & (_f)); DPORT_STAL_OTHER_CPU_END(); val;})

#define _READ_PERI_REG(addr) (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) 
#define _WRITE_PERI_REG(addr, val) (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val) 

//read value from register
#define DPORT_READ_PERI_REG(addr) ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))); DPORT_STAL_OTHER_CPU_END(); val;})

//write value to register
#define DPORT_WRITE_PERI_REG(addr, val) {DPORT_STAL_OTHER_CPU_START(); (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val); DPORT_STAL_OTHER_CPU_END();}   

//clear bits of register controlled by mask
#define DPORT_CLEAR_PERI_REG_MASK(reg, mask) {DPORT_STAL_OTHER_CPU_START(); _WRITE_PERI_REG((reg), (_READ_PERI_REG(reg)&(~(mask)))); DPORT_STAL_OTHER_CPU_END();}

//set bits of register controlled by mask
#define DPORT_SET_PERI_REG_MASK(reg, mask)   {DPORT_STAL_OTHER_CPU_START(); _WRITE_PERI_REG((reg), (_READ_PERI_REG(reg)|(mask))); DPORT_STAL_OTHER_CPU_END();}

//get bits of register controlled by mask
#define DPORT_GET_PERI_REG_MASK(reg, mask)   ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = (_READ_PERI_REG(reg) & (mask)); DPORT_STAL_OTHER_CPU_END(); val;})

//get bits of register controlled by highest bit and lowest bit
#define DPORT_GET_PERI_REG_BITS(reg, hipos,lowpos)      ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = ((_READ_PERI_REG(reg)>>(lowpos))&((1<<((hipos)-(lowpos)+1))-1)); DPORT_STAL_OTHER_CPU_END(); val;})

//set bits of register controlled by mask and shift
#define DPORT_SET_PERI_REG_BITS(reg,bit_map,value,shift) {DPORT_STAL_OTHER_CPU_START(); (_WRITE_PERI_REG((reg),(_READ_PERI_REG(reg)&(~((bit_map)<<(shift))))|(((value) & bit_map)<<(shift)) )); DPORT_STAL_OTHER_CPU_END();}

//get field of register
#define DPORT_GET_PERI_REG_BITS2(reg, mask,shift)      ({uint32_t val; DPORT_STAL_OTHER_CPU_START(); val = ((_READ_PERI_REG(reg)>>(shift))&(mask)); DPORT_STAL_OTHER_CPU_END(); val;})
//}}


#endif /* _ESP_DPORT_ACCESS_H_ */
