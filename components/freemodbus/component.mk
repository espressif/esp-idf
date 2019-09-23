
COMPONENT_ADD_INCLUDEDIRS := common/include
COMPONENT_PRIV_INCLUDEDIRS := common port modbus modbus/ascii modbus/functions 
COMPONENT_PRIV_INCLUDEDIRS += modbus/rtu modbus/tcp modbus/include 
COMPONENT_PRIV_INCLUDEDIRS += serial_slave/port serial_slave/modbus_controller
COMPONENT_PRIV_INCLUDEDIRS += serial_master/port serial_master/modbus_controller
COMPONENT_SRCDIRS := common
COMPONENT_SRCDIRS += modbus modbus/ascii modbus/functions modbus/rtu modbus/tcp
COMPONENT_SRCDIRS += serial_slave/port 
COMPONENT_SRCDIRS += serial_slave/modbus_controller
COMPONENT_SRCDIRS += serial_master/port 
COMPONENT_SRCDIRS += serial_master/modbus_controller
COMPONENT_SRCDIRS += port
