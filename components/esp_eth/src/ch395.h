/*
 * SPDX-FileCopyrightText: 2024 Sergey Kharenko
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2023-2024 NanjingQinhengMicroelectronics CO LTD
 * SPDX-FileContributor: 2024 Sergey Kharenko
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Command Code
 * @note A command operation sequence includes:
 * A command code (for serial port mode, two synchronization codes are required before the command code),
 * Several input data (can be 0),
 * Several output data (can be 0)
 * @note Command code naming rules: CMDxy_NAME
 * Where x and y are both numbers, x indicates the minimum number of input data (number of bytes), y indicates
 * the minimum number of output data (number of bytes), and if y is W, it means that you need to wait for the
 * command to be executed successfully. Some commands can read and write data blocks of 0 to multiple bytes.
 * The number of bytes of the data block itself is not included in the above x or y
 * @{
*/

/**
 * @brief Get the chip and firmware version number
 * @return version number (bit 7 is 0, bit 6 is 1, bit 5~bit 0 are version
 * numbers)
 * @example The value of the version number returned by CH395 is 0x41, that is
 * the version number is 01H
 */
#define CMD01_GET_IC_VER                    0x01



/**
 * @brief Set the serial port communication baud rate ( the default baud rate after power on or reset is 9600bps)
 * @param baudrate(LSB) 3 bytes
 * @return
 *   - CMD_RET_SUCCESS - on success
 *   - others - indicate that the operation is not completed
 */
#define CMD31_SET_BAUDRATE                  0x02



/**
 * @brief Enter sleep state
*/
#define CMD00_ENTER_SLEEP                   0x03



/**
 * @brief Perform hardware reset
*/
#define CMD00_RESET_ALL                     0x05



/**
 * @brief Test communication interface and working status
 * @param Any
 * @return - bitwise inversion of input data
*/
#define CMD11_CHECK_EXIST                   0x06

/**
 * @brief Get the global interrupt status. Programs after version 0x44 need to
 * use this command to get all interrupts due to the increased number of
 * sockets
 * @return - global interrupt status
 */
#define CMD02_GET_GLOB_INT_STATUS_ALL       0x19


/**
 * @brief Set PHY, default is Auto-negotiate
 * @param param PHY parameter
 */
#define CMD10_SET_PHY                       0x20



/**
 * @brief Set MAC address
 * @warning Must be set before @ref CMD0W_INIT_CH395
 * @param address 6-byte MAC address
 */
#define CMD60_SET_MAC_ADDR                  0x21



/**
 * @brief Set IP address
 * @warning Must be set before @ref CMD0W_INIT_CH395
 * @param address 4-byte IP address
 */
#define CMD40_SET_IP_ADDR                   0x22



/**
 * @brief Set gateway IP address
 * @warning Must be set before @ref CMD0W_INIT_CH395
 * @param address 4-byte gateway IP address
 */
#define CMD40_SET_GWIP_ADDR                 0x23


/**
 * @brief Set subnet mask
 * @warning Must be set before @ref CMD0W_INIT_CH395
 * @param address 4-byte subnet mask
 */
#define CMD40_SET_MASK_ADDR                 0x24



/**
 * @brief Set MAC filtering to filter broadcast, multicast, etc.
 * @param filter_type
 * @param HASH0 4-byte hash table
 * @param HASH1 4-byte hash table
 */
#define CMD90_SET_MAC_FILT                  0x25



/**
 * @brief Get the current status of PHY, such as disconnected, 10/100M FULL/HALF
 * @return status Current PHY status
 */
#define CMD01_GET_PHY_STATUS                0x26



/**
 * @brief Initialize CH395
 * @warning The execution time of this command is about 200MS. You need to wait
 * for the successful execution of this command before you can send the next
 * command
 */
#define CMD0W_INIT_CH395                    0x27



/**
 * @brief Get unreachable information
 * @return type unreachable type
 * @return code unreachable protocol code
 * @return ip 4-byte unreachable IP
 */
#define CMD08_GET_UNREACH_IPPORT            0x28



/**
 * @brief Get the global interrupt status
 * @return status global interrupt status
 */
#define CMD01_GET_GLOB_INT_STATUS           0x29



/**
 * @brief Number of retries
 * @note Only valid in TCP mode
 * @param retries number of retries
 */
