/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "esp_dma_utils.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "esp_cache.h"
#endif
#include "hal/emac_hal.h"
#include "esp_private/eth_mac_esp_dma.h"

#define ETH_CRC_LENGTH (4)

#define EMAC_HAL_BUF_MAGIC_ID 0x1E1C8416

static const char *TAG = "esp.emac.dma";

struct emac_esp_dma_t
{
    emac_hal_context_t hal;
    void *descriptors;
    eth_dma_rx_descriptor_t *rx_desc;
    eth_dma_tx_descriptor_t *tx_desc;
    uint8_t *rx_buf[CONFIG_ETH_DMA_RX_BUFFER_NUM];
    uint8_t *tx_buf[CONFIG_ETH_DMA_TX_BUFFER_NUM];
};

typedef struct {
#ifndef NDEBUG
    uint32_t magic_id;
#endif // NDEBUG
    uint32_t copy_len;
}__attribute__((packed)) emac_esp_dma_auto_buf_info_t;

void emac_esp_dma_reset_desc_chain(emac_esp_dma_handle_t emac_esp_dma)
{
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    size_t cache_sync_len;
    esp_err_t ret = ESP_OK;
#endif

    /* reset DMA descriptors */
    emac_esp_dma->rx_desc = (eth_dma_rx_descriptor_t *)(emac_esp_dma->descriptors);
    emac_esp_dma->tx_desc = (eth_dma_tx_descriptor_t *)(emac_esp_dma->descriptors +
                   sizeof(eth_dma_rx_descriptor_t) * CONFIG_ETH_DMA_RX_BUFFER_NUM);
    /* init rx chain */
    for (int i = 0; i < CONFIG_ETH_DMA_RX_BUFFER_NUM; i++) {
        /* Set Own bit of the Rx descriptor Status: DMA */
        emac_esp_dma->rx_desc[i].RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        /* Set Buffer1 size and Second Address Chained bit */
        emac_esp_dma->rx_desc[i].RDES1.SecondAddressChained = 1;
        emac_esp_dma->rx_desc[i].RDES1.ReceiveBuffer1Size = CONFIG_ETH_DMA_BUFFER_SIZE;
        /* Enable Ethernet DMA Rx Descriptor interrupt */
        emac_esp_dma->rx_desc[i].RDES1.DisableInterruptOnComplete = 0;
        /* point to the buffer */
        emac_esp_dma->rx_desc[i].Buffer1Addr = (uint32_t)(emac_esp_dma->rx_buf[i]);
        /* point to next descriptor */
        emac_esp_dma->rx_desc[i].Buffer2NextDescAddr = (uint32_t)(emac_esp_dma->rx_desc + i + 1);

        /* For last descriptor, set next descriptor address register equal to the first descriptor base address */
        if (i == CONFIG_ETH_DMA_RX_BUFFER_NUM - 1) {
            emac_esp_dma->rx_desc[i].Buffer2NextDescAddr = (uint32_t)(emac_esp_dma->rx_desc);
        }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
        ret = esp_cache_msync((void *)&emac_esp_dma->rx_desc[i], cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
    }

    /* init tx chain */
    for (int i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++) {
        /* Set Own bit of the Tx descriptor Status: CPU */
        emac_esp_dma->tx_desc[i].TDES0.Own = EMAC_LL_DMADESC_OWNER_CPU;
        emac_esp_dma->tx_desc[i].TDES0.SecondAddressChained = 1;
        emac_esp_dma->tx_desc[i].TDES1.TransmitBuffer1Size = CONFIG_ETH_DMA_BUFFER_SIZE;
        /* Enable Ethernet DMA Tx Descriptor interrupt */
        emac_esp_dma->tx_desc[1].TDES0.InterruptOnComplete = 1;
        /* Enable Transmit Timestamp */
        emac_esp_dma->tx_desc[i].TDES0.TransmitTimestampEnable = 1;
        /* point to the buffer */
        emac_esp_dma->tx_desc[i].Buffer1Addr = (uint32_t)(emac_esp_dma->tx_buf[i]);
        /* point to next descriptor */
        emac_esp_dma->tx_desc[i].Buffer2NextDescAddr = (uint32_t)(emac_esp_dma->tx_desc + i + 1);

        /* For last descriptor, set next descriptor address register equal to the first descriptor base address */
        if (i == CONFIG_ETH_DMA_TX_BUFFER_NUM - 1) {
            emac_esp_dma->tx_desc[i].Buffer2NextDescAddr = (uint32_t)(emac_esp_dma->tx_desc);
        }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = sizeof(eth_dma_tx_descriptor_t);
        ret = esp_cache_msync((void *)&emac_esp_dma->tx_desc[i], cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
    }

    /* set base address of the first descriptor */
    emac_hal_set_rx_tx_desc_addr(&emac_esp_dma->hal, emac_esp_dma->rx_desc, emac_esp_dma->tx_desc);
}

uint32_t emac_esp_dma_transmit_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t *buf, uint32_t length)
{
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    esp_err_t ret;
    size_t cache_sync_len;
#endif

    /* Get the number of Tx buffers to use for the frame */
    uint32_t bufcount = 0;
    uint32_t lastlen = length;
    uint32_t sentout = 0;
    while (lastlen > CONFIG_ETH_DMA_BUFFER_SIZE) {
        lastlen -= CONFIG_ETH_DMA_BUFFER_SIZE;
        bufcount++;
    }
    if (lastlen) {
        bufcount++;
    }
    if (bufcount > CONFIG_ETH_DMA_TX_BUFFER_NUM) {
        goto err;
    }

    eth_dma_tx_descriptor_t *desc_iter = emac_esp_dma->tx_desc;
    /* A frame is transmitted in multiple descriptor */
    for (size_t i = 0; i < bufcount; i++) {
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = sizeof(eth_dma_tx_descriptor_t);
        ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
#endif
        /* Check if the descriptor is owned by the Ethernet DMA (when 1) or CPU (when 0) */
        if (desc_iter->TDES0.Own != EMAC_LL_DMADESC_OWNER_CPU) {
            goto err;
        }
        /* Clear FIRST and LAST segment bits */
        desc_iter->TDES0.FirstSegment = 0;
        desc_iter->TDES0.LastSegment = 0;
        desc_iter->TDES0.InterruptOnComplete = 0;
        if (i == 0) {
            /* Setting the first segment bit */
            desc_iter->TDES0.FirstSegment = 1;
            //desc_iter->TDES0.DisableCRC = 1;
        }
        if (i == (bufcount - 1)) {
            /* Setting the last segment bit */
            desc_iter->TDES0.LastSegment = 1;
            /* Enable transmit interrupt */
            desc_iter->TDES0.InterruptOnComplete = 1;
            /* Program size */
            desc_iter->TDES1.TransmitBuffer1Size = lastlen;
            /* copy data from uplayer stack buffer */
            memcpy((void *)(desc_iter->Buffer1Addr), buf + i * CONFIG_ETH_DMA_BUFFER_SIZE, lastlen);
            sentout += lastlen;
        } else {
            /* Program size */
            desc_iter->TDES1.TransmitBuffer1Size = CONFIG_ETH_DMA_BUFFER_SIZE;
            /* copy data from uplayer stack buffer */
            memcpy((void *)(desc_iter->Buffer1Addr), buf + i * CONFIG_ETH_DMA_BUFFER_SIZE, CONFIG_ETH_DMA_BUFFER_SIZE);
            sentout += CONFIG_ETH_DMA_BUFFER_SIZE;
        }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = CONFIG_ETH_DMA_BUFFER_SIZE;
        ret = esp_cache_msync((void *)desc_iter->Buffer1Addr, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
        /* Point to next descriptor */
        desc_iter = (eth_dma_tx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    }

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    for (size_t i = 0; i < bufcount; i++) {
        emac_esp_dma->tx_desc->TDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = sizeof(eth_dma_tx_descriptor_t);
        ret = esp_cache_msync((void *)emac_esp_dma->tx_desc, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
        emac_esp_dma->tx_desc = (eth_dma_tx_descriptor_t *)(emac_esp_dma->tx_desc->Buffer2NextDescAddr);
    }
    emac_hal_transmit_poll_demand(&emac_esp_dma->hal);
    return sentout;
err:
    return 0;
}

uint32_t emac_esp_dma_transmit_multiple_buf_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t **buffs, uint32_t *lengths, uint32_t buffs_cnt)
{
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    esp_err_t ret;
    size_t cache_sync_len;
#endif

    /* Get the number of Tx buffers to use for the frame */
    uint32_t dma_bufcount = 0;
    uint32_t sentout = 0;
    uint8_t *ptr = buffs[0];
    uint32_t lastlen = lengths[0];
    uint32_t avail_len = CONFIG_ETH_DMA_BUFFER_SIZE;

    eth_dma_tx_descriptor_t *desc_iter = emac_esp_dma->tx_desc;
    /* A frame is transmitted in multiple descriptor */
    while (dma_bufcount < CONFIG_ETH_DMA_TX_BUFFER_NUM) {
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = sizeof(eth_dma_tx_descriptor_t);
        ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
#endif
        /* Check if the descriptor is owned by the Ethernet DMA (when 1) or CPU (when 0) */
        if (desc_iter->TDES0.Own != EMAC_LL_DMADESC_OWNER_CPU) {
            goto err;
        }
        /* Clear FIRST and LAST segment bits */
        desc_iter->TDES0.FirstSegment = 0;
        desc_iter->TDES0.LastSegment = 0;
        desc_iter->TDES0.InterruptOnComplete = 0;
        desc_iter->TDES1.TransmitBuffer1Size = 0;
        if (dma_bufcount == 0) {
            /* Setting the first segment bit */
            desc_iter->TDES0.FirstSegment = 1;
        }

        while (buffs_cnt > 0) {
            /* Check if input buff data fits to currently available space in the descriptor */
            if (lastlen < avail_len) {
                /* copy data from uplayer stack buffer */
                memcpy((void *)(desc_iter->Buffer1Addr + (CONFIG_ETH_DMA_BUFFER_SIZE - avail_len)), ptr, lastlen);
                sentout += lastlen;
                avail_len -= lastlen;
                desc_iter->TDES1.TransmitBuffer1Size += lastlen;

                /* Update processed input buffers info */
                buffs_cnt--;
                ptr = *(++buffs);
                lastlen = *(++lengths);
            /* There is only limited available space in the current descriptor, use it all */
            } else {
                /* copy data from uplayer stack buffer */
                memcpy((void *)(desc_iter->Buffer1Addr + (CONFIG_ETH_DMA_BUFFER_SIZE - avail_len)), ptr, avail_len);
                sentout += avail_len;
                lastlen -= avail_len;
                /* If lastlen is not zero, input buff will be fragmented over multiple descriptors */
                if (lastlen > 0) {
                    ptr += avail_len;
                /* Input buff fully fits the descriptor, move to the next input buff */
                } else {
                    /* Update processed input buffers info */
                    buffs_cnt--;
                    ptr = *(++buffs);
                    lastlen = *(++lengths);
                }
                avail_len = CONFIG_ETH_DMA_BUFFER_SIZE;
                desc_iter->TDES1.TransmitBuffer1Size = CONFIG_ETH_DMA_BUFFER_SIZE;
                /* The descriptor is full here so exit and use the next descriptor */
                break;
            }
        }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = CONFIG_ETH_DMA_BUFFER_SIZE;
        ret = esp_cache_msync((void *)desc_iter->Buffer1Addr, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
        /* Increase counter of utilized DMA buffers */
        dma_bufcount++;

        /* If all input buffers processed, mark as LAST segment and finish the coping */
        if (buffs_cnt == 0) {
            /* Setting the last segment bit */
            desc_iter->TDES0.LastSegment = 1;
            /* Enable transmit interrupt */
            desc_iter->TDES0.InterruptOnComplete = 1;
            break;
        }

        /* Point to next descriptor */
        desc_iter = (eth_dma_tx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    }

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    for (size_t i = 0; i < dma_bufcount; i++) {
        emac_esp_dma->tx_desc->TDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = sizeof(eth_dma_tx_descriptor_t);
        ret = esp_cache_msync((void *)emac_esp_dma->tx_desc, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
        emac_esp_dma->tx_desc = (eth_dma_tx_descriptor_t *)(emac_esp_dma->tx_desc->Buffer2NextDescAddr);
    }

    emac_hal_transmit_poll_demand(&emac_esp_dma->hal);
    return sentout;
err:
    return 0;
}

static esp_err_t emac_esp_dma_get_valid_recv_len(emac_esp_dma_handle_t emac_esp_dma, uint32_t *ret_len)
{
    eth_dma_rx_descriptor_t *desc_iter = emac_esp_dma->rx_desc;
    uint32_t used_descs = 0;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    size_t cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
    esp_err_t ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    assert(ret == ESP_OK);
#endif

    /* Traverse descriptors owned by CPU */
    while ((desc_iter->RDES0.Own != EMAC_LL_DMADESC_OWNER_DMA) && (used_descs < CONFIG_ETH_DMA_RX_BUFFER_NUM)) {
        used_descs++;
        /* Last segment in frame */
        if (desc_iter->RDES0.LastDescriptor) {
#if CONFIG_IDF_TARGET_ESP32P4
            /* Since Store Forward must be disabled at ESP32P4, DMA descriptors may contain erroneous frames */
            if (desc_iter->RDES0.ErrSummary) {
                emac_esp_dma_flush_recv_frame(emac_esp_dma, NULL, NULL);
                *ret_len = 0;
                return ESP_FAIL;
            }
#endif //CONFIG_IDF_TARGET_ESP32P4
            /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
            *ret_len = desc_iter->RDES0.FrameLength - ETH_CRC_LENGTH;
            break;
        }
        /* First segment in frame */
        if (desc_iter->RDES0.FirstDescriptor) {
            emac_esp_dma->rx_desc = desc_iter;
        }
        /* point to next descriptor */
        desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        size_t cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
        esp_err_t ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
#endif
    }

    return ESP_OK;
}

static void emac_esp_dma_get_remain_frames(emac_esp_dma_handle_t emac_esp_dma, uint32_t *remain_frames, uint32_t *used_descs)
{
    eth_dma_rx_descriptor_t *desc_iter = emac_esp_dma->rx_desc;
    *remain_frames = 0;
    *used_descs = 0;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    size_t cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
    esp_err_t ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    assert(ret == ESP_OK);
#endif
    /* Traverse descriptors owned by CPU */
    while ((desc_iter->RDES0.Own != EMAC_LL_DMADESC_OWNER_DMA) && (*used_descs < CONFIG_ETH_DMA_RX_BUFFER_NUM)) {
        (*used_descs)++;
        /* Last segment in frame */
        if (desc_iter->RDES0.LastDescriptor) {
            (*remain_frames)++;
        }
        /* point to next descriptor */
        desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        size_t cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
        esp_err_t ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
#endif
    }
}

uint8_t *emac_esp_dma_alloc_recv_buf(emac_esp_dma_handle_t emac_esp_dma, uint32_t *size)
{
    uint32_t ret_len = 0;
    uint32_t copy_len = 0;
    uint8_t *buf = NULL;

    if (emac_esp_dma_get_valid_recv_len(emac_esp_dma, &ret_len) != ESP_OK) {
        *size = 0;
        return NULL;
    }
    /* packets larger than expected will be truncated */
    copy_len = ret_len > *size ? *size : ret_len;

    if (copy_len > 0) {
        buf = malloc(copy_len);
        if (buf != NULL) {
            emac_esp_dma_auto_buf_info_t *buff_info = (emac_esp_dma_auto_buf_info_t *)buf;
            /* no need to check allocated buffer min length prior writing since we know that EMAC DMA is configured to
            not forward erroneous or undersized frames (less than 64B) on ESP32, see emac_hal_init_dma_default */
#ifndef NDEBUG
            buff_info->magic_id = EMAC_HAL_BUF_MAGIC_ID;
#endif // NDEBUG
            buff_info->copy_len = copy_len;
        }
    }
    /* indicate actual size of received frame */
    *size = ret_len;
    return buf;
}

uint32_t emac_esp_dma_receive_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t *buf, uint32_t size, uint32_t *frames_remain, uint32_t *free_desc)
{
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    size_t cache_sync_len;
    esp_err_t ret;
#endif
    eth_dma_rx_descriptor_t *desc_iter = emac_esp_dma->rx_desc;
    eth_dma_rx_descriptor_t *first_desc = emac_esp_dma->rx_desc;
    uint32_t ret_len = 0;
    uint32_t copy_len = 0;

    if (size != EMAC_HAL_BUF_SIZE_AUTO) {
        if (emac_esp_dma_get_valid_recv_len(emac_esp_dma, &ret_len) != ESP_OK) {
            goto err;
        }
        /* packets larger than expected will be truncated */
        copy_len = ret_len > size ? size : ret_len;
    } else {
        emac_esp_dma_auto_buf_info_t *buff_info = (emac_esp_dma_auto_buf_info_t *)buf;
#ifndef NDEBUG
        /* check that buffer was allocated by emac_esp_dma_alloc_recv_buf */
        assert(buff_info->magic_id == EMAC_HAL_BUF_MAGIC_ID);
#endif // NDEBUG
        copy_len = buff_info->copy_len;
        ret_len = copy_len;
    }

    if (copy_len) {
        desc_iter = first_desc;
        while(copy_len > CONFIG_ETH_DMA_BUFFER_SIZE) {
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
            cache_sync_len = CONFIG_ETH_DMA_BUFFER_SIZE;
            ret = esp_cache_msync((void *)desc_iter->Buffer1Addr, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
            assert(ret == ESP_OK);
#endif
            memcpy(buf, (void *)(desc_iter->Buffer1Addr), CONFIG_ETH_DMA_BUFFER_SIZE);
            buf += CONFIG_ETH_DMA_BUFFER_SIZE;
            copy_len -= CONFIG_ETH_DMA_BUFFER_SIZE;
            /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
            desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
            cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
            ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
            assert(ret == ESP_OK);
#endif
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE // TODO cleanup (IDF-8993)
        cache_sync_len = CONFIG_ETH_DMA_BUFFER_SIZE;
        ret = esp_cache_msync((void *)desc_iter->Buffer1Addr, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
#endif
        memcpy(buf, (void *)(desc_iter->Buffer1Addr), copy_len);
        desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
        ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
        /* `copy_len` does not include CRC (which may be stored in separate buffer), hence check if we reached the last descriptor */
        while (!desc_iter->RDES0.LastDescriptor) {
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
            desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
            cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
            ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
            assert(ret == ESP_OK);
#endif
        }
        /* update rxdesc */
        emac_esp_dma->rx_desc = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        /* poll rx demand */
        emac_hal_receive_poll_demand(&emac_esp_dma->hal);
    }
err:
    /* check how many frames left to handle */
    uint32_t used_descs = 0;
    emac_esp_dma_get_remain_frames(emac_esp_dma, frames_remain, &used_descs);
    *free_desc = CONFIG_ETH_DMA_RX_BUFFER_NUM - used_descs;
    return ret_len;
}

void emac_esp_dma_flush_recv_frame(emac_esp_dma_handle_t emac_esp_dma, uint32_t *frames_remain, uint32_t *free_desc)
{
    eth_dma_rx_descriptor_t *desc_iter = emac_esp_dma->rx_desc;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    size_t cache_sync_len;
    esp_err_t ret;
    cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
    ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    assert (ret == ESP_OK);
#endif
    /* While not last descriptor => return back to DMA */
    while (!desc_iter->RDES0.LastDescriptor) {
        desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
        ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert (ret == ESP_OK);
#endif
        desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    }
    /* the last descriptor */
    desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    cache_sync_len = sizeof(eth_dma_rx_descriptor_t);
    ret = esp_cache_msync((void *)desc_iter, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    assert (ret == ESP_OK);
#endif
    /* update rxdesc */
    emac_esp_dma->rx_desc = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    /* poll rx demand */
    emac_hal_receive_poll_demand(&emac_esp_dma->hal);

    if (frames_remain != NULL && free_desc != NULL) {
        /* check how many frames left to handle */
        uint32_t used_descs = 0;
        emac_esp_dma_get_remain_frames(emac_esp_dma, frames_remain, &used_descs);
        *free_desc = CONFIG_ETH_DMA_RX_BUFFER_NUM - used_descs;
    }
}

esp_err_t emac_esp_new_dma(const emac_esp_dma_config_t* config, emac_esp_dma_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    *ret_handle = NULL;
    struct emac_esp_dma_t *emac_esp_dma = calloc(1, sizeof(struct emac_esp_dma_t));
    ESP_GOTO_ON_FALSE(emac_esp_dma, ESP_ERR_NO_MEM, err, TAG, "no mem for esp emac_esp_dma object");

    /* alloc memory for ethernet dma descriptor */
    uint32_t desc_size = CONFIG_ETH_DMA_RX_BUFFER_NUM * sizeof(eth_dma_rx_descriptor_t) +
                         CONFIG_ETH_DMA_TX_BUFFER_NUM * sizeof(eth_dma_tx_descriptor_t);
    esp_dma_mem_info_t dma_mem_info = {
        .extra_heap_caps = MALLOC_CAP_INTERNAL,
        .dma_alignment_bytes = 4,
    };
    esp_dma_capable_calloc(1, desc_size, &dma_mem_info, (void*)&emac_esp_dma->descriptors, NULL);

    ESP_GOTO_ON_FALSE(emac_esp_dma->descriptors, ESP_ERR_NO_MEM, err, TAG, "no mem for descriptors");
    /* alloc memory for ethernet dma buffer */
    for (int i = 0; i < CONFIG_ETH_DMA_RX_BUFFER_NUM; i++) {
        esp_dma_capable_calloc(1, CONFIG_ETH_DMA_BUFFER_SIZE, &dma_mem_info, (void*)&emac_esp_dma->rx_buf[i], NULL);
        ESP_GOTO_ON_FALSE(emac_esp_dma->rx_buf[i], ESP_ERR_NO_MEM, err, TAG, "no mem for RX DMA buffers");
    }
    for (int i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++) {
        esp_dma_capable_calloc(1, CONFIG_ETH_DMA_BUFFER_SIZE, &dma_mem_info, (void*)&emac_esp_dma->tx_buf[i], NULL);
        ESP_GOTO_ON_FALSE(emac_esp_dma->tx_buf[i], ESP_ERR_NO_MEM, err, TAG, "no mem for TX DMA buffers");
    }
    emac_hal_init(&emac_esp_dma->hal);
    *ret_handle = emac_esp_dma;
    return ESP_OK;
err:
    return ret;
}

esp_err_t emac_esp_del_dma(emac_esp_dma_handle_t emac_esp_dma)
{
    if (emac_esp_dma) {
        for (int i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++) {
            free(emac_esp_dma->tx_buf[i]);
        }
        for (int i = 0; i < CONFIG_ETH_DMA_RX_BUFFER_NUM; i++) {
            free(emac_esp_dma->rx_buf[i]);
        }
        free(emac_esp_dma->descriptors);
        free(emac_esp_dma);
    }
    return ESP_OK;
}
