/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <sdkconfig.h>
#include "soc/soc_caps.h"
#include "soc/dport_access.h"
#include "esp_attr.h"

IRAM_ATTR uint32_t esp_dport_access_reg_read(uint32_t reg)
{
#if defined(BOOTLOADER_BUILD) || defined(CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE) || !SOC_DPORT_WORKAROUND
    return _DPORT_REG_READ(reg);
#else
    uint32_t apb;
    unsigned int intLvl;
    __asm__ __volatile__ (\
                  "rsil %[LVL], "XTSTR(SOC_DPORT_WORKAROUND_DIS_INTERRUPT_LVL)"\n"\
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

IRAM_ATTR uint32_t esp_dport_access_sequence_reg_read(uint32_t reg)
{
#if defined(BOOTLOADER_BUILD) || defined(CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE) || !SOC_DPORT_WORKAROUND
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
