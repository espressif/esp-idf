// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <rom/ets_sys.h>

#include <freertos/heap_regions.h>

#include "esp_heap_alloc_caps.h"
#include "spiram.h"
#include "esp_log.h"
#include <stdbool.h>

static const char* TAG = "heap_alloc_caps";

/*
This file, combined with a region allocator that supports tags, solves the problem that the ESP32 has RAM that's 
slightly heterogeneous. Some RAM can be byte-accessed, some allows only 32-bit accesses, some can execute memory,
some can be remapped by the MMU to only be accessed by a certain PID etc. In order to allow the most flexible
memory allocation possible, this code makes it possible to request memory that has certain capabilities. The
code will then use its knowledge of how the memory is configured along with a priority scheme to allocate that
memory in the most sane way possible. This should optimize the amount of RAM accessible to the code without
hardwiring addresses.
*/


//Amount of priority slots for the tag descriptors.
#define NO_PRIOS 3


typedef struct {
    const char *name;
    uint32_t prio[NO_PRIOS];
    bool aliasedIram;
} tag_desc_t;

/*
Tag descriptors. These describe the capabilities of a bit of memory that's tagged with the index into this table.
Each tag contains NO_PRIOS entries; later entries are only taken if earlier ones can't fulfill the memory request.
Make sure there are never more than HEAPREGIONS_MAX_TAGCOUNT (in heap_regions.h) tags (ex the last empty marker)

WARNING: The current code assumes the ROM stacks are located in tag 1; no allocation from this tag can be done until
the FreeRTOS scheduler has started.
*/
static const tag_desc_t tag_desc[]={
    { "DRAM", { MALLOC_CAP_DMA|MALLOC_CAP_8BIT, MALLOC_CAP_32BIT, 0 }, false},                        //Tag 0: Plain ole D-port RAM
    { "D/IRAM", { 0, MALLOC_CAP_DMA|MALLOC_CAP_8BIT, MALLOC_CAP_32BIT|MALLOC_CAP_EXEC }, true},       //Tag 1: Plain ole D-port RAM which has an alias on the I-port
    { "IRAM", { MALLOC_CAP_EXEC|MALLOC_CAP_32BIT, 0, 0 }, false},                                     //Tag 2: IRAM
    { "PID2IRAM", { MALLOC_CAP_PID2, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT }, false},                   //Tag 3-8: PID 2-7 IRAM
    { "PID3IRAM", { MALLOC_CAP_PID3, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT }, false},                   //
    { "PID4IRAM", { MALLOC_CAP_PID4, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT }, false},                   //
    { "PID5IRAM", { MALLOC_CAP_PID5, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT }, false},                   //
    { "PID6IRAM", { MALLOC_CAP_PID6, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT }, false},                   //
    { "PID7IRAM", { MALLOC_CAP_PID7, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT }, false},                   //
    { "PID2DRAM", { MALLOC_CAP_PID2, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT }, false},                     //Tag 9-14: PID 2-7 DRAM
    { "PID3DRAM", { MALLOC_CAP_PID3, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT }, false},                     //
    { "PID4DRAM", { MALLOC_CAP_PID4, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT }, false},                     //
    { "PID5DRAM", { MALLOC_CAP_PID5, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT }, false},                     //
    { "PID6DRAM", { MALLOC_CAP_PID6, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT }, false},                     //
    { "PID7DRAM", { MALLOC_CAP_PID7, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT }, false},                     //
    { "SPISRAM", { MALLOC_CAP_SPISRAM, 0, MALLOC_CAP_DMA|MALLOC_CAP_8BIT|MALLOC_CAP_32BIT}, false},   //Tag 15: SPI SRAM data
    { "", { MALLOC_CAP_INVALID, MALLOC_CAP_INVALID, MALLOC_CAP_INVALID }, false} //End
};

