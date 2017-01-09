#ifndef PANIC_H
#define PANIC_H


#define PANIC_RSN_NONE 0
#define PANIC_RSN_DEBUGEXCEPTION 1
#define PANIC_RSN_DOUBLEEXCEPTION 2
#define PANIC_RSN_KERNELEXCEPTION 3
#define PANIC_RSN_COPROCEXCEPTION 4
#define PANIC_RSN_INTWDT_CPU0 5
#define PANIC_RSN_INTWDT_CPU1 6
#define PANIC_RSN_MAX 6


#ifndef __ASSEMBLER__

#include "esp_err.h"

void esp_set_breakpoint_if_jtag(void *fn);

#define ESP_WATCHPOINT_LOAD 0x40000000
#define ESP_WATCHPOINT_STORE 0x80000000
#define ESP_WATCHPOINT_ACCESS 0xC0000000

esp_err_t esp_set_watchpoint(int no, void *adr, int size, int flags);
void esp_clear_watchpoint(int no);


#endif

#endif