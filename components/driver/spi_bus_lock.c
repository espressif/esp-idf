// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <stdatomic.h>
#include "sdkconfig.h"
#include "spi_common_internal.h"
#include "esp_intr_alloc.h"
#include "soc/spi_caps.h"
#include "stdatomic.h"
#include "esp_log.h"
#include <strings.h>


/*
 * This lock is designed to solve the conflicts between SPI devices (used in tasks) and
 * the background operations (ISR or cache access).
 *
 * There are N (device/task) + 1 (BG) acquiring processer candidates that may touch the bus.
 *
 * The core of the lock is a `status` atomic variable, which is always available. No intermediate
 * status is allowed. The atomic operations (mainly `atomic_fetch_and`, `atomic_fetch_or`)
 * atomically read the status, and bitwisely write status value ORed / ANDed with given masks.
 *
 * Definitions of the status:
 * - [30]    WEAK_BG_FLAG, active when the BG is the cache
 * - [29:20] LOCK bits, active when corresponding device is asking for acquiring
 * - [19:10] PENDING bits, active when the BG acknowledges the REQ bits, but hasn't fully handled them.
 * - [ 9: 0] REQ bits, active when corresponding device is requesting for BG operations.
 *
 *   The REQ bits together PENDING bits are called BG bits, which represent the actual BG request
 *   state of devices. Either one of REQ or PENDING being active indicates the device has pending BG
 *   requests. Reason of having two bits instead of one is in the appendix below.
 *
 * Acquiring processer means the current processor (task or ISR) allowed to touch the critical
 * resources, or the SPI bus.
 *
 * States of the lock:
 * - STATE_IDLE: There's no acquiring processor. No device is acquiring the bus, and no BG
 *   operation is in progress.
 *
 * - STATE_ACQ: The acquiring processor is a device task. This means one of the devices is
 *   acquiring the bus.
 *
 * - STATE_BG: The acquiring processor is the ISR, and there is no acquiring device.
 *
 * - STATE_BG_ACQ: The acquiring processor is the ISR, and there is an acquiring device.
 *
 *
 * Whenever a bit is written to the status, it means the a device on a task is trying to acquire
 * the lock (either for the task, or the ISR). When there is no LOCK bits or BG bits active, the
 * caller immediately become the acquiring processor. Otherwise, the task has to block, and the ISR
 * will not be invoked until scheduled by the current acquiring processor.
 *
 * The acquiring processor is responsible to assign the next acquiring processor by calling the
 * scheduler, usually after it finishes some requests, and cleared the corresponding status bit.
 * But there is one exception, when the last bit is cleared from the status, after which there is
 * no other LOCK bits or BG bits active, the acquiring processor lost its role immediately, and
 * don't need to call the scheduler to assign the next acquiring processor.
 *
 * The acquiring processor may also choose to assign a new acquiring device when there is no, by
 * calling `spi_bus_lock_bg_rotate_acq_dev` in the ISR. But the acquiring processor, in this case,
 * is still the ISR, until it calls the scheduler.
 *
 *
 * Transition of the FSM:
 *
 * - STATE_IDLE: no acquiring device, nor acquiring processor, no LOCK or BG bits active
 *   -> STATE_BG: by `req_core`
 *   -> STATE_ACQ: by `acquire_core`
 *
 * - STATE_BG:
 *      * No acquiring device, the ISR is the acquiring processor, there is BG bits active, but no LOCK
 *        bits
 *      * The BG operation should be enabled while turning into this state.
 *
 *   -> STATE_IDLE: by `bg_exit_core` after `clear_pend_core` for all BG bits
 *   -> STATE_BG_ACQ: by `schedule_core`, when there is new LOCK bit set (by `acquire_core`)
 *
 * - STATE_BG_ACQ:
 *      * There is acquiring device, the ISR is the acquiring processor, there may be BG bits active for
 *        the acquiring device.
 *      * The BG operation should be enabled while turning into this state.
 *
 *   -> STATE_ACQ: by `bg_exit_core` after `clear_pend_core` for all BG bits for the acquiring
 *                 device.
 *
 *                 Should not go to the STATE_ACQ (unblock the acquiring task) until all requests of the
 *                 acquiring device are finished. This is to preserve the sequence of foreground (polling) and
 *                 background operations of the device. The background operations queued before the acquiring
 *                 should be completed first.
 *
 * - STATE_ACQ:
 *      * There is acquiring device, the task is the acquiring processor, there is no BG bits active for
 *        the acquiring device.
 *      * The acquiring task (if blocked at `spi_bus_lock_acquire_start` or `spi_bus_lock_wait_bg_done`)
 *        should be resumed while turning into this state.
 *
 *   -> STATE_BG_ACQ: by `req_core`
 *   -> STATE_BG_ACQ (other device): by `acquire_end_core`, when there is LOCK bit for another
 *                    device, and the new acquiring device has active BG bits.
 *   -> STATE_ACQ (other device): by `acquire_end_core`, when there is LOCK bit for another devices,
 *                    but the new acquiring device has no active BG bits.
 *   -> STATE_BG: by `acquire_end_core` when there is no LOCK bit active, but there are active BG
 *                bits.
 *   -> STATE_IDLE: by `acquire_end_core` when there is no LOCK bit, nor BG bit active.
 *
 * The `req_core` used in the task is a little special. It asks for acquiring processor for the
 * ISR. When it succeed for the first time, it will invoke the ISR (hence passing the acquiring
 * role to the BG). Otherwise it will not block, the ISR will be automatically be invoked by other
 * acquiring processor. The caller of `req_core` will never become acquiring processor by this
 * function.
 *
 *
 * Appendix: The design, that having both request bit and pending bit, is to solve the
 * concurrency issue between tasks and the bg, when the task can queue several requests,
 * however the request bit cannot represent the number of requests queued.
 *
 * Here's the workflow of task and ISR work concurrently:
 * - Task: (a) Write to Queue -> (b) Write request bit
 *   The Task have to write request bit (b) after the data is prepared in the queue (a),
 *   otherwise the BG may fail to read from the queue when it sees the request bit set.
 *
 * - BG: (c) Read queue -> (d) Clear request bit
 *   Since the BG cannot know the number of requests queued, it have to repeatedly check the
 *   queue (c), until it find the data is empty, and then clear the request bit (d).
 *
 * The events are possible to happen in the order: (c) -> (a) -> (b) -> (d). This may cause a false
 * clear of the request bit. And there will be data prepared in the queue, but the request bit is
 * inactive.
 *
 * (e) move REQ bits to PEND bits, happen before (c) is introduced to solve this problem. In this
 * case (d) is changed to clear the PEND bit. Even if (e) -> (c) -> (a) -> (b) -> (d), only PEND
 * bit is cleared, while the REQ bit is still active.
 */

