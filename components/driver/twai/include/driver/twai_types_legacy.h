/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/gpio_types.h"
#include "hal/twai_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------- Default initializers and flags ---------------------- */

/** @cond */    //Doxy command to hide preprocessor definitions from docs

/**
 * @brief Initializer macro for general configuration structure.
 *
 * This initializer macros allows the controller ID, TX GPIO, RX GPIO, and operating
 * mode to be configured. The other members of the general configuration structure are
 * assigned default values.
 */
#define TWAI_GENERAL_CONFIG_DEFAULT_V2(controller_num, tx_io_num, rx_io_num, op_mode) {.controller_id = controller_num,             \
                                                                    .mode = op_mode, .tx_io = tx_io_num, .rx_io = rx_io_num,        \
                                                                    .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED,      \
                                                                    .tx_queue_len = 5, .rx_queue_len = 5,                           \
                                                                    .alerts_enabled = TWAI_ALERT_NONE,  .clkout_divider = 0,        \
                                                                    .intr_flags = ESP_INTR_FLAG_LEVEL1, .general_flags = {0}}

/**
 * @brief Initializer macro for general configuration structure.
 *
 * This initializer macros allows the TX GPIO, RX GPIO, and operating mode to be
 * configured. Controller ID is set to 0 and he other members of the general configuration
 * structure are assigned default values.
 */
#define TWAI_GENERAL_CONFIG_DEFAULT(tx_io_num, rx_io_num, op_mode) TWAI_GENERAL_CONFIG_DEFAULT_V2(0, tx_io_num, rx_io_num, op_mode)

/**
 * @brief   Alert flags
 *
 * The following flags represents the various kind of alerts available in
 * the TWAI driver. These flags can be used when configuring/reconfiguring
 * alerts, or when calling twai_read_alerts().
 *
 * @note    The TWAI_ALERT_AND_LOG flag is not an actual alert, but will configure
 *          the TWAI driver to log to UART when an enabled alert occurs.
 */
#define TWAI_ALERT_TX_IDLE                  0x00000001  /**< Alert(1): No more messages to transmit */
#define TWAI_ALERT_TX_SUCCESS               0x00000002  /**< Alert(2): The previous transmission was successful */
#define TWAI_ALERT_RX_DATA                  0x00000004  /**< Alert(4): A frame has been received and added to the RX queue */
#define TWAI_ALERT_BELOW_ERR_WARN           0x00000008  /**< Alert(8): Both error counters have dropped below error warning limit */
#define TWAI_ALERT_ERR_ACTIVE               0x00000010  /**< Alert(16): TWAI controller has become error active */
#define TWAI_ALERT_RECOVERY_IN_PROGRESS     0x00000020  /**< Alert(32): TWAI controller is undergoing bus recovery */
#define TWAI_ALERT_BUS_RECOVERED            0x00000040  /**< Alert(64): TWAI controller has successfully completed bus recovery */
#define TWAI_ALERT_ARB_LOST                 0x00000080  /**< Alert(128): The previous transmission lost arbitration */
#define TWAI_ALERT_ABOVE_ERR_WARN           0x00000100  /**< Alert(256): One of the error counters have exceeded the error warning limit */
#define TWAI_ALERT_BUS_ERROR                0x00000200  /**< Alert(512): A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus */
#define TWAI_ALERT_TX_FAILED                0x00000400  /**< Alert(1024): The previous transmission has failed (for single shot transmission) */
#define TWAI_ALERT_RX_QUEUE_FULL            0x00000800  /**< Alert(2048): The RX queue is full causing a frame to be lost */
#define TWAI_ALERT_ERR_PASS                 0x00001000  /**< Alert(4096): TWAI controller has become error passive */
#define TWAI_ALERT_BUS_OFF                  0x00002000  /**< Alert(8192): Bus-off condition occurred. TWAI controller can no longer influence bus */
#define TWAI_ALERT_RX_FIFO_OVERRUN          0x00004000  /**< Alert(16384): An RX FIFO overrun has occurred */
#define TWAI_ALERT_TX_RETRIED               0x00008000  /**< Alert(32768): An message transmission was cancelled and retried due to an errata workaround */
#define TWAI_ALERT_PERIPH_RESET             0x00010000  /**< Alert(65536): The TWAI controller was reset */
#define TWAI_ALERT_ALL                      0x0001FFFF  /**< Bit mask to enable all alerts during configuration */
#define TWAI_ALERT_NONE                     0x00000000  /**< Bit mask to disable all alerts during configuration */
#define TWAI_ALERT_AND_LOG                  0x00020000  /**< Bit mask to enable alerts to also be logged when they occur */

