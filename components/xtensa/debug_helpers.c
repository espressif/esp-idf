// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_debug_helpers.h"
#include "soc/soc_memory_layout.h"
#include "soc/cpu.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#else
#include "esp32s2beta/rom/ets_sys.h"
#endif

bool IRAM_ATTR esp_backtrace_get_next_frame(esp_backtrace_frame_t *frame)
{
    //Use frame(i-1)'s BS area located below frame(i)'s sp to get frame(i-1)'s sp and frame(i-2)'s pc
    void *base_save = (void *)frame->sp;     //Base save area consists of 4 words under SP
    frame->pc = frame->next_pc;
    frame->next_pc = *((uint32_t *)(base_save - 16));     //If next_pc = 0, indicates frame(i-1) is the last frame on the stack
    frame->sp =  *((uint32_t *)(base_save - 12));

    //Return true if both sp and pc of frame(i-1) are sane, false otherwise
    return (esp_stack_ptr_is_sane(frame->sp) && esp_ptr_executable((void*)esp_cpu_process_stack_pc(frame->pc)));
}

esp_err_t IRAM_ATTR esp_backtrace_print(int depth)
{
    //Check arguments
    if (depth <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    //Initialize stk_frame with first frame of stack
    esp_backtrace_frame_t stk_frame;
    esp_backtrace_get_start(&(stk_frame.pc), &(stk_frame.sp), &(stk_frame.next_pc));
    //esp_cpu_get_backtrace_start(&stk_frame);
    ets_printf("\r\n\r\nBacktrace:");
    ets_printf("0x%08X:0x%08X ", esp_cpu_process_stack_pc(stk_frame.pc), stk_frame.sp);

    //Check if first frame is valid
    bool corrupted = (esp_stack_ptr_is_sane(stk_frame.sp) &&
                      esp_ptr_executable((void*)esp_cpu_process_stack_pc(stk_frame.pc))) ?
                      false : true;

    uint32_t i = (depth <= 0) ? INT32_MAX : depth;
    while (i-- > 0 && stk_frame.next_pc != 0 && !corrupted) {
        if (!esp_backtrace_get_next_frame(&stk_frame)) {    //Get previous stack frame
            corrupted = true;
        }
        ets_printf("0x%08X:0x%08X ", esp_cpu_process_stack_pc(stk_frame.pc), stk_frame.sp);
    }

    //Print backtrace termination marker
    esp_err_t ret = ESP_OK;
    if (corrupted) {
        ets_printf(" |<-CORRUPTED");
        ret =  ESP_FAIL;
    } else if (stk_frame.next_pc != 0) {    //Backtrace continues
        ets_printf(" |<-CONTINUES");
    }
    ets_printf("\r\n\r\n");
    return ret;
}
