/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_lp_core_critical_section_shared.h"
#include "esp_cpu.h"
#if IS_ULP_COCPU
#include "ulp_lp_core_interrupts.h"
#endif

/* Masked interrupt threshold varies between different types of interrupt controller */
#if !SOC_INT_CLIC_SUPPORTED
#define    INT_MASK_THRESHOLD RVHAL_EXCM_LEVEL
#else /* SOC_INT_CLIC_SUPPORTED */
#define    INT_MASK_THRESHOLD RVHAL_EXCM_LEVEL_CLIC
#endif /* !SOC_INIT_CLIC_SUPPORTED */

void ulp_lp_core_spinlock_init(ulp_lp_core_spinlock_t *lock)
{
    int i = 0;
    for (i = 0; i < LOCK_CANDIDATE_NUM_MAX; i++) {
        lock->level[i] = -1;
    }

    for (i = 0; i < LOCK_CANDIDATE_NUM_MAX - 1; i++) {
        lock->last_to_enter[i] = 0;
    }
}

#if RTC_MEM_AMO_INSTRUCTIONS_VERIFIED
/* ULP spinlock ACQ/REL functions also have a hardware implementation base on AMOSWAP instructions,
   which has much better performance but have not been thoroughly verified yet on RTC memory. This set
   of implementation will be adapted once it's verified.
   */
static void ulp_lp_core_spinlock_acquire(ulp_lp_core_spinlock_t *lock)
{

    /* Based on sample code for AMOSWAP from RISCV specs v2.1 */
    asm volatile(
        "li t0, 1\n"                    // Initialize swap value.
        "1:\n"
        "lw t1, (%0)\n"                 // Check if lock is held.
        "bnez t1, 1b\n"              // Retry if held.
        "amoswap.w.aq t1, t0, (%0)\n"   // Attempt to acquire lock.
        "bnez t1, 1b\n"              // Retry if held.
        :
        : "r"(lock)
        : "t0", "t1", "memory"
    );
}

static void ulp_lp_core_spinlock_release(ulp_lp_core_spinlock_t *lock)
{
    asm volatile(
        "amoswap.w.rl x0, x0, (%0)\n"   // Release lock by storing 0
        :
        : "r"(lock)
        : "memory"
    );
}
#else // !RTC_MEM_AMO_INSTRUCTIONS_VERIFIED
/**
 * @brief Obtain the id of current lock candidate.
 *
 * @return lock candidate id
 */
static int ulp_lp_core_spinlock_get_candidate_id(void)
{
    int lock_candidate_id = 0;
#if !IS_ULP_COCPU
    /* Refer to ulp_lp_core_spinlock_candidate_t, each HP_CORE lock candidate's index is the core id plus 1 */
    lock_candidate_id = esp_cpu_get_core_id() + 1;
#else
    lock_candidate_id = LOCK_CANDIDATE_LP_CORE;
#endif
    return lock_candidate_id;
}

/**
 * @brief Software lock implementation is based on the filter algorithm, which is a generalization of Peterson's algorithm, https://en.wikipedia.org/wiki/Peterson%27s_algorithm#Filter_algorithm:_Peterson's_algorithm_for_more_than_two_processes
 *
 */

/**
 * @brief Attempt to acquire the lock. Spins until lock is acquired.
 *
 * @note  This lock is designed for being used by multiple threads, it is safe to try to acquire it
 *        simultaneously from multiple threads on multiple main CPU cores and LP CPU.
 *
 * @note  This function is private to ulp lp core critical section and shall not be called from anywhere else.
 *
 * @param lock Pointer to lock struct, shared with ULP
 */
static void ulp_lp_core_spinlock_acquire(ulp_lp_core_spinlock_t *lock)
{
    /* Level index */
    int lv = 0;
    /* Candidate index */
    int candidate = 0;

    /* Index of the current lock candidate */
    int lock_candidate_id = ulp_lp_core_spinlock_get_candidate_id();

    for (lv = 0; lv < (int)LOCK_CANDIDATE_NUM_MAX - 1; lv++) {
        /* Each candidate has to go through all the levels in order to get the spinlock. Start by notifying other candidates, we have reached level `lv` */
        lock->level[lock_candidate_id] = lv;
        /* Notify other candidates we are the latest one who entered level `lv` */
        lock->last_to_enter[lv] = lock_candidate_id;
        /* If there is any candidate that reached the same or a higher level than this candidate, wait for it to finish. Advance to the next level if another candidate becomes the latest one to arrive at our current level */
        for (candidate = 0; candidate < (int)LOCK_CANDIDATE_NUM_MAX; candidate++) {
            while ((candidate != lock_candidate_id) && (lock->level[candidate] >= lv && lock->last_to_enter[lv] == lock_candidate_id)) {
            }
        }
    }
}

/**
 * @brief Release the lock.
 *
 * @note This function is private to ulp lp core critical section and shall not be called from anywhere else.
 *
 * @param lock Pointer to lock struct, shared with ULP
 */
static void ulp_lp_core_spinlock_release(ulp_lp_core_spinlock_t *lock)
{
    int lock_candidate_id = ulp_lp_core_spinlock_get_candidate_id();

    lock->level[lock_candidate_id] = -1;
}
#endif

void ulp_lp_core_enter_critical(ulp_lp_core_spinlock_t *lock)
{
    /* disable interrupt */
#if !IS_ULP_COCPU   // HP core
    unsigned prev_int_level = rv_utils_set_intlevel_regval(INT_MASK_THRESHOLD);
    lock->prev_int_level = prev_int_level;
#else   // LP core
    ulp_lp_core_intr_disable();
#endif
    /* Busy-wait to acquire the spinlock. Use caution when deploying this lock in time-sensitive scenarios. */
    ulp_lp_core_spinlock_acquire(lock);
}

void ulp_lp_core_exit_critical(ulp_lp_core_spinlock_t *lock)
{
    ulp_lp_core_spinlock_release(lock);

    /* re-enable interrupt */
#if !IS_ULP_COCPU   // HP core
    unsigned prev_int_level = lock->prev_int_level;
    rv_utils_restore_intlevel_regval(prev_int_level);
#else   // LP core
    ulp_lp_core_intr_enable();
#endif
}
