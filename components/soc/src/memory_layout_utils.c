// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "soc/soc_memory_layout.h"
#include "sdkconfig.h"

static const char *TAG = "memory_layout";

/* These variables come from the linker script,
   delimit the start and end of entries created via
   SOC_RESERVE_MEMORY_REGION() macro.
*/
extern soc_reserved_region_t soc_reserved_memory_region_start;
extern soc_reserved_region_t soc_reserved_memory_region_end;

/*
These variables have the start and end of the data and static IRAM
area used by the program. Defined in the linker script.
*/
extern int _data_start, _heap_start, _iram_start, _iram_end;

/* static DRAM & IRAM chunks */
static const size_t EXTRA_RESERVED_REGIONS = 2;

static size_t s_get_num_reserved_regions(void)
{
    return ( ( &soc_reserved_memory_region_end
               - &soc_reserved_memory_region_start ) +
             EXTRA_RESERVED_REGIONS );
}

size_t soc_get_available_memory_region_max_count(void)
{
    /* Worst-case: each reserved memory region splits an available
       region in two, so the maximum possible number of regions
       is the number of regions of memory plus the number of reservations */
    return soc_memory_region_count + s_get_num_reserved_regions();
}

static int s_compare_reserved_regions(const void *a, const void *b)
{
    const soc_reserved_region_t *r_a = (soc_reserved_region_t *)a;
    const soc_reserved_region_t *r_b = (soc_reserved_region_t *)b;
    return (int)r_a->start - (int)r_b->start;
}

/* Initialize a mutable array of reserved regions in 'reserved',
   then sort it by start address and check for overlapping
   reserved regions (illegal).
*/
static void s_prepare_reserved_regions(soc_reserved_region_t *reserved, size_t count)
{
    memcpy(reserved + EXTRA_RESERVED_REGIONS,
           &soc_reserved_memory_region_start,
           (count - EXTRA_RESERVED_REGIONS) * sizeof(soc_reserved_region_t));

    /* Add the EXTRA_RESERVED_REGIONS at the beginning */
    reserved[0].start = (intptr_t)&_data_start; /* DRAM used by data+bss and possibly rodata */
    reserved[0].end = (intptr_t)&_heap_start;
#if CONFIG_IDF_TARGET_ESP32
    //ESP32 has a IRAM-only region 0x4008_0000 - 0x4009_FFFF, protect the used part
    reserved[1].start = (intptr_t)&_iram_start; /* IRAM used by code */
    reserved[1].end = (intptr_t)&_iram_end;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    //ESP32S2 has a big D/IRAM region, the part used by code is reserved
    //The address of the D/I bus are in the same order, directly shift IRAM address to get reserved DRAM address
    const uint32_t i_d_offset = SOC_IRAM_LOW - SOC_DRAM_LOW;
    reserved[1].start = (intptr_t)&_iram_start - i_d_offset; /* IRAM used by code */
    reserved[1].end = (intptr_t)&_iram_end - i_d_offset;
#else
#   error chip not implemented!
#endif

    /* Sort by starting address */
    qsort(reserved, count, sizeof(soc_reserved_region_t), s_compare_reserved_regions);

    /* Validity checks */
    ESP_EARLY_LOGV(TAG, "reserved range is %p - %p",
                   &soc_reserved_memory_region_start,
                   &soc_reserved_memory_region_end);
    ESP_EARLY_LOGD(TAG, "Checking %d reserved memory ranges:", count);
    for (size_t i = 0; i < count; i++)
    {
        ESP_EARLY_LOGD(TAG, "Reserved memory range 0x%08x - 0x%08x",
                       reserved[i].start, reserved[i].end);
        reserved[i].start = reserved[i].start & ~3; /* expand all reserved areas to word boundaries */
        reserved[i].end = (reserved[i].end + 3) & ~3;
        assert(reserved[i].start < reserved[i].end);
        if (i < count - 1) {
            assert(reserved[i+1].start > reserved[i].start);
            if (reserved[i].end > reserved[i+1].start) {
                ESP_EARLY_LOGE(TAG, "SOC_RESERVE_MEMORY_REGION region range " \
                               "0x%08x - 0x%08x overlaps with 0x%08x - 0x%08x",
                               reserved[i].start, reserved[i].end, reserved[i+1].start,
                               reserved[i+1].end);
                abort();
            }
        }
    }
}

