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

#include "heap_alloc_caps.h"
#include "spiram.h"

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

/*
Tag descriptors. These describe the capabilities of a bit of memory that's tagged with the index into this table.
Each tag contains NO_PRIOS entries; later entries are only taken if earlier ones can't fulfill the memory request.
*/
static const uint32_t tagDesc[][NO_PRIOS]={
	{ MALLOC_CAP_DMA|MALLOC_CAP_8BIT, MALLOC_CAP_32BIT, 0 },					//Tag 0: Plain ole D-port RAM
	{ 0, MALLOC_CAP_DMA|MALLOC_CAP_8BIT, MALLOC_CAP_32BIT|MALLOC_CAP_EXEC },	//Tag 1: Plain ole D-port RAM which has an alias on the I-port
	{ MALLOC_CAP_EXEC|MALLOC_CAP_32BIT, 0, 0 },									//Tag 2: IRAM
	{ MALLOC_CAP_PID2, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT },					//Tag 3-8: PID 2-7 IRAM
	{ MALLOC_CAP_PID3, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT },					//
	{ MALLOC_CAP_PID4, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT },					//
	{ MALLOC_CAP_PID5, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT },					//
	{ MALLOC_CAP_PID6, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT },					//
	{ MALLOC_CAP_PID7, 0, MALLOC_CAP_EXEC|MALLOC_CAP_32BIT },					//
	{ MALLOC_CAP_PID2, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT },						//Tag 9-14: PID 2-7 DRAM
	{ MALLOC_CAP_PID3, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT },						//
	{ MALLOC_CAP_PID4, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT },						//
	{ MALLOC_CAP_PID5, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT },						//
	{ MALLOC_CAP_PID6, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT },						//
	{ MALLOC_CAP_PID7, MALLOC_CAP_8BIT, MALLOC_CAP_32BIT },						//
	{ MALLOC_CAP_SPISRAM, 0, MALLOC_CAP_DMA|MALLOC_CAP_8BIT|MALLOC_CAP_32BIT}, //Tag 15: SPI SRAM data
	{ MALLOC_CAP_INVALID, MALLOC_CAP_INVALID, MALLOC_CAP_INVALID } //End
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
//	{ (uint8_t *)0x3FFAE000, 0x2000, 0, 0}, //pool 16 <- can be used for BT <- THIS POOL DOESN'T WORK for some reason! Hw seems fine. ToDo: Figure out.
	{ (uint8_t *)0x3FFB0000, 0x8000, 0, 0}, //pool 15 <- can be used for BT
	{ (uint8_t *)0x3FFB8000, 0x8000, 0, 0}, //pool 14
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
			ets_printf("%s: region %d: hole punching is not supported!\n", i);
			regions[i].xTag=-1; //Just disable memory region. That'll teach them!
		}
	}
}


/*
ToDo: These are very dependent on the linker script, and the logic involving this works only
because we're not using the SPI flash yet! If we enable that, this will break. ToDo: Rewrite by then.
*/
extern int _init_start, _text_end;
extern int _bss_start, _heap_start;

/*
Initialize the heap allocator. We pass it a bunch of region descriptors, but we need to modify those first to accommodate for 
the data as loaded by the bootloader.
ToDo: The regions are different when stuff like trace memory, BT, ... is used. Modify the regions struct on the fly for this.
Same with loading of apps. Same with using SPI RAM.
*/
void heap_alloc_caps_init() {
	int i;
	//Disable the bits of memory where this code is loaded.
	disable_mem_region(&_init_start, &_text_end);
	disable_mem_region(&_bss_start, &_heap_start);
	disable_mem_region((void*)0x3ffae000, (void*)0x3ffb0000); //knock out ROM data region
	disable_mem_region((void*)0x3ffe0000, (void*)0x3ffe8000); //knock out ROM data region
	disable_mem_region((void*)0x40070000, (void*)0x40078000); //CPU0 cache region
	disable_mem_region((void*)0x40078000, (void*)0x40080000); //CPU1 cache region
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

#if 1 //Change to 1 to show the regions the heap allocator is initialized with.
	ets_printf("Initializing heap allocator:\n");
	for (i=0; regions[i].xSizeInBytes!=0; i++) {
		if ( regions[i].xTag != -1 ) ets_printf("Region %02d: %08X len %08X tag %d\n", i, (int)regions[i].pucStartAddress, regions[i].xSizeInBytes, regions[i].xTag);
	}
#endif
	//Initialize the malloc implementation.
	vPortDefineHeapRegionsTagged( regions );
}

/*
Standard malloc() implementation. Will return ho-hum byte-accessible data memory.
*/
void *pvPortMalloc( size_t xWantedSize )
{
	return pvPortMallocCaps( xWantedSize, MALLOC_CAP_8BIT );
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
	for (prio=0; prio<NO_PRIOS; prio++) {
		//Iterate over tag descriptors for this priority
		for (tag=0; tagDesc[tag][prio]!=MALLOC_CAP_INVALID; tag++) {
			if ((tagDesc[tag][prio]&caps)!=0) {
				//Tag has at least one of the caps requested. If caps has other bits set that this prio
				//doesn't cover, see if they're available in other prios.
				remCaps=caps&(~tagDesc[tag][prio]); //Remaining caps to be fulfilled
				j=prio+1;
				while (remCaps!=0 && j<NO_PRIOS) {
					remCaps=remCaps&(~tagDesc[tag][j]);
					j++;
				}
				if (remCaps==0) {
					//This tag can satisfy all the requested capabilities. See if we can grab some memory using it.
					ret=pvPortMallocTagged(xWantedSize, tag);
					if (ret!=NULL) return ret;
				}
			}
		}
	}
	//Nothing usable found.
	return NULL;
}
