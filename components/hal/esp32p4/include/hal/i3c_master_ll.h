/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for I3C register operations

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/lpperi_struct.h"
#include "hal/misc.h"
#include "soc/i3c_mst_mem_struct.h"
#include "soc/i3c_mst_struct.h"
#include "soc/i3c_mst_reg.h"
#include "hal/i3c_master_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I3C_LL_GET_HW(num)          (((num) == 0) ? (&I3C_MST) : NULL)
#define I3C_LL_MASTER_EVENT_INTR    (I3C_MST_TRANSFER_COMPLETE_INT_ENA_M | I3C_MST_COMMAND_DONE_INT_ENA_M | I3C_MST_TX_DATA_BUF_THLD_INT_ENA_M | I3C_MST_RX_DATA_BUF_THLD_INT_ENA_M)
#define I3C_LL_MASTER_DATA_BUFFER_SIZE       (128) // The TX Data Buffer and the RX Data Buffer can store up to 128 bytes at a time
#define I3C_LL_MASTER_TRANSMIT_EVENT_INTR    (I3C_MST_TX_DATA_BUF_THLD_INT_ENA_M | I3C_MST_TRANSFER_COMPLETE_INT_ENA_M | I3C_MST_COMMAND_DONE_INT_ENA_M)
#define I3C_LL_MASTER_RECEIVE_EVENT_INTR     (I3C_MST_RX_DATA_BUF_THLD_INT_ENA_M | I3C_MST_TRANSFER_COMPLETE_INT_ENA_M | I3C_MST_COMMAND_DONE_INT_ENA_M)
#define I3C_MASTER_LL_DEFAULT_SETUP_TIME     (600)

/**
 * @brief I3C master command types
 *
 * This enumeration defines the types of commands that the I3C master can issue.
 * Each command type corresponds to a specific purpose in the I3C communication protocol.
 */
typedef enum {
    I3C_MASTER_LL_COMMAND_REGULAR = 0,             ///< I3C master sends regular command
    I3C_MASTER_LL_COMMAND_IMMEDIATE = 1,           ///< I3C master sends immediate command
    I3C_MASTER_LL_COMMAND_ADDRESS_ASSIGNMENT = 2,  ///< I3C master assigns address command
} i3c_master_ll_command_type_t;

/**
 * @brief I3C master transfer directions
 *
 * This enumeration defines the direction of data transfer for I3C master operations.
 * The transfer direction specifies whether the master sends data to or receives data from the target device.
 */
typedef enum {
    I3C_MASTER_LL_TRANSFER_DIR_WRITE = 0, ///< I3C master write direction
    I3C_MASTER_LL_TRANSFER_DIR_READ = 1,  ///< I3C master read direction
} i3c_master_ll_transfer_direction_t;