#define CMD10_SET_RETRAN_COUNT              0x2A



/**
 * @brief Retry period
 * @note Only valid in TCP mode. The maximum value is 20, cannot be set to 0
 * @param retries number of retries
 */
#define CMD20_SET_RETRAN_PERIOD             0x2B



/**
 * @brief Get command execution status
 * @return status command execution status
 */
#define CMD01_GET_CMD_STATUS                0x2C



/**
 * @brief Get the remote port and IP address.
 * @note Used in TCP server mode
 * @return address 4-byte remote IP address
 * @return port 2-byte remote port number
 */
#define CMD06_GET_REMOT_IPP_SN              0x2D



/**
 * @brief Clear the receive buffer
 * @param socknum index value of the socket
 */
#define CMD10_CLEAR_RECV_BUF_SN             0x2E



/**
 * @brief Get socket n status
 * @param socknum index value of the socket
 * @return status opened or closed
 * @return tcp_status TCP status, only valid in TCP mode and the status is open
 */
#define CMD12_GET_SOCKET_STATUS_SN          0x2F



/**
 * @brief Get the interrupt status of socket n
 * @param socknum index value of the socket
 * @return status socket interrupt status
 */
#define CMD11_GET_INT_STATUS_SN             0x30



/**
 * @brief Set the destination IP address of socket n
 * @param socknum index value of the socket
 * @param address 4-byte IP address
 */
#define CMD50_SET_IP_ADDR_SN                0x31



/**
 * @brief Set the destination port of socket n
 * @param socknum index value of the socket
 * @param port 2-byte destination port
 */
#define CMD30_SET_DES_PORT_SN               0x32



/**
 * @brief Set the source port of socket n
 * @param socknum index value of the socket
 * @param port 2-byte source port
 */
#define CMD30_SET_SOUR_PORT_SN              0x33



/**
 * @brief Set the protocol type of socket n
 * @param socknum index value of the socket
 * @param type protocol type
 */
#define CMD20_SET_PROTO_TYPE_SN             0x34



/**
 * @brief Open socket n
 * @note This command needs to wait for the command to be executed successfully
 * @param socknum index value of the socket
 */
#define CMD1W_OPEN_SOCKET_SN                0x35




/**
 * @brief Socket n listens
 * @note After receiving this command, socket n enters server mode, which is
 * only valid for TCP mode
 * @note This command needs to wait for the command to be executed successfully
 * @param socknum index value of the socket
 */
#define CMD1W_TCP_LISTEN_SN                 0x36



/**
 * @brief Socket n connection
 * @note After receiving this command, socket n enters client mode, which is
 * only valid for TCP mode
 * @note This command needs to wait for the command to be executed successfully
 * @param socknum index value of the socket
 */
#define CMD1W_TCP_CONNECT_SN                0x37




/**
 * @brief Socket n disconnects
 * @note After receiving this command, socket n disconnects the existing
 * connection, which is only valid for TCP mode
 * @note This command needs to wait for the command to be executed successfully
 * @param socknum index value of the socket
 */
#define CMD1W_TCP_DISNCONNECT_SN            0x38



/**
 * @brief Write data to socket n buffer
 * @param socknum index value of the socket
 * @param len 2-byte length
 */
#define CMD30_WRITE_SEND_BUF_SN             0x39



/**
 * @brief Get the length of data received by socket n
 * @param socknum index value of the socket
 * @return len 2-byte receiving length
 */
#define CMD12_GET_RECV_LEN_SN               0x3B



/**
 * @brief Read socket n receive buffer data
 * @param socknum index value of the socket
 * @param len(LSB) 2-byte read length
 * @return data
 */
#define CMD30_READ_RECV_BUF_SN              0x3C



/**
 * @brief Close socket n
 * @param socknum index value of the socket
 * @note This command needs to wait for the command to be executed successfully
 */
#define CMD1W_CLOSE_SOCKET_SN               0x3D



/**
 * @brief Set the IP packet protocol type of socket n
 * @note this command is only valid when the socket is in IP RAW mode
 * @param socknum index value of the socket
 * @param type IP RAW protocol type
 */
#define CMD20_SET_IPRAW_PRO_SN              0x3E



