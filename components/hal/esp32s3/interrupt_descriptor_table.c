// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "sdkconfig.h"
#include "hal/interrupt_controller_hal.h"
#include "hal/interrupt_controller_ll.h"
#include "soc/soc_caps.h"
#include "soc/soc.h"

//We should mark the interrupt for the timer used by FreeRTOS as reserved. The specific timer
//is selectable using menuconfig; we use these cpp bits to convert that into something we can use in
//the table below.
#if CONFIG_FREERTOS_CORETIMER_0
#define INT6RES INTDESC_RESVD
#else
#define INT6RES INTDESC_SPECIAL
#endif

#if CONFIG_FREERTOS_CORETIMER_1
#define INT15RES INTDESC_RESVD
#else
#define INT15RES INTDESC_SPECIAL
#endif

//This is basically a software-readable version of the interrupt usage table in include/soc/soc.h
const static int_desc_t interrupt_descriptor_table [32]={
    { 1, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_RESVD } }, //0
    { 1, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_RESVD } }, //1
    { 1, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //2
    { 1, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //3
    { 1, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_NORMAL} }, //4
    { 1, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_RESVD } }, //5
    { 1, INTTP_NA,    {INT6RES,        INT6RES       } }, //6
    { 1, INTTP_NA,    {INTDESC_SPECIAL,INTDESC_SPECIAL}}, //7
    { 1, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_RESVD } }, //8
    { 1, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //9
    { 1, INTTP_EDGE , {INTDESC_NORMAL, INTDESC_NORMAL} }, //10
    { 3, INTTP_NA,    {INTDESC_SPECIAL,INTDESC_SPECIAL}}, //11
    { 1, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //12
    { 1, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //13
    { 7, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_RESVD } }, //14, NMI
    { 3, INTTP_NA,    {INT15RES,       INT15RES      } }, //15
    { 5, INTTP_NA,    {INTDESC_SPECIAL,INTDESC_SPECIAL} }, //16
    { 1, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //17
    { 1, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //18
    { 2, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //19
    { 2, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //20
    { 2, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //21
    { 3, INTTP_EDGE,  {INTDESC_RESVD,  INTDESC_NORMAL} }, //22
    { 3, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_NORMAL} }, //23
    { 4, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_NORMAL} }, //24
    { 4, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_RESVD } }, //25
    { 5, INTTP_LEVEL, {INTDESC_NORMAL, INTDESC_RESVD } }, //26
    { 3, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_RESVD } }, //27
    { 4, INTTP_EDGE,  {INTDESC_NORMAL, INTDESC_NORMAL} }, //28
    { 3, INTTP_NA,    {INTDESC_SPECIAL,INTDESC_SPECIAL}}, //29
    { 4, INTTP_EDGE,  {INTDESC_RESVD,  INTDESC_RESVD } }, //30
    { 5, INTTP_LEVEL, {INTDESC_RESVD,  INTDESC_RESVD } }, //31
};

const int_desc_t *interrupt_controller_hal_desc_table(void)
{
    return interrupt_descriptor_table;
}