/**
 * @brief I3C Master Command Descriptor
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
            uint32_t cmd_attr   : 3; ///< Command type (0: regular, 1: immediate, 2: address assignment). See `i3c_master_ll_command_type_t`.
            uint32_t tid        : 4; ///< Transaction ID, used as an identification tag for this command. The same value shall be reflected in the Response Descriptor.
            uint32_t cmd        : 8; ///< Transfer Command CCC value, specifies CCC code.
            uint32_t reserved15 : 1; ///< Reserved bit.
            uint32_t dev_indx   : 5; ///< Indicates the DAT table index for the slave device being addressed with the transfer. Static and Device addressing related information will be stored to this index in the DAT. This field is reserved in Broadcast CCC transfer.
            uint32_t reserved21 : 5; ///< Reserved bits.
            uint32_t dev_cnt    : 4; ///< Indicates the number of devices to which Dynamic Address shall be assigned.
            uint32_t roc        : 1; ///< Response on Completion, controls whether the response status is sent after the successful completion of the transfer command. (0: Respond status is not required, 1: Respond status is required).
            uint32_t toc        : 1; ///< Terminate on Completion, controls what bus condition to issue after the transfer command completes. (0: Issue repeated start (Sr) at the end of transfer. 1: Issue Stop (P) at the end of transfer Note: This field must be set to 1 for ENTDAA. It is meaningful for SETDASA transfer)
        } addr; ///< Address assignment command format.

        /**
         * @brief Immediate Command
         *
         *  It is suitable for situations where a small amount of data needs to be transferred quickly,
         *  and the data is directly included in the command.
         */
        struct {
            uint32_t cmd_attr   : 3; ///< Command type (0: regular, 1: immediate, 2: address assignment). See `i3c_master_ll_command_type_t`.
            uint32_t tid        : 4; ///< Transaction ID, used as an identification tag for this command. The same value shall be reflected in the Response Descriptor.
            uint32_t cmd        : 8; ///< Transfer Command CCC value, specifies CCC code.
            uint32_t cp         : 1; ///< Command Present, indicates whether the CMD field is valid for CCC transfer
            uint32_t dev_indx   : 5; ///< Indicates the DAT table index for the slave device being addressed with the transfer. Static and Device addressing related information will be stored to this index in the DAT. This field is reserved in Broadcast CCC transfer.
            uint32_t reserved21 : 2; ///< Reserved bits.
            uint32_t bc         : 3; ///< Byte count (number of bytes transferred in command_h).
            uint32_t mode       : 3; ///< Data Transfer Speed and Mode. Set the mode and speed for the I3C or I2C transfer. Interpretation of this field depends on whether the Device is in I3C Mode vs. I2C Mode (see the DEVICE field in the DAT Table entry indexed by field DEV_INDEX).
            uint32_t rnw        : 1; ///< Identifies the direction of this transfer (1: read, 0: write). See `i3c_master_ll_transfer_direction_t`.
            uint32_t roc        : 1; ///< Response on Completion, controls whether the response status is sent after the successful completion of the transfer command. (0: Respond status is not required, 1: Respond status is required).
            uint32_t toc        : 1; ///< Terminate on Completion, controls what bus condition to issue after the transfer command completes. (0: Issue repeated start at the end of transfer. 1: Issue Stop (P) at the end of transfer Note: This field must be set to 1 for ENTDAA. It is meaningful for SETDASA transfer)
        } immediate; ///< Immediate command format.

        /**
         * @brief Regular Transfer Command
         *
         * Used for general CCC transfer and private data transfer
         */
        struct {
            uint32_t cmd_attr   : 3; ///< Command type (0: regular, 1: immediate, 2: address assignment). See `i3c_master_ll_command_type_t`.
            uint32_t tid        : 4; ///< Transaction ID, used as an identification tag for this command. The same value shall be reflected in the Response Descriptor.
            uint32_t cmd        : 8; ///< Transfer Command CCC value, specifies CCC code.
            uint32_t cp         : 1; ///< Command Present, indicates whether the CMD field is valid for CCC transfer
            uint32_t dev_indx   : 5; ///< Indicates the DAT table index for the slave device being addressed with the transfer. Static and Device addressing related information will be stored to this index in the DAT. This field is reserved in Broadcast CCC transfer.
            uint32_t reserved21 : 5; ///< Reserved bits.
            uint32_t mode       : 3; ///< Data Transfer Speed and Mode. Set the mode and speed for the I3C or I2C transfer. Interpretation of this field depends on whether the Device is in I3C Mode vs. I2C Mode (see the DEVICE field in the DAT Table entry indexed by field DEV_INDEX).
            uint32_t rnw        : 1; ///< Identifies the direction of this transfer (1: read, 0: write). See `i3c_master_ll_transfer_direction_t`.
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
} i3c_master_ll_command_descriptor_t;

/**
 * @brief I3C Master Error State Enumeration
 */
typedef enum {
    I3C_MASTER_LL_NO_ERROR = 0,                       ///< Indicates that the I3C master encountered no errors.
    I3C_MASTER_LL_READ_LENGTH_MISMATCH = 3,           ///< The length of data read from the slave device does not match the expected or requested length.
    I3C_MASTER_LL_BROADCAST_ADDRESS_NACK_ERROR = 4,          ///< Broadcast Address NACK Error.
    I3C_MASTER_LL_ADDRESS_NACK_OR_DYNAMIC_ADDRESS_NACK = 5,  ///< Address NACK or Dynamic Address NACK.
    I3C_MASTER_LL_BUFFER_RX_OVERFLOW_TX_UNDERFLOW = 6,       ///< Buffer RX Overflow or TX Underflow.
    I3C_MASTER_LL_I2C_SLAVE_WRITE_DATA_NACK_ERROR = 9,       ///< I2C Slave Write Data NACK Error.
} i3c_master_ll_error_state_t;

