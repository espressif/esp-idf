// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#include "esp_attr.h"
#include "esp_panic.h"
#include "esp_partition.h"

#ifdef ESP_PLATFORM
// Uncomment this line to force output from this module
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
static const char* TAG = "esp_core_dump_init";
#else
#define ESP_LOGD(...)
#endif

// TODO: allow user to set this in menuconfig or get tasks iteratively
#define COREDUMP_MAX_TASKS_NUM    32

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH

// magic numbers to control core dump data consistency
#define COREDUMP_FLASH_MAGIC_START    0xDEADBEEFUL
#define COREDUMP_FLASH_MAGIC_END      0xACDCFEEDUL

// core dump partition start
static uint32_t s_core_part_start;
// core dump partition size
static uint32_t s_core_part_size;

static uint32_t esp_core_dump_write_flash_padded(size_t off, uint8_t *data, uint32_t data_size)
{
    esp_err_t err;
    uint32_t data_len = 0, k, len;
    union
    {
        uint8_t    data8[4];
        uint32_t   data32;
    } rom_data;

    data_len = (data_size / sizeof(uint32_t)) * sizeof(uint32_t);
    err = spi_flash_write_panic(off, data, data_len);
    if (err != ESP_OK) {
        esp_panicPutStr("ERROR: Failed to write data");
        esp_panicPutHex(err);
        esp_panicPutStr("!\r\n");
        return 0;
    }

    len = data_size % sizeof(uint32_t);
    if (len) {
        // write last bytes with padding, actual TCB len can be retrieved by esptool from core dump header
        rom_data.data32 = 0;
        for (k = 0; k < len; k++)
            rom_data.data8[k] = *(data + data_len + k);
        err = spi_flash_write_panic(off + data_len, &rom_data, sizeof(uint32_t));
        if (err != ESP_OK) {
            esp_panicPutStr("ERROR: Failed to write data end");
            esp_panicPutHex(err);
            esp_panicPutStr("!\r\n");
            return 0;
        }
        data_len += sizeof(uint32_t);
    }

    return data_len;
}

/*
 * |   MAGIC1   |
 * |  TOTAL_LEN |  TASKS_NUM | TCB_SIZE |
 * | TCB_ADDR_1 | STACK_TOP_1 | STACK_END_1 | TCB_1 | STACK_1 |
 * .            .       .         .
 * .            .       .         .
 * | TCB_ADDR_N | STACK_TOP_N | STACK_END_N | TCB_N | STACK_N |
 * |   MAGIC2   |
 */
