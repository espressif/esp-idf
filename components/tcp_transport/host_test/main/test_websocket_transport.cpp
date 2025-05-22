/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
#include <netdb.h>
#include "fmt/core.h"
#include "fmt/ranges.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ws.h"

#define WS_BUFFER_SIZE CONFIG_WS_BUFFER_SIZE

extern "C" {
#include "Mockmock_transport.h"
#include "Mocknetdb.h"
#include "Mockesp_timer.h"
#include "Mockesp_tls_crypto.h"

    int lwip_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen) {
        return 1;
    }

    ssize_t lwip_recv(int s, void *mem, size_t len, int flags) {
        return WS_BUFFER_SIZE;
    }

    ssize_t lwip_send(int s, const void *data, size_t size, int flags) {
        return size;
    }
}

using unique_transport = std::unique_ptr<std::remove_pointer_t<esp_transport_handle_t>, decltype(&esp_transport_destroy)>;

namespace {

std::string make_request() {
    const std::string sec_websocket_key = "x3JJHMbDL1EzLkh9GBhXDw=="; // Use a fixed key for testing
    std::string request;

    request.append("GET / HTTP/1.1\r\n");
    request.append("Connection: Upgrade\r\n");
    request.append("Host: localhost:8080\r\n");
    request.append("User-Agent: ESP32 Websocket Client\r\n");
    request.append("Upgrade: websocket\r\n");
    request.append("Sec-WebSocket-Version: 13\r\n");
    request.append("Sec-WebSocket-Key: ").append(sec_websocket_key).append("\r\n");
    request.append("\r\n");

    return request;
}

std::string make_response() {
    char response[WS_BUFFER_SIZE];
    int response_length = snprintf(response, WS_BUFFER_SIZE,
                                   "HTTP/1.1 101 Switching Protocols\r\n"
                                   "Upgrade: websocket\r\n"
                                   "Connection: Upgrade\r\n"
                                   "Sec-WebSocket-Accept:\r\n"
                                   "\r\n");
    // WebSocket frame header
    unsigned char ws_frame_header[] = {0x81, 0x04}; // First byte: FIN, RSV1-3, and opcode; Second byte: payload length
    unsigned char ws_payload[] = {'T', 'e', 's', 't'}; // Example payload

    // Append the WebSocket frame header to the HTTP response
    std::memcpy(response + response_length, ws_frame_header, sizeof(ws_frame_header));
    response_length += sizeof(ws_frame_header);

    // Append the payload to the HTTP response
    std::memcpy(response + response_length, ws_payload, sizeof(ws_payload));
    response_length += sizeof(ws_payload);
    return std::string(response, response_length);
}

// Callback function for mock_write
int mock_write_callback(esp_transport_handle_t transport, const char *request_sent, int len, int timeout_ms, int num_call) {
    // Assertions to validate the parameters
    assert(transport != nullptr); // Ensure the transport handle is not null
    assert(request_sent != nullptr); // Ensure the request_sent pointer is not null
    assert(len > 0); // Ensure the length of the request is greater than zero
    assert(timeout_ms > 0); // Ensure the timeout is greater than zero
    assert(num_call >= 0); // Ensure the number of calls is non-negative

    // Create a string from the request_sent data
    std::string sent_request(request_sent, len);

    // Generate the expected request
    std::string expected_request = make_request();

    return len;
}

// Callbacks for mocked poll_reed and read functions
int mock_poll_read_callback(esp_transport_handle_t t, int timeout_ms, int num_call)
{
    if (num_call) {
        return 0;
    }
    return 1;
}

int mock_valid_read_callback(esp_transport_handle_t transport, char *buffer, int len, int timeout_ms, int num_call)
{
    std::string websocket_response = make_response();
    std::memcpy(buffer, websocket_response.data(), websocket_response.size());
    return websocket_response.size();
}

// Callback function for mock_read
int mock_valid_read_fragmented_callback(esp_transport_handle_t t, char *buffer, int len, int timeout_ms, int num_call)
{
    static int offset = 0;
    std::string websocket_response = make_response();
    if (buffer == nullptr) {
        return offset == websocket_response.size() ? 0 : 1;
    }
    int read_size = 1;
    if (offset == websocket_response.size()) {
        return 0;
    }
    std::memcpy(buffer, websocket_response.data() + offset, read_size);
    offset += read_size;
    return read_size;
}

int mock_valid_poll_read_fragmented_callback(esp_transport_handle_t t, int timeout_ms, int num_call)
{
    return mock_valid_read_fragmented_callback(t, nullptr, 0, 0, 0);
}

}

