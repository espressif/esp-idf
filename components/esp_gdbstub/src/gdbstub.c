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

#include <string.h>
#include "esp_gdbstub.h"
#include "esp_gdbstub_common.h"
#include "sdkconfig.h"


#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
static void init_task_info(void);
static void find_paniced_task_index(void);
static int handle_task_commands(unsigned char *cmd, int len);
#endif

static void send_reason(void);


static esp_gdbstub_scratch_t s_scratch;


void esp_gdbstub_panic_handler(esp_gdbstub_frame_t *frame)
{
#ifndef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
    esp_gdbstub_frame_to_regfile(frame, &s_scratch.regfile);
#else
    if (s_scratch.state == GDBSTUB_STARTED) {
        /* We have re-entered GDB Stub. Try disabling task support. */
        s_scratch.state = GDBSTUB_TASK_SUPPORT_DISABLED;
        /* Flush any pending GDB packet (this creates a garbage value) */
        esp_gdbstub_send_end();
    } else if (s_scratch.state == GDBSTUB_NOT_STARTED) {
        s_scratch.state = GDBSTUB_STARTED;
        /* Save the paniced frame and get the list of tasks */
        memcpy(&s_scratch.paniced_frame, frame, sizeof(*frame));
        esp_gdbstub_frame_to_regfile(frame, &s_scratch.regfile);
        init_task_info();
        find_paniced_task_index();
        /* Current task is the paniced task */
        if (s_scratch.paniced_task_index == GDBSTUB_CUR_TASK_INDEX_UNKNOWN) {
            s_scratch.current_task_index = 0;
        }
    }
#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

    esp_gdbstub_target_init();
    s_scratch.signal = esp_gdbstub_get_signal(frame);
    send_reason();
    while (true) {
        unsigned char *cmd;
        size_t size;
        int res = esp_gdbstub_read_command(&cmd, &size);
        if (res > 0) {
            /* character received instead of a command */
            continue;
        }
        if (res == GDBSTUB_ST_ERR) {
            esp_gdbstub_send_str_packet("E01");
            continue;
        }
        res = esp_gdbstub_handle_command(cmd, size);
        if (res == GDBSTUB_ST_ERR) {
            esp_gdbstub_send_str_packet(NULL);
        }
    }
}


static void send_reason(void)
{
    esp_gdbstub_send_start();
    esp_gdbstub_send_char('T');
    esp_gdbstub_send_hex(s_scratch.signal, 8);
    esp_gdbstub_send_end();
}

static uint32_t gdbstub_hton(uint32_t i)
{
    return __builtin_bswap32(i);
}

/** Send all registers to gdb */
static void handle_g_command(const unsigned char* cmd, int len)
{
    uint32_t *p = (uint32_t *) &s_scratch.regfile;
    esp_gdbstub_send_start();
    for (int i = 0; i < sizeof(s_scratch.regfile) / sizeof(*p); ++i) {
        esp_gdbstub_send_hex(gdbstub_hton(*p++), 32);
    }
    esp_gdbstub_send_end();
}

/** Receive register values from gdb */
static void handle_G_command(const unsigned char* cmd, int len)
{
    uint32_t *p = (uint32_t *) &s_scratch.regfile;
    for (int i = 0; i < sizeof(s_scratch.regfile) / sizeof(*p); ++i) {
        *p++ = gdbstub_hton(esp_gdbstub_gethex(&cmd, 32));
    }
    esp_gdbstub_send_str_packet("OK");
}

/** Read memory to gdb */
static void handle_m_command(const unsigned char* cmd, int len)
{
    intptr_t addr = (intptr_t) esp_gdbstub_gethex(&cmd, -1);
    cmd++;
    uint32_t size = esp_gdbstub_gethex(&cmd, -1);

    if (esp_gdbstub_readmem(addr) < 0 || esp_gdbstub_readmem(addr + size - 1) < 0) {
        esp_gdbstub_send_str_packet("E01");
        return;
    }

    esp_gdbstub_send_start();
    for (int i = 0; i < size; ++i) {
        int b = esp_gdbstub_readmem(addr++);
        esp_gdbstub_send_hex(b, 8);
    }
    esp_gdbstub_send_end();
}