/*
Region descriptors. These describe all regions of memory available, and tag them according to the
capabilities the hardware has. This array is not marked constant; the initialization code may want to
change the tags of some regions because eg BT is detected, applications are loaded etc.

The priorities here roughly work like this:
- For a normal malloc (MALLOC_CAP_8BIT), give away the DRAM-only memory first, then pass off any dual-use IRAM regions,
  finally eat into the application memory.
- For a malloc where 32-bit-aligned-only access is okay, first allocate IRAM, then DRAM, finally application IRAM.
- Application mallocs (PIDx) will allocate IRAM first, if possible, then DRAM.
- Most other malloc caps only fit in one region anyway.

These region descriptors are very ESP32 specific, because they describe the memory pools available there.

Because of requirements in the coalescing code as well as the heap allocator itself, this list should always
be sorted from low to high start address.

This array is *NOT* const because it gets modified depending on what pools are/aren't available.
*/
static HeapRegionTagged_t regions[]={
    { (uint8_t *)0x3F800000, 0x20000, 15, 0}, //SPI SRAM, if available
    { (uint8_t *)0x3FFAE000, 0x2000, 0, 0}, //pool 16 <- used for rom code
    { (uint8_t *)0x3FFB0000, 0x8000, 0, 0}, //pool 15 <- if BT is enabled, used as BT HW shared memory
    { (uint8_t *)0x3FFB8000, 0x8000, 0, 0}, //pool 14 <- if BT is enabled, used data memory for BT ROM functions.
    { (uint8_t *)0x3FFC0000, 0x2000, 0, 0}, //pool 10-13, mmu page 0
    { (uint8_t *)0x3FFC2000, 0x2000, 0, 0}, //pool 10-13, mmu page 1
    { (uint8_t *)0x3FFC4000, 0x2000, 0, 0}, //pool 10-13, mmu page 2
    { (uint8_t *)0x3FFC6000, 0x2000, 0, 0}, //pool 10-13, mmu page 3
    { (uint8_t *)0x3FFC8000, 0x2000, 0, 0}, //pool 10-13, mmu page 4
    { (uint8_t *)0x3FFCA000, 0x2000, 0, 0}, //pool 10-13, mmu page 5
    { (uint8_t *)0x3FFCC000, 0x2000, 0, 0}, //pool 10-13, mmu page 6
    { (uint8_t *)0x3FFCE000, 0x2000, 0, 0}, //pool 10-13, mmu page 7
    { (uint8_t *)0x3FFD0000, 0x2000, 0, 0}, //pool 10-13, mmu page 8
    { (uint8_t *)0x3FFD2000, 0x2000, 0, 0}, //pool 10-13, mmu page 9
    { (uint8_t *)0x3FFD4000, 0x2000, 0, 0}, //pool 10-13, mmu page 10
    { (uint8_t *)0x3FFD6000, 0x2000, 0, 0}, //pool 10-13, mmu page 11
    { (uint8_t *)0x3FFD8000, 0x2000, 0, 0}, //pool 10-13, mmu page 12
    { (uint8_t *)0x3FFDA000, 0x2000, 0, 0}, //pool 10-13, mmu page 13
    { (uint8_t *)0x3FFDC000, 0x2000, 0, 0}, //pool 10-13, mmu page 14
    { (uint8_t *)0x3FFDE000, 0x2000, 0, 0}, //pool 10-13, mmu page 15
    { (uint8_t *)0x3FFE0000, 0x4000, 1, 0x400BC000}, //pool 9 blk 1
    { (uint8_t *)0x3FFE4000, 0x4000, 1, 0x400B8000}, //pool 9 blk 0
    { (uint8_t *)0x3FFE8000, 0x8000, 1, 0x400B0000}, //pool 8 <- can be remapped to ROM, used for MAC dump
    { (uint8_t *)0x3FFF0000, 0x8000, 1, 0x400A8000}, //pool 7 <- can be used for MAC dump
    { (uint8_t *)0x3FFF8000, 0x4000, 1, 0x400A4000}, //pool 6 blk 1 <- can be used as trace memory
    { (uint8_t *)0x3FFFC000, 0x4000, 1, 0x400A0000}, //pool 6 blk 0 <- can be used as trace memory
    { (uint8_t *)0x40070000, 0x8000, 2, 0}, //pool 0
    { (uint8_t *)0x40078000, 0x8000, 2, 0}, //pool 1
    { (uint8_t *)0x40080000, 0x2000, 2, 0}, //pool 2-5, mmu page 0
    { (uint8_t *)0x40082000, 0x2000, 2, 0}, //pool 2-5, mmu page 1
    { (uint8_t *)0x40084000, 0x2000, 2, 0}, //pool 2-5, mmu page 2
    { (uint8_t *)0x40086000, 0x2000, 2, 0}, //pool 2-5, mmu page 3
    { (uint8_t *)0x40088000, 0x2000, 2, 0}, //pool 2-5, mmu page 4
    { (uint8_t *)0x4008A000, 0x2000, 2, 0}, //pool 2-5, mmu page 5
    { (uint8_t *)0x4008C000, 0x2000, 2, 0}, //pool 2-5, mmu page 6
    { (uint8_t *)0x4008E000, 0x2000, 2, 0}, //pool 2-5, mmu page 7
    { (uint8_t *)0x40090000, 0x2000, 2, 0}, //pool 2-5, mmu page 8
    { (uint8_t *)0x40092000, 0x2000, 2, 0}, //pool 2-5, mmu page 9
    { (uint8_t *)0x40094000, 0x2000, 2, 0}, //pool 2-5, mmu page 10
    { (uint8_t *)0x40096000, 0x2000, 2, 0}, //pool 2-5, mmu page 11
    { (uint8_t *)0x40098000, 0x2000, 2, 0}, //pool 2-5, mmu page 12
    { (uint8_t *)0x4009A000, 0x2000, 2, 0}, //pool 2-5, mmu page 13
    { (uint8_t *)0x4009C000, 0x2000, 2, 0}, //pool 2-5, mmu page 14
    { (uint8_t *)0x4009E000, 0x2000, 2, 0}, //pool 2-5, mmu page 15
    { NULL, 0, 0, 0} //end
};

