/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdatomic.h>
#include <sys/queue.h>
#include "esp_err.h"
#include "driver/i2c_types.h"
#include "hal/i2c_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "driver/i2c_slave.h"
#include "esp_private/periph_ctrl.h"
#include "esp_pm.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PERIPH_CLK_CTRL_SHARED
#define I2C_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I2C_CLOCK_SRC_ATOMIC()
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define I2C_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I2C_RCC_ATOMIC()
#endif

#if SOC_LP_I2C_SUPPORTED
#define LP_I2C_SRC_CLK_ATOMIC()    PERIPH_RCC_ATOMIC()
#define LP_I2C_BUS_CLK_ATOMIC()    PERIPH_RCC_ATOMIC()
#endif

#if CONFIG_I2C_ISR_IRAM_SAFE
#define I2C_MEM_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define I2C_MEM_ALLOC_CAPS     (MALLOC_CAP_DEFAULT)
#endif

// I2C driver object is per-mode, the interrupt source is shared between modes
#if CONFIG_I2C_ISR_IRAM_SAFE
#define I2C_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LOWMED)
#else
#define I2C_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED)
#endif

// Use retention link only when the target supports sleep retention and PM is enabled
#define I2C_USE_RETENTION_LINK  (SOC_I2C_SUPPORT_SLEEP_RETENTION && CONFIG_PM_ENABLE && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

#define I2C_ALLOW_INTR_PRIORITY_MASK ESP_INTR_FLAG_LOWMED

#define I2C_PM_LOCK_NAME_LEN_MAX 16
#define I2C_STATIC_OPERATION_ARRAY_MAX 6

#define ACK_VAL 0
#define NACK_VAL 1

#define I2C_TRANS_READ_COMMAND(ack_value)    {.ack_val = (ack_value), .op_code = I2C_LL_CMD_READ}
#define I2C_TRANS_WRITE_COMMAND(ack_check)   {.ack_en = (ack_check), .op_code = I2C_LL_CMD_WRITE}
#define I2C_TRANS_STOP_COMMAND               {.op_code = I2C_LL_CMD_STOP}
#define I2C_TRANS_START_COMMAND              {.op_code = I2C_LL_CMD_RESTART}

typedef struct i2c_bus_t i2c_bus_t;
typedef struct i2c_master_bus_t i2c_master_bus_t;
typedef struct i2c_bus_t *i2c_bus_handle_t;
typedef struct i2c_master_dev_t i2c_master_dev_t;
typedef struct i2c_slave_dev_t i2c_slave_dev_t;

typedef enum {
    I2C_BUS_MODE_MASTER = 0,
    I2C_BUS_MODE_SLAVE = 1,
} i2c_bus_mode_t;

typedef enum {
    I2C_SLAVE_FIFO = 0,
    I2C_SLAVE_NONFIFO = 1,
} i2c_slave_fifo_mode_t;

enum {
    I2C_TRANS_QUEUE_READY,
    I2C_TRANS_QUEUE_PROGRESS,
    I2C_TRANS_QUEUE_COMPLETE,
    I2C_TRANS_QUEUE_MAX,
};

typedef struct {
    i2c_ll_hw_cmd_t hw_cmd;    // I2C command, defined by hardware
    uint8_t *data; // Pointer to data
    uint16_t bytes_used; // I2C data has been used
    size_t total_bytes; // Total bytes to be transferred
} i2c_operation_t;

typedef struct {
    uint32_t device_address; // Address of I2C device
    i2c_operation_t *ops; // Pointer to I2C operation structure
    size_t cmd_count; // Record how many I2C hardware commands in one transaction
} i2c_transaction_t;

struct i2c_bus_t {
    i2c_port_num_t port_num; // Port(Bus) ID, index from 0
    bool is_lp_i2c;        // true if current port is lp_i2c. false is hp_i2c
    portMUX_TYPE spinlock; // To protect pre-group register level concurrency access
    i2c_hal_context_t hal; // Hal layer for each port(bus)
    soc_module_clk_t clk_src; // Record the port clock source
    uint32_t clk_src_freq_hz; // Record the clock source frequency
    int sda_num; // SDA pin number
    int scl_num; // SCL pin number
    bool pull_up_enable; // Enable pull-ups
    intr_handle_t intr_handle; // I2C interrupt handle
    esp_pm_lock_handle_t pm_lock; // power manage lock
#if CONFIG_PM_ENABLE
    char pm_lock_name[I2C_PM_LOCK_NAME_LEN_MAX]; // pm lock name
#endif
    i2c_bus_mode_t bus_mode; // I2C bus mode
#if SOC_I2C_SUPPORT_SLEEP_RETENTION
    bool retention_link_created;     // mark if the retention link is created.
#endif
};

