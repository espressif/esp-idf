/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#warning driver/can.h is deprecated, please use driver/twai.h instead

#include "hal/can_types.h"
#include "driver/twai.h"

/* ---------------------------- Compatibility ------------------------------- */

#define CAN_GENERAL_CONFIG_DEFAULT(tx_io_num, rx_io_num, op_mode)   TWAI_GENERAL_CONFIG_DEFAULT(tx_io_num, rx_io_num, op_mode)

#define CAN_ALERT_TX_IDLE               TWAI_ALERT_TX_IDLE
#define CAN_ALERT_TX_SUCCESS            TWAI_ALERT_TX_SUCCESS
#define CAN_ALERT_BELOW_ERR_WARN        TWAI_ALERT_BELOW_ERR_WARN
#define CAN_ALERT_ERR_ACTIVE            TWAI_ALERT_ERR_ACTIVE
#define CAN_ALERT_RECOVERY_IN_PROGRESS  TWAI_ALERT_RECOVERY_IN_PROGRESS
#define CAN_ALERT_BUS_RECOVERED         TWAI_ALERT_BUS_RECOVERED
#define CAN_ALERT_ARB_LOST              TWAI_ALERT_ARB_LOST
#define CAN_ALERT_ABOVE_ERR_WARN        TWAI_ALERT_ABOVE_ERR_WARN
#define CAN_ALERT_BUS_ERROR             TWAI_ALERT_BUS_ERROR
#define CAN_ALERT_TX_FAILED             TWAI_ALERT_TX_FAILED
#define CAN_ALERT_RX_QUEUE_FULL         TWAI_ALERT_RX_QUEUE_FULL
#define CAN_ALERT_ERR_PASS              TWAI_ALERT_ERR_PASS
#define CAN_ALERT_BUS_OFF               TWAI_ALERT_BUS_OFF
#define CAN_ALERT_ALL                   TWAI_ALERT_ALL
#define CAN_ALERT_NONE                  TWAI_ALERT_NONE
#define CAN_ALERT_AND_LOG               TWAI_ALERT_AND_LOG

#define CAN_IO_UNUSED                   TWAI_IO_UNUSED

#define CAN_STATE_STOPPED               TWAI_STATE_STOPPED
#define CAN_STATE_RUNNING               TWAI_STATE_RUNNING
#define CAN_STATE_BUS_OFF               TWAI_STATE_BUS_OFF
#define CAN_STATE_RECOVERING            TWAI_STATE_RECOVERING

typedef twai_state_t                    can_state_t;
typedef twai_general_config_t           can_general_config_t;
typedef twai_status_info_t              can_status_info_t;


#define can_driver_install(g_config, t_config, f_config)        twai_driver_install(g_config, t_config, f_config)
#define can_driver_uninstall()                                  twai_driver_uninstall()
#define can_start()                                             twai_start()
#define can_stop()                                              twai_stop()
#define can_transmit(message, ticks_to_wait)                    twai_transmit(message, ticks_to_wait)
#define can_receive(message, ticks_to_wait)                     twai_receive(message, ticks_to_wait)
#define can_read_alerts(alerts, ticks_to_wait)                  twai_read_alerts(alerts, ticks_to_wait)
#define can_reconfigure_alerts(alerts_enabled, current_alerts)  twai_reconfigure_alerts(alerts_enabled, current_alerts)
#define can_initiate_recovery()                                 twai_initiate_recovery()
#define can_get_status_info(status_info)                        twai_get_status_info(status_info)
#define can_clear_transmit_queue()                              twai_clear_transmit_queue()
#define can_clear_receive_queue()                               twai_clear_receive_queue()

#ifdef __cplusplus
}
#endif
