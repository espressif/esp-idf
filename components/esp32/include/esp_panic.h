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

void esp_set_breakpoint_if_jtag(void *fn);

#endif

#endif