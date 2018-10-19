// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

// mbcontroller.c
// Implementation of the modbus controller
// The modbus controller is responsible for processing of modbus packet and transfer data
// into parameter instance.

#include <sys/time.h>               // for calculation of time stamp in milliseconds
#include "esp_log.h"                // for log_write
#include "freertos/FreeRTOS.h"      // for task creation and queue access
#include "freertos/task.h"          // for task api access
#include "freertos/event_groups.h"  // for event groups
#include "freertos/queue.h"         // for queue api access
#include "mb.h"                     // for mb types definition
#include "mbutils.h"                // for mbutils functions definition for stack callback
#include "sdkconfig.h"              // for KConfig values
#include "mbcontroller.h"

static const char* TAG = "MB_CONTROLLER";

#define MB_CHECK(a, ret_val, str, ...) \
    if (!(a)) { \
        ESP_LOGE(TAG, "%s(%u): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

// The Macros below handle the endianness while transfer N byte data into buffer
#define _XFER_4_RD(dst, src) { \
    *(uint8_t *)(dst)++ = *(uint8_t*)(src + 1); \
    *(uint8_t *)(dst)++ = *(uint8_t*)(src + 0); \
    *(uint8_t *)(dst)++ = *(uint8_t*)(src + 3); \
    *(uint8_t *)(dst)++ = *(uint8_t*)(src + 2); \
    (src) += 4; \
}

#define _XFER_2_RD(dst, src) { \
    *(uint8_t *)(dst)++ = *(uint8_t *)(src + 1); \
    *(uint8_t *)(dst)++ = *(uint8_t *)(src + 0); \
    (src) += 2; \
}

#define _XFER_4_WR(dst, src) { \
    *(uint8_t *)(dst + 1) = *(uint8_t *)(src)++; \
    *(uint8_t *)(dst + 0) = *(uint8_t *)(src)++; \
    *(uint8_t *)(dst + 3) = *(uint8_t *)(src)++; \
    *(uint8_t *)(dst + 2) = *(uint8_t *)(src)++ ; \
}

#define _XFER_2_WR(dst, src) { \
    *(uint8_t *)(dst + 1) = *(uint8_t *)(src)++; \
    *(uint8_t *)(dst + 0) = *(uint8_t *)(src)++; \
}

#ifdef CONFIG_MB_CONTROLLER_SLAVE_ID_SUPPORT

#define MB_ID_BYTE0(id) ((uint8_t)(id))
#define MB_ID_BYTE1(id) ((uint8_t)(((uint16_t)(id) >> 8) & 0xFF))
#define MB_ID_BYTE2(id) ((uint8_t)(((uint32_t)(id) >> 16) & 0xFF))
#define MB_ID_BYTE3(id) ((uint8_t)(((uint32_t)(id) >> 24) & 0xFF))

#define MB_CONTROLLER_SLAVE_ID (CONFIG_MB_CONTROLLER_SLAVE_ID)
#define MB_SLAVE_ID_SHORT      (MB_ID_BYTE3(MB_CONTROLLER_SLAVE_ID))

// Slave ID constant
static uint8_t mb_slave_id[] = { MB_ID_BYTE0(MB_CONTROLLER_SLAVE_ID),
                                MB_ID_BYTE1(MB_CONTROLLER_SLAVE_ID),
                                MB_ID_BYTE2(MB_CONTROLLER_SLAVE_ID) };

#endif

// Event group parameters
static TaskHandle_t mb_controller_task_handle = NULL;
static EventGroupHandle_t mb_controller_event_group = NULL;
static QueueHandle_t mb_controller_notification_queue_handle = NULL;

static uint8_t mb_type = 0;
static uint8_t mb_address = 0;
static uint8_t mb_port = 0;
static uint32_t mb_speed = 0;
static uint16_t mb_parity = 0;

// This is array of Modbus address area descriptors
static mb_register_area_descriptor_t mb_area_descriptors[MB_PARAM_COUNT] = { 0 };

