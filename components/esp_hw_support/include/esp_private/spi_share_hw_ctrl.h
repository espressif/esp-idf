/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>

#include "soc/soc_caps.h"
#include "hal/spi_types.h"
#include "soc/spi_periph.h"
#include "soc/gpio_struct.h"
#include "esp_private/periph_ctrl.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define SPI_COMMON_RCC_CLOCK_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define SPI_COMMON_RCC_CLOCK_ATOMIC()
#endif

#define BUS_LOCK_DEBUG  0

#if BUS_LOCK_DEBUG
#define BUS_LOCK_DEBUG_EXECUTE_CHECK(x)  assert(x)
#else
#define BUS_LOCK_DEBUG_EXECUTE_CHECK(x)
#endif

#define CHECK_IOMUX_PIN(HOST, PIN_NAME) if (GPIO.func_in_sel_cfg[spi_periph_signal[(HOST)].PIN_NAME##_in].sig_in_sel) return false

struct spi_bus_lock_t;
struct spi_bus_lock_dev_t;
/// Handle to the lock of an SPI bus
typedef struct spi_bus_lock_t* spi_bus_lock_handle_t;
/// Handle to lock of one of the device on an SPI bus
typedef struct spi_bus_lock_dev_t* spi_bus_lock_dev_handle_t;
/// Background operation control function
typedef void (*bg_ctrl_func_t)(void*);

/**
 * @brief Try to claim a SPI peripheral
 *
 * Call this if your driver wants to manage a SPI peripheral.
 *
 * @param host Peripheral to claim
 * @param source The caller indentification string.
 *
 * @return True if peripheral is claimed successfully; false if peripheral already is claimed.
 */
bool spicommon_periph_claim(spi_host_device_t host, const char* source);

/**
 * @brief Check whether the spi periph is in use.
 *
 * @param host Peripheral to check.
 *
 * @return True if in use, otherwise false.
 */
bool spicommon_periph_in_use(spi_host_device_t host);

/**
 * @brief Return the SPI peripheral so another driver can claim it.
 *
 * @param host Peripheral to return
 *
 * @return True if peripheral is returned successfully; false if peripheral was free to claim already.
 */
bool spicommon_periph_free(spi_host_device_t host);