struct spi_bus_lock_dev_t;
typedef struct spi_bus_lock_dev_t spi_bus_lock_dev_t;

typedef struct spi_bus_lock_t spi_bus_lock_t;


#define MAX_DEV_NUM     10

// Bit 29-20: lock bits, Bit 19-10: pending bits
// Bit 9-0: request bits, Bit 30:
#define LOCK_SHIFT      20
#define PENDING_SHIFT   10
#define REQ_SHIFT       0

#define WEAK_BG_FLAG    BIT(30)    /**< The bus is permanently requested by background operations.
                                     * This flag is weak, will not prevent acquiring of devices. But will help the BG to be re-enabled again after the bus is release.
                                     */

// get the bit mask wher bit [high-1, low] are all 1'b1 s.
#define BIT1_MASK(high, low)   ((UINT32_MAX << (high)) ^ (UINT32_MAX << (low)))

#define LOCK_BIT(mask)      ((mask) << LOCK_SHIFT)
#define REQUEST_BIT(mask)   ((mask) << REQ_SHIFT)
#define PENDING_BIT(mask)   ((mask) << PENDING_SHIFT)
#define DEV_MASK(id)        (LOCK_BIT(1<<id) | PENDING_BIT(1<<id) | REQUEST_BIT(1<<id))
#define ID_DEV_MASK(mask)   (ffs(mask) - 1)

#define REQ_MASK            BIT1_MASK(REQ_SHIFT+MAX_DEV_NUM, REQ_SHIFT)
#define PEND_MASK           BIT1_MASK(PENDING_SHIFT+MAX_DEV_NUM, PENDING_SHIFT)
#define BG_MASK             BIT1_MASK(REQ_SHIFT+MAX_DEV_NUM*2, REQ_SHIFT)
#define LOCK_MASK           BIT1_MASK(LOCK_SHIFT+MAX_DEV_NUM, LOCK_SHIFT)

#define DEV_REQ_MASK(dev)   ((dev)->mask & REQ_MASK)
#define DEV_PEND_MASK(dev)  ((dev)->mask & PEND_MASK)
#define DEV_BG_MASK(dev)    ((dev)->mask & BG_MASK)

struct spi_bus_lock_t {
    /**
     * The core of the lock. These bits are status of the lock, which should be always available.
     * No intermediate status is allowed. This is realized by atomic operations, mainly
     * `atomic_fetch_and`, `atomic_fetch_or`, which atomically read the status, and bitwise write
     * status value ORed / ANDed with given masks.
     *
     * The request bits together pending bits represent the actual bg request state of one device.
     * Either one of them being active indicates the device has pending bg requests.
     *
     * Whenever a bit is written to the status, it means the a device on a task is trying to
     * acquire the lock. But this will succeed only when no LOCK or BG bits active.
     *
     * The acquiring processor is responsible to call the scheduler to pass its role to other tasks
     * or the BG, unless it clear the last bit in the status register.
     */
    //// Critical resources, they are only writable by acquiring processor, and stable only when read by the acquiring processor.
    atomic_uint_fast32_t    status;
    spi_bus_lock_dev_t* volatile acquiring_dev;   ///< The acquiring device
    bool                volatile acq_dev_bg_active;    ///< BG is the acquiring processor serving the acquiring device, used for the wait_bg to skip waiting quickly.
    bool                volatile in_isr;         ///< ISR is touching HW
    //// End of critical resources

