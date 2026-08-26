/** @file
 *  @brief Bluetooth Assigned Numbers, codes and identifiers.
 */

/*
 * SPDX-FileCopyrightText: 2015-2025 Intel Corporation
 * SPDX-FileCopyrightText: 2017-2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_ASSIGNED_NUMBERS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_ASSIGNED_NUMBERS_H_

/**
 * @brief Bluetooth Assigned Numbers, codes and identifiers.
 * @defgroup bt_assigned_numbers Assigned Numbers.
 * @since 1.0
 * @version 1.0.0
 * @ingroup bluetooth
 * @{
 */

#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Core Specification Assigned Numbers
 * @defgroup bt_assigned_numbers_core Core Specification Assigned Numbers
 * @ingroup bt_assigned_numbers
 * @{
 */

/**
 * @name Appearance Numbers
 *
 * Last Modified on 2023-01-05
 * @{
 */

/** Generic Unknown */
#define BT_APPEARANCE_UNKNOWN                               0x0000
/** Generic Phone */
#define BT_APPEARANCE_GENERIC_PHONE                         0x0040
/** Generic Computer */
#define BT_APPEARANCE_GENERIC_COMPUTER                      0x0080
/** Desktop Workstation */
#define BT_APPEARANCE_COMPUTER_DESKTOP_WORKSTATION          0x0081
/** Server-class Computer */
#define BT_APPEARANCE_COMPUTER_SERVER_CLASS                 0x0082
/** Laptop */
#define BT_APPEARANCE_COMPUTER_LAPTOP                       0x0083
/** Handheld PC/PDA (clamshell) */
#define BT_APPEARANCE_COMPUTER_HANDHELD_PCPDA               0x0084
/** Palm­size PC/PDA */
#define BT_APPEARANCE_COMPUTER_PALMSIZE_PCPDA               0x0085
/** Wearable computer (watch size) */
#define BT_APPEARANCE_COMPUTER_WEARABLE_COMPUTER            0x0086
/** Tablet */
#define BT_APPEARANCE_COMPUTER_TABLET                       0x0087
/** Docking Station */
#define BT_APPEARANCE_COMPUTER_DOCKING_STATION              0x0088
/** All in One */
#define BT_APPEARANCE_COMPUTER_ALL_IN_ONE                   0x0089
/** Blade Server */
#define BT_APPEARANCE_COMPUTER_BLADE_SERVER                 0x008A
/** Convertible */
#define BT_APPEARANCE_COMPUTER_CONVERTIBLE                  0x008B
/** Detachable */
#define BT_APPEARANCE_COMPUTER_DETACHABLE                   0x008C
/** IoT Gateway */
#define BT_APPEARANCE_COMPUTER_IOT_GATEWAY                  0x008D
/** Mini PC */
#define BT_APPEARANCE_COMPUTER_MINI_PC                      0x008E
/** Stick PC */
#define BT_APPEARANCE_COMPUTER_STICK_PC                     0x008F
/** Generic Watch */
#define BT_APPEARANCE_GENERIC_WATCH                         0x00C0
/** Sports Watch */
#define BT_APPEARANCE_SPORTS_WATCH                          0x00C1
/** Smartwatch */
#define BT_APPEARANCE_SMARTWATCH                            0x00C2
/** Generic Clock */
#define BT_APPEARANCE_GENERIC_CLOCK                         0x0100
/** Generic Display */
#define BT_APPEARANCE_GENERIC_DISPLAY                       0x0140
/** Generic Remote Control */
#define BT_APPEARANCE_GENERIC_REMOTE                        0x0180
/** Generic Eye-glasses */
#define BT_APPEARANCE_GENERIC_EYEGLASSES                    0x01C0
/** Generic Tag */
#define BT_APPEARANCE_GENERIC_TAG                           0x0200
/** Generic Keyring */
#define BT_APPEARANCE_GENERIC_KEYRING                       0x0240
/** Generic Media Player */
#define BT_APPEARANCE_GENERIC_MEDIA_PLAYER                  0x0280
/** Generic Barcode Scanner */
#define BT_APPEARANCE_GENERIC_BARCODE_SCANNER               0x02C0
/** Generic Thermometer */
#define BT_APPEARANCE_GENERIC_THERMOMETER                   0x0300
/** Ear Thermometer */
#define BT_APPEARANCE_THERMOMETER_EAR                       0x0301
/** Generic Heart Rate Sensor */
#define BT_APPEARANCE_GENERIC_HEART_RATE                    0x0340
/** Heart Rate Belt */
#define BT_APPEARANCE_HEART_RATE_BELT                       0x0341
/** Generic Blood Pressure */
#define BT_APPEARANCE_GENERIC_BLOOD_PRESSURE                0x0380
/** Arm Blood Pressure */
#define BT_APPEARANCE_BLOOD_PRESSURE_ARM                    0x0381
/** Wrist Blood Pressure */
#define BT_APPEARANCE_BLOOD_PRESSURE_WRIST                  0x0382
/** Generic Human Interface Device */
#define BT_APPEARANCE_GENERIC_HID                           0x03C0
/** Keyboard */
#define BT_APPEARANCE_HID_KEYBOARD                          0x03C1
/** Mouse */
#define BT_APPEARANCE_HID_MOUSE                             0x03C2
/** Joystick */
#define BT_APPEARANCE_HID_JOYSTICK                          0x03C3
/** Gamepad */
#define BT_APPEARANCE_HID_GAMEPAD                           0x03C4
/** Digitizer Tablet */
#define BT_APPEARANCE_HID_DIGITIZER_TABLET                  0x03C5
/** Card Reader */
#define BT_APPEARANCE_HID_CARD_READER                       0x03C6
/** Digital Pen */
#define BT_APPEARANCE_HID_DIGITAL_PEN                       0x03C7
/** Barcode Scanner */
#define BT_APPEARANCE_HID_BARCODE_SCANNER                   0x03C8
/** Touchpad */
#define BT_APPEARANCE_HID_TOUCHPAD                          0x03C9
/** Presentation Remote */
#define BT_APPEARANCE_HID_PRESENTATION_REMOTE               0x03CA
/** Generic Glucose Meter */
#define BT_APPEARANCE_GENERIC_GLUCOSE                       0x0400
/** Generic Running Walking Sensor */
#define BT_APPEARANCE_GENERIC_WALKING                       0x0440
/** In-Shoe Running Walking Sensor */
#define BT_APPEARANCE_WALKING_IN_SHOE                       0x0441
/** On-Shoe Running Walking Sensor */
#define BT_APPEARANCE_WALKING_ON_SHOE                       0x0442
/** On-Hip Running Walking Sensor */
#define BT_APPEARANCE_WALKING_ON_HIP                        0x0443
/** Generic Cycling */
#define BT_APPEARANCE_GENERIC_CYCLING                       0x0480
/** Cycling Computer */
#define BT_APPEARANCE_CYCLING_COMPUTER                      0x0481
/** Speed Sensor */
#define BT_APPEARANCE_CYCLING_SPEED                         0x0482
/** Cadence Sensor */
#define BT_APPEARANCE_CYCLING_CADENCE                       0x0483
/** Power Sensor */
#define BT_APPEARANCE_CYCLING_POWER                         0x0484
/** Speed and Cadence Sensor */
#define BT_APPEARANCE_CYCLING_SPEED_CADENCE                 0x0485
/** Generic Control Device */
#define BT_APPEARANCE_GENERIC_CONTROL_DEVICE                0x04C0
/** Switch */
#define BT_APPEARANCE_CONTROL_SWITCH                        0x04C1
/** Multi-switch */
#define BT_APPEARANCE_CONTROL_MULTI_SWITCH                  0x04C2
/** Button */
#define BT_APPEARANCE_CONTROL_BUTTON                        0x04C3
/** Slider */
#define BT_APPEARANCE_CONTROL_SLIDER                        0x04C4
/** Rotary Switch */
#define BT_APPEARANCE_CONTROL_ROTARY_SWITCH                 0x04C5
/** Touch Panel */
#define BT_APPEARANCE_CONTROL_TOUCH_PANEL                   0x04C6
/** Single Switch */
#define BT_APPEARANCE_CONTROL_SINGLE_SWITCH                 0x04C7
/** Double Switch */
#define BT_APPEARANCE_CONTROL_DOUBLE_SWITCH                 0x04C8
/** Triple Switch */
#define BT_APPEARANCE_CONTROL_TRIPLE_SWITCH                 0x04C9
/** Battery Switch */
#define BT_APPEARANCE_CONTROL_BATTERY_SWITCH                0x04CA
/** Energy Harvesting Switch */
#define BT_APPEARANCE_CONTROL_ENERGY_HARVESTING_SWITCH      0x04CB
/** Push Button */
#define BT_APPEARANCE_CONTROL_PUSH_BUTTON                   0x04CC
/** Generic Network Device */
#define BT_APPEARANCE_GENERIC_NETWORK_DEVICE                0x0500
/** Access Point */
#define BT_APPEARANCE_NETWORK_ACCESS_POINT                  0x0501
/** Mesh Device */
#define BT_APPEARANCE_NETWORK_MESH_DEVICE                   0x0502
/** Mesh Network Proxy */
#define BT_APPEARANCE_NETWORK_MESH_PROXY                    0x0503
/** Generic Sensor */
#define BT_APPEARANCE_GENERIC_SENSOR                        0x0540
/** Motion Sensor */
#define BT_APPEARANCE_SENSOR_MOTION                         0x0541
/** Air quality Sensor */
#define BT_APPEARANCE_SENSOR_AIR_QUALITY                    0x0542
/** Temperature Sensor */
#define BT_APPEARANCE_SENSOR_TEMPERATURE                    0x0543
/** Humidity Sensor */
#define BT_APPEARANCE_SENSOR_HUMIDITY                       0x0544
/** Leak Sensor */
#define BT_APPEARANCE_SENSOR_LEAK                           0x0545
/** Smoke Sensor */
#define BT_APPEARANCE_SENSOR_SMOKE                          0x0546
/** Occupancy Sensor */
#define BT_APPEARANCE_SENSOR_OCCUPANCY                      0x0547
/** Contact Sensor */
#define BT_APPEARANCE_SENSOR_CONTACT                        0x0548
/** Carbon Monoxide Sensor */
#define BT_APPEARANCE_SENSOR_CARBON_MONOXIDE                0x0549
/** Carbon Dioxide Sensor */
#define BT_APPEARANCE_SENSOR_CARBON_DIOXIDE                 0x054A
/** Ambient Light Sensor */
#define BT_APPEARANCE_SENSOR_AMBIENT_LIGHT                  0x054B
/** Energy Sensor */
#define BT_APPEARANCE_SENSOR_ENERGY                         0x054C
/** Color Light Sensor */
#define BT_APPEARANCE_SENSOR_COLOR_LIGHT                    0x054D
/** Rain Sensor */
#define BT_APPEARANCE_SENSOR_RAIN                           0x054E
/** Fire Sensor */
#define BT_APPEARANCE_SENSOR_FIRE                           0x054F
/** Wind Sensor */
#define BT_APPEARANCE_SENSOR_WIND                           0x0550
/** Proximity Sensor */
#define BT_APPEARANCE_SENSOR_PROXIMITY                      0x0551
/** Multi-Sensor */
#define BT_APPEARANCE_SENSOR_MULTI                          0x0552
/** Flush Mounted Sensor */
#define BT_APPEARANCE_SENSOR_FLUSH_MOUNTED                  0x0553
/** Ceiling Mounted Sensor */
#define BT_APPEARANCE_SENSOR_CEILING_MOUNTED                0x0554
/** Wall Mounted Sensor */
#define BT_APPEARANCE_SENSOR_WALL_MOUNTED                   0x0555
/** Multisensor */
#define BT_APPEARANCE_MULTISENSOR                           0x0556
/** Energy Meter */
#define BT_APPEARANCE_SENSOR_ENERGY_METER                   0x0557
/** Flame Detector */
#define BT_APPEARANCE_SENSOR_FLAME_DETECTOR                 0x0558
/** Vehicle Tire Pressure Sensor */
#define BT_APPEARANCE_SENSOR_VEHICLE_TIRE_PRESSURE          0x0559
/** Generic Light Fixtures */
#define BT_APPEARANCE_GENERIC_LIGHT_FIXTURES                0x0580
/** Wall Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_WALL                   0x0581
/** Ceiling Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_CEILING                0x0582
/** Floor Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_FLOOR                  0x0583
/** Cabinet Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_CABINET                0x0584
/** Desk Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_DESK                   0x0585
/** Troffer Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_TROFFER                0x0586
/** Pendant Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_PENDANT                0x0587
/** In-ground Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_IN_GROUND              0x0588
/** Flood Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_FLOOD                  0x0589
/** Underwater Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_UNDERWATER             0x058A
/** Bollard with Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_BOLLARD_WITH           0x058B
/** Pathway Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_PATHWAY                0x058C
/** Garden Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_GARDEN                 0x058D
/** Pole-top Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_POLE_TOP               0x058E
/** Spotlight */
#define BT_APPEARANCE_SPOT_LIGHT                            0x058F
/** Linear Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_LINEAR                 0x0590
/** Street Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_STREET                 0x0591
/** Shelves Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_SHELVES                0x0592
/** Bay Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_BAY                    0x0593
/** Emergency Exit Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_EMERGENCY_EXIT         0x0594
/** Light Controller */
#define BT_APPEARANCE_LIGHT_FIXTURES_CONTROLLER             0x0595
/** Light Driver */
#define BT_APPEARANCE_LIGHT_FIXTURES_DRIVER                 0x0596
/** Bulb */
#define BT_APPEARANCE_LIGHT_FIXTURES_BULB                   0x0597
/** Low-bay Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_LOW_BAY                0x0598
/** High-bay Light */
#define BT_APPEARANCE_LIGHT_FIXTURES_HIGH_BAY               0x0599
/** Generic Fan */
#define BT_APPEARANCE_GENERIC_FAN                           0x05C0
/** Ceiling Fan */
#define BT_APPEARANCE_FAN_CEILING                           0x05C1
/** Axial Fan */
#define BT_APPEARANCE_FAN_AXIAL                             0x05C2
/** Exhaust Fan */
#define BT_APPEARANCE_FAN_EXHAUST                           0x05C3
/** Pedestal Fan */
#define BT_APPEARANCE_FAN_PEDESTAL                          0x05C4
/** Desk Fan */
#define BT_APPEARANCE_FAN_DESK                              0x05C5
/** Wall Fan */
#define BT_APPEARANCE_FAN_WALL                              0x05C6
/** Generic HVAC */
#define BT_APPEARANCE_GENERIC_HVAC                          0x0600
/** Thermostat */
#define BT_APPEARANCE_HVAC_THERMOSTAT                       0x0601
/** Humidifier */
#define BT_APPEARANCE_HVAC_HUMIDIFIER                       0x0602
/** De-humidifier */
#define BT_APPEARANCE_HVAC_DEHUMIDIFIER                     0x0603
/** Heater */
#define BT_APPEARANCE_HVAC_HEATER                           0x0604
/** Radiator */
#define BT_APPEARANCE_HVAC_RADIATOR                         0x0605
/** Boiler */
#define BT_APPEARANCE_HVAC_BOILER                           0x0606
/** Heat Pump */
#define BT_APPEARANCE_HVAC_HEAT_PUMP                        0x0607
/** Infrared Heater */
#define BT_APPEARANCE_HVAC_INFRARED_HEATER                  0x0608
/** Radiant Panel Heater */
#define BT_APPEARANCE_HVAC_RADIANT_PANEL_HEATER             0x0609
/** Fan Heater */
#define BT_APPEARANCE_HVAC_FAN_HEATER                       0x060A
/** Air Curtain */
#define BT_APPEARANCE_HVAC_AIR_CURTAIN                      0x060B
/** Generic Air Conditioning */
#define BT_APPEARANCE_GENERIC_AIR_CONDITIONING              0x0640
/** Generic Humidifier */
#define BT_APPEARANCE_GENERIC_HUMIDIFIER                    0x0680
/** Generic Heating */
#define BT_APPEARANCE_GENERIC_HEATING                       0x06C0
/** Radiator */
#define BT_APPEARANCE_HEATING_RADIATOR                      0x06C1
/** Boiler */
#define BT_APPEARANCE_HEATING_BOILER                        0x06C2
/** Heat Pump */
#define BT_APPEARANCE_HEATING_HEAT_PUMP                     0x06C3
/** Infrared Heater */
#define BT_APPEARANCE_HEATING_INFRARED_HEATER               0x06C4
/** Radiant Panel Heater */
#define BT_APPEARANCE_HEATING_RADIANT_PANEL_HEATER          0x06C5
/** Fan Heater */
#define BT_APPEARANCE_HEATING_FAN_HEATER                    0x06C6
/** Air Curtain */
#define BT_APPEARANCE_HEATING_AIR_CURTAIN                   0x06C7
/** Generic Access Control */
#define BT_APPEARANCE_GENERIC_ACCESS_CONTROL                0x0700
/** Access Door */
#define BT_APPEARANCE_CONTROL_ACCESS_DOOR                   0x0701
/** Garage Door */
#define BT_APPEARANCE_CONTROL_GARAGE_DOOR                   0x0702
/** Emergency Exit Door */
#define BT_APPEARANCE_CONTROL_EMERGENCY_EXIT_DOOR           0x0703
/** Access Lock */
#define BT_APPEARANCE_CONTROL_ACCESS_LOCK                   0x0704
/** Elevator */
#define BT_APPEARANCE_CONTROL_ELEVATOR                      0x0705
/** Window */
#define BT_APPEARANCE_CONTROL_WINDOW                        0x0706
/** Entrance Gate */
#define BT_APPEARANCE_CONTROL_ENTRANCE_GATE                 0x0707
/** Door Lock */
#define BT_APPEARANCE_CONTROL_DOOR_LOCK                     0x0708
/** Locker */
#define BT_APPEARANCE_CONTROL_LOCKER                        0x0709
/** Generic Motorized Device */
#define BT_APPEARANCE_GENERIC_MOTORIZED_DEVICE              0x0740
/** Motorized Gate */
#define BT_APPEARANCE_MOTORIZED_GATE                        0x0741
/** Awning */
#define BT_APPEARANCE_MOTORIZED_AWNING                      0x0742
/** Blinds or Shades */
#define BT_APPEARANCE_MOTORIZED_BLINDS_OR_SHADES            0x0743
/** Curtains */
#define BT_APPEARANCE_MOTORIZED_CURTAINS                    0x0744
/** Screen */
#define BT_APPEARANCE_MOTORIZED_SCREEN                      0x0745
/** Generic Power Device */
#define BT_APPEARANCE_GENERIC_POWER_DEVICE                  0x0780
/** Power Outlet */
#define BT_APPEARANCE_POWER_OUTLET                          0x0781
/** Power Strip */
#define BT_APPEARANCE_POWER_STRIP                           0x0782
/** Plug */
#define BT_APPEARANCE_POWER_PLUG                            0x0783
/** Power Supply */
#define BT_APPEARANCE_POWER_SUPPLY                          0x0784
/** LED Driver */
#define BT_APPEARANCE_POWER_LED_DRIVER                      0x0785
/** Fluorescent Lamp Gear */
#define BT_APPEARANCE_POWER_FLUORESCENT_LAMP_GEAR           0x0786
/** HID Lamp Gear */
#define BT_APPEARANCE_POWER_HID_LAMP_GEAR                   0x0787
/** Charge Case */
#define BT_APPEARANCE_POWER_CHARGE_CASE                     0x0788
/** Power Bank */
#define BT_APPEARANCE_POWER_POWER_BANK                      0x0789
/** Generic Light Source */
#define BT_APPEARANCE_GENERIC_LIGHT_SOURCE                  0x07C0
/** Incandescent Light Bulb */
#define BT_APPEARANCE_LIGHT_SOURCE_INCANDESCENT_BULB        0x07C1
/** LED Lamp */
#define BT_APPEARANCE_LIGHT_SOURCE_LED_LAMP                 0x07C2
/** HID Lamp */
#define BT_APPEARANCE_LIGHT_SOURCE_HID_LAMP                 0x07C3
/** Fluorescent Lamp */
#define BT_APPEARANCE_LIGHT_SOURCE_FLUORESCENT_LAMP         0x07C4
/** LED Array */
#define BT_APPEARANCE_LIGHT_SOURCE_LED_ARRAY                0x07C5
/** Multi-Color LED Array */
#define BT_APPEARANCE_LIGHT_SOURCE_MULTICOLOR_LED_ARRAY     0x07C6
/** Low voltage halogen */
#define BT_APPEARANCE_LIGHT_SOURCE_LOW_VOLTAGE_HALOGEN      0x07C7
/** Organic light emitting diode */
#define BT_APPEARANCE_LIGHT_SOURCE_OLED                     0x07C8
/** Generic Window Covering */
#define BT_APPEARANCE_GENERIC_WINDOW_COVERING               0x0800
/** Window Shades */
#define BT_APPEARANCE_WINDOW_SHADES                         0x0801
/** Window Blinds */
#define BT_APPEARANCE_WINDOW_BLINDS                         0x0802
/** Window Awning */
#define BT_APPEARANCE_WINDOW_AWNING                         0x0803
/** Window Curtain */
#define BT_APPEARANCE_WINDOW_CURTAIN                        0x0804
/** Exterior Shutter */
#define BT_APPEARANCE_WINDOW_EXTERIOR_SHUTTER               0x0805
/** Exterior Screen */
#define BT_APPEARANCE_WINDOW_EXTERIOR_SCREEN                0x0806
/** Generic Audio Sink */
#define BT_APPEARANCE_GENERIC_AUDIO_SINK                    0x0840
/** Standalone Speaker */
#define BT_APPEARANCE_AUDIO_SINK_STANDALONE_SPEAKER         0x0841
/** Soundbar */
#define BT_APPEARANCE_AUDIO_SINK_SOUNDBAR                   0x0842
/** Bookshelf Speaker */
#define BT_APPEARANCE_AUDIO_SINK_BOOKSHELF_SPEAKER          0x0843
/** Standmounted Speaker */
#define BT_APPEARANCE_AUDIO_SINK_STANDMOUNTED_SPEAKER       0x0844
/** Speakerphone */
#define BT_APPEARANCE_AUDIO_SINK_SPEAKERPHONE               0x0845
/** Generic Audio Source */
#define BT_APPEARANCE_GENERIC_AUDIO_SOURCE                  0x0880
/** Microphone */
#define BT_APPEARANCE_AUDIO_SOURCE_MICROPHONE               0x0881
/** Alarm */
#define BT_APPEARANCE_AUDIO_SOURCE_ALARM                    0x0882
/** Bell */
#define BT_APPEARANCE_AUDIO_SOURCE_BELL                     0x0883
/** Horn */
#define BT_APPEARANCE_AUDIO_SOURCE_HORN                     0x0884
/** Broadcasting Device */
#define BT_APPEARANCE_AUDIO_SOURCE_BROADCASTING_DEVICE      0x0885
/** Service Desk */
#define BT_APPEARANCE_AUDIO_SOURCE_SERVICE_DESK             0x0886
/** Kiosk */
#define BT_APPEARANCE_AUDIO_SOURCE_KIOSK                    0x0887
/** Broadcasting Room */
#define BT_APPEARANCE_AUDIO_SOURCE_BROADCASTING_ROOM        0x0888
/** Auditorium */
#define BT_APPEARANCE_AUDIO_SOURCE_AUDITORIUM               0x0889
/** Generic Motorized Vehicle */
#define BT_APPEARANCE_GENERIC_MOTORIZED_VEHICLE             0x08C0
/** Car */
#define BT_APPEARANCE_VEHICLE_CAR                           0x08C1
/** Large Goods Vehicle */
#define BT_APPEARANCE_VEHICLE_LARGE_GOODS                   0x08C2
/** 2-Wheeled Vehicle */
#define BT_APPEARANCE_VEHICLE_TWO_WHEELED                   0x08C3
/** Motorbike */
#define BT_APPEARANCE_VEHICLE_MOTORBIKE                     0x08C4
/** Scooter */
#define BT_APPEARANCE_VEHICLE_SCOOTER                       0x08C5
/** Moped */
#define BT_APPEARANCE_VEHICLE_MOPED                         0x08C6
/** 3-Wheeled Vehicle */
#define BT_APPEARANCE_VEHICLE_THREE_WHEELED                 0x08C7
/** Light Vehicle */
#define BT_APPEARANCE_VEHICLE_LIGHT                         0x08C8
/** Quad Bike */
#define BT_APPEARANCE_VEHICLE_QUAD_BIKE                     0x08C9
/** Minibus */
#define BT_APPEARANCE_VEHICLE_MINIBUS                       0x08CA
/** Bus */
#define BT_APPEARANCE_VEHICLE_BUS                           0x08CB
/** Trolley */
#define BT_APPEARANCE_VEHICLE_TROLLEY                       0x08CC
/** Agricultural Vehicle */
#define BT_APPEARANCE_VEHICLE_AGRICULTURAL                  0x08CD
/** Camper/Caravan */
#define BT_APPEARANCE_VEHICLE_CAMPER_OR_CARAVAN             0x08CE
/** Recreational Vehicle/Motor Home */
#define BT_APPEARANCE_VEHICLE_RECREATIONAL                  0x08CF
/** Generic Domestic Appliance */
#define BT_APPEARANCE_GENERIC_DOMESTIC_APPLIANCE            0x0900
/** Refrigerator */
#define BT_APPEARANCE_APPLIANCE_REFRIGERATOR                0x0901
/** Freezer */
#define BT_APPEARANCE_APPLIANCE_FREEZER                     0x0902
/** Oven */
#define BT_APPEARANCE_APPLIANCE_OVEN                        0x0903
/** Microwave */
#define BT_APPEARANCE_APPLIANCE_MICROWAVE                   0x0904
/** Toaster */
#define BT_APPEARANCE_APPLIANCE_TOASTER                     0x0905
/** Washing Machine */
#define BT_APPEARANCE_APPLIANCE_WASHING_MACHINE             0x0906
/** Dryer */
#define BT_APPEARANCE_APPLIANCE_DRYER                       0x0907
/** Coffee maker */
#define BT_APPEARANCE_APPLIANCE_COFFEE_MAKER                0x0908
/** Clothes iron */
#define BT_APPEARANCE_APPLIANCE_CLOTHES_IRON                0x0909
/** Curling iron */
#define BT_APPEARANCE_APPLIANCE_CURLING_IRON                0x090A
/** Hair dryer */
#define BT_APPEARANCE_APPLIANCE_HAIR_DRYER                  0x090B
/** Vacuum cleaner */
#define BT_APPEARANCE_APPLIANCE_VACUUM_CLEANER              0x090C
/** Robotic vacuum cleaner */
#define BT_APPEARANCE_APPLIANCE_ROBOTIC_VACUUM_CLEANER      0x090D
/** Rice cooker */
#define BT_APPEARANCE_APPLIANCE_RICE_COOKER                 0x090E
/** Clothes steamer */
#define BT_APPEARANCE_APPLIANCE_CLOTHES_STEAMER             0x090F
/** Generic Wearable Audio Device */
#define BT_APPEARANCE_GENERIC_WEARABLE_AUDIO_DEVICE         0x0940
/** Earbud */
#define BT_APPEARANCE_WEARABLE_AUDIO_DEVICE_EARBUD          0x0941
/** Headset */
#define BT_APPEARANCE_WEARABLE_AUDIO_DEVICE_HEADSET         0x0942
/** Headphones */
#define BT_APPEARANCE_WEARABLE_AUDIO_DEVICE_HEADPHONES      0x0943
/** Neck Band */
#define BT_APPEARANCE_WEARABLE_AUDIO_DEVICE_NECK_BAND       0x0944
/** Generic Aircraft */
#define BT_APPEARANCE_GENERIC_AIRCRAFT                      0x0980
/** Light Aircraft */
#define BT_APPEARANCE_AIRCRAFT_LIGHT                        0x0981
/** Microlight */
#define BT_APPEARANCE_AIRCRAFT_MICROLIGHT                   0x0982
/** Paraglider */
#define BT_APPEARANCE_AIRCRAFT_PARAGLIDER                   0x0983
/** Large Passenger Aircraft */
#define BT_APPEARANCE_AIRCRAFT_LARGE_PASSENGER              0x0984
/** Generic AV Equipment */
#define BT_APPEARANCE_GENERIC_AV_EQUIPMENT                  0x09C0
/** Amplifier */
#define BT_APPEARANCE_AV_EQUIPMENT_AMPLIFIER                0x09C1
/** Receiver */
#define BT_APPEARANCE_AV_EQUIPMENT_RECEIVER                 0x09C2
/** Radio */
#define BT_APPEARANCE_AV_EQUIPMENT_RADIO                    0x09C3
/** Tuner */
#define BT_APPEARANCE_AV_EQUIPMENT_TUNER                    0x09C4
/** Turntable */
#define BT_APPEARANCE_AV_EQUIPMENT_TURNTABLE                0x09C5
/** CD Player */
#define BT_APPEARANCE_AV_EQUIPMENT_CD_PLAYER                0x09C6
/** DVD Player */
#define BT_APPEARANCE_AV_EQUIPMENT_DVD_PLAYER               0x09C7
/** Bluray Player */
#define BT_APPEARANCE_AV_EQUIPMENT_BLURAY_PLAYER            0x09C8
/** Optical Disc Player */
#define BT_APPEARANCE_AV_EQUIPMENT_OPTICAL_DISC_PLAYER      0x09C9
/** Set-Top Box */
#define BT_APPEARANCE_AV_EQUIPMENT_SET_TOP_BOX              0x09CA
/** Generic Display Equipment */
#define BT_APPEARANCE_GENERIC_DISPLAY_EQUIPMENT             0x0A00
/** Television */
#define BT_APPEARANCE_DISPLAY_EQUIPMENT_TELEVISION          0x0A01
/** Monitor */
#define BT_APPEARANCE_DISPLAY_EQUIPMENT_MONITOR             0x0A02
/** Projector */
#define BT_APPEARANCE_DISPLAY_EQUIPMENT_PROJECTOR           0x0A03
/** Generic Hearing aid */
#define BT_APPEARANCE_GENERIC_HEARING_AID                   0x0A40
/** In-ear hearing aid */
#define BT_APPEARANCE_HEARING_AID_IN_EAR                    0x0A41
/** Behind-ear hearing aid */
#define BT_APPEARANCE_HEARING_AID_BEHIND_EAR                0x0A42
/** Cochlear Implant */
#define BT_APPEARANCE_HEARING_AID_COCHLEAR_IMPLANT          0x0A43
/** Generic Gaming */
#define BT_APPEARANCE_GENERIC_GAMING                        0x0A80
/** Home Video Game Console */
#define BT_APPEARANCE_HOME_VIDEO_GAME_CONSOLE               0x0A81
/** Portable handheld console */
#define BT_APPEARANCE_PORTABLE_HANDHELD_CONSOLE             0x0A82
/** Generic Signage */
#define BT_APPEARANCE_GENERIC_SIGNAGE                       0x0AC0
/** Digital Signage */
#define BT_APPEARANCE_SIGNAGE_DIGITAL                       0x0AC1
/** Electronic Label */
#define BT_APPEARANCE_SIGNAGE_ELECTRONIC_LABEL              0x0AC2
/** Generic Pulse Oximeter */
#define BT_APPEARANCE_GENERIC_PULSE_OXIMETER                0x0C40
/** Fingertip Pulse Oximeter */
#define BT_APPEARANCE_PULSE_OXIMETER_FINGERTIP              0x0C41
/** Wrist Worn Pulse Oximeter */
#define BT_APPEARANCE_PULSE_OXIMETER_WRIST                  0x0C42
/** Generic Weight Scale */
#define BT_APPEARANCE_GENERIC_WEIGHT_SCALE                  0x0C80
/** Generic Personal Mobility Device */
#define BT_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE      0x0CC0
/** Powered Wheelchair */
#define BT_APPEARANCE_MOBILITY_POWERED_WHEELCHAIR           0x0CC1
/** Mobility Scooter */
#define BT_APPEARANCE_MOBILITY_SCOOTER                      0x0CC2
/** Continuous Glucose Monitor */
#define BT_APPEARANCE_CONTINUOUS_GLUCOSE_MONITOR            0x0D00
/** Generic Insulin Pump */
#define BT_APPEARANCE_GENERIC_INSULIN_PUMP                  0x0D40
/** Insulin Pump, durable pump */
#define BT_APPEARANCE_INSULIN_PUMP_DURABLE                  0x0D41
/** Insulin Pump, patch pump */
#define BT_APPEARANCE_INSULIN_PUMP_PATCH                    0x0D44
/** Insulin Pen */
#define BT_APPEARANCE_INSULIN_PEN                           0x0D48
/** Generic Medication Delivery */
#define BT_APPEARANCE_GENERIC_MEDICATION_DELIVERY           0x0D80
/** Generic Spirometer */
#define BT_APPEARANCE_GENERIC_SPIROMETER                    0x0DC0
/** Handheld Spirometer */
#define BT_APPEARANCE_SPIROMETER_HANDHELD                   0x0DC1
/** Generic Outdoor Sports Activity */
#define BT_APPEARANCE_GENERIC_OUTDOOR_SPORTS                0x1440
/** Location Display */
#define BT_APPEARANCE_OUTDOOR_SPORTS_LOCATION               0x1441
/** Location and Navigation Display */
#define BT_APPEARANCE_OUTDOOR_SPORTS_LOCATION_AND_NAV       0x1442
/** Location Pod */
#define BT_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD           0x1443
/** Location and Navigation Pod */
#define BT_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD_AND_NAV   0x1444

/** @} */ /* end of @name Appearance Numbers */

/**
 * @name Common Data Types
 * @{
 */
#define BT_DATA_FLAGS                    0x01 /**< AD flags */
#define BT_DATA_UUID16_SOME              0x02 /**< 16-bit UUID, more available */
#define BT_DATA_UUID16_ALL               0x03 /**< 16-bit UUID, all listed */
#define BT_DATA_UUID32_SOME              0x04 /**< 32-bit UUID, more available */
#define BT_DATA_UUID32_ALL               0x05 /**< 32-bit UUID, all listed */
#define BT_DATA_UUID128_SOME             0x06 /**< 128-bit UUID, more available */
#define BT_DATA_UUID128_ALL              0x07 /**< 128-bit UUID, all listed */
#define BT_DATA_NAME_SHORTENED           0x08 /**< Shortened name */
#define BT_DATA_NAME_COMPLETE            0x09 /**< Complete name */
#define BT_DATA_TX_POWER                 0x0a /**< Tx Power */
#define BT_DATA_DEVICE_CLASS             0x0d /**< Class of Device */
#define BT_DATA_SIMPLE_PAIRING_HASH_C192 0x0e /**< Simple Pairing Hash C-192 */
#define BT_DATA_SIMPLE_PAIRING_RAND_C192 0x0f /**< Simple Pairing Randomizer R-192 */
#define BT_DATA_DEVICE_ID                0x10 /**< Device ID (Profile) */
#define BT_DATA_SM_TK_VALUE              0x10 /**< Security Manager TK Value */
#define BT_DATA_SM_OOB_FLAGS             0x11 /**< Security Manager OOB Flags */
#define BT_DATA_PERIPHERAL_INT_RANGE     0x12 /**< Peripheral Connection Interval Range */
#define BT_DATA_SOLICIT16                0x14 /**< Solicit UUIDs, 16-bit */
#define BT_DATA_SOLICIT128               0x15 /**< Solicit UUIDs, 128-bit */
#define BT_DATA_SVC_DATA16               0x16 /**< Service data, 16-bit UUID */
#define BT_DATA_PUB_TARGET_ADDR          0x17 /**< Public Target Address */
#define BT_DATA_RAND_TARGET_ADDR         0x18 /**< Random Target Address */
#define BT_DATA_GAP_APPEARANCE           0x19 /**< GAP appearance */
#define BT_DATA_ADV_INT                  0x1a /**< Advertising Interval */
#define BT_DATA_LE_BT_DEVICE_ADDRESS     0x1b /**< LE Bluetooth Device Address */
#define BT_DATA_LE_ROLE                  0x1c /**< LE Role */
#define BT_DATA_SIMPLE_PAIRING_HASH      0x1d /**< Simple Pairing Hash C256 */
#define BT_DATA_SIMPLE_PAIRING_RAND      0x1e /**< Simple Pairing Randomizer R256 */
#define BT_DATA_SOLICIT32                0x1f /**< Solicit UUIDs, 32-bit */
#define BT_DATA_SVC_DATA32               0x20 /**< Service data, 32-bit UUID */
#define BT_DATA_SVC_DATA128              0x21 /**< Service data, 128-bit UUID */
#define BT_DATA_LE_SC_CONFIRM_VALUE      0x22 /**< LE SC Confirmation Value */
#define BT_DATA_LE_SC_RANDOM_VALUE       0x23 /**< LE SC Random Value */
#define BT_DATA_URI                      0x24 /**< URI */
#define BT_DATA_INDOOR_POS               0x25 /**< Indoor Positioning */
#define BT_DATA_TRANS_DISCOVER_DATA      0x26 /**< Transport Discovery Data */
#define BT_DATA_LE_SUPPORTED_FEATURES    0x27 /**< LE Supported Features */
#define BT_DATA_CHANNEL_MAP_UPDATE_IND   0x28 /**< Channel Map Update Indication */
#define BT_DATA_MESH_PROV                0x29 /**< Mesh Provisioning PDU */
#define BT_DATA_MESH_MESSAGE             0x2a /**< Mesh Networking PDU */
#define BT_DATA_MESH_BEACON              0x2b /**< Mesh Beacon */
#define BT_DATA_BIG_INFO                 0x2c /**< BIGInfo */
#define BT_DATA_BROADCAST_CODE           0x2d /**< Broadcast Code */
#define BT_DATA_CSIS_RSI                 0x2e /**< CSIS Random Set ID type */
#define BT_DATA_ADV_INT_LONG             0x2f /**< Advertising Interval long */
#define BT_DATA_BROADCAST_NAME           0x30 /**< Broadcast Name */
#define BT_DATA_ENCRYPTED_AD_DATA        0x31 /**< Encrypted Advertising Data */
#define BT_DATA_PAWR_TIMING_INFO         0x32 /**< Periodic Advertising Response Timing Info */
#define BT_DATA_ESL                      0x34 /**< Electronic Shelf Label Profile */
#define BT_DATA_3D_INFO                  0x3D /**< 3D Information Data */

#define BT_DATA_MANUFACTURER_DATA        0xff /**< Manufacturer Specific Data */

/** @} */ /* end of @name Common Data Types */

/**
 * @name Flags data type values
 * @{
 */

#define BT_LE_AD_LIMITED                 0x01 /**< Limited Discoverable */
#define BT_LE_AD_GENERAL                 0x02 /**< General Discoverable */
#define BT_LE_AD_NO_BREDR                0x04 /**< BR/EDR not supported */

/** @} */ /* end of @name Flags data type values */
/** @} */ /* end of bt_assigned_numbers_core */

/**
 * @brief Generic Audio Assigned Numbers
 * @defgroup bt_assigned_numbers_audio Generic Audio Assigned Numbers
 * @ingroup bt_assigned_numbers
 * @{
 */

/**
 * @brief Codec capability types
 *
 * Used to build and parse codec capabilities as specified in the PAC specification.
 * Source is assigned numbers for Generic Audio, bluetooth.com.
 */
enum bt_audio_codec_cap_type {
    /** Supported sampling frequencies */
    BT_AUDIO_CODEC_CAP_TYPE_FREQ = 0x01,

    /** Supported frame durations */
    BT_AUDIO_CODEC_CAP_TYPE_DURATION = 0x02,

    /** Supported audio channel counts */
    BT_AUDIO_CODEC_CAP_TYPE_CHAN_COUNT = 0x03,

    /** Supported octets per codec frame */
    BT_AUDIO_CODEC_CAP_TYPE_FRAME_LEN = 0x04,

    /** Supported maximum codec frames per SDU  */
    BT_AUDIO_CODEC_CAP_TYPE_FRAME_COUNT = 0x05,
};

/** @brief Supported frequencies bitfield */
enum bt_audio_codec_cap_freq {
    /** 8 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_8KHZ = BIT(0),

    /** 11.025 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_11KHZ = BIT(1),

    /** 16 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_16KHZ = BIT(2),

    /** 22.05 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_22KHZ = BIT(3),

    /** 24 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_24KHZ = BIT(4),

    /** 32 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_32KHZ = BIT(5),

    /** 44.1 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_44KHZ = BIT(6),

    /** 48 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_48KHZ = BIT(7),

    /** 88.2 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_88KHZ = BIT(8),

    /** 96 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_96KHZ = BIT(9),

    /** 176.4 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_176KHZ = BIT(10),

    /** 192 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_192KHZ = BIT(11),

    /** 384 Khz sampling frequency */
    BT_AUDIO_CODEC_CAP_FREQ_384KHZ = BIT(12),

    /** Any frequency capability */
    BT_AUDIO_CODEC_CAP_FREQ_ANY =
        (BT_AUDIO_CODEC_CAP_FREQ_8KHZ | BT_AUDIO_CODEC_CAP_FREQ_11KHZ |
         BT_AUDIO_CODEC_CAP_FREQ_16KHZ | BT_AUDIO_CODEC_CAP_FREQ_22KHZ |
         BT_AUDIO_CODEC_CAP_FREQ_24KHZ | BT_AUDIO_CODEC_CAP_FREQ_32KHZ |
         BT_AUDIO_CODEC_CAP_FREQ_44KHZ | BT_AUDIO_CODEC_CAP_FREQ_48KHZ |
         BT_AUDIO_CODEC_CAP_FREQ_88KHZ | BT_AUDIO_CODEC_CAP_FREQ_96KHZ |
         BT_AUDIO_CODEC_CAP_FREQ_176KHZ | BT_AUDIO_CODEC_CAP_FREQ_192KHZ |
         BT_AUDIO_CODEC_CAP_FREQ_384KHZ),
};

/** @brief Supported frame durations bitfield */
enum bt_audio_codec_cap_frame_dur {
    /** 7.5 msec frame duration capability */
    BT_AUDIO_CODEC_CAP_DURATION_7_5 = BIT(0),

    /** 10 msec frame duration capability */
    BT_AUDIO_CODEC_CAP_DURATION_10 = BIT(1),

    /** Any frame duration capability */
    BT_AUDIO_CODEC_CAP_DURATION_ANY =
        (BT_AUDIO_CODEC_CAP_DURATION_7_5 | BT_AUDIO_CODEC_CAP_DURATION_10),

    /**
     * @brief 7.5 msec preferred frame duration capability.
     *
     * This shall only be set if @ref BT_AUDIO_CODEC_CAP_DURATION_7_5 is also set, and if @ref
     * BT_AUDIO_CODEC_CAP_DURATION_PREFER_10 is not set.
     */
    BT_AUDIO_CODEC_CAP_DURATION_PREFER_7_5 = BIT(4),

    /**
     * @brief 10 msec preferred frame duration capability
     *
     * This shall only be set if @ref BT_AUDIO_CODEC_CAP_DURATION_10 is also set, and if @ref
     * BT_AUDIO_CODEC_CAP_DURATION_PREFER_7_5 is not set.
     */
    BT_AUDIO_CODEC_CAP_DURATION_PREFER_10 = BIT(5),
};

/** Supported audio capabilities channel count bitfield */
enum bt_audio_codec_cap_chan_count {
    /** Supporting 1 channel */
    BT_AUDIO_CODEC_CAP_CHAN_COUNT_1 = BIT(0),

    /** Supporting 2 channel */
    BT_AUDIO_CODEC_CAP_CHAN_COUNT_2 = BIT(1),

    /** Supporting 3 channel */
    BT_AUDIO_CODEC_CAP_CHAN_COUNT_3 = BIT(2),

    /** Supporting 4 channel */
    BT_AUDIO_CODEC_CAP_CHAN_COUNT_4 = BIT(3),

    /** Supporting 5 channel */
    BT_AUDIO_CODEC_CAP_CHAN_COUNT_5 = BIT(4),

    /** Supporting 6 channel */
    BT_AUDIO_CODEC_CAP_CHAN_COUNT_6 = BIT(5),

    /** Supporting 7 channel */
    BT_AUDIO_CODEC_CAP_CHAN_COUNT_7 = BIT(6),

    /** Supporting 8 channel */
    BT_AUDIO_CODEC_CAP_CHAN_COUNT_8 = BIT(7),

    /** Supporting all channels */
    BT_AUDIO_CODEC_CAP_CHAN_COUNT_ANY =
        (BT_AUDIO_CODEC_CAP_CHAN_COUNT_1 | BT_AUDIO_CODEC_CAP_CHAN_COUNT_2 |
         BT_AUDIO_CODEC_CAP_CHAN_COUNT_3 | BT_AUDIO_CODEC_CAP_CHAN_COUNT_4 |
         BT_AUDIO_CODEC_CAP_CHAN_COUNT_5 | BT_AUDIO_CODEC_CAP_CHAN_COUNT_6 |
         BT_AUDIO_CODEC_CAP_CHAN_COUNT_7 | BT_AUDIO_CODEC_CAP_CHAN_COUNT_8),
};

/** Minimum supported channel counts */
#define BT_AUDIO_CODEC_CAP_CHAN_COUNT_MIN 1
/** Maximum supported channel counts */
#define BT_AUDIO_CODEC_CAP_CHAN_COUNT_MAX 8

/** The minimum size of a Broadcast Name as defined by Bluetooth Assigned Numbers */
#define BT_AUDIO_BROADCAST_NAME_LEN_MIN          4
/** The maximum size of a Broadcast Name as defined by Bluetooth Assigned Numbers */
#define BT_AUDIO_BROADCAST_NAME_LEN_MAX          128

/**
 * @brief Codec configuration types
 *
 * Used to build and parse codec configurations as specified in the ASCS and BAP specifications.
 * Source is assigned numbers for Generic Audio, bluetooth.com.
 */
enum bt_audio_codec_cfg_type {
    /** Sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ = 0x01,

    /** Frame duration */
    BT_AUDIO_CODEC_CFG_DURATION = 0x02,

    /** Audio channel allocation */
    BT_AUDIO_CODEC_CFG_CHAN_ALLOC = 0x03,

    /** Octets per codec frame */
    BT_AUDIO_CODEC_CFG_FRAME_LEN = 0x04,

    /** Codec frame blocks per SDU */
    BT_AUDIO_CODEC_CFG_FRAME_BLKS_PER_SDU = 0x05,
};

/** Codec configuration sampling frequency */
enum bt_audio_codec_cfg_freq {
    /** 8 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_8KHZ = 0x01,

    /** 11.025 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_11KHZ = 0x02,

    /** 16 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_16KHZ = 0x03,

    /** 22.05 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_22KHZ = 0x04,

    /** 24 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_24KHZ = 0x05,

    /** 32 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_32KHZ = 0x06,

    /** 44.1 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_44KHZ = 0x07,

    /** 48 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_48KHZ = 0x08,

    /** 88.2 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_88KHZ = 0x09,

    /** 96 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_96KHZ = 0x0a,

    /** 176.4 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_176KHZ = 0x0b,

    /** 192 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_192KHZ = 0x0c,

    /** 384 Khz codec sampling frequency */
    BT_AUDIO_CODEC_CFG_FREQ_384KHZ = 0x0d,
};

/** Codec configuration frame duration */
enum bt_audio_codec_cfg_frame_dur {
    /** 7.5 msec Frame Duration configuration */
    BT_AUDIO_CODEC_CFG_DURATION_7_5 = 0x00,

    /** 10 msec Frame Duration configuration */
    BT_AUDIO_CODEC_CFG_DURATION_10 = 0x01,
};

/**
 * @brief Audio Context Type for Generic Audio
 *
 * These values are defined by the Generic Audio Assigned Numbers, bluetooth.com
 */
enum bt_audio_context {
    /** No context type */
    BT_AUDIO_CONTEXT_TYPE_NONE = 0,
    /**
     * Identifies audio where the use case context does not match any other defined value,
     * or where the context is unknown or cannot be determined.
     */
    BT_AUDIO_CONTEXT_TYPE_UNSPECIFIED = BIT(0),
    /**
     * Conversation between humans, for example, in telephony or video calls, including
     * traditional cellular as well as VoIP and Push-to-Talk
     */
    BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL = BIT(1),
    /** Media, for example, music playback, radio, podcast or movie soundtrack, or tv audio */
    BT_AUDIO_CONTEXT_TYPE_MEDIA = BIT(2),
    /**
     * Audio associated with video gaming, for example gaming media; gaming effects; music
     * and in-game voice chat between participants; or a mix of all the above
     */
    BT_AUDIO_CONTEXT_TYPE_GAME = BIT(3),
    /** Instructional audio, for example, in navigation, announcements, or user guidance */
    BT_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL = BIT(4),
    /** Man-machine communication, for example, with voice recognition or virtual assistants */
    BT_AUDIO_CONTEXT_TYPE_VOICE_ASSISTANTS = BIT(5),
    /**
     * Live audio, for example, from a microphone where audio is perceived both through a
     * direct acoustic path and through an LE Audio Stream
     */
    BT_AUDIO_CONTEXT_TYPE_LIVE = BIT(6),
    /**
     * Sound effects including keyboard and touch feedback; menu and user interface sounds;
     * and other system sounds
     */
    BT_AUDIO_CONTEXT_TYPE_SOUND_EFFECTS = BIT(7),
    /**
     * Notification and reminder sounds; attention-seeking audio, for example,
     * in beeps signaling the arrival of a message
     */
    BT_AUDIO_CONTEXT_TYPE_NOTIFICATIONS = BIT(8),
    /**
     * Alerts the user to an incoming call, for example, an incoming telephony or video call,
     * including traditional cellular as well as VoIP and Push-to-Talk
     */
    BT_AUDIO_CONTEXT_TYPE_RINGTONE = BIT(9),
    /**
     * Alarms and timers; immediate alerts, for example, in a critical battery alarm,
     * timer expiry or alarm clock, toaster, cooker, kettle, microwave, etc.
     */
    BT_AUDIO_CONTEXT_TYPE_ALERTS = BIT(10),
    /** Emergency alarm Emergency sounds, for example, fire alarms or other urgent alerts */
    BT_AUDIO_CONTEXT_TYPE_EMERGENCY_ALARM = BIT(11),
};

/**
 * Any known context.
 */
#define BT_AUDIO_CONTEXT_TYPE_ANY    (BT_AUDIO_CONTEXT_TYPE_UNSPECIFIED | \
                      BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                      BT_AUDIO_CONTEXT_TYPE_MEDIA | \
                      BT_AUDIO_CONTEXT_TYPE_GAME | \
                      BT_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL | \
                      BT_AUDIO_CONTEXT_TYPE_VOICE_ASSISTANTS | \
                      BT_AUDIO_CONTEXT_TYPE_LIVE | \
                      BT_AUDIO_CONTEXT_TYPE_SOUND_EFFECTS | \
                      BT_AUDIO_CONTEXT_TYPE_NOTIFICATIONS | \
                      BT_AUDIO_CONTEXT_TYPE_RINGTONE | \
                      BT_AUDIO_CONTEXT_TYPE_ALERTS | \
                      BT_AUDIO_CONTEXT_TYPE_EMERGENCY_ALARM)

