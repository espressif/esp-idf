#
# Main Makefile. This is basically the same as a component makefile.
#

ifeq ($(GCC_NOT_5_2_0), 1)
hid_device_le_prf.o: CFLAGS += -Wno-unused-const-variable 
endif