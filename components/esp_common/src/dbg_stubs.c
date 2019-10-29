// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

// This module implements debug/trace stubs. The stub is a piece of special code which can invoked by OpenOCD
// Currently one stub is used for GCOV functionality
//

#include "eri.h"
#include "xtensa-debug-module.h"
#include "esp_private/dbg_stubs.h"
#include "esp_attr.h"

#if CONFIG_ESP32_DEBUG_STUBS_ENABLE || CONFIG_ESP32S2_DEBUG_STUBS_ENABLE
/*
    Debug stubs is actually a table of 4-byte entries. Every entry is equal to zero or must contain meaningfull data.
    The first entry is a service one and has the followinf format:
        - tramp_addr, 4 bytes; Address of buffer for trampoline/code. Max size is ESP_DBG_STUBS_CODE_BUF_SIZE.
        - min_stack_addr, 4 bytes; Start of the buffer for minimal onboard stack or data. Max size is ESP_DBG_STUBS_STACK_MIN_SIZE.
        - data_alloc, 4 bytes; Address of function to allocate memory on target.
        - data_free, 4 bytes; Address of function to free target memory.
    This entry is used by OpenOCD code to invoke other stub entries and allocate memory for them.
 */

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "esp_dbg_stubs";

#define ESP_DBG_STUBS_TRAX_REG              ERI_TRAX_TRIGGERPC
#define ESP_DBG_STUBS_CODE_BUF_SIZE         32
#define ESP_DBG_STUBS_STACK_MIN_SIZE        2048

#define DBG_STUB_TRAMP_ATTR					IRAM_ATTR

static struct {
    uint32_t    tramp_addr;
    uint32_t    min_stack_addr; // minimal stack addr
    uint32_t    data_alloc;
    uint32_t    data_free;
} s_dbg_stubs_ctl_data;

static uint32_t s_stub_entry[ESP_DBG_STUB_ENTRY_MAX];
static uint8_t s_stub_min_stack[ESP_DBG_STUBS_STACK_MIN_SIZE];
static DBG_STUB_TRAMP_ATTR uint8_t s_stub_code_buf[ESP_DBG_STUBS_CODE_BUF_SIZE];

// TODO: all called funcs should be in IRAM to work with disabled flash cache
static void * esp_dbg_stubs_data_alloc(uint32_t size)
{
    ESP_LOGV(TAG, "%s %d", __func__, size);
	void *p = malloc(size);
    ESP_LOGV(TAG, "%s EXIT %p", __func__, p);
    return p;
}

static void esp_dbg_stubs_data_free(void *addr)
{
    ESP_LOGV(TAG, "%s %p", __func__, addr);
    free(addr);
    ESP_LOGV(TAG, "%s EXIT %p", __func__, addr);
}

void esp_dbg_stubs_init(void)
{
    s_dbg_stubs_ctl_data.tramp_addr     = (uint32_t)s_stub_code_buf;
    s_dbg_stubs_ctl_data.min_stack_addr = (uint32_t)s_stub_min_stack;
    s_dbg_stubs_ctl_data.data_alloc     = (uint32_t)esp_dbg_stubs_data_alloc;
    s_dbg_stubs_ctl_data.data_free      = (uint32_t)esp_dbg_stubs_data_free;

    s_stub_entry[ESP_DBG_STUB_CONTROL_DATA] = (uint32_t)&s_dbg_stubs_ctl_data;
    eri_write(ESP_DBG_STUBS_TRAX_REG, (uint32_t)s_stub_entry);
    ESP_LOGV(TAG, "%s stubs %x", __func__, eri_read(ESP_DBG_STUBS_TRAX_REG));
}

esp_err_t esp_dbg_stub_entry_set(esp_dbg_stub_id_t id, uint32_t entry)
{
    if (id < ESP_DBG_STUB_ENTRY_FIRST || id >= ESP_DBG_STUB_ENTRY_MAX) {
        ESP_LOGE(TAG, "Invalid stub id %d!", id);
        return ESP_ERR_INVALID_ARG;
    }
    s_stub_entry[id] = entry;

    return ESP_OK;
}

#endif
