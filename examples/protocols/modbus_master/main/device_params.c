/*======================================================================================
 * Description:
 *   This C file contains user defined parameters for Modbus example and data dictionary
 *   which describes each value (characteristic) and links it to modbus registers in
 *   in corresponded slave device.
 *=====================================================================================*/

#include "mbcontroller.h"
#include "device_params.h"

// Here are the user instances defined as structures for device parameters packed by 1 byte
// These are keep the values that can be accessed from Modbus master

holding_reg_params_t holding_reg_params = { 0 };

input_reg_params_t input_reg_params = { 0 };

coil_reg_params_t coil_reg_params = { 0 };

discrete_reg_params_t discrete_reg_params = { 0 };

#define HOLD_OFFSET(field) ((uint16_t)(offsetof(holding_reg_params_t, field) + 1))
#define INPUT_OFFSET(field) ((uint16_t)(offsetof(input_reg_params_t, field) + 1))
#define COIL_OFFSET(field) ((uint16_t)(offsetof(coil_reg_params_t, field) + 1))
// Discrete offset macro (options can be used as bit masks)
#define DISCR_OFFSET(field) ((uint16_t)(offsetof(discrete_reg_params_t, field) + 1))

#define STR(fieldname) ((const char*)( fieldname ))
#define OPTS(min_val, max_val, step_val) { .opt1 = min_val, .opt2 = max_val, .opt3 = step_val }

// This table below defines the characteristics supported by this Modbus master device (Data dictionary).
// These characteristics are linked to Modbus parameters of external slave devices in Modbus network.
// For this example next devices are supported:
// MB_DEVICE_ADDR1 : (CID_HUMIDITY_1, CID_TEMPERATURE_1) : Modbus sensor 1 (Humidity and Temperature)
// MB_DEVICE_ADDR2 : (CID_HUMIDITY_2, CID_TEMPERATURE_2) : Modbus sensor 2 (Humidity and Temperature)
// MB_DEVICE_ADDR3 : (CID_RELAY_P1, CID_RELAY_P2) : Modbus output device (Relay outputs on/off)

// There are two options to define instance for parameter:
// 1. Define offset (param_offset field) to the field in the parameter's structure (see example below)
// Once set it will be used as to store parameter value.
// 2. Set param_offset field in characteristics table to zero.
// This will allow to allocate space for parameter storage during initialization and use it as a cache.

// Example Data (Object) Dictionary for Modbus parameters
const mb_parameter_descriptor_t device_parameters[] = {
    // { Cid, Param Name, Units, Modbus Slave Addr, Modbus Reg Type, Reg Start, Reg Size, Instance Offset, Data Type, Data Size, Parameter Options, Access Mode}
    // Parameter: Data channel 0 : Data channel 0 = Voltage
    { CID_DATA_CHAN_0, STR("Data_channel_0"), STR("Volts"), MB_DEVICE_ADDR1, MB_PARAM_INPUT, 0, 2,
                    INPUT_OFFSET(data_chan0), PARAM_TYPE_FLOAT, 4, OPTS( -10, 10, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    { CID_HUMIDITY_1, STR("Humidity_1"), STR("%rH"), MB_DEVICE_ADDR1, MB_PARAM_HOLDING, 0, 2,
            HOLD_OFFSET(mb_device1_humidity), PARAM_TYPE_FLOAT, 4, OPTS( 0, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    // Parameter: Temperature_2 : Temperature from device slave address = 1
    { CID_TEMPERATURE_1, STR("Temperature_1"), STR("°C"), MB_DEVICE_ADDR1, MB_PARAM_HOLDING, 2, 2,
            HOLD_OFFSET(mb_device1_temperature), PARAM_TYPE_FLOAT, 4, OPTS( -40, 80, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    // Parameter: Humidity_2 : Humidity from device slave address = 2
    { CID_HUMIDITY_2, STR("Humidity_2"), STR("%rH"), MB_DEVICE_ADDR2, MB_PARAM_HOLDING, 0, 2,
            HOLD_OFFSET(mb_device2_humidity), PARAM_TYPE_FLOAT, 4, OPTS( 0, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    // Parameter: Temperature_2 : Temperature from device slave address = 2
    { CID_TEMPERATURE_2, STR("Temperature_2"), STR("°C"), MB_DEVICE_ADDR2, MB_PARAM_HOLDING, 2, 2,
            HOLD_OFFSET(mb_device2_temperature), PARAM_TYPE_FLOAT, 4, OPTS( -40, 80, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    // Parameter: Relay P1 : Alarm on/off channel 1 : Output device 1
    { CID_RELAY_P1, STR("RelayP1"), STR("on/off"), MB_DEVICE_ADDR3, MB_PARAM_COIL, 0, 3,
            COIL_OFFSET(coils_port1), PARAM_TYPE_U16, 2, OPTS( BIT0 | BIT1 | BIT2, 0, 0 ), PAR_PERMS_READ_WRITE_TRIGGER },
    // Parameter: Relay P2 : Alarm on/off channel 2 : Output device 1
    { CID_RELAY_P2, STR("RelayP2"), STR("on/off"), MB_DEVICE_ADDR3, MB_PARAM_COIL, 3, 8,
            COIL_OFFSET(coils_port2), PARAM_TYPE_U16, 2, OPTS( BIT3, 0, 0 ), PAR_PERMS_READ_WRITE_TRIGGER },
};

// Calculate number of parameters in the table
const uint16_t num_device_parameters = (sizeof(device_parameters)/sizeof(device_parameters[0]));