/**
 * @brief Parental rating defined by the Generic Audio assigned numbers (bluetooth.com).
 *
 * The numbering scheme is aligned with Annex F of EN 300 707 v1.2.1 which
 * defined parental rating for viewing.
 */
enum bt_audio_parental_rating {
    /** No rating */
    BT_AUDIO_PARENTAL_RATING_NO_RATING        = 0x00,
    /** For all ages */
    BT_AUDIO_PARENTAL_RATING_AGE_ANY          = 0x01,
    /** Recommended for listeners of age 5 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_5_OR_ABOVE   = 0x02,
    /** Recommended for listeners of age 6 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_6_OR_ABOVE   = 0x03,
    /** Recommended for listeners of age 7 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_7_OR_ABOVE   = 0x04,
    /** Recommended for listeners of age 8 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_8_OR_ABOVE   = 0x05,
    /** Recommended for listeners of age 9 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_9_OR_ABOVE   = 0x06,
    /** Recommended for listeners of age 10 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_10_OR_ABOVE  = 0x07,
    /** Recommended for listeners of age 11 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_11_OR_ABOVE  = 0x08,
    /** Recommended for listeners of age 12 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_12_OR_ABOVE  = 0x09,
    /** Recommended for listeners of age 13 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_13_OR_ABOVE  = 0x0A,
    /** Recommended for listeners of age 14 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_14_OR_ABOVE  = 0x0B,
    /** Recommended for listeners of age 15 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_15_OR_ABOVE  = 0x0C,
    /** Recommended for listeners of age 16 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_16_OR_ABOVE  = 0x0D,
    /** Recommended for listeners of age 17 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_17_OR_ABOVE  = 0x0E,
    /** Recommended for listeners of age 18 and above */
    BT_AUDIO_PARENTAL_RATING_AGE_18_OR_ABOVE  = 0x0F
};

