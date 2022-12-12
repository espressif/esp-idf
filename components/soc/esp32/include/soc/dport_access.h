/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _DPORT_ACCESS_H_
#define _DPORT_ACCESS_H_

#include <stdint.h>
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

//Registers Operation {{

// The _DPORT_xxx register read macros access DPORT memory directly (as opposed to
// DPORT_REG_READ which applies SMP-safe protections).
//
// There are several ways to read the DPORT registers:
// 1) Use DPORT_REG_READ versions to be SMP-safe in IDF apps.
//    This method uses the pre-read APB implementation(*) without stall other CPU.
//    This is beneficial for single readings.
// 2) If you want to make a sequence of DPORT reads to buffer,
//    use esp_dport_access_read_buffer(buff_out, address, num_words),
//    it is the faster method and it doesn't stop other CPU.
// 3) If you want to make a sequence of DPORT reads, but you don't want to stop other CPU
//    and you want to do it faster then you need use DPORT_SEQUENCE_REG_READ().
//    The difference from (2) is that the user himself must disable interrupts while DPORT reading.
//    Note that disable interrupt need only if the chip has two cores.
// 4) If you want to make a sequence of DPORT reads,
//    use DPORT_STALL_OTHER_CPU_START() macro explicitly
//    and then use _DPORT_REG_READ macro while other CPU is stalled.
//    After completing read operations, use DPORT_STALL_OTHER_CPU_END().
//    This method uses stall other CPU while reading DPORT registers.
//    Useful for compatibility, as well as for large consecutive readings.
//    This method is slower, but must be used if ROM functions or
//    other code is called which accesses DPORT without any other workaround.
// *) The pre-readable APB register before reading the DPORT register
//    helps synchronize the operation of the two CPUs,
//    so that reading on different CPUs no longer causes random errors APB register.

// _DPORT_REG_WRITE & DPORT_REG_WRITE are equivalent.
#define _DPORT_REG_READ(_r)        (*(volatile uint32_t *)(_r))
#define _DPORT_REG_WRITE(_r, _v)   (*(volatile uint32_t *)(_r)) = (_v)

// Write value to DPORT register (does not require protecting)
#define DPORT_REG_WRITE(_r, _v)   _DPORT_REG_WRITE((_r), (_v))

/**
 * @brief Read value from register, SMP-safe version.
 *
 * This method uses the pre-reading of the APB register before reading the register of the DPORT.
 * This implementation is useful for reading DORT registers for single reading without stall other CPU.
 * There is disable/enable interrupt.
 *
 * @note Should be placed in IRAM.
 *
 * @param reg Register address
 * @return Value
 */
uint32_t esp_dport_access_reg_read(uint32_t reg);

/**
 * @brief Read value from register, NOT SMP-safe version.
 *
 * This method uses the pre-reading of the APB register before reading the register of the DPORT.
 * There is not disable/enable interrupt.
 * The difference from DPORT_REG_READ() is that the user himself must disable interrupts while DPORT reading.
 * This implementation is useful for reading DORT registers in loop without stall other CPU. Note the usage example.
 * The recommended way to read registers sequentially without stall other CPU
 * is to use the method esp_dport_read_buffer(buff_out, address, num_words). It allows you to read registers in the buffer.
 *
 * @note Should be placed in IRAM.
 *
 * \code{c}
 * // This example shows how to use it.
 * { // Use curly brackets to limit the visibility of variables in macros DPORT_INTERRUPT_DISABLE/RESTORE.
 *     DPORT_INTERRUPT_DISABLE(); // Disable interrupt only on current CPU.
 *     for (i = 0; i < max; ++i) {
 *        array[i] = esp_dport_access_sequence_reg_read(Address + i * 4); // reading DPORT registers
 *     }
 *     DPORT_INTERRUPT_RESTORE(); // restore the previous interrupt level
 * }
 * \endcode
 *
 * @param reg Register address
 * @return Value
 */
uint32_t esp_dport_access_sequence_reg_read(uint32_t reg);

/**
 * @brief Read a sequence of DPORT registers to the buffer, SMP-safe version.
 *
 * This implementation uses a method of the pre-reading of the APB register
 * before reading the register of the DPORT, without stall other CPU.
 * There is disable/enable interrupt.
 *
 * @note Should be placed in IRAM.
 *
 * @param[out] buff_out  Contains the read data.
 * @param[in]  address   Initial address for reading registers.
 * @param[in]  num_words The number of words.
 */
void esp_dport_access_read_buffer(uint32_t *buff_out, uint32_t address, uint32_t num_words);

#if defined(BOOTLOADER_BUILD) || defined(CONFIG_FREERTOS_UNICORE) || !SOC_DPORT_WORKAROUND
    #define DPORT_INTERRUPT_DISABLE()
    #define DPORT_INTERRUPT_RESTORE()
    #define DPORT_REG_READ(reg)          _DPORT_REG_READ(reg)
    #define DPORT_SEQUENCE_REG_READ(reg) _DPORT_REG_READ(reg)
#else
    #define DPORT_REG_READ(reg)          esp_dport_access_reg_read(reg)
    #define DPORT_SEQUENCE_REG_READ(reg) esp_dport_access_sequence_reg_read(reg)
    #ifndef XTSTR
    #define _XTSTR(x) # x
    #define XTSTR(x)  _XTSTR(x)
    #endif

    #define DPORT_INTERRUPT_DISABLE() unsigned intLvl = __extension__({ unsigned __tmp; \
                __asm__ __volatile__("rsil %0, " XTSTR(SOC_DPORT_WORKAROUND_DIS_INTERRUPT_LVL) "\n" \
                                    : "=a" (__tmp) : : "memory" ); \
                __tmp;})
    #define DPORT_INTERRUPT_RESTORE() do{ unsigned __tmp = (intLvl); \
                __asm__ __volatile__("wsr.ps %0 ; rsync\n" \
                                    : : "a" (__tmp) : "memory" ); \
                }while(0)
#endif // defined(BOOTLOADER_BUILD) || defined(CONFIG_FREERTOS_UNICORE) || !SOC_DPORT_WORKAROUND

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

#define DPORT_READ_PERI_REG(reg)  DPORT_REG_READ(reg)

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
