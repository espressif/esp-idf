// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "soc/gpio_periph.h"
#include "soc/rtc.h"
#include "hal/emac.h"

#define ETH_CRC_LENGTH (4)

#if CONFIG_ETH_RMII_CLK_OUTPUT
static void emac_config_apll_clock(void)
{
    /* apll_freq = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536)/((o_div + 2) * 2) */
    rtc_xtal_freq_t rtc_xtal_freq = rtc_clk_xtal_freq_get();
    switch (rtc_xtal_freq) {
    case RTC_XTAL_FREQ_40M: // Recommended
        /* 50 MHz = 40MHz * (4 + 6) / (2 * (2 + 2) = 50.000 */
        /* sdm0 = 0, sdm1 = 0, sdm2 = 6, o_div = 2 */
        rtc_clk_apll_enable(true, 0, 0, 6, 2);
        break;
    case RTC_XTAL_FREQ_26M:
        /* 50 MHz = 26MHz * (4 + 15 + 118 / 256 + 39/65536) / ((3 + 2) * 2) = 49.999992 */
        /* sdm0 = 39, sdm1 = 118, sdm2 = 15, o_div = 3 */
        rtc_clk_apll_enable(true, 39, 118, 15, 3);
        break;
    case RTC_XTAL_FREQ_24M:
        /* 50 MHz = 24MHz * (4 + 12 + 255 / 256 + 255/65536) / ((2 + 2) * 2) = 49.499977 */
        /* sdm0 = 255, sdm1 = 255, sdm2 = 12, o_div = 2 */
        rtc_clk_apll_enable(true, 255, 255, 12, 2);
        break;
    default: // Assume we have a 40M xtal
        rtc_clk_apll_enable(true, 0, 0, 6, 2);
        break;
    }
}
#endif

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

void emac_hal_lowlevel_init(emac_hal_context_t *hal)
{
    /* GPIO configuration */
    /* TX_EN to GPIO21 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO21_U, FUNC_GPIO21_EMAC_TX_EN);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[21]);
    /* TXD0 to GPIO19 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO19_U, FUNC_GPIO19_EMAC_TXD0);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[19]);
    /* TXD1 to GPIO22 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO22_U, FUNC_GPIO22_EMAC_TXD1);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[22]);
    /* RXD0 to GPIO25 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO25_U, FUNC_GPIO25_EMAC_RXD0);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[25]);
    /* RXD1 to GPIO26 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO26_U, FUNC_GPIO26_EMAC_RXD1);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[26]);
    /* CRS_DV to GPIO27 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO27_U, FUNC_GPIO27_EMAC_RX_DV);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[27]);
#if CONFIG_ETH_RMII_CLK_INPUT
#if CONFIG_ETH_RMII_CLK_IN_GPIO == 0
    /* RMII clock (50MHz) input to GPIO0 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_EMAC_TX_CLK);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[0]);
#else
#error "ESP32 EMAC only support input RMII clock to GPIO0"
#endif
#endif
#if CONFIG_ETH_RMII_CLK_OUTPUT
#if CONFIG_ETH_RMII_CLK_OUTPUT_GPIO0
    /* APLL clock output to GPIO0 (must be configured to 50MHz!) */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[0]);
#elif CONFIG_ETH_RMII_CLK_OUT_GPIO == 16
    /* RMII CLK (50MHz) output to GPIO16 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO16_U, FUNC_GPIO16_EMAC_CLK_OUT);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[16]);
#elif CONFIG_ETH_RMII_CLK_OUT_GPIO == 17
    /* RMII CLK (50MHz) output to GPIO17 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO17_U, FUNC_GPIO17_EMAC_CLK_OUT_180);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[17]);
#endif
#endif // CONFIG_ETH_RMII_CLK_OUTPUT
    /* Clock configuration */
#if CONFIG_ETH_PHY_INTERFACE_MII
    hal->ext_regs->ex_phyinf_conf.phy_intf_sel = 0;
    hal->ext_regs->ex_clk_ctrl.mii_clk_rx_en = 1;
    hal->ext_regs->ex_clk_ctrl.mii_clk_tx_en = 1;
