// Copyright 2010-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

//Register Bits{{
#define BIT31   0x80000000
#define BIT30   0x40000000
#define BIT29   0x20000000
#define BIT28   0x10000000
#define BIT27   0x08000000
#define BIT26   0x04000000
#define BIT25   0x02000000
#define BIT24   0x01000000
#define BIT23   0x00800000
#define BIT22   0x00400000
#define BIT21   0x00200000
#define BIT20   0x00100000
#define BIT19   0x00080000
#define BIT18   0x00040000
#define BIT17   0x00020000
#define BIT16   0x00010000
#define BIT15   0x00008000
#define BIT14   0x00004000
#define BIT13   0x00002000
#define BIT12   0x00001000
#define BIT11   0x00000800
#define BIT10   0x00000400
#define BIT9     0x00000200
#define BIT8     0x00000100
#define BIT7     0x00000080
#define BIT6     0x00000040
#define BIT5     0x00000020
#define BIT4     0x00000010
#define BIT3     0x00000008
#define BIT2     0x00000004
#define BIT1     0x00000002
#define BIT0     0x00000001
//}}

#define BIT63    (0x80000000ULL << 32)
#define BIT62    (0x40000000ULL << 32)
#define BIT61    (0x20000000ULL << 32)
#define BIT60    (0x10000000ULL << 32)
#define BIT59    (0x08000000ULL << 32)
#define BIT58    (0x04000000ULL << 32)
#define BIT57    (0x02000000ULL << 32)
#define BIT56    (0x01000000ULL << 32)
#define BIT55    (0x00800000ULL << 32)
#define BIT54    (0x00400000ULL << 32)
#define BIT53    (0x00200000ULL << 32)
#define BIT52    (0x00100000ULL << 32)
#define BIT51    (0x00080000ULL << 32)
#define BIT50    (0x00040000ULL << 32)
#define BIT49    (0x00020000ULL << 32)
#define BIT48    (0x00010000ULL << 32)
#define BIT47    (0x00008000ULL << 32)
#define BIT46    (0x00004000ULL << 32)
#define BIT45    (0x00002000ULL << 32)
#define BIT44    (0x00001000ULL << 32)
#define BIT43    (0x00000800ULL << 32)
#define BIT42    (0x00000400ULL << 32)
#define BIT41    (0x00000200ULL << 32)
#define BIT40    (0x00000100ULL << 32)
#define BIT39    (0x00000080ULL << 32)
#define BIT38    (0x00000040ULL << 32)
#define BIT37    (0x00000020ULL << 32)
#define BIT36    (0x00000010ULL << 32)
#define BIT35    (0x00000008ULL << 32)
#define BIT34    (0x00000004ULL << 32)
#define BIT33    (0x00000002ULL << 32)
#define BIT32    (0x00000001ULL << 32)

#ifndef __ASSEMBLER__
#ifndef BIT
#define BIT(nr)                 (1UL << (nr))
#endif
#ifndef BIT64
#define BIT64(nr)               (1ULL << (nr))
#endif
#else
#ifndef BIT
#define BIT(nr)                 (1 << (nr))
#endif
#endif
