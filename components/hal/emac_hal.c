/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "hal/emac_hal.h"
#include "hal/emac_ll.h"
#include "hal/gpio_ll.h"
#include "hal/assert.h"

#define ETH_CRC_LENGTH (4)

#define EMAC_HAL_BUF_MAGIC_ID 0x1E1C8416

typedef struct {
#ifndef NDEBUG
    uint32_t magic_id;
#endif // NDEBUG
    uint32_t copy_len;
}__attribute__((packed)) emac_hal_auto_buf_info_t;

static esp_err_t emac_hal_flush_trans_fifo(emac_hal_context_t *hal)
{
    emac_ll_flush_trans_fifo_enable(hal->dma_regs, true);
    /* no other writes to the Operation Mode register until the flush tx fifo bit is cleared */
    for (uint32_t i = 0; i < 1000; i++) {
        if (emac_ll_get_flush_trans_fifo(hal->dma_regs) == 0) {
            return ESP_OK;
        }
    }
    return ESP_ERR_TIMEOUT;
}

void emac_hal_iomux_init_mii(void)
{
    /* TX_CLK to GPIO0 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_EMAC_TX_CLK);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[0]);
    /* TX_EN to GPIO21 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO21_U, FUNC_GPIO21_EMAC_TX_EN);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[21]);
    /* TXD0 to GPIO19 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO19_U, FUNC_GPIO19_EMAC_TXD0);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[19]);
    /* TXD1 to GPIO22 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO22_U, FUNC_GPIO22_EMAC_TXD1);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[22]);
    /* TXD2 to MTMS */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_MTMS_U, FUNC_MTMS_EMAC_TXD2);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[14]);
    /* TXD3 to MTDI */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_MTDI_U, FUNC_MTDI_EMAC_TXD3);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[12]);

    /* RX_CLK to GPIO5 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5_EMAC_RX_CLK);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[5]);
    /* RX_DV to GPIO27 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO27_U, FUNC_GPIO27_EMAC_RX_DV);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[27]);
    /* RXD0 to GPIO25 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO25_U, FUNC_GPIO25_EMAC_RXD0);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[25]);
    /* RXD1 to GPIO26 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO26_U, FUNC_GPIO26_EMAC_RXD1);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[26]);
    /* RXD2 to U0TXD */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_EMAC_RXD2);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[1]);
    /* RXD3 to MTDO */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_MTDO_U, FUNC_MTDO_EMAC_RXD3);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[15]);
}

void emac_hal_iomux_rmii_clk_input(void)
{
    /* REF_CLK(RMII mode) to GPIO0 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_EMAC_TX_CLK);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[0]);
}

void emac_hal_iomux_rmii_clk_ouput(int num)
{
    switch (num) {
    case 0:
        /* APLL clock output to GPIO0 (must be configured to 50MHz!) */
        gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[0]);
        break;
    case 16:
        /* RMII CLK (50MHz) output to GPIO16 */
        gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO16_U, FUNC_GPIO16_EMAC_CLK_OUT);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[16]);
        break;
    case 17:
        /* RMII CLK (50MHz) output to GPIO17 */
        gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO17_U, FUNC_GPIO17_EMAC_CLK_OUT_180);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[17]);
        break;
    default:
        break;
    }
}

void emac_hal_iomux_init_rmii(void)
{
    /* TX_EN to GPIO21 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO21_U, FUNC_GPIO21_EMAC_TX_EN);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[21]);
    /* TXD0 to GPIO19 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO19_U, FUNC_GPIO19_EMAC_TXD0);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[19]);
    /* TXD1 to GPIO22 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO22_U, FUNC_GPIO22_EMAC_TXD1);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[22]);

    /* CRS_DV to GPIO27 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO27_U, FUNC_GPIO27_EMAC_RX_DV);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[27]);
    /* RXD0 to GPIO25 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO25_U, FUNC_GPIO25_EMAC_RXD0);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[25]);
    /* RXD1 to GPIO26 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO26_U, FUNC_GPIO26_EMAC_RXD1);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[26]);
}

void emac_hal_iomux_init_tx_er(void)
{
    /* TX_ER to GPIO4 */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4_EMAC_TX_ER);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[4]);
}