/**
 * @brief I3C master response data
 *
 * This union represents the response data structure used by the I3C master.
 * It includes fields for data length, transaction ID, and error status.
 */
typedef union {
    struct {
        uint32_t dl         : 16; ///< Data Length. For Write Transfer: Remaining data length (in bytes).
        ///< For Read Transfer: Received data length (in bytes).
        ///< For Address Assignment: Remaining Devices count.
        uint32_t reserved16 : 8;
        uint32_t tid        : 4;  ///< Identification tag for the command. This value shall match one of commands sent on the bus.
        uint32_t err_sts    : 4;  ///< Error state: A 4-bit field indicating the error state of the transaction. See `i3c_master_ll_error_state_t`
    };
    uint32_t val;
} i3c_master_ll_response_descriptor_t;

/**
 * @brief I3C master operating mode
 *
 * This enumeration defines the operating modes for an I3C master.
 * It can either operate in the I3C protocol mode or be backward-compatible with I2C devices.
 */
typedef enum {
    I3C_MASTER_LL_MODE_I3C = 0, ///< I3C works under I3C mode
    I3C_MASTER_LL_MODE_I2C = 1, ///< I3C works under I2C mode
} i3c_master_ll_mode_t;

/**
 * @brief I2C under I3C master speed mode
 *
 * This enumeration defines the speed modes for an I3C master.
 * It can either operate in the I3C protocol mode or be backward-compatible with I2C devices.
 */
typedef enum {
    I3C_MASTER_LL_I2C_FAST_MODE = 0, ///< I3C works under I2C fast mode
    I3C_MASTER_LL_I2C_FAST_MODE_PLUS = 1, ///< I3C works under I2C fast mode plus
} i3c_master_ll_i2c_speed_mode_t;

/**
 * @brief I3C Device address descriptor
 *
 * This structure represents a single entry in the I3C master address table.
 * It is used to store and manage the static and dynamic addresses of devices
 * on the I3C bus in different modes.
 */
typedef union {
    /**
     * @brief I3C device with static address
     *
     * Use SETDASA (Set Dynamic Address from Static Address) command to assign a dynamic address to a device based on its static address.
     */
    struct {
        uint32_t static_addr  : 7;  ///< Static Address of the I3C Target.
        uint32_t reserved7    : 9;  ///< Reserved bits.
        uint32_t dynamic_addr : 7;  ///< For SETDASA command, it is the Dynamic Address to be assigned to the I3C device with Static Address.
        ///< For other transfer, it is the Dynamic Address used as the targets’ address.
        uint32_t reserved24   : 6;  ///< Reserved bits.
        uint32_t dnrc         : 2;  ///< Device NACK Retry Counter. It decides the number of retry when current transfer nacked.
        uint32_t mode         : 1;  ///< Mode indicator, must be I3C mode (0). See `i3c_master_ll_mode_t`.
    } i3c_static;

    /**
     * @brief I3C device with dynamic address
     *
     * Use ENTDAA (Enter Dynamic Address Assignment) command is used to dynamically assign addresses to devices without requiring a static address.
     */
    struct {
        uint32_t reserved0    : 16; ///< Reserved bits.
        uint32_t dynamic_addr : 7;  ///< For ENTDAA command, it is the Dynamic Address to be assigned to the I3C device without Static Address.
        ///< For other transfer, it is the Dynamic Address used as the targets’ address.
        uint32_t par          : 1;  ///< Parity Bit of 7-bit Dynamic Address.
        uint32_t reserved24   : 5;  ///< Reserved bits.
        uint32_t dnrc         : 2;  ///< Device NACK Retry Counter. It decides the number of retry when current transfer nacked.
        uint32_t mode         : 1;  ///< Mode indicator, must be I3C mode (0). See `i3c_master_ll_mode_t`.
    } dynamic;

    /**
     * @brief I2C devices with static address
     *
     * This mode is used to represent devices operating in I2C mode.
     */
    struct {
        uint32_t static_addr : 7;   ///< Static address of the I2C device (7-bit).
        uint32_t reserved7   : 22;  ///< Reserved bits.
        uint32_t dnrc        : 2;   ///< Device NACK Retry Counter. It decides the number of retry when the current transfer is nacked.
        uint32_t mode        : 1;   ///< Mode indicator, must be I2C mode (1). See `i3c_master_ll_mode_t`.
    } i2c_static;

    /**
     * @brief Raw Value
     *
     * This provides direct access to the raw 32-bit value of the entry,
     * allowing for low-level manipulation or initialization.
     */
    uint32_t val; ///< Raw 32-bit value of the address table entry.
} i3c_master_ll_device_address_descriptor_t;

