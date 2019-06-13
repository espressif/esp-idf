// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "esp_gdbstub.h"
#include "sdkconfig.h"

#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

/* Internal error codes used by the routines that parse the incoming gdb packet */
#define GDBSTUB_ST_ENDPACKET -1
#define GDBSTUB_ST_ERR -2
#define GDBSTUB_ST_OK -3

/* Special task index values */
#define GDBSTUB_CUR_TASK_INDEX_UNKNOWN -1

/* Cab be set to a lower value in gdbstub_target_config.h */
#ifndef GDBSTUB_CMD_BUFLEN
#define GDBSTUB_CMD_BUFLEN 512
#endif

#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
typedef enum {
    GDBSTUB_NOT_STARTED,
    GDBSTUB_STARTED,
    GDBSTUB_TASK_SUPPORT_DISABLED
} esp_gdbstub_state_t;

#define GDBSTUB_TASKS_NUM  CONFIG_ESP_GDBSTUB_MAX_TASKS

#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

/* gdbstub temporary run-time data, stored in .bss to reduce stack usage */
typedef struct {
    esp_gdbstub_gdb_regfile_t regfile;
    int signal;
#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
    esp_gdbstub_state_t state;
    int task_count;
    int paniced_task_index;
    int current_task_index;
    int thread_info_index;                             //!< index of the last task passed to qsThreadInfo
    esp_gdbstub_frame_t paniced_frame;
    TaskSnapshot_t tasks[GDBSTUB_TASKS_NUM];           // TODO: add an API to get snapshots one by one
#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
} esp_gdbstub_scratch_t;


/**** Functions provided by the architecture specific part ****/

/**
 * @param frame exception frame pointer
 * @return the appropriate "signal" number for the given exception cause
 */
int esp_gdbstub_get_signal(const esp_gdbstub_frame_t *frame);

/**
 * Write registers from the exception frame to the GDB register file
 * @param frame  exception frame to parse
 * @param dst  pointer to the GDB register file
 */
void esp_gdbstub_frame_to_regfile(const esp_gdbstub_frame_t *frame, esp_gdbstub_gdb_regfile_t *dst);

#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
/**
 * Write registers from the saved frame of a given task to the GDB register file
 * @param tcb  pointer to the TCB of the task
 * @param dst  pointer to the GDB register file
 */
void esp_gdbstub_tcb_to_regfile(TaskHandle_t tcb, esp_gdbstub_gdb_regfile_t *dst);
#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS



/**** Functions provided by the target specific part ****/

/**
 * Do target-specific initialization before gdbstub can start communicating.
 * This may involve, for example, configuring the UART.
 */
void esp_gdbstub_target_init();

/**
 * Receive a byte from the GDB client. Blocks until a byte is available.
 * @return received byte
 */
int esp_gdbstub_getchar();

/**
 * Send a byte to the GDB client
 * @param c  byte to send
 */
void esp_gdbstub_putchar(int c);

/**
 * Read a byte from target memory
 * @param ptr  address
 * @return  byte value, or GDBSTUB_ST_ERR if the address is not readable
 */
int esp_gdbstub_readmem(intptr_t addr);


/**** GDB packet related functions ****/

/** Begin a packet */
void esp_gdbstub_send_start();

/** Send a character as part of the packet */
void esp_gdbstub_send_char(char c);

/** Send a string as part of the packet */
void esp_gdbstub_send_str(const char *s);

/** Send a hex value as part of the packet */
void esp_gdbstub_send_hex(int val, int bits);

/** Finish sending the packet */
void esp_gdbstub_send_end();

/** Send a packet with a string as content */
void esp_gdbstub_send_str_packet(const char* str);

/** Get a hex value from the gdb packet */
uint32_t esp_gdbstub_gethex(const unsigned char **ptr, int bits);

/** Read, unescape, and validate the incoming GDB command */
int esp_gdbstub_read_command(unsigned char **out_cmd, size_t *out_size);

/** Handle a command received from gdb */
int esp_gdbstub_handle_command(unsigned char *cmd, int len);