void emac_hal_iomux_init_rx_er(void)
{
    /* RX_ER to MTCK */
    gpio_ll_iomux_func_sel(PERIPHS_IO_MUX_MTCK_U, FUNC_MTCK_EMAC_RX_ER);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[13]);
}

void emac_hal_init(emac_hal_context_t *hal, void *descriptors,
                   uint8_t **rx_buf, uint8_t **tx_buf)
{
    hal->dma_regs = &EMAC_DMA;
    hal->mac_regs = &EMAC_MAC;
    hal->ext_regs = &EMAC_EXT;
    hal->descriptors = descriptors;
    hal->rx_buf = rx_buf;
    hal->tx_buf = tx_buf;
}

void emac_hal_set_csr_clock_range(emac_hal_context_t *hal, int freq)
{
    /* Tell MAC system clock Frequency in MHz, which will determine the frequency range of MDC(1MHz~2.5MHz) */
    if (freq >= 20000000 && freq < 35000000) {
        emac_ll_set_csr_clock_division(hal->mac_regs, 2);   // CSR clock/16
    } else if (freq >= 35000000 && freq < 60000000) {
        emac_ll_set_csr_clock_division(hal->mac_regs, 3);   // CSR clock/26
    } else if (freq >= 60000000 && freq < 100000000) {
        emac_ll_set_csr_clock_division(hal->mac_regs, 0);   // CSR clock/42
    } else if (freq >= 100000000 && freq < 150000000) {
        emac_ll_set_csr_clock_division(hal->mac_regs, 1);   // CSR clock/62
    } else if (freq >= 150000000 && freq < 250000000) {
        emac_ll_set_csr_clock_division(hal->mac_regs, 4);   // CSR clock/102
    } else {
        emac_ll_set_csr_clock_division(hal->mac_regs, 5);   // CSR clock/124
    }
}

void emac_hal_reset_desc_chain(emac_hal_context_t *hal)
{
    /* reset DMA descriptors */
    hal->rx_desc = (eth_dma_rx_descriptor_t *)(hal->descriptors);
    hal->tx_desc = (eth_dma_tx_descriptor_t *)(hal->descriptors +
                   sizeof(eth_dma_rx_descriptor_t) * CONFIG_ETH_DMA_RX_BUFFER_NUM);
    /* init rx chain */
    for (int i = 0; i < CONFIG_ETH_DMA_RX_BUFFER_NUM; i++) {
        /* Set Own bit of the Rx descriptor Status: DMA */
        hal->rx_desc[i].RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        /* Set Buffer1 size and Second Address Chained bit */
        hal->rx_desc[i].RDES1.SecondAddressChained = 1;
        hal->rx_desc[i].RDES1.ReceiveBuffer1Size = CONFIG_ETH_DMA_BUFFER_SIZE;
        /* Enable Ethernet DMA Rx Descriptor interrupt */
        hal->rx_desc[i].RDES1.DisableInterruptOnComplete = 0;
        /* point to the buffer */
        hal->rx_desc[i].Buffer1Addr = (uint32_t)(hal->rx_buf[i]);
        /* point to next descriptor */
        hal->rx_desc[i].Buffer2NextDescAddr = (uint32_t)(hal->rx_desc + i + 1);
    }
    /* For last descriptor, set next descriptor address register equal to the first descriptor base address */
    hal->rx_desc[CONFIG_ETH_DMA_RX_BUFFER_NUM - 1].Buffer2NextDescAddr = (uint32_t)(hal->rx_desc);

    /* init tx chain */
    for (int i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++) {
        /* Set Own bit of the Tx descriptor Status: CPU */
        hal->tx_desc[i].TDES0.Own = EMAC_LL_DMADESC_OWNER_CPU;
        hal->tx_desc[i].TDES0.SecondAddressChained = 1;
        hal->tx_desc[i].TDES1.TransmitBuffer1Size = CONFIG_ETH_DMA_BUFFER_SIZE;
        /* Enable Ethernet DMA Tx Descriptor interrupt */
        hal->tx_desc[1].TDES0.InterruptOnComplete = 1;
        /* Enable Transmit Timestamp */
        hal->tx_desc[i].TDES0.TransmitTimestampEnable = 1;
        /* point to the buffer */
        hal->tx_desc[i].Buffer1Addr = (uint32_t)(hal->tx_buf[i]);
        /* point to next descriptor */
        hal->tx_desc[i].Buffer2NextDescAddr = (uint32_t)(hal->tx_desc + i + 1);
    }
    /* For last descriptor, set next descriptor address register equal to the first descriptor base address */
    hal->tx_desc[CONFIG_ETH_DMA_TX_BUFFER_NUM - 1].Buffer2NextDescAddr = (uint32_t)(hal->tx_desc);

    /* set base address of the first descriptor */
    emac_ll_set_rx_desc_addr(hal->dma_regs, (uint32_t)hal->rx_desc);
    emac_ll_set_tx_desc_addr(hal->dma_regs, (uint32_t)hal->tx_desc);
}