#elif CONFIG_ETH_PHY_INTERFACE_RMII
    hal->ext_regs->ex_phyinf_conf.phy_intf_sel = 4;
#if CONFIG_ETH_RMII_CLK_INPUT
    hal->ext_regs->ex_clk_ctrl.ext_en = 1;
    hal->ext_regs->ex_clk_ctrl.int_en = 0;
    hal->ext_regs->ex_oscclk_conf.clk_sel = 1;
#elif CONFIG_ETH_RMII_CLK_OUTPUT
    hal->ext_regs->ex_clk_ctrl.ext_en = 0;
    hal->ext_regs->ex_clk_ctrl.int_en = 1;
    hal->ext_regs->ex_oscclk_conf.clk_sel = 0;
    emac_config_apll_clock();
    hal->ext_regs->ex_clkout_conf.div_num = 0;
    hal->ext_regs->ex_clkout_conf.h_div_num = 0;
#if CONFIG_ETH_RMII_CLK_OUTPUT_GPIO0
    /* Choose the APLL clock to output on GPIO */
    REG_WRITE(PIN_CTRL, 6);
#endif // CONFIG_RMII_CLK_OUTPUT_GPIO0
#endif // CONFIG_ETH_RMII_CLK_INPUT
#endif // CONFIG_ETH_PHY_INTERFACE_MII
}

void emac_hal_reset(emac_hal_context_t *hal)
{
    hal->dma_regs->dmabusmode.sw_rst = 1;
}

bool emac_hal_is_reset_done(emac_hal_context_t *hal)
{
    return hal->dma_regs->dmabusmode.sw_rst ? false : true;
}

