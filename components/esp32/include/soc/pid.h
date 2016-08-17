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
#ifndef _SOC_PID_H_
#define _SOC_PID_H_

#define PROPID_GEN_BASE 0x3FF1F000
//Bits 1..7: 1 if interrupt will be triggering PID change
#define PROPID_CONFIG_INTERRUPT_ENABLE	((PROPID_GEN_BASE)+0x000)
//Vectors for the various interrupt handlers
#define PROPID_CONFIG_INTERRUPT_ADDR_1	((PROPID_GEN_BASE)+0x004)
#define PROPID_CONFIG_INTERRUPT_ADDR_2	((PROPID_GEN_BASE)+0x008)
#define PROPID_CONFIG_INTERRUPT_ADDR_3	((PROPID_GEN_BASE)+0x00C)
#define PROPID_CONFIG_INTERRUPT_ADDR_4	((PROPID_GEN_BASE)+0x010)
#define PROPID_CONFIG_INTERRUPT_ADDR_5	((PROPID_GEN_BASE)+0x014)
#define PROPID_CONFIG_INTERRUPT_ADDR_6	((PROPID_GEN_BASE)+0x018)
#define PROPID_CONFIG_INTERRUPT_ADDR_7	((PROPID_GEN_BASE)+0x01C)

//Delay, in CPU cycles, before switching to new PID
#define PROPID_CONFIG_PID_DELAY			((PROPID_GEN_BASE)+0x020)
#define PROPID_CONFIG_NMI_DELAY			((PROPID_GEN_BASE)+0x024)

//Last detected interrupt. Set by hw on int.
#define PROPID_TABLE_LEVEL				((PROPID_GEN_BASE)+0x028)
//PID/prev int data for each int
#define PROPID_FROM_1					((PROPID_GEN_BASE)+0x02C)
#define PROPID_FROM_2					((PROPID_GEN_BASE)+0x030)
#define PROPID_FROM_3					((PROPID_GEN_BASE)+0x034)
#define PROPID_FROM_4					((PROPID_GEN_BASE)+0x038)
#define PROPID_FROM_5					((PROPID_GEN_BASE)+0x03C)
#define PROPID_FROM_6					((PROPID_GEN_BASE)+0x040)
#define PROPID_FROM_7					((PROPID_GEN_BASE)+0x044)
#define PROPID_FROM_PID_MASK	0x7
#define PROPID_FROM_PID_S		0
#define PROPID_FROM_INT_MASK	0xF
#define PROPID_FROM_INT_S		3

//PID to be set after confirm routine
#define PROPID_PID_NEW					((PROPID_GEN_BASE)+0x048)
//Write to kick off PID change
#define PROPID_PID_CONFIRM				((PROPID_GEN_BASE)+0x04c)
//current PID?
#define PROPID_PID_REG					((PROPID_GEN_BASE)+0x050)

//Write to mask NMI
#define PROPID_PID_NMI_MASK_HW_ENABLE	((PROPID_GEN_BASE)+0x054)
//Write to unmask NMI
#define PROPID_PID_NMI_MASK_HW_DISABLE	((PROPID_GEN_BASE)+0x058)
#define PROPID_PID_NMI_MASK_HW_REG		((PROPID_GEN_BASE)+0x05c)

//Debug regs
#define PROPID_PID						((PROPID_GEN_BASE)+0x060)
#define PROPID_NMI_MASK_HW				((PROPID_GEN_BASE)+0x064)

#endif /* _SOC_PID_H_ */
