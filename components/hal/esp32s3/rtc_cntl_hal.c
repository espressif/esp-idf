/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for RTC CNTL (common part)

#include "hal/rtc_hal.h"
#include "soc/soc_caps.h"
#include "esp32s3/rom/lldesc.h"
#include "esp32s3/rom/cache.h"
#include "hal/dma_types.h"
#include "hal/assert.h"
#include "esp_attr.h"

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
    plink->owner  = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    plink->size   = size >> 4;  /* in unit of 16 bytes */
    plink->length = size >> 4;
    plink->buf    = buff;
    plink->offset = 0;
    plink->sosf   = 0;
    STAILQ_NEXT(plink, qe) = next;
    return (void *)plink;
}

#if SOC_PM_SUPPORT_CPU_PD

void rtc_cntl_hal_enable_cpu_retention(void *addr)
{
    rtc_cntl_sleep_retent_t *retent = (rtc_cntl_sleep_retent_t *)addr;

    if (addr) {
        if (retent->cpu_pd_mem) {
            lldesc_t *plink = (lldesc_t *)retent->cpu_pd_mem;

            /* dma link buffer configure */
            rtc_cntl_link_buf_conf_t *pbuf = (rtc_cntl_link_buf_conf_t *)plink->buf;
            pbuf->cfg[0] = 0;
            pbuf->cfg[1] = 0;
            pbuf->cfg[2] = 0;
            pbuf->cfg[3] = 0xfffe0000;

            rtc_cntl_ll_set_cpu_retention_link_addr((uint32_t)plink);
            rtc_cntl_ll_enable_cpu_retention_clock();
            rtc_cntl_ll_enable_cpu_retention();
        }
    }
}

void IRAM_ATTR rtc_cntl_hal_disable_cpu_retention(void *addr)
{
    rtc_cntl_sleep_retent_t *retent = (rtc_cntl_sleep_retent_t *)addr;

    if (addr) {
        if (retent->cpu_pd_mem) {
            /* TODO: I/d-cache tagmem retention has not been implemented yet,
             * so after the system wakes up, all the contents of i/d-cache need
             * to be invalidated. */
            Cache_Invalidate_ICache_All();
            Cache_Invalidate_DCache_All();
            rtc_cntl_ll_disable_cpu_retention();
        }
    }
}

#endif // SOC_PM_SUPPORT_CPU_PD