size_t soc_get_available_memory_regions(soc_memory_region_t *regions)
{
    soc_memory_region_t *out_region = regions;
    /* make a local copy of the "input" regions so we can modify them */
    soc_memory_region_t in_regions[soc_memory_region_count];
    memcpy(in_regions, soc_memory_regions, sizeof(in_regions));
    soc_memory_region_t *in_region = in_regions;

    size_t num_reserved = s_get_num_reserved_regions();
    soc_reserved_region_t reserved[num_reserved];

    s_prepare_reserved_regions(reserved, num_reserved);

    /* Go through the "in" regions (full regions, with no reserved
       sections removed from them) one at a time, trim off each reserved
       region, and then copy them to an out_region once trimmed
    */
    ESP_EARLY_LOGD(TAG, "Building list of available memory regions:");
    while(in_region != in_regions + soc_memory_region_count) {
        soc_memory_region_t in = *in_region;
        ESP_EARLY_LOGV(TAG, "Examining memory region 0x%08x - 0x%08x", in.start, in.start + in.size);
        intptr_t in_start = in.start;
        intptr_t in_end = in_start + in.size;
        bool copy_in_to_out = true;
        bool move_to_next = true;

        for (size_t i = 0; i < num_reserved; i++) {
            if (reserved[i].end <= in_start) {
                /* reserved region ends before 'in' starts */
                continue;
            }
            else if (reserved[i].start >= in_end) {
                /* reserved region starts after 'in' ends */
                break;
            }
            else if (reserved[i].start <= in_start &&
                     reserved[i].end >= in_end) { /* reserved covers all of 'in' */
                ESP_EARLY_LOGV(TAG, "Region 0x%08x - 0x%08x inside of reserved 0x%08x - 0x%08x",
                               in_start, in_end, reserved[i].start, reserved[i].end);
                /* skip 'in' entirely */
                copy_in_to_out = false;
                break;
            }
            else if (in_start < reserved[i].start &&
                     in_end > reserved[i].end) { /* reserved contained inside 'in', need to "hole punch" */
                ESP_EARLY_LOGV(TAG, "Region 0x%08x - 0x%08x contains reserved 0x%08x - 0x%08x",
                               in_start, in_end, reserved[i].start, reserved[i].end);
                assert(in_start < reserved[i].start);
                assert(in_end > reserved[i].end);

                /* shrink this region to end where the reserved section starts */
                in_end = reserved[i].start;
                in.size = in_end - in_start;

                /* update in_region so the 'next' iteration uses the region
                   after the reserved section */
                in_region->size -= (reserved[i].end - in_region->start);
                in_region->start = reserved[i].end;

                /* add first region, then re-run while loop with the updated in_region */
                move_to_next = false;
                break;
            }
            else if (reserved[i].start <= in_start) { /* reserved overlaps start of 'in' */
                ESP_EARLY_LOGV(TAG, "Start of region 0x%08x - 0x%08x overlaps reserved 0x%08x - 0x%08x",
                               in_start, in_end, reserved[i].start, reserved[i].end);
                in.start = reserved[i].end;
                in_start = in.start;
                in.size = in_end - in_start;
            }
            else { /* reserved overlaps end of 'in' */
                ESP_EARLY_LOGV(TAG, "End of region 0x%08x - 0x%08x overlaps reserved 0x%08x - 0x%08x",
                               in_start, in_end, reserved[i].start, reserved[i].end);
                in_end = reserved[i].start;
                in.size = in_end - in_start;
            }
        }

        if (copy_in_to_out) {
            ESP_EARLY_LOGD(TAG, "Available memory region 0x%08x - 0x%08x", in.start, in.start + in.size);
            *out_region++ = in;
        }
        if (move_to_next) {
            in_region++;
        }
    }

    return (out_region - regions); /* return number of regions */
}