TEST_CASE("WebSocket Transport Connection", "[success]")
{
    constexpr static auto timeout = 50;
    constexpr static auto port = 8080;
    constexpr static auto host = "localhost";
    // Initialize the parent handle
    unique_transport parent_handle{esp_transport_init(), esp_transport_destroy};
    REQUIRE(parent_handle);

    // Set mock functions for parent handle
    esp_transport_set_func(parent_handle.get(), mock_connect, mock_read, mock_write, mock_close, mock_poll_read, mock_poll_write, mock_destroy);

    unique_transport websocket_transport{esp_transport_ws_init(parent_handle.get()), esp_transport_destroy};
    REQUIRE(websocket_transport);

    // Allocate buffer for response header
    constexpr size_t response_header_len = 1024;
    std::vector<char> response_header_buffer(response_header_len);
    esp_transport_ws_config_t ws_config = {
        .ws_path = "/",
        .sub_protocol = nullptr,
        .user_agent = nullptr,
        .headers = nullptr,
        .auth = nullptr,
        .response_headers = response_header_buffer.data(),
        .response_headers_len = response_header_len,
        .propagate_control_frames = false
    };
    REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &ws_config) == ESP_OK);

    fmt::print("Attempting to connect to WebSocket\n");
    esp_crypto_sha1_ExpectAnyArgsAndReturn(0);
    esp_crypto_base64_encode_ExpectAnyArgsAndReturn(0);

    // Set the callback function for mock_write
    mock_write_Stub(mock_write_callback);
    mock_connect_ExpectAndReturn(parent_handle.get(), host, port, timeout, ESP_OK);

    SECTION("Happy flow") {
        // Set the callback function for mock_read
        mock_read_Stub(mock_valid_read_callback);
        mock_poll_read_Stub(mock_poll_read_callback);
        esp_crypto_base64_encode_ExpectAnyArgsAndReturn(0);
        mock_destroy_ExpectAnyArgsAndReturn(ESP_OK);

        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        // Verify the response header was stored correctly
        std::string expected_header = make_response();
        REQUIRE(std::string(response_header_buffer.data()) == expected_header);

        char buffer[WS_BUFFER_SIZE];
        int read_len = 0;
        read_len = esp_transport_read(websocket_transport.get(), &buffer[read_len], WS_BUFFER_SIZE - read_len, timeout);

        fmt::print("Read result: {}\n", read_len);
        REQUIRE(read_len > 0);  // Ensure data is read

        std::string response(buffer, read_len);
        REQUIRE(response == "Test");
    }

    SECTION("Happy flow with fragmented reads byte by byte") {
        // Set the callback function for mock_read
        mock_read_Stub(mock_valid_read_fragmented_callback);
        mock_poll_read_Stub(mock_valid_poll_read_fragmented_callback);
        esp_crypto_base64_encode_ExpectAnyArgsAndReturn(0);
        mock_destroy_ExpectAnyArgsAndReturn(ESP_OK);

        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        // Verify the response header was stored correctly
        std::string expected_header = "HTTP/1.1 101 Switching Protocols\r\n"
                                      "Upgrade: websocket\r\n"
                                      "Connection: Upgrade\r\n"
                                      "Sec-WebSocket-Accept:\r\n"
                                      "\r\n";
        REQUIRE(std::string(response_header_buffer.data()) == expected_header);

        char buffer[WS_BUFFER_SIZE];
        int read_len = 0;
        int partial_read;
        while ((partial_read = esp_transport_read(websocket_transport.get(), &buffer[read_len], WS_BUFFER_SIZE - read_len, timeout)) > 0 ) {
            read_len+= partial_read;
        }
        fmt::print("Read result: {}\n", read_len);
        REQUIRE(read_len > 0);  // Ensure data is read

        std::string response(buffer, read_len);
        REQUIRE(response == "Test");
    }

    SECTION("Happy flow with smaller response header") {
        // Set the response header length to 10
        ws_config.response_headers_len = 10;
        REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &ws_config) == ESP_OK);

        // Set the callback function for mock_read
        mock_read_Stub(mock_valid_read_callback);
        mock_poll_read_Stub(mock_poll_read_callback);
        esp_crypto_base64_encode_ExpectAnyArgsAndReturn(0);
        mock_destroy_ExpectAnyArgsAndReturn(ESP_OK);

        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        // Verify the response header was stored correctly. it must contain only ten bytes and be null terminated
        std::string expected_header = "HTTP/1.1 1\0";

        REQUIRE(std::string(response_header_buffer.data()) == expected_header);
    }
}

