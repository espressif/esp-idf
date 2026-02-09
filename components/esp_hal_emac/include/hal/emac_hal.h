/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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
 * @brief Macros to check descriptors datatype size
*/
#define STR(s) #s
#define TYPE_SIZE_ERR_MSG(DATATYPE, SIZE)  #DATATYPE " should occupy " STR(SIZE) " bytes in memory"
#define ASSERT_TYPE_SIZE(DATATYPE, SIZE) ESP_STATIC_ASSERT(sizeof(DATATYPE) == SIZE, TYPE_SIZE_ERR_MSG(DATATYPE, SIZE))

#if SOC_IS(ESP32P4)
// Descriptor must be 64B aligned for ESP32P4 due to cache arrangement
#define EMAC_HAL_DMA_DESC_SIZE                              (64)
#else
#define EMAC_HAL_DMA_DESC_SIZE                              (32)
#endif

/* DMA descriptor control bits */
#define EMAC_HAL_TDES0_INTR_ON_COMPLET                      (1 << 30)
#define EMAC_HAL_TDES0_CRC_APPEND_DISABLE                   (1 << 27)
#define EMAC_HAL_TDES0_PAD_DISABLE                          (1 << 26)
#define EMAC_HAL_TDES0_TX_TS_ENABLE                         (1 << 25)
#define EMAC_HAL_TDES0_CRC_REPLACE_CTRL                     (1 << 24)
#define EMAC_HAL_TDES0_IP_CRC_INSERT_HDR                    (1 << 22)
#define EMAC_HAL_TDES0_IP_CRC_INSERT_HDR_PAYLOAD            (2 << 22)
#define EMAC_HAL_TDES0_IP_CRC_INSERT_HDR_PAYLOAD_PSEUDO     (3 << 22)
#define EMAC_HAL_TDES0_VLAN_REMOVE                          (1 << 18)
#define EMAC_HAL_TDES0_VLAN_INSERT                          (2 << 18)
#define EMAC_HAL_TDES0_VLAN_REPLACE                         (3 << 18)

#define EMAC_HAL_TDES0_IP_CRC_INSERT_DISABLE_MASK           (3 << 22)
#define EMAC_HAL_TDES0_VLAN_INSERT_DISABLE_MASK             (3 << 18)

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
            uint32_t TransmitTimestampEnable : 1; /*!< Enable IEEE1588 hardware timestamping */
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

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    // descriptor must be aligned (due to cache arrangement)
    uint8_t CacheAlign[EMAC_HAL_DMA_DESC_SIZE - 32]; // 32 is size of EMAC DMA descriptor without alignment
#endif
} eth_dma_tx_descriptor_t;
ASSERT_TYPE_SIZE(eth_dma_tx_descriptor_t, EMAC_HAL_DMA_DESC_SIZE);

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

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    // descriptor must be aligned (due to cache arrangement)
    uint8_t CacheAlign[EMAC_HAL_DMA_DESC_SIZE - 32]; // 32 is size of EMAC DMA descriptor without alignment
#endif
} eth_dma_rx_descriptor_t;

ASSERT_TYPE_SIZE(eth_dma_rx_descriptor_t, EMAC_HAL_DMA_DESC_SIZE);

typedef struct emac_mac_dev_s *emac_mac_soc_regs_t;
typedef struct emac_dma_dev_s *emac_dma_soc_regs_t;
#if SOC_IS(ESP32)
typedef struct emac_ext_dev_s *emac_ext_soc_regs_t;
#else
typedef void *emac_ext_soc_regs_t;
#endif
#if SOC_EMAC_IEEE1588V2_SUPPORTED
typedef struct emac_ptp_dev_s *emac_ptp_soc_regs_t;
#endif

typedef struct {
    emac_mac_soc_regs_t mac_regs;
    emac_dma_soc_regs_t dma_regs;
    emac_ext_soc_regs_t ext_regs;
#if SOC_EMAC_IEEE1588V2_SUPPORTED
    emac_ptp_soc_regs_t ptp_regs;
#endif
} emac_hal_context_t;

