#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "unity.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "eri.h"
#include "trax.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/**
 * Tests for sending trace over JTAG
 *
 * block 1 as trace memory, block 0 as normal memory
 * CPU 0 and 1 write to BLK0 directly
 * when watermark is triggered (figure out how):
 *      disable trace for block 1
 *      set internal pointer to use block 1
 *      switch tracemem mux to block 0
 *
 */

#define TRACEMEM_MUX_PROBLK0_APPBLK1    0
#define TRACEMEM_MUX_BLK0_ONLY          1
#define TRACEMEM_MUX_BLK1_ONLY          2
#define TRACEMEM_MUX_PROBLK1_APPBLK0    3


static uint8_t* s_tracemem_blocks[] = {
    (uint8_t*) 0x3FFF8000,
    (uint8_t*) 0x3FFCC000
};

static const size_t TRACEMEM_BLOCK_SIZE = 0x4000;

//static void trace_enable()
//{
//    if (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP) {
//        WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, (which == TRAX_ENA_PRO_APP_SWAP)?TRACEMEM_MUX_PROBLK1_APPBLK0:TRACEMEM_MUX_PROBLK0_APPBLK1);
//    } else {
//        WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, TRACEMEM_MUX_BLK0_ONLY);
//    }
//    WRITE_PERI_REG(DPORT_PRO_TRACEMEM_ENA_REG, (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP || which == TRAX_ENA_PRO));
//    WRITE_PERI_REG(DPORT_APP_TRACEMEM_ENA_REG, (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP || which == TRAX_ENA_APP));
//
//}

typedef struct {
    int block;
    SemaphoreHandle_t done;
} fill_tracemem_arg_t;

static void fill_tracemem(void* p)
{
    fill_tracemem_arg_t* arg = (fill_tracemem_arg_t*) p;
    int coreId = xPortGetCoreID();
    memset(s_tracemem_blocks[arg->block] + coreId * TRACEMEM_BLOCK_SIZE / 2,
            (coreId) ? 0xba:0xab, TRACEMEM_BLOCK_SIZE / 2);
    xSemaphoreGive(arg->done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

TEST_CASE("both CPUs can write to trace block 0", "[trace][ignore]")
{
    WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, TRACEMEM_MUX_BLK1_ONLY);

    fill_tracemem_arg_t arg1 = {
        .block = 0,
        .done = xSemaphoreCreateBinary()
    };

    fill_tracemem_arg_t arg2 = {
        .block = 0,
        .done = xSemaphoreCreateBinary()
    };

    xTaskCreatePinnedToCore(&fill_tracemem, "fill1", 2048, &arg1, 3, NULL, 0);
    xTaskCreatePinnedToCore(&fill_tracemem, "fill2", 2048, &arg2, 3, NULL, 0);

    xSemaphoreTake(arg1.done, 1);
    xSemaphoreTake(arg2.done, 1);

    WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, TRACEMEM_MUX_BLK0_ONLY);
}



