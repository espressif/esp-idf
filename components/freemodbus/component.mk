INCLUDEDIRS := common/include
PRIV_INCLUDEDIRS := common port modbus modbus/ascii modbus/functions
PRIV_INCLUDEDIRS += modbus/rtu modbus/tcp modbus/include
PRIV_INCLUDEDIRS += serial_slave/port serial_slave/modbus_controller
PRIV_INCLUDEDIRS += serial_master/port serial_master/modbus_controller
PRIV_INCLUDEDIRS += tcp_slave/port tcp_slave/modbus_controller
PRIV_INCLUDEDIRS += tcp_master/port tcp_master/modbus_controller
SRCDIRS := common
SRCDIRS += modbus modbus/ascii modbus/functions modbus/rtu modbus/tcp
SRCDIRS += serial_slave/port serial_slave/modbus_controller
SRCDIRS += serial_master/port serial_master/modbus_controller
SRCDIRS += tcp_slave/port tcp_slave/modbus_controller
SRCDIRS += tcp_master/port tcp_master/modbus_controller
SRCDIRS += port

COMPONENT_PRIV_INCLUDEDIRS = $(addprefix freemodbus/, \
	$(PRIV_INCLUDEDIRS) \
	)

COMPONENT_SRCDIRS = $(addprefix freemodbus/, \
	$(SRCDIRS) \
	)

COMPONENT_ADD_INCLUDEDIRS = $(addprefix freemodbus/, \
	$(INCLUDEDIRS) \
	)