typedef enum {
    I3C_MASTER_LL_FIFO_WM_LENGTH_2 = 0x0,
    I3C_MASTER_LL_FIFO_WM_LENGTH_4 = 0x1,
    I3C_MASTER_LL_FIFO_WM_LENGTH_8 = 0x2,
    I3C_MASTER_LL_FIFO_WM_LENGTH_16 = 0x3,
    I3C_MASTER_LL_FIFO_WM_LENGTH_31 = 0x4,
} i3c_master_ll_fifo_wm_t;

/**
 * @brief Set the clock source for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param src_clk Clock source, (1) for PLL_F160M, (0) for XTAL
 */
static inline void i3c_master_ll_set_source_clk(i3c_mst_dev_t *hw, i3c_master_clock_source_t src_clk)
{
    (void)hw; // Suppress unused parameter warning
    switch (src_clk) {
    case I3C_MASTER_CLK_SRC_XTAL:
        HP_SYS_CLKRST.peri_clk_ctrl119.reg_i3c_mst_clk_src_sel = 0;
        break;
    case I3C_MASTER_CLK_SRC_PLL_F160M:
        HP_SYS_CLKRST.peri_clk_ctrl119.reg_i3c_mst_clk_src_sel = 1;
        break;
    case I3C_MASTER_CLK_SRC_PLL_F120M:
        HP_SYS_CLKRST.peri_clk_ctrl119.reg_i3c_mst_clk_src_sel = 2;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_master_ll_set_source_clk(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_master_ll_set_source_clk(__VA_ARGS__);} while(0)

/**
 * @brief Set the device address table for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param addr_table Device address table
 * @param device_number Number of devices
 */
__attribute__((always_inline))
static inline void i3c_master_ll_set_device_address_table(i3c_mst_dev_t *hw, i3c_master_ll_device_address_descriptor_t *addr_table, size_t device_number)
{
    for (int i = 0; i < device_number; i++) {
        I3C_MST_MEM.dev_addr_table[i].val = addr_table[i].val;
    }
}

/**
 * @brief Enable or disable the bus clock for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param enable Whether to enable the bus clock
 */
static inline void i3c_master_ll_enable_bus_clock(i3c_mst_dev_t *hw, bool enable)
{
    (void)hw; // Suppress unused parameter warning
    HP_SYS_CLKRST.soc_clk_ctrl2.reg_i3c_mst_apb_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_master_ll_enable_bus_clock(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_master_ll_enable_bus_clock(__VA_ARGS__);} while(0)

/**
 * @brief Enable or disable the clock for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param enable Whether to enable the clock
 */
static inline void i3c_master_ll_enable_controller_clock(i3c_mst_dev_t *hw, bool enable)
{
    (void)hw; // Suppress unused parameter warning
    HP_SYS_CLKRST.peri_clk_ctrl119.reg_i3c_mst_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_master_ll_enable_controller_clock(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_master_ll_enable_controller_clock(__VA_ARGS__);} while(0)

/**
 * @brief Reset the I3C master registers
 *
 * @param hw I3C master hardware instance
 */
static inline void i3c_master_ll_reset_register(i3c_mst_dev_t *hw)
{
    (void)hw; // Suppress unused parameter warning
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_i3cmst = 1;
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_i3cmst = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_master_ll_reset_register(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_master_ll_reset_register(__VA_ARGS__);} while(0)

/**
 * @brief Set the command table for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param command_buf Command buffer
 * @param command_num Number of commands
 */
__attribute__((always_inline))
static inline void i3c_master_ll_set_command(i3c_mst_dev_t *hw, i3c_master_ll_command_descriptor_t *command_buf, size_t command_num)
{
    for (int i = 0; i < command_num; i++) {
        I3C_MST_MEM.command_buf_port.reg_command = command_buf[i].cmd_l.val;
        I3C_MST_MEM.command_buf_port.reg_command = command_buf[i].cmd_h.val;
    }
}

/**
 * @brief Start a transaction on the I3C master
 *
 * @param hw I3C master hardware instance
 */
__attribute__((always_inline))
static inline void i3c_master_ll_start_transaction(i3c_mst_dev_t *hw)
{
    hw->device_ctrl.reg_trans_start = 1;
    hw->device_ctrl.reg_trans_start = 0;
}

/**
 * @brief Inject the broadcast address in private transfer and legacy I2C transfer
 *
 * @note In-Band Interrupts driven from slaves may not win address arbitration if the broadcast address is not injected.
 *
 * @param hw I3C master hardware instance
 * @param enable Whether to enable or disable the broadcast address injection
 */
static inline void i3c_master_ll_inject_broadcast_address_head(i3c_mst_dev_t *hw, bool enable)
{
    hw->device_ctrl.reg_ba_include = enable ? 1 : 0;
}

/**
 * @brief Set the open-drain timing for I3C master
 *
 * @param hw I3C master hardware instance
 * @param clock_source_freq Clock source frequency
 * @param scl_freq SCL frequency
 */
static inline void i3c_master_ll_set_i3c_open_drain_timing(i3c_mst_dev_t *hw, uint32_t clock_source_freq, uint32_t scl_freq)
{
    uint32_t period_cnt = clock_source_freq / scl_freq / 2;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i3c_mst_od_time, reg_i3c_mst_od_high_period, (period_cnt - 1));
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i3c_mst_od_time, reg_i3c_mst_od_low_period, (period_cnt - 1));
}

/**
 * @brief Set the fast mode timing for I2C master
 *
 * @param hw I3C master hardware instance
 * @param clock_source_freq Clock source frequency
 * @param scl_freq SCL frequency
 */
__attribute__((always_inline))
static inline void i3c_master_ll_set_i2c_fast_mode_timing(i3c_mst_dev_t *hw, uint32_t clock_source_freq, uint32_t scl_freq)
{
    uint32_t period_cnt = clock_source_freq / scl_freq / 2;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i2c_fm_time, reg_i2c_fm_high_period, (period_cnt - 1));
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i2c_fm_time, reg_i2c_fm_low_period, (period_cnt - 1));
}

/**
 * @brief Set the fast mode+ timing for I2C master
 *
 * @param hw I3C master hardware instance
 * @param clock_source_freq Clock source frequency
 * @param scl_freq SCL frequency
 */
__attribute__((always_inline))
static inline void i3c_master_ll_set_i2c_fast_mode_plus_timing(i3c_mst_dev_t *hw, uint32_t clock_source_freq, uint32_t scl_freq)
{
    uint32_t period_cnt = clock_source_freq / scl_freq / 2;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i2c_fmp_time, reg_i2c_fmp_high_period, (period_cnt - 1));
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i2c_fmp_time, reg_i2c_fmp_low_period, (period_cnt - 1));
}

