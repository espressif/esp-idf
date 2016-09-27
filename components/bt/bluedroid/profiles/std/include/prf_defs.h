/**
 ****************************************************************************************
 *
 * @file prf_defs.h
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/9/8
 *
 *
 ****************************************************************************************
 */

#include "bta_api.h"
#include "bta_gattc_int.h"
#include "bta_gatts_int.h"
#include "bta_gatt_api.h"
#include "bt_types.h"


#define ATT_HANDLE_LEN                          0x0002
#define ATT_UUID_16_LEN                         0x0002
#define ATT_UUID_128_LEN                        0x0010
#define ATT_UUID_32_LEN                         0x0004


/*
 * Type Definition
 ****************************************************************************************
 */

/// Characteristic Value Descriptor
typedef struct 
{
	///characteristic uuid
	tBT_UUID	*char_uuid;
	///the permition of the characteristic
	tBTA_GATT_PERM perm;
    /// the properties of the characteristic
    tBTA_GATT_CHAR_PROP prop;
}tCHAR_DESC;

/// UUID - 128-bit type
typedef struct 
{
    /// 128-bit UUID
    UINT8 uuid[ATT_UUID_128_LEN];
}tUUID_128;

/// UUID - 32-bit type
typedef struct 
{
    /// 32-bit UUID
    UINT8 uuid[ATT_UUID_32_LEN];
}tUUID_32;

/// include service entry element
typedef struct 
{
    /// start handle value of included service
    UINT16 start_hdl;
    /// end handle value of included service
    UINT16 end_hdl;
    /// attribute value UUID
    UINT16 uuid;
}tSVC_INCL_DESC;

/// Service Changed type definition
typedef struct 
{
    /// Service start handle which changed
    UINT16 start_hdl;
    /// Service end handle which changed
    UINT16 end_hdl;
}tSVC_CHANG;



/// Common 16-bit Universal Unique Identifier
enum {
    ATT_INVALID_UUID = 0,
    /*----------------- SERVICES ---------------------*/
    /// Generic Access Profile
    SVC_GENERIC_ACCESS_UUID                      = 0x1800,
    /// Attribute Profile
    SVC_GENERIC_ATTRIBUTE_UUID,
    /// Immediate alert Service
    SVC_IMMEDIATE_ALERT_UUID,
    /// Link Loss Service
    SVC_LINK_LOSS_UUID,
    /// Tx Power Service
    SVC_TX_POWER_UUID,
    /// Current Time Service Service
    SVC_CURRENT_TIME_UUID,
    /// Reference Time Update Service
    SVC_REF_TIME_UPDATE_UUID,
    /// Next DST Change Service
    SVC_NEXT_DST_CHANGE_UUID,
    /// Glucose Service
    SVC_GLUCOSE_UUID                             = 0x1808,
    /// Health Thermometer Service
    SVC_HEALTH_THERMOM_UUID                      = 0x1809,
    /// Device Information Service
    SVC_DEVICE_INFO_UUID                         = 0x180A,
    /// Heart Rate Service
   	SVC_HEART_RATE_UUID                          = 0x180D,
    /// Phone Alert Status Service
    SVC_PHONE_ALERT_STATUS_UUID,
    /// Battery Service
    SVC_BATTERY_SERVICE_UUID,
    /// Blood Pressure Service
    SVC_BLOOD_PRESSURE_UUID                      = 0x1810,
    /// Alert Notification Service
    SVC_ALERT_NTF_UUID                           = 0x1811,
    /// HID Service
    SVC_HID_UUID                                 = 0x1812,
    /// Scan Parameters Service
    SVC_SCAN_PARAMETERS_UUID                     = 0x1813,
    /// Running Speed and Cadence Service
    SVC_RUNNING_SPEED_CADENCE_UUID               = 0x1814,
    /// Cycling Speed and Cadence Service
    SVC_CYCLING_SPEED_CADENCE_UUID               = 0x1816,
    /// Cycling Power Service
    SVC_CYCLING_POWER_UUID                       = 0x1818,
    /// Location and Navigation Service
    SVC_LOCATION_AND_NAVIGATION_UUID             = 0x1819,
    /// User Data Service
    SVC_USER_DATA_UUID                           = 0x181C,
    /// Weight Scale Service
    SVC_WEIGHT_SCALE_UUID                        = 0x181D,

