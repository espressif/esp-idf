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

void emac_hal_init(emac_hal_context_t *hal)
{
    hal->dma_regs = &EMAC_DMA;
    hal->mac_regs = &EMAC_MAC;
#if CONFIG_IDF_TARGET_ESP32
    hal->ext_regs = &EMAC_EXT;
#endif
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

void emac_hal_set_rx_tx_desc_addr(emac_hal_context_t *hal, eth_dma_rx_descriptor_t *rx_desc, eth_dma_tx_descriptor_t *tx_desc)
{
    emac_ll_set_rx_desc_addr(hal->dma_regs, (uint32_t)rx_desc);
    emac_ll_set_tx_desc_addr(hal->dma_regs, (uint32_t)tx_desc);
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
#if CONFIG_IDF_TARGET_ESP32P4
    /* Disable Receive Store Forward (Rx FIFO is only 256B) */
    emac_ll_recv_store_forward_enable(hal->dma_regs, false);
#else
    /* Enable Receive Store Forward */
    emac_ll_recv_store_forward_enable(hal->dma_regs, true);
#endif
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