void esp_core_dump_to_flash(XtExcFrame *frame)
{
    union
    {
        uint8_t    data8[16];
        uint32_t   data32[4];
    } rom_data;
    //const esp_partition_t *core_part;
    esp_err_t err;
    TaskSnapshot_t tasks[COREDUMP_MAX_TASKS_NUM];
    UBaseType_t tcb_sz, task_num;
    uint32_t data_len = 0, i, len, sec_num;
    size_t off;

    esp_panicPutStr("Save core dump to flash...\r\n");
    task_num = uxTaskGetSnapshotAll(tasks, COREDUMP_MAX_TASKS_NUM, &tcb_sz);
    // take TCB padding into account, actual TCB size will be stored in header
    if (tcb_sz % sizeof(uint32_t))
        len = (tcb_sz / sizeof(uint32_t) + 1) * sizeof(uint32_t);
    else
        len = tcb_sz;
    // header + magic2 + tasknum*(tcb + stack start/end + tcb addr)
    data_len = 5*sizeof(uint32_t) + task_num*(len + 2*sizeof(uint32_t) + sizeof(uint32_t *));
    for (i = 0; i < task_num; i++) {
        if (tasks[i].pxTCB == xTaskGetCurrentTaskHandle()) {
            // set correct stack top for current task
            tasks[i].pxTopOfStack = (StackType_t *)frame;
            esp_panicPutStr("Current task PC/A0/SP ");
            esp_panicPutHex(frame->pc);
            esp_panicPutStr(" ");
            esp_panicPutHex(frame->a0);
            esp_panicPutStr(" ");
            esp_panicPutHex(frame->a1);
            esp_panicPutStr("\r\n");
        }
#if( portSTACK_GROWTH < 0 )
        len = (uint32_t)tasks[i].pxEndOfStack - (uint32_t)tasks[i].pxTopOfStack;
#else
        len = (uint32_t)tasks[i].pxTopOfStack - (uint32_t)tasks[i].pxEndOfStack;
#endif
        esp_panicPutStr("stack len = ");
        esp_panicPutHex(len);
        esp_panicPutStr(" ");
        esp_panicPutHex((int)tasks[i].pxTopOfStack);
        esp_panicPutStr(" ");
        esp_panicPutHex((int)tasks[i].pxEndOfStack);
        esp_panicPutStr("\r\n");
        // take stack padding into account
        if (len % sizeof(uint32_t))
            len = (len / sizeof(uint32_t) + 1) * sizeof(uint32_t);
        data_len += len;
    }
    esp_panicPutStr("Core dump len =");
    esp_panicPutHex(data_len);
    esp_panicPutStr("\r\n");
    if (data_len > s_core_part_size) {
        esp_panicPutStr("ERROR: Not enough space to save core dump!");
        return;
    }

    // TEST READ START
    err = spi_flash_read_panic(s_core_part_start + 0, &rom_data, sizeof(rom_data));
    if (err != ESP_OK) {
        esp_panicPutStr("ERROR: Failed to read flash ");
        esp_panicPutHex(err);
        esp_panicPutStr("!\r\n");
        return;
    }
    else {
        esp_panicPutStr("Data from flash:\r\n");
        for (i = 0; i < sizeof(rom_data)/sizeof(rom_data.data32[0]); i++) {
            esp_panicPutHex(rom_data.data32[i]);
            esp_panicPutStr("\r\n");
        }
//            rom_data[4] = 0;
//            esp_panicPutStr(rom_data);
//        esp_panicPutStr("\r\n");
    }
    // TEST READ END

    sec_num = data_len / SPI_FLASH_SEC_SIZE;
    if (data_len % SPI_FLASH_SEC_SIZE)
        sec_num++;
    err = spi_flash_erase_range_panic(s_core_part_start + 0, sec_num * SPI_FLASH_SEC_SIZE);
    if (err != ESP_OK) {
        esp_panicPutStr("ERROR: Failed to erase flash ");
        esp_panicPutHex(err);
        esp_panicPutStr("!\r\n");
        return;
    }

    rom_data.data32[0] = COREDUMP_FLASH_MAGIC_START;
    rom_data.data32[1] = data_len;
    rom_data.data32[2] = task_num;
    rom_data.data32[3] = tcb_sz;
    err = spi_flash_write_panic(s_core_part_start + 0, &rom_data, sizeof(rom_data));
    if (err != ESP_OK) {
        esp_panicPutStr("ERROR: Failed to write core dump header ");
        esp_panicPutHex(err);
        esp_panicPutStr("!\r\n");
        return;
    }
    off = sizeof(rom_data);

    for (i = 0; i < task_num; i++) {
        esp_panicPutStr("Dump task ");
        esp_panicPutHex((int)tasks[i].pxTCB);
        esp_panicPutStr("\r\n");

        // save TCB address, stack base and stack top addr
        rom_data.data32[0] = (uint32_t)tasks[i].pxTCB;
        rom_data.data32[1] = (uint32_t)tasks[i].pxTopOfStack;
        rom_data.data32[2] = (uint32_t)tasks[i].pxEndOfStack;
        err = spi_flash_write_panic(s_core_part_start + off, &rom_data, 3*sizeof(uint32_t));
        if (err != ESP_OK) {
            esp_panicPutStr("ERROR: Failed to write task header ");
            esp_panicPutHex(err);
            esp_panicPutStr("!\r\n");
            return;
        }
        off += 3*sizeof(uint32_t);
        // save TCB
        len = esp_core_dump_write_flash_padded(s_core_part_start + off, tasks[i].pxTCB, tcb_sz);
        if (len == 0)
            return;
        off += len;
        // save task stack
        /*int k;
        for (k = 0; k < 8*4; k++) {
            esp_panicPutStr("stack[");
            esp_panicPutDec(k);
            esp_panicPutStr("] = ");
            esp_panicPutHex(((uint8_t *)tasks[i].pxTopOfStack)[k]);
            esp_panicPutStr("\r\n");
        }*/
        len = esp_core_dump_write_flash_padded(s_core_part_start + off,
#if( portSTACK_GROWTH < 0 )
                tasks[i].pxTopOfStack,
                (uint32_t)tasks[i].pxEndOfStack - (uint32_t)tasks[i].pxTopOfStack
#else
                tasks[i].pxEndOfStack,
                (uint32_t)tasks[i].pxTopOfStack - (uint32_t)tasks[i].pxEndOfStack
#endif
                );
        if (len == 0)
            return;
        off += len;
    }

    rom_data.data32[0] = COREDUMP_FLASH_MAGIC_END;
    err = spi_flash_write_panic(s_core_part_start + off, &rom_data, sizeof(uint32_t));
    if (err != ESP_OK) {
        esp_panicPutStr("Failed to write to flash ");
        esp_panicPutHex(err);
        esp_panicPutStr("!\r\n");
        return;
    }

    esp_panicPutStr("Core dump has been saved to flash partition.\r\n");
}
#endif

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_UART
void esp_core_dump_to_uart(XtExcFrame *frame)
{
}
#endif

void esp_core_dump_init()
{
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
    const esp_partition_t *core_part;

    core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, NULL);
    if (!core_part) {
        ESP_LOGE(TAG, "No core dump partition found!");
        return;
    }
    ESP_LOGI(TAG, "Found partition '%s' @ %x %d bytes", core_part->label, core_part->address, core_part->size);
    s_core_part_start = core_part->address;
    s_core_part_size = core_part->size;
#endif
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_UART
#endif
}