void emac_hal_init_mac_default(emac_hal_context_t *hal)
{
    /* MACCR Configuration */
    /* Enable the watchdog on the receiver, frame longer than 2048 Bytes is not allowed */
    emac_ll_watchdog_enable(hal->mac_regs, true);
    /* Enable the jabber timer on the transmitter, frame longer than 2048 Bytes is not allowed */
    emac_ll_jabber_enable(hal->mac_regs, true);
    /* minimum IFG between frames during transmission is 96 bit times */
    emac_ll_set_inter_frame_gap(hal->mac_regs, EMAC_LL_INTERFRAME_GAP_96BIT);
    /* Enable Carrier Sense During Transmission */
    emac_ll_carrier_sense_enable(hal->mac_regs, true);
    /* Select speed: port: 10/100 Mbps, here set default 100M, afterwards, will reset by auto-negotiation */
    emac_ll_set_port_speed(hal->mac_regs, ETH_SPEED_100M);
    /* Allow the reception of frames when the TX_EN signal is asserted in Half-Duplex mode */
    emac_ll_recv_own_enable(hal->mac_regs, true);
    /* Disable internal loopback mode */
    emac_ll_loopback_enable(hal->mac_regs, false);
    /* Select duplex mode: here set default full duplex, afterwards, will reset by auto-negotiation */
    emac_ll_set_duplex(hal->mac_regs, ETH_DUPLEX_FULL);
    /* Select the checksum mode for received frame payload's TCP/UDP/ICMP headers */
    emac_ll_checksum_offload_mode(hal->mac_regs, ETH_CHECKSUM_HW);
    /* Enable MAC retry transmission when a colision occurs in half duplex mode */
    emac_ll_retry_enable(hal->mac_regs, true);
    /* MAC passes all incoming frames to host, without modifying them */
    emac_ll_auto_pad_crc_strip_enable(hal->mac_regs, false);
    /* Set Back-Off limit time before retry a transmittion after a collision */
    emac_ll_set_back_off_limit(hal->mac_regs, EMAC_LL_BACKOFF_LIMIT_10);
    /* Disable deferral check, MAC defers until the CRS signal goes inactive */
    emac_ll_deferral_check_enable(hal->mac_regs, false);
    /* Set preamble length 7 Bytes */
    emac_ll_set_preamble_length(hal->mac_regs, EMAC_LL_PREAMBLE_LENGTH_7);

    /* MACFFR Configuration */
    /* Receiver module passes only those frames to the Application that pass the SA or DA address filter */
    emac_ll_receive_all_enable(hal->mac_regs, false);
    /* Disable source address filter */
    emac_ll_set_src_addr_filter(hal->mac_regs, EMAC_LL_SOURCE_ADDR_FILTER_DISABLE);
    emac_ll_sa_inverse_filter_enable(hal->mac_regs, false);
    /* MAC blocks all control frames */
    emac_ll_set_pass_ctrl_frame_mode(hal->mac_regs, EMAC_LL_CONTROL_FRAME_BLOCKALL);
    /* AFM module passes all received broadcast frames and multicast frames */
    emac_ll_broadcast_frame_enable(hal->mac_regs, true);
    emac_ll_pass_all_multicast_enable(hal->mac_regs, true);
    /* Address Check block operates in normal filtering mode for the DA address */
    emac_ll_da_inverse_filter_enable(hal->mac_regs, false);
    /* Disable Promiscuous Mode */
    emac_ll_promiscuous_mode_enable(hal->mac_regs, false);
}