/*******************************************************************************
 * SPI Bus Lock for arbitration among SPI master (intr, polling) trans, SPI flash operations and
 * flash/psram cache access.
 *
 * NON-PUBLIC API. Don't use it directly in applications.
 *
 * There is the main lock corresponding to an SPI bus, of which several devices (holding child
 * locks) attaching to it. Each of the device is STRONGLY RECOMMENDED to be used in only one task
 * to avoid concurrency issues.
 *
 * Terms:
 * - BG operations (BackGround operations) means some transaction that will not immediately /
 *   explicitly be sent in the task. It can be some cache access, or interrupt transactions.
 *
 * - Operation: usage of the bus, for example, do SPI transactions.
 *
 * - Acquiring processor: the task or the ISR that is allowed to use the bus. No operations will be
 *   performed if there is no acquiring processor. A processor becomes the acquiring processor if
 *   it ask for that when no acquiring processor exist, otherwise it has to wait for the acquiring
 *   processor to handle over the role to it. The acquiring processor will and will only assign one
 *   acquiring processor in the waiting list (if not empty) when it finishes its operation.
 *
 * - Acquiring device: the only device allowed to use the bus. Operations can be performed in
 *   either the BG or the task. When there's no acquiring device, only the ISR is allowed to be the
 *   acquiring processor and perform operations on the bus.
 *
 * When a device wants to perform operations, it either:
 * 1. Acquire the bus, and operate in the task (e.g. polling transactions of SPI master, and SPI flash
 *    operations)
 *
 * 2. Request a BG operation. And the ISR will be enabled at proper time.
 *
 *    For example if a task wants to send an interrupt transaction, it prepares the data in the task,
 *    call `spi_bus_lock_bg_request`, and handle sending in the ISR.
 *
 * 3. When a device has already acquired the bus, BG operations are also allowed. After the
 *    `spi_bus_lock_bg_request` is called, call `spi_bus_lock_wait_bg_done` before operations in task
 *    again to wait until BG operations are done.
 *
 * Any device may try to invoke the ISR (by `spi_bus_lock_bg_request`). The ISR will be invoked and
 * become the acquiring processor immediately when the bus is not acquired by other processors. Any
 * device may also try to acquire the bus (by `spi_bus_lock_acquire_start`). The device will become
 * the acquiring processor immediately when the bus is not acquired and there is no request active.
 *
 * The acquiring processor must be aware of its acquiring role, and properly transfer the acquiring
 * processor to other tasks or ISR when they have nothing else to do. Before picking a new
 * acquiring processor, a new acquiring device must be picked first, if there are other devices,
 * asking to be acquiring device. After that, the new acquiring processor is picked by the sequence
 * below:
 *
 * 1. If there is an acquiring device:
 *      1.1 The ISR, if acquiring device has active BG requests
 *      1.2 The task of the device, if no active BG request for the device
 * 2. The ISR, if there's no acquiring device, but any BG request is active
 * 3. No one becomes the acquiring processor
 *
 * The API also helps on the arbitration of SPI cs lines. The bus is initialized with a cs_num
 * argument. When attaching devices onto the bus with `spi_bus_lock_register_dev`, it will allocate
 * devices with different device ID according to the flags given. If the ID is smaller than the
 * cs_num given when bus is initialized, error will be returned.
 *
 * Usage:
 * * Initialization:
 * 1. Call `spi_bus_init_lock` to register a lock for a bus.
 * 2. Call `spi_bus_lock_set_bg_control` to prepare BG enable/disable functions for
 *    the lock.
 * 3. Call `spi_bus_lock_register_dev` for each devices that may make use of the
 *    bus, properly store the returned handle, representing those devices.
 *
 * * Acquiring:
 * 1. Call `spi_bus_lock_acquire_start` when a device wants to use the bus
 * 2. Call `spi_bus_lock_touch` to mark the bus as touched by this device. Also check if the bus
 *    has been touched by other devices.
 * 3. (optional) Do something on the bus...
 * 4. (optional) Call `spi_bus_lock_bg_request` to inform and invoke the BG. See ISR below about
 *    ISR operations.
 * 5. (optional) If `spi_bus_lock_bg_request` is done, you have to call `spi_bus_lock_wait_bg_done`
 *    before touching the bus again, or do the following steps.
 * 6. Call `spi_bus_lock_acquire_end` to release the bus to other devices.
 *
 * * ISR:
 * 1. Call `spi_bus_lock_bg_entry` when entering the ISR, run or skip the closure for the previous
 *    operation according to the return value.
 * 2. Call `spi_bus_lock_get_acquiring_dev` to get the acquiring device. If there is no acquiring
 *    device, call `spi_bus_lock_bg_check_dev_acq` to check and update a new acquiring device.
 * 3. Call `spi_bus_lock_bg_check_dev_req` to check for request of the desired device. If the
 *    desired device is not requested, go to step 5.
 * 4. Check, start operation for the desired device and go to step 6; otherwise if no operations
 *    can be performed, call `spi_bus_lock_bg_clear_req` to clear the request for this device. If
 *    `spi_bus_lock_bg_clear_req` is called and there is no BG requests active, goto step 6.
 * 5. (optional) If the device is the acquiring device, go to step 6, otherwise
 *    find another desired device, and go back to step 3.
 * 6. Call `spi_bus_lock_bg_exit` to try quitting the ISR. If failed, go back to step 2 to look for
 *    a new request again. Otherwise, quit the ISR.
 *
 * * Deinitialization (optional):
 * 1. Call `spi_bus_lock_unregister_dev` for each device when they are no longer needed.
 * 2. Call `spi_bus_deinit_lock` to release the resources occupied by the lock.
 *
 * Some technical details:
 *
 * The child-lock of each device will have its own Binary Semaphore, which allows the task serving
 * this device (task A) being blocked when it fail to become the acquiring processor while it's
 * calling `spi_bus_lock_acquire_start` or `spi_bus_lock_wait_bg_done`. If it is blocked, there
 * must be an acquiring processor (either the ISR or another task (task B)), is doing transaction
 * on the bus. After that, task A will get unblocked and become the acquiring processor when the
 * ISR call `spi_bus_lock_bg_resume_acquired_dev`, or task B call `spi_bus_lock_acquire_end`.
 *
 * When the device wants to send ISR transaction, it should call `spi_bus_lock_bg_request` after
 * the data is prepared. This function sets a request bit in the critical resource. The ISR will be
 * invoked and become the new acquiring processor, when:
 *
 * 1. A task calls `spi_bus_lock_bg_request` while there is no acquiring processor;
 * 2. A tasks calls `spi_bus_lock_bg_request` while the task is the acquiring processor. Then the
 *    acquiring processor is handled over to the ISR;
 * 3. A tasks who is the acquiring processor release the bus by calling `spi_bus_lock_acquire_end`,
 *    and the ISR happens to be the next acquiring processor.
 *
 * The ISR will check (by `spi_bus_lock_bg_check_dev_req`) and clear a request bit (by
 * `spi_bus_lock_bg_clear_req`) after it confirm that all the requests of the corresponding device
 * are served. The request bit supports being written to recursively, which means, the task don't
 * need to wait for `spi_bus_lock_bg_clear_req` before call another `spi_bus_lock_bg_request`. The
 * API will handle the concurrency conflicts properly.
 *
 * The `spi_bus_lock_bg_exit` (together with `spi_bus_lock_bg_entry` called before)` is responsible
 * to ensure ONE and ONLY ONE of the following will happen when the ISR try to give up its
 * acquiring processor rule:
 *
 * 1. ISR quit, no any task unblocked while the interrupt disabled, and none of the BG bits is
 *    active.
 * 2. ISR quit, there is an acquiring device, and the acquiring processor is passed to the task
 *    serving the acquiring device by unblocking the task.
 * 3. The ISR failed to quit and have to try again.
 ******************************************************************************/

