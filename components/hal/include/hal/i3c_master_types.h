/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/hal_utils.h"

/**
 * @brief I3C Master Command Table Entry
 *
 * This structure represents a command entry for the I3C master. It is used
 * to define and execute different types of I3C commands, including address
 * assignment, immediate commands, and regular transfers. Each entry consists
 * of two parts: the low part (`cmd_l`) and the high part (`cmd_h`).
 */
typedef struct {
    /**
     * @brief Command Low Part (cmd_l)
     *
     * This union defines the command attributes for different types of I3C commands.
     */
    union {
        /**
         * @brief Address Assignment Command
         *
         * Used for dynamically assigning addresses to devices on the I3C bus.
         */
        struct {
            uint32_t cmd_attr   : 3; ///< Command type (0: regular, 1: immediate, 2: address assignment).
            uint32_t tid        : 4; ///< Transaction ID, used as an identification tag for this command. The same value shall be reflected in the Response Descriptor
            uint32_t cmd        : 8; ///< Transfer Command CCC value, specifies CCC code.
            uint32_t reserved15 : 1; ///< Reserved bit.
            uint32_t dev_indx   : 5; ///< Indicates the DAT table index for the slave device being addressed with the transfer. Static and Device addressing related information will be stored to this index in the DAT. This field is reserved in Broadcast CCC transfer.
            uint32_t reserved21 : 5; ///< Reserved bits.
            uint32_t dev_cnt    : 4; ///< Indicates the number of devices to which Dynamic Address shall be assigned.
            uint32_t roc        : 1; ///< Response on Completion, controls whether the response status is sent after the successful completion of the transfer command. (0: Respond status is not required, 1: Respond status is required).
            uint32_t toc        : 1; ///< Terminate on Completion, controls what bus condition to issue after the transfer command completes. (0: Issue repeated start at the end of transfer. 1: Issue Stop (P) at the end of transfer Note: This field must be set to 1 for ENTDAA. It is meaningful for SETDASA transfer)
        } addr; ///< Address assignment command format.

        /**
         * @brief Immediate Command
         *
         * Executes a command with immediate effect, typically for control or configuration purposes.
         */
        struct {
            uint32_t cmd_attr   : 3; ///< Command type (0: regular, 1: immediate, 2: address assignment).
            uint32_t tid        : 4; ///< Transaction ID, used as an identification tag for this command. The same value shall be reflected in the Response Descriptor
            uint32_t cmd        : 8; ///< Transfer Command CCC value, specifies CCC code.
            uint32_t cp         : 1; ///< Command Present, indicates whether the CMD field is valid for CCC transfer
            uint32_t dev_indx   : 5; ///< Indicates the DAT table index for the slave device being addressed with the transfer. Static and Device addressing related information will be stored to this index in the DAT. This field is reserved in Broadcast CCC transfer.
            uint32_t reserved21 : 2; ///< Reserved bits.
            uint32_t bc         : 3; ///< Byte count (number of bytes transferred in command_h).
            uint32_t mode       : 3; ///< Data Transfer Speed and Mode. Set the mode and speed for the I3C or I2C transfer. Interpretation of this field depends on whether the Device is in I3C Mode vs. I2C Mode (see the DEVICE field in the DAT Table entry indexed by field DEV_INDEX).
            uint32_t rnw        : 1; ///< Identifies the direction of this transfer (1: read, 0: write).
            uint32_t roc        : 1; ///< Response on Completion, controls whether the response status is sent after the successful completion of the transfer command. (0: Respond status is not required, 1: Respond status is required).
            uint32_t toc        : 1; ///< Terminate on Completion, controls what bus condition to issue after the transfer command completes. (0: Issue repeated start at the end of transfer. 1: Issue Stop (P) at the end of transfer Note: This field must be set to 1 for ENTDAA. It is meaningful for SETDASA transfer)
        } immediate; ///< Immediate command format.

        /**
         * @brief Regular Transfer Command
         *
         * Used for standard data transfers on the I3C bus.
         */
        struct {
            uint32_t cmd_attr   : 3; ///< Command type (0: regular, 1: immediate, 2: address assignment).
            uint32_t tid        : 4; ///< Transaction ID, used as an identification tag for this command. The same value shall be reflected in the Response Descriptor
            uint32_t cmd        : 8; ///< Transfer Command CCC value, specifies CCC code.
            uint32_t cp         : 1; ///< Command Present, indicates whether the CMD field is valid for CCC transfer
            uint32_t dev_indx   : 5; ///< Indicates the DAT table index for the slave device being addressed with the transfer. Static and Device addressing related information will be stored to this index in the DAT. This field is reserved in Broadcast CCC transfer.
            uint32_t reserved21 : 5; ///< Reserved bits.
            uint32_t mode       : 3; ///< Data Transfer Speed and Mode. Set the mode and speed for the I3C or I2C transfer. Interpretation of this field depends on whether the Device is in I3C Mode vs. I2C Mode (see the DEVICE field in the DAT Table entry indexed by field DEV_INDEX).
            uint32_t rnw        : 1; ///< Identifies the direction of this transfer (1: read, 0: write).
            uint32_t roc        : 1; ///< Response on Completion, controls whether the response status is sent after the successful completion of the transfer command. (0: Respond status is not required, 1: Respond status is required).
            uint32_t toc        : 1; ///< Terminate on Completion, controls what bus condition to issue after the transfer command completes. (0: Issue repeated start at the end of transfer. 1: Issue Stop (P) at the end of transfer Note: This field must be set to 1 for ENTDAA. It is meaningful for SETDASA transfer)
        } regular; ///< Regular transfer command format.

        /**
         * @brief Raw Value Access
         *
         * Provides direct access to the 32-bit value of the command.
         */
        uint32_t val; ///< Raw 32-bit value of the command.
    } cmd_l; ///< Low part of the command.

    /**
     * @brief Command High Part (cmd_h)
     *
     * This union defines additional attributes for different types of I3C commands.
     */
    union {
        /**
         * @brief Address Assignment Command High Part
         *
         * No additional data for address assignment commands.
         */
        struct {
            uint32_t reserved0 : 32; ///< Reserved bits.
        } addr;

        /**
         * @brief Immediate Command High Part
         *
         * Provides up to four bytes of data for the immediate command.
         */
        struct {
            uint32_t byte1 : 8; ///< First byte of data.
            uint32_t byte2 : 8; ///< Second byte of data.
            uint32_t byte3 : 8; ///< Third byte of data.
            uint32_t byte4 : 8; ///< Fourth byte of data.
        } immediate;

        /**
         * @brief Regular Transfer Command High Part
         *
         * Defines the data length for regular transfer commands.
         */
        struct {
            uint32_t reserved0 : 16; ///< Reserved bits.
            uint32_t dl        : 16; ///< Data length (number of bytes to transfer).
        } regular;

        /**
         * @brief Raw Value Access
         *
         * Provides direct access to the 32-bit value of the high part of the command.
         */
        uint32_t val; ///< Raw 32-bit value of the high part of the command.
    } cmd_h; ///< High part of the command.
} i3c_master_command_table_t;

