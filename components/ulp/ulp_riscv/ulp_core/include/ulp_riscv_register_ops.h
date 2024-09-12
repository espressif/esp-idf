/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//Registers Operation {{

/*
 * When COCPU accesses the RTC register, it needs to convert the access address.
 * When COCPU accesses the RTC memory, dont need to convert the access address.
 */
#define WRITE_RTC_MEM(addr, val)    (*((volatile int*)(addr))) = (int) (val)
#define READ_RTC_MEM(addr)		    (*(volatile int*)(addr))

/*
 * When COCPU accesses the RTC register, it needs to convert the access address.
 * When COCPU accesses the RTC memory, dont need to convert the access address.
 */
#define RISCV_REG_CONV(addr)        (((addr&0xffff)<<3 & 0xe000) | (addr & 0x1fff) | 0x8000)
#define ETS_UNCACHED_ADDR(addr)     (RISCV_REG_CONV(addr))

#ifndef __ASSEMBLER__
#define BIT(nr)                 (1UL << (nr))
#else
#define BIT(nr)                 (1 << (nr))
#endif

//write value to register
#define REG_WRITE(_r, _v) ({                                                                                           \
            (*(volatile uint32_t *)RISCV_REG_CONV(_r)) = (_v);                                                                       \
        })

//read value from register
#define REG_READ(_r) ({                                                                                                \
            (*(volatile uint32_t *)RISCV_REG_CONV(_r));                                                                              \
        })

//get bit or get bits from register
#define REG_GET_BIT(_r, _b)  ({                                                                                        \
            (*(volatile uint32_t*)RISCV_REG_CONV(_r) & (_b));                                                                        \
        })

//set bit or set bits to register
#define REG_SET_BIT(_r, _b)  ({                                                                                        \
            (*(volatile uint32_t*)RISCV_REG_CONV(_r) |= (_b));                                                                       \
        })

//clear bit or clear bits of register
#define REG_CLR_BIT(_r, _b)  ({                                                                                        \
            (*(volatile uint32_t*)RISCV_REG_CONV(_r) &= ~(_b));                                                                      \
        })

//set bits of register controlled by mask
#define REG_SET_BITS(_r, _b, _m) ({                                                                                    \
            (*(volatile uint32_t*)RISCV_REG_CONV(_r) = (*(volatile uint32_t*)RISCV_REG_CONV(_r) & ~(_m)) | ((_b) & (_m)));                         \
        })

//get field from register, uses field _S & _V to determine mask
#define REG_GET_FIELD(_r, _f) ({                                                                                       \
            ((REG_READ(_r) >> (_f##_S)) & (_f##_V));                                                                   \
        })

//set field of a register from variable, uses field _S & _V to determine mask
#define REG_SET_FIELD(_r, _f, _v) ({                                                                                   \
            (REG_WRITE((_r),((REG_READ(_r) & ~((_f##_V) << (_f##_S)))|(((_v) & (_f##_V))<<(_f##_S)))));                \
        })

//get field value from a variable, used when _f is not left shifted by _f##_S
#define VALUE_GET_FIELD(_r, _f) (((_r) >> (_f##_S)) & (_f))

//get field value from a variable, used when _f is left shifted by _f##_S
#define VALUE_GET_FIELD2(_r, _f) (((_r) & (_f))>> (_f##_S))

//set field value to a variable, used when _f is not left shifted by _f##_S
#define VALUE_SET_FIELD(_r, _f, _v) ((_r)=(((_r) & ~((_f) << (_f##_S)))|((_v)<<(_f##_S))))

//set field value to a variable, used when _f is left shifted by _f##_S
#define VALUE_SET_FIELD2(_r, _f, _v) ((_r)=(((_r) & ~(_f))|((_v)<<(_f##_S))))

//generate a value from a field value, used when _f is not left shifted by _f##_S
#define FIELD_TO_VALUE(_f, _v) (((_v)&(_f))<<_f##_S)

//generate a value from a field value, used when _f is left shifted by _f##_S
#define FIELD_TO_VALUE2(_f, _v) (((_v)<<_f##_S) & (_f))

//read value from register
#define READ_PERI_REG(addr) ({                                                                                         \
            (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr)));                                                         \
        })

//write value to register
#define WRITE_PERI_REG(addr, val) ({                                                                                   \
            (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val);                                       \
        })

//clear bits of register controlled by mask
#define CLEAR_PERI_REG_MASK(reg, mask) ({                                                                              \
            WRITE_PERI_REG((reg), (READ_PERI_REG(reg)&(~(mask))));                                                     \
        })

//set bits of register controlled by mask
#define SET_PERI_REG_MASK(reg, mask) ({                                                                                \
            WRITE_PERI_REG((reg), (READ_PERI_REG(reg)|(mask)));                                                        \
        })

//get bits of register controlled by mask
#define GET_PERI_REG_MASK(reg, mask) ({                                                                                \
            (READ_PERI_REG(reg) & (mask));                                                                             \
        })

//get bits of register controlled by highest bit and lowest bit
// #define GET_PERI_REG_BITS(reg, hipos,lowpos) ({
//             ASSERT_IF_DPORT_REG((reg), GET_PERI_REG_BITS);
//             ((READ_PERI_REG(reg)>>(lowpos))&((1UL<<((hipos)-(lowpos)+1))-1));
//         })
#define GET_PERI_REG_BITS(reg, bit_map, shift)    ((READ_PERI_REG(reg))&((bit_map)<<(shift)))>>shift

//set bits of register controlled by mask and shift
#define SET_PERI_REG_BITS(reg,bit_map,value,shift) ({                                                                  \
            (WRITE_PERI_REG((reg),(READ_PERI_REG(reg)&(~((bit_map)<<(shift))))|(((value) & bit_map)<<(shift)) ));      \
        })

//get field of register
#define GET_PERI_REG_BITS2(reg, mask,shift) ({                                                                         \
            ((READ_PERI_REG(reg)>>(shift))&(mask));                                                                    \
        })
//}}

#ifdef __cplusplus
}
#endif