#define DEV_NUM_MAX 6     ///< Number of devices supported by this lock

/// Lock configuration struct
typedef struct {
    int host_id;    ///< SPI host id
    int cs_num;     ///< Physical cs numbers of the host
} spi_bus_lock_config_t;

/// Child-lock configuration struct
typedef struct {
    uint32_t flags; ///< flags for the lock, OR-ed of `SPI_BUS_LOCK_DEV_*` flags.
#define SPI_BUS_LOCK_DEV_FLAG_CS_REQUIRED   BIT(0)  ///< The device needs a physical CS pin.
} spi_bus_lock_dev_config_t;

/************* Common *********************/
/**
 * Initialize a lock for an SPI bus.
 *
 * @param out_lock Output of the handle to the lock
 * @return
 *  - ESP_ERR_NO_MEM: if memory exhausted
 *  - ESP_OK: if success
 */
esp_err_t spi_bus_init_lock(spi_bus_lock_handle_t *out_lock, const spi_bus_lock_config_t *config);

/**
 * Free the resources used by an SPI bus lock.
 *
 * @note All attached devices should have been unregistered before calling this
 *       funciton.
 *
 * @param lock Handle to the lock to free.
 */
void spi_bus_deinit_lock(spi_bus_lock_handle_t lock);

/**
 * @brief Get the corresponding lock according to bus id.
 *
 * @param host_id The bus id to get the lock
 * @return The lock handle
 */
spi_bus_lock_handle_t spi_bus_lock_get_by_id(spi_host_device_t host_id);

/**
 * @brief Configure how the SPI bus lock enable the background operation.
 *
 * @note The lock will not try to stop the background operations, but wait for
 *       The background operations finished indicated by `spi_bus_lock_bg_resume_acquired_dev`.
 *
 * @param lock Handle to the lock to set
 * @param bg_enable The enabling function
 * @param bg_disable The disabling function, set to NULL if not required
 * @param arg Argument to pass to the enabling/disabling function.
 */
void spi_bus_lock_set_bg_control(spi_bus_lock_handle_t lock, bg_ctrl_func_t bg_enable,
                                 bg_ctrl_func_t bg_disable, void *arg);

/**
 * Attach a device onto an SPI bus lock. The returning handle is used to perform
 * following requests for the attached device.
 *
 * @param lock SPI bus lock to attach
 * @param out_dev_handle Output handle corresponding to the device
 * @param flags requirement of the device, bitwise OR of SPI_BUS_LOCK_FLAG_* flags
 *
 * @return
 *  - ESP_ERR_NOT_SUPPORTED: if there's no hardware resources for new devices.
 *  - ESP_ERR_NO_MEM: if memory exhausted
 *  - ESP_OK: if success
 */
esp_err_t spi_bus_lock_register_dev(spi_bus_lock_handle_t lock,
                                    spi_bus_lock_dev_config_t *config,
                                    spi_bus_lock_dev_handle_t *out_dev_handle);

/**
 * Detach a device from its bus and free the resources used
 *
 * @param dev_handle Handle to the device.
 */
