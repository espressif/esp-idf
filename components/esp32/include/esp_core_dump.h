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
#ifndef ESP_CORE_DUMP_H_
#define ESP_CORE_DUMP_H_

/**
 * @brief  Initializes core dump module internal data.
 *
 * @note  Should be called at system startup.
 */
void esp_core_dump_init();

/**
 * @brief  Saves core dump to flash.
 *
 * The structure of data stored in flash is as follows:
 * |   MAGIC1   |
 * |  TOTAL_LEN |  TASKS_NUM | TCB_SIZE |
 * | TCB_ADDR_1 | STACK_TOP_1 | STACK_END_1 | TCB_1 | STACK_1 |
 * .            .       .         .
 * .            .       .         .
 * | TCB_ADDR_N | STACK_TOP_N | STACK_END_N | TCB_N | STACK_N |
 * |   MAGIC2   |
 * Core dump in flash consists of header and data for every task in the system at the moment of crash.
 * For flash data integrity control two magic numbers are used at the beginning and the end of core dump.
 * The structure of core dump data is described below in details.
 * 1) MAGIC1 and MAGIC2 are special numbers stored at the beginning and the end of core dump.
 *    They are used to control core dump data integrity. Size of every number is 4 bytes.
 * 2) Core dump starts with header:
 * 2.1) TOTAL_LEN is total length of core dump data in flash including magic numbers. Size is 4 bytes.
 * 2.2) TASKS_NUM is the number of tasks for which data are stored. Size is 4 bytes.
 * 2.3) TCB_SIZE is the size of task's TCB structure. Size is 4 bytes.
 * 3) Core dump header is followed by the data for every task in the system.
 *    Task data are started with task header:
 * 3.1) TCB_ADDR is the address of TCB in memory. Size is 4 bytes.
 * 3.2) STACK_TOP is the top of task's stack (address of the topmost stack item). Size is 4 bytes.
 * 3.2) STACK_END is the end of task's stack (address from which task's stack starts). Size is 4 bytes.
 * 4) Task header is followed by TCB data. Size is TCB_SIZE bytes.
 * 5) Task's stack is placed after TCB data. Size is (STACK_END - STACK_TOP) bytes.
 */
void esp_core_dump_to_flash();

/**
 * @brief  Print base64-encoded core dump to UART.
 *
 * The structure of core dump data is the same as for data stored in flash (@see esp_core_dump_to_flash) with some notes:
 * 1) Magic numbers are not present in core dump printed to UART.
 * 2) Since magic numbers are omitted TOTAL_LEN does not include their size.
 * 3) Printed base64 data are surrounded with special messages to help user recognize the start and end of actual data.
 */
void esp_core_dump_to_uart();

#endif