/**
 * @brief I3C Master Address Table Entry
 *
 * This structure represents a single entry in the I3C master address table.
 * It is used to store and manage the static and dynamic addresses of devices
 * on the I3C bus in different modes, such as SETDASA, ENTDAA, or I2C compatibility.
 */
typedef struct {
    union {
        /**
         * @brief SETDASA Mode
         *
         * The SETDASA (Set Dynamic Address from Static Address) mode is used
         * to assign a dynamic address to a device based on its static address.
         */
        struct {
            uint32_t static_addr  : 7;  ///< Static Address of the I3C Target.
            uint32_t reserved7    : 9;  ///< Reserved bits.
            uint32_t dynamic_addr : 8;  ///< If used for address assignment by SETDASA CCC, it is the Dynamic Address to be assigned to the I3C device with Static Address. For other transfer, the Dynamic Address is used as the targets’ address.
            uint32_t reserved24   : 5;  ///< Reserved bits.
            uint32_t d            : 2;  ///<  Device NACK Retry Counter. It decides the number of retry when current transfer nacked.
            uint32_t mode         : 1;  ///< Mode indicator (I3C mode = 0).
        } setdasa;

        /**
         * @brief ENTDAA Mode
         *
         * The ENTDAA (Enter Dynamic Address Assignment) mode is used for
         * dynamically assigning addresses to devices without requiring a static address.
         */
        struct {
            uint32_t reserved0    : 16; ///< Reserved bits.
            uint32_t dynamic_addr : 7;  ///< If used for address assignment by ENTDAA CCC, it is the Dynamic Address to be assigned to the I3C device without Static Address. For other transfer, the Dynamic Address is used as the targets’ address.
            uint32_t dyn          : 1;  ///< Parity Bit of 7-bit Dynamic Address.
            uint32_t reserved24   : 5;  ///< Reserved bits.
            uint32_t d            : 2;  ///< Device NACK Retry Counter. It decides the number of retry when current transfer nacked.
            uint32_t mode         : 1;  ///< Mode indicator (I3C mode = 0).
        } entdaa;

        /**
         * @brief I2C Compatibility Mode
         *
         * This mode is used to represent devices operating in I2C mode.
         * Only the static address is relevant in this case.
         */
        struct {
            uint32_t static_addr : 7;   ///< Static address of the I2C device (7-bit).
            uint32_t reserved7   : 22;  ///< Reserved bits.
            uint32_t d           : 2;   ///< Device NACK Retry Counter. It decides the number of retry when the current transfer is nacked.
            uint32_t mode        : 1;   ///< Mode indicator (I2C mode = 1).
        } i2c;

        /**
         * @brief Raw Value
         *
         * This provides direct access to the raw 32-bit value of the entry,
         * allowing for low-level manipulation or initialization.
         */
        uint32_t val; ///< Raw 32-bit value of the address table entry.
    } dat; ///< Union of different mode representations.
} i3c_master_address_table_t;