void spi_bus_lock_unregister_dev(spi_bus_lock_dev_handle_t dev_handle);

/**
 * @brief Get the parent bus lock of the device
 *
 * @param dev_handle Handle to the device to get bus lock
 * @return The bus lock handle
 */
spi_bus_lock_handle_t spi_bus_lock_get_parent(spi_bus_lock_dev_handle_t dev_handle);

/**
 * @brief Get the device ID of a lock.
 *
 * The callers should allocate CS pins according to this ID.
 *
 * @param dev_handle Handle to the device to get ID
 * @return ID of the device
 */
int spi_bus_lock_get_dev_id(spi_bus_lock_dev_handle_t dev_handle);

/**
 * @brief The device request to touch bus registers. Can only be called by the acquiring processor.
 *
 * Also check if the registers has been touched by other devices.
 *
 * @param dev_handle Handle to the device to operate the registers
 * @return true if there has been other devices touching SPI registers.
 *     The caller may need to do a full-configuration. Otherwise return
 *     false.
 */
bool spi_bus_lock_touch(spi_bus_lock_dev_handle_t dev_handle);

/************* Acquiring service *********************/
/**
 * Acquiring the SPI bus for exclusive use. Will also wait for the BG to finish all requests of
 * this device before it returns.
 *
 * After successfully return, the caller becomes the acquiring processor.
 *
 * @note For the main flash bus, `bg_disable` will be called to disable the cache.
 *
 * @param dev_handle Handle to the device request for acquiring.
 * @param wait Time to wait until timeout or succeed, must be `portMAX_DELAY` for now.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_ARG: timeout is not portMAX_DELAY
 */
esp_err_t spi_bus_lock_acquire_start(spi_bus_lock_dev_handle_t dev_handle, TickType_t wait);

/**
 * Release the bus acquired. Will pass the acquiring processor to other blocked
 * processors (tasks or ISR), and cause them to be unblocked or invoked.
 *
 * The acquiring device may also become NULL if no device is asking for acquiring.
 * In this case, the BG may be invoked if there is any BG requests.
 *
 * If the new acquiring device has BG requests, the BG will be invoked before the
 * task is resumed later after the BG finishes all requests of the new acquiring
 * device. Otherwise the task of the new acquiring device will be resumed immediately.
 *
 * @param dev_handle Handle to the device releasing the bus.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_STATE: the device hasn't acquired the lock yet
 */
esp_err_t spi_bus_lock_acquire_end(spi_bus_lock_dev_handle_t dev_handle);

/**
 * Get the device acquiring the bus.
 *
 * @note Return value is not stable as the acquiring processor may change
 *       when this function is called.
 *
 * @param lock Lock of SPI bus to get the acquiring device.
 * @return The argument corresponding to the acquiring device, see
 *         `spi_bus_lock_register_dev`.
 */
spi_bus_lock_dev_handle_t spi_bus_lock_get_acquiring_dev(spi_bus_lock_handle_t lock);

/************* BG (Background, for ISR or cache) service *********************/
/**
 * Call by a device to request a BG operation.
 *
 * Depending on the bus lock state, the BG operations may be resumed by this
 * call, or pending until BG operations allowed.
 *
 * Cleared by `spi_bus_lock_bg_clear_req` in the BG.
 *
 * @param dev_handle The device requesting BG operations.
 * @return always ESP_OK
 */
esp_err_t spi_bus_lock_bg_request(spi_bus_lock_dev_handle_t dev_handle);

/**
 * Wait until the ISR has finished all the BG operations for the acquiring device.
 * If any `spi_bus_lock_bg_request` for this device has been called after
 * `spi_bus_lock_acquire_start`, this function must be called before any operation
 * in the task.
 *
 * @note Can only be called when bus acquired by this device.
 *
 * @param dev_handle Handle to the device acquiring the bus.
 * @param wait Time to wait until timeout or succeed, must be `portMAX_DELAY` for now.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_STATE: The device is not the acquiring bus.
 *  - ESP_ERR_INVALID_ARG: Timeout is not portMAX_DELAY.
 */
esp_err_t spi_bus_lock_wait_bg_done(spi_bus_lock_dev_handle_t dev_handle, TickType_t wait);