// The helper function to get time stamp in microseconds
static uint64_t get_time_stamp()
{
    uint64_t time_stamp = esp_timer_get_time();
    return time_stamp;
}

// Helper function to send parameter information to application task
static esp_err_t send_param_info(mb_event_group_t par_type, uint16_t mb_offset,
                                    uint8_t* par_address, uint16_t par_size)
{
    esp_err_t error = ESP_FAIL;
    mb_param_info_t par_info;
    // Check if queue is not full the send parameter information
    par_info.type = par_type;
    par_info.size = par_size;
    par_info.address = par_address;
    par_info.time_stamp = get_time_stamp();
    par_info.mb_offset = mb_offset;
    BaseType_t status = xQueueSend(mb_controller_notification_queue_handle, &par_info, MB_PAR_INFO_TOUT);
    if (pdTRUE == status) {
        ESP_LOGD(TAG, "Queue send parameter info (type, address, size): %d, 0x%.4x, %d",
                par_type, (uint32_t)par_address, par_size);
        error = ESP_OK;
    } else if (errQUEUE_FULL == status) {
        ESP_LOGD(TAG, "Parameter queue is overflowed.");
    }
    return error;
}

static esp_err_t send_param_access_notification(mb_event_group_t event)
{
    esp_err_t err = ESP_FAIL;
    mb_event_group_t bits = (mb_event_group_t)xEventGroupSetBits(mb_controller_event_group, (EventBits_t)event);
    if (bits & event) {
        ESP_LOGD(TAG, "The MB_REG_CHANGE_EVENT = 0x%.2x is set.", (uint8_t)event);
        err = ESP_OK;
    }
    return err;
}

// Modbus task function
static void modbus_task(void *pvParameters) {
    // Main Modbus stack processing cycle
    for (;;) {
        BaseType_t status = xEventGroupWaitBits(mb_controller_event_group,
                                                (BaseType_t)(MB_EVENT_STACK_STARTED),
                                                pdFALSE, // do not clear bits
                                                pdFALSE,
                                                portMAX_DELAY);
        // Check if stack started then poll for data
        if (status & MB_EVENT_STACK_STARTED) {
            (void)eMBPoll(); // allow stack to process data
            (void)xMBPortSerialTxPoll(); // Send response buffer if ready
        }
    }
}

// Blocking function to get event on parameter group change for application task
mb_event_group_t mbcontroller_check_event(mb_event_group_t group)
{
    assert(mb_controller_event_group != NULL);
    BaseType_t status = xEventGroupWaitBits(mb_controller_event_group, (BaseType_t)group,
                                            pdTRUE , pdFALSE, portMAX_DELAY);
    return (mb_event_group_t)status;
}

esp_err_t mbcontroller_set_descriptor(const mb_register_area_descriptor_t descr_info)
{
    MB_CHECK(((descr_info.type < MB_PARAM_COUNT) && (descr_info.type >= MB_PARAM_HOLDING)),
                ESP_ERR_INVALID_ARG, "mb incorrect modbus instance type = (0x%x).",
                (uint32_t)descr_info.type);
    MB_CHECK((descr_info.address != NULL),
                ESP_ERR_INVALID_ARG, "mb instance pointer is NULL.");
    MB_CHECK((descr_info.size >= MB_INST_MIN_SIZE) && (descr_info.size < (MB_INST_MAX_SIZE)),
                ESP_ERR_INVALID_ARG, "mb instance size is incorrect = (0x%x).",
                (uint32_t)descr_info.size);
    mb_area_descriptors[descr_info.type].type = descr_info.type;
    mb_area_descriptors[descr_info.type].start_offset = descr_info.start_offset;
    mb_area_descriptors[descr_info.type].address = (uint8_t*)descr_info.address;
    mb_area_descriptors[descr_info.type].size = descr_info.size;
    return ESP_OK;
}

