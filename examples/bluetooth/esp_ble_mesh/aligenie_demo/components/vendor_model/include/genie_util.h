// Copyright (C) 2018-2020 Alibaba Group Holding Limited
// Adaptations to ESP-IDF Copyright (c) 2020 Espressif Systems (Shanghai) Co. Ltd.
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

#ifndef _GENIE_UTIL_H_
#define _GENIE_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif /**< __cplusplus */

/**
 * p_elem: pointer to the element which the messsage want to be sent to
 * retry: retry counts before desired confirm message received
 * * fill negative value if retransmission is not necessary
 * * fill positive value if retransmission is needed
 * * * will be round in this scope - [GENIE_MODEL_MSG_DFT_RETRY_TIMES, GENIE_MODEL_MSG_MAX_RETRY_TIMES]
 * retry_period: wait for retry_period before retransmit the message, in unit of ms
 * opid: hightest byte in Opcode defined in the vendor model spec designed by Alibaba IoT Group
 * * e.g. for Vendor Message Attr Get message, Opcode is 0xD001A8, corresponding opid is 0xD0
 * * refer to the marco named VENDOR_OP_ATTR_xxxx
 * tid:
 * * if the message is with type of GENIE_OP_ATTR_CONFIME or GENIE_OP_ATTR_CONFIME_TG,
 * * tid should be filled with the replied message's tid
 * len: payload length
 * data: pointer to the genie vendor message's payload
 * */
typedef struct {
    esp_ble_mesh_elem_t *p_elem;
    int8_t retry;
    uint16_t retry_period;
    uint8_t opid;
    uint8_t tid;
    uint16_t len;
    uint8_t *data;
} genie_model_msg_t;

typedef void (*util_timer_handler_t)(void *args);

typedef struct {
    esp_timer_handle_t timer;
    util_timer_handler_t handler;
    void *args;
    uint32_t timeout;
    uint32_t start_ms;
} util_timer_t;

/* semaphore define */
typedef struct {
    SemaphoreHandle_t sem;
} util_semaphore_t;

/**
 * @brief Initialize a timer.
 *
 * This routine initializes a timer, prior to its first use.
 *
 * @param timer     Address of timer.
 * @param handle Function to invoke each time the timer expires.
 * @param args   Arguments sent to handle.
 *
 * @return N/A
 */
void util_timer_init(util_timer_t *timer, util_timer_handler_t handle, void *args);

/**
 * @brief Start a timer.
 *
 * @param timer    Address of timer.
 * @param timeout  time before timeout happen(in milliseconds).
 *
 * @return N/A
 */
void util_timer_start(util_timer_t *timer, uint32_t timeout);

/**
 * @brief Stop a timer.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
void util_timer_stop(util_timer_t *timer);

/**
* @brief check wether a timer is started or not.
*
* @param timer     Address of timer.
*
* @return true if timer is started; otherwise false.
*/
bool util_timer_is_started(util_timer_t *timer);

/**
 * @brief Initialize a semaphore.
 *
 * This routine initializes a semaphore object, prior to its first use.
 *
 * @param sem Address of the semaphore.
 * @param initial_count Initial semaphore count.
 * @param limit Maximum permitted semaphore count.
 *
 * @return 0 Creat a semaphore succcess
 */
int util_semaphore_init(util_semaphore_t *sem, uint32_t initial_count, uint32_t limit);

/**
 * @brief Take a semaphore.
 *
 * This routine takes @a sem.
 *
 * @note Can be called by ISRs, but @a timeout must be set to K_NO_WAIT.
 *
 * @param sem Address of the semaphore.
 * @param timeout Waiting period to take the semaphore (in milliseconds),
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @note When porting code from the nanokernel legacy API to the new API, be
 * careful with the return value of this function. The return value is the
 * reverse of the one of nano_sem_take family of APIs: 0 means success, and
 * non-zero means failure, while the nano_sem_take family returns 1 for success
 * and 0 for failure.
 *
 * @retval 0 Semaphore taken.
 * @retval -EBUSY Returned without waiting.
 * @retval -EAGAIN Waiting period timed out.
 */
int util_semaphore_take(util_semaphore_t *sem, uint32_t timeout);

/**
 * @brief Give a semaphore.
 *
 * This routine gives @a sem, unless the semaphore is already at its maximum
 * permitted count.
 *
 * @note Can be called by ISRs.
 *
 * @param sem Address of the semaphore.
 *
 * @return 0 Give semaphore success
 */
int util_semaphore_give(util_semaphore_t *sem);

/**
 * @brief Delete a semaphore.
 *
 * This routine delete @a sem,
 *
 * @note Can be called by ISRs.
 *
 * @param sem Address of the semaphore.
 *
 * @return 0 delete semaphore success
 */
int util_semaphore_delete(util_semaphore_t *sem);

/**
 * @brief Get a semaphore's count.
 *
 * This routine returns the current count of @a sem.
 *
 * @param sem Address of the semaphore.
 *
 * @return Current semaphore count.
 */
uint32_t util_semaphore_count_get(util_semaphore_t *sem);

/**
 * @brief Get a semaphore's count.
 *
 * This routine returns the current count of @a sem.
 *
 * @param sem Address of the semaphore.
 *
 * @return Current semaphore count.
 */
const char *util_hex2str(const void *buf, size_t len);

#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif /* _GENIE_UTIL_H_ */