TEST_CASE("WebSocket Transport Connection", "[failure]")
{
    constexpr static auto timeout = 50;
    constexpr static auto port = 8080;
    constexpr static auto host = "localhost";
    // Initialize the parent handle
    unique_transport parent_handle{esp_transport_init(), esp_transport_destroy};
    REQUIRE(parent_handle);

    // Set mock functions for parent handle
    esp_transport_set_func(parent_handle.get(), mock_connect, mock_read, mock_write, mock_close, mock_poll_read, mock_poll_write, mock_destroy);

    unique_transport websocket_transport{esp_transport_ws_init(parent_handle.get()), esp_transport_destroy};
    REQUIRE(websocket_transport);

    // Allocate buffer for response header
    constexpr size_t response_header_len = 1024;
    std::vector<char> response_header_buffer(response_header_len);
    esp_transport_ws_config_t ws_config = {
        .ws_path = "/",
        .sub_protocol = nullptr,
        .user_agent = nullptr,
        .headers = nullptr,
        .auth = nullptr,
        .response_headers = response_header_buffer.data(),
        .response_headers_len = response_header_len,
        .propagate_control_frames = false
    };
    REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &ws_config) == ESP_OK);

    fmt::print("Attempting to connect to WebSocket\n");
    esp_crypto_sha1_ExpectAnyArgsAndReturn(0);
    esp_crypto_base64_encode_ExpectAnyArgsAndReturn(0);

    // Set the callback function for mock_write
    mock_write_Stub(mock_write_callback);
    mock_connect_ExpectAndReturn(parent_handle.get(), host, port, timeout, ESP_OK);

    SECTION("ws connect fails (0 len response)") {
        // Set the callback function for mock_read
        mock_read_Stub([](esp_transport_handle_t h, char *buf, int len, int tout, int n) {
            return 0;
        });
        mock_poll_read_Stub(mock_poll_read_callback);
        esp_crypto_base64_encode_ExpectAnyArgsAndReturn(0);
        mock_destroy_ExpectAnyArgsAndReturn(ESP_OK);

        // check that the connect() function fails
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) != 0);

        // Verify the response header is empty
        REQUIRE(std::string(response_header_buffer.data()) == "");
    }

    SECTION("ws connect fails (invalid response)") {
        // Set the callback function for mock_read
        mock_read_Stub([](esp_transport_handle_t h, char *buf, int len, int tout, int n) {
            int resp_len = len / 2;
            std::memset(buf, '?', resp_len);
            return resp_len;
        });
        mock_poll_read_Stub(mock_poll_read_callback);
        esp_crypto_base64_encode_ExpectAnyArgsAndReturn(0);
        mock_destroy_ExpectAnyArgsAndReturn(ESP_OK);

        // check that the connect() function fails
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) != 0);

        // Verify the response header is empty
        REQUIRE(std::string(response_header_buffer.data()) == "");
    }

    SECTION("ws connect fails (big response)") {
        // Set the callback function for mock_read
        mock_read_Stub([](esp_transport_handle_t h, char *buf, int len, int tout, int n) {
            return WS_BUFFER_SIZE;
        });
        mock_poll_read_Stub(mock_poll_read_callback);
        esp_crypto_base64_encode_ExpectAnyArgsAndReturn(0);
        mock_destroy_ExpectAnyArgsAndReturn(ESP_OK);

        // check that the connect() function fails
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) != 0);

        // Verify the response header is empty
        REQUIRE(std::string(response_header_buffer.data()) == "");
    }
}
