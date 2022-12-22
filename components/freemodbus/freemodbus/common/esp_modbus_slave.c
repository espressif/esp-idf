/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"                // for esp_err_t
#include "esp_timer.h"              // for esp_timer_get_time()
#include "sdkconfig.h"              // for KConfig defines

#include "mbc_slave.h"              // for slave private type definitions
#include "mbutils.h"                // for stack bit setting utilities
#include "esp_modbus_common.h"      // for common defines
#include "esp_modbus_slave.h"       // for public slave defines
#include "esp_modbus_callbacks.h"   // for modbus callbacks function pointers declaration

#ifdef CONFIG_FMB_CONTROLLER_SLAVE_ID_SUPPORT

#define MB_ID_BYTE0(id) ((uint8_t)(id))
#define MB_ID_BYTE1(id) ((uint8_t)(((uint16_t)(id) >> 8) & 0xFF))
#define MB_ID_BYTE2(id) ((uint8_t)(((uint32_t)(id) >> 16) & 0xFF))
#define MB_ID_BYTE3(id) ((uint8_t)(((uint32_t)(id) >> 24) & 0xFF))

#define MB_CONTROLLER_SLAVE_ID (CONFIG_FMB_CONTROLLER_SLAVE_ID)
#define MB_SLAVE_ID_SHORT      (MB_ID_BYTE3(MB_CONTROLLER_SLAVE_ID))

// Slave ID constant
static uint8_t mb_slave_id[] = { MB_ID_BYTE0(MB_CONTROLLER_SLAVE_ID),
                                MB_ID_BYTE1(MB_CONTROLLER_SLAVE_ID),
                                MB_ID_BYTE2(MB_CONTROLLER_SLAVE_ID) };

#endif

#define REG_SIZE(type, nregs) ((type == MB_PARAM_INPUT) || (type == MB_PARAM_HOLDING)) ? (nregs >> 1) : (nregs << 3)

// Common interface pointer for slave port
static mb_slave_interface_t* slave_interface_ptr = NULL;
static const char TAG[] __attribute__((unused)) = "MB_CONTROLLER_SLAVE";

// Searches the register in the area specified by type, returns descriptor if found, else NULL
static mb_descr_entry_t* mbc_slave_find_reg_descriptor(mb_param_type_t type, uint16_t addr, size_t regs)
{
    mb_descr_entry_t* it;
    uint16_t reg_size = 0;

    mb_slave_options_t* mbs_opts = &slave_interface_ptr->opts;

    if (LIST_EMPTY(&mbs_opts->mbs_area_descriptors[type])) {
        return NULL;
    }

    // search for the register in each area
    for (it = LIST_FIRST(&mbs_opts->mbs_area_descriptors[type]); it != NULL; it = LIST_NEXT(it, entries)) {
        reg_size = REG_SIZE(type, it->size);
        if ((addr >= it->start_offset)
            && (it->p_data)
            && (regs >= 1)
            && ((addr + regs) <= (it->start_offset + reg_size))
            && (reg_size >= 1)) {
            return it;
        }
    }
    return NULL;
}

static void mbc_slave_free_descriptors(void) {

    mb_descr_entry_t* it;
    mb_slave_options_t* mbs_opts = &slave_interface_ptr->opts;

    for (int descr_type = 0; descr_type < MB_PARAM_COUNT; descr_type++) {
        while ((it = LIST_FIRST(&mbs_opts->mbs_area_descriptors[descr_type]))) {
            LIST_REMOVE(it, entries);
            free(it);
        }
    }
}

void mbc_slave_init_iface(void* handler)
{
    slave_interface_ptr = (mb_slave_interface_t*) handler;
    mb_slave_options_t* mbs_opts = &slave_interface_ptr->opts;
    // Initialize list head for register areas
    LIST_INIT(&mbs_opts->mbs_area_descriptors[MB_PARAM_INPUT]);
    LIST_INIT(&mbs_opts->mbs_area_descriptors[MB_PARAM_HOLDING]);
    LIST_INIT(&mbs_opts->mbs_area_descriptors[MB_PARAM_COIL]);
    LIST_INIT(&mbs_opts->mbs_area_descriptors[MB_PARAM_DISCRETE]);
}

/**
 * Modbus controller destroy function
 */
