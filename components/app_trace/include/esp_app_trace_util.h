/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_APP_TRACE_UTIL_H_
#define ESP_APP_TRACE_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_timer.h"

/** Infinite waiting timeout */
#define ESP_APPTRACE_TMO_INFINITE               ((uint32_t)-1)

/** Structure which holds data necessary for measuring time intervals.
 *
 *  After initialization via esp_apptrace_tmo_init() user needs to call esp_apptrace_tmo_check()
 *  periodically to check timeout for expiration.
 */
typedef struct {
    int64_t   start;   ///< time interval start (in us)
    int64_t   tmo;     ///< timeout value (in us)
    int64_t   elapsed; ///< elapsed time (in us)
} esp_apptrace_tmo_t;

/**
 * @brief Initializes timeout structure.
 *
 * @param tmo       Pointer to timeout structure to be initialized.
 * @param user_tmo  Timeout value (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
*/
static inline void esp_apptrace_tmo_init(esp_apptrace_tmo_t *tmo, uint32_t user_tmo)
{
    tmo->start = esp_timer_get_time();
    tmo->tmo = user_tmo == ESP_APPTRACE_TMO_INFINITE ? (int64_t)-1 : (int64_t)user_tmo;
    tmo->elapsed = 0;
}

/**
 * @brief Checks timeout for expiration.
 *
 * @param tmo Pointer to timeout structure.
 *
 * @return number of remaining us till tmo.
 */
esp_err_t esp_apptrace_tmo_check(esp_apptrace_tmo_t *tmo);

static inline uint32_t esp_apptrace_tmo_remaining_us(esp_apptrace_tmo_t *tmo)
{
    return tmo->tmo != (int64_t)-1 ? (tmo->elapsed - tmo->tmo) : ESP_APPTRACE_TMO_INFINITE;
}

/** Tracing module synchronization lock */
typedef struct {
    spinlock_t mux;
    unsigned int_state;
} esp_apptrace_lock_t;

/**
 * @brief Initializes lock structure.
 *
 * @param lock Pointer to lock structure to be initialized.
 */
static inline void esp_apptrace_lock_init(esp_apptrace_lock_t *lock)
{
    portMUX_INITIALIZE(&lock->mux);
    lock->int_state = 0;
}

/**
 * @brief Tries to acquire lock in specified time period.
 *
 * @param lock Pointer to lock structure.
 * @param tmo  Pointer to timeout struct.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
esp_err_t esp_apptrace_lock_take(esp_apptrace_lock_t *lock, esp_apptrace_tmo_t *tmo);

/**
 * @brief Releases lock.
 *
 * @param lock Pointer to lock structure.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 */
esp_err_t esp_apptrace_lock_give(esp_apptrace_lock_t *lock);

/** Ring buffer control structure.
 *
 * @note For purposes of application tracing module if there is no enough space for user data and write pointer can be wrapped
 *       current ring buffer size can be temporarily shrinked in order to provide buffer with requested size.
 */
typedef struct {
    uint8_t *data;      ///< pointer to data storage
    volatile uint32_t size;      ///< size of data storage
    volatile uint32_t cur_size;  ///< current size of data storage
    volatile uint32_t rd;        ///< read pointer
    volatile uint32_t wr;        ///< write pointer
} esp_apptrace_rb_t;

/**
 * @brief Initializes ring buffer control  structure.
 *
 * @param rb   Pointer to ring buffer structure to be initialized.
 * @param data Pointer to buffer to be used as ring buffer's data storage.
 * @param size Size of buffer to be used as ring buffer's data storage.
 */
static inline void esp_apptrace_rb_init(esp_apptrace_rb_t *rb, uint8_t *data, uint32_t size)
{
    rb->data = data;
    rb->size = rb->cur_size = size;
    rb->rd = 0;
    rb->wr = 0;
}

/**
 * @brief Allocates memory chunk in ring buffer.
 *
 * @param rb   Pointer to ring buffer structure.
 * @param size Size of the memory to allocate.
 *
 * @return Pointer to the allocated memory or NULL in case of failure.
 */
uint8_t *esp_apptrace_rb_produce(esp_apptrace_rb_t *rb, uint32_t size);

/**
 * @brief Consumes memory chunk in ring buffer.
 *
 * @param rb   Pointer to ring buffer structure.
 * @param size Size of the memory to consume.
 *
 * @return Pointer to consumed memory chunk or NULL in case of failure.
 */
uint8_t *esp_apptrace_rb_consume(esp_apptrace_rb_t *rb, uint32_t size);

/**
 * @brief Gets size of memory which can consumed with single call to esp_apptrace_rb_consume().
 *
 * @param rb Pointer to ring buffer structure.
 *
 * @return Size of memory which can consumed.
 *
 * @note Due to read pointer wrapping returned size can be less then the total size of available data.
 */
uint32_t esp_apptrace_rb_read_size_get(esp_apptrace_rb_t *rb);

/**
 * @brief Gets size of memory which can produced with single call to esp_apptrace_rb_produce().
 *
 * @param rb Pointer to ring buffer structure.
 *
 * @return Size of memory which can produced.
 *
 * @note Due to write pointer wrapping returned size can be less then the total size of available data.
 */
uint32_t esp_apptrace_rb_write_size_get(esp_apptrace_rb_t *rb);

int esp_apptrace_log_lock(void);
void esp_apptrace_log_unlock(void);

#define ESP_APPTRACE_LOG( format, ... )   \
    do { \
        esp_apptrace_log_lock(); \
        esp_rom_printf(format, ##__VA_ARGS__); \
        esp_apptrace_log_unlock(); \
    } while(0)

#define ESP_APPTRACE_LOG_LEV( _L_, level, format, ... )   \
    do { \
        if (LOG_LOCAL_LEVEL >= level) { \
            ESP_APPTRACE_LOG(LOG_FORMAT(_L_, format), esp_log_early_timestamp(), TAG, ##__VA_ARGS__); \
        } \
    } while(0)

#define ESP_APPTRACE_LOGE( format, ... )  ESP_APPTRACE_LOG_LEV(E, ESP_LOG_ERROR, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGW( format, ... )  ESP_APPTRACE_LOG_LEV(W, ESP_LOG_WARN, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGI( format, ... )  ESP_APPTRACE_LOG_LEV(I, ESP_LOG_INFO, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGD( format, ... )  ESP_APPTRACE_LOG_LEV(D, ESP_LOG_DEBUG, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGV( format, ... )  ESP_APPTRACE_LOG_LEV(V, ESP_LOG_VERBOSE, format, ##__VA_ARGS__)
#define ESP_APPTRACE_LOGO( format, ... )  ESP_APPTRACE_LOG_LEV(E, ESP_LOG_NONE, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif //ESP_APP_TRACE_UTIL_H_
