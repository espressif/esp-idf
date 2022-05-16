/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "hal/eth_types.h"
#include "soc/emac_dma_struct.h"
#include "soc/emac_mac_struct.h"
#include "soc/emac_ext_struct.h"

/**
* @brief Ethernet DMA TX Descriptor
*
*/
typedef struct {
    volatile union {
        struct {
            uint32_t Deferred : 1;                /*!< MAC defers before transmission */
            uint32_t UnderflowErr : 1;            /*!< DMA encountered an empty transmit buffer */
            uint32_t ExcessiveDeferral : 1;       /*!< Excessive deferral of over 24,288 bit times */
            uint32_t CollisionCount : 4;          /*!< Number of collisions occurred before transmitted */
            uint32_t VLanFrame : 1;               /*!< Transmitted frame is a VLAN-type frame */
            uint32_t ExcessiveCollision : 1;      /*!< Transmission aborted after 16 successive collisions */
            uint32_t LateCollision : 1;           /*!< Collision occurred after the collision window */
            uint32_t NoCarrier : 1;               /*!< Carrier Sense signal from the PHY was not asserted */
            uint32_t LossCarrier : 1;             /*!< Loss of carrier occurred during transmission  */
            uint32_t PayloadChecksumErr : 1;      /*!< Checksum error in TCP/UDP/ICMP datagram payload */
            uint32_t FrameFlushed : 1;            /*!< DMA or MTL flushed the frame */
            uint32_t JabberTimeout : 1;           /*!< MAC transmitter has experienced a jabber timeout */
            uint32_t ErrSummary : 1;              /*!< Error Summary */
            uint32_t IPHeadErr : 1;               /*!< IP Header Error */
            uint32_t TxTimestampStatus : 1;       /*!< Timestamp captured for the transmit frame */
            uint32_t VLANInsertControl : 2;       /*!< VLAN tagging or untagging before transmitting */
            uint32_t SecondAddressChained : 1;    /*!< Second address in the descriptor is Next Descriptor address */
            uint32_t TransmitEndRing : 1;         /*!< Descriptor list reached its final descriptor */
            uint32_t ChecksumInsertControl : 2;   /*!< Control checksum calculation and insertion */
            uint32_t CRCReplacementControl : 1;   /*!< Control CRC replace */
            uint32_t TransmitTimestampEnable : 1; /*!< Enable IEEE1588 harware timestamping */
            uint32_t DisablePad : 1;              /*!< Control add padding when frame short than 64 bytes */
            uint32_t DisableCRC : 1;              /*!< Control append CRC to the end of frame */
            uint32_t FirstSegment : 1;            /*!< Buffer contains the first segment of a frame */
            uint32_t LastSegment : 1;             /*!< Buffer contains the last segment of a frame */
            uint32_t InterruptOnComplete : 1;     /*!< Interrupt after frame transmitted */
            uint32_t Own : 1;                     /*!< Owner of this descriptor: DMA controller or host */
        };
        uint32_t Value;
    } TDES0;
    union {
        struct {
            uint32_t TransmitBuffer1Size : 13; /*!< First data buffer byte size */
            uint32_t Reserved : 3;             /*!< Reserved */
            uint32_t TransmitBuffer2Size : 13; /*!< Second data buffer byte size */
            uint32_t SAInsertControl : 3;      /*!< Control MAC add or replace Source Address field */
        };
        uint32_t Value;
    } TDES1;
    uint32_t Buffer1Addr;         /*!< Buffer1 address pointer */
    uint32_t Buffer2NextDescAddr; /*!< Buffer2 or next descriptor address pointer */
    uint32_t Reserved1;           /*!< Reserved */
    uint32_t Reserved2;           /*!< Reserved */
    uint32_t TimeStampLow;        /*!< Transmit Frame Timestamp Low */
    uint32_t TimeStampHigh;       /*!< Transmit Frame Timestamp High */
} eth_dma_tx_descriptor_t;
#define EMAC_DMATXDESC_CHECKSUM_BYPASS 0            /*!< Checksum engine bypass */
#define EMAC_DMATXDESC_CHECKSUM_IPV4HEADER 1        /*!< IPv4 header checksum insertion  */
#define EMAC_DMATXDESC_CHECKSUM_TCPUDPICMPSEGMENT 2 /*!< TCP/UDP/ICMP Checksum Insertion calculated over segment only */
#define EMAC_DMATXDESC_CHECKSUM_TCPUDPICMPFULL 3    /*!< TCP/UDP/ICMP Checksum Insertion fully calculated */