/** @endcond */

#define TWAI_IO_UNUSED                   ((gpio_num_t) -1)   /**< Marks GPIO as unused in TWAI configuration */

/* ----------------------- Enum and Struct Definitions ---------------------- */

/**
 * @brief TWAI controller handle
 */
typedef struct twai_obj_t *twai_handle_t;

/**
 * @brief   TWAI driver states
 */
typedef enum {
    TWAI_STATE_STOPPED,             /**< Stopped state. The TWAI controller will not participate in any TWAI bus activities */
    TWAI_STATE_RUNNING,             /**< Running state. The TWAI controller can transmit and receive messages */
    TWAI_STATE_BUS_OFF,             /**< Bus-off state. The TWAI controller cannot participate in bus activities until it has recovered */
    TWAI_STATE_RECOVERING,          /**< Recovering state. The TWAI controller is undergoing bus recovery */
} twai_state_t;

/**
 * @brief   Structure for general configuration of the TWAI driver
 *
 * @note    Macro initializers are available for this structure
 */
typedef struct {
    int controller_id;              /**< TWAI controller ID, index from 0.
                                         If you want to install TWAI driver with a non-zero controller_id, please use `twai_driver_install_v2` */
    twai_mode_t mode;               /**< Mode of TWAI controller */
    gpio_num_t tx_io;               /**< Transmit GPIO number */
    gpio_num_t rx_io;               /**< Receive GPIO number */
    gpio_num_t clkout_io;           /**< CLKOUT GPIO number (optional, set to -1 if unused) */
    gpio_num_t bus_off_io;          /**< Bus off indicator GPIO number (optional, set to -1 if unused) */
    uint32_t tx_queue_len;          /**< Number of messages TX queue can hold (set to 0 to disable TX Queue) */
    uint32_t rx_queue_len;          /**< Number of messages RX queue can hold */
    uint32_t alerts_enabled;        /**< Bit field of alerts to enable (see documentation) */
    uint32_t clkout_divider;        /**< CLKOUT divider. Can be 1 or any even number from 2 to 14 (optional, set to 0 if unused) */
    int intr_flags;                 /**< Interrupt flags to set the priority of the driver's ISR. Note that to use the ESP_INTR_FLAG_IRAM, the CONFIG_TWAI_ISR_IN_IRAM option should be enabled first. */
    struct {
        uint32_t sleep_allow_pd;    /**< Set to allow power down. When this flag set, the driver will backup/restore the TWAI registers before/after entering/exist sleep mode.
                                         By this approach, the system can power off TWAI's power domain.
                                         This can save power, but at the expense of more RAM being consumed. */
    } general_flags;                /**< General flags */
    // Ensure TWAI_GENERAL_CONFIG_DEFAULT_V2 is updated and in order if new fields are added
} twai_general_config_t;

/**
 * @brief   Structure to store status information of TWAI driver
 */
typedef struct {
    twai_state_t state;             /**< Current state of TWAI controller (Stopped/Running/Bus-Off/Recovery) */
    uint32_t msgs_to_tx;            /**< Number of messages queued for transmission or awaiting transmission completion */
    uint32_t msgs_to_rx;            /**< Number of messages in RX queue waiting to be read */
    uint32_t tx_error_counter;      /**< Current value of Transmit Error Counter */
    uint32_t rx_error_counter;      /**< Current value of Receive Error Counter */
    uint32_t tx_failed_count;       /**< Number of messages that failed transmissions */
    uint32_t rx_missed_count;       /**< Number of messages that were lost due to a full RX queue (or errata workaround if enabled) */
    uint32_t rx_overrun_count;      /**< Number of messages that were lost due to a RX FIFO overrun */
    uint32_t arb_lost_count;        /**< Number of instances arbitration was lost */
    uint32_t bus_error_count;       /**< Number of instances a bus error has occurred */
} twai_status_info_t;

#ifdef __cplusplus
}
#endif
