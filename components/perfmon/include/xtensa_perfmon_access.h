/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _PERF_MON_ACCESS_H_
#define _PERF_MON_ACCESS_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**@{*/
/**
 * @brief      Init Performance Monitoor
 *
 * Initialize performance monitor register with define values
 *
 * @param[in] id: performance counter number
 * @param[in] select: select value from PMCTRLx register
 * @param[in] mask: mask value from PMCTRLx register
 * @param[in] kernelcnt: kernelcnt value from PMCTRLx register
 * @param[in] tracelevel: tracelevel value from PMCTRLx register
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if one of the arguments is not correct
 */
esp_err_t xtensa_perfmon_init(int id, uint16_t select, uint16_t mask, int kernelcnt, int tracelevel);
/**@}*/

/**@{*/
/**
 * @brief      Reset PM counter
 *
 * Reset PM counter. Writes 0 to the PMx register.
 *
 * @param[in] id: performance counter number
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if id out of range
 */
esp_err_t xtensa_perfmon_reset(int id);
/**@}*/

/**@{*/
/**
 * @brief      Start PM counters
 *
 * Start all PM counters synchronously. Write 1 to the PGM register
 */
void xtensa_perfmon_start(void);
/**@}*/

/**@{*/
/**
 * @brief      Stop PM counters
 *
 * Stop all PM counters synchronously. Write 0 to the PGM register
 */
void xtensa_perfmon_stop(void);
/**@}*/

/**@{*/
/**
 * @brief      Read PM counter
 *
 * Read value of defined PM counter.
 *
 * @param[in] id: performance counter number
 *
 * @return
 *      - Performance counter value
 */
uint32_t xtensa_perfmon_value(int id);
/**@}*/

/**@{*/
/**
 * @brief      Read PM overflow state
 *
 * Read overflow value of defined PM counter.
 *
 * @param[in] id: performance counter number
 *
 * @return
 *      - ESP_OK if there is no overflow (overflow = 0)
 *      - ESP_FAIL if overflow occure (overflow = 1)
 */
esp_err_t xtensa_perfmon_overflow(int id);
/**@}*/

/**@{*/
/**
 * @brief      Dump PM values
 *
 * Dump all PM register to the console.
 *
 */
void xtensa_perfmon_dump(void);
/**@}*/


#ifdef __cplusplus
}
#endif

#endif // _PERF_MON_ACCESS_H_
