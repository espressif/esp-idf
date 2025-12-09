/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: gmac register */
/** Type of register0_macconfigurationregister register
 *  This is the operation mode register for the MAC
 */
typedef union {
    struct {
        /** prelen : R/W; bitpos: [1:0]; default: 0;
         *  Preamble Length for Transmit frames These bits control the number of preamble bytes
         *  that are added to the beginning of every Transmit frame The preamble reduction
         *  occurs only when the MAC is operating in the fullduplex mode 2'b00: 7 bytes of
         *  preamble 2'b01: 5 bytes of preamble 2'b10: 3 bytes of preamble 2'b11: Reserved
         */
        uint32_t prelen:2;
        /** re : R/W; bitpos: [2]; default: 0;
         *  Receiver Enable When this bit is set, the receiver state machine of the MAC is
         *  enabled for receiving frames from the GMII or MII When this bit is reset, the MAC
         *  receive state machine is disabled after the completion of the reception of the
         *  current frame, and does not receive any further frames from the GMII or MII
         */
        uint32_t re:1;
        /** ten : R/W; bitpos: [3]; default: 0;
         *  Transmitter Enable When this bit is set, the transmit state machine of the MAC is
         *  enabled for transmission on the GMII or MII When this bit is reset, the MAC
         *  transmit state machine is disabled after the completion of the transmission of the
         *  current frame, and does not transmit any further frames
         */
        uint32_t ten:1;
        /** dc : R/W; bitpos: [4]; default: 0;
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
        uint32_t dc:1;
        /** bl : R/W; bitpos: [6:5]; default: 0;
         *  BackOff Limit The BackOff limit determines the random integer number _r_ of slot
         *  time delays _4,096 bit times for 1000 Mbps and 512 bit times for 10/100 Mbps_ for
         *  which the MAC waits before rescheduling a transmission attempt during retries after
         *  a collision This bit is applicable only in the halfduplex mode and is reserved _RO_
         *  in the fullduplexonly configuration 00: k= min _n, 10_ 01: k = min _n, 8_ 10: k =
         *  min _n, 4_ 11: k = min _n, 1_ where n = retransmission attempt The random integer r
         *  takes the value in the range 0 ≤ r < 2k
         */
        uint32_t bl:2;
        /** acs : R/W; bitpos: [7]; default: 0;
         *  Automatic Pad or CRC Stripping When this bit is set, the MAC strips the Pad or FCS
         *  field on the incoming frames only if the value of the length field is less than
         *  1,536 bytes All received frames with length field greater than or equal to 1,536
         *  bytes are passed to the application without stripping the Pad or FCS field When
         *  this bit is reset, the MAC passes all incoming frames, without modifying them, to
         *  the Host Note: For information about how the settings of Bit 25 _CST_ and this bit
         *  impact the frame length, see Table 632
         */
        uint32_t acs:1;
        /** lud : R/W; bitpos: [8]; default: 0;
         *  Link Up or Down This bit indicates whether the link is up or down during the
         *  transmission of configuration in the RGMII, SGMII, or SMII interface: 0: Link Down
         *  1: Link Up This bit is reserved _RO with default value_ and is enabled when the
         *  RGMII, SGMII, or SMII interface is enabled during core configuration
         */
        uint32_t lud:1;
        /** dr : R/W; bitpos: [9]; default: 0;
         *  Disable Retry When this bit is set, the MAC attempts only one transmission When a
         *  collision occurs on the GMII or MII interface, the MAC ignores the current frame
         *  transmission and reports a Frame Abort with excessive collision error in the
         *  transmit frame status When this bit is reset, the MAC attempts retries based on the
         *  settings of the BL field _Bits [6:5]_ This bit is applicable only in the halfduplex
         *  mode and is reserved _RO with default value_ in the fullduplexonly configuration
         */
        uint32_t dr:1;
        /** ipc : R/W; bitpos: [10]; default: 0;
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
        uint32_t ipc:1;
        /** dm : R/W; bitpos: [11]; default: 0;
         *  Duplex Mode When this bit is set, the MAC operates in the fullduplex mode where it
         *  can transmit and receive simultaneously This bit is RO with default value of 1'b1
         *  in the fullduplexonly configuration
         */
        uint32_t dm:1;
        /** lm : R/W; bitpos: [12]; default: 0;
         *  Loopback Mode When this bit is set, the MAC operates in the loopback mode at GMII
         *  or MII The _G_MII Receive clock input _clk_rx_i_ is required for the loopback to
         *  work properly, because the Transmit clock is not loopedback internally
         */
        uint32_t lm:1;
        /** do : R/W; bitpos: [13]; default: 0;
         *  Disable Receive Own When this bit is set, the MAC disables the reception of frames
         *  when the phy_txen_o is asserted in the halfduplex mode When this bit is reset, the
         *  MAC receives all packets that are given by the PHY while transmitting This bit is
         *  not applicable if the MAC is operating in the fullduplex mode This bit is reserved
         *  _RO with default value_ if the MAC is configured for the fullduplexonly operation
         */
        uint32_t dro:1;
        /** fes : R/W; bitpos: [14]; default: 0;
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
        uint32_t fes:1;
        /** ps : R/W; bitpos: [15]; default: 0;
         *  Port Select This bit selects the Ethernet line speed 0: For 1000 Mbps operations 1:
         *  For 10 or 100 Mbps operations In 10 or 100 Mbps operations, this bit, along with
         *  FES bit, selects the exact line speed In the 10/100 Mbpsonly _always 1_ or 1000
         *  Mbpsonly _always 0_ configurations, this bit is readonly with the appropriate value
         *  In default 10/100/1000 Mbps configuration, this bit is R_W The mac_portselect_o or
         *  mac_speed_o[1] signal reflects the value of this bit
         */
        uint32_t ps:1;
        /** dcrs : R/W; bitpos: [16]; default: 0;
         *  Disable Carrier Sense During Transmission When set high, this bit makes the MAC
         *  transmitter ignore the _G_MII CRS signal during frame transmission in the
         *  halfduplex mode This request results in no errors generated because of Loss of
         *  Carrier or No Carrier during such transmission When this bit is low, the MAC
         *  transmitter generates such errors because of Carrier Sense and can even abort the
         *  transmissions This bit is reserved _and RO_ in the fullduplexonly configurations
         */
        uint32_t dcrs:1;
        /** ifg : R/W; bitpos: [19:17]; default: 0;
         *  InterFrame Gap These bits control the minimum IFG between frames during
         *  transmission 000: 96 bit times 001: 88 bit times 010: 80 bit times  111: 40 bit
         *  times In the halfduplex mode, the minimum IFG can be configured only for 64 bit
         *  times _IFG = 100_ Lower values are not considered In the 1000Mbps mode, the minimum
         *  IFG supported is 64 bit times _and above_ in the GMACCORE configuration and 80 bit
         *  times _and above_ in other configurations When a JAM pattern is being transmitted
         *  because of backpressure activation, the MAC does not consider the minimum IFG
         */
        uint32_t ifg:3;
        /** je : R/W; bitpos: [20]; default: 0;
         *  Jumbo Frame Enable When this bit is set, the MAC allows Jumbo frames of 9,018 bytes
         *  _9,022 bytes for VLAN tagged frames_ without reporting a giant frame error in the
         *  receive frame status
         */
        uint32_t je:1;
        /** be : R/W; bitpos: [21]; default: 0;
         *  Frame Burst Enable When this bit is set, the MAC allows frame bursting during
         *  transmission in the GMII halfduplex mode This bit is reserved _and RO_ in the
         *  10/100 Mbps only or fullduplexonly configurations
         */
        uint32_t be:1;
        /** jd : R/W; bitpos: [22]; default: 0;
         *  Jabber Disable When this bit is set, the MAC disables the jabber timer on the
         *  transmitter The MAC can transfer frames of up to 16,383 bytes When this bit is
         *  reset, the MAC cuts off the transmitter if the application sends out more than
         *  2,048 bytes of data _10,240 if JE is set high_ during transmission
         */
        uint32_t jd:1;
        /** wd : R/W; bitpos: [23]; default: 0;
         *  Watchdog Disable When this bit is set, the MAC disables the watchdog timer on the
         *  receiver The MAC can receive frames of up to 16,383 bytes When this bit is reset,
         *  the MAC does not allow a receive frame which more than 2,048 bytes _10,240 if JE is
         *  set high_ or the value programmed in Register 55 _Watchdog Timeout Register_ The
         *  MAC cuts off any bytes received after the watchdog limit number of bytes
         */
        uint32_t wd:1;
        /** tc : R/W; bitpos: [24]; default: 0;
         *  Transmit Configuration in RGMII, SGMII, or SMII When set, this bit enables the
         *  transmission of duplex mode, link speed, and link up or down information to the PHY
         *  in the RGMII, SMII, or SGMII port When this bit is reset, no such information is
         *  driven to the PHY This bit is reserved _and RO_ if the RGMII, SMII, or SGMII PHY
         *  port is not selected during core configuration The details of this feature are
         *  explained in the following sections: “Reduced Gigabit Media Independent Interface”
         *  on page 249 “Serial Media Independent Interface” on page 245 “Serial Gigabit Media
         *  Independent Interface” on page 257
         */
        uint32_t tc:1;
        /** cst : R/W; bitpos: [25]; default: 0;
         *  CRC Stripping for Type Frames When this bit is set, the last 4 bytes _FCS_ of all
         *  frames of Ether type _Length/Type field greater than or equal to 1,536_ are
         *  stripped and dropped before forwarding the frame to the application This function
         *  is not valid when the IP Checksum Engine _Type 1_ is enabled in the MAC receiver
         *  This function is valid when Type 2 Checksum Offload Engine is enabled Note: For
         *  information about how the settings of Bit 7 _ACS_ and this bit impact the frame
         *  length, see Table 632
         */
        uint32_t cst:1;
        /** sfterr : R/W; bitpos: [26]; default: 0;
         *  SMII Force Transmit Error When set, this bit indicates to the PHY to force a
         *  transmit error in the SMII frame being transmitted This bit is reserved if the SMII
         *  PHY port is not selected during core configuration
         */
        uint32_t sfterr:1;
        /** twokpe : R/W; bitpos: [27]; default: 0;
         *  IEEE 8023as Support for 2K Packets When set, the MAC considers all frames, with up
         *  to 2,000 bytes length, as normal packets When Bit 20 _JE_ is not set, the MAC
         *  considers all received frames of size more than 2K bytes as Giant frames When this
         *  bit is reset and Bit 20 _JE_ is not set, the MAC considers all received frames of
         *  size more than 1,518 bytes _1,522 bytes for tagged_ as Giant frames When Bit 20 is
         *  set, setting this bit has no effect on Giant Frame status For more information
         *  about how the setting of this bit and Bit 20 impact the Giant frame status, see
         *  Table 631
         */
        uint32_t twokpe:1;
        /** sarc : R/W; bitpos: [30:28]; default: 0;
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
        uint32_t sarc:3;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} gmac_register0_macconfigurationregister_reg_t;

/** Type of register1_macframefilter register
 *  Contains the frame filtering controls
 */
typedef union {
    struct {
        /** promiscuous_mode : R/W; bitpos: [0]; default: 0;
         *  Promiscuous Mode When this bit is set, the Address Filter module passes all
         *  incoming frames irrespective of the destination or source address The SA or DA
         *  Filter Fails status bits of the Receive Status Word are always cleared when PR is
         *  set
         */
        uint32_t promiscuous_mode:1;
        /** huc : R/W; bitpos: [1]; default: 0;
         *  Hash Unicast When set, the MAC performs destination address filtering of unicast
         *  frames according to the hash table When reset, the MAC performs a perfect
         *  destination address filtering for unicast frames, that is, it compares the DA field
         *  with the values programmed in DA registers If Hash Filter is not selected during
         *  core configuration, this bit is reserved _and RO_
         */
        uint32_t huc:1;
        /** hmc : R/W; bitpos: [2]; default: 0;
         *  Hash Multicast When set, the MAC performs destination address filtering of received
         *  multicast frames according to the hash table When reset, the MAC performs a perfect
         *  destination address filtering for multicast frames, that is, it compares the DA
         *  field with the values programmed in DA registers If Hash Filter is not selected
         *  during core configuration, this bit is reserved _and RO_
         */
        uint32_t hmc:1;
        /** daif : R/W; bitpos: [3]; default: 0;
         *  DA Inverse Filtering When this bit is set, the Address Check block operates in
         *  inverse filtering mode for the DA address comparison for both unicast and multicast
         *  frames When reset, normal filtering of frames is performed
         */
        uint32_t daif:1;
        /** pm : R/W; bitpos: [4]; default: 0;
         *  Pass All Multicast When set, this bit indicates that all received frames with a
         *  multicast destination address _first bit in the destination address field is '1'_
         *  are passed When reset, filtering of multicast frame depends on HMC bit
         */
        uint32_t pm:1;
        /** dbf : R/W; bitpos: [5]; default: 0;
         *  Disable Broadcast Frames When this bit is set, the AFM module blocks all incoming
         *  broadcast frames In addition, it overrides all other filter settings When this bit
         *  is reset, the AFM module passes all received broadcast frames
         */
        uint32_t dbf:1;
        /** pcf : R/W; bitpos: [7:6]; default: 0;
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
        uint32_t pcf:2;
        /** saif : R/W; bitpos: [8]; default: 0;
         *  SA Inverse Filtering When this bit is set, the Address Check block operates in
         *  inverse filtering mode for the SA address comparison The frames whose SA matches
         *  the SA registers are marked as failing the SA Address filter When this bit is
         *  reset, frames whose SA does not match the SA registers are marked as failing the SA
         *  Address filter
         */
        uint32_t saif:1;
        /** saf : R/W; bitpos: [9]; default: 0;
         *  Source Address Filter Enable When this bit is set, the MAC compares the SA field of
         *  the received frames with the values programmed in the enabled SA registers If the
         *  comparison fails, the MAC drops the frame When this bit is reset, the MAC forwards
         *  the received frame to the application with updated SAF bit of the Rx Status
         *  depending on the SA address comparison Note: According to the IEEE specification,
         *  Bit 47 of the SA is reserved and set to 0 However, in DWC_gmac, the MAC compares
         *  all 48 bits The software driver should take this into consideration while
         *  programming the MAC address registers for SA
         */
        uint32_t saf:1;
        /** hpf : R/W; bitpos: [10]; default: 0;
         *  Hash or Perfect Filter When this bit is set, it configures the address filter to
         *  pass a frame if it matches either the perfect filtering or the hash filtering as
         *  set by the HMC or HUC bits When this bit is low and the HUC or HMC bit is set, the
         *  frame is passed only if it matches the Hash filter This bit is reserved _and RO_ if
         *  the Hash filter is not selected during core configuration
         */
        uint32_t hpf:1;
        uint32_t reserved_11:5;
        /** vtfe : R/W; bitpos: [16]; default: 0;
         *  VLAN Tag Filter Enable When set, this bit enables the MAC to drop VLAN tagged
         *  frames that do not match the VLAN Tag comparison When reset, the MAC forwards all
         *  frames irrespective of the match status of the VLAN Tag
         */
        uint32_t vtfe:1;
        uint32_t reserved_17:3;
        /** ipfe : R/W; bitpos: [20]; default: 0;
         *  Layer 3 and Layer 4 Filter Enable When set, this bit enables the MAC to drop frames
         *  that do not match the enabled Layer 3 and Layer 4 filters If Layer 3 or Layer 4
         *  filters are not enabled for matching, this bit does not have any effect When reset,
         *  the MAC forwards all frames irrespective of the match status of the Layer 3 and
         *  Layer 4 fields If the Layer 3 and Layer 4 Filtering feature is not selected during
         *  core configuration, this bit is reserved _RO with default value_
         */
        uint32_t ipfe:1;
        /** dntu : R/W; bitpos: [21]; default: 0;
         *  Drop nonTCP/UDP over IP Frames When set, this bit enables the MAC to drop the
         *  nonTCP or UDP over IP frames The MAC forward only those frames that are processed
         *  by the Layer 4 filter When reset, this bit enables the MAC to forward all nonTCP or
         *  UDP over IP frames If the Layer 3 and Layer 4 Filtering feature is not selected
         *  during core configuration, this bit is reserved _RO with default value_
         */
        uint32_t dntu:1;
        uint32_t reserved_22:9;
        /** ra : R/W; bitpos: [31]; default: 0;
         *  Receive All When this bit is set, the MAC Receiver module passes all received
         *  frames, irrespective of whether they pass the address filter or not, to the
         *  Application The result of the SA or DA filtering is updated _pass or fail_ in the
         *  corresponding bits in the Receive Status Word When this bit is reset, the Receiver
         *  module passes only those frames to the Application that pass the SA or DA address
         *  filter
         */
        uint32_t ra:1;
    };
    uint32_t val;
} gmac_register1_macframefilter_reg_t;

/** Type of register2_hashtablehighregister register
 *  Contains the higher 32 bits of the Multicast Hash table This register is present
 *  only when you select the 64bit Hash filter function in coreConsultant _See Table 79_
 */
typedef union {
    struct {
        /** hth : R/W; bitpos: [31:0]; default: 0;
         *  Hash Table High This field contains the upper 32 bits of the Hash table
         */
        uint32_t hth:32;
    };
    uint32_t val;
} gmac_register2_hashtablehighregister_reg_t;

/** Type of register3_hashtablelowregister register
 *  Contains the lower 32 bits of the Multicast Hash table This register is present
 *  only when you select the Hash filter function in coreConsultant _See Table 79_
 */
typedef union {
    struct {
        /** htl : R/W; bitpos: [31:0]; default: 0;
         *  Hash Table Low This field contains the lower 32 bits of the Hash table
         */
        uint32_t htl:32;
    };
    uint32_t val;
} gmac_register3_hashtablelowregister_reg_t;

/** Type of register4_gmiiaddressregister register
 *  Controls the management cycles to an external PHY This register is present only
 *  when you select the Station Management _MDIO_ feature in coreConsultant _See Table
 *  726_
 */
typedef union {
    struct {
        /** gb : R/W1S; bitpos: [0]; default: 0;
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
        uint32_t gb:1;
        /** gw : R/W; bitpos: [1]; default: 0;
         *  GMII Write When set, this bit indicates to the PHY or RevMII that this is a Write
         *  operation using the GMII Data register If this bit is not set, it indicates that
         *  this is a Read operation, that is, placing the data in the GMII Data register
         */
        uint32_t gw:1;
        /** cr : R/W; bitpos: [5:2]; default: 0;
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
        uint32_t cr:4;
        /** gr : R/W; bitpos: [10:6]; default: 0;
         *  GMII Register These bits select the desired GMII register in the selected PHY
         *  device For RevMII, these bits select the desired CSR register in the RevMII
         *  Registers set
         */
        uint32_t gr:5;
        /** pa : R/W; bitpos: [15:11]; default: 0;
         *  Physical Layer Address This field indicates which of the 32 possible PHY devices
         *  are being accessed For RevMII, this field gives the PHY Address of the RevMII module
         */
        uint32_t pa:5;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gmac_register4_gmiiaddressregister_reg_t;

/** Type of register5_gmiidataregister register
 *  Contains the data to be written to or read from the PHY register This register is
 *  present only when you select the Station Management _MDIO_ feature in
 *  coreConsultant _See Table 726_
 */
typedef union {
    struct {
        /** gd : R/W; bitpos: [15:0]; default: 0;
         *  GMII Data This field contains the 16bit data value read from the PHY or RevMII
         *  after a Management Read operation or the 16bit data value to be written to the PHY
         *  or RevMII before a Management Write operation
         */
        uint32_t gd:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gmac_register5_gmiidataregister_reg_t;

/** Type of register6_flowcontrolregister register
 *  Controls the generation of control frames
 */
typedef union {
    struct {
        /** fcb_bpa : R/W1S; bitpos: [0]; default: 0;
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
        uint32_t fcb_bpa:1;
        /** tfe : R/W; bitpos: [1]; default: 0;
         *  Transmit Flow Control Enable In the fullduplex mode, when this bit is set, the MAC
         *  enables the flow control operation to transmit Pause frames When this bit is reset,
         *  the flow control operation in the MAC is disabled, and the MAC does not transmit
         *  any Pause frames In the halfduplex mode, when this bit is set, the MAC enables the
         *  backpressure operation When this bit is reset, the backpressure feature is disabled
         */
        uint32_t tfe:1;
        /** receive_flow_ctrl_e : R/W; bitpos: [2]; default: 0;
         *  Receive Flow Control Enable When this bit is set, the MAC decodes the received
         *  Pause frame and disables its transmitter for a specified _Pause_ time When this bit
         *  is reset, the decode function of the Pause frame is disabled
         */
        uint32_t receive_flow_ctrl_e:1;
        /** up : R/W; bitpos: [3]; default: 0;
         *  Unicast Pause Frame Detect A pause frame is processed when it has the unique
         *  multicast address specified in the IEEE Std 8023 When this bit is set, the MAC can
         *  also detect Pause frames with unicast address of the station This unicast address
         *  should be as specified in the MAC Address0 High Register and MAC Address0 Low
         *  Register When this bit is reset, the MAC only detects Pause frames with unique
         *  multicast address Note: The MAC does not process a Pause frame if the multicast
         *  address of received frame is different from the unique multicast address
         */
        uint32_t up:1;
        /** plt : R/W; bitpos: [5:4]; default: 0;
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
        uint32_t plt:2;
        uint32_t reserved_6:1;
        /** dzpq : R/W; bitpos: [7]; default: 0;
         *  Disable ZeroQuanta Pause When this bit is set, it disables the automatic generation
         *  of the ZeroQuanta Pause frames on the deassertion of the flowcontrol signal from
         *  the FIFO layer _MTL or external sideband flow control signal
         *  sbd_flowctrl_i/mti_flowctrl_i_ When this bit is reset, normal operation with
         *  automatic ZeroQuanta Pause frame generation is enabled
         */
        uint32_t dzpq:1;
        uint32_t reserved_8:8;
        /** pt : R/W; bitpos: [31:16]; default: 0;
         *  Pause Time This field holds the value to be used in the Pause Time field in the
         *  transmit control frame If the Pause Time bits is configured to be
         *  doublesynchronized to the _G_MII clock domain, then consecutive writes to this
         *  register should be performed only after at least four clock cycles in the
         *  destination clock domain
         */
        uint32_t pt:16;
    };
    uint32_t val;
} gmac_register6_flowcontrolregister_reg_t;

/** Type of register7_vlantagregister register
 *  Identifies IEEE 8021Q VLAN type frames
 */
typedef union {
    struct {
        /** vl : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t vl:16;
        /** etv : R/W; bitpos: [16]; default: 0;
         *  Enable 12-Bit VLAN Tag Comparison
         */
        uint32_t etv:1;
        /** vtim : R/W; bitpos: [17]; default: 0;
         *  VLAN Tag Inverse Match Enable When set, this bit enables the VLAN Tag inverse
         *  matching The frames that do not have matching VLAN Tag are marked as matched When
         *  reset, this bit enables the VLAN Tag perfect matching The frames with matched VLAN
         *  Tag are marked as matched
         */
        uint32_t vtim:1;
        /** esvl : R/W; bitpos: [18]; default: 0;
         *  Enable SVLAN When this bit is set, the MAC transmitter and receiver also consider
         *  the SVLAN _Type = 0x88A8_ frames as valid VLAN tagged frames
         */
        uint32_t esvl:1;
        /** vthm : R/W; bitpos: [19]; default: 0;
         *  VLAN Tag Hash Table Match Enable When set, the most significant four bits of the
         *  VLAN tag’s CRC are used to index the content of Register 354 _VLAN Hash Table
         *  Register_ A value of 1 in the VLAN Hash Table register, corresponding to the index,
         *  indicates that the frame matched the VLAN hash table When Bit 16 _ETV_ is set, the
         *  CRC of the 12bit VLAN Identifier _VID_ is used for comparison whereas when ETV is
         *  reset, the CRC of the 16bit VLAN tag is used for comparison When reset, the VLAN
         *  Hash Match operation is not performed If the VLAN Hash feature is not enabled
         *  during core configuration, this bit is reserved _RO with default value_
         */
        uint32_t vthm:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} gmac_register7_vlantagregister_reg_t;

/** Type of register8_versionregister register
 *  Identifies the version of the Core
 */
typedef union {
    struct {
        /** snpsver : RO; bitpos: [7:0]; default: 55;
         *  Synopsysdefined Version _37_
         */
        uint32_t snpsver:8;
        /** userver : RO; bitpos: [15:8]; default: 0;
         *  Userdefined Version _configured with coreConsultant_
         */
        uint32_t userver:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gmac_register8_versionregister_reg_t;

/** Type of register9_debugregister register
 *  Gives the status of various internal blocks for debugging
 */
typedef union {
    struct {
        /** rpests : RO; bitpos: [0]; default: 0;
         *  MAC GMII or MII Receive Protocol Engine Status When high, this bit indicates that
         *  the MAC GMII or MII receive protocol engine is actively receiving data and not in
         *  IDLE state
         */
        uint32_t rpests:1;
        /** rfcfcsts : RO; bitpos: [2:1]; default: 0;
         *  MAC Receive Frame FIFO Controller Status When high, this field indicates the active
         *  state of the small FIFO Read and Write controllers of the MAC Receive Frame
         *  Controller Module RFCFCSTS[1] represents the status of small FIFO Read controller
         *  RFCFCSTS[0] represents the status of small FIFO Write controller
         */
        uint32_t rfcfcsts:2;
        uint32_t reserved_3:1;
        /** rwcsts : RO; bitpos: [4]; default: 0;
         *  MTL Rx FIFO Write Controller Active Status When high, this bit indicates that the
         *  MTL Rx FIFO Write Controller is active and is transferring a received frame to the
         *  FIFO
         */
        uint32_t rwcsts:1;
        /** rrcsts : RO; bitpos: [6:5]; default: 0;
         *  MTL RxFIFO Read Controller State This field gives the state of the Rx FIFO read
         *  Controller: 00: IDLE state 01: Reading frame data 10: Reading frame status _or
         *  timestamp_ 11: Flushing the frame data and status
         */
        uint32_t rrcsts:2;
        uint32_t reserved_7:1;
        /** rxfsts : RO; bitpos: [9:8]; default: 0;
         *  MTL RxFIFO FillLevel Status This field gives the status of the filllevel of the Rx
         *  FIFO: 00: Rx FIFO Empty 01: Rx FIFO filllevel below flowcontrol deactivate
         *  threshold 10: Rx FIFO filllevel above flowcontrol activate threshold 11: Rx FIFO
         *  Full
         */
        uint32_t rxfsts:2;
        uint32_t reserved_10:6;
        /** tpests : RO; bitpos: [16]; default: 0;
         *  MAC GMII or MII Transmit Protocol Engine Status When high, this bit indicates that
         *  the MAC GMII or MII transmit protocol engine is actively transmitting data and is
         *  not in the IDLE state
         */
        uint32_t tpests:1;
        /** tfcsts : RO; bitpos: [18:17]; default: 0;
         *  MAC Transmit Frame Controller Status This field indicates the state of the MAC
         *  Transmit Frame Controller module: 00: IDLE state 01: Waiting for status of previous
         *  frame or IFG or backoff period to be over 10: Generating and transmitting a Pause
         *  frame _in the fullduplex mode_ 11: Transferring input frame for transmission
         */
        uint32_t tfcsts:2;
        /** txpaused : RO; bitpos: [19]; default: 0;
         *  MAC Transmitter in Pause When high, this bit indicates that the MAC transmitter is
         *  in the Pause condition _in the fullduplexonly mode_ and hence does not schedule any
         *  frame for transmission
         */
        uint32_t txpaused:1;
        /** trcsts : RO; bitpos: [21:20]; default: 0;
         *  MTL Tx FIFO Read Controller Status This field indicates the state of the Tx FIFO
         *  Read Controller: 00: IDLE state 01: READ state _transferring data to the MAC
         *  transmitter_ 10: Waiting for TxStatus from the MAC transmitter 11: Writing the
         *  received TxStatus or flushing the Tx FIFO
         */
        uint32_t trcsts:2;
        /** twcsts : RO; bitpos: [22]; default: 0;
         *  MTL Tx FIFO Write Controller Status When high, this bit indicates that the MTL Tx
         *  FIFO Write Controller is active and is transferring data to the Tx FIFO
         */
        uint32_t twcsts:1;
        uint32_t reserved_23:1;
        /** txfsts : RO; bitpos: [24]; default: 0;
         *  MTL Tx FIFO Not Empty Status When high, this bit indicates that the MTL Tx FIFO is
         *  not empty and some data is left for transmission
         */
        uint32_t txfsts:1;
        /** txstsfsts : RO; bitpos: [25]; default: 0;
         *  MTL TxStatus FIFO Full Status When high, this bit indicates that the MTL TxStatus
         *  FIFO is full Therefore, the MTL cannot accept any more frames for transmission This
         *  bit is reserved in the GMACAHB and GMACDMA configurations
         */
        uint32_t txstsfsts:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} gmac_register9_debugregister_reg_t;

/** Type of register10_remotewakeupframefilterregister register
 *  Remote Wake-Up Frame Filter Register
 */
typedef union {
    struct {
        /** wkupfrm_filter : R/W; bitpos: [31:0]; default: 0;
         *  This is the address through which the application writes or reads the remote
         *  wake-up frame filter registers.The reg_wkupfrm_filter register is a pointer to
         *  eight reg_wkupfrm_filter registers.The reg_wkupfrm_filter register is loaded by
         *  sequentially loading the eight register values.Eight sequential writes to this
         *  address(0x0028)write all reg_wkupfrm_filter registers.Similarly, eight sequential
         *  reads from this address(0x0028) read all reg_wkupfrm_filter registers.
         *  This register is present only when you select the PMT module Remote Wake-Up feature
         *  in coreConsultant.
         */
        uint32_t wkupfrm_filter:32;
    };
    uint32_t val;
} gmac_register10_remotewakeupframefilterregister_reg_t;

/** Type of register11_pmtcontrolandstatusregister register
 *  PMT Control and Status Register. This register is present only when you select the
 *  PMT module in coreConsultant.
 */
typedef union {
    struct {
        /** pwrdwn : R/W1S; bitpos: [0]; default: 0;
         *  Power Down
         *  When set, the MAC receiver drops all received frames until it receives the expected
         *  magic packet or remote wake-up frame. This bit is then self-cleared and the
         *  power-down mode is disabled. The Software can also clear this bit before the
         *  expected magic packet or remote wake-up frame is received. The frames, received by
         *  MAC after this bit is cleared, are forwarded to the application. This bit must only
         *  be set when the Magic Packet Enable, Global Unicast, or Remote Wake-Up Frame Enable
         *  bit is set high.
         */
        uint32_t pwrdwn:1;
        /** mgkpkten : R/W; bitpos: [1]; default: 0;
         *  Magic Packet Enable
         *  When set, enables generation of a power management event because of magic packet
         *  reception.
         */
        uint32_t mgkpkten:1;
        /** rwkpkten : R/W; bitpos: [2]; default: 0;
         *  Remote Wake-Up Frame Enable
         *  When set, enables generation of a power management event because of remote wake-up
         *  frame reception.
         */
        uint32_t rwkpkten:1;
        uint32_t reserved_3:2;
        /** mgkprcvd : R/W; bitpos: [5]; default: 0;
         *  Magic Packet Received
         *  When set,this bit indicates that the power management event is generated because of
         *  the reception of a magic packet. This bit is cleared by Read into this register.
         */
        uint32_t mgkprcvd:1;
        /** rwkprcvd : R/W; bitpos: [6]; default: 0;
         *  Remote Wake-Up Frame Received
         *  When set, this bit indicates the power management event is generated because of the
         *  reception of a remote wake-up frame. This bit is cleared by a Read into this
         *  register.
         */
        uint32_t rwkprcvd:1;
        uint32_t reserved_7:2;
        /** glblucast : R/W; bitpos: [9]; default: 0;
         *  Global Unicast.
         *  When set, enables any unicast packet filtered by the MAX(DAF) address recognition
         *  to be a remote wake-up frame.
         */
        uint32_t glblucast:1;
        uint32_t reserved_10:14;
        /** rwkptr : RO; bitpos: [28:24]; default: 0;
         *  Remote Wake-up FIFO Pointer.
         *  This field gives the current value(0 to 31) of the Remote Wake-up Frame filter
         *  register pointer. When the value of this pointer is erual to 7, 15, 23 or 31, the
         *  contents of the Remote Wake-up Frame Filter Register are transferred to the
         *  clk_rx_i domain when a write occurs to that register.
         *  The maximum value of the pointer is 7, 15, 23 and 31 respectively depending on the
         *  number of Remote Wakeup Filters selected during configuration.
         */
        uint32_t rwkptr:5;
        uint32_t reserved_29:2;
        /** rwkfiltrst : R/W1S; bitpos: [31]; default: 0;
         *  Remote Wake-Up Frame Filter Register Pointer Reset.
         *  When this bit is set, it resets the remote wake-up frame filter register pointer to
         *  3'b000. It is automatically cleared after 1 clock cycle.
         */
        uint32_t rwkfiltrst:1;
    };
    uint32_t val;
} gmac_register11_pmtcontrolandstatusregister_reg_t;

/** Type of register12_lpicontrolandstatusregister register
 *  Controls the Low Power Idle _LPI_ operations and provides the LPI status of the
 *  core This register is present only when you select the Energy Efficient Ethernet
 *  feature in coreConsultant
 */
typedef union {
    struct {
        /** tlpien : R/W; bitpos: [0]; default: 0;
         *  Transmit LPI Entry When set, this bit indicates that the MAC Transmitter has
         *  entered the LPI state because of the setting of the LPIEN bit This bit is cleared
         *  by a read into this register
         */
        uint32_t tlpien:1;
        /** tlpiex : R/W; bitpos: [1]; default: 0;
         *  Transmit LPI Exit When set, this bit indicates that the MAC transmitter has exited
         *  the LPI state after the user has cleared the LPIEN bit and the LPI TW Timer has
         *  expired This bit is cleared by a read into this register
         */
        uint32_t tlpiex:1;
        /** rlpien : R/W; bitpos: [2]; default: 0;
         *  Receive LPI Entry When set, this bit indicates that the MAC Receiver has received
         *  an LPI pattern and entered the LPI state This bit is cleared by a read into this
         *  register Note: This bit may not get set if the MAC stops receiving the LPI pattern
         *  for a very short duration, such as, less than 3 clock cycles of CSR clock
         */
        uint32_t rlpien:1;
        /** rlpiex : R/W; bitpos: [3]; default: 0;
         *  Receive LPI Exit When set, this bit indicates that the MAC Receiver has stopped
         *  receiving the LPI pattern on the GMII or MII interface, exited the LPI state, and
         *  resumed the normal reception This bit is cleared by a read into this register Note:
         *  This bit may not get set if the MAC stops receiving the LPI pattern for a very
         *  short duration, such as, less than 3 clock cycles of CSR clock
         */
        uint32_t rlpiex:1;
        uint32_t reserved_4:4;
        /** tlpist : RO; bitpos: [8]; default: 0;
         *  Transmit LPI State When set, this bit indicates that the MAC is transmitting the
         *  LPI pattern on the GMII or MII interface
         */
        uint32_t tlpist:1;
        /** rlpist : RO; bitpos: [9]; default: 0;
         *  Receive LPI State When set, this bit indicates that the MAC is receiving the LPI
         *  pattern on the GMII or MII interface
         */
        uint32_t rlpist:1;
        uint32_t reserved_10:6;
        /** lpien : R/W; bitpos: [16]; default: 0;
         *  LPI Enable When set, this bit instructs the MAC Transmitter to enter the LPI state
         *  When reset, this bit instructs the MAC to exit the LPI state and resume normal
         *  transmission This bit is cleared when the LPITXA bit is set and the MAC exits the
         *  LPI state because of the arrival of a new packet for transmission
         */
        uint32_t lpien:1;
        /** pls : R/W; bitpos: [17]; default: 0;
         *  PHY Link Status This bit indicates the link status of the PHY The MAC Transmitter
         *  asserts the LPI pattern only when the link status is up _okay_ at least for the
         *  time indicated by the LPI LS TIMER When set, the link is considered to be okay _up_
         *  and when reset, the link is considered to be down
         */
        uint32_t pls:1;
        /** plsen : R/W; bitpos: [18]; default: 0;
         *  PHY Link Status Enable This bit enables the link status received on the RGMII,
         *  SGMII, or SMII receive paths to be used for activating the LPI LS TIMER When set,
         *  the MAC uses the linkstatus bits of Register 54 _SGMII/RGMII/SMII Control and
         *  Status Register_ and Bit 17 _PLS_ for the LPI LS Timer trigger When cleared, the
         *  MAC ignores the linkstatus bits of Register 54 and takes only the PLS bit This bit
         *  is RO and reserved if you have not selected the RGMII, SGMII, or SMII PHY interface
         */
        uint32_t plsen:1;
        /** lpitxa : R/W; bitpos: [19]; default: 0;
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
        uint32_t lpitxa:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} gmac_register12_lpicontrolandstatusregister_reg_t;

/** Type of register13_lpitimerscontrolregister register
 *  Controls the timeout values in LPI states This register is present only when you
 *  select the Energy Efficient Ethernet feature in coreConsultant
 */
typedef union {
    struct {
        /** twt : R/W; bitpos: [15:0]; default: 0;
         *  LPI TW TIMER This field specifies the minimum time _in microseconds_ for which the
         *  MAC waits after it stops transmitting the LPI pattern to the PHY and before it
         *  resumes the normal transmission The TLPIEX status bit is set after the expiry of
         *  this timer
         */
        uint32_t twt:16;
        /** lst : R/W; bitpos: [25:16]; default: 1000;
         *  LPI LS TIMER This field specifies the minimum time _in milliseconds_ for which the
         *  link status from the PHY should be up _OKAY_ before the LPI pattern can be
         *  transmitted to the PHY The MAC does not transmit the LPI pattern even when the
         *  LPIEN bit is set unless the LPI LS Timer reaches the programmed terminal count The
         *  default value of the LPI LS Timer is 1000 _1 sec_ as defined in the IEEE standard
         */
        uint32_t lst:10;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} gmac_register13_lpitimerscontrolregister_reg_t;

/** Type of register14_interruptstatusregister register
 *  Contains the interrupt status
 */
typedef union {
    struct {
        /** rgsmiiis : RO; bitpos: [0]; default: 0;
         *  RGMII or SMII Interrupt Status This bit is set because of any change in value of
         *  the Link Status of RGMII or SMII interface _Bit 3 in Register 54 _SGMII/RGMII/SMII
         *  Control and Status Register__ This bit is cleared when you perform a read operation
         *  on the SGMII/RGMII/SMII Control and Status Register This bit is valid only when you
         *  select the optional RGMII or SMII PHY interface during core configuration and
         *  operation
         */
        uint32_t rgsmiiis:1;
        /** pcslchgis : RO; bitpos: [1]; default: 0;
         *  PCS Link Status Changed This bit is set because of any change in Link Status in the
         *  TBI, RTBI, or SGMII PHY interface _Bit 2 in Register 49 _AN Status Register__ This
         *  bit is cleared when you perform a read operation on the AN Status register This bit
         *  is valid only when you select the optional TBI, RTBI, or SGMII PHY interface during
         *  core configuration and operation
         */
        uint32_t pcslchgis:1;
        /** pcsancis : RO; bitpos: [2]; default: 0;
         *  PCS AutoNegotiation Complete This bit is set when the Autonegotiation is completed
         *  in the TBI, RTBI, or SGMII PHY interface _Bit 5 in Register 49 _AN Status
         *  Register__ This bit is cleared when you perform a read operation to the AN Status
         *  register This bit is valid only when you select the optional TBI, RTBI, or SGMII
         *  PHY interface during core configuration and operation
         */
        uint32_t pcsancis:1;
        /** pmtis : RO; bitpos: [3]; default: 0;
         *  PMT Interrupt Status This bit is set when a magic packet or remote wakeup frame is
         *  received in the powerdown mode _see Bits 5 and 6 in the PMT Control and Status
         *  Register_ This bit is cleared when both Bits[6:5] are cleared because of a read
         *  operation to the PMT Control and Status register This bit is valid only when you
         *  select the optional PMT module during core configuration
         */
        uint32_t pmtis:1;
        /** mmcis : RO; bitpos: [4]; default: 0;
         *  MMC Interrupt Status This bit is set high when any of the Bits [7:5] is set high
         *  and cleared only when all of these bits are low This bit is valid only when you
         *  select the optional MMC module during core configuration
         */
        uint32_t mmcis:1;
        /** mmcrxis : RO; bitpos: [5]; default: 0;
         *  MMC Receive Interrupt Status This bit is set high when an interrupt is generated in
         *  the MMC Receive Interrupt Register This bit is cleared when all the bits in this
         *  interrupt register are cleared This bit is valid only when you select the optional
         *  MMC module during core configuration
         */
        uint32_t mmcrxis:1;
        /** mmctxis : RO; bitpos: [6]; default: 0;
         *  MMC Transmit Interrupt Status This bit is set high when an interrupt is generated
         *  in the MMC Transmit Interrupt Register This bit is cleared when all the bits in
         *  this interrupt register are cleared This bit is valid only when you select the
         *  optional MMC module during core configuration
         */
        uint32_t mmctxis:1;
        /** mmcrxipis : RO; bitpos: [7]; default: 0;
         *  MMC Receive Checksum Offload Interrupt Status This bit is set high when an
         *  interrupt is generated in the MMC Receive Checksum Offload Interrupt Register This
         *  bit is cleared when all the bits in this interrupt register are cleared This bit is
         *  valid only when you select the optional MMC module and Checksum Offload Engine
         *  _Type 2_ during core configuration
         */
        uint32_t mmcrxipis:1;
        uint32_t reserved_8:1;
        /** tsis : RO; bitpos: [9]; default: 0;
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
        uint32_t tsis:1;
        /** lpiis : RO; bitpos: [10]; default: 0;
         *  LPI Interrupt Status When the Energy Efficient Ethernet feature is enabled, this
         *  bit is set for any LPI state entry or exit in the MAC Transmitter or Receiver This
         *  bit is cleared on reading Bit 0 of Register 12 _LPI Control and Status Register_ In
         *  all other modes, this bit is reserved
         */
        uint32_t lpiis:1;
        /** gpiis : RO; bitpos: [11]; default: 0;
         *  GPI Interrupt Status When the GPIO feature is enabled, this bit is set when any
         *  active event _LL or LH_ occurs on the GPIS field _Bits [3:0]_ of Register 56
         *  _General Purpose IO Register_ and the corresponding GPIE bit is enabled This bit is
         *  cleared on reading lane 0 _GPIS_ of Register 56 _General Purpose IO Register_ When
         *  the GPIO feature is not enabled, this bit is reserved
         */
        uint32_t gpiis:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} gmac_register14_interruptstatusregister_reg_t;

/** Type of register15_interruptmaskregister register
 *  Contains the masks for generating the interrupts
 */
typedef union {
    struct {
        /** rgsmiiim : R/W; bitpos: [0]; default: 0;
         *  RGMII or SMII Interrupt Mask When set, this bit disables the assertion of the
         *  interrupt signal because of the setting of the RGMII or SMII Interrupt Status bit
         *  in Register 14 _Interrupt Status Register_
         */
        uint32_t rgsmiiim:1;
        /** pcslchgim : R/W; bitpos: [1]; default: 0;
         *  PCS Link Status Interrupt Mask When set, this bit disables the assertion of the
         *  interrupt signal because of the setting of the PCS Linkstatus changed bit in
         *  Register 14 _Interrupt Status Register_
         */
        uint32_t pcslchgim:1;
        /** pcsancim : R/W; bitpos: [2]; default: 0;
         *  PCS AN Completion Interrupt Mask When set, this bit disables the assertion of the
         *  interrupt signal because of the setting of PCS Autonegotiation complete bit in
         *  Register 14 _Interrupt Status Register_
         */
        uint32_t pcsancim:1;
        /** pmtim : R/W; bitpos: [3]; default: 0;
         *  PMT Interrupt Mask When set, this bit disables the assertion of the interrupt
         *  signal because of the setting of PMT Interrupt Status bit in Register 14 _Interrupt
         *  Status Register_
         */
        uint32_t pmtim:1;
        uint32_t reserved_4:5;
        /** tsim : R/W; bitpos: [9]; default: 0;
         *  Timestamp Interrupt Mask When set, this bit disables the assertion of the interrupt
         *  signal because of the setting of Timestamp Interrupt Status bit in Register 14
         *  _Interrupt Status Register_ This bit is valid only when IEEE1588 timestamping is
         *  enabled In all other modes, this bit is reserved
         */
        uint32_t tsim:1;
        /** lpiim : R/W; bitpos: [10]; default: 0;
         *  LPI Interrupt Mask When set, this bit disables the assertion of the interrupt
         *  signal because of the setting of the LPI Interrupt Status bit in Register 14
         *  _Interrupt Status Register_ This bit is valid only when you select the Energy
         *  Efficient Ethernet feature during core configuration In all other modes, this bit
         *  is reserved
         */
        uint32_t lpiim:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} gmac_register15_interruptmaskregister_reg_t;

/** Type of register16_macaddress0highregister register
 *  Contains the higher 16 bits of the first MAC address
 */
typedef union {
    struct {
        /** addrhi_0 : R/W; bitpos: [15:0]; default: 65535;
         *  MAC Address0 [47:32] This field contains the upper 16 bits _47:32_ of the first
         *  6byte MAC address The MAC uses this field for filtering the received frames and
         *  inserting the MAC address in the Transmit Flow Control _Pause_ Frames
         */
        uint32_t addrhi_0:16;
        uint32_t reserved_16:15;
        /** ae_0 : RO; bitpos: [31]; default: 1;
         *  Address Enable This bit is always set to 1
         */
        uint32_t ae_0:1;
    };
    uint32_t val;
} gmac_register16_macaddress0highregister_reg_t;

/** Type of register17_macaddress0lowregister register
 *  Contains the lower 32 bits of the first MAC address
 */
typedef union {
    struct {
        /** addrlo_0 : R/W; bitpos: [31:0]; default: 4294967295;
         *  MAC Address0 [31:0] This field contains the lower 32 bits of the first 6byte MAC
         *  address This is used by the MAC for filtering the received frames and inserting the
         *  MAC address in the Transmit Flow Control _Pause_ Frames
         */
        uint32_t addrlo_0:32;
    };
    uint32_t val;
} gmac_register17_macaddress0lowregister_reg_t;

/** Type of register18_macaddress1highregister register
 *  Contains the higher 16 bits of the second MAC address This register is present only
 *  when Enable MAC Address1 is selected in coreConsultant _See Table 78_
 */
typedef union {
    struct {
        /** addrhi_1 : R/W; bitpos: [15:0]; default: 65535;
         *  MAC Address1 [47:32] This field contains the upper 16 bits _47:32_ of the second
         *  6byte MAC address
         */
        uint32_t addrhi_1:16;
        uint32_t reserved_16:8;
        /** mbc_1 : R/W; bitpos: [29:24]; default: 0;
         *  Mask Byte Control These bits are mask control bits for comparison of each of the
         *  MAC Address bytes When set high, the MAC does not compare the corresponding byte of
         *  received DA or SA with the contents of MAC Address1 registers Each bit controls the
         *  masking of the bytes as follows: Bit 29: Register 18[15:8] Bit 28: Register 18[7:0]
         *  Bit 27: Register 19[31:24]  Bit 24: Register 19[7:0] You can filter a group of
         *  addresses _known as group address filtering_ by masking one or more bytes of the
         *  address
         */
        uint32_t mbc_1:6;
        /** sa_1 : R/W; bitpos: [30]; default: 0;
         *  Source Address When this bit is set, the MAC Address1[47:0] is used to compare with
         *  the SA fields of the received frame When this bit is reset, the MAC Address1[47:0]
         *  is used to compare with the DA fields of the received frame
         */
        uint32_t sa_1:1;
        /** ae_1 : R/W; bitpos: [31]; default: 0;
         *  Address Enable
         */
        uint32_t ae_1:1;
    };
    uint32_t val;
} gmac_register18_macaddress1highregister_reg_t;

/** Type of register19_macaddress1lowregister register
 *  Contains the lower 32 bits of the second MAC address This register is present only
 *  when Enable MAC Address1 is selected in coreConsultant _See Table 78_
 */
typedef union {
    struct {
        /** addrlo_1 : R/W; bitpos: [31:0]; default: 4294967295;
         *  MAC Address1 [31:0] This field contains the lower 32 bits of the second 6byte MAC
         *  address The content of this field is undefined until loaded by the Application
         *  after the initialization process
         */
        uint32_t addrlo_1:32;
    };
    uint32_t val;
} gmac_register19_macaddress1lowregister_reg_t;

/** Type of macaddress2highregistercontainsthehigher16bitsofthethirdmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrhi_2 : R/W; bitpos: [15:0]; default: 65535;
         *  This register is present only when Enable MAC Address2 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrhi_2:16;
        uint32_t reserved_16:8;
        /** mbc_2 : R/W; bitpos: [29:24]; default: 0;
         *  This register is present only when Enable MAC Address2 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t mbc_2:6;
        /** sa_2 : R/W; bitpos: [30]; default: 0;
         *  This register is present only when Enable MAC Address2 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t sa_2:1;
        /** ae_2 : R/W; bitpos: [31]; default: 0;
         *  This register is present only when Enable MAC Address2 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t ae_2:1;
    };
    uint32_t val;
} gmac_macaddress2highregistercontainsthehigher16bitsofthethirdmacaddress_reg_t;

/** Type of macaddress2lowregistercontainsthelower32bitsofthethirdmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrlo_2 : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is present only when Enable MAC Address2 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrlo_2:32;
    };
    uint32_t val;
} gmac_macaddress2lowregistercontainsthelower32bitsofthethirdmacaddress_reg_t;

/** Type of macaddress3highregistercontainsthehigher16bitsofthefourthmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrhi_3 : R/W; bitpos: [15:0]; default: 65535;
         *  This register is present only when Enable MAC Address3 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrhi_3:16;
        uint32_t reserved_16:8;
        /** mbc_3 : R/W; bitpos: [29:24]; default: 0;
         *  This register is present only when Enable MAC Address3 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t mbc_3:6;
        /** sa_3 : R/W; bitpos: [30]; default: 0;
         *  This register is present only when Enable MAC Address3 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t sa_3:1;
        /** ae_3 : R/W; bitpos: [31]; default: 0;
         *  This register is present only when Enable MAC Address3 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t ae_3:1;
    };
    uint32_t val;
} gmac_macaddress3highregistercontainsthehigher16bitsofthefourthmacaddress_reg_t;

/** Type of macaddress3lowregistercontainsthelower32bitsofthefourthmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrlo_3 : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is present only when Enable MAC Address3 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrlo_3:32;
    };
    uint32_t val;
} gmac_macaddress3lowregistercontainsthelower32bitsofthefourthmacaddress_reg_t;

/** Type of macaddress4highregistercontainsthehigher16bitsofthefifthmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrhi_4 : R/W; bitpos: [15:0]; default: 65535;
         *  This register is present only when Enable MAC Address4 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrhi_4:16;
        uint32_t reserved_16:8;
        /** mbc_4 : R/W; bitpos: [29:24]; default: 0;
         *  This register is present only when Enable MAC Address4 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t mbc_4:6;
        /** sa_4 : R/W; bitpos: [30]; default: 0;
         *  This register is present only when Enable MAC Address4 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t sa_4:1;
        /** ae_4 : R/W; bitpos: [31]; default: 0;
         *  This register is present only when Enable MAC Address4 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t ae_4:1;
    };
    uint32_t val;
} gmac_macaddress4highregistercontainsthehigher16bitsofthefifthmacaddress_reg_t;

/** Type of macaddress4lowregistercontainsthelower32bitsofthefifthmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrlo_4 : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is present only when Enable MAC Address4 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrlo_4:32;
    };
    uint32_t val;
} gmac_macaddress4lowregistercontainsthelower32bitsofthefifthmacaddress_reg_t;

/** Type of macaddress5highregistercontainsthehigher16bitsofthesixthmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrhi_5 : R/W; bitpos: [15:0]; default: 65535;
         *  This register is present only when Enable MAC Address5 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrhi_5:16;
        uint32_t reserved_16:8;
        /** mbc_5 : R/W; bitpos: [29:24]; default: 0;
         *  This register is present only when Enable MAC Address5 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t mbc_5:6;
        /** sa_5 : R/W; bitpos: [30]; default: 0;
         *  This register is present only when Enable MAC Address5 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t sa_5:1;
        /** ae_5 : R/W; bitpos: [31]; default: 0;
         *  This register is present only when Enable MAC Address5 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t ae_5:1;
    };
    uint32_t val;
} gmac_macaddress5highregistercontainsthehigher16bitsofthesixthmacaddress_reg_t;

/** Type of macaddress5lowregistercontainsthelower32bitsofthesixthmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrlo_5 : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is present only when Enable MAC Address5 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrlo_5:32;
    };
    uint32_t val;
} gmac_macaddress5lowregistercontainsthelower32bitsofthesixthmacaddress_reg_t;

/** Type of macaddress6highregistercontainsthehigher16bitsoftheseventhmacaddress
 *  register
 *  Reserved
 */
typedef union {
    struct {
        /** addrhi_6 : R/W; bitpos: [15:0]; default: 65535;
         *  This register is present only when Enable MAC Address6 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrhi_6:16;
        uint32_t reserved_16:8;
        /** mbc_6 : R/W; bitpos: [29:24]; default: 0;
         *  This register is present only when Enable MAC Address6 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t mbc_6:6;
        /** sa_6 : R/W; bitpos: [30]; default: 0;
         *  This register is present only when Enable MAC Address6 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t sa_6:1;
        /** ae_6 : R/W; bitpos: [31]; default: 0;
         *  This register is present only when Enable MAC Address6 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t ae_6:1;
    };
    uint32_t val;
} gmac_macaddress6highregistercontainsthehigher16bitsoftheseventhmacaddress_reg_t;

/** Type of macaddress6lowregistercontainsthelower32bitsoftheseventhmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrlo_6 : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is present only when Enable MAC Address6 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrlo_6:32;
    };
    uint32_t val;
} gmac_macaddress6lowregistercontainsthelower32bitsoftheseventhmacaddress_reg_t;

/** Type of macaddress7highregistercontainsthehigher16bitsoftheeighthmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrhi_7 : R/W; bitpos: [15:0]; default: 65535;
         *  This register is present only when Enable MAC Address7 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrhi_7:16;
        uint32_t reserved_16:8;
        /** mbc_7 : R/W; bitpos: [29:24]; default: 0;
         *  This register is present only when Enable MAC Address7 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t mbc_7:6;
        /** sa_7 : R/W; bitpos: [30]; default: 0;
         *  This register is present only when Enable MAC Address7 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t sa_7:1;
        /** ae_7 : R/W; bitpos: [31]; default: 0;
         *  This register is present only when Enable MAC Address7 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t ae_7:1;
    };
    uint32_t val;
} gmac_macaddress7highregistercontainsthehigher16bitsoftheeighthmacaddress_reg_t;

/** Type of macaddress7lowregistercontainsthelower32bitsoftheeighthmacaddress register
 *  Reserved
 */
typedef union {
    struct {
        /** addrlo_7 : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is present only when Enable MAC Address7 is selected in
         *  coreConsultant _See Table 78_
         */
        uint32_t addrlo_7:32;
    };
    uint32_t val;
} gmac_macaddress7lowregistercontainsthelower32bitsoftheeighthmacaddress_reg_t;

/** Type of register48_ancontrolregister register
 *  Enables and/or restarts autonegotiation This register also enables the Physical
 *  Coding Sublayer _PCS_ loopback This register is present only when you select the
 *  TBI, RTBI, or SGMII interface in coreConsultant
 */
typedef union {
    struct {
        uint32_t reserved_0:9;
        /** ran : R/W1S; bitpos: [9]; default: 0;
         *  Restart AutoNegotiation When set, this bit causes autonegotiation to restart if Bit
         *  12 _ANE_ is set This bit is selfclearing after autonegotiation starts This bit
         *  should be cleared for normal operation
         */
        uint32_t ran:1;
        uint32_t reserved_10:2;
        /** auto_negotiation : R/W; bitpos: [12]; default: 0;
         *  AutoNegotiation Enable When set, this bit enables the MAC to perform
         *  autonegotiation with the link partner Clearing this bit disables the autonegotiation
         */
        uint32_t auto_negotiation:1;
        uint32_t reserved_13:1;
        /** ele : R/W; bitpos: [14]; default: 0;
         *  External Loopback Enable When set, this bit causes the PHY to loopback the transmit
         *  data into the receive path The pcs_ewrap_o signal is asserted high when this bit is
         *  set
         */
        uint32_t ele:1;
        uint32_t reserved_15:1;
        /** ecd : R/W; bitpos: [16]; default: 0;
         *  Enable Comma Detect When set, this bit enables the PHY for comma detection and word
         *  resynchronization This bit controls the pcs_en_cdet_o signal on the TBI, RTBI, or
         *  SGMII interface
         */
        uint32_t ecd:1;
        /** lr : R/W; bitpos: [17]; default: 0;
         *  Lock to Reference When set, this bit enables the PHY to lock its PLL to the 125 MHz
         *  reference clock This bit controls the pcs_lck_ref_o signal on the TBI, RTBI, or
         *  SGMII interface
         */
        uint32_t lr:1;
        /** sgmral : R/W; bitpos: [18]; default: 0;
         *  SGMII RAL Control When set, this bit forces the SGMII RAL block to operate in the
         *  speed configured in the Speed and Port Select bits of the MAC Configuration
         *  register This is useful when the SGMII interface is used in a direct MAC to MAC
         *  connection _without a PHY_ and any MAC must reconfigure the speed When reset, the
         *  SGMII RAL block operates according to the link speed status received on SGMII _from
         *  the PHY_ This bit is reserved _and RO_ if the SGMII PHY interface is not selected
         *  during core configuration
         */
        uint32_t sgmral:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} gmac_register48_ancontrolregister_reg_t;

/** Type of register49_anstatusregister register
 *  Indicates the link and autonegotiation status This register is present only when
 *  you select the TBI, RTBI, or SGMII interface in coreConsultant
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** ls : RO; bitpos: [2]; default: 0;
         *  Link Status This bit indicates whether the data channel _link_ is up or down For
         *  the TBI, RTBI or SGMII interfaces, if ANEG is going on, data cannot be transferred
         *  across the link and hence the link is given as down
         */
        uint32_t ls:1;
        /** ana : RO; bitpos: [3]; default: 1;
         *  AutoNegotiation Ability This bit is always high because the MAC supports auto
         *  negotiation
         */
        uint32_t ana:1;
        uint32_t reserved_4:1;
        /** anc : RO; bitpos: [5]; default: 0;
         *  AutoNegotiation Complete When set, this bit indicates that the autonegotiation
         *  process is complete This bit is cleared when autonegotiation is reinitiated
         */
        uint32_t anc:1;
        uint32_t reserved_6:2;
        /** es : RO; bitpos: [8]; default: 1;
         *  Extended Status This bit is tied to high if the TBI or RTBI interface is selected
         *  during core configuration indicating that the MAC supports extended status
         *  information in Register 53 _TBI Extended  Status Register_ This bit is tied to low
         *  if the SGMII interface is selected and the TBI or RTBI interface is not selected
         *  during core configuration indicating that Register 53 is not present
         */
        uint32_t es:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} gmac_register49_anstatusregister_reg_t;

/** Type of register50_autonegotiationadvertisementregister register
 *  This register is configured before autonegotiation begins It contains the
 *  advertised ability of the MAC This register is present only when you select the TBI
 *  or RTBI interface in coreConsultant
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** fd : R/W; bitpos: [5]; default: 1;
         *  FullDuplex When set high, this bit indicates that the MAC supports the fullduplex
         *  mode
         */
        uint32_t fd:1;
        /** hd : R/W; bitpos: [6]; default: 1;
         *  HalfDuplex When set high, this bit indicates that the MAC supports the halfduplex
         *  mode This bit is always low _and RO_ when the MAC is configured for the
         *  fullduplexonly mode
         */
        uint32_t hd:1;
        /** pse : R/W; bitpos: [8:7]; default: 3;
         *  Pause Encoding These bits provide an encoding for the Pause bits, indicating that
         *  the MAC is capable of configuring the Pause function as defined in IEEE 8023x The
         *  encoding of these bits is defined in IEEE 8023z, Section 37214
         */
        uint32_t pse:2;
        uint32_t reserved_9:3;
        /** rfe : R/W; bitpos: [13:12]; default: 0;
         *  Remote Fault Encoding These bits provide a remote fault encoding, indicating to a
         *  link partner that a fault or error condition has occurred The encoding of these
         *  bits is defined in IEEE 8023z, Section 37215
         */
        uint32_t rfe:2;
        uint32_t reserved_14:1;
        /** np : RO; bitpos: [15]; default: 0;
         *  Next Page Support This bit is always low because the MAC does not support the next
         *  page
         */
        uint32_t np:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gmac_register50_autonegotiationadvertisementregister_reg_t;

/** Type of register51_autonegotiationlinkpartnerabilityregister register
 *  Contains the advertised ability of the link partner Its value is valid after
 *  successful completion of autonegotiation or when a new base page has been received
 *  _indicated in the AutoNegotiation Expansion Register_ This register is present only
 *  when you select the TBI or RTBI interface in coreConsultant
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** fd_ability : RO; bitpos: [5]; default: 0;
         *  FullDuplex When set, this bit indicates that the link partner has the ability to
         *  operate in the full duplex mode When cleared, this bit indicates that the link
         *  partner does not have the ability to operate in the fullduplex mode
         */
        uint32_t fd_ability:1;
        /** hd_ability : RO; bitpos: [6]; default: 0;
         *  HalfDuplex When set, this bit indicates that the link partner has the ability to
         *  operate in the halfduplex mode When cleared, this bit indicates that the link
         *  partner does not have the ability to operate in the halfduplex mode
         */
        uint32_t hd_ability:1;
        /** pse_ability : RO; bitpos: [8:7]; default: 0;
         *  Pause Encoding These bits provide an encoding for the Pause bits, indicating that
         *  the link partner's capability of configuring the Pause function as defined in the
         *  IEEE 8023x specification The encoding of these bits is defined in IEEE 8023z,
         *  Section 37214
         */
        uint32_t pse_ability:2;
        uint32_t reserved_9:3;
        /** rfe_ability : RO; bitpos: [13:12]; default: 0;
         *  Remote Fault Encoding These bits provide a remote fault encoding, indicating a
         *  fault or error condition of the link partner The encoding of these bits is defined
         *  in IEEE 8023z, Section 37215
         */
        uint32_t rfe_ability:2;
        /** ack : RO; bitpos: [14]; default: 0;
         *  Acknowledge When set, the autonegotiation function uses this bit to indicate that
         *  the link partner has successfully received the base page of the MAC When cleared,
         *  it indicates that the link partner did not successfully receive the base page of
         *  the MAC
         */
        uint32_t ack:1;
        /** no : RO; bitpos: [15]; default: 0;
         *  Next Page Support When set, this bit indicates that more next page information is
         *  available When cleared, this bit indicates that next page exchange is not desired
         */
        uint32_t no:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gmac_register51_autonegotiationlinkpartnerabilityregister_reg_t;

/** Type of register52_autonegotiationexpansionregister register
 *  Indicates whether a new base page has been received from the link partner This
 *  register is present only when you select the TBI or RTBI interface in coreConsultant
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** npr : RO; bitpos: [1]; default: 0;
         *  New Page Received When set, this bit indicates that the MAC has received a new page
         *  This bit is cleared when read
         */
        uint32_t npr:1;
        /** npa : RO; bitpos: [2]; default: 0;
         *  Next Page Ability This bit is always low because the MAC does not support the next
         *  page function
         */
        uint32_t npa:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gmac_register52_autonegotiationexpansionregister_reg_t;

/** Type of register53_tbiextendedstatusregister register
 *  Indicates all modes of operation of the MAC This register is present only when you
 *  select the TBI or RTBI interface in coreConsultant
 */
typedef union {
    struct {
        uint32_t reserved_0:14;
        /** ghd : RO; bitpos: [14]; default: 1;
         *  1000BASEX HalfDuplex Capable This bit indicates that the MAC is able to perform the
         *  halfduplex and 1000BASEX operations This bit is always low when the MAC is
         *  configured for the fullduplexonly operation during core configuration
         */
        uint32_t ghd:1;
        /** gfd : RO; bitpos: [15]; default: 1;
         *  1000BASEX FullDuplex Capable This bit indicates that the MAC is able to perform the
         *  fullduplex and 1000BASEX operations
         */
        uint32_t gfd:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gmac_register53_tbiextendedstatusregister_reg_t;

/** Type of register54_sgmii_rgmii_smiicontrolandstatusregister register
 *  Indicates the status signals received from the PHY through the SGMII, RGMII, or
 *  SMII interface This register is present only when you select the SGMII, RGMII, or
 *  SMII interface in coreConsultant
 */
typedef union {
    struct {
        /** lnkmod : RO; bitpos: [0]; default: 0;
         *  Link Mode This bit indicates the current mode of operation of the link: 1’b0:
         *  Halfduplex mode 1’b1: Fullduplex mode
         */
        uint32_t lnkmod:1;
        /** lnkspeed : RO; bitpos: [2:1]; default: 2;
         *  Link Speed
         */
        uint32_t lnkspeed:2;
        /** lnksts : RO; bitpos: [3]; default: 0;
         *  Link Status This bit indicates whether the link between the local PHY and the
         *  remote PHY is up or down It gives the status of the link between the SGMII of MAC
         *  and the SGMII of the local PHY The status bits are received from the local PHY
         *  during ANEG between he MAC and PHY on the SGMII link
         */
        uint32_t lnksts:1;
        /** jabto : RO; bitpos: [4]; default: 0;
         *  Jabber Timeout This bit indicates whether there is jabber timeout error _1'b1_ in
         *  the received frame This bit is reserved when the MAC is configured for the SGMII or
         *  RGMII PHY interface
         */
        uint32_t jabto:1;
        /** falscardet : RO; bitpos: [5]; default: 0;
         *  False Carrier Detected This bit indicates whether the SMII PHY detected false
         *  carrier _1'b1_ This bit is reserved when the MAC is configured for the SGMII or
         *  RGMII PHY interface
         */
        uint32_t falscardet:1;
        uint32_t reserved_6:10;
        /** smidrxs : R/W; bitpos: [16]; default: 0;
         *  Delay SMII RX Data Sampling with respect to the SMII SYNC Signal When set, the
         *  first bit of the SMII RX data is sampled one cycle after the SMII SYNC signal When
         *  reset, the first bit of the SMII RX data is sampled along with the SMII SYNC signal
         *  If the SMII PHY Interface with source synchronous mode is selected during core
         *  configuration, this bit is reserved _RO with default value_
         */
        uint32_t smidrxs:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} gmac_register54_sgmii_rgmii_smiicontrolandstatusregister_reg_t;

/** Type of register55_watchdogtimeoutregister register
 *  Controls the watchdog timeout for received frames
 */
typedef union {
    struct {
        /** wto : R/W; bitpos: [13:0]; default: 0;
         *  Watchdog Timeout When Bit 16 _PWE_ is set and Bit 23 _WD_ of Register 0 _MAC
         *  Configuration Register_ is reset, this field is used as watchdog timeout for a
         *  received frame If the length of a received frame exceeds the value of this field,
         *  such frame is terminated and declared as an error frame Note: When Bit 16 _PWE_ is
         *  set, the value in this field should be more than 1,522 _0x05F2_ Otherwise, the IEEE
         *  Std 8023specified valid tagged frames are declared as error frames and are dropped
         */
        uint32_t wto:14;
        uint32_t reserved_14:2;
        /** pwe : R/W; bitpos: [16]; default: 0;
         *  Programmable Watchdog Enable When this bit is set and Bit 23 _WD_ of Register 0
         *  _MAC Configuration Register_ is reset, the WTO field _Bits[13:0]_ is used as
         *  watchdog timeout for a received frame When this bit is cleared, the watchdog
         *  timeout for a received frame is controlled by the setting of Bit 23 _WD_ and Bit 20
         *  _JE_ in Register 0 _MAC Configuration Register_
         */
        uint32_t pwe:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} gmac_register55_watchdogtimeoutregister_reg_t;

/** Type of register56_generalpurposeioregister register
 *  Provides the control to drive up to 4 bits of output ports _GPO_ and also provides
 *  the status of up to 4 input ports _GPIS_
 */
typedef union {
    struct {
        /** gpis : R/W; bitpos: [3:0]; default: 0;
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
        uint32_t gpis:4;
        uint32_t reserved_4:4;
        /** gpo : R/W; bitpos: [11:8]; default: 0;
         *  General Purpose Output When this bit is set, it directly drives the gpo_o output
         *  ports When this bit is reset, it does not directly drive the gpo_o output ports The
         *  number of bits available in this field depend on the GP Output Signal Width option
         *  Other bits are not used _reserved and always reset_
         */
        uint32_t gpo:4;
        uint32_t reserved_12:4;
        /** gpie : R/W; bitpos: [19:16]; default: 0;
         *  GPI Interrupt Enable When this bit is set and the programmed event _LL or LH_
         *  occurs on the corresponding GPIS bit, Bit 11 _GPIIS_ of Register 14 _Interrupt
         *  Status Register_ is set Accordingly, the interrupt is generated on the mci_intr_o
         *  or sbd_intr_o The GPIIS bit is cleared when the host reads the Bits[7:0] of this
         *  register When reset, Bit 11 _GPIIS_ of Register 14 _Interrupt Status Register_ is
         *  not set when any event occurs on the corresponding GPIS bits The number of bits
         *  available in this field depend on the GP Input Signal Width option Other bits are
         *  not used _reserved and always reset_
         */
        uint32_t gpie:4;
        uint32_t reserved_20:4;
        /** gpit : R/W; bitpos: [27:24]; default: 0;
         *  GPI Type When set, this bit indicates that the corresponding GPIS is of latchedlow
         *  _LL_ type When reset, this bit indicates that the corresponding GPIS is of
         *  latchedhigh _LH_ type The number of bits available in this field depend on the GP
         *  Input Signal Width option Other bits are not used _reserved and always reset_
         */
        uint32_t gpit:4;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gmac_register56_generalpurposeioregister_reg_t;

/** Type of register256_layer3andlayer4controlregister0 register
 *  Controls the operations of the Layer 3 and Layer 4 frame filtering
 */
typedef union {
    struct {
        /** l3pen0 : R/W; bitpos: [0]; default: 0;
         *  Layer 3 Protocol Enable When set, this bit indicates that the Layer 3 IP Source or
         *  Destination Address matching is enabled for the IPv6 frames When reset, this bit
         *  indicates that the Layer 3 IP Source or Destination Address matching is enabled for
         *  the IPv4 frames The Layer 3 matching is done only when either L3SAM0 or L3DAM0 bit
         *  is set high
         */
        uint32_t l3pen0:1;
        uint32_t reserved_1:1;
        /** l3sam0 : R/W; bitpos: [2]; default: 0;
         *  Layer 3 IP SA Match Enable When set, this bit indicates that the Layer 3 IP Source
         *  Address field is enabled for matching When reset, the MAC ignores the Layer 3 IP
         *  Source Address field for matching Note: When Bit 0 _L3PEN0_ is set, you should set
         *  either this bit or Bit 4 _L3DAM0_ because either IPv6 SA or DA can be checked for
         *  filtering
         */
        uint32_t l3sam0:1;
        /** l3saim0 : R/W; bitpos: [3]; default: 0;
         *  Layer 3 IP SA Inverse Match Enable When set, this bit indicates that the Layer 3 IP
         *  Source Address field is enabled for inverse matching When reset, this bit indicates
         *  that the Layer 3 IP Source Address field is enabled for perfect matching This bit
         *  is valid and applicable only when Bit 2 _L3SAM0_ is set high
         */
        uint32_t l3saim0:1;
        /** l3dam0 : R/W; bitpos: [4]; default: 0;
         *  Layer 3 IP DA Match Enable When set, this bit indicates that Layer 3 IP Destination
         *  Address field is enabled for matching When reset, the MAC ignores the Layer 3 IP
         *  Destination Address field for matching Note: When Bit 0 _L3PEN0_ is set, you should
         *  set either this bit or Bit 2 _L3SAM0_ because either IPv6 DA or SA can be checked
         *  for filtering
         */
        uint32_t l3dam0:1;
        /** l3daim0 : R/W; bitpos: [5]; default: 0;
         *  Layer 3 IP DA Inverse Match Enable When set, this bit indicates that the Layer 3 IP
         *  Destination Address field is enabled for inverse matching When reset, this bit
         *  indicates that the Layer 3 IP Destination Address field is enabled for perfect
         *  matching This bit is valid and applicable only when Bit 4 _L3DAM0_ is set high
         */
        uint32_t l3daim0:1;
        /** l3hsbm0 : R/W; bitpos: [10:6]; default: 0;
         *  Layer 3 IP SA Higher Bits Match IPv4 Frames: This field contains the number of
         *  lower bits of IP Source Address that are masked for matching in the IPv4 frames The
         *  following list describes the values of this field: 0: No bits are masked 1: LSb[0]
         *  is masked 2: Two LSbs [1:0] are masked  31: All bits except MSb are masked IPv6
         *  Frames: This field contains Bits [4:0] of the field that indicates the number of
         *  higher bits of IP Source or Destination Address matched in the IPv6 frames This
         *  field is valid and applicable only if L3DAM0 or L3SAM0 is set high
         */
        uint32_t l3hsbm0:5;
        /** l3hdbm0 : R/W; bitpos: [15:11]; default: 0;
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
        uint32_t l3hdbm0:5;
        /** l4pen0 : R/W; bitpos: [16]; default: 0;
         *  Layer 4 Protocol Enable When set, this bit indicates that the Source and
         *  Destination Port number fields for UDP frames are used for matching When reset,
         *  this bit indicates that the Source and Destination Port number fields for TCP
         *  frames are used for matching The Layer 4 matching is done only when either L4SPM0
         *  or L4DPM0 bit is set high
         */
        uint32_t l4pen0:1;
        uint32_t reserved_17:1;
        /** l4spm0 : R/W; bitpos: [18]; default: 0;
         *  Layer 4 Source Port Match Enable When set, this bit indicates that the Layer 4
         *  Source Port number field is enabled for matching When reset, the MAC ignores the
         *  Layer 4 Source Port number field for matching
         */
        uint32_t l4spm0:1;
        /** l4spim0 : R/W; bitpos: [19]; default: 0;
         *  Layer 4 Source Port Inverse Match Enable When set, this bit indicates that the
         *  Layer 4 Source Port number field is enabled for inverse matching When reset, this
         *  bit indicates that the Layer 4 Source Port number field is enabled for perfect
         *  matching This bit is valid and applicable only when Bit 18 _L4SPM0_ is set high
         */
        uint32_t l4spim0:1;
        /** l4dpm0 : R/W; bitpos: [20]; default: 0;
         *  Layer 4 Destination Port Match Enable When set, this bit indicates that the Layer 4
         *  Destination Port number field is enabled for matching When reset, the MAC ignores
         *  the Layer 4 Destination Port number field for matching
         */
        uint32_t l4dpm0:1;
        /** l4dpim0 : R/W; bitpos: [21]; default: 0;
         *  Layer 4 Destination Port Inverse Match Enable When set, this bit indicates that the
         *  Layer 4 Destination Port number field is enabled for inverse matching When reset,
         *  this bit indicates that the Layer 4 Destination Port number field is enabled for
         *  perfect matching This bit is valid and applicable only when Bit 20 _L4DPM0_ is set
         *  high
         */
        uint32_t l4dpim0:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} gmac_register256_layer3andlayer4controlregister0_reg_t;

/** Type of register257_layer4addressregister0 register
 *  Layer 4 Port number field It contains the 16bit Source and Destination Port numbers
 *  of the TCP or UDP frame
 */
typedef union {
    struct {
        /** l4sp0 : R/W; bitpos: [15:0]; default: 0;
         *  Layer 4 Source Port Number Field When Bit 16 _L4PEN0_ is reset and Bit 20 _L4DPM0_
         *  is set in Register 256 _Layer 3 and Layer 4 Control Register 0_, this field
         *  contains the value to be matched with the TCP Source Port Number field in the IPv4
         *  or IPv6 frames When Bit 16 _L4PEN0_ and Bit 20 _L4DPM0_ are set in Register 256
         *  _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
         *  matched with the UDP Source Port Number field in the IPv4 or IPv6 frames
         */
        uint32_t l4sp0:16;
        /** l4dp0 : R/W; bitpos: [31:16]; default: 0;
         *  Layer 4 Destination Port Number Field When Bit 16 _L4PEN0_ is reset and Bit 20
         *  _L4DPM0_ is set in Register 256 _Layer 3 and Layer 4 Control Register 0_, this
         *  field contains the value to be matched with the TCP Destination Port Number field
         *  in the IPv4 or IPv6 frames When Bit 16 _L4PEN0_ and Bit 20 _L4DPM0_ are set in
         *  Register 256 _Layer 3 and Layer 4 Control Register 0_, this field contains the
         *  value to be matched with the UDP Destination Port Number field in the IPv4 or IPv6
         *  frames
         */
        uint32_t l4dp0:16;
    };
    uint32_t val;
} gmac_register257_layer4addressregister0_reg_t;

/** Type of register260_layer3address0register0 register
 *  Layer 3 Address field For IPv4 frames, it contains the 32bit IP Source Address
 *  field For IPv6 frames, it contains Bits [31:0] of the 128bit IP Source Address or
 *  Destination Address field
 */
typedef union {
    struct {
        /** l3a00 : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t l3a00:32;
    };
    uint32_t val;
} gmac_register260_layer3address0register0_reg_t;

/** Type of register261_layer3address1register0 register
 *  Layer 3 Address 1 field For IPv4 frames, it contains the 32bit IP Destination
 *  Address field For IPv6 frames, it contains Bits [63:32] of the 128bit IP Source
 *  Address or Destination Address field
 */
typedef union {
    struct {
        /** l3a10 : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t l3a10:32;
    };
    uint32_t val;
} gmac_register261_layer3address1register0_reg_t;

/** Type of register262_layer3address2register0 register
 *  Layer 3 Address 2 field This register is reserved for IPv4 frames For IPv6 frames,
 *  it contains Bits [95:64] of the 128bit IP Source Address or Destination Address
 *  field
 */
typedef union {
    struct {
        /** l3a20 : R/W; bitpos: [31:0]; default: 0;
         *  Layer 3 Address 2 Field When Bit 0 _L3PEN0_ and Bit 2 _L3SAM0_ are set in Register
         *  256 _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
         *  matched with Bits [95:64] of the IP Source Address field in the IPv6 frames When
         *  Bit 0 _L3PEN0_ and Bit 4 _L3DAM0_ are set in Register 256 _Layer 3 and Layer 4
         *  Control Register 0_, this field contains value to be matched with Bits [95:64] of
         *  the IP Destination Address field in the IPv6 frames When Bit 0 _L3PEN0_ is reset in
         *  Register 256 _Layer 3 and Layer 4 Control Register 0_, this register is not used
         */
        uint32_t l3a20:32;
    };
    uint32_t val;
} gmac_register262_layer3address2register0_reg_t;

/** Type of register263_layer3address3register0 register
 *  Layer 3 Address 3 field This register is reserved for IPv4 frames For IPv6 frames,
 *  it contains Bits [127:96] of the 128bit IP Source Address or Destination Address
 *  field
 */
typedef union {
    struct {
        /** l3a30 : R/W; bitpos: [31:0]; default: 0;
         *  Layer 3 Address 3 Field When Bit 0 _L3PEN0_ and Bit 2 _L3SAM0_ are set in Register
         *  256 _Layer 3 and Layer 4 Control Register 0_, this field contains the value to be
         *  matched with Bits [127:96] of the IP Source Address field in the IPv6 frames When
         *  Bit 0 _L3PEN0_ and Bit 4 _L3DAM0_ are set in Register 256 _Layer 3 and Layer 4
         *  Control Register 0_, this field contains the value to be matched with Bits [127:96]
         *  of the IP Destination Address field in the IPv6 frames When Bit 0 _L3PEN0_ is reset
         *  in Register 256 _Layer 3 and Layer 4 Control Register 0_, this register is not used
         */
        uint32_t l3a30:32;
    };
    uint32_t val;
} gmac_register263_layer3address3register0_reg_t;

/** Type of register320_hashtableregister0 register
 *  This register contains the first 32 bits of the hash table when the width of the
 *  Hash table is 128 bits or 256 bits
 */
typedef union {
    struct {
        /** ht31t0 : R/W; bitpos: [31:0]; default: 0;
         *  First 32 bits of Hash Table This field contains the first 32 Bits _31:0_ of the
         *  Hash table Note Registers 321 through 327 are similar to Register 320 _Hash Table
         *  Register 0_ Registers 324 through 327 are present only when you select the 256bit
         *  Hash table during core configuration
         */
        uint32_t ht31t0:32;
    };
    uint32_t val;
} gmac_register320_hashtableregister0_reg_t;

/** Type of register353_vlantaginclusionorreplacementregister register
 *  This register contains the VLAN tag for insertion into or replacement in the
 *  transmit frames
 */
typedef union {
    struct {
        /** vlt : R/W; bitpos: [15:0]; default: 0;
         *  VLAN Tag for Transmit Frames This field contains the value of the VLAN tag to be
         *  inserted or replaced The value must only be changed when the transmit lines are
         *  inactive or during the initialization phase Bits[15:13] are the User Priority, Bit
         *  12 is the CFI/DEI, and Bits[11:0] are the VLAN tag’s VID field
         */
        uint32_t vlt:16;
        /** vlc : R/W; bitpos: [17:16]; default: 0;
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
        uint32_t vlc:2;
        /** vlp : R/W; bitpos: [18]; default: 0;
         *  VLAN Priority Control When this bit is set, the control Bits [17:16] are used for
         *  VLAN deletion, insertion, or replacement When this bit is reset, the
         *  mti_vlan_ctrl_i control input is used, and Bits [17:16] are ignored
         */
        uint32_t vlp:1;
        /** csvl : R/W; bitpos: [19]; default: 0;
         *  CVLAN or SVLAN When this bit is set, SVLAN type _0x88A8_ is inserted or replaced in
         *  the 13th and 14th bytes of transmitted frames When this bit is reset, CVLAN type
         *  _0x8100_ is inserted or replaced in the transmitted frames
         */
        uint32_t csvl:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} gmac_register353_vlantaginclusionorreplacementregister_reg_t;

/** Type of register354_vlanhashtableregister register
 *  This register contains the VLAN hash table
 */
typedef union {
    struct {
        /** vlht : R/W; bitpos: [15:0]; default: 0;
         *  VLAN Hash Table This field contains the 16bit VLAN Hash Table
         */
        uint32_t vlht:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gmac_register354_vlanhashtableregister_reg_t;

/** Type of register544_macaddress32highregister register
 *  Contains the higher 16 bits of the 33rd MAC address This register is present only
 *  when Enable Additional 32 MAC Address Registers is selected in coreConsultant _See
 *  Table 78_
 */
typedef union {
    struct {
        /** addrhi : R/W; bitpos: [15:0]; default: 65535;
         *  MAC Address32 [47:32] This field contains the upper 16 bits _47:32_ of the 33rd
         *  6byte MAC address
         */
        uint32_t addrhi:16;
        uint32_t reserved_16:15;
        /** ae : R/W; bitpos: [31]; default: 0;
         *  Address Enable When this bit is set, the Address filter module uses the 33rd MAC
         *  address for perfect filtering When reset, the address filter module ignores the
         *  address for filtering
         */
        uint32_t ae:1;
    };
    uint32_t val;
} gmac_register544_macaddress32highregister_reg_t;


/** Group: IEEE 1588 Timestamp register */
/** Type of register448_timestampcontrolregister register
 *  Controls the timestamp generation and update logic This register is present only
 *  when IEEE1588 timestamping is enabled during coreConsultant configuration
 */
typedef union {
    struct {
        /** tsena : R/W; bitpos: [0]; default: 0;
         *  Timestamp Enable When set, the timestamp is added for the transmit and receive
         *  frames When disabled, timestamp is not added for the transmit and receive frames
         *  and the Timestamp Generator is also suspended You need to initialize the Timestamp
         *  _system time_ after enabling this mode On the receive side, the MAC processes the
         *  1588 frames only if this bit is set
         */
        uint32_t tsena:1;
        /** tscfupdt : R/W; bitpos: [1]; default: 0;
         *  Timestamp Fine or Coarse Update When set, this bit indicates that the system times
         *  update should be done using the fine update method When reset, it indicates the
         *  system timestamp update should be done using the Coarse method
         */
        uint32_t tscfupdt:1;
        /** tsinit : R/W1S; bitpos: [2]; default: 0;
         *  Timestamp Initialize When set, the system time is initialized _overwritten_ with
         *  the value specified in the Register 452 _System Time  Seconds Update Register_ and
         *  Register 453 _System Time  Nanoseconds Update Register_ This bit should be read
         *  zero before updating it This bit is reset when the initialization is complete The
         *  “Timestamp Higher Word” register _if enabled during core configuration_ can only be
         *  initialized
         */
        uint32_t tsinit:1;
        /** tsupdt : R/W1S; bitpos: [3]; default: 0;
         *  Timestamp Update When set, the system time is updated _added or subtracted_ with
         *  the value specified in Register 452 _System Time  Seconds Update Register_ and
         *  Register 453 _System Time  Nanoseconds Update Register_ This bit should be read
         *  zero before updating it This bit is reset when the update is completed in hardware
         *  The “Timestamp Higher Word” register _if enabled during core configuration_ is not
         *  updated
         */
        uint32_t tsupdt:1;
        /** tstrig : R/W1S; bitpos: [4]; default: 0;
         *  Timestamp Interrupt Trigger Enable When set, the timestamp interrupt is generated
         *  when the System Time becomes greater than the value written in the Target Time
         *  register This bit is reset after the generation of the Timestamp Trigger Interrupt
         */
        uint32_t tstrig:1;
        /** tsaddreg : R/W1S; bitpos: [5]; default: 0;
         *  Addend Reg Update When set, the content of the Timestamp Addend register is updated
         *  in the PTP block for fine correction This is cleared when the update is completed
         *  This register bit should be zero before setting it
         */
        uint32_t tsaddreg:1;
        uint32_t reserved_6:2;
        /** tsenall : R/W; bitpos: [8]; default: 0;
         *  Enable Timestamp for All Frames When set, the timestamp snapshot is enabled for all
         *  frames received by the MAC
         */
        uint32_t tsenall:1;
        /** tsctrlssr : R/W; bitpos: [9]; default: 0;
         *  Timestamp Digital or Binary Rollover Control When set, the Timestamp Low register
         *  rolls over after 0x3B9A_C9FF value _that is, 1 nanosecond accuracy_ and increments
         *  the timestamp _High_ seconds When reset, the rollover value of subsecond register
         *  is 0x7FFF_FFFF The subsecond increment has to be programmed correctly depending on
         *  the PTP reference clock frequency and the value of this bit
         */
        uint32_t tsctrlssr:1;
        /** tsver2ena : R/W; bitpos: [10]; default: 0;
         *  Enable PTP packet Processing for Version 2 Format When set, the PTP packets are
         *  processed using the 1588 version 2 format Otherwise, the PTP packets are processed
         *  using the version 1 format The IEEE 1588 Version 1 and Version 2 format are
         *  described in “PTP Processing and Control” on page 155
         */
        uint32_t tsver2ena:1;
        /** tsipena : R/W; bitpos: [11]; default: 0;
         *  Enable Processing of PTP over Ethernet Frames When set, the MAC receiver processes
         *  the PTP packets encapsulated directly in the Ethernet frames When this bit is
         *  clear, the MAC ignores the PTP over Ethernet packets
         */
        uint32_t tsipena:1;
        /** tsipv6ena : R/W; bitpos: [12]; default: 0;
         *  Enable Processing of PTP Frames Sent over IPv6UDP When set, the MAC receiver
         *  processes PTP packets encapsulated in UDP over IPv6 packets When this bit is clear,
         *  the MAC ignores the PTP transported over UDPIPv6 packets
         */
        uint32_t tsipv6ena:1;
        /** tsipv4ena : R/W; bitpos: [13]; default: 1;
         *  Enable Processing of PTP Frames Sent over IPv4UDP When set, the MAC receiver
         *  processes the PTP packets encapsulated in UDP over IPv4 packets When this bit is
         *  clear, the MAC ignores the PTP transported over UDPIPv4 packets This bit is set by
         *  default
         */
        uint32_t tsipv4ena:1;
        /** tsevntena : R/W; bitpos: [14]; default: 0;
         *  Enable Timestamp Snapshot for Event Messages When set, the timestamp snapshot is
         *  taken only for event messages _SYNC, Delay_Req, Pdelay_Req, or Pdelay_Resp_ When
         *  reset, the snapshot is taken for all messages except Announce, Management, and
         *  Signaling For more information about the timestamp snapshots, see Table 670 on page
         *  462
         */
        uint32_t tsevntena:1;
        /** tsmstrena : R/W; bitpos: [15]; default: 0;
         *  Enable Snapshot for Messages Relevant to Master When set, the snapshot is taken
         *  only for the messages relevant to the master node Otherwise, the snapshot is taken
         *  for the messages relevant to the slave node
         */
        uint32_t tsmstrena:1;
        /** snaptypsel : R/W; bitpos: [17:16]; default: 0;
         *  Select PTP packets for Taking Snapshots These bits along with Bits 15 and 14 decide
         *  the set of PTP packet types for which snapshot needs to be taken The encoding is
         *  given in Table 670 on page 462
         */
        uint32_t snaptypsel:2;
        /** tsenmacaddr : R/W; bitpos: [18]; default: 0;
         *  Enable MAC address for PTP Frame Filtering When set, the DA MAC address _that
         *  matches any MAC Address register_ is used to filter the PTP frames when PTP is
         *  directly sent over Ethernet
         */
        uint32_t tsenmacaddr:1;
        uint32_t reserved_19:5;
        /** atsfc : R/W1S; bitpos: [24]; default: 0;
         *  Auxiliary Snapshot FIFO Clear When set, it resets the pointers of the Auxiliary
         *  Snapshot FIFO This bit is cleared when the pointers are reset and the FIFO is empty
         *  When this bit is high, auxiliary snapshots get stored in the FIFO This bit is
         *  reserved when the Add IEEE 1588 Auxiliary Snapshot option is not selected during
         *  core configuration
         */
        uint32_t atsfc:1;
        /** atsen0 : R/W; bitpos: [25]; default: 0;
         *  Auxiliary Snapshot 0 Enable This field controls capturing the Auxiliary Snapshot
         *  Trigger 0 When this bit is set, the Auxiliary snapshot of event on
         *  ptp_aux_trig_i[0] input is enabled When this bit is reset, the events on this input
         *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
         *  is not selected during core configuration
         */
        uint32_t atsen0:1;
        /** atsen1 : R/W; bitpos: [26]; default: 0;
         *  Auxiliary Snapshot 1 Enable This field controls capturing the Auxiliary Snapshot
         *  Trigger 1 When this bit is set, the Auxiliary snapshot of event on
         *  ptp_aux_trig_i[1] input is enabled When this bit is reset, the events on this input
         *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
         *  is not selected during core configuration or the selected number in the Number of
         *  IEEE 1588 Auxiliary Snapshot Inputs option is less than two
         */
        uint32_t atsen1:1;
        /** atsen2 : R/W; bitpos: [27]; default: 0;
         *  Auxiliary Snapshot 2 Enable This field controls capturing the Auxiliary Snapshot
         *  Trigger 2 When this bit is set, the Auxiliary snapshot of event on
         *  ptp_aux_trig_i[2] input is enabled When this bit is reset, the events on this input
         *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
         *  is not selected during core configuration or the selected number in the Number of
         *  IEEE 1588 Auxiliary Snapshot Inputs option is less than three
         */
        uint32_t atsen2:1;
        /** atsen3 : R/W; bitpos: [28]; default: 0;
         *  Auxiliary Snapshot 3 Enable This field controls capturing the Auxiliary Snapshot
         *  Trigger 3 When this bit is set, the Auxiliary snapshot of event on
         *  ptp_aux_trig_i[3] input is enabled When this bit is reset, the events on this input
         *  are ignored This bit is reserved when the Add IEEE 1588 Auxiliary Snapshot option
         *  is not selected during core configuration or the selected number in the Number of
         *  IEEE 1588 Auxiliary Snapshot Inputs option is less than four
         */
        uint32_t atsen3:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} gmac_register448_timestampcontrolregister_reg_t;

/** Type of register449_subsecondincrementregister register
 *  Contains the 8bit value by which the SubSecond register is incremented This
 *  register is present only when IEEE1588 timestamping is enabled without an external
 *  timestamp input
 */
typedef union {
    struct {
        /** ssinc : R/W; bitpos: [7:0]; default: 0;
         *  Subsecond Increment Value The value programmed in this field is accumulated every
         *  clock cycle _of clk_ptp_i_ with the contents of the subsecond register For example,
         *  when PTP clock is 50 MHz _period is 20 ns_, you should program 20 _0x14_ when the
         *  System Time Nanoseconds register has an accuracy of 1 ns [Bit 9 _TSCTRLSSR_ is set
         *  in Register 448 _Timestamp Control Register_] When TSCTRLSSR is clear, the
         *  Nanoseconds register has a resolution of ~0465ns In this case, you should program a
         *  value of 43 _0x2B_ that is derived by 20ns/0465
         */
        uint32_t ssinc:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gmac_register449_subsecondincrementregister_reg_t;

/** Type of register450_systemtimesecondsregister register
 *  Contains the lower 32 bits of the seconds field of the system time This register is
 *  present only when IEEE1588 timestamping is enabled without an external timestamp
 *  input
 */
typedef union {
    struct {
        /** tss_ro : RO; bitpos: [31:0]; default: 0;
         *  Timestamp Second The value in this field indicates the current value in seconds of
         *  the System Time maintained by the MAC
         */
        uint32_t tss_ro:32;
    };
    uint32_t val;
} gmac_register450_systemtimesecondsregister_reg_t;

/** Type of register451_systemtimenanosecondsregister register
 *  Contains 32 bits of the nanoseconds field of the system time This register is only
 *  present when IEEE1588 timestamping is enabled without an external timestamp input
 */
typedef union {
    struct {
        /** tsss_ro : RO; bitpos: [30:0]; default: 0;
         *  Timestamp Sub Seconds The value in this field has the sub second representation of
         *  time, with an accuracy of 046 ns When Bit 9 _TSCTRLSSR_ is set in Register 448
         *  _Timestamp Control Register_, each bit represents 1 ns and the maximum value is
         *  0x3B9A_C9FF, after which it rollsover to zero
         */
        uint32_t tsss_ro:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} gmac_register451_systemtimenanosecondsregister_reg_t;

/** Type of register452_systemtimesecondsupdateregister register
 *  Contains the lower 32 bits of the seconds field to be written to, added to, or
 *  subtracted from the System Time value This register is only present when IEEE1588
 *  timestamping is enabled without an external timestamp input
 */
typedef union {
    struct {
        /** tss : R/W; bitpos: [31:0]; default: 0;
         *  TIMESTAMP SECOND THE VALUE IN THIS FIELD INDICATES THE TIME IN SECONDS TO BE
         *  INITIALIZED OR ADDED TO THE SYSTEM TIME
         */
        uint32_t tss:32;
    };
    uint32_t val;
} gmac_register452_systemtimesecondsupdateregister_reg_t;

/** Type of register453_systemtimenanosecondsupdateregister register
 *  Contains 32 bits of the nanoseconds field to be written to, added to, or subtracted
 *  from the System Time value This register is only present when IEEE1588 timestamping
 *  is enabled without an external timestamp input
 */
typedef union {
    struct {
        /** tsss : R/W; bitpos: [30:0]; default: 0;
         *  Timestamp Sub Seconds The value in this field has the sub second representation of
         *  time, with an accuracy of 046 ns When Bit 9 _TSCTRLSSR_ is set in Register 448
         *  _Timestamp Control Register_, each bit represents 1 ns and the programmed value
         *  should not exceed 0x3B9A_C9FF
         */
        uint32_t tsss:31;
        /** addsub : R/W; bitpos: [31]; default: 0;
         *  Add or Subtract Time When this bit is set, the time value is subtracted with the
         *  contents of the update register When this bit is reset, the time value is added
         *  with the contents of the update register
         */
        uint32_t addsub:1;
    };
    uint32_t val;
} gmac_register453_systemtimenanosecondsupdateregister_reg_t;

/** Type of register454_timestampaddendregister register
 *  This register is used by the software to readjust the clock frequency linearly to
 *  match the master clock frequency This register is only present when IEEE1588
 *  timestamping is enabled without an external timestamp input
 */
typedef union {
    struct {
        /** tsar : R/W; bitpos: [31:0]; default: 0;
         *  Timestamp Addend Register This field indicates the 32bit time value to be added to
         *  the Accumulator register to achieve time synchronization
         */
        uint32_t tsar:32;
    };
    uint32_t val;
} gmac_register454_timestampaddendregister_reg_t;

/** Type of register455_targettimesecondsregister register
 *  Contains the higher 32 bits of time to be compared with the system time for
 *  interrupt event generation or to start the PPS signal output generation This
 *  register is present only when IEEE1588 timestamping is enabled without an external
 *  timestamp input
 */
typedef union {
    struct {
        /** tstr : R/W; bitpos: [31:0]; default: 0;
         *  Target Time Seconds Register This register stores the time in seconds When the
         *  timestamp value matches or exceeds both Target Timestamp registers, then based on
         *  Bits [6:5] of Register 459 _PPS Control Register_, the MAC starts or stops the PPS
         *  signal output and generates an interrupt _if enabled_
         */
        uint32_t tstr:32;
    };
    uint32_t val;
} gmac_register455_targettimesecondsregister_reg_t;

/** Type of register456_targettimenanosecondsregister register
 *  Contains the lower 32 bits of time to be compared with the system time for
 *  interrupt event generation or to start the PPS signal output generation This
 *  register is present only when IEEE1588 timestamping is enabled without an external
 *  timestamp input
 */
typedef union {
    struct {
        /** ttslo : R/W; bitpos: [30:0]; default: 0;
         *  Target Timestamp Low Register This register stores the time in _signed_ nanoseconds
         *  When the value of the timestamp matches the both Target Timestamp registers, then
         *  based on the TRGTMODSEL0 field _Bits [6:5]_ in Register 459 _PPS Control Register_,
         *  the MAC starts or stops the PPS signal output and generates an interrupt _if
         *  enabled_ This value should not exceed 0x3B9A_C9FF when Bit 9 _TSCTRLSSR_ is set in
         *  Register 448 _Timestamp Control Register_ The actual start or stop time of the PPS
         *  signal output may have an error margin up to one unit of subsecond increment value
         */
        uint32_t ttslo:31;
        /** trgtbusy : R/W1S; bitpos: [31]; default: 0;
         *  Target Time Register Busy The MAC sets this bit when the PPSCMD field _Bit [3:0]_
         *  in Register 459 _PPS Control Register_ is programmed to 010 or 011 Programming the
         *  PPSCMD field to 010 or 011, instructs the MAC to synchronize the Target Time
         *  Registers to the PTP clock domain The MAC clears this bit after synchronizing the
         *  Target Time Registers to the PTP clock domain The application must not update the
         *  Target Time Registers when this bit is read as 1 Otherwise, the synchronization of
         *  the previous programmed time gets corrupted This bit is reserved when the Enable
         *  Flexible PulsePerSecond Output feature is not selected
         */
        uint32_t trgtbusy:1;
    };
    uint32_t val;
} gmac_register456_targettimenanosecondsregister_reg_t;

/** Type of register457_systemtimehigherwordsecondsregister register
 *  Contains the most significant 16bits of the timestamp seconds value This register
 *  is optional and can be selected using the parameter mentioned in “IEEE 1588
 *  Timestamp Block” on page 492
 */
typedef union {
    struct {
        /** tshwr : R/W; bitpos: [15:0]; default: 0;
         *  Timestamp Higher Word Register This field contains the most significant 16bits of
         *  the timestamp seconds value This register is optional and can be selected using the
         *  Enable IEEE 1588 Higher Word Register option during core configuration The register
         *  is directly written to initialize the value This register is incremented when there
         *  is an overflow from the 32bits of the System Time  Seconds register
         */
        uint32_t tshwr:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gmac_register457_systemtimehigherwordsecondsregister_reg_t;

/** Type of register458_timestampstatusregister register
 *  Contains the PTP status This register is available only when the advanced IEEE 1588
 *  timestamp feature is selected
 */
typedef union {
    struct {
        /** tssovf : R/W; bitpos: [0]; default: 0;
         *  Timestamp Seconds Overflow When set, this bit indicates that the seconds value of
         *  the timestamp _when supporting version 2 format_ has overflowed beyond 32’hFFFF_FFFF
         */
        uint32_t tssovf:1;
        /** tstargt : R/W; bitpos: [1]; default: 0;
         *  Timestamp Target Time Reached When set, this bit indicates that the value of system
         *  time is greater than or equal to the value specified in the Register 455 _Target
         *  Time Seconds Register_ and Register 456 _Target Time Nanoseconds Register_
         */
        uint32_t tstargt:1;
        /** auxtstrig : R/W; bitpos: [2]; default: 0;
         *  Auxiliary Timestamp Trigger Snapshot This bit is set high when the auxiliary
         *  snapshot is written to the FIFO This bit is valid only if the Enable IEEE 1588
         *  Auxiliary Snapshot feature is selected
         */
        uint32_t auxtstrig:1;
        /** tstrgterr : R/W; bitpos: [3]; default: 0;
         *  Timestamp Target Time Error This bit is set when the target time, being programmed
         *  in Register 455 and Register 456, is already elapsed This bit is cleared when read
         *  by the application
         */
        uint32_t tstrgterr:1;
        /** tstargt1 : R/W; bitpos: [4]; default: 0;
         *  Timestamp Target Time Reached for Target Time PPS1 When set, this bit indicates
         *  that the value of system time is greater than or equal to the value specified in
         *  Register 480 _PPS1 Target Time High Register_ and Register 481 _PPS1 Target Time
         *  Low Register_
         */
        uint32_t tstargt1:1;
        /** tstrgterr1 : R/W; bitpos: [5]; default: 0;
         *  Timestamp Target Time Error This bit is set when the target time, being programmed
         *  in Register 480 and Register 481, is already elapsed This bit is cleared when read
         *  by the application
         */
        uint32_t tstrgterr1:1;
        /** tstargt2 : R/W; bitpos: [6]; default: 0;
         *  Timestamp Target Time Reached for Target Time PPS2 When set, this bit indicates
         *  that the value of system time is greater than or equal to the value specified in
         *  Register 488 _PPS2 Target Time High Register_ and Register 489 _PPS2 Target Time
         *  Low Register_
         */
        uint32_t tstargt2:1;
        /** tstrgterr2 : R/W; bitpos: [7]; default: 0;
         *  Timestamp Target Time Error This bit is set when the target time, being programmed
         *  in Register 488 and Register 489, is already elapsed This bit is cleared when read
         *  by the application
         */
        uint32_t tstrgterr2:1;
        /** tstargt3 : R/W; bitpos: [8]; default: 0;
         *  Timestamp Target Time Reached for Target Time PPS3 When set, this bit indicates
         *  that the value of system time is greater than or equal to the value specified in
         *  Register 496 _PPS3 Target Time High Register_ and Register 497 _PPS3 Target Time
         *  Low Register_
         */
        uint32_t tstargt3:1;
        /** tstrgterr3 : R/W; bitpos: [9]; default: 0;
         *  Timestamp Target Time Error This bit is set when the target time, being programmed
         *  in Register 496 and Register 497, is already elapsed This bit is cleared when read
         *  by the application
         */
        uint32_t tstrgterr3:1;
        uint32_t reserved_10:6;
        /** atsstn : R/W; bitpos: [19:16]; default: 0;
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
        uint32_t atsstn:4;
        uint32_t reserved_20:4;
        /** atsstm : RO; bitpos: [24]; default: 0;
         *  Auxiliary Timestamp Snapshot Trigger Missed
         */
        uint32_t atsstm:1;
        /** atsns : RO; bitpos: [29:25]; default: 0;
         *  Number of Auxiliary Timestamp Snapshots This field indicates the number of
         *  Snapshots available in the FIFO A value equal to the selected depth of FIFO _4, 8,
         *  or 16_ indicates that the Auxiliary Snapshot FIFO is full These bits are cleared
         *  _to 00000_ when the Auxiliary snapshot FIFO clear bit is set This bit is valid only
         *  if the Add IEEE 1588 Auxiliary Snapshot option is selected during core configuration
         */
        uint32_t atsns:5;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gmac_register458_timestampstatusregister_reg_t;

/** Type of register459_ppscontrolregister register
 *  This register is used to control the interval of the PPS signal output This
 *  register is available only when the advanced IEEE 1588 timestamp feature is selected
 */
typedef union {
    struct {
        /** ppsctrl0 : R/W1S; bitpos: [3:0]; default: 0;
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
        uint32_t ppsctrl0:4;
        /** ppsen0 : R/W; bitpos: [4]; default: 0;
         *  Flexible PPS Output Mode Enable When set low, Bits [3:0] function as PPSCTRL
         *  _backward compatible_ When set high, Bits[3:0] function as PPSCMD
         */
        uint32_t ppsen0:1;
        /** trgtmodsel0 : R/W; bitpos: [6:5]; default: 0;
         *  Target Time Register Mode for PPS0 Output This field indicates the Target Time
         *  registers _register 455 and 456_ mode for PPS0 output signal: 00: Indicates that
         *  the Target Time registers are programmed only for generating the interrupt event
         *  01: Reserved 10: Indicates that the Target Time registers are programmed for
         *  generating the interrupt event and starting or stopping the generation of the PPS0
         *  output signal 11: Indicates that the Target Time registers are programmed only for
         *  starting or stopping the generation of the PPS0 output signal No interrupt is
         *  asserted
         */
        uint32_t trgtmodsel0:2;
        uint32_t reserved_7:1;
        /** ppscmd1 : R/W1S; bitpos: [10:8]; default: 0;
         *  Flexible PPS1 Output Control This field controls the flexible PPS1 output
         *  _ptp_pps_o[1]_ signal This field is similar to PPSCMD0[2:0] in functionality
         */
        uint32_t ppscmd1:3;
        uint32_t reserved_11:2;
        /** trgtmodsel1 : R/W; bitpos: [14:13]; default: 0;
         *  Target Time Register Mode for PPS1 Output This field indicates the Target Time
         *  registers _register 480 and 481_ mode for PPS1 output signal This field is similar
         *  to the TRGTMODSEL0 field
         */
        uint32_t trgtmodsel1:2;
        uint32_t reserved_15:1;
        /** ppscmd2 : R/W1S; bitpos: [18:16]; default: 0;
         *  Flexible PPS2 Output Control This field controls the flexible PPS2 output
         *  _ptp_pps_o[2]_ signal This field is similar to PPSCMD0[2:0] in functionality
         */
        uint32_t ppscmd2:3;
        uint32_t reserved_19:2;
        /** trgtmodsel2 : R/W; bitpos: [22:21]; default: 0;
         *  Target Time Register Mode for PPS2 Output This field indicates the Target Time
         *  registers _register 488 and 489_ mode for PPS2 output signal This field is similar
         *  to the TRGTMODSEL0 field
         */
        uint32_t trgtmodsel2:2;
        uint32_t reserved_23:1;
        /** ppscmd3 : R/W1S; bitpos: [26:24]; default: 0;
         *  Flexible PPS3 Output Control This field controls the flexible PPS3 output
         *  _ptp_pps_o[3]_ signal This field is similar to PPSCMD0[2:0] in functionality
         */
        uint32_t ppscmd3:3;
        uint32_t reserved_27:2;
        /** trgtmodsel3 : R/W; bitpos: [30:29]; default: 0;
         *  Target Time Register Mode for PPS3 Output This field indicates the Target Time
         *  registers _register 496 and 497_ mode for PPS3 output signal This field is similar
         *  to the TRGTMODSEL0 field
         */
        uint32_t trgtmodsel3:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} gmac_register459_ppscontrolregister_reg_t;

/** Type of register460_auxiliarytimestampnanosecondsregister register
 *  Contains the lower 32 bits _nanoseconds field_ of the auxiliary timestamp register
 */
typedef union {
    struct {
        /** auxtslo : RO; bitpos: [30:0]; default: 0;
         *  Contains the lower 31 bits _nanoseconds field_ of the auxiliary timestamp
         */
        uint32_t auxtslo:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} gmac_register460_auxiliarytimestampnanosecondsregister_reg_t;

/** Type of register461_auxiliarytimestampsecondsregister register
 *  Contains the lower 32 bits of the Seconds field of the auxiliary timestamp register
 */
typedef union {
    struct {
        /** auxtshi : RO; bitpos: [31:0]; default: 0;
         *  Contains the lower 32 bits of the Seconds field of the auxiliary timestamp
         */
        uint32_t auxtshi:32;
    };
    uint32_t val;
} gmac_register461_auxiliarytimestampsecondsregister_reg_t;

/** Type of register462_avmaccontrolregister register
 *  Controls the AV traffic and queue management in the MAC Receiver This register is
 *  present only when you select the AV feature in coreConsultant
 */
typedef union {
    struct {
        /** avt : R/W; bitpos: [15:0]; default: 0;
         *  AV EtherType Value This field contains the value that is compared with the
         *  EtherType field of the incoming _tagged or untagged_ Ethernet frame to detect an AV
         *  packet
         */
        uint32_t avt:16;
        /** avp : R/W; bitpos: [18:16]; default: 0;
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
        uint32_t avp:3;
        /** vqe : R/W; bitpos: [19]; default: 0;
         *  VLAN Tagged NonAV Packets Queueing Enable When this bit is set, the MAC also queues
         *  nonAV VLAN tagged packets into the available channels according to the value of the
         *  AVP bits This bit is reserved and readonly if Channel 1 and Channel 2 Receive paths
         *  are not selected during core configuration
         */
        uint32_t vqe:1;
        /** avcd : R/W; bitpos: [20]; default: 0;
         *  AV Channel Disable When this bit is set, the MAC forwards all packets to the
         *  default Channel 0 and the values programmed in the AVP, AVCH, and PTPCH fields are
         *  ignored This bit is reserved and readonly if Channel 1 or Channel 2 receive paths
         *  are not selected during core configuration
         */
        uint32_t avcd:1;
        /** avch : R/W; bitpos: [22:21]; default: 0;
         *  Channel for Queuing the AV Control Packets This field specifies the channel on
         *  which the received untagged AV control packets are queued 00: Channel 0 01: Channel
         *  1 10: Channel 2 11: Reserved These bits are reserved if the receive paths of
         *  Channel 1 or Channel 2 are not enabled
         */
        uint32_t avch:2;
        uint32_t reserved_23:1;
        /** ptpch : R/W; bitpos: [25:24]; default: 0;
         *  Channel for Queuing the PTP Packets This field specifies the channel on which the
         *  untagged PTP packets, sent over the Ethernet payload and not over IPv4 or IPv6, are
         *  queued 00: Channel 0 01: Channel 1 10: Channel 2 11: Reserved These bits are
         *  reserved if the receive paths of Channel 1 or Channel 2 are not enabled
         */
        uint32_t ptpch:2;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} gmac_register462_avmaccontrolregister_reg_t;

/** Type of register472_pps0intervalregister register
 *  Contains the number of units of subsecond increment value between the rising edges
 *  of PPS0 signal output This register is available only when the flexible PPS feature
 *  is selected
 */
typedef union {
    struct {
        /** ppsint : R/W; bitpos: [31:0]; default: 0;
         *  PPS0 Output Signal Interval These bits store the interval between the rising edges
         *  of PPS0 signal output in terms of units of subsecond increment value You need to
         *  program one value less than the required interval For example, if the PTP reference
         *  clock is 50 MHz _period of 20ns_, and desired interval between rising edges of PPS0
         *  signal output is 100ns _that is, five units of subsecond increment value_, then you
         *  should program value 4 _5  1_ in this register
         */
        uint32_t ppsint:32;
    };
    uint32_t val;
} gmac_register472_pps0intervalregister_reg_t;

/** Type of register473_pps0widthregister register
 *  Contains the number of units of subsecond increment value between the rising and
 *  corresponding falling edges of PPS0 signal output This register is available only
 *  when the flexible PPS feature is selected
 */
typedef union {
    struct {
        /** ppswidth : R/W; bitpos: [31:0]; default: 0;
         *  PPS0 Output Signal Width These bits store the width between the rising edge and
         *  corresponding falling edge of the PPS0 signal output in terms of units of subsecond
         *  increment value You need to program one value less than the required interval For
         *  example, if PTP reference clock is 50 MHz _period of 20ns_, and desired width
         *  between the rising and corresponding falling edges of PPS0 signal output is 80ns
         *  _that is, four units of subsecond increment value_, then you should program value 3
         *  _4  1_ in this register Note: The value programmed in this register must be lesser
         *  than the value programmed in Register 472 _PPS0 Interval Register_
         */
        uint32_t ppswidth:32;
    };
    uint32_t val;
} gmac_register473_pps0widthregister_reg_t;

/** Type of register480_pps1targettimesecondsregister register
 *  Contains the higher 32 bits of time to be compared with the system time to generate
 *  the interrupt event or to start generating the PPS1 output signal This register is
 *  present only when IEEE1588 timestamping is enabled without an external timestamp
 *  input and at least one additional PPS output is selected
 */
typedef union {
    struct {
        /** tstrh1 : R/W; bitpos: [31:0]; default: 0;
         *  PPS1 Target Time Seconds Register This register stores the time in seconds When the
         *  timestamp value matches or exceeds both Target Timestamp registers, then based on
         *  Bits [14:13], TRGTMODSEL1, of Register 459 _PPS Control Register_, the MAC starts
         *  or stops the PPS signal output and generates an interrupt _if enabled_
         */
        uint32_t tstrh1:32;
    };
    uint32_t val;
} gmac_register480_pps1targettimesecondsregister_reg_t;

/** Type of register481_pps1targettimenanosecondsregister register
 *  Contains the lower 32 bits of time to be compared with the system time to generate
 *  the interrupt event or to start generating the PPS1 output signal This register is
 *  present only when IEEE1588 timestamping is enabled without an external timestamp
 *  input and at least one additional PPS output is selected
 */
typedef union {
    struct {
        /** ttsl1 : R/W; bitpos: [30:0]; default: 0;
         *  Target Time Low for PPS1 Register This register stores the time in _signed_
         *  nanoseconds When the value of the timestamp matches the both Target Timestamp
         *  registers, then based on the TRGTMODSEL1 field _Bits [14:13]_ in Register 459 _PPS
         *  Control Register_, the MAC starts or stops the PPS signal output and generates an
         *  interrupt _if enabled_ This value should not exceed 0x3B9A_C9FF when Bit 9
         *  _TSCTRLSSR_ is set in Register 448 _Timestamp Control Register_ The actual start or
         *  stop time of the PPS signal output may have an error margin up to one unit of
         *  subsecond increment value
         */
        uint32_t ttsl1:31;
        /** trgtbusy1 : R/W1S; bitpos: [31]; default: 0;
         *  PPS1 Target Time Register Busy The MAC sets this bit when the PPSCMD1 field _Bits
         *  [10:8]_ in Register 459 _PPS Control Register_ is programmed to 010 or 011
         *  Programming the PPSCMD1 field to 010 or 011 instructs the MAC to synchronize the
         *  Target Time Registers to the PTP clock domain The MAC clears this bit after
         *  synchronizing the Target Time Registers to the PTP clock domain The application
         *  must not update the Target Time Registers when this bit is read as 1 Otherwise, the
         *  synchronization of the previous programmed time gets corrupted
         */
        uint32_t trgtbusy1:1;
    };
    uint32_t val;
} gmac_register481_pps1targettimenanosecondsregister_reg_t;


/** Group: dma register */
/** Type of register0_busmoderegister register
 *  Controls the Host Interface Mode
 */
typedef union {
    struct {
        /** swr : R/W1S_SC; bitpos: [0]; default: 1;
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
        uint32_t swr:1;
        /** da : R/W; bitpos: [1]; default: 0;
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
        uint32_t da:1;
        /** dsl : R/W; bitpos: [6:2]; default: 0;
         *  Descriptor Skip Length This bit specifies the number of Word, Dword, or Lword
         *  _depending on the 32bit, 64bit, or 128bit bus_ to skip between two unchained
         *  descriptors The address skipping starts from the end of current descriptor to the
         *  start of next descriptor When the DSL value is equal to zero, the descriptor table
         *  is taken as contiguous by the DMA in Ring mode
         */
        uint32_t dsl:5;
        /** atds : R/W; bitpos: [7]; default: 0;
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
        uint32_t atds:1;
        /** pbl : R/W; bitpos: [13:8]; default: 1;
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
        uint32_t pbl:6;
        /** pr : R/W; bitpos: [15:14]; default: 0;
         *  Priority Ratio These bits control the priority ratio in the weighted roundrobin
         *  arbitration between the Rx DMA and Tx DMA These bits are valid only when Bit 1 _DA_
         *  is reset The priority ratio is Rx:Tx or Tx:Rx depending on whether Bit 27 _TXPR_ is
         *  reset or set 00: The Priority Ratio is 1:1 01: The Priority Ratio is 2:1 10: The
         *  Priority Ratio is 3:1 11: The Priority Ratio is 4:1 In the GMACAXI configuration,
         *  these bits are reserved and readonly _RO_ For more information about the priority
         *  scheme between the transmit and receive paths, see Table 412 in “DMA Arbiter
         *  Functions” on page 167
         */
        uint32_t pr:2;
        /** fb : R/W; bitpos: [16]; default: 0;
         *  Fixed Burst This bit controls whether the AHB or AXI master interface performs
         *  fixed burst transfers or not When set, the AHB interface uses only SINGLE, INCR4,
         *  INCR8, or INCR16 during start of the normal burst transfers When reset, the AHB or
         *  AXI interface uses SINGLE and INCR burst transfer operations For more information,
         *  see Bit 0 _UNDEF_ of the AXI Bus Mode register in the GMACAXI configuration
         */
        uint32_t fb:1;
        /** rpbl : R/W; bitpos: [22:17]; default: 1;
         *  Rx DMA PBL This field indicates the maximum number of beats to be transferred in
         *  one Rx DMA transaction This is the maximum value that is used in a single block
         *  Read or Write The Rx DMA always attempts to burst as specified in the RPBL bit each
         *  time it starts a Burst transfer on the host bus You can program RPBL with values of
         *  1, 2, 4, 8, 16, and 32 Any other value results in undefined behavior This field is
         *  valid and applicable only when USP is set high
         */
        uint32_t rpbl:6;
        /** usp : R/W; bitpos: [23]; default: 0;
         *  Use Separate PBL When set high, this bit configures the Rx DMA to use the value
         *  configured in Bits [22:17] as PBL The PBL value in Bits [13:8] is applicable only
         *  to the Tx DMA operations When reset to low, the PBL value in Bits [13:8] is
         *  applicable for both DMA engines
         */
        uint32_t usp:1;
        /** pblx8 : R/W; bitpos: [24]; default: 0;
         *  PBLx8 Mode When set high, this bit multiplies the programmed PBL value _Bits
         *  [22:17] and Bits[13:8]_ eight times Therefore, the DMA transfers the data in 8, 16,
         *  32, 64, 128, and 256 beats depending on the PBL value Note: This bit function is
         *  not backward compatible Before release 350a, this bit was 4xPBL
         */
        uint32_t pblx8:1;
        /** aal : R/W; bitpos: [25]; default: 0;
         *  AddressAligned Beats When this bit is set high and the FB bit is equal to 1, the
         *  AHB or AXI interface generates all bursts aligned to the start address LS bits If
         *  the FB bit is equal to 0, the first burst _accessing the start address of data
         *  buffer_ is not aligned, but subsequent bursts are aligned to the address This bit
         *  is valid only in the GMACAHB and GMACAXI configurations and is reserved _RO with
         *  default value 0_ in all other configurations
         */
        uint32_t aal:1;
        /** mb : R/W; bitpos: [26]; default: 0;
         *  Mixed Burst When this bit is set high and the FB bit is low, the AHB master
         *  interface starts all bursts of length more than 16 with INCR _undefined burst_,
         *  whereas it reverts to fixed burst transfers _INCRx and SINGLE_ for burst length of
         *  16 and less This bit is valid only in the GMACAHB configuration and reserved in all
         *  other configuration
         */
        uint32_t mb:1;
        /** txpr : R/W; bitpos: [27]; default: 0;
         *  Transmit Priority When set, this bit indicates that the transmit DMA has higher
         *  priority than the receive DMA during arbitration for the systemside bus In the
         *  GMACAXI configuration, this bit is reserved and readonly _RO_ For more information
         *  about the priority scheme between the transmit and receive paths, see Table 412 in
         *  “DMA Arbiter Functions” on page 167
         */
        uint32_t txpr:1;
        /** prwg : R/W; bitpos: [29:28]; default: 0;
         *  Channel Priority Weights This field sets the priority weights for Channel 0 during
         *  the roundrobin arbitration between the DMA channels for the system bus 00: The
         *  priority weight is 1 01: The priority weight is 2 10: The priority weight is 3 11:
         *  The priority weight is 4 This field is present in all DWC_gmac configurations
         *  except GMACAXI when you select the AV feature Otherwise, this field is reserved and
         *  readonly _RO_ For more information about the priority weights of DMA channels, see
         *  “DMA Arbiter Functions” on page 167
         */
        uint32_t prwg:2;
        uint32_t reserved_30:1;
        /** rib : R/W; bitpos: [31]; default: 0;
         *  Rebuild INCRx Burst When this bit is set high and the AHB master gets an EBT
         *  _Retry, Split, or Losing bus grant_, the AHB master interface rebuilds the pending
         *  beats of any burst transfer initiated with INCRx The AHB master interface rebuilds
         *  the beats with a combination of specified bursts with INCRx and SINGLE By default,
         *  the AHB master interface rebuilds pending beats of an EBT with an unspecified
         *  _INCR_ burst This bit is valid only in the GMACAHB configuration It is reserved in
         *  all other configuration
         */
        uint32_t rib:1;
    };
    uint32_t val;
} gmac_register0_busmoderegister_reg_t;

/** Type of register1_transmitpolldemandregister register
 *  Used by the host to instruct the DMA to poll the Transmit Descriptor list
 */
typedef union {
    struct {
        /** tpd : RO; bitpos: [31:0]; default: 0;
         *  Transmit Poll Demand When these bits are written with any value, the DMA reads the
         *  current descriptor to which the Register 18 _Current Host Transmit Descriptor
         *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
         *  transmission returns to the Suspend state and Bit 2 _TU_ of Register 5 _Status
         *  Register_ is asserted If the descriptor is available, the transmission resumes
         */
        uint32_t tpd:32;
    };
    uint32_t val;
} gmac_register1_transmitpolldemandregister_reg_t;

/** Type of register2_receivepolldemandregister register
 *  Used by the host to instruct the DMA to poll the Receive Descriptor list
 */
typedef union {
    struct {
        /** rpd : RO; bitpos: [31:0]; default: 0;
         *  Receive Poll Demand When these bits are written with any value, the DMA reads the
         *  current descriptor to which the Register 19 _Current Host Receive Descriptor
         *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
         *  reception returns to the Suspended state and Bit 7 _RU_ of Register 5 _Status
         *  Register_ is asserted If the descriptor is available, the Rx DMA returns to the
         *  active state
         */
        uint32_t rpd:32;
    };
    uint32_t val;
} gmac_register2_receivepolldemandregister_reg_t;

/** Type of register3_receivedescriptorlistaddressregister register
 *  Points the DMA to the start of the Receive Descriptor list
 */
typedef union {
    struct {
        /** rdesla : R/W; bitpos: [31:0]; default: 0;
         *  Start of Receive List This field contains the base address of the first descriptor
         *  in the Receive Descriptor list The LSB bits _1:0, 2:0, or 3:0_ for 32bit, 64bit, or
         *  128bit bus width are ignored and internally taken as allzero by the DMA Therefore,
         *  these LSB bits are readonly _RO_
         */
        uint32_t rdesla:32;
    };
    uint32_t val;
} gmac_register3_receivedescriptorlistaddressregister_reg_t;

/** Type of register4_transmitdescriptorlistaddressregister register
 *  Points the DMA to the start of the Transmit Descriptor list
 */
typedef union {
    struct {
        /** tdesla : R/W; bitpos: [31:0]; default: 0;
         *  Start of Transmit List This field contains the base address of the first descriptor
         *  in the Transmit Descriptor list The LSB bits _1:0, 2:0, 3:0_ for 32bit, 64bit, or
         *  128bit bus width are ignored and are internally taken as allzero by the DMA
         *  Therefore, these LSB bits are readonly _RO_
         */
        uint32_t tdesla:32;
    };
    uint32_t val;
} gmac_register4_transmitdescriptorlistaddressregister_reg_t;

/** Type of register5_statusregister register
 *  The Software driver _application_ reads this register during interrupt service
 *  routine or polling to determine the status of the DMA
 */
typedef union {
    struct {
        /** ti : R/W; bitpos: [0]; default: 0;
         *  Transmit Interrupt This bit indicates that the frame transmission is complete When
         *  transmission is complete, Bit 31 _OWN_ of TDES0 is reset, and the specific frame
         *  status information is updated in the descriptor
         */
        uint32_t ti:1;
        /** tps : R/W; bitpos: [1]; default: 0;
         *  Transmit Process Stopped This bit is set when the transmission is stopped
         */
        uint32_t tps:1;
        /** tu : R/W; bitpos: [2]; default: 0;
         *  Transmit Buffer Unavailable This bit indicates that the host owns the Next
         *  Descriptor in the Transmit List and the DMA cannot acquire it Transmission is
         *  suspended Bits[22:20] explain the Transmit Process state transitions To resume
         *  processing Transmit descriptors, the host should change the ownership of the
         *  descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command
         */
        uint32_t tu:1;
        /** tjt : R/W; bitpos: [3]; default: 0;
         *  Transmit Jabber Timeout This bit indicates that the Transmit Jabber Timer expired,
         *  which happens when the frame size exceeds 2,048 _10,240 bytes when the Jumbo frame
         *  is enabled_ When the Jabber Timeout occurs, the transmission process is aborted and
         *  placed in the Stopped state This causes the Transmit Jabber Timeout TDES0[14] flag
         *  to assert
         */
        uint32_t tjt:1;
        /** ovf : R/W; bitpos: [4]; default: 0;
         *  Receive Overflow This bit indicates that the Receive Buffer had an Overflow during
         *  frame reception If the partial frame is transferred to the application, the
         *  overflow status is set in RDES0[11]
         */
        uint32_t ovf:1;
        /** unf : R/W; bitpos: [5]; default: 0;
         *  Transmit Underflow This bit indicates that the Transmit Buffer had an Underflow
         *  during frame transmission Transmission is suspended and an Underflow Error TDES0[1]
         *  is set
         */
        uint32_t unf:1;
        /** ri : R/W; bitpos: [6]; default: 0;
         *  Receive Interrupt This bit indicates that the frame reception is complete When
         *  reception is complete, the Bit 31 of RDES1 _Disable Interrupt on Completion_ is
         *  reset in the last Descriptor, and the specific frame status information is updated
         *  in the descriptor The reception remains in the Running state
         */
        uint32_t ri:1;
        /** ru : R/W; bitpos: [7]; default: 0;
         *  Receive Buffer Unavailable This bit indicates that the host owns the Next
         *  Descriptor in the Receive List and the DMA cannot acquire it The Receive Process is
         *  suspended To resume processing Receive descriptors, the host should change the
         *  ownership of the descriptor and issue a Receive Poll Demand command If no Receive
         *  Poll Demand is issued, the Receive Process resumes when the next recognized
         *  incoming frame is received This bit is set only when the previous Receive
         *  Descriptor is owned by the DMA
         */
        uint32_t ru:1;
        /** rps : R/W; bitpos: [8]; default: 0;
         *  Receive Process Stopped This bit is asserted when the Receive Process enters the
         *  Stopped state
         */
        uint32_t rps:1;
        /** rwt : R/W; bitpos: [9]; default: 0;
         *  Receive Watchdog Timeout When set, this bit indicates that the Receive Watchdog
         *  Timer expired while receiving the current frame and the current frame is truncated
         *  after the watchdog timeout
         */
        uint32_t rwt:1;
        /** eti : R/W; bitpos: [10]; default: 0;
         *  Early Transmit Interrupt This bit indicates that the frame to be transmitted is
         *  fully transferred to the MTL Transmit FIFO
         */
        uint32_t eti:1;
        uint32_t reserved_11:2;
        /** fbi : R/W; bitpos: [13]; default: 0;
         *  Fatal Bus Error Interrupt This bit indicates that a bus error occurred, as
         *  described in Bits [25:23] When this bit is set, the corresponding DMA engine
         *  disables all of its bus accesses 12:11		Reserved	00	RO
         */
        uint32_t fbi:1;
        /** eri : R/W; bitpos: [14]; default: 0;
         *  Early Receive Interrupt This bit indicates that the DMA filled the first data
         *  buffer of the packet This bit is cleared when the software writes 1 to this bit or
         *  Bit 6 _RI_ of this register is set _whichever occurs earlier_
         */
        uint32_t eri:1;
        /** ais : R/W; bitpos: [15]; default: 0;
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
        uint32_t ais:1;
        /** nis : R/W; bitpos: [16]; default: 0;
         *  Normal Interrupt Summary Normal Interrupt Summary bit value is the logical OR of
         *  the following bits when the corresponding interrupt bits are enabled in Register 7
         *  _Interrupt Enable Register_: Register 5[0]: Transmit Interrupt Register 5[2]:
         *  Transmit Buffer Unavailable Register 5[6]: Receive Interrupt Register 5[14]: Early
         *  Receive Interrupt Only unmasked bits _interrupts for which interrupt enable is set
         *  in Register 7_ affect the Normal Interrupt Summary bit This is a sticky bit and
         *  must be cleared _by writing 1 to this bit_ each time a corresponding bit, which
         *  causes NIS to be set, is cleared
         */
        uint32_t nis:1;
        /** rs : RO; bitpos: [19:17]; default: 0;
         *  Receive Process State This field indicates the Receive DMA FSM state This field
         *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Receive Command
         *  issued 3’b001: Running: Fetching Receive Transfer Descriptor 3’b010: Reserved for
         *  future use 3’b011: Running: Waiting for receive packet 3’b100: Suspended: Receive
         *  Descriptor Unavailable 3’b101: Running: Closing Receive Descriptor 3’b110:
         *  TIME_STAMP write state 3’b111: Running: Transferring the receive packet data from
         *  receive buffer to host memory
         */
        uint32_t rs:3;
        /** ts : RO; bitpos: [22:20]; default: 0;
         *  Transmit Process State This field indicates the Transmit DMA FSM state This field
         *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Transmit Command
         *  issued 3’b001: Running: Fetching Transmit Transfer Descriptor 3’b010: Running:
         *  Waiting for status 3’b011: Running: Reading Data from host memory buffer and
         *  queuing it to transmit buffer _Tx FIFO_ 3’b100: TIME_STAMP write state 3’b101:
         *  Reserved for future use 3’b110: Suspended: Transmit Descriptor Unavailable or
         *  Transmit Buffer Underflow 3’b111: Running: Closing Transmit Descriptor
         */
        uint32_t ts:3;
        /** eb : RO; bitpos: [25:23]; default: 0;
         *  Error Bits This field indicates the type of error that caused a Bus Error, for
         *  example, error response on the AHB or AXI interface This field is valid only when
         *  Bit 13 _FBI_ is set This field does not generate an interrupt 0 0 0: Error during
         *  Rx DMA Write Data Transfer 0 1 1: Error during Tx DMA Read Data Transfer 1 0 0:
         *  Error during Rx DMA Descriptor Write Access 1 0 1: Error during Tx DMA Descriptor
         *  Write Access 1 1 0: Error during Rx DMA Descriptor Read Access 1 1 1: Error during
         *  Tx DMA Descriptor Read Access Note: 001 and 010 are reserved
         */
        uint32_t eb:3;
        /** gli : RO; bitpos: [26]; default: 0;
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
        uint32_t gli:1;
        /** gmi : RO; bitpos: [27]; default: 0;
         *  GMAC MMC Interrupt This bit reflects an interrupt event in the MMC module of the
         *  DWC_gmac The software must read the corresponding registers in the DWC_gmac to get
         *  the exact cause of the interrupt and clear the source of interrupt to make this bit
         *  as 1’b0 The interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when
         *  this bit is high This bit is applicable only when the MAC Management Counters _MMC_
         *  are enabled Otherwise, this bit is reserved
         */
        uint32_t gmi:1;
        /** gpi : RO; bitpos: [28]; default: 0;
         *  GMAC PMT Interrupt This bit indicates an interrupt event in the PMT module of the
         *  DWC_gmac The software must read the PMT Control and Status Register in the MAC to
         *  get the exact cause of interrupt and clear its source to reset this bit to 1’b0 The
         *  interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when this bit is
         *  high This bit is applicable only when the Power Management feature is enabled
         *  Otherwise, this bit is reserved Note: The GPI and pmt_intr_o interrupts are
         *  generated in different clock domains
         */
        uint32_t gpi:1;
        /** tti : RO; bitpos: [29]; default: 0;
         *  Timestamp Trigger Interrupt This bit indicates an interrupt event in the Timestamp
         *  Generator block of the DWC_gmac The software must read the corresponding registers
         *  in the DWC_gmac to get the exact cause of the interrupt and clear its source to
         *  reset this bit to 1'b0 When this bit is high, the interrupt signal from the
         *  DWC_gmac subsystem _sbd_intr_o_ is high This bit is applicable only when the IEEE
         *  1588 Timestamp feature is enabled Otherwise, this bit is reserved
         */
        uint32_t tti:1;
        /** glpii_gtmsi : RO; bitpos: [30]; default: 0;
         *  GLPII: GMAC LPI Interrupt This bit indicates an interrupt event in the LPI logic of
         *  the MAC To reset this bit to 1'b0, the software must read the corresponding
         *  registers in the DWC_gmac to get the exact cause of the interrupt and clear its
         *  source Note: GLPII status is given only in Channel 0 DMA register and is applicable
         *  only when the Energy Efficient Ethernet feature is enabled Otherwise, this bit is
         *  reserved When this bit is high, the interrupt signal from the MAC _sbd_intr_o_ is
         *  high
         */
        uint32_t glpii_gtmsi:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} gmac_register5_statusregister_reg_t;

/** Type of register6_operationmoderegister register
 *  Establishes the Receive and Transmit operating modes and command Note: This
 *  register is valid and present in the GMACMTL configuration
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** sr : R/W; bitpos: [1]; default: 0;
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
        uint32_t sr:1;
        /** osf : R/W; bitpos: [2]; default: 0;
         *  Operate on Second Frame When this bit is set, it instructs the DMA to process the
         *  second frame of the Transmit data even before the status for the first frame is
         *  obtained
         */
        uint32_t osf:1;
        /** rtc : R/W; bitpos: [4:3]; default: 0;
         *  Receive Threshold Control These two bits control the threshold level of the MTL
         *  Receive FIFO Transfer _request_ to DMA starts when the frame size within the MTL
         *  Receive FIFO is larger than the threshold In addition, full frames with length less
         *  than the threshold are automatically transferred The value of 11 is not applicable
         *  if the configured Receive FIFO size is 128 bytes These bits are valid only when the
         *  RSF bit is zero, and are ignored when the RSF bit is set to 1 00: 64 01: 32 10: 96
         *  11: 128
         */
        uint32_t rtc:2;
        /** dgf : R/W; bitpos: [5]; default: 0;
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
        uint32_t dgf:1;
        /** fuf : R/W; bitpos: [6]; default: 0;
         *  Forward Undersized Good Frames When set, the Rx FIFO forwards Undersized frames
         *  _that is, frames with no Error and length less than 64 bytes_ including padbytes
         *  and CRC When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a
         *  frame is already transferred because of the lower value of Receive Threshold, for
         *  example, RTC = 01
         */
        uint32_t fuf:1;
        /** fef : R/W; bitpos: [7]; default: 0;
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
        uint32_t fef:1;
        /** efc : R/W; bitpos: [8]; default: 0;
         *  Enable HW Flow Control When this bit is set, the flow control signal operation
         *  based on the filllevel of Rx FIFO is enabled When reset, the flow control operation
         *  is disabled This bit is not used _reserved and always reset_ when the Rx FIFO is
         *  less than 4 KB
         */
        uint32_t efc:1;
        /** rfa : R/W; bitpos: [10:9]; default: 0;
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
        uint32_t rfa:2;
        /** rfd : R/W; bitpos: [12:11]; default: 0;
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
        uint32_t rfd:2;
        /** st : R/W; bitpos: [13]; default: 0;
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
        uint32_t st:1;
        /** ttc : R/W; bitpos: [16:14]; default: 0;
         *  Transmit Threshold Control These bits control the threshold level of the MTL
         *  Transmit FIFO Transmission starts when the frame size within the MTL Transmit FIFO
         *  is larger than the threshold In addition, full frames with a length less than the
         *  threshold are also transmitted These bits are used only when Bit 21 _TSF_ is reset
         *  000: 64 001: 128 010: 192 011: 256 100: 40 101: 32 110: 24 111: 16
         */
        uint32_t ttc:3;
        uint32_t reserved_17:3;
        /** ftf : R/W1S; bitpos: [20]; default: 0;
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
        uint32_t ftf:1;
        /** tsf : R/W; bitpos: [21]; default: 0;
         *  Transmit Store and Forward When this bit is set, transmission starts when a full
         *  frame resides in the MTL Transmit FIFO When this bit is set, the TTC values
         *  specified in Bits [16:14] are ignored This bit should be changed only when the
         *  transmission is stopped
         */
        uint32_t tsf:1;
        /** rfd_2 : R/W; bitpos: [22]; default: 0;
         *  MSB of Threshold for Deactivating Flow Control If the DWC_gmac is configured for Rx
         *  FIFO size of 8 KB or more, this bit _when set_ provides additional threshold levels
         *  for deactivating the flow control in both halfduplex and fullduplex modes This bit
         *  _as Most Significant Bit_ along with the RFD _Bits [12:11]_ gives the following
         *  thresholds for deactivating flow control: 100: Full minus 5 KB, that is, FULL — 5
         *  KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that is, FULL —
         *  7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB or less deep
         */
        uint32_t rfd_2:1;
        /** rfa_2 : R/W; bitpos: [23]; default: 0;
         *  MSB of Threshold for Activating Flow Control If the DWC_gmac is configured for an
         *  Rx FIFO size of 8 KB or more, this bit _when set_ provides additional threshold
         *  levels for activating the flow control in both half duplex and fullduplex modes
         *  This bit _as Most Significant Bit_, along with the RFA _Bits [10:9]_, gives the
         *  following thresholds for activating flow control: 100: Full minus 5 KB, that is,
         *  FULL — 5 KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that
         *  is, FULL — 7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB
         *  or less deep
         */
        uint32_t rfa_2:1;
        /** dff : R/W; bitpos: [24]; default: 0;
         *  Disable Flushing of Received Frames When this bit is set, the Rx DMA does not flush
         *  any frames because of the unavailability of receive descriptors or buffers as it
         *  does normally when this bit is reset _See “Receive Process Suspended” on page 83_
         *  This bit is reserved _and RO_ in the GMACMTL configuration
         */
        uint32_t dff:1;
        /** rsf : R/W; bitpos: [25]; default: 0;
         *  Receive Store and Forward When this bit is set, the MTL reads a frame from the Rx
         *  FIFO only after the complete frame has been written to it, ignoring the RTC bits
         *  When this bit is reset, the Rx FIFO operates in the cutthrough mode, subject to the
         *  threshold specified by the RTC bits
         */
        uint32_t rsf:1;
        /** dt : R/W; bitpos: [26]; default: 0;
         *  Disable Dropping of TCP/IP Checksum Error Frames When this bit is set, the MAC does
         *  not drop the frames which only have errors detected by the Receive Checksum Offload
         *  engine Such frames do not have any errors _including FCS error_ in the Ethernet
         *  frame received by the MAC but have errors only in the encapsulated payload When
         *  this bit is reset, all error frames are dropped if the FEF bit is reset If the IPC
         *  Full Checksum Offload Engine _Type 2_ is disabled, this bit is reserved _RO with
         *  value 1'b0_
         */
        uint32_t dt:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} gmac_register6_operationmoderegister_reg_t;

/** Type of register7_interruptenableregister register
 *  Enables the interrupts reported by the Status Register
 */
typedef union {
    struct {
        /** tie : R/W; bitpos: [0]; default: 0;
         *  Transmit Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
         *  _Bit 16_, the Transmit Interrupt is enabled When this bit is reset, the Transmit
         *  Interrupt is disabled The sbd_intr_o interrupt is generated as shown in Figure 61
         *  It is asserted only when the TTI, GPI, GMI, GLI, or GLPII bit of the DMA Status
         *  register is asserted, or when the NIS or AIS Status bit is asserted and the
         *  corresponding Interrupt Enable bits _NIE or AIE_ are enabled
         */
        uint32_t tie:1;
        /** tse : R/W; bitpos: [1]; default: 0;
         *  Transmit Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
         *  _Bit 15_, the Transmission Stopped Interrupt is enabled When this bit is reset, the
         *  Transmission Stopped Interrupt is disabled
         */
        uint32_t tse:1;
        /** tue : R/W; bitpos: [2]; default: 0;
         *  Transmit Buffer Unavailable Enable When this bit is set with Normal Interrupt
         *  Summary Enable _Bit 16_, the Transmit Buffer Unavailable Interrupt is enabled When
         *  this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled
         */
        uint32_t tue:1;
        /** the : R/W; bitpos: [3]; default: 0;
         *  Transmit Jabber Timeout Enable When this bit is set with Abnormal Interrupt Summary
         *  Enable _Bit 15_, the Transmit Jabber Timeout Interrupt is enabled When this bit is
         *  reset, the Transmit Jabber Timeout Interrupt is disabled
         */
        uint32_t the:1;
        /** ove : R/W; bitpos: [4]; default: 0;
         *  Overflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
         *  Enable _Bit 15_, the Receive Overflow Interrupt is enabled When this bit is reset,
         *  the Overflow Interrupt is disabled
         */
        uint32_t ove:1;
        /** une : R/W; bitpos: [5]; default: 0;
         *  Underflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
         *  Enable _Bit 15_, the Transmit Underflow Interrupt is enabled When this bit is
         *  reset, the Underflow Interrupt is disabled
         */
        uint32_t une:1;
        /** rie : R/W; bitpos: [6]; default: 0;
         *  Receive Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
         *  _Bit 16_, the Receive Interrupt is enabled When this bit is reset, the Receive
         *  Interrupt is disabled
         */
        uint32_t rie:1;
        /** rue : R/W; bitpos: [7]; default: 0;
         *  Receive Buffer Unavailable Enable When this bit is set with Abnormal Interrupt
         *  Summary Enable _Bit 15_, the Receive Buffer Unavailable Interrupt is enabled When
         *  this bit is reset, the Receive Buffer Unavailable Interrupt is disabled
         */
        uint32_t rue:1;
        /** rse : R/W; bitpos: [8]; default: 0;
         *  Receive Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
         *  _Bit 15_, the Receive Stopped Interrupt is enabled When this bit is reset, the
         *  Receive Stopped Interrupt is disabled
         */
        uint32_t rse:1;
        /** rwe : R/W; bitpos: [9]; default: 0;
         *  Receive Watchdog Timeout Enable When this bit is set with Abnormal Interrupt
         *  Summary Enable _Bit 15_, the Receive Watchdog Timeout Interrupt is enabled When
         *  this bit is reset, the Receive Watchdog Timeout Interrupt is disabled
         */
        uint32_t rwe:1;
        /** ete : R/W; bitpos: [10]; default: 0;
         *  Early Transmit Interrupt Enable When this bit is set with an Abnormal Interrupt
         *  Summary Enable _Bit 15_, the Early Transmit Interrupt is enabled When this bit is
         *  reset, the Early Transmit Interrupt is disabled
         */
        uint32_t ete:1;
        uint32_t reserved_11:2;
        /** fbe : R/W; bitpos: [13]; default: 0;
         *  Fatal Bus Error Enable When this bit is set with Abnormal Interrupt Summary Enable
         *  _Bit 15_, the Fatal Bus Error Interrupt is enabled When this bit is reset, the
         *  Fatal Bus Error Enable Interrupt is disabled 12:11		Reserved	00	RO
         */
        uint32_t fbe:1;
        /** ere : R/W; bitpos: [14]; default: 0;
         *  Early Receive Interrupt Enable When this bit is set with Normal Interrupt Summary
         *  Enable _Bit 16_, the Early Receive Interrupt is enabled When this bit is reset, the
         *  Early Receive Interrupt is disabled
         */
        uint32_t ere:1;
        /** aie : R/W; bitpos: [15]; default: 0;
         *  Abnormal Interrupt Summary Enable When this bit is set, abnormal interrupt summary
         *  is enabled When this bit is reset, the abnormal interrupt summary is disabled This
         *  bit enables the following interrupts in Register 5 _Status Register_: Register
         *  5[1]: Transmit Process Stopped Register 5[3]: Transmit Jabber Timeout Register
         *  5[4]: Receive Overflow Register 5[5]: Transmit Underflow Register 5[7]: Receive
         *  Buffer Unavailable Register 5[8]: Receive Process Stopped Register 5[9]: Receive
         *  Watchdog Timeout Register 5[10]: Early Transmit Interrupt Register 5[13]: Fatal Bus
         *  Error
         */
        uint32_t aie:1;
        /** nie : R/W; bitpos: [16]; default: 0;
         *  Normal Interrupt Summary Enable When this bit is set, normal interrupt summary is
         *  enabled When this bit is reset, normal interrupt summary is disabled This bit
         *  enables the following interrupts in Register 5 _Status Register_: Register 5[0]:
         *  Transmit Interrupt Register 5[2]: Transmit Buffer Unavailable Register 5[6]:
         *  Receive Interrupt Register 5[14]: Early Receive Interrupt
         */
        uint32_t nie:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} gmac_register7_interruptenableregister_reg_t;

/** Type of register8_missedframeandbufferoverflowcounterregister register
 *  Contains the counters for discarded frames because no host Receive Descriptor was
 *  available or because of Receive FIFO Overflow
 */
typedef union {
    struct {
        /** misfrmcnt : R/W; bitpos: [15:0]; default: 0;
         *  Missed Frame Counter This field indicates the number of frames missed by the
         *  controller because of the Host Receive Buffer being unavailable This counter is
         *  incremented each time the DMA discards an incoming frame The counter is cleared
         *  when this register is read with mci_be_i[0] at 1’b1
         */
        uint32_t misfrmcnt:16;
        /** miscntovf : R/W; bitpos: [16]; default: 0;
         *  Overflow Bit for Missed Frame Counter This bit is set every time Missed Frame
         *  Counter _Bits[15:0]_ overflows, that is, the DMA discards an incoming frame because
         *  of the Host Receive Buffer being unavailable with the missed frame counter at
         *  maximum value In such a scenario, the Missed frame counter is reset to allzeros and
         *  this bit indicates that the rollover happened
         */
        uint32_t miscntovf:1;
        /** ovffrmcnt : R/W; bitpos: [27:17]; default: 0;
         *  Overflow Frame Counter This field indicates the number of frames missed by the
         *  application This counter is incremented each time the MTL FIFO overflows The
         *  counter is cleared when this register is read with mci_be_i[2] at 1’b1
         */
        uint32_t ovffrmcnt:11;
        /** ovfcntovf : R/W; bitpos: [28]; default: 0;
         *  Overflow Bit for FIFO Overflow Counter This bit is set every time the Overflow
         *  Frame Counter _Bits[27:17]_ overflows, that is, the Rx FIFO overflows with the
         *  overflow frame counter at maximum value In such a scenario, the overflow frame
         *  counter is reset to allzeros and this bit indicates that the rollover happened
         */
        uint32_t ovfcntovf:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} gmac_register8_missedframeandbufferoverflowcounterregister_reg_t;

/** Type of register9_receiveinterruptwatchdogtimerregister register
 *  Watchdog timeout for Receive Interrupt _RI_ from DMA
 */
typedef union {
    struct {
        /** riwt : R/W; bitpos: [7:0]; default: 0;
         *  RI Watchdog Timer Count This bit indicates the number of system clock cycles
         *  multiplied by 256 for which the watchdog timer is set The watchdog timer gets
         *  triggered with the programmed value after the Rx DMA completes the transfer of a
         *  frame for which the RI status bit is not set because of the setting in the
         *  corresponding descriptor RDES1[31] When the watchdog timer runs out, the RI bit is
         *  set and the timer is stopped The watchdog timer is reset when the RI bit is set
         *  high because of automatic setting of RI as per RDES1[31] of any received frame
         */
        uint32_t riwt:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gmac_register9_receiveinterruptwatchdogtimerregister_reg_t;

/** Type of register10_axibusmoderegister register
 *  Controls AXI master behavior _mainly controls burst splitting and number of
 *  outstanding requests_
 */
typedef union {
    struct {
        /** undef : RO; bitpos: [0]; default: 1;
         *  AXI Undefined Burst Length This bit is readonly bit and indicates the complement
         *  _invert_ value of Bit 16 _FB_ in Register 0 _Bus Mode Register_ When this bit is
         *  set to 1, the GMACAXI is allowed to perform any burst length equal to or below the
         *  maximum allowed burst length programmed in Bits[7:3] When this bit is set to 0, the
         *  GMACAXI is allowed to perform only fixed burst lengths as indicated by BLEN256,
         *  BLEN128, BLEN64, BLEN32, BLEN16, BLEN8, or BLEN4, or a burst length of 1 If UNDEF
         *  is set and none of the BLEN bits is set, then GMACAXI is allowed to perform a burst
         *  length of 16
         */
        uint32_t undef:1;
        /** blen4 : R/W; bitpos: [1]; default: 0;
         *  AXI Burst Length 4 When this bit is set to 1, the GMACAXI is allowed to select a
         *  burst length of 4 on the AXI master interface Setting this bit has no effect when
         *  UNDEF is set to 1
         */
        uint32_t blen4:1;
        /** blen8 : R/W; bitpos: [2]; default: 0;
         *  AXI Burst Length 8 When this bit is set to 1, the GMACAXI is allowed to select a
         *  burst length of 8 on the AXI master interface Setting this bit has no effect when
         *  UNDEF is set to 1
         */
        uint32_t blen8:1;
        /** blen16 : R/W; bitpos: [3]; default: 0;
         *  AXI Burst Length 16 When this bit is set to 1 or UNDEF is set to 1, the GMACAXI is
         *  allowed to select a burst length of 16 on the AXI master interface
         */
        uint32_t blen16:1;
        /** blen32 : R/W; bitpos: [4]; default: 0;
         *  AXI Burst Length 32 When this bit is set to 1, the GMACAXI is allowed to select a
         *  burst length of 32 on the AXI master interface This bit is present only when the
         *  configuration parameter AXI_BL is set to 32 or more Otherwise, this bit is reserved
         *  and is readonly _RO_
         */
        uint32_t blen32:1;
        /** blen64 : R/W; bitpos: [5]; default: 0;
         *  AXI Burst Length 64 When this bit is set to 1, the GMACAXI is allowed to select a
         *  burst length of 64 on the AXI master interface This bit is present only when the
         *  configuration parameter AXI_BL is set to 64 or more Otherwise, this bit is reserved
         *  and is readonly _RO_
         */
        uint32_t blen64:1;
        /** blen128 : R/W; bitpos: [6]; default: 0;
         *  AXI Burst Length 128 When this bit is set to 1, the GMACAXI is allowed to select a
         *  burst length of 128 on the AXI master interface This bit is present only when the
         *  configuration parameter AXI_BL is set to 128 or more Otherwise, this bit is
         *  reserved and is readonly _RO_
         */
        uint32_t blen128:1;
        /** blen256 : R/W; bitpos: [7]; default: 0;
         *  AXI Burst Length 256 When this bit is set to 1, the GMACAXI is allowed to select a
         *  burst length of 256 on the AXI master interface This bit is present only when the
         *  configuration parameter AXI_BL is set to 256 Otherwise, this bit is reserved and is
         *  readonly _RO_
         */
        uint32_t blen256:1;
        uint32_t reserved_8:4;
        /** axi_aal : RO; bitpos: [12]; default: 0;
         *  AddressAligned Beats This bit is readonly bit and reflects the Bit 25 _AAL_ of
         *  Register 0 _Bus Mode Register_ When this bit is set to 1, the GMACAXI performs
         *  addressaligned burst transfers on both read and write channels 11:8		Reserved	0H	RO
         */
        uint32_t axi_aal:1;
        /** onekbbe : R/W; bitpos: [13]; default: 0;
         *  1 KB Boundary Crossing Enable for the GMACAXI Master When set, the GMACAXI master
         *  performs burst transfers that do not cross 1 KB boundary When reset, the GMACAXI
         *  master performs burst transfers that do not cross 4 KB boundary
         */
        uint32_t onekbbe:1;
        uint32_t reserved_14:2;
        /** rd_osr_lmt : R/W; bitpos: [19:16]; default: 1;
         *  AXI Maximum Read Outstanding Request Limit This value limits the maximum
         *  outstanding request on the AXI read interface Maximum outstanding requests =
         *  RD_OSR_LMT+1 Note: Bit 18 is reserved if AXI_GM_MAX_RD_REQUESTS = 4 Bit 19 is
         *  reserved if AXI_GM_MAX_RD_REQUESTS != 16 15:14		Reserved	00	RO
         */
        uint32_t rd_osr_lmt:4;
        /** wr_osr_lmt : R/W; bitpos: [23:20]; default: 1;
         *  AXI Maximum Write Outstanding Request Limit This value limits the maximum
         *  outstanding request on the AXI write interface Maximum outstanding requests =
         *  WR_OSR_LMT+1 Note: Bit 22 is reserved if AXI_GM_MAX_WR_REQUESTS = 4 Bit 23 bit is
         *  reserved if AXI_GM_MAX_WR_REQUESTS != 16
         */
        uint32_t wr_osr_lmt:4;
        uint32_t reserved_24:6;
        /** lpi_xit_frm : R/W; bitpos: [30]; default: 0;
         *  Unlock on Magic Packet or Remote WakeUp Frame When set to 1, this bit enables the
         *  GMACAXI to come out of the LPI mode only when the magic packet or remote wakeup
         *  frame is received When set to 0, this bit enables the GMACAXI to come out of LPI
         *  mode when any frame is received 29:24		Reserved	00H	RO
         */
        uint32_t lpi_xit_frm:1;
        /** en_lpi : R/W; bitpos: [31]; default: 0;
         *  Enable Low Power Interface _LPI_ When set to 1, this bit enables the LPI mode
         *  supported by the GMACAXI configuration and accepts the LPI request from the AXI
         *  System Clock controller When set to 0, this bit disables the LPI mode and always
         *  denies the LPI request from the AXI System Clock controller
         */
        uint32_t en_lpi:1;
    };
    uint32_t val;
} gmac_register10_axibusmoderegister_reg_t;

/** Type of register11_ahboraxistatusregister register
 *  Gives the idle status of the AHB master interface in the GMACAHB configuration
 *  Gives the idle status of the AXI master's read or write channel in the GMACAXI
 *  configuration
 */
typedef union {
    struct {
        /** axwhsts : RO; bitpos: [0]; default: 0;
         *  AXI Master Write Channel or AHB Master Status When high, it indicates that AXI
         *  master's write channel is active and transferring data in the GMACAXI configuration
         *  In the GMACAHB configuration, it indicates that the AHB master interface FSMs are
         *  in the nonidle state
         */
        uint32_t axwhsts:1;
        /** axirdsts : RO; bitpos: [1]; default: 0;
         *  AXI Master Read Channel Status When high, it indicates that AXI master's read
         *  channel is active and transferring data
         */
        uint32_t axirdsts:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gmac_register11_ahboraxistatusregister_reg_t;

/** Type of register18_currenthosttransmitdescriptorregister register
 *  Points to the start of current Transmit Descriptor read by the DMA
 */
typedef union {
    struct {
        /** curtdesaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Transmit Descriptor Address Pointer
         */
        uint32_t curtdesaptr:32;
    };
    uint32_t val;
} gmac_register18_currenthosttransmitdescriptorregister_reg_t;

/** Type of register19_currenthostreceivedescriptorregister register
 *  Points to the start of current Receive Descriptor read by the DMA
 */
typedef union {
    struct {
        /** currdesaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Receive Descriptor Address Pointer
         */
        uint32_t currdesaptr:32;
    };
    uint32_t val;
} gmac_register19_currenthostreceivedescriptorregister_reg_t;

/** Type of register20_currenthosttransmitbufferaddressregister register
 *  Points to the current Transmit Buffer address read by the DMA
 */
typedef union {
    struct {
        /** curtbufaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Transmit Buffer Address Pointer
         */
        uint32_t curtbufaptr:32;
    };
    uint32_t val;
} gmac_register20_currenthosttransmitbufferaddressregister_reg_t;

/** Type of register21_currenthostreceivebufferaddressregister register
 *  Points to the current Receive Buffer address read by the DMA
 */
typedef union {
    struct {
        /** currbufaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Receive Buffer Address Pointer
         */
        uint32_t currbufaptr:32;
    };
    uint32_t val;
} gmac_register21_currenthostreceivebufferaddressregister_reg_t;

/** Type of register22_hwfeatureregister register
 *  Indicates the presence of the optional features of the core
 */
typedef union {
    struct {
        /** miisel : RO; bitpos: [0]; default: 0;
         *  10 or 100 Mbps support
         */
        uint32_t miisel:1;
        /** gmiisel : RO; bitpos: [1]; default: 0;
         *  1000 Mbps support
         */
        uint32_t gmiisel:1;
        /** hdsel : RO; bitpos: [2]; default: 0;
         *  Halfduplex support
         */
        uint32_t hdsel:1;
        /** exthashen : RO; bitpos: [3]; default: 0;
         *  Expanded DA Hash filter
         */
        uint32_t exthashen:1;
        /** hashsel : RO; bitpos: [4]; default: 0;
         *  HASH filter
         */
        uint32_t hashsel:1;
        /** addmacadrsel : RO; bitpos: [5]; default: 0;
         *  Multiple MAC Address registers
         */
        uint32_t addmacadrsel:1;
        /** pcssel : RO; bitpos: [6]; default: 0;
         *  PCS registers _TBI, SGMII, or RTBI PHY interface_
         */
        uint32_t pcssel:1;
        /** l3l4fltren : RO; bitpos: [7]; default: 0;
         *  Layer 3 and Layer 4 feature
         */
        uint32_t l3l4fltren:1;
        /** smasel : RO; bitpos: [8]; default: 0;
         *  SMA _MDIO_ Interface
         */
        uint32_t smasel:1;
        /** rwksel : RO; bitpos: [9]; default: 0;
         *  PMT remote wakeup frame
         */
        uint32_t rwksel:1;
        /** mgksel : RO; bitpos: [10]; default: 0;
         *  PMT magic packet
         */
        uint32_t mgksel:1;
        /** mmcsel : RO; bitpos: [11]; default: 0;
         *  RMON module
         */
        uint32_t mmcsel:1;
        /** tsver1sel : RO; bitpos: [12]; default: 0;
         *  Only IEEE 15882002 timestamp
         */
        uint32_t tsver1sel:1;
        /** tsver2sel : RO; bitpos: [13]; default: 0;
         *  IEEE 15882008 Advanced timestamp
         */
        uint32_t tsver2sel:1;
        /** eeesel : RO; bitpos: [14]; default: 0;
         *  Energy Efficient Ethernet
         */
        uint32_t eeesel:1;
        /** avsel : RO; bitpos: [15]; default: 0;
         *  AV feature
         */
        uint32_t avsel:1;
        /** txcoesel : RO; bitpos: [16]; default: 0;
         *  Checksum Offload in Tx
         */
        uint32_t txcoesel:1;
        /** rxtyp1coe : RO; bitpos: [17]; default: 0;
         *  IP Checksum Offload _Type 1_ in Rx Note: If IPCHKSUM_EN = Enabled and
         *  IPC_FULL_OFFLOAD = Enabled, then RXTYP1COE = 0 and RXTYP2COE = 1
         */
        uint32_t rxtyp1coe:1;
        /** rxtyp2coe : RO; bitpos: [18]; default: 0;
         *  IP Checksum Offload _Type 2_ in Rx
         */
        uint32_t rxtyp2coe:1;
        /** rxfifosize : RO; bitpos: [19]; default: 0;
         *  Rx FIFO > 2,048 Bytes
         */
        uint32_t rxfifosize:1;
        /** rxchcnt : RO; bitpos: [21:20]; default: 0;
         *  Number of additional Rx Channels
         */
        uint32_t rxchcnt:2;
        /** txchcnt : RO; bitpos: [23:22]; default: 0;
         *  Number of additional Tx Channels
         */
        uint32_t txchcnt:2;
        /** enhdessel : RO; bitpos: [24]; default: 0;
         *  Alternate _Enhanced Descriptor_
         */
        uint32_t enhdessel:1;
        /** inttsen : RO; bitpos: [25]; default: 0;
         *  Timestamping with Internal System Time
         */
        uint32_t inttsen:1;
        /** flexippsen : RO; bitpos: [26]; default: 0;
         *  Flexible PulsePerSecond Output
         */
        uint32_t flexippsen:1;
        /** savlanins : RO; bitpos: [27]; default: 0;
         *  Source Address or VLAN Insertion
         */
        uint32_t savlanins:1;
        /** actphyif : RO; bitpos: [30:28]; default: 0;
         *  Active or selected PHY interface When you have multiple PHY interfaces in your
         *  configuration, this field indicates the sampled value of phy_intf_sel_i during
         *  reset deassertion 000: GMII or MII 001: RGMII 010: SGMII 011: TBI 100: RMII 101:
         *  RTBI 110: SMII 111: RevMII All Others: Reserved
         */
        uint32_t actphyif:3;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} gmac_register22_hwfeatureregister_reg_t;

/** Type of register64_channel1busmoderegister register
 *  Controls the Host Interface mode for Channel 1
 */
typedef union {
    struct {
        /** ch1_swr : R/W1S; bitpos: [0]; default: 1;
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
        uint32_t ch1_swr:1;
        /** ch1_da : R/W; bitpos: [1]; default: 0;
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
        uint32_t ch1_da:1;
        /** ch1_dsl : R/W; bitpos: [6:2]; default: 0;
         *  Descriptor Skip Length This bit specifies the number of Word, Dword, or Lword
         *  _depending on the 32bit, 64bit, or 128bit bus_ to skip between two unchained
         *  descriptors The address skipping starts from the end of current descriptor to the
         *  start of next descriptor When the DSL value is equal to zero, the descriptor table
         *  is taken as contiguous by the DMA in Ring mode
         */
        uint32_t ch1_dsl:5;
        /** ch1_atds : R/W; bitpos: [7]; default: 0;
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
        uint32_t ch1_atds:1;
        /** ch1_pbl : R/W; bitpos: [13:8]; default: 1;
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
        uint32_t ch1_pbl:6;
        /** ch1_pr : R/W; bitpos: [15:14]; default: 0;
         *  Priority Ratio These bits control the priority ratio in the weighted roundrobin
         *  arbitration between the Rx DMA and Tx DMA These bits are valid only when Bit 1 _DA_
         *  is reset The priority ratio is Rx:Tx or Tx:Rx depending on whether Bit 27 _TXPR_ is
         *  reset or set 00: The Priority Ratio is 1:1 01: The Priority Ratio is 2:1 10: The
         *  Priority Ratio is 3:1 11: The Priority Ratio is 4:1 In the GMACAXI configuration,
         *  these bits are reserved and readonly _RO_ For more information about the priority
         *  scheme between the transmit and receive paths, see Table 412 in “DMA Arbiter
         *  Functions” on page 167
         */
        uint32_t ch1_pr:2;
        /** ch1_fb : R/W; bitpos: [16]; default: 0;
         *  Fixed Burst This bit controls whether the AHB or AXI master interface performs
         *  fixed burst transfers or not When set, the AHB interface uses only SINGLE, INCR4,
         *  INCR8, or INCR16 during start of the normal burst transfers When reset, the AHB or
         *  AXI interface uses SINGLE and INCR burst transfer operations For more information,
         *  see Bit 0 _UNDEF_ of the AXI Bus Mode register in the GMACAXI configuration
         */
        uint32_t ch1_fb:1;
        /** ch1_rpbl : R/W; bitpos: [22:17]; default: 1;
         *  Rx DMA PBL This field indicates the maximum number of beats to be transferred in
         *  one Rx DMA transaction This is the maximum value that is used in a single block
         *  Read or Write The Rx DMA always attempts to burst as specified in the RPBL bit each
         *  time it starts a Burst transfer on the host bus You can program RPBL with values of
         *  1, 2, 4, 8, 16, and 32 Any other value results in undefined behavior This field is
         *  valid and applicable only when USP is set high
         */
        uint32_t ch1_rpbl:6;
        /** ch1_usp : R/W; bitpos: [23]; default: 0;
         *  Use Separate PBL When set high, this bit configures the Rx DMA to use the value
         *  configured in Bits [22:17] as PBL The PBL value in Bits [13:8] is applicable only
         *  to the Tx DMA operations When reset to low, the PBL value in Bits [13:8] is
         *  applicable for both DMA engines
         */
        uint32_t ch1_usp:1;
        /** ch1_pblx8 : R/W; bitpos: [24]; default: 0;
         *  PBLx8 Mode When set high, this bit multiplies the programmed PBL value _Bits
         *  [22:17] and Bits[13:8]_ eight times Therefore, the DMA transfers the data in 8, 16,
         *  32, 64, 128, and 256 beats depending on the PBL value Note: This bit function is
         *  not backward compatible Before release 350a, this bit was 4xPBL
         */
        uint32_t ch1_pblx8:1;
        /** ch1_aal : R/W; bitpos: [25]; default: 0;
         *  AddressAligned Beats When this bit is set high and the FB bit is equal to 1, the
         *  AHB or AXI interface generates all bursts aligned to the start address LS bits If
         *  the FB bit is equal to 0, the first burst _accessing the start address of data
         *  buffer_ is not aligned, but subsequent bursts are aligned to the address This bit
         *  is valid only in the GMACAHB and GMACAXI configurations and is reserved _RO with
         *  default value 0_ in all other configurations
         */
        uint32_t ch1_aal:1;
        /** ch1_mb : R/W; bitpos: [26]; default: 0;
         *  Mixed Burst When this bit is set high and the FB bit is low, the AHB master
         *  interface starts all bursts of length more than 16 with INCR _undefined burst_,
         *  whereas it reverts to fixed burst transfers _INCRx and SINGLE_ for burst length of
         *  16 and less This bit is valid only in the GMACAHB configuration and reserved in all
         *  other configuration
         */
        uint32_t ch1_mb:1;
        /** ch1_txpr : R/W; bitpos: [27]; default: 0;
         *  Transmit Priority When set, this bit indicates that the transmit DMA has higher
         *  priority than the receive DMA during arbitration for the systemside bus In the
         *  GMACAXI configuration, this bit is reserved and readonly _RO_ For more information
         *  about the priority scheme between the transmit and receive paths, see Table 412 in
         *  “DMA Arbiter Functions” on page 167
         */
        uint32_t ch1_txpr:1;
        /** ch1_prwg : R/W; bitpos: [29:28]; default: 0;
         *  Channel Priority Weights This field sets the priority weights for Channel 1 during
         *  the roundrobin arbitration between the DMA channels for the system bus 00: The
         *  priority weight is 1 01: The priority weight is 2 10: The priority weight is 3 11:
         *  The priority weight is 4 This field is present in all DWC_gmac configurations
         *  except GMACAXI when you select the AV feature Otherwise, this field is reserved and
         *  readonly _RO_ For more information about the priority weights of DMA channels, see
         *  “DMA Arbiter Functions” on page 167
         */
        uint32_t ch1_prwg:2;
        uint32_t reserved_30:1;
        /** ch1_rib : R/W; bitpos: [31]; default: 0;
         *  Rebuild INCRx Burst When this bit is set high and the AHB master gets an EBT
         *  _Retry, Split, or Losing bus grant_, the AHB master interface rebuilds the pending
         *  beats of any burst transfer initiated with INCRx The AHB master interface rebuilds
         *  the beats with a combination of specified bursts with INCRx and SINGLE By default,
         *  the AHB master interface rebuilds pending beats of an EBT with an unspecified
         *  _INCR_ burst This bit is valid only in the GMACAHB configuration It is reserved in
         *  all other configuration
         */
        uint32_t ch1_rib:1;
    };
    uint32_t val;
} gmac_register64_channel1busmoderegister_reg_t;

/** Type of register65_channel1transmitpolldemandregister register
 *  Used by the host to instruct the DMA to poll the Transmit Descriptor list
 */
typedef union {
    struct {
        /** ch1_tpd : RO; bitpos: [31:0]; default: 0;
         *  Transmit Poll Demand When these bits are written with any value, the DMA reads the
         *  current descriptor to which the Register 18 _Current Host Transmit Descriptor
         *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
         *  transmission returns to the Suspend state and Bit 2 _TU_ of Register 5 _Status
         *  Register_ is asserted If the descriptor is available, the transmission resumes
         */
        uint32_t ch1_tpd:32;
    };
    uint32_t val;
} gmac_register65_channel1transmitpolldemandregister_reg_t;

/** Type of register66_channel1receivepolldemandregister register
 *  Used by the Host to instruct the DMA to poll the Receive Descriptor list
 */
typedef union {
    struct {
        /** ch1_rpd : RO; bitpos: [31:0]; default: 0;
         *  Receive Poll Demand When these bits are written with any value, the DMA reads the
         *  current descriptor to which the Register 19 _Current Host Receive Descriptor
         *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
         *  reception returns to the Suspended state and Bit 7 _RU_ of Register 5 _Status
         *  Register_ is asserted If the descriptor is available, the Rx DMA returns to the
         *  active state
         */
        uint32_t ch1_rpd:32;
    };
    uint32_t val;
} gmac_register66_channel1receivepolldemandregister_reg_t;

/** Type of register67_channel1receivedescriptorlistaddressregister register
 *  Points the DMA to the start of the Receive Descriptor list
 */
typedef union {
    struct {
        /** ch1_rdesla : R/W; bitpos: [31:0]; default: 0;
         *  Start of Receive List This field contains the base address of the first descriptor
         *  in the Receive Descriptor list The LSB bits _1:0, 2:0, or 3:0_ for 32bit, 64bit, or
         *  128bit bus width are ignored and internally taken as allzero by the DMA Therefore,
         *  these LSB bits are readonly _RO_
         */
        uint32_t ch1_rdesla:32;
    };
    uint32_t val;
} gmac_register67_channel1receivedescriptorlistaddressregister_reg_t;

/** Type of register68_channel1transmitdescriptorlistaddressregister register
 *  Points the DMA to the start of the Transmit Descriptor list
 */
typedef union {
    struct {
        /** ch1_tdesla : R/W; bitpos: [31:0]; default: 0;
         *  Start of Transmit List This field contains the base address of the first descriptor
         *  in the Transmit Descriptor list The LSB bits _1:0, 2:0, 3:0_ for 32bit, 64bit, or
         *  128bit bus width are ignored and are internally taken as allzero by the DMA
         *  Therefore, these LSB bits are readonly _RO_
         */
        uint32_t ch1_tdesla:32;
    };
    uint32_t val;
} gmac_register68_channel1transmitdescriptorlistaddressregister_reg_t;

/** Type of register69_channel1statusregister register
 *  The Software driver _application_ reads this register during interrupt service
 *  routine or polling to determine the status of the DMA Bits 29:26 are reserved for
 *  the Channel 1 Status Register
 */
typedef union {
    struct {
        /** ch1_ti : R/W; bitpos: [0]; default: 0;
         *  Transmit Interrupt This bit indicates that the frame transmission is complete When
         *  transmission is complete, Bit 31 _OWN_ of TDES0 is reset, and the specific frame
         *  status information is updated in the descriptor
         */
        uint32_t ch1_ti:1;
        /** ch1_tps : R/W; bitpos: [1]; default: 0;
         *  Transmit Process Stopped This bit is set when the transmission is stopped
         */
        uint32_t ch1_tps:1;
        /** ch1_tu : R/W; bitpos: [2]; default: 0;
         *  Transmit Buffer Unavailable This bit indicates that the host owns the Next
         *  Descriptor in the Transmit List and the DMA cannot acquire it Transmission is
         *  suspended Bits[22:20] explain the Transmit Process state transitions To resume
         *  processing Transmit descriptors, the host should change the ownership of the
         *  descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command
         */
        uint32_t ch1_tu:1;
        /** ch1_tjt : R/W; bitpos: [3]; default: 0;
         *  Transmit Jabber Timeout This bit indicates that the Transmit Jabber Timer expired,
         *  which happens when the frame size exceeds 2,048 _10,240 bytes when the Jumbo frame
         *  is enabled_ When the Jabber Timeout occurs, the transmission process is aborted and
         *  placed in the Stopped state This causes the Transmit Jabber Timeout TDES0[14] flag
         *  to assert
         */
        uint32_t ch1_tjt:1;
        /** ch1_ovf : R/W; bitpos: [4]; default: 0;
         *  Receive Overflow This bit indicates that the Receive Buffer had an Overflow during
         *  frame reception If the partial frame is transferred to the application, the
         *  overflow status is set in RDES0[11]
         */
        uint32_t ch1_ovf:1;
        /** ch1_unf : R/W; bitpos: [5]; default: 0;
         *  Transmit Underflow This bit indicates that the Transmit Buffer had an Underflow
         *  during frame transmission Transmission is suspended and an Underflow Error TDES0[1]
         *  is set
         */
        uint32_t ch1_unf:1;
        /** ch1_ri : R/W; bitpos: [6]; default: 0;
         *  Receive Interrupt This bit indicates that the frame reception is complete When
         *  reception is complete, the Bit 31 of RDES1 _Disable Interrupt on Completion_ is
         *  reset in the last Descriptor, and the specific frame status information is updated
         *  in the descriptor The reception remains in the Running state
         */
        uint32_t ch1_ri:1;
        /** ch1_ru : R/W; bitpos: [7]; default: 0;
         *  Receive Buffer Unavailable This bit indicates that the host owns the Next
         *  Descriptor in the Receive List and the DMA cannot acquire it The Receive Process is
         *  suspended To resume processing Receive descriptors, the host should change the
         *  ownership of the descriptor and issue a Receive Poll Demand command If no Receive
         *  Poll Demand is issued, the Receive Process resumes when the next recognized
         *  incoming frame is received This bit is set only when the previous Receive
         *  Descriptor is owned by the DMA
         */
        uint32_t ch1_ru:1;
        /** ch1_rps : R/W; bitpos: [8]; default: 0;
         *  Receive Process Stopped This bit is asserted when the Receive Process enters the
         *  Stopped state
         */
        uint32_t ch1_rps:1;
        /** ch1_rwt : R/W; bitpos: [9]; default: 0;
         *  Receive Watchdog Timeout When set, this bit indicates that the Receive Watchdog
         *  Timer expired while receiving the current frame and the current frame is truncated
         *  after the watchdog timeout
         */
        uint32_t ch1_rwt:1;
        /** ch1_eti : R/W; bitpos: [10]; default: 0;
         *  Early Transmit Interrupt This bit indicates that the frame to be transmitted is
         *  fully transferred to the MTL Transmit FIFO
         */
        uint32_t ch1_eti:1;
        uint32_t reserved_11:2;
        /** ch1_fbi : R/W; bitpos: [13]; default: 0;
         *  Fatal Bus Error Interrupt This bit indicates that a bus error occurred, as
         *  described in Bits [25:23] When this bit is set, the corresponding DMA engine
         *  disables all of its bus accesses 12:11		Reserved	00	RO
         */
        uint32_t ch1_fbi:1;
        /** ch1_eri : R/W; bitpos: [14]; default: 0;
         *  Early Receive Interrupt This bit indicates that the DMA filled the first data
         *  buffer of the packet This bit is cleared when the software writes 1 to this bit or
         *  Bit 6 _RI_ of this register is set _whichever occurs earlier_
         */
        uint32_t ch1_eri:1;
        /** ch1_ais : R/W; bitpos: [15]; default: 0;
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
        uint32_t ch1_ais:1;
        /** ch1_nis : R/W; bitpos: [16]; default: 0;
         *  Normal Interrupt Summary Normal Interrupt Summary bit value is the logical OR of
         *  the following bits when the corresponding interrupt bits are enabled in Register 7
         *  _Interrupt Enable Register_: Register 5[0]: Transmit Interrupt Register 5[2]:
         *  Transmit Buffer Unavailable Register 5[6]: Receive Interrupt Register 5[14]: Early
         *  Receive Interrupt Only unmasked bits _interrupts for which interrupt enable is set
         *  in Register 7_ affect the Normal Interrupt Summary bit This is a sticky bit and
         *  must be cleared _by writing 1 to this bit_ each time a corresponding bit, which
         *  causes NIS to be set, is cleared
         */
        uint32_t ch1_nis:1;
        /** ch1_rs : RO; bitpos: [19:17]; default: 0;
         *  Receive Process State This field indicates the Receive DMA FSM state This field
         *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Receive Command
         *  issued 3’b001: Running: Fetching Receive Transfer Descriptor 3’b010: Reserved for
         *  future use 3’b011: Running: Waiting for receive packet 3’b100: Suspended: Receive
         *  Descriptor Unavailable 3’b101: Running: Closing Receive Descriptor 3’b110:
         *  TIME_STAMP write state 3’b111: Running: Transferring the receive packet data from
         *  receive buffer to host memory
         */
        uint32_t ch1_rs:3;
        /** ch1_ts : RO; bitpos: [22:20]; default: 0;
         *  Transmit Process State This field indicates the Transmit DMA FSM state This field
         *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Transmit Command
         *  issued 3’b001: Running: Fetching Transmit Transfer Descriptor 3’b010: Running:
         *  Waiting for status 3’b011: Running: Reading Data from host memory buffer and
         *  queuing it to transmit buffer _Tx FIFO_ 3’b100: TIME_STAMP write state 3’b101:
         *  Reserved for future use 3’b110: Suspended: Transmit Descriptor Unavailable or
         *  Transmit Buffer Underflow 3’b111: Running: Closing Transmit Descriptor
         */
        uint32_t ch1_ts:3;
        /** ch1_eb : RO; bitpos: [25:23]; default: 0;
         *  Error Bits This field indicates the type of error that caused a Bus Error, for
         *  example, error response on the AHB or AXI interface This field is valid only when
         *  Bit 13 _FBI_ is set This field does not generate an interrupt 0 0 0: Error during
         *  Rx DMA Write Data Transfer 0 1 1: Error during Tx DMA Read Data Transfer 1 0 0:
         *  Error during Rx DMA Descriptor Write Access 1 0 1: Error during Tx DMA Descriptor
         *  Write Access 1 1 0: Error during Rx DMA Descriptor Read Access 1 1 1: Error during
         *  Tx DMA Descriptor Read Access Note: 001 and 010 are reserved
         */
        uint32_t ch1_eb:3;
        /** ch1_gli : RO; bitpos: [26]; default: 0;
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
        uint32_t ch1_gli:1;
        /** ch1_gmi : RO; bitpos: [27]; default: 0;
         *  GMAC MMC Interrupt This bit reflects an interrupt event in the MMC module of the
         *  DWC_gmac The software must read the corresponding registers in the DWC_gmac to get
         *  the exact cause of the interrupt and clear the source of interrupt to make this bit
         *  as 1’b0 The interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when
         *  this bit is high This bit is applicable only when the MAC Management Counters _MMC_
         *  are enabled Otherwise, this bit is reserved
         */
        uint32_t ch1_gmi:1;
        /** ch1_gpi : RO; bitpos: [28]; default: 0;
         *  GMAC PMT Interrupt This bit indicates an interrupt event in the PMT module of the
         *  DWC_gmac The software must read the PMT Control and Status Register in the MAC to
         *  get the exact cause of interrupt and clear its source to reset this bit to 1’b0 The
         *  interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when this bit is
         *  high This bit is applicable only when the Power Management feature is enabled
         *  Otherwise, this bit is reserved Note: The GPI and pmt_intr_o interrupts are
         *  generated in different clock domains
         */
        uint32_t ch1_gpi:1;
        /** ch1_tti : RO; bitpos: [29]; default: 0;
         *  Timestamp Trigger Interrupt This bit indicates an interrupt event in the Timestamp
         *  Generator block of the DWC_gmac The software must read the corresponding registers
         *  in the DWC_gmac to get the exact cause of the interrupt and clear its source to
         *  reset this bit to 1'b0 When this bit is high, the interrupt signal from the
         *  DWC_gmac subsystem _sbd_intr_o_ is high This bit is applicable only when the IEEE
         *  1588 Timestamp feature is enabled Otherwise, this bit is reserved
         */
        uint32_t ch1_tti:1;
        /** ch1_glpii_gtmsi : RO; bitpos: [30]; default: 0;
         *  GTMSI: GMAC TMS Interrupt _for Channel 1 and Channel 2_ This bit indicates an
         *  interrupt event in the traffic manager and scheduler logic of DWC_gmac To reset
         *  this bit, the software must read the corresponding registers _Channel Status
         *  Register_ to get the exact cause of the interrupt and clear its source Note: GTMSI
         *  status is given only in Channel 1 and Channel 2 DMA register when the AV feature is
         *  enabled and corresponding additional transmit channels are present Otherwise, this
         *  bit is reserved When this bit is high, the interrupt signal from the MAC
         *  _sbd_intr_o_ is high
         */
        uint32_t ch1_glpii_gtmsi:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} gmac_register69_channel1statusregister_reg_t;

/** Type of register70_channel1operationmoderegister register
 *  Establishes the Receive and Transmit operating modes and command
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** ch1_sr : R/W; bitpos: [1]; default: 0;
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
        uint32_t ch1_sr:1;
        /** ch1_osf : R/W; bitpos: [2]; default: 0;
         *  Operate on Second Frame When this bit is set, it instructs the DMA to process the
         *  second frame of the Transmit data even before the status for the first frame is
         *  obtained
         */
        uint32_t ch1_osf:1;
        /** ch1_rtc : R/W; bitpos: [4:3]; default: 0;
         *  Receive Threshold Control These two bits control the threshold level of the MTL
         *  Receive FIFO Transfer _request_ to DMA starts when the frame size within the MTL
         *  Receive FIFO is larger than the threshold In addition, full frames with length less
         *  than the threshold are automatically transferred The value of 11 is not applicable
         *  if the configured Receive FIFO size is 128 bytes These bits are valid only when the
         *  RSF bit is zero, and are ignored when the RSF bit is set to 1 00: 64 01: 32 10: 96
         *  11: 128
         */
        uint32_t ch1_rtc:2;
        /** ch1_dgf : R/W; bitpos: [5]; default: 0;
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
        uint32_t ch1_dgf:1;
        /** ch1_fuf : R/W; bitpos: [6]; default: 0;
         *  Forward Undersized Good Frames When set, the Rx FIFO forwards Undersized frames
         *  _that is, frames with no Error and length less than 64 bytes_ including padbytes
         *  and CRC When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a
         *  frame is already transferred because of the lower value of Receive Threshold, for
         *  example, RTC = 01
         */
        uint32_t ch1_fuf:1;
        /** ch1_fef : R/W; bitpos: [7]; default: 0;
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
        uint32_t ch1_fef:1;
        /** ch1_efc : R/W; bitpos: [8]; default: 0;
         *  Enable HW Flow Control When this bit is set, the flow control signal operation
         *  based on the filllevel of Rx FIFO is enabled When reset, the flow control operation
         *  is disabled This bit is not used _reserved and always reset_ when the Rx FIFO is
         *  less than 4 KB
         */
        uint32_t ch1_efc:1;
        /** ch1_rfa : R/W; bitpos: [10:9]; default: 0;
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
        uint32_t ch1_rfa:2;
        /** ch1_rfd : R/W; bitpos: [12:11]; default: 0;
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
        uint32_t ch1_rfd:2;
        /** ch1_st : R/W; bitpos: [13]; default: 0;
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
        uint32_t ch1_st:1;
        /** ch1_ttc : R/W; bitpos: [16:14]; default: 0;
         *  Transmit Threshold Control These bits control the threshold level of the MTL
         *  Transmit FIFO Transmission starts when the frame size within the MTL Transmit FIFO
         *  is larger than the threshold In addition, full frames with a length less than the
         *  threshold are also transmitted These bits are used only when Bit 21 _TSF_ is reset
         *  000: 64 001: 128 010: 192 011: 256 100: 40 101: 32 110: 24 111: 16
         */
        uint32_t ch1_ttc:3;
        uint32_t reserved_17:3;
        /** ch1_ftf : R/W1S; bitpos: [20]; default: 0;
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
        uint32_t ch1_ftf:1;
        /** ch1_tsf : R/W; bitpos: [21]; default: 0;
         *  Transmit Store and Forward When this bit is set, transmission starts when a full
         *  frame resides in the MTL Transmit FIFO When this bit is set, the TTC values
         *  specified in Bits [16:14] are ignored This bit should be changed only when the
         *  transmission is stopped
         */
        uint32_t ch1_tsf:1;
        /** ch1_rfd_2 : R/W; bitpos: [22]; default: 0;
         *  MSB of Threshold for Deactivating Flow Control If the DWC_gmac is configured for Rx
         *  FIFO size of 8 KB or more, this bit _when set_ provides additional threshold levels
         *  for deactivating the flow control in both halfduplex and fullduplex modes This bit
         *  _as Most Significant Bit_ along with the RFD _Bits [12:11]_ gives the following
         *  thresholds for deactivating flow control: 100: Full minus 5 KB, that is, FULL — 5
         *  KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that is, FULL —
         *  7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB or less deep
         */
        uint32_t ch1_rfd_2:1;
        /** ch1_rfa_2 : R/W; bitpos: [23]; default: 0;
         *  MSB of Threshold for Activating Flow Control If the DWC_gmac is configured for an
         *  Rx FIFO size of 8 KB or more, this bit _when set_ provides additional threshold
         *  levels for activating the flow control in both half duplex and fullduplex modes
         *  This bit _as Most Significant Bit_, along with the RFA _Bits [10:9]_, gives the
         *  following thresholds for activating flow control: 100: Full minus 5 KB, that is,
         *  FULL — 5 KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that
         *  is, FULL — 7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB
         *  or less deep
         */
        uint32_t ch1_rfa_2:1;
        /** ch1_dff : R/W; bitpos: [24]; default: 0;
         *  Disable Flushing of Received Frames When this bit is set, the Rx DMA does not flush
         *  any frames because of the unavailability of receive descriptors or buffers as it
         *  does normally when this bit is reset _See “Receive Process Suspended” on page 83_
         *  This bit is reserved _and RO_ in the GMACMTL configuration
         */
        uint32_t ch1_dff:1;
        /** ch1_rsf : R/W; bitpos: [25]; default: 0;
         *  Receive Store and Forward When this bit is set, the MTL reads a frame from the Rx
         *  FIFO only after the complete frame has been written to it, ignoring the RTC bits
         *  When this bit is reset, the Rx FIFO operates in the cutthrough mode, subject to the
         *  threshold specified by the RTC bits
         */
        uint32_t ch1_rsf:1;
        /** ch1_dt : R/W; bitpos: [26]; default: 0;
         *  Disable Dropping of TCP/IP Checksum Error Frames When this bit is set, the MAC does
         *  not drop the frames which only have errors detected by the Receive Checksum Offload
         *  engine Such frames do not have any errors _including FCS error_ in the Ethernet
         *  frame received by the MAC but have errors only in the encapsulated payload When
         *  this bit is reset, all error frames are dropped if the FEF bit is reset If the IPC
         *  Full Checksum Offload Engine _Type 2_ is disabled, this bit is reserved _RO with
         *  value 1'b0_
         */
        uint32_t ch1_dt:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} gmac_register70_channel1operationmoderegister_reg_t;

/** Type of register71_channel1interruptenableregister register
 *  Enables the interrupts reported by the Status Register
 */
typedef union {
    struct {
        /** ch1_tie : R/W; bitpos: [0]; default: 0;
         *  Transmit Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
         *  _Bit 16_, the Transmit Interrupt is enabled When this bit is reset, the Transmit
         *  Interrupt is disabled The sbd_intr_o interrupt is generated as shown in Figure 61
         *  It is asserted only when the TTI, GPI, GMI, GLI, or GLPII bit of the DMA Status
         *  register is asserted, or when the NIS or AIS Status bit is asserted and the
         *  corresponding Interrupt Enable bits _NIE or AIE_ are enabled
         */
        uint32_t ch1_tie:1;
        /** ch1_tse : R/W; bitpos: [1]; default: 0;
         *  Transmit Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
         *  _Bit 15_, the Transmission Stopped Interrupt is enabled When this bit is reset, the
         *  Transmission Stopped Interrupt is disabled
         */
        uint32_t ch1_tse:1;
        /** ch1_tue : R/W; bitpos: [2]; default: 0;
         *  Transmit Buffer Unavailable Enable When this bit is set with Normal Interrupt
         *  Summary Enable _Bit 16_, the Transmit Buffer Unavailable Interrupt is enabled When
         *  this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled
         */
        uint32_t ch1_tue:1;
        /** ch1_tje : R/W; bitpos: [3]; default: 0;
         *  Transmit Jabber Timeout Enable When this bit is set with Abnormal Interrupt Summary
         *  Enable _Bit 15_, the Transmit Jabber Timeout Interrupt is enabled When this bit is
         *  reset, the Transmit Jabber Timeout Interrupt is disabled
         */
        uint32_t ch1_tje:1;
        /** ch1_ove : R/W; bitpos: [4]; default: 0;
         *  Overflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
         *  Enable _Bit 15_, the Receive Overflow Interrupt is enabled When this bit is reset,
         *  the Overflow Interrupt is disabled
         */
        uint32_t ch1_ove:1;
        /** ch1_une : R/W; bitpos: [5]; default: 0;
         *  Underflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
         *  Enable _Bit 15_, the Transmit Underflow Interrupt is enabled When this bit is
         *  reset, the Underflow Interrupt is disabled
         */
        uint32_t ch1_une:1;
        /** ch1_rie : R/W; bitpos: [6]; default: 0;
         *  Receive Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
         *  _Bit 16_, the Receive Interrupt is enabled When this bit is reset, the Receive
         *  Interrupt is disabled
         */
        uint32_t ch1_rie:1;
        /** ch1_rue : R/W; bitpos: [7]; default: 0;
         *  Receive Buffer Unavailable Enable When this bit is set with Abnormal Interrupt
         *  Summary Enable _Bit 15_, the Receive Buffer Unavailable Interrupt is enabled When
         *  this bit is reset, the Receive Buffer Unavailable Interrupt is disabled
         */
        uint32_t ch1_rue:1;
        /** ch1_rse : R/W; bitpos: [8]; default: 0;
         *  Receive Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
         *  _Bit 15_, the Receive Stopped Interrupt is enabled When this bit is reset, the
         *  Receive Stopped Interrupt is disabled
         */
        uint32_t ch1_rse:1;
        /** ch1_rwe : R/W; bitpos: [9]; default: 0;
         *  Receive Watchdog Timeout Enable When this bit is set with Abnormal Interrupt
         *  Summary Enable _Bit 15_, the Receive Watchdog Timeout Interrupt is enabled When
         *  this bit is reset, the Receive Watchdog Timeout Interrupt is disabled
         */
        uint32_t ch1_rwe:1;
        /** ch1_ete : R/W; bitpos: [10]; default: 0;
         *  Early Transmit Interrupt Enable When this bit is set with an Abnormal Interrupt
         *  Summary Enable _Bit 15_, the Early Transmit Interrupt is enabled When this bit is
         *  reset, the Early Transmit Interrupt is disabled
         */
        uint32_t ch1_ete:1;
        uint32_t reserved_11:2;
        /** ch1_fbe : R/W; bitpos: [13]; default: 0;
         *  Fatal Bus Error Enable When this bit is set with Abnormal Interrupt Summary Enable
         *  _Bit 15_, the Fatal Bus Error Interrupt is enabled When this bit is reset, the
         *  Fatal Bus Error Enable Interrupt is disabled 12:11		Reserved	00	RO
         */
        uint32_t ch1_fbe:1;
        /** ch1_ere : R/W; bitpos: [14]; default: 0;
         *  Early Receive Interrupt Enable When this bit is set with Normal Interrupt Summary
         *  Enable _Bit 16_, the Early Receive Interrupt is enabled When this bit is reset, the
         *  Early Receive Interrupt is disabled
         */
        uint32_t ch1_ere:1;
        /** ch1_aie : R/W; bitpos: [15]; default: 0;
         *  Abnormal Interrupt Summary Enable When this bit is set, abnormal interrupt summary
         *  is enabled When this bit is reset, the abnormal interrupt summary is disabled This
         *  bit enables the following interrupts in Register 5 _Status Register_: Register
         *  5[1]: Transmit Process Stopped Register 5[3]: Transmit Jabber Timeout Register
         *  5[4]: Receive Overflow Register 5[5]: Transmit Underflow Register 5[7]: Receive
         *  Buffer Unavailable Register 5[8]: Receive Process Stopped Register 5[9]: Receive
         *  Watchdog Timeout Register 5[10]: Early Transmit Interrupt Register 5[13]: Fatal Bus
         *  Error
         */
        uint32_t ch1_aie:1;
        /** ch1_nie : R/W; bitpos: [16]; default: 0;
         *  Normal Interrupt Summary Enable When this bit is set, normal interrupt summary is
         *  enabled When this bit is reset, normal interrupt summary is disabled This bit
         *  enables the following interrupts in Register 5 _Status Register_: Register 5[0]:
         *  Transmit Interrupt Register 5[2]: Transmit Buffer Unavailable Register 5[6]:
         *  Receive Interrupt Register 5[14]: Early Receive Interrupt
         */
        uint32_t ch1_nie:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} gmac_register71_channel1interruptenableregister_reg_t;

/** Type of register72_channel1missedframeandbufferoverflowcounterregister register
 *  Contains the counters for discarded frames because no host Receive Descriptor was
 *  available, and discarded frames because of Receive FIFO Overflow
 */
typedef union {
    struct {
        /** ch1_misfrmcnt : R/W; bitpos: [15:0]; default: 0;
         *  Missed Frame Counter This field indicates the number of frames missed by the
         *  controller because of the Host Receive Buffer being unavailable This counter is
         *  incremented each time the DMA discards an incoming frame The counter is cleared
         *  when this register is read with mci_be_i[0] at 1’b1
         */
        uint32_t ch1_misfrmcnt:16;
        /** ch1_miscntovf : R/W; bitpos: [16]; default: 0;
         *  Overflow Bit for Missed Frame Counter This bit is set every time Missed Frame
         *  Counter _Bits[15:0]_ overflows, that is, the DMA discards an incoming frame because
         *  of the Host Receive Buffer being unavailable with the missed frame counter at
         *  maximum value In such a scenario, the Missed frame counter is reset to allzeros and
         *  this bit indicates that the rollover happened
         */
        uint32_t ch1_miscntovf:1;
        /** ch1_ovffrmcnt : R/W; bitpos: [27:17]; default: 0;
         *  Overflow Frame Counter This field indicates the number of frames missed by the
         *  application This counter is incremented each time the MTL FIFO overflows The
         *  counter is cleared when this register is read with mci_be_i[2] at 1’b1
         */
        uint32_t ch1_ovffrmcnt:11;
        /** ch1_ovfcntovf : R/W; bitpos: [28]; default: 0;
         *  Overflow Bit for FIFO Overflow Counter This bit is set every time the Overflow
         *  Frame Counter _Bits[27:17]_ overflows, that is, the Rx FIFO overflows with the
         *  overflow frame counter at maximum value In such a scenario, the overflow frame
         *  counter is reset to allzeros and this bit indicates that the rollover happened
         */
        uint32_t ch1_ovfcntovf:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} gmac_register72_channel1missedframeandbufferoverflowcounterregister_reg_t;

/** Type of register73_channel1receiveinterruptwatchdogtimerregister register
 *  Watchdog timeout for Receive Interrupt _RI_ from DMA
 */
typedef union {
    struct {
        /** ch1_riwt : R/W; bitpos: [7:0]; default: 0;
         *  RI Watchdog Timer Count This bit indicates the number of system clock cycles
         *  multiplied by 256 for which the watchdog timer is set The watchdog timer gets
         *  triggered with the programmed value after the Rx DMA completes the transfer of a
         *  frame for which the RI status bit is not set because of the setting in the
         *  corresponding descriptor RDES1[31] When the watchdog timer runs out, the RI bit is
         *  set and the timer is stopped The watchdog timer is reset when the RI bit is set
         *  high because of automatic setting of RI as per RDES1[31] of any received frame
         */
        uint32_t ch1_riwt:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gmac_register73_channel1receiveinterruptwatchdogtimerregister_reg_t;

/** Type of register76_channel1slotfunctioncontrolandstatusregister register
 *  Contains the control bits for slot function and its status for Channel 1 transmit
 *  path
 */
typedef union {
    struct {
        /** esc : R/W; bitpos: [0]; default: 0;
         *  Enable Slot Comparison When set, this bit enables the checking of the slot numbers,
         *  programmed in the transmit descriptor, with the current reference given in Bits
         *  [19:16] The DMA fetches the data from the corresponding buffer only when the slot
         *  number is equal to the reference slot number or is ahead of the reference slot
         *  number by one slot When reset, this bit disables the checking of the slot numbers
         *  The DMA fetches the data immediately after the descriptor is processed
         */
        uint32_t esc:1;
        /** asc : R/W; bitpos: [1]; default: 0;
         *  Advance Slot Check When set, this bit enables the DMA to fetch the data from the
         *  buffer when the slot number _SLOTNUM_ programmed in the transmit descriptor is:
         *  equal to the reference slot number given in Bits [19:16] or ahead of the reference
         *  slot number by up to two slots This bit is applicable only when Bit 0 _ESC_ is set
         */
        uint32_t asc:1;
        uint32_t reserved_2:14;
        /** rsn : RO; bitpos: [19:16]; default: 0;
         *  Reference Slot Number This field gives the current value of the reference slot
         *  number in DMA used for comparison checking
         */
        uint32_t rsn:4;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} gmac_register76_channel1slotfunctioncontrolandstatusregister_reg_t;

/** Type of register82_channel1currenthosttransmitdescriptorregister register
 *  Points to the start of current Transmit Descriptor read by the DMA
 */
typedef union {
    struct {
        /** ch1_curtdesaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Transmit Descriptor Address Pointer
         */
        uint32_t ch1_curtdesaptr:32;
    };
    uint32_t val;
} gmac_register82_channel1currenthosttransmitdescriptorregister_reg_t;

/** Type of register83_channel1currenthostreceivedescriptorregister register
 *  Points to the start of current Receive Descriptor read by the DMA
 */
typedef union {
    struct {
        /** ch1_currdesaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Receive Descriptor Address Pointer
         */
        uint32_t ch1_currdesaptr:32;
    };
    uint32_t val;
} gmac_register83_channel1currenthostreceivedescriptorregister_reg_t;

/** Type of register84_channel1currenthosttransmitbufferaddressregister register
 *  Points to the current Transmit Buffer address read by the DMA
 */
typedef union {
    struct {
        /** ch1_curtbufaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Transmit Buffer Address Pointer
         */
        uint32_t ch1_curtbufaptr:32;
    };
    uint32_t val;
} gmac_register84_channel1currenthosttransmitbufferaddressregister_reg_t;

/** Type of register85_channel1currenthostreceivebufferaddressregister register
 *  Points to the current Receive Buffer address read by the DMA
 */
typedef union {
    struct {
        /** ch1_currbufaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Receive Buffer Address Pointer
         */
        uint32_t ch1_currbufaptr:32;
    };
    uint32_t val;
} gmac_register85_channel1currenthostreceivebufferaddressregister_reg_t;

/** Type of register88_channel1cbscontrolregister register
 *  Controls the Channel 1 credit shaping operation on the transmit path
 */
typedef union {
    struct {
        /** ch1_cbsd : R/W; bitpos: [0]; default: 0;
         *  CreditBased Shaper Disable When set, the MAC disables the creditbased shaper
         *  algorithm for Channel 1 traffic and makes the traffic management algorithm to
         *  strict priority for Channel 1 over Channel 0 When reset, the creditbased shaper
         *  algorithm schedules the traffic in Channel 1 for transmission
         */
        uint32_t ch1_cbsd:1;
        /** ch1_cc : R/W; bitpos: [1]; default: 0;
         *  Credit Control When reset, the accumulated credit parameter in the creditbased
         *  shaper algorithm logic is set to zero when there is positive credit and no frame to
         *  transmit in Channel 1 When there is no frame waiting in Channel 1 and other channel
         *  is transmitting, no credit is accumulated When set, the accumulated credit
         *  parameter in the creditbased shaper algorithm logic is not reset to zero when there
         *  is positive credit and no frame to transmit in Channel 1 The credit accumulates
         *  even when there is no frame waiting in Channel 1 and another channel is transmitting
         */
        uint32_t ch1_cc:1;
        uint32_t reserved_2:2;
        /** ch1_slc : R/W; bitpos: [6:4]; default: 0;
         *  Slot Count The software can program the number of slots _of duration 125 microsec_
         *  over which the average transmitted bits per slot _provided in the CBS Status
         *  register_ need to be computed for Channel 1 when the creditbased shaper algorithm
         *  is enabled The encoding is as follows: 3'b000: 1 Slot 3'b001: 2 Slots 3'b010: 4
         *  Slots 3'b011: 8 Slots 3'b100: 16 Slots 3'b1013'b111: Reserved
         */
        uint32_t ch1_slc:3;
        uint32_t reserved_7:10;
        /** ch1_abpssie : R/W; bitpos: [17]; default: 0;
         *  Average Bits Per Slot Interrupt Enable When this bit is set, the MAC asserts an
         *  interrupt _sbd_intr_o or mci_intr_o_ when the average bits per slot status is
         *  updated _Bit 17 _ABSU_ in Register 89_ for Channel 1 When this bit is cleared,
         *  interrupt is not asserted for such an event
         */
        uint32_t ch1_abpssie:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gmac_register88_channel1cbscontrolregister_reg_t;

/** Type of register89_channel1cbsstatusregister register
 *  Provides the average traffic transmitted in Channel 1
 */
typedef union {
    struct {
        /** ch1_abs : RO; bitpos: [16:0]; default: 0;
         *  Average Bits per Slot This field contains the average transmitted bits per slot
         *  This field is computed over programmed number of slots _SLC bits in the CBS Control
         *  Register_ for Channel 1 traffic The maximum value is 0x30D4 for 100 Mbps and
         *  0x1E848 for 1000 Mbps
         */
        uint32_t ch1_abs:17;
        /** ch1_absu : RO; bitpos: [17]; default: 0;
         *  ABS Updated When set, this bit indicates that the MAC has updated the ABS value
         *  This bit is cleared when the application reads the ABS value
         */
        uint32_t ch1_absu:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gmac_register89_channel1cbsstatusregister_reg_t;

/** Type of register90_channel1idleslopecreditregister register
 *  Contains the idleSlope credit value required for the creditbased shaper algorithm
 *  for Channel 1
 */
typedef union {
    struct {
        /** ch1_isc : R/W; bitpos: [13:0]; default: 0;
         *  idleSlopeCredit This field contains the idleSlopeCredit value required for the
         *  creditbased shaper algorithm for Channel 1 This is the rate of change of credit in
         *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
         *  credit is increasing The software should program this field with computed credit in
         *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
         *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode
         */
        uint32_t ch1_isc:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} gmac_register90_channel1idleslopecreditregister_reg_t;

/** Type of register91_channel1sendslopecreditregister register
 *  Contains the sendSlope credit value required for the creditbased shaper algorithm
 *  for Channel 1
 */
typedef union {
    struct {
        /** ch1_ssc : R/W; bitpos: [13:0]; default: 0;
         *  sendSlopeCredit This field contains the sendSlopeCredit value required for
         *  creditbased shaper algorithm for Channel 1 This is the rate of change of credit in
         *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
         *  credit is decreasing The software should program this field with computed credit in
         *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
         *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode This field should be
         *  programmed with absolute sendSlopeCredit value The creditbased shaper logic
         *  subtracts it from the accumulated credit when Channel 1 is selected for transmission
         */
        uint32_t ch1_ssc:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} gmac_register91_channel1sendslopecreditregister_reg_t;

/** Type of register92_channel1hicreditregister register
 *  Contains the hiCredit value required for the creditbased shaper algorithm for
 *  Channel 1
 */
typedef union {
    struct {
        /** ch1_hc : R/W; bitpos: [28:0]; default: 0;
         *  hiCredit This field contains the hiCredit value required for the creditbased shaper
         *  algorithm for Channel 1 This is the maximum value that can be accumulated in the
         *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
         *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
         *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
         *  0x0800_0000
         */
        uint32_t ch1_hc:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} gmac_register92_channel1hicreditregister_reg_t;

/** Type of register93_channel1locreditregister register
 *  Contains the loCredit value required for the creditbased shaper algorithm for
 *  Channel 1
 */
typedef union {
    struct {
        /** ch1_lc : R/W; bitpos: [28:0]; default: 536870911;
         *  loCredit This field contains the loCredit value required for the creditbased shaper
         *  algorithm for Channel 1 This is the minimum value that can be accumulated in the
         *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
         *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
         *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
         *  0x0800_0000 The programmed value is 2's complement _negative number_, that is,
         *  0xF800_0000
         */
        uint32_t ch1_lc:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} gmac_register93_channel1locreditregister_reg_t;

/** Type of register128_channel2busmoderegister register
 *  Controls the Host Interface mode for Channel 2
 */
typedef union {
    struct {
        /** ch2_swr : R/W; bitpos: [0]; default: 1;
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
        uint32_t ch2_swr:1;
        /** ch2_da : R/W; bitpos: [1]; default: 0;
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
        uint32_t ch2_da:1;
        /** ch2_dsl : R/W; bitpos: [6:2]; default: 0;
         *  Descriptor Skip Length This bit specifies the number of Word, Dword, or Lword
         *  _depending on the 32bit, 64bit, or 128bit bus_ to skip between two unchained
         *  descriptors The address skipping starts from the end of current descriptor to the
         *  start of next descriptor When the DSL value is equal to zero, the descriptor table
         *  is taken as contiguous by the DMA in Ring mode
         */
        uint32_t ch2_dsl:5;
        /** ch2_atds : R/W; bitpos: [7]; default: 0;
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
        uint32_t ch2_atds:1;
        /** ch2_pbl : R/W; bitpos: [13:8]; default: 1;
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
        uint32_t ch2_pbl:6;
        /** ch2_pr : R/W; bitpos: [15:14]; default: 0;
         *  Priority Ratio These bits control the priority ratio in the weighted roundrobin
         *  arbitration between the Rx DMA and Tx DMA These bits are valid only when Bit 1 _DA_
         *  is reset The priority ratio is Rx:Tx or Tx:Rx depending on whether Bit 27 _TXPR_ is
         *  reset or set 00: The Priority Ratio is 1:1 01: The Priority Ratio is 2:1 10: The
         *  Priority Ratio is 3:1 11: The Priority Ratio is 4:1 In the GMACAXI configuration,
         *  these bits are reserved and readonly _RO_ For more information about the priority
         *  scheme between the transmit and receive paths, see Table 412 in “DMA Arbiter
         *  Functions” on page 167
         */
        uint32_t ch2_pr:2;
        /** ch2_fb : R/W; bitpos: [16]; default: 0;
         *  Fixed Burst This bit controls whether the AHB or AXI master interface performs
         *  fixed burst transfers or not When set, the AHB interface uses only SINGLE, INCR4,
         *  INCR8, or INCR16 during start of the normal burst transfers When reset, the AHB or
         *  AXI interface uses SINGLE and INCR burst transfer operations For more information,
         *  see Bit 0 _UNDEF_ of the AXI Bus Mode register in the GMACAXI configuration
         */
        uint32_t ch2_fb:1;
        /** ch2_rpbl : R/W; bitpos: [22:17]; default: 1;
         *  Rx DMA PBL This field indicates the maximum number of beats to be transferred in
         *  one Rx DMA transaction This is the maximum value that is used in a single block
         *  Read or Write The Rx DMA always attempts to burst as specified in the RPBL bit each
         *  time it starts a Burst transfer on the host bus You can program RPBL with values of
         *  1, 2, 4, 8, 16, and 32 Any other value results in undefined behavior This field is
         *  valid and applicable only when USP is set high
         */
        uint32_t ch2_rpbl:6;
        /** ch2_usp : R/W; bitpos: [23]; default: 0;
         *  Use Separate PBL When set high, this bit configures the Rx DMA to use the value
         *  configured in Bits [22:17] as PBL The PBL value in Bits [13:8] is applicable only
         *  to the Tx DMA operations When reset to low, the PBL value in Bits [13:8] is
         *  applicable for both DMA engines
         */
        uint32_t ch2_usp:1;
        /** ch2_pblx8 : R/W; bitpos: [24]; default: 0;
         *  PBLx8 Mode When set high, this bit multiplies the programmed PBL value _Bits
         *  [22:17] and Bits[13:8]_ eight times Therefore, the DMA transfers the data in 8, 16,
         *  32, 64, 128, and 256 beats depending on the PBL value Note: This bit function is
         *  not backward compatible Before release 350a, this bit was 4xPBL
         */
        uint32_t ch2_pblx8:1;
        /** ch2_aal : R/W; bitpos: [25]; default: 0;
         *  AddressAligned Beats When this bit is set high and the FB bit is equal to 1, the
         *  AHB or AXI interface generates all bursts aligned to the start address LS bits If
         *  the FB bit is equal to 0, the first burst _accessing the start address of data
         *  buffer_ is not aligned, but subsequent bursts are aligned to the address This bit
         *  is valid only in the GMACAHB and GMACAXI configurations and is reserved _RO with
         *  default value 0_ in all other configurations
         */
        uint32_t ch2_aal:1;
        /** ch2_mb : R/W; bitpos: [26]; default: 0;
         *  Mixed Burst When this bit is set high and the FB bit is low, the AHB master
         *  interface starts all bursts of length more than 16 with INCR _undefined burst_,
         *  whereas it reverts to fixed burst transfers _INCRx and SINGLE_ for burst length of
         *  16 and less This bit is valid only in the GMACAHB configuration and reserved in all
         *  other configuration
         */
        uint32_t ch2_mb:1;
        /** ch2_txpr : R/W; bitpos: [27]; default: 0;
         *  Transmit Priority When set, this bit indicates that the transmit DMA has higher
         *  priority than the receive DMA during arbitration for the systemside bus In the
         *  GMACAXI configuration, this bit is reserved and readonly _RO_ For more information
         *  about the priority scheme between the transmit and receive paths, see Table 412 in
         *  “DMA Arbiter Functions” on page 167
         */
        uint32_t ch2_txpr:1;
        /** ch2_prwg : R/W; bitpos: [29:28]; default: 0;
         *  Channel Priority Weights This field sets the priority weights for Channel 2 during
         *  the roundrobin arbitration between the DMA channels for the system bus 00: The
         *  priority weight is 1 01: The priority weight is 2 10: The priority weight is 3 11:
         *  The priority weight is 4 This field is present in all DWC_gmac configurations
         *  except GMACAXI when you select the AV feature Otherwise, this field is reserved and
         *  readonly _RO_ For more information about the priority weights of DMA channels, see
         *  “DMA Arbiter Functions” on page 167
         */
        uint32_t ch2_prwg:2;
        uint32_t reserved_30:1;
        /** ch2_rib : R/W; bitpos: [31]; default: 0;
         *  Rebuild INCRx Burst When this bit is set high and the AHB master gets an EBT
         *  _Retry, Split, or Losing bus grant_, the AHB master interface rebuilds the pending
         *  beats of any burst transfer initiated with INCRx The AHB master interface rebuilds
         *  the beats with a combination of specified bursts with INCRx and SINGLE By default,
         *  the AHB master interface rebuilds pending beats of an EBT with an unspecified
         *  _INCR_ burst This bit is valid only in the GMACAHB configuration It is reserved in
         *  all other configuration
         */
        uint32_t ch2_rib:1;
    };
    uint32_t val;
} gmac_register128_channel2busmoderegister_reg_t;

/** Type of register129_channel2transmitpolldemandregister register
 *  Used by the host to instruct the DMA to poll the Transmit Descriptor list
 */
typedef union {
    struct {
        /** ch2_tpd : RO; bitpos: [31:0]; default: 0;
         *  Transmit Poll Demand When these bits are written with any value, the DMA reads the
         *  current descriptor to which the Register 18 _Current Host Transmit Descriptor
         *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
         *  transmission returns to the Suspend state and Bit 2 _TU_ of Register 5 _Status
         *  Register_ is asserted If the descriptor is available, the transmission resumes
         */
        uint32_t ch2_tpd:32;
    };
    uint32_t val;
} gmac_register129_channel2transmitpolldemandregister_reg_t;

/** Type of register130_channel2receivepolldemandregister register
 *  Used by the Host to instruct the DMA to poll the Receive Descriptor list
 */
typedef union {
    struct {
        /** ch2_rpd : RO; bitpos: [31:0]; default: 0;
         *  Receive Poll Demand When these bits are written with any value, the DMA reads the
         *  current descriptor to which the Register 19 _Current Host Receive Descriptor
         *  Register_ is pointing If that descriptor is not available _owned by the Host_, the
         *  reception returns to the Suspended state and Bit 7 _RU_ of Register 5 _Status
         *  Register_ is asserted If the descriptor is available, the Rx DMA returns to the
         *  active state
         */
        uint32_t ch2_rpd:32;
    };
    uint32_t val;
} gmac_register130_channel2receivepolldemandregister_reg_t;

/** Type of register131_channel2receivedescriptorlistaddressregister register
 *  Points the DMA to the start of the Receive Descriptor list
 */
typedef union {
    struct {
        /** ch2_rdesla : R/W; bitpos: [31:0]; default: 0;
         *  Start of Receive List This field contains the base address of the first descriptor
         *  in the Receive Descriptor list The LSB bits _1:0, 2:0, or 3:0_ for 32bit, 64bit, or
         *  128bit bus width are ignored and internally taken as allzero by the DMA Therefore,
         *  these LSB bits are readonly _RO_
         */
        uint32_t ch2_rdesla:32;
    };
    uint32_t val;
} gmac_register131_channel2receivedescriptorlistaddressregister_reg_t;

/** Type of register132_channel2transmitdescriptorlistaddressregister register
 *  Points the DMA to the start of the Transmit Descriptor List
 */
typedef union {
    struct {
        /** ch2_tdesla : R/W; bitpos: [31:0]; default: 0;
         *  Start of Transmit List This field contains the base address of the first descriptor
         *  in the Transmit Descriptor list The LSB bits _1:0, 2:0, 3:0_ for 32bit, 64bit, or
         *  128bit bus width are ignored and are internally taken as allzero by the DMA
         *  Therefore, these LSB bits are readonly _RO_
         */
        uint32_t ch2_tdesla:32;
    };
    uint32_t val;
} gmac_register132_channel2transmitdescriptorlistaddressregister_reg_t;

/** Type of register133_channel2statusregister register
 *  The software driver _application_ reads this register during interrupt service
 *  routine or polling to determine the status of the DMA Bits [29:26] are reserved for
 *  the Channel 2 Status Register
 */
typedef union {
    struct {
        /** ch2_ti : R/W; bitpos: [0]; default: 0;
         *  Transmit Interrupt This bit indicates that the frame transmission is complete When
         *  transmission is complete, Bit 31 _OWN_ of TDES0 is reset, and the specific frame
         *  status information is updated in the descriptor
         */
        uint32_t ch2_ti:1;
        /** ch2_tps : R/W; bitpos: [1]; default: 0;
         *  Transmit Process Stopped This bit is set when the transmission is stopped
         */
        uint32_t ch2_tps:1;
        /** ch2_tu : R/W; bitpos: [2]; default: 0;
         *  Transmit Buffer Unavailable This bit indicates that the host owns the Next
         *  Descriptor in the Transmit List and the DMA cannot acquire it Transmission is
         *  suspended Bits[22:20] explain the Transmit Process state transitions To resume
         *  processing Transmit descriptors, the host should change the ownership of the
         *  descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command
         */
        uint32_t ch2_tu:1;
        /** ch2_tjt : R/W; bitpos: [3]; default: 0;
         *  Transmit Jabber Timeout This bit indicates that the Transmit Jabber Timer expired,
         *  which happens when the frame size exceeds 2,048 _10,240 bytes when the Jumbo frame
         *  is enabled_ When the Jabber Timeout occurs, the transmission process is aborted and
         *  placed in the Stopped state This causes the Transmit Jabber Timeout TDES0[14] flag
         *  to assert
         */
        uint32_t ch2_tjt:1;
        /** ch2_ovf : R/W; bitpos: [4]; default: 0;
         *  Receive Overflow This bit indicates that the Receive Buffer had an Overflow during
         *  frame reception If the partial frame is transferred to the application, the
         *  overflow status is set in RDES0[11]
         */
        uint32_t ch2_ovf:1;
        /** ch2_unf : R/W; bitpos: [5]; default: 0;
         *  Transmit Underflow This bit indicates that the Transmit Buffer had an Underflow
         *  during frame transmission Transmission is suspended and an Underflow Error TDES0[1]
         *  is set
         */
        uint32_t ch2_unf:1;
        /** ch2_ri : R/W; bitpos: [6]; default: 0;
         *  Receive Interrupt This bit indicates that the frame reception is complete When
         *  reception is complete, the Bit 31 of RDES1 _Disable Interrupt on Completion_ is
         *  reset in the last Descriptor, and the specific frame status information is updated
         *  in the descriptor The reception remains in the Running state
         */
        uint32_t ch2_ri:1;
        /** ch2_ru : R/W; bitpos: [7]; default: 0;
         *  Receive Buffer Unavailable This bit indicates that the host owns the Next
         *  Descriptor in the Receive List and the DMA cannot acquire it The Receive Process is
         *  suspended To resume processing Receive descriptors, the host should change the
         *  ownership of the descriptor and issue a Receive Poll Demand command If no Receive
         *  Poll Demand is issued, the Receive Process resumes when the next recognized
         *  incoming frame is received This bit is set only when the previous Receive
         *  Descriptor is owned by the DMA
         */
        uint32_t ch2_ru:1;
        /** ch2_rps : R/W; bitpos: [8]; default: 0;
         *  Receive Process Stopped This bit is asserted when the Receive Process enters the
         *  Stopped state
         */
        uint32_t ch2_rps:1;
        /** ch2_rwt : R/W; bitpos: [9]; default: 0;
         *  Receive Watchdog Timeout When set, this bit indicates that the Receive Watchdog
         *  Timer expired while receiving the current frame and the current frame is truncated
         *  after the watchdog timeout
         */
        uint32_t ch2_rwt:1;
        /** ch2_eti : R/W; bitpos: [10]; default: 0;
         *  Early Transmit Interrupt This bit indicates that the frame to be transmitted is
         *  fully transferred to the MTL Transmit FIFO
         */
        uint32_t ch2_eti:1;
        uint32_t reserved_11:2;
        /** ch2_fbi : R/W; bitpos: [13]; default: 0;
         *  Fatal Bus Error Interrupt This bit indicates that a bus error occurred, as
         *  described in Bits [25:23] When this bit is set, the corresponding DMA engine
         *  disables all of its bus accesses 12:11		Reserved	00	RO
         */
        uint32_t ch2_fbi:1;
        /** ch2_eri : R/W; bitpos: [14]; default: 0;
         *  Early Receive Interrupt This bit indicates that the DMA filled the first data
         *  buffer of the packet This bit is cleared when the software writes 1 to this bit or
         *  Bit 6 _RI_ of this register is set _whichever occurs earlier_
         */
        uint32_t ch2_eri:1;
        /** ch2_ais : R/W; bitpos: [15]; default: 0;
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
        uint32_t ch2_ais:1;
        /** ch2_nis : R/W; bitpos: [16]; default: 0;
         *  Normal Interrupt Summary Normal Interrupt Summary bit value is the logical OR of
         *  the following bits when the corresponding interrupt bits are enabled in Register 7
         *  _Interrupt Enable Register_: Register 5[0]: Transmit Interrupt Register 5[2]:
         *  Transmit Buffer Unavailable Register 5[6]: Receive Interrupt Register 5[14]: Early
         *  Receive Interrupt Only unmasked bits _interrupts for which interrupt enable is set
         *  in Register 7_ affect the Normal Interrupt Summary bit This is a sticky bit and
         *  must be cleared _by writing 1 to this bit_ each time a corresponding bit, which
         *  causes NIS to be set, is cleared
         */
        uint32_t ch2_nis:1;
        /** ch2_rs : RO; bitpos: [19:17]; default: 0;
         *  Receive Process State This field indicates the Receive DMA FSM state This field
         *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Receive Command
         *  issued 3’b001: Running: Fetching Receive Transfer Descriptor 3’b010: Reserved for
         *  future use 3’b011: Running: Waiting for receive packet 3’b100: Suspended: Receive
         *  Descriptor Unavailable 3’b101: Running: Closing Receive Descriptor 3’b110:
         *  TIME_STAMP write state 3’b111: Running: Transferring the receive packet data from
         *  receive buffer to host memory
         */
        uint32_t ch2_rs:3;
        /** ch2_ts : RO; bitpos: [22:20]; default: 0;
         *  Transmit Process State This field indicates the Transmit DMA FSM state This field
         *  does not generate an interrupt 3’b000: Stopped: Reset or Stop Transmit Command
         *  issued 3’b001: Running: Fetching Transmit Transfer Descriptor 3’b010: Running:
         *  Waiting for status 3’b011: Running: Reading Data from host memory buffer and
         *  queuing it to transmit buffer _Tx FIFO_ 3’b100: TIME_STAMP write state 3’b101:
         *  Reserved for future use 3’b110: Suspended: Transmit Descriptor Unavailable or
         *  Transmit Buffer Underflow 3’b111: Running: Closing Transmit Descriptor
         */
        uint32_t ch2_ts:3;
        /** ch2_eb : RO; bitpos: [25:23]; default: 0;
         *  Error Bits This field indicates the type of error that caused a Bus Error, for
         *  example, error response on the AHB or AXI interface This field is valid only when
         *  Bit 13 _FBI_ is set This field does not generate an interrupt 0 0 0: Error during
         *  Rx DMA Write Data Transfer 0 1 1: Error during Tx DMA Read Data Transfer 1 0 0:
         *  Error during Rx DMA Descriptor Write Access 1 0 1: Error during Tx DMA Descriptor
         *  Write Access 1 1 0: Error during Rx DMA Descriptor Read Access 1 1 1: Error during
         *  Tx DMA Descriptor Read Access Note: 001 and 010 are reserved
         */
        uint32_t ch2_eb:3;
        /** ch2_gli : RO; bitpos: [26]; default: 0;
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
        uint32_t ch2_gli:1;
        /** ch2_gmi : RO; bitpos: [27]; default: 0;
         *  GMAC MMC Interrupt This bit reflects an interrupt event in the MMC module of the
         *  DWC_gmac The software must read the corresponding registers in the DWC_gmac to get
         *  the exact cause of the interrupt and clear the source of interrupt to make this bit
         *  as 1’b0 The interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when
         *  this bit is high This bit is applicable only when the MAC Management Counters _MMC_
         *  are enabled Otherwise, this bit is reserved
         */
        uint32_t ch2_gmi:1;
        /** ch2_gpi : RO; bitpos: [28]; default: 0;
         *  GMAC PMT Interrupt This bit indicates an interrupt event in the PMT module of the
         *  DWC_gmac The software must read the PMT Control and Status Register in the MAC to
         *  get the exact cause of interrupt and clear its source to reset this bit to 1’b0 The
         *  interrupt signal from the DWC_gmac subsystem _sbd_intr_o_ is high when this bit is
         *  high This bit is applicable only when the Power Management feature is enabled
         *  Otherwise, this bit is reserved Note: The GPI and pmt_intr_o interrupts are
         *  generated in different clock domains
         */
        uint32_t ch2_gpi:1;
        /** ch2_tti : RO; bitpos: [29]; default: 0;
         *  Timestamp Trigger Interrupt This bit indicates an interrupt event in the Timestamp
         *  Generator block of the DWC_gmac The software must read the corresponding registers
         *  in the DWC_gmac to get the exact cause of the interrupt and clear its source to
         *  reset this bit to 1'b0 When this bit is high, the interrupt signal from the
         *  DWC_gmac subsystem _sbd_intr_o_ is high This bit is applicable only when the IEEE
         *  1588 Timestamp feature is enabled Otherwise, this bit is reserved
         */
        uint32_t ch2_tti:1;
        /** ch2_glpii_gtmsi : RO; bitpos: [30]; default: 0;
         *  GTMSI: GMAC TMS Interrupt _for Channel 1 and Channel 2_ This bit indicates an
         *  interrupt event in the traffic manager and scheduler logic of DWC_gmac To reset
         *  this bit, the software must read the corresponding registers _Channel Status
         *  Register_ to get the exact cause of the interrupt and clear its source Note: GTMSI
         *  status is given only in Channel 1 and Channel 2 DMA register when the AV feature is
         *  enabled and corresponding additional transmit channels are present Otherwise, this
         *  bit is reserved When this bit is high, the interrupt signal from the MAC
         *  _sbd_intr_o_ is high
         */
        uint32_t ch2_glpii_gtmsi:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} gmac_register133_channel2statusregister_reg_t;

/** Type of register134_channel2operationmoderegister register
 *  Establishes the Receive and Transmit operating modes and command
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** ch2_sr : R/W; bitpos: [1]; default: 0;
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
        uint32_t ch2_sr:1;
        /** ch2_osf : R/W; bitpos: [2]; default: 0;
         *  Operate on Second Frame When this bit is set, it instructs the DMA to process the
         *  second frame of the Transmit data even before the status for the first frame is
         *  obtained
         */
        uint32_t ch2_osf:1;
        /** ch2_rtc : R/W; bitpos: [4:3]; default: 0;
         *  Receive Threshold Control These two bits control the threshold level of the MTL
         *  Receive FIFO Transfer _request_ to DMA starts when the frame size within the MTL
         *  Receive FIFO is larger than the threshold In addition, full frames with length less
         *  than the threshold are automatically transferred The value of 11 is not applicable
         *  if the configured Receive FIFO size is 128 bytes These bits are valid only when the
         *  RSF bit is zero, and are ignored when the RSF bit is set to 1 00: 64 01: 32 10: 96
         *  11: 128
         */
        uint32_t ch2_rtc:2;
        /** ch2_dgf : R/W; bitpos: [5]; default: 0;
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
        uint32_t ch2_dgf:1;
        /** ch2_fuf : R/W; bitpos: [6]; default: 0;
         *  Forward Undersized Good Frames When set, the Rx FIFO forwards Undersized frames
         *  _that is, frames with no Error and length less than 64 bytes_ including padbytes
         *  and CRC When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a
         *  frame is already transferred because of the lower value of Receive Threshold, for
         *  example, RTC = 01
         */
        uint32_t ch2_fuf:1;
        /** ch2_fef : R/W; bitpos: [7]; default: 0;
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
        uint32_t ch2_fef:1;
        /** ch2_efc : R/W; bitpos: [8]; default: 0;
         *  Enable HW Flow Control When this bit is set, the flow control signal operation
         *  based on the filllevel of Rx FIFO is enabled When reset, the flow control operation
         *  is disabled This bit is not used _reserved and always reset_ when the Rx FIFO is
         *  less than 4 KB
         */
        uint32_t ch2_efc:1;
        /** ch2_rfa : R/W; bitpos: [10:9]; default: 0;
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
        uint32_t ch2_rfa:2;
        /** ch2_rfd : R/W; bitpos: [12:11]; default: 0;
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
        uint32_t ch2_rfd:2;
        /** ch2_st : R/W; bitpos: [13]; default: 0;
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
        uint32_t ch2_st:1;
        /** ch2_ttc : R/W; bitpos: [16:14]; default: 0;
         *  Transmit Threshold Control These bits control the threshold level of the MTL
         *  Transmit FIFO Transmission starts when the frame size within the MTL Transmit FIFO
         *  is larger than the threshold In addition, full frames with a length less than the
         *  threshold are also transmitted These bits are used only when Bit 21 _TSF_ is reset
         *  000: 64 001: 128 010: 192 011: 256 100: 40 101: 32 110: 24 111: 16
         */
        uint32_t ch2_ttc:3;
        uint32_t reserved_17:3;
        /** ch2_ftf : R/W; bitpos: [20]; default: 0;
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
        uint32_t ch2_ftf:1;
        /** ch2_tsf : R/W; bitpos: [21]; default: 0;
         *  Transmit Store and Forward When this bit is set, transmission starts when a full
         *  frame resides in the MTL Transmit FIFO When this bit is set, the TTC values
         *  specified in Bits [16:14] are ignored This bit should be changed only when the
         *  transmission is stopped
         */
        uint32_t ch2_tsf:1;
        /** ch2_rfd_2 : R/W; bitpos: [22]; default: 0;
         *  MSB of Threshold for Deactivating Flow Control If the DWC_gmac is configured for Rx
         *  FIFO size of 8 KB or more, this bit _when set_ provides additional threshold levels
         *  for deactivating the flow control in both halfduplex and fullduplex modes This bit
         *  _as Most Significant Bit_ along with the RFD _Bits [12:11]_ gives the following
         *  thresholds for deactivating flow control: 100: Full minus 5 KB, that is, FULL — 5
         *  KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that is, FULL —
         *  7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB or less deep
         */
        uint32_t ch2_rfd_2:1;
        /** ch2_rfa_2 : R/W; bitpos: [23]; default: 0;
         *  MSB of Threshold for Activating Flow Control If the DWC_gmac is configured for an
         *  Rx FIFO size of 8 KB or more, this bit _when set_ provides additional threshold
         *  levels for activating the flow control in both half duplex and fullduplex modes
         *  This bit _as Most Significant Bit_, along with the RFA _Bits [10:9]_, gives the
         *  following thresholds for activating flow control: 100: Full minus 5 KB, that is,
         *  FULL — 5 KB 101: Full minus 6 KB, that is, FULL — 6 KB 110: Full minus 7 KB, that
         *  is, FULL — 7 KB 111: Reserved This bit is reserved _and RO_ if the Rx FIFO is 4 KB
         *  or less deep
         */
        uint32_t ch2_rfa_2:1;
        /** ch2_dff : R/W; bitpos: [24]; default: 0;
         *  Disable Flushing of Received Frames When this bit is set, the Rx DMA does not flush
         *  any frames because of the unavailability of receive descriptors or buffers as it
         *  does normally when this bit is reset _See “Receive Process Suspended” on page 83_
         *  This bit is reserved _and RO_ in the GMACMTL configuration
         */
        uint32_t ch2_dff:1;
        /** ch2_rsf : R/W; bitpos: [25]; default: 0;
         *  Receive Store and Forward When this bit is set, the MTL reads a frame from the Rx
         *  FIFO only after the complete frame has been written to it, ignoring the RTC bits
         *  When this bit is reset, the Rx FIFO operates in the cutthrough mode, subject to the
         *  threshold specified by the RTC bits
         */
        uint32_t ch2_rsf:1;
        /** ch2_dt : R/W; bitpos: [26]; default: 0;
         *  Disable Dropping of TCP/IP Checksum Error Frames When this bit is set, the MAC does
         *  not drop the frames which only have errors detected by the Receive Checksum Offload
         *  engine Such frames do not have any errors _including FCS error_ in the Ethernet
         *  frame received by the MAC but have errors only in the encapsulated payload When
         *  this bit is reset, all error frames are dropped if the FEF bit is reset If the IPC
         *  Full Checksum Offload Engine _Type 2_ is disabled, this bit is reserved _RO with
         *  value 1'b0_
         */
        uint32_t ch2_dt:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} gmac_register134_channel2operationmoderegister_reg_t;

/** Type of register135_channel2interruptenableregister register
 *  Enables the interrupts reported by the Status Register
 */
typedef union {
    struct {
        /** ch2_tie : R/W; bitpos: [0]; default: 0;
         *  Transmit Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
         *  _Bit 16_, the Transmit Interrupt is enabled When this bit is reset, the Transmit
         *  Interrupt is disabled The sbd_intr_o interrupt is generated as shown in Figure 61
         *  It is asserted only when the TTI, GPI, GMI, GLI, or GLPII bit of the DMA Status
         *  register is asserted, or when the NIS or AIS Status bit is asserted and the
         *  corresponding Interrupt Enable bits _NIE or AIE_ are enabled
         */
        uint32_t ch2_tie:1;
        /** ch2_tse : R/W; bitpos: [1]; default: 0;
         *  Transmit Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
         *  _Bit 15_, the Transmission Stopped Interrupt is enabled When this bit is reset, the
         *  Transmission Stopped Interrupt is disabled
         */
        uint32_t ch2_tse:1;
        /** ch2_tue : R/W; bitpos: [2]; default: 0;
         *  Transmit Buffer Unavailable Enable When this bit is set with Normal Interrupt
         *  Summary Enable _Bit 16_, the Transmit Buffer Unavailable Interrupt is enabled When
         *  this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled
         */
        uint32_t ch2_tue:1;
        /** ch2_tje : R/W; bitpos: [3]; default: 0;
         *  Transmit Jabber Timeout Enable When this bit is set with Abnormal Interrupt Summary
         *  Enable _Bit 15_, the Transmit Jabber Timeout Interrupt is enabled When this bit is
         *  reset, the Transmit Jabber Timeout Interrupt is disabled
         */
        uint32_t ch2_tje:1;
        /** ch2_ove : R/W; bitpos: [4]; default: 0;
         *  Overflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
         *  Enable _Bit 15_, the Receive Overflow Interrupt is enabled When this bit is reset,
         *  the Overflow Interrupt is disabled
         */
        uint32_t ch2_ove:1;
        /** ch2_une : R/W; bitpos: [5]; default: 0;
         *  Underflow Interrupt Enable When this bit is set with Abnormal Interrupt Summary
         *  Enable _Bit 15_, the Transmit Underflow Interrupt is enabled When this bit is
         *  reset, the Underflow Interrupt is disabled
         */
        uint32_t ch2_une:1;
        /** ch2_rie : R/W; bitpos: [6]; default: 0;
         *  Receive Interrupt Enable When this bit is set with Normal Interrupt Summary Enable
         *  _Bit 16_, the Receive Interrupt is enabled When this bit is reset, the Receive
         *  Interrupt is disabled
         */
        uint32_t ch2_rie:1;
        /** ch2_rue : R/W; bitpos: [7]; default: 0;
         *  Receive Buffer Unavailable Enable When this bit is set with Abnormal Interrupt
         *  Summary Enable _Bit 15_, the Receive Buffer Unavailable Interrupt is enabled When
         *  this bit is reset, the Receive Buffer Unavailable Interrupt is disabled
         */
        uint32_t ch2_rue:1;
        /** ch2_rse : R/W; bitpos: [8]; default: 0;
         *  Receive Stopped Enable When this bit is set with Abnormal Interrupt Summary Enable
         *  _Bit 15_, the Receive Stopped Interrupt is enabled When this bit is reset, the
         *  Receive Stopped Interrupt is disabled
         */
        uint32_t ch2_rse:1;
        /** ch2_rwe : R/W; bitpos: [9]; default: 0;
         *  Receive Watchdog Timeout Enable When this bit is set with Abnormal Interrupt
         *  Summary Enable _Bit 15_, the Receive Watchdog Timeout Interrupt is enabled When
         *  this bit is reset, the Receive Watchdog Timeout Interrupt is disabled
         */
        uint32_t ch2_rwe:1;
        /** ch2_ete : R/W; bitpos: [10]; default: 0;
         *  Early Transmit Interrupt Enable When this bit is set with an Abnormal Interrupt
         *  Summary Enable _Bit 15_, the Early Transmit Interrupt is enabled When this bit is
         *  reset, the Early Transmit Interrupt is disabled
         */
        uint32_t ch2_ete:1;
        uint32_t reserved_11:2;
        /** ch2_fbe : R/W; bitpos: [13]; default: 0;
         *  Fatal Bus Error Enable When this bit is set with Abnormal Interrupt Summary Enable
         *  _Bit 15_, the Fatal Bus Error Interrupt is enabled When this bit is reset, the
         *  Fatal Bus Error Enable Interrupt is disabled 12:11		Reserved	00	RO
         */
        uint32_t ch2_fbe:1;
        /** ch2_ere : R/W; bitpos: [14]; default: 0;
         *  Early Receive Interrupt Enable When this bit is set with Normal Interrupt Summary
         *  Enable _Bit 16_, the Early Receive Interrupt is enabled When this bit is reset, the
         *  Early Receive Interrupt is disabled
         */
        uint32_t ch2_ere:1;
        /** ch2_aie : R/W; bitpos: [15]; default: 0;
         *  Abnormal Interrupt Summary Enable When this bit is set, abnormal interrupt summary
         *  is enabled When this bit is reset, the abnormal interrupt summary is disabled This
         *  bit enables the following interrupts in Register 5 _Status Register_: Register
         *  5[1]: Transmit Process Stopped Register 5[3]: Transmit Jabber Timeout Register
         *  5[4]: Receive Overflow Register 5[5]: Transmit Underflow Register 5[7]: Receive
         *  Buffer Unavailable Register 5[8]: Receive Process Stopped Register 5[9]: Receive
         *  Watchdog Timeout Register 5[10]: Early Transmit Interrupt Register 5[13]: Fatal Bus
         *  Error
         */
        uint32_t ch2_aie:1;
        /** ch2_nie : R/W; bitpos: [16]; default: 0;
         *  Normal Interrupt Summary Enable When this bit is set, normal interrupt summary is
         *  enabled When this bit is reset, normal interrupt summary is disabled This bit
         *  enables the following interrupts in Register 5 _Status Register_: Register 5[0]:
         *  Transmit Interrupt Register 5[2]: Transmit Buffer Unavailable Register 5[6]:
         *  Receive Interrupt Register 5[14]: Early Receive Interrupt
         */
        uint32_t ch2_nie:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} gmac_register135_channel2interruptenableregister_reg_t;

/** Type of register136_channel2missedframeandbufferoverflowcounterregister register
 *  Contains the counters for discarded frames because no host Receive Descriptor was
 *  available or because of Receive FIFO Overflow
 */
typedef union {
    struct {
        /** ch2_misfrmcnt : R/W; bitpos: [15:0]; default: 0;
         *  Missed Frame Counter This field indicates the number of frames missed by the
         *  controller because of the Host Receive Buffer being unavailable This counter is
         *  incremented each time the DMA discards an incoming frame The counter is cleared
         *  when this register is read with mci_be_i[0] at 1’b1
         */
        uint32_t ch2_misfrmcnt:16;
        /** ch2_miscntovf : R/W; bitpos: [16]; default: 0;
         *  Overflow Bit for Missed Frame Counter This bit is set every time Missed Frame
         *  Counter _Bits[15:0]_ overflows, that is, the DMA discards an incoming frame because
         *  of the Host Receive Buffer being unavailable with the missed frame counter at
         *  maximum value In such a scenario, the Missed frame counter is reset to allzeros and
         *  this bit indicates that the rollover happened
         */
        uint32_t ch2_miscntovf:1;
        /** ch2_ovffrmcnt : R/W; bitpos: [27:17]; default: 0;
         *  Overflow Frame Counter This field indicates the number of frames missed by the
         *  application This counter is incremented each time the MTL FIFO overflows The
         *  counter is cleared when this register is read with mci_be_i[2] at 1’b1
         */
        uint32_t ch2_ovffrmcnt:11;
        /** ch2_ovfcntovf : R/W; bitpos: [28]; default: 0;
         *  Overflow Bit for FIFO Overflow Counter This bit is set every time the Overflow
         *  Frame Counter _Bits[27:17]_ overflows, that is, the Rx FIFO overflows with the
         *  overflow frame counter at maximum value In such a scenario, the overflow frame
         *  counter is reset to allzeros and this bit indicates that the rollover happened
         */
        uint32_t ch2_ovfcntovf:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} gmac_register136_channel2missedframeandbufferoverflowcounterregister_reg_t;

/** Type of register137_channel2receiveinterruptwatchdogtimerregister register
 *  Watchdog timeout for Receive Interrupt _RI_ from DMA
 */
typedef union {
    struct {
        /** ch2_riwt : R/W; bitpos: [7:0]; default: 0;
         *  RI Watchdog Timer Count This bit indicates the number of system clock cycles
         *  multiplied by 256 for which the watchdog timer is set The watchdog timer gets
         *  triggered with the programmed value after the Rx DMA completes the transfer of a
         *  frame for which the RI status bit is not set because of the setting in the
         *  corresponding descriptor RDES1[31] When the watchdog timer runs out, the RI bit is
         *  set and the timer is stopped The watchdog timer is reset when the RI bit is set
         *  high because of automatic setting of RI as per RDES1[31] of any received frame
         */
        uint32_t ch2_riwt:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gmac_register137_channel2receiveinterruptwatchdogtimerregister_reg_t;

/** Type of register146_channel2currenthosttransmitdescriptorregister register
 *  Points to the start of current Transmit Descriptor read by the DMA
 */
typedef union {
    struct {
        /** ch2_curtdesaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Transmit Descriptor Address Pointer
         */
        uint32_t ch2_curtdesaptr:32;
    };
    uint32_t val;
} gmac_register146_channel2currenthosttransmitdescriptorregister_reg_t;

/** Type of register147_channel2currenthostreceivedescriptorregister register
 *  Points to the start of current Receive Descriptor read by the DMA
 */
typedef union {
    struct {
        /** ch2_currdesaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Receive Descriptor Address Pointer
         */
        uint32_t ch2_currdesaptr:32;
    };
    uint32_t val;
} gmac_register147_channel2currenthostreceivedescriptorregister_reg_t;

/** Type of register148_channel2currenthosttransmitbufferaddressregister register
 *  Points to the current Transmit Buffer address read by the DMA
 */
typedef union {
    struct {
        /** ch2_curtbufaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Transmit Buffer Address Pointer
         */
        uint32_t ch2_curtbufaptr:32;
    };
    uint32_t val;
} gmac_register148_channel2currenthosttransmitbufferaddressregister_reg_t;

/** Type of register149_channel2currenthostreceivebufferaddressregister register
 *  Points to the current Receive Buffer address read by the DMA
 */
typedef union {
    struct {
        /** ch2_currbufaptr : RO; bitpos: [31:0]; default: 0;
         *  Host Receive Buffer Address Pointer
         */
        uint32_t ch2_currbufaptr:32;
    };
    uint32_t val;
} gmac_register149_channel2currenthostreceivebufferaddressregister_reg_t;

/** Type of register152_channel2cbscontrolregister register
 *  Controls the Channel 2 credit shaping operation on the transmit path
 */
typedef union {
    struct {
        /** ch2_cbsd : R/W; bitpos: [0]; default: 0;
         *  CreditBased Shaper Disable When set, the MAC disables the creditbased shaper
         *  algorithm for Channel 2 traffic and makes the traffic management algorithm to
         *  strict priority for Channel 2 over Channel 0 When reset, the creditbased shaper
         *  algorithm schedules the traffic in Channel 2 for transmission
         */
        uint32_t ch2_cbsd:1;
        /** ch2_cc : R/W; bitpos: [1]; default: 0;
         *  Credit Control When reset, the accumulated credit parameter in the creditbased
         *  shaper algorithm logic is set to zero when there is positive credit and no frame to
         *  transmit in Channel 2 When there is no frame waiting in Channel 2 and other channel
         *  is transmitting, no credit is accumulated When set, the accumulated credit
         *  parameter in the creditbased shaper algorithm logic is not reset to zero when there
         *  is positive credit and no frame to transmit in Channel 2 The credit accumulates
         *  even when there is no frame waiting in Channel 2 and another channel is transmitting
         */
        uint32_t ch2_cc:1;
        uint32_t reserved_2:2;
        /** ch2_slc : R/W; bitpos: [6:4]; default: 0;
         *  Slot Count The software can program the number of slots _of duration 125 microsec_
         *  over which the average transmitted bits per slot _provided in the CBS Status
         *  register_ need to be computed for Channel 2 when the creditbased shaper algorithm
         *  is enabled The encoding is as follows: 3'b000: 1 Slot 3'b001: 2 Slots 3'b010: 4
         *  Slots 3'b011: 8 Slots 3'b100: 16 Slots 3'b1013'b111: Reserved
         */
        uint32_t ch2_slc:3;
        uint32_t reserved_7:10;
        /** ch2_abpssie : R/W; bitpos: [17]; default: 0;
         *  Average Bits Per Slot Interrupt Enable When this bit is set, the MAC asserts an
         *  interrupt _sbd_intr_o or mci_intr_o_ when the average bits per slot status is
         *  updated _Bit 17 _ABSU_ in Register 153_ for Channel 2 When this bit is cleared,
         *  interrupt is not asserted for such an event
         */
        uint32_t ch2_abpssie:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gmac_register152_channel2cbscontrolregister_reg_t;

/** Type of register153_channel2cbsstatusregister register
 *  Provides the average traffic transmitted in Channel 2
 */
typedef union {
    struct {
        /** ch2_abs : RO; bitpos: [16:0]; default: 0;
         *  Average Bits per Slot This field contains the average transmitted bits per slot
         *  This field is computed over programmed number of slots _SLC bits in the CBS Control
         *  Register_ for Channel 2 traffic The maximum value is 0x30D4 for 100 Mbps and
         *  0x1E848 for 1000 Mbps
         */
        uint32_t ch2_abs:17;
        /** ch2_absu : RO; bitpos: [17]; default: 0;
         *  ABS Updated When set, this bit indicates that the MAC has updated the ABS value
         *  This bit is cleared when the application reads the ABS value
         */
        uint32_t ch2_absu:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gmac_register153_channel2cbsstatusregister_reg_t;

/** Type of register154_channel2idleslopecreditregister register
 *  Contains the idleSlope credit value required for the creditbased shaper algorithm
 *  for Channel 2
 */
typedef union {
    struct {
        /** ch2_isc : R/W; bitpos: [13:0]; default: 0;
         *  idleSlopeCredit This field contains the idleSlopeCredit value required for the
         *  creditbased shaper algorithm for Channel 2 This is the rate of change of credit in
         *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
         *  credit is increasing The software should program this field with computed credit in
         *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
         *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode
         */
        uint32_t ch2_isc:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} gmac_register154_channel2idleslopecreditregister_reg_t;

/** Type of register155_channel2sendslopecreditregister register
 *  Contains the sendSlope credit value required for the creditbased shaper algorithm
 *  for Channel 2
 */
typedef union {
    struct {
        /** ch2_ssc : R/W; bitpos: [13:0]; default: 0;
         *  sendSlopeCredit This field contains the sendSlopeCredit value required for
         *  creditbased shaper algorithm for Channel 2 This is the rate of change of credit in
         *  bits per cycle _40ns and 8ns for 100 Mbps and 1000 Mbps respectively_ when the
         *  credit is decreasing The software should program this field with computed credit in
         *  bits per cycle scaled by 1024 The maximum value is portTransmitRate, that is,
         *  0x2000 in 1000 Mbps mode and 0x1000 in 100 Mbps mode This field should be
         *  programmed with absolute sendSlopeCredit value The creditbased shaper logic
         *  subtracts it from the accumulated credit when Channel 2 is selected for transmission
         */
        uint32_t ch2_ssc:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} gmac_register155_channel2sendslopecreditregister_reg_t;

/** Type of register156_channel2hicreditregister register
 *  Contains the hiCredit value required for the creditbased shaper algorithm for
 *  Channel 2
 */
typedef union {
    struct {
        /** ch2_hc : R/W; bitpos: [28:0]; default: 0;
         *  hiCredit This field contains the hiCredit value required for the creditbased shaper
         *  algorithm for Channel 2 This is the maximum value that can be accumulated in the
         *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
         *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
         *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
         *  0x0800_0000
         */
        uint32_t ch2_hc:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} gmac_register156_channel2hicreditregister_reg_t;

/** Type of register157_channel2locreditregister register
 *  Contains the loCredit value required for the creditbased shaper algorithm for
 *  Channel 2
 */
typedef union {
    struct {
        /** ch2_lc : R/W; bitpos: [28:0]; default: 536870911;
         *  loCredit This field contains the loCredit value required for the creditbased shaper
         *  algorithm for Channel 2 This is the minimum value that can be accumulated in the
         *  credit parameter This is specified in bits scaled by 1,024 The maximum value is
         *  maxInterferenceSize, that is, besteffort maximum frame size which is 16,384 bytes
         *  or 131,072 bits The value to be specified is 131,072 * 1,024 = 134,217,728 or
         *  0x0800_0000 The programmed value is 2's complement _negative number_, that is,
         *  0xF800_0000
         */
        uint32_t ch2_lc:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} gmac_register157_channel2locreditregister_reg_t;


typedef struct {
    volatile gmac_register0_macconfigurationregister_reg_t register0_macconfigurationregister;
    volatile gmac_register1_macframefilter_reg_t register1_macframefilter;
    volatile gmac_register2_hashtablehighregister_reg_t register2_hashtablehighregister;
    volatile gmac_register3_hashtablelowregister_reg_t register3_hashtablelowregister;
    volatile gmac_register4_gmiiaddressregister_reg_t register4_gmiiaddressregister;
    volatile gmac_register5_gmiidataregister_reg_t register5_gmiidataregister;
    volatile gmac_register6_flowcontrolregister_reg_t register6_flowcontrolregister;
    volatile gmac_register7_vlantagregister_reg_t register7_vlantagregister;
    volatile gmac_register8_versionregister_reg_t register8_versionregister;
    volatile gmac_register9_debugregister_reg_t register9_debugregister;
    volatile gmac_register10_remotewakeupframefilterregister_reg_t register10_remotewakeupframefilterregister;
    volatile gmac_register11_pmtcontrolandstatusregister_reg_t register11_pmtcontrolandstatusregister;
    volatile gmac_register12_lpicontrolandstatusregister_reg_t register12_lpicontrolandstatusregister;
    volatile gmac_register13_lpitimerscontrolregister_reg_t register13_lpitimerscontrolregister;
    volatile gmac_register14_interruptstatusregister_reg_t register14_interruptstatusregister;
    volatile gmac_register15_interruptmaskregister_reg_t register15_interruptmaskregister;
    volatile gmac_register16_macaddress0highregister_reg_t register16_macaddress0highregister;
    volatile gmac_register17_macaddress0lowregister_reg_t register17_macaddress0lowregister;
    volatile gmac_register18_macaddress1highregister_reg_t register18_macaddress1highregister;
    volatile gmac_register19_macaddress1lowregister_reg_t register19_macaddress1lowregister;
    volatile gmac_macaddress2highregistercontainsthehigher16bitsofthethirdmacaddress_reg_t macaddress2highregistercontainsthehigher16bitsofthethirdmacaddress;
    volatile gmac_macaddress2lowregistercontainsthelower32bitsofthethirdmacaddress_reg_t macaddress2lowregistercontainsthelower32bitsofthethirdmacaddress;
    volatile gmac_macaddress3highregistercontainsthehigher16bitsofthefourthmacaddress_reg_t macaddress3highregistercontainsthehigher16bitsofthefourthmacaddress;
    volatile gmac_macaddress3lowregistercontainsthelower32bitsofthefourthmacaddress_reg_t macaddress3lowregistercontainsthelower32bitsofthefourthmacaddress;
    volatile gmac_macaddress4highregistercontainsthehigher16bitsofthefifthmacaddress_reg_t macaddress4highregistercontainsthehigher16bitsofthefifthmacaddress;
    volatile gmac_macaddress4lowregistercontainsthelower32bitsofthefifthmacaddress_reg_t macaddress4lowregistercontainsthelower32bitsofthefifthmacaddress;
    volatile gmac_macaddress5highregistercontainsthehigher16bitsofthesixthmacaddress_reg_t macaddress5highregistercontainsthehigher16bitsofthesixthmacaddress;
    volatile gmac_macaddress5lowregistercontainsthelower32bitsofthesixthmacaddress_reg_t macaddress5lowregistercontainsthelower32bitsofthesixthmacaddress;
    volatile gmac_macaddress6highregistercontainsthehigher16bitsoftheseventhmacaddress_reg_t macaddress6highregistercontainsthehigher16bitsoftheseventhmacaddress;
    volatile gmac_macaddress6lowregistercontainsthelower32bitsoftheseventhmacaddress_reg_t macaddress6lowregistercontainsthelower32bitsoftheseventhmacaddress;
    volatile gmac_macaddress7highregistercontainsthehigher16bitsoftheeighthmacaddress_reg_t macaddress7highregistercontainsthehigher16bitsoftheeighthmacaddress;
    volatile gmac_macaddress7lowregistercontainsthelower32bitsoftheeighthmacaddress_reg_t macaddress7lowregistercontainsthelower32bitsoftheeighthmacaddress;
    uint32_t reserved_080[16];
    volatile gmac_register48_ancontrolregister_reg_t register48_ancontrolregister;
    volatile gmac_register49_anstatusregister_reg_t register49_anstatusregister;
    volatile gmac_register50_autonegotiationadvertisementregister_reg_t register50_autonegotiationadvertisementregister;
    volatile gmac_register51_autonegotiationlinkpartnerabilityregister_reg_t register51_autonegotiationlinkpartnerabilityregister;
    volatile gmac_register52_autonegotiationexpansionregister_reg_t register52_autonegotiationexpansionregister;
    volatile gmac_register53_tbiextendedstatusregister_reg_t register53_tbiextendedstatusregister;
    volatile gmac_register54_sgmii_rgmii_smiicontrolandstatusregister_reg_t register54_sgmii_rgmii_smiicontrolandstatusregister;
    volatile gmac_register55_watchdogtimeoutregister_reg_t register55_watchdogtimeoutregister;
    volatile gmac_register56_generalpurposeioregister_reg_t register56_generalpurposeioregister;
    uint32_t reserved_0e4[199];
    volatile gmac_register256_layer3andlayer4controlregister0_reg_t register256_layer3andlayer4controlregister0;
    volatile gmac_register257_layer4addressregister0_reg_t register257_layer4addressregister0;
    uint32_t reserved_408[2];
    volatile gmac_register260_layer3address0register0_reg_t register260_layer3address0register0;
    volatile gmac_register261_layer3address1register0_reg_t register261_layer3address1register0;
    volatile gmac_register262_layer3address2register0_reg_t register262_layer3address2register0;
    volatile gmac_register263_layer3address3register0_reg_t register263_layer3address3register0;
    uint32_t reserved_420[56];
    volatile gmac_register320_hashtableregister0_reg_t register320_hashtableregister0;
    uint32_t reserved_504[32];
    volatile gmac_register353_vlantaginclusionorreplacementregister_reg_t register353_vlantaginclusionorreplacementregister;
    volatile gmac_register354_vlanhashtableregister_reg_t register354_vlanhashtableregister;
    uint32_t reserved_58c[93];
    volatile gmac_register448_timestampcontrolregister_reg_t register448_timestampcontrolregister;
    volatile gmac_register449_subsecondincrementregister_reg_t register449_subsecondincrementregister;
    volatile gmac_register450_systemtimesecondsregister_reg_t register450_systemtimesecondsregister;
    volatile gmac_register451_systemtimenanosecondsregister_reg_t register451_systemtimenanosecondsregister;
    volatile gmac_register452_systemtimesecondsupdateregister_reg_t register452_systemtimesecondsupdateregister;
    volatile gmac_register453_systemtimenanosecondsupdateregister_reg_t register453_systemtimenanosecondsupdateregister;
    volatile gmac_register454_timestampaddendregister_reg_t register454_timestampaddendregister;
    volatile gmac_register455_targettimesecondsregister_reg_t register455_targettimesecondsregister;
    volatile gmac_register456_targettimenanosecondsregister_reg_t register456_targettimenanosecondsregister;
    volatile gmac_register457_systemtimehigherwordsecondsregister_reg_t register457_systemtimehigherwordsecondsregister;
    volatile gmac_register458_timestampstatusregister_reg_t register458_timestampstatusregister;
    volatile gmac_register459_ppscontrolregister_reg_t register459_ppscontrolregister;
    volatile gmac_register460_auxiliarytimestampnanosecondsregister_reg_t register460_auxiliarytimestampnanosecondsregister;
    volatile gmac_register461_auxiliarytimestampsecondsregister_reg_t register461_auxiliarytimestampsecondsregister;
    volatile gmac_register462_avmaccontrolregister_reg_t register462_avmaccontrolregister;
    uint32_t reserved_73c[9];
    volatile gmac_register472_pps0intervalregister_reg_t register472_pps0intervalregister;
    volatile gmac_register473_pps0widthregister_reg_t register473_pps0widthregister;
    uint32_t reserved_768[6];
    volatile gmac_register480_pps1targettimesecondsregister_reg_t register480_pps1targettimesecondsregister;
    volatile gmac_register481_pps1targettimenanosecondsregister_reg_t register481_pps1targettimenanosecondsregister;
    uint32_t reserved_788[62];
    volatile gmac_register544_macaddress32highregister_reg_t register544_macaddress32highregister;
    uint32_t reserved_884[479];
    volatile gmac_register0_busmoderegister_reg_t register0_busmoderegister;
    volatile gmac_register1_transmitpolldemandregister_reg_t register1_transmitpolldemandregister;
    volatile gmac_register2_receivepolldemandregister_reg_t register2_receivepolldemandregister;
    volatile gmac_register3_receivedescriptorlistaddressregister_reg_t register3_receivedescriptorlistaddressregister;
    volatile gmac_register4_transmitdescriptorlistaddressregister_reg_t register4_transmitdescriptorlistaddressregister;
    volatile gmac_register5_statusregister_reg_t register5_statusregister;
    volatile gmac_register6_operationmoderegister_reg_t register6_operationmoderegister;
    volatile gmac_register7_interruptenableregister_reg_t register7_interruptenableregister;
    volatile gmac_register8_missedframeandbufferoverflowcounterregister_reg_t register8_missedframeandbufferoverflowcounterregister;
    volatile gmac_register9_receiveinterruptwatchdogtimerregister_reg_t register9_receiveinterruptwatchdogtimerregister;
    volatile gmac_register10_axibusmoderegister_reg_t register10_axibusmoderegister;
    volatile gmac_register11_ahboraxistatusregister_reg_t register11_ahboraxistatusregister;
    uint32_t reserved_1030[6];
    volatile gmac_register18_currenthosttransmitdescriptorregister_reg_t register18_currenthosttransmitdescriptorregister;
    volatile gmac_register19_currenthostreceivedescriptorregister_reg_t register19_currenthostreceivedescriptorregister;
    volatile gmac_register20_currenthosttransmitbufferaddressregister_reg_t register20_currenthosttransmitbufferaddressregister;
    volatile gmac_register21_currenthostreceivebufferaddressregister_reg_t register21_currenthostreceivebufferaddressregister;
    volatile gmac_register22_hwfeatureregister_reg_t register22_hwfeatureregister;
    uint32_t reserved_105c[41];
    volatile gmac_register64_channel1busmoderegister_reg_t register64_channel1busmoderegister;
    volatile gmac_register65_channel1transmitpolldemandregister_reg_t register65_channel1transmitpolldemandregister;
    volatile gmac_register66_channel1receivepolldemandregister_reg_t register66_channel1receivepolldemandregister;
    volatile gmac_register67_channel1receivedescriptorlistaddressregister_reg_t register67_channel1receivedescriptorlistaddressregister;
    volatile gmac_register68_channel1transmitdescriptorlistaddressregister_reg_t register68_channel1transmitdescriptorlistaddressregister;
    volatile gmac_register69_channel1statusregister_reg_t register69_channel1statusregister;
    volatile gmac_register70_channel1operationmoderegister_reg_t register70_channel1operationmoderegister;
    volatile gmac_register71_channel1interruptenableregister_reg_t register71_channel1interruptenableregister;
    volatile gmac_register72_channel1missedframeandbufferoverflowcounterregister_reg_t register72_channel1missedframeandbufferoverflowcounterregister;
    volatile gmac_register73_channel1receiveinterruptwatchdogtimerregister_reg_t register73_channel1receiveinterruptwatchdogtimerregister;
    uint32_t reserved_1128[2];
    volatile gmac_register76_channel1slotfunctioncontrolandstatusregister_reg_t register76_channel1slotfunctioncontrolandstatusregister;
    uint32_t reserved_1134[5];
    volatile gmac_register82_channel1currenthosttransmitdescriptorregister_reg_t register82_channel1currenthosttransmitdescriptorregister;
    volatile gmac_register83_channel1currenthostreceivedescriptorregister_reg_t register83_channel1currenthostreceivedescriptorregister;
    volatile gmac_register84_channel1currenthosttransmitbufferaddressregister_reg_t register84_channel1currenthosttransmitbufferaddressregister;
    volatile gmac_register85_channel1currenthostreceivebufferaddressregister_reg_t register85_channel1currenthostreceivebufferaddressregister;
    uint32_t reserved_1158[2];
    volatile gmac_register88_channel1cbscontrolregister_reg_t register88_channel1cbscontrolregister;
    volatile gmac_register89_channel1cbsstatusregister_reg_t register89_channel1cbsstatusregister;
    volatile gmac_register90_channel1idleslopecreditregister_reg_t register90_channel1idleslopecreditregister;
    volatile gmac_register91_channel1sendslopecreditregister_reg_t register91_channel1sendslopecreditregister;
    volatile gmac_register92_channel1hicreditregister_reg_t register92_channel1hicreditregister;
    volatile gmac_register93_channel1locreditregister_reg_t register93_channel1locreditregister;
    uint32_t reserved_1178[34];
    volatile gmac_register128_channel2busmoderegister_reg_t register128_channel2busmoderegister;
    volatile gmac_register129_channel2transmitpolldemandregister_reg_t register129_channel2transmitpolldemandregister;
    volatile gmac_register130_channel2receivepolldemandregister_reg_t register130_channel2receivepolldemandregister;
    volatile gmac_register131_channel2receivedescriptorlistaddressregister_reg_t register131_channel2receivedescriptorlistaddressregister;
    volatile gmac_register132_channel2transmitdescriptorlistaddressregister_reg_t register132_channel2transmitdescriptorlistaddressregister;
    volatile gmac_register133_channel2statusregister_reg_t register133_channel2statusregister;
    volatile gmac_register134_channel2operationmoderegister_reg_t register134_channel2operationmoderegister;
    volatile gmac_register135_channel2interruptenableregister_reg_t register135_channel2interruptenableregister;
    volatile gmac_register136_channel2missedframeandbufferoverflowcounterregister_reg_t register136_channel2missedframeandbufferoverflowcounterregister;
    volatile gmac_register137_channel2receiveinterruptwatchdogtimerregister_reg_t register137_channel2receiveinterruptwatchdogtimerregister;
    uint32_t reserved_1228[8];
    volatile gmac_register146_channel2currenthosttransmitdescriptorregister_reg_t register146_channel2currenthosttransmitdescriptorregister;
    volatile gmac_register147_channel2currenthostreceivedescriptorregister_reg_t register147_channel2currenthostreceivedescriptorregister;
    volatile gmac_register148_channel2currenthosttransmitbufferaddressregister_reg_t register148_channel2currenthosttransmitbufferaddressregister;
    volatile gmac_register149_channel2currenthostreceivebufferaddressregister_reg_t register149_channel2currenthostreceivebufferaddressregister;
    uint32_t reserved_1258[2];
    volatile gmac_register152_channel2cbscontrolregister_reg_t register152_channel2cbscontrolregister;
    volatile gmac_register153_channel2cbsstatusregister_reg_t register153_channel2cbsstatusregister;
    volatile gmac_register154_channel2idleslopecreditregister_reg_t register154_channel2idleslopecreditregister;
    volatile gmac_register155_channel2sendslopecreditregister_reg_t register155_channel2sendslopecreditregister;
    volatile gmac_register156_channel2hicreditregister_reg_t register156_channel2hicreditregister;
    volatile gmac_register157_channel2locreditregister_reg_t register157_channel2locreditregister;
} gmac_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(gmac_dev_t) == 0x1278, "Invalid size of gmac_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
