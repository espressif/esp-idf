/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** EMAC_MACCONFIGURATION_REG register
 *  This is the operation mode register for the MAC
 */
#define EMAC_MACCONFIGURATION_REG (DR_REG_EMAC_BASE + 0x0)
/** EMAC_PRELEN : R/W; bitpos: [1:0]; default: 0;
 *  Preamble Length for Transmit frames These bits control the number of preamble bytes
 *  that are added to the beginning of every Transmit frame The preamble reduction
 *  occurs only when the MAC is operating in the fullduplex mode 2'b00: 7 bytes of
 *  preamble 2'b01: 5 bytes of preamble 2'b10: 3 bytes of preamble 2'b11: Reserved
 */
#define EMAC_PRELEN    0x00000003U
#define EMAC_PRELEN_M  (EMAC_PRELEN_V << EMAC_PRELEN_S)
#define EMAC_PRELEN_V  0x00000003U
#define EMAC_PRELEN_S  0
/** EMAC_RE : R/W; bitpos: [2]; default: 0;
 *  Receiver Enable When this bit is set, the receiver state machine of the MAC is
 *  enabled for receiving frames from the GMII or MII When this bit is reset, the MAC
 *  receive state machine is disabled after the completion of the reception of the
 *  current frame, and does not receive any further frames from the GMII or MII
 */
#define EMAC_RE    (BIT(2))
#define EMAC_RE_M  (EMAC_RE_V << EMAC_RE_S)
#define EMAC_RE_V  0x00000001U
#define EMAC_RE_S  2
/** EMAC_TE : R/W; bitpos: [3]; default: 0;
 *  Transmitter Enable When this bit is set, the transmit state machine of the MAC is
 *  enabled for transmission on the GMII or MII When this bit is reset, the MAC
 *  transmit state machine is disabled after the completion of the transmission of the
 *  current frame, and does not transmit any further frames
 */
#define EMAC_TE    (BIT(3))
#define EMAC_TE_M  (EMAC_TE_V << EMAC_TE_S)
#define EMAC_TE_V  0x00000001U
#define EMAC_TE_S  3
/** EMAC_DC : R/W; bitpos: [4]; default: 0;
 *  Deferral Check When this bit is set, the deferral check function is enabled in the
 *  MAC The MAC issues a Frame Abort status, along with the excessive deferral error
 *  bit set in the transmit frame status, when the transmit state machine is deferred
 *  for more than 24,288 bit times in the 10 or 100 Mbps mode If the MAC is configured
 *  for 1000 Mbps operation or if the Jumbo frame mode is enabled in the 10 or 100 Mbps
 *  mode, the threshold for deferral is 155,680 bits times Deferral begins when the
 *  transmitter is ready to transmit, but it is prevented because of an active carrier
 *  sense signal _CRS_ on GMII or MII The defer time is not cumulative For example, if
 *  the transmitter defers for 10,000 bit times because the CRS signal is active and
 *  then the CRS signal becomes inactive, the transmitter transmits and collision
 *  happens Because of collision, the transmitter needs to back off and then defer
 *  again after back off completion In such a scenario, the deferral timer is reset to
 *  0 and it is restarted When this bit is reset, the deferral check function is
 *  disabled and the MAC defers until the CRS signal goes inactive This bit is
 *  applicable only in the halfduplex mode and is reserved _RO_ in the fullduplexonly
 *  configuration
 */
#define EMAC_DC    (BIT(4))
#define EMAC_DC_M  (EMAC_DC_V << EMAC_DC_S)
#define EMAC_DC_V  0x00000001U
#define EMAC_DC_S  4
/** EMAC_BL : R/W; bitpos: [6:5]; default: 0;
 *  BackOff Limit The BackOff limit determines the random integer number _r_ of slot
 *  time delays _4,096 bit times for 1000 Mbps and 512 bit times for 10/100 Mbps_ for
 *  which the MAC waits before rescheduling a transmission attempt during retries after
 *  a collision This bit is applicable only in the halfduplex mode and is reserved _RO_
 *  in the fullduplexonly configuration 00: k= min _n, 10_ 01: k = min _n, 8_ 10: k =
 *  min _n, 4_ 11: k = min _n, 1_ where n = retransmission attempt The random integer r
 *  takes the value in the range 0 ≤ r < 2k
 */
#define EMAC_BL    0x00000003U
#define EMAC_BL_M  (EMAC_BL_V << EMAC_BL_S)
#define EMAC_BL_V  0x00000003U
#define EMAC_BL_S  5
/** EMAC_ACS : R/W; bitpos: [7]; default: 0;
 *  Automatic Pad or CRC Stripping When this bit is set, the MAC strips the Pad or FCS
 *  field on the incoming frames only if the value of the length field is less than
 *  1,536 bytes All received frames with length field greater than or equal to 1,536
 *  bytes are passed to the application without stripping the Pad or FCS field When
 *  this bit is reset, the MAC passes all incoming frames, without modifying them, to
 *  the Host Note: For information about how the settings of Bit 25 _CST_ and this bit
 *  impact the frame length, see Table 632
 */
#define EMAC_ACS    (BIT(7))
#define EMAC_ACS_M  (EMAC_ACS_V << EMAC_ACS_S)
#define EMAC_ACS_V  0x00000001U
#define EMAC_ACS_S  7
/** EMAC_LUD : R/W; bitpos: [8]; default: 0;
 *  Link Up or Down This bit indicates whether the link is up or down during the
 *  transmission of configuration in the RGMII, SGMII, or SMII interface: 0: Link Down
 *  1: Link Up This bit is reserved _RO with default value_ and is enabled when the
 *  RGMII, SGMII, or SMII interface is enabled during core configuration
 */
#define EMAC_LUD    (BIT(8))
#define EMAC_LUD_M  (EMAC_LUD_V << EMAC_LUD_S)
#define EMAC_LUD_V  0x00000001U
#define EMAC_LUD_S  8
/** EMAC_DR : R/W; bitpos: [9]; default: 0;
 *  Disable Retry When this bit is set, the MAC attempts only one transmission When a
 *  collision occurs on the GMII or MII interface, the MAC ignores the current frame
 *  transmission and reports a Frame Abort with excessive collision error in the
 *  transmit frame status When this bit is reset, the MAC attempts retries based on the
 *  settings of the BL field _Bits [6:5]_ This bit is applicable only in the halfduplex
 *  mode and is reserved _RO with default value_ in the fullduplexonly configuration
 */
#define EMAC_DR    (BIT(9))
#define EMAC_DR_M  (EMAC_DR_V << EMAC_DR_S)
#define EMAC_DR_V  0x00000001U
#define EMAC_DR_S  9
/** EMAC_IPC : R/W; bitpos: [10]; default: 0;
 *  Checksum Offload When this bit is set, the MAC calculates the 16bit one’s
 *  complement of the one’s complement sum of all received Ethernet frame payloads It
 *  also checks whether the IPv4 Header checksum _assumed to be bytes 2526 or 2930
 *  _VLAN tagged_ of the received Ethernet frame_ is correct for the received frame and
 *  gives the status in the receive status word The MAC also appends the 16bit checksum
 *  calculated for the IP header datagram payload _bytes after the IPv4 header_ and
 *  appends it to the Ethernet frame transferred to the application _when Type 2 COE is
 *  deselected_ When this bit is reset, this function is disabled When Type 2 COE is
 *  selected, this bit, when set, enables the IPv4 header checksum checking and IPv4 or
 *  IPv6 TCP, UDP, or ICMP payload checksum checking When this bit is reset, the COE
 *  function in the receiver is disabled and the corresponding PCE and IP HCE status
 *  bits _see Table 310 on page 138_ are always cleared If the IP Checksum Offload
 *  feature is not enabled during core configuration, this bit is reserved _RO with
 *  default value_
 */
#define EMAC_IPC    (BIT(10))
#define EMAC_IPC_M  (EMAC_IPC_V << EMAC_IPC_S)
#define EMAC_IPC_V  0x00000001U
#define EMAC_IPC_S  10
/** EMAC_DM : R/W; bitpos: [11]; default: 0;
 *  Duplex Mode When this bit is set, the MAC operates in the fullduplex mode where it
 *  can transmit and receive simultaneously This bit is RO with default value of 1'b1
 *  in the fullduplexonly configuration
 */
#define EMAC_DM    (BIT(11))
#define EMAC_DM_M  (EMAC_DM_V << EMAC_DM_S)
#define EMAC_DM_V  0x00000001U
#define EMAC_DM_S  11
/** EMAC_LM : R/W; bitpos: [12]; default: 0;
 *  Loopback Mode When this bit is set, the MAC operates in the loopback mode at GMII
 *  or MII The _G_MII Receive clock input _clk_rx_i_ is required for the loopback to
 *  work properly, because the Transmit clock is not loopedback internally
 */
#define EMAC_LM    (BIT(12))
#define EMAC_LM_M  (EMAC_LM_V << EMAC_LM_S)
#define EMAC_LM_V  0x00000001U
#define EMAC_LM_S  12
/** EMAC_DO : R/W; bitpos: [13]; default: 0;
 *  Disable Receive Own When this bit is set, the MAC disables the reception of frames
 *  when the phy_txen_o is asserted in the halfduplex mode When this bit is reset, the
 *  MAC receives all packets that are given by the PHY while transmitting This bit is
 *  not applicable if the MAC is operating in the fullduplex mode This bit is reserved
 *  _RO with default value_ if the MAC is configured for the fullduplexonly operation
 */
#define EMAC_DO    (BIT(13))
#define EMAC_DO_M  (EMAC_DO_V << EMAC_DO_S)
#define EMAC_DO_V  0x00000001U
#define EMAC_DO_S  13
/** EMAC_FES : R/W; bitpos: [14]; default: 0;
 *  Speed This bit selects the speed in the MII, RMII, SMII, RGMII, SGMII, or RevMII
 *  interface: 0: 10 Mbps 1: 100 Mbps This bit is reserved _RO_ by default and is
 *  enabled only when the parameter SPEED_SELECT = Enabled This bit generates link
 *  speed encoding when Bit 24 _TC_ is set in the RGMII, SMII, or SGMII mode This bit
 *  is always enabled for RGMII, SGMII, SMII, or RevMII interface In configurations
 *  with RGMII, SGMII, SMII, or RevMII interface, this bit is driven as an output
 *  signal _mac_speed_o[0]_ to reflect the value of this bit in the mac_speed_o signal
 *  In configurations with RMII, MII, or GMII interface, you can optionally drive this
 *  bit as an output signal _mac_speed_o[0]_ to reflect its value in the mac_speed_o
 *  signal
 */
#define EMAC_FES    (BIT(14))
#define EMAC_FES_M  (EMAC_FES_V << EMAC_FES_S)
#define EMAC_FES_V  0x00000001U
#define EMAC_FES_S  14
/** EMAC_PS : R/W; bitpos: [15]; default: 0;
 *  Port Select This bit selects the Ethernet line speed 0: For 1000 Mbps operations 1:
 *  For 10 or 100 Mbps operations In 10 or 100 Mbps operations, this bit, along with
 *  FES bit, selects the exact line speed In the 10/100 Mbpsonly _always 1_ or 1000
 *  Mbpsonly _always 0_ configurations, this bit is readonly with the appropriate value
 *  In default 10/100/1000 Mbps configuration, this bit is R_W The mac_portselect_o or
 *  mac_speed_o[1] signal reflects the value of this bit
 */
#define EMAC_PS    (BIT(15))
#define EMAC_PS_M  (EMAC_PS_V << EMAC_PS_S)
#define EMAC_PS_V  0x00000001U
#define EMAC_PS_S  15
/** EMAC_DCRS : R/W; bitpos: [16]; default: 0;
 *  Disable Carrier Sense During Transmission When set high, this bit makes the MAC
 *  transmitter ignore the _G_MII CRS signal during frame transmission in the
 *  halfduplex mode This request results in no errors generated because of Loss of
 *  Carrier or No Carrier during such transmission When this bit is low, the MAC
 *  transmitter generates such errors because of Carrier Sense and can even abort the
 *  transmissions This bit is reserved _and RO_ in the fullduplexonly configurations
 */
#define EMAC_DCRS    (BIT(16))
#define EMAC_DCRS_M  (EMAC_DCRS_V << EMAC_DCRS_S)
#define EMAC_DCRS_V  0x00000001U
#define EMAC_DCRS_S  16
/** EMAC_IFG : R/W; bitpos: [19:17]; default: 0;
 *  InterFrame Gap These bits control the minimum IFG between frames during
 *  transmission 000: 96 bit times 001: 88 bit times 010: 80 bit times  111: 40 bit
 *  times In the halfduplex mode, the minimum IFG can be configured only for 64 bit
 *  times _IFG = 100_ Lower values are not considered In the 1000Mbps mode, the minimum
 *  IFG supported is 64 bit times _and above_ in the EMACCORE configuration and 80 bit
 *  times _and above_ in other configurations When a JAM pattern is being transmitted
 *  because of backpressure activation, the MAC does not consider the minimum IFG
 */
#define EMAC_IFG    0x00000007U
#define EMAC_IFG_M  (EMAC_IFG_V << EMAC_IFG_S)
#define EMAC_IFG_V  0x00000007U
#define EMAC_IFG_S  17
/** EMAC_JE : R/W; bitpos: [20]; default: 0;
 *  Jumbo Frame Enable When this bit is set, the MAC allows Jumbo frames of 9,018 bytes
 *  _9,022 bytes for VLAN tagged frames_ without reporting a giant frame error in the
 *  receive frame status
 */
#define EMAC_JE    (BIT(20))
#define EMAC_JE_M  (EMAC_JE_V << EMAC_JE_S)
#define EMAC_JE_V  0x00000001U
#define EMAC_JE_S  20
/** EMAC_BE : R/W; bitpos: [21]; default: 0;
 *  Frame Burst Enable When this bit is set, the MAC allows frame bursting during
 *  transmission in the GMII halfduplex mode This bit is reserved _and RO_ in the
 *  10/100 Mbps only or fullduplexonly configurations
 */
#define EMAC_BE    (BIT(21))
#define EMAC_BE_M  (EMAC_BE_V << EMAC_BE_S)
#define EMAC_BE_V  0x00000001U
#define EMAC_BE_S  21
/** EMAC_JD : R/W; bitpos: [22]; default: 0;
 *  Jabber Disable When this bit is set, the MAC disables the jabber timer on the
 *  transmitter The MAC can transfer frames of up to 16,383 bytes When this bit is
 *  reset, the MAC cuts off the transmitter if the application sends out more than
 *  2,048 bytes of data _10,240 if JE is set high_ during transmission
 */
#define EMAC_JD    (BIT(22))
#define EMAC_JD_M  (EMAC_JD_V << EMAC_JD_S)
#define EMAC_JD_V  0x00000001U
#define EMAC_JD_S  22
/** EMAC_WD : R/W; bitpos: [23]; default: 0;
 *  Watchdog Disable When this bit is set, the MAC disables the watchdog timer on the
 *  receiver The MAC can receive frames of up to 16,383 bytes When this bit is reset,
 *  the MAC does not allow a receive frame which more than 2,048 bytes _10,240 if JE is
 *  set high_ or the value programmed in Register 55 _Watchdog Timeout Register_ The
 *  MAC cuts off any bytes received after the watchdog limit number of bytes
 */
#define EMAC_WD    (BIT(23))
#define EMAC_WD_M  (EMAC_WD_V << EMAC_WD_S)
#define EMAC_WD_V  0x00000001U
#define EMAC_WD_S  23
/** EMAC_TC : R/W; bitpos: [24]; default: 0;
 *  Transmit Configuration in RGMII, SGMII, or SMII When set, this bit enables the
 *  transmission of duplex mode, link speed, and link up or down information to the PHY
 *  in the RGMII, SMII, or SGMII port When this bit is reset, no such information is
 *  driven to the PHY This bit is reserved _and RO_ if the RGMII, SMII, or SGMII PHY
 *  port is not selected during core configuration The details of this feature are
 *  explained in the following sections: “Reduced Gigabit Media Independent Interface”
 *  on page 249 “Serial Media Independent Interface” on page 245 “Serial Gigabit Media
 *  Independent Interface” on page 257
 */
#define EMAC_TC    (BIT(24))
#define EMAC_TC_M  (EMAC_TC_V << EMAC_TC_S)
#define EMAC_TC_V  0x00000001U
#define EMAC_TC_S  24
/** EMAC_CST : R/W; bitpos: [25]; default: 0;
 *  CRC Stripping for Type Frames When this bit is set, the last 4 bytes _FCS_ of all
 *  frames of Ether type _Length/Type field greater than or equal to 1,536_ are
 *  stripped and dropped before forwarding the frame to the application This function
 *  is not valid when the IP Checksum Engine _Type 1_ is enabled in the MAC receiver
 *  This function is valid when Type 2 Checksum Offload Engine is enabled Note: For
 *  information about how the settings of Bit 7 _ACS_ and this bit impact the frame
 *  length, see Table 632
 */
#define EMAC_CST    (BIT(25))
#define EMAC_CST_M  (EMAC_CST_V << EMAC_CST_S)
#define EMAC_CST_V  0x00000001U
#define EMAC_CST_S  25
/** EMAC_SFTERR : R/W; bitpos: [26]; default: 0;
 *  SMII Force Transmit Error When set, this bit indicates to the PHY to force a
 *  transmit error in the SMII frame being transmitted This bit is reserved if the SMII
 *  PHY port is not selected during core configuration
 */
#define EMAC_SFTERR    (BIT(26))
#define EMAC_SFTERR_M  (EMAC_SFTERR_V << EMAC_SFTERR_S)
#define EMAC_SFTERR_V  0x00000001U
#define EMAC_SFTERR_S  26
/** EMAC_TWOKPE : R/W; bitpos: [27]; default: 0;
 *  IEEE 8023as Support for 2K Packets When set, the MAC considers all frames, with up
 *  to 2,000 bytes length, as normal packets When Bit 20 _JE_ is not set, the MAC
 *  considers all received frames of size more than 2K bytes as Giant frames When this
 *  bit is reset and Bit 20 _JE_ is not set, the MAC considers all received frames of
 *  size more than 1,518 bytes _1,522 bytes for tagged_ as Giant frames When Bit 20 is
 *  set, setting this bit has no effect on Giant Frame status For more information
 *  about how the setting of this bit and Bit 20 impact the Giant frame status, see
 *  Table 631
 */
#define EMAC_TWOKPE    (BIT(27))
#define EMAC_TWOKPE_M  (EMAC_TWOKPE_V << EMAC_TWOKPE_S)
#define EMAC_TWOKPE_V  0x00000001U
#define EMAC_TWOKPE_S  27
/** EMAC_SARC : R/W; bitpos: [30:28]; default: 0;
 *  Source Address Insertion or Replacement Control This field controls the source
 *  address insertion or replacement for all transmitted frames Bit 30 specifies which
 *  MAC Address register _0 or 1_ is used for source address insertion or replacement
 *  based on the values of Bits [29:28]: 2'b0x: The input signals mti_sa_ctrl_i and
 *  ati_sa_ctrl_i control the SA field generation 2'b10: 	If Bit 30 is set to 0, the
 *  MAC inserts the content of the MAC Address 0 registers _registers 16 and 17_ in the
 *  SA field of all transmitted frames 	If Bit 30 is set to 1 and the Enable MAC
 *  Address Register 1 option is selected during core configuration, the MAC inserts
 *  the content of the MAC Address 1 registers _registers 18 and 19_ in the SA field of
 *  all transmitted frames 2'b11: 	If Bit 30 is set to 0, the MAC replaces the content
 *  of the MAC Address 0 registers _registers 16 and 17_ in the SA field of all
 *  transmitted frames 	If Bit 30 is set to 1 and the Enable MAC Address Register 1
 *  option is selected during core configuration, the MAC replaces the content of the
 *  MAC Address 1 registers _registers 18 and 19_ in the SA field of all transmitted
 *  frames Note: Changes to this field take effect only on the start of a frame If you
 *  write this register field when a frame is being transmitted, only the subsequent
 *  frame can use the updated value, that is, the current frame does not use the
 *  updated value These bits are reserved and RO when the Enable SA, VLAN, and CRC
 *  Insertion on TX feature is not selected during core configuration
 */
#define EMAC_SARC    0x00000007U
#define EMAC_SARC_M  (EMAC_SARC_V << EMAC_SARC_S)
#define EMAC_SARC_V  0x00000007U
#define EMAC_SARC_S  28

/** EMAC_MACFRAMEFILTER_REG register
 *  Contains the frame filtering controls
 */
#define EMAC_MACFRAMEFILTER_REG (DR_REG_EMAC_BASE + 0x4)
/** EMAC_PROMISCUOUS_MODE : R/W; bitpos: [0]; default: 0;
 *  Promiscuous Mode When this bit is set, the Address Filter module passes all
 *  incoming frames irrespective of the destination or source address The SA or DA
 *  Filter Fails status bits of the Receive Status Word are always cleared when PR is
 *  set
 */
#define EMAC_PROMISCUOUS_MODE    (BIT(0))
#define EMAC_PROMISCUOUS_MODE_M  (EMAC_PROMISCUOUS_MODE_V << EMAC_PROMISCUOUS_MODE_S)
#define EMAC_PROMISCUOUS_MODE_V  0x00000001U
#define EMAC_PROMISCUOUS_MODE_S  0
/** EMAC_HUC : R/W; bitpos: [1]; default: 0;
 *  Hash Unicast When set, the MAC performs destination address filtering of unicast
 *  frames according to the hash table When reset, the MAC performs a perfect
 *  destination address filtering for unicast frames, that is, it compares the DA field
 *  with the values programmed in DA registers If Hash Filter is not selected during
 *  core configuration, this bit is reserved _and RO_
 */
#define EMAC_HUC    (BIT(1))
#define EMAC_HUC_M  (EMAC_HUC_V << EMAC_HUC_S)
#define EMAC_HUC_V  0x00000001U
#define EMAC_HUC_S  1
/** EMAC_HMC : R/W; bitpos: [2]; default: 0;
 *  Hash Multicast When set, the MAC performs destination address filtering of received
 *  multicast frames according to the hash table When reset, the MAC performs a perfect
 *  destination address filtering for multicast frames, that is, it compares the DA
 *  field with the values programmed in DA registers If Hash Filter is not selected
 *  during core configuration, this bit is reserved _and RO_
 */
#define EMAC_HMC    (BIT(2))
#define EMAC_HMC_M  (EMAC_HMC_V << EMAC_HMC_S)
#define EMAC_HMC_V  0x00000001U
#define EMAC_HMC_S  2
/** EMAC_DAIF : R/W; bitpos: [3]; default: 0;
 *  DA Inverse Filtering When this bit is set, the Address Check block operates in
 *  inverse filtering mode for the DA address comparison for both unicast and multicast
 *  frames When reset, normal filtering of frames is performed
 */
#define EMAC_DAIF    (BIT(3))
#define EMAC_DAIF_M  (EMAC_DAIF_V << EMAC_DAIF_S)
#define EMAC_DAIF_V  0x00000001U
#define EMAC_DAIF_S  3
/** EMAC_PM : R/W; bitpos: [4]; default: 0;
 *  Pass All Multicast When set, this bit indicates that all received frames with a
 *  multicast destination address _first bit in the destination address field is '1'_
 *  are passed When reset, filtering of multicast frame depends on HMC bit
 */
#define EMAC_PM    (BIT(4))
#define EMAC_PM_M  (EMAC_PM_V << EMAC_PM_S)
#define EMAC_PM_V  0x00000001U
#define EMAC_PM_S  4
/** EMAC_DBF : R/W; bitpos: [5]; default: 0;
 *  Disable Broadcast Frames When this bit is set, the AFM module blocks all incoming
 *  broadcast frames In addition, it overrides all other filter settings When this bit
 *  is reset, the AFM module passes all received broadcast frames
 */
#define EMAC_DBF    (BIT(5))
#define EMAC_DBF_M  (EMAC_DBF_V << EMAC_DBF_S)
#define EMAC_DBF_V  0x00000001U
#define EMAC_DBF_S  5
/** EMAC_PCF : R/W; bitpos: [7:6]; default: 0;
 *  Pass Control Frames These bits control the forwarding of all control frames
 *  _including unicast and multicast Pause frames_ 00: MAC filters all control frames
 *  from reaching the application 01: MAC forwards all control frames except Pause
 *  frames to application even if they fail the Address filter 10: MAC forwards all
 *  control frames to application even if they fail the Address Filter 11: MAC forwards
 *  control frames that pass the Address Filter The following conditions should be true
 *  for the Pause frames processing: Condition 1 : The MAC is in the fullduplex mode
 *  and flow control is enabled by setting Bit 2 _RFE_ of Register 6 _Flow Control
 *  Register_ to 1 Condition 2 : The destination address _DA_ of the received frame
 *  matches the special multicast address or the MAC Address 0 when Bit 3 _UP_ of the
 *  Register 6 _Flow Control Register_ is set Condition 3: The Type field of the
 *  received frame is 0x8808 and the OPCODE field is 0x0001 Note: This field should be
 *  set to 01 only when the Condition 1 is true, that is, the MAC is programmed to
 *  operate in the fullduplex mode and the RFE bit is enabled Otherwise, the Pause
 *  frame filtering may be inconsistent When Condition 1 is false, the Pause frames are
 *  considered as generic control frames Therefore, to pass all control frames
 *  _including Pause frames_ when the fullduplex mode and flow control is not enabled,
 *  you should set the PCF field to 10 or 11 _as required by the application_
 */
#define EMAC_PCF    0x00000003U
#define EMAC_PCF_M  (EMAC_PCF_V << EMAC_PCF_S)
#define EMAC_PCF_V  0x00000003U
#define EMAC_PCF_S  6
/** EMAC_SAIF : R/W; bitpos: [8]; default: 0;
 *  SA Inverse Filtering When this bit is set, the Address Check block operates in
 *  inverse filtering mode for the SA address comparison The frames whose SA matches
 *  the SA registers are marked as failing the SA Address filter When this bit is
 *  reset, frames whose SA does not match the SA registers are marked as failing the SA
 *  Address filter
 */
#define EMAC_SAIF    (BIT(8))
#define EMAC_SAIF_M  (EMAC_SAIF_V << EMAC_SAIF_S)
#define EMAC_SAIF_V  0x00000001U
#define EMAC_SAIF_S  8
/** EMAC_SAF : R/W; bitpos: [9]; default: 0;
 *  Source Address Filter Enable When this bit is set, the MAC compares the SA field of
 *  the received frames with the values programmed in the enabled SA registers If the
 *  comparison fails, the MAC drops the frame When this bit is reset, the MAC forwards
 *  the received frame to the application with updated SAF bit of the Rx Status
 *  depending on the SA address comparison Note: According to the IEEE specification,
 *  Bit 47 of the SA is reserved and set to 0 However, in DWC_EMAC, the MAC compares
 *  all 48 bits The software driver should take this into consideration while
 *  programming the MAC address registers for SA
 */
#define EMAC_SAF    (BIT(9))
#define EMAC_SAF_M  (EMAC_SAF_V << EMAC_SAF_S)
#define EMAC_SAF_V  0x00000001U
#define EMAC_SAF_S  9
/** EMAC_HPF : R/W; bitpos: [10]; default: 0;
 *  Hash or Perfect Filter When this bit is set, it configures the address filter to
 *  pass a frame if it matches either the perfect filtering or the hash filtering as
 *  set by the HMC or HUC bits When this bit is low and the HUC or HMC bit is set, the
 *  frame is passed only if it matches the Hash filter This bit is reserved _and RO_ if
 *  the Hash filter is not selected during core configuration
 */
#define EMAC_HPF    (BIT(10))
#define EMAC_HPF_M  (EMAC_HPF_V << EMAC_HPF_S)
#define EMAC_HPF_V  0x00000001U
#define EMAC_HPF_S  10
/** EMAC_VTFE : R/W; bitpos: [16]; default: 0;
 *  VLAN Tag Filter Enable When set, this bit enables the MAC to drop VLAN tagged
 *  frames that do not match the VLAN Tag comparison When reset, the MAC forwards all
 *  frames irrespective of the match status of the VLAN Tag
 */
#define EMAC_VTFE    (BIT(16))
#define EMAC_VTFE_M  (EMAC_VTFE_V << EMAC_VTFE_S)
#define EMAC_VTFE_V  0x00000001U
#define EMAC_VTFE_S  16
/** EMAC_IPFE : R/W; bitpos: [20]; default: 0;
 *  Layer 3 and Layer 4 Filter Enable When set, this bit enables the MAC to drop frames
 *  that do not match the enabled Layer 3 and Layer 4 filters If Layer 3 or Layer 4
 *  filters are not enabled for matching, this bit does not have any effect When reset,
 *  the MAC forwards all frames irrespective of the match status of the Layer 3 and
 *  Layer 4 fields If the Layer 3 and Layer 4 Filtering feature is not selected during
 *  core configuration, this bit is reserved _RO with default value_
 */
#define EMAC_IPFE    (BIT(20))
#define EMAC_IPFE_M  (EMAC_IPFE_V << EMAC_IPFE_S)
#define EMAC_IPFE_V  0x00000001U
#define EMAC_IPFE_S  20
/** EMAC_DNTU : R/W; bitpos: [21]; default: 0;
 *  Drop nonTCP/UDP over IP Frames When set, this bit enables the MAC to drop the
 *  nonTCP or UDP over IP frames The MAC forward only those frames that are processed
 *  by the Layer 4 filter When reset, this bit enables the MAC to forward all nonTCP or
 *  UDP over IP frames If the Layer 3 and Layer 4 Filtering feature is not selected
 *  during core configuration, this bit is reserved _RO with default value_
 */
#define EMAC_DNTU    (BIT(21))
#define EMAC_DNTU_M  (EMAC_DNTU_V << EMAC_DNTU_S)
#define EMAC_DNTU_V  0x00000001U
#define EMAC_DNTU_S  21
/** EMAC_RA : R/W; bitpos: [31]; default: 0;
 *  Receive All When this bit is set, the MAC Receiver module passes all received
 *  frames, irrespective of whether they pass the address filter or not, to the
 *  Application The result of the SA or DA filtering is updated _pass or fail_ in the
 *  corresponding bits in the Receive Status Word When this bit is reset, the Receiver
 *  module passes only those frames to the Application that pass the SA or DA address
 *  filter
 */
#define EMAC_RA    (BIT(31))
#define EMAC_RA_M  (EMAC_RA_V << EMAC_RA_S)
#define EMAC_RA_V  0x00000001U
#define EMAC_RA_S  31

/** EMAC_HASHTABLEHIGH_REG register
 *  Contains the higher 32 bits of the Multicast Hash table This register is present
 *  only when you select the 64bit Hash filter function in coreConsultant _See Table 79_
 */
#define EMAC_HASHTABLEHIGH_REG (DR_REG_EMAC_BASE + 0x8)
/** EMAC_HTH : R/W; bitpos: [31:0]; default: 0;
 *  Hash Table High This field contains the upper 32 bits of the Hash table
 */
#define EMAC_HTH    0xFFFFFFFFU
#define EMAC_HTH_M  (EMAC_HTH_V << EMAC_HTH_S)
#define EMAC_HTH_V  0xFFFFFFFFU
#define EMAC_HTH_S  0

/** EMAC_HASHTABLELOW_REG register
 *  Contains the lower 32 bits of the Multicast Hash table This register is present
 *  only when you select the Hash filter function in coreConsultant _See Table 79_
 */
#define EMAC_HASHTABLELOW_REG (DR_REG_EMAC_BASE + 0xc)
/** EMAC_HTL : R/W; bitpos: [31:0]; default: 0;
 *  Hash Table Low This field contains the lower 32 bits of the Hash table
 */
#define EMAC_HTL    0xFFFFFFFFU
#define EMAC_HTL_M  (EMAC_HTL_V << EMAC_HTL_S)
#define EMAC_HTL_V  0xFFFFFFFFU
#define EMAC_HTL_S  0

/** EMAC_GMIIADDRESS_REG register
 *  Controls the management cycles to an external PHY This register is present only
 *  when you select the Station Management _MDIO_ feature in coreConsultant _See Table
 *  726_
 */
#define EMAC_GMIIADDRESS_REG (DR_REG_EMAC_BASE + 0x10)
/** EMAC_GB : R/W1S; bitpos: [0]; default: 0;
 *  GMII Busy This bit should read logic 0 before writing to Register 4 and Register 5
 *  During a PHY or RevMII register access, the software sets this bit to 1’b1 to
 *  indicate that a Read or Write access is in progress Register 5 is invalid until
 *  this bit is cleared by the MAC Therefore, Register 5 _GMII Data_ should be kept
 *  valid until the MAC clears this bit during a PHY Write operation Similarly for a
 *  read operation, the contents of Register 5 are not valid until this bit is cleared
 *  The subsequent read or write operation should happen only after the previous
 *  operation is complete Because there is no acknowledgment from the PHY to MAC after
 *  a read or write operation is completed, there is no change in the functionality of
 *  this bit even when the PHY is not present
 */
#define EMAC_GB    (BIT(0))
#define EMAC_GB_M  (EMAC_GB_V << EMAC_GB_S)
#define EMAC_GB_V  0x00000001U
#define EMAC_GB_S  0
/** EMAC_GW : R/W; bitpos: [1]; default: 0;
 *  GMII Write When set, this bit indicates to the PHY or RevMII that this is a Write
 *  operation using the GMII Data register If this bit is not set, it indicates that
 *  this is a Read operation, that is, placing the data in the GMII Data register
 */
#define EMAC_GW    (BIT(1))
#define EMAC_GW_M  (EMAC_GW_V << EMAC_GW_S)
#define EMAC_GW_V  0x00000001U
#define EMAC_GW_S  1
/** EMAC_CR : R/W; bitpos: [5:2]; default: 0;
 *  CSR Clock Range The CSR Clock Range selection determines the frequency of the MDC
 *  clock according to the CSR clock frequency used in your design The CSR clock
 *  corresponding to different EMAC configurations is given in Table 92 on page 564 The
 *  suggested range of CSR clock frequency applicable for each value _when Bit[5] = 0_
 *  ensures that the MDC clock is approximately between the frequency range 10 MHz25
 *  MHz 0000: The CSR clock frequency is 60100 MHz and the MDC clock frequency is CSR
 *  clock/42 0001: The CSR clock frequency is 100150 MHz and the MDC clock frequency is
 *  CSR clock/62 0010: The CSR clock frequency is 2035 MHz and the MDC clock frequency
 *  is CSR clock/16 0011: The CSR clock frequency is 3560 MHz and the MDC clock
 *  frequency is CSR clock/26 0100: The CSR clock frequency is 150250 MHz and the MDC
 *  clock frequency is CSR clock/102 0101: The CSR clock frequency is 250300 MHz and
 *  the MDC clock is CSR clock/124 0110, 0111: Reserved When Bit 5 is set, you can
 *  achieve higher frequency of the MDC clock than the frequency limit of 25 MHz
 *  _specified in the IEEE Std 8023_ and program a clock divider of lower value For
 *  example, when CSR clock is of 100 MHz frequency and you program these bits as 1010,
 *  then the resultant MDC clock is of 125 MHz which is outside the limit of IEEE 8023
 *  specified range Program the following values only if the interfacing chips support
 *  faster MDC clocks 1000: CSR clock/4 1001: CSR clock/6 1010: CSR clock/8 1011: CSR
 *  clock/10 1100: CSR clock/12 1101: CSR clock/14 1110: CSR clock/16 1111: CSR
 *  clock/18 These bits are not used for accessing RevMII These bits are readonly if
 *  the RevMII interface is selected as single PHY interface
 */
#define EMAC_CR    0x0000000FU
#define EMAC_CR_M  (EMAC_CR_V << EMAC_CR_S)
#define EMAC_CR_V  0x0000000FU
#define EMAC_CR_S  2
/** EMAC_GR : R/W; bitpos: [10:6]; default: 0;
 *  GMII Register These bits select the desired GMII register in the selected PHY
 *  device For RevMII, these bits select the desired CSR register in the RevMII
 *  Registers set
 */
#define EMAC_GR    0x0000001FU
#define EMAC_GR_M  (EMAC_GR_V << EMAC_GR_S)
#define EMAC_GR_V  0x0000001FU
#define EMAC_GR_S  6
/** EMAC_PA : R/W; bitpos: [15:11]; default: 0;
 *  Physical Layer Address This field indicates which of the 32 possible PHY devices
 *  are being accessed For RevMII, this field gives the PHY Address of the RevMII module
 */
#define EMAC_PA    0x0000001FU
#define EMAC_PA_M  (EMAC_PA_V << EMAC_PA_S)
#define EMAC_PA_V  0x0000001FU
#define EMAC_PA_S  11

/** EMAC_GMIIDATA_REG register
 *  Contains the data to be written to or read from the PHY register This register is
 *  present only when you select the Station Management _MDIO_ feature in
 *  coreConsultant _See Table 726_
 */
#define EMAC_GMIIDATA_REG (DR_REG_EMAC_BASE + 0x14)
/** EMAC_GD : R/W; bitpos: [15:0]; default: 0;
 *  GMII Data This field contains the 16bit data value read from the PHY or RevMII
 *  after a Management Read operation or the 16bit data value to be written to the PHY
 *  or RevMII before a Management Write operation
 */
#define EMAC_GD    0x0000FFFFU
#define EMAC_GD_M  (EMAC_GD_V << EMAC_GD_S)
#define EMAC_GD_V  0x0000FFFFU
#define EMAC_GD_S  0

/** EMAC_FLOWCONTROL_REG register
 *  Controls the generation of control frames
 */
#define EMAC_FLOWCONTROL_REG (DR_REG_EMAC_BASE + 0x18)
/** EMAC_FCB_BPA : R/W1S; bitpos: [0]; default: 0;
 *  Flow Control Busy or Backpressure Activate This bit initiates a Pause frame in the
 *  fullduplex mode and activates the backpressure function in the halfduplex mode if
 *  the TFE bit is set In the fullduplex mode, this bit should be read as 1'b0 before
 *  writing to the Flow Control register To initiate a Pause frame, the Application
 *  must set this bit to 1'b1 During a transfer of the Control Frame, this bit
 *  continues to be set to signify that a frame transmission is in progress After the
 *  completion of Pause frame transmission, the MAC resets this bit to 1'b0 The Flow
 *  Control register should not be written to until this bit is cleared In the
 *  halfduplex mode, when this bit is set _and TFE is set_, then backpressure is
 *  asserted by the MAC During backpressure, when the MAC receives a new frame, the
 *  transmitter starts sending a JAM pattern resulting in a collision This control
 *  register bit is logically ORed with the mti_flowctrl_i input signal for the
 *  backpressure function When the MAC is configured for the fullduplex mode, the BPA
 *  is automatically disabled
 */
#define EMAC_FCB_BPA    (BIT(0))
#define EMAC_FCB_BPA_M  (EMAC_FCB_BPA_V << EMAC_FCB_BPA_S)
#define EMAC_FCB_BPA_V  0x00000001U
#define EMAC_FCB_BPA_S  0
/** EMAC_TFE : R/W; bitpos: [1]; default: 0;
 *  Transmit Flow Control Enable In the fullduplex mode, when this bit is set, the MAC
 *  enables the flow control operation to transmit Pause frames When this bit is reset,
 *  the flow control operation in the MAC is disabled, and the MAC does not transmit
 *  any Pause frames In the halfduplex mode, when this bit is set, the MAC enables the
 *  backpressure operation When this bit is reset, the backpressure feature is disabled
 */
#define EMAC_TFE    (BIT(1))
#define EMAC_TFE_M  (EMAC_TFE_V << EMAC_TFE_S)
#define EMAC_TFE_V  0x00000001U
#define EMAC_TFE_S  1
/** EMAC_RECEIVE_FLOW_CTRL_E : R/W; bitpos: [2]; default: 0;
 *  Receive Flow Control Enable When this bit is set, the MAC decodes the received
 *  Pause frame and disables its transmitter for a specified _Pause_ time When this bit
 *  is reset, the decode function of the Pause frame is disabled
 */
#define EMAC_RECEIVE_FLOW_CTRL_E    (BIT(2))
#define EMAC_RECEIVE_FLOW_CTRL_E_M  (EMAC_RECEIVE_FLOW_CTRL_E_V << EMAC_RECEIVE_FLOW_CTRL_E_S)
#define EMAC_RECEIVE_FLOW_CTRL_E_V  0x00000001U
#define EMAC_RECEIVE_FLOW_CTRL_E_S  2
/** EMAC_UP : R/W; bitpos: [3]; default: 0;
 *  Unicast Pause Frame Detect A pause frame is processed when it has the unique
 *  multicast address specified in the IEEE Std 8023 When this bit is set, the MAC can
 *  also detect Pause frames with unicast address of the station This unicast address
 *  should be as specified in the MAC Address0 High Register and MAC Address0 Low
 *  Register When this bit is reset, the MAC only detects Pause frames with unique
 *  multicast address Note: The MAC does not process a Pause frame if the multicast
 *  address of received frame is different from the unique multicast address
 */
#define EMAC_UP    (BIT(3))
#define EMAC_UP_M  (EMAC_UP_V << EMAC_UP_S)
#define EMAC_UP_V  0x00000001U
#define EMAC_UP_S  3
/** EMAC_PLT : R/W; bitpos: [5:4]; default: 0;
 *  Pause Low Threshold This field configures the threshold of the Pause timer at which
 *  the input flow control signal mti_flowctrl_i _or sbd_flowctrl_i_ is checked for
 *  automatic retransmission of the Pause frame The threshold values should be always
 *  less than the Pause Time configured in Bits[31:16] For example, if PT = 100H _256
 *  slottimes_, and PLT = 01, then a second Pause frame is automatically transmitted if
 *  the mti_flowctrl_i signal is asserted at 228 _256  28_ slot times after the first
 *  Pause frame is transmitted The following list provides the threshold values for
 *  different values: 00: The threshold is Pause time minus 4 slot times _PT  4 slot
 *  times_ 01: The threshold is Pause time minus 28 slot times _PT  28 slot times_ 10:
 *  The threshold is Pause time minus 144 slot times _PT  144 slot times_ 11: The
 *  threshold is Pause time minus 256 slot times _PT  256 slot times_ The slot time is
 *  defined as the time taken to transmit 512 bits _64 bytes_ on the GMII or MII
 *  interface
 */
#define EMAC_PLT    0x00000003U
#define EMAC_PLT_M  (EMAC_PLT_V << EMAC_PLT_S)
#define EMAC_PLT_V  0x00000003U
#define EMAC_PLT_S  4
/** EMAC_DZPQ : R/W; bitpos: [7]; default: 0;
 *  Disable ZeroQuanta Pause When this bit is set, it disables the automatic generation
 *  of the ZeroQuanta Pause frames on the deassertion of the flowcontrol signal from
 *  the FIFO layer _MTL or external sideband flow control signal
 *  sbd_flowctrl_i/mti_flowctrl_i_ When this bit is reset, normal operation with
 *  automatic ZeroQuanta Pause frame generation is enabled
 */
#define EMAC_DZPQ    (BIT(7))
#define EMAC_DZPQ_M  (EMAC_DZPQ_V << EMAC_DZPQ_S)
#define EMAC_DZPQ_V  0x00000001U
#define EMAC_DZPQ_S  7
/** EMAC_PT : R/W; bitpos: [31:16]; default: 0;
 *  Pause Time This field holds the value to be used in the Pause Time field in the
 *  transmit control frame If the Pause Time bits is configured to be
 *  doublesynchronized to the _G_MII clock domain, then consecutive writes to this
 *  register should be performed only after at least four clock cycles in the
 *  destination clock domain
 */
#define EMAC_PT    0x0000FFFFU
#define EMAC_PT_M  (EMAC_PT_V << EMAC_PT_S)
#define EMAC_PT_V  0x0000FFFFU
#define EMAC_PT_S  16

/** EMAC_VLANTAG_REG register
 *  Identifies IEEE 8021Q VLAN type frames
 */
#define EMAC_VLANTAG_REG (DR_REG_EMAC_BASE + 0x1c)
/** EMAC_VL : R/W; bitpos: [15:0]; default: 0;
 *  VLAN Tag Identifier for Receive Frames This field contains the 8021Q VLAN tag to
 *  identify the VLAN frames and is compared to the 15th and 16th bytes of the frames
 *  being received for VLAN frames The following list describes the bits of this field:
 *  Bits [15:13]: User Priority Bit 12: Canonical Format Indicator _CFI_ or Drop
 *  Eligible Indicator _DEI_ Bits[11:0]: VLAN tag’s VLAN Identifier _VID_ field When
 *  the ETV bit is set, only the VID _Bits[11:0]_ is used for comparison If VL
 *  _VL[11:0] if ETV is set_ is all zeros, the MAC does not check the fifteenth and
 *  16th bytes for VLAN tag comparison, and declares all frames with a Type field value
 *  of 0x8100 or 0x88a8 as VLAN frames
 */
#define EMAC_VL    0x0000FFFFU
#define EMAC_VL_M  (EMAC_VL_V << EMAC_VL_S)
#define EMAC_VL_V  0x0000FFFFU
#define EMAC_VL_S  0
/** EMAC_ETV : R/W; bitpos: [16]; default: 0;
 *  Enable 12-Bit VLAN Tag Comparison
 */
#define EMAC_ETV    (BIT(16))
#define EMAC_ETV_M  (EMAC_ETV_V << EMAC_ETV_S)
#define EMAC_ETV_V  0x00000001U
#define EMAC_ETV_S  16
/** EMAC_VTIM : R/W; bitpos: [17]; default: 0;
 *  VLAN Tag Inverse Match Enable When set, this bit enables the VLAN Tag inverse
 *  matching The frames that do not have matching VLAN Tag are marked as matched When
 *  reset, this bit enables the VLAN Tag perfect matching The frames with matched VLAN
 *  Tag are marked as matched
 */
#define EMAC_VTIM    (BIT(17))
#define EMAC_VTIM_M  (EMAC_VTIM_V << EMAC_VTIM_S)
#define EMAC_VTIM_V  0x00000001U
#define EMAC_VTIM_S  17
/** EMAC_ESVL : R/W; bitpos: [18]; default: 0;
 *  Enable SVLAN When this bit is set, the MAC transmitter and receiver also consider
 *  the SVLAN _Type = 0x88A8_ frames as valid VLAN tagged frames
 */
#define EMAC_ESVL    (BIT(18))
#define EMAC_ESVL_M  (EMAC_ESVL_V << EMAC_ESVL_S)
#define EMAC_ESVL_V  0x00000001U
#define EMAC_ESVL_S  18
/** EMAC_VTHM : R/W; bitpos: [19]; default: 0;
 *  VLAN Tag Hash Table Match Enable When set, the most significant four bits of the
 *  VLAN tag’s CRC are used to index the content of Register 354 _VLAN Hash Table
 *  Register_ A value of 1 in the VLAN Hash Table register, corresponding to the index,
 *  indicates that the frame matched the VLAN hash table When Bit 16 _ETV_ is set, the
 *  CRC of the 12bit VLAN Identifier _VID_ is used for comparison whereas when ETV is
 *  reset, the CRC of the 16bit VLAN tag is used for comparison When reset, the VLAN
 *  Hash Match operation is not performed If the VLAN Hash feature is not enabled
 *  during core configuration, this bit is reserved _RO with default value_
 */
#define EMAC_VTHM    (BIT(19))
#define EMAC_VTHM_M  (EMAC_VTHM_V << EMAC_VTHM_S)
#define EMAC_VTHM_V  0x00000001U
#define EMAC_VTHM_S  19

/** EMAC_VERSION_REG register
 *  Identifies the version of the Core
 */
#define EMAC_VERSION_REG (DR_REG_EMAC_BASE + 0x20)
/** EMAC_SNPSVER : RO; bitpos: [7:0]; default: 55;
 *  Synopsysdefined Version _37_
 */
#define EMAC_SNPSVER    0x000000FFU
#define EMAC_SNPSVER_M  (EMAC_SNPSVER_V << EMAC_SNPSVER_S)
#define EMAC_SNPSVER_V  0x000000FFU
#define EMAC_SNPSVER_S  0
/** EMAC_USERVER : RO; bitpos: [15:8]; default: 0;
 *  Userdefined Version _configured with coreConsultant_
 */
#define EMAC_USERVER    0x000000FFU
#define EMAC_USERVER_M  (EMAC_USERVER_V << EMAC_USERVER_S)
#define EMAC_USERVER_V  0x000000FFU
#define EMAC_USERVER_S  8

/** EMAC_DEBUG_REG register
 *  Gives the status of various internal blocks for debugging
 */
#define EMAC_DEBUG_REG (DR_REG_EMAC_BASE + 0x24)
/** EMAC_RPESTS : RO; bitpos: [0]; default: 0;
 *  MAC GMII or MII Receive Protocol Engine Status When high, this bit indicates that
 *  the MAC GMII or MII receive protocol engine is actively receiving data and not in
 *  IDLE state
 */
#define EMAC_RPESTS    (BIT(0))
#define EMAC_RPESTS_M  (EMAC_RPESTS_V << EMAC_RPESTS_S)
#define EMAC_RPESTS_V  0x00000001U
#define EMAC_RPESTS_S  0
/** EMAC_RFCFCSTS : RO; bitpos: [2:1]; default: 0;
 *  MAC Receive Frame FIFO Controller Status When high, this field indicates the active
 *  state of the small FIFO Read and Write controllers of the MAC Receive Frame
 *  Controller Module RFCFCSTS[1] represents the status of small FIFO Read controller
 *  RFCFCSTS[0] represents the status of small FIFO Write controller
 */
#define EMAC_RFCFCSTS    0x00000003U
#define EMAC_RFCFCSTS_M  (EMAC_RFCFCSTS_V << EMAC_RFCFCSTS_S)
#define EMAC_RFCFCSTS_V  0x00000003U
#define EMAC_RFCFCSTS_S  1
/** EMAC_RWCSTS : RO; bitpos: [4]; default: 0;
 *  MTL Rx FIFO Write Controller Active Status When high, this bit indicates that the
 *  MTL Rx FIFO Write Controller is active and is transferring a received frame to the
 *  FIFO
 */
#define EMAC_RWCSTS    (BIT(4))
#define EMAC_RWCSTS_M  (EMAC_RWCSTS_V << EMAC_RWCSTS_S)
#define EMAC_RWCSTS_V  0x00000001U
#define EMAC_RWCSTS_S  4
/** EMAC_RRCSTS : RO; bitpos: [6:5]; default: 0;
 *  MTL RxFIFO Read Controller State This field gives the state of the Rx FIFO read
 *  Controller: 00: IDLE state 01: Reading frame data 10: Reading frame status _or
 *  timestamp_ 11: Flushing the frame data and status
 */
#define EMAC_RRCSTS    0x00000003U
#define EMAC_RRCSTS_M  (EMAC_RRCSTS_V << EMAC_RRCSTS_S)
#define EMAC_RRCSTS_V  0x00000003U
#define EMAC_RRCSTS_S  5
/** EMAC_RXFSTS : RO; bitpos: [9:8]; default: 0;
 *  MTL RxFIFO FillLevel Status This field gives the status of the filllevel of the Rx
 *  FIFO: 00: Rx FIFO Empty 01: Rx FIFO filllevel below flowcontrol deactivate
 *  threshold 10: Rx FIFO filllevel above flowcontrol activate threshold 11: Rx FIFO
 *  Full
 */
#define EMAC_RXFSTS    0x00000003U
#define EMAC_RXFSTS_M  (EMAC_RXFSTS_V << EMAC_RXFSTS_S)
#define EMAC_RXFSTS_V  0x00000003U
#define EMAC_RXFSTS_S  8
/** EMAC_TPESTS : RO; bitpos: [16]; default: 0;
 *  MAC GMII or MII Transmit Protocol Engine Status When high, this bit indicates that
 *  the MAC GMII or MII transmit protocol engine is actively transmitting data and is
 *  not in the IDLE state
 */
#define EMAC_TPESTS    (BIT(16))
#define EMAC_TPESTS_M  (EMAC_TPESTS_V << EMAC_TPESTS_S)
#define EMAC_TPESTS_V  0x00000001U
#define EMAC_TPESTS_S  16
/** EMAC_TFCSTS : RO; bitpos: [18:17]; default: 0;
 *  MAC Transmit Frame Controller Status This field indicates the state of the MAC
 *  Transmit Frame Controller module: 00: IDLE state 01: Waiting for status of previous
 *  frame or IFG or backoff period to be over 10: Generating and transmitting a Pause
 *  frame _in the fullduplex mode_ 11: Transferring input frame for transmission
 */
#define EMAC_TFCSTS    0x00000003U
#define EMAC_TFCSTS_M  (EMAC_TFCSTS_V << EMAC_TFCSTS_S)
#define EMAC_TFCSTS_V  0x00000003U
#define EMAC_TFCSTS_S  17
/** EMAC_TXPAUSED : RO; bitpos: [19]; default: 0;
 *  MAC Transmitter in Pause When high, this bit indicates that the MAC transmitter is
 *  in the Pause condition _in the fullduplexonly mode_ and hence does not schedule any
 *  frame for transmission
 */
#define EMAC_TXPAUSED    (BIT(19))
#define EMAC_TXPAUSED_M  (EMAC_TXPAUSED_V << EMAC_TXPAUSED_S)
#define EMAC_TXPAUSED_V  0x00000001U
#define EMAC_TXPAUSED_S  19
/** EMAC_TRCSTS : RO; bitpos: [21:20]; default: 0;
 *  MTL Tx FIFO Read Controller Status This field indicates the state of the Tx FIFO
 *  Read Controller: 00: IDLE state 01: READ state _transferring data to the MAC
 *  transmitter_ 10: Waiting for TxStatus from the MAC transmitter 11: Writing the
 *  received TxStatus or flushing the Tx FIFO
 */
#define EMAC_TRCSTS    0x00000003U
#define EMAC_TRCSTS_M  (EMAC_TRCSTS_V << EMAC_TRCSTS_S)
#define EMAC_TRCSTS_V  0x00000003U
#define EMAC_TRCSTS_S  20
/** EMAC_TWCSTS : RO; bitpos: [22]; default: 0;
 *  MTL Tx FIFO Write Controller Status When high, this bit indicates that the MTL Tx
 *  FIFO Write Controller is active and is transferring data to the Tx FIFO
 */
#define EMAC_TWCSTS    (BIT(22))
#define EMAC_TWCSTS_M  (EMAC_TWCSTS_V << EMAC_TWCSTS_S)
#define EMAC_TWCSTS_V  0x00000001U
#define EMAC_TWCSTS_S  22
/** EMAC_TXFSTS : RO; bitpos: [24]; default: 0;
 *  MTL Tx FIFO Not Empty Status When high, this bit indicates that the MTL Tx FIFO is
 *  not empty and some data is left for transmission
 */
#define EMAC_TXFSTS    (BIT(24))
#define EMAC_TXFSTS_M  (EMAC_TXFSTS_V << EMAC_TXFSTS_S)
#define EMAC_TXFSTS_V  0x00000001U
#define EMAC_TXFSTS_S  24
/** EMAC_TXSTSFSTS : RO; bitpos: [25]; default: 0;
 *  MTL TxStatus FIFO Full Status When high, this bit indicates that the MTL TxStatus
 *  FIFO is full Therefore, the MTL cannot accept any more frames for transmission This
 *  bit is reserved in the EMACAHB and EMACDMA configurations
 */
#define EMAC_TXSTSFSTS    (BIT(25))
#define EMAC_TXSTSFSTS_M  (EMAC_TXSTSFSTS_V << EMAC_TXSTSFSTS_S)
#define EMAC_TXSTSFSTS_V  0x00000001U
#define EMAC_TXSTSFSTS_S  25

/** EMAC_REMOTEWAKEUPFRAMEFILTER_REG register
 *  Remote Wake-Up Frame Filter Register
 */
#define EMAC_REMOTEWAKEUPFRAMEFILTER_REG (DR_REG_EMAC_BASE + 0x28)
/** EMAC_WKUPFRM_FILTER : R/W; bitpos: [31:0]; default: 0;
 *  This is the address through which the application writes or reads the remote
 *  wake-up frame filter registers.The reg_wkupfrm_filter register is a pointer to
 *  eight reg_wkupfrm_filter registers.The reg_wkupfrm_filter register is loaded by
 *  sequentially loading the eight register values.Eight sequential writes to this
 *  address(0x0028)write all reg_wkupfrm_filter registers.Similarly, eight sequential
 *  reads from this address(0x0028) read all reg_wkupfrm_filter registers.
 *  This register is present only when you select the PMT module Remote Wake-Up feature
 *  in coreConsultant.
 */
#define EMAC_WKUPFRM_FILTER    0xFFFFFFFFU
#define EMAC_WKUPFRM_FILTER_M  (EMAC_WKUPFRM_FILTER_V << EMAC_WKUPFRM_FILTER_S)
#define EMAC_WKUPFRM_FILTER_V  0xFFFFFFFFU
#define EMAC_WKUPFRM_FILTER_S  0

/** EMAC_PMTCONTROLANDSTATUS_REG register
 *  PMT Control and Status Register. This register is present only when you select the
 *  PMT module in coreConsultant.
 */
#define EMAC_PMTCONTROLANDSTATUS_REG (DR_REG_EMAC_BASE + 0x2c)
/** EMAC_PWRDWN : R/W1S; bitpos: [0]; default: 0;
 *  Power Down
 *  When set, the MAC receiver drops all received frames until it receives the expected
 *  magic packet or remote wake-up frame. This bit is then self-cleared and the
 *  power-down mode is disabled. The Software can also clear this bit before the
 *  expected magic packet or remote wake-up frame is received. The frames, received by
 *  MAC after this bit is cleared, are forwarded to the application. This bit must only
 *  be set when the Magic Packet Enable, Global Unicast, or Remote Wake-Up Frame Enable
 *  bit is set high.
 */
#define EMAC_PWRDWN    (BIT(0))
#define EMAC_PWRDWN_M  (EMAC_PWRDWN_V << EMAC_PWRDWN_S)
#define EMAC_PWRDWN_V  0x00000001U
#define EMAC_PWRDWN_S  0
/** EMAC_MGKPKTEN : R/W; bitpos: [1]; default: 0;
 *  Magic Packet Enable
 *  When set, enables generation of a power management event because of magic packet
 *  reception.
 */
#define EMAC_MGKPKTEN    (BIT(1))
#define EMAC_MGKPKTEN_M  (EMAC_MGKPKTEN_V << EMAC_MGKPKTEN_S)
#define EMAC_MGKPKTEN_V  0x00000001U
#define EMAC_MGKPKTEN_S  1
/** EMAC_RWKPKTEN : R/W; bitpos: [2]; default: 0;
 *  Remote Wake-Up Frame Enable
 *  When set, enables generation of a power management event because of remote wake-up
 *  frame reception.
 */
#define EMAC_RWKPKTEN    (BIT(2))
#define EMAC_RWKPKTEN_M  (EMAC_RWKPKTEN_V << EMAC_RWKPKTEN_S)
#define EMAC_RWKPKTEN_V  0x00000001U
#define EMAC_RWKPKTEN_S  2
/** EMAC_MGKPRCVD : R/W; bitpos: [5]; default: 0;
 *  Magic Packet Received
 *  When set,this bit indicates that the power management event is generated because of
 *  the reception of a magic packet. This bit is cleared by Read into this register.
 */
#define EMAC_MGKPRCVD    (BIT(5))
#define EMAC_MGKPRCVD_M  (EMAC_MGKPRCVD_V << EMAC_MGKPRCVD_S)
#define EMAC_MGKPRCVD_V  0x00000001U
#define EMAC_MGKPRCVD_S  5
/** EMAC_RWKPRCVD : R/W; bitpos: [6]; default: 0;
 *  Remote Wake-Up Frame Received
 *  When set, this bit indicates the power management event is generated because of the
 *  reception of a remote wake-up frame. This bit is cleared by a Read into this
 *  register.
 */
#define EMAC_RWKPRCVD    (BIT(6))
#define EMAC_RWKPRCVD_M  (EMAC_RWKPRCVD_V << EMAC_RWKPRCVD_S)
#define EMAC_RWKPRCVD_V  0x00000001U
#define EMAC_RWKPRCVD_S  6
/** EMAC_GLBLUCAST : R/W; bitpos: [9]; default: 0;
 *  Global Unicast.
 *  When set, enables any unicast packet filtered by the MAX(DAF) address recognition
 *  to be a remote wake-up frame.
 */
#define EMAC_GLBLUCAST    (BIT(9))
#define EMAC_GLBLUCAST_M  (EMAC_GLBLUCAST_V << EMAC_GLBLUCAST_S)
#define EMAC_GLBLUCAST_V  0x00000001U
#define EMAC_GLBLUCAST_S  9
/** EMAC_RWKPTR : RO; bitpos: [28:24]; default: 0;
 *  Remote Wake-up FIFO Pointer.
 *  This field gives the current value(0 to 31) of the Remote Wake-up Frame filter
 *  register pointer. When the value of this pointer is erual to 7, 15, 23 or 31, the
 *  contents of the Remote Wake-up Frame Filter Register are transferred to the
 *  clk_rx_i domain when a write occurs to that register.
 *  The maximum value of the pointer is 7, 15, 23 and 31 respectively depending on the
 *  number of Remote Wakeup Filters selected during configuration.
 */
#define EMAC_RWKPTR    0x0000001FU
#define EMAC_RWKPTR_M  (EMAC_RWKPTR_V << EMAC_RWKPTR_S)
#define EMAC_RWKPTR_V  0x0000001FU
#define EMAC_RWKPTR_S  24
/** EMAC_RWKFILTRST : R/W1S; bitpos: [31]; default: 0;
 *  Remote Wake-Up Frame Filter Register Pointer Reset.
 *  When this bit is set, it resets the remote wake-up frame filter register pointer to
 *  3'b000. It is automatically cleared after 1 clock cycle.
 */
#define EMAC_RWKFILTRST    (BIT(31))
#define EMAC_RWKFILTRST_M  (EMAC_RWKFILTRST_V << EMAC_RWKFILTRST_S)
#define EMAC_RWKFILTRST_V  0x00000001U
#define EMAC_RWKFILTRST_S  31

/** EMAC_LPICONTROLANDSTATUS_REG register
 *  Controls the Low Power Idle _LPI_ operations and provides the LPI status of the
 *  core This register is present only when you select the Energy Efficient Ethernet
 *  feature in coreConsultant
 */
#define EMAC_LPICONTROLANDSTATUS_REG (DR_REG_EMAC_BASE + 0x30)
/** EMAC_TLPIEN : R/W; bitpos: [0]; default: 0;
 *  Transmit LPI Entry When set, this bit indicates that the MAC Transmitter has
 *  entered the LPI state because of the setting of the LPIEN bit This bit is cleared
 *  by a read into this register
 */
#define EMAC_TLPIEN    (BIT(0))
#define EMAC_TLPIEN_M  (EMAC_TLPIEN_V << EMAC_TLPIEN_S)
#define EMAC_TLPIEN_V  0x00000001U
#define EMAC_TLPIEN_S  0
/** EMAC_TLPIEX : R/W; bitpos: [1]; default: 0;
 *  Transmit LPI Exit When set, this bit indicates that the MAC transmitter has exited
 *  the LPI state after the user has cleared the LPIEN bit and the LPI TW Timer has
 *  expired This bit is cleared by a read into this register
 */
#define EMAC_TLPIEX    (BIT(1))
#define EMAC_TLPIEX_M  (EMAC_TLPIEX_V << EMAC_TLPIEX_S)
#define EMAC_TLPIEX_V  0x00000001U
#define EMAC_TLPIEX_S  1
/** EMAC_RLPIEN : R/W; bitpos: [2]; default: 0;
 *  Receive LPI Entry When set, this bit indicates that the MAC Receiver has received
 *  an LPI pattern and entered the LPI state This bit is cleared by a read into this
 *  register Note: This bit may not get set if the MAC stops receiving the LPI pattern
 *  for a very short duration, such as, less than 3 clock cycles of CSR clock
 */
#define EMAC_RLPIEN    (BIT(2))
#define EMAC_RLPIEN_M  (EMAC_RLPIEN_V << EMAC_RLPIEN_S)
#define EMAC_RLPIEN_V  0x00000001U
#define EMAC_RLPIEN_S  2
/** EMAC_RLPIEX : R/W; bitpos: [3]; default: 0;
 *  Receive LPI Exit When set, this bit indicates that the MAC Receiver has stopped
 *  receiving the LPI pattern on the GMII or MII interface, exited the LPI state, and
 *  resumed the normal reception This bit is cleared by a read into this register Note:
 *  This bit may not get set if the MAC stops receiving the LPI pattern for a very
 *  short duration, such as, less than 3 clock cycles of CSR clock
 */
#define EMAC_RLPIEX    (BIT(3))
#define EMAC_RLPIEX_M  (EMAC_RLPIEX_V << EMAC_RLPIEX_S)
#define EMAC_RLPIEX_V  0x00000001U
#define EMAC_RLPIEX_S  3
/** EMAC_TLPIST : RO; bitpos: [8]; default: 0;
 *  Transmit LPI State When set, this bit indicates that the MAC is transmitting the
 *  LPI pattern on the GMII or MII interface
 */
#define EMAC_TLPIST    (BIT(8))
#define EMAC_TLPIST_M  (EMAC_TLPIST_V << EMAC_TLPIST_S)
#define EMAC_TLPIST_V  0x00000001U
#define EMAC_TLPIST_S  8
/** EMAC_RLPIST : RO; bitpos: [9]; default: 0;
 *  Receive LPI State When set, this bit indicates that the MAC is receiving the LPI
 *  pattern on the GMII or MII interface
 */
#define EMAC_RLPIST    (BIT(9))
#define EMAC_RLPIST_M  (EMAC_RLPIST_V << EMAC_RLPIST_S)
#define EMAC_RLPIST_V  0x00000001U
#define EMAC_RLPIST_S  9
/** EMAC_LPIEN : R/W; bitpos: [16]; default: 0;
 *  LPI Enable When set, this bit instructs the MAC Transmitter to enter the LPI state
 *  When reset, this bit instructs the MAC to exit the LPI state and resume normal
 *  transmission This bit is cleared when the LPITXA bit is set and the MAC exits the
 *  LPI state because of the arrival of a new packet for transmission
 */
#define EMAC_LPIEN    (BIT(16))
#define EMAC_LPIEN_M  (EMAC_LPIEN_V << EMAC_LPIEN_S)
#define EMAC_LPIEN_V  0x00000001U
#define EMAC_LPIEN_S  16
/** EMAC_PLS : R/W; bitpos: [17]; default: 0;
 *  PHY Link Status This bit indicates the link status of the PHY The MAC Transmitter
 *  asserts the LPI pattern only when the link status is up _okay_ at least for the
 *  time indicated by the LPI LS TIMER When set, the link is considered to be okay _up_
 *  and when reset, the link is considered to be down
 */
#define EMAC_PLS    (BIT(17))
#define EMAC_PLS_M  (EMAC_PLS_V << EMAC_PLS_S)
#define EMAC_PLS_V  0x00000001U
#define EMAC_PLS_S  17
/** EMAC_PLSEN : R/W; bitpos: [18]; default: 0;
 *  PHY Link Status Enable This bit enables the link status received on the RGMII,
 *  SGMII, or SMII receive paths to be used for activating the LPI LS TIMER When set,
 *  the MAC uses the linkstatus bits of Register 54 _SGMII/RGMII/SMII Control and
 *  Status Register_ and Bit 17 _PLS_ for the LPI LS Timer trigger When cleared, the
 *  MAC ignores the linkstatus bits of Register 54 and takes only the PLS bit This bit
 *  is RO and reserved if you have not selected the RGMII, SGMII, or SMII PHY interface
 */
#define EMAC_PLSEN    (BIT(18))
#define EMAC_PLSEN_M  (EMAC_PLSEN_V << EMAC_PLSEN_S)
#define EMAC_PLSEN_V  0x00000001U
#define EMAC_PLSEN_S  18
/** EMAC_LPITXA : R/W; bitpos: [19]; default: 0;
 *  LPI TX Automate This bit controls the behavior of the MAC when it is entering or
 *  coming out of the LPI mode on the transmit side This bit is not functional in the
 *  EMAC CORE configuration in which the Tx clock gating is done during the LPI mode If
 *  the LPITXA and LPIEN bits are set to 1, the MAC enters the LPI mode only after all
 *  outstanding frames _in the core_ and pending frames _in the application interface_
 *  have been transmitted The MAC comes out of the LPI mode when the application sends
 *  any frame for transmission or the application issues a TX FIFO Flush command In
 *  addition, the MAC automatically clears the LPIEN bit when it exits the LPI state If
 *  TX FIFO Flush is set in Bit 20 of Register 6 _Operation Mode Register_, when the
 *  MAC is in the LPI mode, the MAC exits the LPI mode When this bit is 0, the LPIEN
 *  bit directly controls behavior of the MAC when it is entering or coming out of the
 *  LPI mode
 */
#define EMAC_LPITXA    (BIT(19))
#define EMAC_LPITXA_M  (EMAC_LPITXA_V << EMAC_LPITXA_S)
#define EMAC_LPITXA_V  0x00000001U
#define EMAC_LPITXA_S  19

/** EMAC_LPITIMERSCONTROL_REG register
 *  Controls the timeout values in LPI states This register is present only when you
 *  select the Energy Efficient Ethernet feature in coreConsultant
 */
#define EMAC_LPITIMERSCONTROL_REG (DR_REG_EMAC_BASE + 0x34)
/** EMAC_TWT : R/W; bitpos: [15:0]; default: 0;
 *  LPI TW TIMER This field specifies the minimum time _in microseconds_ for which the
 *  MAC waits after it stops transmitting the LPI pattern to the PHY and before it
 *  resumes the normal transmission The TLPIEX status bit is set after the expiry of
 *  this timer
 */
#define EMAC_TWT    0x0000FFFFU
#define EMAC_TWT_M  (EMAC_TWT_V << EMAC_TWT_S)
#define EMAC_TWT_V  0x0000FFFFU
#define EMAC_TWT_S  0
/** EMAC_LST : R/W; bitpos: [25:16]; default: 1000;
 *  LPI LS TIMER This field specifies the minimum time _in milliseconds_ for which the
 *  link status from the PHY should be up _OKAY_ before the LPI pattern can be
 *  transmitted to the PHY The MAC does not transmit the LPI pattern even when the
 *  LPIEN bit is set unless the LPI LS Timer reaches the programmed terminal count The
 *  default value of the LPI LS Timer is 1000 _1 sec_ as defined in the IEEE standard
 */
#define EMAC_LST    0x000003FFU
#define EMAC_LST_M  (EMAC_LST_V << EMAC_LST_S)
#define EMAC_LST_V  0x000003FFU
#define EMAC_LST_S  16

/** EMAC_INTERRUPTSTATUS_REG register
 *  Contains the interrupt status
 */
#define EMAC_INTERRUPTSTATUS_REG (DR_REG_EMAC_BASE + 0x38)
/** EMAC_RGSMIIIS : RO; bitpos: [0]; default: 0;
 *  RGMII or SMII Interrupt Status This bit is set because of any change in value of
 *  the Link Status of RGMII or SMII interface _Bit 3 in Register 54 _SGMII/RGMII/SMII
 *  Control and Status Register__ This bit is cleared when you perform a read operation
 *  on the SGMII/RGMII/SMII Control and Status Register This bit is valid only when you
 *  select the optional RGMII or SMII PHY interface during core configuration and
 *  operation
 */
#define EMAC_RGSMIIIS    (BIT(0))
#define EMAC_RGSMIIIS_M  (EMAC_RGSMIIIS_V << EMAC_RGSMIIIS_S)
#define EMAC_RGSMIIIS_V  0x00000001U
#define EMAC_RGSMIIIS_S  0
/** EMAC_PCSLCHGIS : RO; bitpos: [1]; default: 0;
 *  PCS Link Status Changed This bit is set because of any change in Link Status in the
 *  TBI, RTBI, or SGMII PHY interface _Bit 2 in Register 49 _AN Status Register__ This
 *  bit is cleared when you perform a read operation on the AN Status register This bit
 *  is valid only when you select the optional TBI, RTBI, or SGMII PHY interface during
 *  core configuration and operation
 */
#define EMAC_PCSLCHGIS    (BIT(1))
#define EMAC_PCSLCHGIS_M  (EMAC_PCSLCHGIS_V << EMAC_PCSLCHGIS_S)
#define EMAC_PCSLCHGIS_V  0x00000001U
#define EMAC_PCSLCHGIS_S  1
/** EMAC_PCSANCIS : RO; bitpos: [2]; default: 0;
 *  PCS AutoNegotiation Complete This bit is set when the Autonegotiation is completed
 *  in the TBI, RTBI, or SGMII PHY interface _Bit 5 in Register 49 _AN Status
 *  Register__ This bit is cleared when you perform a read operation to the AN Status
 *  register This bit is valid only when you select the optional TBI, RTBI, or SGMII
 *  PHY interface during core configuration and operation
 */
#define EMAC_PCSANCIS    (BIT(2))
#define EMAC_PCSANCIS_M  (EMAC_PCSANCIS_V << EMAC_PCSANCIS_S)
#define EMAC_PCSANCIS_V  0x00000001U
#define EMAC_PCSANCIS_S  2
/** EMAC_PMTIS : RO; bitpos: [3]; default: 0;
 *  PMT Interrupt Status This bit is set when a magic packet or remote wakeup frame is
 *  received in the powerdown mode _see Bits 5 and 6 in the PMT Control and Status
 *  Register_ This bit is cleared when both Bits[6:5] are cleared because of a read
 *  operation to the PMT Control and Status register This bit is valid only when you
 *  select the optional PMT module during core configuration
 */
#define EMAC_PMTIS    (BIT(3))
#define EMAC_PMTIS_M  (EMAC_PMTIS_V << EMAC_PMTIS_S)
#define EMAC_PMTIS_V  0x00000001U
#define EMAC_PMTIS_S  3
/** EMAC_MMCIS : RO; bitpos: [4]; default: 0;
 *  MMC Interrupt Status This bit is set high when any of the Bits [7:5] is set high
 *  and cleared only when all of these bits are low This bit is valid only when you
 *  select the optional MMC module during core configuration
 */
#define EMAC_MMCIS    (BIT(4))
#define EMAC_MMCIS_M  (EMAC_MMCIS_V << EMAC_MMCIS_S)
#define EMAC_MMCIS_V  0x00000001U
#define EMAC_MMCIS_S  4
/** EMAC_MMCRXIS : RO; bitpos: [5]; default: 0;
 *  MMC Receive Interrupt Status This bit is set high when an interrupt is generated in
 *  the MMC Receive Interrupt Register This bit is cleared when all the bits in this
 *  interrupt register are cleared This bit is valid only when you select the optional
 *  MMC module during core configuration
 */
#define EMAC_MMCRXIS    (BIT(5))
#define EMAC_MMCRXIS_M  (EMAC_MMCRXIS_V << EMAC_MMCRXIS_S)
#define EMAC_MMCRXIS_V  0x00000001U
#define EMAC_MMCRXIS_S  5
/** EMAC_MMCTXIS : RO; bitpos: [6]; default: 0;
 *  MMC Transmit Interrupt Status This bit is set high when an interrupt is generated
 *  in the MMC Transmit Interrupt Register This bit is cleared when all the bits in
 *  this interrupt register are cleared This bit is valid only when you select the
 *  optional MMC module during core configuration
 */
#define EMAC_MMCTXIS    (BIT(6))
#define EMAC_MMCTXIS_M  (EMAC_MMCTXIS_V << EMAC_MMCTXIS_S)
#define EMAC_MMCTXIS_V  0x00000001U
#define EMAC_MMCTXIS_S  6
/** EMAC_MMCRXIPIS : RO; bitpos: [7]; default: 0;
 *  MMC Receive Checksum Offload Interrupt Status This bit is set high when an
 *  interrupt is generated in the MMC Receive Checksum Offload Interrupt Register This
 *  bit is cleared when all the bits in this interrupt register are cleared This bit is
 *  valid only when you select the optional MMC module and Checksum Offload Engine
 *  _Type 2_ during core configuration
 */
#define EMAC_MMCRXIPIS    (BIT(7))
#define EMAC_MMCRXIPIS_M  (EMAC_MMCRXIPIS_V << EMAC_MMCRXIPIS_S)
#define EMAC_MMCRXIPIS_V  0x00000001U
#define EMAC_MMCRXIPIS_S  7
/** EMAC_TSIS : RO; bitpos: [9]; default: 0;
 *  Timestamp Interrupt Status When the Advanced Timestamp feature is enabled, this bit
 *  is set when R_SS_RC any of the following conditions is true: The system time value
 *  equals or exceeds the value specified in the Target Time High and Low registers
 *  There is an overflow in the seconds register The Auxiliary snapshot trigger is
 *  asserted This bit is cleared on reading Bit 0 of Register 458 _Timestamp Status
 *  Register_ If default Timestamping is enabled, when set, this bit indicates that the
 *  system time value is equal to or exceeds the value specified in the Target Time
 *  registers In this mode, this bit is cleared after the completion of the read of
 *  this bit In all other modes, this bit is reserved
 */
#define EMAC_TSIS    (BIT(9))
#define EMAC_TSIS_M  (EMAC_TSIS_V << EMAC_TSIS_S)
#define EMAC_TSIS_V  0x00000001U
#define EMAC_TSIS_S  9
/** EMAC_LPIIS : RO; bitpos: [10]; default: 0;
 *  LPI Interrupt Status When the Energy Efficient Ethernet feature is enabled, this
 *  bit is set for any LPI state entry or exit in the MAC Transmitter or Receiver This
 *  bit is cleared on reading Bit 0 of Register 12 _LPI Control and Status Register_ In
 *  all other modes, this bit is reserved
 */
#define EMAC_LPIIS    (BIT(10))
#define EMAC_LPIIS_M  (EMAC_LPIIS_V << EMAC_LPIIS_S)
#define EMAC_LPIIS_V  0x00000001U
#define EMAC_LPIIS_S  10
/** EMAC_GPIIS : RO; bitpos: [11]; default: 0;
 *  GPI Interrupt Status When the GPIO feature is enabled, this bit is set when any
 *  active event _LL or LH_ occurs on the GPIS field _Bits [3:0]_ of Register 56
 *  _General Purpose IO Register_ and the corresponding GPIE bit is enabled This bit is
 *  cleared on reading lane 0 _GPIS_ of Register 56 _General Purpose IO Register_ When
 *  the GPIO feature is not enabled, this bit is reserved
 */
#define EMAC_GPIIS    (BIT(11))
#define EMAC_GPIIS_M  (EMAC_GPIIS_V << EMAC_GPIIS_S)
#define EMAC_GPIIS_V  0x00000001U
#define EMAC_GPIIS_S  11

/** EMAC_INTERRUPTMASK_REG register
 *  Contains the masks for generating the interrupts
 */
#define EMAC_INTERRUPTMASK_REG (DR_REG_EMAC_BASE + 0x3c)
/** EMAC_RGSMIIIM : R/W; bitpos: [0]; default: 0;
 *  RGMII or SMII Interrupt Mask When set, this bit disables the assertion of the
 *  interrupt signal because of the setting of the RGMII or SMII Interrupt Status bit
 *  in Register 14 _Interrupt Status Register_
 */
#define EMAC_RGSMIIIM    (BIT(0))
#define EMAC_RGSMIIIM_M  (EMAC_RGSMIIIM_V << EMAC_RGSMIIIM_S)
#define EMAC_RGSMIIIM_V  0x00000001U
#define EMAC_RGSMIIIM_S  0
/** EMAC_PCSLCHGIM : R/W; bitpos: [1]; default: 0;
 *  PCS Link Status Interrupt Mask When set, this bit disables the assertion of the
 *  interrupt signal because of the setting of the PCS Linkstatus changed bit in
 *  Register 14 _Interrupt Status Register_
 */
#define EMAC_PCSLCHGIM    (BIT(1))
#define EMAC_PCSLCHGIM_M  (EMAC_PCSLCHGIM_V << EMAC_PCSLCHGIM_S)
#define EMAC_PCSLCHGIM_V  0x00000001U
#define EMAC_PCSLCHGIM_S  1
/** EMAC_PCSANCIM : R/W; bitpos: [2]; default: 0;
 *  PCS AN Completion Interrupt Mask When set, this bit disables the assertion of the
 *  interrupt signal because of the setting of PCS Autonegotiation complete bit in
 *  Register 14 _Interrupt Status Register_
 */
#define EMAC_PCSANCIM    (BIT(2))
#define EMAC_PCSANCIM_M  (EMAC_PCSANCIM_V << EMAC_PCSANCIM_S)
#define EMAC_PCSANCIM_V  0x00000001U
#define EMAC_PCSANCIM_S  2
/** EMAC_PMTIM : R/W; bitpos: [3]; default: 0;
 *  PMT Interrupt Mask When set, this bit disables the assertion of the interrupt
 *  signal because of the setting of PMT Interrupt Status bit in Register 14 _Interrupt
 *  Status Register_
 */
#define EMAC_PMTIM    (BIT(3))
#define EMAC_PMTIM_M  (EMAC_PMTIM_V << EMAC_PMTIM_S)
#define EMAC_PMTIM_V  0x00000001U
#define EMAC_PMTIM_S  3
/** EMAC_TSIM : R/W; bitpos: [9]; default: 0;
 *  Timestamp Interrupt Mask When set, this bit disables the assertion of the interrupt
 *  signal because of the setting of Timestamp Interrupt Status bit in Register 14
 *  _Interrupt Status Register_ This bit is valid only when IEEE1588 timestamping is
 *  enabled In all other modes, this bit is reserved
 */
#define EMAC_TSIM    (BIT(9))
#define EMAC_TSIM_M  (EMAC_TSIM_V << EMAC_TSIM_S)
#define EMAC_TSIM_V  0x00000001U
#define EMAC_TSIM_S  9
/** EMAC_LPIIM : R/W; bitpos: [10]; default: 0;
 *  LPI Interrupt Mask When set, this bit disables the assertion of the interrupt
 *  signal because of the setting of the LPI Interrupt Status bit in Register 14
 *  _Interrupt Status Register_ This bit is valid only when you select the Energy
 *  Efficient Ethernet feature during core configuration In all other modes, this bit
 *  is reserved
 */
#define EMAC_LPIIM    (BIT(10))
#define EMAC_LPIIM_M  (EMAC_LPIIM_V << EMAC_LPIIM_S)
#define EMAC_LPIIM_V  0x00000001U
#define EMAC_LPIIM_S  10

/** EMAC_MACADDRESS0HIGH_REG register
 *  Contains the higher 16 bits of the first MAC address
 */
#define EMAC_MACADDRESS0HIGH_REG (DR_REG_EMAC_BASE + 0x40)
/** EMAC_ADDRHI_0 : R/W; bitpos: [15:0]; default: 65535;
 *  MAC Address0 [47:32] This field contains the upper 16 bits _47:32_ of the first
 *  6byte MAC address The MAC uses this field for filtering the received frames and
 *  inserting the MAC address in the Transmit Flow Control _Pause_ Frames
 */
#define EMAC_ADDRHI_0    0x0000FFFFU
#define EMAC_ADDRHI_0_M  (EMAC_ADDRHI_0_V << EMAC_ADDRHI_0_S)
#define EMAC_ADDRHI_0_V  0x0000FFFFU
#define EMAC_ADDRHI_0_S  0
/** EMAC_AE_0 : RO; bitpos: [31]; default: 1;
 *  Address Enable This bit is always set to 1
 */
#define EMAC_AE_0    (BIT(31))
#define EMAC_AE_0_M  (EMAC_AE_0_V << EMAC_AE_0_S)
#define EMAC_AE_0_V  0x00000001U
#define EMAC_AE_0_S  31

/** EMAC_MACADDRESS0LOW_REG register
 *  Contains the lower 32 bits of the first MAC address
 */
#define EMAC_MACADDRESS0LOW_REG (DR_REG_EMAC_BASE + 0x44)
/** EMAC_ADDRLO_0 : R/W; bitpos: [31:0]; default: 4294967295;
 *  MAC Address0 [31:0] This field contains the lower 32 bits of the first 6byte MAC
 *  address This is used by the MAC for filtering the received frames and inserting the
 *  MAC address in the Transmit Flow Control _Pause_ Frames
 */
#define EMAC_ADDRLO_0    0xFFFFFFFFU
#define EMAC_ADDRLO_0_M  (EMAC_ADDRLO_0_V << EMAC_ADDRLO_0_S)
#define EMAC_ADDRLO_0_V  0xFFFFFFFFU
#define EMAC_ADDRLO_0_S  0

/** EMAC_MACADDRESS1HIGH_REG register
 *  Contains the higher 16 bits of the second MAC address This register is present only
 *  when Enable MAC Address1 is selected in coreConsultant _See Table 78_
 */
#define EMAC_MACADDRESS1HIGH_REG (DR_REG_EMAC_BASE + 0x48)
/** EMAC_ADDRHI_1 : R/W; bitpos: [15:0]; default: 65535;
 *  MAC Address1 [47:32] This field contains the upper 16 bits _47:32_ of the second
 *  6byte MAC address
 */
#define EMAC_ADDRHI_1    0x0000FFFFU
#define EMAC_ADDRHI_1_M  (EMAC_ADDRHI_1_V << EMAC_ADDRHI_1_S)
#define EMAC_ADDRHI_1_V  0x0000FFFFU
#define EMAC_ADDRHI_1_S  0
/** EMAC_MBC_1 : R/W; bitpos: [29:24]; default: 0;
 *  Mask Byte Control These bits are mask control bits for comparison of each of the
 *  MAC Address bytes When set high, the MAC does not compare the corresponding byte of
 *  received DA or SA with the contents of MAC Address1 registers Each bit controls the
 *  masking of the bytes as follows: Bit 29: Register 18[15:8] Bit 28: Register 18[7:0]
 *  Bit 27: Register 19[31:24]  Bit 24: Register 19[7:0] You can filter a group of
 *  addresses _known as group address filtering_ by masking one or more bytes of the
 *  address
 */
#define EMAC_MBC_1    0x0000003FU
#define EMAC_MBC_1_M  (EMAC_MBC_1_V << EMAC_MBC_1_S)
#define EMAC_MBC_1_V  0x0000003FU
#define EMAC_MBC_1_S  24
/** EMAC_SA_1 : R/W; bitpos: [30]; default: 0;
 *  Source Address When this bit is set, the MAC Address1[47:0] is used to compare with
 *  the SA fields of the received frame When this bit is reset, the MAC Address1[47:0]
 *  is used to compare with the DA fields of the received frame
 */
#define EMAC_SA_1    (BIT(30))
#define EMAC_SA_1_M  (EMAC_SA_1_V << EMAC_SA_1_S)
#define EMAC_SA_1_V  0x00000001U
#define EMAC_SA_1_S  30
/** EMAC_AE_1 : R/W; bitpos: [31]; default: 0;
 *  Address Enable
 */
#define EMAC_AE_1    (BIT(31))
#define EMAC_AE_1_M  (EMAC_AE_1_V << EMAC_AE_1_S)
#define EMAC_AE_1_V  0x00000001U
#define EMAC_AE_1_S  31

/** EMAC_MACADDRESS1LOW_REG register
 *  Contains the lower 32 bits of the second MAC address This register is present only
 *  when Enable MAC Address1 is selected in coreConsultant _See Table 78_
 */
#define EMAC_MACADDRESS1LOW_REG (DR_REG_EMAC_BASE + 0x4c)
/** EMAC_ADDRLO_1 : R/W; bitpos: [31:0]; default: 4294967295;
 *  MAC Address1 [31:0] This field contains the lower 32 bits of the second 6byte MAC
 *  address The content of this field is undefined until loaded by the Application
 *  after the initialization process
 */
#define EMAC_ADDRLO_1    0xFFFFFFFFU
#define EMAC_ADDRLO_1_M  (EMAC_ADDRLO_1_V << EMAC_ADDRLO_1_S)
#define EMAC_ADDRLO_1_V  0xFFFFFFFFU
#define EMAC_ADDRLO_1_S  0

/** EMAC_MACADDRESS2HIGH_REG register
 *  Reserved
 */
#define EMAC_MACADDRESS2HIGH_REG (DR_REG_EMAC_BASE + 0x50)
/** EMAC_ADDRHI_2 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRHI_2    0x0000FFFFU
#define EMAC_ADDRHI_2_M  (EMAC_ADDRHI_2_V << EMAC_ADDRHI_2_S)
#define EMAC_ADDRHI_2_V  0x0000FFFFU
#define EMAC_ADDRHI_2_S  0
/** EMAC_MBC_2 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_MBC_2    0x0000003FU
#define EMAC_MBC_2_M  (EMAC_MBC_2_V << EMAC_MBC_2_S)
#define EMAC_MBC_2_V  0x0000003FU
#define EMAC_MBC_2_S  24
/** EMAC_SA_2 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_SA_2    (BIT(30))
#define EMAC_SA_2_M  (EMAC_SA_2_V << EMAC_SA_2_S)
#define EMAC_SA_2_V  0x00000001U
#define EMAC_SA_2_S  30
/** EMAC_AE_2 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_AE_2    (BIT(31))
#define EMAC_AE_2_M  (EMAC_AE_2_V << EMAC_AE_2_S)
#define EMAC_AE_2_V  0x00000001U
#define EMAC_AE_2_S  31

/** EMAC_MACADDRESS2LOW_REG register
 *  Reserved
 */
#define EMAC_MACADDRESS2LOW_REG (DR_REG_EMAC_BASE + 0x54)
/** EMAC_ADDRLO_2 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRLO_2    0xFFFFFFFFU
#define EMAC_ADDRLO_2_M  (EMAC_ADDRLO_2_V << EMAC_ADDRLO_2_S)
#define EMAC_ADDRLO_2_V  0xFFFFFFFFU
#define EMAC_ADDRLO_2_S  0

/** EMAC_MACADDRESS3HIGH_REG
 *  register
 *  Reserved
 */
#define EMAC_MACADDRESS3HIGH_REG (DR_REG_EMAC_BASE + 0x58)
/** EMAC_ADDRHI_3 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRHI_3    0x0000FFFFU
#define EMAC_ADDRHI_3_M  (EMAC_ADDRHI_3_V << EMAC_ADDRHI_3_S)
#define EMAC_ADDRHI_3_V  0x0000FFFFU
#define EMAC_ADDRHI_3_S  0
/** EMAC_MBC_3 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_MBC_3    0x0000003FU
#define EMAC_MBC_3_M  (EMAC_MBC_3_V << EMAC_MBC_3_S)
#define EMAC_MBC_3_V  0x0000003FU
#define EMAC_MBC_3_S  24
/** EMAC_SA_3 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_SA_3    (BIT(30))
#define EMAC_SA_3_M  (EMAC_SA_3_V << EMAC_SA_3_S)
#define EMAC_SA_3_V  0x00000001U
#define EMAC_SA_3_S  30
/** EMAC_AE_3 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_AE_3    (BIT(31))
#define EMAC_AE_3_M  (EMAC_AE_3_V << EMAC_AE_3_S)
#define EMAC_AE_3_V  0x00000001U
#define EMAC_AE_3_S  31

/** EMAC_MACADDRESS3LOW_REG register
 *  Reserved
 */
#define EMAC_MACADDRESS3LOW_REG (DR_REG_EMAC_BASE + 0x5c)
/** EMAC_ADDRLO_3 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRLO_3    0xFFFFFFFFU
#define EMAC_ADDRLO_3_M  (EMAC_ADDRLO_3_V << EMAC_ADDRLO_3_S)
#define EMAC_ADDRLO_3_V  0xFFFFFFFFU
#define EMAC_ADDRLO_3_S  0

/** EMAC_MACADDRESS4HIGH_REG register
 *  Reserved
 */
#define EMAC_MACADDRESS4HIGH_REG (DR_REG_EMAC_BASE + 0x60)
/** EMAC_ADDRHI_4 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRHI_4    0x0000FFFFU
#define EMAC_ADDRHI_4_M  (EMAC_ADDRHI_4_V << EMAC_ADDRHI_4_S)
#define EMAC_ADDRHI_4_V  0x0000FFFFU
#define EMAC_ADDRHI_4_S  0
/** EMAC_MBC_4 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_MBC_4    0x0000003FU
#define EMAC_MBC_4_M  (EMAC_MBC_4_V << EMAC_MBC_4_S)
#define EMAC_MBC_4_V  0x0000003FU
#define EMAC_MBC_4_S  24
/** EMAC_SA_4 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_SA_4    (BIT(30))
#define EMAC_SA_4_M  (EMAC_SA_4_V << EMAC_SA_4_S)
#define EMAC_SA_4_V  0x00000001U
#define EMAC_SA_4_S  30
/** EMAC_AE_4 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_AE_4    (BIT(31))
#define EMAC_AE_4_M  (EMAC_AE_4_V << EMAC_AE_4_S)
#define EMAC_AE_4_V  0x00000001U
#define EMAC_AE_4_S  31

/** EMAC_MACADDRESS4LOW_REG register
 *  Reserved
 */
#define EMAC_MACADDRESS4LOW_REG (DR_REG_EMAC_BASE + 0x64)
/** EMAC_ADDRLO_4 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRLO_4    0xFFFFFFFFU
#define EMAC_ADDRLO_4_M  (EMAC_ADDRLO_4_V << EMAC_ADDRLO_4_S)
#define EMAC_ADDRLO_4_V  0xFFFFFFFFU
#define EMAC_ADDRLO_4_S  0

/** EMAC_MACADDRESS5HIGH_REG register
 *  Reserved
 */
#define EMAC_MACADDRESS5HIGH_REG (DR_REG_EMAC_BASE + 0x68)
/** EMAC_ADDRHI_5 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRHI_5    0x0000FFFFU
#define EMAC_ADDRHI_5_M  (EMAC_ADDRHI_5_V << EMAC_ADDRHI_5_S)
#define EMAC_ADDRHI_5_V  0x0000FFFFU
#define EMAC_ADDRHI_5_S  0
/** EMAC_MBC_5 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_MBC_5    0x0000003FU
#define EMAC_MBC_5_M  (EMAC_MBC_5_V << EMAC_MBC_5_S)
#define EMAC_MBC_5_V  0x0000003FU
#define EMAC_MBC_5_S  24
/** EMAC_SA_5 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_SA_5    (BIT(30))
#define EMAC_SA_5_M  (EMAC_SA_5_V << EMAC_SA_5_S)
#define EMAC_SA_5_V  0x00000001U
#define EMAC_SA_5_S  30
/** EMAC_AE_5 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_AE_5    (BIT(31))
#define EMAC_AE_5_M  (EMAC_AE_5_V << EMAC_AE_5_S)
#define EMAC_AE_5_V  0x00000001U
#define EMAC_AE_5_S  31

/** EMAC_MACADDRESS5LOW_REG register
 *  Reserved
 */
#define EMAC_MACADDRESS5LOW_REG (DR_REG_EMAC_BASE + 0x6c)
/** EMAC_ADDRLO_5 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRLO_5    0xFFFFFFFFU
#define EMAC_ADDRLO_5_M  (EMAC_ADDRLO_5_V << EMAC_ADDRLO_5_S)
#define EMAC_ADDRLO_5_V  0xFFFFFFFFU
#define EMAC_ADDRLO_5_S  0

/** EMAC_MACADDRESS6HIGH_REG
 *  register
 *  Reserved
 */
#define EMAC_MACADDRESS6HIGH_REG (DR_REG_EMAC_BASE + 0x70)
/** EMAC_ADDRHI_6 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRHI_6    0x0000FFFFU
#define EMAC_ADDRHI_6_M  (EMAC_ADDRHI_6_V << EMAC_ADDRHI_6_S)
#define EMAC_ADDRHI_6_V  0x0000FFFFU
#define EMAC_ADDRHI_6_S  0
/** EMAC_MBC_6 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_MBC_6    0x0000003FU
#define EMAC_MBC_6_M  (EMAC_MBC_6_V << EMAC_MBC_6_S)
#define EMAC_MBC_6_V  0x0000003FU
#define EMAC_MBC_6_S  24
/** EMAC_SA_6 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_SA_6    (BIT(30))
#define EMAC_SA_6_M  (EMAC_SA_6_V << EMAC_SA_6_S)
#define EMAC_SA_6_V  0x00000001U
#define EMAC_SA_6_S  30
/** EMAC_AE_6 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_AE_6    (BIT(31))
#define EMAC_AE_6_M  (EMAC_AE_6_V << EMAC_AE_6_S)
#define EMAC_AE_6_V  0x00000001U
#define EMAC_AE_6_S  31

/** EMAC_MACADDRESS6LOW_REG register
 *  Reserved
 */
#define EMAC_MACADDRESS6LOW_REG (DR_REG_EMAC_BASE + 0x74)
/** EMAC_ADDRLO_6 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRLO_6    0xFFFFFFFFU
#define EMAC_ADDRLO_6_M  (EMAC_ADDRLO_6_V << EMAC_ADDRLO_6_S)
#define EMAC_ADDRLO_6_V  0xFFFFFFFFU
#define EMAC_ADDRLO_6_S  0

/** EMAC_MACADDRESS7HIGH_REG
 *  register
 *  Reserved
 */
#define EMAC_MACADDRESS7HIGH_REG (DR_REG_EMAC_BASE + 0x78)
/** EMAC_ADDRHI_7 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRHI_7    0x0000FFFFU
#define EMAC_ADDRHI_7_M  (EMAC_ADDRHI_7_V << EMAC_ADDRHI_7_S)
#define EMAC_ADDRHI_7_V  0x0000FFFFU
#define EMAC_ADDRHI_7_S  0
/** EMAC_MBC_7 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_MBC_7    0x0000003FU
#define EMAC_MBC_7_M  (EMAC_MBC_7_V << EMAC_MBC_7_S)
#define EMAC_MBC_7_V  0x0000003FU
#define EMAC_MBC_7_S  24
/** EMAC_SA_7 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_SA_7    (BIT(30))
#define EMAC_SA_7_M  (EMAC_SA_7_V << EMAC_SA_7_S)
#define EMAC_SA_7_V  0x00000001U
#define EMAC_SA_7_S  30
/** EMAC_AE_7 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_AE_7    (BIT(31))
#define EMAC_AE_7_M  (EMAC_AE_7_V << EMAC_AE_7_S)
#define EMAC_AE_7_V  0x00000001U
#define EMAC_AE_7_S  31

/** EMAC_MACADDRESS7LOW_REG register
 *  Reserved
 */
#define EMAC_MACADDRESS7LOW_REG (DR_REG_EMAC_BASE + 0x7c)
/** EMAC_ADDRLO_7 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define EMAC_ADDRLO_7    0xFFFFFFFFU
#define EMAC_ADDRLO_7_M  (EMAC_ADDRLO_7_V << EMAC_ADDRLO_7_S)
#define EMAC_ADDRLO_7_V  0xFFFFFFFFU
#define EMAC_ADDRLO_7_S  0

/** EMAC_ANCONTROL_REG register
 *  Enables and/or restarts autonegotiation This register also enables the Physical
 *  Coding Sublayer _PCS_ loopback This register is present only when you select the
 *  TBI, RTBI, or SGMII interface in coreConsultant
 */
#define EMAC_ANCONTROL_REG (DR_REG_EMAC_BASE + 0xc0)
/** EMAC_RAN : R/W1S; bitpos: [9]; default: 0;
 *  Restart AutoNegotiation When set, this bit causes autonegotiation to restart if Bit
 *  12 _ANE_ is set This bit is selfclearing after autonegotiation starts This bit
 *  should be cleared for normal operation
 */
#define EMAC_RAN    (BIT(9))
#define EMAC_RAN_M  (EMAC_RAN_V << EMAC_RAN_S)
#define EMAC_RAN_V  0x00000001U
#define EMAC_RAN_S  9
/** EMAC_ANE : R/W; bitpos: [12]; default: 0;
 *  AutoNegotiation Enable When set, this bit enables the MAC to perform
 *  autonegotiation with the link partner Clearing this bit disables the autonegotiation
 */
#define EMAC_ANE    (BIT(12))
#define EMAC_ANE_M  (EMAC_ANE_V << EMAC_ANE_S)
#define EMAC_ANE_V  0x00000001U
#define EMAC_ANE_S  12
/** EMAC_ELE : R/W; bitpos: [14]; default: 0;
 *  External Loopback Enable When set, this bit causes the PHY to loopback the transmit
 *  data into the receive path The pcs_ewrap_o signal is asserted high when this bit is
 *  set
 */
#define EMAC_ELE    (BIT(14))
#define EMAC_ELE_M  (EMAC_ELE_V << EMAC_ELE_S)
#define EMAC_ELE_V  0x00000001U
#define EMAC_ELE_S  14
/** EMAC_ECD : R/W; bitpos: [16]; default: 0;
 *  Enable Comma Detect When set, this bit enables the PHY for comma detection and word
 *  resynchronization This bit controls the pcs_en_cdet_o signal on the TBI, RTBI, or
 *  SGMII interface
 */
#define EMAC_ECD    (BIT(16))
#define EMAC_ECD_M  (EMAC_ECD_V << EMAC_ECD_S)
#define EMAC_ECD_V  0x00000001U
#define EMAC_ECD_S  16
/** EMAC_LR : R/W; bitpos: [17]; default: 0;
 *  Lock to Reference When set, this bit enables the PHY to lock its PLL to the 125 MHz
 *  reference clock This bit controls the pcs_lck_ref_o signal on the TBI, RTBI, or
 *  SGMII interface
 */
#define EMAC_LR    (BIT(17))
#define EMAC_LR_M  (EMAC_LR_V << EMAC_LR_S)
#define EMAC_LR_V  0x00000001U
#define EMAC_LR_S  17
/** EMAC_SGMRAL : R/W; bitpos: [18]; default: 0;
 *  SGMII RAL Control When set, this bit forces the SGMII RAL block to operate in the
 *  speed configured in the Speed and Port Select bits of the MAC Configuration
 *  register This is useful when the SGMII interface is used in a direct MAC to MAC
 *  connection _without a PHY_ and any MAC must reconfigure the speed When reset, the
 *  SGMII RAL block operates according to the link speed status received on SGMII _from
 *  the PHY_ This bit is reserved _and RO_ if the SGMII PHY interface is not selected
 *  during core configuration
 */
#define EMAC_SGMRAL    (BIT(18))
#define EMAC_SGMRAL_M  (EMAC_SGMRAL_V << EMAC_SGMRAL_S)
#define EMAC_SGMRAL_V  0x00000001U
#define EMAC_SGMRAL_S  18

/** EMAC_ANSTATUS_REG register
 *  Indicates the link and autonegotiation status This register is present only when
 *  you select the TBI, RTBI, or SGMII interface in coreConsultant
 */
#define EMAC_ANSTATUS_REG (DR_REG_EMAC_BASE + 0xc4)
/** EMAC_LS : RO; bitpos: [2]; default: 0;
 *  Link Status This bit indicates whether the data channel _link_ is up or down For
 *  the TBI, RTBI or SGMII interfaces, if ANEG is going on, data cannot be transferred
 *  across the link and hence the link is given as down
 */
#define EMAC_LS    (BIT(2))
#define EMAC_LS_M  (EMAC_LS_V << EMAC_LS_S)
#define EMAC_LS_V  0x00000001U
#define EMAC_LS_S  2
/** EMAC_ANA : RO; bitpos: [3]; default: 1;
 *  AutoNegotiation Ability This bit is always high because the MAC supports auto
 *  negotiation
 */
#define EMAC_ANA    (BIT(3))
#define EMAC_ANA_M  (EMAC_ANA_V << EMAC_ANA_S)
#define EMAC_ANA_V  0x00000001U
#define EMAC_ANA_S  3
/** EMAC_ANC : RO; bitpos: [5]; default: 0;
 *  AutoNegotiation Complete When set, this bit indicates that the autonegotiation
 *  process is complete This bit is cleared when autonegotiation is reinitiated
 */
#define EMAC_ANC    (BIT(5))
#define EMAC_ANC_M  (EMAC_ANC_V << EMAC_ANC_S)
#define EMAC_ANC_V  0x00000001U
#define EMAC_ANC_S  5
/** EMAC_ES : RO; bitpos: [8]; default: 1;
 *  Extended Status This bit is tied to high if the TBI or RTBI interface is selected
 *  during core configuration indicating that the MAC supports extended status
 *  information in Register 53 _TBI Extended  Status Register_ This bit is tied to low
 *  if the SGMII interface is selected and the TBI or RTBI interface is not selected
 *  during core configuration indicating that Register 53 is not present
 */
#define EMAC_ES    (BIT(8))
#define EMAC_ES_M  (EMAC_ES_V << EMAC_ES_S)
#define EMAC_ES_V  0x00000001U
#define EMAC_ES_S  8

/** EMAC_AUTONEGOTIATIONADVERTISEMENT_REG register
 *  This register is configured before autonegotiation begins It contains the
 *  advertised ability of the MAC This register is present only when you select the TBI
 *  or RTBI interface in coreConsultant
 */
#define EMAC_AUTONEGOTIATIONADVERTISEMENT_REG (DR_REG_EMAC_BASE + 0xc8)
/** EMAC_FD : R/W; bitpos: [5]; default: 1;
 *  FullDuplex When set high, this bit indicates that the MAC supports the fullduplex
 *  mode
 */
#define EMAC_FD    (BIT(5))
#define EMAC_FD_M  (EMAC_FD_V << EMAC_FD_S)
#define EMAC_FD_V  0x00000001U
#define EMAC_FD_S  5
/** EMAC_HD : R/W; bitpos: [6]; default: 1;
 *  HalfDuplex When set high, this bit indicates that the MAC supports the halfduplex
 *  mode This bit is always low _and RO_ when the MAC is configured for the
 *  fullduplexonly mode
 */
#define EMAC_HD    (BIT(6))
#define EMAC_HD_M  (EMAC_HD_V << EMAC_HD_S)
#define EMAC_HD_V  0x00000001U
#define EMAC_HD_S  6
/** EMAC_PSE : R/W; bitpos: [8:7]; default: 3;
 *  Pause Encoding These bits provide an encoding for the Pause bits, indicating that
 *  the MAC is capable of configuring the Pause function as defined in IEEE 8023x The
 *  encoding of these bits is defined in IEEE 8023z, Section 37214
 */
#define EMAC_PSE    0x00000003U
#define EMAC_PSE_M  (EMAC_PSE_V << EMAC_PSE_S)
#define EMAC_PSE_V  0x00000003U
#define EMAC_PSE_S  7
/** EMAC_RFE : R/W; bitpos: [13:12]; default: 0;
 *  Remote Fault Encoding These bits provide a remote fault encoding, indicating to a
 *  link partner that a fault or error condition has occurred The encoding of these
 *  bits is defined in IEEE 8023z, Section 37215
 */
#define EMAC_RFE    0x00000003U
#define EMAC_RFE_M  (EMAC_RFE_V << EMAC_RFE_S)
#define EMAC_RFE_V  0x00000003U
#define EMAC_RFE_S  12
/** EMAC_NP : RO; bitpos: [15]; default: 0;
 *  Next Page Support This bit is always low because the MAC does not support the next
 *  page
 */
#define EMAC_NP    (BIT(15))
#define EMAC_NP_M  (EMAC_NP_V << EMAC_NP_S)
#define EMAC_NP_V  0x00000001U
#define EMAC_NP_S  15

/** EMAC_AUTONEGOTIATIONLINKPARTNERABILITY_REG register
 *  Contains the advertised ability of the link partner Its value is valid after
 *  successful completion of autonegotiation or when a new base page has been received
 *  _indicated in the AutoNegotiation Expansion Register_ This register is present only
 *  when you select the TBI or RTBI interface in coreConsultant
 */
#define EMAC_AUTONEGOTIATIONLINKPARTNERABILITY_REG (DR_REG_EMAC_BASE + 0xcc)
/** EMAC_FD_ABILITY : RO; bitpos: [5]; default: 0;
 *  FullDuplex When set, this bit indicates that the link partner has the ability to
 *  operate in the full duplex mode When cleared, this bit indicates that the link
 *  partner does not have the ability to operate in the fullduplex mode
 */
#define EMAC_FD_ABILITY    (BIT(5))
#define EMAC_FD_ABILITY_M  (EMAC_FD_ABILITY_V << EMAC_FD_ABILITY_S)
#define EMAC_FD_ABILITY_V  0x00000001U
#define EMAC_FD_ABILITY_S  5
/** EMAC_HD_ABILITY : RO; bitpos: [6]; default: 0;
 *  HalfDuplex When set, this bit indicates that the link partner has the ability to
 *  operate in the halfduplex mode When cleared, this bit indicates that the link
 *  partner does not have the ability to operate in the halfduplex mode
 */
#define EMAC_HD_ABILITY    (BIT(6))
#define EMAC_HD_ABILITY_M  (EMAC_HD_ABILITY_V << EMAC_HD_ABILITY_S)
#define EMAC_HD_ABILITY_V  0x00000001U
#define EMAC_HD_ABILITY_S  6
/** EMAC_PSE_ABILITY : RO; bitpos: [8:7]; default: 0;
 *  Pause Encoding These bits provide an encoding for the Pause bits, indicating that
 *  the link partner's capability of configuring the Pause function as defined in the
 *  IEEE 8023x specification The encoding of these bits is defined in IEEE 8023z,
 *  Section 37214
 */
#define EMAC_PSE_ABILITY    0x00000003U
#define EMAC_PSE_ABILITY_M  (EMAC_PSE_ABILITY_V << EMAC_PSE_ABILITY_S)
#define EMAC_PSE_ABILITY_V  0x00000003U
#define EMAC_PSE_ABILITY_S  7
/** EMAC_RFE_ABILITY : RO; bitpos: [13:12]; default: 0;
 *  Remote Fault Encoding These bits provide a remote fault encoding, indicating a
 *  fault or error condition of the link partner The encoding of these bits is defined
 *  in IEEE 8023z, Section 37215
 */
#define EMAC_RFE_ABILITY    0x00000003U
#define EMAC_RFE_ABILITY_M  (EMAC_RFE_ABILITY_V << EMAC_RFE_ABILITY_S)
#define EMAC_RFE_ABILITY_V  0x00000003U
#define EMAC_RFE_ABILITY_S  12
/** EMAC_ACK : RO; bitpos: [14]; default: 0;
 *  Acknowledge When set, the autonegotiation function uses this bit to indicate that
 *  the link partner has successfully received the base page of the MAC When cleared,
 *  it indicates that the link partner did not successfully receive the base page of
 *  the MAC
 */
#define EMAC_ACK    (BIT(14))
#define EMAC_ACK_M  (EMAC_ACK_V << EMAC_ACK_S)
#define EMAC_ACK_V  0x00000001U
#define EMAC_ACK_S  14
/** EMAC_NO : RO; bitpos: [15]; default: 0;
 *  Next Page Support When set, this bit indicates that more next page information is
 *  available When cleared, this bit indicates that next page exchange is not desired
 */
#define EMAC_NO    (BIT(15))
#define EMAC_NO_M  (EMAC_NO_V << EMAC_NO_S)
#define EMAC_NO_V  0x00000001U
#define EMAC_NO_S  15

/** EMAC_AUTONEGOTIATIONEXPANSION_REG register
 *  Indicates whether a new base page has been received from the link partner This
 *  register is present only when you select the TBI or RTBI interface in coreConsultant
 */
#define EMAC_AUTONEGOTIATIONEXPANSION_REG (DR_REG_EMAC_BASE + 0xd0)
/** EMAC_NPR : RO; bitpos: [1]; default: 0;
 *  New Page Received When set, this bit indicates that the MAC has received a new page
 *  This bit is cleared when read
 */
#define EMAC_NPR    (BIT(1))
#define EMAC_NPR_M  (EMAC_NPR_V << EMAC_NPR_S)
#define EMAC_NPR_V  0x00000001U
#define EMAC_NPR_S  1
/** EMAC_NPA : RO; bitpos: [2]; default: 0;
 *  Next Page Ability This bit is always low because the MAC does not support the next
 *  page function
 */
#define EMAC_NPA    (BIT(2))
#define EMAC_NPA_M  (EMAC_NPA_V << EMAC_NPA_S)
#define EMAC_NPA_V  0x00000001U
#define EMAC_NPA_S  2

/** EMAC_TBIEXTENDEDSTATUS_REG register
 *  Indicates all modes of operation of the MAC This register is present only when you
 *  select the TBI or RTBI interface in coreConsultant
 */
#define EMAC_TBIEXTENDEDSTATUS_REG (DR_REG_EMAC_BASE + 0xd4)
/** EMAC_GHD : RO; bitpos: [14]; default: 1;
 *  1000BASEX HalfDuplex Capable This bit indicates that the MAC is able to perform the
 *  halfduplex and 1000BASEX operations This bit is always low when the MAC is
 *  configured for the fullduplexonly operation during core configuration
 */
#define EMAC_GHD    (BIT(14))
#define EMAC_GHD_M  (EMAC_GHD_V << EMAC_GHD_S)
#define EMAC_GHD_V  0x00000001U
#define EMAC_GHD_S  14
/** EMAC_GFD : RO; bitpos: [15]; default: 1;
 *  1000BASEX FullDuplex Capable This bit indicates that the MAC is able to perform the
 *  fullduplex and 1000BASEX operations
 */
#define EMAC_GFD    (BIT(15))
#define EMAC_GFD_M  (EMAC_GFD_V << EMAC_GFD_S)
#define EMAC_GFD_V  0x00000001U
#define EMAC_GFD_S  15

/** EMAC_SGMII_RGMII_SMIICONTROLANDSTATUS_REG register
 *  Indicates the status signals received from the PHY through the SGMII, RGMII, or
 *  SMII interface This register is present only when you select the SGMII, RGMII, or
 *  SMII interface in coreConsultant
 */
#define EMAC_SGMII_RGMII_SMIICONTROLANDSTATUS_REG (DR_REG_EMAC_BASE + 0xd8)
/** EMAC_LNKMOD : RO; bitpos: [0]; default: 0;
 *  Link Mode This bit indicates the current mode of operation of the link: 1’b0:
 *  Halfduplex mode 1’b1: Fullduplex mode
 */
#define EMAC_LNKMOD    (BIT(0))
#define EMAC_LNKMOD_M  (EMAC_LNKMOD_V << EMAC_LNKMOD_S)
#define EMAC_LNKMOD_V  0x00000001U
#define EMAC_LNKMOD_S  0
/** EMAC_LNKSPEED : RO; bitpos: [2:1]; default: 2;
 *  Link Speed
 */
#define EMAC_LNKSPEED    0x00000003U
#define EMAC_LNKSPEED_M  (EMAC_LNKSPEED_V << EMAC_LNKSPEED_S)
#define EMAC_LNKSPEED_V  0x00000003U
#define EMAC_LNKSPEED_S  1
/** EMAC_LNKSTS : RO; bitpos: [3]; default: 0;
 *  Link Status This bit indicates whether the link between the local PHY and the
 *  remote PHY is up or down It gives the status of the link between the SGMII of MAC
 *  and the SGMII of the local PHY The status bits are received from the local PHY
 *  during ANEG between he MAC and PHY on the SGMII link
 */
#define EMAC_LNKSTS    (BIT(3))
#define EMAC_LNKSTS_M  (EMAC_LNKSTS_V << EMAC_LNKSTS_S)
#define EMAC_LNKSTS_V  0x00000001U
#define EMAC_LNKSTS_S  3
/** EMAC_JABTO : RO; bitpos: [4]; default: 0;
 *  Jabber Timeout This bit indicates whether there is jabber timeout error _1'b1_ in
 *  the received frame This bit is reserved when the MAC is configured for the SGMII or
 *  RGMII PHY interface
 */
#define EMAC_JABTO    (BIT(4))
#define EMAC_JABTO_M  (EMAC_JABTO_V << EMAC_JABTO_S)
#define EMAC_JABTO_V  0x00000001U
#define EMAC_JABTO_S  4
/** EMAC_FALSCARDET : RO; bitpos: [5]; default: 0;
 *  False Carrier Detected This bit indicates whether the SMII PHY detected false
 *  carrier _1'b1_ This bit is reserved when the MAC is configured for the SGMII or
 *  RGMII PHY interface
 */
#define EMAC_FALSCARDET    (BIT(5))
#define EMAC_FALSCARDET_M  (EMAC_FALSCARDET_V << EMAC_FALSCARDET_S)
#define EMAC_FALSCARDET_V  0x00000001U
#define EMAC_FALSCARDET_S  5
/** EMAC_SMIDRXS : R/W; bitpos: [16]; default: 0;
 *  Delay SMII RX Data Sampling with respect to the SMII SYNC Signal When set, the
 *  first bit of the SMII RX data is sampled one cycle after the SMII SYNC signal When
 *  reset, the first bit of the SMII RX data is sampled along with the SMII SYNC signal
 *  If the SMII PHY Interface with source synchronous mode is selected during core
 *  configuration, this bit is reserved _RO with default value_
 */
#define EMAC_SMIDRXS    (BIT(16))
#define EMAC_SMIDRXS_M  (EMAC_SMIDRXS_V << EMAC_SMIDRXS_S)
#define EMAC_SMIDRXS_V  0x00000001U
#define EMAC_SMIDRXS_S  16

/** EMAC_WATCHDOGTIMEOUT_REG register
 *  Controls the watchdog timeout for received frames
 */
#define EMAC_WATCHDOGTIMEOUT_REG (DR_REG_EMAC_BASE + 0xdc)
/** EMAC_WTO : R/W; bitpos: [13:0]; default: 0;
 *  Watchdog Timeout When Bit 16 _PWE_ is set and Bit 23 _WD_ of Register 0 _MAC
 *  Configuration Register_ is reset, this field is used as watchdog timeout for a
 *  received frame If the length of a received frame exceeds the value of this field,
 *  such frame is terminated and declared as an error frame Note: When Bit 16 _PWE_ is
 *  set, the value in this field should be more than 1,522 _0x05F2_ Otherwise, the IEEE
 *  Std 8023specified valid tagged frames are declared as error frames and are dropped
 */
#define EMAC_WTO    0x00003FFFU
#define EMAC_WTO_M  (EMAC_WTO_V << EMAC_WTO_S)
#define EMAC_WTO_V  0x00003FFFU
#define EMAC_WTO_S  0
/** EMAC_PWE : R/W; bitpos: [16]; default: 0;
 *  Programmable Watchdog Enable When this bit is set and Bit 23 _WD_ of Register 0
 *  _MAC Configuration Register_ is reset, the WTO field _Bits[13:0]_ is used as
 *  watchdog timeout for a received frame When this bit is cleared, the watchdog
 *  timeout for a received frame is controlled by the setting of Bit 23 _WD_ and Bit 20
 *  _JE_ in Register 0 _MAC Configuration Register_
 */
#define EMAC_PWE    (BIT(16))
#define EMAC_PWE_M  (EMAC_PWE_V << EMAC_PWE_S)
#define EMAC_PWE_V  0x00000001U
#define EMAC_PWE_S  16

/** EMAC_GENERALPURPOSEIO_REG register
 *  Provides the control to drive up to 4 bits of output ports _GPO_ and also provides
 *  the status of up to 4 input ports _GPIS_
 */
#define EMAC_GENERALPURPOSEIO_REG (DR_REG_EMAC_BASE + 0xe0)
/** EMAC_GPIS : R/W; bitpos: [3:0]; default: 0;
 *  General Purpose Input Status This field gives the status of the signals connected
 *  to the gpi_i input ports This field is of the following types based on the setting
 *  of the corresponding GPIT field of this register: Latchedlow _LL_: This field is
 *  cleared when the corresponding gpi_i input becomes low This field remains low until
 *  the host reads this field After this, this field reflects the current value of the
 *  gpi_i input Latchedhigh _LH_: This field is set when the corresponding gpi_i input
 *  becomes high This field remains high until the host reads this field After this,
 *  this field reflects the current value of the gpi_i input The number of bits
 *  available in this field depend on the GP Input Signal Width option Other bits are
 *  not used _reserved and always reset_
 */
#define EMAC_GPIS    0x0000000FU
#define EMAC_GPIS_M  (EMAC_GPIS_V << EMAC_GPIS_S)
#define EMAC_GPIS_V  0x0000000FU
#define EMAC_GPIS_S  0
/** EMAC_GPO : R/W; bitpos: [11:8]; default: 0;
 *  General Purpose Output When this bit is set, it directly drives the gpo_o output
 *  ports When this bit is reset, it does not directly drive the gpo_o output ports The
 *  number of bits available in this field depend on the GP Output Signal Width option
 *  Other bits are not used _reserved and always reset_
 */
#define EMAC_GPO    0x0000000FU
#define EMAC_GPO_M  (EMAC_GPO_V << EMAC_GPO_S)
#define EMAC_GPO_V  0x0000000FU
#define EMAC_GPO_S  8
/** EMAC_GPIE : R/W; bitpos: [19:16]; default: 0;
 *  GPI Interrupt Enable When this bit is set and the programmed event _LL or LH_
 *  occurs on the corresponding GPIS bit, Bit 11 _GPIIS_ of Register 14 _Interrupt
 *  Status Register_ is set Accordingly, the interrupt is generated on the mci_intr_o
 *  or sbd_intr_o The GPIIS bit is cleared when the host reads the Bits[7:0] of this
 *  register When reset, Bit 11 _GPIIS_ of Register 14 _Interrupt Status Register_ is
 *  not set when any event occurs on the corresponding GPIS bits The number of bits
 *  available in this field depend on the GP Input Signal Width option Other bits are
 *  not used _reserved and always reset_
 */
#define EMAC_GPIE    0x0000000FU
#define EMAC_GPIE_M  (EMAC_GPIE_V << EMAC_GPIE_S)
#define EMAC_GPIE_V  0x0000000FU
#define EMAC_GPIE_S  16
/** EMAC_GPIT : R/W; bitpos: [27:24]; default: 0;
 *  GPI Type When set, this bit indicates that the corresponding GPIS is of latchedlow
 *  _LL_ type When reset, this bit indicates that the corresponding GPIS is of
 *  latchedhigh _LH_ type The number of bits available in this field depend on the GP
 *  Input Signal Width option Other bits are not used _reserved and always reset_
 */
#define EMAC_GPIT    0x0000000FU
#define EMAC_GPIT_M  (EMAC_GPIT_V << EMAC_GPIT_S)
#define EMAC_GPIT_V  0x0000000FU
#define EMAC_GPIT_S  24

/** EMAC_LAYER3ANDLAYER4CONTROLREGISTER0_REG register
 *  Controls the operations of the Layer 3 and Layer 4 frame filtering
 */
#define EMAC_LAYER3ANDLAYER4CONTROLREGISTER0_REG (DR_REG_EMAC_BASE + 0x400)
/** EMAC_L3PEN0 : R/W; bitpos: [0]; default: 0;
 *  Layer 3 Protocol Enable When set, this bit indicates that the Layer 3 IP Source or
 *  Destination Address matching is enabled for the IPv6 frames When reset, this bit
 *  indicates that the Layer 3 IP Source or Destination Address matching is enabled for
 *  the IPv4 frames The Layer 3 matching is done only when either L3SAM0 or L3DAM0 bit
 *  is set high
 */
#define EMAC_L3PEN0    (BIT(0))
#define EMAC_L3PEN0_M  (EMAC_L3PEN0_V << EMAC_L3PEN0_S)
#define EMAC_L3PEN0_V  0x00000001U
#define EMAC_L3PEN0_S  0
/** EMAC_L3SAM0 : R/W; bitpos: [2]; default: 0;
 *  Layer 3 IP SA Match Enable When set, this bit indicates that the Layer 3 IP Source
 *  Address field is enabled for matching When reset, the MAC ignores the Layer 3 IP
 *  Source Address field for matching Note: When Bit 0 _L3PEN0_ is set, you should set
 *  either this bit or Bit 4 _L3DAM0_ because either IPv6 SA or DA can be checked for
 *  filtering
 */
#define EMAC_L3SAM0    (BIT(2))
#define EMAC_L3SAM0_M  (EMAC_L3SAM0_V << EMAC_L3SAM0_S)
#define EMAC_L3SAM0_V  0x00000001U
#define EMAC_L3SAM0_S  2
/** EMAC_L3SAIM0 : R/W; bitpos: [3]; default: 0;
 *  Layer 3 IP SA Inverse Match Enable When set, this bit indicates that the Layer 3 IP
 *  Source Address field is enabled for inverse matching When reset, this bit indicates
 *  that the Layer 3 IP Source Address field is enabled for perfect matching This bit
 *  is valid and applicable only when Bit 2 _L3SAM0_ is set high
 */
#define EMAC_L3SAIM0    (BIT(3))
#define EMAC_L3SAIM0_M  (EMAC_L3SAIM0_V << EMAC_L3SAIM0_S)
#define EMAC_L3SAIM0_V  0x00000001U
#define EMAC_L3SAIM0_S  3
/** EMAC_L3DAM0 : R/W; bitpos: [4]; default: 0;
 *  Layer 3 IP DA Match Enable When set, this bit indicates that Layer 3 IP Destination
 *  Address field is enabled for matching When reset, the MAC ignores the Layer 3 IP
 *  Destination Address field for matching Note: When Bit 0 _L3PEN0_ is set, you should
 *  set either this bit or Bit 2 _L3SAM0_ because either IPv6 DA or SA can be checked
 *  for filtering
 */
#define EMAC_L3DAM0    (BIT(4))
#define EMAC_L3DAM0_M  (EMAC_L3DAM0_V << EMAC_L3DAM0_S)
#define EMAC_L3DAM0_V  0x00000001U
#define EMAC_L3DAM0_S  4
/** EMAC_L3DAIM0 : R/W; bitpos: [5]; default: 0;
 *  Layer 3 IP DA Inverse Match Enable When set, this bit indicates that the Layer 3 IP
 *  Destination Address field is enabled for inverse matching When reset, this bit
 *  indicates that the Layer 3 IP Destination Address field is enabled for perfect
 *  matching This bit is valid and applicable only when Bit 4 _L3DAM0_ is set high
 */
#define EMAC_L3DAIM0    (BIT(5))
#define EMAC_L3DAIM0_M  (EMAC_L3DAIM0_V << EMAC_L3DAIM0_S)
#define EMAC_L3DAIM0_V  0x00000001U
#define EMAC_L3DAIM0_S  5
/** EMAC_L3HSBM0 : R/W; bitpos: [10:6]; default: 0;
 *  Layer 3 IP SA Higher Bits Match IPv4 Frames: This field contains the number of
 *  lower bits of IP Source Address that are masked for matching in the IPv4 frames The
 *  following list describes the values of this field: 0: No bits are masked 1: LSb[0]
 *  is masked 2: Two LSbs [1:0] are masked  31: All bits except MSb are masked IPv6
 *  Frames: This field contains Bits [4:0] of the field that indicates the number of
 *  higher bits of IP Source or Destination Address matched in the IPv6 frames This
 *  field is valid and applicable only if L3DAM0 or L3SAM0 is set high
 */
#define EMAC_L3HSBM0    0x0000001FU
#define EMAC_L3HSBM0_M  (EMAC_L3HSBM0_V << EMAC_L3HSBM0_S)
#define EMAC_L3HSBM0_V  0x0000001FU
#define EMAC_L3HSBM0_S  6
/** EMAC_L3HDBM0 : R/W; bitpos: [15:11]; default: 0;
 *  Layer 3 IP DA Higher Bits Match IPv4 Frames: This field contains the number of
 *  higher bits of IP Destination Address that are matched in the IPv4 frames The
 *  following list describes the values of this field: 0: No bits are masked 1: LSb[0]
 *  is masked 2: Two LSbs [1:0] are masked  31: All bits except MSb are masked IPv6
 *  Frames: Bits [12:11] of this field correspond to Bits [6:5] of L3HSBM0, which
 *  indicate the number of lower bits of IP Source or Destination Address that are
 *  masked in the IPv6 frames The following list describes the concatenated values of
 *  the L3HDBM0[1:0] and L3HSBM0 bits: 0: No bits are masked 1: LSb[0] is masked 2: Two
 *  LSbs [1:0] are masked … 127: All bits except MSb are masked This field is valid and
 *  applicable only if L3DAM0 or L3SAM0 is set high
 */
#define EMAC_L3HDBM0    0x0000001FU
#define EMAC_L3HDBM0_M  (EMAC_L3HDBM0_V << EMAC_L3HDBM0_S)
#define EMAC_L3HDBM0_V  0x0000001FU
#define EMAC_L3HDBM0_S  11
/** EMAC_L4PEN0 : R/W; bitpos: [16]; default: 0;
 *  Layer 4 Protocol Enable When set, this bit indicates that the Source and
 *  Destination Port number fields for UDP frames are used for matching When reset,
 *  this bit indicates that the Source and Destination Port number fields for TCP
 *  frames are used for matching The Layer 4 matching is done only when either L4SPM0
 *  or L4DPM0 bit is set high
 */
#define EMAC_L4PEN0    (BIT(16))
#define EMAC_L4PEN0_M  (EMAC_L4PEN0_V << EMAC_L4PEN0_S)
#define EMAC_L4PEN0_V  0x00000001U
#define EMAC_L4PEN0_S  16
/** EMAC_L4SPM0 : R/W; bitpos: [18]; default: 0;
 *  Layer 4 Source Port Match Enable When set, this bit indicates that the Layer 4
 *  Source Port number field is enabled for matching When reset, the MAC ignores the
 *  Layer 4 Source Port number field for matching
 */
#define EMAC_L4SPM0    (BIT(18))
#define EMAC_L4SPM0_M  (EMAC_L4SPM0_V << EMAC_L4SPM0_S)
#define EMAC_L4SPM0_V  0x00000001U
#define EMAC_L4SPM0_S  18
/** EMAC_L4SPIM0 : R/W; bitpos: [19]; default: 0;
 *  Layer 4 Source Port Inverse Match Enable When set, this bit indicates that the
 *  Layer 4 Source Port number field is enabled for inverse matching When reset, this
 *  bit indicates that the Layer 4 Source Port number field is enabled for perfect
 *  matching This bit is valid and applicable only when Bit 18 _L4SPM0_ is set high
 */
#define EMAC_L4SPIM0    (BIT(19))
#define EMAC_L4SPIM0_M  (EMAC_L4SPIM0_V << EMAC_L4SPIM0_S)
#define EMAC_L4SPIM0_V  0x00000001U
#define EMAC_L4SPIM0_S  19
/** EMAC_L4DPM0 : R/W; bitpos: [20]; default: 0;
 *  Layer 4 Destination Port Match Enable When set, this bit indicates that the Layer 4
 *  Destination Port number field is enabled for matching When reset, the MAC ignores
 *  the Layer 4 Destination Port number field for matching
 */
#define EMAC_L4DPM0    (BIT(20))
#define EMAC_L4DPM0_M  (EMAC_L4DPM0_V << EMAC_L4DPM0_S)
#define EMAC_L4DPM0_V  0x00000001U
#define EMAC_L4DPM0_S  20
/** EMAC_L4DPIM0 : R/W; bitpos: [21]; default: 0;
 *  Layer 4 Destination Port Inverse Match Enable When set, this bit indicates that the
 *  Layer 4 Destination Port number field is enabled for inverse matching When reset,
 *  this bit indicates that the Layer 4 Destination Port number field is enabled for
 *  perfect matching This bit is valid and applicable only when Bit 20 _L4DPM0_ is set
 *  high
 */
#define EMAC_L4DPIM0    (BIT(21))
#define EMAC_L4DPIM0_M  (EMAC_L4DPIM0_V << EMAC_L4DPIM0_S)
#define EMAC_L4DPIM0_V  0x00000001U
#define EMAC_L4DPIM0_S  21

/** EMAC_LAYER4ADDRESSREGISTER0_REG register
 *  Layer 4 Port number field It contains the 16bit Source and Destination Port numbers
 *  of the TCP or UDP frame
 */
#define EMAC_LAYER4ADDRESSREGISTER0_REG (DR_REG_EMAC_BASE + 0x404)
/** EMAC_L4SP0 : R/W; bitpos: [15:0]; default: 0;
 *  Layer 4 Source Port Number Field When Bit 16 _L4PEN0_ is reset and Bit 20 _L4DPM0_
 *  is set in Register 256 _Layer 3 and Layer 4 Control Register 0_, this field
 *  contains the value to be matched with the TCP Source Port Number field in the IPv4
 *  or IPv6 frames When Bit 16 _L4PEN0_ and Bit 20 _L4DPM0_ are set in Register 256
 *  _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
 *  matched with the UDP Source Port Number field in the IPv4 or IPv6 frames
 */
#define EMAC_L4SP0    0x0000FFFFU
#define EMAC_L4SP0_M  (EMAC_L4SP0_V << EMAC_L4SP0_S)
#define EMAC_L4SP0_V  0x0000FFFFU
#define EMAC_L4SP0_S  0
/** EMAC_L4DP0 : R/W; bitpos: [31:16]; default: 0;
 *  Layer 4 Destination Port Number Field When Bit 16 _L4PEN0_ is reset and Bit 20
 *  _L4DPM0_ is set in Register 256 _Layer 3 and Layer 4 Control Register 0_, this
 *  field contains the value to be matched with the TCP Destination Port Number field
 *  in the IPv4 or IPv6 frames When Bit 16 _L4PEN0_ and Bit 20 _L4DPM0_ are set in
 *  Register 256 _Layer 3 and Layer 4 Control Register 0_, this field contains the
 *  value to be matched with the UDP Destination Port Number field in the IPv4 or IPv6
 *  frames
 */
#define EMAC_L4DP0    0x0000FFFFU
#define EMAC_L4DP0_M  (EMAC_L4DP0_V << EMAC_L4DP0_S)
#define EMAC_L4DP0_V  0x0000FFFFU
#define EMAC_L4DP0_S  16

/** EMAC_LAYER3ADDRESS0REGISTER0_REG register
 *  Layer 3 Address field For IPv4 frames, it contains the 32bit IP Source Address
 *  field For IPv6 frames, it contains Bits [31:0] of the 128bit IP Source Address or
 *  Destination Address field
 */
#define EMAC_LAYER3ADDRESS0REGISTER0_REG (DR_REG_EMAC_BASE + 0x410)
/** EMAC_L3A00 : R/W; bitpos: [31:0]; default: 0;
 *  Layer 3 Address 0 Field When Bit 0 _L3PEN0_ and Bit 2 _L3SAM0_ are set in Register
 *  256 _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
 *  matched with Bits [31:0] of the IP Source Address field in the IPv6 frames When Bit
 *  0 _L3PEN0_ and Bit 4 _L3DAM0_ are set in Register 256 _Layer 3 and Layer 4 Control
 *  Register 0_, this field contains the value to be matched with Bits [31:0] of the IP
 *  Destination Address field in the IPv6 frames When Bit 0 _L3PEN0_ is reset and Bit 2
 *  _L3SAM0_ is set in Register 256 _Layer 3 and Layer 4 Control Register 0_, this
 *  field contains the value to be matched with the IP Source Address field in the IPv4
 *  frames
 */
#define EMAC_L3A00    0xFFFFFFFFU
#define EMAC_L3A00_M  (EMAC_L3A00_V << EMAC_L3A00_S)
#define EMAC_L3A00_V  0xFFFFFFFFU
#define EMAC_L3A00_S  0

/** EMAC_LAYER3ADDRESS1REGISTER0_REG register
 *  Layer 3 Address 1 field For IPv4 frames, it contains the 32bit IP Destination
 *  Address field For IPv6 frames, it contains Bits [63:32] of the 128bit IP Source
 *  Address or Destination Address field
 */
#define EMAC_LAYER3ADDRESS1REGISTER0_REG (DR_REG_EMAC_BASE + 0x414)
/** EMAC_L3A10 : R/W; bitpos: [31:0]; default: 0;
 *  Layer 3 Address 1 Field When Bit 0 _L3PEN0_ and Bit 2 _L3SAM0_ are set in Register
 *  256 _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
 *  matched with Bits [63:32] of the IP Source Address field in the IPv6 frames When
 *  Bit 0 _L3PEN0_ and Bit 4 _L3DAM0_ are set in Register 256 _Layer 3 and Layer 4
 *  Control Register 0_, this field contains the value to be matched with Bits [63:32]
 *  of the IP Destination Address field in the IPv6 frames When Bit 0 _L3PEN0_ is reset
 *  and Bit 4 _L3DAM0_ is set in Register 256 _Layer 3 and Layer 4 Control Register 0_,
 *  this field contains the value to be matched with the IP Destination Address field
 *  in the IPv4 frames
 */
#define EMAC_L3A10    0xFFFFFFFFU
#define EMAC_L3A10_M  (EMAC_L3A10_V << EMAC_L3A10_S)
#define EMAC_L3A10_V  0xFFFFFFFFU
#define EMAC_L3A10_S  0

/** EMAC_LAYER3ADDRESS2REGISTER0_REG register
 *  Layer 3 Address 2 field This register is reserved for IPv4 frames For IPv6 frames,
 *  it contains Bits [95:64] of the 128bit IP Source Address or Destination Address
 *  field
 */
#define EMAC_LAYER3ADDRESS2REGISTER0_REG (DR_REG_EMAC_BASE + 0x418)
/** EMAC_L3A20 : R/W; bitpos: [31:0]; default: 0;
 *  Layer 3 Address 2 Field When Bit 0 _L3PEN0_ and Bit 2 _L3SAM0_ are set in Register
 *  256 _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
 *  matched with Bits [95:64] of the IP Source Address field in the IPv6 frames When
 *  Bit 0 _L3PEN0_ and Bit 4 _L3DAM0_ are set in Register 256 _Layer 3 and Layer 4
 *  Control Register 0_, this field contains value to be matched with Bits [95:64] of
 *  the IP Destination Address field in the IPv6 frames When Bit 0 _L3PEN0_ is reset in
 *  Register 256 _Layer 3 and Layer 4 Control Register 0_, this register is not used
 */
#define EMAC_L3A20    0xFFFFFFFFU
#define EMAC_L3A20_M  (EMAC_L3A20_V << EMAC_L3A20_S)
#define EMAC_L3A20_V  0xFFFFFFFFU
#define EMAC_L3A20_S  0

/** EMAC_LAYER3ADDRESS3REGISTER0_REG register
 *  Layer 3 Address 3 field This register is reserved for IPv4 frames For IPv6 frames,
 *  it contains Bits [127:96] of the 128bit IP Source Address or Destination Address
 *  field
 */
#define EMAC_LAYER3ADDRESS3REGISTER0_REG (DR_REG_EMAC_BASE + 0x41c)
/** EMAC_L3A30 : R/W; bitpos: [31:0]; default: 0;
 *  Layer 3 Address 3 Field When Bit 0 _L3PEN0_ and Bit 2 _L3SAM0_ are set in Register
 *  256 _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
 *  matched with Bits [127:96] of the IP Source Address field in the IPv6 frames When
 *  Bit 0 _L3PEN0_ and Bit 4 _L3DAM0_ are set in Register 256 _Layer 3 and Layer 4
 *  Control Register 0_, this field contains the value to be matched with Bits [127:96]
 *  of the IP Destination Address field in the IPv6 frames When Bit 0 _L3PEN0_ is reset
 *  in Register 256 _Layer 3 and Layer 4 Control Register 0_, this register is not used
 */
#define EMAC_L3A30    0xFFFFFFFFU
#define EMAC_L3A30_M  (EMAC_L3A30_V << EMAC_L3A30_S)
#define EMAC_L3A30_V  0xFFFFFFFFU
#define EMAC_L3A30_S  0

/** EMAC_HASHTABLEREGISTER0_REG register
 *  This register contains the first 32 bits of the hash table when the width of the
 *  Hash table is 128 bits or 256 bits
 */
#define EMAC_HASHTABLEREGISTER0_REG (DR_REG_EMAC_BASE + 0x500)
/** EMAC_HT31T0 : R/W; bitpos: [31:0]; default: 0;
 *  First 32 bits of Hash Table This field contains the first 32 Bits _31:0_ of the
 *  Hash table Note Registers 321 through 327 are similar to Register 320 _Hash Table
 *  Register 0_ Registers 324 through 327 are present only when you select the 256bit
 *  Hash table during core configuration
 */
#define EMAC_HT31T0    0xFFFFFFFFU
#define EMAC_HT31T0_M  (EMAC_HT31T0_V << EMAC_HT31T0_S)
#define EMAC_HT31T0_V  0xFFFFFFFFU
#define EMAC_HT31T0_S  0

/** EMAC_VLANTAGINCLUSIONORREPLACEMENT_REG register
 *  This register contains the VLAN tag for insertion into or replacement in the
 *  transmit frames
 */
#define EMAC_VLANTAGINCLUSIONORREPLACEMENT_REG (DR_REG_EMAC_BASE + 0x584)
/** EMAC_VLT : R/W; bitpos: [15:0]; default: 0;
 *  VLAN Tag for Transmit Frames This field contains the value of the VLAN tag to be
 *  inserted or replaced The value must only be changed when the transmit lines are
 *  inactive or during the initialization phase Bits[15:13] are the User Priority, Bit
 *  12 is the CFI/DEI, and Bits[11:0] are the VLAN tag’s VID field
 */
#define EMAC_VLT    0x0000FFFFU
#define EMAC_VLT_M  (EMAC_VLT_V << EMAC_VLT_S)
#define EMAC_VLT_V  0x0000FFFFU
#define EMAC_VLT_S  0
/** EMAC_VLC : R/W; bitpos: [17:16]; default: 0;
 *  VLAN Tag Control in Transmit Frames 2’b00: No VLAN tag deletion, insertion, or
 *  replacement 2’b01: VLAN tag deletion The MAC removes the VLAN type _bytes 13 and
 *  14_ and VLAN tag _bytes 15 and 16_ of all transmitted frames with VLAN tags 2’b10:
 *  VLAN tag insertion The MAC inserts VLT in bytes 15 and 16 of the frame after
 *  inserting the Type value _0x8100/0x88a8_ in bytes 13 and 14 This operation is
 *  performed on all transmitted frames, irrespective of whether they already have a
 *  VLAN tag 2’b11: VLAN tag replacement The MAC replaces VLT in bytes 15 and 16 of all
 *  VLANtype transmitted frames _Bytes 13 and 14 are 0x8100/0x88a8_ Note: Changes to
 *  this field take effect only on the start of a frame If you write this register
 *  field when a frame is being transmitted, only the subsequent frame can use the
 *  updated value, that is, the current frame does not use the updated value
 */
#define EMAC_VLC    0x00000003U
#define EMAC_VLC_M  (EMAC_VLC_V << EMAC_VLC_S)
#define EMAC_VLC_V  0x00000003U
#define EMAC_VLC_S  16
/** EMAC_VLP : R/W; bitpos: [18]; default: 0;
 *  VLAN Priority Control When this bit is set, the control Bits [17:16] are used for
 *  VLAN deletion, insertion, or replacement When this bit is reset, the
 *  mti_vlan_ctrl_i control input is used, and Bits [17:16] are ignored
 */
#define EMAC_VLP    (BIT(18))
#define EMAC_VLP_M  (EMAC_VLP_V << EMAC_VLP_S)
#define EMAC_VLP_V  0x00000001U
#define EMAC_VLP_S  18
/** EMAC_CSVL : R/W; bitpos: [19]; default: 0;
 *  CVLAN or SVLAN When this bit is set, SVLAN type _0x88A8_ is inserted or replaced in
 *  the 13th and 14th bytes of transmitted frames When this bit is reset, CVLAN type
 *  _0x8100_ is inserted or replaced in the transmitted frames
 */
#define EMAC_CSVL    (BIT(19))
#define EMAC_CSVL_M  (EMAC_CSVL_V << EMAC_CSVL_S)
#define EMAC_CSVL_V  0x00000001U
#define EMAC_CSVL_S  19

/** EMAC_VLANHASHTABLE_REG register
 *  This register contains the VLAN hash table
 */
#define EMAC_VLANHASHTABLE_REG (DR_REG_EMAC_BASE + 0x588)
/** EMAC_VLHT : R/W; bitpos: [15:0]; default: 0;
 *  VLAN Hash Table This field contains the 16bit VLAN Hash Table
 */
#define EMAC_VLHT    0x0000FFFFU
#define EMAC_VLHT_M  (EMAC_VLHT_V << EMAC_VLHT_S)
#define EMAC_VLHT_V  0x0000FFFFU
#define EMAC_VLHT_S  0

/** EMAC_TIMESTAMPCONTROL_REG register
 *  Controls the timestamp generation and update logic This register is present only
 *  when IEEE1588 timestamping is enabled during coreConsultant configuration
 */
#define EMAC_TIMESTAMPCONTROL_REG (DR_REG_EMAC_BASE + 0x700)
/** EMAC_TSENA : R/W; bitpos: [0]; default: 0;
 *  Timestamp Enable When set, the timestamp is added for the transmit and receive
 *  frames When disabled, timestamp is not added for the transmit and receive frames
 *  and the Timestamp Generator is also suspended You need to initialize the Timestamp
 *  _system time_ after enabling this mode On the receive side, the MAC processes the
 *  1588 frames only if this bit is set
 */
#define EMAC_TSENA    (BIT(0))
#define EMAC_TSENA_M  (EMAC_TSENA_V << EMAC_TSENA_S)
#define EMAC_TSENA_V  0x00000001U
#define EMAC_TSENA_S  0
/** EMAC_TSCFUPDT : R/W; bitpos: [1]; default: 0;
 *  Timestamp Fine or Coarse Update When set, this bit indicates that the system times
 *  update should be done using the fine update method When reset, it indicates the
 *  system timestamp update should be done using the Coarse method
 */
#define EMAC_TSCFUPDT    (BIT(1))
#define EMAC_TSCFUPDT_M  (EMAC_TSCFUPDT_V << EMAC_TSCFUPDT_S)
#define EMAC_TSCFUPDT_V  0x00000001U
#define EMAC_TSCFUPDT_S  1
/** EMAC_TSINIT : R/W1S; bitpos: [2]; default: 0;
 *  Timestamp Initialize When set, the system time is initialized _overwritten_ with
 *  the value specified in the Register 452 _System Time  Seconds Update Register_ and
 *  Register 453 _System Time  Nanoseconds Update Register_ This bit should be read
 *  zero before updating it This bit is reset when the initialization is complete The
 *  “Timestamp Higher Word” register _if enabled during core configuration_ can only be
 *  initialized
 */
#define EMAC_TSINIT    (BIT(2))
#define EMAC_TSINIT_M  (EMAC_TSINIT_V << EMAC_TSINIT_S)
#define EMAC_TSINIT_V  0x00000001U
#define EMAC_TSINIT_S  2
/** EMAC_TSUPDT : R/W1S; bitpos: [3]; default: 0;
 *  Timestamp Update When set, the system time is updated _added or subtracted_ with
 *  the value specified in Register 452 _System Time  Seconds Update Register_ and
 *  Register 453 _System Time  Nanoseconds Update Register_ This bit should be read
 *  zero before updating it This bit is reset when the update is completed in hardware
 *  The “Timestamp Higher Word” register _if enabled during core configuration_ is not
 *  updated
 */
#define EMAC_TSUPDT    (BIT(3))
#define EMAC_TSUPDT_M  (EMAC_TSUPDT_V << EMAC_TSUPDT_S)
#define EMAC_TSUPDT_V  0x00000001U
#define EMAC_TSUPDT_S  3
/** EMAC_TSTRIG : R/W1S; bitpos: [4]; default: 0;
 *  Timestamp Interrupt Trigger Enable When set, the timestamp interrupt is generated
 *  when the System Time becomes greater than the value written in the Target Time
 *  register This bit is reset after the generation of the Timestamp Trigger Interrupt
 */
#define EMAC_TSTRIG    (BIT(4))
#define EMAC_TSTRIG_M  (EMAC_TSTRIG_V << EMAC_TSTRIG_S)
#define EMAC_TSTRIG_V  0x00000001U
#define EMAC_TSTRIG_S  4
/** EMAC_TSADDREG : R/W1S; bitpos: [5]; default: 0;
 *  Addend Reg Update When set, the content of the Timestamp Addend register is updated
 *  in the PTP block for fine correction This is cleared when the update is completed
 *  This register bit should be zero before setting it
 */
#define EMAC_TSADDREG    (BIT(5))
#define EMAC_TSADDREG_M  (EMAC_TSADDREG_V << EMAC_TSADDREG_S)
#define EMAC_TSADDREG_V  0x00000001U
#define EMAC_TSADDREG_S  5
/** EMAC_TSENALL : R/W; bitpos: [8]; default: 0;
 *  Enable Timestamp for All Frames When set, the timestamp snapshot is enabled for all
 *  frames received by the MAC
 */
#define EMAC_TSENALL    (BIT(8))
#define EMAC_TSENALL_M  (EMAC_TSENALL_V << EMAC_TSENALL_S)
#define EMAC_TSENALL_V  0x00000001U
#define EMAC_TSENALL_S  8
/** EMAC_TSCTRLSSR : R/W; bitpos: [9]; default: 0;
 *  Timestamp Digital or Binary Rollover Control When set, the Timestamp Low register
 *  rolls over after 0x3B9A_C9FF value _that is, 1 nanosecond accuracy_ and increments
 *  the timestamp _High_ seconds When reset, the rollover value of subsecond register
 *  is 0x7FFF_FFFF The subsecond increment has to be programmed correctly depending on
 *  the PTP reference clock frequency and the value of this bit
 */
#define EMAC_TSCTRLSSR    (BIT(9))
#define EMAC_TSCTRLSSR_M  (EMAC_TSCTRLSSR_V << EMAC_TSCTRLSSR_S)
#define EMAC_TSCTRLSSR_V  0x00000001U
#define EMAC_TSCTRLSSR_S  9
/** EMAC_TSVER2ENA : R/W; bitpos: [10]; default: 0;
 *  Enable PTP packet Processing for Version 2 Format When set, the PTP packets are
 *  processed using the 1588 version 2 format Otherwise, the PTP packets are processed
 *  using the version 1 format The IEEE 1588 Version 1 and Version 2 format are
 *  described in “PTP Processing and Control” on page 155
 */
#define EMAC_TSVER2ENA    (BIT(10))
#define EMAC_TSVER2ENA_M  (EMAC_TSVER2ENA_V << EMAC_TSVER2ENA_S)
#define EMAC_TSVER2ENA_V  0x00000001U
#define EMAC_TSVER2ENA_S  10
/** EMAC_TSIPENA : R/W; bitpos: [11]; default: 0;
 *  Enable Processing of PTP over Ethernet Frames When set, the MAC receiver processes
 *  the PTP packets encapsulated directly in the Ethernet frames When this bit is
 *  clear, the MAC ignores the PTP over Ethernet packets
 */
#define EMAC_TSIPENA    (BIT(11))
#define EMAC_TSIPENA_M  (EMAC_TSIPENA_V << EMAC_TSIPENA_S)
#define EMAC_TSIPENA_V  0x00000001U
#define EMAC_TSIPENA_S  11
/** EMAC_TSIPV6ENA : R/W; bitpos: [12]; default: 0;
 *  Enable Processing of PTP Frames Sent over IPv6UDP When set, the MAC receiver
 *  processes PTP packets encapsulated in UDP over IPv6 packets When this bit is clear,
 *  the MAC ignores the PTP transported over UDPIPv6 packets
 */
#define EMAC_TSIPV6ENA    (BIT(12))
#define EMAC_TSIPV6ENA_M  (EMAC_TSIPV6ENA_V << EMAC_TSIPV6ENA_S)
#define EMAC_TSIPV6ENA_V  0x00000001U
#define EMAC_TSIPV6ENA_S  12
/** EMAC_TSIPV4ENA : R/W; bitpos: [13]; default: 1;
 *  Enable Processing of PTP Frames Sent over IPv4UDP When set, the MAC receiver
 *  processes the PTP packets encapsulated in UDP over IPv4 packets When this bit is
 *  clear, the MAC ignores the PTP transported over UDPIPv4 packets This bit is set by
 *  default
 */
#define EMAC_TSIPV4ENA    (BIT(13))
#define EMAC_TSIPV4ENA_M  (EMAC_TSIPV4ENA_V << EMAC_TSIPV4ENA_S)
#define EMAC_TSIPV4ENA_V  0x00000001U
#define EMAC_TSIPV4ENA_S  13
/** EMAC_TSEVNTENA : R/W; bitpos: [14]; default: 0;
 *  Enable Timestamp Snapshot for Event Messages When set, the timestamp snapshot is
 *  taken only for event messages _SYNC, Delay_Req, Pdelay_Req, or Pdelay_Resp_ When
 *  reset, the snapshot is taken for all messages except Announce, Management, and
 *  Signaling For more information about the timestamp snapshots, see Table 670 on page
 *  462
 */
#define EMAC_TSEVNTENA    (BIT(14))
#define EMAC_TSEVNTENA_M  (EMAC_TSEVNTENA_V << EMAC_TSEVNTENA_S)
#define EMAC_TSEVNTENA_V  0x00000001U
#define EMAC_TSEVNTENA_S  14
/** EMAC_TSMSTRENA : R/W; bitpos: [15]; default: 0;
 *  Enable Snapshot for Messages Relevant to Master When set, the snapshot is taken
 *  only for the messages relevant to the master node Otherwise, the snapshot is taken
 *  for the messages relevant to the slave node
 */
#define EMAC_TSMSTRENA    (BIT(15))
#define EMAC_TSMSTRENA_M  (EMAC_TSMSTRENA_V << EMAC_TSMSTRENA_S)
#define EMAC_TSMSTRENA_V  0x00000001U
#define EMAC_TSMSTRENA_S  15
/** EMAC_SNAPTYPSEL : R/W; bitpos: [17:16]; default: 0;
 *  Select PTP packets for Taking Snapshots These bits along with Bits 15 and 14 decide
 *  the set of PTP packet types for which snapshot needs to be taken The encoding is
 *  given in Table 670 on page 462
 */
#define EMAC_SNAPTYPSEL    0x00000003U
#define EMAC_SNAPTYPSEL_M  (EMAC_SNAPTYPSEL_V << EMAC_SNAPTYPSEL_S)
#define EMAC_SNAPTYPSEL_V  0x00000003U
#define EMAC_SNAPTYPSEL_S  16
/** EMAC_TSENMACADDR : R/W; bitpos: [18]; default: 0;
 *  Enable MAC address for PTP Frame Filtering When set, the DA MAC address _that
 *  matches any MAC Address register_ is used to filter the PTP frames when PTP is
 *  directly sent over Ethernet
 */
#define EMAC_TSENMACADDR    (BIT(18))
#define EMAC_TSENMACADDR_M  (EMAC_TSENMACADDR_V << EMAC_TSENMACADDR_S)
#define EMAC_TSENMACADDR_V  0x00000001U
#define EMAC_TSENMACADDR_S  18
/** EMAC_ATSFC : R/W1S; bitpos: [24]; default: 0;
 *  Auxiliary Snapshot FIFO Clear When set, it resets the pointers of the Auxiliary
 *  Snapshot FIFO This bit is cleared when the pointers are reset and the FIFO is empty
 *  When this bit is high, auxiliary snapshots get stored in the FIFO This bit is
 *  reserved when the Add IEEE 1588 Auxiliary Snapshot option is not selected during
 *  core configuration
 */
#define EMAC_ATSFC    (BIT(24))
#define EMAC_ATSFC_M  (EMAC_ATSFC_V << EMAC_ATSFC_S)
#define EMAC_ATSFC_V  0x00000001U
#define EMAC_ATSFC_S  24
/** EMAC_ATSEN0 : R/W; bitpos: [25]; default: 0;
 *  Auxiliary Snapshot 0 Enable This field controls capturing the Auxiliary Snapshot
 *  Trigger 0 When this bit is set, the Auxiliary snapshot of event on
 *  ptp_aux_trig_i[0] input is enabled When this bit is reset, the events on this input
 *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
 *  is not selected during core configuration
 */
#define EMAC_ATSEN0    (BIT(25))
#define EMAC_ATSEN0_M  (EMAC_ATSEN0_V << EMAC_ATSEN0_S)
#define EMAC_ATSEN0_V  0x00000001U
#define EMAC_ATSEN0_S  25
/** EMAC_ATSEN1 : R/W; bitpos: [26]; default: 0;
 *  Auxiliary Snapshot 1 Enable This field controls capturing the Auxiliary Snapshot
 *  Trigger 1 When this bit is set, the Auxiliary snapshot of event on
 *  ptp_aux_trig_i[1] input is enabled When this bit is reset, the events on this input
 *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
 *  is not selected during core configuration or the selected number in the Number of
 *  IEEE 1588 Auxiliary Snapshot Inputs option is less than two
 */
#define EMAC_ATSEN1    (BIT(26))
#define EMAC_ATSEN1_M  (EMAC_ATSEN1_V << EMAC_ATSEN1_S)
#define EMAC_ATSEN1_V  0x00000001U
#define EMAC_ATSEN1_S  26
/** EMAC_ATSEN2 : R/W; bitpos: [27]; default: 0;
 *  Auxiliary Snapshot 2 Enable This field controls capturing the Auxiliary Snapshot
 *  Trigger 2 When this bit is set, the Auxiliary snapshot of event on
 *  ptp_aux_trig_i[2] input is enabled When this bit is reset, the events on this input
 *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
 *  is not selected during core configuration or the selected number in the Number of
 *  IEEE 1588 Auxiliary Snapshot Inputs option is less than three
 */
#define EMAC_ATSEN2    (BIT(27))
#define EMAC_ATSEN2_M  (EMAC_ATSEN2_V << EMAC_ATSEN2_S)
#define EMAC_ATSEN2_V  0x00000001U
#define EMAC_ATSEN2_S  27
/** EMAC_ATSEN3 : R/W; bitpos: [28]; default: 0;
 *  Auxiliary Snapshot 3 Enable This field controls capturing the Auxiliary Snapshot
 *  Trigger 3 When this bit is set, the Auxiliary snapshot of event on
 *  ptp_aux_trig_i[3] input is enabled When this bit is reset, the events on this input
 *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
 *  is not selected during core configuration or the selected number in the Number of
 *  IEEE 1588 Auxiliary Snapshot Inputs option is less than four
 */
#define EMAC_ATSEN3    (BIT(28))
#define EMAC_ATSEN3_M  (EMAC_ATSEN3_V << EMAC_ATSEN3_S)
#define EMAC_ATSEN3_V  0x00000001U
#define EMAC_ATSEN3_S  28

/** EMAC_SUBSECONDINCREMENT_REG register
 *  Contains the 8bit value by which the SubSecond register is incremented This
 *  register is present only when IEEE1588 timestamping is enabled without an external
 *  timestamp input
 */
#define EMAC_SUBSECONDINCREMENT_REG (DR_REG_EMAC_BASE + 0x704)
/** EMAC_SSINC : R/W; bitpos: [7:0]; default: 0;
 *  Subsecond Increment Value The value programmed in this field is accumulated every
 *  clock cycle _of clk_ptp_i_ with the contents of the subsecond register For example,
 *  when PTP clock is 50 MHz _period is 20 ns_, you should program 20 _0x14_ when the
 *  System Time Nanoseconds register has an accuracy of 1 ns [Bit 9 _TSCTRLSSR_ is set
 *  in Register 448 _Timestamp Control Register_] When TSCTRLSSR is clear, the
 *  Nanoseconds register has a resolution of ~0465ns In this case, you should program a
 *  value of 43 _0x2B_ that is derived by 20ns/0465
 */
#define EMAC_SSINC    0x000000FFU
#define EMAC_SSINC_M  (EMAC_SSINC_V << EMAC_SSINC_S)
#define EMAC_SSINC_V  0x000000FFU
#define EMAC_SSINC_S  0

/** EMAC_SYSTEMTIMESECONDS_REG register
 *  Contains the lower 32 bits of the seconds field of the system time This register is
 *  present only when IEEE1588 timestamping is enabled without an external timestamp
 *  input
 */
#define EMAC_SYSTEMTIMESECONDS_REG (DR_REG_EMAC_BASE + 0x708)
/** EMAC_TSS_RO : RO; bitpos: [31:0]; default: 0;
 *  Timestamp Second The value in this field indicates the current value in seconds of
 *  the System Time maintained by the MAC
 */
#define EMAC_TSS_RO    0xFFFFFFFFU
#define EMAC_TSS_RO_M  (EMAC_TSS_RO_V << EMAC_TSS_RO_S)
#define EMAC_TSS_RO_V  0xFFFFFFFFU
#define EMAC_TSS_RO_S  0

/** EMAC_SYSTEMTIMENANOSECONDS_REG register
 *  Contains 32 bits of the nanoseconds field of the system time This register is only
 *  present when IEEE1588 timestamping is enabled without an external timestamp input
 */
#define EMAC_SYSTEMTIMENANOSECONDS_REG (DR_REG_EMAC_BASE + 0x70c)
/** EMAC_TSSS_RO : RO; bitpos: [30:0]; default: 0;
 *  Timestamp Sub Seconds The value in this field has the sub second representation of
 *  time, with an accuracy of 046 ns When Bit 9 _TSCTRLSSR_ is set in Register 448
 *  _Timestamp Control Register_, each bit represents 1 ns and the maximum value is
 *  0x3B9A_C9FF, after which it rollsover to zero
 */
#define EMAC_TSSS_RO    0x7FFFFFFFU
#define EMAC_TSSS_RO_M  (EMAC_TSSS_RO_V << EMAC_TSSS_RO_S)
#define EMAC_TSSS_RO_V  0x7FFFFFFFU
#define EMAC_TSSS_RO_S  0

/** EMAC_SYSTEMTIMESECONDSUPDATE_REG register
 *  Contains the lower 32 bits of the seconds field to be written to, added to, or
 *  subtracted from the System Time value This register is only present when IEEE1588
 *  timestamping is enabled without an external timestamp input
 */
#define EMAC_SYSTEMTIMESECONDSUPDATE_REG (DR_REG_EMAC_BASE + 0x710)
/** EMAC_TSS : R/W; bitpos: [31:0]; default: 0;
 *  TIMESTAMP SECOND THE VALUE IN THIS FIELD INDICATES THE TIME IN SECONDS TO BE
 *  INITIALIZED OR ADDED TO THE SYSTEM TIME
 */
#define EMAC_TSS    0xFFFFFFFFU
#define EMAC_TSS_M  (EMAC_TSS_V << EMAC_TSS_S)
#define EMAC_TSS_V  0xFFFFFFFFU
#define EMAC_TSS_S  0

/** EMAC_SYSTEMTIMENANOSECONDSUPDATE_REG register
 *  Contains 32 bits of the nanoseconds field to be written to, added to, or subtracted
 *  from the System Time value This register is only present when IEEE1588 timestamping
 *  is enabled without an external timestamp input
 */
#define EMAC_SYSTEMTIMENANOSECONDSUPDATE_REG (DR_REG_EMAC_BASE + 0x714)
/** EMAC_TSSS : R/W; bitpos: [30:0]; default: 0;
 *  Timestamp Sub Seconds The value in this field has the sub second representation of
 *  time, with an accuracy of 046 ns When Bit 9 _TSCTRLSSR_ is set in Register 448
 *  _Timestamp Control Register_, each bit represents 1 ns and the programmed value
 *  should not exceed 0x3B9A_C9FF
 */
#define EMAC_TSSS    0x7FFFFFFFU
#define EMAC_TSSS_M  (EMAC_TSSS_V << EMAC_TSSS_S)
#define EMAC_TSSS_V  0x7FFFFFFFU
#define EMAC_TSSS_S  0
/** EMAC_ADDSUB : R/W; bitpos: [31]; default: 0;
 *  Add or Subtract Time When this bit is set, the time value is subtracted with the
 *  contents of the update register When this bit is reset, the time value is added
 *  with the contents of the update register
 */
#define EMAC_ADDSUB    (BIT(31))
#define EMAC_ADDSUB_M  (EMAC_ADDSUB_V << EMAC_ADDSUB_S)
#define EMAC_ADDSUB_V  0x00000001U
#define EMAC_ADDSUB_S  31

/** EMAC_TIMESTAMPADDEND_REG register
 *  This register is used by the software to readjust the clock frequency linearly to
 *  match the master clock frequency This register is only present when IEEE1588
 *  timestamping is enabled without an external timestamp input
 */
#define EMAC_TIMESTAMPADDEND_REG (DR_REG_EMAC_BASE + 0x718)
/** EMAC_TSAR : R/W; bitpos: [31:0]; default: 0;
 *  Timestamp Addend Register This field indicates the 32bit time value to be added to
 *  the Accumulator register to achieve time synchronization
 */
#define EMAC_TSAR    0xFFFFFFFFU
#define EMAC_TSAR_M  (EMAC_TSAR_V << EMAC_TSAR_S)
#define EMAC_TSAR_V  0xFFFFFFFFU
#define EMAC_TSAR_S  0

/** EMAC_TARGETTIMESECONDS_REG register
 *  Contains the higher 32 bits of time to be compared with the system time for
 *  interrupt event generation or to start the PPS signal output generation This
 *  register is present only when IEEE1588 timestamping is enabled without an external
 *  timestamp input
 */
#define EMAC_TARGETTIMESECONDS_REG (DR_REG_EMAC_BASE + 0x71c)
/** EMAC_TSTR : R/W; bitpos: [31:0]; default: 0;
 *  Target Time Seconds Register This register stores the time in seconds When the
 *  timestamp value matches or exceeds both Target Timestamp registers, then based on
 *  Bits [6:5] of Register 459 _PPS Control Register_, the MAC starts or stops the PPS
 *  signal output and generates an interrupt _if enabled_
 */
#define EMAC_TSTR    0xFFFFFFFFU
#define EMAC_TSTR_M  (EMAC_TSTR_V << EMAC_TSTR_S)
#define EMAC_TSTR_V  0xFFFFFFFFU
#define EMAC_TSTR_S  0

/** EMAC_TARGETTIMENANOSECONDS_REG register
 *  Contains the lower 32 bits of time to be compared with the system time for
 *  interrupt event generation or to start the PPS signal output generation This
 *  register is present only when IEEE1588 timestamping is enabled without an external
 *  timestamp input
 */
#define EMAC_TARGETTIMENANOSECONDS_REG (DR_REG_EMAC_BASE + 0x720)
/** EMAC_TTSLO : R/W; bitpos: [30:0]; default: 0;
 *  Target Timestamp Low Register This register stores the time in _signed_ nanoseconds
 *  When the value of the timestamp matches the both Target Timestamp registers, then
 *  based on the TRGTMODSEL0 field _Bits [6:5]_ in Register 459 _PPS Control Register_,
 *  the MAC starts or stops the PPS signal output and generates an interrupt _if
 *  enabled_ This value should not exceed 0x3B9A_C9FF when Bit 9 _TSCTRLSSR_ is set in
 *  Register 448 _Timestamp Control Register_ The actual start or stop time of the PPS
 *  signal output may have an error margin up to one unit of subsecond increment value
 */
#define EMAC_TTSLO    0x7FFFFFFFU
#define EMAC_TTSLO_M  (EMAC_TTSLO_V << EMAC_TTSLO_S)
#define EMAC_TTSLO_V  0x7FFFFFFFU
#define EMAC_TTSLO_S  0
/** EMAC_TRGTBUSY : R/W1S; bitpos: [31]; default: 0;
 *  Target Time Register Busy The MAC sets this bit when the PPSCMD field _Bit [3:0]_
 *  in Register 459 _PPS Control Register_ is programmed to 010 or 011 Programming the
 *  PPSCMD field to 010 or 011, instructs the MAC to synchronize the Target Time
 *  Registers to the PTP clock domain The MAC clears this bit after synchronizing the
 *  Target Time Registers to the PTP clock domain The application must not update the
 *  Target Time Registers when this bit is read as 1 Otherwise, the synchronization of
 *  the previous programmed time gets corrupted This bit is reserved when the Enable
 *  Flexible PulsePerSecond Output feature is not selected
 */
#define EMAC_TRGTBUSY    (BIT(31))
#define EMAC_TRGTBUSY_M  (EMAC_TRGTBUSY_V << EMAC_TRGTBUSY_S)
#define EMAC_TRGTBUSY_V  0x00000001U
#define EMAC_TRGTBUSY_S  31

/** EMAC_SYSTEMTIMEHIGHERWORDSECONDS_REG register
 *  Contains the most significant 16bits of the timestamp seconds value This register
 *  is optional and can be selected using the parameter mentioned in “IEEE 1588
 *  Timestamp Block” on page 492
 */
#define EMAC_SYSTEMTIMEHIGHERWORDSECONDS_REG (DR_REG_EMAC_BASE + 0x724)
/** EMAC_TSHWR : R/W; bitpos: [15:0]; default: 0;
 *  Timestamp Higher Word Register This field contains the most significant 16bits of
 *  the timestamp seconds value This register is optional and can be selected using the
 *  Enable IEEE 1588 Higher Word Register option during core configuration The register
 *  is directly written to initialize the value This register is incremented when there
 *  is an overflow from the 32bits of the System Time  Seconds register
 */
#define EMAC_TSHWR    0x0000FFFFU
#define EMAC_TSHWR_M  (EMAC_TSHWR_V << EMAC_TSHWR_S)
#define EMAC_TSHWR_V  0x0000FFFFU
#define EMAC_TSHWR_S  0

/** EMAC_TIMESTAMPSTATUS_REG register
 *  Contains the PTP status This register is available only when the advanced IEEE 1588
 *  timestamp feature is selected
 */
#define EMAC_TIMESTAMPSTATUS_REG (DR_REG_EMAC_BASE + 0x728)
/** EMAC_TSSOVF : R/W; bitpos: [0]; default: 0;
 *  Timestamp Seconds Overflow When set, this bit indicates that the seconds value of
 *  the timestamp _when supporting version 2 format_ has overflowed beyond 32’hFFFF_FFFF
 */
#define EMAC_TSSOVF    (BIT(0))
#define EMAC_TSSOVF_M  (EMAC_TSSOVF_V << EMAC_TSSOVF_S)
#define EMAC_TSSOVF_V  0x00000001U
#define EMAC_TSSOVF_S  0
/** EMAC_TSTARGT : R/W; bitpos: [1]; default: 0;
 *  Timestamp Target Time Reached When set, this bit indicates that the value of system
 *  time is greater than or equal to the value specified in the Register 455 _Target
 *  Time Seconds Register_ and Register 456 _Target Time Nanoseconds Register_
 */
#define EMAC_TSTARGT    (BIT(1))
#define EMAC_TSTARGT_M  (EMAC_TSTARGT_V << EMAC_TSTARGT_S)
#define EMAC_TSTARGT_V  0x00000001U
#define EMAC_TSTARGT_S  1
/** EMAC_AUXTSTRIG : R/W; bitpos: [2]; default: 0;
 *  Auxiliary Timestamp Trigger Snapshot This bit is set high when the auxiliary
 *  snapshot is written to the FIFO This bit is valid only if the Enable IEEE 1588
 *  Auxiliary Snapshot feature is selected
 */
#define EMAC_AUXTSTRIG    (BIT(2))
#define EMAC_AUXTSTRIG_M  (EMAC_AUXTSTRIG_V << EMAC_AUXTSTRIG_S)
#define EMAC_AUXTSTRIG_V  0x00000001U
#define EMAC_AUXTSTRIG_S  2
/** EMAC_TSTRGTERR : R/W; bitpos: [3]; default: 0;
 *  Timestamp Target Time Error This bit is set when the target time, being programmed
 *  in Register 455 and Register 456, is already elapsed This bit is cleared when read
 *  by the application
 */
#define EMAC_TSTRGTERR    (BIT(3))
#define EMAC_TSTRGTERR_M  (EMAC_TSTRGTERR_V << EMAC_TSTRGTERR_S)
#define EMAC_TSTRGTERR_V  0x00000001U
#define EMAC_TSTRGTERR_S  3
/** EMAC_TSTARGT1 : R/W; bitpos: [4]; default: 0;
 *  Timestamp Target Time Reached for Target Time PPS1 When set, this bit indicates
 *  that the value of system time is greater than or equal to the value specified in
 *  Register 480 _PPS1 Target Time High Register_ and Register 481 _PPS1 Target Time
 *  Low Register_
 */
#define EMAC_TSTARGT1    (BIT(4))
#define EMAC_TSTARGT1_M  (EMAC_TSTARGT1_V << EMAC_TSTARGT1_S)
#define EMAC_TSTARGT1_V  0x00000001U
#define EMAC_TSTARGT1_S  4
/** EMAC_TSTRGTERR1 : R/W; bitpos: [5]; default: 0;
 *  Timestamp Target Time Error This bit is set when the target time, being programmed
 *  in Register 480 and Register 481, is already elapsed This bit is cleared when read
 *  by the application
 */
#define EMAC_TSTRGTERR1    (BIT(5))
#define EMAC_TSTRGTERR1_M  (EMAC_TSTRGTERR1_V << EMAC_TSTRGTERR1_S)
#define EMAC_TSTRGTERR1_V  0x00000001U
#define EMAC_TSTRGTERR1_S  5
/** EMAC_TSTARGT2 : R/W; bitpos: [6]; default: 0;
 *  Timestamp Target Time Reached for Target Time PPS2 When set, this bit indicates
 *  that the value of system time is greater than or equal to the value specified in
 *  Register 488 _PPS2 Target Time High Register_ and Register 489 _PPS2 Target Time
 *  Low Register_
 */
#define EMAC_TSTARGT2    (BIT(6))
#define EMAC_TSTARGT2_M  (EMAC_TSTARGT2_V << EMAC_TSTARGT2_S)
#define EMAC_TSTARGT2_V  0x00000001U
#define EMAC_TSTARGT2_S  6
/** EMAC_TSTRGTERR2 : R/W; bitpos: [7]; default: 0;
 *  Timestamp Target Time Error This bit is set when the target time, being programmed
 *  in Register 488 and Register 489, is already elapsed This bit is cleared when read
 *  by the application
 */
#define EMAC_TSTRGTERR2    (BIT(7))
#define EMAC_TSTRGTERR2_M  (EMAC_TSTRGTERR2_V << EMAC_TSTRGTERR2_S)
#define EMAC_TSTRGTERR2_V  0x00000001U
#define EMAC_TSTRGTERR2_S  7
/** EMAC_TSTARGT3 : R/W; bitpos: [8]; default: 0;
 *  Timestamp Target Time Reached for Target Time PPS3 When set, this bit indicates
 *  that the value of system time is greater than or equal to the value specified in
 *  Register 496 _PPS3 Target Time High Register_ and Register 497 _PPS3 Target Time
 *  Low Register_
 */
#define EMAC_TSTARGT3    (BIT(8))
#define EMAC_TSTARGT3_M  (EMAC_TSTARGT3_V << EMAC_TSTARGT3_S)
#define EMAC_TSTARGT3_V  0x00000001U
#define EMAC_TSTARGT3_S  8
/** EMAC_TSTRGTERR3 : R/W; bitpos: [9]; default: 0;
 *  Timestamp Target Time Error This bit is set when the target time, being programmed
 *  in Register 496 and Register 497, is already elapsed This bit is cleared when read
 *  by the application
 */
#define EMAC_TSTRGTERR3    (BIT(9))
#define EMAC_TSTRGTERR3_M  (EMAC_TSTRGTERR3_V << EMAC_TSTRGTERR3_S)
#define EMAC_TSTRGTERR3_V  0x00000001U
#define EMAC_TSTRGTERR3_S  9
/** EMAC_ATSSTN : R/W; bitpos: [19:16]; default: 0;
 *  Auxiliary Timestamp Snapshot Trigger Identifier These bits identify the Auxiliary
 *  trigger inputs for which the timestamp available in the Auxiliary Snapshot Register
 *  is applicable When more than one bit is set at the same time, it means that
 *  corresponding auxiliary triggers were sampled at the same clock These bits are
 *  applicable only if the number of Auxiliary snapshots is more than one One bit is
 *  assigned for each trigger as shown in the following list: Bit 16: Auxiliary trigger
 *  0 Bit 17: Auxiliary trigger 1 Bit 18: Auxiliary trigger 2 Bit 19: Auxiliary trigger
 *  3 The software can read this register to find the triggers that are set when the
 *  timestamp is taken
 */
#define EMAC_ATSSTN    0x0000000FU
#define EMAC_ATSSTN_M  (EMAC_ATSSTN_V << EMAC_ATSSTN_S)
#define EMAC_ATSSTN_V  0x0000000FU
#define EMAC_ATSSTN_S  16
/** EMAC_ATSSTM : RO; bitpos: [24]; default: 0;
 *  Auxiliary Timestamp Snapshot Trigger Missed
 */
#define EMAC_ATSSTM    (BIT(24))
#define EMAC_ATSSTM_M  (EMAC_ATSSTM_V << EMAC_ATSSTM_S)
#define EMAC_ATSSTM_V  0x00000001U
#define EMAC_ATSSTM_S  24
/** EMAC_ATSNS : RO; bitpos: [29:25]; default: 0;
 *  Number of Auxiliary Timestamp Snapshots This field indicates the number of
 *  Snapshots available in the FIFO A value equal to the selected depth of FIFO _4, 8,
 *  or 16_ indicates that the Auxiliary Snapshot FIFO is full These bits are cleared
 *  _to 00000_ when the Auxiliary snapshot FIFO clear bit is set This bit is valid only
 *  if the Add IEEE 1588 Auxiliary Snapshot option is selected during core configuration
 */
#define EMAC_ATSNS    0x0000001FU
#define EMAC_ATSNS_M  (EMAC_ATSNS_V << EMAC_ATSNS_S)
#define EMAC_ATSNS_V  0x0000001FU
#define EMAC_ATSNS_S  25

/** EMAC_PPSCONTROL_REG register
 *  This register is used to control the interval of the PPS signal output This
 *  register is available only when the advanced IEEE 1588 timestamp feature is selected
 */
#define EMAC_PPSCONTROL_REG (DR_REG_EMAC_BASE + 0x72c)
/** EMAC_PPSCTRL0 : R/W1S; bitpos: [3:0]; default: 0;
 *  PPSCTRL0: PPS0 Output Frequency Control This field controls the frequency of the
 *  PPS0 output _ptp_pps_o[0]_ signal The default value of PPSCTRL is 0000, and the PPS
 *  output is 1 pulse _of width clk_ptp_i_ every second For other values of PPSCTRL,
 *  the PPS output becomes a generated clock of following frequencies: 0001: The binary
 *  rollover is 2 Hz, and the digital rollover is 1 Hz 0010: The binary rollover is 4
 *  Hz, and the digital rollover is 2 Hz 0011: The binary rollover is 8 Hz, and the
 *  digital rollover is 4 Hz 0100: The binary rollover is 16 Hz, and the digital
 *  rollover is 8 Hz  1111: The binary rollover is 32768 KHz, and the digital rollover
 *  is 16384 KHz Note: In the binary rollover mode, the PPS output _ptp_pps_o_ has a
 *  duty cycle of 50 percent with these frequencies In the digital rollover mode, the
 *  PPS output frequency is an average number The actual clock is of different
 *  frequency that gets synchronized every second For example: When PPSCTRL = 0001, the
 *  PPS _1 Hz_ has a low period of 537 ms and a high period of 463 ms When PPSCTRL =
 *  0010, the PPS _2 Hz_ is a sequence of: 	One clock of 50 percent duty cycle and 537
 *  ms period 	Second clock of 463 ms period _268 ms low and 195 ms high_ When PPSCTRL
 *  = 0011, the PPS _4 Hz_ is a sequence of: 	Three clocks of 50 percent duty cycle and
 *  268 ms period 	Fourth clock of 195 ms period _134 ms low and 61 ms high_ This
 *  behavior is because of the nonlinear toggling of bits in the digital rollover mode
 *  in Register 451 _System Time  Nanoseconds Register_ / PPSCMD0: Flexible PPS0 Output
 *  _ptp_pps_o[0]_ Control Programming these bits with a nonzero value instructs the
 *  MAC to initiate an event When the command is transferred or synchronized to the PTP
 *  clock domain, these bits get cleared automatically The Software should ensure that
 *  these bits are programmed only when they are “allzero” The following list describes
 *  the values of PPSCMD0: 0000: No Command 0001: START Single Pulse This command
 *  generates single pulse rising at the start point defined in Target Time Registers
 *  _register 455 and 456_ and of a duration defined in the PPS0 Width Register 0010:
 *  START Pulse Train This command generates the train of pulses rising at the start
 *  point defined in the Target Time Registers and of a duration defined in the PPS0
 *  Width Register and repeated at interval defined in the PPS Interval Register By
 *  default, the PPS pulse train is freerunning unless stopped by 'STOP Pulse train at
 *  time' or 'STOP Pulse Train immediately' commands 0011: Cancel START This command
 *  cancels the START Single Pulse and START Pulse Train commands if the system time
 *  has not crossed the programmed start time 0100: STOP Pulse train at time This
 *  command stops the train of pulses initiated by the START Pulse Train command
 *  _PPSCMD = 0010_ after the time programmed in the Target Time registers elapses
 *  0101: STOP Pulse Train immediately This command immediately stops the train of
 *  pulses initiated by the START Pulse Train command _PPSCMD = 0010_ 0110: Cancel STOP
 *  Pulse train This command cancels the STOP pulse train at time command if the
 *  programmed stop time has not elapsed The PPS pulse train becomes freerunning on the
 *  successful execution of this command 01111111: Reserved
 */
#define EMAC_PPSCTRL0    0x0000000FU
#define EMAC_PPSCTRL0_M  (EMAC_PPSCTRL0_V << EMAC_PPSCTRL0_S)
#define EMAC_PPSCTRL0_V  0x0000000FU
#define EMAC_PPSCTRL0_S  0
/** EMAC_PPSEN0 : R/W; bitpos: [4]; default: 0;
 *  Flexible PPS Output Mode Enable When set low, Bits [3:0] function as PPSCTRL
 *  _backward compatible_ When set high, Bits[3:0] function as PPSCMD
 */
#define EMAC_PPSEN0    (BIT(4))
#define EMAC_PPSEN0_M  (EMAC_PPSEN0_V << EMAC_PPSEN0_S)
#define EMAC_PPSEN0_V  0x00000001U
#define EMAC_PPSEN0_S  4
/** EMAC_TRGTMODSEL0 : R/W; bitpos: [6:5]; default: 0;
 *  Target Time Register Mode for PPS0 Output This field indicates the Target Time
 *  registers _register 455 and 456_ mode for PPS0 output signal: 00: Indicates that
 *  the Target Time registers are programmed only for generating the interrupt event
 *  01: Reserved 10: Indicates that the Target Time registers are programmed for
 *  generating the interrupt event and starting or stopping the generation of the PPS0
 *  output signal 11: Indicates that the Target Time registers are programmed only for
 *  starting or stopping the generation of the PPS0 output signal No interrupt is
 *  asserted
 */
#define EMAC_TRGTMODSEL0    0x00000003U
#define EMAC_TRGTMODSEL0_M  (EMAC_TRGTMODSEL0_V << EMAC_TRGTMODSEL0_S)
#define EMAC_TRGTMODSEL0_V  0x00000003U
#define EMAC_TRGTMODSEL0_S  5
/** EMAC_PPSCMD1 : R/W1S; bitpos: [10:8]; default: 0;
 *  Flexible PPS1 Output Control This field controls the flexible PPS1 output
 *  _ptp_pps_o[1]_ signal This field is similar to PPSCMD0[2:0] in functionality
 */
#define EMAC_PPSCMD1    0x00000007U
#define EMAC_PPSCMD1_M  (EMAC_PPSCMD1_V << EMAC_PPSCMD1_S)
#define EMAC_PPSCMD1_V  0x00000007U
#define EMAC_PPSCMD1_S  8
/** EMAC_TRGTMODSEL1 : R/W; bitpos: [14:13]; default: 0;
 *  Target Time Register Mode for PPS1 Output This field indicates the Target Time
 *  registers _register 480 and 481_ mode for PPS1 output signal This field is similar
 *  to the TRGTMODSEL0 field
 */
#define EMAC_TRGTMODSEL1    0x00000003U
#define EMAC_TRGTMODSEL1_M  (EMAC_TRGTMODSEL1_V << EMAC_TRGTMODSEL1_S)
#define EMAC_TRGTMODSEL1_V  0x00000003U
#define EMAC_TRGTMODSEL1_S  13
/** EMAC_PPSCMD2 : R/W1S; bitpos: [18:16]; default: 0;
 *  Flexible PPS2 Output Control This field controls the flexible PPS2 output
 *  _ptp_pps_o[2]_ signal This field is similar to PPSCMD0[2:0] in functionality
 */
#define EMAC_PPSCMD2    0x00000007U
#define EMAC_PPSCMD2_M  (EMAC_PPSCMD2_V << EMAC_PPSCMD2_S)
#define EMAC_PPSCMD2_V  0x00000007U
#define EMAC_PPSCMD2_S  16
/** EMAC_TRGTMODSEL2 : R/W; bitpos: [22:21]; default: 0;
 *  Target Time Register Mode for PPS2 Output This field indicates the Target Time
 *  registers _register 488 and 489_ mode for PPS2 output signal This field is similar
 *  to the TRGTMODSEL0 field
 */
#define EMAC_TRGTMODSEL2    0x00000003U
#define EMAC_TRGTMODSEL2_M  (EMAC_TRGTMODSEL2_V << EMAC_TRGTMODSEL2_S)
#define EMAC_TRGTMODSEL2_V  0x00000003U
#define EMAC_TRGTMODSEL2_S  21
/** EMAC_PPSCMD3 : R/W1S; bitpos: [26:24]; default: 0;
 *  Flexible PPS3 Output Control This field controls the flexible PPS3 output
 *  _ptp_pps_o[3]_ signal This field is similar to PPSCMD0[2:0] in functionality
 */
#define EMAC_PPSCMD3    0x00000007U
#define EMAC_PPSCMD3_M  (EMAC_PPSCMD3_V << EMAC_PPSCMD3_S)
#define EMAC_PPSCMD3_V  0x00000007U
#define EMAC_PPSCMD3_S  24
/** EMAC_TRGTMODSEL3 : R/W; bitpos: [30:29]; default: 0;
 *  Target Time Register Mode for PPS3 Output This field indicates the Target Time
 *  registers _register 496 and 497_ mode for PPS3 output signal This field is similar
 *  to the TRGTMODSEL0 field
 */
#define EMAC_TRGTMODSEL3    0x00000003U
#define EMAC_TRGTMODSEL3_M  (EMAC_TRGTMODSEL3_V << EMAC_TRGTMODSEL3_S)
#define EMAC_TRGTMODSEL3_V  0x00000003U
#define EMAC_TRGTMODSEL3_S  29

/** EMAC_AUXILIARYTIMESTAMPNANOSECONDS_REG register
 *  Contains the lower 32 bits _nanoseconds field_ of the auxiliary timestamp register
 */
#define EMAC_AUXILIARYTIMESTAMPNANOSECONDS_REG (DR_REG_EMAC_BASE + 0x730)
/** EMAC_AUXTSLO : RO; bitpos: [30:0]; default: 0;
 *  Contains the lower 31 bits _nanoseconds field_ of the auxiliary timestamp
 */
#define EMAC_AUXTSLO    0x7FFFFFFFU
#define EMAC_AUXTSLO_M  (EMAC_AUXTSLO_V << EMAC_AUXTSLO_S)
#define EMAC_AUXTSLO_V  0x7FFFFFFFU
#define EMAC_AUXTSLO_S  0

/** EMAC_AUXILIARYTIMESTAMPSECONDS_REG register
 *  Contains the lower 32 bits of the Seconds field of the auxiliary timestamp register
 */
#define EMAC_AUXILIARYTIMESTAMPSECONDS_REG (DR_REG_EMAC_BASE + 0x734)
/** EMAC_AUXTSHI : RO; bitpos: [31:0]; default: 0;
 *  Contains the lower 32 bits of the Seconds field of the auxiliary timestamp
 */
#define EMAC_AUXTSHI    0xFFFFFFFFU
#define EMAC_AUXTSHI_M  (EMAC_AUXTSHI_V << EMAC_AUXTSHI_S)
#define EMAC_AUXTSHI_V  0xFFFFFFFFU
#define EMAC_AUXTSHI_S  0

/** EMAC_AVMACCONTROL_REG register
 *  Controls the AV traffic and queue management in the MAC Receiver This register is
 *  present only when you select the AV feature in coreConsultant
 */
#define EMAC_AVMACCONTROL_REG (DR_REG_EMAC_BASE + 0x738)
/** EMAC_AVT : R/W; bitpos: [15:0]; default: 0;
 *  AV EtherType Value This field contains the value that is compared with the
 *  EtherType field of the incoming _tagged or untagged_ Ethernet frame to detect an AV
 *  packet
 */
#define EMAC_AVT    0x0000FFFFU
#define EMAC_AVT_M  (EMAC_AVT_V << EMAC_AVT_S)
#define EMAC_AVT_V  0x0000FFFFU
#define EMAC_AVT_S  0
/** EMAC_AVP : R/W; bitpos: [18:16]; default: 0;
 *  AV Priority for Queuing The value programmed in these bits control the receive
 *  channel _0, 1, or 2_ to which an AV packet with a given priority must be queued If
 *  only Channel 1 receive path is enabled, the AV packets with priority value greater
 *  than or equal to the programmed value are queued on Channel 1 and all other packets
 *  are queued on Channel 0 If Channel 2 receive path is also enabled, the AV packets
 *  with priority value greater than or equal to the programmed value are queued on
 *  Channel 2 The AV packets with value less than the programmed value on Channel 1 and
 *  all other packets are queued on Channel 0 These bits are applicable only if at
 *  least one additional receive channel is selected in the AV mode
 */
#define EMAC_AVP    0x00000007U
#define EMAC_AVP_M  (EMAC_AVP_V << EMAC_AVP_S)
#define EMAC_AVP_V  0x00000007U
#define EMAC_AVP_S  16
/** EMAC_VQE : R/W; bitpos: [19]; default: 0;
 *  VLAN Tagged NonAV Packets Queueing Enable When this bit is set, the MAC also queues
 *  nonAV VLAN tagged packets into the available channels according to the value of the
 *  AVP bits This bit is reserved and readonly if Channel 1 and Channel 2 Receive paths
 *  are not selected during core configuration
 */
#define EMAC_VQE    (BIT(19))
#define EMAC_VQE_M  (EMAC_VQE_V << EMAC_VQE_S)
#define EMAC_VQE_V  0x00000001U
#define EMAC_VQE_S  19
/** EMAC_AVCD : R/W; bitpos: [20]; default: 0;
 *  AV Channel Disable When this bit is set, the MAC forwards all packets to the
 *  default Channel 0 and the values programmed in the AVP, AVCH, and PTPCH fields are
 *  ignored This bit is reserved and readonly if Channel 1 or Channel 2 receive paths
 *  are not selected during core configuration
 */
#define EMAC_AVCD    (BIT(20))
#define EMAC_AVCD_M  (EMAC_AVCD_V << EMAC_AVCD_S)
#define EMAC_AVCD_V  0x00000001U
#define EMAC_AVCD_S  20
/** EMAC_AVCH : R/W; bitpos: [22:21]; default: 0;
 *  Channel for Queuing the AV Control Packets This field specifies the channel on
 *  which the received untagged AV control packets are queued 00: Channel 0 01: Channel
 *  1 10: Channel 2 11: Reserved These bits are reserved if the receive paths of
 *  Channel 1 or Channel 2 are not enabled
 */
#define EMAC_AVCH    0x00000003U
#define EMAC_AVCH_M  (EMAC_AVCH_V << EMAC_AVCH_S)
#define EMAC_AVCH_V  0x00000003U
#define EMAC_AVCH_S  21
/** EMAC_PTPCH : R/W; bitpos: [25:24]; default: 0;
 *  Channel for Queuing the PTP Packets This field specifies the channel on which the
 *  untagged PTP packets, sent over the Ethernet payload and not over IPv4 or IPv6, are
 *  queued 00: Channel 0 01: Channel 1 10: Channel 2 11: Reserved These bits are
 *  reserved if the receive paths of Channel 1 or Channel 2 are not enabled
 */
#define EMAC_PTPCH    0x00000003U
#define EMAC_PTPCH_M  (EMAC_PTPCH_V << EMAC_PTPCH_S)
#define EMAC_PTPCH_V  0x00000003U
#define EMAC_PTPCH_S  24

/** EMAC_PPS0INTERVAL_REG register
 *  Contains the number of units of subsecond increment value between the rising edges
 *  of PPS0 signal output This register is available only when the flexible PPS feature
 *  is selected
 */
#define EMAC_PPS0INTERVAL_REG (DR_REG_EMAC_BASE + 0x760)
/** EMAC_PPSINT : R/W; bitpos: [31:0]; default: 0;
 *  PPS0 Output Signal Interval These bits store the interval between the rising edges
 *  of PPS0 signal output in terms of units of subsecond increment value You need to
 *  program one value less than the required interval For example, if the PTP reference
 *  clock is 50 MHz _period of 20ns_, and desired interval between rising edges of PPS0
 *  signal output is 100ns _that is, five units of subsecond increment value_, then you
 *  should program value 4 _5  1_ in this register
 */
#define EMAC_PPSINT    0xFFFFFFFFU
#define EMAC_PPSINT_M  (EMAC_PPSINT_V << EMAC_PPSINT_S)
#define EMAC_PPSINT_V  0xFFFFFFFFU
#define EMAC_PPSINT_S  0

/** EMAC_PPS0WIDTH_REG register
 *  Contains the number of units of subsecond increment value between the rising and
 *  corresponding falling edges of PPS0 signal output This register is available only
 *  when the flexible PPS feature is selected
 */
#define EMAC_PPS0WIDTH_REG (DR_REG_EMAC_BASE + 0x764)
/** EMAC_PPSWIDTH : R/W; bitpos: [31:0]; default: 0;
 *  PPS0 Output Signal Width These bits store the width between the rising edge and
 *  corresponding falling edge of the PPS0 signal output in terms of units of subsecond
 *  increment value You need to program one value less than the required interval For
 *  example, if PTP reference clock is 50 MHz _period of 20ns_, and desired width
 *  between the rising and corresponding falling edges of PPS0 signal output is 80ns
 *  _that is, four units of subsecond increment value_, then you should program value 3
 *  _4  1_ in this register Note: The value programmed in this register must be lesser
 *  than the value programmed in Register 472 _PPS0 Interval Register_
 */
#define EMAC_PPSWIDTH    0xFFFFFFFFU
#define EMAC_PPSWIDTH_M  (EMAC_PPSWIDTH_V << EMAC_PPSWIDTH_S)
#define EMAC_PPSWIDTH_V  0xFFFFFFFFU
#define EMAC_PPSWIDTH_S  0

/** EMAC_PPS1TARGETTIMESECONDS_REG register
 *  Contains the higher 32 bits of time to be compared with the system time to generate
 *  the interrupt event or to start generating the PPS1 output signal This register is
 *  present only when IEEE1588 timestamping is enabled without an external timestamp
 *  input and at least one additional PPS output is selected
 */
#define EMAC_PPS1TARGETTIMESECONDS_REG (DR_REG_EMAC_BASE + 0x780)
/** EMAC_TSTRH1 : R/W; bitpos: [31:0]; default: 0;
 *  PPS1 Target Time Seconds Register This register stores the time in seconds When the
 *  timestamp value matches or exceeds both Target Timestamp registers, then based on
 *  Bits [14:13], TRGTMODSEL1, of Register 459 _PPS Control Register_, the MAC starts
 *  or stops the PPS signal output and generates an interrupt _if enabled_
 */
#define EMAC_TSTRH1    0xFFFFFFFFU
#define EMAC_TSTRH1_M  (EMAC_TSTRH1_V << EMAC_TSTRH1_S)
#define EMAC_TSTRH1_V  0xFFFFFFFFU
#define EMAC_TSTRH1_S  0

/** EMAC_PPS1TARGETTIMENANOSECONDS_REG register
 *  Contains the lower 32 bits of time to be compared with the system time to generate
 *  the interrupt event or to start generating the PPS1 output signal This register is
 *  present only when IEEE1588 timestamping is enabled without an external timestamp
 *  input and at least one additional PPS output is selected
 */
#define EMAC_PPS1TARGETTIMENANOSECONDS_REG (DR_REG_EMAC_BASE + 0x784)
/** EMAC_TTSL1 : R/W; bitpos: [30:0]; default: 0;
 *  Target Time Low for PPS1 Register This register stores the time in _signed_
 *  nanoseconds When the value of the timestamp matches the both Target Timestamp
 *  registers, then based on the TRGTMODSEL1 field _Bits [14:13]_ in Register 459 _PPS
 *  Control Register_, the MAC starts or stops the PPS signal output and generates an
 *  interrupt _if enabled_ This value should not exceed 0x3B9A_C9FF when Bit 9
 *  _TSCTRLSSR_ is set in Register 448 _Timestamp Control Register_ The actual start or
 *  stop time of the PPS signal output may have an error margin up to one unit of
 *  subsecond increment value
 */
#define EMAC_TTSL1    0x7FFFFFFFU
#define EMAC_TTSL1_M  (EMAC_TTSL1_V << EMAC_TTSL1_S)
#define EMAC_TTSL1_V  0x7FFFFFFFU
#define EMAC_TTSL1_S  0
/** EMAC_TRGTBUSY1 : R/W1S; bitpos: [31]; default: 0;
 *  PPS1 Target Time Register Busy The MAC sets this bit when the PPSCMD1 field _Bits
 *  [10:8]_ in Register 459 _PPS Control Register_ is programmed to 010 or 011
 *  Programming the PPSCMD1 field to 010 or 011 instructs the MAC to synchronize the
 *  Target Time Registers to the PTP clock domain The MAC clears this bit after
 *  synchronizing the Target Time Registers to the PTP clock domain The application
 *  must not update the Target Time Registers when this bit is read as 1 Otherwise, the
 *  synchronization of the previous programmed time gets corrupted
 */
#define EMAC_TRGTBUSY1    (BIT(31))
#define EMAC_TRGTBUSY1_M  (EMAC_TRGTBUSY1_V << EMAC_TRGTBUSY1_S)
#define EMAC_TRGTBUSY1_V  0x00000001U
#define EMAC_TRGTBUSY1_S  31

/** EMAC_MACADDRESS32HIGH_REG register
 *  Contains the higher 16 bits of the 33rd MAC address This register is present only
 *  when Enable Additional 32 MAC Address Registers is selected in coreConsultant _See
 *  Table 78_
 */
#define EMAC_MACADDRESS32HIGH_REG (DR_REG_EMAC_BASE + 0x880)
/** EMAC_ADDRHI : R/W; bitpos: [15:0]; default: 65535;
 *  MAC Address32 [47:32] This field contains the upper 16 bits _47:32_ of the 33rd
 *  6byte MAC address
 */
#define EMAC_ADDRHI    0x0000FFFFU
#define EMAC_ADDRHI_M  (EMAC_ADDRHI_V << EMAC_ADDRHI_S)
#define EMAC_ADDRHI_V  0x0000FFFFU
#define EMAC_ADDRHI_S  0
/** EMAC_AE : R/W; bitpos: [31]; default: 0;
 *  Address Enable When this bit is set, the Address filter module uses the 33rd MAC
 *  address for perfect filtering When reset, the address filter module ignores the
 *  address for filtering
 */
#define EMAC_AE    (BIT(31))
#define EMAC_AE_M  (EMAC_AE_V << EMAC_AE_S)
#define EMAC_AE_V  0x00000001U
#define EMAC_AE_S  31

/** EMAC_BUSMODE_REG register
 *  Controls the Host Interface Mode
 */
#define EMAC_BUSMODE_REG (DR_REG_EMAC_BASE + 0x1000)
/** EMAC_SWR : R/W1S_SC; bitpos: [0]; default: 1;
 *  Software Reset When this bit is set, the MAC DMA Controller resets the logic and
 *  all internal registers of the MAC It is cleared automatically after the reset
 *  operation is complete in all of the DWC_EMAC clock domains Before reprogramming any
 *  register of the DWC_EMAC, you should read a zero _0_ value in this bit Note: The
 *  Software reset function is driven only by this bit Bit 0 of Register 64 _Channel 1
 *  Bus Mode Register_ or Register 128 _Channel 2 Bus Mode Register_ has no impact on
 *  the Software reset function The reset operation is completed only when all resets
 *  in all active clock domains are deasserted Therefore, it is essential that all PHY
 *  inputs clocks _applicable for the selected PHY interface_ are present for the
 *  software reset completion The time to complete the software reset operation depends
 *  on the frequency of the slowest active clock
 */
#define EMAC_SWR    (BIT(0))
#define EMAC_SWR_M  (EMAC_SWR_V << EMAC_SWR_S)
#define EMAC_SWR_V  0x00000001U
#define EMAC_SWR_S  0
/** EMAC_DA : R/W; bitpos: [1]; default: 0;
 *  DMA Arbitration Scheme This bit specifies the arbitration scheme between the
 *  transmit and receive paths of Channel 0 0: Weighted roundrobin with Rx:Tx or Tx:Rx
 *  The priority between the paths is according to the priority specified in Bits
 *  [15:14] _PR_ and priority weights specified in Bit 27 _TXPR_ 1: Fixed priority The
 *  transmit path has priority over receive path when Bit 27 _TXPR_ is set Otherwise,
 *  receive path has priority over the transmit path In the EMACAXI configuration,
 *  these bits are reserved and are readonly _RO_ For more information about the
 *  priority scheme between the transmit and receive paths, see Table 412 in “DMA
 *  Arbiter Functions” on page 167
 */
#define EMAC_DA    (BIT(1))
#define EMAC_DA_M  (EMAC_DA_V << EMAC_DA_S)
#define EMAC_DA_V  0x00000001U
#define EMAC_DA_S  1
/** EMAC_DSL : R/W; bitpos: [6:2]; default: 0;
 *  Descriptor Skip Length This bit specifies the number of Word, Dword, or Lword
 *  _depending on the 32bit, 64bit, or 128bit bus_ to skip between two unchained
 *  descriptors The address skipping starts from the end of current descriptor to the
 *  start of next descriptor When the DSL value is equal to zero, the descriptor table
 *  is taken as contiguous by the DMA in Ring mode
 */
#define EMAC_DSL    0x0000001FU
#define EMAC_DSL_M  (EMAC_DSL_V << EMAC_DSL_S)
#define EMAC_DSL_V  0x0000001FU
#define EMAC_DSL_S  2
/** EMAC_ATDS : R/W; bitpos: [7]; default: 0;
 *  Alternate Descriptor Size When set, the size of the alternate descriptor _described
 *  in “Alternate or Enhanced Descriptors” on page 545_ increases to 32 bytes _8
 *  DWORDS_ This is required when the Advanced Timestamp feature or the IPC Full
 *  Checksum Offload Engine _Type 2_ is enabled in the receiver The enhanced descriptor
 *  is not required if the Advanced Timestamp and IPC Full Checksum Offload Engine
 *  _Type 2_ features are not enabled In such case, you can use the 16 bytes descriptor
 *  to save 4 bytes of memory This bit is present only when you select the Alternate
 *  Descriptor feature and any one of the following features during core configuration:
 *  Advanced Timestamp feature IPC Full Checksum Offload Engine _Type 2_ feature
 *  Otherwise, this bit is reserved and is readonly When reset, the descriptor size
 *  reverts back to 4 DWORDs _16 bytes_ This bit preserves the backward compatibility
 *  for the descriptor size In versions prior to 350a, the descriptor size is 16 bytes
 *  for both normal and enhanced descriptors In version 350a, descriptor size is
 *  increased to 32 bytes because of the Advanced Timestamp and IPC Full Checksum
 *  Offload Engine _Type 2_ features
 */
#define EMAC_ATDS    (BIT(7))
#define EMAC_ATDS_M  (EMAC_ATDS_V << EMAC_ATDS_S)
#define EMAC_ATDS_V  0x00000001U
#define EMAC_ATDS_S  7
/** EMAC_PBL : R/W; bitpos: [13:8]; default: 1;
 *  Programmable Burst Length These bits indicate the maximum number of beats to be
 *  transferred in one DMA transaction This is the maximum value that is used in a
 *  single block Read or Write The DMA always attempts to burst as specified in PBL
 *  each time it starts a Burst transfer on the host bus PBL can be programmed with
 *  permissible values of 1, 2, 4, 8, 16, and 32 Any other value results in undefined
 *  behavior When USP is set high, this PBL value is applicable only for Tx DMA
 *  transactions If the number of beats to be transferred is more than 32, then perform
 *  the following steps: 1 Set the PBLx8 mode 2 Set the PBL For example, if the maximum
 *  number of beats to be transferred is 64, then first set PBLx8 to 1 and then set PBL
 *  to 8 The PBL values have the following limitation: The maximum number of possible
 *  beats _PBL_ is limited by the size of the Tx FIFO and Rx FIFO in the MTL layer and
 *  the data bus width on the DMA The FIFO has a constraint that the maximum beat
 *  supported is half the depth of the FIFO, except when specified For different data
 *  bus widths and FIFO sizes, the valid PBL range _including x8 mode_ is provided in
 *  Table 66 on page 382
 */
#define EMAC_PBL    0x0000003FU
#define EMAC_PBL_M  (EMAC_PBL_V << EMAC_PBL_S)
#define EMAC_PBL_V  0x0000003FU
#define EMAC_PBL_S  8
/** EMAC_PR : R/W; bitpos: [15:14]; default: 0;
 *  Priority Ratio These bits control the priority ratio in the weighted roundrobin
 *  arbitration between the Rx DMA and Tx DMA These bits are valid only when Bit 1 _DA_
 *  is reset The priority ratio is Rx:Tx or Tx:Rx depending on whether Bit 27 _TXPR_ is
 *  reset or set 00: The Priority Ratio is 1:1 01: The Priority Ratio is 2:1 10: The
 *  Priority Ratio is 3:1 11: The Priority Ratio is 4:1 In the EMACAXI configuration,
 *  these bits are reserved and readonly _RO_ For more information about the priority
 *  scheme between the transmit and receive paths, see Table 412 in “DMA Arbiter
 *  Functions” on page 167
 */
#define EMAC_PR    0x00000003U
#define EMAC_PR_M  (EMAC_PR_V << EMAC_PR_S)
#define EMAC_PR_V  0x00000003U
#define EMAC_PR_S  14
/** EMAC_FB : R/W; bitpos: [16]; default: 0;
 *  Fixed Burst This bit controls whether the AHB or AXI master interface performs
 *  fixed burst transfers or not When set, the AHB interface uses only SINGLE, INCR4,
 *  INCR8, or INCR16 during start of the normal burst transfers When reset, the AHB or
 *  AXI interface uses SINGLE and INCR burst transfer operations For more information,
 *  see Bit 0 _UNDEF_ of the AXI Bus Mode register in the EMACAXI configuration
 */
#define EMAC_FB    (BIT(16))
#define EMAC_FB_M  (EMAC_FB_V << EMAC_FB_S)
#define EMAC_FB_V  0x00000001U
#define EMAC_FB_S  16
/** EMAC_RPBL : R/W; bitpos: [22:17]; default: 1;
 *  Rx DMA PBL This field indicates the maximum number of beats to be transferred in
 *  one Rx DMA transaction This is the maximum value that is used in a single block
 *  Read or Write The Rx DMA always attempts to burst as specified in the RPBL bit each
 *  time it starts a Burst transfer on the host bus You can program RPBL with values of
 *  1, 2, 4, 8, 16, and 32 Any other value results in undefined behavior This field is
 *  valid and applicable only when USP is set high
 */
#define EMAC_RPBL    0x0000003FU
#define EMAC_RPBL_M  (EMAC_RPBL_V << EMAC_RPBL_S)
#define EMAC_RPBL_V  0x0000003FU
#define EMAC_RPBL_S  17
/** EMAC_USP : R/W; bitpos: [23]; default: 0;
 *  Use Separate PBL When set high, this bit configures the Rx DMA to use the value
 *  configured in Bits [22:17] as PBL The PBL value in Bits [13:8] is applicable only
 *  to the Tx DMA operations When reset to low, the PBL value in Bits [13:8] is
 *  applicable for both DMA engines
 */
#define EMAC_USP    (BIT(23))
#define EMAC_USP_M  (EMAC_USP_V << EMAC_USP_S)
#define EMAC_USP_V  0x00000001U
#define EMAC_USP_S  23
/** EMAC_PBLX8 : R/W; bitpos: [24]; default: 0;
 *  PBLx8 Mode When set high, this bit multiplies the programmed PBL value _Bits
 *  [22:17] and Bits[13:8]_ eight times Therefore, the DMA transfers the data in 8, 16,
 *  32, 64, 128, and 256 beats depending on the PBL value Note: This bit function is
 *  not backward compatible Before release 350a, this bit was 4xPBL
 */
#define EMAC_PBLX8    (BIT(24))
#define EMAC_PBLX8_M  (EMAC_PBLX8_V << EMAC_PBLX8_S)
#define EMAC_PBLX8_V  0x00000001U
#define EMAC_PBLX8_S  24
/** EMAC_AAL : R/W; bitpos: [25]; default: 0;
 *  AddressAligned Beats When this bit is set high and the FB bit is equal to 1, the
 *  AHB or AXI interface generates all bursts aligned to the start address LS bits If
 *  the FB bit is equal to 0, the first burst _accessing the start address of data
 *  buffer_ is not aligned, but subsequent bursts are aligned to the address This bit
 *  is valid only in the EMACAHB and EMACAXI configurations and is reserved _RO with
 *  default value 0_ in all other configurations
 */
#define EMAC_AAL    (BIT(25))
#define EMAC_AAL_M  (EMAC_AAL_V << EMAC_AAL_S)
#define EMAC_AAL_V  0x00000001U
#define EMAC_AAL_S  25
/** EMAC_MB : R/W; bitpos: [26]; default: 0;
 *  Mixed Burst When this bit is set high and the FB bit is low, the AHB master
 *  interface starts all bursts of length more than 16 with INCR _undefined burst_,
 *  whereas it reverts to fixed burst transfers _INCRx and SINGLE_ for burst length of
 *  16 and less This bit is valid only in the EMACAHB configuration and reserved in all
 *  other configuration
 */
#define EMAC_MB    (BIT(26))
#define EMAC_MB_M  (EMAC_MB_V << EMAC_MB_S)
#define EMAC_MB_V  0x00000001U
#define EMAC_MB_S  26
/** EMAC_TXPR : R/W; bitpos: [27]; default: 0;
 *  Transmit Priority When set, this bit indicates that the transmit DMA has higher
 *  priority than the receive DMA during arbitration for the systemside bus In the
 *  EMACAXI configuration, this bit is reserved and readonly _RO_ For more information
 *  about the priority scheme between the transmit and receive paths, see Table 412 in
 *  “DMA Arbiter Functions” on page 167
 */
#define EMAC_TXPR    (BIT(27))
#define EMAC_TXPR_M  (EMAC_TXPR_V << EMAC_TXPR_S)
#define EMAC_TXPR_V  0x00000001U
#define EMAC_TXPR_S  27
/** EMAC_PRWG : R/W; bitpos: [29:28]; default: 0;
 *  Channel Priority Weights This field sets the priority weights for Channel 0 during
 *  the roundrobin arbitration between the DMA channels for the system bus 00: The
 *  priority weight is 1 01: The priority weight is 2 10: The priority weight is 3 11:
 *  The priority weight is 4 This field is present in all DWC_EMAC configurations
 *  except EMACAXI when you select the AV feature Otherwise, this field is reserved and
 *  readonly _RO_ For more information about the priority weights of DMA channels, see
 *  “DMA Arbiter Functions” on page 167
 */
#define EMAC_PRWG    0x00000003U
#define EMAC_PRWG_M  (EMAC_PRWG_V << EMAC_PRWG_S)
#define EMAC_PRWG_V  0x00000003U
#define EMAC_PRWG_S  28
/** EMAC_RIB : R/W; bitpos: [31]; default: 0;
 *  Rebuild INCRx Burst When this bit is set high and the AHB master gets an EBT
 *  _Retry, Split, or Losing bus grant_, the AHB master interface rebuilds the pending
 *  beats of any burst transfer initiated with INCRx The AHB master interface rebuilds
 *  the beats with a combination of specified bursts with INCRx and SINGLE By default,
 *  the AHB master interface rebuilds pending beats of an EBT with an unspecified
 *  _INCR_ burst This bit is valid only in the EMACAHB configuration It is reserved in
 *  all other configuration
 */
#define EMAC_RIB    (BIT(31))
#define EMAC_RIB_M  (EMAC_RIB_V << EMAC_RIB_S)
#define EMAC_RIB_V  0x00000001U
#define EMAC_RIB_S  31

/** EMAC_TRANSMITPOLLDEMAND_REG register
 *  Used by the host to instruct the DMA to poll the Transmit Descriptor list
 */
#define EMAC_TRANSMITPOLLDEMAND_REG (DR_REG_EMAC_BASE + 0x1004)
/** EMAC_TPD : RO; bitpos: [31:0]; default: 0;
 *  Transmit Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 18 _Current Host Transmit Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  transmission returns to the Suspend state and Bit 2 _TU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the transmission resumes
 */
#define EMAC_TPD    0xFFFFFFFFU
#define EMAC_TPD_M  (EMAC_TPD_V << EMAC_TPD_S)
#define EMAC_TPD_V  0xFFFFFFFFU
#define EMAC_TPD_S  0

/** EMAC_RECEIVEPOLLDEMAND_REG register
 *  Used by the host to instruct the DMA to poll the Receive Descriptor list
 */
#define EMAC_RECEIVEPOLLDEMAND_REG (DR_REG_EMAC_BASE + 0x1008)
/** EMAC_RPD : RO; bitpos: [31:0]; default: 0;
 *  Receive Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 19 _Current Host Receive Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  reception returns to the Suspended state and Bit 7 _RU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the Rx DMA returns to the
 *  active state
 */
#define EMAC_RPD    0xFFFFFFFFU
#define EMAC_RPD_M  (EMAC_RPD_V << EMAC_RPD_S)
#define EMAC_RPD_V  0xFFFFFFFFU
#define EMAC_RPD_S  0

/** EMAC_RECEIVEDESCRIPTORLISTADDRESS_REG register
 *  Points the DMA to the start of the Receive Descriptor list
 */
#define EMAC_RECEIVEDESCRIPTORLISTADDRESS_REG (DR_REG_EMAC_BASE + 0x100c)
/** EMAC_RDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Receive List This field contains the base address of the first descriptor
 *  in the Receive Descriptor list The LSB bits _1:0, 2:0, or 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and internally taken as allzero by the DMA Therefore,
 *  these LSB bits are readonly _RO_
 */
#define EMAC_RDESLA    0xFFFFFFFFU
#define EMAC_RDESLA_M  (EMAC_RDESLA_V << EMAC_RDESLA_S)
#define EMAC_RDESLA_V  0xFFFFFFFFU
#define EMAC_RDESLA_S  0

/** EMAC_TRANSMITDESCRIPTORLISTADDRESS_REG register
 *  Points the DMA to the start of the Transmit Descriptor list
 */
#define EMAC_TRANSMITDESCRIPTORLISTADDRESS_REG (DR_REG_EMAC_BASE + 0x1010)
/** EMAC_TDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Transmit List This field contains the base address of the first descriptor
 *  in the Transmit Descriptor list The LSB bits _1:0, 2:0, 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and are internally taken as allzero by the DMA
 *  Therefore, these LSB bits are readonly _RO_
 */
#define EMAC_TDESLA    0xFFFFFFFFU
#define EMAC_TDESLA_M  (EMAC_TDESLA_V << EMAC_TDESLA_S)
#define EMAC_TDESLA_V  0xFFFFFFFFU
#define EMAC_TDESLA_S  0

/** EMAC_STATUS_REG register
 *  The Software driver _application_ reads this register during interrupt service
 *  routine or polling to determine the status of the DMA
 */
#define EMAC_STATUS_REG (DR_REG_EMAC_BASE + 0x1014)
/** EMAC_TI : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt This bit indicates that the frame transmission is complete When
 *  transmission is complete, Bit 31 _OWN_ of TDES0 is reset, and the specific frame
 *  status information is updated in the descriptor
 */
#define EMAC_TI    (BIT(0))
#define EMAC_TI_M  (EMAC_TI_V << EMAC_TI_S)
#define EMAC_TI_V  0x00000001U
#define EMAC_TI_S  0
/** EMAC_TPS : R/W; bitpos: [1]; default: 0;
 *  Transmit Process Stopped This bit is set when the transmission is stopped
 */
#define EMAC_TPS    (BIT(1))
#define EMAC_TPS_M  (EMAC_TPS_V << EMAC_TPS_S)
#define EMAC_TPS_V  0x00000001U
#define EMAC_TPS_S  1
/** EMAC_TU : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Transmit List and the DMA cannot acquire it Transmission is
 *  suspended Bits[22:20] explain the Transmit Process state transitions To resume
 *  processing Transmit descriptors, the host should change the ownership of the
 *  descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command
 */
#define EMAC_TU    (BIT(2))
#define EMAC_TU_M  (EMAC_TU_V << EMAC_TU_S)
#define EMAC_TU_V  0x00000001U
#define EMAC_TU_S  2
/** EMAC_TJT : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout This bit indicates that the Transmit Jabber Timer expired,
 *  which happens when the frame size exceeds 2,048 _10,240 bytes when the Jumbo frame
 *  is enabled_ When the Jabber Timeout occurs, the transmission process is aborted and
 *  placed in the Stopped state This causes the Transmit Jabber Timeout TDES0[14] flag
 *  to assert
 */
#define EMAC_TJT    (BIT(3))
#define EMAC_TJT_M  (EMAC_TJT_V << EMAC_TJT_S)
#define EMAC_TJT_V  0x00000001U
#define EMAC_TJT_S  3
/** EMAC_OVF : R/W; bitpos: [4]; default: 0;
 *  Receive Overflow This bit indicates that the Receive Buffer had an Overflow during
 *  frame reception If the partial frame is transferred to the application, the
 *  overflow status is set in RDES0[11]
 */
#define EMAC_OVF    (BIT(4))
#define EMAC_OVF_M  (EMAC_OVF_V << EMAC_OVF_S)
#define EMAC_OVF_V  0x00000001U
#define EMAC_OVF_S  4
/** EMAC_UNF : R/W; bitpos: [5]; default: 0;
 *  Transmit Underflow This bit indicates that the Transmit Buffer had an Underflow
 *  during frame transmission Transmission is suspended and an Underflow Error TDES0[1]
 *  is set
 */
#define EMAC_UNF    (BIT(5))
#define EMAC_UNF_M  (EMAC_UNF_V << EMAC_UNF_S)
#define EMAC_UNF_V  0x00000001U
#define EMAC_UNF_S  5
/** EMAC_RI : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt This bit indicates that the frame reception is complete When
 *  reception is complete, the Bit 31 of RDES1 _Disable Interrupt on Completion_ is
 *  reset in the last Descriptor, and the specific frame status information is updated
 *  in the descriptor The reception remains in the Running state
 */
#define EMAC_RI    (BIT(6))
#define EMAC_RI_M  (EMAC_RI_V << EMAC_RI_S)
#define EMAC_RI_V  0x00000001U
#define EMAC_RI_S  6
/** EMAC_RU : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Receive List and the DMA cannot acquire it The Receive Process is
 *  suspended To resume processing Receive descriptors, the host should change the
 *  ownership of the descriptor and issue a Receive Poll Demand command If no Receive
 *  Poll Demand is issued, the Receive Process resumes when the next recognized
 *  incoming frame is received This bit is set only when the previous Receive
 *  Descriptor is owned by the DMA
 */
#define EMAC_RU    (BIT(7))
#define EMAC_RU_M  (EMAC_RU_V << EMAC_RU_S)
#define EMAC_RU_V  0x00000001U
#define EMAC_RU_S  7
/** EMAC_RPS : R/W; bitpos: [8]; default: 0;
 *  Receive Process Stopped This bit is asserted when the Receive Process enters the
 *  Stopped state
 */
#define EMAC_RPS    (BIT(8))
#define EMAC_RPS_M  (EMAC_RPS_V << EMAC_RPS_S)
#define EMAC_RPS_V  0x00000001U
#define EMAC_RPS_S  8
/** EMAC_RWT : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout When set, this bit indicates that the Receive Watchdog
 *  Timer expired while receiving the current frame and the current frame is truncated
 *  after the watchdog timeout
 */
#define EMAC_RWT    (BIT(9))
#define EMAC_RWT_M  (EMAC_RWT_V << EMAC_RWT_S)
#define EMAC_RWT_V  0x00000001U
#define EMAC_RWT_S  9
/** EMAC_ETI : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt This bit indicates that the frame to be transmitted is
 *  fully transferred to the MTL Transmit FIFO
 */
#define EMAC_ETI    (BIT(10))
#define EMAC_ETI_M  (EMAC_ETI_V << EMAC_ETI_S)
#define EMAC_ETI_V  0x00000001U
#define EMAC_ETI_S  10
/** EMAC_FBI : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Interrupt This bit indicates that a bus error occurred, as
 *  described in Bits [25:23] When this bit is set, the corresponding DMA engine
 *  disables all of its bus accesses 12:11		Reserved	00	RO
 */
#define EMAC_FBI    (BIT(13))
#define EMAC_FBI_M  (EMAC_FBI_V << EMAC_FBI_S)
#define EMAC_FBI_V  0x00000001U
#define EMAC_FBI_S  13
/** EMAC_ERI : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt This bit indicates that the DMA filled the first data
 *  buffer of the packet This bit is cleared when the software writes 1 to this bit or
 *  Bit 6 _RI_ of this register is set _whichever occurs earlier_
 */
#define EMAC_ERI    (BIT(14))
#define EMAC_ERI_M  (EMAC_ERI_V << EMAC_ERI_S)
#define EMAC_ERI_V  0x00000001U
#define EMAC_ERI_S  14
/** EMAC_AIS : R/W; bitpos: [15]; default: 0;
 *  Abnormal Interrupt Summary Abnormal Interrupt Summary bit value is the logical OR
 *  of the following when the corresponding interrupt bits are enabled in Register 7
 *  _Interrupt Enable Register_: Register 5[1]: Transmit Process Stopped Register 5[3]:
 *  Transmit Jabber Timeout Register 5[4]: Receive FIFO Overflow Register 5[5]:
 *  Transmit Underflow Register 5[7]: Receive Buffer Unavailable Register 5[8]: Receive
 *  Process Stopped Register 5[9]: Receive Watchdog Timeout Register 5[10]: Early
 *  Transmit Interrupt Register 5[13]: Fatal Bus Error Only unmasked bits affect the
 *  Abnormal Interrupt Summary bit This is a sticky bit and must be cleared _by writing
 *  1 to this bit_ each time a corresponding bit, which causes AIS to be set, is cleared
 */
#define EMAC_AIS    (BIT(15))
#define EMAC_AIS_M  (EMAC_AIS_V << EMAC_AIS_S)
#define EMAC_AIS_V  0x00000001U
#define EMAC_AIS_S  15
/** EMAC_NIS : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Normal Interrupt Summary bit value is the logical OR of
 *  the following bits when the corresponding interrupt bits are enabled in Register 7
 *  _Interrupt Enable Register_: Register 5[0]: Transmit Interrupt Register 5[2]:
 *  Transmit Buffer Unavailable Register 5[6]: Receive Interrupt Register 5[14]: Early
 *  Receive Interrupt Only unmasked bits _interrupts for which interrupt enable is set
 *  in Register 7_ affect the Normal Interrupt Summary bit This is a sticky bit and
 *  must be cleared _by writing 1 to this bit_ each time a corresponding bit, which
 *  causes NIS to be set, is cleared
 */
#define EMAC_NIS    (BIT(16))
#define EMAC_NIS_M  (EMAC_NIS_V << EMAC_NIS_S)
#define EMAC_NIS_V  0x00000001U
#define EMAC_NIS_S  16
/** EMAC_RS : RO; bitpos: [19:17]; default: 0;
 *  Receive Process State This field indicates the Receive DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Receive Command
 *  issued 3’b001: Running: Fetching Receive Transfer Descriptor 3’b010: Reserved for
 *  future use 3’b011: Running: Waiting for receive packet 3’b100: Suspended: Receive
 *  Descriptor Unavailable 3’b101: Running: Closing Receive Descriptor 3’b110:
 *  TIME_STAMP write state 3’b111: Running: Transferring the receive packet data from
 *  receive buffer to host memory
 */
#define EMAC_RS    0x00000007U
#define EMAC_RS_M  (EMAC_RS_V << EMAC_RS_S)
#define EMAC_RS_V  0x00000007U
#define EMAC_RS_S  17
/** EMAC_TS : RO; bitpos: [22:20]; default: 0;
 *  Transmit Process State This field indicates the Transmit DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Transmit Command
 *  issued 3’b001: Running: Fetching Transmit Transfer Descriptor 3’b010: Running:
 *  Waiting for status 3’b011: Running: Reading Data from host memory buffer and
 *  queuing it to transmit buffer _Tx FIFO_ 3’b100: TIME_STAMP write state 3’b101:
 *  Reserved for future use 3’b110: Suspended: Transmit Descriptor Unavailable or
 *  Transmit Buffer Underflow 3’b111: Running: Closing Transmit Descriptor
 */
#define EMAC_TS    0x00000007U
#define EMAC_TS_M  (EMAC_TS_V << EMAC_TS_S)
#define EMAC_TS_V  0x00000007U
#define EMAC_TS_S  20
/** EMAC_EB : RO; bitpos: [25:23]; default: 0;
 *  Error Bits This field indicates the type of error that caused a Bus Error, for
 *  example, error response on the AHB or AXI interface This field is valid only when
 *  Bit 13 _FBI_ is set This field does not generate an interrupt 0 0 0: Error during
 *  Rx DMA Write Data Transfer 0 1 1: Error during Tx DMA Read Data Transfer 1 0 0:
 *  Error during Rx DMA Descriptor Write Access 1 0 1: Error during Tx DMA Descriptor
 *  Write Access 1 1 0: Error during Rx DMA Descriptor Read Access 1 1 1: Error during
 *  Tx DMA Descriptor Read Access Note: 001 and 010 are reserved
 */
#define EMAC_EB    0x00000007U
#define EMAC_EB_M  (EMAC_EB_V << EMAC_EB_S)
#define EMAC_EB_V  0x00000007U
#define EMAC_EB_S  23
/** EMAC_GLI : RO; bitpos: [26]; default: 0;
 *  EMAC Line Interface Interrupt When set, this bit reflects any of the following
 *  interrupt events in the DWC_EMAC interfaces _if present and enabled in your
 *  configuration_: PCS _TBI, RTBI, or SGMII_: Link change or autonegotiation complete
 *  event SMII or RGMII: Link change event General Purpose Input Status _GPIS_: Any LL
 *  or LH event on the gpi_i input ports To identify the exact cause of the interrupt,
 *  the software must first read Bit 11 and Bits[2:0] of Register 14 _Interrupt Status
 *  Register_ and then to clear the source of interrupt _which also clears the GLI
 *  interrupt_, read any of the following corresponding registers: PCS _TBI, RTBI, or
 *  SGMII_: Register 49 _AN Status Register_ SMII or RGMII: Register 54
 *  _SGMII/RGMII/SMII Control and Status Register_ General Purpose Input _GPI_:
 *  Register 56 _General Purpose IO Register_ The interrupt signal from the DWC_EMAC
 *  subsystem _sbd_intr_o_ is high when this bit is high
 */
#define EMAC_GLI    (BIT(26))
#define EMAC_GLI_M  (EMAC_GLI_V << EMAC_GLI_S)
#define EMAC_GLI_V  0x00000001U
#define EMAC_GLI_S  26
/** EMAC_GMI : RO; bitpos: [27]; default: 0;
 *  EMAC MMC Interrupt This bit reflects an interrupt event in the MMC module of the
 *  DWC_EMAC The software must read the corresponding registers in the DWC_EMAC to get
 *  the exact cause of the interrupt and clear the source of interrupt to make this bit
 *  as 1’b0 The interrupt signal from the DWC_EMAC subsystem _sbd_intr_o_ is high when
 *  this bit is high This bit is applicable only when the MAC Management Counters _MMC_
 *  are enabled Otherwise, this bit is reserved
 */
#define EMAC_GMI    (BIT(27))
#define EMAC_GMI_M  (EMAC_GMI_V << EMAC_GMI_S)
#define EMAC_GMI_V  0x00000001U
#define EMAC_GMI_S  27
/** EMAC_GPI : RO; bitpos: [28]; default: 0;
 *  EMAC PMT Interrupt This bit indicates an interrupt event in the PMT module of the
 *  DWC_EMAC The software must read the PMT Control and Status Register in the MAC to
 *  get the exact cause of interrupt and clear its source to reset this bit to 1’b0 The
 *  interrupt signal from the DWC_EMAC subsystem _sbd_intr_o_ is high when this bit is
 *  high This bit is applicable only when the Power Management feature is enabled
 *  Otherwise, this bit is reserved Note: The GPI and pmt_intr_o interrupts are
 *  generated in different clock domains
 */
#define EMAC_GPI    (BIT(28))
#define EMAC_GPI_M  (EMAC_GPI_V << EMAC_GPI_S)
#define EMAC_GPI_V  0x00000001U
#define EMAC_GPI_S  28
/** EMAC_TTI : RO; bitpos: [29]; default: 0;
 *  Timestamp Trigger Interrupt This bit indicates an interrupt event in the Timestamp
 *  Generator block of the DWC_EMAC The software must read the corresponding registers
 *  in the DWC_EMAC to get the exact cause of the interrupt and clear its source to
 *  reset this bit to 1'b0 When this bit is high, the interrupt signal from the
 *  DWC_EMAC subsystem _sbd_intr_o_ is high This bit is applicable only when the IEEE
 *  1588 Timestamp feature is enabled Otherwise, this bit is reserved
 */
#define EMAC_TTI    (BIT(29))
#define EMAC_TTI_M  (EMAC_TTI_V << EMAC_TTI_S)
#define EMAC_TTI_V  0x00000001U
#define EMAC_TTI_S  29
/** EMAC_GLPII_GTMSI : RO; bitpos: [30]; default: 0;
 *  GLPII: EMAC LPI Interrupt This bit indicates an interrupt event in the LPI logic of
 *  the MAC To reset this bit to 1'b0, the software must read the corresponding
 *  registers in the DWC_EMAC to get the exact cause of the interrupt and clear its
 *  source Note: GLPII status is given only in Channel 0 DMA register and is applicable
 *  only when the Energy Efficient Ethernet feature is enabled Otherwise, this bit is
 *  reserved When this bit is high, the interrupt signal from the MAC _sbd_intr_o_ is
 *  high
 */
#define EMAC_GLPII_GTMSI    (BIT(30))
#define EMAC_GLPII_GTMSI_M  (EMAC_GLPII_GTMSI_V << EMAC_GLPII_GTMSI_S)
#define EMAC_GLPII_GTMSI_V  0x00000001U
#define EMAC_GLPII_GTMSI_S  30

/** EMAC_OPERATIONMODE_REG register
 *  Establishes the Receive and Transmit operating modes and command Note: This
 *  register is valid and present in the EMACMTL configuration
 */
#define EMAC_OPERATIONMODE_REG (DR_REG_EMAC_BASE + 0x1018)
/** EMAC_SR : R/W; bitpos: [1]; default: 0;
 *  Start or Stop Receive When this bit is set, the Receive process is placed in the
 *  Running state The DMA attempts to acquire the descriptor from the Receive list and
 *  processes the incoming frames The descriptor acquisition is attempted from the
 *  current position in the list, which is the address set by the Register 3 _Receive
 *  Descriptor List Address Register_ or the position retained when the Receive process
 *  was previously stopped If the DMA does not own the descriptor, reception is
 *  suspended and Bit 7 _Receive Buffer Unavailable_ of Register 5 _Status Register_ is
 *  set The Start Receive command is effective only when the reception has stopped If
 *  the command is issued before setting Register 3 _Receive Descriptor List Address
 *  Register_, the DMA behavior is unpredictable When this bit is cleared, the Rx DMA
 *  operation is stopped after the transfer of the current frame The next descriptor
 *  position in the Receive list is saved and becomes the current position after the
 *  Receive process is restarted The Stop Receive command is effective only when the
 *  Receive process is in either the Running _waiting for receive packet_ or in the
 *  Suspended state Note: For information about how to pause the transmission, see
 *  “Stopping and Starting Transmission” on page 715
 */
#define EMAC_SR    (BIT(1))
#define EMAC_SR_M  (EMAC_SR_V << EMAC_SR_S)
#define EMAC_SR_V  0x00000001U
#define EMAC_SR_S  1
/** EMAC_OSF : R/W; bitpos: [2]; default: 0;
 *  Operate on Second Frame When this bit is set, it instructs the DMA to process the
 *  second frame of the Transmit data even before the status for the first frame is
 *  obtained
 */
#define EMAC_OSF    (BIT(2))
#define EMAC_OSF_M  (EMAC_OSF_V << EMAC_OSF_S)
#define EMAC_OSF_V  0x00000001U
#define EMAC_OSF_S  2
/** EMAC_RTC : R/W; bitpos: [4:3]; default: 0;
 *  Receive Threshold Control These two bits control the threshold level of the MTL
 *  Receive FIFO Transfer _request_ to DMA starts when the frame size within the MTL
 *  Receive FIFO is larger than the threshold In addition, full frames with length less
 *  than the threshold are automatically transferred The value of 11 is not applicable
 *  if the configured Receive FIFO size is 128 bytes These bits are valid only when the
 *  RSF bit is zero, and are ignored when the RSF bit is set to 1 00: 64 01: 32 10: 96
 *  11: 128
 */
#define EMAC_RTC    0x00000003U
#define EMAC_RTC_M  (EMAC_RTC_V << EMAC_RTC_S)
#define EMAC_RTC_V  0x00000003U
#define EMAC_RTC_S  3
/** EMAC_DGF : R/W; bitpos: [5]; default: 0;
 *  Drop Giant Frames When set, the MAC drops the received giant frames in the Rx FIFO,
 *  that is, frames that are larger than the computed giant frame limit When reset, the
 *  MAC does not drop the giant frames in the Rx FIFO Note: This bit is available in
 *  the following configurations in which the giant frame status is not provided in Rx
 *  status and giant frames are not dropped by default: Configurations in which IP
 *  Checksum Offload _Type 1_ is selected in Rx Configurations in which the IPC Full
 *  Checksum Offload Engine _Type 2_ is selected in Rx with normal descriptor format
 *  Configurations in which the Advanced Timestamp feature is selected In all other
 *  configurations, this bit is not used _reserved and always reset_
 */
#define EMAC_DGF    (BIT(5))
#define EMAC_DGF_M  (EMAC_DGF_V << EMAC_DGF_S)
#define EMAC_DGF_V  0x00000001U
#define EMAC_DGF_S  5
/** EMAC_FUF : R/W; bitpos: [6]; default: 0;
 *  Forward Undersized Good Frames When set, the Rx FIFO forwards Undersized frames
 *  _that is, frames with no Error and length less than 64 bytes_ including padbytes
 *  and CRC When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a
 *  frame is already transferred because of the lower value of Receive Threshold, for
 *  example, RTC = 01
 */
#define EMAC_FUF    (BIT(6))
#define EMAC_FUF_M  (EMAC_FUF_V << EMAC_FUF_S)
#define EMAC_FUF_V  0x00000001U
#define EMAC_FUF_S  6
/** EMAC_FEF : R/W; bitpos: [7]; default: 0;
 *  Forward Error Frames When this bit is reset, the Rx FIFO drops frames with error
 *  status _CRC error, collision error, GMII_ER, giant frame, watchdog timeout, or
 *  overflow_ However, if the start byte _write_ pointer of a frame is already
 *  transferred to the read controller side _in Threshold mode_, then the frame is not
 *  dropped In the EMACMTL configuration in which the Frame Length FIFO is also enabled
 *  during core configuration, the Rx FIFO drops the error frames if that frame's start
 *  byte is not transferred _output_ on the ARI bus When the FEF bit is set, all frames
 *  except runt error frames are forwarded to the DMA If the Bit 25 _RSF_ is set and
 *  the Rx FIFO overflows when a partial frame is written, then the frame is dropped
 *  irrespective of the FEF bit setting However, if the Bit 25 _RSF_ is reset and the
 *  Rx FIFO overflows when a partial frame is written, then a partial frame may be
 *  forwarded to the DMA Note: When FEF bit is reset, the giant frames are dropped if
 *  the giant frame status is given in Rx Status _in Table 86 or Table 823_ in the
 *  following configurations: The IP checksum engine _Type 1_ and full checksum offload
 *  engine _Type 2_ are not selected The advanced timestamp feature is not selected but
 *  the extended status is selected The extended status is available with the following
 *  features: 	L3L4 filter in EMACCORE or EMACMTL configurations 	Full checksum offload
 *  engine _Type 2_ with enhanced descriptor format in the EMACDMA, EMACAHB, or EMACAXI
 *  configurations
 */
#define EMAC_FEF    (BIT(7))
#define EMAC_FEF_M  (EMAC_FEF_V << EMAC_FEF_S)
#define EMAC_FEF_V  0x00000001U
#define EMAC_FEF_S  7
/** EMAC_EFC : R/W; bitpos: [8]; default: 0;
 *  Enable HW Flow Control When this bit is set, the flow control signal operation
 *  based on the filllevel of Rx FIFO is enabled When reset, the flow control operation
 *  is disabled This bit is not used _reserved and always reset_ when the Rx FIFO is
 *  less than 4 KB
 */
#define EMAC_EFC    (BIT(8))
#define EMAC_EFC_M  (EMAC_EFC_V << EMAC_EFC_S)
#define EMAC_EFC_V  0x00000001U
#define EMAC_EFC_S  8
/** EMAC_RFA : R/W; bitpos: [10:9]; default: 0;
 *  Threshold for Activating Flow Control _in halfduplex and fullduplex modes_ These
 *  bits control the threshold _Fill level of Rx FIFO_ at which the flow control is
 *  activated 00: Full minus 1 KB, that is, FULL—1KB 01: Full minus 2 KB, that is,
 *  FULL—2KB 10: Full minus 3 KB, that is, FULL—3KB 11: Full minus 4 KB, that is,
 *  FULL—4KB These values are applicable only to Rx FIFOs of 4 KB or more and when Bit
 *  8 _EFC_ is set high If the Rx FIFO is 8 KB or more, an additional Bit _RFA_2_ is
 *  used for more threshold levels as described in Bit 23 These bits are reserved and
 *  readonly when the depth of Rx FIFO is less than 4 KB Note: When FIFO size is
 *  exactly 4 KB, although the DWC_EMAC allows you to program the value of these bits
 *  to 11, the software should not program these bits to 2'b11 The value 2'b11 means
 *  flow control on FIFO empty condition
 */
#define EMAC_RFA    0x00000003U
#define EMAC_RFA_M  (EMAC_RFA_V << EMAC_RFA_S)
#define EMAC_RFA_V  0x00000003U
#define EMAC_RFA_S  9
/** EMAC_RFD : R/W; bitpos: [12:11]; default: 0;
 *  Threshold for Deactivating Flow Control _in halfduplex and fullduplex modes_ These
 *  bits control the threshold _Filllevel of Rx FIFO_ at which the flow control is
 *  deasserted after activation 00: Full minus 1 KB, that is, FULL — 1 KB 01: Full
 *  minus 2 KB, that is, FULL — 2 KB 10: Full minus 3 KB, that is, FULL — 3 KB 11: Full
 *  minus 4 KB, that is, FULL — 4 KB The deassertion is effective only after flow
 *  control is asserted If the Rx FIFO is 8 KB or more, an additional Bit _RFD_2_ is
 *  used for more threshold levels as described in Bit 22 These bits are reserved and
 *  readonly when the Rx FIFO depth is less than 4 KB Note: For proper flow control,
 *  the value programmed in the “RFD_2, RFD” fields should be equal to or more than the
 *  value programmed in the “RFA_2, RFA” fields
 */
#define EMAC_RFD    0x00000003U
#define EMAC_RFD_M  (EMAC_RFD_V << EMAC_RFD_S)
#define EMAC_RFD_V  0x00000003U
#define EMAC_RFD_S  11
/** EMAC_ST : R/W; bitpos: [13]; default: 0;
 *  Start or Stop Transmission Command When this bit is set, transmission is placed in
 *  the Running state, and the DMA checks the Transmit List at the current position for
 *  a frame to be transmitted Descriptor acquisition is attempted either from the
 *  current position in the list, which is the Transmit List Base Address set by
 *  Register 4 _Transmit Descriptor List Address Register_, or from the position
 *  retained when transmission was stopped previously If the DMA does not own the
 *  current descriptor, transmission enters the Suspended state and Bit 2 _Transmit
 *  Buffer Unavailable_ of Register 5 _Status Register_ is set The Start Transmission
 *  command is effective only when transmission is stopped If the command is issued
 *  before setting Register 4 _Transmit Descriptor List Address Register_, then the DMA
 *  behavior is unpredictable When this bit is reset, the transmission process is
 *  placed in the Stopped state after completing the transmission of the current frame
 *  The Next Descriptor position in the Transmit List is saved, and it becomes the
 *  current position when transmission is restarted To change the list address, you
 *  need to program Register 4 _Transmit Descriptor List Address Register_ with a new
 *  value when this bit is reset The new value is considered when this bit is set again
 *  The stop transmission command is effective only when the transmission of the
 *  current frame is complete or the transmission is in the Suspended state Note: For
 *  information about how to pause the transmission, see “Stopping and Starting
 *  Transmission” on page 715
 */
#define EMAC_ST    (BIT(13))
#define EMAC_ST_M  (EMAC_ST_V << EMAC_ST_S)
#define EMAC_ST_V  0x00000001U
#define EMAC_ST_S  13
/** EMAC_TTC : R/W; bitpos: [16:14]; default: 0;
 *  Transmit Threshold Control These bits control the threshold level of the MTL
 *  Transmit FIFO Transmission starts when the frame size within the MTL Transmit FIFO
 *  is larger than the threshold In addition, full frames with a length less than the
 *  threshold are also transmitted These bits are used only when Bit 21 _TSF_ is reset
 *  000: 64 001: 128 010: 192 011: 256 100: 40 101: 32 110: 24 111: 16
 */
#define EMAC_TTC    0x00000007U
#define EMAC_TTC_M  (EMAC_TTC_V << EMAC_TTC_S)
#define EMAC_TTC_V  0x00000007U
#define EMAC_TTC_S  14
/** EMAC_FTF : R/W1S; bitpos: [20]; default: 0;
 *  Flush Transmit FIFO When this bit is set, the transmit FIFO controller logic is
 *  reset to its default values and thus all data in the Tx FIFO is lost or flushed
 *  This bit is cleared internally when the flushing operation is complete The
 *  Operation Mode register should not be written to until this bit is cleared The data
 *  which is already accepted by the MAC transmitter is not flushed It is scheduled for
 *  transmission and results in underflow and runt frame transmission Note: The flush
 *  operation is complete only when the Tx FIFO is emptied of its contents and all the
 *  pending Transmit Status of the transmitted frames are accepted by the host In order
 *  to complete this flush operation, the PHY transmit clock _clk_tx_i_ is required to
 *  be active 19:17		Reserved 	000	RO
 */
#define EMAC_FTF    (BIT(20))
#define EMAC_FTF_M  (EMAC_FTF_V << EMAC_FTF_S)
#define EMAC_FTF_V  0x00000001U
#define EMAC_FTF_S  20
/** EMAC_TSF : R/W; bitpos: [21]; default: 0;
 *  Transmit Store and Forward When this bit is set, transmission starts when a full
 *  frame resides in the MTL Transmit FIFO When this bit is set, the TTC values
 *  specified in Bits [16:14] are ignored This bit should be changed only when the
 *  transmission is stopped
 */
#define EMAC_TSF    (BIT(21))
#define EMAC_TSF_M  (EMAC_TSF_V << EMAC_TSF_S)
#define EMAC_TSF_V  0x00000001U
#define EMAC_TSF_S  21
/** EMAC_RFD_2 : R/W; bitpos: [22]; default: 0;
 *  MSB of Threshold for Deactivating Flow Control If the DWC_EMAC is configured for Rx
 *  FIFO size of 8 KB or more, this bit _when set_ provides additional threshold levels
 *  for deactivating the flow control in both halfduplex and fullduplex modes This bit
 *  _as Most Significant Bit_ along with the RFD _Bits [12:11]_ gives the following
 *  thresholds for deactivating flow control: 100: Full minus 5 KB, that is, FULL — 5
 *  KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that is, FULL —
 *  7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB or less deep
 */
#define EMAC_RFD_2    (BIT(22))
#define EMAC_RFD_2_M  (EMAC_RFD_2_V << EMAC_RFD_2_S)
#define EMAC_RFD_2_V  0x00000001U
#define EMAC_RFD_2_S  22
/** EMAC_RFA_2 : R/W; bitpos: [23]; default: 0;
 *  MSB of Threshold for Activating Flow Control If the DWC_EMAC is configured for an
 *  Rx FIFO size of 8 KB or more, this bit _when set_ provides additional threshold
 *  levels for activating the flow control in both half duplex and fullduplex modes
 *  This bit _as Most Significant Bit_, along with the RFA _Bits [10:9]_, gives the
 *  following thresholds for activating flow control: 100: Full minus 5 KB, that is,
 *  FULL — 5 KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that
 *  is, FULL — 7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB
 *  or less deep
 */
#define EMAC_RFA_2    (BIT(23))
#define EMAC_RFA_2_M  (EMAC_RFA_2_V << EMAC_RFA_2_S)
#define EMAC_RFA_2_V  0x00000001U
#define EMAC_RFA_2_S  23
/** EMAC_DFF : R/W; bitpos: [24]; default: 0;
 *  Disable Flushing of Received Frames When this bit is set, the Rx DMA does not flush
 *  any frames because of the unavailability of receive descriptors or buffers as it
 *  does normally when this bit is reset _See “Receive Process Suspended” on page 83_
 *  This bit is reserved _and RO_ in the EMACMTL configuration
 */
#define EMAC_DFF    (BIT(24))
#define EMAC_DFF_M  (EMAC_DFF_V << EMAC_DFF_S)
#define EMAC_DFF_V  0x00000001U
#define EMAC_DFF_S  24
/** EMAC_RSF : R/W; bitpos: [25]; default: 0;
 *  Receive Store and Forward When this bit is set, the MTL reads a frame from the Rx
 *  FIFO only after the complete frame has been written to it, ignoring the RTC bits
 *  When this bit is reset, the Rx FIFO operates in the cutthrough mode, subject to the
 *  threshold specified by the RTC bits
 */
#define EMAC_RSF    (BIT(25))
#define EMAC_RSF_M  (EMAC_RSF_V << EMAC_RSF_S)
#define EMAC_RSF_V  0x00000001U
#define EMAC_RSF_S  25
/** EMAC_DT : R/W; bitpos: [26]; default: 0;
 *  Disable Dropping of TCP/IP Checksum Error Frames When this bit is set, the MAC does
 *  not drop the frames which only have errors detected by the Receive Checksum Offload
 *  engine Such frames do not have any errors _including FCS error_ in the Ethernet
 *  frame received by the MAC but have errors only in the encapsulated payload When
 *  this bit is reset, all error frames are dropped if the FEF bit is reset If the IPC
 *  Full Checksum Offload Engine _Type 2_ is disabled, this bit is reserved _RO with
 *  value 1'b0_
 */
#define EMAC_DT    (BIT(26))
#define EMAC_DT_M  (EMAC_DT_V << EMAC_DT_S)
#define EMAC_DT_V  0x00000001U
#define EMAC_DT_S  26

/** EMAC_INTERRUPTENABLE_REG register
 *  Enables the interrupts reported by the Status Register
 */
#define EMAC_INTERRUPTENABLE_REG (DR_REG_EMAC_BASE + 0x101c)
/** EMAC_TIE : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Transmit Interrupt is enabled When this bit is reset, the Transmit
 *  Interrupt is disabled The sbd_intr_o interrupt is generated as shown in Figure 61
 *  It is asserted only when the TTI, GPI, GMI, GLI, or GLPII bit of the DMA Status
 *  register is asserted, or when the NIS or AIS Status bit is asserted and the
 *  corresponding Interrupt Enable bits _NIE or AIE_ are enabled
 */
#define EMAC_TIE    (BIT(0))
#define EMAC_TIE_M  (EMAC_TIE_V << EMAC_TIE_S)
#define EMAC_TIE_V  0x00000001U
#define EMAC_TIE_S  0
/** EMAC_TSE : R/W; bitpos: [1]; default: 0;
 *  Transmit Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Transmission Stopped Interrupt is enabled When this bit is reset, the
 *  Transmission Stopped Interrupt is disabled
 */
#define EMAC_TSE    (BIT(1))
#define EMAC_TSE_M  (EMAC_TSE_V << EMAC_TSE_S)
#define EMAC_TSE_V  0x00000001U
#define EMAC_TSE_S  1
/** EMAC_TUE : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable Enable When this bit is set with Normal Interrupt
 *  Summary Enable _Bit 16_, the Transmit Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled
 */
#define EMAC_TUE    (BIT(2))
#define EMAC_TUE_M  (EMAC_TUE_V << EMAC_TUE_S)
#define EMAC_TUE_V  0x00000001U
#define EMAC_TUE_S  2
/** EMAC_TJE : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Jabber Timeout Interrupt is enabled When this bit is
 *  reset, the Transmit Jabber Timeout Interrupt is disabled
 */
#define EMAC_TJE    (BIT(3))
#define EMAC_TJE_M  (EMAC_TJE_V << EMAC_TJE_S)
#define EMAC_TJE_V  0x00000001U
#define EMAC_TJE_S  3
/** EMAC_OVE : R/W; bitpos: [4]; default: 0;
 *  Overflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Receive Overflow Interrupt is enabled When this bit is reset,
 *  the Overflow Interrupt is disabled
 */
#define EMAC_OVE    (BIT(4))
#define EMAC_OVE_M  (EMAC_OVE_V << EMAC_OVE_S)
#define EMAC_OVE_V  0x00000001U
#define EMAC_OVE_S  4
/** EMAC_UNE : R/W; bitpos: [5]; default: 0;
 *  Underflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Underflow Interrupt is enabled When this bit is
 *  reset, the Underflow Interrupt is disabled
 */
#define EMAC_UNE    (BIT(5))
#define EMAC_UNE_M  (EMAC_UNE_V << EMAC_UNE_S)
#define EMAC_UNE_V  0x00000001U
#define EMAC_UNE_S  5
/** EMAC_RIE : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Receive Interrupt is enabled When this bit is reset, the Receive
 *  Interrupt is disabled
 */
#define EMAC_RIE    (BIT(6))
#define EMAC_RIE_M  (EMAC_RIE_V << EMAC_RIE_S)
#define EMAC_RIE_V  0x00000001U
#define EMAC_RIE_S  6
/** EMAC_RUE : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Receive Buffer Unavailable Interrupt is disabled
 */
#define EMAC_RUE    (BIT(7))
#define EMAC_RUE_M  (EMAC_RUE_V << EMAC_RUE_S)
#define EMAC_RUE_V  0x00000001U
#define EMAC_RUE_S  7
/** EMAC_RSE : R/W; bitpos: [8]; default: 0;
 *  Receive Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Receive Stopped Interrupt is enabled When this bit is reset, the
 *  Receive Stopped Interrupt is disabled
 */
#define EMAC_RSE    (BIT(8))
#define EMAC_RSE_M  (EMAC_RSE_V << EMAC_RSE_S)
#define EMAC_RSE_V  0x00000001U
#define EMAC_RSE_S  8
/** EMAC_RWE : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Watchdog Timeout Interrupt is enabled When
 *  this bit is reset, the Receive Watchdog Timeout Interrupt is disabled
 */
#define EMAC_RWE    (BIT(9))
#define EMAC_RWE_M  (EMAC_RWE_V << EMAC_RWE_S)
#define EMAC_RWE_V  0x00000001U
#define EMAC_RWE_S  9
/** EMAC_ETE : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt Enable When this bit is set with an Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Early Transmit Interrupt is enabled When this bit is
 *  reset, the Early Transmit Interrupt is disabled
 */
#define EMAC_ETE    (BIT(10))
#define EMAC_ETE_M  (EMAC_ETE_V << EMAC_ETE_S)
#define EMAC_ETE_V  0x00000001U
#define EMAC_ETE_S  10
/** EMAC_FBE : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Fatal Bus Error Interrupt is enabled When this bit is reset, the
 *  Fatal Bus Error Enable Interrupt is disabled 12:11		Reserved	00	RO
 */
#define EMAC_FBE    (BIT(13))
#define EMAC_FBE_M  (EMAC_FBE_V << EMAC_FBE_S)
#define EMAC_FBE_V  0x00000001U
#define EMAC_FBE_S  13
/** EMAC_ERE : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt Enable When this bit is set with Normal Interrupt Summary
 *  Enable _Bit 16_, the Early Receive Interrupt is enabled When this bit is reset, the
 *  Early Receive Interrupt is disabled
 */
#define EMAC_ERE    (BIT(14))
#define EMAC_ERE_M  (EMAC_ERE_V << EMAC_ERE_S)
#define EMAC_ERE_V  0x00000001U
#define EMAC_ERE_S  14
/** EMAC_AIE : R/W; bitpos: [15]; default: 0;
 *  Abnormal Interrupt Summary Enable When this bit is set, abnormal interrupt summary
 *  is enabled When this bit is reset, the abnormal interrupt summary is disabled This
 *  bit enables the following interrupts in Register 5 _Status Register_: Register
 *  5[1]: Transmit Process Stopped Register 5[3]: Transmit Jabber Timeout Register
 *  5[4]: Receive Overflow Register 5[5]: Transmit Underflow Register 5[7]: Receive
 *  Buffer Unavailable Register 5[8]: Receive Process Stopped Register 5[9]: Receive
 *  Watchdog Timeout Register 5[10]: Early Transmit Interrupt Register 5[13]: Fatal Bus
 *  Error
 */
#define EMAC_AIE    (BIT(15))
#define EMAC_AIE_M  (EMAC_AIE_V << EMAC_AIE_S)
#define EMAC_AIE_V  0x00000001U
#define EMAC_AIE_S  15
/** EMAC_NIE : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Enable When this bit is set, normal interrupt summary is
 *  enabled When this bit is reset, normal interrupt summary is disabled This bit
 *  enables the following interrupts in Register 5 _Status Register_: Register 5[0]:
 *  Transmit Interrupt Register 5[2]: Transmit Buffer Unavailable Register 5[6]:
 *  Receive Interrupt Register 5[14]: Early Receive Interrupt
 */
#define EMAC_NIE    (BIT(16))
#define EMAC_NIE_M  (EMAC_NIE_V << EMAC_NIE_S)
#define EMAC_NIE_V  0x00000001U
#define EMAC_NIE_S  16

/** EMAC_MISSEDFRAMEANDBUFFEROVERFLOWCOUNTER_REG register
 *  Contains the counters for discarded frames because no host Receive Descriptor was
 *  available or because of Receive FIFO Overflow
 */
#define EMAC_MISSEDFRAMEANDBUFFEROVERFLOWCOUNTER_REG (DR_REG_EMAC_BASE + 0x1020)
/** EMAC_MISFRMCNT : R/W; bitpos: [15:0]; default: 0;
 *  Missed Frame Counter This field indicates the number of frames missed by the
 *  controller because of the Host Receive Buffer being unavailable This counter is
 *  incremented each time the DMA discards an incoming frame The counter is cleared
 *  when this register is read with mci_be_i[0] at 1’b1
 */
#define EMAC_MISFRMCNT    0x0000FFFFU
#define EMAC_MISFRMCNT_M  (EMAC_MISFRMCNT_V << EMAC_MISFRMCNT_S)
#define EMAC_MISFRMCNT_V  0x0000FFFFU
#define EMAC_MISFRMCNT_S  0
/** EMAC_MISCNTOVF : R/W; bitpos: [16]; default: 0;
 *  Overflow Bit for Missed Frame Counter This bit is set every time Missed Frame
 *  Counter _Bits[15:0]_ overflows, that is, the DMA discards an incoming frame because
 *  of the Host Receive Buffer being unavailable with the missed frame counter at
 *  maximum value In such a scenario, the Missed frame counter is reset to allzeros and
 *  this bit indicates that the rollover happened
 */
#define EMAC_MISCNTOVF    (BIT(16))
#define EMAC_MISCNTOVF_M  (EMAC_MISCNTOVF_V << EMAC_MISCNTOVF_S)
#define EMAC_MISCNTOVF_V  0x00000001U
#define EMAC_MISCNTOVF_S  16
/** EMAC_OVFFRMCNT : R/W; bitpos: [27:17]; default: 0;
 *  Overflow Frame Counter This field indicates the number of frames missed by the
 *  application This counter is incremented each time the MTL FIFO overflows The
 *  counter is cleared when this register is read with mci_be_i[2] at 1’b1
 */
#define EMAC_OVFFRMCNT    0x000007FFU
#define EMAC_OVFFRMCNT_M  (EMAC_OVFFRMCNT_V << EMAC_OVFFRMCNT_S)
#define EMAC_OVFFRMCNT_V  0x000007FFU
#define EMAC_OVFFRMCNT_S  17
/** EMAC_OVFCNTOVF : R/W; bitpos: [28]; default: 0;
 *  Overflow Bit for FIFO Overflow Counter This bit is set every time the Overflow
 *  Frame Counter _Bits[27:17]_ overflows, that is, the Rx FIFO overflows with the
 *  overflow frame counter at maximum value In such a scenario, the overflow frame
 *  counter is reset to allzeros and this bit indicates that the rollover happened
 */
#define EMAC_OVFCNTOVF    (BIT(28))
#define EMAC_OVFCNTOVF_M  (EMAC_OVFCNTOVF_V << EMAC_OVFCNTOVF_S)
#define EMAC_OVFCNTOVF_V  0x00000001U
#define EMAC_OVFCNTOVF_S  28

/** EMAC_RECEIVEINTERRUPTWATCHDOGTIMER_REG register
 *  Watchdog timeout for Receive Interrupt _RI_ from DMA
 */
#define EMAC_RECEIVEINTERRUPTWATCHDOGTIMER_REG (DR_REG_EMAC_BASE + 0x1024)
/** EMAC_RIWT : R/W; bitpos: [7:0]; default: 0;
 *  RI Watchdog Timer Count This bit indicates the number of system clock cycles
 *  multiplied by 256 for which the watchdog timer is set The watchdog timer gets
 *  triggered with the programmed value after the Rx DMA completes the transfer of a
 *  frame for which the RI status bit is not set because of the setting in the
 *  corresponding descriptor RDES1[31] When the watchdog timer runs out, the RI bit is
 *  set and the timer is stopped The watchdog timer is reset when the RI bit is set
 *  high because of automatic setting of RI as per RDES1[31] of any received frame
 */
#define EMAC_RIWT    0x000000FFU
#define EMAC_RIWT_M  (EMAC_RIWT_V << EMAC_RIWT_S)
#define EMAC_RIWT_V  0x000000FFU
#define EMAC_RIWT_S  0

/** EMAC_AXIBUSMODE_REG register
 *  Controls AXI master behavior _mainly controls burst splitting and number of
 *  outstanding requests_
 */
#define EMAC_AXIBUSMODE_REG (DR_REG_EMAC_BASE + 0x1028)
/** EMAC_UNDEF : RO; bitpos: [0]; default: 1;
 *  AXI Undefined Burst Length This bit is readonly bit and indicates the complement
 *  _invert_ value of Bit 16 _FB_ in Register 0 _Bus Mode Register_ When this bit is
 *  set to 1, the EMACAXI is allowed to perform any burst length equal to or below the
 *  maximum allowed burst length programmed in Bits[7:3] When this bit is set to 0, the
 *  EMACAXI is allowed to perform only fixed burst lengths as indicated by BLEN256,
 *  BLEN128, BLEN64, BLEN32, BLEN16, BLEN8, or BLEN4, or a burst length of 1 If UNDEF
 *  is set and none of the BLEN bits is set, then EMACAXI is allowed to perform a burst
 *  length of 16
 */
#define EMAC_UNDEF    (BIT(0))
#define EMAC_UNDEF_M  (EMAC_UNDEF_V << EMAC_UNDEF_S)
#define EMAC_UNDEF_V  0x00000001U
#define EMAC_UNDEF_S  0
/** EMAC_BLEN4 : R/W; bitpos: [1]; default: 0;
 *  AXI Burst Length 4 When this bit is set to 1, the EMACAXI is allowed to select a
 *  burst length of 4 on the AXI master interface Setting this bit has no effect when
 *  UNDEF is set to 1
 */
#define EMAC_BLEN4    (BIT(1))
#define EMAC_BLEN4_M  (EMAC_BLEN4_V << EMAC_BLEN4_S)
#define EMAC_BLEN4_V  0x00000001U
#define EMAC_BLEN4_S  1
/** EMAC_BLEN8 : R/W; bitpos: [2]; default: 0;
 *  AXI Burst Length 8 When this bit is set to 1, the EMACAXI is allowed to select a
 *  burst length of 8 on the AXI master interface Setting this bit has no effect when
 *  UNDEF is set to 1
 */
#define EMAC_BLEN8    (BIT(2))
#define EMAC_BLEN8_M  (EMAC_BLEN8_V << EMAC_BLEN8_S)
#define EMAC_BLEN8_V  0x00000001U
#define EMAC_BLEN8_S  2
/** EMAC_BLEN16 : R/W; bitpos: [3]; default: 0;
 *  AXI Burst Length 16 When this bit is set to 1 or UNDEF is set to 1, the EMACAXI is
 *  allowed to select a burst length of 16 on the AXI master interface
 */
#define EMAC_BLEN16    (BIT(3))
#define EMAC_BLEN16_M  (EMAC_BLEN16_V << EMAC_BLEN16_S)
#define EMAC_BLEN16_V  0x00000001U
#define EMAC_BLEN16_S  3
/** EMAC_BLEN32 : R/W; bitpos: [4]; default: 0;
 *  AXI Burst Length 32 When this bit is set to 1, the EMACAXI is allowed to select a
 *  burst length of 32 on the AXI master interface This bit is present only when the
 *  configuration parameter AXI_BL is set to 32 or more Otherwise, this bit is reserved
 *  and is readonly _RO_
 */
#define EMAC_BLEN32    (BIT(4))
#define EMAC_BLEN32_M  (EMAC_BLEN32_V << EMAC_BLEN32_S)
#define EMAC_BLEN32_V  0x00000001U
#define EMAC_BLEN32_S  4
/** EMAC_BLEN64 : R/W; bitpos: [5]; default: 0;
 *  AXI Burst Length 64 When this bit is set to 1, the EMACAXI is allowed to select a
 *  burst length of 64 on the AXI master interface This bit is present only when the
 *  configuration parameter AXI_BL is set to 64 or more Otherwise, this bit is reserved
 *  and is readonly _RO_
 */
#define EMAC_BLEN64    (BIT(5))
#define EMAC_BLEN64_M  (EMAC_BLEN64_V << EMAC_BLEN64_S)
#define EMAC_BLEN64_V  0x00000001U
#define EMAC_BLEN64_S  5
/** EMAC_BLEN128 : R/W; bitpos: [6]; default: 0;
 *  AXI Burst Length 128 When this bit is set to 1, the EMACAXI is allowed to select a
 *  burst length of 128 on the AXI master interface This bit is present only when the
 *  configuration parameter AXI_BL is set to 128 or more Otherwise, this bit is
 *  reserved and is readonly _RO_
 */
#define EMAC_BLEN128    (BIT(6))
#define EMAC_BLEN128_M  (EMAC_BLEN128_V << EMAC_BLEN128_S)
#define EMAC_BLEN128_V  0x00000001U
#define EMAC_BLEN128_S  6
/** EMAC_BLEN256 : R/W; bitpos: [7]; default: 0;
 *  AXI Burst Length 256 When this bit is set to 1, the EMACAXI is allowed to select a
 *  burst length of 256 on the AXI master interface This bit is present only when the
 *  configuration parameter AXI_BL is set to 256 Otherwise, this bit is reserved and is
 *  readonly _RO_
 */
#define EMAC_BLEN256    (BIT(7))
#define EMAC_BLEN256_M  (EMAC_BLEN256_V << EMAC_BLEN256_S)
#define EMAC_BLEN256_V  0x00000001U
#define EMAC_BLEN256_S  7
/** EMAC_AXI_AAL : RO; bitpos: [12]; default: 0;
 *  AddressAligned Beats This bit is readonly bit and reflects the Bit 25 _AAL_ of
 *  Register 0 _Bus Mode Register_ When this bit is set to 1, the EMACAXI performs
 *  addressaligned burst transfers on both read and write channels 11:8		Reserved	0H	RO
 */
#define EMAC_AXI_AAL    (BIT(12))
#define EMAC_AXI_AAL_M  (EMAC_AXI_AAL_V << EMAC_AXI_AAL_S)
#define EMAC_AXI_AAL_V  0x00000001U
#define EMAC_AXI_AAL_S  12
/** EMAC_ONEKBBE : R/W; bitpos: [13]; default: 0;
 *  1 KB Boundary Crossing Enable for the EMACAXI Master When set, the EMACAXI master
 *  performs burst transfers that do not cross 1 KB boundary When reset, the EMACAXI
 *  master performs burst transfers that do not cross 4 KB boundary
 */
#define EMAC_ONEKBBE    (BIT(13))
#define EMAC_ONEKBBE_M  (EMAC_ONEKBBE_V << EMAC_ONEKBBE_S)
#define EMAC_ONEKBBE_V  0x00000001U
#define EMAC_ONEKBBE_S  13
/** EMAC_RD_OSR_LMT : R/W; bitpos: [19:16]; default: 1;
 *  AXI Maximum Read Outstanding Request Limit This value limits the maximum
 *  outstanding request on the AXI read interface Maximum outstanding requests =
 *  RD_OSR_LMT+1 Note: Bit 18 is reserved if AXI_GM_MAX_RD_REQUESTS = 4 Bit 19 is
 *  reserved if AXI_GM_MAX_RD_REQUESTS != 16 15:14		Reserved	00	RO
 */
#define EMAC_RD_OSR_LMT    0x0000000FU
#define EMAC_RD_OSR_LMT_M  (EMAC_RD_OSR_LMT_V << EMAC_RD_OSR_LMT_S)
#define EMAC_RD_OSR_LMT_V  0x0000000FU
#define EMAC_RD_OSR_LMT_S  16
/** EMAC_WR_OSR_LMT : R/W; bitpos: [23:20]; default: 1;
 *  AXI Maximum Write Outstanding Request Limit This value limits the maximum
 *  outstanding request on the AXI write interface Maximum outstanding requests =
 *  WR_OSR_LMT+1 Note: Bit 22 is reserved if AXI_GM_MAX_WR_REQUESTS = 4 Bit 23 bit is
 *  reserved if AXI_GM_MAX_WR_REQUESTS != 16
 */
#define EMAC_WR_OSR_LMT    0x0000000FU
#define EMAC_WR_OSR_LMT_M  (EMAC_WR_OSR_LMT_V << EMAC_WR_OSR_LMT_S)
#define EMAC_WR_OSR_LMT_V  0x0000000FU
#define EMAC_WR_OSR_LMT_S  20
/** EMAC_LPI_XIT_FRM : R/W; bitpos: [30]; default: 0;
 *  Unlock on Magic Packet or Remote WakeUp Frame When set to 1, this bit enables the
 *  EMACAXI to come out of the LPI mode only when the magic packet or remote wakeup
 *  frame is received When set to 0, this bit enables the EMACAXI to come out of LPI
 *  mode when any frame is received 29:24		Reserved	00H	RO
 */
#define EMAC_LPI_XIT_FRM    (BIT(30))
#define EMAC_LPI_XIT_FRM_M  (EMAC_LPI_XIT_FRM_V << EMAC_LPI_XIT_FRM_S)
#define EMAC_LPI_XIT_FRM_V  0x00000001U
#define EMAC_LPI_XIT_FRM_S  30
/** EMAC_EN_LPI : R/W; bitpos: [31]; default: 0;
 *  Enable Low Power Interface _LPI_ When set to 1, this bit enables the LPI mode
 *  supported by the EMACAXI configuration and accepts the LPI request from the AXI
 *  System Clock controller When set to 0, this bit disables the LPI mode and always
 *  denies the LPI request from the AXI System Clock controller
 */
#define EMAC_EN_LPI    (BIT(31))
#define EMAC_EN_LPI_M  (EMAC_EN_LPI_V << EMAC_EN_LPI_S)
#define EMAC_EN_LPI_V  0x00000001U
#define EMAC_EN_LPI_S  31

/** EMAC_AHBORAXISTATUS_REG register
 *  Gives the idle status of the AHB master interface in the EMACAHB configuration
 *  Gives the idle status of the AXI master's read or write channel in the EMACAXI
 *  configuration
 */
#define EMAC_AHBORAXISTATUS_REG (DR_REG_EMAC_BASE + 0x102c)
/** EMAC_AXWHSTS : RO; bitpos: [0]; default: 0;
 *  AXI Master Write Channel or AHB Master Status When high, it indicates that AXI
 *  master's write channel is active and transferring data in the EMACAXI configuration
 *  In the EMACAHB configuration, it indicates that the AHB master interface FSMs are
 *  in the nonidle state
 */
#define EMAC_AXWHSTS    (BIT(0))
#define EMAC_AXWHSTS_M  (EMAC_AXWHSTS_V << EMAC_AXWHSTS_S)
#define EMAC_AXWHSTS_V  0x00000001U
#define EMAC_AXWHSTS_S  0
/** EMAC_AXIRDSTS : RO; bitpos: [1]; default: 0;
 *  AXI Master Read Channel Status When high, it indicates that AXI master's read
 *  channel is active and transferring data
 */
#define EMAC_AXIRDSTS    (BIT(1))
#define EMAC_AXIRDSTS_M  (EMAC_AXIRDSTS_V << EMAC_AXIRDSTS_S)
#define EMAC_AXIRDSTS_V  0x00000001U
#define EMAC_AXIRDSTS_S  1

/** EMAC_CURRENTHOSTTRANSMITDESCRIPTOR_REG register
 *  Points to the start of current Transmit Descriptor read by the DMA
 */
#define EMAC_CURRENTHOSTTRANSMITDESCRIPTOR_REG (DR_REG_EMAC_BASE + 0x1048)
/** EMAC_CURTDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Descriptor Address Pointer
 */
#define EMAC_CURTDESAPTR    0xFFFFFFFFU
#define EMAC_CURTDESAPTR_M  (EMAC_CURTDESAPTR_V << EMAC_CURTDESAPTR_S)
#define EMAC_CURTDESAPTR_V  0xFFFFFFFFU
#define EMAC_CURTDESAPTR_S  0

/** EMAC_CURRENTHOSTRECEIVEDESCRIPTOR_REG register
 *  Points to the start of current Receive Descriptor read by the DMA
 */
#define EMAC_CURRENTHOSTRECEIVEDESCRIPTOR_REG (DR_REG_EMAC_BASE + 0x104c)
/** EMAC_CURRDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Descriptor Address Pointer
 */
#define EMAC_CURRDESAPTR    0xFFFFFFFFU
#define EMAC_CURRDESAPTR_M  (EMAC_CURRDESAPTR_V << EMAC_CURRDESAPTR_S)
#define EMAC_CURRDESAPTR_V  0xFFFFFFFFU
#define EMAC_CURRDESAPTR_S  0

/** EMAC_CURRENTHOSTTRANSMITBUFFERADDRESS_REG register
 *  Points to the current Transmit Buffer address read by the DMA
 */
#define EMAC_CURRENTHOSTTRANSMITBUFFERADDRESS_REG (DR_REG_EMAC_BASE + 0x1050)
/** EMAC_CURTBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Buffer Address Pointer
 */
#define EMAC_CURTBUFAPTR    0xFFFFFFFFU
#define EMAC_CURTBUFAPTR_M  (EMAC_CURTBUFAPTR_V << EMAC_CURTBUFAPTR_S)
#define EMAC_CURTBUFAPTR_V  0xFFFFFFFFU
#define EMAC_CURTBUFAPTR_S  0

/** EMAC_CURRENTHOSTRECEIVEBUFFERADDRESS_REG register
 *  Points to the current Receive Buffer address read by the DMA
 */
#define EMAC_CURRENTHOSTRECEIVEBUFFERADDRESS_REG (DR_REG_EMAC_BASE + 0x1054)
/** EMAC_CURRBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Buffer Address Pointer
 */
#define EMAC_CURRBUFAPTR    0xFFFFFFFFU
#define EMAC_CURRBUFAPTR_M  (EMAC_CURRBUFAPTR_V << EMAC_CURRBUFAPTR_S)
#define EMAC_CURRBUFAPTR_V  0xFFFFFFFFU
#define EMAC_CURRBUFAPTR_S  0

/** EMAC_HWFEATURE_REG register
 *  Indicates the presence of the optional features of the core
 */
#define EMAC_HWFEATURE_REG (DR_REG_EMAC_BASE + 0x1058)
/** EMAC_MIISEL : RO; bitpos: [0]; default: 0;
 *  10 or 100 Mbps support
 */
#define EMAC_MIISEL    (BIT(0))
#define EMAC_MIISEL_M  (EMAC_MIISEL_V << EMAC_MIISEL_S)
#define EMAC_MIISEL_V  0x00000001U
#define EMAC_MIISEL_S  0
/** EMAC_GMIISEL : RO; bitpos: [1]; default: 0;
 *  1000 Mbps support
 */
#define EMAC_GMIISEL    (BIT(1))
#define EMAC_GMIISEL_M  (EMAC_GMIISEL_V << EMAC_GMIISEL_S)
#define EMAC_GMIISEL_V  0x00000001U
#define EMAC_GMIISEL_S  1
/** EMAC_HDSEL : RO; bitpos: [2]; default: 0;
 *  Halfduplex support
 */
#define EMAC_HDSEL    (BIT(2))
#define EMAC_HDSEL_M  (EMAC_HDSEL_V << EMAC_HDSEL_S)
#define EMAC_HDSEL_V  0x00000001U
#define EMAC_HDSEL_S  2
/** EMAC_EXTHASHEN : RO; bitpos: [3]; default: 0;
 *  Expanded DA Hash filter
 */
#define EMAC_EXTHASHEN    (BIT(3))
#define EMAC_EXTHASHEN_M  (EMAC_EXTHASHEN_V << EMAC_EXTHASHEN_S)
#define EMAC_EXTHASHEN_V  0x00000001U
#define EMAC_EXTHASHEN_S  3
/** EMAC_HASHSEL : RO; bitpos: [4]; default: 0;
 *  HASH filter
 */
#define EMAC_HASHSEL    (BIT(4))
#define EMAC_HASHSEL_M  (EMAC_HASHSEL_V << EMAC_HASHSEL_S)
#define EMAC_HASHSEL_V  0x00000001U
#define EMAC_HASHSEL_S  4
/** EMAC_ADDMACADRSEL : RO; bitpos: [5]; default: 0;
 *  Multiple MAC Address registers
 */
#define EMAC_ADDMACADRSEL    (BIT(5))
#define EMAC_ADDMACADRSEL_M  (EMAC_ADDMACADRSEL_V << EMAC_ADDMACADRSEL_S)
#define EMAC_ADDMACADRSEL_V  0x00000001U
#define EMAC_ADDMACADRSEL_S  5
/** EMAC_PCSSEL : RO; bitpos: [6]; default: 0;
 *  PCS registers _TBI, SGMII, or RTBI PHY interface_
 */
#define EMAC_PCSSEL    (BIT(6))
#define EMAC_PCSSEL_M  (EMAC_PCSSEL_V << EMAC_PCSSEL_S)
#define EMAC_PCSSEL_V  0x00000001U
#define EMAC_PCSSEL_S  6
/** EMAC_L3L4FLTREN : RO; bitpos: [7]; default: 0;
 *  Layer 3 and Layer 4 feature
 */
#define EMAC_L3L4FLTREN    (BIT(7))
#define EMAC_L3L4FLTREN_M  (EMAC_L3L4FLTREN_V << EMAC_L3L4FLTREN_S)
#define EMAC_L3L4FLTREN_V  0x00000001U
#define EMAC_L3L4FLTREN_S  7
/** EMAC_SMASEL : RO; bitpos: [8]; default: 0;
 *  SMA _MDIO_ Interface
 */
#define EMAC_SMASEL    (BIT(8))
#define EMAC_SMASEL_M  (EMAC_SMASEL_V << EMAC_SMASEL_S)
#define EMAC_SMASEL_V  0x00000001U
#define EMAC_SMASEL_S  8
/** EMAC_RWKSEL : RO; bitpos: [9]; default: 0;
 *  PMT remote wakeup frame
 */
#define EMAC_RWKSEL    (BIT(9))
#define EMAC_RWKSEL_M  (EMAC_RWKSEL_V << EMAC_RWKSEL_S)
#define EMAC_RWKSEL_V  0x00000001U
#define EMAC_RWKSEL_S  9
/** EMAC_MGKSEL : RO; bitpos: [10]; default: 0;
 *  PMT magic packet
 */
#define EMAC_MGKSEL    (BIT(10))
#define EMAC_MGKSEL_M  (EMAC_MGKSEL_V << EMAC_MGKSEL_S)
#define EMAC_MGKSEL_V  0x00000001U
#define EMAC_MGKSEL_S  10
/** EMAC_MMCSEL : RO; bitpos: [11]; default: 0;
 *  RMON module
 */
#define EMAC_MMCSEL    (BIT(11))
#define EMAC_MMCSEL_M  (EMAC_MMCSEL_V << EMAC_MMCSEL_S)
#define EMAC_MMCSEL_V  0x00000001U
#define EMAC_MMCSEL_S  11
/** EMAC_TSVER1SEL : RO; bitpos: [12]; default: 0;
 *  Only IEEE 15882002 timestamp
 */
#define EMAC_TSVER1SEL    (BIT(12))
#define EMAC_TSVER1SEL_M  (EMAC_TSVER1SEL_V << EMAC_TSVER1SEL_S)
#define EMAC_TSVER1SEL_V  0x00000001U
#define EMAC_TSVER1SEL_S  12
/** EMAC_TSVER2SEL : RO; bitpos: [13]; default: 0;
 *  IEEE 15882008 Advanced timestamp
 */
#define EMAC_TSVER2SEL    (BIT(13))
#define EMAC_TSVER2SEL_M  (EMAC_TSVER2SEL_V << EMAC_TSVER2SEL_S)
#define EMAC_TSVER2SEL_V  0x00000001U
#define EMAC_TSVER2SEL_S  13
/** EMAC_EEESEL : RO; bitpos: [14]; default: 0;
 *  Energy Efficient Ethernet
 */
#define EMAC_EEESEL    (BIT(14))
#define EMAC_EEESEL_M  (EMAC_EEESEL_V << EMAC_EEESEL_S)
#define EMAC_EEESEL_V  0x00000001U
#define EMAC_EEESEL_S  14
/** EMAC_AVSEL : RO; bitpos: [15]; default: 0;
 *  AV feature
 */
#define EMAC_AVSEL    (BIT(15))
#define EMAC_AVSEL_M  (EMAC_AVSEL_V << EMAC_AVSEL_S)
#define EMAC_AVSEL_V  0x00000001U
#define EMAC_AVSEL_S  15
/** EMAC_TXCOESEL : RO; bitpos: [16]; default: 0;
 *  Checksum Offload in Tx
 */
#define EMAC_TXCOESEL    (BIT(16))
#define EMAC_TXCOESEL_M  (EMAC_TXCOESEL_V << EMAC_TXCOESEL_S)
#define EMAC_TXCOESEL_V  0x00000001U
#define EMAC_TXCOESEL_S  16
/** EMAC_RXTYP1COE : RO; bitpos: [17]; default: 0;
 *  IP Checksum Offload _Type 1_ in Rx Note: If IPCHKSUM_EN = Enabled and
 *  IPC_FULL_OFFLOAD = Enabled, then RXTYP1COE = 0 and RXTYP2COE = 1
 */
#define EMAC_RXTYP1COE    (BIT(17))
#define EMAC_RXTYP1COE_M  (EMAC_RXTYP1COE_V << EMAC_RXTYP1COE_S)
#define EMAC_RXTYP1COE_V  0x00000001U
#define EMAC_RXTYP1COE_S  17
/** EMAC_RXTYP2COE : RO; bitpos: [18]; default: 0;
 *  IP Checksum Offload _Type 2_ in Rx
 */
#define EMAC_RXTYP2COE    (BIT(18))
#define EMAC_RXTYP2COE_M  (EMAC_RXTYP2COE_V << EMAC_RXTYP2COE_S)
#define EMAC_RXTYP2COE_V  0x00000001U
#define EMAC_RXTYP2COE_S  18
/** EMAC_RXFIFOSIZE : RO; bitpos: [19]; default: 0;
 *  Rx FIFO > 2,048 Bytes
 */
#define EMAC_RXFIFOSIZE    (BIT(19))
#define EMAC_RXFIFOSIZE_M  (EMAC_RXFIFOSIZE_V << EMAC_RXFIFOSIZE_S)
#define EMAC_RXFIFOSIZE_V  0x00000001U
#define EMAC_RXFIFOSIZE_S  19
/** EMAC_RXCHCNT : RO; bitpos: [21:20]; default: 0;
 *  Number of additional Rx Channels
 */
#define EMAC_RXCHCNT    0x00000003U
#define EMAC_RXCHCNT_M  (EMAC_RXCHCNT_V << EMAC_RXCHCNT_S)
#define EMAC_RXCHCNT_V  0x00000003U
#define EMAC_RXCHCNT_S  20
/** EMAC_TXCHCNT : RO; bitpos: [23:22]; default: 0;
 *  Number of additional Tx Channels
 */
#define EMAC_TXCHCNT    0x00000003U
#define EMAC_TXCHCNT_M  (EMAC_TXCHCNT_V << EMAC_TXCHCNT_S)
#define EMAC_TXCHCNT_V  0x00000003U
#define EMAC_TXCHCNT_S  22
/** EMAC_ENHDESSEL : RO; bitpos: [24]; default: 0;
 *  Alternate _Enhanced Descriptor_
 */
#define EMAC_ENHDESSEL    (BIT(24))
#define EMAC_ENHDESSEL_M  (EMAC_ENHDESSEL_V << EMAC_ENHDESSEL_S)
#define EMAC_ENHDESSEL_V  0x00000001U
#define EMAC_ENHDESSEL_S  24
/** EMAC_INTTSEN : RO; bitpos: [25]; default: 0;
 *  Timestamping with Internal System Time
 */
#define EMAC_INTTSEN    (BIT(25))
#define EMAC_INTTSEN_M  (EMAC_INTTSEN_V << EMAC_INTTSEN_S)
#define EMAC_INTTSEN_V  0x00000001U
#define EMAC_INTTSEN_S  25
/** EMAC_FLEXIPPSEN : RO; bitpos: [26]; default: 0;
 *  Flexible PulsePerSecond Output
 */
#define EMAC_FLEXIPPSEN    (BIT(26))
#define EMAC_FLEXIPPSEN_M  (EMAC_FLEXIPPSEN_V << EMAC_FLEXIPPSEN_S)
#define EMAC_FLEXIPPSEN_V  0x00000001U
#define EMAC_FLEXIPPSEN_S  26
/** EMAC_SAVLANINS : RO; bitpos: [27]; default: 0;
 *  Source Address or VLAN Insertion
 */
#define EMAC_SAVLANINS    (BIT(27))
#define EMAC_SAVLANINS_M  (EMAC_SAVLANINS_V << EMAC_SAVLANINS_S)
#define EMAC_SAVLANINS_V  0x00000001U
#define EMAC_SAVLANINS_S  27
/** EMAC_ACTPHYIF : RO; bitpos: [30:28]; default: 0;
 *  Active or selected PHY interface When you have multiple PHY interfaces in your
 *  configuration, this field indicates the sampled value of phy_intf_sel_i during
 *  reset deassertion 000: GMII or MII 001: RGMII 010: SGMII 011: TBI 100: RMII 101:
 *  RTBI 110: SMII 111: RevMII All Others: Reserved
 */
#define EMAC_ACTPHYIF    0x00000007U
#define EMAC_ACTPHYIF_M  (EMAC_ACTPHYIF_V << EMAC_ACTPHYIF_S)
#define EMAC_ACTPHYIF_V  0x00000007U
#define EMAC_ACTPHYIF_S  28

/** EMAC_CHANNEL1BUSMODE_REG register
 *  Controls the Host Interface mode for Channel 1
 */
#define EMAC_CHANNEL1BUSMODE_REG (DR_REG_EMAC_BASE + 0x1100)
/** EMAC_CH1_SWR : R/W1S; bitpos: [0]; default: 1;
 *  Software Reset When this bit is set, the MAC DMA Controller resets the logic and
 *  all internal registers of the MAC It is cleared automatically after the reset
 *  operation is complete in all of the DWC_EMAC clock domains Before reprogramming any
 *  register of the DWC_EMAC, you should read a zero _0_ value in this bit Note: The
 *  Software reset function is driven only by this bit Bit 0 of Register 64 _Channel 1
 *  Bus Mode Register_ or Register 128 _Channel 2 Bus Mode Register_ has no impact on
 *  the Software reset function The reset operation is completed only when all resets
 *  in all active clock domains are deasserted Therefore, it is essential that all PHY
 *  inputs clocks _applicable for the selected PHY interface_ are present for the
 *  software reset completion The time to complete the software reset operation depends
 *  on the frequency of the slowest active clock
 */
#define EMAC_CH1_SWR    (BIT(0))
#define EMAC_CH1_SWR_M  (EMAC_CH1_SWR_V << EMAC_CH1_SWR_S)
#define EMAC_CH1_SWR_V  0x00000001U
#define EMAC_CH1_SWR_S  0
/** EMAC_CH1_DA : R/W; bitpos: [1]; default: 0;
 *  DMA Arbitration Scheme This bit specifies the arbitration scheme between the
 *  transmit and receive paths of Channel 1 0: Weighted roundrobin with Rx:Tx or Tx:Rx
 *  The priority between the paths is according to the priority specified in Bits
 *  [15:14] _PR_ and priority weights specified in Bit 27 _TXPR_ 1: Fixed priority The
 *  transmit path has priority over receive path when Bit 27 _TXPR_ is set Otherwise,
 *  receive path has priority over the transmit path In the EMACAXI configuration,
 *  these bits are reserved and are readonly _RO_ For more information about the
 *  priority scheme between the transmit and receive paths, see Table 412 in “DMA
 *  Arbiter Functions” on page 167
 */
#define EMAC_CH1_DA    (BIT(1))
#define EMAC_CH1_DA_M  (EMAC_CH1_DA_V << EMAC_CH1_DA_S)
#define EMAC_CH1_DA_V  0x00000001U
#define EMAC_CH1_DA_S  1
/** EMAC_CH1_DSL : R/W; bitpos: [6:2]; default: 0;
 *  Descriptor Skip Length This bit specifies the number of Word, Dword, or Lword
 *  _depending on the 32bit, 64bit, or 128bit bus_ to skip between two unchained
 *  descriptors The address skipping starts from the end of current descriptor to the
 *  start of next descriptor When the DSL value is equal to zero, the descriptor table
 *  is taken as contiguous by the DMA in Ring mode
 */
#define EMAC_CH1_DSL    0x0000001FU
#define EMAC_CH1_DSL_M  (EMAC_CH1_DSL_V << EMAC_CH1_DSL_S)
#define EMAC_CH1_DSL_V  0x0000001FU
#define EMAC_CH1_DSL_S  2
/** EMAC_CH1_ATDS : R/W; bitpos: [7]; default: 0;
 *  Alternate Descriptor Size When set, the size of the alternate descriptor _described
 *  in “Alternate or Enhanced Descriptors” on page 545_ increases to 32 bytes _8
 *  DWORDS_ This is required when the Advanced Timestamp feature or the IPC Full
 *  Checksum Offload Engine _Type 2_ is enabled in the receiver The enhanced descriptor
 *  is not required if the Advanced Timestamp and IPC Full Checksum Offload Engine
 *  _Type 2_ features are not enabled In such case, you can use the 16 bytes descriptor
 *  to save 4 bytes of memory This bit is present only when you select the Alternate
 *  Descriptor feature and any one of the following features during core configuration:
 *  Advanced Timestamp feature IPC Full Checksum Offload Engine _Type 2_ feature
 *  Otherwise, this bit is reserved and is readonly When reset, the descriptor size
 *  reverts back to 4 DWORDs _16 bytes_ This bit preserves the backward compatibility
 *  for the descriptor size In versions prior to 350a, the descriptor size is 16 bytes
 *  for both normal and enhanced descriptors In version 350a, descriptor size is
 *  increased to 32 bytes because of the Advanced Timestamp and IPC Full Checksum
 *  Offload Engine _Type 2_ features
 */
#define EMAC_CH1_ATDS    (BIT(7))
#define EMAC_CH1_ATDS_M  (EMAC_CH1_ATDS_V << EMAC_CH1_ATDS_S)
#define EMAC_CH1_ATDS_V  0x00000001U
#define EMAC_CH1_ATDS_S  7
/** EMAC_CH1_PBL : R/W; bitpos: [13:8]; default: 1;
 *  Programmable Burst Length These bits indicate the maximum number of beats to be
 *  transferred in one DMA transaction This is the maximum value that is used in a
 *  single block Read or Write The DMA always attempts to burst as specified in PBL
 *  each time it starts a Burst transfer on the host bus PBL can be programmed with
 *  permissible values of 1, 2, 4, 8, 16, and 32 Any other value results in undefined
 *  behavior When USP is set high, this PBL value is applicable only for Tx DMA
 *  transactions If the number of beats to be transferred is more than 32, then perform
 *  the following steps: 1 Set the PBLx8 mode 2 Set the PBL For example, if the maximum
 *  number of beats to be transferred is 64, then first set PBLx8 to 1 and then set PBL
 *  to 8 The PBL values have the following limitation: The maximum number of possible
 *  beats _PBL_ is limited by the size of the Tx FIFO and Rx FIFO in the MTL layer and
 *  the data bus width on the DMA The FIFO has a constraint that the maximum beat
 *  supported is half the depth of the FIFO, except when specified For different data
 *  bus widths and FIFO sizes, the valid PBL range _including x8 mode_ is provided in
 *  Table 66 on page 382
 */
#define EMAC_CH1_PBL    0x0000003FU
#define EMAC_CH1_PBL_M  (EMAC_CH1_PBL_V << EMAC_CH1_PBL_S)
#define EMAC_CH1_PBL_V  0x0000003FU
#define EMAC_CH1_PBL_S  8
/** EMAC_CH1_PR : R/W; bitpos: [15:14]; default: 0;
 *  Priority Ratio These bits control the priority ratio in the weighted roundrobin
 *  arbitration between the Rx DMA and Tx DMA These bits are valid only when Bit 1 _DA_
 *  is reset The priority ratio is Rx:Tx or Tx:Rx depending on whether Bit 27 _TXPR_ is
 *  reset or set 00: The Priority Ratio is 1:1 01: The Priority Ratio is 2:1 10: The
 *  Priority Ratio is 3:1 11: The Priority Ratio is 4:1 In the EMACAXI configuration,
 *  these bits are reserved and readonly _RO_ For more information about the priority
 *  scheme between the transmit and receive paths, see Table 412 in “DMA Arbiter
 *  Functions” on page 167
 */
#define EMAC_CH1_PR    0x00000003U
#define EMAC_CH1_PR_M  (EMAC_CH1_PR_V << EMAC_CH1_PR_S)
#define EMAC_CH1_PR_V  0x00000003U
#define EMAC_CH1_PR_S  14
/** EMAC_CH1_FB : R/W; bitpos: [16]; default: 0;
 *  Fixed Burst This bit controls whether the AHB or AXI master interface performs
 *  fixed burst transfers or not When set, the AHB interface uses only SINGLE, INCR4,
 *  INCR8, or INCR16 during start of the normal burst transfers When reset, the AHB or
 *  AXI interface uses SINGLE and INCR burst transfer operations For more information,
 *  see Bit 0 _UNDEF_ of the AXI Bus Mode register in the EMACAXI configuration
 */
#define EMAC_CH1_FB    (BIT(16))
#define EMAC_CH1_FB_M  (EMAC_CH1_FB_V << EMAC_CH1_FB_S)
#define EMAC_CH1_FB_V  0x00000001U
#define EMAC_CH1_FB_S  16
/** EMAC_CH1_RPBL : R/W; bitpos: [22:17]; default: 1;
 *  Rx DMA PBL This field indicates the maximum number of beats to be transferred in
 *  one Rx DMA transaction This is the maximum value that is used in a single block
 *  Read or Write The Rx DMA always attempts to burst as specified in the RPBL bit each
 *  time it starts a Burst transfer on the host bus You can program RPBL with values of
 *  1, 2, 4, 8, 16, and 32 Any other value results in undefined behavior This field is
 *  valid and applicable only when USP is set high
 */
#define EMAC_CH1_RPBL    0x0000003FU
#define EMAC_CH1_RPBL_M  (EMAC_CH1_RPBL_V << EMAC_CH1_RPBL_S)
#define EMAC_CH1_RPBL_V  0x0000003FU
#define EMAC_CH1_RPBL_S  17
/** EMAC_CH1_USP : R/W; bitpos: [23]; default: 0;
 *  Use Separate PBL When set high, this bit configures the Rx DMA to use the value
 *  configured in Bits [22:17] as PBL The PBL value in Bits [13:8] is applicable only
 *  to the Tx DMA operations When reset to low, the PBL value in Bits [13:8] is
 *  applicable for both DMA engines
 */
#define EMAC_CH1_USP    (BIT(23))
#define EMAC_CH1_USP_M  (EMAC_CH1_USP_V << EMAC_CH1_USP_S)
#define EMAC_CH1_USP_V  0x00000001U
#define EMAC_CH1_USP_S  23
/** EMAC_CH1_PBLX8 : R/W; bitpos: [24]; default: 0;
 *  PBLx8 Mode When set high, this bit multiplies the programmed PBL value _Bits
 *  [22:17] and Bits[13:8]_ eight times Therefore, the DMA transfers the data in 8, 16,
 *  32, 64, 128, and 256 beats depending on the PBL value Note: This bit function is
 *  not backward compatible Before release 350a, this bit was 4xPBL
 */
#define EMAC_CH1_PBLX8    (BIT(24))
#define EMAC_CH1_PBLX8_M  (EMAC_CH1_PBLX8_V << EMAC_CH1_PBLX8_S)
#define EMAC_CH1_PBLX8_V  0x00000001U
#define EMAC_CH1_PBLX8_S  24
/** EMAC_CH1_AAL : R/W; bitpos: [25]; default: 0;
 *  AddressAligned Beats When this bit is set high and the FB bit is equal to 1, the
 *  AHB or AXI interface generates all bursts aligned to the start address LS bits If
 *  the FB bit is equal to 0, the first burst _accessing the start address of data
 *  buffer_ is not aligned, but subsequent bursts are aligned to the address This bit
 *  is valid only in the EMACAHB and EMACAXI configurations and is reserved _RO with
 *  default value 0_ in all other configurations
 */
#define EMAC_CH1_AAL    (BIT(25))
#define EMAC_CH1_AAL_M  (EMAC_CH1_AAL_V << EMAC_CH1_AAL_S)
#define EMAC_CH1_AAL_V  0x00000001U
#define EMAC_CH1_AAL_S  25
/** EMAC_CH1_MB : R/W; bitpos: [26]; default: 0;
 *  Mixed Burst When this bit is set high and the FB bit is low, the AHB master
 *  interface starts all bursts of length more than 16 with INCR _undefined burst_,
 *  whereas it reverts to fixed burst transfers _INCRx and SINGLE_ for burst length of
 *  16 and less This bit is valid only in the EMACAHB configuration and reserved in all
 *  other configuration
 */
#define EMAC_CH1_MB    (BIT(26))
#define EMAC_CH1_MB_M  (EMAC_CH1_MB_V << EMAC_CH1_MB_S)
#define EMAC_CH1_MB_V  0x00000001U
#define EMAC_CH1_MB_S  26
/** EMAC_CH1_TXPR : R/W; bitpos: [27]; default: 0;
 *  Transmit Priority When set, this bit indicates that the transmit DMA has higher
 *  priority than the receive DMA during arbitration for the systemside bus In the
 *  EMACAXI configuration, this bit is reserved and readonly _RO_ For more information
 *  about the priority scheme between the transmit and receive paths, see Table 412 in
 *  “DMA Arbiter Functions” on page 167
 */
#define EMAC_CH1_TXPR    (BIT(27))
#define EMAC_CH1_TXPR_M  (EMAC_CH1_TXPR_V << EMAC_CH1_TXPR_S)
#define EMAC_CH1_TXPR_V  0x00000001U
#define EMAC_CH1_TXPR_S  27
/** EMAC_CH1_PRWG : R/W; bitpos: [29:28]; default: 0;
 *  Channel Priority Weights This field sets the priority weights for Channel 1 during
 *  the roundrobin arbitration between the DMA channels for the system bus 00: The
 *  priority weight is 1 01: The priority weight is 2 10: The priority weight is 3 11:
 *  The priority weight is 4 This field is present in all DWC_EMAC configurations
 *  except EMACAXI when you select the AV feature Otherwise, this field is reserved and
 *  readonly _RO_ For more information about the priority weights of DMA channels, see
 *  “DMA Arbiter Functions” on page 167
 */
#define EMAC_CH1_PRWG    0x00000003U
#define EMAC_CH1_PRWG_M  (EMAC_CH1_PRWG_V << EMAC_CH1_PRWG_S)
#define EMAC_CH1_PRWG_V  0x00000003U
#define EMAC_CH1_PRWG_S  28
/** EMAC_CH1_RIB : R/W; bitpos: [31]; default: 0;
 *  Rebuild INCRx Burst When this bit is set high and the AHB master gets an EBT
 *  _Retry, Split, or Losing bus grant_, the AHB master interface rebuilds the pending
 *  beats of any burst transfer initiated with INCRx The AHB master interface rebuilds
 *  the beats with a combination of specified bursts with INCRx and SINGLE By default,
 *  the AHB master interface rebuilds pending beats of an EBT with an unspecified
 *  _INCR_ burst This bit is valid only in the EMACAHB configuration It is reserved in
 *  all other configuration
 */
#define EMAC_CH1_RIB    (BIT(31))
#define EMAC_CH1_RIB_M  (EMAC_CH1_RIB_V << EMAC_CH1_RIB_S)
#define EMAC_CH1_RIB_V  0x00000001U
#define EMAC_CH1_RIB_S  31

/** EMAC_CHANNEL1TRANSMITPOLLDEMAND_REG register
 *  Used by the host to instruct the DMA to poll the Transmit Descriptor list
 */
#define EMAC_CHANNEL1TRANSMITPOLLDEMAND_REG (DR_REG_EMAC_BASE + 0x1104)
/** EMAC_CH1_TPD : RO; bitpos: [31:0]; default: 0;
 *  Transmit Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 18 _Current Host Transmit Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  transmission returns to the Suspend state and Bit 2 _TU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the transmission resumes
 */
#define EMAC_CH1_TPD    0xFFFFFFFFU
#define EMAC_CH1_TPD_M  (EMAC_CH1_TPD_V << EMAC_CH1_TPD_S)
#define EMAC_CH1_TPD_V  0xFFFFFFFFU
#define EMAC_CH1_TPD_S  0

/** EMAC_CHANNEL1RECEIVEPOLLDEMAND_REG register
 *  Used by the Host to instruct the DMA to poll the Receive Descriptor list
 */
#define EMAC_CHANNEL1RECEIVEPOLLDEMAND_REG (DR_REG_EMAC_BASE + 0x1108)
/** EMAC_CH1_RPD : RO; bitpos: [31:0]; default: 0;
 *  Receive Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 19 _Current Host Receive Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  reception returns to the Suspended state and Bit 7 _RU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the Rx DMA returns to the
 *  active state
 */
#define EMAC_CH1_RPD    0xFFFFFFFFU
#define EMAC_CH1_RPD_M  (EMAC_CH1_RPD_V << EMAC_CH1_RPD_S)
#define EMAC_CH1_RPD_V  0xFFFFFFFFU
#define EMAC_CH1_RPD_S  0

/** EMAC_CHANNEL1RECEIVEDESCRIPTORLISTADDRESS_REG register
 *  Points the DMA to the start of the Receive Descriptor list
 */
#define EMAC_CHANNEL1RECEIVEDESCRIPTORLISTADDRESS_REG (DR_REG_EMAC_BASE + 0x110c)
/** EMAC_CH1_RDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Receive List This field contains the base address of the first descriptor
 *  in the Receive Descriptor list The LSB bits _1:0, 2:0, or 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and internally taken as allzero by the DMA Therefore,
 *  these LSB bits are readonly _RO_
 */
#define EMAC_CH1_RDESLA    0xFFFFFFFFU
#define EMAC_CH1_RDESLA_M  (EMAC_CH1_RDESLA_V << EMAC_CH1_RDESLA_S)
#define EMAC_CH1_RDESLA_V  0xFFFFFFFFU
#define EMAC_CH1_RDESLA_S  0

/** EMAC_CHANNEL1TRANSMITDESCRIPTORLISTADDRESS_REG register
 *  Points the DMA to the start of the Transmit Descriptor list
 */
#define EMAC_CHANNEL1TRANSMITDESCRIPTORLISTADDRESS_REG (DR_REG_EMAC_BASE + 0x1110)
/** EMAC_CH1_TDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Transmit List This field contains the base address of the first descriptor
 *  in the Transmit Descriptor list The LSB bits _1:0, 2:0, 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and are internally taken as allzero by the DMA
 *  Therefore, these LSB bits are readonly _RO_
 */
#define EMAC_CH1_TDESLA    0xFFFFFFFFU
#define EMAC_CH1_TDESLA_M  (EMAC_CH1_TDESLA_V << EMAC_CH1_TDESLA_S)
#define EMAC_CH1_TDESLA_V  0xFFFFFFFFU
#define EMAC_CH1_TDESLA_S  0

/** EMAC_CHANNEL1STATUS_REG register
 *  The Software driver _application_ reads this register during interrupt service
 *  routine or polling to determine the status of the DMA Bits 29:26 are reserved for
 *  the Channel 1 Status Register
 */
#define EMAC_CHANNEL1STATUS_REG (DR_REG_EMAC_BASE + 0x1114)
/** EMAC_CH1_TI : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt This bit indicates that the frame transmission is complete When
 *  transmission is complete, Bit 31 _OWN_ of TDES0 is reset, and the specific frame
 *  status information is updated in the descriptor
 */
#define EMAC_CH1_TI    (BIT(0))
#define EMAC_CH1_TI_M  (EMAC_CH1_TI_V << EMAC_CH1_TI_S)
#define EMAC_CH1_TI_V  0x00000001U
#define EMAC_CH1_TI_S  0
/** EMAC_CH1_TPS : R/W; bitpos: [1]; default: 0;
 *  Transmit Process Stopped This bit is set when the transmission is stopped
 */
#define EMAC_CH1_TPS    (BIT(1))
#define EMAC_CH1_TPS_M  (EMAC_CH1_TPS_V << EMAC_CH1_TPS_S)
#define EMAC_CH1_TPS_V  0x00000001U
#define EMAC_CH1_TPS_S  1
/** EMAC_CH1_TU : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Transmit List and the DMA cannot acquire it Transmission is
 *  suspended Bits[22:20] explain the Transmit Process state transitions To resume
 *  processing Transmit descriptors, the host should change the ownership of the
 *  descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command
 */
#define EMAC_CH1_TU    (BIT(2))
#define EMAC_CH1_TU_M  (EMAC_CH1_TU_V << EMAC_CH1_TU_S)
#define EMAC_CH1_TU_V  0x00000001U
#define EMAC_CH1_TU_S  2
/** EMAC_CH1_TJT : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout This bit indicates that the Transmit Jabber Timer expired,
 *  which happens when the frame size exceeds 2,048 _10,240 bytes when the Jumbo frame
 *  is enabled_ When the Jabber Timeout occurs, the transmission process is aborted and
 *  placed in the Stopped state This causes the Transmit Jabber Timeout TDES0[14] flag
 *  to assert
 */
#define EMAC_CH1_TJT    (BIT(3))
#define EMAC_CH1_TJT_M  (EMAC_CH1_TJT_V << EMAC_CH1_TJT_S)
#define EMAC_CH1_TJT_V  0x00000001U
#define EMAC_CH1_TJT_S  3
/** EMAC_CH1_OVF : R/W; bitpos: [4]; default: 0;
 *  Receive Overflow This bit indicates that the Receive Buffer had an Overflow during
 *  frame reception If the partial frame is transferred to the application, the
 *  overflow status is set in RDES0[11]
 */
#define EMAC_CH1_OVF    (BIT(4))
#define EMAC_CH1_OVF_M  (EMAC_CH1_OVF_V << EMAC_CH1_OVF_S)
#define EMAC_CH1_OVF_V  0x00000001U
#define EMAC_CH1_OVF_S  4
/** EMAC_CH1_UNF : R/W; bitpos: [5]; default: 0;
 *  Transmit Underflow This bit indicates that the Transmit Buffer had an Underflow
 *  during frame transmission Transmission is suspended and an Underflow Error TDES0[1]
 *  is set
 */
#define EMAC_CH1_UNF    (BIT(5))
#define EMAC_CH1_UNF_M  (EMAC_CH1_UNF_V << EMAC_CH1_UNF_S)
#define EMAC_CH1_UNF_V  0x00000001U
#define EMAC_CH1_UNF_S  5
/** EMAC_CH1_RI : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt This bit indicates that the frame reception is complete When
 *  reception is complete, the Bit 31 of RDES1 _Disable Interrupt on Completion_ is
 *  reset in the last Descriptor, and the specific frame status information is updated
 *  in the descriptor The reception remains in the Running state
 */
#define EMAC_CH1_RI    (BIT(6))
#define EMAC_CH1_RI_M  (EMAC_CH1_RI_V << EMAC_CH1_RI_S)
#define EMAC_CH1_RI_V  0x00000001U
#define EMAC_CH1_RI_S  6
/** EMAC_CH1_RU : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Receive List and the DMA cannot acquire it The Receive Process is
 *  suspended To resume processing Receive descriptors, the host should change the
 *  ownership of the descriptor and issue a Receive Poll Demand command If no Receive
 *  Poll Demand is issued, the Receive Process resumes when the next recognized
 *  incoming frame is received This bit is set only when the previous Receive
 *  Descriptor is owned by the DMA
 */
#define EMAC_CH1_RU    (BIT(7))
#define EMAC_CH1_RU_M  (EMAC_CH1_RU_V << EMAC_CH1_RU_S)
#define EMAC_CH1_RU_V  0x00000001U
#define EMAC_CH1_RU_S  7
/** EMAC_CH1_RPS : R/W; bitpos: [8]; default: 0;
 *  Receive Process Stopped This bit is asserted when the Receive Process enters the
 *  Stopped state
 */
#define EMAC_CH1_RPS    (BIT(8))
#define EMAC_CH1_RPS_M  (EMAC_CH1_RPS_V << EMAC_CH1_RPS_S)
#define EMAC_CH1_RPS_V  0x00000001U
#define EMAC_CH1_RPS_S  8
/** EMAC_CH1_RWT : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout When set, this bit indicates that the Receive Watchdog
 *  Timer expired while receiving the current frame and the current frame is truncated
 *  after the watchdog timeout
 */
#define EMAC_CH1_RWT    (BIT(9))
#define EMAC_CH1_RWT_M  (EMAC_CH1_RWT_V << EMAC_CH1_RWT_S)
#define EMAC_CH1_RWT_V  0x00000001U
#define EMAC_CH1_RWT_S  9
/** EMAC_CH1_ETI : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt This bit indicates that the frame to be transmitted is
 *  fully transferred to the MTL Transmit FIFO
 */
#define EMAC_CH1_ETI    (BIT(10))
#define EMAC_CH1_ETI_M  (EMAC_CH1_ETI_V << EMAC_CH1_ETI_S)
#define EMAC_CH1_ETI_V  0x00000001U
#define EMAC_CH1_ETI_S  10
/** EMAC_CH1_FBI : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Interrupt This bit indicates that a bus error occurred, as
 *  described in Bits [25:23] When this bit is set, the corresponding DMA engine
 *  disables all of its bus accesses 12:11		Reserved	00	RO
 */
#define EMAC_CH1_FBI    (BIT(13))
#define EMAC_CH1_FBI_M  (EMAC_CH1_FBI_V << EMAC_CH1_FBI_S)
#define EMAC_CH1_FBI_V  0x00000001U
#define EMAC_CH1_FBI_S  13
/** EMAC_CH1_ERI : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt This bit indicates that the DMA filled the first data
 *  buffer of the packet This bit is cleared when the software writes 1 to this bit or
 *  Bit 6 _RI_ of this register is set _whichever occurs earlier_
 */
#define EMAC_CH1_ERI    (BIT(14))
#define EMAC_CH1_ERI_M  (EMAC_CH1_ERI_V << EMAC_CH1_ERI_S)
#define EMAC_CH1_ERI_V  0x00000001U
#define EMAC_CH1_ERI_S  14
/** EMAC_CH1_AIS : R/W; bitpos: [15]; default: 0;
 *  Abnormal Interrupt Summary Abnormal Interrupt Summary bit value is the logical OR
 *  of the following when the corresponding interrupt bits are enabled in Register 7
 *  _Interrupt Enable Register_: Register 5[1]: Transmit Process Stopped Register 5[3]:
 *  Transmit Jabber Timeout Register 5[4]: Receive FIFO Overflow Register 5[5]:
 *  Transmit Underflow Register 5[7]: Receive Buffer Unavailable Register 5[8]: Receive
 *  Process Stopped Register 5[9]: Receive Watchdog Timeout Register 5[10]: Early
 *  Transmit Interrupt Register 5[13]: Fatal Bus Error Only unmasked bits affect the
 *  Abnormal Interrupt Summary bit This is a sticky bit and must be cleared _by writing
 *  1 to this bit_ each time a corresponding bit, which causes AIS to be set, is cleared
 */
#define EMAC_CH1_AIS    (BIT(15))
#define EMAC_CH1_AIS_M  (EMAC_CH1_AIS_V << EMAC_CH1_AIS_S)
#define EMAC_CH1_AIS_V  0x00000001U
#define EMAC_CH1_AIS_S  15
/** EMAC_CH1_NIS : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Normal Interrupt Summary bit value is the logical OR of
 *  the following bits when the corresponding interrupt bits are enabled in Register 7
 *  _Interrupt Enable Register_: Register 5[0]: Transmit Interrupt Register 5[2]:
 *  Transmit Buffer Unavailable Register 5[6]: Receive Interrupt Register 5[14]: Early
 *  Receive Interrupt Only unmasked bits _interrupts for which interrupt enable is set
 *  in Register 7_ affect the Normal Interrupt Summary bit This is a sticky bit and
 *  must be cleared _by writing 1 to this bit_ each time a corresponding bit, which
 *  causes NIS to be set, is cleared
 */
#define EMAC_CH1_NIS    (BIT(16))
#define EMAC_CH1_NIS_M  (EMAC_CH1_NIS_V << EMAC_CH1_NIS_S)
#define EMAC_CH1_NIS_V  0x00000001U
#define EMAC_CH1_NIS_S  16
/** EMAC_CH1_RS : RO; bitpos: [19:17]; default: 0;
 *  Receive Process State This field indicates the Receive DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Receive Command
 *  issued 3’b001: Running: Fetching Receive Transfer Descriptor 3’b010: Reserved for
 *  future use 3’b011: Running: Waiting for receive packet 3’b100: Suspended: Receive
 *  Descriptor Unavailable 3’b101: Running: Closing Receive Descriptor 3’b110:
 *  TIME_STAMP write state 3’b111: Running: Transferring the receive packet data from
 *  receive buffer to host memory
 */
#define EMAC_CH1_RS    0x00000007U
#define EMAC_CH1_RS_M  (EMAC_CH1_RS_V << EMAC_CH1_RS_S)
#define EMAC_CH1_RS_V  0x00000007U
#define EMAC_CH1_RS_S  17
/** EMAC_CH1_TS : RO; bitpos: [22:20]; default: 0;
 *  Transmit Process State This field indicates the Transmit DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Transmit Command
 *  issued 3’b001: Running: Fetching Transmit Transfer Descriptor 3’b010: Running:
 *  Waiting for status 3’b011: Running: Reading Data from host memory buffer and
 *  queuing it to transmit buffer _Tx FIFO_ 3’b100: TIME_STAMP write state 3’b101:
 *  Reserved for future use 3’b110: Suspended: Transmit Descriptor Unavailable or
 *  Transmit Buffer Underflow 3’b111: Running: Closing Transmit Descriptor
 */
#define EMAC_CH1_TS    0x00000007U
#define EMAC_CH1_TS_M  (EMAC_CH1_TS_V << EMAC_CH1_TS_S)
#define EMAC_CH1_TS_V  0x00000007U
#define EMAC_CH1_TS_S  20
/** EMAC_CH1_EB : RO; bitpos: [25:23]; default: 0;
 *  Error Bits This field indicates the type of error that caused a Bus Error, for
 *  example, error response on the AHB or AXI interface This field is valid only when
 *  Bit 13 _FBI_ is set This field does not generate an interrupt 0 0 0: Error during
 *  Rx DMA Write Data Transfer 0 1 1: Error during Tx DMA Read Data Transfer 1 0 0:
 *  Error during Rx DMA Descriptor Write Access 1 0 1: Error during Tx DMA Descriptor
 *  Write Access 1 1 0: Error during Rx DMA Descriptor Read Access 1 1 1: Error during
 *  Tx DMA Descriptor Read Access Note: 001 and 010 are reserved
 */
#define EMAC_CH1_EB    0x00000007U
#define EMAC_CH1_EB_M  (EMAC_CH1_EB_V << EMAC_CH1_EB_S)
#define EMAC_CH1_EB_V  0x00000007U
#define EMAC_CH1_EB_S  23
/** EMAC_CH1_GLI : RO; bitpos: [26]; default: 0;
 *  EMAC Line Interface Interrupt When set, this bit reflects any of the following
 *  interrupt events in the DWC_EMAC interfaces _if present and enabled in your
 *  configuration_: PCS _TBI, RTBI, or SGMII_: Link change or autonegotiation complete
 *  event SMII or RGMII: Link change event General Purpose Input Status _GPIS_: Any LL
 *  or LH event on the gpi_i input ports To identify the exact cause of the interrupt,
 *  the software must first read Bit 11 and Bits[2:0] of Register 14 _Interrupt Status
 *  Register_ and then to clear the source of interrupt _which also clears the GLI
 *  interrupt_, read any of the following corresponding registers: PCS _TBI, RTBI, or
 *  SGMII_: Register 49 _AN Status Register_ SMII or RGMII: Register 54
 *  _SGMII/RGMII/SMII Control and Status Register_ General Purpose Input _GPI_:
 *  Register 56 _General Purpose IO Register_ The interrupt signal from the DWC_EMAC
 *  subsystem _sbd_intr_o_ is high when this bit is high
 */
#define EMAC_CH1_GLI    (BIT(26))
#define EMAC_CH1_GLI_M  (EMAC_CH1_GLI_V << EMAC_CH1_GLI_S)
#define EMAC_CH1_GLI_V  0x00000001U
#define EMAC_CH1_GLI_S  26
/** EMAC_CH1_GMI : RO; bitpos: [27]; default: 0;
 *  EMAC MMC Interrupt This bit reflects an interrupt event in the MMC module of the
 *  DWC_EMAC The software must read the corresponding registers in the DWC_EMAC to get
 *  the exact cause of the interrupt and clear the source of interrupt to make this bit
 *  as 1’b0 The interrupt signal from the DWC_EMAC subsystem _sbd_intr_o_ is high when
 *  this bit is high This bit is applicable only when the MAC Management Counters _MMC_
 *  are enabled Otherwise, this bit is reserved
 */
#define EMAC_CH1_GMI    (BIT(27))
#define EMAC_CH1_GMI_M  (EMAC_CH1_GMI_V << EMAC_CH1_GMI_S)
#define EMAC_CH1_GMI_V  0x00000001U
#define EMAC_CH1_GMI_S  27
/** EMAC_CH1_GPI : RO; bitpos: [28]; default: 0;
 *  EMAC PMT Interrupt This bit indicates an interrupt event in the PMT module of the
 *  DWC_EMAC The software must read the PMT Control and Status Register in the MAC to
 *  get the exact cause of interrupt and clear its source to reset this bit to 1’b0 The
 *  interrupt signal from the DWC_EMAC subsystem _sbd_intr_o_ is high when this bit is
 *  high This bit is applicable only when the Power Management feature is enabled
 *  Otherwise, this bit is reserved Note: The GPI and pmt_intr_o interrupts are
 *  generated in different clock domains
 */
#define EMAC_CH1_GPI    (BIT(28))
#define EMAC_CH1_GPI_M  (EMAC_CH1_GPI_V << EMAC_CH1_GPI_S)
#define EMAC_CH1_GPI_V  0x00000001U
#define EMAC_CH1_GPI_S  28
/** EMAC_CH1_TTI : RO; bitpos: [29]; default: 0;
 *  Timestamp Trigger Interrupt This bit indicates an interrupt event in the Timestamp
 *  Generator block of the DWC_EMAC The software must read the corresponding registers
 *  in the DWC_EMAC to get the exact cause of the interrupt and clear its source to
 *  reset this bit to 1'b0 When this bit is high, the interrupt signal from the
 *  DWC_EMAC subsystem _sbd_intr_o_ is high This bit is applicable only when the IEEE
 *  1588 Timestamp feature is enabled Otherwise, this bit is reserved
 */
#define EMAC_CH1_TTI    (BIT(29))
#define EMAC_CH1_TTI_M  (EMAC_CH1_TTI_V << EMAC_CH1_TTI_S)
#define EMAC_CH1_TTI_V  0x00000001U
#define EMAC_CH1_TTI_S  29
/** EMAC_CH1_GLPII_GTMSI : RO; bitpos: [30]; default: 0;
 *  GTMSI: EMAC TMS Interrupt _for Channel 1 and Channel 2_ This bit indicates an
 *  interrupt event in the traffic manager and scheduler logic of DWC_EMAC To reset
 *  this bit, the software must read the corresponding registers _Channel Status
 *  Register_ to get the exact cause of the interrupt and clear its source Note: GTMSI
 *  status is given only in Channel 1 and Channel 2 DMA register when the AV feature is
 *  enabled and corresponding additional transmit channels are present Otherwise, this
 *  bit is reserved When this bit is high, the interrupt signal from the MAC
 *  _sbd_intr_o_ is high
 */
#define EMAC_CH1_GLPII_GTMSI    (BIT(30))
#define EMAC_CH1_GLPII_GTMSI_M  (EMAC_CH1_GLPII_GTMSI_V << EMAC_CH1_GLPII_GTMSI_S)
#define EMAC_CH1_GLPII_GTMSI_V  0x00000001U
#define EMAC_CH1_GLPII_GTMSI_S  30

/** EMAC_CHANNEL1OPERATIONMODE_REG register
 *  Establishes the Receive and Transmit operating modes and command
 */
#define EMAC_CHANNEL1OPERATIONMODE_REG (DR_REG_EMAC_BASE + 0x1118)
/** EMAC_CH1_SR : R/W; bitpos: [1]; default: 0;
 *  Start or Stop Receive When this bit is set, the Receive process is placed in the
 *  Running state The DMA attempts to acquire the descriptor from the Receive list and
 *  processes the incoming frames The descriptor acquisition is attempted from the
 *  current position in the list, which is the address set by the Register 3 _Receive
 *  Descriptor List Address Register_ or the position retained when the Receive process
 *  was previously stopped If the DMA does not own the descriptor, reception is
 *  suspended and Bit 7 _Receive Buffer Unavailable_ of Register 5 _Status Register_ is
 *  set The Start Receive command is effective only when the reception has stopped If
 *  the command is issued before setting Register 3 _Receive Descriptor List Address
 *  Register_, the DMA behavior is unpredictable When this bit is cleared, the Rx DMA
 *  operation is stopped after the transfer of the current frame The next descriptor
 *  position in the Receive list is saved and becomes the current position after the
 *  Receive process is restarted The Stop Receive command is effective only when the
 *  Receive process is in either the Running _waiting for receive packet_ or in the
 *  Suspended state Note: For information about how to pause the transmission, see
 *  “Stopping and Starting Transmission” on page 715
 */
#define EMAC_CH1_SR    (BIT(1))
#define EMAC_CH1_SR_M  (EMAC_CH1_SR_V << EMAC_CH1_SR_S)
#define EMAC_CH1_SR_V  0x00000001U
#define EMAC_CH1_SR_S  1
/** EMAC_CH1_OSF : R/W; bitpos: [2]; default: 0;
 *  Operate on Second Frame When this bit is set, it instructs the DMA to process the
 *  second frame of the Transmit data even before the status for the first frame is
 *  obtained
 */
#define EMAC_CH1_OSF    (BIT(2))
#define EMAC_CH1_OSF_M  (EMAC_CH1_OSF_V << EMAC_CH1_OSF_S)
#define EMAC_CH1_OSF_V  0x00000001U
#define EMAC_CH1_OSF_S  2
/** EMAC_CH1_RTC : R/W; bitpos: [4:3]; default: 0;
 *  Receive Threshold Control These two bits control the threshold level of the MTL
 *  Receive FIFO Transfer _request_ to DMA starts when the frame size within the MTL
 *  Receive FIFO is larger than the threshold In addition, full frames with length less
 *  than the threshold are automatically transferred The value of 11 is not applicable
 *  if the configured Receive FIFO size is 128 bytes These bits are valid only when the
 *  RSF bit is zero, and are ignored when the RSF bit is set to 1 00: 64 01: 32 10: 96
 *  11: 128
 */
#define EMAC_CH1_RTC    0x00000003U
#define EMAC_CH1_RTC_M  (EMAC_CH1_RTC_V << EMAC_CH1_RTC_S)
#define EMAC_CH1_RTC_V  0x00000003U
#define EMAC_CH1_RTC_S  3
/** EMAC_CH1_DGF : R/W; bitpos: [5]; default: 0;
 *  Drop Giant Frames When set, the MAC drops the received giant frames in the Rx FIFO,
 *  that is, frames that are larger than the computed giant frame limit When reset, the
 *  MAC does not drop the giant frames in the Rx FIFO Note: This bit is available in
 *  the following configurations in which the giant frame status is not provided in Rx
 *  status and giant frames are not dropped by default: Configurations in which IP
 *  Checksum Offload _Type 1_ is selected in Rx Configurations in which the IPC Full
 *  Checksum Offload Engine _Type 2_ is selected in Rx with normal descriptor format
 *  Configurations in which the Advanced Timestamp feature is selected In all other
 *  configurations, this bit is not used _reserved and always reset_
 */
#define EMAC_CH1_DGF    (BIT(5))
#define EMAC_CH1_DGF_M  (EMAC_CH1_DGF_V << EMAC_CH1_DGF_S)
#define EMAC_CH1_DGF_V  0x00000001U
#define EMAC_CH1_DGF_S  5
/** EMAC_CH1_FUF : R/W; bitpos: [6]; default: 0;
 *  Forward Undersized Good Frames When set, the Rx FIFO forwards Undersized frames
 *  _that is, frames with no Error and length less than 64 bytes_ including padbytes
 *  and CRC When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a
 *  frame is already transferred because of the lower value of Receive Threshold, for
 *  example, RTC = 01
 */
#define EMAC_CH1_FUF    (BIT(6))
#define EMAC_CH1_FUF_M  (EMAC_CH1_FUF_V << EMAC_CH1_FUF_S)
#define EMAC_CH1_FUF_V  0x00000001U
#define EMAC_CH1_FUF_S  6
/** EMAC_CH1_FEF : R/W; bitpos: [7]; default: 0;
 *  Forward Error Frames When this bit is reset, the Rx FIFO drops frames with error
 *  status _CRC error, collision error, GMII_ER, giant frame, watchdog timeout, or
 *  overflow_ However, if the start byte _write_ pointer of a frame is already
 *  transferred to the read controller side _in Threshold mode_, then the frame is not
 *  dropped In the EMACMTL configuration in which the Frame Length FIFO is also enabled
 *  during core configuration, the Rx FIFO drops the error frames if that frame's start
 *  byte is not transferred _output_ on the ARI bus When the FEF bit is set, all frames
 *  except runt error frames are forwarded to the DMA If the Bit 25 _RSF_ is set and
 *  the Rx FIFO overflows when a partial frame is written, then the frame is dropped
 *  irrespective of the FEF bit setting However, if the Bit 25 _RSF_ is reset and the
 *  Rx FIFO overflows when a partial frame is written, then a partial frame may be
 *  forwarded to the DMA Note: When FEF bit is reset, the giant frames are dropped if
 *  the giant frame status is given in Rx Status _in Table 86 or Table 823_ in the
 *  following configurations: The IP checksum engine _Type 1_ and full checksum offload
 *  engine _Type 2_ are not selected The advanced timestamp feature is not selected but
 *  the extended status is selected The extended status is available with the following
 *  features: 	L3L4 filter in EMACCORE or EMACMTL configurations 	Full checksum offload
 *  engine _Type 2_ with enhanced descriptor format in the EMACDMA, EMACAHB, or EMACAXI
 *  configurations
 */
#define EMAC_CH1_FEF    (BIT(7))
#define EMAC_CH1_FEF_M  (EMAC_CH1_FEF_V << EMAC_CH1_FEF_S)
#define EMAC_CH1_FEF_V  0x00000001U
#define EMAC_CH1_FEF_S  7
/** EMAC_CH1_EFC : R/W; bitpos: [8]; default: 0;
 *  Enable HW Flow Control When this bit is set, the flow control signal operation
 *  based on the filllevel of Rx FIFO is enabled When reset, the flow control operation
 *  is disabled This bit is not used _reserved and always reset_ when the Rx FIFO is
 *  less than 4 KB
 */
#define EMAC_CH1_EFC    (BIT(8))
#define EMAC_CH1_EFC_M  (EMAC_CH1_EFC_V << EMAC_CH1_EFC_S)
#define EMAC_CH1_EFC_V  0x00000001U
#define EMAC_CH1_EFC_S  8
/** EMAC_CH1_RFA : R/W; bitpos: [10:9]; default: 0;
 *  Threshold for Activating Flow Control _in halfduplex and fullduplex modes_ These
 *  bits control the threshold _Fill level of Rx FIFO_ at which the flow control is
 *  activated 00: Full minus 1 KB, that is, FULL—1KB 01: Full minus 2 KB, that is,
 *  FULL—2KB 10: Full minus 3 KB, that is, FULL—3KB 11: Full minus 4 KB, that is,
 *  FULL—4KB These values are applicable only to Rx FIFOs of 4 KB or more and when Bit
 *  8 _EFC_ is set high If the Rx FIFO is 8 KB or more, an additional Bit _RFA_2_ is
 *  used for more threshold levels as described in Bit 23 These bits are reserved and
 *  readonly when the depth of Rx FIFO is less than 4 KB Note: When FIFO size is
 *  exactly 4 KB, although the DWC_EMAC allows you to program the value of these bits
 *  to 11, the software should not program these bits to 2'b11 The value 2'b11 means
 *  flow control on FIFO empty condition
 */
#define EMAC_CH1_RFA    0x00000003U
#define EMAC_CH1_RFA_M  (EMAC_CH1_RFA_V << EMAC_CH1_RFA_S)
#define EMAC_CH1_RFA_V  0x00000003U
#define EMAC_CH1_RFA_S  9
/** EMAC_CH1_RFD : R/W; bitpos: [12:11]; default: 0;
 *  Threshold for Deactivating Flow Control _in halfduplex and fullduplex modes_ These
 *  bits control the threshold _Filllevel of Rx FIFO_ at which the flow control is
 *  deasserted after activation 00: Full minus 1 KB, that is, FULL — 1 KB 01: Full
 *  minus 2 KB, that is, FULL — 2 KB 10: Full minus 3 KB, that is, FULL — 3 KB 11: Full
 *  minus 4 KB, that is, FULL — 4 KB The deassertion is effective only after flow
 *  control is asserted If the Rx FIFO is 8 KB or more, an additional Bit _RFD_2_ is
 *  used for more threshold levels as described in Bit 22 These bits are reserved and
 *  readonly when the Rx FIFO depth is less than 4 KB Note: For proper flow control,
 *  the value programmed in the “RFD_2, RFD” fields should be equal to or more than the
 *  value programmed in the “RFA_2, RFA” fields
 */
#define EMAC_CH1_RFD    0x00000003U
#define EMAC_CH1_RFD_M  (EMAC_CH1_RFD_V << EMAC_CH1_RFD_S)
#define EMAC_CH1_RFD_V  0x00000003U
#define EMAC_CH1_RFD_S  11
/** EMAC_CH1_ST : R/W; bitpos: [13]; default: 0;
 *  Start or Stop Transmission Command When this bit is set, transmission is placed in
 *  the Running state, and the DMA checks the Transmit List at the current position for
 *  a frame to be transmitted Descriptor acquisition is attempted either from the
 *  current position in the list, which is the Transmit List Base Address set by
 *  Register 4 _Transmit Descriptor List Address Register_, or from the position
 *  retained when transmission was stopped previously If the DMA does not own the
 *  current descriptor, transmission enters the Suspended state and Bit 2 _Transmit
 *  Buffer Unavailable_ of Register 5 _Status Register_ is set The Start Transmission
 *  command is effective only when transmission is stopped If the command is issued
 *  before setting Register 4 _Transmit Descriptor List Address Register_, then the DMA
 *  behavior is unpredictable When this bit is reset, the transmission process is
 *  placed in the Stopped state after completing the transmission of the current frame
 *  The Next Descriptor position in the Transmit List is saved, and it becomes the
 *  current position when transmission is restarted To change the list address, you
 *  need to program Register 4 _Transmit Descriptor List Address Register_ with a new
 *  value when this bit is reset The new value is considered when this bit is set again
 *  The stop transmission command is effective only when the transmission of the
 *  current frame is complete or the transmission is in the Suspended state Note: For
 *  information about how to pause the transmission, see “Stopping and Starting
 *  Transmission” on page 715
 */
#define EMAC_CH1_ST    (BIT(13))
#define EMAC_CH1_ST_M  (EMAC_CH1_ST_V << EMAC_CH1_ST_S)
#define EMAC_CH1_ST_V  0x00000001U
#define EMAC_CH1_ST_S  13
/** EMAC_CH1_TTC : R/W; bitpos: [16:14]; default: 0;
 *  Transmit Threshold Control These bits control the threshold level of the MTL
 *  Transmit FIFO Transmission starts when the frame size within the MTL Transmit FIFO
 *  is larger than the threshold In addition, full frames with a length less than the
 *  threshold are also transmitted These bits are used only when Bit 21 _TSF_ is reset
 *  000: 64 001: 128 010: 192 011: 256 100: 40 101: 32 110: 24 111: 16
 */
#define EMAC_CH1_TTC    0x00000007U
#define EMAC_CH1_TTC_M  (EMAC_CH1_TTC_V << EMAC_CH1_TTC_S)
#define EMAC_CH1_TTC_V  0x00000007U
#define EMAC_CH1_TTC_S  14
/** EMAC_CH1_FTF : R/W1S; bitpos: [20]; default: 0;
 *  Flush Transmit FIFO When this bit is set, the transmit FIFO controller logic is
 *  reset to its default values and thus all data in the Tx FIFO is lost or flushed
 *  This bit is cleared internally when the flushing operation is complete The
 *  Operation Mode register should not be written to until this bit is cleared The data
 *  which is already accepted by the MAC transmitter is not flushed It is scheduled for
 *  transmission and results in underflow and runt frame transmission Note: The flush
 *  operation is complete only when the Tx FIFO is emptied of its contents and all the
 *  pending Transmit Status of the transmitted frames are accepted by the host In order
 *  to complete this flush operation, the PHY transmit clock _clk_tx_i_ is required to
 *  be active 19:17		Reserved 	000	RO
 */
#define EMAC_CH1_FTF    (BIT(20))
#define EMAC_CH1_FTF_M  (EMAC_CH1_FTF_V << EMAC_CH1_FTF_S)
#define EMAC_CH1_FTF_V  0x00000001U
#define EMAC_CH1_FTF_S  20
/** EMAC_CH1_TSF : R/W; bitpos: [21]; default: 0;
 *  Transmit Store and Forward When this bit is set, transmission starts when a full
 *  frame resides in the MTL Transmit FIFO When this bit is set, the TTC values
 *  specified in Bits [16:14] are ignored This bit should be changed only when the
 *  transmission is stopped
 */
#define EMAC_CH1_TSF    (BIT(21))
#define EMAC_CH1_TSF_M  (EMAC_CH1_TSF_V << EMAC_CH1_TSF_S)
#define EMAC_CH1_TSF_V  0x00000001U
#define EMAC_CH1_TSF_S  21
/** EMAC_CH1_RFD_2 : R/W; bitpos: [22]; default: 0;
 *  MSB of Threshold for Deactivating Flow Control If the DWC_EMAC is configured for Rx
 *  FIFO size of 8 KB or more, this bit _when set_ provides additional threshold levels
 *  for deactivating the flow control in both halfduplex and fullduplex modes This bit
 *  _as Most Significant Bit_ along with the RFD _Bits [12:11]_ gives the following
 *  thresholds for deactivating flow control: 100: Full minus 5 KB, that is, FULL — 5
 *  KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that is, FULL —
 *  7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB or less deep
 */
#define EMAC_CH1_RFD_2    (BIT(22))
#define EMAC_CH1_RFD_2_M  (EMAC_CH1_RFD_2_V << EMAC_CH1_RFD_2_S)
#define EMAC_CH1_RFD_2_V  0x00000001U
#define EMAC_CH1_RFD_2_S  22
/** EMAC_CH1_RFA_2 : R/W; bitpos: [23]; default: 0;
 *  MSB of Threshold for Activating Flow Control If the DWC_EMAC is configured for an
 *  Rx FIFO size of 8 KB or more, this bit _when set_ provides additional threshold
 *  levels for activating the flow control in both half duplex and fullduplex modes
 *  This bit _as Most Significant Bit_, along with the RFA _Bits [10:9]_, gives the
 *  following thresholds for activating flow control: 100: Full minus 5 KB, that is,
 *  FULL — 5 KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that
 *  is, FULL — 7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB
 *  or less deep
 */
#define EMAC_CH1_RFA_2    (BIT(23))
#define EMAC_CH1_RFA_2_M  (EMAC_CH1_RFA_2_V << EMAC_CH1_RFA_2_S)
#define EMAC_CH1_RFA_2_V  0x00000001U
#define EMAC_CH1_RFA_2_S  23
/** EMAC_CH1_DFF : R/W; bitpos: [24]; default: 0;
 *  Disable Flushing of Received Frames When this bit is set, the Rx DMA does not flush
 *  any frames because of the unavailability of receive descriptors or buffers as it
 *  does normally when this bit is reset _See “Receive Process Suspended” on page 83_
 *  This bit is reserved _and RO_ in the EMACMTL configuration
 */
#define EMAC_CH1_DFF    (BIT(24))
#define EMAC_CH1_DFF_M  (EMAC_CH1_DFF_V << EMAC_CH1_DFF_S)
#define EMAC_CH1_DFF_V  0x00000001U
#define EMAC_CH1_DFF_S  24
/** EMAC_CH1_RSF : R/W; bitpos: [25]; default: 0;
 *  Receive Store and Forward When this bit is set, the MTL reads a frame from the Rx
 *  FIFO only after the complete frame has been written to it, ignoring the RTC bits
 *  When this bit is reset, the Rx FIFO operates in the cutthrough mode, subject to the
 *  threshold specified by the RTC bits
 */
#define EMAC_CH1_RSF    (BIT(25))
#define EMAC_CH1_RSF_M  (EMAC_CH1_RSF_V << EMAC_CH1_RSF_S)
#define EMAC_CH1_RSF_V  0x00000001U
#define EMAC_CH1_RSF_S  25
/** EMAC_CH1_DT : R/W; bitpos: [26]; default: 0;
 *  Disable Dropping of TCP/IP Checksum Error Frames When this bit is set, the MAC does
 *  not drop the frames which only have errors detected by the Receive Checksum Offload
 *  engine Such frames do not have any errors _including FCS error_ in the Ethernet
 *  frame received by the MAC but have errors only in the encapsulated payload When
 *  this bit is reset, all error frames are dropped if the FEF bit is reset If the IPC
 *  Full Checksum Offload Engine _Type 2_ is disabled, this bit is reserved _RO with
 *  value 1'b0_
 */
#define EMAC_CH1_DT    (BIT(26))
#define EMAC_CH1_DT_M  (EMAC_CH1_DT_V << EMAC_CH1_DT_S)
#define EMAC_CH1_DT_V  0x00000001U
#define EMAC_CH1_DT_S  26

/** EMAC_CHANNEL1INTERRUPTENABLE_REG register
 *  Enables the interrupts reported by the Status Register
 */
#define EMAC_CHANNEL1INTERRUPTENABLE_REG (DR_REG_EMAC_BASE + 0x111c)
/** EMAC_CH1_TIE : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Transmit Interrupt is enabled When this bit is reset, the Transmit
 *  Interrupt is disabled The sbd_intr_o interrupt is generated as shown in Figure 61
 *  It is asserted only when the TTI, GPI, GMI, GLI, or GLPII bit of the DMA Status
 *  register is asserted, or when the NIS or AIS Status bit is asserted and the
 *  corresponding Interrupt Enable bits _NIE or AIE_ are enabled
 */
#define EMAC_CH1_TIE    (BIT(0))
#define EMAC_CH1_TIE_M  (EMAC_CH1_TIE_V << EMAC_CH1_TIE_S)
#define EMAC_CH1_TIE_V  0x00000001U
#define EMAC_CH1_TIE_S  0
/** EMAC_CH1_TSE : R/W; bitpos: [1]; default: 0;
 *  Transmit Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Transmission Stopped Interrupt is enabled When this bit is reset, the
 *  Transmission Stopped Interrupt is disabled
 */
#define EMAC_CH1_TSE    (BIT(1))
#define EMAC_CH1_TSE_M  (EMAC_CH1_TSE_V << EMAC_CH1_TSE_S)
#define EMAC_CH1_TSE_V  0x00000001U
#define EMAC_CH1_TSE_S  1
/** EMAC_CH1_TUE : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable Enable When this bit is set with Normal Interrupt
 *  Summary Enable _Bit 16_, the Transmit Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled
 */
#define EMAC_CH1_TUE    (BIT(2))
#define EMAC_CH1_TUE_M  (EMAC_CH1_TUE_V << EMAC_CH1_TUE_S)
#define EMAC_CH1_TUE_V  0x00000001U
#define EMAC_CH1_TUE_S  2
/** EMAC_CH1_TJE : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Jabber Timeout Interrupt is enabled When this bit is
 *  reset, the Transmit Jabber Timeout Interrupt is disabled
 */
#define EMAC_CH1_TJE    (BIT(3))
#define EMAC_CH1_TJE_M  (EMAC_CH1_TJE_V << EMAC_CH1_TJE_S)
#define EMAC_CH1_TJE_V  0x00000001U
#define EMAC_CH1_TJE_S  3
/** EMAC_CH1_OVE : R/W; bitpos: [4]; default: 0;
 *  Overflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Receive Overflow Interrupt is enabled When this bit is reset,
 *  the Overflow Interrupt is disabled
 */
#define EMAC_CH1_OVE    (BIT(4))
#define EMAC_CH1_OVE_M  (EMAC_CH1_OVE_V << EMAC_CH1_OVE_S)
#define EMAC_CH1_OVE_V  0x00000001U
#define EMAC_CH1_OVE_S  4
/** EMAC_CH1_UNE : R/W; bitpos: [5]; default: 0;
 *  Underflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Underflow Interrupt is enabled When this bit is
 *  reset, the Underflow Interrupt is disabled
 */
#define EMAC_CH1_UNE    (BIT(5))
#define EMAC_CH1_UNE_M  (EMAC_CH1_UNE_V << EMAC_CH1_UNE_S)
#define EMAC_CH1_UNE_V  0x00000001U
#define EMAC_CH1_UNE_S  5
/** EMAC_CH1_RIE : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Receive Interrupt is enabled When this bit is reset, the Receive
 *  Interrupt is disabled
 */
#define EMAC_CH1_RIE    (BIT(6))
#define EMAC_CH1_RIE_M  (EMAC_CH1_RIE_V << EMAC_CH1_RIE_S)
#define EMAC_CH1_RIE_V  0x00000001U
#define EMAC_CH1_RIE_S  6
/** EMAC_CH1_RUE : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Receive Buffer Unavailable Interrupt is disabled
 */
#define EMAC_CH1_RUE    (BIT(7))
#define EMAC_CH1_RUE_M  (EMAC_CH1_RUE_V << EMAC_CH1_RUE_S)
#define EMAC_CH1_RUE_V  0x00000001U
#define EMAC_CH1_RUE_S  7
/** EMAC_CH1_RSE : R/W; bitpos: [8]; default: 0;
 *  Receive Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Receive Stopped Interrupt is enabled When this bit is reset, the
 *  Receive Stopped Interrupt is disabled
 */
#define EMAC_CH1_RSE    (BIT(8))
#define EMAC_CH1_RSE_M  (EMAC_CH1_RSE_V << EMAC_CH1_RSE_S)
#define EMAC_CH1_RSE_V  0x00000001U
#define EMAC_CH1_RSE_S  8
/** EMAC_CH1_RWE : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Watchdog Timeout Interrupt is enabled When
 *  this bit is reset, the Receive Watchdog Timeout Interrupt is disabled
 */
#define EMAC_CH1_RWE    (BIT(9))
#define EMAC_CH1_RWE_M  (EMAC_CH1_RWE_V << EMAC_CH1_RWE_S)
#define EMAC_CH1_RWE_V  0x00000001U
#define EMAC_CH1_RWE_S  9
/** EMAC_CH1_ETE : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt Enable When this bit is set with an Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Early Transmit Interrupt is enabled When this bit is
 *  reset, the Early Transmit Interrupt is disabled
 */
#define EMAC_CH1_ETE    (BIT(10))
#define EMAC_CH1_ETE_M  (EMAC_CH1_ETE_V << EMAC_CH1_ETE_S)
#define EMAC_CH1_ETE_V  0x00000001U
#define EMAC_CH1_ETE_S  10
/** EMAC_CH1_FBE : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Fatal Bus Error Interrupt is enabled When this bit is reset, the
 *  Fatal Bus Error Enable Interrupt is disabled 12:11		Reserved	00	RO
 */
#define EMAC_CH1_FBE    (BIT(13))
#define EMAC_CH1_FBE_M  (EMAC_CH1_FBE_V << EMAC_CH1_FBE_S)
#define EMAC_CH1_FBE_V  0x00000001U
#define EMAC_CH1_FBE_S  13
/** EMAC_CH1_ERE : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt Enable When this bit is set with Normal Interrupt Summary
 *  Enable _Bit 16_, the Early Receive Interrupt is enabled When this bit is reset, the
 *  Early Receive Interrupt is disabled
 */
#define EMAC_CH1_ERE    (BIT(14))
#define EMAC_CH1_ERE_M  (EMAC_CH1_ERE_V << EMAC_CH1_ERE_S)
#define EMAC_CH1_ERE_V  0x00000001U
#define EMAC_CH1_ERE_S  14
/** EMAC_CH1_AIE : R/W; bitpos: [15]; default: 0;
 *  Abnormal Interrupt Summary Enable When this bit is set, abnormal interrupt summary
 *  is enabled When this bit is reset, the abnormal interrupt summary is disabled This
 *  bit enables the following interrupts in Register 5 _Status Register_: Register
 *  5[1]: Transmit Process Stopped Register 5[3]: Transmit Jabber Timeout Register
 *  5[4]: Receive Overflow Register 5[5]: Transmit Underflow Register 5[7]: Receive
 *  Buffer Unavailable Register 5[8]: Receive Process Stopped Register 5[9]: Receive
 *  Watchdog Timeout Register 5[10]: Early Transmit Interrupt Register 5[13]: Fatal Bus
 *  Error
 */
#define EMAC_CH1_AIE    (BIT(15))
#define EMAC_CH1_AIE_M  (EMAC_CH1_AIE_V << EMAC_CH1_AIE_S)
#define EMAC_CH1_AIE_V  0x00000001U
#define EMAC_CH1_AIE_S  15
/** EMAC_CH1_NIE : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Enable When this bit is set, normal interrupt summary is
 *  enabled When this bit is reset, normal interrupt summary is disabled This bit
 *  enables the following interrupts in Register 5 _Status Register_: Register 5[0]:
 *  Transmit Interrupt Register 5[2]: Transmit Buffer Unavailable Register 5[6]:
 *  Receive Interrupt Register 5[14]: Early Receive Interrupt
 */
#define EMAC_CH1_NIE    (BIT(16))
#define EMAC_CH1_NIE_M  (EMAC_CH1_NIE_V << EMAC_CH1_NIE_S)
#define EMAC_CH1_NIE_V  0x00000001U
#define EMAC_CH1_NIE_S  16

/** EMAC_CHANNEL1MISSEDFRAMEANDBUFFEROVERFLOWCOUNTER_REG register
 *  Contains the counters for discarded frames because no host Receive Descriptor was
 *  available, and discarded frames because of Receive FIFO Overflow
 */
#define EMAC_CHANNEL1MISSEDFRAMEANDBUFFEROVERFLOWCOUNTER_REG (DR_REG_EMAC_BASE + 0x1120)
/** EMAC_CH1_MISFRMCNT : R/W; bitpos: [15:0]; default: 0;
 *  Missed Frame Counter This field indicates the number of frames missed by the
 *  controller because of the Host Receive Buffer being unavailable This counter is
 *  incremented each time the DMA discards an incoming frame The counter is cleared
 *  when this register is read with mci_be_i[0] at 1’b1
 */
#define EMAC_CH1_MISFRMCNT    0x0000FFFFU
#define EMAC_CH1_MISFRMCNT_M  (EMAC_CH1_MISFRMCNT_V << EMAC_CH1_MISFRMCNT_S)
#define EMAC_CH1_MISFRMCNT_V  0x0000FFFFU
#define EMAC_CH1_MISFRMCNT_S  0
/** EMAC_CH1_MISCNTOVF : R/W; bitpos: [16]; default: 0;
 *  Overflow Bit for Missed Frame Counter This bit is set every time Missed Frame
 *  Counter _Bits[15:0]_ overflows, that is, the DMA discards an incoming frame because
 *  of the Host Receive Buffer being unavailable with the missed frame counter at
 *  maximum value In such a scenario, the Missed frame counter is reset to allzeros and
 *  this bit indicates that the rollover happened
 */
#define EMAC_CH1_MISCNTOVF    (BIT(16))
#define EMAC_CH1_MISCNTOVF_M  (EMAC_CH1_MISCNTOVF_V << EMAC_CH1_MISCNTOVF_S)
#define EMAC_CH1_MISCNTOVF_V  0x00000001U
#define EMAC_CH1_MISCNTOVF_S  16
/** EMAC_CH1_OVFFRMCNT : R/W; bitpos: [27:17]; default: 0;
 *  Overflow Frame Counter This field indicates the number of frames missed by the
 *  application This counter is incremented each time the MTL FIFO overflows The
 *  counter is cleared when this register is read with mci_be_i[2] at 1’b1
 */
#define EMAC_CH1_OVFFRMCNT    0x000007FFU
#define EMAC_CH1_OVFFRMCNT_M  (EMAC_CH1_OVFFRMCNT_V << EMAC_CH1_OVFFRMCNT_S)
#define EMAC_CH1_OVFFRMCNT_V  0x000007FFU
#define EMAC_CH1_OVFFRMCNT_S  17
/** EMAC_CH1_OVFCNTOVF : R/W; bitpos: [28]; default: 0;
 *  Overflow Bit for FIFO Overflow Counter This bit is set every time the Overflow
 *  Frame Counter _Bits[27:17]_ overflows, that is, the Rx FIFO overflows with the
 *  overflow frame counter at maximum value In such a scenario, the overflow frame
 *  counter is reset to allzeros and this bit indicates that the rollover happened
 */
#define EMAC_CH1_OVFCNTOVF    (BIT(28))
#define EMAC_CH1_OVFCNTOVF_M  (EMAC_CH1_OVFCNTOVF_V << EMAC_CH1_OVFCNTOVF_S)
#define EMAC_CH1_OVFCNTOVF_V  0x00000001U
#define EMAC_CH1_OVFCNTOVF_S  28

/** EMAC_CHANNEL1RECEIVEINTERRUPTWATCHDOGTIMER_REG register
 *  Watchdog timeout for Receive Interrupt _RI_ from DMA
 */
#define EMAC_CHANNEL1RECEIVEINTERRUPTWATCHDOGTIMER_REG (DR_REG_EMAC_BASE + 0x1124)
/** EMAC_CH1_RIWT : R/W; bitpos: [7:0]; default: 0;
 *  RI Watchdog Timer Count This bit indicates the number of system clock cycles
 *  multiplied by 256 for which the watchdog timer is set The watchdog timer gets
 *  triggered with the programmed value after the Rx DMA completes the transfer of a
 *  frame for which the RI status bit is not set because of the setting in the
 *  corresponding descriptor RDES1[31] When the watchdog timer runs out, the RI bit is
 *  set and the timer is stopped The watchdog timer is reset when the RI bit is set
 *  high because of automatic setting of RI as per RDES1[31] of any received frame
 */
#define EMAC_CH1_RIWT    0x000000FFU
#define EMAC_CH1_RIWT_M  (EMAC_CH1_RIWT_V << EMAC_CH1_RIWT_S)
#define EMAC_CH1_RIWT_V  0x000000FFU
#define EMAC_CH1_RIWT_S  0

/** EMAC_CHANNEL1SLOTFUNCTIONCONTROLANDSTATUS_REG register
 *  Contains the control bits for slot function and its status for Channel 1 transmit
 *  path
 */
#define EMAC_CHANNEL1SLOTFUNCTIONCONTROLANDSTATUS_REG (DR_REG_EMAC_BASE + 0x1130)
/** EMAC_ESC : R/W; bitpos: [0]; default: 0;
 *  Enable Slot Comparison When set, this bit enables the checking of the slot numbers,
 *  programmed in the transmit descriptor, with the current reference given in Bits
 *  [19:16] The DMA fetches the data from the corresponding buffer only when the slot
 *  number is equal to the reference slot number or is ahead of the reference slot
 *  number by one slot When reset, this bit disables the checking of the slot numbers
 *  The DMA fetches the data immediately after the descriptor is processed
 */
#define EMAC_ESC    (BIT(0))
#define EMAC_ESC_M  (EMAC_ESC_V << EMAC_ESC_S)
#define EMAC_ESC_V  0x00000001U
#define EMAC_ESC_S  0
/** EMAC_ASC : R/W; bitpos: [1]; default: 0;
 *  Advance Slot Check When set, this bit enables the DMA to fetch the data from the
 *  buffer when the slot number _SLOTNUM_ programmed in the transmit descriptor is:
 *  equal to the reference slot number given in Bits [19:16] or ahead of the reference
 *  slot number by up to two slots This bit is applicable only when Bit 0 _ESC_ is set
 */
#define EMAC_ASC    (BIT(1))
#define EMAC_ASC_M  (EMAC_ASC_V << EMAC_ASC_S)
#define EMAC_ASC_V  0x00000001U
#define EMAC_ASC_S  1
/** EMAC_RSN : RO; bitpos: [19:16]; default: 0;
 *  Reference Slot Number This field gives the current value of the reference slot
 *  number in DMA used for comparison checking
 */
#define EMAC_RSN    0x0000000FU
#define EMAC_RSN_M  (EMAC_RSN_V << EMAC_RSN_S)
#define EMAC_RSN_V  0x0000000FU
#define EMAC_RSN_S  16

/** EMAC_CHANNEL1CURRENTHOSTTRANSMITDESCRIPTOR_REG register
 *  Points to the start of current Transmit Descriptor read by the DMA
 */
#define EMAC_CHANNEL1CURRENTHOSTTRANSMITDESCRIPTOR_REG (DR_REG_EMAC_BASE + 0x1148)
/** EMAC_CH1_CURTDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Descriptor Address Pointer
 */
#define EMAC_CH1_CURTDESAPTR    0xFFFFFFFFU
#define EMAC_CH1_CURTDESAPTR_M  (EMAC_CH1_CURTDESAPTR_V << EMAC_CH1_CURTDESAPTR_S)
#define EMAC_CH1_CURTDESAPTR_V  0xFFFFFFFFU
#define EMAC_CH1_CURTDESAPTR_S  0

/** EMAC_CHANNEL1CURRENTHOSTRECEIVEDESCRIPTOR_REG register
 *  Points to the start of current Receive Descriptor read by the DMA
 */
#define EMAC_CHANNEL1CURRENTHOSTRECEIVEDESCRIPTOR_REG (DR_REG_EMAC_BASE + 0x114c)
/** EMAC_CH1_CURRDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Descriptor Address Pointer
 */
#define EMAC_CH1_CURRDESAPTR    0xFFFFFFFFU
#define EMAC_CH1_CURRDESAPTR_M  (EMAC_CH1_CURRDESAPTR_V << EMAC_CH1_CURRDESAPTR_S)
#define EMAC_CH1_CURRDESAPTR_V  0xFFFFFFFFU
#define EMAC_CH1_CURRDESAPTR_S  0

/** EMAC_CHANNEL1CURRENTHOSTTRANSMITBUFFERADDRESS_REG register
 *  Points to the current Transmit Buffer address read by the DMA
 */
#define EMAC_CHANNEL1CURRENTHOSTTRANSMITBUFFERADDRESS_REG (DR_REG_EMAC_BASE + 0x1150)
/** EMAC_CH1_CURTBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Buffer Address Pointer
 */
#define EMAC_CH1_CURTBUFAPTR    0xFFFFFFFFU
#define EMAC_CH1_CURTBUFAPTR_M  (EMAC_CH1_CURTBUFAPTR_V << EMAC_CH1_CURTBUFAPTR_S)
#define EMAC_CH1_CURTBUFAPTR_V  0xFFFFFFFFU
#define EMAC_CH1_CURTBUFAPTR_S  0

/** EMAC_CHANNEL1CURRENTHOSTRECEIVEBUFFERADDRESS_REG register
 *  Points to the current Receive Buffer address read by the DMA
 */
#define EMAC_CHANNEL1CURRENTHOSTRECEIVEBUFFERADDRESS_REG (DR_REG_EMAC_BASE + 0x1154)
/** EMAC_CH1_CURRBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Buffer Address Pointer
 */
#define EMAC_CH1_CURRBUFAPTR    0xFFFFFFFFU
#define EMAC_CH1_CURRBUFAPTR_M  (EMAC_CH1_CURRBUFAPTR_V << EMAC_CH1_CURRBUFAPTR_S)
#define EMAC_CH1_CURRBUFAPTR_V  0xFFFFFFFFU
#define EMAC_CH1_CURRBUFAPTR_S  0

/** EMAC_CHANNEL1CBSCONTROL_REG register
 *  Controls the Channel 1 credit shaping operation on the transmit path
 */
#define EMAC_CHANNEL1CBSCONTROL_REG (DR_REG_EMAC_BASE + 0x1160)
/** EMAC_CH1_CBSD : R/W; bitpos: [0]; default: 0;
 *  CreditBased Shaper Disable When set, the MAC disables the creditbased shaper
 *  algorithm for Channel 1 traffic and makes the traffic management algorithm to
 *  strict priority for Channel 1 over Channel 0 When reset, the creditbased shaper
 *  algorithm schedules the traffic in Channel 1 for transmission
 */
#define EMAC_CH1_CBSD    (BIT(0))
#define EMAC_CH1_CBSD_M  (EMAC_CH1_CBSD_V << EMAC_CH1_CBSD_S)
#define EMAC_CH1_CBSD_V  0x00000001U
#define EMAC_CH1_CBSD_S  0
/** EMAC_CH1_CC : R/W; bitpos: [1]; default: 0;
 *  Credit Control When reset, the accumulated credit parameter in the creditbased
 *  shaper algorithm logic is set to zero when there is positive credit and no frame to
 *  transmit in Channel 1 When there is no frame waiting in Channel 1 and other channel
 *  is transmitting, no credit is accumulated When set, the accumulated credit
 *  parameter in the creditbased shaper algorithm logic is not reset to zero when there
 *  is positive credit and no frame to transmit in Channel 1 The credit accumulates
 *  even when there is no frame waiting in Channel 1 and another channel is transmitting
 */
#define EMAC_CH1_CC    (BIT(1))
#define EMAC_CH1_CC_M  (EMAC_CH1_CC_V << EMAC_CH1_CC_S)
#define EMAC_CH1_CC_V  0x00000001U
#define EMAC_CH1_CC_S  1
/** EMAC_CH1_SLC : R/W; bitpos: [6:4]; default: 0;
 *  Slot Count The software can program the number of slots _of duration 125 microsec_
 *  over which the average transmitted bits per slot _provided in the CBS Status
 *  register_ need to be computed for Channel 1 when the creditbased shaper algorithm
 *  is enabled The encoding is as follows: 3'b000: 1 Slot 3'b001: 2 Slots 3'b010: 4
 *  Slots 3'b011: 8 Slots 3'b100: 16 Slots 3'b1013'b111: Reserved
 */
#define EMAC_CH1_SLC    0x00000007U
#define EMAC_CH1_SLC_M  (EMAC_CH1_SLC_V << EMAC_CH1_SLC_S)
#define EMAC_CH1_SLC_V  0x00000007U
#define EMAC_CH1_SLC_S  4
/** EMAC_CH1_ABPSSIE : R/W; bitpos: [17]; default: 0;
 *  Average Bits Per Slot Interrupt Enable When this bit is set, the MAC asserts an
 *  interrupt _sbd_intr_o or mci_intr_o_ when the average bits per slot status is
 *  updated _Bit 17 _ABSU_ in Register 89_ for Channel 1 When this bit is cleared,
 *  interrupt is not asserted for such an event
 */
#define EMAC_CH1_ABPSSIE    (BIT(17))
#define EMAC_CH1_ABPSSIE_M  (EMAC_CH1_ABPSSIE_V << EMAC_CH1_ABPSSIE_S)
#define EMAC_CH1_ABPSSIE_V  0x00000001U
#define EMAC_CH1_ABPSSIE_S  17

/** EMAC_CHANNEL1CBSSTATUS_REG register
 *  Provides the average traffic transmitted in Channel 1
 */
#define EMAC_CHANNEL1CBSSTATUS_REG (DR_REG_EMAC_BASE + 0x1164)
/** EMAC_CH1_ABS : RO; bitpos: [16:0]; default: 0;
 *  Average Bits per Slot This field contains the average transmitted bits per slot
 *  This field is computed over programmed number of slots _SLC bits in the CBS Control
 *  Register_ for Channel 1 traffic The maximum value is 0x30D4 for 100 Mbps and
 *  0x1E848 for 1000 Mbps
 */
#define EMAC_CH1_ABS    0x0001FFFFU
#define EMAC_CH1_ABS_M  (EMAC_CH1_ABS_V << EMAC_CH1_ABS_S)
#define EMAC_CH1_ABS_V  0x0001FFFFU
#define EMAC_CH1_ABS_S  0
/** EMAC_CH1_ABSU : RO; bitpos: [17]; default: 0;
 *  ABS Updated When set, this bit indicates that the MAC has updated the ABS value
 *  This bit is cleared when the application reads the ABS value
 */
#define EMAC_CH1_ABSU    (BIT(17))
#define EMAC_CH1_ABSU_M  (EMAC_CH1_ABSU_V << EMAC_CH1_ABSU_S)
#define EMAC_CH1_ABSU_V  0x00000001U
#define EMAC_CH1_ABSU_S  17

/** EMAC_CHANNEL1IDLESLOPECREDIT_REG register
 *  Contains the idleSlope credit value required for the creditbased shaper algorithm
 *  for Channel 1
 */
#define EMAC_CHANNEL1IDLESLOPECREDIT_REG (DR_REG_EMAC_BASE + 0x1168)
/** EMAC_CH1_ISC : R/W; bitpos: [13:0]; default: 0;
 *  idleSlopeCredit This field contains the idleSlopeCredit value required for the
 *  creditbased shaper algorithm for Channel 1 This is the rate of change of credit in
 *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
 *  credit is increasing The software should program this field with computed credit in
 *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
 *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode
 */
#define EMAC_CH1_ISC    0x00003FFFU
#define EMAC_CH1_ISC_M  (EMAC_CH1_ISC_V << EMAC_CH1_ISC_S)
#define EMAC_CH1_ISC_V  0x00003FFFU
#define EMAC_CH1_ISC_S  0

/** EMAC_CHANNEL1SENDSLOPECREDIT_REG register
 *  Contains the sendSlope credit value required for the creditbased shaper algorithm
 *  for Channel 1
 */
#define EMAC_CHANNEL1SENDSLOPECREDIT_REG (DR_REG_EMAC_BASE + 0x116c)
/** EMAC_CH1_SSC : R/W; bitpos: [13:0]; default: 0;
 *  sendSlopeCredit This field contains the sendSlopeCredit value required for
 *  creditbased shaper algorithm for Channel 1 This is the rate of change of credit in
 *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
 *  credit is decreasing The software should program this field with computed credit in
 *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
 *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode This field should be
 *  programmed with absolute sendSlopeCredit value The creditbased shaper logic
 *  subtracts it from the accumulated credit when Channel 1 is selected for transmission
 */
#define EMAC_CH1_SSC    0x00003FFFU
#define EMAC_CH1_SSC_M  (EMAC_CH1_SSC_V << EMAC_CH1_SSC_S)
#define EMAC_CH1_SSC_V  0x00003FFFU
#define EMAC_CH1_SSC_S  0

/** EMAC_CHANNEL1HICREDIT_REG register
 *  Contains the hiCredit value required for the creditbased shaper algorithm for
 *  Channel 1
 */
#define EMAC_CHANNEL1HICREDIT_REG (DR_REG_EMAC_BASE + 0x1170)
/** EMAC_CH1_HC : R/W; bitpos: [28:0]; default: 0;
 *  hiCredit This field contains the hiCredit value required for the creditbased shaper
 *  algorithm for Channel 1 This is the maximum value that can be accumulated in the
 *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
 *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
 *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
 *  0x0800_0000
 */
#define EMAC_CH1_HC    0x1FFFFFFFU
#define EMAC_CH1_HC_M  (EMAC_CH1_HC_V << EMAC_CH1_HC_S)
#define EMAC_CH1_HC_V  0x1FFFFFFFU
#define EMAC_CH1_HC_S  0

/** EMAC_CHANNEL1LOCREDIT_REG register
 *  Contains the loCredit value required for the creditbased shaper algorithm for
 *  Channel 1
 */
#define EMAC_CHANNEL1LOCREDIT_REG (DR_REG_EMAC_BASE + 0x1174)
/** EMAC_CH1_LC : R/W; bitpos: [28:0]; default: 536870911;
 *  loCredit This field contains the loCredit value required for the creditbased shaper
 *  algorithm for Channel 1 This is the minimum value that can be accumulated in the
 *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
 *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
 *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
 *  0x0800_0000 The programmed value is 2's complement _negative number_, that is,
 *  0xF800_0000
 */
#define EMAC_CH1_LC    0x1FFFFFFFU
#define EMAC_CH1_LC_M  (EMAC_CH1_LC_V << EMAC_CH1_LC_S)
#define EMAC_CH1_LC_V  0x1FFFFFFFU
#define EMAC_CH1_LC_S  0

/** EMAC_CHANNEL2BUSMODE_REG register
 *  Controls the Host Interface mode for Channel 2
 */
#define EMAC_CHANNEL2BUSMODE_REG (DR_REG_EMAC_BASE + 0x1200)
/** EMAC_CH2_SWR : R/W; bitpos: [0]; default: 1;
 *  Software Reset When this bit is set, the MAC DMA Controller resets the logic and
 *  all internal registers of the MAC It is cleared automatically after the reset
 *  operation is complete in all of the DWC_EMAC clock domains Before reprogramming any
 *  register of the DWC_EMAC, you should read a zero _0_ value in this bit Note: The
 *  Software reset function is driven only by this bit Bit 0 of Register 64 _Channel 1
 *  Bus Mode Register_ or Register 128 _Channel 2 Bus Mode Register_ has no impact on
 *  the Software reset function The reset operation is completed only when all resets
 *  in all active clock domains are deasserted Therefore, it is essential that all PHY
 *  inputs clocks _applicable for the selected PHY interface_ are present for the
 *  software reset completion The time to complete the software reset operation depends
 *  on the frequency of the slowest active clock
 */
#define EMAC_CH2_SWR    (BIT(0))
#define EMAC_CH2_SWR_M  (EMAC_CH2_SWR_V << EMAC_CH2_SWR_S)
#define EMAC_CH2_SWR_V  0x00000001U
#define EMAC_CH2_SWR_S  0
/** EMAC_CH2_DA : R/W; bitpos: [1]; default: 0;
 *  DMA Arbitration Scheme This bit specifies the arbitration scheme between the
 *  transmit and receive paths of Channel 2 0: Weighted roundrobin with Rx:Tx or Tx:Rx
 *  The priority between the paths is according to the priority specified in Bits
 *  [15:14] _PR_ and priority weights specified in Bit 27 _TXPR_ 1: Fixed priority The
 *  transmit path has priority over receive path when Bit 27 _TXPR_ is set Otherwise,
 *  receive path has priority over the transmit path In the EMACAXI configuration,
 *  these bits are reserved and are readonly _RO_ For more information about the
 *  priority scheme between the transmit and receive paths, see Table 412 in “DMA
 *  Arbiter Functions” on page 167
 */
#define EMAC_CH2_DA    (BIT(1))
#define EMAC_CH2_DA_M  (EMAC_CH2_DA_V << EMAC_CH2_DA_S)
#define EMAC_CH2_DA_V  0x00000001U
#define EMAC_CH2_DA_S  1
/** EMAC_CH2_DSL : R/W; bitpos: [6:2]; default: 0;
 *  Descriptor Skip Length This bit specifies the number of Word, Dword, or Lword
 *  _depending on the 32bit, 64bit, or 128bit bus_ to skip between two unchained
 *  descriptors The address skipping starts from the end of current descriptor to the
 *  start of next descriptor When the DSL value is equal to zero, the descriptor table
 *  is taken as contiguous by the DMA in Ring mode
 */
#define EMAC_CH2_DSL    0x0000001FU
#define EMAC_CH2_DSL_M  (EMAC_CH2_DSL_V << EMAC_CH2_DSL_S)
#define EMAC_CH2_DSL_V  0x0000001FU
#define EMAC_CH2_DSL_S  2
/** EMAC_CH2_ATDS : R/W; bitpos: [7]; default: 0;
 *  Alternate Descriptor Size When set, the size of the alternate descriptor _described
 *  in “Alternate or Enhanced Descriptors” on page 545_ increases to 32 bytes _8
 *  DWORDS_ This is required when the Advanced Timestamp feature or the IPC Full
 *  Checksum Offload Engine _Type 2_ is enabled in the receiver The enhanced descriptor
 *  is not required if the Advanced Timestamp and IPC Full Checksum Offload Engine
 *  _Type 2_ features are not enabled In such case, you can use the 16 bytes descriptor
 *  to save 4 bytes of memory This bit is present only when you select the Alternate
 *  Descriptor feature and any one of the following features during core configuration:
 *  Advanced Timestamp feature IPC Full Checksum Offload Engine _Type 2_ feature
 *  Otherwise, this bit is reserved and is readonly When reset, the descriptor size
 *  reverts back to 4 DWORDs _16 bytes_ This bit preserves the backward compatibility
 *  for the descriptor size In versions prior to 350a, the descriptor size is 16 bytes
 *  for both normal and enhanced descriptors In version 350a, descriptor size is
 *  increased to 32 bytes because of the Advanced Timestamp and IPC Full Checksum
 *  Offload Engine _Type 2_ features
 */
#define EMAC_CH2_ATDS    (BIT(7))
#define EMAC_CH2_ATDS_M  (EMAC_CH2_ATDS_V << EMAC_CH2_ATDS_S)
#define EMAC_CH2_ATDS_V  0x00000001U
#define EMAC_CH2_ATDS_S  7
/** EMAC_CH2_PBL : R/W; bitpos: [13:8]; default: 1;
 *  Programmable Burst Length These bits indicate the maximum number of beats to be
 *  transferred in one DMA transaction This is the maximum value that is used in a
 *  single block Read or Write The DMA always attempts to burst as specified in PBL
 *  each time it starts a Burst transfer on the host bus PBL can be programmed with
 *  permissible values of 1, 2, 4, 8, 16, and 32 Any other value results in undefined
 *  behavior When USP is set high, this PBL value is applicable only for Tx DMA
 *  transactions If the number of beats to be transferred is more than 32, then perform
 *  the following steps: 1 Set the PBLx8 mode 2 Set the PBL For example, if the maximum
 *  number of beats to be transferred is 64, then first set PBLx8 to 1 and then set PBL
 *  to 8 The PBL values have the following limitation: The maximum number of possible
 *  beats _PBL_ is limited by the size of the Tx FIFO and Rx FIFO in the MTL layer and
 *  the data bus width on the DMA The FIFO has a constraint that the maximum beat
 *  supported is half the depth of the FIFO, except when specified For different data
 *  bus widths and FIFO sizes, the valid PBL range _including x8 mode_ is provided in
 *  Table 66 on page 382
 */
#define EMAC_CH2_PBL    0x0000003FU
#define EMAC_CH2_PBL_M  (EMAC_CH2_PBL_V << EMAC_CH2_PBL_S)
#define EMAC_CH2_PBL_V  0x0000003FU
#define EMAC_CH2_PBL_S  8
/** EMAC_CH2_PR : R/W; bitpos: [15:14]; default: 0;
 *  Priority Ratio These bits control the priority ratio in the weighted roundrobin
 *  arbitration between the Rx DMA and Tx DMA These bits are valid only when Bit 1 _DA_
 *  is reset The priority ratio is Rx:Tx or Tx:Rx depending on whether Bit 27 _TXPR_ is
 *  reset or set 00: The Priority Ratio is 1:1 01: The Priority Ratio is 2:1 10: The
 *  Priority Ratio is 3:1 11: The Priority Ratio is 4:1 In the EMACAXI configuration,
 *  these bits are reserved and readonly _RO_ For more information about the priority
 *  scheme between the transmit and receive paths, see Table 412 in “DMA Arbiter
 *  Functions” on page 167
 */
#define EMAC_CH2_PR    0x00000003U
#define EMAC_CH2_PR_M  (EMAC_CH2_PR_V << EMAC_CH2_PR_S)
#define EMAC_CH2_PR_V  0x00000003U
#define EMAC_CH2_PR_S  14
/** EMAC_CH2_FB : R/W; bitpos: [16]; default: 0;
 *  Fixed Burst This bit controls whether the AHB or AXI master interface performs
 *  fixed burst transfers or not When set, the AHB interface uses only SINGLE, INCR4,
 *  INCR8, or INCR16 during start of the normal burst transfers When reset, the AHB or
 *  AXI interface uses SINGLE and INCR burst transfer operations For more information,
 *  see Bit 0 _UNDEF_ of the AXI Bus Mode register in the EMACAXI configuration
 */
#define EMAC_CH2_FB    (BIT(16))
#define EMAC_CH2_FB_M  (EMAC_CH2_FB_V << EMAC_CH2_FB_S)
#define EMAC_CH2_FB_V  0x00000001U
#define EMAC_CH2_FB_S  16
/** EMAC_CH2_RPBL : R/W; bitpos: [22:17]; default: 1;
 *  Rx DMA PBL This field indicates the maximum number of beats to be transferred in
 *  one Rx DMA transaction This is the maximum value that is used in a single block
 *  Read or Write The Rx DMA always attempts to burst as specified in the RPBL bit each
 *  time it starts a Burst transfer on the host bus You can program RPBL with values of
 *  1, 2, 4, 8, 16, and 32 Any other value results in undefined behavior This field is
 *  valid and applicable only when USP is set high
 */
#define EMAC_CH2_RPBL    0x0000003FU
#define EMAC_CH2_RPBL_M  (EMAC_CH2_RPBL_V << EMAC_CH2_RPBL_S)
#define EMAC_CH2_RPBL_V  0x0000003FU
#define EMAC_CH2_RPBL_S  17
/** EMAC_CH2_USP : R/W; bitpos: [23]; default: 0;
 *  Use Separate PBL When set high, this bit configures the Rx DMA to use the value
 *  configured in Bits [22:17] as PBL The PBL value in Bits [13:8] is applicable only
 *  to the Tx DMA operations When reset to low, the PBL value in Bits [13:8] is
 *  applicable for both DMA engines
 */
#define EMAC_CH2_USP    (BIT(23))
#define EMAC_CH2_USP_M  (EMAC_CH2_USP_V << EMAC_CH2_USP_S)
#define EMAC_CH2_USP_V  0x00000001U
#define EMAC_CH2_USP_S  23
/** EMAC_CH2_PBLX8 : R/W; bitpos: [24]; default: 0;
 *  PBLx8 Mode When set high, this bit multiplies the programmed PBL value _Bits
 *  [22:17] and Bits[13:8]_ eight times Therefore, the DMA transfers the data in 8, 16,
 *  32, 64, 128, and 256 beats depending on the PBL value Note: This bit function is
 *  not backward compatible Before release 350a, this bit was 4xPBL
 */
#define EMAC_CH2_PBLX8    (BIT(24))
#define EMAC_CH2_PBLX8_M  (EMAC_CH2_PBLX8_V << EMAC_CH2_PBLX8_S)
#define EMAC_CH2_PBLX8_V  0x00000001U
#define EMAC_CH2_PBLX8_S  24
/** EMAC_CH2_AAL : R/W; bitpos: [25]; default: 0;
 *  AddressAligned Beats When this bit is set high and the FB bit is equal to 1, the
 *  AHB or AXI interface generates all bursts aligned to the start address LS bits If
 *  the FB bit is equal to 0, the first burst _accessing the start address of data
 *  buffer_ is not aligned, but subsequent bursts are aligned to the address This bit
 *  is valid only in the EMACAHB and EMACAXI configurations and is reserved _RO with
 *  default value 0_ in all other configurations
 */
#define EMAC_CH2_AAL    (BIT(25))
#define EMAC_CH2_AAL_M  (EMAC_CH2_AAL_V << EMAC_CH2_AAL_S)
#define EMAC_CH2_AAL_V  0x00000001U
#define EMAC_CH2_AAL_S  25
/** EMAC_CH2_MB : R/W; bitpos: [26]; default: 0;
 *  Mixed Burst When this bit is set high and the FB bit is low, the AHB master
 *  interface starts all bursts of length more than 16 with INCR _undefined burst_,
 *  whereas it reverts to fixed burst transfers _INCRx and SINGLE_ for burst length of
 *  16 and less This bit is valid only in the EMACAHB configuration and reserved in all
 *  other configuration
 */
#define EMAC_CH2_MB    (BIT(26))
#define EMAC_CH2_MB_M  (EMAC_CH2_MB_V << EMAC_CH2_MB_S)
#define EMAC_CH2_MB_V  0x00000001U
#define EMAC_CH2_MB_S  26
/** EMAC_CH2_TXPR : R/W; bitpos: [27]; default: 0;
 *  Transmit Priority When set, this bit indicates that the transmit DMA has higher
 *  priority than the receive DMA during arbitration for the systemside bus In the
 *  EMACAXI configuration, this bit is reserved and readonly _RO_ For more information
 *  about the priority scheme between the transmit and receive paths, see Table 412 in
 *  “DMA Arbiter Functions” on page 167
 */
#define EMAC_CH2_TXPR    (BIT(27))
#define EMAC_CH2_TXPR_M  (EMAC_CH2_TXPR_V << EMAC_CH2_TXPR_S)
#define EMAC_CH2_TXPR_V  0x00000001U
#define EMAC_CH2_TXPR_S  27
/** EMAC_CH2_PRWG : R/W; bitpos: [29:28]; default: 0;
 *  Channel Priority Weights This field sets the priority weights for Channel 2 during
 *  the roundrobin arbitration between the DMA channels for the system bus 00: The
 *  priority weight is 1 01: The priority weight is 2 10: The priority weight is 3 11:
 *  The priority weight is 4 This field is present in all DWC_EMAC configurations
 *  except EMACAXI when you select the AV feature Otherwise, this field is reserved and
 *  readonly _RO_ For more information about the priority weights of DMA channels, see
 *  “DMA Arbiter Functions” on page 167
 */
#define EMAC_CH2_PRWG    0x00000003U
#define EMAC_CH2_PRWG_M  (EMAC_CH2_PRWG_V << EMAC_CH2_PRWG_S)
#define EMAC_CH2_PRWG_V  0x00000003U
#define EMAC_CH2_PRWG_S  28
/** EMAC_CH2_RIB : R/W; bitpos: [31]; default: 0;
 *  Rebuild INCRx Burst When this bit is set high and the AHB master gets an EBT
 *  _Retry, Split, or Losing bus grant_, the AHB master interface rebuilds the pending
 *  beats of any burst transfer initiated with INCRx The AHB master interface rebuilds
 *  the beats with a combination of specified bursts with INCRx and SINGLE By default,
 *  the AHB master interface rebuilds pending beats of an EBT with an unspecified
 *  _INCR_ burst This bit is valid only in the EMACAHB configuration It is reserved in
 *  all other configuration
 */
#define EMAC_CH2_RIB    (BIT(31))
#define EMAC_CH2_RIB_M  (EMAC_CH2_RIB_V << EMAC_CH2_RIB_S)
#define EMAC_CH2_RIB_V  0x00000001U
#define EMAC_CH2_RIB_S  31

/** EMAC_CHANNEL2TRANSMITPOLLDEMAND_REG register
 *  Used by the host to instruct the DMA to poll the Transmit Descriptor list
 */
#define EMAC_CHANNEL2TRANSMITPOLLDEMAND_REG (DR_REG_EMAC_BASE + 0x1204)
/** EMAC_CH2_TPD : RO; bitpos: [31:0]; default: 0;
 *  Transmit Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 18 _Current Host Transmit Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  transmission returns to the Suspend state and Bit 2 _TU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the transmission resumes
 */
#define EMAC_CH2_TPD    0xFFFFFFFFU
#define EMAC_CH2_TPD_M  (EMAC_CH2_TPD_V << EMAC_CH2_TPD_S)
#define EMAC_CH2_TPD_V  0xFFFFFFFFU
#define EMAC_CH2_TPD_S  0

/** EMAC_CHANNEL2RECEIVEPOLLDEMAND_REG register
 *  Used by the Host to instruct the DMA to poll the Receive Descriptor list
 */
#define EMAC_CHANNEL2RECEIVEPOLLDEMAND_REG (DR_REG_EMAC_BASE + 0x1208)
/** EMAC_CH2_RPD : RO; bitpos: [31:0]; default: 0;
 *  Receive Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 19 _Current Host Receive Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  reception returns to the Suspended state and Bit 7 _RU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the Rx DMA returns to the
 *  active state
 */
#define EMAC_CH2_RPD    0xFFFFFFFFU
#define EMAC_CH2_RPD_M  (EMAC_CH2_RPD_V << EMAC_CH2_RPD_S)
#define EMAC_CH2_RPD_V  0xFFFFFFFFU
#define EMAC_CH2_RPD_S  0

/** EMAC_CHANNEL2RECEIVEDESCRIPTORLISTADDRESS_REG register
 *  Points the DMA to the start of the Receive Descriptor list
 */
#define EMAC_CHANNEL2RECEIVEDESCRIPTORLISTADDRESS_REG (DR_REG_EMAC_BASE + 0x120c)
/** EMAC_CH2_RDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Receive List This field contains the base address of the first descriptor
 *  in the Receive Descriptor list The LSB bits _1:0, 2:0, or 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and internally taken as allzero by the DMA Therefore,
 *  these LSB bits are readonly _RO_
 */
#define EMAC_CH2_RDESLA    0xFFFFFFFFU
#define EMAC_CH2_RDESLA_M  (EMAC_CH2_RDESLA_V << EMAC_CH2_RDESLA_S)
#define EMAC_CH2_RDESLA_V  0xFFFFFFFFU
#define EMAC_CH2_RDESLA_S  0

/** EMAC_CHANNEL2TRANSMITDESCRIPTORLISTADDRESS_REG register
 *  Points the DMA to the start of the Transmit Descriptor List
 */
#define EMAC_CHANNEL2TRANSMITDESCRIPTORLISTADDRESS_REG (DR_REG_EMAC_BASE + 0x1210)
/** EMAC_CH2_TDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Transmit List This field contains the base address of the first descriptor
 *  in the Transmit Descriptor list The LSB bits _1:0, 2:0, 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and are internally taken as allzero by the DMA
 *  Therefore, these LSB bits are readonly _RO_
 */
#define EMAC_CH2_TDESLA    0xFFFFFFFFU
#define EMAC_CH2_TDESLA_M  (EMAC_CH2_TDESLA_V << EMAC_CH2_TDESLA_S)
#define EMAC_CH2_TDESLA_V  0xFFFFFFFFU
#define EMAC_CH2_TDESLA_S  0

/** EMAC_CHANNEL2STATUS_REG register
 *  The software driver _application_ reads this register during interrupt service
 *  routine or polling to determine the status of the DMA Bits [29:26] are reserved for
 *  the Channel 2 Status Register
 */
#define EMAC_CHANNEL2STATUS_REG (DR_REG_EMAC_BASE + 0x1214)
/** EMAC_CH2_TI : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt This bit indicates that the frame transmission is complete When
 *  transmission is complete, Bit 31 _OWN_ of TDES0 is reset, and the specific frame
 *  status information is updated in the descriptor
 */
#define EMAC_CH2_TI    (BIT(0))
#define EMAC_CH2_TI_M  (EMAC_CH2_TI_V << EMAC_CH2_TI_S)
#define EMAC_CH2_TI_V  0x00000001U
#define EMAC_CH2_TI_S  0
/** EMAC_CH2_TPS : R/W; bitpos: [1]; default: 0;
 *  Transmit Process Stopped This bit is set when the transmission is stopped
 */
#define EMAC_CH2_TPS    (BIT(1))
#define EMAC_CH2_TPS_M  (EMAC_CH2_TPS_V << EMAC_CH2_TPS_S)
#define EMAC_CH2_TPS_V  0x00000001U
#define EMAC_CH2_TPS_S  1
/** EMAC_CH2_TU : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Transmit List and the DMA cannot acquire it Transmission is
 *  suspended Bits[22:20] explain the Transmit Process state transitions To resume
 *  processing Transmit descriptors, the host should change the ownership of the
 *  descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command
 */
#define EMAC_CH2_TU    (BIT(2))
#define EMAC_CH2_TU_M  (EMAC_CH2_TU_V << EMAC_CH2_TU_S)
#define EMAC_CH2_TU_V  0x00000001U
#define EMAC_CH2_TU_S  2
/** EMAC_CH2_TJT : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout This bit indicates that the Transmit Jabber Timer expired,
 *  which happens when the frame size exceeds 2,048 _10,240 bytes when the Jumbo frame
 *  is enabled_ When the Jabber Timeout occurs, the transmission process is aborted and
 *  placed in the Stopped state This causes the Transmit Jabber Timeout TDES0[14] flag
 *  to assert
 */
#define EMAC_CH2_TJT    (BIT(3))
#define EMAC_CH2_TJT_M  (EMAC_CH2_TJT_V << EMAC_CH2_TJT_S)
#define EMAC_CH2_TJT_V  0x00000001U
#define EMAC_CH2_TJT_S  3
/** EMAC_CH2_OVF : R/W; bitpos: [4]; default: 0;
 *  Receive Overflow This bit indicates that the Receive Buffer had an Overflow during
 *  frame reception If the partial frame is transferred to the application, the
 *  overflow status is set in RDES0[11]
 */
#define EMAC_CH2_OVF    (BIT(4))
#define EMAC_CH2_OVF_M  (EMAC_CH2_OVF_V << EMAC_CH2_OVF_S)
#define EMAC_CH2_OVF_V  0x00000001U
#define EMAC_CH2_OVF_S  4
/** EMAC_CH2_UNF : R/W; bitpos: [5]; default: 0;
 *  Transmit Underflow This bit indicates that the Transmit Buffer had an Underflow
 *  during frame transmission Transmission is suspended and an Underflow Error TDES0[1]
 *  is set
 */
#define EMAC_CH2_UNF    (BIT(5))
#define EMAC_CH2_UNF_M  (EMAC_CH2_UNF_V << EMAC_CH2_UNF_S)
#define EMAC_CH2_UNF_V  0x00000001U
#define EMAC_CH2_UNF_S  5
/** EMAC_CH2_RI : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt This bit indicates that the frame reception is complete When
 *  reception is complete, the Bit 31 of RDES1 _Disable Interrupt on Completion_ is
 *  reset in the last Descriptor, and the specific frame status information is updated
 *  in the descriptor The reception remains in the Running state
 */
#define EMAC_CH2_RI    (BIT(6))
#define EMAC_CH2_RI_M  (EMAC_CH2_RI_V << EMAC_CH2_RI_S)
#define EMAC_CH2_RI_V  0x00000001U
#define EMAC_CH2_RI_S  6
/** EMAC_CH2_RU : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Receive List and the DMA cannot acquire it The Receive Process is
 *  suspended To resume processing Receive descriptors, the host should change the
 *  ownership of the descriptor and issue a Receive Poll Demand command If no Receive
 *  Poll Demand is issued, the Receive Process resumes when the next recognized
 *  incoming frame is received This bit is set only when the previous Receive
 *  Descriptor is owned by the DMA
 */
#define EMAC_CH2_RU    (BIT(7))
#define EMAC_CH2_RU_M  (EMAC_CH2_RU_V << EMAC_CH2_RU_S)
#define EMAC_CH2_RU_V  0x00000001U
#define EMAC_CH2_RU_S  7
/** EMAC_CH2_RPS : R/W; bitpos: [8]; default: 0;
 *  Receive Process Stopped This bit is asserted when the Receive Process enters the
 *  Stopped state
 */
#define EMAC_CH2_RPS    (BIT(8))
#define EMAC_CH2_RPS_M  (EMAC_CH2_RPS_V << EMAC_CH2_RPS_S)
#define EMAC_CH2_RPS_V  0x00000001U
#define EMAC_CH2_RPS_S  8
/** EMAC_CH2_RWT : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout When set, this bit indicates that the Receive Watchdog
 *  Timer expired while receiving the current frame and the current frame is truncated
 *  after the watchdog timeout
 */
#define EMAC_CH2_RWT    (BIT(9))
#define EMAC_CH2_RWT_M  (EMAC_CH2_RWT_V << EMAC_CH2_RWT_S)
#define EMAC_CH2_RWT_V  0x00000001U
#define EMAC_CH2_RWT_S  9
/** EMAC_CH2_ETI : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt This bit indicates that the frame to be transmitted is
 *  fully transferred to the MTL Transmit FIFO
 */
#define EMAC_CH2_ETI    (BIT(10))
#define EMAC_CH2_ETI_M  (EMAC_CH2_ETI_V << EMAC_CH2_ETI_S)
#define EMAC_CH2_ETI_V  0x00000001U
#define EMAC_CH2_ETI_S  10
/** EMAC_CH2_FBI : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Interrupt This bit indicates that a bus error occurred, as
 *  described in Bits [25:23] When this bit is set, the corresponding DMA engine
 *  disables all of its bus accesses 12:11		Reserved	00	RO
 */
#define EMAC_CH2_FBI    (BIT(13))
#define EMAC_CH2_FBI_M  (EMAC_CH2_FBI_V << EMAC_CH2_FBI_S)
#define EMAC_CH2_FBI_V  0x00000001U
#define EMAC_CH2_FBI_S  13
/** EMAC_CH2_ERI : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt This bit indicates that the DMA filled the first data
 *  buffer of the packet This bit is cleared when the software writes 1 to this bit or
 *  Bit 6 _RI_ of this register is set _whichever occurs earlier_
 */
#define EMAC_CH2_ERI    (BIT(14))
#define EMAC_CH2_ERI_M  (EMAC_CH2_ERI_V << EMAC_CH2_ERI_S)
#define EMAC_CH2_ERI_V  0x00000001U
#define EMAC_CH2_ERI_S  14
/** EMAC_CH2_AIS : R/W; bitpos: [15]; default: 0;
 *  Abnormal Interrupt Summary Abnormal Interrupt Summary bit value is the logical OR
 *  of the following when the corresponding interrupt bits are enabled in Register 7
 *  _Interrupt Enable Register_: Register 5[1]: Transmit Process Stopped Register 5[3]:
 *  Transmit Jabber Timeout Register 5[4]: Receive FIFO Overflow Register 5[5]:
 *  Transmit Underflow Register 5[7]: Receive Buffer Unavailable Register 5[8]: Receive
 *  Process Stopped Register 5[9]: Receive Watchdog Timeout Register 5[10]: Early
 *  Transmit Interrupt Register 5[13]: Fatal Bus Error Only unmasked bits affect the
 *  Abnormal Interrupt Summary bit This is a sticky bit and must be cleared _by writing
 *  1 to this bit_ each time a corresponding bit, which causes AIS to be set, is cleared
 */
#define EMAC_CH2_AIS    (BIT(15))
#define EMAC_CH2_AIS_M  (EMAC_CH2_AIS_V << EMAC_CH2_AIS_S)
#define EMAC_CH2_AIS_V  0x00000001U
#define EMAC_CH2_AIS_S  15
/** EMAC_CH2_NIS : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Normal Interrupt Summary bit value is the logical OR of
 *  the following bits when the corresponding interrupt bits are enabled in Register 7
 *  _Interrupt Enable Register_: Register 5[0]: Transmit Interrupt Register 5[2]:
 *  Transmit Buffer Unavailable Register 5[6]: Receive Interrupt Register 5[14]: Early
 *  Receive Interrupt Only unmasked bits _interrupts for which interrupt enable is set
 *  in Register 7_ affect the Normal Interrupt Summary bit This is a sticky bit and
 *  must be cleared _by writing 1 to this bit_ each time a corresponding bit, which
 *  causes NIS to be set, is cleared
 */
#define EMAC_CH2_NIS    (BIT(16))
#define EMAC_CH2_NIS_M  (EMAC_CH2_NIS_V << EMAC_CH2_NIS_S)
#define EMAC_CH2_NIS_V  0x00000001U
#define EMAC_CH2_NIS_S  16
/** EMAC_CH2_RS : RO; bitpos: [19:17]; default: 0;
 *  Receive Process State This field indicates the Receive DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Receive Command
 *  issued 3’b001: Running: Fetching Receive Transfer Descriptor 3’b010: Reserved for
 *  future use 3’b011: Running: Waiting for receive packet 3’b100: Suspended: Receive
 *  Descriptor Unavailable 3’b101: Running: Closing Receive Descriptor 3’b110:
 *  TIME_STAMP write state 3’b111: Running: Transferring the receive packet data from
 *  receive buffer to host memory
 */
#define EMAC_CH2_RS    0x00000007U
#define EMAC_CH2_RS_M  (EMAC_CH2_RS_V << EMAC_CH2_RS_S)
#define EMAC_CH2_RS_V  0x00000007U
#define EMAC_CH2_RS_S  17
/** EMAC_CH2_TS : RO; bitpos: [22:20]; default: 0;
 *  Transmit Process State This field indicates the Transmit DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Transmit Command
 *  issued 3’b001: Running: Fetching Transmit Transfer Descriptor 3’b010: Running:
 *  Waiting for status 3’b011: Running: Reading Data from host memory buffer and
 *  queuing it to transmit buffer _Tx FIFO_ 3’b100: TIME_STAMP write state 3’b101:
 *  Reserved for future use 3’b110: Suspended: Transmit Descriptor Unavailable or
 *  Transmit Buffer Underflow 3’b111: Running: Closing Transmit Descriptor
 */
#define EMAC_CH2_TS    0x00000007U
#define EMAC_CH2_TS_M  (EMAC_CH2_TS_V << EMAC_CH2_TS_S)
#define EMAC_CH2_TS_V  0x00000007U
#define EMAC_CH2_TS_S  20
/** EMAC_CH2_EB : RO; bitpos: [25:23]; default: 0;
 *  Error Bits This field indicates the type of error that caused a Bus Error, for
 *  example, error response on the AHB or AXI interface This field is valid only when
 *  Bit 13 _FBI_ is set This field does not generate an interrupt 0 0 0: Error during
 *  Rx DMA Write Data Transfer 0 1 1: Error during Tx DMA Read Data Transfer 1 0 0:
 *  Error during Rx DMA Descriptor Write Access 1 0 1: Error during Tx DMA Descriptor
 *  Write Access 1 1 0: Error during Rx DMA Descriptor Read Access 1 1 1: Error during
 *  Tx DMA Descriptor Read Access Note: 001 and 010 are reserved
 */
#define EMAC_CH2_EB    0x00000007U
#define EMAC_CH2_EB_M  (EMAC_CH2_EB_V << EMAC_CH2_EB_S)
#define EMAC_CH2_EB_V  0x00000007U
#define EMAC_CH2_EB_S  23
/** EMAC_CH2_GLI : RO; bitpos: [26]; default: 0;
 *  EMAC Line Interface Interrupt When set, this bit reflects any of the following
 *  interrupt events in the DWC_EMAC interfaces _if present and enabled in your
 *  configuration_: PCS _TBI, RTBI, or SGMII_: Link change or autonegotiation complete
 *  event SMII or RGMII: Link change event General Purpose Input Status _GPIS_: Any LL
 *  or LH event on the gpi_i input ports To identify the exact cause of the interrupt,
 *  the software must first read Bit 11 and Bits[2:0] of Register 14 _Interrupt Status
 *  Register_ and then to clear the source of interrupt _which also clears the GLI
 *  interrupt_, read any of the following corresponding registers: PCS _TBI, RTBI, or
 *  SGMII_: Register 49 _AN Status Register_ SMII or RGMII: Register 54
 *  _SGMII/RGMII/SMII Control and Status Register_ General Purpose Input _GPI_:
 *  Register 56 _General Purpose IO Register_ The interrupt signal from the DWC_EMAC
 *  subsystem _sbd_intr_o_ is high when this bit is high
 */
#define EMAC_CH2_GLI    (BIT(26))
#define EMAC_CH2_GLI_M  (EMAC_CH2_GLI_V << EMAC_CH2_GLI_S)
#define EMAC_CH2_GLI_V  0x00000001U
#define EMAC_CH2_GLI_S  26
/** EMAC_CH2_GMI : RO; bitpos: [27]; default: 0;
 *  EMAC MMC Interrupt This bit reflects an interrupt event in the MMC module of the
 *  DWC_EMAC The software must read the corresponding registers in the DWC_EMAC to get
 *  the exact cause of the interrupt and clear the source of interrupt to make this bit
 *  as 1’b0 The interrupt signal from the DWC_EMAC subsystem _sbd_intr_o_ is high when
 *  this bit is high This bit is applicable only when the MAC Management Counters _MMC_
 *  are enabled Otherwise, this bit is reserved
 */
#define EMAC_CH2_GMI    (BIT(27))
#define EMAC_CH2_GMI_M  (EMAC_CH2_GMI_V << EMAC_CH2_GMI_S)
#define EMAC_CH2_GMI_V  0x00000001U
#define EMAC_CH2_GMI_S  27
/** EMAC_CH2_GPI : RO; bitpos: [28]; default: 0;
 *  EMAC PMT Interrupt This bit indicates an interrupt event in the PMT module of the
 *  DWC_EMAC The software must read the PMT Control and Status Register in the MAC to
 *  get the exact cause of interrupt and clear its source to reset this bit to 1’b0 The
 *  interrupt signal from the DWC_EMAC subsystem _sbd_intr_o_ is high when this bit is
 *  high This bit is applicable only when the Power Management feature is enabled
 *  Otherwise, this bit is reserved Note: The GPI and pmt_intr_o interrupts are
 *  generated in different clock domains
 */
#define EMAC_CH2_GPI    (BIT(28))
#define EMAC_CH2_GPI_M  (EMAC_CH2_GPI_V << EMAC_CH2_GPI_S)
#define EMAC_CH2_GPI_V  0x00000001U
#define EMAC_CH2_GPI_S  28
/** EMAC_CH2_TTI : RO; bitpos: [29]; default: 0;
 *  Timestamp Trigger Interrupt This bit indicates an interrupt event in the Timestamp
 *  Generator block of the DWC_EMAC The software must read the corresponding registers
 *  in the DWC_EMAC to get the exact cause of the interrupt and clear its source to
 *  reset this bit to 1'b0 When this bit is high, the interrupt signal from the
 *  DWC_EMAC subsystem _sbd_intr_o_ is high This bit is applicable only when the IEEE
 *  1588 Timestamp feature is enabled Otherwise, this bit is reserved
 */
#define EMAC_CH2_TTI    (BIT(29))
#define EMAC_CH2_TTI_M  (EMAC_CH2_TTI_V << EMAC_CH2_TTI_S)
#define EMAC_CH2_TTI_V  0x00000001U
#define EMAC_CH2_TTI_S  29
/** EMAC_CH2_GLPII_GTMSI : RO; bitpos: [30]; default: 0;
 *  GTMSI: EMAC TMS Interrupt _for Channel 1 and Channel 2_ This bit indicates an
 *  interrupt event in the traffic manager and scheduler logic of DWC_EMAC To reset
 *  this bit, the software must read the corresponding registers _Channel Status
 *  Register_ to get the exact cause of the interrupt and clear its source Note: GTMSI
 *  status is given only in Channel 1 and Channel 2 DMA register when the AV feature is
 *  enabled and corresponding additional transmit channels are present Otherwise, this
 *  bit is reserved When this bit is high, the interrupt signal from the MAC
 *  _sbd_intr_o_ is high
 */
#define EMAC_CH2_GLPII_GTMSI    (BIT(30))
#define EMAC_CH2_GLPII_GTMSI_M  (EMAC_CH2_GLPII_GTMSI_V << EMAC_CH2_GLPII_GTMSI_S)
#define EMAC_CH2_GLPII_GTMSI_V  0x00000001U
#define EMAC_CH2_GLPII_GTMSI_S  30

/** EMAC_CHANNEL2OPERATIONMODE_REG register
 *  Establishes the Receive and Transmit operating modes and command
 */
#define EMAC_CHANNEL2OPERATIONMODE_REG (DR_REG_EMAC_BASE + 0x1218)
/** EMAC_CH2_SR : R/W; bitpos: [1]; default: 0;
 *  Start or Stop Receive When this bit is set, the Receive process is placed in the
 *  Running state The DMA attempts to acquire the descriptor from the Receive list and
 *  processes the incoming frames The descriptor acquisition is attempted from the
 *  current position in the list, which is the address set by the Register 3 _Receive
 *  Descriptor List Address Register_ or the position retained when the Receive process
 *  was previously stopped If the DMA does not own the descriptor, reception is
 *  suspended and Bit 7 _Receive Buffer Unavailable_ of Register 5 _Status Register_ is
 *  set The Start Receive command is effective only when the reception has stopped If
 *  the command is issued before setting Register 3 _Receive Descriptor List Address
 *  Register_, the DMA behavior is unpredictable When this bit is cleared, the Rx DMA
 *  operation is stopped after the transfer of the current frame The next descriptor
 *  position in the Receive list is saved and becomes the current position after the
 *  Receive process is restarted The Stop Receive command is effective only when the
 *  Receive process is in either the Running _waiting for receive packet_ or in the
 *  Suspended state Note: For information about how to pause the transmission, see
 *  “Stopping and Starting Transmission” on page 715
 */
#define EMAC_CH2_SR    (BIT(1))
#define EMAC_CH2_SR_M  (EMAC_CH2_SR_V << EMAC_CH2_SR_S)
#define EMAC_CH2_SR_V  0x00000001U
#define EMAC_CH2_SR_S  1
/** EMAC_CH2_OSF : R/W; bitpos: [2]; default: 0;
 *  Operate on Second Frame When this bit is set, it instructs the DMA to process the
 *  second frame of the Transmit data even before the status for the first frame is
 *  obtained
 */
#define EMAC_CH2_OSF    (BIT(2))
#define EMAC_CH2_OSF_M  (EMAC_CH2_OSF_V << EMAC_CH2_OSF_S)
#define EMAC_CH2_OSF_V  0x00000001U
#define EMAC_CH2_OSF_S  2
/** EMAC_CH2_RTC : R/W; bitpos: [4:3]; default: 0;
 *  Receive Threshold Control These two bits control the threshold level of the MTL
 *  Receive FIFO Transfer _request_ to DMA starts when the frame size within the MTL
 *  Receive FIFO is larger than the threshold In addition, full frames with length less
 *  than the threshold are automatically transferred The value of 11 is not applicable
 *  if the configured Receive FIFO size is 128 bytes These bits are valid only when the
 *  RSF bit is zero, and are ignored when the RSF bit is set to 1 00: 64 01: 32 10: 96
 *  11: 128
 */
#define EMAC_CH2_RTC    0x00000003U
#define EMAC_CH2_RTC_M  (EMAC_CH2_RTC_V << EMAC_CH2_RTC_S)
#define EMAC_CH2_RTC_V  0x00000003U
#define EMAC_CH2_RTC_S  3
/** EMAC_CH2_DGF : R/W; bitpos: [5]; default: 0;
 *  Drop Giant Frames When set, the MAC drops the received giant frames in the Rx FIFO,
 *  that is, frames that are larger than the computed giant frame limit When reset, the
 *  MAC does not drop the giant frames in the Rx FIFO Note: This bit is available in
 *  the following configurations in which the giant frame status is not provided in Rx
 *  status and giant frames are not dropped by default: Configurations in which IP
 *  Checksum Offload _Type 1_ is selected in Rx Configurations in which the IPC Full
 *  Checksum Offload Engine _Type 2_ is selected in Rx with normal descriptor format
 *  Configurations in which the Advanced Timestamp feature is selected In all other
 *  configurations, this bit is not used _reserved and always reset_
 */
#define EMAC_CH2_DGF    (BIT(5))
#define EMAC_CH2_DGF_M  (EMAC_CH2_DGF_V << EMAC_CH2_DGF_S)
#define EMAC_CH2_DGF_V  0x00000001U
#define EMAC_CH2_DGF_S  5
/** EMAC_CH2_FUF : R/W; bitpos: [6]; default: 0;
 *  Forward Undersized Good Frames When set, the Rx FIFO forwards Undersized frames
 *  _that is, frames with no Error and length less than 64 bytes_ including padbytes
 *  and CRC When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a
 *  frame is already transferred because of the lower value of Receive Threshold, for
 *  example, RTC = 01
 */
#define EMAC_CH2_FUF    (BIT(6))
#define EMAC_CH2_FUF_M  (EMAC_CH2_FUF_V << EMAC_CH2_FUF_S)
#define EMAC_CH2_FUF_V  0x00000001U
#define EMAC_CH2_FUF_S  6
/** EMAC_CH2_FEF : R/W; bitpos: [7]; default: 0;
 *  Forward Error Frames When this bit is reset, the Rx FIFO drops frames with error
 *  status _CRC error, collision error, GMII_ER, giant frame, watchdog timeout, or
 *  overflow_ However, if the start byte _write_ pointer of a frame is already
 *  transferred to the read controller side _in Threshold mode_, then the frame is not
 *  dropped In the EMACMTL configuration in which the Frame Length FIFO is also enabled
 *  during core configuration, the Rx FIFO drops the error frames if that frame's start
 *  byte is not transferred _output_ on the ARI bus When the FEF bit is set, all frames
 *  except runt error frames are forwarded to the DMA If the Bit 25 _RSF_ is set and
 *  the Rx FIFO overflows when a partial frame is written, then the frame is dropped
 *  irrespective of the FEF bit setting However, if the Bit 25 _RSF_ is reset and the
 *  Rx FIFO overflows when a partial frame is written, then a partial frame may be
 *  forwarded to the DMA Note: When FEF bit is reset, the giant frames are dropped if
 *  the giant frame status is given in Rx Status _in Table 86 or Table 823_ in the
 *  following configurations: The IP checksum engine _Type 1_ and full checksum offload
 *  engine _Type 2_ are not selected The advanced timestamp feature is not selected but
 *  the extended status is selected The extended status is available with the following
 *  features: 	L3L4 filter in EMACCORE or EMACMTL configurations 	Full checksum offload
 *  engine _Type 2_ with enhanced descriptor format in the EMACDMA, EMACAHB, or EMACAXI
 *  configurations
 */
#define EMAC_CH2_FEF    (BIT(7))
#define EMAC_CH2_FEF_M  (EMAC_CH2_FEF_V << EMAC_CH2_FEF_S)
#define EMAC_CH2_FEF_V  0x00000001U
#define EMAC_CH2_FEF_S  7
/** EMAC_CH2_EFC : R/W; bitpos: [8]; default: 0;
 *  Enable HW Flow Control When this bit is set, the flow control signal operation
 *  based on the filllevel of Rx FIFO is enabled When reset, the flow control operation
 *  is disabled This bit is not used _reserved and always reset_ when the Rx FIFO is
 *  less than 4 KB
 */
#define EMAC_CH2_EFC    (BIT(8))
#define EMAC_CH2_EFC_M  (EMAC_CH2_EFC_V << EMAC_CH2_EFC_S)
#define EMAC_CH2_EFC_V  0x00000001U
#define EMAC_CH2_EFC_S  8
/** EMAC_CH2_RFA : R/W; bitpos: [10:9]; default: 0;
 *  Threshold for Activating Flow Control _in halfduplex and fullduplex modes_ These
 *  bits control the threshold _Fill level of Rx FIFO_ at which the flow control is
 *  activated 00: Full minus 1 KB, that is, FULL—1KB 01: Full minus 2 KB, that is,
 *  FULL—2KB 10: Full minus 3 KB, that is, FULL—3KB 11: Full minus 4 KB, that is,
 *  FULL—4KB These values are applicable only to Rx FIFOs of 4 KB or more and when Bit
 *  8 _EFC_ is set high If the Rx FIFO is 8 KB or more, an additional Bit _RFA_2_ is
 *  used for more threshold levels as described in Bit 23 These bits are reserved and
 *  readonly when the depth of Rx FIFO is less than 4 KB Note: When FIFO size is
 *  exactly 4 KB, although the DWC_EMAC allows you to program the value of these bits
 *  to 11, the software should not program these bits to 2'b11 The value 2'b11 means
 *  flow control on FIFO empty condition
 */
#define EMAC_CH2_RFA    0x00000003U
#define EMAC_CH2_RFA_M  (EMAC_CH2_RFA_V << EMAC_CH2_RFA_S)
#define EMAC_CH2_RFA_V  0x00000003U
#define EMAC_CH2_RFA_S  9
/** EMAC_CH2_RFD : R/W; bitpos: [12:11]; default: 0;
 *  Threshold for Deactivating Flow Control _in halfduplex and fullduplex modes_ These
 *  bits control the threshold _Filllevel of Rx FIFO_ at which the flow control is
 *  deasserted after activation 00: Full minus 1 KB, that is, FULL — 1 KB 01: Full
 *  minus 2 KB, that is, FULL — 2 KB 10: Full minus 3 KB, that is, FULL — 3 KB 11: Full
 *  minus 4 KB, that is, FULL — 4 KB The deassertion is effective only after flow
 *  control is asserted If the Rx FIFO is 8 KB or more, an additional Bit _RFD_2_ is
 *  used for more threshold levels as described in Bit 22 These bits are reserved and
 *  readonly when the Rx FIFO depth is less than 4 KB Note: For proper flow control,
 *  the value programmed in the “RFD_2, RFD” fields should be equal to or more than the
 *  value programmed in the “RFA_2, RFA” fields
 */
#define EMAC_CH2_RFD    0x00000003U
#define EMAC_CH2_RFD_M  (EMAC_CH2_RFD_V << EMAC_CH2_RFD_S)
#define EMAC_CH2_RFD_V  0x00000003U
#define EMAC_CH2_RFD_S  11
/** EMAC_CH2_ST : R/W; bitpos: [13]; default: 0;
 *  Start or Stop Transmission Command When this bit is set, transmission is placed in
 *  the Running state, and the DMA checks the Transmit List at the current position for
 *  a frame to be transmitted Descriptor acquisition is attempted either from the
 *  current position in the list, which is the Transmit List Base Address set by
 *  Register 4 _Transmit Descriptor List Address Register_, or from the position
 *  retained when transmission was stopped previously If the DMA does not own the
 *  current descriptor, transmission enters the Suspended state and Bit 2 _Transmit
 *  Buffer Unavailable_ of Register 5 _Status Register_ is set The Start Transmission
 *  command is effective only when transmission is stopped If the command is issued
 *  before setting Register 4 _Transmit Descriptor List Address Register_, then the DMA
 *  behavior is unpredictable When this bit is reset, the transmission process is
 *  placed in the Stopped state after completing the transmission of the current frame
 *  The Next Descriptor position in the Transmit List is saved, and it becomes the
 *  current position when transmission is restarted To change the list address, you
 *  need to program Register 4 _Transmit Descriptor List Address Register_ with a new
 *  value when this bit is reset The new value is considered when this bit is set again
 *  The stop transmission command is effective only when the transmission of the
 *  current frame is complete or the transmission is in the Suspended state Note: For
 *  information about how to pause the transmission, see “Stopping and Starting
 *  Transmission” on page 715
 */
#define EMAC_CH2_ST    (BIT(13))
#define EMAC_CH2_ST_M  (EMAC_CH2_ST_V << EMAC_CH2_ST_S)
#define EMAC_CH2_ST_V  0x00000001U
#define EMAC_CH2_ST_S  13
/** EMAC_CH2_TTC : R/W; bitpos: [16:14]; default: 0;
 *  Transmit Threshold Control These bits control the threshold level of the MTL
 *  Transmit FIFO Transmission starts when the frame size within the MTL Transmit FIFO
 *  is larger than the threshold In addition, full frames with a length less than the
 *  threshold are also transmitted These bits are used only when Bit 21 _TSF_ is reset
 *  000: 64 001: 128 010: 192 011: 256 100: 40 101: 32 110: 24 111: 16
 */
#define EMAC_CH2_TTC    0x00000007U
#define EMAC_CH2_TTC_M  (EMAC_CH2_TTC_V << EMAC_CH2_TTC_S)
#define EMAC_CH2_TTC_V  0x00000007U
#define EMAC_CH2_TTC_S  14
/** EMAC_CH2_FTF : R/W; bitpos: [20]; default: 0;
 *  Flush Transmit FIFO When this bit is set, the transmit FIFO controller logic is
 *  reset to its default values and thus all data in the Tx FIFO is lost or flushed
 *  This bit is cleared internally when the flushing operation is complete The
 *  Operation Mode register should not be written to until this bit is cleared The data
 *  which is already accepted by the MAC transmitter is not flushed It is scheduled for
 *  transmission and results in underflow and runt frame transmission Note: The flush
 *  operation is complete only when the Tx FIFO is emptied of its contents and all the
 *  pending Transmit Status of the transmitted frames are accepted by the host In order
 *  to complete this flush operation, the PHY transmit clock _clk_tx_i_ is required to
 *  be active 19:17		Reserved 	000	RO
 */
#define EMAC_CH2_FTF    (BIT(20))
#define EMAC_CH2_FTF_M  (EMAC_CH2_FTF_V << EMAC_CH2_FTF_S)
#define EMAC_CH2_FTF_V  0x00000001U
#define EMAC_CH2_FTF_S  20
/** EMAC_CH2_TSF : R/W; bitpos: [21]; default: 0;
 *  Transmit Store and Forward When this bit is set, transmission starts when a full
 *  frame resides in the MTL Transmit FIFO When this bit is set, the TTC values
 *  specified in Bits [16:14] are ignored This bit should be changed only when the
 *  transmission is stopped
 */
#define EMAC_CH2_TSF    (BIT(21))
#define EMAC_CH2_TSF_M  (EMAC_CH2_TSF_V << EMAC_CH2_TSF_S)
#define EMAC_CH2_TSF_V  0x00000001U
#define EMAC_CH2_TSF_S  21
/** EMAC_CH2_RFD_2 : R/W; bitpos: [22]; default: 0;
 *  MSB of Threshold for Deactivating Flow Control If the DWC_EMAC is configured for Rx
 *  FIFO size of 8 KB or more, this bit _when set_ provides additional threshold levels
 *  for deactivating the flow control in both halfduplex and fullduplex modes This bit
 *  _as Most Significant Bit_ along with the RFD _Bits [12:11]_ gives the following
 *  thresholds for deactivating flow control: 100: Full minus 5 KB, that is, FULL — 5
 *  KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that is, FULL —
 *  7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB or less deep
 */
#define EMAC_CH2_RFD_2    (BIT(22))
#define EMAC_CH2_RFD_2_M  (EMAC_CH2_RFD_2_V << EMAC_CH2_RFD_2_S)
#define EMAC_CH2_RFD_2_V  0x00000001U
#define EMAC_CH2_RFD_2_S  22
/** EMAC_CH2_RFA_2 : R/W; bitpos: [23]; default: 0;
 *  MSB of Threshold for Activating Flow Control If the DWC_EMAC is configured for an
 *  Rx FIFO size of 8 KB or more, this bit _when set_ provides additional threshold
 *  levels for activating the flow control in both half duplex and fullduplex modes
 *  This bit _as Most Significant Bit_, along with the RFA _Bits [10:9]_, gives the
 *  following thresholds for activating flow control: 100: Full minus 5 KB, that is,
 *  FULL — 5 KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that
 *  is, FULL — 7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB
 *  or less deep
 */
#define EMAC_CH2_RFA_2    (BIT(23))
#define EMAC_CH2_RFA_2_M  (EMAC_CH2_RFA_2_V << EMAC_CH2_RFA_2_S)
#define EMAC_CH2_RFA_2_V  0x00000001U
#define EMAC_CH2_RFA_2_S  23
/** EMAC_CH2_DFF : R/W; bitpos: [24]; default: 0;
 *  Disable Flushing of Received Frames When this bit is set, the Rx DMA does not flush
 *  any frames because of the unavailability of receive descriptors or buffers as it
 *  does normally when this bit is reset _See “Receive Process Suspended” on page 83_
 *  This bit is reserved _and RO_ in the EMACMTL configuration
 */
#define EMAC_CH2_DFF    (BIT(24))
#define EMAC_CH2_DFF_M  (EMAC_CH2_DFF_V << EMAC_CH2_DFF_S)
#define EMAC_CH2_DFF_V  0x00000001U
#define EMAC_CH2_DFF_S  24
/** EMAC_CH2_RSF : R/W; bitpos: [25]; default: 0;
 *  Receive Store and Forward When this bit is set, the MTL reads a frame from the Rx
 *  FIFO only after the complete frame has been written to it, ignoring the RTC bits
 *  When this bit is reset, the Rx FIFO operates in the cutthrough mode, subject to the
 *  threshold specified by the RTC bits
 */
#define EMAC_CH2_RSF    (BIT(25))
#define EMAC_CH2_RSF_M  (EMAC_CH2_RSF_V << EMAC_CH2_RSF_S)
#define EMAC_CH2_RSF_V  0x00000001U
#define EMAC_CH2_RSF_S  25
/** EMAC_CH2_DT : R/W; bitpos: [26]; default: 0;
 *  Disable Dropping of TCP/IP Checksum Error Frames When this bit is set, the MAC does
 *  not drop the frames which only have errors detected by the Receive Checksum Offload
 *  engine Such frames do not have any errors _including FCS error_ in the Ethernet
 *  frame received by the MAC but have errors only in the encapsulated payload When
 *  this bit is reset, all error frames are dropped if the FEF bit is reset If the IPC
 *  Full Checksum Offload Engine _Type 2_ is disabled, this bit is reserved _RO with
 *  value 1'b0_
 */
#define EMAC_CH2_DT    (BIT(26))
#define EMAC_CH2_DT_M  (EMAC_CH2_DT_V << EMAC_CH2_DT_S)
#define EMAC_CH2_DT_V  0x00000001U
#define EMAC_CH2_DT_S  26

/** EMAC_CHANNEL2INTERRUPTENABLE_REG register
 *  Enables the interrupts reported by the Status Register
 */
#define EMAC_CHANNEL2INTERRUPTENABLE_REG (DR_REG_EMAC_BASE + 0x121c)
/** EMAC_CH2_TIE : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Transmit Interrupt is enabled When this bit is reset, the Transmit
 *  Interrupt is disabled The sbd_intr_o interrupt is generated as shown in Figure 61
 *  It is asserted only when the TTI, GPI, GMI, GLI, or GLPII bit of the DMA Status
 *  register is asserted, or when the NIS or AIS Status bit is asserted and the
 *  corresponding Interrupt Enable bits _NIE or AIE_ are enabled
 */
#define EMAC_CH2_TIE    (BIT(0))
#define EMAC_CH2_TIE_M  (EMAC_CH2_TIE_V << EMAC_CH2_TIE_S)
#define EMAC_CH2_TIE_V  0x00000001U
#define EMAC_CH2_TIE_S  0
/** EMAC_CH2_TSE : R/W; bitpos: [1]; default: 0;
 *  Transmit Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Transmission Stopped Interrupt is enabled When this bit is reset, the
 *  Transmission Stopped Interrupt is disabled
 */
#define EMAC_CH2_TSE    (BIT(1))
#define EMAC_CH2_TSE_M  (EMAC_CH2_TSE_V << EMAC_CH2_TSE_S)
#define EMAC_CH2_TSE_V  0x00000001U
#define EMAC_CH2_TSE_S  1
/** EMAC_CH2_TUE : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable Enable When this bit is set with Normal Interrupt
 *  Summary Enable _Bit 16_, the Transmit Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled
 */
#define EMAC_CH2_TUE    (BIT(2))
#define EMAC_CH2_TUE_M  (EMAC_CH2_TUE_V << EMAC_CH2_TUE_S)
#define EMAC_CH2_TUE_V  0x00000001U
#define EMAC_CH2_TUE_S  2
/** EMAC_CH2_TJE : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Jabber Timeout Interrupt is enabled When this bit is
 *  reset, the Transmit Jabber Timeout Interrupt is disabled
 */
#define EMAC_CH2_TJE    (BIT(3))
#define EMAC_CH2_TJE_M  (EMAC_CH2_TJE_V << EMAC_CH2_TJE_S)
#define EMAC_CH2_TJE_V  0x00000001U
#define EMAC_CH2_TJE_S  3
/** EMAC_CH2_OVE : R/W; bitpos: [4]; default: 0;
 *  Overflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Receive Overflow Interrupt is enabled When this bit is reset,
 *  the Overflow Interrupt is disabled
 */
#define EMAC_CH2_OVE    (BIT(4))
#define EMAC_CH2_OVE_M  (EMAC_CH2_OVE_V << EMAC_CH2_OVE_S)
#define EMAC_CH2_OVE_V  0x00000001U
#define EMAC_CH2_OVE_S  4
/** EMAC_CH2_UNE : R/W; bitpos: [5]; default: 0;
 *  Underflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Underflow Interrupt is enabled When this bit is
 *  reset, the Underflow Interrupt is disabled
 */
#define EMAC_CH2_UNE    (BIT(5))
#define EMAC_CH2_UNE_M  (EMAC_CH2_UNE_V << EMAC_CH2_UNE_S)
#define EMAC_CH2_UNE_V  0x00000001U
#define EMAC_CH2_UNE_S  5
/** EMAC_CH2_RIE : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Receive Interrupt is enabled When this bit is reset, the Receive
 *  Interrupt is disabled
 */
#define EMAC_CH2_RIE    (BIT(6))
#define EMAC_CH2_RIE_M  (EMAC_CH2_RIE_V << EMAC_CH2_RIE_S)
#define EMAC_CH2_RIE_V  0x00000001U
#define EMAC_CH2_RIE_S  6
/** EMAC_CH2_RUE : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Receive Buffer Unavailable Interrupt is disabled
 */
#define EMAC_CH2_RUE    (BIT(7))
#define EMAC_CH2_RUE_M  (EMAC_CH2_RUE_V << EMAC_CH2_RUE_S)
#define EMAC_CH2_RUE_V  0x00000001U
#define EMAC_CH2_RUE_S  7
/** EMAC_CH2_RSE : R/W; bitpos: [8]; default: 0;
 *  Receive Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Receive Stopped Interrupt is enabled When this bit is reset, the
 *  Receive Stopped Interrupt is disabled
 */
#define EMAC_CH2_RSE    (BIT(8))
#define EMAC_CH2_RSE_M  (EMAC_CH2_RSE_V << EMAC_CH2_RSE_S)
#define EMAC_CH2_RSE_V  0x00000001U
#define EMAC_CH2_RSE_S  8
/** EMAC_CH2_RWE : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Watchdog Timeout Interrupt is enabled When
 *  this bit is reset, the Receive Watchdog Timeout Interrupt is disabled
 */
#define EMAC_CH2_RWE    (BIT(9))
#define EMAC_CH2_RWE_M  (EMAC_CH2_RWE_V << EMAC_CH2_RWE_S)
#define EMAC_CH2_RWE_V  0x00000001U
#define EMAC_CH2_RWE_S  9
/** EMAC_CH2_ETE : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt Enable When this bit is set with an Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Early Transmit Interrupt is enabled When this bit is
 *  reset, the Early Transmit Interrupt is disabled
 */
#define EMAC_CH2_ETE    (BIT(10))
#define EMAC_CH2_ETE_M  (EMAC_CH2_ETE_V << EMAC_CH2_ETE_S)
#define EMAC_CH2_ETE_V  0x00000001U
#define EMAC_CH2_ETE_S  10
/** EMAC_CH2_FBE : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Fatal Bus Error Interrupt is enabled When this bit is reset, the
 *  Fatal Bus Error Enable Interrupt is disabled 12:11		Reserved	00	RO
 */
#define EMAC_CH2_FBE    (BIT(13))
#define EMAC_CH2_FBE_M  (EMAC_CH2_FBE_V << EMAC_CH2_FBE_S)
#define EMAC_CH2_FBE_V  0x00000001U
#define EMAC_CH2_FBE_S  13
/** EMAC_CH2_ERE : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt Enable When this bit is set with Normal Interrupt Summary
 *  Enable _Bit 16_, the Early Receive Interrupt is enabled When this bit is reset, the
 *  Early Receive Interrupt is disabled
 */
#define EMAC_CH2_ERE    (BIT(14))
#define EMAC_CH2_ERE_M  (EMAC_CH2_ERE_V << EMAC_CH2_ERE_S)
#define EMAC_CH2_ERE_V  0x00000001U
#define EMAC_CH2_ERE_S  14
/** EMAC_CH2_AIE : R/W; bitpos: [15]; default: 0;
 *  Abnormal Interrupt Summary Enable When this bit is set, abnormal interrupt summary
 *  is enabled When this bit is reset, the abnormal interrupt summary is disabled This
 *  bit enables the following interrupts in Register 5 _Status Register_: Register
 *  5[1]: Transmit Process Stopped Register 5[3]: Transmit Jabber Timeout Register
 *  5[4]: Receive Overflow Register 5[5]: Transmit Underflow Register 5[7]: Receive
 *  Buffer Unavailable Register 5[8]: Receive Process Stopped Register 5[9]: Receive
 *  Watchdog Timeout Register 5[10]: Early Transmit Interrupt Register 5[13]: Fatal Bus
 *  Error
 */
#define EMAC_CH2_AIE    (BIT(15))
#define EMAC_CH2_AIE_M  (EMAC_CH2_AIE_V << EMAC_CH2_AIE_S)
#define EMAC_CH2_AIE_V  0x00000001U
#define EMAC_CH2_AIE_S  15
/** EMAC_CH2_NIE : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Enable When this bit is set, normal interrupt summary is
 *  enabled When this bit is reset, normal interrupt summary is disabled This bit
 *  enables the following interrupts in Register 5 _Status Register_: Register 5[0]:
 *  Transmit Interrupt Register 5[2]: Transmit Buffer Unavailable Register 5[6]:
 *  Receive Interrupt Register 5[14]: Early Receive Interrupt
 */
#define EMAC_CH2_NIE    (BIT(16))
#define EMAC_CH2_NIE_M  (EMAC_CH2_NIE_V << EMAC_CH2_NIE_S)
#define EMAC_CH2_NIE_V  0x00000001U
#define EMAC_CH2_NIE_S  16

/** EMAC_CHANNEL2MISSEDFRAMEANDBUFFEROVERFLOWCOUNTER_REG register
 *  Contains the counters for discarded frames because no host Receive Descriptor was
 *  available or because of Receive FIFO Overflow
 */
#define EMAC_CHANNEL2MISSEDFRAMEANDBUFFEROVERFLOWCOUNTER_REG (DR_REG_EMAC_BASE + 0x1220)
/** EMAC_CH2_MISFRMCNT : R/W; bitpos: [15:0]; default: 0;
 *  Missed Frame Counter This field indicates the number of frames missed by the
 *  controller because of the Host Receive Buffer being unavailable This counter is
 *  incremented each time the DMA discards an incoming frame The counter is cleared
 *  when this register is read with mci_be_i[0] at 1’b1
 */
#define EMAC_CH2_MISFRMCNT    0x0000FFFFU
#define EMAC_CH2_MISFRMCNT_M  (EMAC_CH2_MISFRMCNT_V << EMAC_CH2_MISFRMCNT_S)
#define EMAC_CH2_MISFRMCNT_V  0x0000FFFFU
#define EMAC_CH2_MISFRMCNT_S  0
/** EMAC_CH2_MISCNTOVF : R/W; bitpos: [16]; default: 0;
 *  Overflow Bit for Missed Frame Counter This bit is set every time Missed Frame
 *  Counter _Bits[15:0]_ overflows, that is, the DMA discards an incoming frame because
 *  of the Host Receive Buffer being unavailable with the missed frame counter at
 *  maximum value In such a scenario, the Missed frame counter is reset to allzeros and
 *  this bit indicates that the rollover happened
 */
#define EMAC_CH2_MISCNTOVF    (BIT(16))
#define EMAC_CH2_MISCNTOVF_M  (EMAC_CH2_MISCNTOVF_V << EMAC_CH2_MISCNTOVF_S)
#define EMAC_CH2_MISCNTOVF_V  0x00000001U
#define EMAC_CH2_MISCNTOVF_S  16
/** EMAC_CH2_OVFFRMCNT : R/W; bitpos: [27:17]; default: 0;
 *  Overflow Frame Counter This field indicates the number of frames missed by the
 *  application This counter is incremented each time the MTL FIFO overflows The
 *  counter is cleared when this register is read with mci_be_i[2] at 1’b1
 */
#define EMAC_CH2_OVFFRMCNT    0x000007FFU
#define EMAC_CH2_OVFFRMCNT_M  (EMAC_CH2_OVFFRMCNT_V << EMAC_CH2_OVFFRMCNT_S)
#define EMAC_CH2_OVFFRMCNT_V  0x000007FFU
#define EMAC_CH2_OVFFRMCNT_S  17
/** EMAC_CH2_OVFCNTOVF : R/W; bitpos: [28]; default: 0;
 *  Overflow Bit for FIFO Overflow Counter This bit is set every time the Overflow
 *  Frame Counter _Bits[27:17]_ overflows, that is, the Rx FIFO overflows with the
 *  overflow frame counter at maximum value In such a scenario, the overflow frame
 *  counter is reset to allzeros and this bit indicates that the rollover happened
 */
#define EMAC_CH2_OVFCNTOVF    (BIT(28))
#define EMAC_CH2_OVFCNTOVF_M  (EMAC_CH2_OVFCNTOVF_V << EMAC_CH2_OVFCNTOVF_S)
#define EMAC_CH2_OVFCNTOVF_V  0x00000001U
#define EMAC_CH2_OVFCNTOVF_S  28

/** EMAC_CHANNEL2RECEIVEINTERRUPTWATCHDOGTIMER_REG register
 *  Watchdog timeout for Receive Interrupt _RI_ from DMA
 */
#define EMAC_CHANNEL2RECEIVEINTERRUPTWATCHDOGTIMER_REG (DR_REG_EMAC_BASE + 0x1224)
/** EMAC_CH2_RIWT : R/W; bitpos: [7:0]; default: 0;
 *  RI Watchdog Timer Count This bit indicates the number of system clock cycles
 *  multiplied by 256 for which the watchdog timer is set The watchdog timer gets
 *  triggered with the programmed value after the Rx DMA completes the transfer of a
 *  frame for which the RI status bit is not set because of the setting in the
 *  corresponding descriptor RDES1[31] When the watchdog timer runs out, the RI bit is
 *  set and the timer is stopped The watchdog timer is reset when the RI bit is set
 *  high because of automatic setting of RI as per RDES1[31] of any received frame
 */
#define EMAC_CH2_RIWT    0x000000FFU
#define EMAC_CH2_RIWT_M  (EMAC_CH2_RIWT_V << EMAC_CH2_RIWT_S)
#define EMAC_CH2_RIWT_V  0x000000FFU
#define EMAC_CH2_RIWT_S  0

/** EMAC_CHANNEL2CURRENTHOSTTRANSMITDESCRIPTOR_REG register
 *  Points to the start of current Transmit Descriptor read by the DMA
 */
#define EMAC_CHANNEL2CURRENTHOSTTRANSMITDESCRIPTOR_REG (DR_REG_EMAC_BASE + 0x1248)
/** EMAC_CH2_CURTDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Descriptor Address Pointer
 */
#define EMAC_CH2_CURTDESAPTR    0xFFFFFFFFU
#define EMAC_CH2_CURTDESAPTR_M  (EMAC_CH2_CURTDESAPTR_V << EMAC_CH2_CURTDESAPTR_S)
#define EMAC_CH2_CURTDESAPTR_V  0xFFFFFFFFU
#define EMAC_CH2_CURTDESAPTR_S  0

/** EMAC_CHANNEL2CURRENTHOSTRECEIVEDESCRIPTOR_REG register
 *  Points to the start of current Receive Descriptor read by the DMA
 */
#define EMAC_CHANNEL2CURRENTHOSTRECEIVEDESCRIPTOR_REG (DR_REG_EMAC_BASE + 0x124c)
/** EMAC_CH2_CURRDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Descriptor Address Pointer
 */
#define EMAC_CH2_CURRDESAPTR    0xFFFFFFFFU
#define EMAC_CH2_CURRDESAPTR_M  (EMAC_CH2_CURRDESAPTR_V << EMAC_CH2_CURRDESAPTR_S)
#define EMAC_CH2_CURRDESAPTR_V  0xFFFFFFFFU
#define EMAC_CH2_CURRDESAPTR_S  0

/** EMAC_CHANNEL2CURRENTHOSTTRANSMITBUFFERADDRESS_REG register
 *  Points to the current Transmit Buffer address read by the DMA
 */
#define EMAC_CHANNEL2CURRENTHOSTTRANSMITBUFFERADDRESS_REG (DR_REG_EMAC_BASE + 0x1250)
/** EMAC_CH2_CURTBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Buffer Address Pointer
 */
#define EMAC_CH2_CURTBUFAPTR    0xFFFFFFFFU
#define EMAC_CH2_CURTBUFAPTR_M  (EMAC_CH2_CURTBUFAPTR_V << EMAC_CH2_CURTBUFAPTR_S)
#define EMAC_CH2_CURTBUFAPTR_V  0xFFFFFFFFU
#define EMAC_CH2_CURTBUFAPTR_S  0

/** EMAC_CHANNEL2CURRENTHOSTRECEIVEBUFFERADDRESS_REG register
 *  Points to the current Receive Buffer address read by the DMA
 */
#define EMAC_CHANNEL2CURRENTHOSTRECEIVEBUFFERADDRESS_REG (DR_REG_EMAC_BASE + 0x1254)
/** EMAC_CH2_CURRBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Buffer Address Pointer
 */
#define EMAC_CH2_CURRBUFAPTR    0xFFFFFFFFU
#define EMAC_CH2_CURRBUFAPTR_M  (EMAC_CH2_CURRBUFAPTR_V << EMAC_CH2_CURRBUFAPTR_S)
#define EMAC_CH2_CURRBUFAPTR_V  0xFFFFFFFFU
#define EMAC_CH2_CURRBUFAPTR_S  0

/** EMAC_CHANNEL2CBSCONTROL_REG register
 *  Controls the Channel 2 credit shaping operation on the transmit path
 */
#define EMAC_CHANNEL2CBSCONTROL_REG (DR_REG_EMAC_BASE + 0x1260)
/** EMAC_CH2_CBSD : R/W; bitpos: [0]; default: 0;
 *  CreditBased Shaper Disable When set, the MAC disables the creditbased shaper
 *  algorithm for Channel 2 traffic and makes the traffic management algorithm to
 *  strict priority for Channel 2 over Channel 0 When reset, the creditbased shaper
 *  algorithm schedules the traffic in Channel 2 for transmission
 */
#define EMAC_CH2_CBSD    (BIT(0))
#define EMAC_CH2_CBSD_M  (EMAC_CH2_CBSD_V << EMAC_CH2_CBSD_S)
#define EMAC_CH2_CBSD_V  0x00000001U
#define EMAC_CH2_CBSD_S  0
/** EMAC_CH2_CC : R/W; bitpos: [1]; default: 0;
 *  Credit Control When reset, the accumulated credit parameter in the creditbased
 *  shaper algorithm logic is set to zero when there is positive credit and no frame to
 *  transmit in Channel 2 When there is no frame waiting in Channel 2 and other channel
 *  is transmitting, no credit is accumulated When set, the accumulated credit
 *  parameter in the creditbased shaper algorithm logic is not reset to zero when there
 *  is positive credit and no frame to transmit in Channel 2 The credit accumulates
 *  even when there is no frame waiting in Channel 2 and another channel is transmitting
 */
#define EMAC_CH2_CC    (BIT(1))
#define EMAC_CH2_CC_M  (EMAC_CH2_CC_V << EMAC_CH2_CC_S)
#define EMAC_CH2_CC_V  0x00000001U
#define EMAC_CH2_CC_S  1
/** EMAC_CH2_SLC : R/W; bitpos: [6:4]; default: 0;
 *  Slot Count The software can program the number of slots _of duration 125 microsec_
 *  over which the average transmitted bits per slot _provided in the CBS Status
 *  register_ need to be computed for Channel 2 when the creditbased shaper algorithm
 *  is enabled The encoding is as follows: 3'b000: 1 Slot 3'b001: 2 Slots 3'b010: 4
 *  Slots 3'b011: 8 Slots 3'b100: 16 Slots 3'b1013'b111: Reserved
 */
#define EMAC_CH2_SLC    0x00000007U
#define EMAC_CH2_SLC_M  (EMAC_CH2_SLC_V << EMAC_CH2_SLC_S)
#define EMAC_CH2_SLC_V  0x00000007U
#define EMAC_CH2_SLC_S  4
/** EMAC_CH2_ABPSSIE : R/W; bitpos: [17]; default: 0;
 *  Average Bits Per Slot Interrupt Enable When this bit is set, the MAC asserts an
 *  interrupt _sbd_intr_o or mci_intr_o_ when the average bits per slot status is
 *  updated _Bit 17 _ABSU_ in Register 153_ for Channel 2 When this bit is cleared,
 *  interrupt is not asserted for such an event
 */
#define EMAC_CH2_ABPSSIE    (BIT(17))
#define EMAC_CH2_ABPSSIE_M  (EMAC_CH2_ABPSSIE_V << EMAC_CH2_ABPSSIE_S)
#define EMAC_CH2_ABPSSIE_V  0x00000001U
#define EMAC_CH2_ABPSSIE_S  17

/** EMAC_CHANNEL2CBSSTATUS_REG register
 *  Provides the average traffic transmitted in Channel 2
 */
#define EMAC_CHANNEL2CBSSTATUS_REG (DR_REG_EMAC_BASE + 0x1264)
/** EMAC_CH2_ABS : RO; bitpos: [16:0]; default: 0;
 *  Average Bits per Slot This field contains the average transmitted bits per slot
 *  This field is computed over programmed number of slots _SLC bits in the CBS Control
 *  Register_ for Channel 2 traffic The maximum value is 0x30D4 for 100 Mbps and
 *  0x1E848 for 1000 Mbps
 */
#define EMAC_CH2_ABS    0x0001FFFFU
#define EMAC_CH2_ABS_M  (EMAC_CH2_ABS_V << EMAC_CH2_ABS_S)
#define EMAC_CH2_ABS_V  0x0001FFFFU
#define EMAC_CH2_ABS_S  0
/** EMAC_CH2_ABSU : RO; bitpos: [17]; default: 0;
 *  ABS Updated When set, this bit indicates that the MAC has updated the ABS value
 *  This bit is cleared when the application reads the ABS value
 */
#define EMAC_CH2_ABSU    (BIT(17))
#define EMAC_CH2_ABSU_M  (EMAC_CH2_ABSU_V << EMAC_CH2_ABSU_S)
#define EMAC_CH2_ABSU_V  0x00000001U
#define EMAC_CH2_ABSU_S  17

/** EMAC_CHANNEL2IDLESLOPECREDIT_REG register
 *  Contains the idleSlope credit value required for the creditbased shaper algorithm
 *  for Channel 2
 */
#define EMAC_CHANNEL2IDLESLOPECREDIT_REG (DR_REG_EMAC_BASE + 0x1268)
/** EMAC_CH2_ISC : R/W; bitpos: [13:0]; default: 0;
 *  idleSlopeCredit This field contains the idleSlopeCredit value required for the
 *  creditbased shaper algorithm for Channel 2 This is the rate of change of credit in
 *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
 *  credit is increasing The software should program this field with computed credit in
 *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
 *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode
 */
#define EMAC_CH2_ISC    0x00003FFFU
#define EMAC_CH2_ISC_M  (EMAC_CH2_ISC_V << EMAC_CH2_ISC_S)
#define EMAC_CH2_ISC_V  0x00003FFFU
#define EMAC_CH2_ISC_S  0

/** EMAC_CHANNEL2SENDSLOPECREDIT_REG register
 *  Contains the sendSlope credit value required for the creditbased shaper algorithm
 *  for Channel 2
 */
#define EMAC_CHANNEL2SENDSLOPECREDIT_REG (DR_REG_EMAC_BASE + 0x126c)
/** EMAC_CH2_SSC : R/W; bitpos: [13:0]; default: 0;
 *  sendSlopeCredit This field contains the sendSlopeCredit value required for
 *  creditbased shaper algorithm for Channel 2 This is the rate of change of credit in
 *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
 *  credit is decreasing The software should program this field with computed credit in
 *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
 *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode This field should be
 *  programmed with absolute sendSlopeCredit value The creditbased shaper logic
 *  subtracts it from the accumulated credit when Channel 2 is selected for transmission
 */
#define EMAC_CH2_SSC    0x00003FFFU
#define EMAC_CH2_SSC_M  (EMAC_CH2_SSC_V << EMAC_CH2_SSC_S)
#define EMAC_CH2_SSC_V  0x00003FFFU
#define EMAC_CH2_SSC_S  0

/** EMAC_CHANNEL2HICREDIT_REG register
 *  Contains the hiCredit value required for the creditbased shaper algorithm for
 *  Channel 2
 */
#define EMAC_CHANNEL2HICREDIT_REG (DR_REG_EMAC_BASE + 0x1270)
/** EMAC_CH2_HC : R/W; bitpos: [28:0]; default: 0;
 *  hiCredit This field contains the hiCredit value required for the creditbased shaper
 *  algorithm for Channel 2 This is the maximum value that can be accumulated in the
 *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
 *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
 *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
 *  0x0800_0000
 */
#define EMAC_CH2_HC    0x1FFFFFFFU
#define EMAC_CH2_HC_M  (EMAC_CH2_HC_V << EMAC_CH2_HC_S)
#define EMAC_CH2_HC_V  0x1FFFFFFFU
#define EMAC_CH2_HC_S  0

/** EMAC_CHANNEL2LOCREDIT_REG register
 *  Contains the loCredit value required for the creditbased shaper algorithm for
 *  Channel 2
 */
#define EMAC_CHANNEL2LOCREDIT_REG (DR_REG_EMAC_BASE + 0x1274)
/** EMAC_CH2_LC : R/W; bitpos: [28:0]; default: 536870911;
 *  loCredit This field contains the loCredit value required for the creditbased shaper
 *  algorithm for Channel 2 This is the minimum value that can be accumulated in the
 *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
 *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
 *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
 *  0x0800_0000 The programmed value is 2's complement _negative number_, that is,
 *  0xF800_0000
 */
#define EMAC_CH2_LC    0x1FFFFFFFU
#define EMAC_CH2_LC_M  (EMAC_CH2_LC_V << EMAC_CH2_LC_S)
#define EMAC_CH2_LC_V  0x1FFFFFFFU
#define EMAC_CH2_LC_S  0

#ifdef __cplusplus
}
#endif
