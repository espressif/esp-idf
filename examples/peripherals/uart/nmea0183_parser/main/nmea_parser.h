/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
#include "driver/uart.h"

#define GPS_MAX_SATELLITES_IN_USE (12)
#define GPS_MAX_SATELLITES_IN_VIEW (16)

/**
 * @brief Declare of NMEA Parser Event base
 *
 */
ESP_EVENT_DECLARE_BASE(ESP_NMEA_EVENT);

/**
 * @brief GPS fix type
 *
 */
typedef enum {
    GPS_FIX_INVALID, /*!< Not fixed */
    GPS_FIX_GPS,     /*!< GPS */
    GPS_FIX_DGPS,    /*!< Differential GPS */
} gps_fix_t;

/**
 * @brief GPS fix mode
 *
 */
typedef enum {
    GPS_MODE_INVALID = 1, /*!< Not fixed */
    GPS_MODE_2D,          /*!< 2D GPS */
    GPS_MODE_3D           /*!< 3D GPS */
} gps_fix_mode_t;

/**
 * @brief GPS satellite information
 *
 */
typedef struct {
    uint8_t num;       /*!< Satellite number */
    uint8_t elevation; /*!< Satellite elevation */
    uint16_t azimuth;  /*!< Satellite azimuth */
    uint8_t snr;       /*!< Satellite signal noise ratio */
} gps_satellite_t;

/**
 * @brief GPS time
 *
 */
typedef struct {
    uint8_t hour;      /*!< Hour */
    uint8_t minute;    /*!< Minute */
    uint8_t second;    /*!< Second */
    uint16_t thousand; /*!< Thousand */
} gps_time_t;

/**
 * @brief GPS date
 *
 */
typedef struct {
    uint8_t day;   /*!< Day (start from 1) */
    uint8_t month; /*!< Month (start from 1) */
    uint16_t year; /*!< Year (start from 2000) */
} gps_date_t;

/**
 * @brief NMEA Statement
 *
 */
typedef enum {
    STATEMENT_UNKNOWN = 0, /*!< Unknown statement */
    STATEMENT_GGA,         /*!< GGA */
    STATEMENT_GSA,         /*!< GSA */
    STATEMENT_RMC,         /*!< RMC */
    STATEMENT_GSV,         /*!< GSV */
    STATEMENT_GLL,         /*!< GLL */
    STATEMENT_VTG          /*!< VTG */
} nmea_statement_t;

/**
 * @brief GPS object
 *
 */
typedef struct {
    float latitude;                                                /*!< Latitude (degrees) */
    float longitude;                                               /*!< Longitude (degrees) */
    float altitude;                                                /*!< Altitude (meters) */
    gps_fix_t fix;                                                 /*!< Fix status */
    uint8_t sats_in_use;                                           /*!< Number of satellites in use */
    gps_time_t tim;                                                /*!< time in UTC */
    gps_fix_mode_t fix_mode;                                       /*!< Fix mode */
    uint8_t sats_id_in_use[GPS_MAX_SATELLITES_IN_USE];             /*!< ID list of satellite in use */
    float dop_h;                                                   /*!< Horizontal dilution of precision */
    float dop_p;                                                   /*!< Position dilution of precision  */
    float dop_v;                                                   /*!< Vertical dilution of precision  */
    uint8_t sats_in_view;                                          /*!< Number of satellites in view */
    gps_satellite_t sats_desc_in_view[GPS_MAX_SATELLITES_IN_VIEW]; /*!< Information of satellites in view */
    gps_date_t date;                                               /*!< Fix date */
    bool valid;                                                    /*!< GPS validity */
    float speed;                                                   /*!< Ground speed, unit: m/s */
    float cog;                                                     /*!< Course over ground */
    float variation;                                               /*!< Magnetic variation */
} gps_t;

/**
 * @brief Configuration of NMEA Parser
 *
 */
typedef struct {
    struct {
        uart_port_t uart_port;        /*!< UART port number */
        uint32_t rx_pin;              /*!< UART Rx Pin number */
        uint32_t baud_rate;           /*!< UART baud rate */
        uart_word_length_t data_bits; /*!< UART data bits length */
        uart_parity_t parity;         /*!< UART parity */
        uart_stop_bits_t stop_bits;   /*!< UART stop bits length */
        uint32_t event_queue_size;    /*!< UART event queue size */
    } uart;                           /*!< UART specific configuration */
} nmea_parser_config_t;

/**
 * @brief NMEA Parser Handle
 *
 */
typedef void *nmea_parser_handle_t;

/**
 * @brief Default configuration for NMEA Parser
 *
 */
#define NMEA_PARSER_CONFIG_DEFAULT()              \
    {                                             \
        .uart = {                                 \
            .uart_port = UART_NUM_1,              \
            .rx_pin = CONFIG_NMEA_PARSER_UART_RXD,\
            .baud_rate = 9600,                    \
            .data_bits = UART_DATA_8_BITS,        \
            .parity = UART_PARITY_DISABLE,        \
            .stop_bits = UART_STOP_BITS_1,        \
            .event_queue_size = 16                \
        }                                         \
    }

/**
 * @brief NMEA Parser Event ID
 *
 */
typedef enum {
    GPS_UPDATE, /*!< GPS information has been updated */
    GPS_UNKNOWN /*!< Unknown statements detected */
} nmea_event_id_t;

/**
 * @brief Init NMEA Parser
 *
 * @param config Configuration of NMEA Parser
 * @return nmea_parser_handle_t handle of NMEA parser
 */
nmea_parser_handle_t nmea_parser_init(const nmea_parser_config_t *config);

/**
 * @brief Deinit NMEA Parser
 *
 * @param nmea_hdl handle of NMEA parser
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t nmea_parser_deinit(nmea_parser_handle_t nmea_hdl);

/**
 * @brief Add user defined handler for NMEA parser
 *
 * @param nmea_hdl handle of NMEA parser
 * @param event_handler user defined event handler
 * @param handler_args handler specific arguments
 * @return esp_err_t
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for the handler
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t nmea_parser_add_handler(nmea_parser_handle_t nmea_hdl, esp_event_handler_t event_handler, void *handler_args);

/**
 * @brief Remove user defined handler for NMEA parser
 *
 * @param nmea_hdl handle of NMEA parser
 * @param event_handler user defined event handler
 * @return esp_err_t
 *  - ESP_OK: Success
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t nmea_parser_remove_handler(nmea_parser_handle_t nmea_hdl, esp_event_handler_t event_handler);

#ifdef __cplusplus
}
#endif
