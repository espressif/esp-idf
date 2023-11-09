/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
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

#define DEFAULT_RETENTION_WAIT_CYCLES          (0x7f)
#define DEFAULT_RETENTION_CLKOFF_WAIT_CYCLES   (0xf)
#define DEFAULT_RETENTION_DONE_WAIT_CYCLES     (0x7)

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
            rtc_cntl_ll_config_cpu_retention_timing(
                    DEFAULT_RETENTION_WAIT_CYCLES,
                    DEFAULT_RETENTION_CLKOFF_WAIT_CYCLES,
                    DEFAULT_RETENTION_DONE_WAIT_CYCLES
                );
            rtc_cntl_ll_enable_cpu_retention_clock();
            rtc_cntl_ll_enable_cpu_retention();
#if SOC_PM_SUPPORT_TAGMEM_PD
            if (!retent->tagmem.dcache.enable) {
                // Here we only need to care for the safety of the PSRAM data in the DCache.
                // Since only rodata, bss, heap data may be placed in PSRAM, and these data won't be
                // modified in the sleep process code after now, so it is safe to writeback here.
                Cache_WriteBack_All();
            }
#endif
        }
    }
}

void IRAM_ATTR rtc_cntl_hal_disable_cpu_retention(void *addr)
{
    rtc_cntl_sleep_retent_t *retent = (rtc_cntl_sleep_retent_t *)addr;

    if (addr) {
        if (retent->cpu_pd_mem) {
            /* I/d-cache tagmem retention has not been included or not
             * been enabled, after the system wakes up, all the contents
             * of i/d-cache need to be invalidated. */
#if SOC_PM_SUPPORT_TAGMEM_PD
            if (!retent->tagmem.icache.enable) {
                Cache_Invalidate_ICache_All();
            }
            if (!retent->tagmem.dcache.enable) {
                Cache_Invalidate_DCache_All();
            }
#else
            Cache_Invalidate_ICache_All();
            Cache_Invalidate_DCache_All();
#endif // SOC_PM_SUPPORT_TAGMEM_PD
            rtc_cntl_ll_disable_cpu_retention();
        }
    }
}

#endif // SOC_PM_SUPPORT_CPU_PD

#if SOC_PM_SUPPORT_TAGMEM_PD

void rtc_cntl_hal_enable_tagmem_retention(void *addr)
{
    rtc_cntl_sleep_retent_t *retent = (rtc_cntl_sleep_retent_t *)addr;

    if (addr) {
        if (retent->tagmem.link_addr) {
            rtc_cntl_ll_set_tagmem_retention_link_addr((uint32_t)(retent->tagmem.link_addr));
            rtc_cntl_ll_enable_tagmem_retention();
            if (retent->tagmem.icache.enable) {
                rtc_cntl_ll_enable_icache_tagmem_retention(
                        retent->tagmem.icache.start_point,
                        retent->tagmem.icache.vld_size,
                        retent->tagmem.icache.size
                    );
            }
            if (retent->tagmem.dcache.enable) {
                rtc_cntl_ll_enable_dcache_tagmem_retention(
                        retent->tagmem.dcache.start_point,
                        retent->tagmem.dcache.vld_size,
                        retent->tagmem.dcache.size
                    );
            }
        }
    }
}

void IRAM_ATTR rtc_cntl_hal_disable_tagmem_retention(void *addr)
{
    rtc_cntl_sleep_retent_t *retent = (rtc_cntl_sleep_retent_t *)addr;

    if (addr) {
        if (retent->tagmem.link_addr) {
            rtc_cntl_ll_disable_tagmem_retention();
            if (retent->tagmem.icache.enable) {
                rtc_cntl_ll_disable_icache_tagmem_retention();
            }
            if (retent->tagmem.dcache.enable) {
                rtc_cntl_ll_disable_dcache_tagmem_retention();
            }
        }
    }
}

#endif // SOC_PM_SUPPORT_TAGMEM_PD