/**
* @brief Set the push-pull timing for I3C master
*
* @param hw I3C master hardware instance
* @param clock_source_freq Clock source frequency
* @param scl_freq SCL frequency
*/
static inline void i3c_master_ll_set_i3c_push_pull_timing(i3c_mst_dev_t *hw, uint32_t clock_source_freq, uint32_t scl_freq)
{
    uint32_t period_cnt = clock_source_freq / scl_freq / 2;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i3c_mst_pp_time, reg_i3c_mst_pp_high_period, (period_cnt - 1));
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i3c_mst_pp_time, reg_i3c_mst_pp_low_period, (period_cnt - 1));
}

/**
 * @brief Set the restart setup time for I3C master
 *
 * @param hw I3C master hardware instance
 * @param time_ns Time in nanoseconds
 * @param clock_source_period_ns Clock source period in nanoseconds
 */
static inline void i3c_master_ll_set_restart_setup_time(i3c_mst_dev_t *hw, uint32_t time_ns, uint32_t clock_source_period_ns)
{
    hw->scl_rstart_setup.reg_scl_rstart_setup_time = (time_ns + clock_source_period_ns) / clock_source_period_ns;
}

/**
 * @brief Set the start hold time for I3C master
 *
 * @param hw I3C master hardware instance
 * @param time_ns Time in nanoseconds
 * @param clock_source_period_ns Clock source period in nanoseconds
 */
static inline void i3c_master_ll_set_start_hold_time(i3c_mst_dev_t *hw, uint32_t time_ns, uint32_t clock_source_period_ns)
{
    hw->scl_start_hold.reg_scl_start_hold_time = time_ns / clock_source_period_ns + 1;
}

