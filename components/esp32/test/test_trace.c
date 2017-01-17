#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "unity.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "eri.h"
#include "trax.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"
#include "freertos/task.h"


// TODO: move these (and same definitions in trax.c to dport_reg.h)
#define TRACEMEM_MUX_PROBLK0_APPBLK1    0
#define TRACEMEM_MUX_BLK0_ONLY          1
#define TRACEMEM_MUX_BLK1_ONLY          2
#define TRACEMEM_MUX_PROBLK1_APPBLK0    3

static uint8_t* s_tracemem_blocks[] = {
    (uint8_t*) 0x3FFFC000,
    (uint8_t*) 0x3FFF8000
};

static const size_t TRACEMEM_BLOCK_SIZE = 0x4000;

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
    // Configure block 1 as trace memory, enable access via both CPUs
    WRITE_PERI_REG(DPORT_PRO_TRACEMEM_ENA_REG, DPORT_PRO_TRACEMEM_ENA_M);
    WRITE_PERI_REG(DPORT_APP_TRACEMEM_ENA_REG, DPORT_APP_TRACEMEM_ENA_M);
    WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, TRACEMEM_MUX_BLK1_ONLY);

    // Stop trace, if any (on the current CPU)
    eri_write(ERI_TRAX_TRAXCTRL, eri_read(ERI_TRAX_TRAXCTRL) | TRAXCTRL_TRSTP);
    eri_write(ERI_TRAX_TRAXCTRL, TRAXCTRL_TMEN);
    // TODO: make sure trace is not running on the other CPU

    // fill two halves of the first trace mem block
    fill_tracemem_arg_t arg1 = {
        .block = 0,
        .done = xSemaphoreCreateBinary()
    };

    fill_tracemem_arg_t arg2 = {
        .block = 0,
        .done = xSemaphoreCreateBinary()
    };
    xTaskCreatePinnedToCore(&fill_tracemem, "fill1", 2048, &arg1, 3, NULL, 0);
    xTaskCreatePinnedToCore(&fill_tracemem, "fill2", 2048, &arg2, 3, NULL, 1);
    xSemaphoreTake(arg1.done, 1);
    xSemaphoreTake(arg2.done, 1);
    vSemaphoreDelete(arg1.done);
    vSemaphoreDelete(arg2.done);

    // Block 0 is filled with data — configure it as trace memory so that it is accessible via TRAX module
    WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, TRACEMEM_MUX_BLK0_ONLY);
    // Block 1 can now be filled with data
}