/** @brief Audio Active State defined by the Generic Audio assigned numbers (bluetooth.com). */
enum bt_audio_active_state {
    /** No audio data is being transmitted */
    BT_AUDIO_ACTIVE_STATE_DISABLED       = 0x00,
    /** Audio data is being transmitted */
    BT_AUDIO_ACTIVE_STATE_ENABLED        = 0x01,
};

/** Assisted Listening Stream defined by the Generic Audio assigned numbers (bluetooth.com). */
enum bt_audio_assisted_listening_stream {
    /** Unspecified audio enhancement */
    BT_AUDIO_ASSISTED_LISTENING_STREAM_UNSPECIFIED = 0x00,
};

/**
 * @brief Codec metadata type IDs
 *
 * Metadata types defined by the Generic Audio assigned numbers (bluetooth.com).
 */
enum bt_audio_metadata_type {
    /**
     * @brief Preferred audio context.
     *
     * Bitfield of preferred audio contexts.
     *
     * If 0, the context type is not a preferred use case for this codec
     * configuration.
     *
     * See the BT_AUDIO_CONTEXT_* for valid values.
     */
    BT_AUDIO_METADATA_TYPE_PREF_CONTEXT = 0x01,

    /**
     * @brief Streaming audio context.
     *
     * Bitfield of streaming audio contexts.
     *
     * If 0, the context type is not a preferred use case for this codec
     * configuration.
     *
     * See the BT_AUDIO_CONTEXT_* for valid values.
     */
    BT_AUDIO_METADATA_TYPE_STREAM_CONTEXT = 0x02,