/**
 * @brief Set the stop hold time for I3C master
 *
 * @param hw I3C master hardware instance
 * @param time_ns Time in nanoseconds
 * @param clock_source_period_ns Clock source period in nanoseconds
 */
static inline void i3c_master_ll_set_stop_hold_time(i3c_mst_dev_t *hw, uint32_t time_ns, uint32_t clock_source_period_ns)
{
    hw->scl_stop_hold.reg_scl_stop_hold_time = time_ns / clock_source_period_ns + 1;
}

/**
 * @brief Set the stop setup time for I3C master
 *
 * @param hw I3C master hardware instance
 * @param time_ns Time in nanoseconds
 * @param clock_source_period_ns Clock source period in nanoseconds
 */
static inline void i3c_master_ll_set_stop_setup_time(i3c_mst_dev_t *hw, uint32_t time_ns, uint32_t clock_source_period_ns)
{
    hw->scl_stop_setup.reg_scl_stop_setup_time = (time_ns + clock_source_period_ns) / clock_source_period_ns;
}

/**
 * @brief Get the data byte count from the I3C master
 *
 * @param hw I3C master hardware instance
 * @return Number of data bytes, meaning differently according to different transfer
 */
static inline uint32_t i3c_master_ll_get_data_byte_count(i3c_mst_dev_t *hw)
{
    return hw->present_state1.data_byte_cnt;
}

/**
 * @brief Read data from the RX port of the I3C master
 *
 * @param hw I3C master hardware instance
 * @param data Pointer to store the read data
 * @param length Length of data to read
 */
__attribute__((always_inline))
static inline void i3c_master_ll_read_rx_port(i3c_mst_dev_t *hw, uint8_t *data, size_t length)
{
    uint32_t data_word = 0;
    int x = 0;
    uint32_t *data_u32 = (uint32_t *)data;
    for (x = 0; x < (int)length / 4; x++) {
        data_u32[x] = I3C_MST_MEM.rx_data_port.rx_data_port;
    }
    if (length % 4) {
        data_word = I3C_MST_MEM.rx_data_port.rx_data_port;
        for (int i = 0; i < length % 4; i++) {
            data[x * 4 + i] = (data_word >> 8 * i) & 0xFF;
        }
    }
}

/**
 * @brief Write data to the TX buffer via FIFO port
 *
 * @param hw I3C master hardware instance
 * @param data Pointer to the data to write
 * @param length Length of data to write
 */
__attribute__((always_inline))
static inline void i3c_master_ll_write_tx_port(i3c_mst_dev_t *hw, const uint8_t *data, size_t length)
{
    int x = 0;
    uint32_t data_word = 0;
    const uint32_t *data_u32 = (const uint32_t *)data;
    for (x = 0; x < (int)length / 4; x++) {
        I3C_MST_MEM.tx_data_port.reg_tx_data_port = data_u32[x];
    }

    if (length % 4) {
        data_word = 0;
        for (int i = 0; i < length % 4; i++) {
            data_word |= (data[x * 4 + i] << 8 * i);
        }
        I3C_MST_MEM.tx_data_port.reg_tx_data_port = data_word;
    }
}

/**
 * @brief Get the interrupt status register of the I3C master
 *
 * @param dev I3C master hardware instance
 * @return Pointer to the interrupt status register
 */
static inline volatile void *i3c_master_ll_get_interrupt_status_reg(i3c_mst_dev_t *dev)
{
    return &dev->int_st;
}

/**
 * @brief Clear the interrupt mask for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param mask Interrupt mask to clear
 */
__attribute__((always_inline))
static inline void i3c_master_ll_clear_intr_mask(i3c_mst_dev_t *dev, uint32_t mask)
{
    dev->int_clr.val = mask;
}

/**
 * @brief Enable the interrupt mask for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param mask Interrupt mask to enable
 */
__attribute__((always_inline))
static inline void i3c_master_ll_enable_intr_mask(i3c_mst_dev_t *dev, uint32_t mask)
{
    dev->int_st_ena.val |= mask;
}

/**
 * @brief Disable the interrupt mask for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param mask Interrupt mask to disable
 */