esp_err_t mbc_slave_destroy(void)
{
    esp_err_t error = ESP_OK;
    // Is initialization done?
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    // Check if interface has been initialized
    MB_SLAVE_CHECK((slave_interface_ptr->destroy != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    // Call the slave port destroy function
    error = slave_interface_ptr->destroy();
    MB_SLAVE_CHECK((error == ESP_OK),
                    ESP_ERR_INVALID_STATE,
                    "Slave destroy failure error=(0x%x).",
                    error);
    // Destroy all opened descriptors
    mbc_slave_free_descriptors();
    free(slave_interface_ptr);
    slave_interface_ptr = NULL;
    return error;
}

/**
 * Setup Modbus controller parameters
 */
esp_err_t mbc_slave_setup(void* comm_info)
{
    esp_err_t error = ESP_OK;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->setup != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    error = slave_interface_ptr->setup(comm_info);
    MB_SLAVE_CHECK((error == ESP_OK),
                    ESP_ERR_INVALID_STATE,
                    "Slave setup failure error=(0x%x).",
                    error);
    return error;
}

/**
 * Start Modbus controller start function
 */
esp_err_t mbc_slave_start(void)
{
    esp_err_t error = ESP_OK;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->start != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
#ifdef CONFIG_FMB_CONTROLLER_SLAVE_ID_SUPPORT
    // Set the slave ID if the KConfig option is selected
    eMBErrorCode status = eMBSetSlaveID(MB_SLAVE_ID_SHORT, TRUE, (UCHAR*)mb_slave_id, sizeof(mb_slave_id));
    MB_SLAVE_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE, "mb stack set slave ID failure.");
#endif
    error = slave_interface_ptr->start();
    MB_SLAVE_CHECK((error == ESP_OK),
                    ESP_ERR_INVALID_STATE,
                    "Slave start failure error=(0x%x).",
                    error);
    return error;
}

/**
 * Blocking function to get event on parameter group change for application task
 */
mb_event_group_t mbc_slave_check_event(mb_event_group_t group)
{
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    MB_EVENT_NO_EVENTS,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->check_event != NULL),
                    MB_EVENT_NO_EVENTS,
                    "Slave interface is not correctly initialized.");
    mb_event_group_t event = slave_interface_ptr->check_event(group);
    return event;
}

/**
 * Function to get notification about parameter change from application task
 */
esp_err_t mbc_slave_get_param_info(mb_param_info_t* reg_info, uint32_t timeout)
{
    esp_err_t error = ESP_OK;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->get_param_info != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    error = slave_interface_ptr->get_param_info(reg_info, timeout);
    MB_SLAVE_CHECK((error == ESP_OK),
                    ESP_ERR_INVALID_STATE,
                    "Slave get parameter info failure error=(0x%x).",
                    error);
    return error;
}

/**
 * Function to set area descriptors for modbus parameters
 */
