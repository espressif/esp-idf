# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set(COMPONENT_SRCS "modbus/ascii/mbascii.c"
                   "modbus/rtu/mbcrc.c"
                   "modbus/functions/mbfunccoils.c"
                   "modbus/functions/mbfuncdiag.c"
                   "modbus/functions/mbfuncdisc.c"
                   "modbus/functions/mbfuncholding.c"
                   "modbus/functions/mbfuncinput.c"
                   "modbus/functions/mbfuncother.c"
                   "modbus/rtu/mbrtu.c"
                   "modbus/tcp/mbtcp.c"
                   "modbus/functions/mbutils.c"
                   "port/portevent.c"
                   "port/portother.c"
                   "port/portserial.c"
                   "port/porttimer.c"
                   "modbus_controller/mbcontroller.c"
                   "modbus/mb.c")
                
set(COMPONENT_ADD_INCLUDEDIRS modbus/include modbus_controller)
set(COMPONENT_PRIV_INCLUDEDIRS modbus port modbus/ascii modbus/functions modbus/rtu modbus/include)
set(COMPONENT_REQUIRES "driver")

register_component()