/**
 * @brief Turn on/off PING
 * @note it is turned on by default
 * @param enable 0 means to turn off PING, 1 means to turn on PING
 */
#define CMD10_PING_ENABLE                   0x3F



/**
 * @brief Get MAC address
 * @return address 6-byte MAC address
 */
#define CMD06_GET_MAC_ADDR                  0x40



/**
 * @brief Set DHCP enable
 * @param enable 1 starts DHCP, 0 turns off DHCP
 */
#define CMD10_DHCP_ENABLE                   0x41



/**
 * @brief Get DHCP status
 * @return
 *   - 0 success
 *   - others fail
 */
#define CMD01_GET_DHCP_STATUS               0x42



/**
 * @brief Get IP, subnet mask, gateway info
 * @return IP 4 bytes
 * @return gateway 4 bytes
 * @return mask 4 bytes
 * @return DNS1 4 bytes
 * @return DNS2 4 bytes
 */
#define CMD014_GET_IP_INF                   0x43




/**
 * @brief Set PPPOE user name
 * @param name using 0 to end
 */
#define CMD00_PPPOE_SET_USER_NAME           0x44



/**
 * @brief Set PPPOE password
 * @param password using 0 to end
 */
#define CMD00_PPPOE_SET_PASSWORD            0x45



/**
 * @brief Set PPPOE enable
 * @param enable 1 starts, 0 turns off
 */
#define CMD10_PPPOE_ENABLE                  0x46



/**
 * @brief Get PPPOE status
 * @return
 *   - 0 success
 *   - others fail
 */
#define CMD01_GET_PPPOE_STATUS              0x47



/**
 * @brief Set TCP MSS
 * @param MSS(LSB) 2 bytes TCP MSS
 */
#define CMD20_SET_TCP_MSS                   0x50



/**
 * @brief Set TTL
 * @param socknum index value of the socket
 * @param ttl TTL value, the maximum is 128
 */
#define CMD20_SET_TTL                       0x51



/**
 * @brief Set SOCKET receive buffer
 * @param socknum index value of the socket
 * @param blkidx starting block index
 * @param blknum block number
 */
#define CMD30_SET_RECV_BUF                  0x52



/**
 * @brief Set SOCKET send buffer
 * @param socknum index value of the socket
 * @param blkidx starting block index
 * @param blknum block number
 */
#define CMD30_SET_SEND_BUF                  0x53



/**
 * @brief Set MAC receive buffer
 * @param size size of the MAC receive buffer in 1 byte, in units of 16
 */
#define CMD10_SET_MAC_RECV_BUF              0x54



/**
 * @brief Set function parameters
 * @param params 4-byte startup parameters
 */
#define CMD40_SET_FUN_PARA                  0x55



/**
 * @brief Set KEEPLIVE idle time
 * @param time(LSB) 4-byte keep-alive timer idle time
 */
#define CMD40_SET_KEEP_LIVE_IDLE            0x56



/**
 * @brief Set KEEPLIVE interval time
 * @param time(LSB) 4-byte keep-alive timer interval time
 */
#define CMD40_SET_KEEP_LIVE_INTVL           0x57



/**
 * @brief Set KEEPLIVE retries
 * @param times retry times
 */
#define CMD10_SET_KEEP_LIVE_CNT             0x58



/**
 * @brief Set socket n KEEPLIVE
 * @param socknum index value of the socket
 * @param enable 1 starts, 0 turns off
 */
#define CMD20_SET_KEEP_LIVE_SN              0X59



/**
 * @brief Erase EEPROM
 */
#define CMD00_EEPROM_ERASE                  0xE9



/**
 * @brief Write EEPROM
 * @param address 2-byte address
 * @param length length must be less than 64
 */
#define CMD30_EEPROM_WRITE                  0xEA



/**
 * @brief Read EEPROM
 * @param address 2-byte address
 * @param length length must be less than 64
 */
#define CMD30_EEPROM_READ                   0xEB



/**
 * @brief Read GPIO register
 * @param address REG address
 */
#define CMD10_READ_GPIO_REG                 0xEC



/**
 * @brief Write GPIO register
 * @param address REG address
 * @param data
 */
#define CMD20_WRITE_GPIO_REG                0xED

/**
 * @}
*/