// Initialization of Modbus controller
esp_err_t mbcontroller_init(void) {
    mb_type = MB_MODE_RTU;
    mb_address = MB_DEVICE_ADDRESS;
    mb_port = MB_UART_PORT;
    mb_speed = MB_DEVICE_SPEED;
    mb_parity = MB_PARITY_NONE;

    // Initialization of active context of the modbus controller
    BaseType_t status = 0;
    // Parameter change notification queue
    mb_controller_event_group = xEventGroupCreate();
    MB_CHECK((mb_controller_event_group != NULL),
            ESP_ERR_NO_MEM, "mb event group error.");
    // Parameter change notification queue
    mb_controller_notification_queue_handle = xQueueCreate(
                                                MB_CONTROLLER_NOTIFY_QUEUE_SIZE,
                                                sizeof(mb_param_info_t));
    MB_CHECK((mb_controller_notification_queue_handle != NULL),
            ESP_ERR_NO_MEM, "mb notify queue creation error.");
    // Create modbus controller task
    status = xTaskCreate((void*)&modbus_task,
                            "modbus_task",
                            MB_CONTROLLER_STACK_SIZE,
                            NULL,
                            MB_CONTROLLER_PRIORITY,
                            &mb_controller_task_handle);
    if (status != pdPASS) {
        vTaskDelete(mb_controller_task_handle);
        MB_CHECK((status == pdPASS), ESP_ERR_NO_MEM,
                "mb controller task creation error, xTaskCreate() returns (0x%x).",
                (uint32_t)status);
    }
    assert(mb_controller_task_handle != NULL); // The task is created but handle is incorrect
    return ESP_OK;
}

// Function to get notification about parameter change from application task
esp_err_t mbcontroller_get_param_info(mb_param_info_t* reg_info, uint32_t timeout)
{
    esp_err_t err = ESP_ERR_TIMEOUT;
    MB_CHECK((mb_controller_notification_queue_handle != NULL),
                ESP_ERR_INVALID_ARG, "mb queue handle is invalid.");
    MB_CHECK((reg_info != NULL), ESP_ERR_INVALID_ARG, "mb register information is invalid.");
    BaseType_t status = xQueueReceive(mb_controller_notification_queue_handle,
                                        reg_info, pdMS_TO_TICKS(timeout));
    if (status == pdTRUE) {
        err = ESP_OK;
    }
    return err;
}

// Start Modbus controller start function
esp_err_t mbcontroller_start(void)
{
    eMBErrorCode status = MB_EIO;
    // Initialize Modbus stack using mbcontroller parameters
    status = eMBInit((eMBMode)mb_type, (UCHAR)mb_address, (UCHAR)mb_port,
                            (ULONG)mb_speed, (eMBParity)mb_parity);
    MB_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE,
            "mb stack initialization failure, eMBInit() returns (0x%x).", status);
#ifdef CONFIG_MB_CONTROLLER_SLAVE_ID_SUPPORT
    status = eMBSetSlaveID(MB_SLAVE_ID_SHORT, TRUE, (UCHAR*)mb_slave_id, sizeof(mb_slave_id));
    MB_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE, "mb stack set slave ID failure.");
#endif
    status = eMBEnable();
    MB_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE,
            "mb stack set slave ID failure, eMBEnable() returned (0x%x).", (uint32_t)status);
    // Set the mbcontroller start flag
    EventBits_t flag = xEventGroupSetBits(mb_controller_event_group,
                                            (EventBits_t)MB_EVENT_STACK_STARTED);
    MB_CHECK((flag & MB_EVENT_STACK_STARTED),
                ESP_ERR_INVALID_STATE, "mb stack start event set error.");
    return ESP_OK;
}