esp_err_t mbc_slave_set_descriptor(mb_register_area_descriptor_t descr_data)
{
    esp_err_t error = ESP_OK;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");

    if (slave_interface_ptr->set_descriptor != NULL) {
        error = slave_interface_ptr->set_descriptor(descr_data);
        MB_SLAVE_CHECK((error == ESP_OK),
                        ESP_ERR_INVALID_STATE,
                        "Slave set descriptor failure error=(0x%x).",
                        (uint16_t)error);
    } else {
        mb_slave_options_t* mbs_opts = &slave_interface_ptr->opts;
        // Check if the address is already in the descriptor list
        mb_descr_entry_t* it = mbc_slave_find_reg_descriptor(descr_data.type, descr_data.start_offset, 1);
        MB_SLAVE_CHECK((it == NULL), ESP_ERR_INVALID_ARG, "mb incorrect descriptor or already defined.");

        mb_descr_entry_t* new_descr = (mb_descr_entry_t*) heap_caps_malloc(sizeof(mb_descr_entry_t),
                                            MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
        MB_SLAVE_CHECK((new_descr != NULL), ESP_ERR_NO_MEM, "mb can not allocate memory for descriptor.");
        new_descr->start_offset = descr_data.start_offset;
        new_descr->type = descr_data.type;
        new_descr->p_data = descr_data.address;
        new_descr->size = descr_data.size;
        LIST_INSERT_HEAD(&mbs_opts->mbs_area_descriptors[descr_data.type], new_descr, entries);
        error = ESP_OK;
    }
    return error;
}

// The helper function to get time stamp in microseconds
static uint64_t mbc_slave_get_time_stamp(void)
{
    uint64_t time_stamp = esp_timer_get_time();
    return time_stamp;
}

// Helper function to send parameter information to application task
static esp_err_t mbc_slave_send_param_info(mb_event_group_t par_type, uint16_t mb_offset,
                                    uint8_t* par_address, uint16_t par_size)
{
    MB_SLAVE_ASSERT(slave_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &slave_interface_ptr->opts;
    esp_err_t error = ESP_FAIL;
    mb_param_info_t par_info;
    // Check if queue is not full the send parameter information
    par_info.type = par_type;
    par_info.size = par_size;
    par_info.address = par_address;
    par_info.time_stamp = mbc_slave_get_time_stamp();
    par_info.mb_offset = mb_offset;
    BaseType_t status = xQueueSend(mbs_opts->mbs_notification_queue_handle, &par_info, MB_PAR_INFO_TOUT);
    if (pdTRUE == status) {
        ESP_LOGD(TAG, "Queue send parameter info (type, address, size): %d, 0x%.4x, %d",
                par_type, (uint32_t)par_address, par_size);
        error = ESP_OK;
    } else if (errQUEUE_FULL == status) {
        ESP_LOGD(TAG, "Parameter queue is overflowed.");
    }
    return error;
}

// Helper function to send notification
static esp_err_t mbc_slave_send_param_access_notification(mb_event_group_t event)
{
    MB_SLAVE_ASSERT(slave_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &slave_interface_ptr->opts;
    esp_err_t err = ESP_FAIL;
    mb_event_group_t bits = (mb_event_group_t)xEventGroupSetBits(mbs_opts->mbs_event_group, (EventBits_t)event);
    if (bits & event) {
        ESP_LOGD(TAG, "The MB_REG_CHANGE_EVENT = 0x%.2x is set.", (uint8_t)event);
        err = ESP_OK;
    }
    return err;
}

/*
 * Below are the common slave read/write register callback functions
 * The concrete slave port can override them using interface function pointers
 */

// Callback function for reading of MB Input Registers
eMBErrorCode mbc_reg_input_slave_cb(UCHAR * reg_buffer, USHORT address, USHORT n_regs)
{
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    MB_EILLSTATE, "Slave stack uninitialized.");
    MB_SLAVE_CHECK((reg_buffer != NULL),
                    MB_EINVAL, "Slave stack call failed.");
    eMBErrorCode status = MB_ENOERR;
    address--; // address of register is already +1
    mb_descr_entry_t* it = mbc_slave_find_reg_descriptor(MB_PARAM_INPUT, address, n_regs);
    if (it != NULL) {
        uint16_t input_reg_start = (uint16_t)it->start_offset; // Get Modbus start address
        uint8_t* input_buffer = (uint8_t*)it->p_data; // Get instance address
        uint16_t regs = n_regs;
        uint16_t reg_index;
        // If input or configuration parameters are incorrect then return an error to stack layer
        reg_index = (uint16_t)(address - input_reg_start);
        reg_index <<= 1; // register Address to byte address
        input_buffer += reg_index;
        uint8_t* buffer_start = input_buffer;
        while (regs > 0) {
            _XFER_2_RD(reg_buffer, input_buffer);
            reg_index += 2;
            regs -= 1;
        }
        // Send access notification
        (void)mbc_slave_send_param_access_notification(MB_EVENT_INPUT_REG_RD);
        // Send parameter info to application task
        (void)mbc_slave_send_param_info(MB_EVENT_INPUT_REG_RD, (uint16_t)address,
                        (uint8_t*)buffer_start, (uint16_t)n_regs);
    } else {
        status = MB_ENOREG;
    }
    return status;
}

// Callback function for reading of MB Holding Registers
// Executed by stack when request to read/write holding registers is received
eMBErrorCode mbc_reg_holding_slave_cb(UCHAR * reg_buffer, USHORT address, USHORT n_regs, eMBRegisterMode mode)
{
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    MB_EILLSTATE, "Slave stack uninitialized.");
    MB_SLAVE_CHECK((reg_buffer != NULL),
                    MB_EINVAL, "Slave stack call failed.");
    eMBErrorCode status = MB_ENOERR;
    uint16_t reg_index;
    address--; // address of register is already +1
    mb_descr_entry_t* it = mbc_slave_find_reg_descriptor(MB_PARAM_HOLDING, address, n_regs);
    if (it != NULL) {
        uint16_t reg_holding_start = (uint16_t)it->start_offset; // Get Modbus start address
        uint8_t* holding_buffer = (uint8_t*)it->p_data; // Get instance address
        uint16_t regs = n_regs;
        reg_index = (uint16_t) (address - reg_holding_start);
        reg_index <<= 1; // register Address to byte address
        holding_buffer += reg_index;
        uint8_t* buffer_start = holding_buffer;
        switch (mode) {
            case MB_REG_READ:
                while (regs > 0) {
                    _XFER_2_RD(reg_buffer, holding_buffer);
                    reg_index += 2;
                    regs -= 1;
                };
                // Send access notification
                (void)mbc_slave_send_param_access_notification(MB_EVENT_HOLDING_REG_RD);
                // Send parameter info
                (void)mbc_slave_send_param_info(MB_EVENT_HOLDING_REG_RD, (uint16_t)address,
                                (uint8_t*)buffer_start, (uint16_t)n_regs);
                break;
            case MB_REG_WRITE:
                while (regs > 0) {
                    _XFER_2_WR(holding_buffer, reg_buffer);
                    holding_buffer += 2;
                    reg_index += 2;
                    regs -= 1;
                };
                // Send access notification
                (void)mbc_slave_send_param_access_notification(MB_EVENT_HOLDING_REG_WR);
                // Send parameter info
                (void)mbc_slave_send_param_info(MB_EVENT_HOLDING_REG_WR, (uint16_t)address,
                                (uint8_t*)buffer_start, (uint16_t)n_regs);
                break;
        }
    } else {
        status = MB_ENOREG;
    }
    return status;
}

// Callback function for reading of MB Coils Registers
eMBErrorCode mbc_reg_coils_slave_cb(UCHAR* reg_buffer, USHORT address, USHORT n_coils, eMBRegisterMode mode)
{
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    MB_EILLSTATE, "Slave stack uninitialized.");
    MB_SLAVE_CHECK((reg_buffer != NULL),
                    MB_EINVAL, "Slave stack call failed.");
    eMBErrorCode status = MB_ENOERR;
    uint16_t reg_index;
    uint16_t coils = n_coils;
    address--; // The address is already +1
    mb_descr_entry_t* it = mbc_slave_find_reg_descriptor(MB_PARAM_COIL, address, n_coils);
    if (it != NULL) {
        uint16_t reg_coils_start = (uint16_t)it->start_offset; // MB offset of coils
        uint8_t* reg_coils_buf = (uint8_t*)it->p_data;
        reg_index = (uint16_t) (address - it->start_offset);
        CHAR* coils_data_buf = (CHAR*)(reg_coils_buf + (reg_index >> 3));
        switch (mode) {
            case MB_REG_READ:
                while (coils > 0) {
                    uint8_t result = xMBUtilGetBits((uint8_t*)reg_coils_buf, reg_index, 1);
                    xMBUtilSetBits(reg_buffer, reg_index - (address - reg_coils_start), 1, result);
                    reg_index++;
                    coils--;
                }
                // Send an event to notify application task about event
                (void)mbc_slave_send_param_access_notification(MB_EVENT_COILS_RD);
                (void)mbc_slave_send_param_info(MB_EVENT_COILS_RD, (uint16_t)address,
                                (uint8_t*)(coils_data_buf), (uint16_t)n_coils);
                break;
            case MB_REG_WRITE:
                while (coils > 0) {
                    uint8_t result = xMBUtilGetBits(reg_buffer,
                            reg_index - (address - reg_coils_start), 1);
                    xMBUtilSetBits((uint8_t*)reg_coils_buf, reg_index, 1, result);
                    reg_index++;
                    coils--;
                }
                // Send an event to notify application task about event
                (void)mbc_slave_send_param_access_notification(MB_EVENT_COILS_WR);
                (void)mbc_slave_send_param_info(MB_EVENT_COILS_WR, (uint16_t)address,
                                (uint8_t*)coils_data_buf, (uint16_t)n_coils);
                break;
        } // switch ( eMode )
    } else {
        // If the configuration or input parameters are incorrect then return error to stack
        status = MB_ENOREG;
    }
    return status;
}

