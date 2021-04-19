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
#include "soc/cpu.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void init_regfile(esp_gdbstub_gdb_regfile_t *dst)
{
    memset(dst, 0, sizeof(*dst));
}

void esp_gdbstub_frame_to_regfile(const esp_gdbstub_frame_t *frame, esp_gdbstub_gdb_regfile_t *dst)
{
    init_regfile(dst);
    dst->pc = frame->mepc;

    // We omit register x0 here since it's the zero register and always hard-wired to 0.
    // See The RISC-V Instruction Set Manual Volume I: Unprivileged ISA Document Version 20191213 for more details.
    memcpy(&(dst->x[1]), &frame->ra, sizeof(uint32_t) * 31);
}

#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

/* Represents FreeRTOS TCB structure */
typedef struct {
    uint8_t *top_of_stack;
    /* Other members aren't needed */
} dummy_tcb_t;


void esp_gdbstub_tcb_to_regfile(TaskHandle_t tcb, esp_gdbstub_gdb_regfile_t *dst)
{
    const dummy_tcb_t *dummy_tcb = (const dummy_tcb_t *) tcb;

    const RvExcFrame *frame = (RvExcFrame *) dummy_tcb->top_of_stack;
    esp_gdbstub_frame_to_regfile(frame, dst);
}

#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

int esp_gdbstub_get_signal(const esp_gdbstub_frame_t *frame)
{
      return 5; // SIGTRAP, see IDF-2490
}

void _xt_gdbstub_int(void * frame)
{

}