// Modbus controller destroy function
esp_err_t mbcontroller_destroy(void)
{
    eMBErrorCode mb_error = MB_ENOERR;
    // Stop polling by clearing correspondent bit in the event group
    EventBits_t flag = xEventGroupClearBits(mb_controller_event_group,
                                    (EventBits_t)MB_EVENT_STACK_STARTED);
    MB_CHECK((flag & MB_EVENT_STACK_STARTED),
                ESP_ERR_INVALID_STATE, "mb stack stop event failure.");
    // Desable and then destroy the Modbus stack
    mb_error = eMBDisable();
    MB_CHECK((mb_error == MB_ENOERR), ESP_ERR_INVALID_STATE, "mb stack disable failure.");
    (void)vTaskDelete(mb_controller_task_handle);
    (void)vQueueDelete(mb_controller_notification_queue_handle);
    (void)vEventGroupDelete(mb_controller_event_group);
    mb_error = eMBClose();
    MB_CHECK((mb_error == MB_ENOERR), ESP_ERR_INVALID_STATE,
            "mb stack close failure returned (0x%x).", (uint32_t)mb_error);
    return ESP_OK;
}

// Setup modbus controller parameters
esp_err_t mbcontroller_setup(const mb_communication_info_t comm_info)
{
    MB_CHECK(((comm_info.mode == MB_MODE_RTU) || (comm_info.mode == MB_MODE_ASCII)),
                ESP_ERR_INVALID_ARG, "mb incorrect mode = (0x%x).",
                (uint32_t)comm_info.mode);
    MB_CHECK((comm_info.slave_addr <= MB_ADDRESS_MAX),
                ESP_ERR_INVALID_ARG, "mb wrong slave address = (0x%x).",
                (uint32_t)comm_info.slave_addr);
    MB_CHECK((comm_info.port <= UART_NUM_2), ESP_ERR_INVALID_ARG,
                "mb wrong port to set = (0x%x).", (uint32_t)comm_info.port);
    MB_CHECK((comm_info.parity <= UART_PARITY_EVEN), ESP_ERR_INVALID_ARG,
                "mb wrong parity option = (0x%x).", (uint32_t)comm_info.parity);
    mb_type = (uint8_t)comm_info.mode;
    mb_address = (uint8_t)comm_info.slave_addr;
    mb_port = (uint8_t)comm_info.port;
    mb_speed = (uint32_t)comm_info.baudrate;
    mb_parity = (uint8_t)comm_info.parity;
    return ESP_OK;
}

/* ----------------------- Callback functions for Modbus stack ---------------------------------*/
// These are executed by modbus stack to read appropriate type of registers.

// This is required to suppress warning when register start address is zero
#pragma GCC diagnostic ignored "-Wtype-limits"