/* For the startup code, the stacks live in memory tagged by this tag. Hence, we only enable allocating from this tag 
   once FreeRTOS has started up completely. */
#define NONOS_STACK_TAG 1

static bool nonos_stack_in_use=true;

void heap_alloc_enable_nonos_stack_tag()
{
    nonos_stack_in_use=false;
}

//Modify regions array to disable the given range of memory.
static void disable_mem_region(void *from, void *to) {
    int i;
    //Align from and to on word boundaries
    from=(void*)((uint32_t)from&~3);
    to=(void*)(((uint32_t)to+3)&~3);
    for (i=0; regions[i].xSizeInBytes!=0; i++) {
        void *regStart=regions[i].pucStartAddress;
        void *regEnd=regions[i].pucStartAddress+regions[i].xSizeInBytes;
        if (regStart>=from && regEnd<=to) {
            //Entire region falls in the range. Disable entirely.
            regions[i].xTag=-1;
        } else if (regStart>=from && regEnd>to && regStart<to) {
            //Start of the region falls in the range. Modify address/len.
            int overlap=(uint8_t *)to-(uint8_t *)regStart;
            regions[i].pucStartAddress+=overlap;
            regions[i].xSizeInBytes-=overlap;
            if (regions[i].xExecAddr) regions[i].xExecAddr+=overlap;
        } else if (regStart<from && regEnd>from && regEnd<=to) {
            //End of the region falls in the range. Modify length.
            regions[i].xSizeInBytes-=(uint8_t *)regEnd-(uint8_t *)from;
        } else if (regStart<from && regEnd>to) {
            //Range punches a hole in the region! We do not support this.
            ESP_EARLY_LOGE(TAG, "region %d: hole punching is not supported!", i);
            regions[i].xTag=-1; //Just disable memory region. That'll teach them!
        }
    }
}


