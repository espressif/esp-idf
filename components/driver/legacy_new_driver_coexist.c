/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief This count is used to prevent the coexistence of
 *        the legacy timer group driver (deprecated/driver/timer.h) and the new gptimer driver (driver/gptimer.h).
 */
int timer_group_driver_init_count = 0;

/**
 * @brief This count is used to prevent the coexistence of
 *        the legacy pcnt driver (deprecated/driver/pcnt.h) and the new pulse_cnt driver (driver/pulse_cnt.h).
 */
int pcnt_driver_init_count = 0;

/**
 * @brief This count is used to prevent the coexistence of
 *        the legacy temperature sensor driver (deprecated/driver/temp_sensor.h) and the new temperature sensor driver (driver/temperature_sensor.h).
 */
int temp_sensor_driver_init_count = 0;
