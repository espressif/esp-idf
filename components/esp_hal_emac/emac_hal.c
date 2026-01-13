/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "hal/emac_hal.h"
#include "hal/emac_ll.h"
#if SOC_EMAC_IEEE1588V2_SUPPORTED
#include "esp_rom_sys.h"
#define EMAC_PTP_INIT_TIMEOUT_US (10)
#endif // SOC_EMAC_IEEE1588V2_SUPPORTED

static uint8_t emac_crs_div_table[] = {
    42,
    62,
    16,
    26,
    102,
    124,
};

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
#if SOC_IS(ESP32)
    hal->ext_regs = &EMAC_EXT;
#else
    hal->ext_regs = NULL;
#endif
#if SOC_EMAC_IEEE1588V2_SUPPORTED
    hal->ptp_regs = &EMAC_PTP;
#endif
}

void emac_hal_find_set_closest_csr_clock_range(emac_hal_context_t *hal, int mdc_freq_hz, int freq_hz)
{
    int min_diff = abs(freq_hz / emac_crs_div_table[0] - mdc_freq_hz);
    uint32_t best_div = 0;

    for (int i = 1; i < sizeof(emac_crs_div_table) / sizeof(emac_crs_div_table[0]); i++) {
        int cur_diff = abs(freq_hz / emac_crs_div_table[i] - mdc_freq_hz);
        if (cur_diff < min_diff) {
            min_diff = cur_diff;
            best_div = i;
        }
    }
    emac_ll_set_csr_clock_division(hal->mac_regs, best_div);
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
    /* EMACINTMASK */
    /* Disable (mask) all interrupts */
    emac_ll_disable_corresponding_emac_intr(hal->mac_regs, 0xFFFFFFFF);

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
    /* Enable MAC retry transmission when a collision occurs in half duplex mode */
    emac_ll_retry_enable(hal->mac_regs, true);
    /* MAC passes all incoming frames to host, without modifying them */
    emac_ll_auto_pad_crc_strip_enable(hal->mac_regs, false);
    /* Set Back-Off limit time before retry a transmission after a collision */
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
    /* AFM module passes all received broadcast frames */
    emac_ll_broadcast_frame_enable(hal->mac_regs, true);
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
#if SOC_IS(ESP32P4)
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

esp_err_t emac_hal_add_addr_da_filter(emac_hal_context_t *hal, const uint8_t *mac_addr, uint8_t addr_num)
{
    if (addr_num < 1 || addr_num > EMAC_LL_MAX_MAC_ADDR_NUM || mac_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    emac_ll_add_addr_filter(hal->mac_regs, addr_num, mac_addr, 0, false);
    return ESP_OK;
}

esp_err_t emac_hal_get_addr_da_filter(emac_hal_context_t *hal, uint8_t *mac_addr, uint8_t addr_num)
{
    if (addr_num < 1 || addr_num > EMAC_LL_MAX_MAC_ADDR_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    bool is_source = false;
    if ((emac_ll_get_addr_filter(hal->mac_regs, addr_num, mac_addr, NULL, &is_source) != true) || is_source) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t emac_hal_add_addr_da_filter_auto(emac_hal_context_t *hal, uint8_t *mac_addr)
{
    for (uint8_t i = 1; i <= EMAC_LL_MAX_MAC_ADDR_NUM; i++) {
        // find the first free address filter
        if (emac_ll_get_addr_filter(hal->mac_regs, i, NULL, NULL, NULL) == false) {
            emac_hal_add_addr_da_filter(hal, mac_addr, i);
            return ESP_OK;
        }
    }
    return ESP_FAIL;
}

esp_err_t emac_hal_rm_addr_da_filter(emac_hal_context_t *hal, const uint8_t *mac_addr, uint8_t addr_num)
{
    esp_err_t ret = ESP_OK;
    if (mac_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t addr_got[6];
    if ((ret = emac_hal_get_addr_da_filter(hal, addr_got, addr_num)) == ESP_OK) {
        if (memcmp(addr_got, mac_addr, 6) == 0) {
            emac_ll_rm_addr_filter(hal->mac_regs, addr_num);
        } else {
            ret = ESP_ERR_NOT_FOUND;
        }
    }
    return ret;
}

esp_err_t emac_hal_rm_addr_da_filter_auto(emac_hal_context_t *hal, const uint8_t *mac_addr)
{
    if (mac_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t addr_got[6];
    for (uint8_t i = 1; i <= EMAC_LL_MAX_MAC_ADDR_NUM; i++) {
        if (emac_hal_get_addr_da_filter(hal, addr_got, i) == ESP_OK) {
            if (memcmp(addr_got, mac_addr, 6) == 0) {
                emac_ll_rm_addr_filter(hal->mac_regs, i);
                return ESP_OK;
            }
        }
    }
    return ESP_ERR_NOT_FOUND;
}

void emac_hal_clear_addr_da_filters(emac_hal_context_t *hal)
{
    for (uint8_t i = 1; i <= EMAC_LL_MAX_MAC_ADDR_NUM; i++) {
        if (emac_hal_get_addr_da_filter(hal, NULL, i) == ESP_OK) {
            emac_ll_rm_addr_filter(hal->mac_regs, i);
        }
    }
}

#if SOC_EMAC_IEEE1588V2_SUPPORTED
static inline uint32_t subsecond2nanosecond(emac_hal_context_t *hal, uint32_t subsecond)
{
    if (emac_ll_is_ts_digital_roll_set(hal->ptp_regs)) {
        return subsecond;
    }
    uint64_t val = subsecond * 1000000000ll; // 1 s = 10e9 ns
    val >>= 31; // Sub-Second register is 31 bit
    return (uint32_t)val;
}

static inline uint32_t nanosecond2subsecond(emac_hal_context_t *hal, uint32_t nanosecond)
{
    if (emac_ll_is_ts_digital_roll_set(hal->ptp_regs)) {
        return nanosecond;
    }
    uint64_t val = (uint64_t)nanosecond << 31;
    val /= 1000000000ll;
    return (uint32_t)val;
}

esp_err_t emac_hal_get_rxdesc_timestamp(emac_hal_context_t *hal, eth_dma_rx_descriptor_t *rxdesc, uint32_t *seconds, uint32_t *nano_seconds)
{
    if (!rxdesc->RDES0.TSAvailIPChecksumErrGiantFrame) {
        return ESP_ERR_INVALID_STATE;
    }

    if (seconds) {
        *seconds = rxdesc->TimeStampHigh;
    }
    if (nano_seconds) {
        *nano_seconds = subsecond2nanosecond(hal, rxdesc->TimeStampLow);
    }
    rxdesc->RDES0.TSAvailIPChecksumErrGiantFrame = 0;
    return ESP_OK;
}

esp_err_t emac_hal_get_txdesc_timestamp(emac_hal_context_t *hal, eth_dma_tx_descriptor_t *txdesc, uint32_t *seconds, uint32_t *nano_seconds)
{
    if (txdesc->TDES0.Own == EMAC_LL_DMADESC_OWNER_DMA || !txdesc->TDES0.TxTimestampStatus) {
        return ESP_ERR_INVALID_STATE;
    }
    if (seconds) {
        *seconds = txdesc->TimeStampHigh;
    }
    if (nano_seconds) {
        *nano_seconds = subsecond2nanosecond(hal, txdesc->TimeStampLow);
    }
    txdesc->TDES0.TxTimestampStatus = 0;
    return ESP_OK;
}

esp_err_t emac_hal_ptp_start(emac_hal_context_t *hal, const emac_hal_ptp_config_t *config)
{
    uint8_t base_increment;

    // Enable time stamping frame filtering (applicable to receive)
    emac_ll_ts_ptp_ether_enable(hal->ptp_regs, true);
    // Process frames with v2 format
    emac_ll_ptp_v2_proc_enable(hal->ptp_regs, true);
    // Enable time stamping frame filtering
    emac_ll_ts_mac_addr_filter_enable(hal->ptp_regs, true);
    // Process also Pdelay messages
    emac_ll_ts_ptp_snap_type_sel(hal->ptp_regs, 1);

    /* Un-mask the Time stamp trigger interrupt */
    emac_ll_enable_corresponding_emac_intr(hal->mac_regs, EMAC_LL_CONFIG_ENABLE_MAC_INTR_MASK);

    /* Enable the timestamp feature */
    emac_ll_ts_enable(hal->ptp_regs, true);
    /* Set digital or binary rollover */
    if (config->roll == ETH_PTP_DIGITAL_ROLLOVER) {
        emac_ll_ts_digital_roll_enable(hal->ptp_regs, true);
    } else {
        emac_ll_ts_digital_roll_enable(hal->ptp_regs, false);
    }
    /* Set sub second increment based on the required PTP accuracy */
    if (emac_ll_is_ts_digital_roll_set(hal->ptp_regs)) {
        /**
         *   tick(ns)         10^9
         * ———————————— = ————————————— ==> Increment = tick
         *   Increment        10^9
         */
        base_increment = config->ptp_req_accuracy_ns;
    } else {
        /**
         *   tick(ns)         10^9                       tick * 2^31      tick
         * ———————————— = ————————————— ==> Increment = ————————————— ≈ —————————
         *   Increment        2^31                           10^9         0.465
         */
        base_increment = config->ptp_req_accuracy_ns / 0.465;
    }
    emac_ll_set_ts_sub_second_incre_val(hal->ptp_regs, base_increment);
    /* Set Update Mode */
    emac_ll_set_ts_update_method(hal->ptp_regs, config->upd_method);
    int32_t to = 0;
    /* If you are using the Fine correction method */
    if (config->upd_method == ETH_PTP_UPDATE_METHOD_FINE) {
        /**
         *           2^32                 2^32                      TsysClk(ns)
         * Addend = ——————— = —————————————————————————— = 2^32 * ——————————————
         *           ratio     SysClk(MHz)/PTPaccur(MHz)            Taccur(ns)
         */
        uint32_t base_addend = (1ll << 32) * config->ptp_clk_src_period_ns / config->ptp_req_accuracy_ns;
        emac_ll_set_ts_addend_val(hal->ptp_regs, base_addend);
        emac_ll_ts_addend_do_update(hal->ptp_regs);
        while (!emac_ll_is_ts_addend_update_done(hal->ptp_regs) && to < EMAC_PTP_INIT_TIMEOUT_US) {
            esp_rom_delay_us(1);
            to++;
        }
        if (to >= EMAC_PTP_INIT_TIMEOUT_US) {
            return ESP_ERR_TIMEOUT;
        }
    }
    /* Initialize timestamp */
    emac_ll_set_ts_update_second_val(hal->ptp_regs, 0);
    emac_ll_set_ts_update_sub_second_val(hal->ptp_regs, 0);
    emac_ll_ts_init_do(hal->ptp_regs);
    to = 0;
    while (!emac_ll_is_ts_init_done(hal->ptp_regs) && to < EMAC_PTP_INIT_TIMEOUT_US) {
        esp_rom_delay_us(1);
        to++;
    }
    if (to >= EMAC_PTP_INIT_TIMEOUT_US) {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

esp_err_t emac_hal_ptp_stop(emac_hal_context_t *hal)
{
    /* Disable the timestamp feature */
    emac_ll_ts_enable(hal->ptp_regs, false);
    return ESP_OK;
}

esp_err_t emac_hal_ptp_adj_inc(emac_hal_context_t *hal, int32_t adj_ppb)
{
    if (emac_ll_get_ts_update_method(hal->ptp_regs) != ETH_PTP_UPDATE_METHOD_FINE ||
            !emac_ll_is_ts_addend_update_done(hal->ptp_regs)) {
        return ESP_ERR_INVALID_STATE;
    }
    /**
     *       Sysclk(MHz) * ppb    Sysclk * ppb
     * var = ————————————————— = ———————————————
     *            10^9                10^9
     *
     *          2^32 * PTPClk(MHz)                    2^32 * PTPClk(MHz)
     * old = —————————————————————————  =>  SysClk = ——————————————————————
     *             SysClk(MHz)                             old
     *
     *        2^32 * PTPClk(MHz)        2^32 * PTPClk(MHz)           2^32 * PTPClk(MHz)
     * new = ———————————————————— = —————————————————————————— = ———————————————————————————————————— =
     *        SysClk(MHz) - var                Sysclk * ppb       2^32 * PTPClk(MHz)    (     ppb  )
     *                               SysClk - ———————————————    ———————————————————— - (1 - ——————)
     *                                             10^9                old              (     10^9 )
     *
     *      old           old * 10^9
     * = ————————————— = —————————————
     *        ppb         10^9 - ppb
     *   1 - ——————
     *        10^9
     */
    static uint32_t addend_base = 0;
    if (addend_base == 0) {
        addend_base = emac_ll_get_ts_addend_val(hal->ptp_regs);
    }

    if (adj_ppb > 5120000) {
        adj_ppb = 5120000;
    }
    if (adj_ppb < -5120000) {
        adj_ppb = -5120000;
    }
    /* calculate the rate by which you want to speed up or slow down the system time increments */
    int64_t addend_new = (int64_t)addend_base * 1000000000ll;
    addend_new /= 1000000000ll - adj_ppb;

    emac_ll_set_ts_addend_val(hal->ptp_regs, addend_new);
    emac_ll_ts_addend_do_update(hal->ptp_regs);

    return ESP_OK;
}

esp_err_t emac_hal_adj_freq_factor(emac_hal_context_t *hal, double scale_factor)
{
    if (emac_ll_get_ts_update_method(hal->ptp_regs) != ETH_PTP_UPDATE_METHOD_FINE ||
            !emac_ll_is_ts_addend_update_done(hal->ptp_regs)) {
        return ESP_ERR_INVALID_STATE;
    }

    uint32_t addend_new = (emac_ll_get_ts_addend_val(hal->ptp_regs) * scale_factor);
    emac_ll_set_ts_addend_val(hal->ptp_regs, addend_new);
    emac_ll_ts_addend_do_update(hal->ptp_regs);

    return ESP_OK;
}

esp_err_t emac_hal_ptp_time_add(emac_hal_context_t *hal, uint32_t off_sec, uint32_t off_nsec, bool sign)
{
    emac_ll_set_ts_update_second_val(hal->ptp_regs, off_sec);
    emac_ll_set_ts_update_sub_second_val(hal->ptp_regs, nanosecond2subsecond(hal, off_nsec));
    if (sign) {
        emac_ll_ts_update_time_add(hal->ptp_regs);
    } else {
        emac_ll_ts_update_time_sub(hal->ptp_regs);
    }
    if (!emac_ll_is_ts_update_time_done(hal->ptp_regs)) {
        return ESP_ERR_INVALID_STATE;
    }
    emac_ll_ts_update_time_do(hal->ptp_regs);
    return ESP_OK;
}

esp_err_t emac_hal_ptp_set_sys_time(emac_hal_context_t *hal, uint32_t seconds, uint32_t nano_seconds)
{
    emac_ll_set_ts_update_second_val(hal->ptp_regs, seconds);
    emac_ll_set_ts_update_sub_second_val(hal->ptp_regs, nanosecond2subsecond(hal, nano_seconds));

    if (!emac_ll_is_ts_init_done(hal->ptp_regs)) {
        return ESP_ERR_INVALID_STATE;
    }
    emac_ll_ts_init_do(hal->ptp_regs);
    return ESP_OK;
}

esp_err_t emac_hal_ptp_get_sys_time(emac_hal_context_t *hal, uint32_t *seconds, uint32_t *nano_seconds)
{
    if (seconds == NULL || nano_seconds == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *seconds = emac_ll_get_ts_seconds_val(hal->ptp_regs);
    *nano_seconds = subsecond2nanosecond(hal, emac_ll_get_ts_sub_seconds_val(hal->ptp_regs));
    return ESP_OK;
}

esp_err_t emac_hal_ptp_set_target_time(emac_hal_context_t *hal, uint32_t seconds, uint32_t nano_seconds)
{
    emac_ll_set_ts_target_second_val(hal->ptp_regs, seconds);
    emac_ll_set_ts_target_sub_second_val(hal->ptp_regs, nanosecond2subsecond(hal, nano_seconds));
    /*  Enable the PTP Time Stamp interrupt trigger */
    emac_ll_ts_target_int_trig_enable(hal->ptp_regs);
    return ESP_OK;
}

esp_err_t emac_hal_ptp_enable_ts4all(emac_hal_context_t *hal, bool enable)
{
    emac_ll_ts_all_enable(hal->ptp_regs, enable);
    return ESP_OK;
}

esp_err_t emac_hal_set_pps0_out_freq(emac_hal_context_t *hal, uint32_t freq_hz)
{
    uint8_t n;
    if (freq_hz == 0) { // 0 is special case for 1PPS (narrow pulse)
        n = 0;
    } else if (!(freq_hz & (freq_hz - 1)) && freq_hz <= 16384) { // is power of two and less than maximum supported frequency
        n = 1 + __builtin_ctz(freq_hz);
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    emac_ll_set_pps0_out_freq(hal->ptp_regs, n);
    return ESP_OK;
}
#endif // SOC_EMAC_IEEE1588V2_SUPPORTED

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
