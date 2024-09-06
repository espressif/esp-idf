/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_DPORT_REG_H_
#define _SOC_DPORT_REG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/interrupt_reg.h"
#include "soc/system_reg.h"
#include "soc/sensitive_reg.h"
#include "soc/soc.h"

#define DPORT_DATE_REG SYSTEM_DATE_REG

#ifndef __ASSEMBLER__
#include "soc/dport_access.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /*_SOC_DPORT_REG_H_ */