void emac_hal_enable_flow_ctrl(emac_hal_context_t *hal, bool enable)
{
    /* MACFCR Configuration */
    if (enable) {
        /* Pause time */
        emac_ll_set_pause_time(hal->mac_regs, EMAC_LL_PAUSE_TIME);
        /* Enable generation of Zero-Quanta Pause Control frames */
        emac_ll_zero_quanta_pause_enable(hal->mac_regs, true);
        /* Threshold of the PAUSE to be checked for automatic retransmission of PAUSE Frame */
        emac_ll_set_pause_low_threshold(hal->mac_regs, EMAC_LL_PAUSE_LOW_THRESHOLD_MINUS_28);
        /* Don't allow MAC detect Pause frames with MAC address0 unicast address and unique multicast address */
        emac_ll_unicast_pause_frame_detect_enable(hal->mac_regs, false);
        /* Enable MAC to decode the received Pause frame and disable its transmitter for a specific time */
        emac_ll_receive_flow_ctrl_enable(hal->mac_regs, true);
        /* Enable MAC to transmit Pause frames in full duplex mode or the MAC back-pressure operation in half duplex mode */
        emac_ll_transmit_flow_ctrl_enable(hal->mac_regs, true);
    } else {
        emac_ll_clear(hal->mac_regs);
    }
}

void emac_hal_init_dma_default(emac_hal_context_t *hal, emac_hal_dma_config_t *hal_config)
{
    /* DMAOMR Configuration */
    /* Enable Dropping of TCP/IP Checksum Error Frames */
    emac_ll_drop_tcp_err_frame_enable(hal->dma_regs, true);
    /* Enable Receive Store Forward */
    emac_ll_recv_store_forward_enable(hal->dma_regs, true);
    /* Enable Flushing of Received Frames because of the unavailability of receive descriptors or buffers */
    emac_ll_flush_recv_frame_enable(hal->dma_regs, true);
    /* Disable Transmit Store Forward */
    emac_ll_trans_store_forward_enable(hal->dma_regs, false);
    /* Flush Transmit FIFO */
    emac_hal_flush_trans_fifo(hal);
    /* Transmit Threshold Control */
    emac_ll_set_transmit_threshold(hal->dma_regs, EMAC_LL_TRANSMIT_THRESHOLD_CONTROL_64);
    /* Disable Forward Error Frame */
    emac_ll_forward_err_frame_enable(hal->dma_regs, false);
    /* Disable forward undersized good frame */
    emac_ll_forward_undersized_good_frame_enable(hal->dma_regs, false);
    /* Receive Threshold Control */
    emac_ll_set_recv_threshold(hal->dma_regs, EMAC_LL_RECEIVE_THRESHOLD_CONTROL_64);
    /* Allow the DMA to process a second frame of Transmit data even before obtaining the status for the first frame */
    emac_ll_opt_second_frame_enable(hal->dma_regs, true);

    /* DMABMR Configuration */
    /* Enable Mixed Burst */
    emac_ll_mixed_burst_enable(hal->dma_regs, true);
    /* Enable Address Aligned Beates */
    emac_ll_addr_align_enable(hal->dma_regs, true);
    /* Don't use Separate PBL */
    emac_ll_use_separate_pbl_enable(hal->dma_regs, false);
    /* Set Rx/Tx DMA Burst Length */
    emac_ll_set_prog_burst_len(hal->dma_regs, hal_config->dma_burst_len);
    /* Enable Enhanced Descriptor,8 Words(32 Bytes) */
    emac_ll_enhance_desc_enable(hal->dma_regs, true);
    /* Specifies the number of word to skip between two unchained descriptors (Ring mode) */
    emac_ll_set_desc_skip_len(hal->dma_regs, 0);
    /* DMA Arbitration Scheme */
    emac_ll_fixed_arbitration_enable(hal->dma_regs, false);
    /* Set priority ratio in the weighted round-robin arbitration between Rx DMA and Tx DMA */
    emac_ll_set_priority_ratio(hal->dma_regs, EMAC_LL_DMA_ARBITRATION_ROUNDROBIN_RXTX_1_1);
}

