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

#include <esp_expression_with_stack.h>
#include <freertos/xtensa_rtos.h>
#include <freertos/xtensa_context.h>

StackType_t * esp_switch_stack_setup(StackType_t *stack, size_t stack_size)
{
#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK
    int watchpoint_place = (((int)stack + 31) & ~31);
#endif    
    StackType_t *top_of_stack =  (StackType_t *)&stack[0] +                  
            ((stack_size * sizeof(StackType_t)) / sizeof(StackType_t));

    //Align stack to a 16byte boundary, as required by CPU specific:
    top_of_stack =  (StackType_t *)(((UBaseType_t)(top_of_stack - 31) -
                                    ALIGNUP(0x10, sizeof(XtSolFrame) )) & 
                                    ~0xf);

    //Fake stack frame to do not break the backtrace
    XtSolFrame *frame = (XtSolFrame *)top_of_stack;
    frame->a0 = 0;
    frame->a1 = (UBaseType_t)top_of_stack;

#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK
    esp_set_watchpoint(2, (char*)watchpoint_place, 32, ESP_WATCHPOINT_STORE);    
#endif

    return top_of_stack;           
}