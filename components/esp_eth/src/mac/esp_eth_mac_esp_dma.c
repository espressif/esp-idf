/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_cache.h"
#include "hal/emac_hal.h"
#include "esp_heap_caps.h"
#include "esp_private/eth_mac_esp_dma.h"

#define ETH_CRC_LENGTH (4)

#define EMAC_ALLOC_BUF_MAGIC_ID 0x1E1C8416

#define EMAC_TDES0_FS_CTRL_FLAGS_MASK 0x0FCC0000 // modifiable bits mask associated with the First Segment
#define EMAC_TDES0_LS_CTRL_FLAGS_MASK 0x40000000 // modifiable bits mask associated with the Last Segment

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#define DMA_CACHE_WB(addr, size) do {                                                           \
    esp_err_t msync_ret = esp_cache_msync((void *)addr, size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);    \
    assert(msync_ret == ESP_OK);                                                                \
    } while(0)
#else
#define DMA_CACHE_WB(addr, size)
#endif

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#define DMA_CACHE_INVALIDATE(addr, size) do {                                                   \
    esp_err_t msync_ret = esp_cache_msync((void *)addr, size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);    \
    assert(msync_ret == ESP_OK);                                                                \
    } while(0)
#else
#define DMA_CACHE_INVALIDATE(addr, size)
#endif

static const char *TAG = "esp.emac.dma";

struct emac_esp_dma_t {
    emac_hal_context_t hal;
    uint32_t tx_desc_flags;
    uint32_t rx_desc_flags;
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
} __attribute__((packed)) emac_esp_dma_auto_buf_info_t;

void emac_esp_dma_reset(emac_esp_dma_handle_t emac_esp_dma)
{
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
        DMA_CACHE_WB(&emac_esp_dma->rx_desc[i], EMAC_HAL_DMA_DESC_SIZE);
    }

    /* init tx chain */
    for (int i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++) {
        /* Set Own bit of the Tx descriptor Status: CPU */
        emac_esp_dma->tx_desc[i].TDES0.Own = EMAC_LL_DMADESC_OWNER_CPU;
        emac_esp_dma->tx_desc[i].TDES0.SecondAddressChained = 1;
        emac_esp_dma->tx_desc[i].TDES1.TransmitBuffer1Size = CONFIG_ETH_DMA_BUFFER_SIZE;
        /* point to the buffer */
        emac_esp_dma->tx_desc[i].Buffer1Addr = (uint32_t)(emac_esp_dma->tx_buf[i]);
        /* point to next descriptor */
        emac_esp_dma->tx_desc[i].Buffer2NextDescAddr = (uint32_t)(emac_esp_dma->tx_desc + i + 1);

        /* For last descriptor, set next descriptor address register equal to the first descriptor base address */
        if (i == CONFIG_ETH_DMA_TX_BUFFER_NUM - 1) {
            emac_esp_dma->tx_desc[i].Buffer2NextDescAddr = (uint32_t)(emac_esp_dma->tx_desc);
        }
        DMA_CACHE_WB(&emac_esp_dma->tx_desc[i], EMAC_HAL_DMA_DESC_SIZE);
    }

    /* set base address of the first descriptor */
    emac_hal_set_rx_tx_desc_addr(&emac_esp_dma->hal, emac_esp_dma->rx_desc, emac_esp_dma->tx_desc);
}

void emac_esp_dma_set_tdes0_ctrl_bits(emac_esp_dma_handle_t emac_esp_dma, uint32_t flag)
{
    emac_esp_dma->tx_desc_flags |= flag;
}

void emac_esp_dma_clear_tdes0_ctrl_bits(emac_esp_dma_handle_t emac_esp_dma, uint32_t flag)
{
    emac_esp_dma->tx_desc_flags &= ~flag;
}

