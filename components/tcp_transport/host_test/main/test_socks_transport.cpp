/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <array>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include "fmt/core.h"
#include "fmt/ranges.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include "esp_transport.h"
#include "esp_transport_socks_proxy.h"

extern "C" {
#include "Mockmock_transport.h"
#include "Mocknetdb.h"
#include "Mockesp_timer.h"

    uint16_t lwip_htons(uint16_t n)
    {
        return __builtin_bswap16(n);
    }
}

using unique_transport = std::unique_ptr<std::remove_pointer_t<esp_transport_handle_t>, decltype(&esp_transport_destroy)>;
using namespace std::literals;

namespace  {

/*
 * Makes possible to pass a capturing lambda as a callback
 */
decltype(auto) capture_lambda(auto callable)
{
    // make a static copy of the lambda to extend it's lifetime and avoid the capture.
    [[maybe_unused]]static auto call = callable;
    return []<typename... Args>(Args... args) {
        return call(args...);
    };
}

auto make_response(socks_transport_error_t response)
{
    return std::array<char, 8>({0x00, static_cast<char>(response), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
}
}

TEST_CASE("Initial", "[Initialization]")
{
    esp_transport_socks_proxy_config_t config{ .version = SOCKS4,
            .address = "test_socks4_proxy",
            .port = 1080};

    mock_destroy_IgnoreAndReturn(ESP_OK);
    unique_transport test_parent{esp_transport_init(), esp_transport_destroy};
    esp_transport_set_func(test_parent.get(), mock_connect, mock_read, mock_write, mock_close, mock_poll_read, mock_poll_write, mock_destroy);

    SECTION("Initialize with invalid parent transport") {
        esp_transport_handle_t parent_handle = nullptr;
        unique_transport socks_transport{esp_transport_socks_proxy_init(parent_handle, &config), esp_transport_destroy};
        REQUIRE(socks_transport == nullptr);
    }

    SECTION("Initialize with NULL config") {
        auto *socks_transport = esp_transport_socks_proxy_init(test_parent.get(), nullptr);
        REQUIRE(socks_transport == nullptr);
    }

    SECTION("Initialize with NULL address config") {
        config.address = nullptr;
        auto *socks_transport = esp_transport_socks_proxy_init(test_parent.get(), &config);
        REQUIRE(socks_transport == nullptr);
    }

    SECTION("Successful Initialization") {
        auto *socks_transport = esp_transport_socks_proxy_init(test_parent.get(), &config);
        REQUIRE(socks_transport != nullptr);
        esp_transport_destroy(socks_transport);
    }
}

TEST_CASE("Requests to Proxy", "[Requests]")
{
    constexpr auto timeout = 50;
    esp_transport_socks_proxy_config_t config{ .version = SOCKS4,
                                               .address = "test_socks4_proxy",
                                               .port = 1080};

    auto test_target = "test_target"sv;
    auto target_port = 80;
    unique_transport test_parent{esp_transport_init(), esp_transport_destroy};
    REQUIRE(test_parent);
    esp_transport_set_func(test_parent.get(), mock_connect, mock_read, mock_write, mock_close, mock_poll_read, mock_poll_write, mock_destroy);
    unique_transport socks_transport{esp_transport_socks_proxy_init(test_parent.get(), &config), esp_transport_destroy};

    mock_destroy_IgnoreAndReturn(ESP_OK);
    esp_timer_get_time_IgnoreAndReturn(0);

    SECTION("Failure to connect to proxy") {
        mock_connect_ExpectAndReturn(test_parent.get(), config.address, config.port, timeout, -1);
        REQUIRE(esp_transport_connect(socks_transport.get(), test_target.data(), target_port, timeout) == -1);
    }

    GIVEN("Proxy accepted the connection") {

        mock_connect_ExpectAndReturn(test_parent.get(), config.address, config.port, timeout, 0);
        auto expect_addr_info = [](std::string_view test_target, int return_value) {
            lwip_getaddrinfo_ExpectAndReturn(test_target.data(), nullptr, nullptr, nullptr, return_value);
            lwip_getaddrinfo_IgnoreArg_hints();
            lwip_getaddrinfo_IgnoreArg_res();
            struct addrinfo addr_info = {};
            struct sockaddr_in sockaddr = {};
            sockaddr.sin_addr.s_addr = 0x5a5a5a5a;
            lwip_freeaddrinfo_Ignore();
            return std::tuple{addr_info, sockaddr};
        };

        SECTION("Failure to resolve target") {
            auto [addr_info, sockaddr] = expect_addr_info(test_target, EAI_NONAME);
            addr_info.ai_addr = reinterpret_cast<struct sockaddr *>(&sockaddr);
            auto *p_addr_info = &addr_info;
            lwip_getaddrinfo_ReturnThruPtr_res(&p_addr_info);
            REQUIRE(esp_transport_connect(socks_transport.get(), "test_target", 8080, timeout) == -1);
            REQUIRE(errno == SOCKS_RESPONSE_TARGET_NOT_FOUND);
        }

        GIVEN("Success on target resolution") {
            auto [addr_info, sockaddr] = expect_addr_info(test_target, 0);
            addr_info.ai_addr = reinterpret_cast<struct sockaddr *>(&sockaddr);
            auto *p_addr_info = &addr_info;
            lwip_getaddrinfo_ReturnThruPtr_res(&p_addr_info);
            auto expected_request = std::array<char,9>{0x04, 0x01, 0x00, 0x50, 0x5a, 0x5a, 0x5a, 0x5a, 0x00 };
            mock_write_Stub(capture_lambda([&test_parent, expected_request, &timeout](esp_transport_handle_t transport, const char *request_sent, int len, int timeout_ms, [[maybe_unused]]int num_call) {
                using namespace Catch::Matchers;
                REQUIRE(transport == test_parent.get());
                REQUIRE(len == expected_request.size());
                REQUIRE(timeout_ms == timeout);
                REQUIRE(std::equal(request_sent,request_sent+len, std::begin(expected_request), std::end(expected_request)));
                return len;
            }));

            SECTION("Successful connection request") {

                auto proxy_response = make_response(SOCKS_RESPONSE_SUCCESS);


                mock_read_ExpectAndReturn(test_parent.get(), proxy_response.data(), proxy_response.size(), timeout, proxy_response.size());
                mock_read_IgnoreArg_buffer();
                mock_read_ReturnArrayThruPtr_buffer(proxy_response.data(), proxy_response.size());
                REQUIRE(esp_transport_connect(socks_transport.get(), test_target.data(), target_port, timeout) == 0);
            };

            SECTION("Proxy rejected request") {
                auto proxy_response = make_response(SOCKS_RESPONSE_REQUEST_REJECTED);

                mock_read_ExpectAndReturn(test_parent.get(), proxy_response.data(), proxy_response.size(), timeout, proxy_response.size());
                mock_read_IgnoreArg_buffer();
                mock_read_ReturnArrayThruPtr_buffer(proxy_response.data(), proxy_response.size());
                REQUIRE(esp_transport_connect(socks_transport.get(), test_target.data(), target_port, timeout) == -1);
                REQUIRE(errno == SOCKS_RESPONSE_REQUEST_REJECTED);
            }

            SECTION("Client not running identification protocol") {
                auto proxy_response = make_response(SOCKS_RESPONSE_NOT_RUNNING_IDENTD);

                mock_read_ExpectAndReturn(test_parent.get(), proxy_response.data(), proxy_response.size(), timeout, proxy_response.size());
                mock_read_IgnoreArg_buffer();
                mock_read_ReturnArrayThruPtr_buffer(proxy_response.data(), proxy_response.size());
                REQUIRE(esp_transport_connect(socks_transport.get(), test_target.data(), target_port, timeout) == -1);
                REQUIRE(errno == SOCKS_RESPONSE_NOT_RUNNING_IDENTD);
            }

        }

    }
}