    /*------------------- UNITS ---------------------*/
    /// No defined unit
    UNIT_UNITLESS_UUID                       = 0x2700,
    /// Length Unit - Metre
    UNIT_METRE_UUID,
    //Mass unit - Kilogram
    UNIT_KG_UUID,
    ///Time unit - second
    UNIT_SECOND_UUID,
    ///Electric current unit - Ampere
    UNIT_AMPERE_UUID,
    ///Thermodynamic Temperature unit - Kelvin
    UNIT_KELVIN_UUID,
    /// Amount of substance unit - mole
    UNIT_MOLE_UUID,
    ///Luminous intensity unit - candela
    UNIT_CANDELA_UUID,
    ///Area unit - square metres
    UNIT_SQ_METRE_UUID                       = 0x2710,
    ///Colume unit - cubic metres
    UNIT_CUBIC_METRE_UUID,
    ///Velocity unit - metres per second
    UNIT_METRE_PER_SECOND_UUID,
    ///Acceleration unit - metres per second squared
    UNIT_METRES_PER_SEC_SQ_UUID,
    ///Wavenumber unit - reciprocal metre
    UNIT_RECIPROCAL_METRE_UUID,
    ///Density unit - kilogram per cubic metre
    UNIT_DENS_KG_PER_CUBIC_METRE_UUID,
    ///Surface density unit - kilogram per square metre
    UNIT_KG_PER_SQ_METRE_UUID,
    ///Specific volume unit - cubic metre per kilogram
    UNIT_CUBIC_METRE_PER_KG_UUID,
    ///Current density unit - ampere per square metre
    UNIT_AMPERE_PER_SQ_METRE_UUID,
    ///Magnetic field strength unit - Ampere per metre
    UNIT_AMPERE_PER_METRE_UUID,
    ///Amount concentration unit - mole per cubic metre
    UNIT_MOLE_PER_CUBIC_METRE_UUID,
    ///Mass Concentration unit - kilogram per cubic metre
    UNIT_MASS_KG_PER_CUBIC_METRE_UUID,
    ///Luminance unit - candela per square metre
    UNIT_CANDELA_PER_SQ_METRE_UUID,
    ///Refractive index unit
    UNIT_REFRACTIVE_INDEX_UUID,
    ///Relative permeability unit
    UNIT_RELATIVE_PERMEABILITY_UUID,
    ///Plane angle unit - radian
    UNIT_RADIAN_UUID                         = 0x2720,
    ///Solid angle unit - steradian
    UNIT_STERADIAN_UUID,
    ///Frequency unit - Hertz
    UNIT_HERTZ_UUID,
    ///Force unit - Newton
    UNIT_NEWTON_UUID,
    ///Pressure unit - Pascal
    UNIT_PASCAL_UUID,
    ///Energy unit - Joule
    UNIT_JOULE_UUID,
    ///Power unit - Watt
    UNIT_WATT_UUID,
    ///electric Charge unit - Coulomb
    UNIT_COULOMB_UUID,
    ///Electric potential difference - Volt
    UNIT_VOLT_UUID,
    ///Capacitance unit - Farad
    UNIT_FARAD_UUID,
    ///electric resistance unit - Ohm
    UNIT_OHM_UUID,
    ///Electric conductance - Siemens
    UNIT_SIEMENS_UUID,
    ///Magnetic flux unit - Weber
    UNIT_WEBER_UUID,
    ///Magnetic flux density unit - Tesla
    UNIT_TESLA_UUID,
    ///Inductance unit - Henry
    UNIT_HENRY_UUID,
    ///Temperature unit - degree Celsius
    UNIT_CELSIUS_UUID,
    ///Luminous flux unit - lumen
    UNIT_LUMEN_UUID,
    //Illuminance unit - lux
    UNIT_LUX_UUID,
    ///Activity referred to a radionuclide unit - becquerel
    UNIT_BECQUEREL_UUID,
    ///Absorbed dose unit - Gray
    UNIT_GRAY_UUID,
    ///Dose equivalent unit - Sievert
    UNIT_SIEVERT_UUID,
    ///Catalytic activity unit - Katal
    UNIT_KATAL_UUID,
    ///Synamic viscosity unit - Pascal second
    UNIT_PASCAL_SECOND_UUID                      = 0x2740,
    ///Moment of force unit - Newton metre
    UNIT_NEWTON_METRE_UUID,
    ///surface tension unit - Newton per metre
    UNIT_NEWTON_PER_METRE_UUID,
    ///Angular velocity unit - radian per second
    UNIT_RADIAN_PER_SECOND_UUID,
    ///Angular acceleration unit - radian per second squared
    UNIT_RADIAN_PER_SECOND_SQ_UUID,
    ///Heat flux density unit - Watt per square metre
    UNIT_WATT_PER_SQ_METRE_UUID,
    ///HEat capacity unit - Joule per Kelvin
    UNIT_JOULE_PER_KELVIN_UUID,
    ///Specific heat capacity unit - Joule per kilogram kelvin
    UNIT_JOULE_PER_KG_KELVIN_UUID,
    ///Specific Energy unit - Joule per kilogram
    UNIT_JOULE_PER_KG_UUID,
    ///Thermal conductivity - Watt per metre Kelvin
    UNIT_WATT_PER_METRE_KELVIN_UUID,
    ///Energy Density unit - joule per cubic metre
    UNIT_JOULE_PER_CUBIC_METRE_UUID,
    ///Electric field strength unit - volt per metre
    UNIT_VOLT_PER_METRE_UUID,
    ///Electric charge density unit - coulomb per cubic metre
    UNIT_COULOMB_PER_CUBIC_METRE_UUID,
    ///Surface charge density unit - coulomb per square metre
    UNIT_SURF_COULOMB_PER_SQ_METRE_UUID,
    ///Electric flux density unit - coulomb per square metre
    UNIT_FLUX_COULOMB_PER_SQ_METRE_UUID,
    ///Permittivity unit - farad per metre
    UNIT_FARAD_PER_METRE_UUID,
    ///Permeability unit - henry per metre
    UNIT_HENRY_PER_METRE_UUID,
    ///Molar energy unit - joule per mole
    UNIT_JOULE_PER_MOLE_UUID,
    ///Molar entropy unit - joule per mole kelvin
    UNIT_JOULE_PER_MOLE_KELVIN_UUID,
    ///Exposure unit - coulomb per kilogram
    UNIT_COULOMB_PER_KG_UUID,
    ///Absorbed dose rate unit - gray per second
    UNIT_GRAY_PER_SECOND_UUID,
    ///Radiant intensity unit - watt per steradian
    UNIT_WATT_PER_STERADIAN_UUID,
    ///Radiance unit - watt per square meter steradian
    UNIT_WATT_PER_SQ_METRE_STERADIAN_UUID,
    ///Catalytic activity concentration unit - katal per cubic metre
    UNIT_KATAL_PER_CUBIC_METRE_UUID,
    ///Time unit - minute
    UNIT_MINUTE_UUID                         = 0x2760,
    ///Time unit - hour
    UNIT_HOUR_UUID,
    ///Time unit - day
    UNIT_DAY_UUID,
    ///Plane angle unit - degree
    UNIT_ANGLE_DEGREE_UUID,
    ///Plane angle unit - minute
    UNIT_ANGLE_MINUTE_UUID,
    ///Plane angle unit - second
    UNIT_ANGLE_SECOND_UUID,
    ///Area unit - hectare
    UNIT_HECTARE_UUID,
    ///Volume unit - litre
    UNIT_LITRE_UUID,
    ///Mass unit - tonne
    UNIT_TONNE_UUID,
    ///Pressure unit - bar
    UNIT_BAR_UUID                            = 0x2780,
    ///Pressure unit - millimetre of mercury
    UNIT_MM_MERCURY_UUID,
    ///Length unit - angstrom
    UNIT_ANGSTROM_UUID,
    ///Length unit - nautical mile
    UNIT_NAUTICAL_MILE_UUID,
    ///Area unit - barn
    UNIT_BARN_UUID,
    ///Velocity unit - knot
    UNIT_KNOT_UUID,
    ///Logarithmic radio quantity unit - neper
    UNIT_NEPER_UUID,
    ///Logarithmic radio quantity unit - bel
    UNIT_BEL_UUID,
    ///Length unit - yard
    UNIT_YARD_UUID                           = 0x27A0,
    ///Length unit - parsec
    UNIT_PARSEC_UUID,
    ///length unit - inch
    UNIT_INCH_UUID,
    ///length unit - foot
    UNIT_FOOT_UUID,
    ///length unit - mile
    UNIT_MILE_UUID,
    ///pressure unit - pound-force per square inch
    UNIT_POUND_FORCE_PER_SQ_INCH_UUID,
    ///velocity unit - kilometre per hour
    UNIT_KM_PER_HOUR_UUID,
    ///velocity unit - mile per hour
    UNIT_MILE_PER_HOUR_UUID,
    ///angular velocity unit - revolution per minute
    UNIT_REVOLUTION_PER_MINUTE_UUID,
    ///energy unit - gram calorie
    UNIT_GRAM_CALORIE_UUID,
    ///energy unit - kilogram calorie
    UNIT_KG_CALORIE_UUID,
    /// energy unit - kilowatt hour
    UNIT_KILOWATT_HOUR_UUID,
    ///thermodynamic temperature unit - degree Fahrenheit
    UNIT_FAHRENHEIT_UUID,
    ///percentage
    UNIT_PERCENTAGE_UUID,
    ///per mille
    UNIT_PER_MILLE_UUID,
    ///period unit - beats per minute)
    UNIT_BEATS_PER_MINUTE_UUID,
    ///electric charge unit - ampere hours
    UNIT_AMPERE_HOURS_UUID,
    ///mass density unit - milligram per decilitre
    UNIT_MILLIGRAM_PER_DECILITRE_UUID,
    ///mass density unit - millimole per litre
    UNIT_MILLIMOLE_PER_LITRE_UUID,
    ///time unit - year
    UNIT_YEAR_UUID,
    ////time unit - month
    UNIT_MONTH_UUID,


