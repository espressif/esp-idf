LWIP_COMPONENT_DIR=../
FREERTOS_COMPONENT_DIR=$(LWIP_COMPONENT_DIR)/../freertos
COMPONENT_DIR=$(LWIP_COMPONENT_DIR)/../
MOCKS_DIR=$(LWIP_COMPONENT_DIR)/../../tools/mocks

CFLAGS=-D IDF_VER=\"v3.1\" \
		-DESP_PLATFORM \
		-DLWIP_NO_CTYPE_H=1 \
		-DWITH_POSIX \
		-fdata-sections \
		-ffunction-sections \
		-ggdb \
		-MMD \
		-MP \
		-nostdlib \
		-Og \
		-std=gnu99 \
		-Wall \
		-Werror=all \
		-Wextra \
		-Wno-address \
		-Wno-constant-conversion \
		-Wno-error=deprecated-declarations \
		-Wno-error=unused-function \
		-Wno-error=unused-variable \
		-Wno-incompatible-pointer-types \
		-Wno-incompatible-pointer-types-discards-qualifiers \
		-Wno-int-to-pointer-cast \
		-Wno-macro-redefined \
		-Wno-sign-compare \
		-Wno-typedef-redefinition \
		-Wno-unused-parameter \
		-Wno-unused-variable

INC_DIRS=-I . \
		-I ./build/config \
		-I $(LWIP_COMPONENT_DIR)/include/apps \
		-I $(LWIP_COMPONENT_DIR)/include/apps/ping \
		-I $(LWIP_COMPONENT_DIR)/include/apps/sntp \
		-I $(LWIP_COMPONENT_DIR)/lwip/src/include \
		-I $(LWIP_COMPONENT_DIR)/lwip/src/include/netif \
		-I $(LWIP_COMPONENT_DIR)/lwip/src/include/posix \
		-I $(LWIP_COMPONENT_DIR)/port/include \
		-I $(LWIP_COMPONENT_DIR)/port/linux/include \
		-I $(LWIP_COMPONENT_DIR)/port/freertos/include \
		-I $(FREERTOS_COMPONENT_DIR)/FreeRTOS-Kernel/include \
		-I $(FREERTOS_COMPONENT_DIR)/esp_additions/include/freertos \
		-I $(FREERTOS_COMPONENT_DIR)/FreeRTOS-Kernel/portable/linux/include \
		-I $(COMPONENT_DIR)/esp_hw_support/include \
		-I $(COMPONENT_DIR)/linux/include

TEST_NAME=test
FUZZ=afl-fuzz
GEN_CFG=generate_config
LD=$(CC)
ifeq ($(MODE),dhcp_client)
	DEPENDENCY_INJECTION=-include dhcp_di.h
	OBJECTS=dhcp.o def.o esp32_mock.o test_dhcp_client.o
	SAMPLE_PACKETS=in_dhcp_client
else ifeq ($(MODE),dhcp_server)
	DEPENDENCY_INJECTION=-include dhcpserver_di.h
	OBJECTS=dhcpserver.o def.o esp32_mock.o test_dhcp_server.o
	SAMPLE_PACKETS=in_dhcp_server
else ifeq ($(MODE),dns)
	CFLAGS+=-DNOT_MOCK_DNS
	DEPENDENCY_INJECTION=-include dns_di.h
	OBJECTS=dns.o def.o esp32_mock.o test_dns.o
	SAMPLE_PACKETS=in_dns
else
	$(error Please specify MODE: dhcp_server, dhcp_client, dns)
endif

ifeq ($(INSTR),off)
	CC=gcc
	CFLAGS+=-DINSTR_IS_OFF
	TEST_NAME=test_sim
else
	CC=afl-clang-fast
endif

CFLAGS+=$(INC_DIRS)

all: $(TEST_NAME)

def.o: ../lwip/src/core/def.c $(GEN_CFG)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -D BUILDING_DEF $(DEPENDENCY_INJECTION) -c $< -o $@

dns.o: ../lwip/src/core/dns.c $(GEN_CFG)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(DEPENDENCY_INJECTION) -c $< -o $@

dhcp.o: ../lwip/src/core/ipv4/dhcp.c $(GEN_CFG)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(DEPENDENCY_INJECTION) -c $< -o $@

dhcpserver.o: ../apps/dhcpserver/dhcpserver.c $(GEN_CFG)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(DEPENDENCY_INJECTION) -c $< -o $@

%.o: %.c $(GEN_CFG)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: $(GEN_CFG)
$(GEN_CFG):
# Run reconfiguration without potential AFL in PATHs
	PATH=$(subst $(AFL_PATH):,/:,$(PATH)) idf.py reconfigure

$(TEST_NAME): $(OBJECTS)
	@echo "[LD] $@"
	@$(LD)  $(OBJECTS) -o $@ $(LDLIBS)

fuzz: $(TEST_NAME)
	@$(FUZZ) -t 5000+ -i "$(SAMPLE_PACKETS)" -o "out" -- ./$(TEST_NAME)
