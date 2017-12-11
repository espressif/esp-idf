#
# Component Makefile
#

COMPONENT_SRCDIRS := .

COMPONENT_ADD_INCLUDEDIRS = include

COMPONENT_ADD_LDFLAGS = -lapp_trace

# do not produce gcov info for this module, it is used as transport for gcov
CFLAGS := $(subst --coverage,,$(CFLAGS))

ifdef CONFIG_SYSVIEW_ENABLE

COMPONENT_ADD_INCLUDEDIRS += \
	sys_view/Config \
	sys_view/SEGGER \
	sys_view/Sample/OS

COMPONENT_SRCDIRS += \
	gcov \
	sys_view/SEGGER \
	sys_view/Sample/OS \
	sys_view/Sample/Config \
	sys_view/esp32
else
COMPONENT_SRCDIRS += gcov
endif