    /*---------------- DECLARATIONS -----------------*/
    /// Primary service Declaration
    DECL_PRIMARY_SERVICE_UUID                     = 0x2800,
    /// Secondary service Declaration
    DECL_SECONDARY_SERVICE_UUID,
    /// Include Declaration
    DECL_INCLUDE_UUID,
    /// Characteristic Declaration
    DECL_CHARACTERISTIC_UUID,


    /*----------------- DESCRIPTORS -----------------*/
    /// Characteristic extended properties
    DESC_CHAR_EXT_PROPERTIES_UUID                 = 0x2900,
    /// Characteristic user description
    DESC_CHAR_USER_DESCRIPTION_UUID,
    /// Client characteristic configuration
    DESC_CLIENT_CHAR_CFG_UUID,
    /// Server characteristic configuration
    DESC_SERVER_CHAR_CFG_UUID,
    /// Characteristic Presentation Format
    DESC_CHAR_PRES_FORMAT_UUID,
    /// Characteristic Aggregate Format
    DESC_CHAR_AGGREGATE_FORMAT_UUID,
    /// Valid Range
    DESC_VALID_RANGE_UUID,
    /// External Report Reference
    DESC_EXT_REPORT_REF_UUID,
    /// Report Reference
    DESC_REPORT_REF_UUID,


