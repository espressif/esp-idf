#ifndef PANIC_H
#define PANIC_H


#define PANIC_RSN_NONE 0
#define PANIC_RSN_DEBUGEXCEPTION 1
#define PANIC_RSN_DOUBLEEXCEPTION 2
#define PANIC_RSN_KERNELEXCEPTION 3
#define PANIC_RSN_COPROCEXCEPTION 4
#define PANIC_RSN_INTWDT 5
#define PANIC_RSN_MAX 5


#ifndef __ASSEMBLER__

void setBreakpointIfJtag(void *fn);

#endif

#endif