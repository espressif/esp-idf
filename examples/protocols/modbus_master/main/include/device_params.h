/*=====================================================================================
 * Description:
 *   C file to define user defined parameters for Modbus example
 *====================================================================================*/

#ifndef _DEVICEPARAMS_H_
#define _DEVICEPARAMS_H_

#include "mbcontroller.h"   // for common Modbus defines

// Enumeration of modbus device addresses accessed by master device
enum {
    MB_DEVICE_ADDR1 = 1,
    MB_DEVICE_ADDR2 = 2,
    MB_DEVICE_ADDR3 = 3,
};

// Enumeration of all supported CIDs for device (used in parameter definition table)
enum {
    CID_DATA_CHAN_0 = 0,
    CID_HUMIDITY_1,
    CID_TEMPERATURE_1,
    CID_HUMIDITY_2,
    CID_TEMPERATURE_2,
    CID_RELAY_P1,
    CID_RELAY_P2,
    CID_COUNT,
};

#define DEVICE_PARAM_MAX_SIZE 24

// The structures below define the parameters that will be accessed by Modbus master device.
// These parameters reflect the parameters in the address space of external devices in Modbus network.
// They defined for each modbus register type (coils, discreet inputs, holding registers, input registers).
// These are not required but can be used by user to link characteristic to corresponded field
// See the parameter definition table for more information.
// It is just example and it is responsibility of user to define them as required.
#pragma pack(push, 1)
typedef struct
{
    // Parameter: discrete_input0
    uint8_t discrete_input0:1;
    // Parameter: discrete_input1
    uint8_t discrete_input1:1;
    // Parameter: discrete_input2
    uint8_t discrete_input2:1;
    // Parameter: discrete_input3
    uint8_t discrete_input3:1;
    // Parameter: discrete_input4
    uint8_t discrete_input4:1;
    // Parameter: discrete_input5
    uint8_t discrete_input5:1;
    // Parameter: discrete_input6
    uint8_t discrete_input6:1;
    // Parameter: discrete_input7
    uint8_t discrete_input7:1;
    uint8_t discrete_input_port1:8;
} discrete_reg_params_t;
#pragma pack(pop)

// Note: For correct access the coils storage for each addressed parameter
// has to include at least 2 byte (register)!
#pragma pack(push, 1)
typedef union
{
    struct {
        // Parameter: Coil 0 : Coil0

        uint8_t coil0:1;
        // Parameter: Coil 1 : Coil1
        uint8_t coil1:1;
        // Parameter: Coil 2 : Coil2
        uint8_t coil2:1;
        // Parameter: Coil 3 : Coil3
        uint8_t coil3:1;
        // Parameter: Coil 4 : Coil4
        uint8_t coil4:1;
        // Parameter: Coil 5 : Coil5
        uint8_t coil5:1;
        // Parameter: Coil 6 : Coil6
        uint8_t coil6:1;
        // Parameter: Coil 7 : Coil7
        uint8_t coil7:1;
        uint8_t coil_port2:8;
    };
    uint8_t coils_port1;
    uint8_t coils_port2;
} coil_reg_params_t;
#pragma pack(pop)

// Input register structure to keep characteristic's values
#pragma pack(push, 1)
typedef struct
{
    // Parameter: Data channel 0 : data_chan0
    float data_chan0;
    // Parameter: Data channel 1 : data_chan1
    float data_chan1;
    // Parameter: Data channel 2 : data_chan2
    float data_chan2;
    // Parameter: Data channel 3 : data_chan3
    float data_chan3;
} input_reg_params_t;
#pragma pack(pop)

// Holding register structure to keep characteristic's values
#pragma pack(push, 1)
typedef struct
{
    // Parameter: Data channel 0 : mb_device1_humidity
    float mb_device1_humidity;
    // Parameter: Data channel 1 : mb_device1_temperature
    float mb_device1_temperature;
    // Parameter: Data channel 2 : mb_device2_humidity
    float mb_device2_humidity;
    // Parameter: Data channel 3 : mb_device2_temperature
    float mb_device2_temperature;
    // Parameter: Protocol version  : protocol_version
    uint16_t mb_device1_protocol_version;
    // Parameter: Hardware version  : hardware_version
    uint16_t mb_device1_hardware_version;
    // Parameter: Software Version : software_version
    uint16_t mb_device1_software_version;
    // Parameter: Software Revision : software_revision
    uint16_t mb_device1_software_revision;
    // Parameter: Device Type : deviceType :
    uint16_t mb_device1_device_type;
    uint8_t mb_device1_string_test[PARAM_SIZE_ASCII24];
} holding_reg_params_t;
#pragma pack(pop)

extern holding_reg_params_t holding_reg_params;
extern input_reg_params_t input_reg_params;
extern coil_reg_params_t coil_reg_params;
extern discrete_reg_params_t discrete_reg_params;

extern const mb_parameter_descriptor_t device_parameters[];
extern const uint16_t num_device_parameters;

#endif /* _DEVICEPARAMS_H_ */