/** Handle a command received from gdb */
int esp_gdbstub_handle_command(unsigned char *cmd, int len)
{
    unsigned char *data = cmd + 1;
    if (cmd[0] == 'g') 
    {
        handle_g_command(data, len - 1);
    } else if (cmd[0] == 'G') {
        /* receive content for all registers from gdb */
        handle_G_command(data, len - 1);
    } else if (cmd[0] == 'm') {
        /* read memory to gdb */
        handle_m_command(data, len - 1);
    } else if (cmd[0] == '?') {
        /* Reply with stop reason */
        send_reason();
#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
    } else if (s_scratch.state != GDBSTUB_TASK_SUPPORT_DISABLED) {
        return handle_task_commands(cmd, len);
#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
    } else {
        /* Unrecognized command */
        return GDBSTUB_ST_ERR;
    }
    return GDBSTUB_ST_OK;
}

/* Everything below is related to the support for listing FreeRTOS tasks as threads in GDB */

#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

static void init_task_info(void)
{
    unsigned tcb_size;
    s_scratch.task_count = uxTaskGetSnapshotAll(s_scratch.tasks, GDBSTUB_TASKS_NUM, &tcb_size);
}

static bool get_task_handle(size_t index, TaskHandle_t *handle)
{
    if (index >= s_scratch.task_count) {
        return false;
    }
    *handle = (TaskHandle_t) s_scratch.tasks[index].pxTCB;
    return true;
}

/** Get the index of the task running on the current CPU, and save the result */
static void find_paniced_task_index(void)
{
    TaskHandle_t cur_handle = xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID());
    TaskHandle_t handle;
    for (int i = 0; i < s_scratch.task_count; i++) {
        if (get_task_handle(i, &handle) && cur_handle == handle) {
            s_scratch.paniced_task_index = i;
            return;
        }
    }
    s_scratch.paniced_task_index = GDBSTUB_CUR_TASK_INDEX_UNKNOWN;
}

/** H command sets the "current task" for the purpose of further commands */
static void handle_H_command(const unsigned char* cmd, int len)
{
    if (cmd[1] == 'g' || cmd[1] == 'c') {
        const char *ret = "OK";
        cmd += 2;
        int requested_task_index = esp_gdbstub_gethex(&cmd, -1);
        if (requested_task_index == s_scratch.paniced_task_index ||
                (requested_task_index == 0 && s_scratch.current_task_index == GDBSTUB_CUR_TASK_INDEX_UNKNOWN)) {
            /* Get the registers of the paniced task */
            esp_gdbstub_frame_to_regfile(&s_scratch.paniced_frame, &s_scratch.regfile);
        } else if (requested_task_index > s_scratch.task_count) {
            ret = "E00";
        } else {
            TaskHandle_t handle = NULL;
            get_task_handle(requested_task_index, &handle);
            /* FIXME: for the task currently running on the other CPU, extracting the registers from TCB
             * isn't valid. Need to use some IPC mechanism to obtain the registers of the other CPU
             */
            if (handle != NULL) {
                esp_gdbstub_tcb_to_regfile(handle, &s_scratch.regfile);
            }
        }
        esp_gdbstub_send_str_packet(ret);
    } else {
        esp_gdbstub_send_str_packet(NULL);
    }
}

/** qC returns the current thread ID */
static void handle_qC_command(const unsigned char* cmd, int len)
{
    esp_gdbstub_send_start();
    esp_gdbstub_send_str("QC");
    esp_gdbstub_send_hex(s_scratch.current_task_index, 32);
    esp_gdbstub_send_end();
}

