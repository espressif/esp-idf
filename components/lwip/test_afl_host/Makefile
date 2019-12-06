COMPONENTS_DIR=../..
CFLAGS=-std=gnu99 -Og -ggdb -ffunction-sections -fdata-sections -nostdlib -Wall  -Werror=all -Wno-int-to-pointer-cast -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wextra \
-Wno-unused-parameter -Wno-sign-compare -Wno-address   -Wno-unused-variable -DESP_PLATFORM -D IDF_VER=\"v3.1\" -MMD -MP -DWITH_POSIX -DLWIP_NO_CTYPE_H=1
INC_DIRS=-I . -I ./build/config -I $(COMPONENTS_DIR)/newlib/platform_include -I $(COMPONENTS_DIR)/newlib/include -I $(COMPONENTS_DIR)/driver/include -I $(COMPONENTS_DIR)/esp32/include -I $(COMPONENTS_DIR)/ethernet/include -I $(COMPONENTS_DIR)/freertos/include -I $(COMPONENTS_DIR)/heap/include -I $(COMPONENTS_DIR)/lwip/lwip/src/include  -I $(COMPONENTS_DIR)/lwip/include/apps -I $(COMPONENTS_DIR)/lwip/lwip/src/include/netif -I $(COMPONENTS_DIR)/lwip/lwip/src/include/posix -I $(COMPONENTS_DIR)/lwip/port/esp32/include -I $(COMPONENTS_DIR)/lwip/lwip/src/include/posix -I $(COMPONENTS_DIR)/lwip/include/apps/ping -I $(COMPONENTS_DIR)/lwip/include/apps/sntp  -I $(COMPONENTS_DIR)/soc/esp32/include -I $(COMPONENTS_DIR)/soc/include -I $(COMPONENTS_DIR)/tcpip_adapter/include -I $(COMPONENTS_DIR)/esp_rom/include  -I $(COMPONENTS_DIR)/esp_common/include -I $(COMPONENTS_DIR)/xtensa/include -I $(COMPONENTS_DIR)/xtensa/esp32/include -I $(COMPONENTS_DIR)/esp_wifi/include -I $(COMPONENTS_DIR)/esp_event/include
TEST_NAME=test
FUZZ=afl-fuzz
GEN_CFG=generate_config
LD=$(CC)
ifeq ($(MODE),dhcp_client)
    DEPENDENCY_INJECTION=-include dhcp_di.h
    OBJECTS=dhcp.o def.o network_mock.o test_dhcp_client.o
    SAMPLE_PACKETS=in_dhcp_client
else ifeq ($(MODE),dhcp_server)
    DEPENDENCY_INJECTION=-include dhcpserver_di.h
    OBJECTS=dhcpserver.o def.o test_dhcp_server.o network_mock.o
    SAMPLE_PACKETS=in_dhcp_server
else ifeq ($(MODE),dns)
    CFLAGS+=-DNOT_MOCK_DNS
    DEPENDENCY_INJECTION=-include dns_di.h
    OBJECTS=dns.o def.o test_dns.o network_mock.o
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
	$(IDF_PATH)/tools/idf.py reconfigure

$(TEST_NAME): $(OBJECTS)
	@echo "[LD] $@"
	@$(LD)  $(OBJECTS) -o $@ $(LDLIBS)

fuzz: $(TEST_NAME)
	@$(FUZZ) -t 5000+ -i "$(SAMPLE_PACKETS)" -o "out" -- ./$(TEST_NAME)