void emac_hal_set_phy_cmd(emac_hal_context_t *hal, uint32_t phy_addr, uint32_t phy_reg, bool write)
{
    /* Write the result value into the MII Address register */
    emac_ll_set_phy_addr(hal->mac_regs, phy_addr);
    /* Set the PHY register address */
    emac_ll_set_phy_reg(hal->mac_regs, phy_reg);
    /* Set as write mode */
    emac_ll_write_enable(hal->mac_regs, write);
    /* Set MII busy bit */
    emac_ll_set_busy(hal->mac_regs, true);

}

void emac_hal_set_address(emac_hal_context_t *hal, uint8_t *mac_addr)
{
    /* Make sure mac address is unicast type */
    if (!(mac_addr[0] & 0x01)) {
        emac_ll_set_addr(hal->mac_regs, mac_addr);
    }
}

void emac_hal_start(emac_hal_context_t *hal)
{
    /* Enable Ethernet MAC and DMA Interrupt */
    emac_ll_enable_corresponding_intr(hal->dma_regs, EMAC_LL_CONFIG_ENABLE_INTR_MASK);
    /* Clear all pending interrupts */
    emac_ll_clear_all_pending_intr(hal->dma_regs);

    /* Enable transmit state machine of the MAC for transmission on the MII */
    emac_ll_transmit_enable(hal->mac_regs, true);
    /* Start DMA transmission */
    /* Note that the EMAC Databook states the DMA could be started prior enabling
       the MAC transmitter. However, it turned out that such order may cause the MAC
       transmitter hangs */
    emac_ll_start_stop_dma_transmit(hal->dma_regs, true);

    /* Start DMA reception */
    emac_ll_start_stop_dma_receive(hal->dma_regs, true);
    /* Enable receive state machine of the MAC for reception from the MII */
    emac_ll_receive_enable(hal->mac_regs, true);
}

esp_err_t emac_hal_stop(emac_hal_context_t *hal)
{
    /* Stop DMA transmission */
    emac_ll_start_stop_dma_transmit(hal->dma_regs, false);

    if (emac_ll_transmit_frame_ctrl_status(hal->mac_regs) != 0x0) {
        /* Previous transmit in progress */
        return ESP_ERR_INVALID_STATE;
    }

    /* Disable transmit state machine of the MAC for transmission on the MII */
    emac_ll_receive_enable(hal->mac_regs, false);
    /* Disable receive state machine of the MAC for reception from the MII */
    emac_ll_transmit_enable(hal->mac_regs, false);

    if (emac_ll_receive_read_ctrl_state(hal->mac_regs) != 0x0) {
        /* Previous receive copy in progress */
        return ESP_ERR_INVALID_STATE;
    }

    /* Stop DMA reception */
    emac_ll_start_stop_dma_receive(hal->dma_regs, false);

    /* Flush Transmit FIFO */
    emac_hal_flush_trans_fifo(hal);

    /* Disable Ethernet MAC and DMA Interrupt */
    emac_ll_disable_all_intr(hal->dma_regs);

    return ESP_OK;
}

uint32_t emac_hal_transmit_frame(emac_hal_context_t *hal, uint8_t *buf, uint32_t length)
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

    eth_dma_tx_descriptor_t *desc_iter = hal->tx_desc;
    /* A frame is transmitted in multiple descriptor */
    for (size_t i = 0; i < bufcount; i++) {
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
        /* Point to next descriptor */
        desc_iter = (eth_dma_tx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    }

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    for (size_t i = 0; i < bufcount; i++) {
        hal->tx_desc->TDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        hal->tx_desc = (eth_dma_tx_descriptor_t *)(hal->tx_desc->Buffer2NextDescAddr);
    }
    emac_ll_transmit_poll_demand(hal->dma_regs, 0);
    return sentout;
err:
    return 0;
}