// Callback function for reading of MB Input Registers
eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                USHORT usNRegs)
{
    assert(pucRegBuffer != NULL);
    USHORT usRegInputNregs = (USHORT)(mb_area_descriptors[MB_PARAM_INPUT].size >> 1); // Number of input registers
    USHORT usInputRegStart = (USHORT)mb_area_descriptors[MB_PARAM_INPUT].start_offset; // Get Modbus start address
    UCHAR* pucInputBuffer = (UCHAR*)mb_area_descriptors[MB_PARAM_INPUT].address; // Get instance address
    USHORT usRegs = usNRegs;
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex;
    // If input or configuration parameters are incorrect then return an error to stack layer
    if ((usAddress >= usInputRegStart)
            && (pucInputBuffer != NULL)
            && (usNRegs >= 1)
            && ((usAddress + usRegs) <= (usInputRegStart + usRegInputNregs + 1))
            && (usRegInputNregs >= 1)) {
        iRegIndex = (USHORT)(usAddress - usInputRegStart - 1);
        iRegIndex <<= 1; // register Address to byte address
        pucInputBuffer += iRegIndex;
        UCHAR* pucBufferStart = pucInputBuffer;
        while (usRegs > 0) {
            _XFER_2_RD(pucRegBuffer, pucInputBuffer);
            iRegIndex += 2;
            usRegs -= 1;
        }
        // Send access notification
        (void)send_param_access_notification(MB_EVENT_INPUT_REG_RD);
        // Send parameter info to application task
        (void)send_param_info(MB_EVENT_INPUT_REG_RD, (uint16_t)usAddress,
                        (uint8_t*)pucBufferStart, (uint16_t)usNRegs);
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

// Callback function for reading of MB Holding Registers
// Executed by stack when request to read/write holding registers is received
eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs, eMBRegisterMode eMode)
{
    assert(pucRegBuffer != NULL);
    USHORT usRegHoldingNregs = (USHORT)(mb_area_descriptors[MB_PARAM_HOLDING].size >> 1);
    USHORT usRegHoldingStart = (USHORT)mb_area_descriptors[MB_PARAM_HOLDING].start_offset;
    UCHAR* pucHoldingBuffer = (UCHAR*)mb_area_descriptors[MB_PARAM_HOLDING].address;
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex;
    USHORT usRegs = usNRegs;
    // Check input and configuration parameters for correctness
    if ((usAddress >= usRegHoldingStart)
            && (pucHoldingBuffer != NULL)
            && ((usAddress + usRegs) <= (usRegHoldingStart + usRegHoldingNregs + 1))
            && (usRegHoldingNregs >= 1)
            && (usNRegs >= 1)) {
        iRegIndex = (USHORT) (usAddress - usRegHoldingStart - 1);
        iRegIndex <<= 1; // register Address to byte address
        pucHoldingBuffer += iRegIndex;
        UCHAR* pucBufferStart = pucHoldingBuffer;
        switch (eMode) {
            case MB_REG_READ:
                while (usRegs > 0) {
                    _XFER_2_RD(pucRegBuffer, pucHoldingBuffer);
                    iRegIndex += 2;
                    usRegs -= 1;
                };
                // Send access notification
                (void)send_param_access_notification(MB_EVENT_HOLDING_REG_RD);
                // Send parameter info
                (void)send_param_info(MB_EVENT_HOLDING_REG_RD, (uint16_t)usAddress,
                                (uint8_t*)pucBufferStart, (uint16_t)usNRegs);
                break;
            case MB_REG_WRITE:
                while (usRegs > 0) {
                    _XFER_2_WR(pucHoldingBuffer, pucRegBuffer);
                    pucHoldingBuffer += 2;
                    iRegIndex += 2;
                    usRegs -= 1;
                };
                // Send access notification
                (void)send_param_access_notification(MB_EVENT_HOLDING_REG_WR);
                // Send parameter info
                (void)send_param_info(MB_EVENT_HOLDING_REG_WR, (uint16_t)usAddress,
                                (uint8_t*)pucBufferStart, (uint16_t)usNRegs);
                break;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

// Callback function for reading of MB Coils Registers
eMBErrorCode eMBRegCoilsCB(UCHAR* pucRegBuffer, USHORT usAddress,
        USHORT usNCoils, eMBRegisterMode eMode)
{
    assert(NULL != pucRegBuffer);
    USHORT usRegCoilNregs = (USHORT)(mb_area_descriptors[MB_PARAM_COIL].size >> 1); // number of registers in storage area
    USHORT usRegCoilsStart = (USHORT)mb_area_descriptors[MB_PARAM_COIL].start_offset; // MB offset of coils registers
    UCHAR* pucRegCoilsBuf = (UCHAR*)mb_area_descriptors[MB_PARAM_COIL].address;
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex;
    USHORT usCoils = usNCoils;
    usAddress--; // The address is already +1
    if ((usAddress >= usRegCoilsStart)
            && (usRegCoilNregs >= 1)
            && ((usAddress + usCoils) <= (usRegCoilsStart + (usRegCoilNregs << 4) + 1))
            && (pucRegCoilsBuf != NULL)
            && (usNCoils >= 1)) {
        iRegIndex = (USHORT) (usAddress - usRegCoilsStart);
        CHAR* pucCoilsDataBuf = (CHAR*)(pucRegCoilsBuf + (iRegIndex >> 3));
        switch (eMode) {
            case MB_REG_READ:
                while (usCoils > 0) {
                    UCHAR ucResult = xMBUtilGetBits((UCHAR*)pucRegCoilsBuf, iRegIndex, 1);
                    xMBUtilSetBits(pucRegBuffer, iRegIndex - (usAddress - usRegCoilsStart), 1, ucResult);
                    iRegIndex++;
                    usCoils--;
                }
                // Send an event to notify application task about event
                (void)send_param_access_notification(MB_EVENT_COILS_WR);
                (void)send_param_info(MB_EVENT_COILS_WR, (uint16_t)usAddress,
                                (uint8_t*)(pucCoilsDataBuf), (uint16_t)usNCoils);
                break;
            case MB_REG_WRITE:
                while (usCoils > 0) {
                    UCHAR ucResult = xMBUtilGetBits(pucRegBuffer,
                            iRegIndex - (usAddress - usRegCoilsStart), 1);
                    xMBUtilSetBits((uint8_t*)pucRegCoilsBuf, iRegIndex, 1, ucResult);
                    iRegIndex++;
                    usCoils--;
                }
                // Send an event to notify application task about event
                (void)send_param_access_notification(MB_EVENT_COILS_WR);
                (void)send_param_info(MB_EVENT_COILS_WR, (uint16_t)usAddress,
                                (uint8_t*)pucCoilsDataBuf, (uint16_t)usNCoils);
                break;
        } // switch ( eMode )
    } else {
        // If the configuration or input parameters are incorrect then return error to stack
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

// Callback function for reading of MB Discrete Input Registers
eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress,
                            USHORT usNDiscrete)
{
    assert(pucRegBuffer != NULL);
    USHORT usRegDiscreteNregs = (USHORT)(mb_area_descriptors[MB_PARAM_DISCRETE].size >> 1); // number of registers in storage area
    USHORT usRegDiscreteStart = (USHORT)mb_area_descriptors[MB_PARAM_DISCRETE].start_offset; // MB offset of registers
    UCHAR* pucRegDiscreteBuf = (UCHAR*)mb_area_descriptors[MB_PARAM_DISCRETE].address; // the storage address
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex, iRegBitIndex, iNReg;
    UCHAR* pucDiscreteInputBuf;
    iNReg = usNDiscrete / 8 + 1;
    pucDiscreteInputBuf = (UCHAR*) pucRegDiscreteBuf;
    // It already plus one in modbus function method.
    usAddress--;
    if ((usAddress >= usRegDiscreteStart)
            && (usRegDiscreteNregs >= 1)
            && (pucRegDiscreteBuf != NULL)
            && ((usAddress + usNDiscrete) <= (usRegDiscreteStart + (usRegDiscreteNregs * 16)))
            && (usNDiscrete >= 1)) {
        iRegIndex = (USHORT) (usAddress - usRegDiscreteStart) / 8; // Get register index in the buffer for bit number
        iRegBitIndex = (USHORT)(usAddress - usRegDiscreteStart) % 8; // Get bit index
        UCHAR* pucTempBuf = &pucDiscreteInputBuf[iRegIndex];
        while (iNReg > 0) {
            *pucRegBuffer++ = xMBUtilGetBits(&pucDiscreteInputBuf[iRegIndex++], iRegBitIndex, 8);
            iNReg--;
        }
        pucRegBuffer--;
        // Last discrete
        usNDiscrete = usNDiscrete % 8;
        // Filling zero to high bit
        *pucRegBuffer = *pucRegBuffer << (8 - usNDiscrete);
        *pucRegBuffer = *pucRegBuffer >> (8 - usNDiscrete);
        // Send an event to notify application task about event
        (void)send_param_access_notification(MB_EVENT_DISCRETE_RD);
        (void)send_param_info(MB_EVENT_DISCRETE_RD, (uint16_t)usAddress,
                            (uint8_t*)pucTempBuf, (uint16_t)usNDiscrete);
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
#pragma GCC diagnostic pop   // require GCC