/**
 * @brief EMAC related configuration
 */
typedef struct {
    eth_mac_dma_burst_len_t dma_burst_len;  /*!< eth-type enum of chosen dma burst-len */
} emac_hal_dma_config_t;

#if SOC_EMAC_IEEE1588V2_SUPPORTED
/**
 * @brief EMAC PTP configuration parameters
 */
typedef struct {
    eth_mac_ptp_update_method_t upd_method;
    eth_mac_ptp_roll_type_t roll;
    float ptp_clk_src_period_ns;         /*!< 1/ptp_ref_clk */
    float ptp_req_accuracy_ns;           /*!< required PTP accuracy in ns, must be greater than clk_src period */
} emac_hal_ptp_config_t;
#endif

void emac_hal_init(emac_hal_context_t *hal);

#define emac_hal_get_phy_intf(hal) emac_ll_get_phy_intf((hal)->ext_regs)

#define emac_hal_clock_enable_mii(hal) emac_ll_clock_enable_mii((hal)->ext_regs)

#define emac_hal_clock_enable_rmii_input(hal) emac_ll_clock_enable_rmii_input((hal)->ext_regs)

#if SOC_IS(ESP32P4)
#define emac_hal_clock_rmii_rx_tx_div(hal, div) emac_ll_clock_rmii_rx_tx_div((hal)->ext_regs, div)
#endif // SOC_IS(ESP32P4)

#define emac_hal_clock_enable_rmii_output(hal) emac_ll_clock_enable_rmii_output((hal)->ext_regs)

#define emac_hal_reset(hal) emac_ll_reset((hal)->dma_regs)

#define emac_hal_is_reset_done(hal) emac_ll_is_reset_done((hal)->dma_regs)

void emac_hal_find_set_closest_csr_clock_range(emac_hal_context_t *hal, int mdc_freq_hz, int freq_hz);

void emac_hal_set_csr_clock_range(emac_hal_context_t *hal, int freq);

void emac_hal_init_mac_default(emac_hal_context_t *hal);

void emac_hal_init_dma_default(emac_hal_context_t *hal, emac_hal_dma_config_t *hal_config);

#define emac_hal_set_speed(hal, speed) emac_ll_set_port_speed((hal)->mac_regs, speed)

#define emac_hal_set_duplex(hal, duplex) emac_ll_set_duplex((hal)->mac_regs, duplex)

#define emac_hal_set_promiscuous(hal, enable) emac_ll_promiscuous_mode_enable((hal)->mac_regs, enable)

#define emac_hal_pass_all_multicast_enable(hal, enable) emac_ll_pass_all_multicast_enable((hal)->mac_regs, enable)

/**
 * @brief Send MAC-CTRL frames to peer (EtherType=0x8808, opcode=0x0001, dest_addr=MAC-specific-ctrl-proto-01 (01:80:c2:00:00:01))
 */
#define emac_hal_send_pause_frame(hal, enable) emac_ll_pause_frame_enable((hal)->ext_regs, enable)

#define emac_hal_is_mii_busy(hal) emac_ll_is_mii_busy((hal)->mac_regs)

void emac_hal_set_phy_cmd(emac_hal_context_t *hal, uint32_t phy_addr, uint32_t phy_reg, bool write);

#define emac_hal_set_phy_data(hal, reg_value) emac_ll_set_phy_data((hal)->mac_regs, reg_value)

#define emac_hal_get_phy_data(hal) emac_ll_get_phy_data((hal)->mac_regs)

void emac_hal_set_address(emac_hal_context_t *hal, uint8_t *mac_addr);

#define emac_hal_get_address(hal, mac_addr) emac_ll_get_addr((hal)->mac_regs, mac_addr)

esp_err_t emac_hal_add_addr_da_filter(emac_hal_context_t *hal, const uint8_t *mac_addr, uint8_t addr_num);

