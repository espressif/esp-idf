/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * SOC Capability evaluation helpers
 *
 * These macros provide a standardized way to query SOC capabilities without directly
 * accessing internal implementation macros in soc_caps_full.h and public macros in soc_caps.h.
 *
 * The main categories of macros are:
 * - SOC_IS : Check if the SOC is a specific target (e.g., SOC_IS(ESP32S3))
 * - SOC_MODULE_ATTR : Get a specific attribute of a module (e.g., SOC_MODULE_ATTR(GPTIMER, NUM))
 * - SOC_MODULE_SUPPORT : Check if a module supports a feature (e.g., SOC_MODULE_SUPPORT(GPTIMER, ETM))
 */

#define _SOC_CAPS_EVAL(_name)               _SOC_CAPS_ ## _name

/* User-facing semantic macros */
// Check if the SOC has a specific module
#define SOC_HAS(_module)                    SOC_ ## _module ## _SUPPORTED
// Check if the SOC is a specific target
#define SOC_IS(_target)                     _SOC_CAPS_EVAL(TARGET_IS_ ## _target)
// Generic query macro, which can be used to get any module attribute (e.g. version, ID, number, etc.)
#define SOC_MODULE_ATTR(_module, _attr)     _SOC_CAPS_EVAL(_module ## _ ## _attr)
// Generic support check macro, which can be used to check if a module supports a specific feature (e.g. ETM, DMA, etc.)
#define SOC_MODULE_SUPPORT(_module, _feat)  _SOC_CAPS_EVAL(_module ## _SUPPORT_ ## _feat)