/**
 * @defgroup Socket Protocal
 * @{
 */
#define PROTO_TYPE_IP_RAW           0 /* IP layer raw data*/
#define PROTO_TYPE_MAC_RAW          1 /* MAC layer raw data*/
#define PROTO_TYPE_UDP              2 /* UDP protocol type*/
#define PROTO_TYPE_TCP              3 /* TCP protocol type*/
/**
 * @}
 */



/**
 * @defgroup PHY Command Parameters/Status
 * @{
 */
#define PHY_DISCONN                 (1<<0) /* PHY disconnect*/
#define PHY_10M_FLL                 (1<<1) /* 10M full duplex*/
#define PHY_10M_HALF                (1<<2) /* 10M half-duplex*/
#define PHY_100M_FLL                (1<<3) /* 100M full duplex*/
#define PHY_100M_HALF               (1<<4) /* 100M half-duplex*/
#define PHY_AUTO                    (1<<5) /* PHY automatic mode*/
/**
 * @}
 */



/**
 * @defgroup MAC Filter
 * @{
 */
#define MAC_FILT_RECV_BORADPKT      (1<<0) /* Enable receiving broadcast packets*/
#define MAC_FILT_RECV_ALL           (1<<1) /* Enable receiving all data packets*/
#define MAC_FILT_RECV_MULTIPKT      (1<<2) /* Enable receiving multicast packets*/
#define MAC_FILT_RECV_ENABLE        (1<<3) /* Enable reception*/
#define MAC_FILT_SEND_ENABLE        (1<<4) /* Enable sending*/
/**
 * @}
 */



/**
 * @defgroup Global Interrupt Status
 * @{
 */
#define GINT_STAT_UNREACH           (1<<0) /* Unreachable interrupt*/
#define GINT_STAT_IP_CONFLI         (1<<1) /* IP conflict*/
#define GINT_STAT_PHY_CHANGE        (1<<2) /* PHY status change*/
#define GINT_STAT_DHCP              (1<<3) /* PHY status change*/
#define GINT_STAT_SOCK0             (1<<4) /* socket0 generates an interrupt*/
#define GINT_STAT_SOCK1             (1<<5) /* socket1 generates an interrupt*/
#define GINT_STAT_SOCK2             (1<<6) /* socket2 generates an interrupt*/
#define GINT_STAT_SOCK3             (1<<7) /* socket3 generates an interrupt*/
#define GINT_STAT_SOCK4             (1<<8) /* Scoket4 generates an interrupt*/
#define GINT_STAT_SOCK5             (1<<9) /* Scoket5 generates an interrupt*/
#define GINT_STAT_SOCK6             (1<<10) /* Scoket6 generates an interrupt*/
#define GINT_STAT_SOCK7             (1<<11) /* Scoket7 generates an interrupt*/
/**
 * @}
 */




/**
 * @defgroup Socket Interrupt Status
 * @{
 */
#define SINT_STAT_SENBUF_FREE       (1<<0) /* Send buffer is free*/
#define SINT_STAT_SEND_OK           (1<<1) /* Sent successfully*/
#define SINT_STAT_RECV              (1<<2) /* The socket port receives data or the receive buffer is not empty*/
#define SINT_STAT_CONNECT           (1<<3) /* The connection is successful, this interrupt is generated in TCP mode*/
#define SINT_STAT_DISCONNECT        (1<<4) /* The connection is disconnected, this interrupt occurs in TCP mode*/
#define SINT_STAT_TIM_OUT           (1<<6) /* This interrupt occurs in ARP and TCP modes*/
/**
 * @}
 */



/**
 * @defgroup Command Status
 * @{
 */
