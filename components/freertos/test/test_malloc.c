/*
 Generic test for malloc/free
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"

static int tryAllocMem() {
    int **mem;
    int i, noAllocated, j;
    mem=malloc(sizeof(int)*1024);
    if (!mem) return 0;
    for (i=0; i<1024; i++) {
        mem[i]=malloc(1024);
        if (mem[i]==NULL) break;
        for (j=0; j<1024/4; j++) mem[i][j]=(0xdeadbeef);
    }
    noAllocated=i;
    for (i=0; i<noAllocated; i++) {
        for (j=0; j<1024/4; j++) {
            TEST_ASSERT(mem[i][j]==(0xdeadbeef));
        }
        free(mem[i]);
    }
    free(mem);
    return noAllocated;
}


TEST_CASE("Malloc/overwrite, then free all available DRAM", "[freertos]")
{
    int m1=0, m2=0;
    m1=tryAllocMem();
    m2=tryAllocMem();
    printf("Could allocate %dK on first try, %dK on 2nd try.\n", m1, m2);
    TEST_ASSERT(m1==m2);
}