/** T command checks if the task is alive.
 *  Since GDB isn't going to ask about the tasks which haven't been listed by q*ThreadInfo,
 *  and the state of tasks can not change (no stepping allowed), simply return "OK" here.
 */
static void handle_T_command(const unsigned char* cmd, int len)
{
    esp_gdbstub_send_str_packet("OK");
}

/** qfThreadInfo requests the start of the thread list, qsThreadInfo (below) is repeated to
 *  get the subsequent threads.
 */
static void handle_qfThreadInfo_command(const unsigned char* cmd, int len)
{
    /* The first task in qfThreadInfo reply is going to be the one which GDB will request to stop.
     * Therefore it has to be the paniced task.
     * Reply with the paniced task index, and later skip over this index while handling qsThreadInfo
     */
    esp_gdbstub_send_start();
    esp_gdbstub_send_str("m");
    esp_gdbstub_send_hex(s_scratch.paniced_task_index, 32);
    esp_gdbstub_send_end();

    s_scratch.thread_info_index = 0;
}

static void handle_qsThreadInfo_command(const unsigned char* cmd, int len)
{
    int next_task_index = ++s_scratch.thread_info_index;

    if (next_task_index == s_scratch.task_count) {
        /* No more tasks */
        esp_gdbstub_send_str_packet("l");
        return;
    }

    if (next_task_index == s_scratch.paniced_task_index) {
        /* Have already sent this one in the reply to qfThreadInfo, skip over it */
        handle_qsThreadInfo_command(cmd, len);
        return;
    }

    esp_gdbstub_send_start();
    esp_gdbstub_send_str("m");
    esp_gdbstub_send_hex(next_task_index, 32);
    esp_gdbstub_send_end();
}

/** qThreadExtraInfo requests the thread name */
static void handle_qThreadExtraInfo_command(const unsigned char* cmd, int len)
{
    cmd += sizeof("qThreadExtraInfo,") - 1;
    int task_index = esp_gdbstub_gethex(&cmd, -1);
    TaskHandle_t handle;
    if (!get_task_handle(task_index, &handle)) {
        esp_gdbstub_send_str_packet("E01");
        return;
    }
    esp_gdbstub_send_start();
    const char* task_name = pcTaskGetTaskName(handle);
    while (*task_name) {
        esp_gdbstub_send_hex(*task_name, 8);
        task_name++;
    }
    /** TODO: add "Running" or "Suspended" and "CPU0" or "CPU1" */
    esp_gdbstub_send_end();
}

bool command_name_matches(const char* pattern, const unsigned char* ucmd, int len)
{
    const char* cmd = (const char*) ucmd;
    const char* end = cmd + len;
    for (; *pattern && cmd < end; ++cmd, ++pattern) {
        if (*pattern == '?') {
            continue;
        }
        if (*pattern != *cmd) {
            return false;
        }
    }
    return *pattern == 0 && (cmd == end || *cmd == ',');
}

/** Handle all the thread-related commands */
static int handle_task_commands(unsigned char *cmd, int len)
{
    if (cmd[0] == 'H') {
        /* Continue with task */
        handle_H_command(cmd, len);
    } else if (cmd[0] == 'T') {
        /* Task alive check */
        handle_T_command(cmd, len);
    } else if (cmd[0] == 'q') {
        if (command_name_matches("qfThreadInfo", cmd, len)) {
            handle_qfThreadInfo_command(cmd, len);
        } else if (command_name_matches("qsThreadInfo", cmd, len)) {
            handle_qsThreadInfo_command(cmd, len);
        } else if (command_name_matches("qC", cmd, len)) {
            handle_qC_command(cmd, len);
        } else if (command_name_matches("qThreadExtraInfo", cmd, len)) {
            handle_qThreadExtraInfo_command(cmd, len);
        } else {
            /* Unrecognized command */
            return GDBSTUB_ST_ERR;
        }
    } else {
        /* Unrecognized command */
        return GDBSTUB_ST_ERR;
    }
    return GDBSTUB_ST_OK;
}

#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

