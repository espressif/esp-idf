#
# Component Makefile
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# Simpler condition than in CMakeLists.txt;
# We assume that FreeRTOS is always included into the build with GNU Make.
ifndef IS_BOOTLOADER_BUILD
COMPONENT_OBJEXCLUDE := log_noos.o
else
COMPONENT_OBJEXCLUDE := log_freertos.o
endif

COMPONENT_ADD_LDFRAGMENTS += linker.lf