_Static_assert(sizeof(eth_dma_tx_descriptor_t) == 32, "eth_dma_tx_descriptor_t should occupy 32 bytes in memory");

/**
* @brief Ethernet DMA RX Descriptor
*
*/
typedef struct {
    volatile union {
        struct {
            uint32_t ExtendStatusAvailable : 1;          /*!< Extended statsu is available in RDES4 */
            uint32_t CRCErr : 1;                         /*!< CRC error occurred on frame */
            uint32_t DribbleBitErr : 1;                  /*!< frame contains non int multiple of 8 bits */
            uint32_t ReceiveErr : 1;                     /*!< Receive error */
            uint32_t ReceiveWatchdogTimeout : 1;         /*!< Receive Watchdog timeout */
            uint32_t FrameType : 1;                      /*!< Ethernet type or IEEE802.3 */
            uint32_t LateCollision : 1;                  /*!< Late collision occurred during reception */
            uint32_t TSAvailIPChecksumErrGiantFrame : 1; /*!< Timestamp available or IP Checksum error or Giant frame */
            uint32_t LastDescriptor : 1;                 /*!< Last buffer of the frame */
            uint32_t FirstDescriptor : 1;                /*!< First buffer of the frame */
            uint32_t VLANTag : 1;                        /*!< VLAN Tag: received frame is a VLAN frame */
            uint32_t OverflowErr : 1;                    /*!< Frame was damaged due to buffer overflow */
            uint32_t LengthErr : 1;                      /*!< Frame size not matching with length field */
            uint32_t SourceAddrFilterFail : 1;           /*!< SA field of frame failed the SA filter */
            uint32_t DescriptorErr : 1;                  /*!< Frame truncated and DMA doesn't own next descriptor */
            uint32_t ErrSummary : 1;                     /*!< Error Summary, OR of all errors in RDES */
            uint32_t FrameLength : 14;                   /*!< Byte length of received frame */
            uint32_t DestinationAddrFilterFail : 1;      /*!< Frame failed in the DA Filter in the MAC */
            uint32_t Own : 1;                            /*!< Owner of this descriptor: DMA controller or host */
        };
        uint32_t Value;
    } RDES0;
    union {
        struct {
            uint32_t ReceiveBuffer1Size : 13;        /*!< First data buffer size in bytes */
            uint32_t Reserved1 : 1;                  /*!< Reserved */
            uint32_t SecondAddressChained : 1;       /*!< Seconde address is the Next Descriptor address */
            uint32_t ReceiveEndOfRing : 1;           /*!< Descriptor reached its final descriptor */
            uint32_t ReceiveBuffer2Size : 13;        /*!< Second data buffer size in bytes */
            uint32_t Reserved : 2;                   /*!< Reserved */
            uint32_t DisableInterruptOnComplete : 1; /*!< Disable the assertion of interrupt to host */
        };
        uint32_t Value;
    } RDES1;
    uint32_t Buffer1Addr;         /*!< Buffer1 address pointer */
    uint32_t Buffer2NextDescAddr; /*!< Buffer2 or next descriptor address pointer */
    volatile union {
        struct {
            uint32_t IPPayloadType : 3;                 /*!< Type of payload in the IP datagram */
            uint32_t IPHeadErr : 1;                     /*!< IP header error */
            uint32_t IPPayloadErr : 1;                  /*!< IP payload error */
            uint32_t IPChecksumBypass : 1;              /*!< Checksum offload engine is bypassed */
            uint32_t IPv4PacketReceived : 1;            /*!< Received packet is an IPv4 packet */
            uint32_t IPv6PacketReceived : 1;            /*!< Received packet is an IPv6 packet */
            uint32_t MessageType : 4;                   /*!< PTP Message Type */
            uint32_t PTPFrameType : 1;                  /*!< PTP message is over Ethernet or IPv4/IPv6 */
            uint32_t PTPVersion : 1;                    /*!< Version of PTP protocol */
            uint32_t TimestampDropped : 1;              /*!< Timestamp dropped because of overflow */
            uint32_t Reserved1 : 1;                     /*!< Reserved */
            uint32_t AVPacketReceived : 1;              /*!< AV packet is received */
            uint32_t AVTaggedPacketReceived : 1;        /*!< AV tagged packet is received */
            uint32_t VLANTagPrioVal : 3;                /*!< VLAN tag's user value in the received packekt */
            uint32_t Reserved2 : 3;                     /*!< Reserved */
            uint32_t Layer3FilterMatch : 1;             /*!< Received frame matches one of the enabled Layer3 IP */
            uint32_t Layer4FilterMatch : 1;             /*!< Received frame matches one of the enabled Layer4 IP */
            uint32_t Layer3Layer4FilterNumberMatch : 2; /*!< Number of Layer3 and Layer4 Filter that matches the received frame */
            uint32_t Reserved3 : 4;                     /*!< Reserved */
        };
        uint32_t Value;
    } ExtendedStatus;
    uint32_t Reserved;      /*!< Reserved */
    uint32_t TimeStampLow;  /*!< Receive frame timestamp low */
    uint32_t TimeStampHigh; /*!< Receive frame timestamp high */
} eth_dma_rx_descriptor_t;