// Callback function for reading of MB Discrete Input Registers
eMBErrorCode mbc_reg_discrete_slave_cb(UCHAR* reg_buffer, USHORT address, USHORT n_discrete)
{
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    MB_EILLSTATE, "Slave stack uninitialized.");
    MB_SLAVE_CHECK((reg_buffer != NULL),
                    MB_EINVAL, "Slave stack call failed.");

    eMBErrorCode status = MB_ENOERR;
    uint16_t reg_index;
    uint16_t reg_bit_index;
    uint16_t n_reg;
    uint8_t* discrete_input_buf;
    // It already plus one in modbus function method.
    address--;
    mb_descr_entry_t* it = mbc_slave_find_reg_descriptor(MB_PARAM_DISCRETE, address, n_discrete);
    if (it != NULL) {
        uint16_t reg_discrete_start = (uint16_t)it->start_offset; // MB offset of registers
        n_reg = (n_discrete >> 3) + 1;
        discrete_input_buf = (uint8_t*)it->p_data; // the storage address
        reg_index = (uint16_t) (address - reg_discrete_start) / 8; // Get register index in the buffer for bit number
        reg_bit_index = (uint16_t)(address - reg_discrete_start) % 8; // Get bit index
        uint8_t* temp_buf = &discrete_input_buf[reg_index];
        while (n_reg > 0) {
            *reg_buffer++ = xMBUtilGetBits(&discrete_input_buf[reg_index++], reg_bit_index, 8);
            n_reg--;
        }
        reg_buffer--;
        // Last discrete
        n_discrete = n_discrete % 8;
        // Filling zero to high bit
        *reg_buffer = *reg_buffer << (8 - n_discrete);
        *reg_buffer = *reg_buffer >> (8 - n_discrete);
        // Send an event to notify application task about event
        (void)mbc_slave_send_param_access_notification(MB_EVENT_DISCRETE_RD);
        (void)mbc_slave_send_param_info(MB_EVENT_DISCRETE_RD, (uint16_t)address,
                            (uint8_t*)temp_buf, (uint16_t)n_discrete);
    } else {
        status = MB_ENOREG;
    }
    return status;
}

