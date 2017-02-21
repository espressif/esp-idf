/*
This code triggers a psram-related silicon bug in rev0 silicon. The bug is fixed in rev1 silicon.
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
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rom/ets_sys.h"
#include "esp_heap_alloc_caps.h"


typedef struct {
    char name[9];
    int dummy[4];
    int *wadfile;
    int dummy2[2];
} xlumpinfo_t;


static xlumpinfo_t *xlumpinfo=(xlumpinfo_t*)0x3f80002c;
static int xnumlumps=5500;

static int isValidPtr(void *ptr) {
    if (((int)ptr&0xff000000)==0x3f000000) return 1;
    return 0;
}


int test_weird_corruption() {
    int err=0;
    xlumpinfo_t *marked = (xlumpinfo_t*)0x3fff2824;

    size_t i, num_marked = 0, num_unmarked = 0;
//  int is_marked = 0, mark_end = 0;
    xlumpinfo_t *lump = xlumpinfo;
    int x;

    for (x=0; x<xnumlumps; x++) {
        if (xlumpinfo[x].wadfile && !isValidPtr(xlumpinfo[x].wadfile)) {
            ets_printf("Lump wad before error!");
        }
    }

    for (i=xnumlumps; i--; lump++) {
        xlumpinfo[num_unmarked]=*lump;       // else move down THIS list
        num_unmarked++;
    }


    // Append marked list to end of unmarked list
    memcpy(&xlumpinfo[num_unmarked], marked, num_marked * sizeof(*marked));

//  free(marked);                                   // free marked list


    xnumlumps = num_unmarked + num_marked;           // new total number of lumps

    for (x=0; x<xnumlumps; x++) {
        if (xlumpinfo[x].wadfile && !isValidPtr(xlumpinfo[x].wadfile)) {
            ets_printf("Lump wad error for %s at addr %p! Index=%d unmarked=%d marked=%d Should be %p is %p\n", 
                xlumpinfo[x].name, &xlumpinfo[x].wadfile, x, num_unmarked, num_marked, xlumpinfo[x+1].wadfile,xlumpinfo[x].wadfile);
            int j;
            for (j=0; j<10; j++) {
                ets_printf("%08x ", ((uint32_t*)&xlumpinfo[x])[j]);
            }
            ets_printf("\n");

            for (j=0; j<10; j++) {
                ets_printf("%08x ", ((uint32_t*)&xlumpinfo[x-1])[j]);
            }
            ets_printf("\n");
            err++;
        }
    }
    return err;
}


TEST_CASE("PSram bug in rev0 silicon (Doom bug)", "[psram][ignore]")
{
    int i, r=0;
    xlumpinfo_t *p;
#if CONFIG_MEMMAP_SMP
    printf("WARNING\n");
    printf("WARNING - This bug sometimes doesn't show up unless MEMMAP_SMP is disabled. You have it enabled now, so the test may erroneously pass.\n");
    printf("WARNING\n");
#endif
#if !CONFIG_MEMMAP_SPIRAM_ENABLE
    printf("WARNING\n");
    printf("WARNING - This test needs psram enabled. You don't seem to have that; the test is likely to crash.\n");
    printf("WARNING\n");
#endif
    printf("Stack ptr %p\n", &i);
    p=pvPortMallocCaps(sizeof(xlumpinfo_t)*5600, MALLOC_CAP_SPIRAM);
    xlumpinfo=p;
    //Bug seems to only trigger when address ends in 2C
    while (((int)xlumpinfo&0xff)!=0x2c) xlumpinfo=(xlumpinfo_t*)(((char*)xlumpinfo)+1);
    printf("lumpinfo ptr %p\n", xlumpinfo);

    for (i=0; i<xnumlumps; i++) {
        sprintf(xlumpinfo[i].name, "DIGD");
        xlumpinfo[i].wadfile=(int*)0x3ffef664;
    }
    ets_printf("Running corruption test...\n");
    for (i=0; i<20; i++) {
        r+=test_weird_corruption();
    }
    free(p);
    TEST_ASSERT(r==0);
}