_Static_assert(sizeof(eth_dma_rx_descriptor_t) == 32, "eth_dma_rx_descriptor_t should occupy 32 bytes in memory");

typedef struct {
    emac_mac_dev_t *mac_regs;
    emac_dma_dev_t *dma_regs;
    emac_ext_dev_t *ext_regs;
    uint8_t **rx_buf;
    uint8_t **tx_buf;
    void *descriptors;
    eth_dma_rx_descriptor_t *rx_desc;
    eth_dma_tx_descriptor_t *tx_desc;

} emac_hal_context_t;

void emac_hal_init(emac_hal_context_t *hal, void *descriptors,
                   uint8_t **rx_buf, uint8_t **tx_buf);

void emac_hal_iomux_init_mii(void);

void emac_hal_iomux_init_rmii(void);

void emac_hal_iomux_rmii_clk_input(void);

void emac_hal_iomux_rmii_clk_ouput(int num);

void emac_hal_iomux_init_tx_er(void);

void emac_hal_iomux_init_rx_er(void);

void emac_hal_reset_desc_chain(emac_hal_context_t *hal);

void emac_hal_reset(emac_hal_context_t *hal);

bool emac_hal_is_reset_done(emac_hal_context_t *hal);

void emac_hal_set_csr_clock_range(emac_hal_context_t *hal, int freq);

void emac_hal_init_mac_default(emac_hal_context_t *hal);

void emac_hal_init_dma_default(emac_hal_context_t *hal);

void emac_hal_set_speed(emac_hal_context_t *hal, uint32_t speed);

void emac_hal_set_duplex(emac_hal_context_t *hal, eth_duplex_t duplex);

void emac_hal_set_promiscuous(emac_hal_context_t *hal, bool enable);

/**
 * @brief Send MAC-CTRL frames to peer (EtherType=0x8808, opcode=0x0001, dest_addr=MAC-specific-ctrl-proto-01 (01:80:c2:00:00:01))
 */
void emac_hal_send_pause_frame(emac_hal_context_t *hal, bool enable);

bool emac_hal_is_mii_busy(emac_hal_context_t *hal);

void emac_hal_set_phy_cmd(emac_hal_context_t *hal, uint32_t phy_addr, uint32_t phy_reg, bool write);

void emac_hal_set_phy_data(emac_hal_context_t *hal, uint32_t reg_value);

uint32_t emac_hal_get_phy_data(emac_hal_context_t *hal);

void emac_hal_set_address(emac_hal_context_t *hal, uint8_t *mac_addr);

/**
 * @brief Starts EMAC Transmission & Reception
 *
 * @param hal EMAC HAL context infostructure
 */
void emac_hal_start(emac_hal_context_t *hal);

/**
 * @brief Stops EMAC Transmission & Reception
 *
 * @param hal EMAC HAL context infostructure
 * @return
 *     - ESP_OK: succeed
  *    - ESP_ERR_INVALID_STATE: previous frame transmission/reception is not completed. When this error occurs,
  *      wait and reapeat the EMAC stop again.
 */
esp_err_t emac_hal_stop(emac_hal_context_t *hal);

uint32_t emac_hal_get_tx_desc_owner(emac_hal_context_t *hal);

uint32_t emac_hal_transmit_frame(emac_hal_context_t *hal, uint8_t *buf, uint32_t length);

uint32_t emac_hal_receive_frame(emac_hal_context_t *hal, uint8_t *buf, uint32_t size, uint32_t *frames_remain, uint32_t *free_desc);

void emac_hal_enable_flow_ctrl(emac_hal_context_t *hal, bool enable);

uint32_t emac_hal_get_intr_enable_status(emac_hal_context_t *hal);

uint32_t emac_hal_get_intr_status(emac_hal_context_t *hal);

void emac_hal_clear_corresponding_intr(emac_hal_context_t *hal, uint32_t bits);

void emac_hal_clear_all_intr(emac_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
