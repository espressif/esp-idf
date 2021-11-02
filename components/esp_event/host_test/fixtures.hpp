/* ESP Event Fixtures

   This code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_event.h"

#include "catch.hpp"

extern "C" {
#include "Mocktask.h"
#include "Mockqueue.h"
}

/**
 * Exception which is thrown if there is some internal cmock error which results in a
 * longjump to the location of a TEST_PROTECT() call.
 *
 * @note This is a temporary solution until there is a better integration of CATCH into CMock.
 *      Note also that usually there will be a segfault when cmock fails a second time.
 *      This means paying attention to the first error message is crucial for removing errors.
 */
class CMockException : public std::exception {
public:
    virtual ~CMockException() { }

    /**
     * @return A reminder to look at the actual cmock log.
     */
    virtual const char *what() const noexcept
    {
        return "CMock encountered an error. Look at the CMock log";
    }
};

/**
 * Helper macro for setting up a test protect call for CMock.
 *
 * This macro should be used at the beginning of any test cases
 * which use generated CMock mock functions.
 * This is necessary because CMock uses longjmp which screws up C++ stacks and
 * also the CATCH mechanisms.
 *
 * @note This is a temporary solution until there is a better integration of CATCH into CMock.
 *      Note also that usually there will be a segfault when cmock fails a second time.
 *      This means paying attention to the first error message is crucial for removing errors.
 */
#define CMOCK_SETUP() \
    do { \
        if (!TEST_PROTECT()) {      \
            throw CMockException(); \
        }                           \
    } \
    while (0)

struct CMockFix {
    CMockFix()
    {
        CMOCK_SETUP();
    }

    ~CMockFix()
    {
        Mocktask_Verify();
        Mockqueue_Verify();
    }
};

enum class CreateAnd {
    FAIL,
    SUCCEED,
    IGNORE
};

struct MockQueue : public CMockFix {
    MockQueue (CreateAnd flags) : queue(reinterpret_cast<QueueHandle_t>(0xdeadbeef))
    {
        if (flags == CreateAnd::FAIL) {
            xQueueGenericCreate_ExpectAnyArgsAndReturn(nullptr);
        } else if (flags == CreateAnd::IGNORE) {
            xQueueGenericCreate_IgnoreAndReturn(queue);
            vQueueDelete_Ignore();
        } else {
            xQueueGenericCreate_ExpectAnyArgsAndReturn(queue);
            vQueueDelete_Expect(queue);
        }
    }

    ~MockQueue()
    {
        xQueueGenericCreate_StopIgnore();
        vQueueDelete_StopIgnore();
    }

    QueueHandle_t queue;
};

struct MockMutex : public CMockFix {
    MockMutex (CreateAnd flags) : sem(reinterpret_cast<QueueHandle_t>(0xdeadbeef))
    {
        if (flags == CreateAnd::FAIL) {
            xQueueCreateMutex_ExpectAnyArgsAndReturn(nullptr);
        } else if (flags == CreateAnd::IGNORE) {
            xQueueCreateMutex_IgnoreAndReturn(sem);
            vQueueDelete_Ignore();
        } else {
            xQueueCreateMutex_ExpectAnyArgsAndReturn(sem);
            vQueueDelete_Expect(sem);
        }
    }

    ~MockMutex()
    {
        xQueueCreateMutex_StopIgnore();
        vQueueDelete_StopIgnore();
    }

    QueueHandle_t sem;
};

struct MockTask : public CMockFix {
    MockTask (CreateAnd flags) : task((TaskHandle_t) 1)
    {
        if (flags == CreateAnd::FAIL) {
            xTaskCreatePinnedToCore_ExpectAnyArgsAndReturn(pdFALSE);
        } else if (flags == CreateAnd::IGNORE) {
            xTaskCreatePinnedToCore_IgnoreAndReturn(pdTRUE);
            xTaskCreatePinnedToCore_ReturnThruPtr_pvCreatedTask(&task);
            vTaskDelete_Ignore();
        } else {
            xTaskCreatePinnedToCore_ExpectAnyArgsAndReturn(pdTRUE);
            xTaskCreatePinnedToCore_ReturnThruPtr_pvCreatedTask(&task);
            vTaskDelete_Expect(task);
        }
    }

    ~MockTask()
    {
        xTaskCreatePinnedToCore_StopIgnore();
        vTaskDelete_StopIgnore();
    }

    TaskHandle_t task;
};
