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

/** GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG register
 *  This is the operation mode register for the MAC
 */
#define GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG (DR_REG_GMAC_BASE + 0x0)
/** GMAC_PRELEN : R/W; bitpos: [1:0]; default: 0;
 *  Preamble Length for Transmit frames These bits control the number of preamble bytes
 *  that are added to the beginning of every Transmit frame The preamble reduction
 *  occurs only when the MAC is operating in the fullduplex mode 2'b00: 7 bytes of
 *  preamble 2'b01: 5 bytes of preamble 2'b10: 3 bytes of preamble 2'b11: Reserved
 */
#define GMAC_PRELEN    0x00000003U
#define GMAC_PRELEN_M  (GMAC_PRELEN_V << GMAC_PRELEN_S)
#define GMAC_PRELEN_V  0x00000003U
#define GMAC_PRELEN_S  0
/** GMAC_RE : R/W; bitpos: [2]; default: 0;
 *  Receiver Enable When this bit is set, the receiver state machine of the MAC is
 *  enabled for receiving frames from the GMII or MII When this bit is reset, the MAC
 *  receive state machine is disabled after the completion of the reception of the
 *  current frame, and does not receive any further frames from the GMII or MII
 */
#define GMAC_RE    (BIT(2))
#define GMAC_RE_M  (GMAC_RE_V << GMAC_RE_S)
#define GMAC_RE_V  0x00000001U
#define GMAC_RE_S  2
/** GMAC_TE : R/W; bitpos: [3]; default: 0;
 *  Transmitter Enable When this bit is set, the transmit state machine of the MAC is
 *  enabled for transmission on the GMII or MII When this bit is reset, the MAC
 *  transmit state machine is disabled after the completion of the transmission of the
 *  current frame, and does not transmit any further frames
 */
#define GMAC_TE    (BIT(3))
#define GMAC_TE_M  (GMAC_TE_V << GMAC_TE_S)
#define GMAC_TE_V  0x00000001U
#define GMAC_TE_S  3
/** GMAC_DC : R/W; bitpos: [4]; default: 0;
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
#define GMAC_DC    (BIT(4))
#define GMAC_DC_M  (GMAC_DC_V << GMAC_DC_S)
#define GMAC_DC_V  0x00000001U
#define GMAC_DC_S  4
/** GMAC_BL : R/W; bitpos: [6:5]; default: 0;
 *  BackOff Limit The BackOff limit determines the random integer number _r_ of slot
 *  time delays _4,096 bit times for 1000 Mbps and 512 bit times for 10/100 Mbps_ for
 *  which the MAC waits before rescheduling a transmission attempt during retries after
 *  a collision This bit is applicable only in the halfduplex mode and is reserved _RO_
 *  in the fullduplexonly configuration 00: k= min _n, 10_ 01: k = min _n, 8_ 10: k =
 *  min _n, 4_ 11: k = min _n, 1_ where n = retransmission attempt The random integer r
 *  takes the value in the range 0 ≤ r < 2k
 */
#define GMAC_BL    0x00000003U
#define GMAC_BL_M  (GMAC_BL_V << GMAC_BL_S)
#define GMAC_BL_V  0x00000003U
#define GMAC_BL_S  5
/** GMAC_ACS : R/W; bitpos: [7]; default: 0;
 *  Automatic Pad or CRC Stripping When this bit is set, the MAC strips the Pad or FCS
 *  field on the incoming frames only if the value of the length field is less than
 *  1,536 bytes All received frames with length field greater than or equal to 1,536
 *  bytes are passed to the application without stripping the Pad or FCS field When
 *  this bit is reset, the MAC passes all incoming frames, without modifying them, to
 *  the Host Note: For information about how the settings of Bit 25 _CST_ and this bit
 *  impact the frame length, see Table 632
 */
#define GMAC_ACS    (BIT(7))
#define GMAC_ACS_M  (GMAC_ACS_V << GMAC_ACS_S)
#define GMAC_ACS_V  0x00000001U
#define GMAC_ACS_S  7
/** GMAC_LUD : R/W; bitpos: [8]; default: 0;
 *  Link Up or Down This bit indicates whether the link is up or down during the
 *  transmission of configuration in the RGMII, SGMII, or SMII interface: 0: Link Down
 *  1: Link Up This bit is reserved _RO with default value_ and is enabled when the
 *  RGMII, SGMII, or SMII interface is enabled during core configuration
 */
#define GMAC_LUD    (BIT(8))
#define GMAC_LUD_M  (GMAC_LUD_V << GMAC_LUD_S)
#define GMAC_LUD_V  0x00000001U
#define GMAC_LUD_S  8
/** GMAC_DR : R/W; bitpos: [9]; default: 0;
 *  Disable Retry When this bit is set, the MAC attempts only one transmission When a
 *  collision occurs on the GMII or MII interface, the MAC ignores the current frame
 *  transmission and reports a Frame Abort with excessive collision error in the
 *  transmit frame status When this bit is reset, the MAC attempts retries based on the
 *  settings of the BL field _Bits [6:5]_ This bit is applicable only in the halfduplex
 *  mode and is reserved _RO with default value_ in the fullduplexonly configuration
 */
#define GMAC_DR    (BIT(9))
#define GMAC_DR_M  (GMAC_DR_V << GMAC_DR_S)
#define GMAC_DR_V  0x00000001U
#define GMAC_DR_S  9
/** GMAC_IPC : R/W; bitpos: [10]; default: 0;
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
#define GMAC_IPC    (BIT(10))
#define GMAC_IPC_M  (GMAC_IPC_V << GMAC_IPC_S)
#define GMAC_IPC_V  0x00000001U
#define GMAC_IPC_S  10
/** GMAC_DM : R/W; bitpos: [11]; default: 0;
 *  Duplex Mode When this bit is set, the MAC operates in the fullduplex mode where it
 *  can transmit and receive simultaneously This bit is RO with default value of 1'b1
 *  in the fullduplexonly configuration
 */
#define GMAC_DM    (BIT(11))
#define GMAC_DM_M  (GMAC_DM_V << GMAC_DM_S)
#define GMAC_DM_V  0x00000001U
#define GMAC_DM_S  11
/** GMAC_LM : R/W; bitpos: [12]; default: 0;
 *  Loopback Mode When this bit is set, the MAC operates in the loopback mode at GMII
 *  or MII The _G_MII Receive clock input _clk_rx_i_ is required for the loopback to
 *  work properly, because the Transmit clock is not loopedback internally
 */
#define GMAC_LM    (BIT(12))
#define GMAC_LM_M  (GMAC_LM_V << GMAC_LM_S)
#define GMAC_LM_V  0x00000001U
#define GMAC_LM_S  12
/** GMAC_DO : R/W; bitpos: [13]; default: 0;
 *  Disable Receive Own When this bit is set, the MAC disables the reception of frames
 *  when the phy_txen_o is asserted in the halfduplex mode When this bit is reset, the
 *  MAC receives all packets that are given by the PHY while transmitting This bit is
 *  not applicable if the MAC is operating in the fullduplex mode This bit is reserved
 *  _RO with default value_ if the MAC is configured for the fullduplexonly operation
 */
#define GMAC_DO    (BIT(13))
#define GMAC_DO_M  (GMAC_DO_V << GMAC_DO_S)
#define GMAC_DO_V  0x00000001U
#define GMAC_DO_S  13
/** GMAC_FES : R/W; bitpos: [14]; default: 0;
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
#define GMAC_FES    (BIT(14))
#define GMAC_FES_M  (GMAC_FES_V << GMAC_FES_S)
#define GMAC_FES_V  0x00000001U
#define GMAC_FES_S  14
/** GMAC_PS : R/W; bitpos: [15]; default: 0;
 *  Port Select This bit selects the Ethernet line speed 0: For 1000 Mbps operations 1:
 *  For 10 or 100 Mbps operations In 10 or 100 Mbps operations, this bit, along with
 *  FES bit, selects the exact line speed In the 10/100 Mbpsonly _always 1_ or 1000
 *  Mbpsonly _always 0_ configurations, this bit is readonly with the appropriate value
 *  In default 10/100/1000 Mbps configuration, this bit is R_W The mac_portselect_o or
 *  mac_speed_o[1] signal reflects the value of this bit
 */
#define GMAC_PS    (BIT(15))
#define GMAC_PS_M  (GMAC_PS_V << GMAC_PS_S)
#define GMAC_PS_V  0x00000001U
#define GMAC_PS_S  15
/** GMAC_DCRS : R/W; bitpos: [16]; default: 0;
 *  Disable Carrier Sense During Transmission When set high, this bit makes the MAC
 *  transmitter ignore the _G_MII CRS signal during frame transmission in the
 *  halfduplex mode This request results in no errors generated because of Loss of
 *  Carrier or No Carrier during such transmission When this bit is low, the MAC
 *  transmitter generates such errors because of Carrier Sense and can even abort the
 *  transmissions This bit is reserved _and RO_ in the fullduplexonly configurations
 */
#define GMAC_DCRS    (BIT(16))
#define GMAC_DCRS_M  (GMAC_DCRS_V << GMAC_DCRS_S)
#define GMAC_DCRS_V  0x00000001U
#define GMAC_DCRS_S  16
/** GMAC_IFG : R/W; bitpos: [19:17]; default: 0;
 *  InterFrame Gap These bits control the minimum IFG between frames during
 *  transmission 000: 96 bit times 001: 88 bit times 010: 80 bit times  111: 40 bit
 *  times In the halfduplex mode, the minimum IFG can be configured only for 64 bit
 *  times _IFG = 100_ Lower values are not considered In the 1000Mbps mode, the minimum
 *  IFG supported is 64 bit times _and above_ in the GMACCORE configuration and 80 bit
 *  times _and above_ in other configurations When a JAM pattern is being transmitted
 *  because of backpressure activation, the MAC does not consider the minimum IFG
 */
#define GMAC_IFG    0x00000007U
#define GMAC_IFG_M  (GMAC_IFG_V << GMAC_IFG_S)
#define GMAC_IFG_V  0x00000007U
#define GMAC_IFG_S  17
/** GMAC_JE : R/W; bitpos: [20]; default: 0;
 *  Jumbo Frame Enable When this bit is set, the MAC allows Jumbo frames of 9,018 bytes
 *  _9,022 bytes for VLAN tagged frames_ without reporting a giant frame error in the
 *  receive frame status
 */
#define GMAC_JE    (BIT(20))
#define GMAC_JE_M  (GMAC_JE_V << GMAC_JE_S)
#define GMAC_JE_V  0x00000001U
#define GMAC_JE_S  20
/** GMAC_BE : R/W; bitpos: [21]; default: 0;
 *  Frame Burst Enable When this bit is set, the MAC allows frame bursting during
 *  transmission in the GMII halfduplex mode This bit is reserved _and RO_ in the
 *  10/100 Mbps only or fullduplexonly configurations
 */
#define GMAC_BE    (BIT(21))
#define GMAC_BE_M  (GMAC_BE_V << GMAC_BE_S)
#define GMAC_BE_V  0x00000001U
#define GMAC_BE_S  21
/** GMAC_JD : R/W; bitpos: [22]; default: 0;
 *  Jabber Disable When this bit is set, the MAC disables the jabber timer on the
 *  transmitter The MAC can transfer frames of up to 16,383 bytes When this bit is
 *  reset, the MAC cuts off the transmitter if the application sends out more than
 *  2,048 bytes of data _10,240 if JE is set high_ during transmission
 */
#define GMAC_JD    (BIT(22))
#define GMAC_JD_M  (GMAC_JD_V << GMAC_JD_S)
#define GMAC_JD_V  0x00000001U
#define GMAC_JD_S  22
/** GMAC_WD : R/W; bitpos: [23]; default: 0;
 *  Watchdog Disable When this bit is set, the MAC disables the watchdog timer on the
 *  receiver The MAC can receive frames of up to 16,383 bytes When this bit is reset,
 *  the MAC does not allow a receive frame which more than 2,048 bytes _10,240 if JE is
 *  set high_ or the value programmed in Register 55 _Watchdog Timeout Register_ The
 *  MAC cuts off any bytes received after the watchdog limit number of bytes
 */
#define GMAC_WD    (BIT(23))
#define GMAC_WD_M  (GMAC_WD_V << GMAC_WD_S)
#define GMAC_WD_V  0x00000001U
#define GMAC_WD_S  23
/** GMAC_TC : R/W; bitpos: [24]; default: 0;
 *  Transmit Configuration in RGMII, SGMII, or SMII When set, this bit enables the
 *  transmission of duplex mode, link speed, and link up or down information to the PHY
 *  in the RGMII, SMII, or SGMII port When this bit is reset, no such information is
 *  driven to the PHY This bit is reserved _and RO_ if the RGMII, SMII, or SGMII PHY
 *  port is not selected during core configuration The details of this feature are
 *  explained in the following sections: “Reduced Gigabit Media Independent Interface”
 *  on page 249 “Serial Media Independent Interface” on page 245 “Serial Gigabit Media
 *  Independent Interface” on page 257
 */
#define GMAC_TC    (BIT(24))
#define GMAC_TC_M  (GMAC_TC_V << GMAC_TC_S)
#define GMAC_TC_V  0x00000001U
#define GMAC_TC_S  24
/** GMAC_CST : R/W; bitpos: [25]; default: 0;
 *  CRC Stripping for Type Frames When this bit is set, the last 4 bytes _FCS_ of all
 *  frames of Ether type _Length/Type field greater than or equal to 1,536_ are
 *  stripped and dropped before forwarding the frame to the application This function
 *  is not valid when the IP Checksum Engine _Type 1_ is enabled in the MAC receiver
 *  This function is valid when Type 2 Checksum Offload Engine is enabled Note: For
 *  information about how the settings of Bit 7 _ACS_ and this bit impact the frame
 *  length, see Table 632
 */
#define GMAC_CST    (BIT(25))
#define GMAC_CST_M  (GMAC_CST_V << GMAC_CST_S)
#define GMAC_CST_V  0x00000001U
#define GMAC_CST_S  25
/** GMAC_SFTERR : R/W; bitpos: [26]; default: 0;
 *  SMII Force Transmit Error When set, this bit indicates to the PHY to force a
 *  transmit error in the SMII frame being transmitted This bit is reserved if the SMII
 *  PHY port is not selected during core configuration
 */
#define GMAC_SFTERR    (BIT(26))
#define GMAC_SFTERR_M  (GMAC_SFTERR_V << GMAC_SFTERR_S)
#define GMAC_SFTERR_V  0x00000001U
#define GMAC_SFTERR_S  26
/** GMAC_TWOKPE : R/W; bitpos: [27]; default: 0;
 *  IEEE 8023as Support for 2K Packets When set, the MAC considers all frames, with up
 *  to 2,000 bytes length, as normal packets When Bit 20 _JE_ is not set, the MAC
 *  considers all received frames of size more than 2K bytes as Giant frames When this
 *  bit is reset and Bit 20 _JE_ is not set, the MAC considers all received frames of
 *  size more than 1,518 bytes _1,522 bytes for tagged_ as Giant frames When Bit 20 is
 *  set, setting this bit has no effect on Giant Frame status For more information
 *  about how the setting of this bit and Bit 20 impact the Giant frame status, see
 *  Table 631
 */
#define GMAC_TWOKPE    (BIT(27))
#define GMAC_TWOKPE_M  (GMAC_TWOKPE_V << GMAC_TWOKPE_S)
#define GMAC_TWOKPE_V  0x00000001U
#define GMAC_TWOKPE_S  27
/** GMAC_SARC : R/W; bitpos: [30:28]; default: 0;
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
#define GMAC_SARC    0x00000007U
#define GMAC_SARC_M  (GMAC_SARC_V << GMAC_SARC_S)
#define GMAC_SARC_V  0x00000007U
#define GMAC_SARC_S  28

/** GMAC_REGISTER1_MACFRAMEFILTER_REG register
 *  Contains the frame filtering controls
 */
#define GMAC_REGISTER1_MACFRAMEFILTER_REG (DR_REG_GMAC_BASE + 0x4)
/** GMAC_PROMISCUOUS_MODE : R/W; bitpos: [0]; default: 0;
 *  Promiscuous Mode When this bit is set, the Address Filter module passes all
 *  incoming frames irrespective of the destination or source address The SA or DA
 *  Filter Fails status bits of the Receive Status Word are always cleared when PR is
 *  set
 */
#define GMAC_PROMISCUOUS_MODE    (BIT(0))
#define GMAC_PROMISCUOUS_MODE_M  (GMAC_PROMISCUOUS_MODE_V << GMAC_PROMISCUOUS_MODE_S)
#define GMAC_PROMISCUOUS_MODE_V  0x00000001U
#define GMAC_PROMISCUOUS_MODE_S  0
/** GMAC_HUC : R/W; bitpos: [1]; default: 0;
 *  Hash Unicast When set, the MAC performs destination address filtering of unicast
 *  frames according to the hash table When reset, the MAC performs a perfect
 *  destination address filtering for unicast frames, that is, it compares the DA field
 *  with the values programmed in DA registers If Hash Filter is not selected during
 *  core configuration, this bit is reserved _and RO_
 */
#define GMAC_HUC    (BIT(1))
#define GMAC_HUC_M  (GMAC_HUC_V << GMAC_HUC_S)
#define GMAC_HUC_V  0x00000001U
#define GMAC_HUC_S  1
/** GMAC_HMC : R/W; bitpos: [2]; default: 0;
 *  Hash Multicast When set, the MAC performs destination address filtering of received
 *  multicast frames according to the hash table When reset, the MAC performs a perfect
 *  destination address filtering for multicast frames, that is, it compares the DA
 *  field with the values programmed in DA registers If Hash Filter is not selected
 *  during core configuration, this bit is reserved _and RO_
 */
#define GMAC_HMC    (BIT(2))
#define GMAC_HMC_M  (GMAC_HMC_V << GMAC_HMC_S)
#define GMAC_HMC_V  0x00000001U
#define GMAC_HMC_S  2
/** GMAC_DAIF : R/W; bitpos: [3]; default: 0;
 *  DA Inverse Filtering When this bit is set, the Address Check block operates in
 *  inverse filtering mode for the DA address comparison for both unicast and multicast
 *  frames When reset, normal filtering of frames is performed
 */
#define GMAC_DAIF    (BIT(3))
#define GMAC_DAIF_M  (GMAC_DAIF_V << GMAC_DAIF_S)
#define GMAC_DAIF_V  0x00000001U
#define GMAC_DAIF_S  3
/** GMAC_PM : R/W; bitpos: [4]; default: 0;
 *  Pass All Multicast When set, this bit indicates that all received frames with a
 *  multicast destination address _first bit in the destination address field is '1'_
 *  are passed When reset, filtering of multicast frame depends on HMC bit
 */
#define GMAC_PM    (BIT(4))
#define GMAC_PM_M  (GMAC_PM_V << GMAC_PM_S)
#define GMAC_PM_V  0x00000001U
#define GMAC_PM_S  4
/** GMAC_DBF : R/W; bitpos: [5]; default: 0;
 *  Disable Broadcast Frames When this bit is set, the AFM module blocks all incoming
 *  broadcast frames In addition, it overrides all other filter settings When this bit
 *  is reset, the AFM module passes all received broadcast frames
 */
#define GMAC_DBF    (BIT(5))
#define GMAC_DBF_M  (GMAC_DBF_V << GMAC_DBF_S)
#define GMAC_DBF_V  0x00000001U
#define GMAC_DBF_S  5
/** GMAC_PCF : R/W; bitpos: [7:6]; default: 0;
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
#define GMAC_PCF    0x00000003U
#define GMAC_PCF_M  (GMAC_PCF_V << GMAC_PCF_S)
#define GMAC_PCF_V  0x00000003U
#define GMAC_PCF_S  6
/** GMAC_SAIF : R/W; bitpos: [8]; default: 0;
 *  SA Inverse Filtering When this bit is set, the Address Check block operates in
 *  inverse filtering mode for the SA address comparison The frames whose SA matches
 *  the SA registers are marked as failing the SA Address filter When this bit is
 *  reset, frames whose SA does not match the SA registers are marked as failing the SA
 *  Address filter
 */
#define GMAC_SAIF    (BIT(8))
#define GMAC_SAIF_M  (GMAC_SAIF_V << GMAC_SAIF_S)
#define GMAC_SAIF_V  0x00000001U
#define GMAC_SAIF_S  8
/** GMAC_SAF : R/W; bitpos: [9]; default: 0;
 *  Source Address Filter Enable When this bit is set, the MAC compares the SA field of
 *  the received frames with the values programmed in the enabled SA registers If the
 *  comparison fails, the MAC drops the frame When this bit is reset, the MAC forwards
 *  the received frame to the application with updated SAF bit of the Rx Status
 *  depending on the SA address comparison Note: According to the IEEE specification,
 *  Bit 47 of the SA is reserved and set to 0 However, in DWC_gmac, the MAC compares
 *  all 48 bits The software driver should take this into consideration while
 *  programming the MAC address registers for SA
 */
#define GMAC_SAF    (BIT(9))
#define GMAC_SAF_M  (GMAC_SAF_V << GMAC_SAF_S)
#define GMAC_SAF_V  0x00000001U
#define GMAC_SAF_S  9
/** GMAC_HPF : R/W; bitpos: [10]; default: 0;
 *  Hash or Perfect Filter When this bit is set, it configures the address filter to
 *  pass a frame if it matches either the perfect filtering or the hash filtering as
 *  set by the HMC or HUC bits When this bit is low and the HUC or HMC bit is set, the
 *  frame is passed only if it matches the Hash filter This bit is reserved _and RO_ if
 *  the Hash filter is not selected during core configuration
 */
#define GMAC_HPF    (BIT(10))
#define GMAC_HPF_M  (GMAC_HPF_V << GMAC_HPF_S)
#define GMAC_HPF_V  0x00000001U
#define GMAC_HPF_S  10
/** GMAC_VTFE : R/W; bitpos: [16]; default: 0;
 *  VLAN Tag Filter Enable When set, this bit enables the MAC to drop VLAN tagged
 *  frames that do not match the VLAN Tag comparison When reset, the MAC forwards all
 *  frames irrespective of the match status of the VLAN Tag
 */
#define GMAC_VTFE    (BIT(16))
#define GMAC_VTFE_M  (GMAC_VTFE_V << GMAC_VTFE_S)
#define GMAC_VTFE_V  0x00000001U
#define GMAC_VTFE_S  16
/** GMAC_IPFE : R/W; bitpos: [20]; default: 0;
 *  Layer 3 and Layer 4 Filter Enable When set, this bit enables the MAC to drop frames
 *  that do not match the enabled Layer 3 and Layer 4 filters If Layer 3 or Layer 4
 *  filters are not enabled for matching, this bit does not have any effect When reset,
 *  the MAC forwards all frames irrespective of the match status of the Layer 3 and
 *  Layer 4 fields If the Layer 3 and Layer 4 Filtering feature is not selected during
 *  core configuration, this bit is reserved _RO with default value_
 */
#define GMAC_IPFE    (BIT(20))
#define GMAC_IPFE_M  (GMAC_IPFE_V << GMAC_IPFE_S)
#define GMAC_IPFE_V  0x00000001U
#define GMAC_IPFE_S  20
/** GMAC_DNTU : R/W; bitpos: [21]; default: 0;
 *  Drop nonTCP/UDP over IP Frames When set, this bit enables the MAC to drop the
 *  nonTCP or UDP over IP frames The MAC forward only those frames that are processed
 *  by the Layer 4 filter When reset, this bit enables the MAC to forward all nonTCP or
 *  UDP over IP frames If the Layer 3 and Layer 4 Filtering feature is not selected
 *  during core configuration, this bit is reserved _RO with default value_
 */
#define GMAC_DNTU    (BIT(21))
#define GMAC_DNTU_M  (GMAC_DNTU_V << GMAC_DNTU_S)
#define GMAC_DNTU_V  0x00000001U
#define GMAC_DNTU_S  21
/** GMAC_RA : R/W; bitpos: [31]; default: 0;
 *  Receive All When this bit is set, the MAC Receiver module passes all received
 *  frames, irrespective of whether they pass the address filter or not, to the
 *  Application The result of the SA or DA filtering is updated _pass or fail_ in the
 *  corresponding bits in the Receive Status Word When this bit is reset, the Receiver
 *  module passes only those frames to the Application that pass the SA or DA address
 *  filter
 */
#define GMAC_RA    (BIT(31))
#define GMAC_RA_M  (GMAC_RA_V << GMAC_RA_S)
#define GMAC_RA_V  0x00000001U
#define GMAC_RA_S  31

/** GMAC_REGISTER2_HASHTABLEHIGHREGISTER_REG register
 *  Contains the higher 32 bits of the Multicast Hash table This register is present
 *  only when you select the 64bit Hash filter function in coreConsultant _See Table 79_
 */
#define GMAC_REGISTER2_HASHTABLEHIGHREGISTER_REG (DR_REG_GMAC_BASE + 0x8)
/** GMAC_HTH : R/W; bitpos: [31:0]; default: 0;
 *  Hash Table High This field contains the upper 32 bits of the Hash table
 */
#define GMAC_HTH    0xFFFFFFFFU
#define GMAC_HTH_M  (GMAC_HTH_V << GMAC_HTH_S)
#define GMAC_HTH_V  0xFFFFFFFFU
#define GMAC_HTH_S  0

/** GMAC_REGISTER3_HASHTABLELOWREGISTER_REG register
 *  Contains the lower 32 bits of the Multicast Hash table This register is present
 *  only when you select the Hash filter function in coreConsultant _See Table 79_
 */
#define GMAC_REGISTER3_HASHTABLELOWREGISTER_REG (DR_REG_GMAC_BASE + 0xc)
/** GMAC_HTL : R/W; bitpos: [31:0]; default: 0;
 *  Hash Table Low This field contains the lower 32 bits of the Hash table
 */
#define GMAC_HTL    0xFFFFFFFFU
#define GMAC_HTL_M  (GMAC_HTL_V << GMAC_HTL_S)
#define GMAC_HTL_V  0xFFFFFFFFU
#define GMAC_HTL_S  0

/** GMAC_REGISTER4_GMIIADDRESSREGISTER_REG register
 *  Controls the management cycles to an external PHY This register is present only
 *  when you select the Station Management _MDIO_ feature in coreConsultant _See Table
 *  726_
 */
#define GMAC_REGISTER4_GMIIADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x10)
/** GMAC_GB : R/W1S; bitpos: [0]; default: 0;
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
#define GMAC_GB    (BIT(0))
#define GMAC_GB_M  (GMAC_GB_V << GMAC_GB_S)
#define GMAC_GB_V  0x00000001U
#define GMAC_GB_S  0
/** GMAC_GW : R/W; bitpos: [1]; default: 0;
 *  GMII Write When set, this bit indicates to the PHY or RevMII that this is a Write
 *  operation using the GMII Data register If this bit is not set, it indicates that
 *  this is a Read operation, that is, placing the data in the GMII Data register
 */
#define GMAC_GW    (BIT(1))
#define GMAC_GW_M  (GMAC_GW_V << GMAC_GW_S)
#define GMAC_GW_V  0x00000001U
#define GMAC_GW_S  1
/** GMAC_CR : R/W; bitpos: [5:2]; default: 0;
 *  CSR Clock Range The CSR Clock Range selection determines the frequency of the MDC
 *  clock according to the CSR clock frequency used in your design The CSR clock
 *  corresponding to different GMAC configurations is given in Table 92 on page 564 The
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
#define GMAC_CR    0x0000000FU
#define GMAC_CR_M  (GMAC_CR_V << GMAC_CR_S)
#define GMAC_CR_V  0x0000000FU
#define GMAC_CR_S  2
/** GMAC_GR : R/W; bitpos: [10:6]; default: 0;
 *  GMII Register These bits select the desired GMII register in the selected PHY
 *  device For RevMII, these bits select the desired CSR register in the RevMII
 *  Registers set
 */
#define GMAC_GR    0x0000001FU
#define GMAC_GR_M  (GMAC_GR_V << GMAC_GR_S)
#define GMAC_GR_V  0x0000001FU
#define GMAC_GR_S  6
/** GMAC_PA : R/W; bitpos: [15:11]; default: 0;
 *  Physical Layer Address This field indicates which of the 32 possible PHY devices
 *  are being accessed For RevMII, this field gives the PHY Address of the RevMII module
 */
#define GMAC_PA    0x0000001FU
#define GMAC_PA_M  (GMAC_PA_V << GMAC_PA_S)
#define GMAC_PA_V  0x0000001FU
#define GMAC_PA_S  11

/** GMAC_REGISTER5_GMIIDATAREGISTER_REG register
 *  Contains the data to be written to or read from the PHY register This register is
 *  present only when you select the Station Management _MDIO_ feature in
 *  coreConsultant _See Table 726_
 */
#define GMAC_REGISTER5_GMIIDATAREGISTER_REG (DR_REG_GMAC_BASE + 0x14)
/** GMAC_GD : R/W; bitpos: [15:0]; default: 0;
 *  GMII Data This field contains the 16bit data value read from the PHY or RevMII
 *  after a Management Read operation or the 16bit data value to be written to the PHY
 *  or RevMII before a Management Write operation
 */
#define GMAC_GD    0x0000FFFFU
#define GMAC_GD_M  (GMAC_GD_V << GMAC_GD_S)
#define GMAC_GD_V  0x0000FFFFU
#define GMAC_GD_S  0

/** GMAC_REGISTER6_FLOWCONTROLREGISTER_REG register
 *  Controls the generation of control frames
 */
#define GMAC_REGISTER6_FLOWCONTROLREGISTER_REG (DR_REG_GMAC_BASE + 0x18)
/** GMAC_FCB_BPA : R/W1S; bitpos: [0]; default: 0;
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
#define GMAC_FCB_BPA    (BIT(0))
#define GMAC_FCB_BPA_M  (GMAC_FCB_BPA_V << GMAC_FCB_BPA_S)
#define GMAC_FCB_BPA_V  0x00000001U
#define GMAC_FCB_BPA_S  0
/** GMAC_TFE : R/W; bitpos: [1]; default: 0;
 *  Transmit Flow Control Enable In the fullduplex mode, when this bit is set, the MAC
 *  enables the flow control operation to transmit Pause frames When this bit is reset,
 *  the flow control operation in the MAC is disabled, and the MAC does not transmit
 *  any Pause frames In the halfduplex mode, when this bit is set, the MAC enables the
 *  backpressure operation When this bit is reset, the backpressure feature is disabled
 */
#define GMAC_TFE    (BIT(1))
#define GMAC_TFE_M  (GMAC_TFE_V << GMAC_TFE_S)
#define GMAC_TFE_V  0x00000001U
#define GMAC_TFE_S  1
/** GMAC_RECEIVE_FLOW_CTRL_E : R/W; bitpos: [2]; default: 0;
 *  Receive Flow Control Enable When this bit is set, the MAC decodes the received
 *  Pause frame and disables its transmitter for a specified _Pause_ time When this bit
 *  is reset, the decode function of the Pause frame is disabled
 */
#define GMAC_RECEIVE_FLOW_CTRL_E    (BIT(2))
#define GMAC_RECEIVE_FLOW_CTRL_E_M  (GMAC_RECEIVE_FLOW_CTRL_E_V << GMAC_RECEIVE_FLOW_CTRL_E_S)
#define GMAC_RECEIVE_FLOW_CTRL_E_V  0x00000001U
#define GMAC_RECEIVE_FLOW_CTRL_E_S  2
/** GMAC_UP : R/W; bitpos: [3]; default: 0;
 *  Unicast Pause Frame Detect A pause frame is processed when it has the unique
 *  multicast address specified in the IEEE Std 8023 When this bit is set, the MAC can
 *  also detect Pause frames with unicast address of the station This unicast address
 *  should be as specified in the MAC Address0 High Register and MAC Address0 Low
 *  Register When this bit is reset, the MAC only detects Pause frames with unique
 *  multicast address Note: The MAC does not process a Pause frame if the multicast
 *  address of received frame is different from the unique multicast address
 */
#define GMAC_UP    (BIT(3))
#define GMAC_UP_M  (GMAC_UP_V << GMAC_UP_S)
#define GMAC_UP_V  0x00000001U
#define GMAC_UP_S  3
/** GMAC_PLT : R/W; bitpos: [5:4]; default: 0;
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
#define GMAC_PLT    0x00000003U
#define GMAC_PLT_M  (GMAC_PLT_V << GMAC_PLT_S)
#define GMAC_PLT_V  0x00000003U
#define GMAC_PLT_S  4
/** GMAC_DZPQ : R/W; bitpos: [7]; default: 0;
 *  Disable ZeroQuanta Pause When this bit is set, it disables the automatic generation
 *  of the ZeroQuanta Pause frames on the deassertion of the flowcontrol signal from
 *  the FIFO layer _MTL or external sideband flow control signal
 *  sbd_flowctrl_i/mti_flowctrl_i_ When this bit is reset, normal operation with
 *  automatic ZeroQuanta Pause frame generation is enabled
 */
#define GMAC_DZPQ    (BIT(7))
#define GMAC_DZPQ_M  (GMAC_DZPQ_V << GMAC_DZPQ_S)
#define GMAC_DZPQ_V  0x00000001U
#define GMAC_DZPQ_S  7
/** GMAC_PT : R/W; bitpos: [31:16]; default: 0;
 *  Pause Time This field holds the value to be used in the Pause Time field in the
 *  transmit control frame If the Pause Time bits is configured to be
 *  doublesynchronized to the _G_MII clock domain, then consecutive writes to this
 *  register should be performed only after at least four clock cycles in the
 *  destination clock domain
 */
#define GMAC_PT    0x0000FFFFU
#define GMAC_PT_M  (GMAC_PT_V << GMAC_PT_S)
#define GMAC_PT_V  0x0000FFFFU
#define GMAC_PT_S  16

/** GMAC_REGISTER7_VLANTAGREGISTER_REG register
 *  Identifies IEEE 8021Q VLAN type frames
 */
#define GMAC_REGISTER7_VLANTAGREGISTER_REG (DR_REG_GMAC_BASE + 0x1c)
/** GMAC_VL : R/W; bitpos: [15:0]; default: 0;
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
#define GMAC_VL    0x0000FFFFU
#define GMAC_VL_M  (GMAC_VL_V << GMAC_VL_S)
#define GMAC_VL_V  0x0000FFFFU
#define GMAC_VL_S  0
/** GMAC_ETV : R/W; bitpos: [16]; default: 0;
 *  Enable 12-Bit VLAN Tag Comparison
 */
#define GMAC_ETV    (BIT(16))
#define GMAC_ETV_M  (GMAC_ETV_V << GMAC_ETV_S)
#define GMAC_ETV_V  0x00000001U
#define GMAC_ETV_S  16
/** GMAC_VTIM : R/W; bitpos: [17]; default: 0;
 *  VLAN Tag Inverse Match Enable When set, this bit enables the VLAN Tag inverse
 *  matching The frames that do not have matching VLAN Tag are marked as matched When
 *  reset, this bit enables the VLAN Tag perfect matching The frames with matched VLAN
 *  Tag are marked as matched
 */
#define GMAC_VTIM    (BIT(17))
#define GMAC_VTIM_M  (GMAC_VTIM_V << GMAC_VTIM_S)
#define GMAC_VTIM_V  0x00000001U
#define GMAC_VTIM_S  17
/** GMAC_ESVL : R/W; bitpos: [18]; default: 0;
 *  Enable SVLAN When this bit is set, the MAC transmitter and receiver also consider
 *  the SVLAN _Type = 0x88A8_ frames as valid VLAN tagged frames
 */
#define GMAC_ESVL    (BIT(18))
#define GMAC_ESVL_M  (GMAC_ESVL_V << GMAC_ESVL_S)
#define GMAC_ESVL_V  0x00000001U
#define GMAC_ESVL_S  18
/** GMAC_VTHM : R/W; bitpos: [19]; default: 0;
 *  VLAN Tag Hash Table Match Enable When set, the most significant four bits of the
 *  VLAN tag’s CRC are used to index the content of Register 354 _VLAN Hash Table
 *  Register_ A value of 1 in the VLAN Hash Table register, corresponding to the index,
 *  indicates that the frame matched the VLAN hash table When Bit 16 _ETV_ is set, the
 *  CRC of the 12bit VLAN Identifier _VID_ is used for comparison whereas when ETV is
 *  reset, the CRC of the 16bit VLAN tag is used for comparison When reset, the VLAN
 *  Hash Match operation is not performed If the VLAN Hash feature is not enabled
 *  during core configuration, this bit is reserved _RO with default value_
 */
#define GMAC_VTHM    (BIT(19))
#define GMAC_VTHM_M  (GMAC_VTHM_V << GMAC_VTHM_S)
#define GMAC_VTHM_V  0x00000001U
#define GMAC_VTHM_S  19

/** GMAC_REGISTER8_VERSIONREGISTER_REG register
 *  Identifies the version of the Core
 */
#define GMAC_REGISTER8_VERSIONREGISTER_REG (DR_REG_GMAC_BASE + 0x20)
/** GMAC_SNPSVER : RO; bitpos: [7:0]; default: 55;
 *  Synopsysdefined Version _37_
 */
#define GMAC_SNPSVER    0x000000FFU
#define GMAC_SNPSVER_M  (GMAC_SNPSVER_V << GMAC_SNPSVER_S)
#define GMAC_SNPSVER_V  0x000000FFU
#define GMAC_SNPSVER_S  0
/** GMAC_USERVER : RO; bitpos: [15:8]; default: 0;
 *  Userdefined Version _configured with coreConsultant_
 */
#define GMAC_USERVER    0x000000FFU
#define GMAC_USERVER_M  (GMAC_USERVER_V << GMAC_USERVER_S)
#define GMAC_USERVER_V  0x000000FFU
#define GMAC_USERVER_S  8

/** GMAC_REGISTER9_DEBUGREGISTER_REG register
 *  Gives the status of various internal blocks for debugging
 */
#define GMAC_REGISTER9_DEBUGREGISTER_REG (DR_REG_GMAC_BASE + 0x24)
/** GMAC_RPESTS : RO; bitpos: [0]; default: 0;
 *  MAC GMII or MII Receive Protocol Engine Status When high, this bit indicates that
 *  the MAC GMII or MII receive protocol engine is actively receiving data and not in
 *  IDLE state
 */
#define GMAC_RPESTS    (BIT(0))
#define GMAC_RPESTS_M  (GMAC_RPESTS_V << GMAC_RPESTS_S)
#define GMAC_RPESTS_V  0x00000001U
#define GMAC_RPESTS_S  0
/** GMAC_RFCFCSTS : RO; bitpos: [2:1]; default: 0;
 *  MAC Receive Frame FIFO Controller Status When high, this field indicates the active
 *  state of the small FIFO Read and Write controllers of the MAC Receive Frame
 *  Controller Module RFCFCSTS[1] represents the status of small FIFO Read controller
 *  RFCFCSTS[0] represents the status of small FIFO Write controller
 */
#define GMAC_RFCFCSTS    0x00000003U
#define GMAC_RFCFCSTS_M  (GMAC_RFCFCSTS_V << GMAC_RFCFCSTS_S)
#define GMAC_RFCFCSTS_V  0x00000003U
#define GMAC_RFCFCSTS_S  1
/** GMAC_RWCSTS : RO; bitpos: [4]; default: 0;
 *  MTL Rx FIFO Write Controller Active Status When high, this bit indicates that the
 *  MTL Rx FIFO Write Controller is active and is transferring a received frame to the
 *  FIFO
 */
#define GMAC_RWCSTS    (BIT(4))
#define GMAC_RWCSTS_M  (GMAC_RWCSTS_V << GMAC_RWCSTS_S)
#define GMAC_RWCSTS_V  0x00000001U
#define GMAC_RWCSTS_S  4
/** GMAC_RRCSTS : RO; bitpos: [6:5]; default: 0;
 *  MTL RxFIFO Read Controller State This field gives the state of the Rx FIFO read
 *  Controller: 00: IDLE state 01: Reading frame data 10: Reading frame status _or
 *  timestamp_ 11: Flushing the frame data and status
 */
#define GMAC_RRCSTS    0x00000003U
#define GMAC_RRCSTS_M  (GMAC_RRCSTS_V << GMAC_RRCSTS_S)
#define GMAC_RRCSTS_V  0x00000003U
#define GMAC_RRCSTS_S  5
/** GMAC_RXFSTS : RO; bitpos: [9:8]; default: 0;
 *  MTL RxFIFO FillLevel Status This field gives the status of the filllevel of the Rx
 *  FIFO: 00: Rx FIFO Empty 01: Rx FIFO filllevel below flowcontrol deactivate
 *  threshold 10: Rx FIFO filllevel above flowcontrol activate threshold 11: Rx FIFO
 *  Full
 */
#define GMAC_RXFSTS    0x00000003U
#define GMAC_RXFSTS_M  (GMAC_RXFSTS_V << GMAC_RXFSTS_S)
#define GMAC_RXFSTS_V  0x00000003U
#define GMAC_RXFSTS_S  8
/** GMAC_TPESTS : RO; bitpos: [16]; default: 0;
 *  MAC GMII or MII Transmit Protocol Engine Status When high, this bit indicates that
 *  the MAC GMII or MII transmit protocol engine is actively transmitting data and is
 *  not in the IDLE state
 */
#define GMAC_TPESTS    (BIT(16))
#define GMAC_TPESTS_M  (GMAC_TPESTS_V << GMAC_TPESTS_S)
#define GMAC_TPESTS_V  0x00000001U
#define GMAC_TPESTS_S  16
/** GMAC_TFCSTS : RO; bitpos: [18:17]; default: 0;
 *  MAC Transmit Frame Controller Status This field indicates the state of the MAC
 *  Transmit Frame Controller module: 00: IDLE state 01: Waiting for status of previous
 *  frame or IFG or backoff period to be over 10: Generating and transmitting a Pause
 *  frame _in the fullduplex mode_ 11: Transferring input frame for transmission
 */
#define GMAC_TFCSTS    0x00000003U
#define GMAC_TFCSTS_M  (GMAC_TFCSTS_V << GMAC_TFCSTS_S)
#define GMAC_TFCSTS_V  0x00000003U
#define GMAC_TFCSTS_S  17
/** GMAC_TXPAUSED : RO; bitpos: [19]; default: 0;
 *  MAC Transmitter in Pause When high, this bit indicates that the MAC transmitter is
 *  in the Pause condition _in the fullduplexonly mode_ and hence does not schedule any
 *  frame for transmission
 */
#define GMAC_TXPAUSED    (BIT(19))
#define GMAC_TXPAUSED_M  (GMAC_TXPAUSED_V << GMAC_TXPAUSED_S)
#define GMAC_TXPAUSED_V  0x00000001U
#define GMAC_TXPAUSED_S  19
/** GMAC_TRCSTS : RO; bitpos: [21:20]; default: 0;
 *  MTL Tx FIFO Read Controller Status This field indicates the state of the Tx FIFO
 *  Read Controller: 00: IDLE state 01: READ state _transferring data to the MAC
 *  transmitter_ 10: Waiting for TxStatus from the MAC transmitter 11: Writing the
 *  received TxStatus or flushing the Tx FIFO
 */
#define GMAC_TRCSTS    0x00000003U
#define GMAC_TRCSTS_M  (GMAC_TRCSTS_V << GMAC_TRCSTS_S)
#define GMAC_TRCSTS_V  0x00000003U
#define GMAC_TRCSTS_S  20
/** GMAC_TWCSTS : RO; bitpos: [22]; default: 0;
 *  MTL Tx FIFO Write Controller Status When high, this bit indicates that the MTL Tx
 *  FIFO Write Controller is active and is transferring data to the Tx FIFO
 */
#define GMAC_TWCSTS    (BIT(22))
#define GMAC_TWCSTS_M  (GMAC_TWCSTS_V << GMAC_TWCSTS_S)
#define GMAC_TWCSTS_V  0x00000001U
#define GMAC_TWCSTS_S  22
/** GMAC_TXFSTS : RO; bitpos: [24]; default: 0;
 *  MTL Tx FIFO Not Empty Status When high, this bit indicates that the MTL Tx FIFO is
 *  not empty and some data is left for transmission
 */
#define GMAC_TXFSTS    (BIT(24))
#define GMAC_TXFSTS_M  (GMAC_TXFSTS_V << GMAC_TXFSTS_S)
#define GMAC_TXFSTS_V  0x00000001U
#define GMAC_TXFSTS_S  24
/** GMAC_TXSTSFSTS : RO; bitpos: [25]; default: 0;
 *  MTL TxStatus FIFO Full Status When high, this bit indicates that the MTL TxStatus
 *  FIFO is full Therefore, the MTL cannot accept any more frames for transmission This
 *  bit is reserved in the GMACAHB and GMACDMA configurations
 */
#define GMAC_TXSTSFSTS    (BIT(25))
#define GMAC_TXSTSFSTS_M  (GMAC_TXSTSFSTS_V << GMAC_TXSTSFSTS_S)
#define GMAC_TXSTSFSTS_V  0x00000001U
#define GMAC_TXSTSFSTS_S  25

/** GMAC_REGISTER10_REMOTEWAKEUPFRAMEFILTERREGISTER_REG register
 *  Remote Wake-Up Frame Filter Register
 */
#define GMAC_REGISTER10_REMOTEWAKEUPFRAMEFILTERREGISTER_REG (DR_REG_GMAC_BASE + 0x28)
/** GMAC_WKUPFRM_FILTER : R/W; bitpos: [31:0]; default: 0;
 *  This is the address through which the application writes or reads the remote
 *  wake-up frame filter registers.The reg_wkupfrm_filter register is a pointer to
 *  eight reg_wkupfrm_filter registers.The reg_wkupfrm_filter register is loaded by
 *  sequentially loading the eight register values.Eight sequential writes to this
 *  address(0x0028)write all reg_wkupfrm_filter registers.Similarly, eight sequential
 *  reads from this address(0x0028) read all reg_wkupfrm_filter registers.
 *  This register is present only when you select the PMT module Remote Wake-Up feature
 *  in coreConsultant.
 */
#define GMAC_WKUPFRM_FILTER    0xFFFFFFFFU
#define GMAC_WKUPFRM_FILTER_M  (GMAC_WKUPFRM_FILTER_V << GMAC_WKUPFRM_FILTER_S)
#define GMAC_WKUPFRM_FILTER_V  0xFFFFFFFFU
#define GMAC_WKUPFRM_FILTER_S  0

/** GMAC_REGISTER11_PMTCONTROLANDSTATUSREGISTER_REG register
 *  PMT Control and Status Register. This register is present only when you select the
 *  PMT module in coreConsultant.
 */
#define GMAC_REGISTER11_PMTCONTROLANDSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x2c)
/** GMAC_PWRDWN : R/W1S; bitpos: [0]; default: 0;
 *  Power Down
 *  When set, the MAC receiver drops all received frames until it receives the expected
 *  magic packet or remote wake-up frame. This bit is then self-cleared and the
 *  power-down mode is disabled. The Software can also clear this bit before the
 *  expected magic packet or remote wake-up frame is received. The frames, received by
 *  MAC after this bit is cleared, are forwarded to the application. This bit must only
 *  be set when the Magic Packet Enable, Global Unicast, or Remote Wake-Up Frame Enable
 *  bit is set high.
 */
#define GMAC_PWRDWN    (BIT(0))
#define GMAC_PWRDWN_M  (GMAC_PWRDWN_V << GMAC_PWRDWN_S)
#define GMAC_PWRDWN_V  0x00000001U
#define GMAC_PWRDWN_S  0
/** GMAC_MGKPKTEN : R/W; bitpos: [1]; default: 0;
 *  Magic Packet Enable
 *  When set, enables generation of a power management event because of magic packet
 *  reception.
 */
#define GMAC_MGKPKTEN    (BIT(1))
#define GMAC_MGKPKTEN_M  (GMAC_MGKPKTEN_V << GMAC_MGKPKTEN_S)
#define GMAC_MGKPKTEN_V  0x00000001U
#define GMAC_MGKPKTEN_S  1
/** GMAC_RWKPKTEN : R/W; bitpos: [2]; default: 0;
 *  Remote Wake-Up Frame Enable
 *  When set, enables generation of a power management event because of remote wake-up
 *  frame reception.
 */
#define GMAC_RWKPKTEN    (BIT(2))
#define GMAC_RWKPKTEN_M  (GMAC_RWKPKTEN_V << GMAC_RWKPKTEN_S)
#define GMAC_RWKPKTEN_V  0x00000001U
#define GMAC_RWKPKTEN_S  2
/** GMAC_MGKPRCVD : R/W; bitpos: [5]; default: 0;
 *  Magic Packet Received
 *  When set,this bit indicates that the power management event is generated because of
 *  the reception of a magic packet. This bit is cleared by Read into this register.
 */
#define GMAC_MGKPRCVD    (BIT(5))
#define GMAC_MGKPRCVD_M  (GMAC_MGKPRCVD_V << GMAC_MGKPRCVD_S)
#define GMAC_MGKPRCVD_V  0x00000001U
#define GMAC_MGKPRCVD_S  5
/** GMAC_RWKPRCVD : R/W; bitpos: [6]; default: 0;
 *  Remote Wake-Up Frame Received
 *  When set, this bit indicates the power management event is generated because of the
 *  reception of a remote wake-up frame. This bit is cleared by a Read into this
 *  register.
 */
#define GMAC_RWKPRCVD    (BIT(6))
#define GMAC_RWKPRCVD_M  (GMAC_RWKPRCVD_V << GMAC_RWKPRCVD_S)
#define GMAC_RWKPRCVD_V  0x00000001U
#define GMAC_RWKPRCVD_S  6
/** GMAC_GLBLUCAST : R/W; bitpos: [9]; default: 0;
 *  Global Unicast.
 *  When set, enables any unicast packet filtered by the MAX(DAF) address recognition
 *  to be a remote wake-up frame.
 */
#define GMAC_GLBLUCAST    (BIT(9))
#define GMAC_GLBLUCAST_M  (GMAC_GLBLUCAST_V << GMAC_GLBLUCAST_S)
#define GMAC_GLBLUCAST_V  0x00000001U
#define GMAC_GLBLUCAST_S  9
/** GMAC_RWKPTR : RO; bitpos: [28:24]; default: 0;
 *  Remote Wake-up FIFO Pointer.
 *  This field gives the current value(0 to 31) of the Remote Wake-up Frame filter
 *  register pointer. When the value of this pointer is erual to 7, 15, 23 or 31, the
 *  contents of the Remote Wake-up Frame Filter Register are transferred to the
 *  clk_rx_i domain when a write occurs to that register.
 *  The maximum value of the pointer is 7, 15, 23 and 31 respectively depending on the
 *  number of Remote Wakeup Filters selected during configuration.
 */
#define GMAC_RWKPTR    0x0000001FU
#define GMAC_RWKPTR_M  (GMAC_RWKPTR_V << GMAC_RWKPTR_S)
#define GMAC_RWKPTR_V  0x0000001FU
#define GMAC_RWKPTR_S  24
/** GMAC_RWKFILTRST : R/W1S; bitpos: [31]; default: 0;
 *  Remote Wake-Up Frame Filter Register Pointer Reset.
 *  When this bit is set, it resets the remote wake-up frame filter register pointer to
 *  3'b000. It is automatically cleared after 1 clock cycle.
 */
#define GMAC_RWKFILTRST    (BIT(31))
#define GMAC_RWKFILTRST_M  (GMAC_RWKFILTRST_V << GMAC_RWKFILTRST_S)
#define GMAC_RWKFILTRST_V  0x00000001U
#define GMAC_RWKFILTRST_S  31

/** GMAC_REGISTER12_LPICONTROLANDSTATUSREGISTER_REG register
 *  Controls the Low Power Idle _LPI_ operations and provides the LPI status of the
 *  core This register is present only when you select the Energy Efficient Ethernet
 *  feature in coreConsultant
 */
#define GMAC_REGISTER12_LPICONTROLANDSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x30)
/** GMAC_TLPIEN : R/W; bitpos: [0]; default: 0;
 *  Transmit LPI Entry When set, this bit indicates that the MAC Transmitter has
 *  entered the LPI state because of the setting of the LPIEN bit This bit is cleared
 *  by a read into this register
 */
#define GMAC_TLPIEN    (BIT(0))
#define GMAC_TLPIEN_M  (GMAC_TLPIEN_V << GMAC_TLPIEN_S)
#define GMAC_TLPIEN_V  0x00000001U
#define GMAC_TLPIEN_S  0
/** GMAC_TLPIEX : R/W; bitpos: [1]; default: 0;
 *  Transmit LPI Exit When set, this bit indicates that the MAC transmitter has exited
 *  the LPI state after the user has cleared the LPIEN bit and the LPI TW Timer has
 *  expired This bit is cleared by a read into this register
 */
#define GMAC_TLPIEX    (BIT(1))
#define GMAC_TLPIEX_M  (GMAC_TLPIEX_V << GMAC_TLPIEX_S)
#define GMAC_TLPIEX_V  0x00000001U
#define GMAC_TLPIEX_S  1
/** GMAC_RLPIEN : R/W; bitpos: [2]; default: 0;
 *  Receive LPI Entry When set, this bit indicates that the MAC Receiver has received
 *  an LPI pattern and entered the LPI state This bit is cleared by a read into this
 *  register Note: This bit may not get set if the MAC stops receiving the LPI pattern
 *  for a very short duration, such as, less than 3 clock cycles of CSR clock
 */
#define GMAC_RLPIEN    (BIT(2))
#define GMAC_RLPIEN_M  (GMAC_RLPIEN_V << GMAC_RLPIEN_S)
#define GMAC_RLPIEN_V  0x00000001U
#define GMAC_RLPIEN_S  2
/** GMAC_RLPIEX : R/W; bitpos: [3]; default: 0;
 *  Receive LPI Exit When set, this bit indicates that the MAC Receiver has stopped
 *  receiving the LPI pattern on the GMII or MII interface, exited the LPI state, and
 *  resumed the normal reception This bit is cleared by a read into this register Note:
 *  This bit may not get set if the MAC stops receiving the LPI pattern for a very
 *  short duration, such as, less than 3 clock cycles of CSR clock
 */
#define GMAC_RLPIEX    (BIT(3))
#define GMAC_RLPIEX_M  (GMAC_RLPIEX_V << GMAC_RLPIEX_S)
#define GMAC_RLPIEX_V  0x00000001U
#define GMAC_RLPIEX_S  3
/** GMAC_TLPIST : RO; bitpos: [8]; default: 0;
 *  Transmit LPI State When set, this bit indicates that the MAC is transmitting the
 *  LPI pattern on the GMII or MII interface
 */
#define GMAC_TLPIST    (BIT(8))
#define GMAC_TLPIST_M  (GMAC_TLPIST_V << GMAC_TLPIST_S)
#define GMAC_TLPIST_V  0x00000001U
#define GMAC_TLPIST_S  8
/** GMAC_RLPIST : RO; bitpos: [9]; default: 0;
 *  Receive LPI State When set, this bit indicates that the MAC is receiving the LPI
 *  pattern on the GMII or MII interface
 */
#define GMAC_RLPIST    (BIT(9))
#define GMAC_RLPIST_M  (GMAC_RLPIST_V << GMAC_RLPIST_S)
#define GMAC_RLPIST_V  0x00000001U
#define GMAC_RLPIST_S  9
/** GMAC_LPIEN : R/W; bitpos: [16]; default: 0;
 *  LPI Enable When set, this bit instructs the MAC Transmitter to enter the LPI state
 *  When reset, this bit instructs the MAC to exit the LPI state and resume normal
 *  transmission This bit is cleared when the LPITXA bit is set and the MAC exits the
 *  LPI state because of the arrival of a new packet for transmission
 */
#define GMAC_LPIEN    (BIT(16))
#define GMAC_LPIEN_M  (GMAC_LPIEN_V << GMAC_LPIEN_S)
#define GMAC_LPIEN_V  0x00000001U
#define GMAC_LPIEN_S  16
/** GMAC_PLS : R/W; bitpos: [17]; default: 0;
 *  PHY Link Status This bit indicates the link status of the PHY The MAC Transmitter
 *  asserts the LPI pattern only when the link status is up _okay_ at least for the
 *  time indicated by the LPI LS TIMER When set, the link is considered to be okay _up_
 *  and when reset, the link is considered to be down
 */
#define GMAC_PLS    (BIT(17))
#define GMAC_PLS_M  (GMAC_PLS_V << GMAC_PLS_S)
#define GMAC_PLS_V  0x00000001U
#define GMAC_PLS_S  17
/** GMAC_PLSEN : R/W; bitpos: [18]; default: 0;
 *  PHY Link Status Enable This bit enables the link status received on the RGMII,
 *  SGMII, or SMII receive paths to be used for activating the LPI LS TIMER When set,
 *  the MAC uses the linkstatus bits of Register 54 _SGMII/RGMII/SMII Control and
 *  Status Register_ and Bit 17 _PLS_ for the LPI LS Timer trigger When cleared, the
 *  MAC ignores the linkstatus bits of Register 54 and takes only the PLS bit This bit
 *  is RO and reserved if you have not selected the RGMII, SGMII, or SMII PHY interface
 */
#define GMAC_PLSEN    (BIT(18))
#define GMAC_PLSEN_M  (GMAC_PLSEN_V << GMAC_PLSEN_S)
#define GMAC_PLSEN_V  0x00000001U
#define GMAC_PLSEN_S  18
/** GMAC_LPITXA : R/W; bitpos: [19]; default: 0;
 *  LPI TX Automate This bit controls the behavior of the MAC when it is entering or
 *  coming out of the LPI mode on the transmit side This bit is not functional in the
 *  GMAC CORE configuration in which the Tx clock gating is done during the LPI mode If
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
#define GMAC_LPITXA    (BIT(19))
#define GMAC_LPITXA_M  (GMAC_LPITXA_V << GMAC_LPITXA_S)
#define GMAC_LPITXA_V  0x00000001U
#define GMAC_LPITXA_S  19

/** GMAC_REGISTER13_LPITIMERSCONTROLREGISTER_REG register
 *  Controls the timeout values in LPI states This register is present only when you
 *  select the Energy Efficient Ethernet feature in coreConsultant
 */
#define GMAC_REGISTER13_LPITIMERSCONTROLREGISTER_REG (DR_REG_GMAC_BASE + 0x34)
/** GMAC_TWT : R/W; bitpos: [15:0]; default: 0;
 *  LPI TW TIMER This field specifies the minimum time _in microseconds_ for which the
 *  MAC waits after it stops transmitting the LPI pattern to the PHY and before it
 *  resumes the normal transmission The TLPIEX status bit is set after the expiry of
 *  this timer
 */
#define GMAC_TWT    0x0000FFFFU
#define GMAC_TWT_M  (GMAC_TWT_V << GMAC_TWT_S)
#define GMAC_TWT_V  0x0000FFFFU
#define GMAC_TWT_S  0
/** GMAC_LST : R/W; bitpos: [25:16]; default: 1000;
 *  LPI LS TIMER This field specifies the minimum time _in milliseconds_ for which the
 *  link status from the PHY should be up _OKAY_ before the LPI pattern can be
 *  transmitted to the PHY The MAC does not transmit the LPI pattern even when the
 *  LPIEN bit is set unless the LPI LS Timer reaches the programmed terminal count The
 *  default value of the LPI LS Timer is 1000 _1 sec_ as defined in the IEEE standard
 */
#define GMAC_LST    0x000003FFU
#define GMAC_LST_M  (GMAC_LST_V << GMAC_LST_S)
#define GMAC_LST_V  0x000003FFU
#define GMAC_LST_S  16

/** GMAC_REGISTER14_INTERRUPTSTATUSREGISTER_REG register
 *  Contains the interrupt status
 */
#define GMAC_REGISTER14_INTERRUPTSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x38)
/** GMAC_RGSMIIIS : RO; bitpos: [0]; default: 0;
 *  RGMII or SMII Interrupt Status This bit is set because of any change in value of
 *  the Link Status of RGMII or SMII interface _Bit 3 in Register 54 _SGMII/RGMII/SMII
 *  Control and Status Register__ This bit is cleared when you perform a read operation
 *  on the SGMII/RGMII/SMII Control and Status Register This bit is valid only when you
 *  select the optional RGMII or SMII PHY interface during core configuration and
 *  operation
 */
#define GMAC_RGSMIIIS    (BIT(0))
#define GMAC_RGSMIIIS_M  (GMAC_RGSMIIIS_V << GMAC_RGSMIIIS_S)
#define GMAC_RGSMIIIS_V  0x00000001U
#define GMAC_RGSMIIIS_S  0
/** GMAC_PCSLCHGIS : RO; bitpos: [1]; default: 0;
 *  PCS Link Status Changed This bit is set because of any change in Link Status in the
 *  TBI, RTBI, or SGMII PHY interface _Bit 2 in Register 49 _AN Status Register__ This
 *  bit is cleared when you perform a read operation on the AN Status register This bit
 *  is valid only when you select the optional TBI, RTBI, or SGMII PHY interface during
 *  core configuration and operation
 */
#define GMAC_PCSLCHGIS    (BIT(1))
#define GMAC_PCSLCHGIS_M  (GMAC_PCSLCHGIS_V << GMAC_PCSLCHGIS_S)
#define GMAC_PCSLCHGIS_V  0x00000001U
#define GMAC_PCSLCHGIS_S  1
/** GMAC_PCSANCIS : RO; bitpos: [2]; default: 0;
 *  PCS AutoNegotiation Complete This bit is set when the Autonegotiation is completed
 *  in the TBI, RTBI, or SGMII PHY interface _Bit 5 in Register 49 _AN Status
 *  Register__ This bit is cleared when you perform a read operation to the AN Status
 *  register This bit is valid only when you select the optional TBI, RTBI, or SGMII
 *  PHY interface during core configuration and operation
 */
#define GMAC_PCSANCIS    (BIT(2))
#define GMAC_PCSANCIS_M  (GMAC_PCSANCIS_V << GMAC_PCSANCIS_S)
#define GMAC_PCSANCIS_V  0x00000001U
#define GMAC_PCSANCIS_S  2
/** GMAC_PMTIS : RO; bitpos: [3]; default: 0;
 *  PMT Interrupt Status This bit is set when a magic packet or remote wakeup frame is
 *  received in the powerdown mode _see Bits 5 and 6 in the PMT Control and Status
 *  Register_ This bit is cleared when both Bits[6:5] are cleared because of a read
 *  operation to the PMT Control and Status register This bit is valid only when you
 *  select the optional PMT module during core configuration
 */
#define GMAC_PMTIS    (BIT(3))
#define GMAC_PMTIS_M  (GMAC_PMTIS_V << GMAC_PMTIS_S)
#define GMAC_PMTIS_V  0x00000001U
#define GMAC_PMTIS_S  3
/** GMAC_MMCIS : RO; bitpos: [4]; default: 0;
 *  MMC Interrupt Status This bit is set high when any of the Bits [7:5] is set high
 *  and cleared only when all of these bits are low This bit is valid only when you
 *  select the optional MMC module during core configuration
 */
#define GMAC_MMCIS    (BIT(4))
#define GMAC_MMCIS_M  (GMAC_MMCIS_V << GMAC_MMCIS_S)
#define GMAC_MMCIS_V  0x00000001U
#define GMAC_MMCIS_S  4
/** GMAC_MMCRXIS : RO; bitpos: [5]; default: 0;
 *  MMC Receive Interrupt Status This bit is set high when an interrupt is generated in
 *  the MMC Receive Interrupt Register This bit is cleared when all the bits in this
 *  interrupt register are cleared This bit is valid only when you select the optional
 *  MMC module during core configuration
 */
#define GMAC_MMCRXIS    (BIT(5))
#define GMAC_MMCRXIS_M  (GMAC_MMCRXIS_V << GMAC_MMCRXIS_S)
#define GMAC_MMCRXIS_V  0x00000001U
#define GMAC_MMCRXIS_S  5
/** GMAC_MMCTXIS : RO; bitpos: [6]; default: 0;
 *  MMC Transmit Interrupt Status This bit is set high when an interrupt is generated
 *  in the MMC Transmit Interrupt Register This bit is cleared when all the bits in
 *  this interrupt register are cleared This bit is valid only when you select the
 *  optional MMC module during core configuration
 */
#define GMAC_MMCTXIS    (BIT(6))
#define GMAC_MMCTXIS_M  (GMAC_MMCTXIS_V << GMAC_MMCTXIS_S)
#define GMAC_MMCTXIS_V  0x00000001U
#define GMAC_MMCTXIS_S  6
/** GMAC_MMCRXIPIS : RO; bitpos: [7]; default: 0;
 *  MMC Receive Checksum Offload Interrupt Status This bit is set high when an
 *  interrupt is generated in the MMC Receive Checksum Offload Interrupt Register This
 *  bit is cleared when all the bits in this interrupt register are cleared This bit is
 *  valid only when you select the optional MMC module and Checksum Offload Engine
 *  _Type 2_ during core configuration
 */
#define GMAC_MMCRXIPIS    (BIT(7))
#define GMAC_MMCRXIPIS_M  (GMAC_MMCRXIPIS_V << GMAC_MMCRXIPIS_S)
#define GMAC_MMCRXIPIS_V  0x00000001U
#define GMAC_MMCRXIPIS_S  7
/** GMAC_TSIS : RO; bitpos: [9]; default: 0;
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
#define GMAC_TSIS    (BIT(9))
#define GMAC_TSIS_M  (GMAC_TSIS_V << GMAC_TSIS_S)
#define GMAC_TSIS_V  0x00000001U
#define GMAC_TSIS_S  9
/** GMAC_LPIIS : RO; bitpos: [10]; default: 0;
 *  LPI Interrupt Status When the Energy Efficient Ethernet feature is enabled, this
 *  bit is set for any LPI state entry or exit in the MAC Transmitter or Receiver This
 *  bit is cleared on reading Bit 0 of Register 12 _LPI Control and Status Register_ In
 *  all other modes, this bit is reserved
 */
#define GMAC_LPIIS    (BIT(10))
#define GMAC_LPIIS_M  (GMAC_LPIIS_V << GMAC_LPIIS_S)
#define GMAC_LPIIS_V  0x00000001U
#define GMAC_LPIIS_S  10
/** GMAC_GPIIS : RO; bitpos: [11]; default: 0;
 *  GPI Interrupt Status When the GPIO feature is enabled, this bit is set when any
 *  active event _LL or LH_ occurs on the GPIS field _Bits [3:0]_ of Register 56
 *  _General Purpose IO Register_ and the corresponding GPIE bit is enabled This bit is
 *  cleared on reading lane 0 _GPIS_ of Register 56 _General Purpose IO Register_ When
 *  the GPIO feature is not enabled, this bit is reserved
 */
#define GMAC_GPIIS    (BIT(11))
#define GMAC_GPIIS_M  (GMAC_GPIIS_V << GMAC_GPIIS_S)
#define GMAC_GPIIS_V  0x00000001U
#define GMAC_GPIIS_S  11

/** GMAC_REGISTER15_INTERRUPTMASKREGISTER_REG register
 *  Contains the masks for generating the interrupts
 */
#define GMAC_REGISTER15_INTERRUPTMASKREGISTER_REG (DR_REG_GMAC_BASE + 0x3c)
/** GMAC_RGSMIIIM : R/W; bitpos: [0]; default: 0;
 *  RGMII or SMII Interrupt Mask When set, this bit disables the assertion of the
 *  interrupt signal because of the setting of the RGMII or SMII Interrupt Status bit
 *  in Register 14 _Interrupt Status Register_
 */
#define GMAC_RGSMIIIM    (BIT(0))
#define GMAC_RGSMIIIM_M  (GMAC_RGSMIIIM_V << GMAC_RGSMIIIM_S)
#define GMAC_RGSMIIIM_V  0x00000001U
#define GMAC_RGSMIIIM_S  0
/** GMAC_PCSLCHGIM : R/W; bitpos: [1]; default: 0;
 *  PCS Link Status Interrupt Mask When set, this bit disables the assertion of the
 *  interrupt signal because of the setting of the PCS Linkstatus changed bit in
 *  Register 14 _Interrupt Status Register_
 */
#define GMAC_PCSLCHGIM    (BIT(1))
#define GMAC_PCSLCHGIM_M  (GMAC_PCSLCHGIM_V << GMAC_PCSLCHGIM_S)
#define GMAC_PCSLCHGIM_V  0x00000001U
#define GMAC_PCSLCHGIM_S  1
/** GMAC_PCSANCIM : R/W; bitpos: [2]; default: 0;
 *  PCS AN Completion Interrupt Mask When set, this bit disables the assertion of the
 *  interrupt signal because of the setting of PCS Autonegotiation complete bit in
 *  Register 14 _Interrupt Status Register_
 */
#define GMAC_PCSANCIM    (BIT(2))
#define GMAC_PCSANCIM_M  (GMAC_PCSANCIM_V << GMAC_PCSANCIM_S)
#define GMAC_PCSANCIM_V  0x00000001U
#define GMAC_PCSANCIM_S  2
/** GMAC_PMTIM : R/W; bitpos: [3]; default: 0;
 *  PMT Interrupt Mask When set, this bit disables the assertion of the interrupt
 *  signal because of the setting of PMT Interrupt Status bit in Register 14 _Interrupt
 *  Status Register_
 */
#define GMAC_PMTIM    (BIT(3))
#define GMAC_PMTIM_M  (GMAC_PMTIM_V << GMAC_PMTIM_S)
#define GMAC_PMTIM_V  0x00000001U
#define GMAC_PMTIM_S  3
/** GMAC_TSIM : R/W; bitpos: [9]; default: 0;
 *  Timestamp Interrupt Mask When set, this bit disables the assertion of the interrupt
 *  signal because of the setting of Timestamp Interrupt Status bit in Register 14
 *  _Interrupt Status Register_ This bit is valid only when IEEE1588 timestamping is
 *  enabled In all other modes, this bit is reserved
 */
#define GMAC_TSIM    (BIT(9))
#define GMAC_TSIM_M  (GMAC_TSIM_V << GMAC_TSIM_S)
#define GMAC_TSIM_V  0x00000001U
#define GMAC_TSIM_S  9
/** GMAC_LPIIM : R/W; bitpos: [10]; default: 0;
 *  LPI Interrupt Mask When set, this bit disables the assertion of the interrupt
 *  signal because of the setting of the LPI Interrupt Status bit in Register 14
 *  _Interrupt Status Register_ This bit is valid only when you select the Energy
 *  Efficient Ethernet feature during core configuration In all other modes, this bit
 *  is reserved
 */
#define GMAC_LPIIM    (BIT(10))
#define GMAC_LPIIM_M  (GMAC_LPIIM_V << GMAC_LPIIM_S)
#define GMAC_LPIIM_V  0x00000001U
#define GMAC_LPIIM_S  10

/** GMAC_REGISTER16_MACADDRESS0HIGHREGISTER_REG register
 *  Contains the higher 16 bits of the first MAC address
 */
#define GMAC_REGISTER16_MACADDRESS0HIGHREGISTER_REG (DR_REG_GMAC_BASE + 0x40)
/** GMAC_ADDRHI_0 : R/W; bitpos: [15:0]; default: 65535;
 *  MAC Address0 [47:32] This field contains the upper 16 bits _47:32_ of the first
 *  6byte MAC address The MAC uses this field for filtering the received frames and
 *  inserting the MAC address in the Transmit Flow Control _Pause_ Frames
 */
#define GMAC_ADDRHI_0    0x0000FFFFU
#define GMAC_ADDRHI_0_M  (GMAC_ADDRHI_0_V << GMAC_ADDRHI_0_S)
#define GMAC_ADDRHI_0_V  0x0000FFFFU
#define GMAC_ADDRHI_0_S  0
/** GMAC_AE_0 : RO; bitpos: [31]; default: 1;
 *  Address Enable This bit is always set to 1
 */
#define GMAC_AE_0    (BIT(31))
#define GMAC_AE_0_M  (GMAC_AE_0_V << GMAC_AE_0_S)
#define GMAC_AE_0_V  0x00000001U
#define GMAC_AE_0_S  31

/** GMAC_REGISTER17_MACADDRESS0LOWREGISTER_REG register
 *  Contains the lower 32 bits of the first MAC address
 */
#define GMAC_REGISTER17_MACADDRESS0LOWREGISTER_REG (DR_REG_GMAC_BASE + 0x44)
/** GMAC_ADDRLO_0 : R/W; bitpos: [31:0]; default: 4294967295;
 *  MAC Address0 [31:0] This field contains the lower 32 bits of the first 6byte MAC
 *  address This is used by the MAC for filtering the received frames and inserting the
 *  MAC address in the Transmit Flow Control _Pause_ Frames
 */
#define GMAC_ADDRLO_0    0xFFFFFFFFU
#define GMAC_ADDRLO_0_M  (GMAC_ADDRLO_0_V << GMAC_ADDRLO_0_S)
#define GMAC_ADDRLO_0_V  0xFFFFFFFFU
#define GMAC_ADDRLO_0_S  0

/** GMAC_REGISTER18_MACADDRESS1HIGHREGISTER_REG register
 *  Contains the higher 16 bits of the second MAC address This register is present only
 *  when Enable MAC Address1 is selected in coreConsultant _See Table 78_
 */
#define GMAC_REGISTER18_MACADDRESS1HIGHREGISTER_REG (DR_REG_GMAC_BASE + 0x48)
/** GMAC_ADDRHI_1 : R/W; bitpos: [15:0]; default: 65535;
 *  MAC Address1 [47:32] This field contains the upper 16 bits _47:32_ of the second
 *  6byte MAC address
 */
#define GMAC_ADDRHI_1    0x0000FFFFU
#define GMAC_ADDRHI_1_M  (GMAC_ADDRHI_1_V << GMAC_ADDRHI_1_S)
#define GMAC_ADDRHI_1_V  0x0000FFFFU
#define GMAC_ADDRHI_1_S  0
/** GMAC_MBC_1 : R/W; bitpos: [29:24]; default: 0;
 *  Mask Byte Control These bits are mask control bits for comparison of each of the
 *  MAC Address bytes When set high, the MAC does not compare the corresponding byte of
 *  received DA or SA with the contents of MAC Address1 registers Each bit controls the
 *  masking of the bytes as follows: Bit 29: Register 18[15:8] Bit 28: Register 18[7:0]
 *  Bit 27: Register 19[31:24]  Bit 24: Register 19[7:0] You can filter a group of
 *  addresses _known as group address filtering_ by masking one or more bytes of the
 *  address
 */
#define GMAC_MBC_1    0x0000003FU
#define GMAC_MBC_1_M  (GMAC_MBC_1_V << GMAC_MBC_1_S)
#define GMAC_MBC_1_V  0x0000003FU
#define GMAC_MBC_1_S  24
/** GMAC_SA_1 : R/W; bitpos: [30]; default: 0;
 *  Source Address When this bit is set, the MAC Address1[47:0] is used to compare with
 *  the SA fields of the received frame When this bit is reset, the MAC Address1[47:0]
 *  is used to compare with the DA fields of the received frame
 */
#define GMAC_SA_1    (BIT(30))
#define GMAC_SA_1_M  (GMAC_SA_1_V << GMAC_SA_1_S)
#define GMAC_SA_1_V  0x00000001U
#define GMAC_SA_1_S  30
/** GMAC_AE_1 : R/W; bitpos: [31]; default: 0;
 *  Address Enable
 */
#define GMAC_AE_1    (BIT(31))
#define GMAC_AE_1_M  (GMAC_AE_1_V << GMAC_AE_1_S)
#define GMAC_AE_1_V  0x00000001U
#define GMAC_AE_1_S  31

/** GMAC_REGISTER19_MACADDRESS1LOWREGISTER_REG register
 *  Contains the lower 32 bits of the second MAC address This register is present only
 *  when Enable MAC Address1 is selected in coreConsultant _See Table 78_
 */
#define GMAC_REGISTER19_MACADDRESS1LOWREGISTER_REG (DR_REG_GMAC_BASE + 0x4c)
/** GMAC_ADDRLO_1 : R/W; bitpos: [31:0]; default: 4294967295;
 *  MAC Address1 [31:0] This field contains the lower 32 bits of the second 6byte MAC
 *  address The content of this field is undefined until loaded by the Application
 *  after the initialization process
 */
#define GMAC_ADDRLO_1    0xFFFFFFFFU
#define GMAC_ADDRLO_1_M  (GMAC_ADDRLO_1_V << GMAC_ADDRLO_1_S)
#define GMAC_ADDRLO_1_V  0xFFFFFFFFU
#define GMAC_ADDRLO_1_S  0

/** GMAC_MACADDRESS2HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHETHIRDMACADDRESS_REG register
 *  Reserved
 */
#define GMAC_MACADDRESS2HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHETHIRDMACADDRESS_REG (DR_REG_GMAC_BASE + 0x50)
/** GMAC_ADDRHI_2 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRHI_2    0x0000FFFFU
#define GMAC_ADDRHI_2_M  (GMAC_ADDRHI_2_V << GMAC_ADDRHI_2_S)
#define GMAC_ADDRHI_2_V  0x0000FFFFU
#define GMAC_ADDRHI_2_S  0
/** GMAC_MBC_2 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_MBC_2    0x0000003FU
#define GMAC_MBC_2_M  (GMAC_MBC_2_V << GMAC_MBC_2_S)
#define GMAC_MBC_2_V  0x0000003FU
#define GMAC_MBC_2_S  24
/** GMAC_SA_2 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_SA_2    (BIT(30))
#define GMAC_SA_2_M  (GMAC_SA_2_V << GMAC_SA_2_S)
#define GMAC_SA_2_V  0x00000001U
#define GMAC_SA_2_S  30
/** GMAC_AE_2 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_AE_2    (BIT(31))
#define GMAC_AE_2_M  (GMAC_AE_2_V << GMAC_AE_2_S)
#define GMAC_AE_2_V  0x00000001U
#define GMAC_AE_2_S  31

/** GMAC_MACADDRESS2LOWREGISTERCONTAINSTHELOWER32BITSOFTHETHIRDMACADDRESS_REG register
 *  Reserved
 */
#define GMAC_MACADDRESS2LOWREGISTERCONTAINSTHELOWER32BITSOFTHETHIRDMACADDRESS_REG (DR_REG_GMAC_BASE + 0x54)
/** GMAC_ADDRLO_2 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address2 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRLO_2    0xFFFFFFFFU
#define GMAC_ADDRLO_2_M  (GMAC_ADDRLO_2_V << GMAC_ADDRLO_2_S)
#define GMAC_ADDRLO_2_V  0xFFFFFFFFU
#define GMAC_ADDRLO_2_S  0

/** GMAC_MACADDRESS3HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHEFOURTHMACADDRESS_REG
 *  register
 *  Reserved
 */
#define GMAC_MACADDRESS3HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHEFOURTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x58)
/** GMAC_ADDRHI_3 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRHI_3    0x0000FFFFU
#define GMAC_ADDRHI_3_M  (GMAC_ADDRHI_3_V << GMAC_ADDRHI_3_S)
#define GMAC_ADDRHI_3_V  0x0000FFFFU
#define GMAC_ADDRHI_3_S  0
/** GMAC_MBC_3 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_MBC_3    0x0000003FU
#define GMAC_MBC_3_M  (GMAC_MBC_3_V << GMAC_MBC_3_S)
#define GMAC_MBC_3_V  0x0000003FU
#define GMAC_MBC_3_S  24
/** GMAC_SA_3 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_SA_3    (BIT(30))
#define GMAC_SA_3_M  (GMAC_SA_3_V << GMAC_SA_3_S)
#define GMAC_SA_3_V  0x00000001U
#define GMAC_SA_3_S  30
/** GMAC_AE_3 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_AE_3    (BIT(31))
#define GMAC_AE_3_M  (GMAC_AE_3_V << GMAC_AE_3_S)
#define GMAC_AE_3_V  0x00000001U
#define GMAC_AE_3_S  31

/** GMAC_MACADDRESS3LOWREGISTERCONTAINSTHELOWER32BITSOFTHEFOURTHMACADDRESS_REG register
 *  Reserved
 */
#define GMAC_MACADDRESS3LOWREGISTERCONTAINSTHELOWER32BITSOFTHEFOURTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x5c)
/** GMAC_ADDRLO_3 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address3 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRLO_3    0xFFFFFFFFU
#define GMAC_ADDRLO_3_M  (GMAC_ADDRLO_3_V << GMAC_ADDRLO_3_S)
#define GMAC_ADDRLO_3_V  0xFFFFFFFFU
#define GMAC_ADDRLO_3_S  0

/** GMAC_MACADDRESS4HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHEFIFTHMACADDRESS_REG register
 *  Reserved
 */
#define GMAC_MACADDRESS4HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHEFIFTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x60)
/** GMAC_ADDRHI_4 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRHI_4    0x0000FFFFU
#define GMAC_ADDRHI_4_M  (GMAC_ADDRHI_4_V << GMAC_ADDRHI_4_S)
#define GMAC_ADDRHI_4_V  0x0000FFFFU
#define GMAC_ADDRHI_4_S  0
/** GMAC_MBC_4 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_MBC_4    0x0000003FU
#define GMAC_MBC_4_M  (GMAC_MBC_4_V << GMAC_MBC_4_S)
#define GMAC_MBC_4_V  0x0000003FU
#define GMAC_MBC_4_S  24
/** GMAC_SA_4 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_SA_4    (BIT(30))
#define GMAC_SA_4_M  (GMAC_SA_4_V << GMAC_SA_4_S)
#define GMAC_SA_4_V  0x00000001U
#define GMAC_SA_4_S  30
/** GMAC_AE_4 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_AE_4    (BIT(31))
#define GMAC_AE_4_M  (GMAC_AE_4_V << GMAC_AE_4_S)
#define GMAC_AE_4_V  0x00000001U
#define GMAC_AE_4_S  31

/** GMAC_MACADDRESS4LOWREGISTERCONTAINSTHELOWER32BITSOFTHEFIFTHMACADDRESS_REG register
 *  Reserved
 */
#define GMAC_MACADDRESS4LOWREGISTERCONTAINSTHELOWER32BITSOFTHEFIFTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x64)
/** GMAC_ADDRLO_4 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address4 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRLO_4    0xFFFFFFFFU
#define GMAC_ADDRLO_4_M  (GMAC_ADDRLO_4_V << GMAC_ADDRLO_4_S)
#define GMAC_ADDRLO_4_V  0xFFFFFFFFU
#define GMAC_ADDRLO_4_S  0

/** GMAC_MACADDRESS5HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHESIXTHMACADDRESS_REG register
 *  Reserved
 */
#define GMAC_MACADDRESS5HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHESIXTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x68)
/** GMAC_ADDRHI_5 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRHI_5    0x0000FFFFU
#define GMAC_ADDRHI_5_M  (GMAC_ADDRHI_5_V << GMAC_ADDRHI_5_S)
#define GMAC_ADDRHI_5_V  0x0000FFFFU
#define GMAC_ADDRHI_5_S  0
/** GMAC_MBC_5 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_MBC_5    0x0000003FU
#define GMAC_MBC_5_M  (GMAC_MBC_5_V << GMAC_MBC_5_S)
#define GMAC_MBC_5_V  0x0000003FU
#define GMAC_MBC_5_S  24
/** GMAC_SA_5 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_SA_5    (BIT(30))
#define GMAC_SA_5_M  (GMAC_SA_5_V << GMAC_SA_5_S)
#define GMAC_SA_5_V  0x00000001U
#define GMAC_SA_5_S  30
/** GMAC_AE_5 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_AE_5    (BIT(31))
#define GMAC_AE_5_M  (GMAC_AE_5_V << GMAC_AE_5_S)
#define GMAC_AE_5_V  0x00000001U
#define GMAC_AE_5_S  31

/** GMAC_MACADDRESS5LOWREGISTERCONTAINSTHELOWER32BITSOFTHESIXTHMACADDRESS_REG register
 *  Reserved
 */
#define GMAC_MACADDRESS5LOWREGISTERCONTAINSTHELOWER32BITSOFTHESIXTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x6c)
/** GMAC_ADDRLO_5 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address5 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRLO_5    0xFFFFFFFFU
#define GMAC_ADDRLO_5_M  (GMAC_ADDRLO_5_V << GMAC_ADDRLO_5_S)
#define GMAC_ADDRLO_5_V  0xFFFFFFFFU
#define GMAC_ADDRLO_5_S  0

/** GMAC_MACADDRESS6HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHESEVENTHMACADDRESS_REG
 *  register
 *  Reserved
 */
#define GMAC_MACADDRESS6HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHESEVENTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x70)
/** GMAC_ADDRHI_6 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRHI_6    0x0000FFFFU
#define GMAC_ADDRHI_6_M  (GMAC_ADDRHI_6_V << GMAC_ADDRHI_6_S)
#define GMAC_ADDRHI_6_V  0x0000FFFFU
#define GMAC_ADDRHI_6_S  0
/** GMAC_MBC_6 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_MBC_6    0x0000003FU
#define GMAC_MBC_6_M  (GMAC_MBC_6_V << GMAC_MBC_6_S)
#define GMAC_MBC_6_V  0x0000003FU
#define GMAC_MBC_6_S  24
/** GMAC_SA_6 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_SA_6    (BIT(30))
#define GMAC_SA_6_M  (GMAC_SA_6_V << GMAC_SA_6_S)
#define GMAC_SA_6_V  0x00000001U
#define GMAC_SA_6_S  30
/** GMAC_AE_6 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_AE_6    (BIT(31))
#define GMAC_AE_6_M  (GMAC_AE_6_V << GMAC_AE_6_S)
#define GMAC_AE_6_V  0x00000001U
#define GMAC_AE_6_S  31

/** GMAC_MACADDRESS6LOWREGISTERCONTAINSTHELOWER32BITSOFTHESEVENTHMACADDRESS_REG register
 *  Reserved
 */
#define GMAC_MACADDRESS6LOWREGISTERCONTAINSTHELOWER32BITSOFTHESEVENTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x74)
/** GMAC_ADDRLO_6 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address6 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRLO_6    0xFFFFFFFFU
#define GMAC_ADDRLO_6_M  (GMAC_ADDRLO_6_V << GMAC_ADDRLO_6_S)
#define GMAC_ADDRLO_6_V  0xFFFFFFFFU
#define GMAC_ADDRLO_6_S  0

/** GMAC_MACADDRESS7HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHEEIGHTHMACADDRESS_REG
 *  register
 *  Reserved
 */
#define GMAC_MACADDRESS7HIGHREGISTERCONTAINSTHEHIGHER16BITSOFTHEEIGHTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x78)
/** GMAC_ADDRHI_7 : R/W; bitpos: [15:0]; default: 65535;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRHI_7    0x0000FFFFU
#define GMAC_ADDRHI_7_M  (GMAC_ADDRHI_7_V << GMAC_ADDRHI_7_S)
#define GMAC_ADDRHI_7_V  0x0000FFFFU
#define GMAC_ADDRHI_7_S  0
/** GMAC_MBC_7 : R/W; bitpos: [29:24]; default: 0;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_MBC_7    0x0000003FU
#define GMAC_MBC_7_M  (GMAC_MBC_7_V << GMAC_MBC_7_S)
#define GMAC_MBC_7_V  0x0000003FU
#define GMAC_MBC_7_S  24
/** GMAC_SA_7 : R/W; bitpos: [30]; default: 0;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_SA_7    (BIT(30))
#define GMAC_SA_7_M  (GMAC_SA_7_V << GMAC_SA_7_S)
#define GMAC_SA_7_V  0x00000001U
#define GMAC_SA_7_S  30
/** GMAC_AE_7 : R/W; bitpos: [31]; default: 0;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_AE_7    (BIT(31))
#define GMAC_AE_7_M  (GMAC_AE_7_V << GMAC_AE_7_S)
#define GMAC_AE_7_V  0x00000001U
#define GMAC_AE_7_S  31

/** GMAC_MACADDRESS7LOWREGISTERCONTAINSTHELOWER32BITSOFTHEEIGHTHMACADDRESS_REG register
 *  Reserved
 */
#define GMAC_MACADDRESS7LOWREGISTERCONTAINSTHELOWER32BITSOFTHEEIGHTHMACADDRESS_REG (DR_REG_GMAC_BASE + 0x7c)
/** GMAC_ADDRLO_7 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is present only when Enable MAC Address7 is selected in
 *  coreConsultant _See Table 78_
 */
#define GMAC_ADDRLO_7    0xFFFFFFFFU
#define GMAC_ADDRLO_7_M  (GMAC_ADDRLO_7_V << GMAC_ADDRLO_7_S)
#define GMAC_ADDRLO_7_V  0xFFFFFFFFU
#define GMAC_ADDRLO_7_S  0

/** GMAC_REGISTER48_ANCONTROLREGISTER_REG register
 *  Enables and/or restarts autonegotiation This register also enables the Physical
 *  Coding Sublayer _PCS_ loopback This register is present only when you select the
 *  TBI, RTBI, or SGMII interface in coreConsultant
 */
#define GMAC_REGISTER48_ANCONTROLREGISTER_REG (DR_REG_GMAC_BASE + 0xc0)
/** GMAC_RAN : R/W1S; bitpos: [9]; default: 0;
 *  Restart AutoNegotiation When set, this bit causes autonegotiation to restart if Bit
 *  12 _ANE_ is set This bit is selfclearing after autonegotiation starts This bit
 *  should be cleared for normal operation
 */
#define GMAC_RAN    (BIT(9))
#define GMAC_RAN_M  (GMAC_RAN_V << GMAC_RAN_S)
#define GMAC_RAN_V  0x00000001U
#define GMAC_RAN_S  9
/** GMAC_ANE : R/W; bitpos: [12]; default: 0;
 *  AutoNegotiation Enable When set, this bit enables the MAC to perform
 *  autonegotiation with the link partner Clearing this bit disables the autonegotiation
 */
#define GMAC_ANE    (BIT(12))
#define GMAC_ANE_M  (GMAC_ANE_V << GMAC_ANE_S)
#define GMAC_ANE_V  0x00000001U
#define GMAC_ANE_S  12
/** GMAC_ELE : R/W; bitpos: [14]; default: 0;
 *  External Loopback Enable When set, this bit causes the PHY to loopback the transmit
 *  data into the receive path The pcs_ewrap_o signal is asserted high when this bit is
 *  set
 */
#define GMAC_ELE    (BIT(14))
#define GMAC_ELE_M  (GMAC_ELE_V << GMAC_ELE_S)
#define GMAC_ELE_V  0x00000001U
#define GMAC_ELE_S  14
/** GMAC_ECD : R/W; bitpos: [16]; default: 0;
 *  Enable Comma Detect When set, this bit enables the PHY for comma detection and word
 *  resynchronization This bit controls the pcs_en_cdet_o signal on the TBI, RTBI, or
 *  SGMII interface
 */
#define GMAC_ECD    (BIT(16))
#define GMAC_ECD_M  (GMAC_ECD_V << GMAC_ECD_S)
#define GMAC_ECD_V  0x00000001U
#define GMAC_ECD_S  16
/** GMAC_LR : R/W; bitpos: [17]; default: 0;
 *  Lock to Reference When set, this bit enables the PHY to lock its PLL to the 125 MHz
 *  reference clock This bit controls the pcs_lck_ref_o signal on the TBI, RTBI, or
 *  SGMII interface
 */
#define GMAC_LR    (BIT(17))
#define GMAC_LR_M  (GMAC_LR_V << GMAC_LR_S)
#define GMAC_LR_V  0x00000001U
#define GMAC_LR_S  17
/** GMAC_SGMRAL : R/W; bitpos: [18]; default: 0;
 *  SGMII RAL Control When set, this bit forces the SGMII RAL block to operate in the
 *  speed configured in the Speed and Port Select bits of the MAC Configuration
 *  register This is useful when the SGMII interface is used in a direct MAC to MAC
 *  connection _without a PHY_ and any MAC must reconfigure the speed When reset, the
 *  SGMII RAL block operates according to the link speed status received on SGMII _from
 *  the PHY_ This bit is reserved _and RO_ if the SGMII PHY interface is not selected
 *  during core configuration
 */
#define GMAC_SGMRAL    (BIT(18))
#define GMAC_SGMRAL_M  (GMAC_SGMRAL_V << GMAC_SGMRAL_S)
#define GMAC_SGMRAL_V  0x00000001U
#define GMAC_SGMRAL_S  18

/** GMAC_REGISTER49_ANSTATUSREGISTER_REG register
 *  Indicates the link and autonegotiation status This register is present only when
 *  you select the TBI, RTBI, or SGMII interface in coreConsultant
 */
#define GMAC_REGISTER49_ANSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0xc4)
/** GMAC_LS : RO; bitpos: [2]; default: 0;
 *  Link Status This bit indicates whether the data channel _link_ is up or down For
 *  the TBI, RTBI or SGMII interfaces, if ANEG is going on, data cannot be transferred
 *  across the link and hence the link is given as down
 */
#define GMAC_LS    (BIT(2))
#define GMAC_LS_M  (GMAC_LS_V << GMAC_LS_S)
#define GMAC_LS_V  0x00000001U
#define GMAC_LS_S  2
/** GMAC_ANA : RO; bitpos: [3]; default: 1;
 *  AutoNegotiation Ability This bit is always high because the MAC supports auto
 *  negotiation
 */
#define GMAC_ANA    (BIT(3))
#define GMAC_ANA_M  (GMAC_ANA_V << GMAC_ANA_S)
#define GMAC_ANA_V  0x00000001U
#define GMAC_ANA_S  3
/** GMAC_ANC : RO; bitpos: [5]; default: 0;
 *  AutoNegotiation Complete When set, this bit indicates that the autonegotiation
 *  process is complete This bit is cleared when autonegotiation is reinitiated
 */
#define GMAC_ANC    (BIT(5))
#define GMAC_ANC_M  (GMAC_ANC_V << GMAC_ANC_S)
#define GMAC_ANC_V  0x00000001U
#define GMAC_ANC_S  5
/** GMAC_ES : RO; bitpos: [8]; default: 1;
 *  Extended Status This bit is tied to high if the TBI or RTBI interface is selected
 *  during core configuration indicating that the MAC supports extended status
 *  information in Register 53 _TBI Extended  Status Register_ This bit is tied to low
 *  if the SGMII interface is selected and the TBI or RTBI interface is not selected
 *  during core configuration indicating that Register 53 is not present
 */
#define GMAC_ES    (BIT(8))
#define GMAC_ES_M  (GMAC_ES_V << GMAC_ES_S)
#define GMAC_ES_V  0x00000001U
#define GMAC_ES_S  8

/** GMAC_REGISTER50_AUTONEGOTIATIONADVERTISEMENTREGISTER_REG register
 *  This register is configured before autonegotiation begins It contains the
 *  advertised ability of the MAC This register is present only when you select the TBI
 *  or RTBI interface in coreConsultant
 */
#define GMAC_REGISTER50_AUTONEGOTIATIONADVERTISEMENTREGISTER_REG (DR_REG_GMAC_BASE + 0xc8)
/** GMAC_FD : R/W; bitpos: [5]; default: 1;
 *  FullDuplex When set high, this bit indicates that the MAC supports the fullduplex
 *  mode
 */
#define GMAC_FD    (BIT(5))
#define GMAC_FD_M  (GMAC_FD_V << GMAC_FD_S)
#define GMAC_FD_V  0x00000001U
#define GMAC_FD_S  5
/** GMAC_HD : R/W; bitpos: [6]; default: 1;
 *  HalfDuplex When set high, this bit indicates that the MAC supports the halfduplex
 *  mode This bit is always low _and RO_ when the MAC is configured for the
 *  fullduplexonly mode
 */
#define GMAC_HD    (BIT(6))
#define GMAC_HD_M  (GMAC_HD_V << GMAC_HD_S)
#define GMAC_HD_V  0x00000001U
#define GMAC_HD_S  6
/** GMAC_PSE : R/W; bitpos: [8:7]; default: 3;
 *  Pause Encoding These bits provide an encoding for the Pause bits, indicating that
 *  the MAC is capable of configuring the Pause function as defined in IEEE 8023x The
 *  encoding of these bits is defined in IEEE 8023z, Section 37214
 */
#define GMAC_PSE    0x00000003U
#define GMAC_PSE_M  (GMAC_PSE_V << GMAC_PSE_S)
#define GMAC_PSE_V  0x00000003U
#define GMAC_PSE_S  7
/** GMAC_RFE : R/W; bitpos: [13:12]; default: 0;
 *  Remote Fault Encoding These bits provide a remote fault encoding, indicating to a
 *  link partner that a fault or error condition has occurred The encoding of these
 *  bits is defined in IEEE 8023z, Section 37215
 */
#define GMAC_RFE    0x00000003U
#define GMAC_RFE_M  (GMAC_RFE_V << GMAC_RFE_S)
#define GMAC_RFE_V  0x00000003U
#define GMAC_RFE_S  12
/** GMAC_NP : RO; bitpos: [15]; default: 0;
 *  Next Page Support This bit is always low because the MAC does not support the next
 *  page
 */
#define GMAC_NP    (BIT(15))
#define GMAC_NP_M  (GMAC_NP_V << GMAC_NP_S)
#define GMAC_NP_V  0x00000001U
#define GMAC_NP_S  15

/** GMAC_REGISTER51_AUTONEGOTIATIONLINKPARTNERABILITYREGISTER_REG register
 *  Contains the advertised ability of the link partner Its value is valid after
 *  successful completion of autonegotiation or when a new base page has been received
 *  _indicated in the AutoNegotiation Expansion Register_ This register is present only
 *  when you select the TBI or RTBI interface in coreConsultant
 */
#define GMAC_REGISTER51_AUTONEGOTIATIONLINKPARTNERABILITYREGISTER_REG (DR_REG_GMAC_BASE + 0xcc)
/** GMAC_FD_ABILITY : RO; bitpos: [5]; default: 0;
 *  FullDuplex When set, this bit indicates that the link partner has the ability to
 *  operate in the full duplex mode When cleared, this bit indicates that the link
 *  partner does not have the ability to operate in the fullduplex mode
 */
#define GMAC_FD_ABILITY    (BIT(5))
#define GMAC_FD_ABILITY_M  (GMAC_FD_ABILITY_V << GMAC_FD_ABILITY_S)
#define GMAC_FD_ABILITY_V  0x00000001U
#define GMAC_FD_ABILITY_S  5
/** GMAC_HD_ABILITY : RO; bitpos: [6]; default: 0;
 *  HalfDuplex When set, this bit indicates that the link partner has the ability to
 *  operate in the halfduplex mode When cleared, this bit indicates that the link
 *  partner does not have the ability to operate in the halfduplex mode
 */
#define GMAC_HD_ABILITY    (BIT(6))
#define GMAC_HD_ABILITY_M  (GMAC_HD_ABILITY_V << GMAC_HD_ABILITY_S)
#define GMAC_HD_ABILITY_V  0x00000001U
#define GMAC_HD_ABILITY_S  6
/** GMAC_PSE_ABILITY : RO; bitpos: [8:7]; default: 0;
 *  Pause Encoding These bits provide an encoding for the Pause bits, indicating that
 *  the link partner's capability of configuring the Pause function as defined in the
 *  IEEE 8023x specification The encoding of these bits is defined in IEEE 8023z,
 *  Section 37214
 */
#define GMAC_PSE_ABILITY    0x00000003U
#define GMAC_PSE_ABILITY_M  (GMAC_PSE_ABILITY_V << GMAC_PSE_ABILITY_S)
#define GMAC_PSE_ABILITY_V  0x00000003U
#define GMAC_PSE_ABILITY_S  7
/** GMAC_RFE_ABILITY : RO; bitpos: [13:12]; default: 0;
 *  Remote Fault Encoding These bits provide a remote fault encoding, indicating a
 *  fault or error condition of the link partner The encoding of these bits is defined
 *  in IEEE 8023z, Section 37215
 */
#define GMAC_RFE_ABILITY    0x00000003U
#define GMAC_RFE_ABILITY_M  (GMAC_RFE_ABILITY_V << GMAC_RFE_ABILITY_S)
#define GMAC_RFE_ABILITY_V  0x00000003U
#define GMAC_RFE_ABILITY_S  12
/** GMAC_ACK : RO; bitpos: [14]; default: 0;
 *  Acknowledge When set, the autonegotiation function uses this bit to indicate that
 *  the link partner has successfully received the base page of the MAC When cleared,
 *  it indicates that the link partner did not successfully receive the base page of
 *  the MAC
 */
#define GMAC_ACK    (BIT(14))
#define GMAC_ACK_M  (GMAC_ACK_V << GMAC_ACK_S)
#define GMAC_ACK_V  0x00000001U
#define GMAC_ACK_S  14
/** GMAC_NO : RO; bitpos: [15]; default: 0;
 *  Next Page Support When set, this bit indicates that more next page information is
 *  available When cleared, this bit indicates that next page exchange is not desired
 */
#define GMAC_NO    (BIT(15))
#define GMAC_NO_M  (GMAC_NO_V << GMAC_NO_S)
#define GMAC_NO_V  0x00000001U
#define GMAC_NO_S  15

/** GMAC_REGISTER52_AUTONEGOTIATIONEXPANSIONREGISTER_REG register
 *  Indicates whether a new base page has been received from the link partner This
 *  register is present only when you select the TBI or RTBI interface in coreConsultant
 */
#define GMAC_REGISTER52_AUTONEGOTIATIONEXPANSIONREGISTER_REG (DR_REG_GMAC_BASE + 0xd0)
/** GMAC_NPR : RO; bitpos: [1]; default: 0;
 *  New Page Received When set, this bit indicates that the MAC has received a new page
 *  This bit is cleared when read
 */
#define GMAC_NPR    (BIT(1))
#define GMAC_NPR_M  (GMAC_NPR_V << GMAC_NPR_S)
#define GMAC_NPR_V  0x00000001U
#define GMAC_NPR_S  1
/** GMAC_NPA : RO; bitpos: [2]; default: 0;
 *  Next Page Ability This bit is always low because the MAC does not support the next
 *  page function
 */
#define GMAC_NPA    (BIT(2))
#define GMAC_NPA_M  (GMAC_NPA_V << GMAC_NPA_S)
#define GMAC_NPA_V  0x00000001U
#define GMAC_NPA_S  2

/** GMAC_REGISTER53_TBIEXTENDEDSTATUSREGISTER_REG register
 *  Indicates all modes of operation of the MAC This register is present only when you
 *  select the TBI or RTBI interface in coreConsultant
 */
#define GMAC_REGISTER53_TBIEXTENDEDSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0xd4)
/** GMAC_GHD : RO; bitpos: [14]; default: 1;
 *  1000BASEX HalfDuplex Capable This bit indicates that the MAC is able to perform the
 *  halfduplex and 1000BASEX operations This bit is always low when the MAC is
 *  configured for the fullduplexonly operation during core configuration
 */
#define GMAC_GHD    (BIT(14))
#define GMAC_GHD_M  (GMAC_GHD_V << GMAC_GHD_S)
#define GMAC_GHD_V  0x00000001U
#define GMAC_GHD_S  14
/** GMAC_GFD : RO; bitpos: [15]; default: 1;
 *  1000BASEX FullDuplex Capable This bit indicates that the MAC is able to perform the
 *  fullduplex and 1000BASEX operations
 */
#define GMAC_GFD    (BIT(15))
#define GMAC_GFD_M  (GMAC_GFD_V << GMAC_GFD_S)
#define GMAC_GFD_V  0x00000001U
#define GMAC_GFD_S  15

/** GMAC_REGISTER54_SGMII_RGMII_SMIICONTROLANDSTATUSREGISTER_REG register
 *  Indicates the status signals received from the PHY through the SGMII, RGMII, or
 *  SMII interface This register is present only when you select the SGMII, RGMII, or
 *  SMII interface in coreConsultant
 */
#define GMAC_REGISTER54_SGMII_RGMII_SMIICONTROLANDSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0xd8)
/** GMAC_LNKMOD : RO; bitpos: [0]; default: 0;
 *  Link Mode This bit indicates the current mode of operation of the link: 1’b0:
 *  Halfduplex mode 1’b1: Fullduplex mode
 */
#define GMAC_LNKMOD    (BIT(0))
#define GMAC_LNKMOD_M  (GMAC_LNKMOD_V << GMAC_LNKMOD_S)
#define GMAC_LNKMOD_V  0x00000001U
#define GMAC_LNKMOD_S  0
/** GMAC_LNKSPEED : RO; bitpos: [2:1]; default: 2;
 *  Link Speed
 */
#define GMAC_LNKSPEED    0x00000003U
#define GMAC_LNKSPEED_M  (GMAC_LNKSPEED_V << GMAC_LNKSPEED_S)
#define GMAC_LNKSPEED_V  0x00000003U
#define GMAC_LNKSPEED_S  1
/** GMAC_LNKSTS : RO; bitpos: [3]; default: 0;
 *  Link Status This bit indicates whether the link between the local PHY and the
 *  remote PHY is up or down It gives the status of the link between the SGMII of MAC
 *  and the SGMII of the local PHY The status bits are received from the local PHY
 *  during ANEG between he MAC and PHY on the SGMII link
 */
#define GMAC_LNKSTS    (BIT(3))
#define GMAC_LNKSTS_M  (GMAC_LNKSTS_V << GMAC_LNKSTS_S)
#define GMAC_LNKSTS_V  0x00000001U
#define GMAC_LNKSTS_S  3
/** GMAC_JABTO : RO; bitpos: [4]; default: 0;
 *  Jabber Timeout This bit indicates whether there is jabber timeout error _1'b1_ in
 *  the received frame This bit is reserved when the MAC is configured for the SGMII or
 *  RGMII PHY interface
 */
#define GMAC_JABTO    (BIT(4))
#define GMAC_JABTO_M  (GMAC_JABTO_V << GMAC_JABTO_S)
#define GMAC_JABTO_V  0x00000001U
#define GMAC_JABTO_S  4
/** GMAC_FALSCARDET : RO; bitpos: [5]; default: 0;
 *  False Carrier Detected This bit indicates whether the SMII PHY detected false
 *  carrier _1'b1_ This bit is reserved when the MAC is configured for the SGMII or
 *  RGMII PHY interface
 */
#define GMAC_FALSCARDET    (BIT(5))
#define GMAC_FALSCARDET_M  (GMAC_FALSCARDET_V << GMAC_FALSCARDET_S)
#define GMAC_FALSCARDET_V  0x00000001U
#define GMAC_FALSCARDET_S  5
/** GMAC_SMIDRXS : R/W; bitpos: [16]; default: 0;
 *  Delay SMII RX Data Sampling with respect to the SMII SYNC Signal When set, the
 *  first bit of the SMII RX data is sampled one cycle after the SMII SYNC signal When
 *  reset, the first bit of the SMII RX data is sampled along with the SMII SYNC signal
 *  If the SMII PHY Interface with source synchronous mode is selected during core
 *  configuration, this bit is reserved _RO with default value_
 */
#define GMAC_SMIDRXS    (BIT(16))
#define GMAC_SMIDRXS_M  (GMAC_SMIDRXS_V << GMAC_SMIDRXS_S)
#define GMAC_SMIDRXS_V  0x00000001U
#define GMAC_SMIDRXS_S  16

/** GMAC_REGISTER55_WATCHDOGTIMEOUTREGISTER_REG register
 *  Controls the watchdog timeout for received frames
 */
#define GMAC_REGISTER55_WATCHDOGTIMEOUTREGISTER_REG (DR_REG_GMAC_BASE + 0xdc)
/** GMAC_WTO : R/W; bitpos: [13:0]; default: 0;
 *  Watchdog Timeout When Bit 16 _PWE_ is set and Bit 23 _WD_ of Register 0 _MAC
 *  Configuration Register_ is reset, this field is used as watchdog timeout for a
 *  received frame If the length of a received frame exceeds the value of this field,
 *  such frame is terminated and declared as an error frame Note: When Bit 16 _PWE_ is
 *  set, the value in this field should be more than 1,522 _0x05F2_ Otherwise, the IEEE
 *  Std 8023specified valid tagged frames are declared as error frames and are dropped
 */
#define GMAC_WTO    0x00003FFFU
#define GMAC_WTO_M  (GMAC_WTO_V << GMAC_WTO_S)
#define GMAC_WTO_V  0x00003FFFU
#define GMAC_WTO_S  0
/** GMAC_PWE : R/W; bitpos: [16]; default: 0;
 *  Programmable Watchdog Enable When this bit is set and Bit 23 _WD_ of Register 0
 *  _MAC Configuration Register_ is reset, the WTO field _Bits[13:0]_ is used as
 *  watchdog timeout for a received frame When this bit is cleared, the watchdog
 *  timeout for a received frame is controlled by the setting of Bit 23 _WD_ and Bit 20
 *  _JE_ in Register 0 _MAC Configuration Register_
 */
#define GMAC_PWE    (BIT(16))
#define GMAC_PWE_M  (GMAC_PWE_V << GMAC_PWE_S)
#define GMAC_PWE_V  0x00000001U
#define GMAC_PWE_S  16

/** GMAC_REGISTER56_GENERALPURPOSEIOREGISTER_REG register
 *  Provides the control to drive up to 4 bits of output ports _GPO_ and also provides
 *  the status of up to 4 input ports _GPIS_
 */
#define GMAC_REGISTER56_GENERALPURPOSEIOREGISTER_REG (DR_REG_GMAC_BASE + 0xe0)
/** GMAC_GPIS : R/W; bitpos: [3:0]; default: 0;
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
#define GMAC_GPIS    0x0000000FU
#define GMAC_GPIS_M  (GMAC_GPIS_V << GMAC_GPIS_S)
#define GMAC_GPIS_V  0x0000000FU
#define GMAC_GPIS_S  0
/** GMAC_GPO : R/W; bitpos: [11:8]; default: 0;
 *  General Purpose Output When this bit is set, it directly drives the gpo_o output
 *  ports When this bit is reset, it does not directly drive the gpo_o output ports The
 *  number of bits available in this field depend on the GP Output Signal Width option
 *  Other bits are not used _reserved and always reset_
 */
#define GMAC_GPO    0x0000000FU
#define GMAC_GPO_M  (GMAC_GPO_V << GMAC_GPO_S)
#define GMAC_GPO_V  0x0000000FU
#define GMAC_GPO_S  8
/** GMAC_GPIE : R/W; bitpos: [19:16]; default: 0;
 *  GPI Interrupt Enable When this bit is set and the programmed event _LL or LH_
 *  occurs on the corresponding GPIS bit, Bit 11 _GPIIS_ of Register 14 _Interrupt
 *  Status Register_ is set Accordingly, the interrupt is generated on the mci_intr_o
 *  or sbd_intr_o The GPIIS bit is cleared when the host reads the Bits[7:0] of this
 *  register When reset, Bit 11 _GPIIS_ of Register 14 _Interrupt Status Register_ is
 *  not set when any event occurs on the corresponding GPIS bits The number of bits
 *  available in this field depend on the GP Input Signal Width option Other bits are
 *  not used _reserved and always reset_
 */
#define GMAC_GPIE    0x0000000FU
#define GMAC_GPIE_M  (GMAC_GPIE_V << GMAC_GPIE_S)
#define GMAC_GPIE_V  0x0000000FU
#define GMAC_GPIE_S  16
/** GMAC_GPIT : R/W; bitpos: [27:24]; default: 0;
 *  GPI Type When set, this bit indicates that the corresponding GPIS is of latchedlow
 *  _LL_ type When reset, this bit indicates that the corresponding GPIS is of
 *  latchedhigh _LH_ type The number of bits available in this field depend on the GP
 *  Input Signal Width option Other bits are not used _reserved and always reset_
 */
#define GMAC_GPIT    0x0000000FU
#define GMAC_GPIT_M  (GMAC_GPIT_V << GMAC_GPIT_S)
#define GMAC_GPIT_V  0x0000000FU
#define GMAC_GPIT_S  24

/** GMAC_REGISTER256_LAYER3ANDLAYER4CONTROLREGISTER0_REG register
 *  Controls the operations of the Layer 3 and Layer 4 frame filtering
 */
#define GMAC_REGISTER256_LAYER3ANDLAYER4CONTROLREGISTER0_REG (DR_REG_GMAC_BASE + 0x400)
/** GMAC_L3PEN0 : R/W; bitpos: [0]; default: 0;
 *  Layer 3 Protocol Enable When set, this bit indicates that the Layer 3 IP Source or
 *  Destination Address matching is enabled for the IPv6 frames When reset, this bit
 *  indicates that the Layer 3 IP Source or Destination Address matching is enabled for
 *  the IPv4 frames The Layer 3 matching is done only when either L3SAM0 or L3DAM0 bit
 *  is set high
 */
#define GMAC_L3PEN0    (BIT(0))
#define GMAC_L3PEN0_M  (GMAC_L3PEN0_V << GMAC_L3PEN0_S)
#define GMAC_L3PEN0_V  0x00000001U
#define GMAC_L3PEN0_S  0
/** GMAC_L3SAM0 : R/W; bitpos: [2]; default: 0;
 *  Layer 3 IP SA Match Enable When set, this bit indicates that the Layer 3 IP Source
 *  Address field is enabled for matching When reset, the MAC ignores the Layer 3 IP
 *  Source Address field for matching Note: When Bit 0 _L3PEN0_ is set, you should set
 *  either this bit or Bit 4 _L3DAM0_ because either IPv6 SA or DA can be checked for
 *  filtering
 */
#define GMAC_L3SAM0    (BIT(2))
#define GMAC_L3SAM0_M  (GMAC_L3SAM0_V << GMAC_L3SAM0_S)
#define GMAC_L3SAM0_V  0x00000001U
#define GMAC_L3SAM0_S  2
/** GMAC_L3SAIM0 : R/W; bitpos: [3]; default: 0;
 *  Layer 3 IP SA Inverse Match Enable When set, this bit indicates that the Layer 3 IP
 *  Source Address field is enabled for inverse matching When reset, this bit indicates
 *  that the Layer 3 IP Source Address field is enabled for perfect matching This bit
 *  is valid and applicable only when Bit 2 _L3SAM0_ is set high
 */
#define GMAC_L3SAIM0    (BIT(3))
#define GMAC_L3SAIM0_M  (GMAC_L3SAIM0_V << GMAC_L3SAIM0_S)
#define GMAC_L3SAIM0_V  0x00000001U
#define GMAC_L3SAIM0_S  3
/** GMAC_L3DAM0 : R/W; bitpos: [4]; default: 0;
 *  Layer 3 IP DA Match Enable When set, this bit indicates that Layer 3 IP Destination
 *  Address field is enabled for matching When reset, the MAC ignores the Layer 3 IP
 *  Destination Address field for matching Note: When Bit 0 _L3PEN0_ is set, you should
 *  set either this bit or Bit 2 _L3SAM0_ because either IPv6 DA or SA can be checked
 *  for filtering
 */
#define GMAC_L3DAM0    (BIT(4))
#define GMAC_L3DAM0_M  (GMAC_L3DAM0_V << GMAC_L3DAM0_S)
#define GMAC_L3DAM0_V  0x00000001U
#define GMAC_L3DAM0_S  4
/** GMAC_L3DAIM0 : R/W; bitpos: [5]; default: 0;
 *  Layer 3 IP DA Inverse Match Enable When set, this bit indicates that the Layer 3 IP
 *  Destination Address field is enabled for inverse matching When reset, this bit
 *  indicates that the Layer 3 IP Destination Address field is enabled for perfect
 *  matching This bit is valid and applicable only when Bit 4 _L3DAM0_ is set high
 */
#define GMAC_L3DAIM0    (BIT(5))
#define GMAC_L3DAIM0_M  (GMAC_L3DAIM0_V << GMAC_L3DAIM0_S)
#define GMAC_L3DAIM0_V  0x00000001U
#define GMAC_L3DAIM0_S  5
/** GMAC_L3HSBM0 : R/W; bitpos: [10:6]; default: 0;
 *  Layer 3 IP SA Higher Bits Match IPv4 Frames: This field contains the number of
 *  lower bits of IP Source Address that are masked for matching in the IPv4 frames The
 *  following list describes the values of this field: 0: No bits are masked 1: LSb[0]
 *  is masked 2: Two LSbs [1:0] are masked  31: All bits except MSb are masked IPv6
 *  Frames: This field contains Bits [4:0] of the field that indicates the number of
 *  higher bits of IP Source or Destination Address matched in the IPv6 frames This
 *  field is valid and applicable only if L3DAM0 or L3SAM0 is set high
 */
#define GMAC_L3HSBM0    0x0000001FU
#define GMAC_L3HSBM0_M  (GMAC_L3HSBM0_V << GMAC_L3HSBM0_S)
#define GMAC_L3HSBM0_V  0x0000001FU
#define GMAC_L3HSBM0_S  6
/** GMAC_L3HDBM0 : R/W; bitpos: [15:11]; default: 0;
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
#define GMAC_L3HDBM0    0x0000001FU
#define GMAC_L3HDBM0_M  (GMAC_L3HDBM0_V << GMAC_L3HDBM0_S)
#define GMAC_L3HDBM0_V  0x0000001FU
#define GMAC_L3HDBM0_S  11
/** GMAC_L4PEN0 : R/W; bitpos: [16]; default: 0;
 *  Layer 4 Protocol Enable When set, this bit indicates that the Source and
 *  Destination Port number fields for UDP frames are used for matching When reset,
 *  this bit indicates that the Source and Destination Port number fields for TCP
 *  frames are used for matching The Layer 4 matching is done only when either L4SPM0
 *  or L4DPM0 bit is set high
 */
#define GMAC_L4PEN0    (BIT(16))
#define GMAC_L4PEN0_M  (GMAC_L4PEN0_V << GMAC_L4PEN0_S)
#define GMAC_L4PEN0_V  0x00000001U
#define GMAC_L4PEN0_S  16
/** GMAC_L4SPM0 : R/W; bitpos: [18]; default: 0;
 *  Layer 4 Source Port Match Enable When set, this bit indicates that the Layer 4
 *  Source Port number field is enabled for matching When reset, the MAC ignores the
 *  Layer 4 Source Port number field for matching
 */
#define GMAC_L4SPM0    (BIT(18))
#define GMAC_L4SPM0_M  (GMAC_L4SPM0_V << GMAC_L4SPM0_S)
#define GMAC_L4SPM0_V  0x00000001U
#define GMAC_L4SPM0_S  18
/** GMAC_L4SPIM0 : R/W; bitpos: [19]; default: 0;
 *  Layer 4 Source Port Inverse Match Enable When set, this bit indicates that the
 *  Layer 4 Source Port number field is enabled for inverse matching When reset, this
 *  bit indicates that the Layer 4 Source Port number field is enabled for perfect
 *  matching This bit is valid and applicable only when Bit 18 _L4SPM0_ is set high
 */
#define GMAC_L4SPIM0    (BIT(19))
#define GMAC_L4SPIM0_M  (GMAC_L4SPIM0_V << GMAC_L4SPIM0_S)
#define GMAC_L4SPIM0_V  0x00000001U
#define GMAC_L4SPIM0_S  19
/** GMAC_L4DPM0 : R/W; bitpos: [20]; default: 0;
 *  Layer 4 Destination Port Match Enable When set, this bit indicates that the Layer 4
 *  Destination Port number field is enabled for matching When reset, the MAC ignores
 *  the Layer 4 Destination Port number field for matching
 */
#define GMAC_L4DPM0    (BIT(20))
#define GMAC_L4DPM0_M  (GMAC_L4DPM0_V << GMAC_L4DPM0_S)
#define GMAC_L4DPM0_V  0x00000001U
#define GMAC_L4DPM0_S  20
/** GMAC_L4DPIM0 : R/W; bitpos: [21]; default: 0;
 *  Layer 4 Destination Port Inverse Match Enable When set, this bit indicates that the
 *  Layer 4 Destination Port number field is enabled for inverse matching When reset,
 *  this bit indicates that the Layer 4 Destination Port number field is enabled for
 *  perfect matching This bit is valid and applicable only when Bit 20 _L4DPM0_ is set
 *  high
 */
#define GMAC_L4DPIM0    (BIT(21))
#define GMAC_L4DPIM0_M  (GMAC_L4DPIM0_V << GMAC_L4DPIM0_S)
#define GMAC_L4DPIM0_V  0x00000001U
#define GMAC_L4DPIM0_S  21

/** GMAC_REGISTER257_LAYER4ADDRESSREGISTER0_REG register
 *  Layer 4 Port number field It contains the 16bit Source and Destination Port numbers
 *  of the TCP or UDP frame
 */
#define GMAC_REGISTER257_LAYER4ADDRESSREGISTER0_REG (DR_REG_GMAC_BASE + 0x404)
/** GMAC_L4SP0 : R/W; bitpos: [15:0]; default: 0;
 *  Layer 4 Source Port Number Field When Bit 16 _L4PEN0_ is reset and Bit 20 _L4DPM0_
 *  is set in Register 256 _Layer 3 and Layer 4 Control Register 0_, this field
 *  contains the value to be matched with the TCP Source Port Number field in the IPv4
 *  or IPv6 frames When Bit 16 _L4PEN0_ and Bit 20 _L4DPM0_ are set in Register 256
 *  _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
 *  matched with the UDP Source Port Number field in the IPv4 or IPv6 frames
 */
#define GMAC_L4SP0    0x0000FFFFU
#define GMAC_L4SP0_M  (GMAC_L4SP0_V << GMAC_L4SP0_S)
#define GMAC_L4SP0_V  0x0000FFFFU
#define GMAC_L4SP0_S  0
/** GMAC_L4DP0 : R/W; bitpos: [31:16]; default: 0;
 *  Layer 4 Destination Port Number Field When Bit 16 _L4PEN0_ is reset and Bit 20
 *  _L4DPM0_ is set in Register 256 _Layer 3 and Layer 4 Control Register 0_, this
 *  field contains the value to be matched with the TCP Destination Port Number field
 *  in the IPv4 or IPv6 frames When Bit 16 _L4PEN0_ and Bit 20 _L4DPM0_ are set in
 *  Register 256 _Layer 3 and Layer 4 Control Register 0_, this field contains the
 *  value to be matched with the UDP Destination Port Number field in the IPv4 or IPv6
 *  frames
 */
#define GMAC_L4DP0    0x0000FFFFU
#define GMAC_L4DP0_M  (GMAC_L4DP0_V << GMAC_L4DP0_S)
#define GMAC_L4DP0_V  0x0000FFFFU
#define GMAC_L4DP0_S  16

/** GMAC_REGISTER260_LAYER3ADDRESS0REGISTER0_REG register
 *  Layer 3 Address field For IPv4 frames, it contains the 32bit IP Source Address
 *  field For IPv6 frames, it contains Bits [31:0] of the 128bit IP Source Address or
 *  Destination Address field
 */
#define GMAC_REGISTER260_LAYER3ADDRESS0REGISTER0_REG (DR_REG_GMAC_BASE + 0x410)
/** GMAC_L3A00 : R/W; bitpos: [31:0]; default: 0;
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
#define GMAC_L3A00    0xFFFFFFFFU
#define GMAC_L3A00_M  (GMAC_L3A00_V << GMAC_L3A00_S)
#define GMAC_L3A00_V  0xFFFFFFFFU
#define GMAC_L3A00_S  0

/** GMAC_REGISTER261_LAYER3ADDRESS1REGISTER0_REG register
 *  Layer 3 Address 1 field For IPv4 frames, it contains the 32bit IP Destination
 *  Address field For IPv6 frames, it contains Bits [63:32] of the 128bit IP Source
 *  Address or Destination Address field
 */
#define GMAC_REGISTER261_LAYER3ADDRESS1REGISTER0_REG (DR_REG_GMAC_BASE + 0x414)
/** GMAC_L3A10 : R/W; bitpos: [31:0]; default: 0;
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
#define GMAC_L3A10    0xFFFFFFFFU
#define GMAC_L3A10_M  (GMAC_L3A10_V << GMAC_L3A10_S)
#define GMAC_L3A10_V  0xFFFFFFFFU
#define GMAC_L3A10_S  0

/** GMAC_REGISTER262_LAYER3ADDRESS2REGISTER0_REG register
 *  Layer 3 Address 2 field This register is reserved for IPv4 frames For IPv6 frames,
 *  it contains Bits [95:64] of the 128bit IP Source Address or Destination Address
 *  field
 */
#define GMAC_REGISTER262_LAYER3ADDRESS2REGISTER0_REG (DR_REG_GMAC_BASE + 0x418)
/** GMAC_L3A20 : R/W; bitpos: [31:0]; default: 0;
 *  Layer 3 Address 2 Field When Bit 0 _L3PEN0_ and Bit 2 _L3SAM0_ are set in Register
 *  256 _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
 *  matched with Bits [95:64] of the IP Source Address field in the IPv6 frames When
 *  Bit 0 _L3PEN0_ and Bit 4 _L3DAM0_ are set in Register 256 _Layer 3 and Layer 4
 *  Control Register 0_, this field contains value to be matched with Bits [95:64] of
 *  the IP Destination Address field in the IPv6 frames When Bit 0 _L3PEN0_ is reset in
 *  Register 256 _Layer 3 and Layer 4 Control Register 0_, this register is not used
 */
#define GMAC_L3A20    0xFFFFFFFFU
#define GMAC_L3A20_M  (GMAC_L3A20_V << GMAC_L3A20_S)
#define GMAC_L3A20_V  0xFFFFFFFFU
#define GMAC_L3A20_S  0

/** GMAC_REGISTER263_LAYER3ADDRESS3REGISTER0_REG register
 *  Layer 3 Address 3 field This register is reserved for IPv4 frames For IPv6 frames,
 *  it contains Bits [127:96] of the 128bit IP Source Address or Destination Address
 *  field
 */
#define GMAC_REGISTER263_LAYER3ADDRESS3REGISTER0_REG (DR_REG_GMAC_BASE + 0x41c)
/** GMAC_L3A30 : R/W; bitpos: [31:0]; default: 0;
 *  Layer 3 Address 3 Field When Bit 0 _L3PEN0_ and Bit 2 _L3SAM0_ are set in Register
 *  256 _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
 *  matched with Bits [127:96] of the IP Source Address field in the IPv6 frames When
 *  Bit 0 _L3PEN0_ and Bit 4 _L3DAM0_ are set in Register 256 _Layer 3 and Layer 4
 *  Control Register 0_, this field contains the value to be matched with Bits [127:96]
 *  of the IP Destination Address field in the IPv6 frames When Bit 0 _L3PEN0_ is reset
 *  in Register 256 _Layer 3 and Layer 4 Control Register 0_, this register is not used
 */
#define GMAC_L3A30    0xFFFFFFFFU
#define GMAC_L3A30_M  (GMAC_L3A30_V << GMAC_L3A30_S)
#define GMAC_L3A30_V  0xFFFFFFFFU
#define GMAC_L3A30_S  0

/** GMAC_REGISTER320_HASHTABLEREGISTER0_REG register
 *  This register contains the first 32 bits of the hash table when the width of the
 *  Hash table is 128 bits or 256 bits
 */
#define GMAC_REGISTER320_HASHTABLEREGISTER0_REG (DR_REG_GMAC_BASE + 0x500)
/** GMAC_HT31T0 : R/W; bitpos: [31:0]; default: 0;
 *  First 32 bits of Hash Table This field contains the first 32 Bits _31:0_ of the
 *  Hash table Note Registers 321 through 327 are similar to Register 320 _Hash Table
 *  Register 0_ Registers 324 through 327 are present only when you select the 256bit
 *  Hash table during core configuration
 */
#define GMAC_HT31T0    0xFFFFFFFFU
#define GMAC_HT31T0_M  (GMAC_HT31T0_V << GMAC_HT31T0_S)
#define GMAC_HT31T0_V  0xFFFFFFFFU
#define GMAC_HT31T0_S  0

/** GMAC_REGISTER353_VLANTAGINCLUSIONORREPLACEMENTREGISTER_REG register
 *  This register contains the VLAN tag for insertion into or replacement in the
 *  transmit frames
 */
#define GMAC_REGISTER353_VLANTAGINCLUSIONORREPLACEMENTREGISTER_REG (DR_REG_GMAC_BASE + 0x584)
/** GMAC_VLT : R/W; bitpos: [15:0]; default: 0;
 *  VLAN Tag for Transmit Frames This field contains the value of the VLAN tag to be
 *  inserted or replaced The value must only be changed when the transmit lines are
 *  inactive or during the initialization phase Bits[15:13] are the User Priority, Bit
 *  12 is the CFI/DEI, and Bits[11:0] are the VLAN tag’s VID field
 */
#define GMAC_VLT    0x0000FFFFU
#define GMAC_VLT_M  (GMAC_VLT_V << GMAC_VLT_S)
#define GMAC_VLT_V  0x0000FFFFU
#define GMAC_VLT_S  0
/** GMAC_VLC : R/W; bitpos: [17:16]; default: 0;
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
#define GMAC_VLC    0x00000003U
#define GMAC_VLC_M  (GMAC_VLC_V << GMAC_VLC_S)
#define GMAC_VLC_V  0x00000003U
#define GMAC_VLC_S  16
/** GMAC_VLP : R/W; bitpos: [18]; default: 0;
 *  VLAN Priority Control When this bit is set, the control Bits [17:16] are used for
 *  VLAN deletion, insertion, or replacement When this bit is reset, the
 *  mti_vlan_ctrl_i control input is used, and Bits [17:16] are ignored
 */
#define GMAC_VLP    (BIT(18))
#define GMAC_VLP_M  (GMAC_VLP_V << GMAC_VLP_S)
#define GMAC_VLP_V  0x00000001U
#define GMAC_VLP_S  18
/** GMAC_CSVL : R/W; bitpos: [19]; default: 0;
 *  CVLAN or SVLAN When this bit is set, SVLAN type _0x88A8_ is inserted or replaced in
 *  the 13th and 14th bytes of transmitted frames When this bit is reset, CVLAN type
 *  _0x8100_ is inserted or replaced in the transmitted frames
 */
#define GMAC_CSVL    (BIT(19))
#define GMAC_CSVL_M  (GMAC_CSVL_V << GMAC_CSVL_S)
#define GMAC_CSVL_V  0x00000001U
#define GMAC_CSVL_S  19

/** GMAC_REGISTER354_VLANHASHTABLEREGISTER_REG register
 *  This register contains the VLAN hash table
 */
#define GMAC_REGISTER354_VLANHASHTABLEREGISTER_REG (DR_REG_GMAC_BASE + 0x588)
/** GMAC_VLHT : R/W; bitpos: [15:0]; default: 0;
 *  VLAN Hash Table This field contains the 16bit VLAN Hash Table
 */
#define GMAC_VLHT    0x0000FFFFU
#define GMAC_VLHT_M  (GMAC_VLHT_V << GMAC_VLHT_S)
#define GMAC_VLHT_V  0x0000FFFFU
#define GMAC_VLHT_S  0

/** GMAC_REGISTER448_TIMESTAMPCONTROLREGISTER_REG register
 *  Controls the timestamp generation and update logic This register is present only
 *  when IEEE1588 timestamping is enabled during coreConsultant configuration
 */
#define GMAC_REGISTER448_TIMESTAMPCONTROLREGISTER_REG (DR_REG_GMAC_BASE + 0x700)
/** GMAC_TSENA : R/W; bitpos: [0]; default: 0;
 *  Timestamp Enable When set, the timestamp is added for the transmit and receive
 *  frames When disabled, timestamp is not added for the transmit and receive frames
 *  and the Timestamp Generator is also suspended You need to initialize the Timestamp
 *  _system time_ after enabling this mode On the receive side, the MAC processes the
 *  1588 frames only if this bit is set
 */
#define GMAC_TSENA    (BIT(0))
#define GMAC_TSENA_M  (GMAC_TSENA_V << GMAC_TSENA_S)
#define GMAC_TSENA_V  0x00000001U
#define GMAC_TSENA_S  0
/** GMAC_TSCFUPDT : R/W; bitpos: [1]; default: 0;
 *  Timestamp Fine or Coarse Update When set, this bit indicates that the system times
 *  update should be done using the fine update method When reset, it indicates the
 *  system timestamp update should be done using the Coarse method
 */
#define GMAC_TSCFUPDT    (BIT(1))
#define GMAC_TSCFUPDT_M  (GMAC_TSCFUPDT_V << GMAC_TSCFUPDT_S)
#define GMAC_TSCFUPDT_V  0x00000001U
#define GMAC_TSCFUPDT_S  1
/** GMAC_TSINIT : R/W1S; bitpos: [2]; default: 0;
 *  Timestamp Initialize When set, the system time is initialized _overwritten_ with
 *  the value specified in the Register 452 _System Time  Seconds Update Register_ and
 *  Register 453 _System Time  Nanoseconds Update Register_ This bit should be read
 *  zero before updating it This bit is reset when the initialization is complete The
 *  “Timestamp Higher Word” register _if enabled during core configuration_ can only be
 *  initialized
 */
#define GMAC_TSINIT    (BIT(2))
#define GMAC_TSINIT_M  (GMAC_TSINIT_V << GMAC_TSINIT_S)
#define GMAC_TSINIT_V  0x00000001U
#define GMAC_TSINIT_S  2
/** GMAC_TSUPDT : R/W1S; bitpos: [3]; default: 0;
 *  Timestamp Update When set, the system time is updated _added or subtracted_ with
 *  the value specified in Register 452 _System Time  Seconds Update Register_ and
 *  Register 453 _System Time  Nanoseconds Update Register_ This bit should be read
 *  zero before updating it This bit is reset when the update is completed in hardware
 *  The “Timestamp Higher Word” register _if enabled during core configuration_ is not
 *  updated
 */
#define GMAC_TSUPDT    (BIT(3))
#define GMAC_TSUPDT_M  (GMAC_TSUPDT_V << GMAC_TSUPDT_S)
#define GMAC_TSUPDT_V  0x00000001U
#define GMAC_TSUPDT_S  3
/** GMAC_TSTRIG : R/W1S; bitpos: [4]; default: 0;
 *  Timestamp Interrupt Trigger Enable When set, the timestamp interrupt is generated
 *  when the System Time becomes greater than the value written in the Target Time
 *  register This bit is reset after the generation of the Timestamp Trigger Interrupt
 */
#define GMAC_TSTRIG    (BIT(4))
#define GMAC_TSTRIG_M  (GMAC_TSTRIG_V << GMAC_TSTRIG_S)
#define GMAC_TSTRIG_V  0x00000001U
#define GMAC_TSTRIG_S  4
/** GMAC_TSADDREG : R/W1S; bitpos: [5]; default: 0;
 *  Addend Reg Update When set, the content of the Timestamp Addend register is updated
 *  in the PTP block for fine correction This is cleared when the update is completed
 *  This register bit should be zero before setting it
 */
#define GMAC_TSADDREG    (BIT(5))
#define GMAC_TSADDREG_M  (GMAC_TSADDREG_V << GMAC_TSADDREG_S)
#define GMAC_TSADDREG_V  0x00000001U
#define GMAC_TSADDREG_S  5
/** GMAC_TSENALL : R/W; bitpos: [8]; default: 0;
 *  Enable Timestamp for All Frames When set, the timestamp snapshot is enabled for all
 *  frames received by the MAC
 */
#define GMAC_TSENALL    (BIT(8))
#define GMAC_TSENALL_M  (GMAC_TSENALL_V << GMAC_TSENALL_S)
#define GMAC_TSENALL_V  0x00000001U
#define GMAC_TSENALL_S  8
/** GMAC_TSCTRLSSR : R/W; bitpos: [9]; default: 0;
 *  Timestamp Digital or Binary Rollover Control When set, the Timestamp Low register
 *  rolls over after 0x3B9A_C9FF value _that is, 1 nanosecond accuracy_ and increments
 *  the timestamp _High_ seconds When reset, the rollover value of subsecond register
 *  is 0x7FFF_FFFF The subsecond increment has to be programmed correctly depending on
 *  the PTP reference clock frequency and the value of this bit
 */
#define GMAC_TSCTRLSSR    (BIT(9))
#define GMAC_TSCTRLSSR_M  (GMAC_TSCTRLSSR_V << GMAC_TSCTRLSSR_S)
#define GMAC_TSCTRLSSR_V  0x00000001U
#define GMAC_TSCTRLSSR_S  9
/** GMAC_TSVER2ENA : R/W; bitpos: [10]; default: 0;
 *  Enable PTP packet Processing for Version 2 Format When set, the PTP packets are
 *  processed using the 1588 version 2 format Otherwise, the PTP packets are processed
 *  using the version 1 format The IEEE 1588 Version 1 and Version 2 format are
 *  described in “PTP Processing and Control” on page 155
 */
#define GMAC_TSVER2ENA    (BIT(10))
#define GMAC_TSVER2ENA_M  (GMAC_TSVER2ENA_V << GMAC_TSVER2ENA_S)
#define GMAC_TSVER2ENA_V  0x00000001U
#define GMAC_TSVER2ENA_S  10
/** GMAC_TSIPENA : R/W; bitpos: [11]; default: 0;
 *  Enable Processing of PTP over Ethernet Frames When set, the MAC receiver processes
 *  the PTP packets encapsulated directly in the Ethernet frames When this bit is
 *  clear, the MAC ignores the PTP over Ethernet packets
 */
#define GMAC_TSIPENA    (BIT(11))
#define GMAC_TSIPENA_M  (GMAC_TSIPENA_V << GMAC_TSIPENA_S)
#define GMAC_TSIPENA_V  0x00000001U
#define GMAC_TSIPENA_S  11
/** GMAC_TSIPV6ENA : R/W; bitpos: [12]; default: 0;
 *  Enable Processing of PTP Frames Sent over IPv6UDP When set, the MAC receiver
 *  processes PTP packets encapsulated in UDP over IPv6 packets When this bit is clear,
 *  the MAC ignores the PTP transported over UDPIPv6 packets
 */
#define GMAC_TSIPV6ENA    (BIT(12))
#define GMAC_TSIPV6ENA_M  (GMAC_TSIPV6ENA_V << GMAC_TSIPV6ENA_S)
#define GMAC_TSIPV6ENA_V  0x00000001U
#define GMAC_TSIPV6ENA_S  12
/** GMAC_TSIPV4ENA : R/W; bitpos: [13]; default: 1;
 *  Enable Processing of PTP Frames Sent over IPv4UDP When set, the MAC receiver
 *  processes the PTP packets encapsulated in UDP over IPv4 packets When this bit is
 *  clear, the MAC ignores the PTP transported over UDPIPv4 packets This bit is set by
 *  default
 */
#define GMAC_TSIPV4ENA    (BIT(13))
#define GMAC_TSIPV4ENA_M  (GMAC_TSIPV4ENA_V << GMAC_TSIPV4ENA_S)
#define GMAC_TSIPV4ENA_V  0x00000001U
#define GMAC_TSIPV4ENA_S  13
/** GMAC_TSEVNTENA : R/W; bitpos: [14]; default: 0;
 *  Enable Timestamp Snapshot for Event Messages When set, the timestamp snapshot is
 *  taken only for event messages _SYNC, Delay_Req, Pdelay_Req, or Pdelay_Resp_ When
 *  reset, the snapshot is taken for all messages except Announce, Management, and
 *  Signaling For more information about the timestamp snapshots, see Table 670 on page
 *  462
 */
#define GMAC_TSEVNTENA    (BIT(14))
#define GMAC_TSEVNTENA_M  (GMAC_TSEVNTENA_V << GMAC_TSEVNTENA_S)
#define GMAC_TSEVNTENA_V  0x00000001U
#define GMAC_TSEVNTENA_S  14
/** GMAC_TSMSTRENA : R/W; bitpos: [15]; default: 0;
 *  Enable Snapshot for Messages Relevant to Master When set, the snapshot is taken
 *  only for the messages relevant to the master node Otherwise, the snapshot is taken
 *  for the messages relevant to the slave node
 */
#define GMAC_TSMSTRENA    (BIT(15))
#define GMAC_TSMSTRENA_M  (GMAC_TSMSTRENA_V << GMAC_TSMSTRENA_S)
#define GMAC_TSMSTRENA_V  0x00000001U
#define GMAC_TSMSTRENA_S  15
/** GMAC_SNAPTYPSEL : R/W; bitpos: [17:16]; default: 0;
 *  Select PTP packets for Taking Snapshots These bits along with Bits 15 and 14 decide
 *  the set of PTP packet types for which snapshot needs to be taken The encoding is
 *  given in Table 670 on page 462
 */
#define GMAC_SNAPTYPSEL    0x00000003U
#define GMAC_SNAPTYPSEL_M  (GMAC_SNAPTYPSEL_V << GMAC_SNAPTYPSEL_S)
#define GMAC_SNAPTYPSEL_V  0x00000003U
#define GMAC_SNAPTYPSEL_S  16
/** GMAC_TSENMACADDR : R/W; bitpos: [18]; default: 0;
 *  Enable MAC address for PTP Frame Filtering When set, the DA MAC address _that
 *  matches any MAC Address register_ is used to filter the PTP frames when PTP is
 *  directly sent over Ethernet
 */
#define GMAC_TSENMACADDR    (BIT(18))
#define GMAC_TSENMACADDR_M  (GMAC_TSENMACADDR_V << GMAC_TSENMACADDR_S)
#define GMAC_TSENMACADDR_V  0x00000001U
#define GMAC_TSENMACADDR_S  18
/** GMAC_ATSFC : R/W1S; bitpos: [24]; default: 0;
 *  Auxiliary Snapshot FIFO Clear When set, it resets the pointers of the Auxiliary
 *  Snapshot FIFO This bit is cleared when the pointers are reset and the FIFO is empty
 *  When this bit is high, auxiliary snapshots get stored in the FIFO This bit is
 *  reserved when the Add IEEE 1588 Auxiliary Snapshot option is not selected during
 *  core configuration
 */
#define GMAC_ATSFC    (BIT(24))
#define GMAC_ATSFC_M  (GMAC_ATSFC_V << GMAC_ATSFC_S)
#define GMAC_ATSFC_V  0x00000001U
#define GMAC_ATSFC_S  24
/** GMAC_ATSEN0 : R/W; bitpos: [25]; default: 0;
 *  Auxiliary Snapshot 0 Enable This field controls capturing the Auxiliary Snapshot
 *  Trigger 0 When this bit is set, the Auxiliary snapshot of event on
 *  ptp_aux_trig_i[0] input is enabled When this bit is reset, the events on this input
 *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
 *  is not selected during core configuration
 */
#define GMAC_ATSEN0    (BIT(25))
#define GMAC_ATSEN0_M  (GMAC_ATSEN0_V << GMAC_ATSEN0_S)
#define GMAC_ATSEN0_V  0x00000001U
#define GMAC_ATSEN0_S  25
/** GMAC_ATSEN1 : R/W; bitpos: [26]; default: 0;
 *  Auxiliary Snapshot 1 Enable This field controls capturing the Auxiliary Snapshot
 *  Trigger 1 When this bit is set, the Auxiliary snapshot of event on
 *  ptp_aux_trig_i[1] input is enabled When this bit is reset, the events on this input
 *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
 *  is not selected during core configuration or the selected number in the Number of
 *  IEEE 1588 Auxiliary Snapshot Inputs option is less than two
 */
#define GMAC_ATSEN1    (BIT(26))
#define GMAC_ATSEN1_M  (GMAC_ATSEN1_V << GMAC_ATSEN1_S)
#define GMAC_ATSEN1_V  0x00000001U
#define GMAC_ATSEN1_S  26
/** GMAC_ATSEN2 : R/W; bitpos: [27]; default: 0;
 *  Auxiliary Snapshot 2 Enable This field controls capturing the Auxiliary Snapshot
 *  Trigger 2 When this bit is set, the Auxiliary snapshot of event on
 *  ptp_aux_trig_i[2] input is enabled When this bit is reset, the events on this input
 *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
 *  is not selected during core configuration or the selected number in the Number of
 *  IEEE 1588 Auxiliary Snapshot Inputs option is less than three
 */
#define GMAC_ATSEN2    (BIT(27))
#define GMAC_ATSEN2_M  (GMAC_ATSEN2_V << GMAC_ATSEN2_S)
#define GMAC_ATSEN2_V  0x00000001U
#define GMAC_ATSEN2_S  27
/** GMAC_ATSEN3 : R/W; bitpos: [28]; default: 0;
 *  Auxiliary Snapshot 3 Enable This field controls capturing the Auxiliary Snapshot
 *  Trigger 3 When this bit is set, the Auxiliary snapshot of event on
 *  ptp_aux_trig_i[3] input is enabled When this bit is reset, the events on this input
 *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
 *  is not selected during core configuration or the selected number in the Number of
 *  IEEE 1588 Auxiliary Snapshot Inputs option is less than four
 */
#define GMAC_ATSEN3    (BIT(28))
#define GMAC_ATSEN3_M  (GMAC_ATSEN3_V << GMAC_ATSEN3_S)
#define GMAC_ATSEN3_V  0x00000001U
#define GMAC_ATSEN3_S  28

/** GMAC_REGISTER449_SUBSECONDINCREMENTREGISTER_REG register
 *  Contains the 8bit value by which the SubSecond register is incremented This
 *  register is present only when IEEE1588 timestamping is enabled without an external
 *  timestamp input
 */
#define GMAC_REGISTER449_SUBSECONDINCREMENTREGISTER_REG (DR_REG_GMAC_BASE + 0x704)
/** GMAC_SSINC : R/W; bitpos: [7:0]; default: 0;
 *  Subsecond Increment Value The value programmed in this field is accumulated every
 *  clock cycle _of clk_ptp_i_ with the contents of the subsecond register For example,
 *  when PTP clock is 50 MHz _period is 20 ns_, you should program 20 _0x14_ when the
 *  System Time Nanoseconds register has an accuracy of 1 ns [Bit 9 _TSCTRLSSR_ is set
 *  in Register 448 _Timestamp Control Register_] When TSCTRLSSR is clear, the
 *  Nanoseconds register has a resolution of ~0465ns In this case, you should program a
 *  value of 43 _0x2B_ that is derived by 20ns/0465
 */
#define GMAC_SSINC    0x000000FFU
#define GMAC_SSINC_M  (GMAC_SSINC_V << GMAC_SSINC_S)
#define GMAC_SSINC_V  0x000000FFU
#define GMAC_SSINC_S  0

/** GMAC_REGISTER450_SYSTEMTIMESECONDSREGISTER_REG register
 *  Contains the lower 32 bits of the seconds field of the system time This register is
 *  present only when IEEE1588 timestamping is enabled without an external timestamp
 *  input
 */
#define GMAC_REGISTER450_SYSTEMTIMESECONDSREGISTER_REG (DR_REG_GMAC_BASE + 0x708)
/** GMAC_TSS_RO : RO; bitpos: [31:0]; default: 0;
 *  Timestamp Second The value in this field indicates the current value in seconds of
 *  the System Time maintained by the MAC
 */
#define GMAC_TSS_RO    0xFFFFFFFFU
#define GMAC_TSS_RO_M  (GMAC_TSS_RO_V << GMAC_TSS_RO_S)
#define GMAC_TSS_RO_V  0xFFFFFFFFU
#define GMAC_TSS_RO_S  0

/** GMAC_REGISTER451_SYSTEMTIMENANOSECONDSREGISTER_REG register
 *  Contains 32 bits of the nanoseconds field of the system time This register is only
 *  present when IEEE1588 timestamping is enabled without an external timestamp input
 */
#define GMAC_REGISTER451_SYSTEMTIMENANOSECONDSREGISTER_REG (DR_REG_GMAC_BASE + 0x70c)
/** GMAC_TSSS_RO : RO; bitpos: [30:0]; default: 0;
 *  Timestamp Sub Seconds The value in this field has the sub second representation of
 *  time, with an accuracy of 046 ns When Bit 9 _TSCTRLSSR_ is set in Register 448
 *  _Timestamp Control Register_, each bit represents 1 ns and the maximum value is
 *  0x3B9A_C9FF, after which it rollsover to zero
 */
#define GMAC_TSSS_RO    0x7FFFFFFFU
#define GMAC_TSSS_RO_M  (GMAC_TSSS_RO_V << GMAC_TSSS_RO_S)
#define GMAC_TSSS_RO_V  0x7FFFFFFFU
#define GMAC_TSSS_RO_S  0

/** GMAC_REGISTER452_SYSTEMTIMESECONDSUPDATEREGISTER_REG register
 *  Contains the lower 32 bits of the seconds field to be written to, added to, or
 *  subtracted from the System Time value This register is only present when IEEE1588
 *  timestamping is enabled without an external timestamp input
 */
#define GMAC_REGISTER452_SYSTEMTIMESECONDSUPDATEREGISTER_REG (DR_REG_GMAC_BASE + 0x710)
/** GMAC_TSS : R/W; bitpos: [31:0]; default: 0;
 *  TIMESTAMP SECOND THE VALUE IN THIS FIELD INDICATES THE TIME IN SECONDS TO BE
 *  INITIALIZED OR ADDED TO THE SYSTEM TIME
 */
#define GMAC_TSS    0xFFFFFFFFU
#define GMAC_TSS_M  (GMAC_TSS_V << GMAC_TSS_S)
#define GMAC_TSS_V  0xFFFFFFFFU
#define GMAC_TSS_S  0

/** GMAC_REGISTER453_SYSTEMTIMENANOSECONDSUPDATEREGISTER_REG register
 *  Contains 32 bits of the nanoseconds field to be written to, added to, or subtracted
 *  from the System Time value This register is only present when IEEE1588 timestamping
 *  is enabled without an external timestamp input
 */
#define GMAC_REGISTER453_SYSTEMTIMENANOSECONDSUPDATEREGISTER_REG (DR_REG_GMAC_BASE + 0x714)
/** GMAC_TSSS : R/W; bitpos: [30:0]; default: 0;
 *  Timestamp Sub Seconds The value in this field has the sub second representation of
 *  time, with an accuracy of 046 ns When Bit 9 _TSCTRLSSR_ is set in Register 448
 *  _Timestamp Control Register_, each bit represents 1 ns and the programmed value
 *  should not exceed 0x3B9A_C9FF
 */
#define GMAC_TSSS    0x7FFFFFFFU
#define GMAC_TSSS_M  (GMAC_TSSS_V << GMAC_TSSS_S)
#define GMAC_TSSS_V  0x7FFFFFFFU
#define GMAC_TSSS_S  0
/** GMAC_ADDSUB : R/W; bitpos: [31]; default: 0;
 *  Add or Subtract Time When this bit is set, the time value is subtracted with the
 *  contents of the update register When this bit is reset, the time value is added
 *  with the contents of the update register
 */
#define GMAC_ADDSUB    (BIT(31))
#define GMAC_ADDSUB_M  (GMAC_ADDSUB_V << GMAC_ADDSUB_S)
#define GMAC_ADDSUB_V  0x00000001U
#define GMAC_ADDSUB_S  31

/** GMAC_REGISTER454_TIMESTAMPADDENDREGISTER_REG register
 *  This register is used by the software to readjust the clock frequency linearly to
 *  match the master clock frequency This register is only present when IEEE1588
 *  timestamping is enabled without an external timestamp input
 */
#define GMAC_REGISTER454_TIMESTAMPADDENDREGISTER_REG (DR_REG_GMAC_BASE + 0x718)
/** GMAC_TSAR : R/W; bitpos: [31:0]; default: 0;
 *  Timestamp Addend Register This field indicates the 32bit time value to be added to
 *  the Accumulator register to achieve time synchronization
 */
#define GMAC_TSAR    0xFFFFFFFFU
#define GMAC_TSAR_M  (GMAC_TSAR_V << GMAC_TSAR_S)
#define GMAC_TSAR_V  0xFFFFFFFFU
#define GMAC_TSAR_S  0

/** GMAC_REGISTER455_TARGETTIMESECONDSREGISTER_REG register
 *  Contains the higher 32 bits of time to be compared with the system time for
 *  interrupt event generation or to start the PPS signal output generation This
 *  register is present only when IEEE1588 timestamping is enabled without an external
 *  timestamp input
 */
#define GMAC_REGISTER455_TARGETTIMESECONDSREGISTER_REG (DR_REG_GMAC_BASE + 0x71c)
/** GMAC_TSTR : R/W; bitpos: [31:0]; default: 0;
 *  Target Time Seconds Register This register stores the time in seconds When the
 *  timestamp value matches or exceeds both Target Timestamp registers, then based on
 *  Bits [6:5] of Register 459 _PPS Control Register_, the MAC starts or stops the PPS
 *  signal output and generates an interrupt _if enabled_
 */
#define GMAC_TSTR    0xFFFFFFFFU
#define GMAC_TSTR_M  (GMAC_TSTR_V << GMAC_TSTR_S)
#define GMAC_TSTR_V  0xFFFFFFFFU
#define GMAC_TSTR_S  0

/** GMAC_REGISTER456_TARGETTIMENANOSECONDSREGISTER_REG register
 *  Contains the lower 32 bits of time to be compared with the system time for
 *  interrupt event generation or to start the PPS signal output generation This
 *  register is present only when IEEE1588 timestamping is enabled without an external
 *  timestamp input
 */
#define GMAC_REGISTER456_TARGETTIMENANOSECONDSREGISTER_REG (DR_REG_GMAC_BASE + 0x720)
/** GMAC_TTSLO : R/W; bitpos: [30:0]; default: 0;
 *  Target Timestamp Low Register This register stores the time in _signed_ nanoseconds
 *  When the value of the timestamp matches the both Target Timestamp registers, then
 *  based on the TRGTMODSEL0 field _Bits [6:5]_ in Register 459 _PPS Control Register_,
 *  the MAC starts or stops the PPS signal output and generates an interrupt _if
 *  enabled_ This value should not exceed 0x3B9A_C9FF when Bit 9 _TSCTRLSSR_ is set in
 *  Register 448 _Timestamp Control Register_ The actual start or stop time of the PPS
 *  signal output may have an error margin up to one unit of subsecond increment value
 */
#define GMAC_TTSLO    0x7FFFFFFFU
#define GMAC_TTSLO_M  (GMAC_TTSLO_V << GMAC_TTSLO_S)
#define GMAC_TTSLO_V  0x7FFFFFFFU
#define GMAC_TTSLO_S  0
/** GMAC_TRGTBUSY : R/W1S; bitpos: [31]; default: 0;
 *  Target Time Register Busy The MAC sets this bit when the PPSCMD field _Bit [3:0]_
 *  in Register 459 _PPS Control Register_ is programmed to 010 or 011 Programming the
 *  PPSCMD field to 010 or 011, instructs the MAC to synchronize the Target Time
 *  Registers to the PTP clock domain The MAC clears this bit after synchronizing the
 *  Target Time Registers to the PTP clock domain The application must not update the
 *  Target Time Registers when this bit is read as 1 Otherwise, the synchronization of
 *  the previous programmed time gets corrupted This bit is reserved when the Enable
 *  Flexible PulsePerSecond Output feature is not selected
 */
#define GMAC_TRGTBUSY    (BIT(31))
#define GMAC_TRGTBUSY_M  (GMAC_TRGTBUSY_V << GMAC_TRGTBUSY_S)
#define GMAC_TRGTBUSY_V  0x00000001U
#define GMAC_TRGTBUSY_S  31

/** GMAC_REGISTER457_SYSTEMTIMEHIGHERWORDSECONDSREGISTER_REG register
 *  Contains the most significant 16bits of the timestamp seconds value This register
 *  is optional and can be selected using the parameter mentioned in “IEEE 1588
 *  Timestamp Block” on page 492
 */
#define GMAC_REGISTER457_SYSTEMTIMEHIGHERWORDSECONDSREGISTER_REG (DR_REG_GMAC_BASE + 0x724)
/** GMAC_TSHWR : R/W; bitpos: [15:0]; default: 0;
 *  Timestamp Higher Word Register This field contains the most significant 16bits of
 *  the timestamp seconds value This register is optional and can be selected using the
 *  Enable IEEE 1588 Higher Word Register option during core configuration The register
 *  is directly written to initialize the value This register is incremented when there
 *  is an overflow from the 32bits of the System Time  Seconds register
 */
#define GMAC_TSHWR    0x0000FFFFU
#define GMAC_TSHWR_M  (GMAC_TSHWR_V << GMAC_TSHWR_S)
#define GMAC_TSHWR_V  0x0000FFFFU
#define GMAC_TSHWR_S  0

/** GMAC_REGISTER458_TIMESTAMPSTATUSREGISTER_REG register
 *  Contains the PTP status This register is available only when the advanced IEEE 1588
 *  timestamp feature is selected
 */
#define GMAC_REGISTER458_TIMESTAMPSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x728)
/** GMAC_TSSOVF : R/W; bitpos: [0]; default: 0;
 *  Timestamp Seconds Overflow When set, this bit indicates that the seconds value of
 *  the timestamp _when supporting version 2 format_ has overflowed beyond 32’hFFFF_FFFF
 */
#define GMAC_TSSOVF    (BIT(0))
#define GMAC_TSSOVF_M  (GMAC_TSSOVF_V << GMAC_TSSOVF_S)
#define GMAC_TSSOVF_V  0x00000001U
#define GMAC_TSSOVF_S  0
/** GMAC_TSTARGT : R/W; bitpos: [1]; default: 0;
 *  Timestamp Target Time Reached When set, this bit indicates that the value of system
 *  time is greater than or equal to the value specified in the Register 455 _Target
 *  Time Seconds Register_ and Register 456 _Target Time Nanoseconds Register_
 */
#define GMAC_TSTARGT    (BIT(1))
#define GMAC_TSTARGT_M  (GMAC_TSTARGT_V << GMAC_TSTARGT_S)
#define GMAC_TSTARGT_V  0x00000001U
#define GMAC_TSTARGT_S  1
/** GMAC_AUXTSTRIG : R/W; bitpos: [2]; default: 0;
 *  Auxiliary Timestamp Trigger Snapshot This bit is set high when the auxiliary
 *  snapshot is written to the FIFO This bit is valid only if the Enable IEEE 1588
 *  Auxiliary Snapshot feature is selected
 */
#define GMAC_AUXTSTRIG    (BIT(2))
#define GMAC_AUXTSTRIG_M  (GMAC_AUXTSTRIG_V << GMAC_AUXTSTRIG_S)
#define GMAC_AUXTSTRIG_V  0x00000001U
#define GMAC_AUXTSTRIG_S  2
/** GMAC_TSTRGTERR : R/W; bitpos: [3]; default: 0;
 *  Timestamp Target Time Error This bit is set when the target time, being programmed
 *  in Register 455 and Register 456, is already elapsed This bit is cleared when read
 *  by the application
 */
#define GMAC_TSTRGTERR    (BIT(3))
#define GMAC_TSTRGTERR_M  (GMAC_TSTRGTERR_V << GMAC_TSTRGTERR_S)
#define GMAC_TSTRGTERR_V  0x00000001U
#define GMAC_TSTRGTERR_S  3
/** GMAC_TSTARGT1 : R/W; bitpos: [4]; default: 0;
 *  Timestamp Target Time Reached for Target Time PPS1 When set, this bit indicates
 *  that the value of system time is greater than or equal to the value specified in
 *  Register 480 _PPS1 Target Time High Register_ and Register 481 _PPS1 Target Time
 *  Low Register_
 */
#define GMAC_TSTARGT1    (BIT(4))
#define GMAC_TSTARGT1_M  (GMAC_TSTARGT1_V << GMAC_TSTARGT1_S)
#define GMAC_TSTARGT1_V  0x00000001U
#define GMAC_TSTARGT1_S  4
/** GMAC_TSTRGTERR1 : R/W; bitpos: [5]; default: 0;
 *  Timestamp Target Time Error This bit is set when the target time, being programmed
 *  in Register 480 and Register 481, is already elapsed This bit is cleared when read
 *  by the application
 */
#define GMAC_TSTRGTERR1    (BIT(5))
#define GMAC_TSTRGTERR1_M  (GMAC_TSTRGTERR1_V << GMAC_TSTRGTERR1_S)
#define GMAC_TSTRGTERR1_V  0x00000001U
#define GMAC_TSTRGTERR1_S  5
/** GMAC_TSTARGT2 : R/W; bitpos: [6]; default: 0;
 *  Timestamp Target Time Reached for Target Time PPS2 When set, this bit indicates
 *  that the value of system time is greater than or equal to the value specified in
 *  Register 488 _PPS2 Target Time High Register_ and Register 489 _PPS2 Target Time
 *  Low Register_
 */
#define GMAC_TSTARGT2    (BIT(6))
#define GMAC_TSTARGT2_M  (GMAC_TSTARGT2_V << GMAC_TSTARGT2_S)
#define GMAC_TSTARGT2_V  0x00000001U
#define GMAC_TSTARGT2_S  6
/** GMAC_TSTRGTERR2 : R/W; bitpos: [7]; default: 0;
 *  Timestamp Target Time Error This bit is set when the target time, being programmed
 *  in Register 488 and Register 489, is already elapsed This bit is cleared when read
 *  by the application
 */
#define GMAC_TSTRGTERR2    (BIT(7))
#define GMAC_TSTRGTERR2_M  (GMAC_TSTRGTERR2_V << GMAC_TSTRGTERR2_S)
#define GMAC_TSTRGTERR2_V  0x00000001U
#define GMAC_TSTRGTERR2_S  7
/** GMAC_TSTARGT3 : R/W; bitpos: [8]; default: 0;
 *  Timestamp Target Time Reached for Target Time PPS3 When set, this bit indicates
 *  that the value of system time is greater than or equal to the value specified in
 *  Register 496 _PPS3 Target Time High Register_ and Register 497 _PPS3 Target Time
 *  Low Register_
 */
#define GMAC_TSTARGT3    (BIT(8))
#define GMAC_TSTARGT3_M  (GMAC_TSTARGT3_V << GMAC_TSTARGT3_S)
#define GMAC_TSTARGT3_V  0x00000001U
#define GMAC_TSTARGT3_S  8
/** GMAC_TSTRGTERR3 : R/W; bitpos: [9]; default: 0;
 *  Timestamp Target Time Error This bit is set when the target time, being programmed
 *  in Register 496 and Register 497, is already elapsed This bit is cleared when read
 *  by the application
 */
#define GMAC_TSTRGTERR3    (BIT(9))
#define GMAC_TSTRGTERR3_M  (GMAC_TSTRGTERR3_V << GMAC_TSTRGTERR3_S)
#define GMAC_TSTRGTERR3_V  0x00000001U
#define GMAC_TSTRGTERR3_S  9
/** GMAC_ATSSTN : R/W; bitpos: [19:16]; default: 0;
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
#define GMAC_ATSSTN    0x0000000FU
#define GMAC_ATSSTN_M  (GMAC_ATSSTN_V << GMAC_ATSSTN_S)
#define GMAC_ATSSTN_V  0x0000000FU
#define GMAC_ATSSTN_S  16
/** GMAC_ATSSTM : RO; bitpos: [24]; default: 0;
 *  Auxiliary Timestamp Snapshot Trigger Missed
 */
#define GMAC_ATSSTM    (BIT(24))
#define GMAC_ATSSTM_M  (GMAC_ATSSTM_V << GMAC_ATSSTM_S)
#define GMAC_ATSSTM_V  0x00000001U
#define GMAC_ATSSTM_S  24
/** GMAC_ATSNS : RO; bitpos: [29:25]; default: 0;
 *  Number of Auxiliary Timestamp Snapshots This field indicates the number of
 *  Snapshots available in the FIFO A value equal to the selected depth of FIFO _4, 8,
 *  or 16_ indicates that the Auxiliary Snapshot FIFO is full These bits are cleared
 *  _to 00000_ when the Auxiliary snapshot FIFO clear bit is set This bit is valid only
 *  if the Add IEEE 1588 Auxiliary Snapshot option is selected during core configuration
 */
#define GMAC_ATSNS    0x0000001FU
#define GMAC_ATSNS_M  (GMAC_ATSNS_V << GMAC_ATSNS_S)
#define GMAC_ATSNS_V  0x0000001FU
#define GMAC_ATSNS_S  25

/** GMAC_REGISTER459_PPSCONTROLREGISTER_REG register
 *  This register is used to control the interval of the PPS signal output This
 *  register is available only when the advanced IEEE 1588 timestamp feature is selected
 */
#define GMAC_REGISTER459_PPSCONTROLREGISTER_REG (DR_REG_GMAC_BASE + 0x72c)
/** GMAC_PPSCTRL0 : R/W1S; bitpos: [3:0]; default: 0;
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
#define GMAC_PPSCTRL0    0x0000000FU
#define GMAC_PPSCTRL0_M  (GMAC_PPSCTRL0_V << GMAC_PPSCTRL0_S)
#define GMAC_PPSCTRL0_V  0x0000000FU
#define GMAC_PPSCTRL0_S  0
/** GMAC_PPSEN0 : R/W; bitpos: [4]; default: 0;
 *  Flexible PPS Output Mode Enable When set low, Bits [3:0] function as PPSCTRL
 *  _backward compatible_ When set high, Bits[3:0] function as PPSCMD
 */
#define GMAC_PPSEN0    (BIT(4))
#define GMAC_PPSEN0_M  (GMAC_PPSEN0_V << GMAC_PPSEN0_S)
#define GMAC_PPSEN0_V  0x00000001U
#define GMAC_PPSEN0_S  4
/** GMAC_TRGTMODSEL0 : R/W; bitpos: [6:5]; default: 0;
 *  Target Time Register Mode for PPS0 Output This field indicates the Target Time
 *  registers _register 455 and 456_ mode for PPS0 output signal: 00: Indicates that
 *  the Target Time registers are programmed only for generating the interrupt event
 *  01: Reserved 10: Indicates that the Target Time registers are programmed for
 *  generating the interrupt event and starting or stopping the generation of the PPS0
 *  output signal 11: Indicates that the Target Time registers are programmed only for
 *  starting or stopping the generation of the PPS0 output signal No interrupt is
 *  asserted
 */
#define GMAC_TRGTMODSEL0    0x00000003U
#define GMAC_TRGTMODSEL0_M  (GMAC_TRGTMODSEL0_V << GMAC_TRGTMODSEL0_S)
#define GMAC_TRGTMODSEL0_V  0x00000003U
#define GMAC_TRGTMODSEL0_S  5
/** GMAC_PPSCMD1 : R/W1S; bitpos: [10:8]; default: 0;
 *  Flexible PPS1 Output Control This field controls the flexible PPS1 output
 *  _ptp_pps_o[1]_ signal This field is similar to PPSCMD0[2:0] in functionality
 */
#define GMAC_PPSCMD1    0x00000007U
#define GMAC_PPSCMD1_M  (GMAC_PPSCMD1_V << GMAC_PPSCMD1_S)
#define GMAC_PPSCMD1_V  0x00000007U
#define GMAC_PPSCMD1_S  8
/** GMAC_TRGTMODSEL1 : R/W; bitpos: [14:13]; default: 0;
 *  Target Time Register Mode for PPS1 Output This field indicates the Target Time
 *  registers _register 480 and 481_ mode for PPS1 output signal This field is similar
 *  to the TRGTMODSEL0 field
 */
#define GMAC_TRGTMODSEL1    0x00000003U
#define GMAC_TRGTMODSEL1_M  (GMAC_TRGTMODSEL1_V << GMAC_TRGTMODSEL1_S)
#define GMAC_TRGTMODSEL1_V  0x00000003U
#define GMAC_TRGTMODSEL1_S  13
/** GMAC_PPSCMD2 : R/W1S; bitpos: [18:16]; default: 0;
 *  Flexible PPS2 Output Control This field controls the flexible PPS2 output
 *  _ptp_pps_o[2]_ signal This field is similar to PPSCMD0[2:0] in functionality
 */
#define GMAC_PPSCMD2    0x00000007U
#define GMAC_PPSCMD2_M  (GMAC_PPSCMD2_V << GMAC_PPSCMD2_S)
#define GMAC_PPSCMD2_V  0x00000007U
#define GMAC_PPSCMD2_S  16
/** GMAC_TRGTMODSEL2 : R/W; bitpos: [22:21]; default: 0;
 *  Target Time Register Mode for PPS2 Output This field indicates the Target Time
 *  registers _register 488 and 489_ mode for PPS2 output signal This field is similar
 *  to the TRGTMODSEL0 field
 */
#define GMAC_TRGTMODSEL2    0x00000003U
#define GMAC_TRGTMODSEL2_M  (GMAC_TRGTMODSEL2_V << GMAC_TRGTMODSEL2_S)
#define GMAC_TRGTMODSEL2_V  0x00000003U
#define GMAC_TRGTMODSEL2_S  21
/** GMAC_PPSCMD3 : R/W1S; bitpos: [26:24]; default: 0;
 *  Flexible PPS3 Output Control This field controls the flexible PPS3 output
 *  _ptp_pps_o[3]_ signal This field is similar to PPSCMD0[2:0] in functionality
 */
#define GMAC_PPSCMD3    0x00000007U
#define GMAC_PPSCMD3_M  (GMAC_PPSCMD3_V << GMAC_PPSCMD3_S)
#define GMAC_PPSCMD3_V  0x00000007U
#define GMAC_PPSCMD3_S  24
/** GMAC_TRGTMODSEL3 : R/W; bitpos: [30:29]; default: 0;
 *  Target Time Register Mode for PPS3 Output This field indicates the Target Time
 *  registers _register 496 and 497_ mode for PPS3 output signal This field is similar
 *  to the TRGTMODSEL0 field
 */
#define GMAC_TRGTMODSEL3    0x00000003U
#define GMAC_TRGTMODSEL3_M  (GMAC_TRGTMODSEL3_V << GMAC_TRGTMODSEL3_S)
#define GMAC_TRGTMODSEL3_V  0x00000003U
#define GMAC_TRGTMODSEL3_S  29

/** GMAC_REGISTER460_AUXILIARYTIMESTAMPNANOSECONDSREGISTER_REG register
 *  Contains the lower 32 bits _nanoseconds field_ of the auxiliary timestamp register
 */
#define GMAC_REGISTER460_AUXILIARYTIMESTAMPNANOSECONDSREGISTER_REG (DR_REG_GMAC_BASE + 0x730)
/** GMAC_AUXTSLO : RO; bitpos: [30:0]; default: 0;
 *  Contains the lower 31 bits _nanoseconds field_ of the auxiliary timestamp
 */
#define GMAC_AUXTSLO    0x7FFFFFFFU
#define GMAC_AUXTSLO_M  (GMAC_AUXTSLO_V << GMAC_AUXTSLO_S)
#define GMAC_AUXTSLO_V  0x7FFFFFFFU
#define GMAC_AUXTSLO_S  0

/** GMAC_REGISTER461_AUXILIARYTIMESTAMPSECONDSREGISTER_REG register
 *  Contains the lower 32 bits of the Seconds field of the auxiliary timestamp register
 */
#define GMAC_REGISTER461_AUXILIARYTIMESTAMPSECONDSREGISTER_REG (DR_REG_GMAC_BASE + 0x734)
/** GMAC_AUXTSHI : RO; bitpos: [31:0]; default: 0;
 *  Contains the lower 32 bits of the Seconds field of the auxiliary timestamp
 */
#define GMAC_AUXTSHI    0xFFFFFFFFU
#define GMAC_AUXTSHI_M  (GMAC_AUXTSHI_V << GMAC_AUXTSHI_S)
#define GMAC_AUXTSHI_V  0xFFFFFFFFU
#define GMAC_AUXTSHI_S  0

/** GMAC_REGISTER462_AVMACCONTROLREGISTER_REG register
 *  Controls the AV traffic and queue management in the MAC Receiver This register is
 *  present only when you select the AV feature in coreConsultant
 */
#define GMAC_REGISTER462_AVMACCONTROLREGISTER_REG (DR_REG_GMAC_BASE + 0x738)
/** GMAC_AVT : R/W; bitpos: [15:0]; default: 0;
 *  AV EtherType Value This field contains the value that is compared with the
 *  EtherType field of the incoming _tagged or untagged_ Ethernet frame to detect an AV
 *  packet
 */
#define GMAC_AVT    0x0000FFFFU
#define GMAC_AVT_M  (GMAC_AVT_V << GMAC_AVT_S)
#define GMAC_AVT_V  0x0000FFFFU
#define GMAC_AVT_S  0
/** GMAC_AVP : R/W; bitpos: [18:16]; default: 0;
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
#define GMAC_AVP    0x00000007U
#define GMAC_AVP_M  (GMAC_AVP_V << GMAC_AVP_S)
#define GMAC_AVP_V  0x00000007U
#define GMAC_AVP_S  16
/** GMAC_VQE : R/W; bitpos: [19]; default: 0;
 *  VLAN Tagged NonAV Packets Queueing Enable When this bit is set, the MAC also queues
 *  nonAV VLAN tagged packets into the available channels according to the value of the
 *  AVP bits This bit is reserved and readonly if Channel 1 and Channel 2 Receive paths
 *  are not selected during core configuration
 */
#define GMAC_VQE    (BIT(19))
#define GMAC_VQE_M  (GMAC_VQE_V << GMAC_VQE_S)
#define GMAC_VQE_V  0x00000001U
#define GMAC_VQE_S  19
/** GMAC_AVCD : R/W; bitpos: [20]; default: 0;
 *  AV Channel Disable When this bit is set, the MAC forwards all packets to the
 *  default Channel 0 and the values programmed in the AVP, AVCH, and PTPCH fields are
 *  ignored This bit is reserved and readonly if Channel 1 or Channel 2 receive paths
 *  are not selected during core configuration
 */
#define GMAC_AVCD    (BIT(20))
#define GMAC_AVCD_M  (GMAC_AVCD_V << GMAC_AVCD_S)
#define GMAC_AVCD_V  0x00000001U
#define GMAC_AVCD_S  20
/** GMAC_AVCH : R/W; bitpos: [22:21]; default: 0;
 *  Channel for Queuing the AV Control Packets This field specifies the channel on
 *  which the received untagged AV control packets are queued 00: Channel 0 01: Channel
 *  1 10: Channel 2 11: Reserved These bits are reserved if the receive paths of
 *  Channel 1 or Channel 2 are not enabled
 */
#define GMAC_AVCH    0x00000003U
#define GMAC_AVCH_M  (GMAC_AVCH_V << GMAC_AVCH_S)
#define GMAC_AVCH_V  0x00000003U
#define GMAC_AVCH_S  21
/** GMAC_PTPCH : R/W; bitpos: [25:24]; default: 0;
 *  Channel for Queuing the PTP Packets This field specifies the channel on which the
 *  untagged PTP packets, sent over the Ethernet payload and not over IPv4 or IPv6, are
 *  queued 00: Channel 0 01: Channel 1 10: Channel 2 11: Reserved These bits are
 *  reserved if the receive paths of Channel 1 or Channel 2 are not enabled
 */
#define GMAC_PTPCH    0x00000003U
#define GMAC_PTPCH_M  (GMAC_PTPCH_V << GMAC_PTPCH_S)
#define GMAC_PTPCH_V  0x00000003U
#define GMAC_PTPCH_S  24

/** GMAC_REGISTER472_PPS0INTERVALREGISTER_REG register
 *  Contains the number of units of subsecond increment value between the rising edges
 *  of PPS0 signal output This register is available only when the flexible PPS feature
 *  is selected
 */
#define GMAC_REGISTER472_PPS0INTERVALREGISTER_REG (DR_REG_GMAC_BASE + 0x760)
/** GMAC_PPSINT : R/W; bitpos: [31:0]; default: 0;
 *  PPS0 Output Signal Interval These bits store the interval between the rising edges
 *  of PPS0 signal output in terms of units of subsecond increment value You need to
 *  program one value less than the required interval For example, if the PTP reference
 *  clock is 50 MHz _period of 20ns_, and desired interval between rising edges of PPS0
 *  signal output is 100ns _that is, five units of subsecond increment value_, then you
 *  should program value 4 _5  1_ in this register
 */
#define GMAC_PPSINT    0xFFFFFFFFU
#define GMAC_PPSINT_M  (GMAC_PPSINT_V << GMAC_PPSINT_S)
#define GMAC_PPSINT_V  0xFFFFFFFFU
#define GMAC_PPSINT_S  0

/** GMAC_REGISTER473_PPS0WIDTHREGISTER_REG register
 *  Contains the number of units of subsecond increment value between the rising and
 *  corresponding falling edges of PPS0 signal output This register is available only
 *  when the flexible PPS feature is selected
 */
#define GMAC_REGISTER473_PPS0WIDTHREGISTER_REG (DR_REG_GMAC_BASE + 0x764)
/** GMAC_PPSWIDTH : R/W; bitpos: [31:0]; default: 0;
 *  PPS0 Output Signal Width These bits store the width between the rising edge and
 *  corresponding falling edge of the PPS0 signal output in terms of units of subsecond
 *  increment value You need to program one value less than the required interval For
 *  example, if PTP reference clock is 50 MHz _period of 20ns_, and desired width
 *  between the rising and corresponding falling edges of PPS0 signal output is 80ns
 *  _that is, four units of subsecond increment value_, then you should program value 3
 *  _4  1_ in this register Note: The value programmed in this register must be lesser
 *  than the value programmed in Register 472 _PPS0 Interval Register_
 */
#define GMAC_PPSWIDTH    0xFFFFFFFFU
#define GMAC_PPSWIDTH_M  (GMAC_PPSWIDTH_V << GMAC_PPSWIDTH_S)
#define GMAC_PPSWIDTH_V  0xFFFFFFFFU
#define GMAC_PPSWIDTH_S  0

/** GMAC_REGISTER480_PPS1TARGETTIMESECONDSREGISTER_REG register
 *  Contains the higher 32 bits of time to be compared with the system time to generate
 *  the interrupt event or to start generating the PPS1 output signal This register is
 *  present only when IEEE1588 timestamping is enabled without an external timestamp
 *  input and at least one additional PPS output is selected
 */
#define GMAC_REGISTER480_PPS1TARGETTIMESECONDSREGISTER_REG (DR_REG_GMAC_BASE + 0x780)
/** GMAC_TSTRH1 : R/W; bitpos: [31:0]; default: 0;
 *  PPS1 Target Time Seconds Register This register stores the time in seconds When the
 *  timestamp value matches or exceeds both Target Timestamp registers, then based on
 *  Bits [14:13], TRGTMODSEL1, of Register 459 _PPS Control Register_, the MAC starts
 *  or stops the PPS signal output and generates an interrupt _if enabled_
 */
#define GMAC_TSTRH1    0xFFFFFFFFU
#define GMAC_TSTRH1_M  (GMAC_TSTRH1_V << GMAC_TSTRH1_S)
#define GMAC_TSTRH1_V  0xFFFFFFFFU
#define GMAC_TSTRH1_S  0

/** GMAC_REGISTER481_PPS1TARGETTIMENANOSECONDSREGISTER_REG register
 *  Contains the lower 32 bits of time to be compared with the system time to generate
 *  the interrupt event or to start generating the PPS1 output signal This register is
 *  present only when IEEE1588 timestamping is enabled without an external timestamp
 *  input and at least one additional PPS output is selected
 */
#define GMAC_REGISTER481_PPS1TARGETTIMENANOSECONDSREGISTER_REG (DR_REG_GMAC_BASE + 0x784)
/** GMAC_TTSL1 : R/W; bitpos: [30:0]; default: 0;
 *  Target Time Low for PPS1 Register This register stores the time in _signed_
 *  nanoseconds When the value of the timestamp matches the both Target Timestamp
 *  registers, then based on the TRGTMODSEL1 field _Bits [14:13]_ in Register 459 _PPS
 *  Control Register_, the MAC starts or stops the PPS signal output and generates an
 *  interrupt _if enabled_ This value should not exceed 0x3B9A_C9FF when Bit 9
 *  _TSCTRLSSR_ is set in Register 448 _Timestamp Control Register_ The actual start or
 *  stop time of the PPS signal output may have an error margin up to one unit of
 *  subsecond increment value
 */
#define GMAC_TTSL1    0x7FFFFFFFU
#define GMAC_TTSL1_M  (GMAC_TTSL1_V << GMAC_TTSL1_S)
#define GMAC_TTSL1_V  0x7FFFFFFFU
#define GMAC_TTSL1_S  0
/** GMAC_TRGTBUSY1 : R/W1S; bitpos: [31]; default: 0;
 *  PPS1 Target Time Register Busy The MAC sets this bit when the PPSCMD1 field _Bits
 *  [10:8]_ in Register 459 _PPS Control Register_ is programmed to 010 or 011
 *  Programming the PPSCMD1 field to 010 or 011 instructs the MAC to synchronize the
 *  Target Time Registers to the PTP clock domain The MAC clears this bit after
 *  synchronizing the Target Time Registers to the PTP clock domain The application
 *  must not update the Target Time Registers when this bit is read as 1 Otherwise, the
 *  synchronization of the previous programmed time gets corrupted
 */
#define GMAC_TRGTBUSY1    (BIT(31))
#define GMAC_TRGTBUSY1_M  (GMAC_TRGTBUSY1_V << GMAC_TRGTBUSY1_S)
#define GMAC_TRGTBUSY1_V  0x00000001U
#define GMAC_TRGTBUSY1_S  31

/** GMAC_REGISTER544_MACADDRESS32HIGHREGISTER_REG register
 *  Contains the higher 16 bits of the 33rd MAC address This register is present only
 *  when Enable Additional 32 MAC Address Registers is selected in coreConsultant _See
 *  Table 78_
 */
#define GMAC_REGISTER544_MACADDRESS32HIGHREGISTER_REG (DR_REG_GMAC_BASE + 0x880)
/** GMAC_ADDRHI : R/W; bitpos: [15:0]; default: 65535;
 *  MAC Address32 [47:32] This field contains the upper 16 bits _47:32_ of the 33rd
 *  6byte MAC address
 */
#define GMAC_ADDRHI    0x0000FFFFU
#define GMAC_ADDRHI_M  (GMAC_ADDRHI_V << GMAC_ADDRHI_S)
#define GMAC_ADDRHI_V  0x0000FFFFU
#define GMAC_ADDRHI_S  0
/** GMAC_AE : R/W; bitpos: [31]; default: 0;
 *  Address Enable When this bit is set, the Address filter module uses the 33rd MAC
 *  address for perfect filtering When reset, the address filter module ignores the
 *  address for filtering
 */
#define GMAC_AE    (BIT(31))
#define GMAC_AE_M  (GMAC_AE_V << GMAC_AE_S)
#define GMAC_AE_V  0x00000001U
#define GMAC_AE_S  31

/** GMAC_REGISTER0_BUSMODEREGISTER_REG register
 *  Controls the Host Interface Mode
 */
#define GMAC_REGISTER0_BUSMODEREGISTER_REG (DR_REG_GMAC_BASE + 0x1000)
/** GMAC_SWR : R/W1S_SC; bitpos: [0]; default: 1;
 *  Software Reset When this bit is set, the MAC DMA Controller resets the logic and
 *  all internal registers of the MAC It is cleared automatically after the reset
 *  operation is complete in all of the DWC_gmac clock domains Before reprogramming any
 *  register of the DWC_gmac, you should read a zero _0_ value in this bit Note: The
 *  Software reset function is driven only by this bit Bit 0 of Register 64 _Channel 1
 *  Bus Mode Register_ or Register 128 _Channel 2 Bus Mode Register_ has no impact on
 *  the Software reset function The reset operation is completed only when all resets
 *  in all active clock domains are deasserted Therefore, it is essential that all PHY
 *  inputs clocks _applicable for the selected PHY interface_ are present for the
 *  software reset completion The time to complete the software reset operation depends
 *  on the frequency of the slowest active clock
 */
#define GMAC_SWR    (BIT(0))
#define GMAC_SWR_M  (GMAC_SWR_V << GMAC_SWR_S)
#define GMAC_SWR_V  0x00000001U
#define GMAC_SWR_S  0
/** GMAC_DA : R/W; bitpos: [1]; default: 0;
 *  DMA Arbitration Scheme This bit specifies the arbitration scheme between the
 *  transmit and receive paths of Channel 0 0: Weighted roundrobin with Rx:Tx or Tx:Rx
 *  The priority between the paths is according to the priority specified in Bits
 *  [15:14] _PR_ and priority weights specified in Bit 27 _TXPR_ 1: Fixed priority The
 *  transmit path has priority over receive path when Bit 27 _TXPR_ is set Otherwise,
 *  receive path has priority over the transmit path In the GMACAXI configuration,
 *  these bits are reserved and are readonly _RO_ For more information about the
 *  priority scheme between the transmit and receive paths, see Table 412 in “DMA
 *  Arbiter Functions” on page 167
 */
#define GMAC_DA    (BIT(1))
#define GMAC_DA_M  (GMAC_DA_V << GMAC_DA_S)
#define GMAC_DA_V  0x00000001U
#define GMAC_DA_S  1
/** GMAC_DSL : R/W; bitpos: [6:2]; default: 0;
 *  Descriptor Skip Length This bit specifies the number of Word, Dword, or Lword
 *  _depending on the 32bit, 64bit, or 128bit bus_ to skip between two unchained
 *  descriptors The address skipping starts from the end of current descriptor to the
 *  start of next descriptor When the DSL value is equal to zero, the descriptor table
 *  is taken as contiguous by the DMA in Ring mode
 */
#define GMAC_DSL    0x0000001FU
#define GMAC_DSL_M  (GMAC_DSL_V << GMAC_DSL_S)
#define GMAC_DSL_V  0x0000001FU
#define GMAC_DSL_S  2
/** GMAC_ATDS : R/W; bitpos: [7]; default: 0;
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
#define GMAC_ATDS    (BIT(7))
#define GMAC_ATDS_M  (GMAC_ATDS_V << GMAC_ATDS_S)
#define GMAC_ATDS_V  0x00000001U
#define GMAC_ATDS_S  7
/** GMAC_PBL : R/W; bitpos: [13:8]; default: 1;
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
#define GMAC_PBL    0x0000003FU
#define GMAC_PBL_M  (GMAC_PBL_V << GMAC_PBL_S)
#define GMAC_PBL_V  0x0000003FU
#define GMAC_PBL_S  8
/** GMAC_PR : R/W; bitpos: [15:14]; default: 0;
 *  Priority Ratio These bits control the priority ratio in the weighted roundrobin
 *  arbitration between the Rx DMA and Tx DMA These bits are valid only when Bit 1 _DA_
 *  is reset The priority ratio is Rx:Tx or Tx:Rx depending on whether Bit 27 _TXPR_ is
 *  reset or set 00: The Priority Ratio is 1:1 01: The Priority Ratio is 2:1 10: The
 *  Priority Ratio is 3:1 11: The Priority Ratio is 4:1 In the GMACAXI configuration,
 *  these bits are reserved and readonly _RO_ For more information about the priority
 *  scheme between the transmit and receive paths, see Table 412 in “DMA Arbiter
 *  Functions” on page 167
 */
#define GMAC_PR    0x00000003U
#define GMAC_PR_M  (GMAC_PR_V << GMAC_PR_S)
#define GMAC_PR_V  0x00000003U
#define GMAC_PR_S  14
/** GMAC_FB : R/W; bitpos: [16]; default: 0;
 *  Fixed Burst This bit controls whether the AHB or AXI master interface performs
 *  fixed burst transfers or not When set, the AHB interface uses only SINGLE, INCR4,
 *  INCR8, or INCR16 during start of the normal burst transfers When reset, the AHB or
 *  AXI interface uses SINGLE and INCR burst transfer operations For more information,
 *  see Bit 0 _UNDEF_ of the AXI Bus Mode register in the GMACAXI configuration
 */
#define GMAC_FB    (BIT(16))
#define GMAC_FB_M  (GMAC_FB_V << GMAC_FB_S)
#define GMAC_FB_V  0x00000001U
#define GMAC_FB_S  16
/** GMAC_RPBL : R/W; bitpos: [22:17]; default: 1;
 *  Rx DMA PBL This field indicates the maximum number of beats to be transferred in
 *  one Rx DMA transaction This is the maximum value that is used in a single block
 *  Read or Write The Rx DMA always attempts to burst as specified in the RPBL bit each
 *  time it starts a Burst transfer on the host bus You can program RPBL with values of
 *  1, 2, 4, 8, 16, and 32 Any other value results in undefined behavior This field is
 *  valid and applicable only when USP is set high
 */
#define GMAC_RPBL    0x0000003FU
#define GMAC_RPBL_M  (GMAC_RPBL_V << GMAC_RPBL_S)
#define GMAC_RPBL_V  0x0000003FU
#define GMAC_RPBL_S  17
/** GMAC_USP : R/W; bitpos: [23]; default: 0;
 *  Use Separate PBL When set high, this bit configures the Rx DMA to use the value
 *  configured in Bits [22:17] as PBL The PBL value in Bits [13:8] is applicable only
 *  to the Tx DMA operations When reset to low, the PBL value in Bits [13:8] is
 *  applicable for both DMA engines
 */
#define GMAC_USP    (BIT(23))
#define GMAC_USP_M  (GMAC_USP_V << GMAC_USP_S)
#define GMAC_USP_V  0x00000001U
#define GMAC_USP_S  23
/** GMAC_PBLX8 : R/W; bitpos: [24]; default: 0;
 *  PBLx8 Mode When set high, this bit multiplies the programmed PBL value _Bits
 *  [22:17] and Bits[13:8]_ eight times Therefore, the DMA transfers the data in 8, 16,
 *  32, 64, 128, and 256 beats depending on the PBL value Note: This bit function is
 *  not backward compatible Before release 350a, this bit was 4xPBL
 */
#define GMAC_PBLX8    (BIT(24))
#define GMAC_PBLX8_M  (GMAC_PBLX8_V << GMAC_PBLX8_S)
#define GMAC_PBLX8_V  0x00000001U
#define GMAC_PBLX8_S  24
/** GMAC_AAL : R/W; bitpos: [25]; default: 0;
 *  AddressAligned Beats When this bit is set high and the FB bit is equal to 1, the
 *  AHB or AXI interface generates all bursts aligned to the start address LS bits If
 *  the FB bit is equal to 0, the first burst _accessing the start address of data
 *  buffer_ is not aligned, but subsequent bursts are aligned to the address This bit
 *  is valid only in the GMACAHB and GMACAXI configurations and is reserved _RO with
 *  default value 0_ in all other configurations
 */
#define GMAC_AAL    (BIT(25))
#define GMAC_AAL_M  (GMAC_AAL_V << GMAC_AAL_S)
#define GMAC_AAL_V  0x00000001U
#define GMAC_AAL_S  25
/** GMAC_MB : R/W; bitpos: [26]; default: 0;
 *  Mixed Burst When this bit is set high and the FB bit is low, the AHB master
 *  interface starts all bursts of length more than 16 with INCR _undefined burst_,
 *  whereas it reverts to fixed burst transfers _INCRx and SINGLE_ for burst length of
 *  16 and less This bit is valid only in the GMACAHB configuration and reserved in all
 *  other configuration
 */
#define GMAC_MB    (BIT(26))
#define GMAC_MB_M  (GMAC_MB_V << GMAC_MB_S)
#define GMAC_MB_V  0x00000001U
#define GMAC_MB_S  26
/** GMAC_TXPR : R/W; bitpos: [27]; default: 0;
 *  Transmit Priority When set, this bit indicates that the transmit DMA has higher
 *  priority than the receive DMA during arbitration for the systemside bus In the
 *  GMACAXI configuration, this bit is reserved and readonly _RO_ For more information
 *  about the priority scheme between the transmit and receive paths, see Table 412 in
 *  “DMA Arbiter Functions” on page 167
 */
#define GMAC_TXPR    (BIT(27))
#define GMAC_TXPR_M  (GMAC_TXPR_V << GMAC_TXPR_S)
#define GMAC_TXPR_V  0x00000001U
#define GMAC_TXPR_S  27
/** GMAC_PRWG : R/W; bitpos: [29:28]; default: 0;
 *  Channel Priority Weights This field sets the priority weights for Channel 0 during
 *  the roundrobin arbitration between the DMA channels for the system bus 00: The
 *  priority weight is 1 01: The priority weight is 2 10: The priority weight is 3 11:
 *  The priority weight is 4 This field is present in all DWC_gmac configurations
 *  except GMACAXI when you select the AV feature Otherwise, this field is reserved and
 *  readonly _RO_ For more information about the priority weights of DMA channels, see
 *  “DMA Arbiter Functions” on page 167
 */
#define GMAC_PRWG    0x00000003U
#define GMAC_PRWG_M  (GMAC_PRWG_V << GMAC_PRWG_S)
#define GMAC_PRWG_V  0x00000003U
#define GMAC_PRWG_S  28
/** GMAC_RIB : R/W; bitpos: [31]; default: 0;
 *  Rebuild INCRx Burst When this bit is set high and the AHB master gets an EBT
 *  _Retry, Split, or Losing bus grant_, the AHB master interface rebuilds the pending
 *  beats of any burst transfer initiated with INCRx The AHB master interface rebuilds
 *  the beats with a combination of specified bursts with INCRx and SINGLE By default,
 *  the AHB master interface rebuilds pending beats of an EBT with an unspecified
 *  _INCR_ burst This bit is valid only in the GMACAHB configuration It is reserved in
 *  all other configuration
 */
#define GMAC_RIB    (BIT(31))
#define GMAC_RIB_M  (GMAC_RIB_V << GMAC_RIB_S)
#define GMAC_RIB_V  0x00000001U
#define GMAC_RIB_S  31

/** GMAC_REGISTER1_TRANSMITPOLLDEMANDREGISTER_REG register
 *  Used by the host to instruct the DMA to poll the Transmit Descriptor list
 */
#define GMAC_REGISTER1_TRANSMITPOLLDEMANDREGISTER_REG (DR_REG_GMAC_BASE + 0x1004)
/** GMAC_TPD : RO; bitpos: [31:0]; default: 0;
 *  Transmit Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 18 _Current Host Transmit Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  transmission returns to the Suspend state and Bit 2 _TU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the transmission resumes
 */
#define GMAC_TPD    0xFFFFFFFFU
#define GMAC_TPD_M  (GMAC_TPD_V << GMAC_TPD_S)
#define GMAC_TPD_V  0xFFFFFFFFU
#define GMAC_TPD_S  0

/** GMAC_REGISTER2_RECEIVEPOLLDEMANDREGISTER_REG register
 *  Used by the host to instruct the DMA to poll the Receive Descriptor list
 */
#define GMAC_REGISTER2_RECEIVEPOLLDEMANDREGISTER_REG (DR_REG_GMAC_BASE + 0x1008)
/** GMAC_RPD : RO; bitpos: [31:0]; default: 0;
 *  Receive Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 19 _Current Host Receive Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  reception returns to the Suspended state and Bit 7 _RU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the Rx DMA returns to the
 *  active state
 */
#define GMAC_RPD    0xFFFFFFFFU
#define GMAC_RPD_M  (GMAC_RPD_V << GMAC_RPD_S)
#define GMAC_RPD_V  0xFFFFFFFFU
#define GMAC_RPD_S  0

/** GMAC_REGISTER3_RECEIVEDESCRIPTORLISTADDRESSREGISTER_REG register
 *  Points the DMA to the start of the Receive Descriptor list
 */
#define GMAC_REGISTER3_RECEIVEDESCRIPTORLISTADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x100c)
/** GMAC_RDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Receive List This field contains the base address of the first descriptor
 *  in the Receive Descriptor list The LSB bits _1:0, 2:0, or 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and internally taken as allzero by the DMA Therefore,
 *  these LSB bits are readonly _RO_
 */
#define GMAC_RDESLA    0xFFFFFFFFU
#define GMAC_RDESLA_M  (GMAC_RDESLA_V << GMAC_RDESLA_S)
#define GMAC_RDESLA_V  0xFFFFFFFFU
#define GMAC_RDESLA_S  0

/** GMAC_REGISTER4_TRANSMITDESCRIPTORLISTADDRESSREGISTER_REG register
 *  Points the DMA to the start of the Transmit Descriptor list
 */
#define GMAC_REGISTER4_TRANSMITDESCRIPTORLISTADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x1010)
/** GMAC_TDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Transmit List This field contains the base address of the first descriptor
 *  in the Transmit Descriptor list The LSB bits _1:0, 2:0, 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and are internally taken as allzero by the DMA
 *  Therefore, these LSB bits are readonly _RO_
 */
#define GMAC_TDESLA    0xFFFFFFFFU
#define GMAC_TDESLA_M  (GMAC_TDESLA_V << GMAC_TDESLA_S)
#define GMAC_TDESLA_V  0xFFFFFFFFU
#define GMAC_TDESLA_S  0

/** GMAC_REGISTER5_STATUSREGISTER_REG register
 *  The Software driver _application_ reads this register during interrupt service
 *  routine or polling to determine the status of the DMA
 */
#define GMAC_REGISTER5_STATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x1014)
/** GMAC_TI : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt This bit indicates that the frame transmission is complete When
 *  transmission is complete, Bit 31 _OWN_ of TDES0 is reset, and the specific frame
 *  status information is updated in the descriptor
 */
#define GMAC_TI    (BIT(0))
#define GMAC_TI_M  (GMAC_TI_V << GMAC_TI_S)
#define GMAC_TI_V  0x00000001U
#define GMAC_TI_S  0
/** GMAC_TPS : R/W; bitpos: [1]; default: 0;
 *  Transmit Process Stopped This bit is set when the transmission is stopped
 */
#define GMAC_TPS    (BIT(1))
#define GMAC_TPS_M  (GMAC_TPS_V << GMAC_TPS_S)
#define GMAC_TPS_V  0x00000001U
#define GMAC_TPS_S  1
/** GMAC_TU : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Transmit List and the DMA cannot acquire it Transmission is
 *  suspended Bits[22:20] explain the Transmit Process state transitions To resume
 *  processing Transmit descriptors, the host should change the ownership of the
 *  descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command
 */
#define GMAC_TU    (BIT(2))
#define GMAC_TU_M  (GMAC_TU_V << GMAC_TU_S)
#define GMAC_TU_V  0x00000001U
#define GMAC_TU_S  2
/** GMAC_TJT : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout This bit indicates that the Transmit Jabber Timer expired,
 *  which happens when the frame size exceeds 2,048 _10,240 bytes when the Jumbo frame
 *  is enabled_ When the Jabber Timeout occurs, the transmission process is aborted and
 *  placed in the Stopped state This causes the Transmit Jabber Timeout TDES0[14] flag
 *  to assert
 */
#define GMAC_TJT    (BIT(3))
#define GMAC_TJT_M  (GMAC_TJT_V << GMAC_TJT_S)
#define GMAC_TJT_V  0x00000001U
#define GMAC_TJT_S  3
/** GMAC_OVF : R/W; bitpos: [4]; default: 0;
 *  Receive Overflow This bit indicates that the Receive Buffer had an Overflow during
 *  frame reception If the partial frame is transferred to the application, the
 *  overflow status is set in RDES0[11]
 */
#define GMAC_OVF    (BIT(4))
#define GMAC_OVF_M  (GMAC_OVF_V << GMAC_OVF_S)
#define GMAC_OVF_V  0x00000001U
#define GMAC_OVF_S  4
/** GMAC_UNF : R/W; bitpos: [5]; default: 0;
 *  Transmit Underflow This bit indicates that the Transmit Buffer had an Underflow
 *  during frame transmission Transmission is suspended and an Underflow Error TDES0[1]
 *  is set
 */
#define GMAC_UNF    (BIT(5))
#define GMAC_UNF_M  (GMAC_UNF_V << GMAC_UNF_S)
#define GMAC_UNF_V  0x00000001U
#define GMAC_UNF_S  5
/** GMAC_RI : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt This bit indicates that the frame reception is complete When
 *  reception is complete, the Bit 31 of RDES1 _Disable Interrupt on Completion_ is
 *  reset in the last Descriptor, and the specific frame status information is updated
 *  in the descriptor The reception remains in the Running state
 */
#define GMAC_RI    (BIT(6))
#define GMAC_RI_M  (GMAC_RI_V << GMAC_RI_S)
#define GMAC_RI_V  0x00000001U
#define GMAC_RI_S  6
/** GMAC_RU : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Receive List and the DMA cannot acquire it The Receive Process is
 *  suspended To resume processing Receive descriptors, the host should change the
 *  ownership of the descriptor and issue a Receive Poll Demand command If no Receive
 *  Poll Demand is issued, the Receive Process resumes when the next recognized
 *  incoming frame is received This bit is set only when the previous Receive
 *  Descriptor is owned by the DMA
 */
#define GMAC_RU    (BIT(7))
#define GMAC_RU_M  (GMAC_RU_V << GMAC_RU_S)
#define GMAC_RU_V  0x00000001U
#define GMAC_RU_S  7
/** GMAC_RPS : R/W; bitpos: [8]; default: 0;
 *  Receive Process Stopped This bit is asserted when the Receive Process enters the
 *  Stopped state
 */
#define GMAC_RPS    (BIT(8))
#define GMAC_RPS_M  (GMAC_RPS_V << GMAC_RPS_S)
#define GMAC_RPS_V  0x00000001U
#define GMAC_RPS_S  8
/** GMAC_RWT : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout When set, this bit indicates that the Receive Watchdog
 *  Timer expired while receiving the current frame and the current frame is truncated
 *  after the watchdog timeout
 */
#define GMAC_RWT    (BIT(9))
#define GMAC_RWT_M  (GMAC_RWT_V << GMAC_RWT_S)
#define GMAC_RWT_V  0x00000001U
#define GMAC_RWT_S  9
/** GMAC_ETI : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt This bit indicates that the frame to be transmitted is
 *  fully transferred to the MTL Transmit FIFO
 */
#define GMAC_ETI    (BIT(10))
#define GMAC_ETI_M  (GMAC_ETI_V << GMAC_ETI_S)
#define GMAC_ETI_V  0x00000001U
#define GMAC_ETI_S  10
/** GMAC_FBI : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Interrupt This bit indicates that a bus error occurred, as
 *  described in Bits [25:23] When this bit is set, the corresponding DMA engine
 *  disables all of its bus accesses 12:11		Reserved	00	RO
 */
#define GMAC_FBI    (BIT(13))
#define GMAC_FBI_M  (GMAC_FBI_V << GMAC_FBI_S)
#define GMAC_FBI_V  0x00000001U
#define GMAC_FBI_S  13
/** GMAC_ERI : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt This bit indicates that the DMA filled the first data
 *  buffer of the packet This bit is cleared when the software writes 1 to this bit or
 *  Bit 6 _RI_ of this register is set _whichever occurs earlier_
 */
#define GMAC_ERI    (BIT(14))
#define GMAC_ERI_M  (GMAC_ERI_V << GMAC_ERI_S)
#define GMAC_ERI_V  0x00000001U
#define GMAC_ERI_S  14
/** GMAC_AIS : R/W; bitpos: [15]; default: 0;
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
#define GMAC_AIS    (BIT(15))
#define GMAC_AIS_M  (GMAC_AIS_V << GMAC_AIS_S)
#define GMAC_AIS_V  0x00000001U
#define GMAC_AIS_S  15
/** GMAC_NIS : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Normal Interrupt Summary bit value is the logical OR of
 *  the following bits when the corresponding interrupt bits are enabled in Register 7
 *  _Interrupt Enable Register_: Register 5[0]: Transmit Interrupt Register 5[2]:
 *  Transmit Buffer Unavailable Register 5[6]: Receive Interrupt Register 5[14]: Early
 *  Receive Interrupt Only unmasked bits _interrupts for which interrupt enable is set
 *  in Register 7_ affect the Normal Interrupt Summary bit This is a sticky bit and
 *  must be cleared _by writing 1 to this bit_ each time a corresponding bit, which
 *  causes NIS to be set, is cleared
 */
#define GMAC_NIS    (BIT(16))
#define GMAC_NIS_M  (GMAC_NIS_V << GMAC_NIS_S)
#define GMAC_NIS_V  0x00000001U
#define GMAC_NIS_S  16
/** GMAC_RS : RO; bitpos: [19:17]; default: 0;
 *  Receive Process State This field indicates the Receive DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Receive Command
 *  issued 3’b001: Running: Fetching Receive Transfer Descriptor 3’b010: Reserved for
 *  future use 3’b011: Running: Waiting for receive packet 3’b100: Suspended: Receive
 *  Descriptor Unavailable 3’b101: Running: Closing Receive Descriptor 3’b110:
 *  TIME_STAMP write state 3’b111: Running: Transferring the receive packet data from
 *  receive buffer to host memory
 */
#define GMAC_RS    0x00000007U
#define GMAC_RS_M  (GMAC_RS_V << GMAC_RS_S)
#define GMAC_RS_V  0x00000007U
#define GMAC_RS_S  17
/** GMAC_TS : RO; bitpos: [22:20]; default: 0;
 *  Transmit Process State This field indicates the Transmit DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Transmit Command
 *  issued 3’b001: Running: Fetching Transmit Transfer Descriptor 3’b010: Running:
 *  Waiting for status 3’b011: Running: Reading Data from host memory buffer and
 *  queuing it to transmit buffer _Tx FIFO_ 3’b100: TIME_STAMP write state 3’b101:
 *  Reserved for future use 3’b110: Suspended: Transmit Descriptor Unavailable or
 *  Transmit Buffer Underflow 3’b111: Running: Closing Transmit Descriptor
 */
#define GMAC_TS    0x00000007U
#define GMAC_TS_M  (GMAC_TS_V << GMAC_TS_S)
#define GMAC_TS_V  0x00000007U
#define GMAC_TS_S  20
/** GMAC_EB : RO; bitpos: [25:23]; default: 0;
 *  Error Bits This field indicates the type of error that caused a Bus Error, for
 *  example, error response on the AHB or AXI interface This field is valid only when
 *  Bit 13 _FBI_ is set This field does not generate an interrupt 0 0 0: Error during
 *  Rx DMA Write Data Transfer 0 1 1: Error during Tx DMA Read Data Transfer 1 0 0:
 *  Error during Rx DMA Descriptor Write Access 1 0 1: Error during Tx DMA Descriptor
 *  Write Access 1 1 0: Error during Rx DMA Descriptor Read Access 1 1 1: Error during
 *  Tx DMA Descriptor Read Access Note: 001 and 010 are reserved
 */
#define GMAC_EB    0x00000007U
#define GMAC_EB_M  (GMAC_EB_V << GMAC_EB_S)
#define GMAC_EB_V  0x00000007U
#define GMAC_EB_S  23
/** GMAC_GLI : RO; bitpos: [26]; default: 0;
 *  GMAC Line Interface Interrupt When set, this bit reflects any of the following
 *  interrupt events in the DWC_gmac interfaces _if present and enabled in your
 *  configuration_: PCS _TBI, RTBI, or SGMII_: Link change or autonegotiation complete
 *  event SMII or RGMII: Link change event General Purpose Input Status _GPIS_: Any LL
 *  or LH event on the gpi_i input ports To identify the exact cause of the interrupt,
 *  the software must first read Bit 11 and Bits[2:0] of Register 14 _Interrupt Status
 *  Register_ and then to clear the source of interrupt _which also clears the GLI
 *  interrupt_, read any of the following corresponding registers: PCS _TBI, RTBI, or
 *  SGMII_: Register 49 _AN Status Register_ SMII or RGMII: Register 54
 *  _SGMII/RGMII/SMII Control and Status Register_ General Purpose Input _GPI_:
 *  Register 56 _General Purpose IO Register_ The interrupt signal from the DWC_gmac
 *  subsystem _sbd_intr_o_ is high when this bit is high
 */
#define GMAC_GLI    (BIT(26))
#define GMAC_GLI_M  (GMAC_GLI_V << GMAC_GLI_S)
#define GMAC_GLI_V  0x00000001U
#define GMAC_GLI_S  26
/** GMAC_GMI : RO; bitpos: [27]; default: 0;
 *  GMAC MMC Interrupt This bit reflects an interrupt event in the MMC module of the
 *  DWC_gmac The software must read the corresponding registers in the DWC_gmac to get
 *  the exact cause of the interrupt and clear the source of interrupt to make this bit
 *  as 1’b0 The interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when
 *  this bit is high This bit is applicable only when the MAC Management Counters _MMC_
 *  are enabled Otherwise, this bit is reserved
 */
#define GMAC_GMI    (BIT(27))
#define GMAC_GMI_M  (GMAC_GMI_V << GMAC_GMI_S)
#define GMAC_GMI_V  0x00000001U
#define GMAC_GMI_S  27
/** GMAC_GPI : RO; bitpos: [28]; default: 0;
 *  GMAC PMT Interrupt This bit indicates an interrupt event in the PMT module of the
 *  DWC_gmac The software must read the PMT Control and Status Register in the MAC to
 *  get the exact cause of interrupt and clear its source to reset this bit to 1’b0 The
 *  interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when this bit is
 *  high This bit is applicable only when the Power Management feature is enabled
 *  Otherwise, this bit is reserved Note: The GPI and pmt_intr_o interrupts are
 *  generated in different clock domains
 */
#define GMAC_GPI    (BIT(28))
#define GMAC_GPI_M  (GMAC_GPI_V << GMAC_GPI_S)
#define GMAC_GPI_V  0x00000001U
#define GMAC_GPI_S  28
/** GMAC_TTI : RO; bitpos: [29]; default: 0;
 *  Timestamp Trigger Interrupt This bit indicates an interrupt event in the Timestamp
 *  Generator block of the DWC_gmac The software must read the corresponding registers
 *  in the DWC_gmac to get the exact cause of the interrupt and clear its source to
 *  reset this bit to 1'b0 When this bit is high, the interrupt signal from the
 *  DWC_gmac subsystem _sbd_intr_o_ is high This bit is applicable only when the IEEE
 *  1588 Timestamp feature is enabled Otherwise, this bit is reserved
 */
#define GMAC_TTI    (BIT(29))
#define GMAC_TTI_M  (GMAC_TTI_V << GMAC_TTI_S)
#define GMAC_TTI_V  0x00000001U
#define GMAC_TTI_S  29
/** GMAC_GLPII_GTMSI : RO; bitpos: [30]; default: 0;
 *  GLPII: GMAC LPI Interrupt This bit indicates an interrupt event in the LPI logic of
 *  the MAC To reset this bit to 1'b0, the software must read the corresponding
 *  registers in the DWC_gmac to get the exact cause of the interrupt and clear its
 *  source Note: GLPII status is given only in Channel 0 DMA register and is applicable
 *  only when the Energy Efficient Ethernet feature is enabled Otherwise, this bit is
 *  reserved When this bit is high, the interrupt signal from the MAC _sbd_intr_o_ is
 *  high
 */
#define GMAC_GLPII_GTMSI    (BIT(30))
#define GMAC_GLPII_GTMSI_M  (GMAC_GLPII_GTMSI_V << GMAC_GLPII_GTMSI_S)
#define GMAC_GLPII_GTMSI_V  0x00000001U
#define GMAC_GLPII_GTMSI_S  30

/** GMAC_REGISTER6_OPERATIONMODEREGISTER_REG register
 *  Establishes the Receive and Transmit operating modes and command Note: This
 *  register is valid and present in the GMACMTL configuration
 */
#define GMAC_REGISTER6_OPERATIONMODEREGISTER_REG (DR_REG_GMAC_BASE + 0x1018)
/** GMAC_SR : R/W; bitpos: [1]; default: 0;
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
#define GMAC_SR    (BIT(1))
#define GMAC_SR_M  (GMAC_SR_V << GMAC_SR_S)
#define GMAC_SR_V  0x00000001U
#define GMAC_SR_S  1
/** GMAC_OSF : R/W; bitpos: [2]; default: 0;
 *  Operate on Second Frame When this bit is set, it instructs the DMA to process the
 *  second frame of the Transmit data even before the status for the first frame is
 *  obtained
 */
#define GMAC_OSF    (BIT(2))
#define GMAC_OSF_M  (GMAC_OSF_V << GMAC_OSF_S)
#define GMAC_OSF_V  0x00000001U
#define GMAC_OSF_S  2
/** GMAC_RTC : R/W; bitpos: [4:3]; default: 0;
 *  Receive Threshold Control These two bits control the threshold level of the MTL
 *  Receive FIFO Transfer _request_ to DMA starts when the frame size within the MTL
 *  Receive FIFO is larger than the threshold In addition, full frames with length less
 *  than the threshold are automatically transferred The value of 11 is not applicable
 *  if the configured Receive FIFO size is 128 bytes These bits are valid only when the
 *  RSF bit is zero, and are ignored when the RSF bit is set to 1 00: 64 01: 32 10: 96
 *  11: 128
 */
#define GMAC_RTC    0x00000003U
#define GMAC_RTC_M  (GMAC_RTC_V << GMAC_RTC_S)
#define GMAC_RTC_V  0x00000003U
#define GMAC_RTC_S  3
/** GMAC_DGF : R/W; bitpos: [5]; default: 0;
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
#define GMAC_DGF    (BIT(5))
#define GMAC_DGF_M  (GMAC_DGF_V << GMAC_DGF_S)
#define GMAC_DGF_V  0x00000001U
#define GMAC_DGF_S  5
/** GMAC_FUF : R/W; bitpos: [6]; default: 0;
 *  Forward Undersized Good Frames When set, the Rx FIFO forwards Undersized frames
 *  _that is, frames with no Error and length less than 64 bytes_ including padbytes
 *  and CRC When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a
 *  frame is already transferred because of the lower value of Receive Threshold, for
 *  example, RTC = 01
 */
#define GMAC_FUF    (BIT(6))
#define GMAC_FUF_M  (GMAC_FUF_V << GMAC_FUF_S)
#define GMAC_FUF_V  0x00000001U
#define GMAC_FUF_S  6
/** GMAC_FEF : R/W; bitpos: [7]; default: 0;
 *  Forward Error Frames When this bit is reset, the Rx FIFO drops frames with error
 *  status _CRC error, collision error, GMII_ER, giant frame, watchdog timeout, or
 *  overflow_ However, if the start byte _write_ pointer of a frame is already
 *  transferred to the read controller side _in Threshold mode_, then the frame is not
 *  dropped In the GMACMTL configuration in which the Frame Length FIFO is also enabled
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
 *  features: 	L3L4 filter in GMACCORE or GMACMTL configurations 	Full checksum offload
 *  engine _Type 2_ with enhanced descriptor format in the GMACDMA, GMACAHB, or GMACAXI
 *  configurations
 */
#define GMAC_FEF    (BIT(7))
#define GMAC_FEF_M  (GMAC_FEF_V << GMAC_FEF_S)
#define GMAC_FEF_V  0x00000001U
#define GMAC_FEF_S  7
/** GMAC_EFC : R/W; bitpos: [8]; default: 0;
 *  Enable HW Flow Control When this bit is set, the flow control signal operation
 *  based on the filllevel of Rx FIFO is enabled When reset, the flow control operation
 *  is disabled This bit is not used _reserved and always reset_ when the Rx FIFO is
 *  less than 4 KB
 */
#define GMAC_EFC    (BIT(8))
#define GMAC_EFC_M  (GMAC_EFC_V << GMAC_EFC_S)
#define GMAC_EFC_V  0x00000001U
#define GMAC_EFC_S  8
/** GMAC_RFA : R/W; bitpos: [10:9]; default: 0;
 *  Threshold for Activating Flow Control _in halfduplex and fullduplex modes_ These
 *  bits control the threshold _Fill level of Rx FIFO_ at which the flow control is
 *  activated 00: Full minus 1 KB, that is, FULL—1KB 01: Full minus 2 KB, that is,
 *  FULL—2KB 10: Full minus 3 KB, that is, FULL—3KB 11: Full minus 4 KB, that is,
 *  FULL—4KB These values are applicable only to Rx FIFOs of 4 KB or more and when Bit
 *  8 _EFC_ is set high If the Rx FIFO is 8 KB or more, an additional Bit _RFA_2_ is
 *  used for more threshold levels as described in Bit 23 These bits are reserved and
 *  readonly when the depth of Rx FIFO is less than 4 KB Note: When FIFO size is
 *  exactly 4 KB, although the DWC_gmac allows you to program the value of these bits
 *  to 11, the software should not program these bits to 2'b11 The value 2'b11 means
 *  flow control on FIFO empty condition
 */
#define GMAC_RFA    0x00000003U
#define GMAC_RFA_M  (GMAC_RFA_V << GMAC_RFA_S)
#define GMAC_RFA_V  0x00000003U
#define GMAC_RFA_S  9
/** GMAC_RFD : R/W; bitpos: [12:11]; default: 0;
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
#define GMAC_RFD    0x00000003U
#define GMAC_RFD_M  (GMAC_RFD_V << GMAC_RFD_S)
#define GMAC_RFD_V  0x00000003U
#define GMAC_RFD_S  11
/** GMAC_ST : R/W; bitpos: [13]; default: 0;
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
#define GMAC_ST    (BIT(13))
#define GMAC_ST_M  (GMAC_ST_V << GMAC_ST_S)
#define GMAC_ST_V  0x00000001U
#define GMAC_ST_S  13
/** GMAC_TTC : R/W; bitpos: [16:14]; default: 0;
 *  Transmit Threshold Control These bits control the threshold level of the MTL
 *  Transmit FIFO Transmission starts when the frame size within the MTL Transmit FIFO
 *  is larger than the threshold In addition, full frames with a length less than the
 *  threshold are also transmitted These bits are used only when Bit 21 _TSF_ is reset
 *  000: 64 001: 128 010: 192 011: 256 100: 40 101: 32 110: 24 111: 16
 */
#define GMAC_TTC    0x00000007U
#define GMAC_TTC_M  (GMAC_TTC_V << GMAC_TTC_S)
#define GMAC_TTC_V  0x00000007U
#define GMAC_TTC_S  14
/** GMAC_FTF : R/W1S; bitpos: [20]; default: 0;
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
#define GMAC_FTF    (BIT(20))
#define GMAC_FTF_M  (GMAC_FTF_V << GMAC_FTF_S)
#define GMAC_FTF_V  0x00000001U
#define GMAC_FTF_S  20
/** GMAC_TSF : R/W; bitpos: [21]; default: 0;
 *  Transmit Store and Forward When this bit is set, transmission starts when a full
 *  frame resides in the MTL Transmit FIFO When this bit is set, the TTC values
 *  specified in Bits [16:14] are ignored This bit should be changed only when the
 *  transmission is stopped
 */
#define GMAC_TSF    (BIT(21))
#define GMAC_TSF_M  (GMAC_TSF_V << GMAC_TSF_S)
#define GMAC_TSF_V  0x00000001U
#define GMAC_TSF_S  21
/** GMAC_RFD_2 : R/W; bitpos: [22]; default: 0;
 *  MSB of Threshold for Deactivating Flow Control If the DWC_gmac is configured for Rx
 *  FIFO size of 8 KB or more, this bit _when set_ provides additional threshold levels
 *  for deactivating the flow control in both halfduplex and fullduplex modes This bit
 *  _as Most Significant Bit_ along with the RFD _Bits [12:11]_ gives the following
 *  thresholds for deactivating flow control: 100: Full minus 5 KB, that is, FULL — 5
 *  KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that is, FULL —
 *  7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB or less deep
 */
#define GMAC_RFD_2    (BIT(22))
#define GMAC_RFD_2_M  (GMAC_RFD_2_V << GMAC_RFD_2_S)
#define GMAC_RFD_2_V  0x00000001U
#define GMAC_RFD_2_S  22
/** GMAC_RFA_2 : R/W; bitpos: [23]; default: 0;
 *  MSB of Threshold for Activating Flow Control If the DWC_gmac is configured for an
 *  Rx FIFO size of 8 KB or more, this bit _when set_ provides additional threshold
 *  levels for activating the flow control in both half duplex and fullduplex modes
 *  This bit _as Most Significant Bit_, along with the RFA _Bits [10:9]_, gives the
 *  following thresholds for activating flow control: 100: Full minus 5 KB, that is,
 *  FULL — 5 KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that
 *  is, FULL — 7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB
 *  or less deep
 */
#define GMAC_RFA_2    (BIT(23))
#define GMAC_RFA_2_M  (GMAC_RFA_2_V << GMAC_RFA_2_S)
#define GMAC_RFA_2_V  0x00000001U
#define GMAC_RFA_2_S  23
/** GMAC_DFF : R/W; bitpos: [24]; default: 0;
 *  Disable Flushing of Received Frames When this bit is set, the Rx DMA does not flush
 *  any frames because of the unavailability of receive descriptors or buffers as it
 *  does normally when this bit is reset _See “Receive Process Suspended” on page 83_
 *  This bit is reserved _and RO_ in the GMACMTL configuration
 */
#define GMAC_DFF    (BIT(24))
#define GMAC_DFF_M  (GMAC_DFF_V << GMAC_DFF_S)
#define GMAC_DFF_V  0x00000001U
#define GMAC_DFF_S  24
/** GMAC_RSF : R/W; bitpos: [25]; default: 0;
 *  Receive Store and Forward When this bit is set, the MTL reads a frame from the Rx
 *  FIFO only after the complete frame has been written to it, ignoring the RTC bits
 *  When this bit is reset, the Rx FIFO operates in the cutthrough mode, subject to the
 *  threshold specified by the RTC bits
 */
#define GMAC_RSF    (BIT(25))
#define GMAC_RSF_M  (GMAC_RSF_V << GMAC_RSF_S)
#define GMAC_RSF_V  0x00000001U
#define GMAC_RSF_S  25
/** GMAC_DT : R/W; bitpos: [26]; default: 0;
 *  Disable Dropping of TCP/IP Checksum Error Frames When this bit is set, the MAC does
 *  not drop the frames which only have errors detected by the Receive Checksum Offload
 *  engine Such frames do not have any errors _including FCS error_ in the Ethernet
 *  frame received by the MAC but have errors only in the encapsulated payload When
 *  this bit is reset, all error frames are dropped if the FEF bit is reset If the IPC
 *  Full Checksum Offload Engine _Type 2_ is disabled, this bit is reserved _RO with
 *  value 1'b0_
 */
#define GMAC_DT    (BIT(26))
#define GMAC_DT_M  (GMAC_DT_V << GMAC_DT_S)
#define GMAC_DT_V  0x00000001U
#define GMAC_DT_S  26

/** GMAC_REGISTER7_INTERRUPTENABLEREGISTER_REG register
 *  Enables the interrupts reported by the Status Register
 */
#define GMAC_REGISTER7_INTERRUPTENABLEREGISTER_REG (DR_REG_GMAC_BASE + 0x101c)
/** GMAC_TIE : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Transmit Interrupt is enabled When this bit is reset, the Transmit
 *  Interrupt is disabled The sbd_intr_o interrupt is generated as shown in Figure 61
 *  It is asserted only when the TTI, GPI, GMI, GLI, or GLPII bit of the DMA Status
 *  register is asserted, or when the NIS or AIS Status bit is asserted and the
 *  corresponding Interrupt Enable bits _NIE or AIE_ are enabled
 */
#define GMAC_TIE    (BIT(0))
#define GMAC_TIE_M  (GMAC_TIE_V << GMAC_TIE_S)
#define GMAC_TIE_V  0x00000001U
#define GMAC_TIE_S  0
/** GMAC_TSE : R/W; bitpos: [1]; default: 0;
 *  Transmit Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Transmission Stopped Interrupt is enabled When this bit is reset, the
 *  Transmission Stopped Interrupt is disabled
 */
#define GMAC_TSE    (BIT(1))
#define GMAC_TSE_M  (GMAC_TSE_V << GMAC_TSE_S)
#define GMAC_TSE_V  0x00000001U
#define GMAC_TSE_S  1
/** GMAC_TUE : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable Enable When this bit is set with Normal Interrupt
 *  Summary Enable _Bit 16_, the Transmit Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled
 */
#define GMAC_TUE    (BIT(2))
#define GMAC_TUE_M  (GMAC_TUE_V << GMAC_TUE_S)
#define GMAC_TUE_V  0x00000001U
#define GMAC_TUE_S  2
/** GMAC_TJE : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Jabber Timeout Interrupt is enabled When this bit is
 *  reset, the Transmit Jabber Timeout Interrupt is disabled
 */
#define GMAC_TJE    (BIT(3))
#define GMAC_TJE_M  (GMAC_TJE_V << GMAC_TJE_S)
#define GMAC_TJE_V  0x00000001U
#define GMAC_TJE_S  3
/** GMAC_OVE : R/W; bitpos: [4]; default: 0;
 *  Overflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Receive Overflow Interrupt is enabled When this bit is reset,
 *  the Overflow Interrupt is disabled
 */
#define GMAC_OVE    (BIT(4))
#define GMAC_OVE_M  (GMAC_OVE_V << GMAC_OVE_S)
#define GMAC_OVE_V  0x00000001U
#define GMAC_OVE_S  4
/** GMAC_UNE : R/W; bitpos: [5]; default: 0;
 *  Underflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Underflow Interrupt is enabled When this bit is
 *  reset, the Underflow Interrupt is disabled
 */
#define GMAC_UNE    (BIT(5))
#define GMAC_UNE_M  (GMAC_UNE_V << GMAC_UNE_S)
#define GMAC_UNE_V  0x00000001U
#define GMAC_UNE_S  5
/** GMAC_RIE : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Receive Interrupt is enabled When this bit is reset, the Receive
 *  Interrupt is disabled
 */
#define GMAC_RIE    (BIT(6))
#define GMAC_RIE_M  (GMAC_RIE_V << GMAC_RIE_S)
#define GMAC_RIE_V  0x00000001U
#define GMAC_RIE_S  6
/** GMAC_RUE : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Receive Buffer Unavailable Interrupt is disabled
 */
#define GMAC_RUE    (BIT(7))
#define GMAC_RUE_M  (GMAC_RUE_V << GMAC_RUE_S)
#define GMAC_RUE_V  0x00000001U
#define GMAC_RUE_S  7
/** GMAC_RSE : R/W; bitpos: [8]; default: 0;
 *  Receive Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Receive Stopped Interrupt is enabled When this bit is reset, the
 *  Receive Stopped Interrupt is disabled
 */
#define GMAC_RSE    (BIT(8))
#define GMAC_RSE_M  (GMAC_RSE_V << GMAC_RSE_S)
#define GMAC_RSE_V  0x00000001U
#define GMAC_RSE_S  8
/** GMAC_RWE : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Watchdog Timeout Interrupt is enabled When
 *  this bit is reset, the Receive Watchdog Timeout Interrupt is disabled
 */
#define GMAC_RWE    (BIT(9))
#define GMAC_RWE_M  (GMAC_RWE_V << GMAC_RWE_S)
#define GMAC_RWE_V  0x00000001U
#define GMAC_RWE_S  9
/** GMAC_ETE : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt Enable When this bit is set with an Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Early Transmit Interrupt is enabled When this bit is
 *  reset, the Early Transmit Interrupt is disabled
 */
#define GMAC_ETE    (BIT(10))
#define GMAC_ETE_M  (GMAC_ETE_V << GMAC_ETE_S)
#define GMAC_ETE_V  0x00000001U
#define GMAC_ETE_S  10
/** GMAC_FBE : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Fatal Bus Error Interrupt is enabled When this bit is reset, the
 *  Fatal Bus Error Enable Interrupt is disabled 12:11		Reserved	00	RO
 */
#define GMAC_FBE    (BIT(13))
#define GMAC_FBE_M  (GMAC_FBE_V << GMAC_FBE_S)
#define GMAC_FBE_V  0x00000001U
#define GMAC_FBE_S  13
/** GMAC_ERE : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt Enable When this bit is set with Normal Interrupt Summary
 *  Enable _Bit 16_, the Early Receive Interrupt is enabled When this bit is reset, the
 *  Early Receive Interrupt is disabled
 */
#define GMAC_ERE    (BIT(14))
#define GMAC_ERE_M  (GMAC_ERE_V << GMAC_ERE_S)
#define GMAC_ERE_V  0x00000001U
#define GMAC_ERE_S  14
/** GMAC_AIE : R/W; bitpos: [15]; default: 0;
 *  Abnormal Interrupt Summary Enable When this bit is set, abnormal interrupt summary
 *  is enabled When this bit is reset, the abnormal interrupt summary is disabled This
 *  bit enables the following interrupts in Register 5 _Status Register_: Register
 *  5[1]: Transmit Process Stopped Register 5[3]: Transmit Jabber Timeout Register
 *  5[4]: Receive Overflow Register 5[5]: Transmit Underflow Register 5[7]: Receive
 *  Buffer Unavailable Register 5[8]: Receive Process Stopped Register 5[9]: Receive
 *  Watchdog Timeout Register 5[10]: Early Transmit Interrupt Register 5[13]: Fatal Bus
 *  Error
 */
#define GMAC_AIE    (BIT(15))
#define GMAC_AIE_M  (GMAC_AIE_V << GMAC_AIE_S)
#define GMAC_AIE_V  0x00000001U
#define GMAC_AIE_S  15
/** GMAC_NIE : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Enable When this bit is set, normal interrupt summary is
 *  enabled When this bit is reset, normal interrupt summary is disabled This bit
 *  enables the following interrupts in Register 5 _Status Register_: Register 5[0]:
 *  Transmit Interrupt Register 5[2]: Transmit Buffer Unavailable Register 5[6]:
 *  Receive Interrupt Register 5[14]: Early Receive Interrupt
 */
#define GMAC_NIE    (BIT(16))
#define GMAC_NIE_M  (GMAC_NIE_V << GMAC_NIE_S)
#define GMAC_NIE_V  0x00000001U
#define GMAC_NIE_S  16

/** GMAC_REGISTER8_MISSEDFRAMEANDBUFFEROVERFLOWCOUNTERREGISTER_REG register
 *  Contains the counters for discarded frames because no host Receive Descriptor was
 *  available or because of Receive FIFO Overflow
 */
#define GMAC_REGISTER8_MISSEDFRAMEANDBUFFEROVERFLOWCOUNTERREGISTER_REG (DR_REG_GMAC_BASE + 0x1020)
/** GMAC_MISFRMCNT : R/W; bitpos: [15:0]; default: 0;
 *  Missed Frame Counter This field indicates the number of frames missed by the
 *  controller because of the Host Receive Buffer being unavailable This counter is
 *  incremented each time the DMA discards an incoming frame The counter is cleared
 *  when this register is read with mci_be_i[0] at 1’b1
 */
#define GMAC_MISFRMCNT    0x0000FFFFU
#define GMAC_MISFRMCNT_M  (GMAC_MISFRMCNT_V << GMAC_MISFRMCNT_S)
#define GMAC_MISFRMCNT_V  0x0000FFFFU
#define GMAC_MISFRMCNT_S  0
/** GMAC_MISCNTOVF : R/W; bitpos: [16]; default: 0;
 *  Overflow Bit for Missed Frame Counter This bit is set every time Missed Frame
 *  Counter _Bits[15:0]_ overflows, that is, the DMA discards an incoming frame because
 *  of the Host Receive Buffer being unavailable with the missed frame counter at
 *  maximum value In such a scenario, the Missed frame counter is reset to allzeros and
 *  this bit indicates that the rollover happened
 */
#define GMAC_MISCNTOVF    (BIT(16))
#define GMAC_MISCNTOVF_M  (GMAC_MISCNTOVF_V << GMAC_MISCNTOVF_S)
#define GMAC_MISCNTOVF_V  0x00000001U
#define GMAC_MISCNTOVF_S  16
/** GMAC_OVFFRMCNT : R/W; bitpos: [27:17]; default: 0;
 *  Overflow Frame Counter This field indicates the number of frames missed by the
 *  application This counter is incremented each time the MTL FIFO overflows The
 *  counter is cleared when this register is read with mci_be_i[2] at 1’b1
 */
#define GMAC_OVFFRMCNT    0x000007FFU
#define GMAC_OVFFRMCNT_M  (GMAC_OVFFRMCNT_V << GMAC_OVFFRMCNT_S)
#define GMAC_OVFFRMCNT_V  0x000007FFU
#define GMAC_OVFFRMCNT_S  17
/** GMAC_OVFCNTOVF : R/W; bitpos: [28]; default: 0;
 *  Overflow Bit for FIFO Overflow Counter This bit is set every time the Overflow
 *  Frame Counter _Bits[27:17]_ overflows, that is, the Rx FIFO overflows with the
 *  overflow frame counter at maximum value In such a scenario, the overflow frame
 *  counter is reset to allzeros and this bit indicates that the rollover happened
 */
#define GMAC_OVFCNTOVF    (BIT(28))
#define GMAC_OVFCNTOVF_M  (GMAC_OVFCNTOVF_V << GMAC_OVFCNTOVF_S)
#define GMAC_OVFCNTOVF_V  0x00000001U
#define GMAC_OVFCNTOVF_S  28

/** GMAC_REGISTER9_RECEIVEINTERRUPTWATCHDOGTIMERREGISTER_REG register
 *  Watchdog timeout for Receive Interrupt _RI_ from DMA
 */
#define GMAC_REGISTER9_RECEIVEINTERRUPTWATCHDOGTIMERREGISTER_REG (DR_REG_GMAC_BASE + 0x1024)
/** GMAC_RIWT : R/W; bitpos: [7:0]; default: 0;
 *  RI Watchdog Timer Count This bit indicates the number of system clock cycles
 *  multiplied by 256 for which the watchdog timer is set The watchdog timer gets
 *  triggered with the programmed value after the Rx DMA completes the transfer of a
 *  frame for which the RI status bit is not set because of the setting in the
 *  corresponding descriptor RDES1[31] When the watchdog timer runs out, the RI bit is
 *  set and the timer is stopped The watchdog timer is reset when the RI bit is set
 *  high because of automatic setting of RI as per RDES1[31] of any received frame
 */
#define GMAC_RIWT    0x000000FFU
#define GMAC_RIWT_M  (GMAC_RIWT_V << GMAC_RIWT_S)
#define GMAC_RIWT_V  0x000000FFU
#define GMAC_RIWT_S  0

/** GMAC_REGISTER10_AXIBUSMODEREGISTER_REG register
 *  Controls AXI master behavior _mainly controls burst splitting and number of
 *  outstanding requests_
 */
#define GMAC_REGISTER10_AXIBUSMODEREGISTER_REG (DR_REG_GMAC_BASE + 0x1028)
/** GMAC_UNDEF : RO; bitpos: [0]; default: 1;
 *  AXI Undefined Burst Length This bit is readonly bit and indicates the complement
 *  _invert_ value of Bit 16 _FB_ in Register 0 _Bus Mode Register_ When this bit is
 *  set to 1, the GMACAXI is allowed to perform any burst length equal to or below the
 *  maximum allowed burst length programmed in Bits[7:3] When this bit is set to 0, the
 *  GMACAXI is allowed to perform only fixed burst lengths as indicated by BLEN256,
 *  BLEN128, BLEN64, BLEN32, BLEN16, BLEN8, or BLEN4, or a burst length of 1 If UNDEF
 *  is set and none of the BLEN bits is set, then GMACAXI is allowed to perform a burst
 *  length of 16
 */
#define GMAC_UNDEF    (BIT(0))
#define GMAC_UNDEF_M  (GMAC_UNDEF_V << GMAC_UNDEF_S)
#define GMAC_UNDEF_V  0x00000001U
#define GMAC_UNDEF_S  0
/** GMAC_BLEN4 : R/W; bitpos: [1]; default: 0;
 *  AXI Burst Length 4 When this bit is set to 1, the GMACAXI is allowed to select a
 *  burst length of 4 on the AXI master interface Setting this bit has no effect when
 *  UNDEF is set to 1
 */
#define GMAC_BLEN4    (BIT(1))
#define GMAC_BLEN4_M  (GMAC_BLEN4_V << GMAC_BLEN4_S)
#define GMAC_BLEN4_V  0x00000001U
#define GMAC_BLEN4_S  1
/** GMAC_BLEN8 : R/W; bitpos: [2]; default: 0;
 *  AXI Burst Length 8 When this bit is set to 1, the GMACAXI is allowed to select a
 *  burst length of 8 on the AXI master interface Setting this bit has no effect when
 *  UNDEF is set to 1
 */
#define GMAC_BLEN8    (BIT(2))
#define GMAC_BLEN8_M  (GMAC_BLEN8_V << GMAC_BLEN8_S)
#define GMAC_BLEN8_V  0x00000001U
#define GMAC_BLEN8_S  2
/** GMAC_BLEN16 : R/W; bitpos: [3]; default: 0;
 *  AXI Burst Length 16 When this bit is set to 1 or UNDEF is set to 1, the GMACAXI is
 *  allowed to select a burst length of 16 on the AXI master interface
 */
#define GMAC_BLEN16    (BIT(3))
#define GMAC_BLEN16_M  (GMAC_BLEN16_V << GMAC_BLEN16_S)
#define GMAC_BLEN16_V  0x00000001U
#define GMAC_BLEN16_S  3
/** GMAC_BLEN32 : R/W; bitpos: [4]; default: 0;
 *  AXI Burst Length 32 When this bit is set to 1, the GMACAXI is allowed to select a
 *  burst length of 32 on the AXI master interface This bit is present only when the
 *  configuration parameter AXI_BL is set to 32 or more Otherwise, this bit is reserved
 *  and is readonly _RO_
 */
#define GMAC_BLEN32    (BIT(4))
#define GMAC_BLEN32_M  (GMAC_BLEN32_V << GMAC_BLEN32_S)
#define GMAC_BLEN32_V  0x00000001U
#define GMAC_BLEN32_S  4
/** GMAC_BLEN64 : R/W; bitpos: [5]; default: 0;
 *  AXI Burst Length 64 When this bit is set to 1, the GMACAXI is allowed to select a
 *  burst length of 64 on the AXI master interface This bit is present only when the
 *  configuration parameter AXI_BL is set to 64 or more Otherwise, this bit is reserved
 *  and is readonly _RO_
 */
#define GMAC_BLEN64    (BIT(5))
#define GMAC_BLEN64_M  (GMAC_BLEN64_V << GMAC_BLEN64_S)
#define GMAC_BLEN64_V  0x00000001U
#define GMAC_BLEN64_S  5
/** GMAC_BLEN128 : R/W; bitpos: [6]; default: 0;
 *  AXI Burst Length 128 When this bit is set to 1, the GMACAXI is allowed to select a
 *  burst length of 128 on the AXI master interface This bit is present only when the
 *  configuration parameter AXI_BL is set to 128 or more Otherwise, this bit is
 *  reserved and is readonly _RO_
 */
#define GMAC_BLEN128    (BIT(6))
#define GMAC_BLEN128_M  (GMAC_BLEN128_V << GMAC_BLEN128_S)
#define GMAC_BLEN128_V  0x00000001U
#define GMAC_BLEN128_S  6
/** GMAC_BLEN256 : R/W; bitpos: [7]; default: 0;
 *  AXI Burst Length 256 When this bit is set to 1, the GMACAXI is allowed to select a
 *  burst length of 256 on the AXI master interface This bit is present only when the
 *  configuration parameter AXI_BL is set to 256 Otherwise, this bit is reserved and is
 *  readonly _RO_
 */
#define GMAC_BLEN256    (BIT(7))
#define GMAC_BLEN256_M  (GMAC_BLEN256_V << GMAC_BLEN256_S)
#define GMAC_BLEN256_V  0x00000001U
#define GMAC_BLEN256_S  7
/** GMAC_AXI_AAL : RO; bitpos: [12]; default: 0;
 *  AddressAligned Beats This bit is readonly bit and reflects the Bit 25 _AAL_ of
 *  Register 0 _Bus Mode Register_ When this bit is set to 1, the GMACAXI performs
 *  addressaligned burst transfers on both read and write channels 11:8		Reserved	0H	RO
 */
#define GMAC_AXI_AAL    (BIT(12))
#define GMAC_AXI_AAL_M  (GMAC_AXI_AAL_V << GMAC_AXI_AAL_S)
#define GMAC_AXI_AAL_V  0x00000001U
#define GMAC_AXI_AAL_S  12
/** GMAC_ONEKBBE : R/W; bitpos: [13]; default: 0;
 *  1 KB Boundary Crossing Enable for the GMACAXI Master When set, the GMACAXI master
 *  performs burst transfers that do not cross 1 KB boundary When reset, the GMACAXI
 *  master performs burst transfers that do not cross 4 KB boundary
 */
#define GMAC_ONEKBBE    (BIT(13))
#define GMAC_ONEKBBE_M  (GMAC_ONEKBBE_V << GMAC_ONEKBBE_S)
#define GMAC_ONEKBBE_V  0x00000001U
#define GMAC_ONEKBBE_S  13
/** GMAC_RD_OSR_LMT : R/W; bitpos: [19:16]; default: 1;
 *  AXI Maximum Read Outstanding Request Limit This value limits the maximum
 *  outstanding request on the AXI read interface Maximum outstanding requests =
 *  RD_OSR_LMT+1 Note: Bit 18 is reserved if AXI_GM_MAX_RD_REQUESTS = 4 Bit 19 is
 *  reserved if AXI_GM_MAX_RD_REQUESTS != 16 15:14		Reserved	00	RO
 */
#define GMAC_RD_OSR_LMT    0x0000000FU
#define GMAC_RD_OSR_LMT_M  (GMAC_RD_OSR_LMT_V << GMAC_RD_OSR_LMT_S)
#define GMAC_RD_OSR_LMT_V  0x0000000FU
#define GMAC_RD_OSR_LMT_S  16
/** GMAC_WR_OSR_LMT : R/W; bitpos: [23:20]; default: 1;
 *  AXI Maximum Write Outstanding Request Limit This value limits the maximum
 *  outstanding request on the AXI write interface Maximum outstanding requests =
 *  WR_OSR_LMT+1 Note: Bit 22 is reserved if AXI_GM_MAX_WR_REQUESTS = 4 Bit 23 bit is
 *  reserved if AXI_GM_MAX_WR_REQUESTS != 16
 */
#define GMAC_WR_OSR_LMT    0x0000000FU
#define GMAC_WR_OSR_LMT_M  (GMAC_WR_OSR_LMT_V << GMAC_WR_OSR_LMT_S)
#define GMAC_WR_OSR_LMT_V  0x0000000FU
#define GMAC_WR_OSR_LMT_S  20
/** GMAC_LPI_XIT_FRM : R/W; bitpos: [30]; default: 0;
 *  Unlock on Magic Packet or Remote WakeUp Frame When set to 1, this bit enables the
 *  GMACAXI to come out of the LPI mode only when the magic packet or remote wakeup
 *  frame is received When set to 0, this bit enables the GMACAXI to come out of LPI
 *  mode when any frame is received 29:24		Reserved	00H	RO
 */
#define GMAC_LPI_XIT_FRM    (BIT(30))
#define GMAC_LPI_XIT_FRM_M  (GMAC_LPI_XIT_FRM_V << GMAC_LPI_XIT_FRM_S)
#define GMAC_LPI_XIT_FRM_V  0x00000001U
#define GMAC_LPI_XIT_FRM_S  30
/** GMAC_EN_LPI : R/W; bitpos: [31]; default: 0;
 *  Enable Low Power Interface _LPI_ When set to 1, this bit enables the LPI mode
 *  supported by the GMACAXI configuration and accepts the LPI request from the AXI
 *  System Clock controller When set to 0, this bit disables the LPI mode and always
 *  denies the LPI request from the AXI System Clock controller
 */
#define GMAC_EN_LPI    (BIT(31))
#define GMAC_EN_LPI_M  (GMAC_EN_LPI_V << GMAC_EN_LPI_S)
#define GMAC_EN_LPI_V  0x00000001U
#define GMAC_EN_LPI_S  31

/** GMAC_REGISTER11_AHBORAXISTATUSREGISTER_REG register
 *  Gives the idle status of the AHB master interface in the GMACAHB configuration
 *  Gives the idle status of the AXI master's read or write channel in the GMACAXI
 *  configuration
 */
#define GMAC_REGISTER11_AHBORAXISTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x102c)
/** GMAC_AXWHSTS : RO; bitpos: [0]; default: 0;
 *  AXI Master Write Channel or AHB Master Status When high, it indicates that AXI
 *  master's write channel is active and transferring data in the GMACAXI configuration
 *  In the GMACAHB configuration, it indicates that the AHB master interface FSMs are
 *  in the nonidle state
 */
#define GMAC_AXWHSTS    (BIT(0))
#define GMAC_AXWHSTS_M  (GMAC_AXWHSTS_V << GMAC_AXWHSTS_S)
#define GMAC_AXWHSTS_V  0x00000001U
#define GMAC_AXWHSTS_S  0
/** GMAC_AXIRDSTS : RO; bitpos: [1]; default: 0;
 *  AXI Master Read Channel Status When high, it indicates that AXI master's read
 *  channel is active and transferring data
 */
#define GMAC_AXIRDSTS    (BIT(1))
#define GMAC_AXIRDSTS_M  (GMAC_AXIRDSTS_V << GMAC_AXIRDSTS_S)
#define GMAC_AXIRDSTS_V  0x00000001U
#define GMAC_AXIRDSTS_S  1

/** GMAC_REGISTER18_CURRENTHOSTTRANSMITDESCRIPTORREGISTER_REG register
 *  Points to the start of current Transmit Descriptor read by the DMA
 */
#define GMAC_REGISTER18_CURRENTHOSTTRANSMITDESCRIPTORREGISTER_REG (DR_REG_GMAC_BASE + 0x1048)
/** GMAC_CURTDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Descriptor Address Pointer
 */
#define GMAC_CURTDESAPTR    0xFFFFFFFFU
#define GMAC_CURTDESAPTR_M  (GMAC_CURTDESAPTR_V << GMAC_CURTDESAPTR_S)
#define GMAC_CURTDESAPTR_V  0xFFFFFFFFU
#define GMAC_CURTDESAPTR_S  0

/** GMAC_REGISTER19_CURRENTHOSTRECEIVEDESCRIPTORREGISTER_REG register
 *  Points to the start of current Receive Descriptor read by the DMA
 */
#define GMAC_REGISTER19_CURRENTHOSTRECEIVEDESCRIPTORREGISTER_REG (DR_REG_GMAC_BASE + 0x104c)
/** GMAC_CURRDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Descriptor Address Pointer
 */
#define GMAC_CURRDESAPTR    0xFFFFFFFFU
#define GMAC_CURRDESAPTR_M  (GMAC_CURRDESAPTR_V << GMAC_CURRDESAPTR_S)
#define GMAC_CURRDESAPTR_V  0xFFFFFFFFU
#define GMAC_CURRDESAPTR_S  0

/** GMAC_REGISTER20_CURRENTHOSTTRANSMITBUFFERADDRESSREGISTER_REG register
 *  Points to the current Transmit Buffer address read by the DMA
 */
#define GMAC_REGISTER20_CURRENTHOSTTRANSMITBUFFERADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x1050)
/** GMAC_CURTBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Buffer Address Pointer
 */
#define GMAC_CURTBUFAPTR    0xFFFFFFFFU
#define GMAC_CURTBUFAPTR_M  (GMAC_CURTBUFAPTR_V << GMAC_CURTBUFAPTR_S)
#define GMAC_CURTBUFAPTR_V  0xFFFFFFFFU
#define GMAC_CURTBUFAPTR_S  0

/** GMAC_REGISTER21_CURRENTHOSTRECEIVEBUFFERADDRESSREGISTER_REG register
 *  Points to the current Receive Buffer address read by the DMA
 */
#define GMAC_REGISTER21_CURRENTHOSTRECEIVEBUFFERADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x1054)
/** GMAC_CURRBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Buffer Address Pointer
 */
#define GMAC_CURRBUFAPTR    0xFFFFFFFFU
#define GMAC_CURRBUFAPTR_M  (GMAC_CURRBUFAPTR_V << GMAC_CURRBUFAPTR_S)
#define GMAC_CURRBUFAPTR_V  0xFFFFFFFFU
#define GMAC_CURRBUFAPTR_S  0

/** GMAC_REGISTER22_HWFEATUREREGISTER_REG register
 *  Indicates the presence of the optional features of the core
 */
#define GMAC_REGISTER22_HWFEATUREREGISTER_REG (DR_REG_GMAC_BASE + 0x1058)
/** GMAC_MIISEL : RO; bitpos: [0]; default: 0;
 *  10 or 100 Mbps support
 */
#define GMAC_MIISEL    (BIT(0))
#define GMAC_MIISEL_M  (GMAC_MIISEL_V << GMAC_MIISEL_S)
#define GMAC_MIISEL_V  0x00000001U
#define GMAC_MIISEL_S  0
/** GMAC_GMIISEL : RO; bitpos: [1]; default: 0;
 *  1000 Mbps support
 */
#define GMAC_GMIISEL    (BIT(1))
#define GMAC_GMIISEL_M  (GMAC_GMIISEL_V << GMAC_GMIISEL_S)
#define GMAC_GMIISEL_V  0x00000001U
#define GMAC_GMIISEL_S  1
/** GMAC_HDSEL : RO; bitpos: [2]; default: 0;
 *  Halfduplex support
 */
#define GMAC_HDSEL    (BIT(2))
#define GMAC_HDSEL_M  (GMAC_HDSEL_V << GMAC_HDSEL_S)
#define GMAC_HDSEL_V  0x00000001U
#define GMAC_HDSEL_S  2
/** GMAC_EXTHASHEN : RO; bitpos: [3]; default: 0;
 *  Expanded DA Hash filter
 */
#define GMAC_EXTHASHEN    (BIT(3))
#define GMAC_EXTHASHEN_M  (GMAC_EXTHASHEN_V << GMAC_EXTHASHEN_S)
#define GMAC_EXTHASHEN_V  0x00000001U
#define GMAC_EXTHASHEN_S  3
/** GMAC_HASHSEL : RO; bitpos: [4]; default: 0;
 *  HASH filter
 */
#define GMAC_HASHSEL    (BIT(4))
#define GMAC_HASHSEL_M  (GMAC_HASHSEL_V << GMAC_HASHSEL_S)
#define GMAC_HASHSEL_V  0x00000001U
#define GMAC_HASHSEL_S  4
/** GMAC_ADDMACADRSEL : RO; bitpos: [5]; default: 0;
 *  Multiple MAC Address registers
 */
#define GMAC_ADDMACADRSEL    (BIT(5))
#define GMAC_ADDMACADRSEL_M  (GMAC_ADDMACADRSEL_V << GMAC_ADDMACADRSEL_S)
#define GMAC_ADDMACADRSEL_V  0x00000001U
#define GMAC_ADDMACADRSEL_S  5
/** GMAC_PCSSEL : RO; bitpos: [6]; default: 0;
 *  PCS registers _TBI, SGMII, or RTBI PHY interface_
 */
#define GMAC_PCSSEL    (BIT(6))
#define GMAC_PCSSEL_M  (GMAC_PCSSEL_V << GMAC_PCSSEL_S)
#define GMAC_PCSSEL_V  0x00000001U
#define GMAC_PCSSEL_S  6
/** GMAC_L3L4FLTREN : RO; bitpos: [7]; default: 0;
 *  Layer 3 and Layer 4 feature
 */
#define GMAC_L3L4FLTREN    (BIT(7))
#define GMAC_L3L4FLTREN_M  (GMAC_L3L4FLTREN_V << GMAC_L3L4FLTREN_S)
#define GMAC_L3L4FLTREN_V  0x00000001U
#define GMAC_L3L4FLTREN_S  7
/** GMAC_SMASEL : RO; bitpos: [8]; default: 0;
 *  SMA _MDIO_ Interface
 */
#define GMAC_SMASEL    (BIT(8))
#define GMAC_SMASEL_M  (GMAC_SMASEL_V << GMAC_SMASEL_S)
#define GMAC_SMASEL_V  0x00000001U
#define GMAC_SMASEL_S  8
/** GMAC_RWKSEL : RO; bitpos: [9]; default: 0;
 *  PMT remote wakeup frame
 */
#define GMAC_RWKSEL    (BIT(9))
#define GMAC_RWKSEL_M  (GMAC_RWKSEL_V << GMAC_RWKSEL_S)
#define GMAC_RWKSEL_V  0x00000001U
#define GMAC_RWKSEL_S  9
/** GMAC_MGKSEL : RO; bitpos: [10]; default: 0;
 *  PMT magic packet
 */
#define GMAC_MGKSEL    (BIT(10))
#define GMAC_MGKSEL_M  (GMAC_MGKSEL_V << GMAC_MGKSEL_S)
#define GMAC_MGKSEL_V  0x00000001U
#define GMAC_MGKSEL_S  10
/** GMAC_MMCSEL : RO; bitpos: [11]; default: 0;
 *  RMON module
 */
#define GMAC_MMCSEL    (BIT(11))
#define GMAC_MMCSEL_M  (GMAC_MMCSEL_V << GMAC_MMCSEL_S)
#define GMAC_MMCSEL_V  0x00000001U
#define GMAC_MMCSEL_S  11
/** GMAC_TSVER1SEL : RO; bitpos: [12]; default: 0;
 *  Only IEEE 15882002 timestamp
 */
#define GMAC_TSVER1SEL    (BIT(12))
#define GMAC_TSVER1SEL_M  (GMAC_TSVER1SEL_V << GMAC_TSVER1SEL_S)
#define GMAC_TSVER1SEL_V  0x00000001U
#define GMAC_TSVER1SEL_S  12
/** GMAC_TSVER2SEL : RO; bitpos: [13]; default: 0;
 *  IEEE 15882008 Advanced timestamp
 */
#define GMAC_TSVER2SEL    (BIT(13))
#define GMAC_TSVER2SEL_M  (GMAC_TSVER2SEL_V << GMAC_TSVER2SEL_S)
#define GMAC_TSVER2SEL_V  0x00000001U
#define GMAC_TSVER2SEL_S  13
/** GMAC_EEESEL : RO; bitpos: [14]; default: 0;
 *  Energy Efficient Ethernet
 */
#define GMAC_EEESEL    (BIT(14))
#define GMAC_EEESEL_M  (GMAC_EEESEL_V << GMAC_EEESEL_S)
#define GMAC_EEESEL_V  0x00000001U
#define GMAC_EEESEL_S  14
/** GMAC_AVSEL : RO; bitpos: [15]; default: 0;
 *  AV feature
 */
#define GMAC_AVSEL    (BIT(15))
#define GMAC_AVSEL_M  (GMAC_AVSEL_V << GMAC_AVSEL_S)
#define GMAC_AVSEL_V  0x00000001U
#define GMAC_AVSEL_S  15
/** GMAC_TXCOESEL : RO; bitpos: [16]; default: 0;
 *  Checksum Offload in Tx
 */
#define GMAC_TXCOESEL    (BIT(16))
#define GMAC_TXCOESEL_M  (GMAC_TXCOESEL_V << GMAC_TXCOESEL_S)
#define GMAC_TXCOESEL_V  0x00000001U
#define GMAC_TXCOESEL_S  16
/** GMAC_RXTYP1COE : RO; bitpos: [17]; default: 0;
 *  IP Checksum Offload _Type 1_ in Rx Note: If IPCHKSUM_EN = Enabled and
 *  IPC_FULL_OFFLOAD = Enabled, then RXTYP1COE = 0 and RXTYP2COE = 1
 */
#define GMAC_RXTYP1COE    (BIT(17))
#define GMAC_RXTYP1COE_M  (GMAC_RXTYP1COE_V << GMAC_RXTYP1COE_S)
#define GMAC_RXTYP1COE_V  0x00000001U
#define GMAC_RXTYP1COE_S  17
/** GMAC_RXTYP2COE : RO; bitpos: [18]; default: 0;
 *  IP Checksum Offload _Type 2_ in Rx
 */
#define GMAC_RXTYP2COE    (BIT(18))
#define GMAC_RXTYP2COE_M  (GMAC_RXTYP2COE_V << GMAC_RXTYP2COE_S)
#define GMAC_RXTYP2COE_V  0x00000001U
#define GMAC_RXTYP2COE_S  18
/** GMAC_RXFIFOSIZE : RO; bitpos: [19]; default: 0;
 *  Rx FIFO > 2,048 Bytes
 */
#define GMAC_RXFIFOSIZE    (BIT(19))
#define GMAC_RXFIFOSIZE_M  (GMAC_RXFIFOSIZE_V << GMAC_RXFIFOSIZE_S)
#define GMAC_RXFIFOSIZE_V  0x00000001U
#define GMAC_RXFIFOSIZE_S  19
/** GMAC_RXCHCNT : RO; bitpos: [21:20]; default: 0;
 *  Number of additional Rx Channels
 */
#define GMAC_RXCHCNT    0x00000003U
#define GMAC_RXCHCNT_M  (GMAC_RXCHCNT_V << GMAC_RXCHCNT_S)
#define GMAC_RXCHCNT_V  0x00000003U
#define GMAC_RXCHCNT_S  20
/** GMAC_TXCHCNT : RO; bitpos: [23:22]; default: 0;
 *  Number of additional Tx Channels
 */
#define GMAC_TXCHCNT    0x00000003U
#define GMAC_TXCHCNT_M  (GMAC_TXCHCNT_V << GMAC_TXCHCNT_S)
#define GMAC_TXCHCNT_V  0x00000003U
#define GMAC_TXCHCNT_S  22
/** GMAC_ENHDESSEL : RO; bitpos: [24]; default: 0;
 *  Alternate _Enhanced Descriptor_
 */
#define GMAC_ENHDESSEL    (BIT(24))
#define GMAC_ENHDESSEL_M  (GMAC_ENHDESSEL_V << GMAC_ENHDESSEL_S)
#define GMAC_ENHDESSEL_V  0x00000001U
#define GMAC_ENHDESSEL_S  24
/** GMAC_INTTSEN : RO; bitpos: [25]; default: 0;
 *  Timestamping with Internal System Time
 */
#define GMAC_INTTSEN    (BIT(25))
#define GMAC_INTTSEN_M  (GMAC_INTTSEN_V << GMAC_INTTSEN_S)
#define GMAC_INTTSEN_V  0x00000001U
#define GMAC_INTTSEN_S  25
/** GMAC_FLEXIPPSEN : RO; bitpos: [26]; default: 0;
 *  Flexible PulsePerSecond Output
 */
#define GMAC_FLEXIPPSEN    (BIT(26))
#define GMAC_FLEXIPPSEN_M  (GMAC_FLEXIPPSEN_V << GMAC_FLEXIPPSEN_S)
#define GMAC_FLEXIPPSEN_V  0x00000001U
#define GMAC_FLEXIPPSEN_S  26
/** GMAC_SAVLANINS : RO; bitpos: [27]; default: 0;
 *  Source Address or VLAN Insertion
 */
#define GMAC_SAVLANINS    (BIT(27))
#define GMAC_SAVLANINS_M  (GMAC_SAVLANINS_V << GMAC_SAVLANINS_S)
#define GMAC_SAVLANINS_V  0x00000001U
#define GMAC_SAVLANINS_S  27
/** GMAC_ACTPHYIF : RO; bitpos: [30:28]; default: 0;
 *  Active or selected PHY interface When you have multiple PHY interfaces in your
 *  configuration, this field indicates the sampled value of phy_intf_sel_i during
 *  reset deassertion 000: GMII or MII 001: RGMII 010: SGMII 011: TBI 100: RMII 101:
 *  RTBI 110: SMII 111: RevMII All Others: Reserved
 */
#define GMAC_ACTPHYIF    0x00000007U
#define GMAC_ACTPHYIF_M  (GMAC_ACTPHYIF_V << GMAC_ACTPHYIF_S)
#define GMAC_ACTPHYIF_V  0x00000007U
#define GMAC_ACTPHYIF_S  28

/** GMAC_REGISTER64_CHANNEL1BUSMODEREGISTER_REG register
 *  Controls the Host Interface mode for Channel 1
 */
#define GMAC_REGISTER64_CHANNEL1BUSMODEREGISTER_REG (DR_REG_GMAC_BASE + 0x1100)
/** GMAC_CH1_SWR : R/W1S; bitpos: [0]; default: 1;
 *  Software Reset When this bit is set, the MAC DMA Controller resets the logic and
 *  all internal registers of the MAC It is cleared automatically after the reset
 *  operation is complete in all of the DWC_gmac clock domains Before reprogramming any
 *  register of the DWC_gmac, you should read a zero _0_ value in this bit Note: The
 *  Software reset function is driven only by this bit Bit 0 of Register 64 _Channel 1
 *  Bus Mode Register_ or Register 128 _Channel 2 Bus Mode Register_ has no impact on
 *  the Software reset function The reset operation is completed only when all resets
 *  in all active clock domains are deasserted Therefore, it is essential that all PHY
 *  inputs clocks _applicable for the selected PHY interface_ are present for the
 *  software reset completion The time to complete the software reset operation depends
 *  on the frequency of the slowest active clock
 */
#define GMAC_CH1_SWR    (BIT(0))
#define GMAC_CH1_SWR_M  (GMAC_CH1_SWR_V << GMAC_CH1_SWR_S)
#define GMAC_CH1_SWR_V  0x00000001U
#define GMAC_CH1_SWR_S  0
/** GMAC_CH1_DA : R/W; bitpos: [1]; default: 0;
 *  DMA Arbitration Scheme This bit specifies the arbitration scheme between the
 *  transmit and receive paths of Channel 1 0: Weighted roundrobin with Rx:Tx or Tx:Rx
 *  The priority between the paths is according to the priority specified in Bits
 *  [15:14] _PR_ and priority weights specified in Bit 27 _TXPR_ 1: Fixed priority The
 *  transmit path has priority over receive path when Bit 27 _TXPR_ is set Otherwise,
 *  receive path has priority over the transmit path In the GMACAXI configuration,
 *  these bits are reserved and are readonly _RO_ For more information about the
 *  priority scheme between the transmit and receive paths, see Table 412 in “DMA
 *  Arbiter Functions” on page 167
 */
#define GMAC_CH1_DA    (BIT(1))
#define GMAC_CH1_DA_M  (GMAC_CH1_DA_V << GMAC_CH1_DA_S)
#define GMAC_CH1_DA_V  0x00000001U
#define GMAC_CH1_DA_S  1
/** GMAC_CH1_DSL : R/W; bitpos: [6:2]; default: 0;
 *  Descriptor Skip Length This bit specifies the number of Word, Dword, or Lword
 *  _depending on the 32bit, 64bit, or 128bit bus_ to skip between two unchained
 *  descriptors The address skipping starts from the end of current descriptor to the
 *  start of next descriptor When the DSL value is equal to zero, the descriptor table
 *  is taken as contiguous by the DMA in Ring mode
 */
#define GMAC_CH1_DSL    0x0000001FU
#define GMAC_CH1_DSL_M  (GMAC_CH1_DSL_V << GMAC_CH1_DSL_S)
#define GMAC_CH1_DSL_V  0x0000001FU
#define GMAC_CH1_DSL_S  2
/** GMAC_CH1_ATDS : R/W; bitpos: [7]; default: 0;
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
#define GMAC_CH1_ATDS    (BIT(7))
#define GMAC_CH1_ATDS_M  (GMAC_CH1_ATDS_V << GMAC_CH1_ATDS_S)
#define GMAC_CH1_ATDS_V  0x00000001U
#define GMAC_CH1_ATDS_S  7
/** GMAC_CH1_PBL : R/W; bitpos: [13:8]; default: 1;
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
#define GMAC_CH1_PBL    0x0000003FU
#define GMAC_CH1_PBL_M  (GMAC_CH1_PBL_V << GMAC_CH1_PBL_S)
#define GMAC_CH1_PBL_V  0x0000003FU
#define GMAC_CH1_PBL_S  8
/** GMAC_CH1_PR : R/W; bitpos: [15:14]; default: 0;
 *  Priority Ratio These bits control the priority ratio in the weighted roundrobin
 *  arbitration between the Rx DMA and Tx DMA These bits are valid only when Bit 1 _DA_
 *  is reset The priority ratio is Rx:Tx or Tx:Rx depending on whether Bit 27 _TXPR_ is
 *  reset or set 00: The Priority Ratio is 1:1 01: The Priority Ratio is 2:1 10: The
 *  Priority Ratio is 3:1 11: The Priority Ratio is 4:1 In the GMACAXI configuration,
 *  these bits are reserved and readonly _RO_ For more information about the priority
 *  scheme between the transmit and receive paths, see Table 412 in “DMA Arbiter
 *  Functions” on page 167
 */
#define GMAC_CH1_PR    0x00000003U
#define GMAC_CH1_PR_M  (GMAC_CH1_PR_V << GMAC_CH1_PR_S)
#define GMAC_CH1_PR_V  0x00000003U
#define GMAC_CH1_PR_S  14
/** GMAC_CH1_FB : R/W; bitpos: [16]; default: 0;
 *  Fixed Burst This bit controls whether the AHB or AXI master interface performs
 *  fixed burst transfers or not When set, the AHB interface uses only SINGLE, INCR4,
 *  INCR8, or INCR16 during start of the normal burst transfers When reset, the AHB or
 *  AXI interface uses SINGLE and INCR burst transfer operations For more information,
 *  see Bit 0 _UNDEF_ of the AXI Bus Mode register in the GMACAXI configuration
 */
#define GMAC_CH1_FB    (BIT(16))
#define GMAC_CH1_FB_M  (GMAC_CH1_FB_V << GMAC_CH1_FB_S)
#define GMAC_CH1_FB_V  0x00000001U
#define GMAC_CH1_FB_S  16
/** GMAC_CH1_RPBL : R/W; bitpos: [22:17]; default: 1;
 *  Rx DMA PBL This field indicates the maximum number of beats to be transferred in
 *  one Rx DMA transaction This is the maximum value that is used in a single block
 *  Read or Write The Rx DMA always attempts to burst as specified in the RPBL bit each
 *  time it starts a Burst transfer on the host bus You can program RPBL with values of
 *  1, 2, 4, 8, 16, and 32 Any other value results in undefined behavior This field is
 *  valid and applicable only when USP is set high
 */
#define GMAC_CH1_RPBL    0x0000003FU
#define GMAC_CH1_RPBL_M  (GMAC_CH1_RPBL_V << GMAC_CH1_RPBL_S)
#define GMAC_CH1_RPBL_V  0x0000003FU
#define GMAC_CH1_RPBL_S  17
/** GMAC_CH1_USP : R/W; bitpos: [23]; default: 0;
 *  Use Separate PBL When set high, this bit configures the Rx DMA to use the value
 *  configured in Bits [22:17] as PBL The PBL value in Bits [13:8] is applicable only
 *  to the Tx DMA operations When reset to low, the PBL value in Bits [13:8] is
 *  applicable for both DMA engines
 */
#define GMAC_CH1_USP    (BIT(23))
#define GMAC_CH1_USP_M  (GMAC_CH1_USP_V << GMAC_CH1_USP_S)
#define GMAC_CH1_USP_V  0x00000001U
#define GMAC_CH1_USP_S  23
/** GMAC_CH1_PBLX8 : R/W; bitpos: [24]; default: 0;
 *  PBLx8 Mode When set high, this bit multiplies the programmed PBL value _Bits
 *  [22:17] and Bits[13:8]_ eight times Therefore, the DMA transfers the data in 8, 16,
 *  32, 64, 128, and 256 beats depending on the PBL value Note: This bit function is
 *  not backward compatible Before release 350a, this bit was 4xPBL
 */
#define GMAC_CH1_PBLX8    (BIT(24))
#define GMAC_CH1_PBLX8_M  (GMAC_CH1_PBLX8_V << GMAC_CH1_PBLX8_S)
#define GMAC_CH1_PBLX8_V  0x00000001U
#define GMAC_CH1_PBLX8_S  24
/** GMAC_CH1_AAL : R/W; bitpos: [25]; default: 0;
 *  AddressAligned Beats When this bit is set high and the FB bit is equal to 1, the
 *  AHB or AXI interface generates all bursts aligned to the start address LS bits If
 *  the FB bit is equal to 0, the first burst _accessing the start address of data
 *  buffer_ is not aligned, but subsequent bursts are aligned to the address This bit
 *  is valid only in the GMACAHB and GMACAXI configurations and is reserved _RO with
 *  default value 0_ in all other configurations
 */
#define GMAC_CH1_AAL    (BIT(25))
#define GMAC_CH1_AAL_M  (GMAC_CH1_AAL_V << GMAC_CH1_AAL_S)
#define GMAC_CH1_AAL_V  0x00000001U
#define GMAC_CH1_AAL_S  25
/** GMAC_CH1_MB : R/W; bitpos: [26]; default: 0;
 *  Mixed Burst When this bit is set high and the FB bit is low, the AHB master
 *  interface starts all bursts of length more than 16 with INCR _undefined burst_,
 *  whereas it reverts to fixed burst transfers _INCRx and SINGLE_ for burst length of
 *  16 and less This bit is valid only in the GMACAHB configuration and reserved in all
 *  other configuration
 */
#define GMAC_CH1_MB    (BIT(26))
#define GMAC_CH1_MB_M  (GMAC_CH1_MB_V << GMAC_CH1_MB_S)
#define GMAC_CH1_MB_V  0x00000001U
#define GMAC_CH1_MB_S  26
/** GMAC_CH1_TXPR : R/W; bitpos: [27]; default: 0;
 *  Transmit Priority When set, this bit indicates that the transmit DMA has higher
 *  priority than the receive DMA during arbitration for the systemside bus In the
 *  GMACAXI configuration, this bit is reserved and readonly _RO_ For more information
 *  about the priority scheme between the transmit and receive paths, see Table 412 in
 *  “DMA Arbiter Functions” on page 167
 */
#define GMAC_CH1_TXPR    (BIT(27))
#define GMAC_CH1_TXPR_M  (GMAC_CH1_TXPR_V << GMAC_CH1_TXPR_S)
#define GMAC_CH1_TXPR_V  0x00000001U
#define GMAC_CH1_TXPR_S  27
/** GMAC_CH1_PRWG : R/W; bitpos: [29:28]; default: 0;
 *  Channel Priority Weights This field sets the priority weights for Channel 1 during
 *  the roundrobin arbitration between the DMA channels for the system bus 00: The
 *  priority weight is 1 01: The priority weight is 2 10: The priority weight is 3 11:
 *  The priority weight is 4 This field is present in all DWC_gmac configurations
 *  except GMACAXI when you select the AV feature Otherwise, this field is reserved and
 *  readonly _RO_ For more information about the priority weights of DMA channels, see
 *  “DMA Arbiter Functions” on page 167
 */
#define GMAC_CH1_PRWG    0x00000003U
#define GMAC_CH1_PRWG_M  (GMAC_CH1_PRWG_V << GMAC_CH1_PRWG_S)
#define GMAC_CH1_PRWG_V  0x00000003U
#define GMAC_CH1_PRWG_S  28
/** GMAC_CH1_RIB : R/W; bitpos: [31]; default: 0;
 *  Rebuild INCRx Burst When this bit is set high and the AHB master gets an EBT
 *  _Retry, Split, or Losing bus grant_, the AHB master interface rebuilds the pending
 *  beats of any burst transfer initiated with INCRx The AHB master interface rebuilds
 *  the beats with a combination of specified bursts with INCRx and SINGLE By default,
 *  the AHB master interface rebuilds pending beats of an EBT with an unspecified
 *  _INCR_ burst This bit is valid only in the GMACAHB configuration It is reserved in
 *  all other configuration
 */
#define GMAC_CH1_RIB    (BIT(31))
#define GMAC_CH1_RIB_M  (GMAC_CH1_RIB_V << GMAC_CH1_RIB_S)
#define GMAC_CH1_RIB_V  0x00000001U
#define GMAC_CH1_RIB_S  31

/** GMAC_REGISTER65_CHANNEL1TRANSMITPOLLDEMANDREGISTER_REG register
 *  Used by the host to instruct the DMA to poll the Transmit Descriptor list
 */
#define GMAC_REGISTER65_CHANNEL1TRANSMITPOLLDEMANDREGISTER_REG (DR_REG_GMAC_BASE + 0x1104)
/** GMAC_CH1_TPD : RO; bitpos: [31:0]; default: 0;
 *  Transmit Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 18 _Current Host Transmit Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  transmission returns to the Suspend state and Bit 2 _TU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the transmission resumes
 */
#define GMAC_CH1_TPD    0xFFFFFFFFU
#define GMAC_CH1_TPD_M  (GMAC_CH1_TPD_V << GMAC_CH1_TPD_S)
#define GMAC_CH1_TPD_V  0xFFFFFFFFU
#define GMAC_CH1_TPD_S  0

/** GMAC_REGISTER66_CHANNEL1RECEIVEPOLLDEMANDREGISTER_REG register
 *  Used by the Host to instruct the DMA to poll the Receive Descriptor list
 */
#define GMAC_REGISTER66_CHANNEL1RECEIVEPOLLDEMANDREGISTER_REG (DR_REG_GMAC_BASE + 0x1108)
/** GMAC_CH1_RPD : RO; bitpos: [31:0]; default: 0;
 *  Receive Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 19 _Current Host Receive Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  reception returns to the Suspended state and Bit 7 _RU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the Rx DMA returns to the
 *  active state
 */
#define GMAC_CH1_RPD    0xFFFFFFFFU
#define GMAC_CH1_RPD_M  (GMAC_CH1_RPD_V << GMAC_CH1_RPD_S)
#define GMAC_CH1_RPD_V  0xFFFFFFFFU
#define GMAC_CH1_RPD_S  0

/** GMAC_REGISTER67_CHANNEL1RECEIVEDESCRIPTORLISTADDRESSREGISTER_REG register
 *  Points the DMA to the start of the Receive Descriptor list
 */
#define GMAC_REGISTER67_CHANNEL1RECEIVEDESCRIPTORLISTADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x110c)
/** GMAC_CH1_RDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Receive List This field contains the base address of the first descriptor
 *  in the Receive Descriptor list The LSB bits _1:0, 2:0, or 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and internally taken as allzero by the DMA Therefore,
 *  these LSB bits are readonly _RO_
 */
#define GMAC_CH1_RDESLA    0xFFFFFFFFU
#define GMAC_CH1_RDESLA_M  (GMAC_CH1_RDESLA_V << GMAC_CH1_RDESLA_S)
#define GMAC_CH1_RDESLA_V  0xFFFFFFFFU
#define GMAC_CH1_RDESLA_S  0

/** GMAC_REGISTER68_CHANNEL1TRANSMITDESCRIPTORLISTADDRESSREGISTER_REG register
 *  Points the DMA to the start of the Transmit Descriptor list
 */
#define GMAC_REGISTER68_CHANNEL1TRANSMITDESCRIPTORLISTADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x1110)
/** GMAC_CH1_TDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Transmit List This field contains the base address of the first descriptor
 *  in the Transmit Descriptor list The LSB bits _1:0, 2:0, 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and are internally taken as allzero by the DMA
 *  Therefore, these LSB bits are readonly _RO_
 */
#define GMAC_CH1_TDESLA    0xFFFFFFFFU
#define GMAC_CH1_TDESLA_M  (GMAC_CH1_TDESLA_V << GMAC_CH1_TDESLA_S)
#define GMAC_CH1_TDESLA_V  0xFFFFFFFFU
#define GMAC_CH1_TDESLA_S  0

/** GMAC_REGISTER69_CHANNEL1STATUSREGISTER_REG register
 *  The Software driver _application_ reads this register during interrupt service
 *  routine or polling to determine the status of the DMA Bits 29:26 are reserved for
 *  the Channel 1 Status Register
 */
#define GMAC_REGISTER69_CHANNEL1STATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x1114)
/** GMAC_CH1_TI : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt This bit indicates that the frame transmission is complete When
 *  transmission is complete, Bit 31 _OWN_ of TDES0 is reset, and the specific frame
 *  status information is updated in the descriptor
 */
#define GMAC_CH1_TI    (BIT(0))
#define GMAC_CH1_TI_M  (GMAC_CH1_TI_V << GMAC_CH1_TI_S)
#define GMAC_CH1_TI_V  0x00000001U
#define GMAC_CH1_TI_S  0
/** GMAC_CH1_TPS : R/W; bitpos: [1]; default: 0;
 *  Transmit Process Stopped This bit is set when the transmission is stopped
 */
#define GMAC_CH1_TPS    (BIT(1))
#define GMAC_CH1_TPS_M  (GMAC_CH1_TPS_V << GMAC_CH1_TPS_S)
#define GMAC_CH1_TPS_V  0x00000001U
#define GMAC_CH1_TPS_S  1
/** GMAC_CH1_TU : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Transmit List and the DMA cannot acquire it Transmission is
 *  suspended Bits[22:20] explain the Transmit Process state transitions To resume
 *  processing Transmit descriptors, the host should change the ownership of the
 *  descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command
 */
#define GMAC_CH1_TU    (BIT(2))
#define GMAC_CH1_TU_M  (GMAC_CH1_TU_V << GMAC_CH1_TU_S)
#define GMAC_CH1_TU_V  0x00000001U
#define GMAC_CH1_TU_S  2
/** GMAC_CH1_TJT : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout This bit indicates that the Transmit Jabber Timer expired,
 *  which happens when the frame size exceeds 2,048 _10,240 bytes when the Jumbo frame
 *  is enabled_ When the Jabber Timeout occurs, the transmission process is aborted and
 *  placed in the Stopped state This causes the Transmit Jabber Timeout TDES0[14] flag
 *  to assert
 */
#define GMAC_CH1_TJT    (BIT(3))
#define GMAC_CH1_TJT_M  (GMAC_CH1_TJT_V << GMAC_CH1_TJT_S)
#define GMAC_CH1_TJT_V  0x00000001U
#define GMAC_CH1_TJT_S  3
/** GMAC_CH1_OVF : R/W; bitpos: [4]; default: 0;
 *  Receive Overflow This bit indicates that the Receive Buffer had an Overflow during
 *  frame reception If the partial frame is transferred to the application, the
 *  overflow status is set in RDES0[11]
 */
#define GMAC_CH1_OVF    (BIT(4))
#define GMAC_CH1_OVF_M  (GMAC_CH1_OVF_V << GMAC_CH1_OVF_S)
#define GMAC_CH1_OVF_V  0x00000001U
#define GMAC_CH1_OVF_S  4
/** GMAC_CH1_UNF : R/W; bitpos: [5]; default: 0;
 *  Transmit Underflow This bit indicates that the Transmit Buffer had an Underflow
 *  during frame transmission Transmission is suspended and an Underflow Error TDES0[1]
 *  is set
 */
#define GMAC_CH1_UNF    (BIT(5))
#define GMAC_CH1_UNF_M  (GMAC_CH1_UNF_V << GMAC_CH1_UNF_S)
#define GMAC_CH1_UNF_V  0x00000001U
#define GMAC_CH1_UNF_S  5
/** GMAC_CH1_RI : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt This bit indicates that the frame reception is complete When
 *  reception is complete, the Bit 31 of RDES1 _Disable Interrupt on Completion_ is
 *  reset in the last Descriptor, and the specific frame status information is updated
 *  in the descriptor The reception remains in the Running state
 */
#define GMAC_CH1_RI    (BIT(6))
#define GMAC_CH1_RI_M  (GMAC_CH1_RI_V << GMAC_CH1_RI_S)
#define GMAC_CH1_RI_V  0x00000001U
#define GMAC_CH1_RI_S  6
/** GMAC_CH1_RU : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Receive List and the DMA cannot acquire it The Receive Process is
 *  suspended To resume processing Receive descriptors, the host should change the
 *  ownership of the descriptor and issue a Receive Poll Demand command If no Receive
 *  Poll Demand is issued, the Receive Process resumes when the next recognized
 *  incoming frame is received This bit is set only when the previous Receive
 *  Descriptor is owned by the DMA
 */
#define GMAC_CH1_RU    (BIT(7))
#define GMAC_CH1_RU_M  (GMAC_CH1_RU_V << GMAC_CH1_RU_S)
#define GMAC_CH1_RU_V  0x00000001U
#define GMAC_CH1_RU_S  7
/** GMAC_CH1_RPS : R/W; bitpos: [8]; default: 0;
 *  Receive Process Stopped This bit is asserted when the Receive Process enters the
 *  Stopped state
 */
#define GMAC_CH1_RPS    (BIT(8))
#define GMAC_CH1_RPS_M  (GMAC_CH1_RPS_V << GMAC_CH1_RPS_S)
#define GMAC_CH1_RPS_V  0x00000001U
#define GMAC_CH1_RPS_S  8
/** GMAC_CH1_RWT : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout When set, this bit indicates that the Receive Watchdog
 *  Timer expired while receiving the current frame and the current frame is truncated
 *  after the watchdog timeout
 */
#define GMAC_CH1_RWT    (BIT(9))
#define GMAC_CH1_RWT_M  (GMAC_CH1_RWT_V << GMAC_CH1_RWT_S)
#define GMAC_CH1_RWT_V  0x00000001U
#define GMAC_CH1_RWT_S  9
/** GMAC_CH1_ETI : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt This bit indicates that the frame to be transmitted is
 *  fully transferred to the MTL Transmit FIFO
 */
#define GMAC_CH1_ETI    (BIT(10))
#define GMAC_CH1_ETI_M  (GMAC_CH1_ETI_V << GMAC_CH1_ETI_S)
#define GMAC_CH1_ETI_V  0x00000001U
#define GMAC_CH1_ETI_S  10
/** GMAC_CH1_FBI : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Interrupt This bit indicates that a bus error occurred, as
 *  described in Bits [25:23] When this bit is set, the corresponding DMA engine
 *  disables all of its bus accesses 12:11		Reserved	00	RO
 */
#define GMAC_CH1_FBI    (BIT(13))
#define GMAC_CH1_FBI_M  (GMAC_CH1_FBI_V << GMAC_CH1_FBI_S)
#define GMAC_CH1_FBI_V  0x00000001U
#define GMAC_CH1_FBI_S  13
/** GMAC_CH1_ERI : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt This bit indicates that the DMA filled the first data
 *  buffer of the packet This bit is cleared when the software writes 1 to this bit or
 *  Bit 6 _RI_ of this register is set _whichever occurs earlier_
 */
#define GMAC_CH1_ERI    (BIT(14))
#define GMAC_CH1_ERI_M  (GMAC_CH1_ERI_V << GMAC_CH1_ERI_S)
#define GMAC_CH1_ERI_V  0x00000001U
#define GMAC_CH1_ERI_S  14
/** GMAC_CH1_AIS : R/W; bitpos: [15]; default: 0;
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
#define GMAC_CH1_AIS    (BIT(15))
#define GMAC_CH1_AIS_M  (GMAC_CH1_AIS_V << GMAC_CH1_AIS_S)
#define GMAC_CH1_AIS_V  0x00000001U
#define GMAC_CH1_AIS_S  15
/** GMAC_CH1_NIS : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Normal Interrupt Summary bit value is the logical OR of
 *  the following bits when the corresponding interrupt bits are enabled in Register 7
 *  _Interrupt Enable Register_: Register 5[0]: Transmit Interrupt Register 5[2]:
 *  Transmit Buffer Unavailable Register 5[6]: Receive Interrupt Register 5[14]: Early
 *  Receive Interrupt Only unmasked bits _interrupts for which interrupt enable is set
 *  in Register 7_ affect the Normal Interrupt Summary bit This is a sticky bit and
 *  must be cleared _by writing 1 to this bit_ each time a corresponding bit, which
 *  causes NIS to be set, is cleared
 */
#define GMAC_CH1_NIS    (BIT(16))
#define GMAC_CH1_NIS_M  (GMAC_CH1_NIS_V << GMAC_CH1_NIS_S)
#define GMAC_CH1_NIS_V  0x00000001U
#define GMAC_CH1_NIS_S  16
/** GMAC_CH1_RS : RO; bitpos: [19:17]; default: 0;
 *  Receive Process State This field indicates the Receive DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Receive Command
 *  issued 3’b001: Running: Fetching Receive Transfer Descriptor 3’b010: Reserved for
 *  future use 3’b011: Running: Waiting for receive packet 3’b100: Suspended: Receive
 *  Descriptor Unavailable 3’b101: Running: Closing Receive Descriptor 3’b110:
 *  TIME_STAMP write state 3’b111: Running: Transferring the receive packet data from
 *  receive buffer to host memory
 */
#define GMAC_CH1_RS    0x00000007U
#define GMAC_CH1_RS_M  (GMAC_CH1_RS_V << GMAC_CH1_RS_S)
#define GMAC_CH1_RS_V  0x00000007U
#define GMAC_CH1_RS_S  17
/** GMAC_CH1_TS : RO; bitpos: [22:20]; default: 0;
 *  Transmit Process State This field indicates the Transmit DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Transmit Command
 *  issued 3’b001: Running: Fetching Transmit Transfer Descriptor 3’b010: Running:
 *  Waiting for status 3’b011: Running: Reading Data from host memory buffer and
 *  queuing it to transmit buffer _Tx FIFO_ 3’b100: TIME_STAMP write state 3’b101:
 *  Reserved for future use 3’b110: Suspended: Transmit Descriptor Unavailable or
 *  Transmit Buffer Underflow 3’b111: Running: Closing Transmit Descriptor
 */
#define GMAC_CH1_TS    0x00000007U
#define GMAC_CH1_TS_M  (GMAC_CH1_TS_V << GMAC_CH1_TS_S)
#define GMAC_CH1_TS_V  0x00000007U
#define GMAC_CH1_TS_S  20
/** GMAC_CH1_EB : RO; bitpos: [25:23]; default: 0;
 *  Error Bits This field indicates the type of error that caused a Bus Error, for
 *  example, error response on the AHB or AXI interface This field is valid only when
 *  Bit 13 _FBI_ is set This field does not generate an interrupt 0 0 0: Error during
 *  Rx DMA Write Data Transfer 0 1 1: Error during Tx DMA Read Data Transfer 1 0 0:
 *  Error during Rx DMA Descriptor Write Access 1 0 1: Error during Tx DMA Descriptor
 *  Write Access 1 1 0: Error during Rx DMA Descriptor Read Access 1 1 1: Error during
 *  Tx DMA Descriptor Read Access Note: 001 and 010 are reserved
 */
#define GMAC_CH1_EB    0x00000007U
#define GMAC_CH1_EB_M  (GMAC_CH1_EB_V << GMAC_CH1_EB_S)
#define GMAC_CH1_EB_V  0x00000007U
#define GMAC_CH1_EB_S  23
/** GMAC_CH1_GLI : RO; bitpos: [26]; default: 0;
 *  GMAC Line Interface Interrupt When set, this bit reflects any of the following
 *  interrupt events in the DWC_gmac interfaces _if present and enabled in your
 *  configuration_: PCS _TBI, RTBI, or SGMII_: Link change or autonegotiation complete
 *  event SMII or RGMII: Link change event General Purpose Input Status _GPIS_: Any LL
 *  or LH event on the gpi_i input ports To identify the exact cause of the interrupt,
 *  the software must first read Bit 11 and Bits[2:0] of Register 14 _Interrupt Status
 *  Register_ and then to clear the source of interrupt _which also clears the GLI
 *  interrupt_, read any of the following corresponding registers: PCS _TBI, RTBI, or
 *  SGMII_: Register 49 _AN Status Register_ SMII or RGMII: Register 54
 *  _SGMII/RGMII/SMII Control and Status Register_ General Purpose Input _GPI_:
 *  Register 56 _General Purpose IO Register_ The interrupt signal from the DWC_gmac
 *  subsystem _sbd_intr_o_ is high when this bit is high
 */
#define GMAC_CH1_GLI    (BIT(26))
#define GMAC_CH1_GLI_M  (GMAC_CH1_GLI_V << GMAC_CH1_GLI_S)
#define GMAC_CH1_GLI_V  0x00000001U
#define GMAC_CH1_GLI_S  26
/** GMAC_CH1_GMI : RO; bitpos: [27]; default: 0;
 *  GMAC MMC Interrupt This bit reflects an interrupt event in the MMC module of the
 *  DWC_gmac The software must read the corresponding registers in the DWC_gmac to get
 *  the exact cause of the interrupt and clear the source of interrupt to make this bit
 *  as 1’b0 The interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when
 *  this bit is high This bit is applicable only when the MAC Management Counters _MMC_
 *  are enabled Otherwise, this bit is reserved
 */
#define GMAC_CH1_GMI    (BIT(27))
#define GMAC_CH1_GMI_M  (GMAC_CH1_GMI_V << GMAC_CH1_GMI_S)
#define GMAC_CH1_GMI_V  0x00000001U
#define GMAC_CH1_GMI_S  27
/** GMAC_CH1_GPI : RO; bitpos: [28]; default: 0;
 *  GMAC PMT Interrupt This bit indicates an interrupt event in the PMT module of the
 *  DWC_gmac The software must read the PMT Control and Status Register in the MAC to
 *  get the exact cause of interrupt and clear its source to reset this bit to 1’b0 The
 *  interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when this bit is
 *  high This bit is applicable only when the Power Management feature is enabled
 *  Otherwise, this bit is reserved Note: The GPI and pmt_intr_o interrupts are
 *  generated in different clock domains
 */
#define GMAC_CH1_GPI    (BIT(28))
#define GMAC_CH1_GPI_M  (GMAC_CH1_GPI_V << GMAC_CH1_GPI_S)
#define GMAC_CH1_GPI_V  0x00000001U
#define GMAC_CH1_GPI_S  28
/** GMAC_CH1_TTI : RO; bitpos: [29]; default: 0;
 *  Timestamp Trigger Interrupt This bit indicates an interrupt event in the Timestamp
 *  Generator block of the DWC_gmac The software must read the corresponding registers
 *  in the DWC_gmac to get the exact cause of the interrupt and clear its source to
 *  reset this bit to 1'b0 When this bit is high, the interrupt signal from the
 *  DWC_gmac subsystem _sbd_intr_o_ is high This bit is applicable only when the IEEE
 *  1588 Timestamp feature is enabled Otherwise, this bit is reserved
 */
#define GMAC_CH1_TTI    (BIT(29))
#define GMAC_CH1_TTI_M  (GMAC_CH1_TTI_V << GMAC_CH1_TTI_S)
#define GMAC_CH1_TTI_V  0x00000001U
#define GMAC_CH1_TTI_S  29
/** GMAC_CH1_GLPII_GTMSI : RO; bitpos: [30]; default: 0;
 *  GTMSI: GMAC TMS Interrupt _for Channel 1 and Channel 2_ This bit indicates an
 *  interrupt event in the traffic manager and scheduler logic of DWC_gmac To reset
 *  this bit, the software must read the corresponding registers _Channel Status
 *  Register_ to get the exact cause of the interrupt and clear its source Note: GTMSI
 *  status is given only in Channel 1 and Channel 2 DMA register when the AV feature is
 *  enabled and corresponding additional transmit channels are present Otherwise, this
 *  bit is reserved When this bit is high, the interrupt signal from the MAC
 *  _sbd_intr_o_ is high
 */
#define GMAC_CH1_GLPII_GTMSI    (BIT(30))
#define GMAC_CH1_GLPII_GTMSI_M  (GMAC_CH1_GLPII_GTMSI_V << GMAC_CH1_GLPII_GTMSI_S)
#define GMAC_CH1_GLPII_GTMSI_V  0x00000001U
#define GMAC_CH1_GLPII_GTMSI_S  30

/** GMAC_REGISTER70_CHANNEL1OPERATIONMODEREGISTER_REG register
 *  Establishes the Receive and Transmit operating modes and command
 */
#define GMAC_REGISTER70_CHANNEL1OPERATIONMODEREGISTER_REG (DR_REG_GMAC_BASE + 0x1118)
/** GMAC_CH1_SR : R/W; bitpos: [1]; default: 0;
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
#define GMAC_CH1_SR    (BIT(1))
#define GMAC_CH1_SR_M  (GMAC_CH1_SR_V << GMAC_CH1_SR_S)
#define GMAC_CH1_SR_V  0x00000001U
#define GMAC_CH1_SR_S  1
/** GMAC_CH1_OSF : R/W; bitpos: [2]; default: 0;
 *  Operate on Second Frame When this bit is set, it instructs the DMA to process the
 *  second frame of the Transmit data even before the status for the first frame is
 *  obtained
 */
#define GMAC_CH1_OSF    (BIT(2))
#define GMAC_CH1_OSF_M  (GMAC_CH1_OSF_V << GMAC_CH1_OSF_S)
#define GMAC_CH1_OSF_V  0x00000001U
#define GMAC_CH1_OSF_S  2
/** GMAC_CH1_RTC : R/W; bitpos: [4:3]; default: 0;
 *  Receive Threshold Control These two bits control the threshold level of the MTL
 *  Receive FIFO Transfer _request_ to DMA starts when the frame size within the MTL
 *  Receive FIFO is larger than the threshold In addition, full frames with length less
 *  than the threshold are automatically transferred The value of 11 is not applicable
 *  if the configured Receive FIFO size is 128 bytes These bits are valid only when the
 *  RSF bit is zero, and are ignored when the RSF bit is set to 1 00: 64 01: 32 10: 96
 *  11: 128
 */
#define GMAC_CH1_RTC    0x00000003U
#define GMAC_CH1_RTC_M  (GMAC_CH1_RTC_V << GMAC_CH1_RTC_S)
#define GMAC_CH1_RTC_V  0x00000003U
#define GMAC_CH1_RTC_S  3
/** GMAC_CH1_DGF : R/W; bitpos: [5]; default: 0;
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
#define GMAC_CH1_DGF    (BIT(5))
#define GMAC_CH1_DGF_M  (GMAC_CH1_DGF_V << GMAC_CH1_DGF_S)
#define GMAC_CH1_DGF_V  0x00000001U
#define GMAC_CH1_DGF_S  5
/** GMAC_CH1_FUF : R/W; bitpos: [6]; default: 0;
 *  Forward Undersized Good Frames When set, the Rx FIFO forwards Undersized frames
 *  _that is, frames with no Error and length less than 64 bytes_ including padbytes
 *  and CRC When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a
 *  frame is already transferred because of the lower value of Receive Threshold, for
 *  example, RTC = 01
 */
#define GMAC_CH1_FUF    (BIT(6))
#define GMAC_CH1_FUF_M  (GMAC_CH1_FUF_V << GMAC_CH1_FUF_S)
#define GMAC_CH1_FUF_V  0x00000001U
#define GMAC_CH1_FUF_S  6
/** GMAC_CH1_FEF : R/W; bitpos: [7]; default: 0;
 *  Forward Error Frames When this bit is reset, the Rx FIFO drops frames with error
 *  status _CRC error, collision error, GMII_ER, giant frame, watchdog timeout, or
 *  overflow_ However, if the start byte _write_ pointer of a frame is already
 *  transferred to the read controller side _in Threshold mode_, then the frame is not
 *  dropped In the GMACMTL configuration in which the Frame Length FIFO is also enabled
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
 *  features: 	L3L4 filter in GMACCORE or GMACMTL configurations 	Full checksum offload
 *  engine _Type 2_ with enhanced descriptor format in the GMACDMA, GMACAHB, or GMACAXI
 *  configurations
 */
#define GMAC_CH1_FEF    (BIT(7))
#define GMAC_CH1_FEF_M  (GMAC_CH1_FEF_V << GMAC_CH1_FEF_S)
#define GMAC_CH1_FEF_V  0x00000001U
#define GMAC_CH1_FEF_S  7
/** GMAC_CH1_EFC : R/W; bitpos: [8]; default: 0;
 *  Enable HW Flow Control When this bit is set, the flow control signal operation
 *  based on the filllevel of Rx FIFO is enabled When reset, the flow control operation
 *  is disabled This bit is not used _reserved and always reset_ when the Rx FIFO is
 *  less than 4 KB
 */
#define GMAC_CH1_EFC    (BIT(8))
#define GMAC_CH1_EFC_M  (GMAC_CH1_EFC_V << GMAC_CH1_EFC_S)
#define GMAC_CH1_EFC_V  0x00000001U
#define GMAC_CH1_EFC_S  8
/** GMAC_CH1_RFA : R/W; bitpos: [10:9]; default: 0;
 *  Threshold for Activating Flow Control _in halfduplex and fullduplex modes_ These
 *  bits control the threshold _Fill level of Rx FIFO_ at which the flow control is
 *  activated 00: Full minus 1 KB, that is, FULL—1KB 01: Full minus 2 KB, that is,
 *  FULL—2KB 10: Full minus 3 KB, that is, FULL—3KB 11: Full minus 4 KB, that is,
 *  FULL—4KB These values are applicable only to Rx FIFOs of 4 KB or more and when Bit
 *  8 _EFC_ is set high If the Rx FIFO is 8 KB or more, an additional Bit _RFA_2_ is
 *  used for more threshold levels as described in Bit 23 These bits are reserved and
 *  readonly when the depth of Rx FIFO is less than 4 KB Note: When FIFO size is
 *  exactly 4 KB, although the DWC_gmac allows you to program the value of these bits
 *  to 11, the software should not program these bits to 2'b11 The value 2'b11 means
 *  flow control on FIFO empty condition
 */
#define GMAC_CH1_RFA    0x00000003U
#define GMAC_CH1_RFA_M  (GMAC_CH1_RFA_V << GMAC_CH1_RFA_S)
#define GMAC_CH1_RFA_V  0x00000003U
#define GMAC_CH1_RFA_S  9
/** GMAC_CH1_RFD : R/W; bitpos: [12:11]; default: 0;
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
#define GMAC_CH1_RFD    0x00000003U
#define GMAC_CH1_RFD_M  (GMAC_CH1_RFD_V << GMAC_CH1_RFD_S)
#define GMAC_CH1_RFD_V  0x00000003U
#define GMAC_CH1_RFD_S  11
/** GMAC_CH1_ST : R/W; bitpos: [13]; default: 0;
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
#define GMAC_CH1_ST    (BIT(13))
#define GMAC_CH1_ST_M  (GMAC_CH1_ST_V << GMAC_CH1_ST_S)
#define GMAC_CH1_ST_V  0x00000001U
#define GMAC_CH1_ST_S  13
/** GMAC_CH1_TTC : R/W; bitpos: [16:14]; default: 0;
 *  Transmit Threshold Control These bits control the threshold level of the MTL
 *  Transmit FIFO Transmission starts when the frame size within the MTL Transmit FIFO
 *  is larger than the threshold In addition, full frames with a length less than the
 *  threshold are also transmitted These bits are used only when Bit 21 _TSF_ is reset
 *  000: 64 001: 128 010: 192 011: 256 100: 40 101: 32 110: 24 111: 16
 */
#define GMAC_CH1_TTC    0x00000007U
#define GMAC_CH1_TTC_M  (GMAC_CH1_TTC_V << GMAC_CH1_TTC_S)
#define GMAC_CH1_TTC_V  0x00000007U
#define GMAC_CH1_TTC_S  14
/** GMAC_CH1_FTF : R/W1S; bitpos: [20]; default: 0;
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
#define GMAC_CH1_FTF    (BIT(20))
#define GMAC_CH1_FTF_M  (GMAC_CH1_FTF_V << GMAC_CH1_FTF_S)
#define GMAC_CH1_FTF_V  0x00000001U
#define GMAC_CH1_FTF_S  20
/** GMAC_CH1_TSF : R/W; bitpos: [21]; default: 0;
 *  Transmit Store and Forward When this bit is set, transmission starts when a full
 *  frame resides in the MTL Transmit FIFO When this bit is set, the TTC values
 *  specified in Bits [16:14] are ignored This bit should be changed only when the
 *  transmission is stopped
 */
#define GMAC_CH1_TSF    (BIT(21))
#define GMAC_CH1_TSF_M  (GMAC_CH1_TSF_V << GMAC_CH1_TSF_S)
#define GMAC_CH1_TSF_V  0x00000001U
#define GMAC_CH1_TSF_S  21
/** GMAC_CH1_RFD_2 : R/W; bitpos: [22]; default: 0;
 *  MSB of Threshold for Deactivating Flow Control If the DWC_gmac is configured for Rx
 *  FIFO size of 8 KB or more, this bit _when set_ provides additional threshold levels
 *  for deactivating the flow control in both halfduplex and fullduplex modes This bit
 *  _as Most Significant Bit_ along with the RFD _Bits [12:11]_ gives the following
 *  thresholds for deactivating flow control: 100: Full minus 5 KB, that is, FULL — 5
 *  KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that is, FULL —
 *  7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB or less deep
 */
#define GMAC_CH1_RFD_2    (BIT(22))
#define GMAC_CH1_RFD_2_M  (GMAC_CH1_RFD_2_V << GMAC_CH1_RFD_2_S)
#define GMAC_CH1_RFD_2_V  0x00000001U
#define GMAC_CH1_RFD_2_S  22
/** GMAC_CH1_RFA_2 : R/W; bitpos: [23]; default: 0;
 *  MSB of Threshold for Activating Flow Control If the DWC_gmac is configured for an
 *  Rx FIFO size of 8 KB or more, this bit _when set_ provides additional threshold
 *  levels for activating the flow control in both half duplex and fullduplex modes
 *  This bit _as Most Significant Bit_, along with the RFA _Bits [10:9]_, gives the
 *  following thresholds for activating flow control: 100: Full minus 5 KB, that is,
 *  FULL — 5 KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that
 *  is, FULL — 7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB
 *  or less deep
 */
#define GMAC_CH1_RFA_2    (BIT(23))
#define GMAC_CH1_RFA_2_M  (GMAC_CH1_RFA_2_V << GMAC_CH1_RFA_2_S)
#define GMAC_CH1_RFA_2_V  0x00000001U
#define GMAC_CH1_RFA_2_S  23
/** GMAC_CH1_DFF : R/W; bitpos: [24]; default: 0;
 *  Disable Flushing of Received Frames When this bit is set, the Rx DMA does not flush
 *  any frames because of the unavailability of receive descriptors or buffers as it
 *  does normally when this bit is reset _See “Receive Process Suspended” on page 83_
 *  This bit is reserved _and RO_ in the GMACMTL configuration
 */
#define GMAC_CH1_DFF    (BIT(24))
#define GMAC_CH1_DFF_M  (GMAC_CH1_DFF_V << GMAC_CH1_DFF_S)
#define GMAC_CH1_DFF_V  0x00000001U
#define GMAC_CH1_DFF_S  24
/** GMAC_CH1_RSF : R/W; bitpos: [25]; default: 0;
 *  Receive Store and Forward When this bit is set, the MTL reads a frame from the Rx
 *  FIFO only after the complete frame has been written to it, ignoring the RTC bits
 *  When this bit is reset, the Rx FIFO operates in the cutthrough mode, subject to the
 *  threshold specified by the RTC bits
 */
#define GMAC_CH1_RSF    (BIT(25))
#define GMAC_CH1_RSF_M  (GMAC_CH1_RSF_V << GMAC_CH1_RSF_S)
#define GMAC_CH1_RSF_V  0x00000001U
#define GMAC_CH1_RSF_S  25
/** GMAC_CH1_DT : R/W; bitpos: [26]; default: 0;
 *  Disable Dropping of TCP/IP Checksum Error Frames When this bit is set, the MAC does
 *  not drop the frames which only have errors detected by the Receive Checksum Offload
 *  engine Such frames do not have any errors _including FCS error_ in the Ethernet
 *  frame received by the MAC but have errors only in the encapsulated payload When
 *  this bit is reset, all error frames are dropped if the FEF bit is reset If the IPC
 *  Full Checksum Offload Engine _Type 2_ is disabled, this bit is reserved _RO with
 *  value 1'b0_
 */
#define GMAC_CH1_DT    (BIT(26))
#define GMAC_CH1_DT_M  (GMAC_CH1_DT_V << GMAC_CH1_DT_S)
#define GMAC_CH1_DT_V  0x00000001U
#define GMAC_CH1_DT_S  26

/** GMAC_REGISTER71_CHANNEL1INTERRUPTENABLEREGISTER_REG register
 *  Enables the interrupts reported by the Status Register
 */
#define GMAC_REGISTER71_CHANNEL1INTERRUPTENABLEREGISTER_REG (DR_REG_GMAC_BASE + 0x111c)
/** GMAC_CH1_TIE : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Transmit Interrupt is enabled When this bit is reset, the Transmit
 *  Interrupt is disabled The sbd_intr_o interrupt is generated as shown in Figure 61
 *  It is asserted only when the TTI, GPI, GMI, GLI, or GLPII bit of the DMA Status
 *  register is asserted, or when the NIS or AIS Status bit is asserted and the
 *  corresponding Interrupt Enable bits _NIE or AIE_ are enabled
 */
#define GMAC_CH1_TIE    (BIT(0))
#define GMAC_CH1_TIE_M  (GMAC_CH1_TIE_V << GMAC_CH1_TIE_S)
#define GMAC_CH1_TIE_V  0x00000001U
#define GMAC_CH1_TIE_S  0
/** GMAC_CH1_TSE : R/W; bitpos: [1]; default: 0;
 *  Transmit Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Transmission Stopped Interrupt is enabled When this bit is reset, the
 *  Transmission Stopped Interrupt is disabled
 */
#define GMAC_CH1_TSE    (BIT(1))
#define GMAC_CH1_TSE_M  (GMAC_CH1_TSE_V << GMAC_CH1_TSE_S)
#define GMAC_CH1_TSE_V  0x00000001U
#define GMAC_CH1_TSE_S  1
/** GMAC_CH1_TUE : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable Enable When this bit is set with Normal Interrupt
 *  Summary Enable _Bit 16_, the Transmit Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled
 */
#define GMAC_CH1_TUE    (BIT(2))
#define GMAC_CH1_TUE_M  (GMAC_CH1_TUE_V << GMAC_CH1_TUE_S)
#define GMAC_CH1_TUE_V  0x00000001U
#define GMAC_CH1_TUE_S  2
/** GMAC_CH1_TJE : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Jabber Timeout Interrupt is enabled When this bit is
 *  reset, the Transmit Jabber Timeout Interrupt is disabled
 */
#define GMAC_CH1_TJE    (BIT(3))
#define GMAC_CH1_TJE_M  (GMAC_CH1_TJE_V << GMAC_CH1_TJE_S)
#define GMAC_CH1_TJE_V  0x00000001U
#define GMAC_CH1_TJE_S  3
/** GMAC_CH1_OVE : R/W; bitpos: [4]; default: 0;
 *  Overflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Receive Overflow Interrupt is enabled When this bit is reset,
 *  the Overflow Interrupt is disabled
 */
#define GMAC_CH1_OVE    (BIT(4))
#define GMAC_CH1_OVE_M  (GMAC_CH1_OVE_V << GMAC_CH1_OVE_S)
#define GMAC_CH1_OVE_V  0x00000001U
#define GMAC_CH1_OVE_S  4
/** GMAC_CH1_UNE : R/W; bitpos: [5]; default: 0;
 *  Underflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Underflow Interrupt is enabled When this bit is
 *  reset, the Underflow Interrupt is disabled
 */
#define GMAC_CH1_UNE    (BIT(5))
#define GMAC_CH1_UNE_M  (GMAC_CH1_UNE_V << GMAC_CH1_UNE_S)
#define GMAC_CH1_UNE_V  0x00000001U
#define GMAC_CH1_UNE_S  5
/** GMAC_CH1_RIE : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Receive Interrupt is enabled When this bit is reset, the Receive
 *  Interrupt is disabled
 */
#define GMAC_CH1_RIE    (BIT(6))
#define GMAC_CH1_RIE_M  (GMAC_CH1_RIE_V << GMAC_CH1_RIE_S)
#define GMAC_CH1_RIE_V  0x00000001U
#define GMAC_CH1_RIE_S  6
/** GMAC_CH1_RUE : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Receive Buffer Unavailable Interrupt is disabled
 */
#define GMAC_CH1_RUE    (BIT(7))
#define GMAC_CH1_RUE_M  (GMAC_CH1_RUE_V << GMAC_CH1_RUE_S)
#define GMAC_CH1_RUE_V  0x00000001U
#define GMAC_CH1_RUE_S  7
/** GMAC_CH1_RSE : R/W; bitpos: [8]; default: 0;
 *  Receive Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Receive Stopped Interrupt is enabled When this bit is reset, the
 *  Receive Stopped Interrupt is disabled
 */
#define GMAC_CH1_RSE    (BIT(8))
#define GMAC_CH1_RSE_M  (GMAC_CH1_RSE_V << GMAC_CH1_RSE_S)
#define GMAC_CH1_RSE_V  0x00000001U
#define GMAC_CH1_RSE_S  8
/** GMAC_CH1_RWE : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Watchdog Timeout Interrupt is enabled When
 *  this bit is reset, the Receive Watchdog Timeout Interrupt is disabled
 */
#define GMAC_CH1_RWE    (BIT(9))
#define GMAC_CH1_RWE_M  (GMAC_CH1_RWE_V << GMAC_CH1_RWE_S)
#define GMAC_CH1_RWE_V  0x00000001U
#define GMAC_CH1_RWE_S  9
/** GMAC_CH1_ETE : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt Enable When this bit is set with an Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Early Transmit Interrupt is enabled When this bit is
 *  reset, the Early Transmit Interrupt is disabled
 */
#define GMAC_CH1_ETE    (BIT(10))
#define GMAC_CH1_ETE_M  (GMAC_CH1_ETE_V << GMAC_CH1_ETE_S)
#define GMAC_CH1_ETE_V  0x00000001U
#define GMAC_CH1_ETE_S  10
/** GMAC_CH1_FBE : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Fatal Bus Error Interrupt is enabled When this bit is reset, the
 *  Fatal Bus Error Enable Interrupt is disabled 12:11		Reserved	00	RO
 */
#define GMAC_CH1_FBE    (BIT(13))
#define GMAC_CH1_FBE_M  (GMAC_CH1_FBE_V << GMAC_CH1_FBE_S)
#define GMAC_CH1_FBE_V  0x00000001U
#define GMAC_CH1_FBE_S  13
/** GMAC_CH1_ERE : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt Enable When this bit is set with Normal Interrupt Summary
 *  Enable _Bit 16_, the Early Receive Interrupt is enabled When this bit is reset, the
 *  Early Receive Interrupt is disabled
 */
#define GMAC_CH1_ERE    (BIT(14))
#define GMAC_CH1_ERE_M  (GMAC_CH1_ERE_V << GMAC_CH1_ERE_S)
#define GMAC_CH1_ERE_V  0x00000001U
#define GMAC_CH1_ERE_S  14
/** GMAC_CH1_AIE : R/W; bitpos: [15]; default: 0;
 *  Abnormal Interrupt Summary Enable When this bit is set, abnormal interrupt summary
 *  is enabled When this bit is reset, the abnormal interrupt summary is disabled This
 *  bit enables the following interrupts in Register 5 _Status Register_: Register
 *  5[1]: Transmit Process Stopped Register 5[3]: Transmit Jabber Timeout Register
 *  5[4]: Receive Overflow Register 5[5]: Transmit Underflow Register 5[7]: Receive
 *  Buffer Unavailable Register 5[8]: Receive Process Stopped Register 5[9]: Receive
 *  Watchdog Timeout Register 5[10]: Early Transmit Interrupt Register 5[13]: Fatal Bus
 *  Error
 */
#define GMAC_CH1_AIE    (BIT(15))
#define GMAC_CH1_AIE_M  (GMAC_CH1_AIE_V << GMAC_CH1_AIE_S)
#define GMAC_CH1_AIE_V  0x00000001U
#define GMAC_CH1_AIE_S  15
/** GMAC_CH1_NIE : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Enable When this bit is set, normal interrupt summary is
 *  enabled When this bit is reset, normal interrupt summary is disabled This bit
 *  enables the following interrupts in Register 5 _Status Register_: Register 5[0]:
 *  Transmit Interrupt Register 5[2]: Transmit Buffer Unavailable Register 5[6]:
 *  Receive Interrupt Register 5[14]: Early Receive Interrupt
 */
#define GMAC_CH1_NIE    (BIT(16))
#define GMAC_CH1_NIE_M  (GMAC_CH1_NIE_V << GMAC_CH1_NIE_S)
#define GMAC_CH1_NIE_V  0x00000001U
#define GMAC_CH1_NIE_S  16

/** GMAC_REGISTER72_CHANNEL1MISSEDFRAMEANDBUFFEROVERFLOWCOUNTERREGISTER_REG register
 *  Contains the counters for discarded frames because no host Receive Descriptor was
 *  available, and discarded frames because of Receive FIFO Overflow
 */
#define GMAC_REGISTER72_CHANNEL1MISSEDFRAMEANDBUFFEROVERFLOWCOUNTERREGISTER_REG (DR_REG_GMAC_BASE + 0x1120)
/** GMAC_CH1_MISFRMCNT : R/W; bitpos: [15:0]; default: 0;
 *  Missed Frame Counter This field indicates the number of frames missed by the
 *  controller because of the Host Receive Buffer being unavailable This counter is
 *  incremented each time the DMA discards an incoming frame The counter is cleared
 *  when this register is read with mci_be_i[0] at 1’b1
 */
#define GMAC_CH1_MISFRMCNT    0x0000FFFFU
#define GMAC_CH1_MISFRMCNT_M  (GMAC_CH1_MISFRMCNT_V << GMAC_CH1_MISFRMCNT_S)
#define GMAC_CH1_MISFRMCNT_V  0x0000FFFFU
#define GMAC_CH1_MISFRMCNT_S  0
/** GMAC_CH1_MISCNTOVF : R/W; bitpos: [16]; default: 0;
 *  Overflow Bit for Missed Frame Counter This bit is set every time Missed Frame
 *  Counter _Bits[15:0]_ overflows, that is, the DMA discards an incoming frame because
 *  of the Host Receive Buffer being unavailable with the missed frame counter at
 *  maximum value In such a scenario, the Missed frame counter is reset to allzeros and
 *  this bit indicates that the rollover happened
 */
#define GMAC_CH1_MISCNTOVF    (BIT(16))
#define GMAC_CH1_MISCNTOVF_M  (GMAC_CH1_MISCNTOVF_V << GMAC_CH1_MISCNTOVF_S)
#define GMAC_CH1_MISCNTOVF_V  0x00000001U
#define GMAC_CH1_MISCNTOVF_S  16
/** GMAC_CH1_OVFFRMCNT : R/W; bitpos: [27:17]; default: 0;
 *  Overflow Frame Counter This field indicates the number of frames missed by the
 *  application This counter is incremented each time the MTL FIFO overflows The
 *  counter is cleared when this register is read with mci_be_i[2] at 1’b1
 */
#define GMAC_CH1_OVFFRMCNT    0x000007FFU
#define GMAC_CH1_OVFFRMCNT_M  (GMAC_CH1_OVFFRMCNT_V << GMAC_CH1_OVFFRMCNT_S)
#define GMAC_CH1_OVFFRMCNT_V  0x000007FFU
#define GMAC_CH1_OVFFRMCNT_S  17
/** GMAC_CH1_OVFCNTOVF : R/W; bitpos: [28]; default: 0;
 *  Overflow Bit for FIFO Overflow Counter This bit is set every time the Overflow
 *  Frame Counter _Bits[27:17]_ overflows, that is, the Rx FIFO overflows with the
 *  overflow frame counter at maximum value In such a scenario, the overflow frame
 *  counter is reset to allzeros and this bit indicates that the rollover happened
 */
#define GMAC_CH1_OVFCNTOVF    (BIT(28))
#define GMAC_CH1_OVFCNTOVF_M  (GMAC_CH1_OVFCNTOVF_V << GMAC_CH1_OVFCNTOVF_S)
#define GMAC_CH1_OVFCNTOVF_V  0x00000001U
#define GMAC_CH1_OVFCNTOVF_S  28

/** GMAC_REGISTER73_CHANNEL1RECEIVEINTERRUPTWATCHDOGTIMERREGISTER_REG register
 *  Watchdog timeout for Receive Interrupt _RI_ from DMA
 */
#define GMAC_REGISTER73_CHANNEL1RECEIVEINTERRUPTWATCHDOGTIMERREGISTER_REG (DR_REG_GMAC_BASE + 0x1124)
/** GMAC_CH1_RIWT : R/W; bitpos: [7:0]; default: 0;
 *  RI Watchdog Timer Count This bit indicates the number of system clock cycles
 *  multiplied by 256 for which the watchdog timer is set The watchdog timer gets
 *  triggered with the programmed value after the Rx DMA completes the transfer of a
 *  frame for which the RI status bit is not set because of the setting in the
 *  corresponding descriptor RDES1[31] When the watchdog timer runs out, the RI bit is
 *  set and the timer is stopped The watchdog timer is reset when the RI bit is set
 *  high because of automatic setting of RI as per RDES1[31] of any received frame
 */
#define GMAC_CH1_RIWT    0x000000FFU
#define GMAC_CH1_RIWT_M  (GMAC_CH1_RIWT_V << GMAC_CH1_RIWT_S)
#define GMAC_CH1_RIWT_V  0x000000FFU
#define GMAC_CH1_RIWT_S  0

/** GMAC_REGISTER76_CHANNEL1SLOTFUNCTIONCONTROLANDSTATUSREGISTER_REG register
 *  Contains the control bits for slot function and its status for Channel 1 transmit
 *  path
 */
#define GMAC_REGISTER76_CHANNEL1SLOTFUNCTIONCONTROLANDSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x1130)
/** GMAC_ESC : R/W; bitpos: [0]; default: 0;
 *  Enable Slot Comparison When set, this bit enables the checking of the slot numbers,
 *  programmed in the transmit descriptor, with the current reference given in Bits
 *  [19:16] The DMA fetches the data from the corresponding buffer only when the slot
 *  number is equal to the reference slot number or is ahead of the reference slot
 *  number by one slot When reset, this bit disables the checking of the slot numbers
 *  The DMA fetches the data immediately after the descriptor is processed
 */
#define GMAC_ESC    (BIT(0))
#define GMAC_ESC_M  (GMAC_ESC_V << GMAC_ESC_S)
#define GMAC_ESC_V  0x00000001U
#define GMAC_ESC_S  0
/** GMAC_ASC : R/W; bitpos: [1]; default: 0;
 *  Advance Slot Check When set, this bit enables the DMA to fetch the data from the
 *  buffer when the slot number _SLOTNUM_ programmed in the transmit descriptor is:
 *  equal to the reference slot number given in Bits [19:16] or ahead of the reference
 *  slot number by up to two slots This bit is applicable only when Bit 0 _ESC_ is set
 */
#define GMAC_ASC    (BIT(1))
#define GMAC_ASC_M  (GMAC_ASC_V << GMAC_ASC_S)
#define GMAC_ASC_V  0x00000001U
#define GMAC_ASC_S  1
/** GMAC_RSN : RO; bitpos: [19:16]; default: 0;
 *  Reference Slot Number This field gives the current value of the reference slot
 *  number in DMA used for comparison checking
 */
#define GMAC_RSN    0x0000000FU
#define GMAC_RSN_M  (GMAC_RSN_V << GMAC_RSN_S)
#define GMAC_RSN_V  0x0000000FU
#define GMAC_RSN_S  16

/** GMAC_REGISTER82_CHANNEL1CURRENTHOSTTRANSMITDESCRIPTORREGISTER_REG register
 *  Points to the start of current Transmit Descriptor read by the DMA
 */
#define GMAC_REGISTER82_CHANNEL1CURRENTHOSTTRANSMITDESCRIPTORREGISTER_REG (DR_REG_GMAC_BASE + 0x1148)
/** GMAC_CH1_CURTDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Descriptor Address Pointer
 */
#define GMAC_CH1_CURTDESAPTR    0xFFFFFFFFU
#define GMAC_CH1_CURTDESAPTR_M  (GMAC_CH1_CURTDESAPTR_V << GMAC_CH1_CURTDESAPTR_S)
#define GMAC_CH1_CURTDESAPTR_V  0xFFFFFFFFU
#define GMAC_CH1_CURTDESAPTR_S  0

/** GMAC_REGISTER83_CHANNEL1CURRENTHOSTRECEIVEDESCRIPTORREGISTER_REG register
 *  Points to the start of current Receive Descriptor read by the DMA
 */
#define GMAC_REGISTER83_CHANNEL1CURRENTHOSTRECEIVEDESCRIPTORREGISTER_REG (DR_REG_GMAC_BASE + 0x114c)
/** GMAC_CH1_CURRDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Descriptor Address Pointer
 */
#define GMAC_CH1_CURRDESAPTR    0xFFFFFFFFU
#define GMAC_CH1_CURRDESAPTR_M  (GMAC_CH1_CURRDESAPTR_V << GMAC_CH1_CURRDESAPTR_S)
#define GMAC_CH1_CURRDESAPTR_V  0xFFFFFFFFU
#define GMAC_CH1_CURRDESAPTR_S  0

/** GMAC_REGISTER84_CHANNEL1CURRENTHOSTTRANSMITBUFFERADDRESSREGISTER_REG register
 *  Points to the current Transmit Buffer address read by the DMA
 */
#define GMAC_REGISTER84_CHANNEL1CURRENTHOSTTRANSMITBUFFERADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x1150)
/** GMAC_CH1_CURTBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Buffer Address Pointer
 */
#define GMAC_CH1_CURTBUFAPTR    0xFFFFFFFFU
#define GMAC_CH1_CURTBUFAPTR_M  (GMAC_CH1_CURTBUFAPTR_V << GMAC_CH1_CURTBUFAPTR_S)
#define GMAC_CH1_CURTBUFAPTR_V  0xFFFFFFFFU
#define GMAC_CH1_CURTBUFAPTR_S  0

/** GMAC_REGISTER85_CHANNEL1CURRENTHOSTRECEIVEBUFFERADDRESSREGISTER_REG register
 *  Points to the current Receive Buffer address read by the DMA
 */
#define GMAC_REGISTER85_CHANNEL1CURRENTHOSTRECEIVEBUFFERADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x1154)
/** GMAC_CH1_CURRBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Buffer Address Pointer
 */
#define GMAC_CH1_CURRBUFAPTR    0xFFFFFFFFU
#define GMAC_CH1_CURRBUFAPTR_M  (GMAC_CH1_CURRBUFAPTR_V << GMAC_CH1_CURRBUFAPTR_S)
#define GMAC_CH1_CURRBUFAPTR_V  0xFFFFFFFFU
#define GMAC_CH1_CURRBUFAPTR_S  0

/** GMAC_REGISTER88_CHANNEL1CBSCONTROLREGISTER_REG register
 *  Controls the Channel 1 credit shaping operation on the transmit path
 */
#define GMAC_REGISTER88_CHANNEL1CBSCONTROLREGISTER_REG (DR_REG_GMAC_BASE + 0x1160)
/** GMAC_CH1_CBSD : R/W; bitpos: [0]; default: 0;
 *  CreditBased Shaper Disable When set, the MAC disables the creditbased shaper
 *  algorithm for Channel 1 traffic and makes the traffic management algorithm to
 *  strict priority for Channel 1 over Channel 0 When reset, the creditbased shaper
 *  algorithm schedules the traffic in Channel 1 for transmission
 */
#define GMAC_CH1_CBSD    (BIT(0))
#define GMAC_CH1_CBSD_M  (GMAC_CH1_CBSD_V << GMAC_CH1_CBSD_S)
#define GMAC_CH1_CBSD_V  0x00000001U
#define GMAC_CH1_CBSD_S  0
/** GMAC_CH1_CC : R/W; bitpos: [1]; default: 0;
 *  Credit Control When reset, the accumulated credit parameter in the creditbased
 *  shaper algorithm logic is set to zero when there is positive credit and no frame to
 *  transmit in Channel 1 When there is no frame waiting in Channel 1 and other channel
 *  is transmitting, no credit is accumulated When set, the accumulated credit
 *  parameter in the creditbased shaper algorithm logic is not reset to zero when there
 *  is positive credit and no frame to transmit in Channel 1 The credit accumulates
 *  even when there is no frame waiting in Channel 1 and another channel is transmitting
 */
#define GMAC_CH1_CC    (BIT(1))
#define GMAC_CH1_CC_M  (GMAC_CH1_CC_V << GMAC_CH1_CC_S)
#define GMAC_CH1_CC_V  0x00000001U
#define GMAC_CH1_CC_S  1
/** GMAC_CH1_SLC : R/W; bitpos: [6:4]; default: 0;
 *  Slot Count The software can program the number of slots _of duration 125 microsec_
 *  over which the average transmitted bits per slot _provided in the CBS Status
 *  register_ need to be computed for Channel 1 when the creditbased shaper algorithm
 *  is enabled The encoding is as follows: 3'b000: 1 Slot 3'b001: 2 Slots 3'b010: 4
 *  Slots 3'b011: 8 Slots 3'b100: 16 Slots 3'b1013'b111: Reserved
 */
#define GMAC_CH1_SLC    0x00000007U
#define GMAC_CH1_SLC_M  (GMAC_CH1_SLC_V << GMAC_CH1_SLC_S)
#define GMAC_CH1_SLC_V  0x00000007U
#define GMAC_CH1_SLC_S  4
/** GMAC_CH1_ABPSSIE : R/W; bitpos: [17]; default: 0;
 *  Average Bits Per Slot Interrupt Enable When this bit is set, the MAC asserts an
 *  interrupt _sbd_intr_o or mci_intr_o_ when the average bits per slot status is
 *  updated _Bit 17 _ABSU_ in Register 89_ for Channel 1 When this bit is cleared,
 *  interrupt is not asserted for such an event
 */
#define GMAC_CH1_ABPSSIE    (BIT(17))
#define GMAC_CH1_ABPSSIE_M  (GMAC_CH1_ABPSSIE_V << GMAC_CH1_ABPSSIE_S)
#define GMAC_CH1_ABPSSIE_V  0x00000001U
#define GMAC_CH1_ABPSSIE_S  17

/** GMAC_REGISTER89_CHANNEL1CBSSTATUSREGISTER_REG register
 *  Provides the average traffic transmitted in Channel 1
 */
#define GMAC_REGISTER89_CHANNEL1CBSSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x1164)
/** GMAC_CH1_ABS : RO; bitpos: [16:0]; default: 0;
 *  Average Bits per Slot This field contains the average transmitted bits per slot
 *  This field is computed over programmed number of slots _SLC bits in the CBS Control
 *  Register_ for Channel 1 traffic The maximum value is 0x30D4 for 100 Mbps and
 *  0x1E848 for 1000 Mbps
 */
#define GMAC_CH1_ABS    0x0001FFFFU
#define GMAC_CH1_ABS_M  (GMAC_CH1_ABS_V << GMAC_CH1_ABS_S)
#define GMAC_CH1_ABS_V  0x0001FFFFU
#define GMAC_CH1_ABS_S  0
/** GMAC_CH1_ABSU : RO; bitpos: [17]; default: 0;
 *  ABS Updated When set, this bit indicates that the MAC has updated the ABS value
 *  This bit is cleared when the application reads the ABS value
 */
#define GMAC_CH1_ABSU    (BIT(17))
#define GMAC_CH1_ABSU_M  (GMAC_CH1_ABSU_V << GMAC_CH1_ABSU_S)
#define GMAC_CH1_ABSU_V  0x00000001U
#define GMAC_CH1_ABSU_S  17

/** GMAC_REGISTER90_CHANNEL1IDLESLOPECREDITREGISTER_REG register
 *  Contains the idleSlope credit value required for the creditbased shaper algorithm
 *  for Channel 1
 */
#define GMAC_REGISTER90_CHANNEL1IDLESLOPECREDITREGISTER_REG (DR_REG_GMAC_BASE + 0x1168)
/** GMAC_CH1_ISC : R/W; bitpos: [13:0]; default: 0;
 *  idleSlopeCredit This field contains the idleSlopeCredit value required for the
 *  creditbased shaper algorithm for Channel 1 This is the rate of change of credit in
 *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
 *  credit is increasing The software should program this field with computed credit in
 *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
 *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode
 */
#define GMAC_CH1_ISC    0x00003FFFU
#define GMAC_CH1_ISC_M  (GMAC_CH1_ISC_V << GMAC_CH1_ISC_S)
#define GMAC_CH1_ISC_V  0x00003FFFU
#define GMAC_CH1_ISC_S  0

/** GMAC_REGISTER91_CHANNEL1SENDSLOPECREDITREGISTER_REG register
 *  Contains the sendSlope credit value required for the creditbased shaper algorithm
 *  for Channel 1
 */
#define GMAC_REGISTER91_CHANNEL1SENDSLOPECREDITREGISTER_REG (DR_REG_GMAC_BASE + 0x116c)
/** GMAC_CH1_SSC : R/W; bitpos: [13:0]; default: 0;
 *  sendSlopeCredit This field contains the sendSlopeCredit value required for
 *  creditbased shaper algorithm for Channel 1 This is the rate of change of credit in
 *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
 *  credit is decreasing The software should program this field with computed credit in
 *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
 *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode This field should be
 *  programmed with absolute sendSlopeCredit value The creditbased shaper logic
 *  subtracts it from the accumulated credit when Channel 1 is selected for transmission
 */
#define GMAC_CH1_SSC    0x00003FFFU
#define GMAC_CH1_SSC_M  (GMAC_CH1_SSC_V << GMAC_CH1_SSC_S)
#define GMAC_CH1_SSC_V  0x00003FFFU
#define GMAC_CH1_SSC_S  0

/** GMAC_REGISTER92_CHANNEL1HICREDITREGISTER_REG register
 *  Contains the hiCredit value required for the creditbased shaper algorithm for
 *  Channel 1
 */
#define GMAC_REGISTER92_CHANNEL1HICREDITREGISTER_REG (DR_REG_GMAC_BASE + 0x1170)
/** GMAC_CH1_HC : R/W; bitpos: [28:0]; default: 0;
 *  hiCredit This field contains the hiCredit value required for the creditbased shaper
 *  algorithm for Channel 1 This is the maximum value that can be accumulated in the
 *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
 *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
 *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
 *  0x0800_0000
 */
#define GMAC_CH1_HC    0x1FFFFFFFU
#define GMAC_CH1_HC_M  (GMAC_CH1_HC_V << GMAC_CH1_HC_S)
#define GMAC_CH1_HC_V  0x1FFFFFFFU
#define GMAC_CH1_HC_S  0

/** GMAC_REGISTER93_CHANNEL1LOCREDITREGISTER_REG register
 *  Contains the loCredit value required for the creditbased shaper algorithm for
 *  Channel 1
 */
#define GMAC_REGISTER93_CHANNEL1LOCREDITREGISTER_REG (DR_REG_GMAC_BASE + 0x1174)
/** GMAC_CH1_LC : R/W; bitpos: [28:0]; default: 536870911;
 *  loCredit This field contains the loCredit value required for the creditbased shaper
 *  algorithm for Channel 1 This is the minimum value that can be accumulated in the
 *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
 *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
 *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
 *  0x0800_0000 The programmed value is 2's complement _negative number_, that is,
 *  0xF800_0000
 */
#define GMAC_CH1_LC    0x1FFFFFFFU
#define GMAC_CH1_LC_M  (GMAC_CH1_LC_V << GMAC_CH1_LC_S)
#define GMAC_CH1_LC_V  0x1FFFFFFFU
#define GMAC_CH1_LC_S  0

/** GMAC_REGISTER128_CHANNEL2BUSMODEREGISTER_REG register
 *  Controls the Host Interface mode for Channel 2
 */
#define GMAC_REGISTER128_CHANNEL2BUSMODEREGISTER_REG (DR_REG_GMAC_BASE + 0x1200)
/** GMAC_CH2_SWR : R/W; bitpos: [0]; default: 1;
 *  Software Reset When this bit is set, the MAC DMA Controller resets the logic and
 *  all internal registers of the MAC It is cleared automatically after the reset
 *  operation is complete in all of the DWC_gmac clock domains Before reprogramming any
 *  register of the DWC_gmac, you should read a zero _0_ value in this bit Note: The
 *  Software reset function is driven only by this bit Bit 0 of Register 64 _Channel 1
 *  Bus Mode Register_ or Register 128 _Channel 2 Bus Mode Register_ has no impact on
 *  the Software reset function The reset operation is completed only when all resets
 *  in all active clock domains are deasserted Therefore, it is essential that all PHY
 *  inputs clocks _applicable for the selected PHY interface_ are present for the
 *  software reset completion The time to complete the software reset operation depends
 *  on the frequency of the slowest active clock
 */
#define GMAC_CH2_SWR    (BIT(0))
#define GMAC_CH2_SWR_M  (GMAC_CH2_SWR_V << GMAC_CH2_SWR_S)
#define GMAC_CH2_SWR_V  0x00000001U
#define GMAC_CH2_SWR_S  0
/** GMAC_CH2_DA : R/W; bitpos: [1]; default: 0;
 *  DMA Arbitration Scheme This bit specifies the arbitration scheme between the
 *  transmit and receive paths of Channel 2 0: Weighted roundrobin with Rx:Tx or Tx:Rx
 *  The priority between the paths is according to the priority specified in Bits
 *  [15:14] _PR_ and priority weights specified in Bit 27 _TXPR_ 1: Fixed priority The
 *  transmit path has priority over receive path when Bit 27 _TXPR_ is set Otherwise,
 *  receive path has priority over the transmit path In the GMACAXI configuration,
 *  these bits are reserved and are readonly _RO_ For more information about the
 *  priority scheme between the transmit and receive paths, see Table 412 in “DMA
 *  Arbiter Functions” on page 167
 */
#define GMAC_CH2_DA    (BIT(1))
#define GMAC_CH2_DA_M  (GMAC_CH2_DA_V << GMAC_CH2_DA_S)
#define GMAC_CH2_DA_V  0x00000001U
#define GMAC_CH2_DA_S  1
/** GMAC_CH2_DSL : R/W; bitpos: [6:2]; default: 0;
 *  Descriptor Skip Length This bit specifies the number of Word, Dword, or Lword
 *  _depending on the 32bit, 64bit, or 128bit bus_ to skip between two unchained
 *  descriptors The address skipping starts from the end of current descriptor to the
 *  start of next descriptor When the DSL value is equal to zero, the descriptor table
 *  is taken as contiguous by the DMA in Ring mode
 */
#define GMAC_CH2_DSL    0x0000001FU
#define GMAC_CH2_DSL_M  (GMAC_CH2_DSL_V << GMAC_CH2_DSL_S)
#define GMAC_CH2_DSL_V  0x0000001FU
#define GMAC_CH2_DSL_S  2
/** GMAC_CH2_ATDS : R/W; bitpos: [7]; default: 0;
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
#define GMAC_CH2_ATDS    (BIT(7))
#define GMAC_CH2_ATDS_M  (GMAC_CH2_ATDS_V << GMAC_CH2_ATDS_S)
#define GMAC_CH2_ATDS_V  0x00000001U
#define GMAC_CH2_ATDS_S  7
/** GMAC_CH2_PBL : R/W; bitpos: [13:8]; default: 1;
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
#define GMAC_CH2_PBL    0x0000003FU
#define GMAC_CH2_PBL_M  (GMAC_CH2_PBL_V << GMAC_CH2_PBL_S)
#define GMAC_CH2_PBL_V  0x0000003FU
#define GMAC_CH2_PBL_S  8
/** GMAC_CH2_PR : R/W; bitpos: [15:14]; default: 0;
 *  Priority Ratio These bits control the priority ratio in the weighted roundrobin
 *  arbitration between the Rx DMA and Tx DMA These bits are valid only when Bit 1 _DA_
 *  is reset The priority ratio is Rx:Tx or Tx:Rx depending on whether Bit 27 _TXPR_ is
 *  reset or set 00: The Priority Ratio is 1:1 01: The Priority Ratio is 2:1 10: The
 *  Priority Ratio is 3:1 11: The Priority Ratio is 4:1 In the GMACAXI configuration,
 *  these bits are reserved and readonly _RO_ For more information about the priority
 *  scheme between the transmit and receive paths, see Table 412 in “DMA Arbiter
 *  Functions” on page 167
 */
#define GMAC_CH2_PR    0x00000003U
#define GMAC_CH2_PR_M  (GMAC_CH2_PR_V << GMAC_CH2_PR_S)
#define GMAC_CH2_PR_V  0x00000003U
#define GMAC_CH2_PR_S  14
/** GMAC_CH2_FB : R/W; bitpos: [16]; default: 0;
 *  Fixed Burst This bit controls whether the AHB or AXI master interface performs
 *  fixed burst transfers or not When set, the AHB interface uses only SINGLE, INCR4,
 *  INCR8, or INCR16 during start of the normal burst transfers When reset, the AHB or
 *  AXI interface uses SINGLE and INCR burst transfer operations For more information,
 *  see Bit 0 _UNDEF_ of the AXI Bus Mode register in the GMACAXI configuration
 */
#define GMAC_CH2_FB    (BIT(16))
#define GMAC_CH2_FB_M  (GMAC_CH2_FB_V << GMAC_CH2_FB_S)
#define GMAC_CH2_FB_V  0x00000001U
#define GMAC_CH2_FB_S  16
/** GMAC_CH2_RPBL : R/W; bitpos: [22:17]; default: 1;
 *  Rx DMA PBL This field indicates the maximum number of beats to be transferred in
 *  one Rx DMA transaction This is the maximum value that is used in a single block
 *  Read or Write The Rx DMA always attempts to burst as specified in the RPBL bit each
 *  time it starts a Burst transfer on the host bus You can program RPBL with values of
 *  1, 2, 4, 8, 16, and 32 Any other value results in undefined behavior This field is
 *  valid and applicable only when USP is set high
 */
#define GMAC_CH2_RPBL    0x0000003FU
#define GMAC_CH2_RPBL_M  (GMAC_CH2_RPBL_V << GMAC_CH2_RPBL_S)
#define GMAC_CH2_RPBL_V  0x0000003FU
#define GMAC_CH2_RPBL_S  17
/** GMAC_CH2_USP : R/W; bitpos: [23]; default: 0;
 *  Use Separate PBL When set high, this bit configures the Rx DMA to use the value
 *  configured in Bits [22:17] as PBL The PBL value in Bits [13:8] is applicable only
 *  to the Tx DMA operations When reset to low, the PBL value in Bits [13:8] is
 *  applicable for both DMA engines
 */
#define GMAC_CH2_USP    (BIT(23))
#define GMAC_CH2_USP_M  (GMAC_CH2_USP_V << GMAC_CH2_USP_S)
#define GMAC_CH2_USP_V  0x00000001U
#define GMAC_CH2_USP_S  23
/** GMAC_CH2_PBLX8 : R/W; bitpos: [24]; default: 0;
 *  PBLx8 Mode When set high, this bit multiplies the programmed PBL value _Bits
 *  [22:17] and Bits[13:8]_ eight times Therefore, the DMA transfers the data in 8, 16,
 *  32, 64, 128, and 256 beats depending on the PBL value Note: This bit function is
 *  not backward compatible Before release 350a, this bit was 4xPBL
 */
#define GMAC_CH2_PBLX8    (BIT(24))
#define GMAC_CH2_PBLX8_M  (GMAC_CH2_PBLX8_V << GMAC_CH2_PBLX8_S)
#define GMAC_CH2_PBLX8_V  0x00000001U
#define GMAC_CH2_PBLX8_S  24
/** GMAC_CH2_AAL : R/W; bitpos: [25]; default: 0;
 *  AddressAligned Beats When this bit is set high and the FB bit is equal to 1, the
 *  AHB or AXI interface generates all bursts aligned to the start address LS bits If
 *  the FB bit is equal to 0, the first burst _accessing the start address of data
 *  buffer_ is not aligned, but subsequent bursts are aligned to the address This bit
 *  is valid only in the GMACAHB and GMACAXI configurations and is reserved _RO with
 *  default value 0_ in all other configurations
 */
#define GMAC_CH2_AAL    (BIT(25))
#define GMAC_CH2_AAL_M  (GMAC_CH2_AAL_V << GMAC_CH2_AAL_S)
#define GMAC_CH2_AAL_V  0x00000001U
#define GMAC_CH2_AAL_S  25
/** GMAC_CH2_MB : R/W; bitpos: [26]; default: 0;
 *  Mixed Burst When this bit is set high and the FB bit is low, the AHB master
 *  interface starts all bursts of length more than 16 with INCR _undefined burst_,
 *  whereas it reverts to fixed burst transfers _INCRx and SINGLE_ for burst length of
 *  16 and less This bit is valid only in the GMACAHB configuration and reserved in all
 *  other configuration
 */
#define GMAC_CH2_MB    (BIT(26))
#define GMAC_CH2_MB_M  (GMAC_CH2_MB_V << GMAC_CH2_MB_S)
#define GMAC_CH2_MB_V  0x00000001U
#define GMAC_CH2_MB_S  26
/** GMAC_CH2_TXPR : R/W; bitpos: [27]; default: 0;
 *  Transmit Priority When set, this bit indicates that the transmit DMA has higher
 *  priority than the receive DMA during arbitration for the systemside bus In the
 *  GMACAXI configuration, this bit is reserved and readonly _RO_ For more information
 *  about the priority scheme between the transmit and receive paths, see Table 412 in
 *  “DMA Arbiter Functions” on page 167
 */
#define GMAC_CH2_TXPR    (BIT(27))
#define GMAC_CH2_TXPR_M  (GMAC_CH2_TXPR_V << GMAC_CH2_TXPR_S)
#define GMAC_CH2_TXPR_V  0x00000001U
#define GMAC_CH2_TXPR_S  27
/** GMAC_CH2_PRWG : R/W; bitpos: [29:28]; default: 0;
 *  Channel Priority Weights This field sets the priority weights for Channel 2 during
 *  the roundrobin arbitration between the DMA channels for the system bus 00: The
 *  priority weight is 1 01: The priority weight is 2 10: The priority weight is 3 11:
 *  The priority weight is 4 This field is present in all DWC_gmac configurations
 *  except GMACAXI when you select the AV feature Otherwise, this field is reserved and
 *  readonly _RO_ For more information about the priority weights of DMA channels, see
 *  “DMA Arbiter Functions” on page 167
 */
#define GMAC_CH2_PRWG    0x00000003U
#define GMAC_CH2_PRWG_M  (GMAC_CH2_PRWG_V << GMAC_CH2_PRWG_S)
#define GMAC_CH2_PRWG_V  0x00000003U
#define GMAC_CH2_PRWG_S  28
/** GMAC_CH2_RIB : R/W; bitpos: [31]; default: 0;
 *  Rebuild INCRx Burst When this bit is set high and the AHB master gets an EBT
 *  _Retry, Split, or Losing bus grant_, the AHB master interface rebuilds the pending
 *  beats of any burst transfer initiated with INCRx The AHB master interface rebuilds
 *  the beats with a combination of specified bursts with INCRx and SINGLE By default,
 *  the AHB master interface rebuilds pending beats of an EBT with an unspecified
 *  _INCR_ burst This bit is valid only in the GMACAHB configuration It is reserved in
 *  all other configuration
 */
#define GMAC_CH2_RIB    (BIT(31))
#define GMAC_CH2_RIB_M  (GMAC_CH2_RIB_V << GMAC_CH2_RIB_S)
#define GMAC_CH2_RIB_V  0x00000001U
#define GMAC_CH2_RIB_S  31

/** GMAC_REGISTER129_CHANNEL2TRANSMITPOLLDEMANDREGISTER_REG register
 *  Used by the host to instruct the DMA to poll the Transmit Descriptor list
 */
#define GMAC_REGISTER129_CHANNEL2TRANSMITPOLLDEMANDREGISTER_REG (DR_REG_GMAC_BASE + 0x1204)
/** GMAC_CH2_TPD : RO; bitpos: [31:0]; default: 0;
 *  Transmit Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 18 _Current Host Transmit Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  transmission returns to the Suspend state and Bit 2 _TU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the transmission resumes
 */
#define GMAC_CH2_TPD    0xFFFFFFFFU
#define GMAC_CH2_TPD_M  (GMAC_CH2_TPD_V << GMAC_CH2_TPD_S)
#define GMAC_CH2_TPD_V  0xFFFFFFFFU
#define GMAC_CH2_TPD_S  0

/** GMAC_REGISTER130_CHANNEL2RECEIVEPOLLDEMANDREGISTER_REG register
 *  Used by the Host to instruct the DMA to poll the Receive Descriptor list
 */
#define GMAC_REGISTER130_CHANNEL2RECEIVEPOLLDEMANDREGISTER_REG (DR_REG_GMAC_BASE + 0x1208)
/** GMAC_CH2_RPD : RO; bitpos: [31:0]; default: 0;
 *  Receive Poll Demand When these bits are written with any value, the DMA reads the
 *  current descriptor to which the Register 19 _Current Host Receive Descriptor
 *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
 *  reception returns to the Suspended state and Bit 7 _RU_ of Register 5 _Status
 *  Register_ is asserted If the descriptor is available, the Rx DMA returns to the
 *  active state
 */
#define GMAC_CH2_RPD    0xFFFFFFFFU
#define GMAC_CH2_RPD_M  (GMAC_CH2_RPD_V << GMAC_CH2_RPD_S)
#define GMAC_CH2_RPD_V  0xFFFFFFFFU
#define GMAC_CH2_RPD_S  0

/** GMAC_REGISTER131_CHANNEL2RECEIVEDESCRIPTORLISTADDRESSREGISTER_REG register
 *  Points the DMA to the start of the Receive Descriptor list
 */
#define GMAC_REGISTER131_CHANNEL2RECEIVEDESCRIPTORLISTADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x120c)
/** GMAC_CH2_RDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Receive List This field contains the base address of the first descriptor
 *  in the Receive Descriptor list The LSB bits _1:0, 2:0, or 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and internally taken as allzero by the DMA Therefore,
 *  these LSB bits are readonly _RO_
 */
#define GMAC_CH2_RDESLA    0xFFFFFFFFU
#define GMAC_CH2_RDESLA_M  (GMAC_CH2_RDESLA_V << GMAC_CH2_RDESLA_S)
#define GMAC_CH2_RDESLA_V  0xFFFFFFFFU
#define GMAC_CH2_RDESLA_S  0

/** GMAC_REGISTER132_CHANNEL2TRANSMITDESCRIPTORLISTADDRESSREGISTER_REG register
 *  Points the DMA to the start of the Transmit Descriptor List
 */
#define GMAC_REGISTER132_CHANNEL2TRANSMITDESCRIPTORLISTADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x1210)
/** GMAC_CH2_TDESLA : R/W; bitpos: [31:0]; default: 0;
 *  Start of Transmit List This field contains the base address of the first descriptor
 *  in the Transmit Descriptor list The LSB bits _1:0, 2:0, 3:0_ for 32bit, 64bit, or
 *  128bit bus width are ignored and are internally taken as allzero by the DMA
 *  Therefore, these LSB bits are readonly _RO_
 */
#define GMAC_CH2_TDESLA    0xFFFFFFFFU
#define GMAC_CH2_TDESLA_M  (GMAC_CH2_TDESLA_V << GMAC_CH2_TDESLA_S)
#define GMAC_CH2_TDESLA_V  0xFFFFFFFFU
#define GMAC_CH2_TDESLA_S  0

/** GMAC_REGISTER133_CHANNEL2STATUSREGISTER_REG register
 *  The software driver _application_ reads this register during interrupt service
 *  routine or polling to determine the status of the DMA Bits [29:26] are reserved for
 *  the Channel 2 Status Register
 */
#define GMAC_REGISTER133_CHANNEL2STATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x1214)
/** GMAC_CH2_TI : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt This bit indicates that the frame transmission is complete When
 *  transmission is complete, Bit 31 _OWN_ of TDES0 is reset, and the specific frame
 *  status information is updated in the descriptor
 */
#define GMAC_CH2_TI    (BIT(0))
#define GMAC_CH2_TI_M  (GMAC_CH2_TI_V << GMAC_CH2_TI_S)
#define GMAC_CH2_TI_V  0x00000001U
#define GMAC_CH2_TI_S  0
/** GMAC_CH2_TPS : R/W; bitpos: [1]; default: 0;
 *  Transmit Process Stopped This bit is set when the transmission is stopped
 */
#define GMAC_CH2_TPS    (BIT(1))
#define GMAC_CH2_TPS_M  (GMAC_CH2_TPS_V << GMAC_CH2_TPS_S)
#define GMAC_CH2_TPS_V  0x00000001U
#define GMAC_CH2_TPS_S  1
/** GMAC_CH2_TU : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Transmit List and the DMA cannot acquire it Transmission is
 *  suspended Bits[22:20] explain the Transmit Process state transitions To resume
 *  processing Transmit descriptors, the host should change the ownership of the
 *  descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command
 */
#define GMAC_CH2_TU    (BIT(2))
#define GMAC_CH2_TU_M  (GMAC_CH2_TU_V << GMAC_CH2_TU_S)
#define GMAC_CH2_TU_V  0x00000001U
#define GMAC_CH2_TU_S  2
/** GMAC_CH2_TJT : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout This bit indicates that the Transmit Jabber Timer expired,
 *  which happens when the frame size exceeds 2,048 _10,240 bytes when the Jumbo frame
 *  is enabled_ When the Jabber Timeout occurs, the transmission process is aborted and
 *  placed in the Stopped state This causes the Transmit Jabber Timeout TDES0[14] flag
 *  to assert
 */
#define GMAC_CH2_TJT    (BIT(3))
#define GMAC_CH2_TJT_M  (GMAC_CH2_TJT_V << GMAC_CH2_TJT_S)
#define GMAC_CH2_TJT_V  0x00000001U
#define GMAC_CH2_TJT_S  3
/** GMAC_CH2_OVF : R/W; bitpos: [4]; default: 0;
 *  Receive Overflow This bit indicates that the Receive Buffer had an Overflow during
 *  frame reception If the partial frame is transferred to the application, the
 *  overflow status is set in RDES0[11]
 */
#define GMAC_CH2_OVF    (BIT(4))
#define GMAC_CH2_OVF_M  (GMAC_CH2_OVF_V << GMAC_CH2_OVF_S)
#define GMAC_CH2_OVF_V  0x00000001U
#define GMAC_CH2_OVF_S  4
/** GMAC_CH2_UNF : R/W; bitpos: [5]; default: 0;
 *  Transmit Underflow This bit indicates that the Transmit Buffer had an Underflow
 *  during frame transmission Transmission is suspended and an Underflow Error TDES0[1]
 *  is set
 */
#define GMAC_CH2_UNF    (BIT(5))
#define GMAC_CH2_UNF_M  (GMAC_CH2_UNF_V << GMAC_CH2_UNF_S)
#define GMAC_CH2_UNF_V  0x00000001U
#define GMAC_CH2_UNF_S  5
/** GMAC_CH2_RI : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt This bit indicates that the frame reception is complete When
 *  reception is complete, the Bit 31 of RDES1 _Disable Interrupt on Completion_ is
 *  reset in the last Descriptor, and the specific frame status information is updated
 *  in the descriptor The reception remains in the Running state
 */
#define GMAC_CH2_RI    (BIT(6))
#define GMAC_CH2_RI_M  (GMAC_CH2_RI_V << GMAC_CH2_RI_S)
#define GMAC_CH2_RI_V  0x00000001U
#define GMAC_CH2_RI_S  6
/** GMAC_CH2_RU : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable This bit indicates that the host owns the Next
 *  Descriptor in the Receive List and the DMA cannot acquire it The Receive Process is
 *  suspended To resume processing Receive descriptors, the host should change the
 *  ownership of the descriptor and issue a Receive Poll Demand command If no Receive
 *  Poll Demand is issued, the Receive Process resumes when the next recognized
 *  incoming frame is received This bit is set only when the previous Receive
 *  Descriptor is owned by the DMA
 */
#define GMAC_CH2_RU    (BIT(7))
#define GMAC_CH2_RU_M  (GMAC_CH2_RU_V << GMAC_CH2_RU_S)
#define GMAC_CH2_RU_V  0x00000001U
#define GMAC_CH2_RU_S  7
/** GMAC_CH2_RPS : R/W; bitpos: [8]; default: 0;
 *  Receive Process Stopped This bit is asserted when the Receive Process enters the
 *  Stopped state
 */
#define GMAC_CH2_RPS    (BIT(8))
#define GMAC_CH2_RPS_M  (GMAC_CH2_RPS_V << GMAC_CH2_RPS_S)
#define GMAC_CH2_RPS_V  0x00000001U
#define GMAC_CH2_RPS_S  8
/** GMAC_CH2_RWT : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout When set, this bit indicates that the Receive Watchdog
 *  Timer expired while receiving the current frame and the current frame is truncated
 *  after the watchdog timeout
 */
#define GMAC_CH2_RWT    (BIT(9))
#define GMAC_CH2_RWT_M  (GMAC_CH2_RWT_V << GMAC_CH2_RWT_S)
#define GMAC_CH2_RWT_V  0x00000001U
#define GMAC_CH2_RWT_S  9
/** GMAC_CH2_ETI : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt This bit indicates that the frame to be transmitted is
 *  fully transferred to the MTL Transmit FIFO
 */
#define GMAC_CH2_ETI    (BIT(10))
#define GMAC_CH2_ETI_M  (GMAC_CH2_ETI_V << GMAC_CH2_ETI_S)
#define GMAC_CH2_ETI_V  0x00000001U
#define GMAC_CH2_ETI_S  10
/** GMAC_CH2_FBI : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Interrupt This bit indicates that a bus error occurred, as
 *  described in Bits [25:23] When this bit is set, the corresponding DMA engine
 *  disables all of its bus accesses 12:11		Reserved	00	RO
 */
#define GMAC_CH2_FBI    (BIT(13))
#define GMAC_CH2_FBI_M  (GMAC_CH2_FBI_V << GMAC_CH2_FBI_S)
#define GMAC_CH2_FBI_V  0x00000001U
#define GMAC_CH2_FBI_S  13
/** GMAC_CH2_ERI : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt This bit indicates that the DMA filled the first data
 *  buffer of the packet This bit is cleared when the software writes 1 to this bit or
 *  Bit 6 _RI_ of this register is set _whichever occurs earlier_
 */
#define GMAC_CH2_ERI    (BIT(14))
#define GMAC_CH2_ERI_M  (GMAC_CH2_ERI_V << GMAC_CH2_ERI_S)
#define GMAC_CH2_ERI_V  0x00000001U
#define GMAC_CH2_ERI_S  14
/** GMAC_CH2_AIS : R/W; bitpos: [15]; default: 0;
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
#define GMAC_CH2_AIS    (BIT(15))
#define GMAC_CH2_AIS_M  (GMAC_CH2_AIS_V << GMAC_CH2_AIS_S)
#define GMAC_CH2_AIS_V  0x00000001U
#define GMAC_CH2_AIS_S  15
/** GMAC_CH2_NIS : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Normal Interrupt Summary bit value is the logical OR of
 *  the following bits when the corresponding interrupt bits are enabled in Register 7
 *  _Interrupt Enable Register_: Register 5[0]: Transmit Interrupt Register 5[2]:
 *  Transmit Buffer Unavailable Register 5[6]: Receive Interrupt Register 5[14]: Early
 *  Receive Interrupt Only unmasked bits _interrupts for which interrupt enable is set
 *  in Register 7_ affect the Normal Interrupt Summary bit This is a sticky bit and
 *  must be cleared _by writing 1 to this bit_ each time a corresponding bit, which
 *  causes NIS to be set, is cleared
 */
#define GMAC_CH2_NIS    (BIT(16))
#define GMAC_CH2_NIS_M  (GMAC_CH2_NIS_V << GMAC_CH2_NIS_S)
#define GMAC_CH2_NIS_V  0x00000001U
#define GMAC_CH2_NIS_S  16
/** GMAC_CH2_RS : RO; bitpos: [19:17]; default: 0;
 *  Receive Process State This field indicates the Receive DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Receive Command
 *  issued 3’b001: Running: Fetching Receive Transfer Descriptor 3’b010: Reserved for
 *  future use 3’b011: Running: Waiting for receive packet 3’b100: Suspended: Receive
 *  Descriptor Unavailable 3’b101: Running: Closing Receive Descriptor 3’b110:
 *  TIME_STAMP write state 3’b111: Running: Transferring the receive packet data from
 *  receive buffer to host memory
 */
#define GMAC_CH2_RS    0x00000007U
#define GMAC_CH2_RS_M  (GMAC_CH2_RS_V << GMAC_CH2_RS_S)
#define GMAC_CH2_RS_V  0x00000007U
#define GMAC_CH2_RS_S  17
/** GMAC_CH2_TS : RO; bitpos: [22:20]; default: 0;
 *  Transmit Process State This field indicates the Transmit DMA FSM state This field
 *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Transmit Command
 *  issued 3’b001: Running: Fetching Transmit Transfer Descriptor 3’b010: Running:
 *  Waiting for status 3’b011: Running: Reading Data from host memory buffer and
 *  queuing it to transmit buffer _Tx FIFO_ 3’b100: TIME_STAMP write state 3’b101:
 *  Reserved for future use 3’b110: Suspended: Transmit Descriptor Unavailable or
 *  Transmit Buffer Underflow 3’b111: Running: Closing Transmit Descriptor
 */
#define GMAC_CH2_TS    0x00000007U
#define GMAC_CH2_TS_M  (GMAC_CH2_TS_V << GMAC_CH2_TS_S)
#define GMAC_CH2_TS_V  0x00000007U
#define GMAC_CH2_TS_S  20
/** GMAC_CH2_EB : RO; bitpos: [25:23]; default: 0;
 *  Error Bits This field indicates the type of error that caused a Bus Error, for
 *  example, error response on the AHB or AXI interface This field is valid only when
 *  Bit 13 _FBI_ is set This field does not generate an interrupt 0 0 0: Error during
 *  Rx DMA Write Data Transfer 0 1 1: Error during Tx DMA Read Data Transfer 1 0 0:
 *  Error during Rx DMA Descriptor Write Access 1 0 1: Error during Tx DMA Descriptor
 *  Write Access 1 1 0: Error during Rx DMA Descriptor Read Access 1 1 1: Error during
 *  Tx DMA Descriptor Read Access Note: 001 and 010 are reserved
 */
#define GMAC_CH2_EB    0x00000007U
#define GMAC_CH2_EB_M  (GMAC_CH2_EB_V << GMAC_CH2_EB_S)
#define GMAC_CH2_EB_V  0x00000007U
#define GMAC_CH2_EB_S  23
/** GMAC_CH2_GLI : RO; bitpos: [26]; default: 0;
 *  GMAC Line Interface Interrupt When set, this bit reflects any of the following
 *  interrupt events in the DWC_gmac interfaces _if present and enabled in your
 *  configuration_: PCS _TBI, RTBI, or SGMII_: Link change or autonegotiation complete
 *  event SMII or RGMII: Link change event General Purpose Input Status _GPIS_: Any LL
 *  or LH event on the gpi_i input ports To identify the exact cause of the interrupt,
 *  the software must first read Bit 11 and Bits[2:0] of Register 14 _Interrupt Status
 *  Register_ and then to clear the source of interrupt _which also clears the GLI
 *  interrupt_, read any of the following corresponding registers: PCS _TBI, RTBI, or
 *  SGMII_: Register 49 _AN Status Register_ SMII or RGMII: Register 54
 *  _SGMII/RGMII/SMII Control and Status Register_ General Purpose Input _GPI_:
 *  Register 56 _General Purpose IO Register_ The interrupt signal from the DWC_gmac
 *  subsystem _sbd_intr_o_ is high when this bit is high
 */
#define GMAC_CH2_GLI    (BIT(26))
#define GMAC_CH2_GLI_M  (GMAC_CH2_GLI_V << GMAC_CH2_GLI_S)
#define GMAC_CH2_GLI_V  0x00000001U
#define GMAC_CH2_GLI_S  26
/** GMAC_CH2_GMI : RO; bitpos: [27]; default: 0;
 *  GMAC MMC Interrupt This bit reflects an interrupt event in the MMC module of the
 *  DWC_gmac The software must read the corresponding registers in the DWC_gmac to get
 *  the exact cause of the interrupt and clear the source of interrupt to make this bit
 *  as 1’b0 The interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when
 *  this bit is high This bit is applicable only when the MAC Management Counters _MMC_
 *  are enabled Otherwise, this bit is reserved
 */
#define GMAC_CH2_GMI    (BIT(27))
#define GMAC_CH2_GMI_M  (GMAC_CH2_GMI_V << GMAC_CH2_GMI_S)
#define GMAC_CH2_GMI_V  0x00000001U
#define GMAC_CH2_GMI_S  27
/** GMAC_CH2_GPI : RO; bitpos: [28]; default: 0;
 *  GMAC PMT Interrupt This bit indicates an interrupt event in the PMT module of the
 *  DWC_gmac The software must read the PMT Control and Status Register in the MAC to
 *  get the exact cause of interrupt and clear its source to reset this bit to 1’b0 The
 *  interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when this bit is
 *  high This bit is applicable only when the Power Management feature is enabled
 *  Otherwise, this bit is reserved Note: The GPI and pmt_intr_o interrupts are
 *  generated in different clock domains
 */
#define GMAC_CH2_GPI    (BIT(28))
#define GMAC_CH2_GPI_M  (GMAC_CH2_GPI_V << GMAC_CH2_GPI_S)
#define GMAC_CH2_GPI_V  0x00000001U
#define GMAC_CH2_GPI_S  28
/** GMAC_CH2_TTI : RO; bitpos: [29]; default: 0;
 *  Timestamp Trigger Interrupt This bit indicates an interrupt event in the Timestamp
 *  Generator block of the DWC_gmac The software must read the corresponding registers
 *  in the DWC_gmac to get the exact cause of the interrupt and clear its source to
 *  reset this bit to 1'b0 When this bit is high, the interrupt signal from the
 *  DWC_gmac subsystem _sbd_intr_o_ is high This bit is applicable only when the IEEE
 *  1588 Timestamp feature is enabled Otherwise, this bit is reserved
 */
#define GMAC_CH2_TTI    (BIT(29))
#define GMAC_CH2_TTI_M  (GMAC_CH2_TTI_V << GMAC_CH2_TTI_S)
#define GMAC_CH2_TTI_V  0x00000001U
#define GMAC_CH2_TTI_S  29
/** GMAC_CH2_GLPII_GTMSI : RO; bitpos: [30]; default: 0;
 *  GTMSI: GMAC TMS Interrupt _for Channel 1 and Channel 2_ This bit indicates an
 *  interrupt event in the traffic manager and scheduler logic of DWC_gmac To reset
 *  this bit, the software must read the corresponding registers _Channel Status
 *  Register_ to get the exact cause of the interrupt and clear its source Note: GTMSI
 *  status is given only in Channel 1 and Channel 2 DMA register when the AV feature is
 *  enabled and corresponding additional transmit channels are present Otherwise, this
 *  bit is reserved When this bit is high, the interrupt signal from the MAC
 *  _sbd_intr_o_ is high
 */
#define GMAC_CH2_GLPII_GTMSI    (BIT(30))
#define GMAC_CH2_GLPII_GTMSI_M  (GMAC_CH2_GLPII_GTMSI_V << GMAC_CH2_GLPII_GTMSI_S)
#define GMAC_CH2_GLPII_GTMSI_V  0x00000001U
#define GMAC_CH2_GLPII_GTMSI_S  30

/** GMAC_REGISTER134_CHANNEL2OPERATIONMODEREGISTER_REG register
 *  Establishes the Receive and Transmit operating modes and command
 */
#define GMAC_REGISTER134_CHANNEL2OPERATIONMODEREGISTER_REG (DR_REG_GMAC_BASE + 0x1218)
/** GMAC_CH2_SR : R/W; bitpos: [1]; default: 0;
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
#define GMAC_CH2_SR    (BIT(1))
#define GMAC_CH2_SR_M  (GMAC_CH2_SR_V << GMAC_CH2_SR_S)
#define GMAC_CH2_SR_V  0x00000001U
#define GMAC_CH2_SR_S  1
/** GMAC_CH2_OSF : R/W; bitpos: [2]; default: 0;
 *  Operate on Second Frame When this bit is set, it instructs the DMA to process the
 *  second frame of the Transmit data even before the status for the first frame is
 *  obtained
 */
#define GMAC_CH2_OSF    (BIT(2))
#define GMAC_CH2_OSF_M  (GMAC_CH2_OSF_V << GMAC_CH2_OSF_S)
#define GMAC_CH2_OSF_V  0x00000001U
#define GMAC_CH2_OSF_S  2
/** GMAC_CH2_RTC : R/W; bitpos: [4:3]; default: 0;
 *  Receive Threshold Control These two bits control the threshold level of the MTL
 *  Receive FIFO Transfer _request_ to DMA starts when the frame size within the MTL
 *  Receive FIFO is larger than the threshold In addition, full frames with length less
 *  than the threshold are automatically transferred The value of 11 is not applicable
 *  if the configured Receive FIFO size is 128 bytes These bits are valid only when the
 *  RSF bit is zero, and are ignored when the RSF bit is set to 1 00: 64 01: 32 10: 96
 *  11: 128
 */
#define GMAC_CH2_RTC    0x00000003U
#define GMAC_CH2_RTC_M  (GMAC_CH2_RTC_V << GMAC_CH2_RTC_S)
#define GMAC_CH2_RTC_V  0x00000003U
#define GMAC_CH2_RTC_S  3
/** GMAC_CH2_DGF : R/W; bitpos: [5]; default: 0;
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
#define GMAC_CH2_DGF    (BIT(5))
#define GMAC_CH2_DGF_M  (GMAC_CH2_DGF_V << GMAC_CH2_DGF_S)
#define GMAC_CH2_DGF_V  0x00000001U
#define GMAC_CH2_DGF_S  5
/** GMAC_CH2_FUF : R/W; bitpos: [6]; default: 0;
 *  Forward Undersized Good Frames When set, the Rx FIFO forwards Undersized frames
 *  _that is, frames with no Error and length less than 64 bytes_ including padbytes
 *  and CRC When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a
 *  frame is already transferred because of the lower value of Receive Threshold, for
 *  example, RTC = 01
 */
#define GMAC_CH2_FUF    (BIT(6))
#define GMAC_CH2_FUF_M  (GMAC_CH2_FUF_V << GMAC_CH2_FUF_S)
#define GMAC_CH2_FUF_V  0x00000001U
#define GMAC_CH2_FUF_S  6
/** GMAC_CH2_FEF : R/W; bitpos: [7]; default: 0;
 *  Forward Error Frames When this bit is reset, the Rx FIFO drops frames with error
 *  status _CRC error, collision error, GMII_ER, giant frame, watchdog timeout, or
 *  overflow_ However, if the start byte _write_ pointer of a frame is already
 *  transferred to the read controller side _in Threshold mode_, then the frame is not
 *  dropped In the GMACMTL configuration in which the Frame Length FIFO is also enabled
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
 *  features: 	L3L4 filter in GMACCORE or GMACMTL configurations 	Full checksum offload
 *  engine _Type 2_ with enhanced descriptor format in the GMACDMA, GMACAHB, or GMACAXI
 *  configurations
 */
#define GMAC_CH2_FEF    (BIT(7))
#define GMAC_CH2_FEF_M  (GMAC_CH2_FEF_V << GMAC_CH2_FEF_S)
#define GMAC_CH2_FEF_V  0x00000001U
#define GMAC_CH2_FEF_S  7
/** GMAC_CH2_EFC : R/W; bitpos: [8]; default: 0;
 *  Enable HW Flow Control When this bit is set, the flow control signal operation
 *  based on the filllevel of Rx FIFO is enabled When reset, the flow control operation
 *  is disabled This bit is not used _reserved and always reset_ when the Rx FIFO is
 *  less than 4 KB
 */
#define GMAC_CH2_EFC    (BIT(8))
#define GMAC_CH2_EFC_M  (GMAC_CH2_EFC_V << GMAC_CH2_EFC_S)
#define GMAC_CH2_EFC_V  0x00000001U
#define GMAC_CH2_EFC_S  8
/** GMAC_CH2_RFA : R/W; bitpos: [10:9]; default: 0;
 *  Threshold for Activating Flow Control _in halfduplex and fullduplex modes_ These
 *  bits control the threshold _Fill level of Rx FIFO_ at which the flow control is
 *  activated 00: Full minus 1 KB, that is, FULL—1KB 01: Full minus 2 KB, that is,
 *  FULL—2KB 10: Full minus 3 KB, that is, FULL—3KB 11: Full minus 4 KB, that is,
 *  FULL—4KB These values are applicable only to Rx FIFOs of 4 KB or more and when Bit
 *  8 _EFC_ is set high If the Rx FIFO is 8 KB or more, an additional Bit _RFA_2_ is
 *  used for more threshold levels as described in Bit 23 These bits are reserved and
 *  readonly when the depth of Rx FIFO is less than 4 KB Note: When FIFO size is
 *  exactly 4 KB, although the DWC_gmac allows you to program the value of these bits
 *  to 11, the software should not program these bits to 2'b11 The value 2'b11 means
 *  flow control on FIFO empty condition
 */
#define GMAC_CH2_RFA    0x00000003U
#define GMAC_CH2_RFA_M  (GMAC_CH2_RFA_V << GMAC_CH2_RFA_S)
#define GMAC_CH2_RFA_V  0x00000003U
#define GMAC_CH2_RFA_S  9
/** GMAC_CH2_RFD : R/W; bitpos: [12:11]; default: 0;
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
#define GMAC_CH2_RFD    0x00000003U
#define GMAC_CH2_RFD_M  (GMAC_CH2_RFD_V << GMAC_CH2_RFD_S)
#define GMAC_CH2_RFD_V  0x00000003U
#define GMAC_CH2_RFD_S  11
/** GMAC_CH2_ST : R/W; bitpos: [13]; default: 0;
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
#define GMAC_CH2_ST    (BIT(13))
#define GMAC_CH2_ST_M  (GMAC_CH2_ST_V << GMAC_CH2_ST_S)
#define GMAC_CH2_ST_V  0x00000001U
#define GMAC_CH2_ST_S  13
/** GMAC_CH2_TTC : R/W; bitpos: [16:14]; default: 0;
 *  Transmit Threshold Control These bits control the threshold level of the MTL
 *  Transmit FIFO Transmission starts when the frame size within the MTL Transmit FIFO
 *  is larger than the threshold In addition, full frames with a length less than the
 *  threshold are also transmitted These bits are used only when Bit 21 _TSF_ is reset
 *  000: 64 001: 128 010: 192 011: 256 100: 40 101: 32 110: 24 111: 16
 */
#define GMAC_CH2_TTC    0x00000007U
#define GMAC_CH2_TTC_M  (GMAC_CH2_TTC_V << GMAC_CH2_TTC_S)
#define GMAC_CH2_TTC_V  0x00000007U
#define GMAC_CH2_TTC_S  14
/** GMAC_CH2_FTF : R/W; bitpos: [20]; default: 0;
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
#define GMAC_CH2_FTF    (BIT(20))
#define GMAC_CH2_FTF_M  (GMAC_CH2_FTF_V << GMAC_CH2_FTF_S)
#define GMAC_CH2_FTF_V  0x00000001U
#define GMAC_CH2_FTF_S  20
/** GMAC_CH2_TSF : R/W; bitpos: [21]; default: 0;
 *  Transmit Store and Forward When this bit is set, transmission starts when a full
 *  frame resides in the MTL Transmit FIFO When this bit is set, the TTC values
 *  specified in Bits [16:14] are ignored This bit should be changed only when the
 *  transmission is stopped
 */
#define GMAC_CH2_TSF    (BIT(21))
#define GMAC_CH2_TSF_M  (GMAC_CH2_TSF_V << GMAC_CH2_TSF_S)
#define GMAC_CH2_TSF_V  0x00000001U
#define GMAC_CH2_TSF_S  21
/** GMAC_CH2_RFD_2 : R/W; bitpos: [22]; default: 0;
 *  MSB of Threshold for Deactivating Flow Control If the DWC_gmac is configured for Rx
 *  FIFO size of 8 KB or more, this bit _when set_ provides additional threshold levels
 *  for deactivating the flow control in both halfduplex and fullduplex modes This bit
 *  _as Most Significant Bit_ along with the RFD _Bits [12:11]_ gives the following
 *  thresholds for deactivating flow control: 100: Full minus 5 KB, that is, FULL — 5
 *  KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that is, FULL —
 *  7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB or less deep
 */
#define GMAC_CH2_RFD_2    (BIT(22))
#define GMAC_CH2_RFD_2_M  (GMAC_CH2_RFD_2_V << GMAC_CH2_RFD_2_S)
#define GMAC_CH2_RFD_2_V  0x00000001U
#define GMAC_CH2_RFD_2_S  22
/** GMAC_CH2_RFA_2 : R/W; bitpos: [23]; default: 0;
 *  MSB of Threshold for Activating Flow Control If the DWC_gmac is configured for an
 *  Rx FIFO size of 8 KB or more, this bit _when set_ provides additional threshold
 *  levels for activating the flow control in both half duplex and fullduplex modes
 *  This bit _as Most Significant Bit_, along with the RFA _Bits [10:9]_, gives the
 *  following thresholds for activating flow control: 100: Full minus 5 KB, that is,
 *  FULL — 5 KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that
 *  is, FULL — 7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB
 *  or less deep
 */
#define GMAC_CH2_RFA_2    (BIT(23))
#define GMAC_CH2_RFA_2_M  (GMAC_CH2_RFA_2_V << GMAC_CH2_RFA_2_S)
#define GMAC_CH2_RFA_2_V  0x00000001U
#define GMAC_CH2_RFA_2_S  23
/** GMAC_CH2_DFF : R/W; bitpos: [24]; default: 0;
 *  Disable Flushing of Received Frames When this bit is set, the Rx DMA does not flush
 *  any frames because of the unavailability of receive descriptors or buffers as it
 *  does normally when this bit is reset _See “Receive Process Suspended” on page 83_
 *  This bit is reserved _and RO_ in the GMACMTL configuration
 */
#define GMAC_CH2_DFF    (BIT(24))
#define GMAC_CH2_DFF_M  (GMAC_CH2_DFF_V << GMAC_CH2_DFF_S)
#define GMAC_CH2_DFF_V  0x00000001U
#define GMAC_CH2_DFF_S  24
/** GMAC_CH2_RSF : R/W; bitpos: [25]; default: 0;
 *  Receive Store and Forward When this bit is set, the MTL reads a frame from the Rx
 *  FIFO only after the complete frame has been written to it, ignoring the RTC bits
 *  When this bit is reset, the Rx FIFO operates in the cutthrough mode, subject to the
 *  threshold specified by the RTC bits
 */
#define GMAC_CH2_RSF    (BIT(25))
#define GMAC_CH2_RSF_M  (GMAC_CH2_RSF_V << GMAC_CH2_RSF_S)
#define GMAC_CH2_RSF_V  0x00000001U
#define GMAC_CH2_RSF_S  25
/** GMAC_CH2_DT : R/W; bitpos: [26]; default: 0;
 *  Disable Dropping of TCP/IP Checksum Error Frames When this bit is set, the MAC does
 *  not drop the frames which only have errors detected by the Receive Checksum Offload
 *  engine Such frames do not have any errors _including FCS error_ in the Ethernet
 *  frame received by the MAC but have errors only in the encapsulated payload When
 *  this bit is reset, all error frames are dropped if the FEF bit is reset If the IPC
 *  Full Checksum Offload Engine _Type 2_ is disabled, this bit is reserved _RO with
 *  value 1'b0_
 */
#define GMAC_CH2_DT    (BIT(26))
#define GMAC_CH2_DT_M  (GMAC_CH2_DT_V << GMAC_CH2_DT_S)
#define GMAC_CH2_DT_V  0x00000001U
#define GMAC_CH2_DT_S  26

/** GMAC_REGISTER135_CHANNEL2INTERRUPTENABLEREGISTER_REG register
 *  Enables the interrupts reported by the Status Register
 */
#define GMAC_REGISTER135_CHANNEL2INTERRUPTENABLEREGISTER_REG (DR_REG_GMAC_BASE + 0x121c)
/** GMAC_CH2_TIE : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Transmit Interrupt is enabled When this bit is reset, the Transmit
 *  Interrupt is disabled The sbd_intr_o interrupt is generated as shown in Figure 61
 *  It is asserted only when the TTI, GPI, GMI, GLI, or GLPII bit of the DMA Status
 *  register is asserted, or when the NIS or AIS Status bit is asserted and the
 *  corresponding Interrupt Enable bits _NIE or AIE_ are enabled
 */
#define GMAC_CH2_TIE    (BIT(0))
#define GMAC_CH2_TIE_M  (GMAC_CH2_TIE_V << GMAC_CH2_TIE_S)
#define GMAC_CH2_TIE_V  0x00000001U
#define GMAC_CH2_TIE_S  0
/** GMAC_CH2_TSE : R/W; bitpos: [1]; default: 0;
 *  Transmit Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Transmission Stopped Interrupt is enabled When this bit is reset, the
 *  Transmission Stopped Interrupt is disabled
 */
#define GMAC_CH2_TSE    (BIT(1))
#define GMAC_CH2_TSE_M  (GMAC_CH2_TSE_V << GMAC_CH2_TSE_S)
#define GMAC_CH2_TSE_V  0x00000001U
#define GMAC_CH2_TSE_S  1
/** GMAC_CH2_TUE : R/W; bitpos: [2]; default: 0;
 *  Transmit Buffer Unavailable Enable When this bit is set with Normal Interrupt
 *  Summary Enable _Bit 16_, the Transmit Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled
 */
#define GMAC_CH2_TUE    (BIT(2))
#define GMAC_CH2_TUE_M  (GMAC_CH2_TUE_V << GMAC_CH2_TUE_S)
#define GMAC_CH2_TUE_V  0x00000001U
#define GMAC_CH2_TUE_S  2
/** GMAC_CH2_TJE : R/W; bitpos: [3]; default: 0;
 *  Transmit Jabber Timeout Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Jabber Timeout Interrupt is enabled When this bit is
 *  reset, the Transmit Jabber Timeout Interrupt is disabled
 */
#define GMAC_CH2_TJE    (BIT(3))
#define GMAC_CH2_TJE_M  (GMAC_CH2_TJE_V << GMAC_CH2_TJE_S)
#define GMAC_CH2_TJE_V  0x00000001U
#define GMAC_CH2_TJE_S  3
/** GMAC_CH2_OVE : R/W; bitpos: [4]; default: 0;
 *  Overflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Receive Overflow Interrupt is enabled When this bit is reset,
 *  the Overflow Interrupt is disabled
 */
#define GMAC_CH2_OVE    (BIT(4))
#define GMAC_CH2_OVE_M  (GMAC_CH2_OVE_V << GMAC_CH2_OVE_S)
#define GMAC_CH2_OVE_V  0x00000001U
#define GMAC_CH2_OVE_S  4
/** GMAC_CH2_UNE : R/W; bitpos: [5]; default: 0;
 *  Underflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
 *  Enable _Bit 15_, the Transmit Underflow Interrupt is enabled When this bit is
 *  reset, the Underflow Interrupt is disabled
 */
#define GMAC_CH2_UNE    (BIT(5))
#define GMAC_CH2_UNE_M  (GMAC_CH2_UNE_V << GMAC_CH2_UNE_S)
#define GMAC_CH2_UNE_V  0x00000001U
#define GMAC_CH2_UNE_S  5
/** GMAC_CH2_RIE : R/W; bitpos: [6]; default: 0;
 *  Receive Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
 *  _Bit 16_, the Receive Interrupt is enabled When this bit is reset, the Receive
 *  Interrupt is disabled
 */
#define GMAC_CH2_RIE    (BIT(6))
#define GMAC_CH2_RIE_M  (GMAC_CH2_RIE_V << GMAC_CH2_RIE_S)
#define GMAC_CH2_RIE_V  0x00000001U
#define GMAC_CH2_RIE_S  6
/** GMAC_CH2_RUE : R/W; bitpos: [7]; default: 0;
 *  Receive Buffer Unavailable Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Buffer Unavailable Interrupt is enabled When
 *  this bit is reset, the Receive Buffer Unavailable Interrupt is disabled
 */
#define GMAC_CH2_RUE    (BIT(7))
#define GMAC_CH2_RUE_M  (GMAC_CH2_RUE_V << GMAC_CH2_RUE_S)
#define GMAC_CH2_RUE_V  0x00000001U
#define GMAC_CH2_RUE_S  7
/** GMAC_CH2_RSE : R/W; bitpos: [8]; default: 0;
 *  Receive Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Receive Stopped Interrupt is enabled When this bit is reset, the
 *  Receive Stopped Interrupt is disabled
 */
#define GMAC_CH2_RSE    (BIT(8))
#define GMAC_CH2_RSE_M  (GMAC_CH2_RSE_V << GMAC_CH2_RSE_S)
#define GMAC_CH2_RSE_V  0x00000001U
#define GMAC_CH2_RSE_S  8
/** GMAC_CH2_RWE : R/W; bitpos: [9]; default: 0;
 *  Receive Watchdog Timeout Enable When this bit is set with Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Receive Watchdog Timeout Interrupt is enabled When
 *  this bit is reset, the Receive Watchdog Timeout Interrupt is disabled
 */
#define GMAC_CH2_RWE    (BIT(9))
#define GMAC_CH2_RWE_M  (GMAC_CH2_RWE_V << GMAC_CH2_RWE_S)
#define GMAC_CH2_RWE_V  0x00000001U
#define GMAC_CH2_RWE_S  9
/** GMAC_CH2_ETE : R/W; bitpos: [10]; default: 0;
 *  Early Transmit Interrupt Enable When this bit is set with an Abnormal Interrupt
 *  Summary Enable _Bit 15_, the Early Transmit Interrupt is enabled When this bit is
 *  reset, the Early Transmit Interrupt is disabled
 */
#define GMAC_CH2_ETE    (BIT(10))
#define GMAC_CH2_ETE_M  (GMAC_CH2_ETE_V << GMAC_CH2_ETE_S)
#define GMAC_CH2_ETE_V  0x00000001U
#define GMAC_CH2_ETE_S  10
/** GMAC_CH2_FBE : R/W; bitpos: [13]; default: 0;
 *  Fatal Bus Error Enable When this bit is set with Abnormal Interrupt Summary Enable
 *  _Bit 15_, the Fatal Bus Error Interrupt is enabled When this bit is reset, the
 *  Fatal Bus Error Enable Interrupt is disabled 12:11		Reserved	00	RO
 */
#define GMAC_CH2_FBE    (BIT(13))
#define GMAC_CH2_FBE_M  (GMAC_CH2_FBE_V << GMAC_CH2_FBE_S)
#define GMAC_CH2_FBE_V  0x00000001U
#define GMAC_CH2_FBE_S  13
/** GMAC_CH2_ERE : R/W; bitpos: [14]; default: 0;
 *  Early Receive Interrupt Enable When this bit is set with Normal Interrupt Summary
 *  Enable _Bit 16_, the Early Receive Interrupt is enabled When this bit is reset, the
 *  Early Receive Interrupt is disabled
 */
#define GMAC_CH2_ERE    (BIT(14))
#define GMAC_CH2_ERE_M  (GMAC_CH2_ERE_V << GMAC_CH2_ERE_S)
#define GMAC_CH2_ERE_V  0x00000001U
#define GMAC_CH2_ERE_S  14
/** GMAC_CH2_AIE : R/W; bitpos: [15]; default: 0;
 *  Abnormal Interrupt Summary Enable When this bit is set, abnormal interrupt summary
 *  is enabled When this bit is reset, the abnormal interrupt summary is disabled This
 *  bit enables the following interrupts in Register 5 _Status Register_: Register
 *  5[1]: Transmit Process Stopped Register 5[3]: Transmit Jabber Timeout Register
 *  5[4]: Receive Overflow Register 5[5]: Transmit Underflow Register 5[7]: Receive
 *  Buffer Unavailable Register 5[8]: Receive Process Stopped Register 5[9]: Receive
 *  Watchdog Timeout Register 5[10]: Early Transmit Interrupt Register 5[13]: Fatal Bus
 *  Error
 */
#define GMAC_CH2_AIE    (BIT(15))
#define GMAC_CH2_AIE_M  (GMAC_CH2_AIE_V << GMAC_CH2_AIE_S)
#define GMAC_CH2_AIE_V  0x00000001U
#define GMAC_CH2_AIE_S  15
/** GMAC_CH2_NIE : R/W; bitpos: [16]; default: 0;
 *  Normal Interrupt Summary Enable When this bit is set, normal interrupt summary is
 *  enabled When this bit is reset, normal interrupt summary is disabled This bit
 *  enables the following interrupts in Register 5 _Status Register_: Register 5[0]:
 *  Transmit Interrupt Register 5[2]: Transmit Buffer Unavailable Register 5[6]:
 *  Receive Interrupt Register 5[14]: Early Receive Interrupt
 */
#define GMAC_CH2_NIE    (BIT(16))
#define GMAC_CH2_NIE_M  (GMAC_CH2_NIE_V << GMAC_CH2_NIE_S)
#define GMAC_CH2_NIE_V  0x00000001U
#define GMAC_CH2_NIE_S  16

/** GMAC_REGISTER136_CHANNEL2MISSEDFRAMEANDBUFFEROVERFLOWCOUNTERREGISTER_REG register
 *  Contains the counters for discarded frames because no host Receive Descriptor was
 *  available or because of Receive FIFO Overflow
 */
#define GMAC_REGISTER136_CHANNEL2MISSEDFRAMEANDBUFFEROVERFLOWCOUNTERREGISTER_REG (DR_REG_GMAC_BASE + 0x1220)
/** GMAC_CH2_MISFRMCNT : R/W; bitpos: [15:0]; default: 0;
 *  Missed Frame Counter This field indicates the number of frames missed by the
 *  controller because of the Host Receive Buffer being unavailable This counter is
 *  incremented each time the DMA discards an incoming frame The counter is cleared
 *  when this register is read with mci_be_i[0] at 1’b1
 */
#define GMAC_CH2_MISFRMCNT    0x0000FFFFU
#define GMAC_CH2_MISFRMCNT_M  (GMAC_CH2_MISFRMCNT_V << GMAC_CH2_MISFRMCNT_S)
#define GMAC_CH2_MISFRMCNT_V  0x0000FFFFU
#define GMAC_CH2_MISFRMCNT_S  0
/** GMAC_CH2_MISCNTOVF : R/W; bitpos: [16]; default: 0;
 *  Overflow Bit for Missed Frame Counter This bit is set every time Missed Frame
 *  Counter _Bits[15:0]_ overflows, that is, the DMA discards an incoming frame because
 *  of the Host Receive Buffer being unavailable with the missed frame counter at
 *  maximum value In such a scenario, the Missed frame counter is reset to allzeros and
 *  this bit indicates that the rollover happened
 */
#define GMAC_CH2_MISCNTOVF    (BIT(16))
#define GMAC_CH2_MISCNTOVF_M  (GMAC_CH2_MISCNTOVF_V << GMAC_CH2_MISCNTOVF_S)
#define GMAC_CH2_MISCNTOVF_V  0x00000001U
#define GMAC_CH2_MISCNTOVF_S  16
/** GMAC_CH2_OVFFRMCNT : R/W; bitpos: [27:17]; default: 0;
 *  Overflow Frame Counter This field indicates the number of frames missed by the
 *  application This counter is incremented each time the MTL FIFO overflows The
 *  counter is cleared when this register is read with mci_be_i[2] at 1’b1
 */
#define GMAC_CH2_OVFFRMCNT    0x000007FFU
#define GMAC_CH2_OVFFRMCNT_M  (GMAC_CH2_OVFFRMCNT_V << GMAC_CH2_OVFFRMCNT_S)
#define GMAC_CH2_OVFFRMCNT_V  0x000007FFU
#define GMAC_CH2_OVFFRMCNT_S  17
/** GMAC_CH2_OVFCNTOVF : R/W; bitpos: [28]; default: 0;
 *  Overflow Bit for FIFO Overflow Counter This bit is set every time the Overflow
 *  Frame Counter _Bits[27:17]_ overflows, that is, the Rx FIFO overflows with the
 *  overflow frame counter at maximum value In such a scenario, the overflow frame
 *  counter is reset to allzeros and this bit indicates that the rollover happened
 */
#define GMAC_CH2_OVFCNTOVF    (BIT(28))
#define GMAC_CH2_OVFCNTOVF_M  (GMAC_CH2_OVFCNTOVF_V << GMAC_CH2_OVFCNTOVF_S)
#define GMAC_CH2_OVFCNTOVF_V  0x00000001U
#define GMAC_CH2_OVFCNTOVF_S  28

/** GMAC_REGISTER137_CHANNEL2RECEIVEINTERRUPTWATCHDOGTIMERREGISTER_REG register
 *  Watchdog timeout for Receive Interrupt _RI_ from DMA
 */
#define GMAC_REGISTER137_CHANNEL2RECEIVEINTERRUPTWATCHDOGTIMERREGISTER_REG (DR_REG_GMAC_BASE + 0x1224)
/** GMAC_CH2_RIWT : R/W; bitpos: [7:0]; default: 0;
 *  RI Watchdog Timer Count This bit indicates the number of system clock cycles
 *  multiplied by 256 for which the watchdog timer is set The watchdog timer gets
 *  triggered with the programmed value after the Rx DMA completes the transfer of a
 *  frame for which the RI status bit is not set because of the setting in the
 *  corresponding descriptor RDES1[31] When the watchdog timer runs out, the RI bit is
 *  set and the timer is stopped The watchdog timer is reset when the RI bit is set
 *  high because of automatic setting of RI as per RDES1[31] of any received frame
 */
#define GMAC_CH2_RIWT    0x000000FFU
#define GMAC_CH2_RIWT_M  (GMAC_CH2_RIWT_V << GMAC_CH2_RIWT_S)
#define GMAC_CH2_RIWT_V  0x000000FFU
#define GMAC_CH2_RIWT_S  0

/** GMAC_REGISTER146_CHANNEL2CURRENTHOSTTRANSMITDESCRIPTORREGISTER_REG register
 *  Points to the start of current Transmit Descriptor read by the DMA
 */
#define GMAC_REGISTER146_CHANNEL2CURRENTHOSTTRANSMITDESCRIPTORREGISTER_REG (DR_REG_GMAC_BASE + 0x1248)
/** GMAC_CH2_CURTDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Descriptor Address Pointer
 */
#define GMAC_CH2_CURTDESAPTR    0xFFFFFFFFU
#define GMAC_CH2_CURTDESAPTR_M  (GMAC_CH2_CURTDESAPTR_V << GMAC_CH2_CURTDESAPTR_S)
#define GMAC_CH2_CURTDESAPTR_V  0xFFFFFFFFU
#define GMAC_CH2_CURTDESAPTR_S  0

/** GMAC_REGISTER147_CHANNEL2CURRENTHOSTRECEIVEDESCRIPTORREGISTER_REG register
 *  Points to the start of current Receive Descriptor read by the DMA
 */
#define GMAC_REGISTER147_CHANNEL2CURRENTHOSTRECEIVEDESCRIPTORREGISTER_REG (DR_REG_GMAC_BASE + 0x124c)
/** GMAC_CH2_CURRDESAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Descriptor Address Pointer
 */
#define GMAC_CH2_CURRDESAPTR    0xFFFFFFFFU
#define GMAC_CH2_CURRDESAPTR_M  (GMAC_CH2_CURRDESAPTR_V << GMAC_CH2_CURRDESAPTR_S)
#define GMAC_CH2_CURRDESAPTR_V  0xFFFFFFFFU
#define GMAC_CH2_CURRDESAPTR_S  0

/** GMAC_REGISTER148_CHANNEL2CURRENTHOSTTRANSMITBUFFERADDRESSREGISTER_REG register
 *  Points to the current Transmit Buffer address read by the DMA
 */
#define GMAC_REGISTER148_CHANNEL2CURRENTHOSTTRANSMITBUFFERADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x1250)
/** GMAC_CH2_CURTBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Transmit Buffer Address Pointer
 */
#define GMAC_CH2_CURTBUFAPTR    0xFFFFFFFFU
#define GMAC_CH2_CURTBUFAPTR_M  (GMAC_CH2_CURTBUFAPTR_V << GMAC_CH2_CURTBUFAPTR_S)
#define GMAC_CH2_CURTBUFAPTR_V  0xFFFFFFFFU
#define GMAC_CH2_CURTBUFAPTR_S  0

/** GMAC_REGISTER149_CHANNEL2CURRENTHOSTRECEIVEBUFFERADDRESSREGISTER_REG register
 *  Points to the current Receive Buffer address read by the DMA
 */
#define GMAC_REGISTER149_CHANNEL2CURRENTHOSTRECEIVEBUFFERADDRESSREGISTER_REG (DR_REG_GMAC_BASE + 0x1254)
/** GMAC_CH2_CURRBUFAPTR : RO; bitpos: [31:0]; default: 0;
 *  Host Receive Buffer Address Pointer
 */
#define GMAC_CH2_CURRBUFAPTR    0xFFFFFFFFU
#define GMAC_CH2_CURRBUFAPTR_M  (GMAC_CH2_CURRBUFAPTR_V << GMAC_CH2_CURRBUFAPTR_S)
#define GMAC_CH2_CURRBUFAPTR_V  0xFFFFFFFFU
#define GMAC_CH2_CURRBUFAPTR_S  0

/** GMAC_REGISTER152_CHANNEL2CBSCONTROLREGISTER_REG register
 *  Controls the Channel 2 credit shaping operation on the transmit path
 */
#define GMAC_REGISTER152_CHANNEL2CBSCONTROLREGISTER_REG (DR_REG_GMAC_BASE + 0x1260)
/** GMAC_CH2_CBSD : R/W; bitpos: [0]; default: 0;
 *  CreditBased Shaper Disable When set, the MAC disables the creditbased shaper
 *  algorithm for Channel 2 traffic and makes the traffic management algorithm to
 *  strict priority for Channel 2 over Channel 0 When reset, the creditbased shaper
 *  algorithm schedules the traffic in Channel 2 for transmission
 */
#define GMAC_CH2_CBSD    (BIT(0))
#define GMAC_CH2_CBSD_M  (GMAC_CH2_CBSD_V << GMAC_CH2_CBSD_S)
#define GMAC_CH2_CBSD_V  0x00000001U
#define GMAC_CH2_CBSD_S  0
/** GMAC_CH2_CC : R/W; bitpos: [1]; default: 0;
 *  Credit Control When reset, the accumulated credit parameter in the creditbased
 *  shaper algorithm logic is set to zero when there is positive credit and no frame to
 *  transmit in Channel 2 When there is no frame waiting in Channel 2 and other channel
 *  is transmitting, no credit is accumulated When set, the accumulated credit
 *  parameter in the creditbased shaper algorithm logic is not reset to zero when there
 *  is positive credit and no frame to transmit in Channel 2 The credit accumulates
 *  even when there is no frame waiting in Channel 2 and another channel is transmitting
 */
#define GMAC_CH2_CC    (BIT(1))
#define GMAC_CH2_CC_M  (GMAC_CH2_CC_V << GMAC_CH2_CC_S)
#define GMAC_CH2_CC_V  0x00000001U
#define GMAC_CH2_CC_S  1
/** GMAC_CH2_SLC : R/W; bitpos: [6:4]; default: 0;
 *  Slot Count The software can program the number of slots _of duration 125 microsec_
 *  over which the average transmitted bits per slot _provided in the CBS Status
 *  register_ need to be computed for Channel 2 when the creditbased shaper algorithm
 *  is enabled The encoding is as follows: 3'b000: 1 Slot 3'b001: 2 Slots 3'b010: 4
 *  Slots 3'b011: 8 Slots 3'b100: 16 Slots 3'b1013'b111: Reserved
 */
#define GMAC_CH2_SLC    0x00000007U
#define GMAC_CH2_SLC_M  (GMAC_CH2_SLC_V << GMAC_CH2_SLC_S)
#define GMAC_CH2_SLC_V  0x00000007U
#define GMAC_CH2_SLC_S  4
/** GMAC_CH2_ABPSSIE : R/W; bitpos: [17]; default: 0;
 *  Average Bits Per Slot Interrupt Enable When this bit is set, the MAC asserts an
 *  interrupt _sbd_intr_o or mci_intr_o_ when the average bits per slot status is
 *  updated _Bit 17 _ABSU_ in Register 153_ for Channel 2 When this bit is cleared,
 *  interrupt is not asserted for such an event
 */
#define GMAC_CH2_ABPSSIE    (BIT(17))
#define GMAC_CH2_ABPSSIE_M  (GMAC_CH2_ABPSSIE_V << GMAC_CH2_ABPSSIE_S)
#define GMAC_CH2_ABPSSIE_V  0x00000001U
#define GMAC_CH2_ABPSSIE_S  17

/** GMAC_REGISTER153_CHANNEL2CBSSTATUSREGISTER_REG register
 *  Provides the average traffic transmitted in Channel 2
 */
#define GMAC_REGISTER153_CHANNEL2CBSSTATUSREGISTER_REG (DR_REG_GMAC_BASE + 0x1264)
/** GMAC_CH2_ABS : RO; bitpos: [16:0]; default: 0;
 *  Average Bits per Slot This field contains the average transmitted bits per slot
 *  This field is computed over programmed number of slots _SLC bits in the CBS Control
 *  Register_ for Channel 2 traffic The maximum value is 0x30D4 for 100 Mbps and
 *  0x1E848 for 1000 Mbps
 */
#define GMAC_CH2_ABS    0x0001FFFFU
#define GMAC_CH2_ABS_M  (GMAC_CH2_ABS_V << GMAC_CH2_ABS_S)
#define GMAC_CH2_ABS_V  0x0001FFFFU
#define GMAC_CH2_ABS_S  0
/** GMAC_CH2_ABSU : RO; bitpos: [17]; default: 0;
 *  ABS Updated When set, this bit indicates that the MAC has updated the ABS value
 *  This bit is cleared when the application reads the ABS value
 */
#define GMAC_CH2_ABSU    (BIT(17))
#define GMAC_CH2_ABSU_M  (GMAC_CH2_ABSU_V << GMAC_CH2_ABSU_S)
#define GMAC_CH2_ABSU_V  0x00000001U
#define GMAC_CH2_ABSU_S  17

/** GMAC_REGISTER154_CHANNEL2IDLESLOPECREDITREGISTER_REG register
 *  Contains the idleSlope credit value required for the creditbased shaper algorithm
 *  for Channel 2
 */
#define GMAC_REGISTER154_CHANNEL2IDLESLOPECREDITREGISTER_REG (DR_REG_GMAC_BASE + 0x1268)
/** GMAC_CH2_ISC : R/W; bitpos: [13:0]; default: 0;
 *  idleSlopeCredit This field contains the idleSlopeCredit value required for the
 *  creditbased shaper algorithm for Channel 2 This is the rate of change of credit in
 *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
 *  credit is increasing The software should program this field with computed credit in
 *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
 *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode
 */
#define GMAC_CH2_ISC    0x00003FFFU
#define GMAC_CH2_ISC_M  (GMAC_CH2_ISC_V << GMAC_CH2_ISC_S)
#define GMAC_CH2_ISC_V  0x00003FFFU
#define GMAC_CH2_ISC_S  0

/** GMAC_REGISTER155_CHANNEL2SENDSLOPECREDITREGISTER_REG register
 *  Contains the sendSlope credit value required for the creditbased shaper algorithm
 *  for Channel 2
 */
#define GMAC_REGISTER155_CHANNEL2SENDSLOPECREDITREGISTER_REG (DR_REG_GMAC_BASE + 0x126c)
/** GMAC_CH2_SSC : R/W; bitpos: [13:0]; default: 0;
 *  sendSlopeCredit This field contains the sendSlopeCredit value required for
 *  creditbased shaper algorithm for Channel 2 This is the rate of change of credit in
 *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
 *  credit is decreasing The software should program this field with computed credit in
 *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
 *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode This field should be
 *  programmed with absolute sendSlopeCredit value The creditbased shaper logic
 *  subtracts it from the accumulated credit when Channel 2 is selected for transmission
 */
#define GMAC_CH2_SSC    0x00003FFFU
#define GMAC_CH2_SSC_M  (GMAC_CH2_SSC_V << GMAC_CH2_SSC_S)
#define GMAC_CH2_SSC_V  0x00003FFFU
#define GMAC_CH2_SSC_S  0

/** GMAC_REGISTER156_CHANNEL2HICREDITREGISTER_REG register
 *  Contains the hiCredit value required for the creditbased shaper algorithm for
 *  Channel 2
 */
#define GMAC_REGISTER156_CHANNEL2HICREDITREGISTER_REG (DR_REG_GMAC_BASE + 0x1270)
/** GMAC_CH2_HC : R/W; bitpos: [28:0]; default: 0;
 *  hiCredit This field contains the hiCredit value required for the creditbased shaper
 *  algorithm for Channel 2 This is the maximum value that can be accumulated in the
 *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
 *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
 *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
 *  0x0800_0000
 */
#define GMAC_CH2_HC    0x1FFFFFFFU
#define GMAC_CH2_HC_M  (GMAC_CH2_HC_V << GMAC_CH2_HC_S)
#define GMAC_CH2_HC_V  0x1FFFFFFFU
#define GMAC_CH2_HC_S  0

/** GMAC_REGISTER157_CHANNEL2LOCREDITREGISTER_REG register
 *  Contains the loCredit value required for the creditbased shaper algorithm for
 *  Channel 2
 */
#define GMAC_REGISTER157_CHANNEL2LOCREDITREGISTER_REG (DR_REG_GMAC_BASE + 0x1274)
/** GMAC_CH2_LC : R/W; bitpos: [28:0]; default: 536870911;
 *  loCredit This field contains the loCredit value required for the creditbased shaper
 *  algorithm for Channel 2 This is the minimum value that can be accumulated in the
 *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
 *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
 *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
 *  0x0800_0000 The programmed value is 2's complement _negative number_, that is,
 *  0xF800_0000
 */
#define GMAC_CH2_LC    0x1FFFFFFFU
#define GMAC_CH2_LC_M  (GMAC_CH2_LC_V << GMAC_CH2_LC_S)
#define GMAC_CH2_LC_V  0x1FFFFFFFU
#define GMAC_CH2_LC_S  0

#ifdef __cplusplus
}
#endif