typedef struct i2c_master_device_list {
    i2c_master_dev_t *device;
    SLIST_ENTRY(i2c_master_device_list) next;
} i2c_master_device_list_t;

struct i2c_master_bus_t {
    i2c_bus_t *base;                                                 // bus base class
    SemaphoreHandle_t bus_lock_mux;                                  // semaphore to lock bus process
    int cmd_idx;                                                     //record current command index, for master mode
    _Atomic i2c_master_status_t status;                              // record current command status, for master mode
    i2c_master_event_t event;                                        // record current i2c bus event
    int rx_cnt;                                                      // record current read index, for master mode
    i2c_transaction_t i2c_trans;                                     // Pointer to I2C transfer structure
    i2c_operation_t i2c_ops[I2C_STATIC_OPERATION_ARRAY_MAX];         // I2C operation array
    _Atomic uint16_t trans_idx;                                      // Index of I2C transaction command.
    SemaphoreHandle_t cmd_semphr;                                    // Semaphore between task and interrupt, using for synchronizing ISR and I2C task.
    QueueHandle_t event_queue;                                       // I2C event queue
    uint32_t read_buf_pos;                                           // Read buffer position
    bool contains_read;                                              // Whether command array includes read operation, true: yes, otherwise, false.
    uint32_t read_len_static;                                        // Read static buffer length
    uint32_t w_r_size;                                               // The size send/receive last time.
    bool trans_over_buffer;                                          // Data length is more than hardware fifo length, needs interrupt.
    bool async_trans;                                                // asynchronous transaction, true after callback is installed.
    bool ack_check_disable;                                          // Disable ACK check
    volatile bool trans_done;                                        // transaction command finish
    bool bypass_nack_log;                                             // Bypass the error log. Sometimes the error is expected.
    SLIST_HEAD(i2c_master_device_list_head, i2c_master_device_list) device_list;      // I2C device (instance) list
    // async trans members
    bool async_break;                                                // break transaction loop flag.
    i2c_addr_bit_len_t addr_10bits_bus;                              // Slave address is 10 bits.
    size_t queue_size;                                               // I2C transaction queue size.
    size_t num_trans_inflight;                                       // Indicates the number of transactions that are undergoing but not recycled to ready_queue
    size_t num_trans_inqueue;                                        // Indicates the number of transaction in queue transaction.
    void* queues_storage;                                            // storage of transaction queues
    bool sent_all;                                                   // true if the queue transaction is sent
    bool in_progress;                                                // true if current transaction is in progress
    bool trans_finish;                                               // true if current command has been sent out.
    bool queue_trans;                                                // true if current transaction is in queue
    bool new_queue;                                                  // true if allow a new queue transaction
    QueueHandle_t trans_queues[I2C_TRANS_QUEUE_MAX];                 // transaction queues.
    StaticQueue_t trans_queue_structs[I2C_TRANS_QUEUE_MAX];          // memory to store the static structure for trans_queues
    i2c_operation_t (*i2c_async_ops)[I2C_STATIC_OPERATION_ARRAY_MAX]; // pointer to asynchronous operation(s).
    uint32_t ops_prepare_idx;                                        // Index for the operations can be written into `i2c_async_ops` array.
    uint32_t ops_cur_size;                                           // Indicates how many operations have already put in `i2c_async_ops`.
    i2c_transaction_t i2c_trans_pool[];                              // I2C transaction pool.
};

struct i2c_master_dev_t {
    i2c_master_bus_t *master_bus;         // I2C master bus base class
    uint16_t device_address;              // I2C device address
    uint32_t scl_speed_hz;                // SCL clock frequency
    uint32_t scl_wait_us;                // SCL await time (unit:us)
    i2c_addr_bit_len_t addr_10bits;       // Whether I2C device is a 10-bits address device.
    bool ack_check_disable;               // Disable ACK check
    i2c_master_callback_t on_trans_done;  // I2C master transaction done callback.
    void *user_ctx;                       // Callback user context
};

typedef struct {
    bool trans_complete;  // Event of transaction complete
    bool slave_stretch;   // Event of slave stretch happens
    bool addr_unmatch;    // Event of address unmatched
} i2c_slave_evt_t;

typedef struct {
    uint8_t *buffer;            // Pointer to the buffer need to be received in ISR
    uint32_t rcv_fifo_cnt;      // receive fifo count.
} i2c_slave_receive_t;

#if !CONFIG_I2C_ENABLE_SLAVE_DRIVER_VERSION_2