/**
 * Below are the stack callback functions to read/write registers
 */
eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    eMBErrorCode error = MB_ENOERR;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    // Check if the callback is overridden in concrete port
    if (slave_interface_ptr->slave_reg_cb_discrete) {
        error = slave_interface_ptr->slave_reg_cb_discrete(pucRegBuffer, usAddress, usNDiscrete);
    } else {
        error = mbc_reg_discrete_slave_cb(pucRegBuffer, usAddress, usNDiscrete);
    }

    return error;
}

eMBErrorCode eMBRegCoilsCB(UCHAR* pucRegBuffer, USHORT usAddress,
                            USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode error = MB_ENOERR;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");

    if (slave_interface_ptr->slave_reg_cb_coils) {
        error = slave_interface_ptr->slave_reg_cb_coils(pucRegBuffer, usAddress, usNCoils, eMode);
    } else {
        error = mbc_reg_coils_slave_cb(pucRegBuffer, usAddress, usNCoils, eMode);
    }
    return error;
}

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode error = MB_ENOERR;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");

    if (slave_interface_ptr->slave_reg_cb_holding) {
        error = slave_interface_ptr->slave_reg_cb_holding(pucRegBuffer, usAddress, usNRegs, eMode);
    } else {
        error = mbc_reg_holding_slave_cb(pucRegBuffer, usAddress, usNRegs, eMode);
    }
    return error;
}

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode error = ESP_ERR_INVALID_STATE;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");

    if (slave_interface_ptr->slave_reg_cb_input) {
        error = slave_interface_ptr->slave_reg_cb_input(pucRegBuffer, usAddress, usNRegs);
    } else {
        error = mbc_reg_input_slave_cb(pucRegBuffer, usAddress, usNRegs);
    }
    return error;
}