    /*--------------- CHARACTERISTICS ---------------*/
    /// Device name
    CHAR_DEVICE_NAME_UUID                        = 0x2A00,
    /// Appearance
    CHAR_APPEARANCE_UUID                         = 0x2A01,
    /// Privacy flag
    CHAR_PRIVACY_FLAG_UUID                       = 0x2A02,
    /// Reconnection address
    CHAR_RECONNECTION_ADDR_UUID                  = 0x2A03,
    /// Peripheral preferred connection parameters
    CHAR_PERIPH_PREF_CON_PARAM_UUID              = 0x2A04,
    /// Service handles changed
    CHAR_SERVICE_CHANGED_UUID                    = 0x2A05,
    /// Alert Level characteristic
    CHAR_ALERT_LEVEL_UUID                        = 0x2A06,
    /// Tx Power Level
    CHAR_TX_POWER_LEVEL_UUID                     = 0x2A07,
    /// Date Time
    CHAR_DATE_TIME_UUID                          = 0x2A08,
    /// Day of Week
    CHAR_DAY_WEEK_UUID                           = 0x2A09,
    /// Day Date Time
    CHAR_DAY_DATE_TIME_UUID                      = 0x2A0A,
    /// Exact time 256
    CHAR_EXACT_TIME_256_UUID                     = 0x2A0C,
    /// DST Offset
    CHAR_DST_OFFSET_UUID                         = 0x2A0D,
    /// Time zone
    CHAR_TIME_ZONE_UUID                          = 0x2A0E,
    /// Local time Information
    CHAR_LOCAL_TIME_INFO_UUID                    = 0x2A0F,
    /// Time with DST
    CHAR_TIME_WITH_DST_UUID                      = 0x2A11,
    /// Time Accuracy
    CHAR_TIME_ACCURACY_UUID                      = 0x2A12,
    ///Time Source
    CHAR_TIME_SOURCE_UUID                        = 0x2A13,
    /// Reference Time Information
    CHAR_REFERENCE_TIME_INFO_UUID                = 0x2A14,
    /// Time Update Control Point
    CHAR_TIME_UPDATE_CNTL_POINT_UUID             = 0x2A16,
    /// Time Update State
    CHAR_TIME_UPDATE_STATE_UUID                  = 0x2A17,
    /// Glucose Measurement
    CHAR_GLUCOSE_MEAS_UUID                       = 0x2A18,
    /// Battery Level
    CHAR_BATTERY_LEVEL_UUID                      = 0x2A19,
    /// Temperature Measurement
    CHAR_TEMPERATURE_MEAS_UUID                   = 0x2A1C,
    /// Temperature Type
    CHAR_TEMPERATURE_TYPE_UUID                   = 0x2A1D,
    /// Intermediate Temperature
    CHAR_INTERMED_TEMPERATURE_UUID               = 0x2A1E,
    /// Measurement Interval
    CHAR_MEAS_INTERVAL_UUID                      = 0x2A21,
    /// Boot Keyboard Input Report
    CHAR_BOOT_KB_IN_REPORT_UUID                  = 0x2A22,
    /// System ID
    CHAR_SYS_ID_UUID                             = 0x2A23,
    /// Model Number String
    CHAR_MODEL_NB_UUID                           = 0x2A24,
    /// Serial Number String
    CHAR_SERIAL_NB_UUID                          = 0x2A25,
    /// Firmware Revision String
    CHAR_FW_REV_UUID                             = 0x2A26,
    /// Hardware revision String
    CHAR_HW_REV_UUID                             = 0x2A27,
    /// Software Revision String
    CHAR_SW_REV_UUID                             = 0x2A28,
    /// Manufacturer Name String
    CHAR_MANUF_NAME_UUID                         = 0x2A29,
    /// IEEE Regulatory Certification Data List
    CHAR_IEEE_CERTIF_UUID                        = 0x2A2A,
    /// CT Time
    CHAR_CT_TIME_UUID                            = 0x2A2B,
    /// Scan Refresh
    CHAR_SCAN_REFRESH_UUID                       = 0x2A31,
    /// Boot Keyboard Output Report
    CHAR_BOOT_KB_OUT_REPORT_UUID                 = 0x2A32,
    /// Boot Mouse Input Report
    CHAR_BOOT_MOUSE_IN_REPORT_UUID               = 0x2A33,
    /// Glucose Measurement Context
    CHAR_GLUCOSE_MEAS_CTX_UUID                   = 0x2A34,
    /// Blood Pressure Measurement
    CHAR_BLOOD_PRESSURE_MEAS_UUID                = 0x2A35,
    /// Intermediate Cuff Pressure
    CHAR_INTERMEDIATE_CUFF_PRESSURE_UUID         = 0x2A36,
    /// Heart Rate Measurement
    CHAR_HEART_RATE_MEAS_UUID                    = 0x2A37,
    /// Body Sensor Location
    CHAR_BODY_SENSOR_LOCATION_UUID               = 0x2A38,
    /// Heart Rate Control Point
    CHAR_HEART_RATE_CNTL_POINT_UUID              = 0x2A39,
    /// Alert Status
    CHAR_ALERT_STATUS_UUID                       = 0x2A3F,
    /// Ringer Control Point
    CHAR_RINGER_CNTL_POINT_UUID                  = 0x2A40,
    /// Ringer Setting
    CHAR_RINGER_SETTING_UUID                     = 0x2A41,
    /// Alert Category ID Bit Mask
    CHAR_ALERT_CAT_ID_BIT_MASK_UUID              = 0x2A42,
    /// Alert Category ID
    CHAR_ALERT_CAT_ID_UUID                       = 0x2A43,
    /// Alert Notification Control Point
    CHAR_ALERT_NTF_CTNL_PT_UUID                  = 0x2A44,
    /// Unread Alert Status
    CHAR_UNREAD_ALERT_STATUS_UUID                = 0x2A45,
    /// New Alert
    CHAR_NEW_ALERT_UUID                          = 0x2A46,
    /// Supported New Alert Category
    CHAR_SUP_NEW_ALERT_CAT_UUID                  = 0x2A47,
    /// Supported Unread Alert Category
    CHAR_SUP_UNREAD_ALERT_CAT_UUID               = 0x2A48,
    /// Blood Pressure Feature
    CHAR_BLOOD_PRESSURE_FEATURE_UUID             = 0x2A49,
    /// HID Information
    CHAR_HID_INFO_UUID                           = 0x2A4A,
    /// Report Map
    CHAR_REPORT_MAP_UUID                         = 0x2A4B,
    /// HID Control Point
    CHAR_HID_CTNL_PT_UUID                        = 0x2A4C,
    /// Report
    CHAR_REPORT_UUID                             = 0x2A4D,
    /// Protocol Mode
    CHAR_PROTOCOL_MODE_UUID                      = 0x2A4E,
    /// Scan Interval Window
    CHAR_SCAN_INTV_WD_UUID                       = 0x2A4F,
    /// PnP ID
    CHAR_PNP_ID_UUID                             = 0x2A50,
    /// Glucose Feature
    CHAR_GLUCOSE_FEATURE_UUID                    = 0x2A51,
    /// Record access control point
    CHAR_REC_ACCESS_CTRL_PT_UUID                 = 0x2A52,
    /// RSC Measurement
    CHAR_RSC_MEAS_UUID                           = 0x2A53,
    /// RSC Feature
    CHAR_RSC_FEAT_UUID                           = 0x2A54,
    /// SC Control Point
    CHAR_SC_CNTL_PT_UUID                         = 0x2A55,
    /// CSC Measurement
    CHAR_CSC_MEAS_UUID                           = 0x2A5B,
    /// CSC Feature
    CHAR_CSC_FEAT_UUID                           = 0x2A5C,
    /// Sensor Location
    CHAR_SENSOR_LOC_UUID                         = 0x2A5D,
    /// CP Measurement
    CHAR_CP_MEAS_UUID                            = 0x2A63,
    /// CP Vector
    CHAR_CP_VECTOR_UUID                          = 0x2A64,
    /// CP Feature
    CHAR_CP_FEAT_UUID                            = 0x2A65,
    /// CP Control Point
    CHAR_CP_CNTL_PT_UUID                         = 0x2A66,
    /// Location and Speed
    CHAR_LOC_SPEED_UUID                          = 0x2A67,
    /// Navigation
    CHAR_NAVIGATION_UUID                         = 0x2A68,
    /// Position Quality
    CHAR_POS_QUALITY_UUID                        = 0x2A69,
    /// LN Feature
    CHAR_LN_FEAT_UUID                            = 0x2A6A,
    /// LN Control Point
    CHAR_LN_CNTL_PT_UUID                         = 0x2A6B,

