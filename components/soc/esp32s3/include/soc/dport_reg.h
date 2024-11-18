/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/interrupt_reg.h"
#include "soc/system_reg.h"
#include "soc/sensitive_reg.h"
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DPORT_DATE_REG SYSTEM_DATE_REG

#ifndef __ASSEMBLER__
#include "soc/dport_access.h"
#endif

#ifdef __cplusplus
}
#endif
