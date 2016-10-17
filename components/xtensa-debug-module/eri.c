#include <stdint.h>
#include "eri.h"

uint32_t eri_read(int addr) {
    uint32_t ret;
    asm(
        "RER %0,%1"
        :"=r"(ret):"r"(addr)
        );
    return ret;
}

void eri_write(int addr, uint32_t data) {
    asm volatile (
        "WER %0,%1"
        ::"r"(data),"r"(addr)
    );
}