esp_err_t emac_hal_get_addr_da_filter(emac_hal_context_t *hal, uint8_t *mac_addr, uint8_t addr_num);

esp_err_t emac_hal_add_addr_da_filter_auto(emac_hal_context_t *hal, uint8_t *mac_addr);

esp_err_t emac_hal_rm_addr_da_filter(emac_hal_context_t *hal, const uint8_t *mac_addr, uint8_t addr_num);

esp_err_t emac_hal_rm_addr_da_filter_auto(emac_hal_context_t *hal, const uint8_t *mac_addr);

void emac_hal_clear_addr_da_filters(emac_hal_context_t *hal);
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
  *      wait and repeat the EMAC stop again.
 */
esp_err_t emac_hal_stop(emac_hal_context_t *hal);

void emac_hal_enable_flow_ctrl(emac_hal_context_t *hal, bool enable);

#define emac_hal_get_intr_enable_status(hal) emac_ll_get_intr_enable_status((hal)->dma_regs)

#define emac_hal_get_intr_status(hal) emac_ll_get_intr_status((hal)->dma_regs)

#define emac_hal_clear_corresponding_intr(hal, bits) emac_ll_clear_corresponding_intr((hal)->dma_regs, bits)

#define emac_hal_clear_all_intr(hal) emac_ll_clear_all_pending_intr((hal)->dma_regs)

void emac_hal_set_rx_tx_desc_addr(emac_hal_context_t *hal, eth_dma_rx_descriptor_t *rx_desc, eth_dma_tx_descriptor_t *tx_desc);

#define emac_hal_receive_poll_demand(hal) emac_ll_receive_poll_demand((hal)->dma_regs, 0)

#define emac_hal_transmit_poll_demand(hal) emac_ll_transmit_poll_demand((hal)->dma_regs, 0)

#define emac_hal_get_hw_feat(hal) emac_ll_get_hw_feat((hal)->dma_regs)

#if SOC_EMAC_IEEE1588V2_SUPPORTED
#define emac_hal_get_ts_status(hal) emac_ll_get_ts_status((hal)->ptp_regs);

#define emac_hal_clock_enable_ptp(hal, clk_src, enable) emac_ll_clock_enable_ptp((hal)->ext_regs, clk_src, enable);

/**
 * @brief Start Ethernet PTP timestamp for transmit and receive frames
 *
 * @param hal EMAC HAL context infostructure
 * @return
 *         - ESP_OK: on success
 *         - ESP_ERR_TIMEOUT: on PTP block is busy
 */
esp_err_t emac_hal_ptp_start(emac_hal_context_t *hal, const emac_hal_ptp_config_t *config);

/**
 * @brief Stop Ethernet PTP timestamp
 *
 * @param hal EMAC HAL context infostructure
 * @return
 *          Always return ESP_OK
 */
esp_err_t emac_hal_ptp_stop(emac_hal_context_t *hal);

/**
 * @brief Updates time stamp addend register relatively to the base value
 *
 * @param hal EMAC HAL context infostructure
 * @param adj_ppb Correction value in ppb(parts per billion) (adj*10^9).
 *            For example, if the crystal used is 5 Hz off, then this value should be 5000.
 * @return
 *         - ESP_OK: on success
 *         - ESP_ERR_INVALID_STATE: on PTP block is busy
 */
esp_err_t emac_hal_ptp_adj_inc(emac_hal_context_t *hal, int32_t adj_ppb);

/**
 * @brief Updates time stamp addend register relatively to the previous value
 *
 * @param hal EMAC HAL context infostructure
 * @param scale_factor scale factor with which the addend register value is updated
 * @return
 *         - ESP_OK: on success
 *         - ESP_ERR_INVALID_STATE: on PTP block is busy
 */
esp_err_t emac_hal_adj_freq_factor(emac_hal_context_t *hal, double ratio);