    /// Weight Scale Feature
    CHAR_WS_FEAT_UUID                            = 0x2A9E,
    /// Weight Scale Measurement
    CHAR_WS_MEAS_UUID                            = 0x2A9D,
    /// User height
    CHAR_UDS_USER_HEIGHT_UUID                    = 0x2A8E,
    /// User age
    CHAR_UDS_USER_AGE_UUID                       = 0x2A80,
    /// User date of birth
    CHAR_UDS_USER_DATE_OF_BIRTH_UUID             = 0x2A85,
    /// User database change increment
    CHAR_UDS_USER_DB_CHANGE_INCR_UUID            = 0x2A99,
    /// User Data Index
    CHAR_UDS_USER_DATA_INDEX_UUID                = 0x2A9A,
    /// User Data Control Point
    CHAR_UDS_USER_CTRL_PT_UUID                   = 0x2A9F,
	/// Central Address Resolution
	CHAR_CENT_ADDR_RES_UUID                      = 0x2AA6,
    /// Last define
    LAST
};

/// Format for Characteristic Presentation
enum {
    /// unsigned 1-bit: true or false
    FORMAT_BOOL_UUID     = 0x01,
    /// unsigned 2-bit integer
    FORMAT_2BIT_UUID,
    /// unsigned 4-bit integer
    FORMAT_NIBBLE_UUID,
    /// unsigned 8-bit integer
    FORMAT_UINT8_UUID,
    /// unsigned 12-bit integer
    FORMAT_UINT12_UUID,
    /// unsigned 16-bit integer
    FORMAT_UINT16_UUID,
    /// unsigned 24-bit integer
    FORMAT_UINT24_UUID,
    /// unsigned 32-bit integer
    FORMAT_UINT32_UUID,
    /// unsigned 48-bit integer
    FORMAT_UINT48_UUID,
    /// unsigned 64-bit integer
    FORMAT_UINT64_UUID,
    /// unsigned 128-bit integer
    FORMAT_UINT128_UUID,
    /// signed 8-bit integer
    FORMAT_SINT8_UUID,
    /// signed 12-bit integer
    FORMAT_SINT12_UUID,
    /// signed 16-bit integer
    FORMAT_SINT16_UUID,
    /// signed 24-bit integer
    FORMAT_SINT24_UUID,
    /// signed 32-bit integer
    FORMAT_SINT32_UUID,
    /// signed 48-bit integer
    FORMAT_SINT48_UUID,
    /// signed 64-bit integer
    FORMAT_SINT64_UUID,
    /// signed 128-bit integer
    FORMAT_SINT128_UUID,
    /// IEEE-754 32-bit floating point
    FORMAT_FLOAT32_UUID,
    /// IEEE-754 64-bit floating point
    FORMAT_FLOAT64_UUID,
    /// IEEE-11073 16-bit SFLOAT
    FORMAT_SFLOAT_UUID,
    /// IEEE-11073 32-bit FLOAT
    FORMAT_FLOAT_UUID,
    /// IEEE-20601 format
    FORMAT_DUINT16_UUID,
    /// UTF-8 string
    FORMAT_UTF8S_UUID,
    /// UTF-16 string
    FORMAT_UTF16S_UUID,
    /// Opaque structure
    FORMAT_STRUCT_UUID,
    /// Last format
    FORMAT_LAST_UUID
};