/**
 * @brief I3C master command types
 *
 * This enumeration defines the types of commands that the I3C master can issue.
 * Each command type corresponds to a specific purpose in the I3C communication protocol.
 */
typedef enum {
    I3C_MASTER_REGULAR_COMMAND = 0,                               ///< I3C master sends regular command
    I3C_MASTER_IMMEDIATE_COMMAND = 1,                             ///< I3C master sends immediate command
    I3C_MASTER_ADDRESS_ASSIGNMENT_COMMAND = 2,                    ///< I3C master assigns address command
} i3c_master_command_type_t;

/**
 * @brief I3C master transfer directions
 *
 * This enumeration defines the direction of data transfer for I3C master operations.
 * The transfer direction specifies whether the master sends data to or receives data from the target device.
 */
typedef enum {
    I3C_MASTER_WRITE_TRANSFER = 0,                                ///< I3C master write direction
    I3C_MASTER_READ_TRANSFER = 1,                                 ///< I3C master read direction
} i3c_master_transfer_direction_t;

/**
 * @brief I3C master response data
 *
 * This union represents the response data structure used by the I3C master.
 * It includes fields for data length, transaction ID, and error status.
 */
typedef union {
    struct {
        uint32_t dl         : 16; ///< Data Length
        uint32_t reserved16 : 8;
        uint32_t tid        : 4;  ///< Transaction ID, used as an identification tag for this command. The same value shall be reflected in the Response Descriptor
        uint32_t err_sts    : 4;  ///< Error state: A 4-bit field indicating the error state of the transaction.
    };
    uint32_t val;
} i3c_master_response_data_t;

#if SOC_I3C_MASTER_SUPPORTED
/**
 * @brief I2C group clock source
 */
typedef soc_periph_i3c_master_clk_src_t i3c_master_clock_source_t;
#else
/**
 * @brief default type
 */
typedef int                             i3c_master_clock_source_t;
#endif

#ifdef __cplusplus
}
#endif
