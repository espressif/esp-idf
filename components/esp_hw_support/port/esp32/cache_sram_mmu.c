/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "esp_attr.h"
#include "string.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/cache_utils.h"
#include "spi_flash_mmap.h"

//Errors that can be returned
#define MMU_SET_ADDR_ALIGNED_ERROR 1
#define MMU_SET_PAGE_SIZE_ERROR 3
#define MMU_SET_VADDR_OUT_RANGE 5


#define PROCACHE_MMU_ADDR_BASE 0x3FF10000
#define APPCACHE_MMU_ADDR_BASE 0x3FF12000


//sram
#define PRO_DRAM1_START_ADDR   0x3F800000
#define PRO_DRAM1_END_ADDR(psize)     (PRO_DRAM1_START_ADDR + ((psize) << 17))
//cache mmu register file address
#define CACHE_MMU_ADDRESS_BASE(cpu_no) ((cpu_no) ? (APPCACHE_MMU_ADDR_BASE) : (PROCACHE_MMU_ADDR_BASE))
//virtual address, physical address check
#define ADDRESS_CHECK(addr,psize) (((addr) & (0xFFFF >>((64/(psize))-1))) != 0)
//CPU number check
#define CPU_NUMBER_CHECK(cpu_no)  (((cpu_no)<0) || ((cpu_no)>1))
//PID check
#define PID_CHECK(pid)  (((pid)<0) || ((pid)>7))
//flash MMU edge check (flash size default : 16*1024 K)
#define FLASH_MMU_EDGE_CHECK(mmu_val,num) (((mmu_val) + (num)) > 256)
//sram MMU edge check (sram size default : 8*1024 K)
#define SRAM_MMU_EDGE_CHECK(mmu_val,num,psize) (((mmu_val) + (num)) > ((8*1024)/(psize)))

//We can relegate to the ROM version if the 2nd core isn't running (yet) and the RTOS is not started yet, for instance
//in the bootloader and in the app start process. The ROM code manually disables the cache, without using
//cache guards.
unsigned int cache_sram_mmu_set_rom(int cpu_no, int pid, unsigned int vaddr, unsigned int paddr, int psize, int num);


#ifndef BOOTLOADER_BUILD

/*
Note that this function is a replacement for the ROM function with the same name, with these differences:
- It uses the DPORT workarounds
- It fixes a bug where the ROM version throws an error when vaddr is more than 2MiB into the memory region
- It uses the SPI cache guards to make sure the MMU is idle
*/
unsigned int IRAM_ATTR cache_sram_mmu_set(int cpu_no, int pid, unsigned int vaddr, unsigned int paddr, int psize, int num)
{
    const spi_flash_guard_funcs_t *guard=spi_flash_guard_get();
    if (!guard) {
        //Still starting up; guards not available yet. Use ROM version of code.
        return cache_sram_mmu_set_rom(cpu_no, pid, vaddr, paddr, psize, num);
    }

    unsigned int i,shift,mask_s;
    unsigned int mmu_addr;
    unsigned int mmu_table_val;
    //address check
    if( (ADDRESS_CHECK(vaddr,psize)) || (ADDRESS_CHECK(paddr,psize)) ){
        return MMU_SET_ADDR_ALIGNED_ERROR;
    }
    //psize check
    if(psize == 32) {
        shift  = 15;
        mask_s = 0;
    } else if(psize == 16) {
        shift  = 14;
        mask_s = 1;
    } else if(psize == 8) {
        shift  = 13;
        mask_s = 2;
    } else if(psize == 4) {
        shift  = 12;
        mask_s = 3;
    } else if(psize == 2) {
        shift  = 11;
        mask_s = 4;
    } else {
        return MMU_SET_PAGE_SIZE_ERROR;
    }
    //mmu value
    mmu_table_val = paddr >> shift;
    //mmu_addr
    if(pid == 0 || pid == 1){
        if(vaddr >= PRO_DRAM1_START_ADDR && vaddr < PRO_DRAM1_END_ADDR(psize)){
            mmu_addr = 1152 + ((vaddr & (0x3FFFFF >> mask_s)) >> shift);
        } else{
            return MMU_SET_VADDR_OUT_RANGE;
        }
    } else {
        if(vaddr >= PRO_DRAM1_START_ADDR && vaddr < PRO_DRAM1_END_ADDR(psize)){
            mmu_addr = (1024 + (pid<<7)) + ((vaddr & (0x3FFFFF >> mask_s)) >> shift);
        } else{
            return MMU_SET_VADDR_OUT_RANGE;
        }
    }

    //The MMU registers are implemented in such a way that lookups from the cache subsystem may collide with
    //CPU access to the MMU registers. We use the flash guards to make sure the cache is disabled.
    guard->start();

    //mmu change
    for ( i = 0; i < num; i++){
        *(volatile unsigned int *)(CACHE_MMU_ADDRESS_BASE(cpu_no) + mmu_addr * 4) = mmu_table_val + i; //write table
        mmu_addr++;
    }

    if(cpu_no == 0){
        DPORT_REG_SET_FIELD(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CMMU_SRAM_PAGE_MODE, mask_s);
    } else {
        DPORT_REG_SET_FIELD(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CMMU_SRAM_PAGE_MODE, mask_s);
    }

    guard->end();

    return 0;
}


#else

//For the bootloader, we can always use the ROM version of this: it works well enough and keeps the size of the bootloader binary down.
unsigned int cache_sram_mmu_set(int cpu_no, int pid, unsigned int vaddr, unsigned int paddr, int psize, int num) {
    return cache_sram_mmu_set_rom(cpu_no, pid, vaddr, paddr, psize, num);
}

#endif
