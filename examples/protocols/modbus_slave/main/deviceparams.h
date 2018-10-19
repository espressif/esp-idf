/*=====================================================================================
 * Description:
 *   The Modbus parameter structures used to define Modbus instances that
 *   can be addressed by Modbus protocol. Define these structures per your needs in
 *   your application. Below is just an example of possible parameters.
 *====================================================================================*/
#ifndef _DEVICE_PARAMS
#define _DEVICE_PARAMS

#define A24_ARR_SIZE 24

// This file defines structure of modbus parameters which reflect correspond modbus address space
// for each modbus register type (coils, discreet inputs, holding registers, input registers)
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

#pragma pack(push, 1)
typedef struct
{
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
    // Coils port 1
    uint8_t coil_port1:8;
} coil_reg_params_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    // Parameter: Data channel 0 : data_chan0 : NV Address: 0
    float data_chan0;
    // Parameter: Data channel 1 : data_chan1 : NV Address: 0
    float data_chan1;
    // Parameter: Data channel 2 : data_chan2 : NV Address: 0
    float data_chan2;
    // Parameter: Data channel 3 : data_chan3 : NV Address: 0
    float data_chan3;
} input_reg_params_t;
#pragma pack(pop)

//See register map for more information.
#pragma pack(push, 1)
typedef struct
{
    // Parameter: Data channel 0 : DataChan0
    float data_chan0;
    // Parameter: Data channel 1 : DataChan1
    float data_chan1;
    // Parameter: Data channel 2 : DataChan2
    float data_chan2;
    // Parameter: Data channel 3 : DataChan3
    float data_chan3;
    // Parameter: Protocol version  : protocol_version
    uint16_t protocol_version;
    // Parameter: Hardware version  : hardware_version
    uint16_t hardware_version;
    // Parameter: Software Version : software_version
    uint16_t software_version;
    // Parameter: Software Revision : software_revision
    uint16_t software_revision;
    // Parameter: Device Type : deviceType :
    uint16_t deviceType;
    // Parameter: Modbus Network Address : modbus_address
    uint16_t modbus_address;
    // Parameter: Modbus Baudrate : modbus_baud
    uint16_t modbus_baud;
    // Parameter: Modbus parity  : modbus_parity
    uint16_t modbus_parity;
    // Parameter: Modbus stopbit  : modbus_stop_bits
    uint16_t modbus_stop_bits;
    // Parameter: Brace control  : modbus_brace_ctrl
    uint16_t modbus_brace_ctrl;
    // Parameter: Serial number  : serial_number
    uint32_t serial_number;
    // Parameter: Up time  : up_time
    uint32_t up_time;
    // Parameter: Device state  : device_state
    uint16_t device_state;
    // Parameter: Test Float0  : test_float0
    float test_float0;
    // Parameter: Test Float1  : test_float1
    float test_float1;
    // Parameter: Test Float2  : test_float2
    float test_float2;
    // Parameter: Test Float3  : test_float3
    float test_float3;
    // Parameter: Test String  : string_test
    uint8_t string_test[A24_ARR_SIZE];
} holding_reg_params_t;
#pragma pack(pop)

extern holding_reg_params_t holding_reg_params;
extern input_reg_params_t input_reg_params;
extern coil_reg_params_t coil_reg_params;
extern discrete_reg_params_t discrete_reg_params;

#endif // !defined(_DEVICE_PARAMS)
