// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

// The HAL layer for RTC CNTL (common part)

#include "soc/soc_caps.h"
#include "soc/lldesc.h"
#include "hal/rtc_hal.h"
#include "hal/assert.h"

#define RTC_CNTL_HAL_LINK_BUF_SIZE_MIN  (SOC_RTC_CNTL_CPU_PD_DMA_BLOCK_SIZE) /* The minimum size of dma link buffer */

typedef struct rtc_cntl_link_buf_conf {
    uint32_t cfg[4];    /* 4 word for dma link buffer configuration */
} rtc_cntl_link_buf_conf_t;

void * rtc_cntl_hal_dma_link_init(void *elem, void *buff, int size, void *next)
{
    HAL_ASSERT(elem != NULL);
    HAL_ASSERT(buff != NULL);
    HAL_ASSERT(size >= RTC_CNTL_HAL_LINK_BUF_SIZE_MIN);

    lldesc_t *plink = (lldesc_t *)elem;

    plink->eof    = next ? 0 : 1;
    plink->owner  = 1;
    plink->size   = size >> 4;  /* in unit of 16 bytes */
    plink->length = size >> 4;
    plink->buf    = buff;
    plink->offset = 0;
    plink->sosf   = 0;
    STAILQ_NEXT(plink, qe) = next;
    return (void *)plink;
}

void rtc_cntl_hal_enable_cpu_retention(void *addr)
{
    if (addr) {
        lldesc_t *plink = (lldesc_t *)addr;

        /* dma link buffer configure */
        rtc_cntl_link_buf_conf_t *pbuf = (rtc_cntl_link_buf_conf_t *)plink->buf;
        pbuf->cfg[0] = 0;
        pbuf->cfg[1] = 0;
        pbuf->cfg[2] = 0;
        pbuf->cfg[3] = (uint32_t)-1;

        rtc_cntl_ll_enable_cpu_retention((uint32_t)addr);
    }
}