    /** UTF-8 encoded title or summary of stream content */
    BT_AUDIO_METADATA_TYPE_PROGRAM_INFO = 0x03,

    /**
     * @brief Language
     *
     * 3 octet lower case language code defined by ISO 639-3
     * Possible values can be found at https://iso639-3.sil.org/code_tables/639/data
     */
    BT_AUDIO_METADATA_TYPE_LANG = 0x04,

    /** Array of 8-bit CCID values */
    BT_AUDIO_METADATA_TYPE_CCID_LIST = 0x05,

    /**
     * @brief Parental rating
     *
     * See @ref bt_audio_parental_rating for valid values.
     */
    BT_AUDIO_METADATA_TYPE_PARENTAL_RATING = 0x06,

    /** UTF-8 encoded URI for additional Program information */
    BT_AUDIO_METADATA_TYPE_PROGRAM_INFO_URI = 0x07,

    /**
     * @brief Audio active state
     *
     * See @ref bt_audio_active_state for valid values.
     */
    BT_AUDIO_METADATA_TYPE_AUDIO_STATE = 0x08,

    /** Broadcast Audio Immediate Rendering flag  */
    BT_AUDIO_METADATA_TYPE_BROADCAST_IMMEDIATE = 0x09,

    /**
     * @brief Assisted listening stream
     *
     * See @ref bt_audio_assisted_listening_stream for valid values.
     */
    BT_AUDIO_METADATA_TYPE_ASSISTED_LISTENING_STREAM = 0x0A,

