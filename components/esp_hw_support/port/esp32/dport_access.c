/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <sdkconfig.h>
#include "soc/dport_reg.h"
#include "xtensa/core-macros.h"

/**
 * @brief Read a sequence of DPORT registers to the buffer, SMP-safe version.
 *
 * This implementation uses a method of the pre-reading of the APB register
 * before reading the register of the DPORT, without stall other CPU.
 * There is disable/enable interrupt.
 *
 * @param[out] buff_out  Contains the read data.
 * @param[in]  address   Initial address for reading registers.
 * @param[in]  num_words The number of words.
 */
void IRAM_ATTR esp_dport_access_read_buffer(uint32_t *buff_out, uint32_t address, uint32_t num_words)
{
    DPORT_INTERRUPT_DISABLE();
    for (uint32_t i = 0;  i < num_words; ++i) {
        buff_out[i] = DPORT_SEQUENCE_REG_READ(address + i * 4);
    }
    DPORT_INTERRUPT_RESTORE();
}

/**
 * @brief Read value from register, SMP-safe version.
 *
 * This method uses the pre-reading of the APB register before reading the register of the DPORT.
 * This implementation is useful for reading DORT registers for single reading without stall other CPU.
 * There is disable/enable interrupt.
 *
 * @param reg Register address
 * @return Value
 */
uint32_t IRAM_ATTR esp_dport_access_reg_read(uint32_t reg)
{
#if defined(BOOTLOADER_BUILD) || !defined(CONFIG_ESP32_DPORT_WORKAROUND) || !defined(ESP_PLATFORM)
    return _DPORT_REG_READ(reg);
#else
    uint32_t apb;
    unsigned int intLvl;
    __asm__ __volatile__ (\
                  "rsil %[LVL], "XTSTR(CONFIG_ESP32_DPORT_DIS_INTERRUPT_LVL)"\n"\
                  "movi %[APB], "XTSTR(0x3ff40078)"\n"\
                  "l32i %[APB], %[APB], 0\n"\
                  "l32i %[REG], %[REG], 0\n"\
                  "wsr  %[LVL], "XTSTR(PS)"\n"\
                  "rsync\n"\
                  : [APB]"=a"(apb), [REG]"+a"(reg), [LVL]"=a"(intLvl)\
                  : \
                  : "memory" \
                  );
    return reg;
#endif
}

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
uint32_t IRAM_ATTR esp_dport_access_sequence_reg_read(uint32_t reg)
{
#if defined(BOOTLOADER_BUILD) || !defined(CONFIG_ESP32_DPORT_WORKAROUND) || !defined(ESP_PLATFORM)
    return _DPORT_REG_READ(reg);
#else
    uint32_t apb;
    __asm__ __volatile__ (\
                  "movi %[APB], "XTSTR(0x3ff40078)"\n"\
                  "l32i %[APB], %[APB], 0\n"\
                  "l32i %[REG], %[REG], 0\n"\
                  : [APB]"=a"(apb), [REG]"+a"(reg)\
                  : \
                  : "memory" \
                  );
    return reg;
#endif
}