/**
 * @brief Adds or subtracts to the PTP system time.
 *
 * @param hal EMAC HAL context infostructure
 * @param off_sec the PTP Time update second value
 * @param off_nsec the PTP Time update nano-second value
 * @param sign specifies the PTP Time update value sign(true means positive, false means negative)
 * @return
 *        - ESP_OK: on success
 *        - ESP_ERR_INVALID_STATE: on waiting for previous update to end
 */
esp_err_t emac_hal_ptp_time_add(emac_hal_context_t *hal, uint32_t off_sec, uint32_t off_nsec, bool sign);

/**
 * @brief Initialize the PTP time base
 *
 * @param hal EMAC HAL context infostructure
 * @param seconds specifies the PTP Time init second value
 * @param nano_seconds specifies the PTP Time init nano-second value
 * @return
 *         - ESP_OK: on success,
 *         - ESP_ERR_INVALID_STATE: on waiting for previous init to end
 */
esp_err_t emac_hal_ptp_set_sys_time(emac_hal_context_t *hal, uint32_t seconds, uint32_t nano_seconds);

/**
 * @brief Get the current value of the system time maintained by the MAC
 *
 * @param hal EMAC HAL context infostructure
 * @param seconds get the PTP system time second value
 * @param nano_seconds get the PTP system time nano-second value
 * @return
 *          - ESP_OK: on success
 *          - ESP_ERR_INVALID_ARG: on invalid argument
 */
esp_err_t emac_hal_ptp_get_sys_time(emac_hal_context_t *hal, uint32_t *seconds, uint32_t *nano_seconds);

/**
 * @brief Set target time to trigger event when the system time exceeds the target time
 *
 * @param hal EMAC HAL context infostructure
 * @param seconds specifies the PTP target time second value
 * @param nano_seconds specifies the PTP target Time nano-second value
 * @return
 *          - ESP_OK on success, ESP_ERR_TIMEOUT on busy
 */
esp_err_t emac_hal_ptp_set_target_time(emac_hal_context_t *hal, uint32_t seconds, uint32_t nano_seconds);

/**
 * @brief Enable rx/tx timestamps for all Ethernet frames
 *
 * @param hal EMAC HAL context infostructure
 * @param enable timestamping for non-PTP Ethernet frames
 * @return
 *          - ESP_OK on success
 */
esp_err_t emac_hal_ptp_enable_ts4all(emac_hal_context_t *hal, bool enable);

/**
 * @brief Get timestamp from receive descriptor
 *
 * @param hal EMAC HAL context infostructure
 * @param rxdesc Pointer to receive descriptor
 * @param seconds Pointer to store seconds part of timestamp
 * @param nano_seconds Pointer to store nanoseconds part of timestamp
 *
 * @return
 *     - ESP_OK: On success
 *     - ESP_ERR_INVALID_STATE: Descriptor does not contain time stamp information (frame might be filtered)
 */
esp_err_t emac_hal_get_rxdesc_timestamp(emac_hal_context_t *hal, eth_dma_rx_descriptor_t *rxdesc, uint32_t *seconds, uint32_t *nano_seconds);

/**
 * @brief Get timestamp from transmit descriptor
 *
 * @param hal EMAC HAL context infostructure
 * @param txdesc Pointer to transmit descriptor
 * @param seconds Pointer to store seconds part of timestamp
 * @param nano_seconds Pointer to store nanoseconds part of timestamp
 *
 * @return
 *     - ESP_OK: On success
 *     - ESP_ERR_INVALID_STATE: descriptor is still owned by DMA or time stamp is not ready yet
 */
esp_err_t emac_hal_get_txdesc_timestamp(emac_hal_context_t *hal, eth_dma_tx_descriptor_t *txdesc, uint32_t *seconds, uint32_t *nano_seconds);

esp_err_t emac_hal_set_pps0_out_freq(emac_hal_context_t *hal, uint32_t freq_hz);

#endif // SOC_EMAC_IEEE1588V2_SUPPORTED
#endif  // SOC_EMAC_SUPPORTED

#ifdef __cplusplus
}
#endif