    /** UTF-8 encoded Broadcast name */
    BT_AUDIO_METADATA_TYPE_BROADCAST_NAME = 0x0B,

    /** Extended metadata */
    BT_AUDIO_METADATA_TYPE_EXTENDED = 0xFE,

    /** Vendor specific metadata */
    BT_AUDIO_METADATA_TYPE_VENDOR = 0xFF,
};

/**
 * @brief Location values for BT Audio.
 *
 * These values are defined by the Generic Audio Assigned Numbers, bluetooth.com
 */
enum bt_audio_location {
    /** Mono Audio (no specified Audio Location) */
    BT_AUDIO_LOCATION_MONO_AUDIO = 0,
    /** Front Left */
    BT_AUDIO_LOCATION_FRONT_LEFT = BIT(0),
    /** Front Right */
    BT_AUDIO_LOCATION_FRONT_RIGHT = BIT(1),
    /** Front Center */
    BT_AUDIO_LOCATION_FRONT_CENTER = BIT(2),
    /** Low Frequency Effects 1 */
    BT_AUDIO_LOCATION_LOW_FREQ_EFFECTS_1 = BIT(3),
    /** Back Left */
    BT_AUDIO_LOCATION_BACK_LEFT = BIT(4),
    /** Back Right */
    BT_AUDIO_LOCATION_BACK_RIGHT = BIT(5),
    /** Front Left of Center */
    BT_AUDIO_LOCATION_FRONT_LEFT_OF_CENTER = BIT(6),
    /** Front Right of Center */
    BT_AUDIO_LOCATION_FRONT_RIGHT_OF_CENTER = BIT(7),
    /** Back Center */
    BT_AUDIO_LOCATION_BACK_CENTER = BIT(8),
    /** Low Frequency Effects 2 */
    BT_AUDIO_LOCATION_LOW_FREQ_EFFECTS_2 = BIT(9),
    /** Side Left */
    BT_AUDIO_LOCATION_SIDE_LEFT = BIT(10),
    /** Side Right */
    BT_AUDIO_LOCATION_SIDE_RIGHT = BIT(11),
    /** Top Front Left */
    BT_AUDIO_LOCATION_TOP_FRONT_LEFT = BIT(12),
    /** Top Front Right */
    BT_AUDIO_LOCATION_TOP_FRONT_RIGHT = BIT(13),
    /** Top Front Center */
    BT_AUDIO_LOCATION_TOP_FRONT_CENTER = BIT(14),
    /** Top Center */
    BT_AUDIO_LOCATION_TOP_CENTER = BIT(15),
    /** Top Back Left */
    BT_AUDIO_LOCATION_TOP_BACK_LEFT = BIT(16),
    /** Top Back Right */
    BT_AUDIO_LOCATION_TOP_BACK_RIGHT = BIT(17),
    /** Top Side Left */
    BT_AUDIO_LOCATION_TOP_SIDE_LEFT = BIT(18),
    /** Top Side Right */
    BT_AUDIO_LOCATION_TOP_SIDE_RIGHT = BIT(19),
    /** Top Back Center */
    BT_AUDIO_LOCATION_TOP_BACK_CENTER = BIT(20),
    /** Bottom Front Center */
    BT_AUDIO_LOCATION_BOTTOM_FRONT_CENTER = BIT(21),
    /** Bottom Front Left */
    BT_AUDIO_LOCATION_BOTTOM_FRONT_LEFT = BIT(22),
    /** Bottom Front Right */
    BT_AUDIO_LOCATION_BOTTOM_FRONT_RIGHT = BIT(23),
    /** Front Left Wide */
    BT_AUDIO_LOCATION_FRONT_LEFT_WIDE = BIT(24),
    /** Front Right Wide */
    BT_AUDIO_LOCATION_FRONT_RIGHT_WIDE = BIT(25),
    /** Left Surround */
    BT_AUDIO_LOCATION_LEFT_SURROUND = BIT(26),
    /** Right Surround */
    BT_AUDIO_LOCATION_RIGHT_SURROUND = BIT(27),
};