    atomic_intptr_t     dev[DEV_NUM_MAX];     ///< Child locks.
    bg_ctrl_func_t      bg_enable;      ///< Function to enable background operations.
    bg_ctrl_func_t      bg_disable;     ///< Function to disable background operations
    void*               bg_arg;            ///< Argument for `bg_enable` and `bg_disable` functions.

    spi_bus_lock_dev_t* last_dev;       ///< Last used device, to decide whether to refresh all registers.
    int                 periph_cs_num;  ///< Number of the CS pins the HW has.

    //debug information
    int                 host_id;        ///< Host ID, for debug information printing
    uint32_t            new_req;        ///< Last int_req when `spi_bus_lock_bg_start` is called. Debug use.
};

struct spi_bus_lock_dev_t {
    SemaphoreHandle_t   semphr;     ///< Binray semaphore to notify the device it claimed the bus
    spi_bus_lock_t*     parent;     ///< Pointer to parent spi_bus_lock_t
    uint32_t            mask;       ///< Bitwise OR-ed mask of the REQ, PEND, LOCK bits of this device
};

/**
 * @note 1
 * This critical section is only used to fix such condition:
 *
 * define: lock_bits = (lock->status & LOCK_MASK) >> LOCK_SHIFT;  This `lock_bits` is the Bit 29-20 of the lock->status
 *
 * 1. spi_hdl_1:
 *    acquire_end_core():
 *    uint32_t status = lock_status_clear(lock, dev_handle->mask & LOCK_MASK);
 *
 *    Becuase this is the first `spi_hdl_1`, so after this , lock_bits == 0`b0. status == 0
 *
 * 2. spi_hdl_2:
 *    acquire_core:
 *    uint32_t status = lock_status_fetch_set(lock, dev_handle->mask & LOCK_MASK);
 *
 *    Then here status is 0`b0, but lock_bits == 0`b10. Because this is the `spi_hdl_2`
 *
 * 3. spi_hdl_2:
 *    `acquire_core` return true, because status == 0. `spi_bus_lock_acquire_start(spi_hdl_2)` then won't block.
 *
 * 4. spi_hdl_2:
 *    spi_device_polling_end(spi_hdl_2).
 *
 * 5. spi_hdl_1:
 *    acquire_end_core:
 *    status is 0, so it cleas the lock->acquiring_dev
 *
 * 6. spi_hdl_2:
 *    spi_device_polling_end:
 *    assert(handle == get_acquiring_dev(host)); Fail
 *
 * @note 2
 * Only use this critical section in this condition. The critical section scope is limited to the smallest.
 * As `spi_bus_lock` influences the all the SPIs (including MSPI) a lot!
 */
portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;

DRAM_ATTR static const char TAG[] = "bus_lock";

