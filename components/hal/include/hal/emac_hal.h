/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once



#include <stdint.h>
#include <stdbool.h>
#include "esp_assert.h"
#include "esp_err.h"
#include "hal/eth_types.h"
#include "soc/soc_caps.h"
#ifdef __cplusplus
extern "C" {
#endif
#if SOC_EMAC_SUPPORTED
#include "hal/emac_ll.h"



/**
 * @brief Indicate to ::emac_hal_receive_frame that receive frame buffer was allocated by ::emac_hal_alloc_recv_buf
 *
 */
#define EMAC_HAL_BUF_SIZE_AUTO 0

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

ESP_STATIC_ASSERT(sizeof(eth_dma_tx_descriptor_t) == 32, "eth_dma_tx_descriptor_t should occupy 32 bytes in memory");

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

ESP_STATIC_ASSERT(sizeof(eth_dma_rx_descriptor_t) == 32, "eth_dma_rx_descriptor_t should occupy 32 bytes in memory");

typedef struct emac_mac_dev_s *emac_mac_soc_regs_t;
typedef struct emac_dma_dev_s *emac_dma_soc_regs_t;
typedef struct emac_ext_dev_s *emac_ext_soc_regs_t;

typedef struct {
    emac_mac_soc_regs_t mac_regs;
    emac_dma_soc_regs_t dma_regs;
    emac_ext_soc_regs_t ext_regs;
    uint8_t **rx_buf;
    uint8_t **tx_buf;
    void *descriptors;
    eth_dma_rx_descriptor_t *rx_desc;
    eth_dma_tx_descriptor_t *tx_desc;

} emac_hal_context_t;

/**
 * @brief EMAC related configuration
 */
typedef struct {
    eth_mac_dma_burst_len_t dma_burst_len;  /*!< eth-type enum of chosen dma burst-len */
} emac_hal_dma_config_t;

void emac_hal_init(emac_hal_context_t *hal, void *descriptors,
                   uint8_t **rx_buf, uint8_t **tx_buf);

void emac_hal_iomux_init_mii(void);

void emac_hal_iomux_init_rmii(void);

void emac_hal_iomux_rmii_clk_input(void);

void emac_hal_iomux_rmii_clk_ouput(int num);

void emac_hal_iomux_init_tx_er(void);

void emac_hal_iomux_init_rx_er(void);

static inline void emac_hal_clock_enable_mii(emac_hal_context_t *hal)
{
    emac_ll_clock_enable_mii(hal->ext_regs);
}

static inline void emac_hal_clock_enable_rmii_input(emac_hal_context_t *hal)
{
    emac_ll_clock_enable_rmii_input(hal->ext_regs);
}

static inline void emac_hal_clock_enable_rmii_output(emac_hal_context_t *hal)
{
    emac_ll_clock_enable_rmii_output(hal->ext_regs);
}

void emac_hal_reset_desc_chain(emac_hal_context_t *hal);

static inline void *emac_hal_get_desc_chain(emac_hal_context_t *hal)
{
    return hal->descriptors;
}

static inline void emac_hal_reset(emac_hal_context_t *hal)
{
    emac_ll_reset(hal->dma_regs);
}

static inline bool emac_hal_is_reset_done(emac_hal_context_t *hal)
{
    return emac_ll_is_reset_done(hal->dma_regs);
}

void emac_hal_set_csr_clock_range(emac_hal_context_t *hal, int freq);

void emac_hal_init_mac_default(emac_hal_context_t *hal);

void emac_hal_init_dma_default(emac_hal_context_t *hal, emac_hal_dma_config_t *hal_config);

static inline void emac_hal_set_speed(emac_hal_context_t *hal, eth_speed_t speed)
{
    emac_ll_set_port_speed(hal->mac_regs, speed);
}

static inline void emac_hal_set_duplex(emac_hal_context_t *hal, eth_duplex_t duplex)
{
    emac_ll_set_duplex(hal->mac_regs, duplex);
}

static inline void emac_hal_set_promiscuous(emac_hal_context_t *hal, bool enable)
{
    emac_ll_promiscuous_mode_enable(hal->mac_regs, enable);
}

/**
 * @brief Send MAC-CTRL frames to peer (EtherType=0x8808, opcode=0x0001, dest_addr=MAC-specific-ctrl-proto-01 (01:80:c2:00:00:01))
 */
static inline void emac_hal_send_pause_frame(emac_hal_context_t *hal, bool enable)
{
    emac_ll_pause_frame_enable(hal->ext_regs, enable);
}

static inline bool emac_hal_is_mii_busy(emac_hal_context_t *hal)
{
    return emac_ll_is_mii_busy(hal->mac_regs);
}

void emac_hal_set_phy_cmd(emac_hal_context_t *hal, uint32_t phy_addr, uint32_t phy_reg, bool write);

static inline void emac_hal_set_phy_data(emac_hal_context_t *hal, uint32_t reg_value)
{
    emac_ll_set_phy_data(hal->mac_regs, reg_value);
}

static inline uint32_t emac_hal_get_phy_data(emac_hal_context_t *hal)
{
    return emac_ll_get_phy_data(hal->mac_regs);
}

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

/**
 * @brief Transmit data from buffer over EMAC
 *
 * @param[in] hal EMAC HAL context infostructure
 * @param[in] buf buffer to be transmitted
 * @param[in] length length of the buffer
 * @return number of transmitted bytes when success
 */
uint32_t emac_hal_transmit_frame(emac_hal_context_t *hal, uint8_t *buf, uint32_t length);

/**
 * @brief Transmit data from multiple buffers over EMAC in single Ethernet frame. Data will be joint into
 *        single frame in order in which the buffers are stored in input array.
 *
 * @param[in] hal EMAC HAL context infostructure
 * @param[in] buffs array of pointers to buffers to be transmitted
 * @param[in] lengths array of lengths of the buffers
 * @param[in] inbuffs_cnt number of buffers (i.e. input arrays size)
 * @return number of transmitted bytes when success
 *
 * @pre @p lengths array must have the same size as @p buffs array and their elements need to be stored in the same
 *      order, i.e. lengths[1] is a length assocaited with data buffer referenced at buffs[1] position.
 */
uint32_t emac_hal_transmit_multiple_buf_frame(emac_hal_context_t *hal, uint8_t **buffs, uint32_t *lengths, uint32_t inbuffs_cnt);

/**
 * @brief Allocate buffer with size equal to actually received Ethernet frame size.
 *
 * @param[in] hal EMAC HAL context infostructure
 * @param[in, out] size as an input defines maximum size of buffer to be allocated. As an output, indicates actual size of received
 *                      Ethernet frame which is waiting to be processed. Returned size may be 0 when there is no waiting frame.
 *
 * @note If maximum allowed size of buffer to be allocated is less than actual size of received Ethernet frame, the buffer
 *       is allocated with that limit and the frame will be truncated by emac_hal_receive_frame.
 *
 * @return Pointer to allocated buffer
 *         NULL when allocation fails or when there is no waiting Ethernet frame
 */
uint8_t *emac_hal_alloc_recv_buf(emac_hal_context_t *hal, uint32_t *size);

/**
 * @brief Copy received Ethernet frame from EMAC DMA memory space to application.
 *
 * @param[in] hal EMAC HAL context infostructure
 * @param[in] buf buffer into which the Ethernet frame is to be copied
 * @param[in] size buffer size. When buffer was allocated by ::emac_hal_alloc_recv_buf, this parameter needs to be set
 *                 to EMAC_HAL_BUF_SIZE_AUTO
 * @param[out] frames_remain number of frames remaining to be processed
 * @param[out] free_desc muber of free DMA Rx descriptors
 *
 * @return number of copied bytes when success
 *         0 when there is no waiting Ethernet frame or on error
 *
 * @note FCS field is never copied
 * @note If buffer size is less than actual size of received Ethernet frame, the frame will be truncated.
 * @note When this function is called with EMAC_HAL_BUF_SIZE_AUTO size parameter, buffer needs to be allocated by
 *       ::emac_hal_alloc_recv_buf function at first.
 */
uint32_t emac_hal_receive_frame(emac_hal_context_t *hal, uint8_t *buf, uint32_t size, uint32_t *frames_remain, uint32_t *free_desc);

uint32_t emac_hal_flush_recv_frame(emac_hal_context_t *hal, uint32_t *frames_remain, uint32_t *free_desc);

void emac_hal_enable_flow_ctrl(emac_hal_context_t *hal, bool enable);

static inline uint32_t emac_hal_get_intr_enable_status(emac_hal_context_t *hal)
{
    return emac_ll_get_intr_enable_status(hal->dma_regs);
}

static inline uint32_t emac_hal_get_intr_status(emac_hal_context_t *hal)
{
    return emac_ll_get_intr_status(hal->dma_regs);
}

static inline void emac_hal_clear_corresponding_intr(emac_hal_context_t *hal, uint32_t bits)
{
    emac_ll_clear_corresponding_intr(hal->dma_regs, bits);
}

static inline void emac_hal_clear_all_intr(emac_hal_context_t *hal)
{
    emac_ll_clear_all_pending_intr(hal->dma_regs);
}

#endif  // SOC_EMAC_SUPPORTED

#ifdef __cplusplus
}
#endif
