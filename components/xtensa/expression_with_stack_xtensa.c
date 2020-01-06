#include <esp_expression_with_stack.h>
#include <freertos/xtensa_rtos.h>
#include <freertos/xtensa_context.h>

StackType_t * esp_switch_stack_setup(StackType_t *stack, size_t stack_size)
{
    int watchpoint_place = (((int)stack + 31) & ~31);
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

    esp_set_watchpoint(2, (char*)watchpoint_place, 32, ESP_WATCHPOINT_STORE);    
    return top_of_stack;           
}