struct i2c_slave_dev_t {
    i2c_bus_t *base;                            // bus base class
    SemaphoreHandle_t slv_rx_mux;               // Mutex for slave rx direction
    SemaphoreHandle_t slv_tx_mux;               // Mutex for slave tx direction
    RingbufHandle_t rx_ring_buf;                // Handle for rx ringbuffer
    RingbufHandle_t tx_ring_buf;                // Handle for tx ringbuffer
    uint8_t data_buf[SOC_I2C_FIFO_LEN];         // Data buffer for slave
    uint32_t trans_data_length;                 // Send data length
    i2c_slave_event_callbacks_t callbacks;      // I2C slave callbacks
    void *user_ctx;                             // Callback user context
    i2c_slave_fifo_mode_t fifo_mode;            // Slave fifo mode.
    QueueHandle_t slv_evt_queue;                // Event Queue used in slave nonfifo mode.
    i2c_slave_evt_t slave_evt;                  // Slave event structure.
    i2c_slave_receive_t receive_desc;           // Slave receive descriptor
    uint32_t already_receive_len;               // Data length already received in ISR.
};

#else // CONFIG_I2C_ENABLE_SLAVE_DRIVER_VERSION_2

struct i2c_slave_dev_t {
    i2c_bus_t *base;                                  // bus base class
    SemaphoreHandle_t operation_mux;                  // Mux for i2c slave operation
    i2c_slave_request_callback_t request_callback;    // i2c slave request callback
    i2c_slave_received_callback_t receive_callback;   // i2c_slave receive callback
    void *user_ctx;                                   // Callback user context
    RingbufHandle_t rx_ring_buf;                      // receive ringbuffer
    RingbufHandle_t tx_ring_buf;                      // transmit ringbuffer
    uint32_t rx_data_count;                           // receive data count
    i2c_slave_receive_t receive_desc;                 // slave receive descriptor
};

#endif // CONFIG_I2C_ENABLE_SLAVE_DRIVER_VERSION_2

/**
 * @brief Acquire I2C bus handle
 *
 * @param port_num I2C port number.
 * @return
 *      - ESP_OK: Acquire bus handle successfully.
 *      - ESP_ERR_INVALID_ARG: Argument error.
 *      - ESP_ERR_INVALID_STATE: Acquire bus invalid state because bus has already acquired.
 */
esp_err_t i2c_acquire_bus_handle(i2c_port_num_t port_num, i2c_bus_handle_t *i2c_new_bus, i2c_bus_mode_t mode);

/**
 * @brief Release I2C bus handle
 *
 * @param i2c_bus I2C bus handle, returned from `i2c_acquire_bus_handle`
 * @return ESP_OK: If release successfully
 *         ESP_ERR_INVALID_STATE: Release bus failed because same bus has been required several times.
 *         Otherwise: Other reasons.
 */
esp_err_t i2c_release_bus_handle(i2c_bus_handle_t i2c_bus);

/**
 * @brief Set clock source for I2C peripheral
 *
 * @param handle I2C bus handle
 * @param clk_src Clock source
 * @return
 *      - ESP_OK: Set clock source successfully
 *      - ESP_ERR_NOT_SUPPORTED: Set clock source failed because the clk_src is not supported
 *      - ESP_ERR_INVALID_STATE: Set clock source failed because the clk_src is different from other I2C controller
 *      - ESP_FAIL: Set clock source failed because of other error
 */
esp_err_t i2c_select_periph_clock(i2c_bus_handle_t handle, soc_module_clk_t clk_src);

/**
 * @brief Set I2C SCL/SDA pins
 *
 * @param handle I2C bus handle
 * @return
 *      - ESP_OK: I2C set SCL/SDA pins successfully.
 *      - ESP_ERR_INVALID_ARG: Argument error.
 *      - Otherwise: Set SCL/SDA IOs error.
 */
esp_err_t i2c_common_set_pins(i2c_bus_handle_t handle);

/**
 * @brief Deinit I2C SCL/SDA pins
 *
 * @param handle I2C bus handle
 * @return
 *      - ESP_OK: I2C set SCL/SDA pins successfully.
 *      - ESP_ERR_INVALID_ARG: Argument error.
 *      - Otherwise: Set SCL/SDA IOs error.
 */
esp_err_t i2c_common_deinit_pins(i2c_bus_handle_t handle);

/**
 * @brief Check whether bus is acquired
 *
 * @param port_num number of port
 * @return true if the bus is occupied, false if the bus is not occupied.
*/
bool i2c_bus_occupied(i2c_port_num_t port_num);

/**
 * @brief Create sleep retention link
 *
 * @param handle I2C bus handle
 */
void i2c_create_retention_module(i2c_bus_handle_t handle);

#ifdef __cplusplus
}
#endif