/**
 * Handle interrupt and closure of last operation. Should be called at the beginning of the ISR,
 * when the ISR is acting as the acquiring processor.
 *
 * @param lock The SPI bus lock
 *
 * @return false if the ISR has already touched the HW, should run closure of the
 *         last operation first; otherwise true if the ISR just start operating
 *         on the HW, closure should be skipped.
 */
bool spi_bus_lock_bg_entry(spi_bus_lock_handle_t lock);

/**
 * Handle the scheduling of other acquiring devices, and control of HW operation
 * status.
 *
 * If no BG request is found, call with `wip=false`. This function will return false,
 * indicating there is incoming BG requests for the current acquiring device (or
 * for all devices if there is no acquiring device) and the ISR needs retry.
 * Otherwise may schedule a new acquiring processor (unblock the task) if there
 * is, and return true.
 *
 * Otherwise if a BG request is started in this ISR, call with `wip=true` and the
 * function will enable the interrupt to make the ISR be called again when the
 * request is done.
 *
 * This function is safe and should still be called when the ISR just lost its acquiring processor
 * role, but hasn't quit.
 *
 * @note This function will not change acquiring device. The ISR call
 *       `spi_bus_lock_bg_update_acquiring` to check for new acquiring device,
 *       when acquiring devices need to be served before other devices.
 *
 * @param lock The SPI bus lock.
 * @param wip Whether an operation is being executed when quitting the ISR.
 * @param do_yield[out] Not touched when no yielding required, otherwise set
 *                      to pdTRUE.
 * @return false if retry is required, indicating that there is pending BG request.
 *         otherwise true and quit ISR is allowed.
 */
bool spi_bus_lock_bg_exit(spi_bus_lock_handle_t lock, bool wip, BaseType_t* do_yield);

/**
 * Check whether there is device asking for the acquiring device, and the desired
 * device for the next operation is also recommended.
 *
 * @note Must be called when the ISR is acting as the acquiring processor, and
 *        there is no acquiring device.
 *
 * @param lock The SPI bus lock.
 * @param out_dev_lock The recommended device for hte next operation. It's the new
 *        acquiring device when found, otherwise a device that has active BG request.
 *
 * @return true if the ISR need to quit (new acquiring device has no active BG
 *         request, or no active BG requests for all devices when there is no
 *         acquiring device), otherwise false.
 */
bool spi_bus_lock_bg_check_dev_acq(spi_bus_lock_handle_t lock, spi_bus_lock_dev_handle_t *out_dev_lock);

/**
 * Check if the device has BG requests. Must be called when the ISR is acting as
 * the acquiring processor.
 *
 * @note This is not stable, may become true again when a task request for BG
 *       operation (by `spi_bus_lock_bg_request`).
 *
 * @param dev_lock The device to check.
 * @return true if the device has BG requests, otherwise false.
 */
bool spi_bus_lock_bg_check_dev_req(spi_bus_lock_dev_handle_t dev_lock);

/**
 * Clear the pending BG operation request of a device after served. Must be
 * called when the ISR is acting as the acquiring processor.
 *
 * @note When the return value is true, the ISR will lost the acquiring processor role. Then
 *       `spi_bus_lock_bg_exit` must be called and checked before calling all other functions that
 *       require to be called when the ISR is the acquiring processor again.
 *
 * @param dev_handle The device whose request is served.
 * @return True if no pending requests for the acquiring device, or for all devices
 *         if there is no acquiring device. Otherwise false. When the return value is
 *         true, the ISR is no longer the acquiring processor.
 */
bool spi_bus_lock_bg_clear_req(spi_bus_lock_dev_handle_t dev_lock);

/**
 * Check if there is any active BG requests.
 *
 * @param lock The SPI bus lock.
 * @return true if any device has active BG requst, otherwise false.
 */
bool spi_bus_lock_bg_req_exist(spi_bus_lock_handle_t lock);

/*******************************************************************************
 * Variable and APIs for the OS to initialize the locks for the main chip
 ******************************************************************************/
/// The lock for the main bus
extern const spi_bus_lock_handle_t g_main_spi_bus_lock;

/// The lock for the main flash device
extern const spi_bus_lock_dev_handle_t g_spi_lock_main_flash_dev;

/**
 * @brief Initialize the main flash device, called during chip startup.
 *
 * @return
 *      - ESP_OK: if success
 *      - ESP_ERR_NO_MEM: memory exhausted
 */
esp_err_t spi_bus_lock_init_main_dev(void);

#ifdef __cplusplus
}
#endif