/*
Warning: These variables are assumed to have the start and end of the data and iram
area used statically by the program, respectively. These variables are defined in the ld
file.
*/
extern int _data_start, _heap_start, _init_start, _iram_text_end;

/*
Initialize the heap allocator. We pass it a bunch of region descriptors, but we need to modify those first to accommodate for 
the data as loaded by the bootloader.
ToDo: The regions are different when stuff like trace memory, BT, ... is used. Modify the regions struct on the fly for this.
Same with loading of apps. Same with using SPI RAM.
*/
void heap_alloc_caps_init() {
    int i;
    //Compile-time assert to see if we don't have more tags than is set in heap_regions.h
    _Static_assert((sizeof(tag_desc)/sizeof(tag_desc[0]))-1 <= HEAPREGIONS_MAX_TAGCOUNT, "More than HEAPREGIONS_MAX_TAGCOUNT tags defined!");
    //Disable the bits of memory where this code is loaded.
    disable_mem_region(&_data_start, &_heap_start);           //DRAM used by bss/data static variables
    disable_mem_region(&_init_start, &_iram_text_end);        //IRAM used by code
    disable_mem_region((void*)0x40070000, (void*)0x40078000); //CPU0 cache region
    disable_mem_region((void*)0x40078000, (void*)0x40080000); //CPU1 cache region

    /* Warning: The ROM stack is located in the 0x3ffe0000 area. We do not specifically disable that area here because
       after the scheduler has started, the ROM stack is not used anymore by anything. We handle it instead by not allowing
       any mallocs from tag 1 (the IRAM/DRAM region) until the scheduler has started.

       The 0x3ffe0000 region also contains static RAM for various ROM functions. The following lines
       reserve the regions for UART and ETSC, so these functions are usable. Libraries like xtos, which are
       not usable in FreeRTOS anyway, are commented out in the linker script so they cannot be used; we
       do not disable their memory regions here and they will be used as general purpose heap memory.

       Enabling the heap allocator for this region but disabling allocation here until FreeRTOS is started up
       is a somewhat risky action in theory, because on initializing the allocator, vPortDefineHeapRegionsTagged
        will go and write linked list entries at the start and end of all regions. For the ESP32, these linked 
       list entries happen to end up in a region that is not touched by the stack; they can be placed safely there.*/
    disable_mem_region((void*)0x3ffe0000, (void*)0x3ffe0440); //Reserve ROM PRO data region
    disable_mem_region((void*)0x3ffe4000, (void*)0x3ffe4350); //Reserve ROM APP data region

#if CONFIG_BT_ENABLED
#if CONFIG_BT_DRAM_RELEASE
    disable_mem_region((void*)0x3ffb0000, (void*)0x3ffb3000); //Reserve BT data region
    disable_mem_region((void*)0x3ffb8000, (void*)0x3ffbbb28); //Reserve BT data region
    disable_mem_region((void*)0x3ffbdb28, (void*)0x3ffc0000); //Reserve BT data region
#else
    disable_mem_region((void*)0x3ffb0000, (void*)0x3ffc0000); //Reserve BT hardware shared memory & BT data region
#endif
    disable_mem_region((void*)0x3ffae000, (void*)0x3ffaff10); //Reserve ROM data region, inc region needed for BT ROM routines
#else
    disable_mem_region((void*)0x3ffae000, (void*)0x3ffae2a0); //Reserve ROM data region
#endif

#if CONFIG_MEMMAP_TRACEMEM
#if CONFIG_MEMMAP_TRACEMEM_TWOBANKS
    disable_mem_region((void*)0x3fff8000, (void*)0x40000000); //Reserve trace mem region
#else
    disable_mem_region((void*)0x3fff8000, (void*)0x3fffc000); //Reserve trace mem region
#endif
#endif

#if 0
    enable_spi_sram();
#else
    disable_mem_region((void*)0x3f800000, (void*)0x3f820000); //SPI SRAM not installed
#endif

    //The heap allocator will treat every region given to it as separate. In order to get bigger ranges of contiguous memory,
    //it's useful to coalesce adjacent regions that have the same tag.

    for (i=1; regions[i].xSizeInBytes!=0; i++) {
        if (regions[i].pucStartAddress == (regions[i-1].pucStartAddress + regions[i-1].xSizeInBytes) &&
                                    regions[i].xTag == regions[i-1].xTag ) {
            regions[i-1].xTag=-1;
            regions[i].pucStartAddress=regions[i-1].pucStartAddress;
            regions[i].xSizeInBytes+=regions[i-1].xSizeInBytes;
        }
    }

    ESP_EARLY_LOGI(TAG, "Initializing. RAM available for dynamic allocation:");
    for (i=0; regions[i].xSizeInBytes!=0; i++) {
        if (regions[i].xTag != -1) {
            ESP_EARLY_LOGI(TAG, "At %08X len %08X (%d KiB): %s", 
                    (int)regions[i].pucStartAddress, regions[i].xSizeInBytes, regions[i].xSizeInBytes/1024, tag_desc[regions[i].xTag].name);
        }
    }
    //Initialize the malloc implementation.
    vPortDefineHeapRegionsTagged( regions );
}