uint32_t emac_esp_dma_transmit_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t *buf, uint32_t length)
{
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
        DMA_CACHE_INVALIDATE(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
        /* Check if the descriptor is owned by the Ethernet DMA (when 1) or CPU (when 0) */
        if (desc_iter->TDES0.Own != EMAC_LL_DMADESC_OWNER_CPU) {
            goto err;
        }
        /* Clear FIRST and LAST segment bits */
        desc_iter->TDES0.FirstSegment = 0;
        desc_iter->TDES0.LastSegment = 0;
        desc_iter->TDES0.Value &= ~(EMAC_TDES0_FS_CTRL_FLAGS_MASK | EMAC_TDES0_LS_CTRL_FLAGS_MASK);
        if (i == 0) {
            /* Setting the first segment bit */
            desc_iter->TDES0.FirstSegment = 1;
            desc_iter->TDES0.Value |= emac_esp_dma->tx_desc_flags & EMAC_TDES0_FS_CTRL_FLAGS_MASK;
        }
        if (i == (bufcount - 1)) {
            /* Setting the last segment bit */
            desc_iter->TDES0.LastSegment = 1;
            desc_iter->TDES0.Value |= emac_esp_dma->tx_desc_flags & EMAC_TDES0_LS_CTRL_FLAGS_MASK;
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
        DMA_CACHE_WB(desc_iter->Buffer1Addr, CONFIG_ETH_DMA_BUFFER_SIZE);
        /* Point to next descriptor */
        desc_iter = (eth_dma_tx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    }

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    for (size_t i = 0; i < bufcount; i++) {
        emac_esp_dma->tx_desc->TDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        DMA_CACHE_WB(emac_esp_dma->tx_desc, EMAC_HAL_DMA_DESC_SIZE);
        emac_esp_dma->tx_desc = (eth_dma_tx_descriptor_t *)(emac_esp_dma->tx_desc->Buffer2NextDescAddr);
    }
    emac_hal_transmit_poll_demand(&emac_esp_dma->hal);
    return sentout;
err:
    return 0;
}

uint32_t emac_esp_dma_transmit_multiple_buf_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t **buffs, uint32_t *lengths, uint32_t buffs_cnt)
{
    /* Get the number of Tx buffers to use for the frame */
    uint32_t dma_bufcount = 0;
    uint32_t sentout = 0;
    uint8_t *ptr = buffs[0];
    uint32_t lastlen = lengths[0];
    uint32_t avail_len = CONFIG_ETH_DMA_BUFFER_SIZE;

    eth_dma_tx_descriptor_t *desc_iter = emac_esp_dma->tx_desc;
    /* A frame is transmitted in multiple descriptor */
    while (dma_bufcount < CONFIG_ETH_DMA_TX_BUFFER_NUM) {
        DMA_CACHE_INVALIDATE(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
        /* Check if the descriptor is owned by the Ethernet DMA (when 1) or CPU (when 0) */
        if (desc_iter->TDES0.Own != EMAC_LL_DMADESC_OWNER_CPU) {
            goto err;
        }
        /* Clear FIRST and LAST segment bits */
        desc_iter->TDES0.FirstSegment = 0;
        desc_iter->TDES0.LastSegment = 0;
        desc_iter->TDES0.Value &= ~(EMAC_TDES0_FS_CTRL_FLAGS_MASK | EMAC_TDES0_LS_CTRL_FLAGS_MASK);
        desc_iter->TDES1.TransmitBuffer1Size = 0;
        if (dma_bufcount == 0) {
            /* Setting the first segment bit */
            desc_iter->TDES0.FirstSegment = 1;
            desc_iter->TDES0.Value |= emac_esp_dma->tx_desc_flags & EMAC_TDES0_FS_CTRL_FLAGS_MASK;
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
        DMA_CACHE_WB(desc_iter->Buffer1Addr, CONFIG_ETH_DMA_BUFFER_SIZE);
        /* Increase counter of utilized DMA buffers */
        dma_bufcount++;

        /* If all input buffers processed, mark as LAST segment and finish the coping */
        if (buffs_cnt == 0) {
            /* Setting the last segment bit */
            desc_iter->TDES0.LastSegment = 1;
            desc_iter->TDES0.Value |= emac_esp_dma->tx_desc_flags & EMAC_TDES0_LS_CTRL_FLAGS_MASK;
            break;
        }

        /* Point to next descriptor */
        desc_iter = (eth_dma_tx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    }

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    for (size_t i = 0; i < dma_bufcount; i++) {
        emac_esp_dma->tx_desc->TDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        DMA_CACHE_WB(emac_esp_dma->tx_desc, EMAC_HAL_DMA_DESC_SIZE);
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
    DMA_CACHE_INVALIDATE(desc_iter, EMAC_HAL_DMA_DESC_SIZE);

    /* Traverse descriptors owned by CPU */
    while ((desc_iter->RDES0.Own == EMAC_LL_DMADESC_OWNER_CPU) && (used_descs < CONFIG_ETH_DMA_RX_BUFFER_NUM)) {
        used_descs++;
        /* Last segment in frame */
        if (desc_iter->RDES0.LastDescriptor) {
            /* Since Store Forward must be disabled on some targets, DMA descriptors may contain erroneous frames */
            /* In addition, "Descriptor Error" (no free descriptors) may truncate a frame even if Store Forward is enabled */
            if (desc_iter->RDES0.ErrSummary) {
                emac_esp_dma_flush_recv_frame(emac_esp_dma);
                *ret_len = 0;
                return ESP_FAIL;
            }
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
        DMA_CACHE_INVALIDATE(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
    }

    return ESP_OK;
}

void emac_esp_dma_get_remain_frames(emac_esp_dma_handle_t emac_esp_dma, uint32_t *remain_frames, uint32_t *free_descs)
{
    eth_dma_rx_descriptor_t *desc_iter = emac_esp_dma->rx_desc;
    *remain_frames = 0;
    uint32_t used_descs = 0;

    DMA_CACHE_INVALIDATE(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
    /* Traverse descriptors owned by CPU */
    while ((desc_iter->RDES0.Own == EMAC_LL_DMADESC_OWNER_CPU) && (used_descs < CONFIG_ETH_DMA_RX_BUFFER_NUM)) {
        used_descs++;
        /* Last segment in frame */
        if (desc_iter->RDES0.LastDescriptor) {
            (*remain_frames)++;
        }
        /* point to next descriptor */
        desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        DMA_CACHE_INVALIDATE(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
    }
    *free_descs = CONFIG_ETH_DMA_RX_BUFFER_NUM - used_descs;
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
            buff_info->magic_id = EMAC_ALLOC_BUF_MAGIC_ID;
#endif // NDEBUG
            buff_info->copy_len = copy_len;
        }
    }
    /* indicate actual size of received frame */
    *size = ret_len;
    return buf;
}

uint32_t emac_esp_dma_receive_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t *buf, uint32_t size)
{
    uint32_t ret_len = 0;
    uint32_t copy_len = 0;

    if (size != EMAC_DMA_BUF_SIZE_AUTO) {
        if (emac_esp_dma_get_valid_recv_len(emac_esp_dma, &ret_len) != ESP_OK) {
            return 0;
        }
        /* packets larger than expected will be truncated */
        copy_len = ret_len > size ? size : ret_len;
    } else {
        emac_esp_dma_auto_buf_info_t *buff_info = (emac_esp_dma_auto_buf_info_t *)buf;
#ifndef NDEBUG
        /* check that buffer was allocated by emac_esp_dma_alloc_recv_buf */
        assert(buff_info->magic_id == EMAC_ALLOC_BUF_MAGIC_ID);
#endif // NDEBUG
        copy_len = buff_info->copy_len;
        ret_len = copy_len;
    }

    if (copy_len) {
        eth_dma_rx_descriptor_t *desc_iter = emac_esp_dma->rx_desc;
        while (copy_len > CONFIG_ETH_DMA_BUFFER_SIZE) {
            DMA_CACHE_INVALIDATE(desc_iter->Buffer1Addr, CONFIG_ETH_DMA_BUFFER_SIZE);
            memcpy(buf, (void *)(desc_iter->Buffer1Addr), CONFIG_ETH_DMA_BUFFER_SIZE);
            buf += CONFIG_ETH_DMA_BUFFER_SIZE;
            copy_len -= CONFIG_ETH_DMA_BUFFER_SIZE;
            /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
            desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
            DMA_CACHE_WB(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        }
        DMA_CACHE_INVALIDATE(desc_iter->Buffer1Addr, CONFIG_ETH_DMA_BUFFER_SIZE);
        memcpy(buf, (void *)(desc_iter->Buffer1Addr), copy_len);
        desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        DMA_CACHE_WB(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
        /* `copy_len` does not include CRC (which may be stored in separate buffer), hence check if we reached the last descriptor */
        while (!desc_iter->RDES0.LastDescriptor) {
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
            desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
            DMA_CACHE_WB(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
        }
        /* update rxdesc */
        emac_esp_dma->rx_desc = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        /* poll rx demand */
        emac_hal_receive_poll_demand(&emac_esp_dma->hal);
    }
    return ret_len;
}

void emac_esp_dma_flush_recv_frame(emac_esp_dma_handle_t emac_esp_dma)
{
    eth_dma_rx_descriptor_t *desc_iter = emac_esp_dma->rx_desc;

    DMA_CACHE_INVALIDATE(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
    /* While not last descriptor => return back to DMA */
    while (!desc_iter->RDES0.LastDescriptor) {
        desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        DMA_CACHE_WB(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
        desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    }
    /* the last descriptor */
    desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
    DMA_CACHE_WB(desc_iter, EMAC_HAL_DMA_DESC_SIZE);
    /* update rxdesc */
    emac_esp_dma->rx_desc = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    /* poll rx demand */
    emac_hal_receive_poll_demand(&emac_esp_dma->hal);
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

esp_err_t emac_esp_new_dma(const emac_esp_dma_config_t *config, emac_esp_dma_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    *ret_handle = NULL;
    struct emac_esp_dma_t *emac_esp_dma = calloc(1, sizeof(struct emac_esp_dma_t));
    ESP_GOTO_ON_FALSE(emac_esp_dma, ESP_ERR_NO_MEM, err, TAG, "no mem for esp emac_esp_dma object");

    /* alloc memory for ethernet dma descriptor */
    uint32_t desc_size = CONFIG_ETH_DMA_RX_BUFFER_NUM * sizeof(eth_dma_rx_descriptor_t) +
                         CONFIG_ETH_DMA_TX_BUFFER_NUM * sizeof(eth_dma_tx_descriptor_t);
    emac_esp_dma->descriptors = heap_caps_aligned_calloc(4, 1, desc_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(emac_esp_dma->descriptors, ESP_ERR_NO_MEM, err, TAG, "no mem for descriptors");
    /* alloc memory for ethernet dma buffer */
    for (int i = 0; i < CONFIG_ETH_DMA_RX_BUFFER_NUM; i++) {
        emac_esp_dma->rx_buf[i] = heap_caps_aligned_calloc(4, 1, CONFIG_ETH_DMA_BUFFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        ESP_GOTO_ON_FALSE(emac_esp_dma->rx_buf[i], ESP_ERR_NO_MEM, err, TAG, "no mem for RX DMA buffers");
    }
    for (int i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++) {
        emac_esp_dma->tx_buf[i] = heap_caps_aligned_calloc(4, 1, CONFIG_ETH_DMA_BUFFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        ESP_GOTO_ON_FALSE(emac_esp_dma->tx_buf[i], ESP_ERR_NO_MEM, err, TAG, "no mem for TX DMA buffers");
    }
    emac_hal_init(&emac_esp_dma->hal);
    *ret_handle = emac_esp_dma;
    return ESP_OK;
err:
    emac_esp_del_dma(emac_esp_dma);
    return ret;
}
