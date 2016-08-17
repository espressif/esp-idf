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
#ifndef _ROM_CACHE_H_
#define _ROM_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif

//===========================================
//  function :     cache_init
//  description:   initialise cache mmu, mark all entries as invalid.
//  conditions:
//                 Call Cache_Read_Disable() before calling this function.
//  inputs:
//                 cpu_no is CPU number,0(PRO CPU) or 1(APP CPU),
//  output:        NONE
//===========================================
void mmu_init(int cpu_no);

//===========================================
//  function :     cache_flash_mmu_set
//  description:   Configure MMU to cache a flash region.
//  conditions:
//                 Call this function to configure the flash cache before enabling it.
//                 Check return value to verify MMU was set correctly.
//  inputs:
//                 cpu_no is CPU number,0(PRO CPU) or 1(APP CPU),
//                 pid    is process identifier. Range 0~7
//                 vaddr  is "virtual" address in CPU address space. Can be IRam0, IRam1, IRom0 and DRom0 memory address.
//                        Should be aligned by psize
//                 paddr  is "physical" address in flash controller's address space.
//                        ie for 16M flash the range is 0x000000~0xFFFFFF. Should be aligned by psize
//                 psize  is page size of flash, in kilobytes. Can be 64, 32, 16.
//                 num    is number of pages to be set, valid range 0 ~ (flash size)/(page size)
//  output:        error status
//                 0 : mmu set success
//                 1 : vaddr or paddr is not aligned
//                 2 : pid error
//                 3 : psize error
//                 4 : mmu table to be written is out of range
//                 5 : vaddr is out of range
//===========================================
unsigned int cache_flash_mmu_set(int cpu_no, int pid, unsigned int vaddr, unsigned int paddr,  int psize, int num);

unsigned int cache_sram_mmu_set(int cpu_no, int pid,unsigned int vaddr, unsigned int paddr, int psize, int num);

void Cache_Read_Init(int cpu_no);

void Cache_Read_Disable(int cpu_no);

void Cache_Read_Enable(int cpu_no);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_CACHE_H_ */