#define LOCK_CHECK(a, str, ret_val, ...) \
    if (!(a)) { \
        ESP_LOGE(TAG,"%s(%d): "str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

static inline uint32_t mask_get_id(uint32_t mask);
static inline uint32_t dev_lock_get_id(spi_bus_lock_dev_t *dev_lock);

/*******************************************************************************
 * atomic operations to the status
 ******************************************************************************/
SPI_MASTER_ISR_ATTR static inline uint32_t lock_status_fetch_set(spi_bus_lock_t *lock, uint32_t set)
{
    return atomic_fetch_or(&lock->status, set);
}

IRAM_ATTR static inline uint32_t lock_status_fetch_clear(spi_bus_lock_t *lock, uint32_t clear)
{
    return atomic_fetch_and(&lock->status, ~clear);
}

IRAM_ATTR static inline uint32_t lock_status_fetch(spi_bus_lock_t *lock)
{
    return atomic_load(&lock->status);
}

SPI_MASTER_ISR_ATTR static inline void lock_status_init(spi_bus_lock_t *lock)
{
    atomic_store(&lock->status, 0);
}

// return the remaining status bits
IRAM_ATTR static inline uint32_t lock_status_clear(spi_bus_lock_t* lock, uint32_t clear)
{
    //the fetch and clear should be atomic, avoid missing the all '0' status when all bits are clear.
    uint32_t state = lock_status_fetch_clear(lock, clear);
    return state & (~clear);
}

/*******************************************************************************
 * Schedule service
 *
 * The modification to the status bits may cause rotating of the acquiring processor. It also have
 * effects to `acquired_dev` (the acquiring device), `in_isr` (HW used in BG), and
 * `acq_dev_bg_active` (wait_bg_end can be skipped) members of the lock structure.
 *
 * Most of them should be atomic, and special attention should be paid to the operation
 * sequence.
 ******************************************************************************/
SPI_MASTER_ISR_ATTR static inline void resume_dev_in_isr(spi_bus_lock_dev_t *dev_lock, BaseType_t *do_yield)
{
    xSemaphoreGiveFromISR(dev_lock->semphr, do_yield);
}

IRAM_ATTR static inline void resume_dev(const spi_bus_lock_dev_t *dev_lock)
{
    xSemaphoreGive(dev_lock->semphr);
}

SPI_MASTER_ISR_ATTR static inline void bg_disable(spi_bus_lock_t *lock)
{
    BUS_LOCK_DEBUG_EXECUTE_CHECK(lock->bg_disable);
    lock->bg_disable(lock->bg_arg);
}

IRAM_ATTR static inline void bg_enable(spi_bus_lock_t* lock)
{
    BUS_LOCK_DEBUG_EXECUTE_CHECK(lock->bg_enable);
    lock->bg_enable(lock->bg_arg);
}

// Set the REQ bit. If we become the acquiring processor, invoke the ISR and pass that to it.
// The caller will never become the acquiring processor after this function returns.
SPI_MASTER_ATTR static inline void req_core(spi_bus_lock_dev_t *dev_handle)
{
    spi_bus_lock_t *lock = dev_handle->parent;

    // Though `acquired_dev` is critical resource, `dev_handle == lock->acquired_dev`
    // is a stable statement unless `acquire_start` or `acquire_end` is called by current
    // device.
    if (dev_handle == lock->acquiring_dev){
        // Set the REQ bit and check BG bits if we are the acquiring processor.
        // If the BG bits were not active before, invoke the BG again.

        // Avoid competitive risk against the `clear_pend_core`, `acq_dev_bg_active` should be set before
        // setting REQ bit.
        lock->acq_dev_bg_active = true;
        uint32_t status = lock_status_fetch_set(lock, DEV_REQ_MASK(dev_handle));
        if ((status & DEV_BG_MASK(dev_handle)) == 0) {
            bg_enable(lock); //acquiring processor passed to BG
        }
    } else {
        uint32_t status = lock_status_fetch_set(lock, DEV_REQ_MASK(dev_handle));
        if (status == 0) {
            bg_enable(lock); //acquiring processor passed to BG
        }
    }
}

//Set the LOCK bit. Handle related stuff and return true if we become the acquiring processor.
SPI_MASTER_ISR_ATTR static inline bool acquire_core(spi_bus_lock_dev_t *dev_handle)
{
    spi_bus_lock_t* lock = dev_handle->parent;

    //For this critical section, search `@note 1` in this file, to know details
    portENTER_CRITICAL_SAFE(&s_spinlock);
    uint32_t status = lock_status_fetch_set(lock, dev_handle->mask & LOCK_MASK);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    // Check all bits except WEAK_BG
    if ((status & (BG_MASK | LOCK_MASK)) == 0) {
        //succeed at once
        lock->acquiring_dev = dev_handle;
        BUS_LOCK_DEBUG_EXECUTE_CHECK(!lock->acq_dev_bg_active);
        if (status & WEAK_BG_FLAG) {
            //Mainly to disable the cache (Weak_BG), that is not able to disable itself
            bg_disable(lock);
        }
        return true;
    }
    return false;
}

/**
 * Find the next acquiring processor according to the status. Will directly change
 * the acquiring device if new one found.
 *
 * Cases:
 * - BG should still be the acquiring processor (Return false):
 *     1. Acquiring device has active BG bits: out_desired_dev = new acquiring device
 *     2. No acquiring device, but BG active: out_desired_dev = randomly pick one device with active BG bits
 * - BG should yield to the task (Return true):
 *     3. Acquiring device has no active BG bits: out_desired_dev = new acquiring device
 *     4. No acquiring device while no active BG bits: out_desired_dev=NULL
 *
 * Acquiring device task need to be resumed only when case 3.
 *
 * This scheduling can happen in either task or ISR, so `in_isr` or `bg_active` not touched.
 *
 * @param lock
 * @param status Current status
 * @param out_desired_dev Desired device to work next, see above.
 *
 * @return False if BG should still be the acquiring processor, otherwise True (yield to task).
 */
IRAM_ATTR static inline bool
schedule_core(spi_bus_lock_t *lock, uint32_t status, spi_bus_lock_dev_t **out_desired_dev)
{
    spi_bus_lock_dev_t* desired_dev = NULL;
    uint32_t lock_bits = (status & LOCK_MASK) >> LOCK_SHIFT;
    uint32_t bg_bits = status & BG_MASK;
    bg_bits = ((bg_bits >> REQ_SHIFT) | (bg_bits >> PENDING_SHIFT)) & REQ_MASK;

    bool bg_yield;
    if (lock_bits) {
        int dev_id = mask_get_id(lock_bits);
        desired_dev = (spi_bus_lock_dev_t *)atomic_load(&lock->dev[dev_id]);
        BUS_LOCK_DEBUG_EXECUTE_CHECK(desired_dev);

        lock->acquiring_dev = desired_dev;
        bg_yield = ((bg_bits & desired_dev->mask) == 0);
        lock->acq_dev_bg_active = !bg_yield;
    } else {
        lock->acq_dev_bg_active = false;
        if (bg_bits) {
            int dev_id = mask_get_id(bg_bits);
            desired_dev = (spi_bus_lock_dev_t *)atomic_load(&lock->dev[dev_id]);
            BUS_LOCK_DEBUG_EXECUTE_CHECK(desired_dev);

            lock->acquiring_dev = NULL;
            bg_yield = false;
        } else {
            desired_dev = NULL;
            lock->acquiring_dev = NULL;
            bg_yield = true;
        }
    }
    *out_desired_dev = desired_dev;
    return bg_yield;
}

//Clear the LOCK bit and trigger a rescheduling.
IRAM_ATTR static inline void acquire_end_core(spi_bus_lock_dev_t *dev_handle)
{
    spi_bus_lock_t* lock = dev_handle->parent;
    spi_bus_lock_dev_t* desired_dev = NULL;

    //For this critical section, search `@note 1` in this file, to know details
    portENTER_CRITICAL_SAFE(&s_spinlock);
    uint32_t status = lock_status_clear(lock, dev_handle->mask & LOCK_MASK);
    bool invoke_bg = !schedule_core(lock, status, &desired_dev);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    if (invoke_bg) {
        bg_enable(lock);
    } else if (desired_dev) {
        resume_dev(desired_dev);
    } else if (status & WEAK_BG_FLAG) {
        bg_enable(lock);
    }
}

// Move the REQ bits to corresponding PEND bits. Must be called by acquiring processor.
// Have no side effects on the acquiring device/processor.
SPI_MASTER_ISR_ATTR static inline void update_pend_core(spi_bus_lock_t *lock, uint32_t status)
{
    uint32_t active_req_bits = status & REQ_MASK;
#if PENDING_SHIFT > REQ_SHIFT
    uint32_t pending_mask = active_req_bits << (PENDING_SHIFT - REQ_SHIFT);
#else
    uint32_t pending_mask = active_req_bits >> (REQ_SHIFT - PENDING_SHIFT);
#endif
    // We have to set the PEND bits and then clear the REQ bits, since BG bits are using bitwise OR logic,
    // this will not influence the effectiveness of the BG bits of every device.
    lock_status_fetch_set(lock, pending_mask);
    lock_status_fetch_clear(lock, active_req_bits);
}

// Clear the PEND bit (not REQ bit!) of a device, return the suggestion whether we can try to quit the ISR.
// Lost the acquiring processor immediately when the BG bits for active device are inactive, indiciating by the return value.
// Can be called only when ISR is acting as the acquiring processor.
SPI_MASTER_ISR_ATTR static inline bool clear_pend_core(spi_bus_lock_dev_t *dev_handle)
{
    bool finished;
    spi_bus_lock_t *lock = dev_handle->parent;
    uint32_t pend_mask = DEV_PEND_MASK(dev_handle);
    BUS_LOCK_DEBUG_EXECUTE_CHECK(lock_status_fetch(lock) & pend_mask);

    uint32_t status = lock_status_clear(lock, pend_mask);

    if (lock->acquiring_dev == dev_handle) {
        finished = ((status & DEV_REQ_MASK(dev_handle)) == 0);
        if (finished) {
            lock->acq_dev_bg_active = false;
        }
    } else {
        finished = (status == 0);
    }
    return finished;
}

// Return true if the ISR has already touched the HW, which means previous operations should
// be terminated first, before we use the HW again. Otherwise return false.
// In either case `in_isr` will be marked as true, until call to `bg_exit_core` with `wip=false` successfully.
SPI_MASTER_ISR_ATTR static inline bool bg_entry_core(spi_bus_lock_t *lock)
{
    BUS_LOCK_DEBUG_EXECUTE_CHECK(!lock->acquiring_dev || lock->acq_dev_bg_active);
    /*
     * The interrupt is disabled at the entry of ISR to avoid competitive risk as below:
     *
     * The `esp_intr_enable` will be called (b) after new BG request is queued (a) in the task;
     * while `esp_intr_disable` should be called (c) if we check and found the sending queue is empty (d).
     * If (c) happens after (d), if things happens in this sequence:
     * (d) -> (a) -> (b) -> (c), the interrupt will be disabled while there's pending BG request in the queue.
     *
     * To avoid this, interrupt is disabled here, and re-enabled later if required. (c) -> (d) -> (a) -> (b) -> revert (c) if !d
     */
    bg_disable(lock);
    if (lock->in_isr) {
        return false;
    } else {
        lock->in_isr = true;
        return true;
    }
}

// Handle the conditions of status and interrupt, avoiding the ISR being disabled when there is any new coming BG requests.
// When called with `wip=true`, means the ISR is performing some operations. Will enable the interrupt again and exit unconditionally.
// When called with `wip=false`, will only return `true` when there is no coming BG request. If return value is `false`, the ISR should try again.
// Will not change acquiring device.
SPI_MASTER_ISR_ATTR static inline bool bg_exit_core(spi_bus_lock_t *lock, bool wip, BaseType_t *do_yield)
{
    //See comments in `bg_entry_core`, re-enable interrupt disabled in entry if we do need the interrupt
    if (wip) {
        bg_enable(lock);
        BUS_LOCK_DEBUG_EXECUTE_CHECK(!lock->acquiring_dev || lock->acq_dev_bg_active);
        return true;
    }

    bool ret;
    uint32_t status = lock_status_fetch(lock);
    if (lock->acquiring_dev) {
        if (status & DEV_BG_MASK(lock->acquiring_dev)) {
            BUS_LOCK_DEBUG_EXECUTE_CHECK(lock->acq_dev_bg_active);
            ret = false;
        } else {
            // The request may happen any time, even after we fetched the status.
            // The value of `acq_dev_bg_active` is random.
            resume_dev_in_isr(lock->acquiring_dev, do_yield);
            ret = true;
        }
    } else {
        BUS_LOCK_DEBUG_EXECUTE_CHECK(!lock->acq_dev_bg_active);
        ret = !(status & BG_MASK);
    }
    if (ret) {
        //when successfully exit, but no transaction done, mark BG as inactive
        lock->in_isr = false;
    }
    return ret;
}

IRAM_ATTR static inline void dev_wait_prepare(spi_bus_lock_dev_t *dev_handle)
{
    xSemaphoreTake(dev_handle->semphr, 0);
}

SPI_MASTER_ISR_ATTR static inline esp_err_t dev_wait(spi_bus_lock_dev_t *dev_handle, TickType_t wait)
{
    BaseType_t ret = xSemaphoreTake(dev_handle->semphr, wait);

    if (ret == pdFALSE) return ESP_ERR_TIMEOUT;
    return ESP_OK;
}

/*******************************************************************************
 * Initialization & Deinitialization
 ******************************************************************************/
esp_err_t spi_bus_init_lock(spi_bus_lock_handle_t *out_lock, const spi_bus_lock_config_t *config)
{
    spi_bus_lock_t* lock = (spi_bus_lock_t*)calloc(sizeof(spi_bus_lock_t), 1);
    if (lock == NULL) {
        return ESP_ERR_NO_MEM;
    }

    lock_status_init(lock);
    lock->acquiring_dev = NULL;
    lock->last_dev = NULL;
    lock->periph_cs_num = config->cs_num;
    lock->host_id = config->host_id;

    *out_lock = lock;
    return ESP_OK;
}

void spi_bus_deinit_lock(spi_bus_lock_handle_t lock)
{
    for (int i = 0; i < DEV_NUM_MAX; i++) {
        assert(atomic_load(&lock->dev[i]) == (intptr_t)NULL);
    }
    free(lock);
}

static int try_acquire_free_dev(spi_bus_lock_t *lock, bool cs_required)
{
    if (cs_required) {
        int i;
        for (i = 0; i < lock->periph_cs_num; i++) {
            intptr_t null = (intptr_t) NULL;
            //use 1 to occupy the slot, actual setup comes later
            if (atomic_compare_exchange_strong(&lock->dev[i], &null, (intptr_t) 1)) {
                break;
            }
        }
        return ((i == lock->periph_cs_num)? -1: i);
    } else {
        int i;
        for (i = DEV_NUM_MAX - 1; i >= 0; i--) {
            intptr_t null = (intptr_t) NULL;
            //use 1 to occupy the slot, actual setup comes later
            if (atomic_compare_exchange_strong(&lock->dev[i], &null, (intptr_t) 1)) {
                break;
            }
        }
        return i;
    }
}

esp_err_t spi_bus_lock_register_dev(spi_bus_lock_handle_t lock, spi_bus_lock_dev_config_t *config,
                                    spi_bus_lock_dev_handle_t *out_dev_handle)
{
    if (lock == NULL) return ESP_ERR_INVALID_ARG;
    int id = try_acquire_free_dev(lock, config->flags & SPI_BUS_LOCK_DEV_FLAG_CS_REQUIRED);
    if (id == -1) return ESP_ERR_NOT_SUPPORTED;

    spi_bus_lock_dev_t* dev_lock = (spi_bus_lock_dev_t*)heap_caps_calloc(sizeof(spi_bus_lock_dev_t), 1, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (dev_lock == NULL) {
        return ESP_ERR_NO_MEM;
    }
    dev_lock->semphr = xSemaphoreCreateBinary();
    if (dev_lock->semphr == NULL) {
        free(dev_lock);
        atomic_store(&lock->dev[id], (intptr_t)NULL);
        return ESP_ERR_NO_MEM;
    }
    dev_lock->parent = lock;
    dev_lock->mask = DEV_MASK(id);

    ESP_LOGV(TAG, "device registered on bus %d slot %d.", lock->host_id, id);
    atomic_store(&lock->dev[id], (intptr_t)dev_lock);
    *out_dev_handle = dev_lock;
    return ESP_OK;
}

void spi_bus_lock_unregister_dev(spi_bus_lock_dev_handle_t dev_handle)
{
    int id = dev_lock_get_id(dev_handle);

    spi_bus_lock_t* lock = dev_handle->parent;
    BUS_LOCK_DEBUG_EXECUTE_CHECK(atomic_load(&lock->dev[id]) == (intptr_t)dev_handle);

    if (lock->last_dev == dev_handle) lock->last_dev = NULL;

    atomic_store(&lock->dev[id], (intptr_t)NULL);
    if (dev_handle->semphr) {
        vSemaphoreDelete(dev_handle->semphr);
    }

    free(dev_handle);
}

IRAM_ATTR static inline uint32_t mask_get_id(uint32_t mask)
{
    return ID_DEV_MASK(mask);
}

IRAM_ATTR static inline uint32_t dev_lock_get_id(spi_bus_lock_dev_t *dev_lock)
{
    return mask_get_id(dev_lock->mask);
}

void spi_bus_lock_set_bg_control(spi_bus_lock_handle_t lock, bg_ctrl_func_t bg_enable, bg_ctrl_func_t bg_disable, void *arg)
{
    lock->bg_enable = bg_enable;
    lock->bg_disable = bg_disable;
    lock->bg_arg = arg;
}

IRAM_ATTR int spi_bus_lock_get_dev_id(spi_bus_lock_dev_handle_t dev_handle)
{
    return (dev_handle? dev_lock_get_id(dev_handle): -1);
}

//will be called when cache disabled
IRAM_ATTR bool spi_bus_lock_touch(spi_bus_lock_dev_handle_t dev_handle)
{
    spi_bus_lock_dev_t* last_dev = dev_handle->parent->last_dev;
    dev_handle->parent->last_dev = dev_handle;
    if (last_dev != dev_handle) {
        int last_dev_id = (last_dev? dev_lock_get_id(last_dev): -1);
        ESP_DRAM_LOGV(TAG, "SPI dev changed from %d to %d",
                    last_dev_id, dev_lock_get_id(dev_handle));
    }
    return (dev_handle != last_dev);
}

/*******************************************************************************
 * Acquiring service
 ******************************************************************************/
IRAM_ATTR esp_err_t spi_bus_lock_acquire_start(spi_bus_lock_dev_t *dev_handle, TickType_t wait)
{
    LOCK_CHECK(wait == portMAX_DELAY, "timeout other than portMAX_DELAY not supported", ESP_ERR_INVALID_ARG);

    spi_bus_lock_t* lock = dev_handle->parent;

    // Clear the semaphore before checking
    dev_wait_prepare(dev_handle);
    if (!acquire_core(dev_handle)) {
        //block until becoming the acquiring processor (help by previous acquiring processor)
        esp_err_t err = dev_wait(dev_handle, wait);
        //TODO: add timeout handling here.
        if (err != ESP_OK) return err;
    }

    ESP_DRAM_LOGV(TAG, "dev %d acquired.", dev_lock_get_id(dev_handle));
    BUS_LOCK_DEBUG_EXECUTE_CHECK(lock->acquiring_dev == dev_handle);

    //When arrives at here, requests of this device should already be handled
    uint32_t status = lock_status_fetch(lock);
    (void) status;
    BUS_LOCK_DEBUG_EXECUTE_CHECK((status & DEV_BG_MASK(dev_handle)) == 0);

    return ESP_OK;
}

IRAM_ATTR esp_err_t spi_bus_lock_acquire_end(spi_bus_lock_dev_t *dev_handle)
{
    //release the bus
    spi_bus_lock_t* lock = dev_handle->parent;
    LOCK_CHECK(lock->acquiring_dev == dev_handle, "Cannot release a lock that hasn't been acquired.", ESP_ERR_INVALID_STATE);

    acquire_end_core(dev_handle);

    ESP_LOGV(TAG, "dev %d released.", dev_lock_get_id(dev_handle));
    return ESP_OK;
}

SPI_MASTER_ISR_ATTR spi_bus_lock_dev_handle_t spi_bus_lock_get_acquiring_dev(spi_bus_lock_t *lock)
{
    return lock->acquiring_dev;
}

/*******************************************************************************
 * BG (background operation) service
 ******************************************************************************/
SPI_MASTER_ISR_ATTR bool spi_bus_lock_bg_entry(spi_bus_lock_t* lock)
{
    return bg_entry_core(lock);
}

SPI_MASTER_ISR_ATTR bool spi_bus_lock_bg_exit(spi_bus_lock_t* lock, bool wip, BaseType_t* do_yield)
{
    return bg_exit_core(lock, wip, do_yield);
}

SPI_MASTER_ATTR esp_err_t spi_bus_lock_bg_request(spi_bus_lock_dev_t *dev_handle)
{
    req_core(dev_handle);
    return ESP_OK;
}

IRAM_ATTR esp_err_t spi_bus_lock_wait_bg_done(spi_bus_lock_dev_handle_t dev_handle, TickType_t wait)
{
    spi_bus_lock_t *lock = dev_handle->parent;
    LOCK_CHECK(lock->acquiring_dev == dev_handle, "Cannot wait for a device that is not acquired", ESP_ERR_INVALID_STATE);
    LOCK_CHECK(wait == portMAX_DELAY, "timeout other than portMAX_DELAY not supported", ESP_ERR_INVALID_ARG);

    // If no BG bits active, skip quickly. This is ensured by `spi_bus_lock_wait_bg_done`
    // cannot be executed with `bg_request` on the same device concurrently.
    if (lock_status_fetch(lock) & DEV_BG_MASK(dev_handle)) {
        // Clear the semaphore before checking
        dev_wait_prepare(dev_handle);
        if (lock_status_fetch(lock) & DEV_BG_MASK(dev_handle)) {
            //block until becoming the acquiring processor (help by previous acquiring processor)
            esp_err_t err = dev_wait(dev_handle, wait);
            //TODO: add timeout handling here.
            if (err != ESP_OK) return err;
        }
    }

    BUS_LOCK_DEBUG_EXECUTE_CHECK(!lock->acq_dev_bg_active);
    BUS_LOCK_DEBUG_EXECUTE_CHECK((lock_status_fetch(lock) & DEV_BG_MASK(dev_handle)) == 0);
    return ESP_OK;
}

SPI_MASTER_ISR_ATTR bool spi_bus_lock_bg_clear_req(spi_bus_lock_dev_t *dev_handle)
{
    bool finished = clear_pend_core(dev_handle);
    ESP_EARLY_LOGV(TAG, "dev %d served from bg.", dev_lock_get_id(dev_handle));
    return finished;
}

SPI_MASTER_ISR_ATTR bool spi_bus_lock_bg_check_dev_acq(spi_bus_lock_t *lock,
                                                       spi_bus_lock_dev_handle_t *out_dev_lock)
{
    BUS_LOCK_DEBUG_EXECUTE_CHECK(!lock->acquiring_dev);
    uint32_t status = lock_status_fetch(lock);
    return schedule_core(lock, status, out_dev_lock);
}

SPI_MASTER_ISR_ATTR bool spi_bus_lock_bg_check_dev_req(spi_bus_lock_dev_t *dev_lock)
{
    spi_bus_lock_t* lock = dev_lock->parent;
    uint32_t status = lock_status_fetch(lock);
    uint32_t dev_status = status & dev_lock->mask;

    // move REQ bits of all device to corresponding PEND bits.
    // To reduce executing time, only done when the REQ bit of the calling device is set.
    if (dev_status & REQ_MASK) {
        update_pend_core(lock, status);
        return true;
    } else {
        return dev_status & PEND_MASK;
    }
}

SPI_MASTER_ISR_ATTR bool spi_bus_lock_bg_req_exist(spi_bus_lock_t *lock)
{
    uint32_t status = lock_status_fetch(lock);
    return status & BG_MASK;
}

/*******************************************************************************
 * Static variables of the locks of the main flash
 ******************************************************************************/
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
static spi_bus_lock_dev_t lock_main_flash_dev;

static spi_bus_lock_t main_spi_bus_lock = {
    /*
     * the main bus cache is permanently required, this flag is set here and never clear so that the
     * cache will always be enabled if acquiring devices yield.
     */
    .status = ATOMIC_VAR_INIT(WEAK_BG_FLAG),
    .acquiring_dev = NULL,
    .dev = {ATOMIC_VAR_INIT((intptr_t)&lock_main_flash_dev)},
    .new_req = 0,
    .periph_cs_num = SOC_SPI_PERIPH_CS_NUM(0),
};
const spi_bus_lock_handle_t g_main_spi_bus_lock = &main_spi_bus_lock;

esp_err_t spi_bus_lock_init_main_bus(void)
{
    spi_bus_main_set_lock(g_main_spi_bus_lock);
    return ESP_OK;
}

static StaticSemaphore_t main_flash_semphr;

static spi_bus_lock_dev_t lock_main_flash_dev = {
    .semphr = NULL,
    .parent = &main_spi_bus_lock,
    .mask = DEV_MASK(0),
};
const spi_bus_lock_dev_handle_t g_spi_lock_main_flash_dev = &lock_main_flash_dev;

esp_err_t spi_bus_lock_init_main_dev(void)
{
    g_spi_lock_main_flash_dev->semphr = xSemaphoreCreateBinaryStatic(&main_flash_semphr);
    if (g_spi_lock_main_flash_dev->semphr == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}
#else //CONFIG_SPI_FLASH_SHARE_SPI1_BUS

//when the dev lock is not initialized, point to NULL
const spi_bus_lock_dev_handle_t g_spi_lock_main_flash_dev = NULL;

#endif