__attribute__((always_inline))
static inline void i3c_master_ll_disable_intr_mask(i3c_mst_dev_t *dev, uint32_t mask)
{
    dev->int_st_ena.val &= (~mask);
}

/**
 * @brief Get the interrupt mask status for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param intr_status Pointer to store the interrupt status
 */
__attribute__((always_inline))
static inline void i3c_master_ll_get_intr_mask(i3c_mst_dev_t *dev, uint32_t *intr_status)
{
    *intr_status = dev->int_st.val;
}

/**
 * @brief Get the TX FIFO empty count for the I3C master
 *
 * @param dev I3C master hardware instance
 * @return TX FIFO empty count
 */
__attribute__((always_inline))
static inline uint8_t i3c_master_ll_get_tx_fifo_empty_count(i3c_mst_dev_t *dev)
{
    return dev->data_buffer_status_level.tx_data_buf_empty_cnt;
}

/**
 * @brief Get the RX FIFO full count for the I3C master
 *
 * @param dev I3C master hardware instance
 * @return RX FIFO full count
 */
__attribute__((always_inline))
static inline uint8_t i3c_master_ll_get_rx_fifo_full_count(i3c_mst_dev_t *dev)
{
    return dev->data_buffer_status_level.rx_data_buf_cnt;
}

/**
 * @brief Set the TX data FIFO watermark threshold for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param fifo_wm FIFO watermark threshold
 */
static inline void i3c_master_ll_set_tx_data_fifo_wm_threshold(i3c_mst_dev_t *dev, i3c_master_ll_fifo_wm_t fifo_wm)
{
    dev->data_buffer_thld_ctrl.reg_tx_data_buf_thld = fifo_wm;
}

/**
 * @brief Set the RX data FIFO watermark threshold for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param fifo_wm FIFO watermark threshold
 */
static inline void i3c_master_ll_set_rx_data_fifo_wm_threshold(i3c_mst_dev_t *dev, i3c_master_ll_fifo_wm_t fifo_wm)
{
    dev->data_buffer_thld_ctrl.reg_rx_data_buf_thld = fifo_wm;
}

/**
 * @brief Enable or disable TX by DMA for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param enable Whether to enable TX by DMA
 */
static inline void i3c_master_ll_enable_tx_by_dma(i3c_mst_dev_t *dev, bool enable)
{
    dev->device_ctrl.reg_dma_tx_en = enable;
}

/**
 * @brief Enable or disable RX by DMA for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param enable Whether to enable RX by DMA
 */
static inline void i3c_master_ll_enable_rx_by_dma(i3c_mst_dev_t *dev, bool enable)
{
    dev->device_ctrl.reg_dma_rx_en = enable;
}

/**
 * @brief Get the response buffer value from the I3C master
 *
 * @param dev I3C master hardware instance
 * @return Response data
 */
__attribute__((always_inline))
static inline i3c_master_ll_response_descriptor_t i3c_master_ll_get_response_data(i3c_mst_dev_t *dev)
{
    return (i3c_master_ll_response_descriptor_t)(I3C_MST_MEM.response_buf_port.val);
}

/**
 * @brief Set the init value of dct_index, which points to the entry in the DCT table
 *
 * @param dev I3C master hardware instance
 * @param index DCT index value
 */
static inline void i3c_master_ll_set_init_dct_index(i3c_mst_dev_t *dev, uint32_t index)
{
    dev->device_table.reg_dct_daa_init_index = index;
}

/**
 * @brief Set the init value of dat_index, which points to the entry in the DAT table
 *
 * @param dev I3C master hardware instance
 * @param index DAT index value
 */
static inline void i3c_master_ll_set_init_dat_index(i3c_mst_dev_t *dev, uint32_t index)
{
    dev->device_table.reg_dat_daa_init_index = index;
}

/**
 * @brief Get the current DCT index
 *
 * @param dev I3C master hardware instance
 * @return Current DCT index
 */
static inline uint32_t i3c_master_ll_get_current_dct_index(i3c_mst_dev_t *dev)
{
    return dev->device_table.present_dct_index;
}

/**
 * @brief Get the current DAT index
 *
 * @param dev I3C master hardware instance
 * @return Current DAT index
 */
static inline uint32_t i3c_master_ll_get_current_dat_index(i3c_mst_dev_t *dev)
{
    return dev->device_table.present_dat_index;
}

#ifdef __cplusplus
}
#endif