//First and last words of the D/IRAM region, for both the DRAM address as well as the IRAM alias.
#define DIRAM_IRAM_START 0x400A0000
#define DIRAM_IRAM_END   0x400BFFFC
#define DIRAM_DRAM_START 0x3FFE0000
#define DIRAM_DRAM_END   0x3FFFFFFC

/*
  This takes a memory chunk in a region that can be addressed as both DRAM as well as IRAM. It will convert it to
  IRAM in such a way that it can be later freed. It assumes both the address as wel as the length to be word-aligned.
  It returns a region that's 1 word smaller than the region given because it stores the original Dram address there.
  
  In theory, we can also make this work by prepending a struct that looks similar to the block link struct used by the
  heap allocator itself, which will allow inspection tools relying on any block returned from any sort of malloc to
  have such a block in front of it, work. We may do this later, if/when there is demand for it. For now, a simple
  pointer is used.
*/
static void *dram_alloc_to_iram_addr(void *addr, size_t len) 
{
    uint32_t dstart=(int)addr; //First word
    uint32_t dend=((int)addr)+len-4; //Last word
    configASSERT(dstart>=DIRAM_DRAM_START);
    configASSERT(dend<=DIRAM_DRAM_END);
    configASSERT((dstart&3)==0);
    configASSERT((dend&3)==0);
    uint32_t istart=DIRAM_IRAM_START+(DIRAM_DRAM_END-dend);
    uint32_t *iptr=(uint32_t*)istart;
    *iptr=dstart;
    return (void*)(iptr+1);
}

/*
Standard malloc() implementation. Will return standard no-frills byte-accessible data memory.
*/
void *pvPortMalloc( size_t xWantedSize )
{
    return pvPortMallocCaps( xWantedSize, MALLOC_CAP_8BIT );
}

/*
 Standard free() implementation. Will pass memory on to the allocator unless it's an IRAM address where the
 actual meory is allocated in DRAM, it will convert to the DRAM address then.
 */
void vPortFree( void *pv )
{
    if (((int)pv>=DIRAM_IRAM_START) && ((int)pv<=DIRAM_IRAM_END)) {
        //Memory allocated here is actually allocated in the DRAM alias region and
        //cannot be de-allocated as usual. dram_alloc_to_iram_addr stores a pointer to
        //the equivalent DRAM address, though; free that.
        uint32_t* dramAddrPtr=(uint32_t*)pv;
        return vPortFreeTagged((void*)dramAddrPtr[-1]);
    }

    return vPortFreeTagged(pv);
}

