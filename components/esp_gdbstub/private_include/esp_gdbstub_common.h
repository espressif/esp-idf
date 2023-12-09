/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "esp_gdbstub_arch.h"
#include "sdkconfig.h"

#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_private/freertos_debug.h"
#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

/* Internal error codes used by the routines that parse the incoming gdb packet */
#define GDBSTUB_ST_ENDPACKET -1
#define GDBSTUB_ST_ERR -2
#define GDBSTUB_ST_OK -3
#define GDBSTUB_ST_CONT -4

/* Special task index values */
#define GDBSTUB_CUR_TASK_INDEX_UNKNOWN -1

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

extern esp_gdbstub_scratch_t s_scratch;
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

/**
 * Signal handler for debugging interrupts of the application.
 */
void esp_gdbstub_int(void *frame);

/**
 * Signal handler for transport protocol interrupts.
 */
void gdbstub_handle_uart_int(esp_gdbstub_frame_t *regs_frame);

#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
/**
 * Write registers from the saved frame of a given task to the GDB register file
 * @param tcb  pointer to the TCB of the task
 * @param dst  pointer to the GDB register file
 */
void esp_gdbstub_tcb_to_regfile(TaskHandle_t tcb, esp_gdbstub_gdb_regfile_t *dst);
#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS


/**** UART related functions ****/

/**
 * Receive a byte from the GDB client. Blocks until a byte is available.
 * @return received byte
 */
int esp_gdbstub_getchar(void);

/**
 * Send a byte to the GDB client
 * @param c  byte to send
 */
void esp_gdbstub_putchar(int c);

/**
 * Make sure all bytes sent using putchar() end up at the host.
 * (Usually stubbed for UART, but can be useful for other channels)
 */
void esp_gdbstub_flush(void);

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
/**
 * Read a data from fifo and detect start symbol
 * @return  1 if break symbol was detected, or 0 if not
 */
int esp_gdbstub_getfifo(void);
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

/**** GDB packet related functions ****/

/** Begin a packet */
void esp_gdbstub_send_start(void);

/** Send a character as part of the packet */
void esp_gdbstub_send_char(char c);

/** Send a string as part of the packet */
void esp_gdbstub_send_str(const char *s);

/** Send a hex value as part of the packet */
void esp_gdbstub_send_hex(int val, int bits);

/** Finish sending the packet */
void esp_gdbstub_send_end(void);

/** Send a packet with a string as content */
void esp_gdbstub_send_str_packet(const char *str);

/** Get a hex value from the gdb packet */
uint32_t esp_gdbstub_gethex(const unsigned char **ptr, int bits);

/** Read, unescape, and validate the incoming GDB command */
int esp_gdbstub_read_command(unsigned char **out_cmd, size_t *out_size);

/** Handle a command received from gdb */
int esp_gdbstub_handle_command(unsigned char *cmd, int len);

void esp_gdbstub_init_dports(void);
void esp_gdbstub_stall_other_cpus_start(void);
void esp_gdbstub_stall_other_cpus_end(void);

void esp_gdbstub_clear_step(void);
void esp_gdbstub_do_step(esp_gdbstub_frame_t *regs_frame);
void esp_gdbstub_trigger_cpu(void);

/**
 * Write a value to register in frame
 * @param frame  gdbstub frame
 * @param reg_index  register index, depends on architecture
 * @param value  32 bit data value
 */
void esp_gdbstub_set_register(esp_gdbstub_frame_t *frame, uint32_t reg_index, uint32_t value);
