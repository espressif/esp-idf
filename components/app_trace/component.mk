#
# Component Makefile
#

COMPONENT_SRCDIRS := .

COMPONENT_ADD_INCLUDEDIRS := include

COMPONENT_ADD_LDFLAGS := -lapp_trace 

ifdef CONFIG_SYSVIEW_ENABLE
#COMPONENT_EXTRA_INCLUDES := freertos

COMPONENT_ADD_INCLUDEDIRS += \
	sys_view/Config \
	sys_view/SEGGER \
	sys_view/Sample/OS

COMPONENT_SRCDIRS += \
	sys_view/SEGGER \
	sys_view/Sample/OS \
	sys_view/Sample/Config \
	sys_view/esp32
endif