void emac_hal_set_csr_clock_range(emac_hal_context_t *hal)
{
    /* Tell MAC system clock Frequency, which will determin the frequency range of MDC(1MHz~2.5MHz) */
    if (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ >= 20 && CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ < 35) {
        hal->mac_regs->emacgmiiaddr.miicsrclk = 2;
    } else if (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ >= 35 && CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ < 60) {
        hal->mac_regs->emacgmiiaddr.miicsrclk = 3;
    } else if (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ >= 60 && CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ < 100) {
        hal->mac_regs->emacgmiiaddr.miicsrclk = 0;
    } else if (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ >= 100 && CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ < 150) {
        hal->mac_regs->emacgmiiaddr.miicsrclk = 1;
    } else if (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ > 150 && CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ < 250) {
        hal->mac_regs->emacgmiiaddr.miicsrclk = 4;
    } else {
        hal->mac_regs->emacgmiiaddr.miicsrclk = 5;
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
        hal->rx_desc[i].RDES0.Own = 1;
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
        /* Set Second Address Chained bit */
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
    hal->dma_regs->dmarxbaseaddr = (uint32_t)hal->rx_desc;
    hal->dma_regs->dmatxbaseaddr = (uint32_t)hal->tx_desc;
}

void emac_hal_init_mac_default(emac_hal_context_t *hal)
{
    /* MACCR Configuration */
    typeof(hal->mac_regs->gmacconfig) maccr = hal->mac_regs->gmacconfig;
    /* Enable the watchdog on the receiver, frame longer than 2048 Bytes is not allowed */
    maccr.watchdog = EMAC_WATCHDOG_ENABLE;
    /* Enable the jabber timer on the transmitter, frame longer than 2048 Bytes is not allowed */
    maccr.jabber = EMAC_JABBER_ENABLE;
    /* minimum IFG between frames during transmission is 96 bit times */
    maccr.interframegap = EMAC_INTERFRAME_GAP_96BIT;
    /* Enable Carrier Sense During Transmission */
    maccr.disablecrs = EMAC_CARRIERSENSE_ENABLE;
    /* Select port: 10/100 Mbps */
    maccr.mii = EMAC_PORT_10_100MBPS;
    /* Select speed: here set default 100M, afterwards, will reset by auto-negotiation */
    maccr.fespeed = EMAC_SPEED_100M;
    /* Allow the reception of frames when the TX_EN signal is asserted in Half-Duplex mode */
    maccr.rxown = EMAC_RECEIVE_OWN_ENABLE;
    /* Disable internal loopback mode */
    maccr.loopback = EMAC_LOOPBACK_DISABLE;
    /* Select duplex mode: here set default full duplex, afterwards, will reset by auto-negotiation */
    maccr.duplex = EMAC_DUPLEX_FULL;
    /* Select the checksum mode for received frame payload's TCP/UDP/ICMP headers */
    maccr.rxipcoffload = EMAC_CHECKSUM_HW;
    /* Enable MAC retry transmission when a colision occurs in half duplex mode */
    maccr.retry = EMAC_RETRY_TRANSMISSION_ENABLE;
    /* MAC passes all incoming frames to host, without modifying them */
    maccr.padcrcstrip = EMAC_AUTO_PAD_CRC_STRIP_DISABLE;
    /* Set Back-Off limit time before retry a transmittion after a collision */
    maccr.backofflimit = EMAC_BACKOFF_LIMIT_10;
    /* Disable deferral check, MAC defers until the CRS signal goes inactive */
    maccr.deferralcheck = EMAC_DEFERRAL_CHECK_DISABLE;
    /* Set preamble length 7 Bytes */
    maccr.pltf = EMAC_PREAMBLE_LENGTH_7;
    hal->mac_regs->gmacconfig = maccr;

    /* MACFFR Configuration */
    typeof(hal->mac_regs->gmacff) macffr = hal->mac_regs->gmacff;
    /* Receiver module passes only those frames to the Application that pass the SA or DA address filter */
    macffr.receive_all = EMAC_RECEIVE_ALL_DISABLE;
    /* Disable source address filter */
    macffr.safe = EMAC_SOURCE_ADDR_FILTER_DISABLE;
    macffr.saif = 0;
    /* MAC blocks all control frames */
    macffr.pcf = EMAC_CONTROL_FRAME_BLOCKALL;
    /* AFM module passes all received broadcast frames and multicast frames */
    macffr.dbf = EMAC_RECEPT_BROADCAST_ENABLE;
    macffr.pam = 1;
    /* Address Check block operates in normal filtering mode for the DA address */
    macffr.daif = EMAC_DEST_ADDR_FILTER_NORMAL;
    /* Disable Promiscuous Mode */
    macffr.pmode = EMAC_PROMISCUOUS_DISABLE;
    hal->mac_regs->gmacff = macffr;

    /* MACFCR Configuration */
    typeof(hal->mac_regs->gmacfc) macfcr = hal->mac_regs->gmacfc;
    /* Pause time */
    macfcr.pause_time = EMAC_PAUSE_TIME;
    /* Enable generation of Zero-Quanta Pause Control frames */
    macfcr.dzpq = EMAC_ZERO_QUANTA_PAUSE_ENABLE;
    /* Threshold of the PAUSE to be checked for automatic retransmission of PAUSE Frame */
    macfcr.plt = EMAC_PAUSE_LOW_THRESHOLD_MINUS_28;
    /* Don't allow MAC detect Pause frames with MAC address0 unicast address and unique multicast address */
    macfcr.upfd = EMAC_UNICAST_PAUSE_DETECT_DISABLE;
    /* Enable MAC to decode the received Pause frame and disable its transmitter for a specific time */
    macfcr.rfce = EMAC_RECEIVE_FLOW_CONTROL_ENABLE;
    /* Enable MAC to transmit Pause frames in full duplex mode or the MAC back-pressure operation in half duplex mode */
    macfcr.tfce = EMAC_TRANSMIT_FLOW_CONTROL_ENABLE;
    hal->mac_regs->gmacfc = macfcr;
}

void emac_hal_init_dma_default(emac_hal_context_t *hal)
{
    /* DMAOMR Configuration */
    typeof(hal->dma_regs->dmaoperation_mode) dmaomr = hal->dma_regs->dmaoperation_mode;
    /* Enable Dropping of TCP/IP Checksum Error Frames */
    dmaomr.dis_drop_tcpip_err_fram = EMAC_DROP_TCPIP_CHECKSUM_ERROR_ENABLE;
    /* Enable Receive Store Forward */
    dmaomr.rx_store_forward = EMAC_RECEIVE_STORE_FORWARD_ENABLE;
    /* Enable Flushing of Received Frames because of the unavailability of receive descriptors or buffers */
    dmaomr.dis_flush_recv_frames = EMAC_FLUSH_RECEIVED_FRAME_ENABLE;
    /* Enable Transmit Store Forward */
    dmaomr.tx_str_fwd = EMAC_TRANSMIT_STORE_FORWARD_ENABLE;
    /* Flush Transmit FIFO */
    dmaomr.flush_tx_fifo = 1;
    /* Transmit Threshold Control */
    dmaomr.tx_thresh_ctrl = EMAC_TRANSMIT_THRESHOLD_CONTROL_64;
    /* Disable Forward Error Frame */
    dmaomr.fwd_err_frame = EMAC_FORWARD_ERROR_FRAME_DISABLE;
    /* Disable forward undersized good frame */
    dmaomr.fwd_under_gf = EMAC_FORWARD_UNDERSIZED_GOOD_FRAME_DISABLE;
    /* Receive Threshold Control */
    dmaomr.rx_thresh_ctrl = EMAC_RECEIVE_THRESHOLD_CONTROL_64;
    /* Allow the DMA to process a second frame of Transmit data even before obtaining the status for the first frame */
    dmaomr.opt_second_frame = EMAC_OPERATE_SECOND_FRAME_ENABLE;
    hal->dma_regs->dmaoperation_mode = dmaomr;

    /* DMABMR Configuration */
    typeof(hal->dma_regs->dmabusmode) dmabmr = hal->dma_regs->dmabusmode;
    /* Enable Mixed Burst */
    dmabmr.dmamixedburst = EMAC_MIXED_BURST_ENABLE;
    /* Enable Address Aligned Beates */
    dmabmr.dmaaddralibea = EMAC_ADDR_ALIGN_BEATS_ENABLE;
    /* Use Separate PBL */
    dmabmr.use_sep_pbl = EMAC_USE_SEPARATE_PBL;
    /* Set Rx/Tx DMA Burst Length */
    dmabmr.rx_dma_pbl = EMAC_DMA_BURST_LENGTH_32BEAT;
    dmabmr.prog_burst_len = EMAC_DMA_BURST_LENGTH_32BEAT;
    /* Enable Enhanced Descriptor,8 Words(32 Bytes) */
    dmabmr.alt_desc_size = EMAC_ENHANCED_DESCRIPTOR_ENABLE;
    /* Specifies the number of word to skip between two unchained descriptors (Ring mode) */
    dmabmr.desc_skip_len = 0;
    /* DMA Arbitration Scheme */
    dmabmr.dma_arb_sch = EMAC_DMA_ARBITRATION_SCHEME_ROUNDROBIN;
    /* Set priority ratio in the weighted round-robin arbitration between Rx DMA and Tx DMA */
    dmabmr.pri_ratio = EMAC_DMA_ARBITRATION_ROUNDROBIN_RXTX_1_1;
    hal->dma_regs->dmabusmode = dmabmr;
}

void emac_hal_set_speed(emac_hal_context_t *hal, uint32_t speed)
{
    hal->mac_regs->gmacconfig.fespeed = speed;
}

void emac_hal_set_duplex(emac_hal_context_t *hal, uint32_t duplex)
{
    hal->mac_regs->gmacconfig.duplex = duplex;
}

void emac_hal_set_promiscuous(emac_hal_context_t *hal, bool enable)
{
    if (enable) {
        hal->mac_regs->gmacff.pmode = 1;
    } else {
        hal->mac_regs->gmacff.pmode = 0;
    }
}

bool emac_hal_is_mii_busy(emac_hal_context_t *hal)
{
    return hal->mac_regs->emacgmiiaddr.miibusy ? true : false;
}

void emac_hal_set_phy_cmd(emac_hal_context_t *hal, uint32_t phy_addr, uint32_t phy_reg, bool write)
{
    typeof(hal->mac_regs->emacgmiiaddr) macmiiar = hal->mac_regs->emacgmiiaddr;
    macmiiar.miidev = phy_addr;
    /* Set the PHY register address */
    macmiiar.miireg = phy_reg;
    if (write) {
        /* Set write mode */
        macmiiar.miiwrite = 1;
    } else {
        /* Set read mode */
        macmiiar.miiwrite = 0;
    }
    /* Set MII busy bit */
    macmiiar.miibusy = 1;
    /* Write the result value into the MII Address register */
    hal->mac_regs->emacgmiiaddr = macmiiar;
}

void emac_hal_set_phy_data(emac_hal_context_t *hal, uint32_t reg_value)
{
    hal->mac_regs->emacmiidata.mii_data = reg_value;
}

uint32_t emac_hal_get_phy_data(emac_hal_context_t *hal)
{
    return hal->mac_regs->emacmiidata.mii_data;
}

void emac_hal_set_address(emac_hal_context_t *hal, uint8_t *mac_addr)
{
    /* Make sure mac address is unicast type */
    if (!(mac_addr[0] & 0x01)) {
        hal->mac_regs->emacaddr0high.address0_hi = (mac_addr[5] << 8) | mac_addr[4];
        hal->mac_regs->emacaddr0low = (mac_addr[3] << 24) | (mac_addr[2] << 16) | (mac_addr[1] << 8) | (mac_addr[0]);
    }
}

void emac_hal_start(emac_hal_context_t *hal)
{
    typeof(hal->dma_regs->dmaoperation_mode) opm = hal->dma_regs->dmaoperation_mode;
    typeof(hal->mac_regs->gmacconfig) cfg = hal->mac_regs->gmacconfig;

    /* Enable Ethernet MAC and DMA Interrupt */
    hal->dma_regs->dmain_en.val = 0xFFFFFFFF;

    /* Flush Transmit FIFO */
    opm.flush_tx_fifo = 1;
    /* Start DMA transmission */
    opm.start_stop_transmission_command = 1;
    /* Start DMA reception */
    opm.start_stop_rx = 1;
    /* Enable transmit state machine of the MAC for transmission on the MII */
    cfg.tx = 1;
    /* Enable receive state machine of the MAC for reception from the MII */
    cfg.rx = 1;

    hal->dma_regs->dmaoperation_mode = opm;
    hal->mac_regs->gmacconfig = cfg;

    /* Clear all pending interrupts */
    hal->dma_regs->dmastatus.val = 0xFFFFFFFF;
}

void emac_hal_stop(emac_hal_context_t *hal)
{
    typeof(hal->dma_regs->dmaoperation_mode) opm = hal->dma_regs->dmaoperation_mode;
    typeof(hal->mac_regs->gmacconfig) cfg = hal->mac_regs->gmacconfig;

    /* Flush Transmit FIFO */
    opm.flush_tx_fifo = 1;
    /* Stop DMA transmission */
    opm.start_stop_transmission_command = 0;
    /* Stop DMA reception */
    opm.start_stop_rx = 0;
    /* Disable receive state machine of the MAC for reception from the MII */
    cfg.rx = 0;
    /* Disable transmit state machine of the MAC for transmission on the MII */
    cfg.tx = 0;

    hal->dma_regs->dmaoperation_mode = opm;
    hal->mac_regs->gmacconfig = cfg;

    /* Disable Ethernet MAC and DMA Interrupt */
    hal->dma_regs->dmain_en.val = 0x0;
}

uint32_t emac_hal_get_tx_desc_owner(emac_hal_context_t *hal)
{
    return hal->tx_desc->TDES0.Own;
}

void emac_hal_transmit_frame(emac_hal_context_t *hal, uint8_t *buf, uint32_t length)
{
    /* Get the number of Tx buffers to use for the frame */
    uint32_t bufcount = 0;
    uint32_t lastlen = length;
    while (lastlen > CONFIG_ETH_DMA_BUFFER_SIZE) {
        lastlen -= CONFIG_ETH_DMA_BUFFER_SIZE;
        bufcount++;
    }
    if (lastlen) {
        bufcount++;
    }
    /* A frame is transmitted in multiple descriptor */
    for (uint32_t i = 0; i < bufcount; i++) {
        /* Clear FIRST and LAST segment bits */
        hal->tx_desc->TDES0.FirstSegment = 0;
        hal->tx_desc->TDES0.LastSegment = 0;
        if (i == 0) {
            /* Setting the first segment bit */
            hal->tx_desc->TDES0.FirstSegment = 1;
        }
        if (i == (bufcount - 1)) {
            /* Setting the last segment bit */
            hal->tx_desc->TDES0.LastSegment = 1;
            /* Enable transmit interrupt */
            hal->tx_desc->TDES0.InterruptOnComplete = 1;
            /* Program size */
            hal->tx_desc->TDES1.TransmitBuffer1Size = lastlen;
            /* copy data from uplayer stack buffer */
            memcpy((void *)(hal->tx_desc->Buffer1Addr), buf + i * CONFIG_ETH_DMA_BUFFER_SIZE, lastlen);
        } else {
            /* Program size */
            hal->tx_desc->TDES1.TransmitBuffer1Size = CONFIG_ETH_DMA_BUFFER_SIZE;
            /* copy data from uplayer stack buffer */
            memcpy((void *)(hal->tx_desc->Buffer1Addr), buf + i * CONFIG_ETH_DMA_BUFFER_SIZE, CONFIG_ETH_DMA_BUFFER_SIZE);
        }
        /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
        hal->tx_desc->TDES0.Own = EMAC_DMADESC_OWNER_DMA;
        /* Point to next descriptor */
        hal->tx_desc = (eth_dma_tx_descriptor_t *)(hal->tx_desc->Buffer2NextDescAddr);
    }
    hal->dma_regs->dmatxpolldemand = 0;
}

uint32_t emac_hal_receive_frame(emac_hal_context_t *hal, uint8_t *buf, uint32_t size, uint32_t *frames_remain)
{
    eth_dma_rx_descriptor_t *desc_iter = NULL;
    eth_dma_rx_descriptor_t *first_desc = NULL;
    uint32_t iter = 0;
    uint32_t seg_count = 0;
    uint32_t len = 0;
    uint32_t frame_count = 0;

    first_desc = hal->rx_desc;
    desc_iter = hal->rx_desc;
    /* Traverse descriptors owned by CPU */
    while ((desc_iter->RDES0.Own != EMAC_DMADESC_OWNER_DMA) && (iter < CONFIG_ETH_DMA_RX_BUFFER_NUM) && !frame_count) {
        iter++;
        seg_count++;
        /* Last segment in frame */
        if (desc_iter->RDES0.LastDescriptor) {
            /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
            len = desc_iter->RDES0.FrameLength - ETH_CRC_LENGTH;
            /* check if the buffer can store the whole frame */
            if (len > size) {
                /* return the real size that we want */
                /* user need to compare the return value to the size they prepared when this function returned */
                return len;
            }
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
    /* there's at least one frame to process */
    if (frame_count) {
        /* check how many frames left to handle */
        while ((desc_iter->RDES0.Own != EMAC_DMADESC_OWNER_DMA) && (iter < CONFIG_ETH_DMA_RX_BUFFER_NUM)) {
            iter++;
            if (desc_iter->RDES0.LastDescriptor) {
                frame_count++;
            }
            /* point to next descriptor */
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        }
        desc_iter = first_desc;
        for (iter = 0; iter < seg_count - 1; iter++) {
            /* copy data to buffer */
            memcpy(buf + iter * CONFIG_ETH_DMA_BUFFER_SIZE,
                   (void *)(desc_iter->Buffer1Addr), CONFIG_ETH_DMA_BUFFER_SIZE);
            /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
            desc_iter->RDES0.Own = EMAC_DMADESC_OWNER_DMA;
            desc_iter = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        }
        memcpy(buf + iter * CONFIG_ETH_DMA_BUFFER_SIZE,
               (void *)(desc_iter->Buffer1Addr), len % CONFIG_ETH_DMA_BUFFER_SIZE);
        desc_iter->RDES0.Own = EMAC_DMADESC_OWNER_DMA;
        /* update rxdesc */
        hal->rx_desc = (eth_dma_rx_descriptor_t *)(desc_iter->Buffer2NextDescAddr);
        /* poll rx demand */
        hal->dma_regs->dmarxpolldemand = 0;
        frame_count--;
    }
    *frames_remain = frame_count;
    return len;
}

void emac_hal_isr(void *arg)
{
    emac_hal_context_t *hal = (emac_hal_context_t *)arg;
    typeof(hal->dma_regs->dmastatus) dma_status = hal->dma_regs->dmastatus;
    /* DMA Normal Interrupt */
    if (dma_status.norm_int_summ) {
        /* Transmit Interrupt */
        if (dma_status.trans_int) {
            emac_hal_tx_complete_cb(arg);
            hal->dma_regs->dmastatus.trans_int = 1;
        }
        /* Transmit Buffer Unavailable */
        if (dma_status.trans_buf_unavail) {
            emac_hal_tx_unavail_cb(arg);
            hal->dma_regs->dmastatus.trans_buf_unavail = 1;
        }
        /* Receive Interrupt */
        if (dma_status.recv_int) {
            emac_hal_rx_complete_cb(arg);
            hal->dma_regs->dmastatus.recv_int = 1;
        }
        /* Early Receive Interrupt */
        if (dma_status.early_recv_int) {
            emac_hal_rx_early_cb(arg);
            hal->dma_regs->dmastatus.early_recv_int = 1;
        }
        hal->dma_regs->dmastatus.norm_int_summ = 1;
    }
    /* DMA Abnormal Interrupt */
    if (dma_status.abn_int_summ) {
        /* Transmit Process Stopped */
        if (dma_status.trans_proc_stop) {
            hal->dma_regs->dmastatus.trans_proc_stop = 1;
        }
        /* Transmit Jabber Timeout */
        if (dma_status.trans_jabber_to) {
            hal->dma_regs->dmastatus.trans_jabber_to = 1;
        }
        /* Receive FIFO Overflow */
        if (dma_status.recv_ovflow) {
            hal->dma_regs->dmastatus.recv_ovflow = 1;
        }
        /* Transmit Underflow */
        if (dma_status.trans_undflow) {
            hal->dma_regs->dmastatus.trans_undflow = 1;
        }
        /* Receive Buffer Unavailable */
        if (dma_status.recv_buf_unavail) {
            emac_hal_rx_unavail_cb(arg);
            hal->dma_regs->dmastatus.recv_buf_unavail = 1;
        }
        /* Receive Process Stopped */
        if (dma_status.recv_proc_stop) {
            hal->dma_regs->dmastatus.recv_proc_stop = 1;
        }
        /* Receive Watchdog Timeout */
        if (dma_status.recv_wdt_to) {
            hal->dma_regs->dmastatus.recv_wdt_to = 1;
        }
        /* Early Transmit Interrupt */
        if (dma_status.early_trans_int) {
            hal->dma_regs->dmastatus.early_trans_int = 1;
        }
        /* Fatal Bus Error */
        if (dma_status.fatal_bus_err_int) {
            hal->dma_regs->dmastatus.fatal_bus_err_int = 1;
        }
        hal->dma_regs->dmastatus.abn_int_summ = 1;
    }
}

__attribute__((weak)) void emac_hal_tx_complete_cb(void *arg)
{
    // This is a weak function, do nothing by default
    // Upper code can rewrite this function
    // Note: you're in the interrupt context
    return;
}

__attribute__((weak)) void emac_hal_tx_unavail_cb(void *arg)
{
    // This is a weak function, do nothing by default
    // Upper code can rewrite this function
    // Note: you're in the interrupt context
    return;
}

__attribute__((weak)) void emac_hal_rx_complete_cb(void *arg)
{
    // This is a weak function, do nothing by default
    // Upper code can rewrite this function
    // Note: you're in the interrupt context
    return;
}

__attribute__((weak)) void emac_hal_rx_early_cb(void *arg)
{
    // This is a weak function, do nothing by default
    // Upper code can rewrite this function
    // Note: you're in the interrupt context
    return;
}

__attribute__((weak)) void emac_hal_rx_unavail_cb(void *arg)
{
    // This is a weak function, do nothing by default
    // Upper code can rewrite this function
    // Note: you're in the interrupt context
    return;
}
