/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I3C_MASTER_IBI_DATA_SIZE_MAX      (32)

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

/**
 * @brief I3C Common Command Code (CCC) enumeration
 *
 * This enumeration defines all I3C CCC command codes as per MIPI I3C specification.
 */
typedef enum {
    I3C_CCC_ENEC              = 0x00,    ///< Enable Events Command
    I3C_CCC_DISEC             = 0x01,    ///< Disable Events Command
    I3C_CCC_ENTAS0            = 0x02,    ///< Enable Target Activity State 0
    I3C_CCC_ENTAS1            = 0x03,    ///< Enable Target Activity State 1
    I3C_CCC_ENTAS2            = 0x04,    ///< Enable Target Activity State 2
    I3C_CCC_ENTAS3            = 0x05,    ///< Enable Target Activity State 3
    I3C_CCC_RSTDAA            = 0x06,    ///< Reset Dynamic Address Assignment
    I3C_CCC_ENTDAA            = 0x07,    ///< Enter Dynamic Address Assignment
    I3C_CCC_DEFTGTS           = 0x08,    ///< Define List of Targets
    I3C_CCC_SETMWL            = 0x09,    ///< Set Max Write Length
    I3C_CCC_SETMRL            = 0x0A,    ///< Set Max Read Length
    I3C_CCC_ENTTM             = 0x0B,    ///< Enter Test Mode
    I3C_CCC_SETBUSCON         = 0x0C,    ///< Set Bus Context
    /* 0x0D Reserved */
    I3C_CCC_ENDXFER           = 0x0E,    ///< End Data Transfer
    I3C_CCC_ENTHDR0           = 0x20,    ///< Enter HDR Mode 0 (DDR)
    I3C_CCC_ENTHDR1           = 0x21,    ///< Enter HDR Mode 1 (TSP)
    I3C_CCC_ENTHDR2           = 0x22,    ///< Enter HDR Mode 2 (TSL)
    I3C_CCC_ENTHDR3           = 0x23,    ///< Enter HDR Mode 3
    I3C_CCC_ENTHDR4           = 0x24,    ///< Enter HDR Mode 4
    I3C_CCC_ENTHDR5           = 0x25,    ///< Enter HDR Mode 5
    I3C_CCC_ENTHDR6           = 0x26,    ///< Enter HDR Mode 6
    I3C_CCC_ENTHDR7           = 0x27,    ///< Enter HDR Mode 7
    I3C_CCC_SETXTIME          = 0x28,    ///< Set Exchange Timing Information
    I3C_CCC_SETAASA           = 0x29,    ///< Set All Addresses to Static Addresses
    I3C_CCC_RSTACT            = 0x2A,    ///< Target Reset Action
    I3C_CCC_DEFGRPA           = 0x2B,    ///< Define List of Group Address
    I3C_CCC_RSTGRPA           = 0x2C,    ///< Reset Group Address(es)
    I3C_CCC_MLANE             = 0x2D,    ///< Multi-Lane Data Transfer Control

    I3C_CCC_ENEC_DIRECT       = 0x80,    ///< Enable Events Command (Direct)
    I3C_CCC_DISEC_DIRECT      = 0x81,    ///< Disable Events Command (Direct)
    I3C_CCC_ENTAS0_DIRECT     = 0x82,    ///< Enable Target Activity State 0 (Direct)
    I3C_CCC_ENTAS1_DIRECT     = 0x83,    ///< Enable Target Activity State 1 (Direct)
    I3C_CCC_ENTAS2_DIRECT     = 0x84,    ///< Enable Target Activity State 2 (Direct)
    I3C_CCC_ENTAS3_DIRECT     = 0x85,    ///< Enable Target Activity State 3 (Direct)
    I3C_CCC_RSTDAA_DIRECT     = 0x86,    ///< Reset Dynamic Address Assignment (Direct)
    I3C_CCC_SETDASA           = 0x87,    ///< Set Dynamic Address from Static Address
    I3C_CCC_SETNEWDA          = 0x88,    ///< Set New Dynamic Address
    I3C_CCC_SETMWL_DIRECT     = 0x89,    ///< Set Max Write Length (Direct)
    I3C_CCC_SETMRL_DIRECT     = 0x8A,    ///< Set Max Read Length (Direct)
    I3C_CCC_GETMWL            = 0x8B,    ///< Get Max Write Length
    I3C_CCC_GETMRL            = 0x8C,    ///< Get Max Read Length
    I3C_CCC_GETPID            = 0x8D,    ///< Get Provisional ID
    I3C_CCC_GETBCR            = 0x8E,    ///< Get Bus Characteristics Register
    I3C_CCC_GETDCR            = 0x8F,    ///< Get Device Characteristics Register
    I3C_CCC_GETSTATUS         = 0x90,    ///< Get Device Status
    I3C_CCC_GETACCMST         = 0x91,    ///< Get Accept Controller Role
    I3C_CCC_SETBRGTGT         = 0x93,    ///< Set Bridge Targets
    I3C_CCC_GETMXDS           = 0x94,    ///< Get Max Data Speed
    I3C_CCC_GETCAPS           = 0x95,    ///< Get Optional Feature Capabilities
    I3C_CCC_SETROUTE          = 0x96,    ///< Set Route
    I3C_CCC_D2DXFER           = 0x97,    ///< Device to Device(s) Tunneling Control
    I3C_CCC_SETXTIME_DIRECT   = 0x98,    ///< Set Exchange Timing Information (Direct)
    I3C_CCC_GETXTIME          = 0x99,    ///< Get Exchange Timing Information
    I3C_CCC_RSTACT_DIRECT     = 0x9A,    ///< Target Reset Action (Direct)
    I3C_CCC_SETGRPA           = 0x9B,    ///< Set Group Address
    I3C_CCC_RSTGRPA_DIRECT    = 0x9C,    ///< Reset Group Address (Direct)
    I3C_CCC_MLANE_DIRECT      = 0x9D,    ///< Multi-Lane Data Transfer Control (Direct)
} i3c_ccc_code_t;

/**
 * @brief I3C internal pull-up resistor value
 */
typedef enum {
    I3C_MASTER_INTERNAL_PULLUP_RESISTOR_DISABLED = 0, /*!< Internal pull-up resistor disabled */
    I3C_MASTER_INTERNAL_PULLUP_RESISTOR_0_3K = 1, /*!< Internal pull-up resistor 0.3K */
    I3C_MASTER_INTERNAL_PULLUP_RESISTOR_0_6K = 2, /*!< Internal pull-up resistor 0.6K */
    I3C_MASTER_INTERNAL_PULLUP_RESISTOR_1_2K = 3, /*!< Internal pull-up resistor 1.2K */
    I3C_MASTER_INTERNAL_PULLUP_RESISTOR_2_4K = 4, /*!< Internal pull-up resistor 2.4K */
} i3c_master_internal_pullup_resistor_val_t;

#ifdef __cplusplus
}
#endif