/*
Routine to allocate a bit of memory with certain capabilities. caps is a bitfield of MALLOC_CAP_* bits.
*/
void *pvPortMallocCaps( size_t xWantedSize, uint32_t caps ) 
{
    int prio;
    int tag, j;
    void *ret=NULL;
    uint32_t remCaps;
    if (caps & MALLOC_CAP_EXEC) {
        //MALLOC_CAP_EXEC forces an alloc from IRAM. There is a region which has both this
        //as well as the following caps, but the following caps are not possible for IRAM.
        //Thus, the combination is impossible and we return NULL directly, even although our tag_desc
        //table would indicate there is a tag for this.
        if ((caps & MALLOC_CAP_8BIT) || (caps & MALLOC_CAP_DMA)) {
            return NULL;
        }
        //If any, EXEC memory should be 32-bit aligned, so round up to the next multiple of 4.
        xWantedSize=(xWantedSize+3)&(~3);
    }
    for (prio=0; prio<NO_PRIOS; prio++) {
        //Iterate over tag descriptors for this priority
        for (tag=0; tag_desc[tag].prio[prio]!=MALLOC_CAP_INVALID; tag++) {
            if (nonos_stack_in_use && tag == NONOS_STACK_TAG) {
                //Non-os stack lives here and is still in use. Don't alloc here.
                continue;
            }
            if ((tag_desc[tag].prio[prio]&caps)!=0) {
                //Tag has at least one of the caps requested. If caps has other bits set that this prio
                //doesn't cover, see if they're available in other prios.
                remCaps=caps&(~tag_desc[tag].prio[prio]); //Remaining caps to be fulfilled
                j=prio+1;
                while (remCaps!=0 && j<NO_PRIOS) {
                    remCaps=remCaps&(~tag_desc[tag].prio[j]);
                    j++;
                }
                if (remCaps==0) {
                    //This tag can satisfy all the requested capabilities. See if we can grab some memory using it.
                    if ((caps & MALLOC_CAP_EXEC) && tag_desc[tag].aliasedIram) {
                        //This is special, insofar that what we're going to get back is probably a DRAM address. If so,
                        //we need to 'invert' it (lowest address in DRAM == highest address in IRAM and vice-versa) and
                        //add a pointer to the DRAM equivalent before the address we're going to return.
                        ret=pvPortMallocTagged(xWantedSize+4, tag);
                        if (ret!=NULL) return dram_alloc_to_iram_addr(ret, xWantedSize+4);
                    } else {
                        //Just try to alloc, nothing special.
                        ret=pvPortMallocTagged(xWantedSize, tag);
                        if (ret!=NULL) return ret;
                    }
                }
            }
        }
    }
    //Nothing usable found.
    return NULL;
}


size_t xPortGetFreeHeapSizeCaps( uint32_t caps )
{
    int prio;
    int tag;
    size_t ret=0;
    for (prio=0; prio<NO_PRIOS; prio++) {
        //Iterate over tag descriptors for this priority
        for (tag=0; tag_desc[tag].prio[prio]!=MALLOC_CAP_INVALID; tag++) {
            if ((tag_desc[tag].prio[prio]&caps)!=0) {
                ret+=xPortGetFreeHeapSizeTagged(tag);
            }
        }
    }
    return ret;
}

size_t xPortGetMinimumEverFreeHeapSizeCaps( uint32_t caps )
{
    int prio;
    int tag;
    size_t ret=0;
    for (prio=0; prio<NO_PRIOS; prio++) {
        //Iterate over tag descriptors for this priority
        for (tag=0; tag_desc[tag].prio[prio]!=MALLOC_CAP_INVALID; tag++) {
            if ((tag_desc[tag].prio[prio]&caps)!=0) {
                ret+=xPortGetMinimumEverFreeHeapSizeTagged(tag);
            }
        }
    }
    return ret;
}

size_t xPortGetFreeHeapSize( void )
{
    return xPortGetFreeHeapSizeCaps( MALLOC_CAP_8BIT );
}

size_t xPortGetMinimumEverFreeHeapSize( void )
{
    return xPortGetMinimumEverFreeHeapSizeCaps( MALLOC_CAP_8BIT );
}


