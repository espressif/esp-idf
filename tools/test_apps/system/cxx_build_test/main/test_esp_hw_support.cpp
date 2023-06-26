/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_cpu.h"
#include "clk_ctrl_os.h"
//#include "clk_tree.h" TODO: outdated header name (IDF-7286)
#include "dport_access.h"
#include "esp_async_memcpy.h"
#include "esp_chip_info.h"
#include "esp_crc.h"
#include "esp_etm.h"
#include "esp_fault.h"
#include "esp_interface.h"
#include "esp_intr_alloc.h"
#include "esp_mac.h"
#include "esp_memory_utils.h"
#include "esp_memprot_err.h"
#include "esp_memprot.h"
#include "esp_memprot_types.h"
#include "esp_random.h"
#include "esp_sleep.h"
#include "esp_wake_stub.h"
#include "intr_types.h"
#include "rtc_wdt.h"
#include "spinlock.h"

#include "soc/soc_caps.h"

#if SOC_HMAC_SUPPORTED
#include "esp_hmac.h"
#endif

#if SOC_DIG_SIGN_SUPPORTED
#include "esp_ds_err.h"
#include "esp_ds.h"
#endif

extern "C" void app_main() { }
