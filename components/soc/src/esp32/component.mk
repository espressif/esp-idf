COMPONENT_ADD_INCLUDEDIRS += src/$(SOC_NAME) src/$(SOC_NAME)/include
COMPONENT_SRCDIRS += src/$(SOC_NAME)

ifndef CONFIG_ETH_USE_ESP32_EMAC
    COMPONENT_OBJEXCLUDE += src/esp32/emac_hal.o
endif

src/esp32/rtc_clk.o: CFLAGS += -fno-jump-tables -fno-tree-switch-conversion