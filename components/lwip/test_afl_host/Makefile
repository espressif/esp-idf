COMPONENTS_DIR=../..
CFLAGS=-std=gnu99 -Og -ggdb -ffunction-sections -fdata-sections -nostdlib -Wall  -Werror=all -Wno-int-to-pointer-cast -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wextra \
-Wno-unused-parameter -Wno-sign-compare -Wno-address   -Wno-unused-variable -DESP_PLATFORM -D IDF_VER=\"v3.1\" -MMD -MP -DWITH_POSIX
INC_DIRS=-I . -I $(COMPONENTS_DIR)/lwip/include/lwip -I $(COMPONENTS_DIR)/lwip/include/lwip/port -I $(COMPONENTS_DIR)/lwip/include/lwip/posix -I $(COMPONENTS_DIR)/lwip/apps/ping -I $(COMPONENTS_DIR)/app_trace/include -I $(COMPONENTS_DIR)/app_update/include -I $(COMPONENTS_DIR)/bootloader_support/include -I $(COMPONENTS_DIR)/bt/include -I $(COMPONENTS_DIR)/coap/port/include -I $(COMPONENTS_DIR)/coap/port/include/coap -I $(COMPONENTS_DIR)/coap/libcoap/include -I \ $(COMPONENTS_DIR)/coap/libcoap/include/coap -I $(COMPONENTS_DIR)/console -I $(COMPONENTS_DIR)/cxx/include -I $(COMPONENTS_DIR)/driver/include -I $(COMPONENTS_DIR)/esp-tls -I $(COMPONENTS_DIR)/esp32/include -I $(COMPONENTS_DIR)/esp_adc_cal/include -I $(COMPONENTS_DIR)/ethernet/include -I $(COMPONENTS_DIR)/expat/port/include -I $(COMPONENTS_DIR)/expat/include/expat -I $(COMPONENTS_DIR)/fatfs/src -I $(COMPONENTS_DIR)/freertos/include -I $(COMPONENTS_DIR)/heap/include -I \ $(COMPONENTS_DIR)/idf_test/include -I $(COMPONENTS_DIR)/jsmn/include -I $(COMPONENTS_DIR)/json/cJSON -I $(COMPONENTS_DIR)/libsodium/libsodium/src/libsodium/include -I $(COMPONENTS_DIR)/libsodium/port_include -I $(COMPONENTS_DIR)/log/include -I /home/david/esp/esp-idf/examples/wifi/simple_wifi/main/include -I $(COMPONENTS_DIR)/mbedtls/port/include -I $(COMPONENTS_DIR)/mbedtls/include -I $(COMPONENTS_DIR)/mdns/include -I $(COMPONENTS_DIR)/micro-ecc/micro-ecc -I \ $(COMPONENTS_DIR)/newlib/platform_include -I $(COMPONENTS_DIR)/newlib/include -I $(COMPONENTS_DIR)/nghttp/port/include -I $(COMPONENTS_DIR)/nghttp/nghttp2/lib/includes -I $(COMPONENTS_DIR)/nvs_flash/include -I $(COMPONENTS_DIR)/openssl/include -I $(COMPONENTS_DIR)/pthread/include -I $(COMPONENTS_DIR)/sdmmc/include -I $(COMPONENTS_DIR)/smartconfig/include -I $(COMPONENTS_DIR)/soc/esp32/include -I $(COMPONENTS_DIR)/soc/include -I $(COMPONENTS_DIR)/spi_flash/include -I \ $(COMPONENTS_DIR)/spiffs/include -I $(COMPONENTS_DIR)/tcpip_adapter/include -I $(COMPONENTS_DIR)/ulp/include -I $(COMPONENTS_DIR)/vfs/include -I $(COMPONENTS_DIR)/wear_levelling/include -I $(COMPONENTS_DIR)/wpa_supplicant/include -I $(COMPONENTS_DIR)/wpa_supplicant/port/include -I $(COMPONENTS_DIR)/esp32/include -I $(COMPONENTS_DIR)/xtensa-debug-module/include
TEST_NAME=test
FUZZ=afl-fuzz
LD=$(CC)
ifeq ($(MODE),dhcp_client)
    DEPENDENCY_INJECTION=-include dhcp_di.h
    OBJECTS=dhcp.o network_mock.o test_dhcp_client.o
    SAMPLE_PACKETS=in_dhcp_client
else ifeq ($(MODE),dhcp_server)
    DEPENDENCY_INJECTION=-include dhcpserver_di.h
    OBJECTS=dhcpserver.o test_dhcp_server.o network_mock.o
    SAMPLE_PACKETS=in_dhcp_server
else ifeq ($(MODE),dns)
    CFLAGS+=-DNOT_MOCK_DNS
    DEPENDENCY_INJECTION=-include dns_di.h
    OBJECTS=dns.o test_dns.o network_mock.o
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

dns.o: ../core/dns.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(DEPENDENCY_INJECTION) -c $< -o $@

dhcp.o: ../core/ipv4/dhcp.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(DEPENDENCY_INJECTION) -c $< -o $@

dhcpserver.o: ../apps/dhcpserver.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(DEPENDENCY_INJECTION) -c $< -o $@

%.o: %.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(TEST_NAME): $(OBJECTS)
	@echo "[LD] $@"
	@$(LD)  $(OBJECTS) -o $@ $(LDLIBS)

fuzz: $(TEST_NAME)
	@$(FUZZ) -t 500 -i "$(SAMPLE_PACKETS)" -o "out" -- ./$(TEST_NAME)
