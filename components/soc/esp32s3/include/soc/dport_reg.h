/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "interrupt_reg.h"
#include "system_reg.h"
#include "sensitive_reg.h"
#include "soc.h"

#define DPORT_DATE_REG SYSTEM_DATE_REG

#ifndef __ASSEMBLER__
#include "dport_access.h"
#endif

#ifdef __cplusplus
}
#endif
