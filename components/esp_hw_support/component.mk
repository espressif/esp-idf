COMPONENT_SRCDIRS := . port/$(IDF_TARGET)
COMPONENT_ADD_INCLUDEDIRS := . include port/$(IDF_TARGET)/private_include

port/$(IDF_TARGET)/rtc_clk.o: CFLAGS += -fno-jump-tables -fno-tree-switch-conversion