/**
 * Any known location.
 */
#define BT_AUDIO_LOCATION_ANY (BT_AUDIO_LOCATION_FRONT_LEFT | \
                   BT_AUDIO_LOCATION_FRONT_RIGHT | \
                   BT_AUDIO_LOCATION_FRONT_CENTER | \
                   BT_AUDIO_LOCATION_LOW_FREQ_EFFECTS_1 | \
                   BT_AUDIO_LOCATION_BACK_LEFT | \
                   BT_AUDIO_LOCATION_BACK_RIGHT | \
                   BT_AUDIO_LOCATION_FRONT_LEFT_OF_CENTER | \
                   BT_AUDIO_LOCATION_FRONT_RIGHT_OF_CENTER | \
                   BT_AUDIO_LOCATION_BACK_CENTER | \
                   BT_AUDIO_LOCATION_LOW_FREQ_EFFECTS_2 | \
                   BT_AUDIO_LOCATION_SIDE_LEFT | \
                   BT_AUDIO_LOCATION_SIDE_RIGHT | \
                   BT_AUDIO_LOCATION_TOP_FRONT_LEFT | \
                   BT_AUDIO_LOCATION_TOP_FRONT_RIGHT | \
                   BT_AUDIO_LOCATION_TOP_FRONT_CENTER | \
                   BT_AUDIO_LOCATION_TOP_CENTER | \
                   BT_AUDIO_LOCATION_TOP_BACK_LEFT | \
                   BT_AUDIO_LOCATION_TOP_BACK_RIGHT | \
                   BT_AUDIO_LOCATION_TOP_SIDE_LEFT | \
                   BT_AUDIO_LOCATION_TOP_SIDE_RIGHT | \
                   BT_AUDIO_LOCATION_TOP_BACK_CENTER | \
                   BT_AUDIO_LOCATION_BOTTOM_FRONT_CENTER | \
                   BT_AUDIO_LOCATION_BOTTOM_FRONT_LEFT | \
                   BT_AUDIO_LOCATION_BOTTOM_FRONT_RIGHT | \
                   BT_AUDIO_LOCATION_FRONT_LEFT_WIDE | \
                   BT_AUDIO_LOCATION_FRONT_RIGHT_WIDE | \
                   BT_AUDIO_LOCATION_LEFT_SURROUND | \
                   BT_AUDIO_LOCATION_RIGHT_SURROUND)

/** @} */ /* end of bt_assigned_numbers_audio */

/**
 * @name Company Identifiers (see Bluetooth Assigned Numbers)
 * @{
 */

#define BT_COMP_ID_LF           0x05f1 /**< The Linux Foundation */

/** @} */ /* end of @name Company Identifiers */

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_ASSIGNED_NUMBERS_H_ */