uint32_t emac_hal_transmit_multiple_buf_frame(emac_hal_context_t *hal, uint8_t **buffs, uint32_t *lengths, uint32_t buffs_cnt)
{
    /* Get the number of Tx buffers to use for the frame */
    uint32_t dma_bufcount = 0;
    uint32_t sentout = 0;
    uint8_t *ptr = buffs[0];
    uint32_t lastlen = lengths[0];
    uint32_t avail_len = CONFIG_ETH_DMA_BUFFER_SIZE;

    eth_dma_tx_descriptor_t *desc_iter = hal->tx_desc;
    /* A frame is transmitted in multiple descriptor */
    while (dma_bufcount < CONFIG_ETH_DMA_TX_BUFFER_NUM) {
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
        hal->tx_desc->TDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        hal->tx_desc = (eth_dma_tx_descriptor_t *)(hal->tx_desc->Buffer2NextDescAddr);
    }
    emac_ll_transmit_poll_demand(hal->dma_regs, 0);
    return sentout;
err:
    return 0;
}

uint8_t *emac_hal_alloc_recv_buf(emac_hal_context_t *hal, uint32_t *size)
{
    eth_dma_rx_descriptor_t *desc_iter = hal->rx_desc;
    uint32_t used_descs = 0;
    uint32_t ret_len = 0;
    uint32_t copy_len = 0;
    uint8_t *buf = NULL;

    /* Traverse descriptors owned by CPU */
    while ((desc_iter->RDES0.Own != EMAC_LL_DMADESC_OWNER_DMA) && (used_descs < CONFIG_ETH_DMA_RX_BUFFER_NUM)) {
        used_descs++;
        /* Last segment in frame */
        if (desc_iter->RDES0.LastDescriptor) {
            /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
            ret_len = desc_iter->RDES0.FrameLength - ETH_CRC_LENGTH;
            /* packets larger than expected will be truncated */
            copy_len = ret_len > *size ? *size : ret_len;
            break;
        }
        /* point to next descriptor */
        desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    }
    if (copy_len > 0) {
        buf = malloc(copy_len);
        if (buf != NULL) {
            emac_hal_auto_buf_info_t *buff_info = (emac_hal_auto_buf_info_t *)buf;
            /* no need to check allocated buffer min lenght prior writing since we know that EMAC DMA is configured to
            not forward erroneous or undersized frames (less than 64B), see emac_hal_init_dma_default */
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

uint32_t emac_hal_receive_frame(emac_hal_context_t *hal, uint8_t *buf, uint32_t size, uint32_t *frames_remain, uint32_t *free_desc)
{
    eth_dma_rx_descriptor_t *desc_iter = hal->rx_desc;
    eth_dma_rx_descriptor_t *first_desc = hal->rx_desc;
    uint32_t used_descs = 0;
    uint32_t ret_len = 0;
    uint32_t copy_len = 0;
    uint32_t frame_count = 0;

    if (size != EMAC_HAL_BUF_SIZE_AUTO) {
        /* Traverse descriptors owned by CPU */
        while ((desc_iter->RDES0.Own != EMAC_LL_DMADESC_OWNER_DMA) && (used_descs < CONFIG_ETH_DMA_RX_BUFFER_NUM) && !frame_count) {
            used_descs++;
            /* Last segment in frame */
            if (desc_iter->RDES0.LastDescriptor) {
                /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
                ret_len = desc_iter->RDES0.FrameLength - ETH_CRC_LENGTH;
                /* packets larger than expected will be truncated */
                copy_len = ret_len > size ? size : ret_len;
                /* update unhandled frame count */
                frame_count++;
            }
            /* First segment in frame */
            if (desc_iter->RDES0.FirstDescriptor) {
                first_desc = desc_iter;
            }
            /* point to next descriptor */
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        }
    } else {
        emac_hal_auto_buf_info_t *buff_info = (emac_hal_auto_buf_info_t *)buf;
#ifndef NDEBUG
        /* check that buffer was allocated by emac_hal_alloc_recv_buf */
        HAL_ASSERT(buff_info->magic_id == EMAC_HAL_BUF_MAGIC_ID);
#endif // NDEBUG
        copy_len = buff_info->copy_len;
        ret_len = copy_len;
    }

    if (copy_len) {
        /* check how many frames left to handle */
        while ((desc_iter->RDES0.Own != EMAC_LL_DMADESC_OWNER_DMA) && (used_descs < CONFIG_ETH_DMA_RX_BUFFER_NUM)) {
            used_descs++;
            if (desc_iter->RDES0.LastDescriptor) {
                frame_count++;
            }
            /* point to next descriptor */
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        }
        desc_iter = first_desc;
        while(copy_len > CONFIG_ETH_DMA_BUFFER_SIZE) {
            used_descs--;
            memcpy(buf, (void *)(desc_iter->Buffer1Addr), CONFIG_ETH_DMA_BUFFER_SIZE);
            buf += CONFIG_ETH_DMA_BUFFER_SIZE;
            copy_len -= CONFIG_ETH_DMA_BUFFER_SIZE;
            /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
            desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        }
        memcpy(buf, (void *)(desc_iter->Buffer1Addr), copy_len);
        desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        used_descs--;
        /* `copy_len` does not include CRC, hence check if we reached the last descriptor */
        while (!desc_iter->RDES0.LastDescriptor) {
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
            desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
            used_descs--;
        }
        /* update rxdesc */
        hal->rx_desc = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        /* poll rx demand */
        emac_ll_receive_poll_demand(hal->dma_regs, 0);
        frame_count--;
    }
    *frames_remain = frame_count;
    *free_desc = CONFIG_ETH_DMA_RX_BUFFER_NUM - used_descs;
    return ret_len;
}

uint32_t emac_hal_flush_recv_frame(emac_hal_context_t *hal, uint32_t *frames_remain, uint32_t *free_desc)
{
    eth_dma_rx_descriptor_t *desc_iter = hal->rx_desc;
    eth_dma_rx_descriptor_t *first_desc = hal->rx_desc;
    uint32_t used_descs = 0;
    uint32_t frame_len = 0;
    uint32_t frame_count = 0;

    /* Traverse descriptors owned by CPU */
    while ((desc_iter->RDES0.Own != EMAC_LL_DMADESC_OWNER_DMA) && (used_descs < CONFIG_ETH_DMA_RX_BUFFER_NUM) && !frame_count) {
        used_descs++;
        /* Last segment in frame */
        if (desc_iter->RDES0.LastDescriptor) {
            /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
            frame_len = desc_iter->RDES0.FrameLength - ETH_CRC_LENGTH;
            /* update unhandled frame count */
            frame_count++;
        }
        /* First segment in frame */
        if (desc_iter->RDES0.FirstDescriptor) {
            first_desc = desc_iter;
        }
        /* point to next descriptor */
        desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
    }

    /* if there is at least one frame waiting */
    if (frame_len) {
        /* check how many frames left to handle */
        while ((desc_iter->RDES0.Own != EMAC_LL_DMADESC_OWNER_DMA) && (used_descs < CONFIG_ETH_DMA_RX_BUFFER_NUM)) {
            used_descs++;
            if (desc_iter->RDES0.LastDescriptor) {
                frame_count++;
            }
            /* point to next descriptor */
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        }
        desc_iter = first_desc;
        /* return descriptors to DMA */
        while (!desc_iter->RDES0.LastDescriptor) {
            desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
            used_descs--;
        }
        desc_iter->RDES0.Own = EMAC_LL_DMADESC_OWNER_DMA;
        used_descs--;
        /* update rxdesc */
        hal->rx_desc = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        /* poll rx demand */
        emac_ll_receive_poll_demand(hal->dma_regs, 0);
        frame_count--;
    }
    *frames_remain = frame_count;
    *free_desc = CONFIG_ETH_DMA_RX_BUFFER_NUM - used_descs;
    return frame_len;
}