#define CH395_ERR_SUCCESS           0x00 /* Command operation successful*/
#define CH395_RET_ABORT             0x5F /* Command operation failed*/
#define CH395_ERR_BUSY              0x10 /* Busy status, indicating that the command is currently being executed*/
#define CH395_ERR_MEM               0x11 /* Memory error*/
#define CH395_ERR_BUF               0x12 /* Buffer error*/
#define CH395_ERR_TIMEOUT           0x13 /* timeout*/
#define CH395_ERR_RTE               0x14 /* Routing error*/
#define CH395_ERR_ABRT              0x15 /* Connection stopped*/
#define CH395_ERR_RST               0x16 /* Connection reset*/
#define CH395_ERR_CLSD              0x17 /* connection closed/socket in closed state*/
#define CH395_ERR_CONN              0x18 /* No connection*/
#define CH395_ERR_VAL               0x19 /* Wrong value*/
#define CH395_ERR_ARG               0x1A /* Wrong parameter*/
#define CH395_ERR_USE               0x1B /* Already used*/
#define CH395_ERR_IF                0x1C /* MAC error*/
#define CH395_ERR_ISCONN            0x1D /* Connected*/
#define CH395_ERR_OPEN              0X20 /* Already opened*/
#define CH395_ERR_UNKNOW            0xFA /* Unknown error*/
/**
 * @}
 */



/**
 * @defgroup PPP Status
 * @{
 */
#define CH395_PPP_SUCCESS           0 /* Success*/
#define CH395_PPPERR_PARM           1 /* Invalid parameter*/
#define CH395_PPPERR_OPEN           2 /* Unable to open PPP session*/
#define CH395_PPPERR_DEVICE         3 /* Invalid PPP device*/
#define CH395_PPPERR_ALLOC          4 /* Resource allocation failed*/
#define CH395_PPPERR_USER           5 /* User interrupt*/
#define CH395_PPPERR_CONNECT        6 /* Disconnected*/
#define CH395_PPPERR_AUTHFAIL       7 /* Challenge identification failed*/
#define CH395_PPPERR_PROTOCOL       8 /* Handshake protocol failed*/
#define CH395_PPPERR_TIME_OUT       9 /* Timeout failure*/
#define CH395_PPPERR_CLOSE          10 /* Close failed*/
/**
 * @}
 */



/**
 * @defgroup Unreachable Code
 * @note Please refer to the RFC792 document for other values
 * @{
 */
#define UNREACH_CODE_HOST           0 /* The host is unreachable*/
#define UNREACH_CODE_NET            1 /* The network is unreachable*/
#define UNREACH_CODE_PROTOCOL       2 /* The protocol is unreachable*/
#define UNREACH_CODE_PROT           3 /* Port unreachable*/
/**
 * @}
 */




/**
 * @defgroup Command Header
 * @{
 */
#define SER_SYNC_CODE1              0x57 /* Serial port command synchronization code 1 */
#define SER_SYNC_CODE2              0xAB /* Serial port command synchronization code 2 */
/**
 * @}
 */



/**
 * @defgroup TCP Status
 * @{
 */
# define TCP_CLOSED                 0
# define TCP_LISTEN                 1
# define TCP_SYN_SENT               2
# define TCP_SYN_RCVD               3
# define TCP_ESTABLISHED            4
# define TCP_FIN_WAIT_1             5
# define TCP_FIN_WAIT_2             6
# define TCP_CLOSE_WAIT             7
# define TCP_CLOSING                8
# define TCP_LAST_ACK               9
# define TCP_TIME_WAIT              10
/**
 * @}
 */



/**
 * @defgroup GPIO Register Address
 * @{
 */
#define GPIO_DIR_REG                0x80 /* Register direction register, 1: output; 0: input*/
#define GPIO_IN_REG                 0x81 /* GPIO input register*/
#define GPIO_OUT_REG                0x82 /* GPIO output register*/
#define GPIO_CLR_REG                0x83 /* GPIO output clear: 0=keep, 1=clear */
#define GPIO_PU_REG                 0x84 /* GPIO pull-up enable register*/
#define GPIO_PD_REG                 0x85 /* GPIO pull-down enable register*/
/**
 * @}
 */




/**
 * @defgroup Function Parameter
 * @{
 */
#define FUN_PARA_FLAG_TCP_SERVER    (1<<1) /* tcp server multi-connection mode flag, supported by version 0X44 and later*/
#define FUN_PARA_FLAG_LOW_PWR       (1<<2) /* Low energy consumption mode */
#define SOCK_CTRL_FLAG_SOCKET_CLOSE (1<<3) /* CH395 does not actively close the Socket */
#define SOCK_DISABLE_SEND_OK_INT    (1<<4) /* send ok interrupt control bit, 1 means turning off send ok interrupt*/
/**
 * @}
 */

# ifdef __cplusplus
}
# endif
