#
# Component Makefile
#
COMPONENT_SRCDIRS := . $(IDF_TARGET)
COMPONENT_OBJEXCLUDE += spi_slave_hd.o dedic_gpio.o gdma.o usb_serial_jtag.o

COMPONENT_ADD_INCLUDEDIRS := include $(IDF_TARGET)/include $(IDF_TARGET)/include/driver

COMPONENT_PRIV_INCLUDEDIRS := include/driver

# uses C11 atomic feature
spi_master.o: CFLAGS += -std=gnu11
