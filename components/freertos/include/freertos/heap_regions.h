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
#ifndef _HEAP_REGIONS_H
#define _HEAP_REGIONS_H

#include "freertos/FreeRTOS.h"


typedef struct HeapRegionTagged
{
	uint8_t *pucStartAddress;
	size_t xSizeInBytes;
	BaseType_t xTag;
	uint32_t xExecAddr;
} HeapRegionTagged_t;


void vPortDefineHeapRegionsTagged( const HeapRegionTagged_t * const pxHeapRegions );
void *pvPortMallocTagged( size_t xWantedSize, BaseType_t tag );



#endif