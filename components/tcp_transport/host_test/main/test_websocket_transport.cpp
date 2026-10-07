/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <algorithm>
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

    // Provide actual implementations for crypto functions instead of using mocks
    int esp_crypto_sha1(const unsigned char *input, size_t ilen, unsigned char output[20]) {
        // Pre-calculated SHA1 of "x3JJHMbDL1EzLkh9GBhXDw==258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
        unsigned char expected_sha1[20] = {
            0x1D, 0x29, 0xAB, 0x73, 0x4B, 0x0C, 0x95, 0x85, 0x24, 0x00,
            0x69, 0xA6, 0xE4, 0xE3, 0xE9, 0x1B, 0x61, 0xDA, 0x19, 0x69
        };
        if (output) {
            memcpy(output, expected_sha1, 20);
        }
        return 0;
    }

    int esp_crypto_base64_encode(unsigned char *dst, size_t dlen, size_t *olen,
                                  const unsigned char *src, size_t slen) {
        // This function is called twice:
        // 1. To encode random 16 bytes -> client key "x3JJHMbDL1EzLkh9GBhXDw=="
        // 2. To encode SHA1 hash (20 bytes) -> expected accept key "HSmrc0sMlYUkAGmm5OPpG2HaGWk="

        if (slen == 16) {
            // First call: encoding random bytes to client key
            const char* client_key = "x3JJHMbDL1EzLkh9GBhXDw==";
            size_t key_len = strlen(client_key);
            if (dst && dlen > key_len) {
                memcpy(dst, client_key, key_len + 1);
                if (olen) {
                    *olen = key_len;
                }
            }
        } else if (slen == 20) {
            // Second call: encoding SHA1 hash to expected accept key
            const char* accept_key = "HSmrc0sMlYUkAGmm5OPpG2HaGWk=";
            size_t key_len = strlen(accept_key);
            if (dst && dlen > key_len) {
                memcpy(dst, accept_key, key_len + 1);
                if (olen) {
                    *olen = key_len;
                }
            }
        }
        return 0;
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
    // Expected server key calculated from client key "x3JJHMbDL1EzLkh9GBhXDw==" + magic string
    // SHA1("x3JJHMbDL1EzLkh9GBhXDw==258EAFA5-E914-47DA-95CA-C5AB0DC85B11") base64 encoded
    const char* expected_accept_key = "HSmrc0sMlYUkAGmm5OPpG2HaGWk=";
    int response_length = snprintf(response, WS_BUFFER_SIZE,
                                   "HTTP/1.1 101 Switching Protocols\r\n"
                                   "Upgrade: websocket\r\n"
                                   "Connection: Upgrade\r\n"
                                   "Sec-WebSocket-Accept: %s\r\n"
                                   "\r\n", expected_accept_key);
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

// Masking key used by make_masked_response(); none of its bytes collide with the
// 0x42 marker used by the overflow regression test.
constexpr std::array<unsigned char, 4> masked_response_key = {0x01, 0x02, 0x03, 0x04};

// Builds the handshake response followed by a *masked* WebSocket data frame.
// Servers must not mask frames (RFC 6455 5.1), so this simulates a malicious or
// misbehaving server; the client must still parse it without writing the 4-byte
// masking key past the caller's buffer.
std::string make_masked_response() {
    char response[WS_BUFFER_SIZE];
    const char* expected_accept_key = "HSmrc0sMlYUkAGmm5OPpG2HaGWk=";
    int response_length = snprintf(response, WS_BUFFER_SIZE,
                                   "HTTP/1.1 101 Switching Protocols\r\n"
                                   "Upgrade: websocket\r\n"
                                   "Connection: Upgrade\r\n"
                                   "Sec-WebSocket-Accept: %s\r\n"
                                   "\r\n", expected_accept_key);

    const char plain_payload[] = {'T', 'e', 's', 't'};
    // First byte: FIN + text opcode. Second byte: MASK bit set + payload length.
    unsigned char ws_frame_header[] = {0x81, static_cast<unsigned char>(0x80 | sizeof(plain_payload))};

    std::memcpy(response + response_length, ws_frame_header, sizeof(ws_frame_header));
    response_length += sizeof(ws_frame_header);

    std::memcpy(response + response_length, masked_response_key.data(), masked_response_key.size());
    response_length += masked_response_key.size();

    for (size_t i = 0; i < sizeof(plain_payload); i++) {
        response[response_length++] =
            static_cast<char>(static_cast<unsigned char>(plain_payload[i]) ^ masked_response_key[i % 4]);
    }
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

int mock_masked_read_callback(esp_transport_handle_t transport, char *buffer, int len, int timeout_ms, int num_call)
{
    std::string websocket_response = make_masked_response();
    std::memcpy(buffer, websocket_response.data(), websocket_response.size());
    return websocket_response.size();
}

// Callback function for mock_read
int mock_valid_read_fragmented_callback(esp_transport_handle_t t, char *buffer, int len, int timeout_ms, int num_call)
{
    static int offset = 0;
    static bool reset_on_next_call = false;

    // Reset offset when starting a new read sequence
    if (reset_on_next_call && buffer != nullptr) {
        offset = 0;
        reset_on_next_call = false;
    }

    std::string websocket_response = make_response();
    if (buffer == nullptr) {
        bool has_more = offset < websocket_response.size();
        if (!has_more) {
            reset_on_next_call = true;  // Prepare for next test
        }
        return has_more ? 1 : 0;
    }
    int read_size = 1;
    if (offset >= websocket_response.size()) {
        reset_on_next_call = true;
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

// Server byte stream (handshake response followed by raw WebSocket frames) served by mock_stream_read_callback
std::string s_server_stream;
size_t s_server_stream_offset = 0;
int s_pong_frames_written = 0;

void set_server_stream(const std::vector<uint8_t> &frames)
{
    s_server_stream = "HTTP/1.1 101 Switching Protocols\r\n"
                      "Upgrade: websocket\r\n"
                      "Connection: Upgrade\r\n"
                      "Sec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWk=\r\n"
                      "\r\n";
    s_server_stream.append(frames.begin(), frames.end());
    s_server_stream_offset = 0;
}

int mock_stream_read_callback(esp_transport_handle_t t, char *buffer, int len, int timeout_ms, int num_call)
{
    size_t to_copy = std::min(s_server_stream.size() - s_server_stream_offset, static_cast<size_t>(len));
    std::memcpy(buffer, s_server_stream.data() + s_server_stream_offset, to_copy);
    s_server_stream_offset += to_copy;
    return static_cast<int>(to_copy);
}

int mock_stream_poll_read_callback(esp_transport_handle_t t, int timeout_ms, int num_call)
{
    return s_server_stream_offset < s_server_stream.size() ? 1 : 0;
}

int mock_poll_write_ready_callback(esp_transport_handle_t t, int timeout_ms, int num_call)
{
    return 1;
}

int mock_write_count_pong_callback(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms, int num_call)
{
    if (len > 0 && static_cast<uint8_t>(buffer[0]) == 0x8A) { // FIN | PONG
        s_pong_frames_written++;
    }
    return len;
}

// Everything the client wrote to the parent transport, and how often it closed it
std::string s_client_bytes_written;
int s_close_calls = 0;

int mock_write_capture_callback(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms, int num_call)
{
    s_client_bytes_written.append(buffer, len);
    return len;
}

int mock_close_count_callback(esp_transport_handle_t t, int num_call)
{
    s_close_calls++;
    return 0;
}

// True if the client wrote a CLOSE frame (the handshake request is ASCII, so 0x88 can only be a CLOSE header)
bool close_frame_written()
{
    return s_client_bytes_written.find('\x88') != std::string::npos;
}

}

TEST_CASE("WebSocket Transport Connection", "[success]")
{
    // CMock state is global and test cases run in random order, so start each one from clean mocks
    Mockmock_transport_Init();
    Mocknetdb_Init();
    Mockesp_timer_Init();
    constexpr static auto timeout = 50;
    constexpr static auto port = 8080;
    constexpr static auto host = "localhost";
    mock_destroy_IgnoreAndReturn(ESP_OK);
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
        .header_hook = NULL,
        .header_user_context = NULL,
        .auth = nullptr,
        .response_headers = response_header_buffer.data(),
        .response_headers_len = response_header_len,
        .propagate_control_frames = false,
    };
    REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &ws_config) == ESP_OK);

    fmt::print("Attempting to connect to WebSocket\n");

    // Set the callback function for mock_write
    mock_write_Stub(mock_write_callback);
    mock_connect_ExpectAndReturn(parent_handle.get(), host, port, timeout, ESP_OK);

    SECTION("Happy flow") {
        // Set the callback function for mock_read
        mock_read_Stub(mock_valid_read_callback);
        mock_poll_read_Stub(mock_poll_read_callback);

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

        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        // Verify the response header was stored correctly
        std::string expected_header = "HTTP/1.1 101 Switching Protocols\r\n"
                                      "Upgrade: websocket\r\n"
                                      "Connection: Upgrade\r\n"
                                      "Sec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWk=\r\n"
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
        // Set the response header length to a size that's smaller than the full response
        // but still large enough to find the header delimiter
        ws_config.response_headers_len = 130;  // Large enough for the header but smaller than full response
        REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &ws_config) == ESP_OK);

        // Set the callback function for mock_read
        mock_read_Stub(mock_valid_read_callback);
        mock_poll_read_Stub(mock_poll_read_callback);

        // Create a marker to check that the value after the end of the response header buffer is not overwritten
        std::string expected_full_response = make_response();
        char marker = 0x42;  // Use a distinctive marker value
        response_header_buffer[ws_config.response_headers_len] = marker;

        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        // Verify the response header was stored correctly and truncated at the header boundary
        std::string expected_header = "HTTP/1.1 101 Switching Protocols\r\n"
                                      "Upgrade: websocket\r\n"
                                      "Connection: Upgrade\r\n"
                                      "Sec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWk=\r\n"
                                      "\r\n";

        REQUIRE(std::string(response_header_buffer.data()) == expected_header);
        // Verify the marker after the buffer wasn't overwritten
        REQUIRE(response_header_buffer[ws_config.response_headers_len] == marker);
    }

    SECTION("Poll read with buffered data") {
        // Set the callback function for mock_read
        mock_read_Stub(mock_valid_read_callback);

        static int parent_poll_calls = 0;
        parent_poll_calls = 0;

        // Verify poll_read is not called when buffer has data
        mock_poll_read_Stub([](esp_transport_handle_t t, int timeout_ms, int num_call){
            parent_poll_calls++;
            return 0;
        });

        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        // buffer should contain "Test" (4 bytes)
        // ws_poll_read should return 1 because buffer is not empty
        REQUIRE(esp_transport_poll_read(websocket_transport.get(), timeout) == 1);
        REQUIRE(parent_poll_calls == 0);

        // Read the data to empty the buffer
        char buffer[10];
        int read_len = esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout);
        REQUIRE(read_len == 4);

        // Now buffer is empty, ws_poll_read should call parent poll
        esp_transport_poll_read(websocket_transport.get(), timeout);
        REQUIRE(parent_poll_calls == 1);
    }

    SECTION("Masked frame does not overflow small caller buffer") {
        // A malicious server sends a masked frame. Reading it into a buffer smaller
        // than the 4-byte masking key must not write the mask past the buffer.
        mock_read_Stub(mock_masked_read_callback);
        mock_poll_read_Stub(mock_poll_read_callback);

        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        // Fill a buffer with a distinctive marker, then only allow reading 1 byte.
        // Before the fix, parsing the mask wrote 4 bytes into buffer[0..3], clobbering
        // the marker bytes past the 1-byte allowed region.
        constexpr int allowed_len = 1;
        std::array<char, 16> buffer;
        constexpr char marker = 0x42;
        buffer.fill(marker);

        int read_len = esp_transport_read(websocket_transport.get(), buffer.data(), allowed_len, timeout);
        REQUIRE(read_len == allowed_len);
        REQUIRE(buffer[0] == 'T');  // first payload byte, correctly unmasked

        // Every byte past the allowed region must be untouched.
        for (size_t i = allowed_len; i < buffer.size(); i++) {
            REQUIRE(buffer[i] == marker);
        }
    }

    SECTION("Masked server frame is correctly unmasked") {
        mock_read_Stub(mock_masked_read_callback);
        mock_poll_read_Stub(mock_poll_read_callback);

        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        char buffer[WS_BUFFER_SIZE];
        int read_len = esp_transport_read(websocket_transport.get(), buffer, WS_BUFFER_SIZE, timeout);
        REQUIRE(read_len == 4);

        std::string response(buffer, read_len);
        REQUIRE(response == "Test");
    }
}

TEST_CASE("WebSocket Transport Connection", "[failure]")
{
    Mockmock_transport_Init();
    Mocknetdb_Init();
    Mockesp_timer_Init();
    constexpr static auto timeout = 50;
    constexpr static auto port = 8080;
    constexpr static auto host = "localhost";
    mock_destroy_IgnoreAndReturn(ESP_OK);
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
        .header_hook = NULL,
        .header_user_context = NULL,
        .auth = nullptr,
        .response_headers = response_header_buffer.data(),
        .response_headers_len = response_header_len,
        .propagate_control_frames = false,
    };
    REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &ws_config) == ESP_OK);

    fmt::print("Attempting to connect to WebSocket\n");

    // Set the callback function for mock_write
    mock_write_Stub(mock_write_callback);
    mock_connect_ExpectAndReturn(parent_handle.get(), host, port, timeout, ESP_OK);

    SECTION("ws connect fails (0 len response)") {
        // Set the callback function for mock_read
        mock_read_Stub([](esp_transport_handle_t h, char *buf, int len, int tout, int n) {
            return 0;
        });
        mock_poll_read_Stub(mock_poll_read_callback);

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

        // check that the connect() function fails
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) != 0);

        // Verify the response header is empty
        REQUIRE(std::string(response_header_buffer.data()) == "");
    }

    SECTION("ws connect fails (buffer full, no delimiter)") {
        // Mock read to fill buffer with non-delimiter data
        mock_read_Stub([](esp_transport_handle_t h, char *buf, int len, int tout, int n) {
             if (len > 0) {
                 memset(buf, 'A', len);
             }
             return len;
        });
        mock_poll_read_Stub(mock_poll_read_callback);

        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == -1);
    }

    SECTION("ws connect succeeds (response header buffer too small for handshake verification)") {
        // Set a very small response header buffer
        ws_config.response_headers_len = 50;
        REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &ws_config) == ESP_OK);

        // Set the callback function for mock_read
        mock_read_Stub(mock_valid_read_callback);
        mock_poll_read_Stub(mock_poll_read_callback);

        // Connect should now succeed even with small user buffer
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);
    }
}

TEST_CASE("WebSocket Transport fragmented messages", "[fragmentation]")
{
    Mockmock_transport_Init();
    Mocknetdb_Init();
    Mockesp_timer_Init();
    constexpr static auto timeout = 50;
    constexpr static auto port = 8080;
    constexpr static auto host = "localhost";
    mock_destroy_IgnoreAndReturn(ESP_OK);
    unique_transport parent_handle{esp_transport_init(), esp_transport_destroy};
    REQUIRE(parent_handle);
    esp_transport_set_func(parent_handle.get(), mock_connect, mock_read, mock_write, mock_close, mock_poll_read, mock_poll_write, mock_destroy);

    unique_transport websocket_transport{esp_transport_ws_init(parent_handle.get()), esp_transport_destroy};
    REQUIRE(websocket_transport);

    esp_transport_ws_config_t ws_config = {
        .ws_path = "/",
        .sub_protocol = nullptr,
        .user_agent = nullptr,
        .headers = nullptr,
        .header_hook = NULL,
        .header_user_context = NULL,
        .auth = nullptr,
        .response_headers = nullptr,
        .response_headers_len = 0,
        .propagate_control_frames = false,
    };
    REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &ws_config) == ESP_OK);

    mock_write_Stub(mock_write_callback);
    mock_read_Stub(mock_stream_read_callback);
    mock_poll_read_Stub(mock_stream_poll_read_callback);
    mock_connect_ExpectAndReturn(parent_handle.get(), host, port, timeout, ESP_OK);

    char buffer[16];

    SECTION("Continuation frame completes a fragmented message") {
        set_server_stream({
            0x01, 0x03, 'a', 'b', 'c',  // TEXT, FIN=0
            0x80, 0x02, 'd', 'e',       // CONT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 3);
        REQUIRE(std::string(buffer, 3) == "abc");
        REQUIRE(esp_transport_ws_get_read_opcode(websocket_transport.get()) == WS_TRANSPORT_OPCODES_TEXT);
        REQUIRE_FALSE(esp_transport_ws_get_fin_flag(websocket_transport.get()));

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 2);
        REQUIRE(std::string(buffer, 2) == "de");
        REQUIRE(esp_transport_ws_get_read_opcode(websocket_transport.get()) == WS_TRANSPORT_OPCODES_CONT);
        REQUIRE(esp_transport_ws_get_fin_flag(websocket_transport.get()));
    }

    SECTION("Middle continuation frame keeps the fragmented message open") {
        set_server_stream({
            0x01, 0x01, 'a',            // TEXT, FIN=0
            0x00, 0x01, 'b',            // CONT, FIN=0
            0x80, 0x01, 'c',            // CONT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(buffer[0] == 'a');
        REQUIRE_FALSE(esp_transport_ws_get_fin_flag(websocket_transport.get()));

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(buffer[0] == 'b');
        REQUIRE(esp_transport_ws_get_read_opcode(websocket_transport.get()) == WS_TRANSPORT_OPCODES_CONT);
        REQUIRE_FALSE(esp_transport_ws_get_fin_flag(websocket_transport.get()));

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(buffer[0] == 'c');
        REQUIRE(esp_transport_ws_get_read_opcode(websocket_transport.get()) == WS_TRANSPORT_OPCODES_CONT);
        REQUIRE(esp_transport_ws_get_fin_flag(websocket_transport.get()));
    }

    SECTION("Control frame interleaved in a fragmented message is allowed") {
        mock_write_Stub(mock_write_count_pong_callback);
        mock_poll_write_Stub(mock_poll_write_ready_callback);
        s_pong_frames_written = 0;
        set_server_stream({
            0x01, 0x02, 'H', 'e',       // TEXT, FIN=0
            0x89, 0x00,                 // PING, FIN=1
            0x80, 0x01, 'y',            // CONT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 2);
        // PING is handled internally (PONG sent) and not reported to the reader
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 0);
        REQUIRE(s_pong_frames_written == 1);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(buffer[0] == 'y');
        REQUIRE(esp_transport_ws_get_read_opcode(websocket_transport.get()) == WS_TRANSPORT_OPCODES_CONT);
        REQUIRE(esp_transport_ws_get_fin_flag(websocket_transport.get()));
    }

    SECTION("Continuation frame without a fragmented message is rejected") {
        set_server_stream({
            0x80, 0x02, 'x', 'y',       // CONT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
    }

    SECTION("Continuation frame after the fragmented message completed is rejected") {
        set_server_stream({
            0x01, 0x01, 'a',            // TEXT, FIN=0
            0x80, 0x01, 'b',            // CONT, FIN=1
            0x80, 0x01, 'c',            // CONT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
    }

    SECTION("New data frame before the fragmented message completes is rejected") {
        set_server_stream({
            0x01, 0x01, 'a',            // TEXT, FIN=0
            0x81, 0x01, 'b',            // TEXT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
    }

    SECTION("Reconnect clears an unfinished fragmented message") {
        set_server_stream({
            0x01, 0x01, 'a',            // TEXT, FIN=0
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);

        set_server_stream({
            0x81, 0x01, 'b',            // TEXT, FIN=1
        });
        mock_connect_ExpectAndReturn(parent_handle.get(), host, port, timeout, ESP_OK);
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(buffer[0] == 'b');
    }
}

TEST_CASE("WebSocket Transport UTF-8 validation", "[utf8]")
{
    Mockmock_transport_Init();
    Mocknetdb_Init();
    Mockesp_timer_Init();
    constexpr static auto timeout = 50;
    constexpr static auto port = 8080;
    constexpr static auto host = "localhost";
    mock_destroy_IgnoreAndReturn(ESP_OK);
    unique_transport parent_handle{esp_transport_init(), esp_transport_destroy};
    REQUIRE(parent_handle);
    esp_transport_set_func(parent_handle.get(), mock_connect, mock_read, mock_write, mock_close, mock_poll_read, mock_poll_write, mock_destroy);

    unique_transport websocket_transport{esp_transport_ws_init(parent_handle.get()), esp_transport_destroy};
    REQUIRE(websocket_transport);

    esp_transport_ws_config_t ws_config = {
        .ws_path = "/",
        .sub_protocol = nullptr,
        .user_agent = nullptr,
        .headers = nullptr,
        .header_hook = NULL,
        .header_user_context = NULL,
        .auth = nullptr,
        .response_headers = nullptr,
        .response_headers_len = 0,
        .propagate_control_frames = false,
    };
    REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &ws_config) == ESP_OK);

    mock_write_Stub(mock_write_capture_callback);
    mock_read_Stub(mock_stream_read_callback);
    mock_poll_read_Stub(mock_stream_poll_read_callback);
    mock_poll_write_Stub(mock_poll_write_ready_callback);
    mock_close_Stub(mock_close_count_callback);
    mock_connect_ExpectAndReturn(parent_handle.get(), host, port, timeout, ESP_OK);
    s_client_bytes_written.clear();
    s_close_calls = 0;

    char buffer[16];

    SECTION("Valid ASCII text is accepted") {
        set_server_stream({
            0x81, 0x05, 'H', 'e', 'l', 'l', 'o',            // TEXT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 5);
        REQUIRE(std::string(buffer, 5) == "Hello");
    }

    SECTION("Valid 2-, 3- and 4-byte UTF-8 is accepted") {
        set_server_stream({
            0x81, 0x0D,                                     // TEXT, FIN=1
            0xC3, 0xA9,                                     // U+00E9
            0xE2, 0x82, 0xAC,                               // U+20AC
            0xF0, 0x9F, 0x98, 0x80,                         // U+1F600
            0xF4, 0x8F, 0xBF, 0xBF,                         // U+10FFFF
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 13);
    }

    SECTION("Code point split across reads of the same frame is accepted") {
        set_server_stream({
            0x81, 0x03, 0xE2, 0x82, 0xAC,                   // TEXT, FIN=1, "€"
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, 1, timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, 1, timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, 1, timeout) == 1);
    }

    SECTION("Code point split across continuation frames is accepted") {
        set_server_stream({
            0x01, 0x01, 0xE2,                               // TEXT, FIN=0
            0x00, 0x01, 0x82,                               // CONT, FIN=0
            0x80, 0x01, 0xAC,                               // CONT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
    }

    SECTION("Control frame inside a split code point does not disturb validation") {
        mock_write_Stub(mock_write_count_pong_callback);
        mock_poll_write_Stub(mock_poll_write_ready_callback);
        set_server_stream({
            0x01, 0x01, 0xC3,                               // TEXT, FIN=0
            0x89, 0x00,                                     // PING, FIN=1
            0x80, 0x01, 0xA9,                               // CONT, FIN=1, completes "é"
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 0);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
    }

    SECTION("Invalid sequence in a single frame is rejected") {
        set_server_stream({
            0x81, 0x05, 0xCE, 0xBA, 0xED, 0xA0, 0x80,       // TEXT, FIN=1, contains surrogate U+D800
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
    }

    SECTION("Invalid continuation frame fails after the valid first fragment") {
        set_server_stream({
            0x01, 0x02, 0xCE, 0xBA,                         // TEXT, FIN=0, valid
            0x00, 0x04, 0xF4, 0x90, 0x80, 0x80,             // CONT, FIN=0, U+110000 (out of range)
            0x80, 0x01, 'x',                                // CONT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 2);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
    }

    SECTION("Invalid byte is rejected in a later read of the same frame") {
        set_server_stream({
            0x81, 0x05, 0xCE, 0xBA, 0xF4, 0x90, 0x80,       // TEXT, FIN=1 (last byte unread after fail)
            0x81, 0x05, 'H', 'e', 'l', 'l', 'o',            // must not be delivered after the protocol error
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, 3, timeout) == 3);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, 1, timeout) < 0);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(s_close_calls == 1);
    }

    SECTION("Unread tail of a failed frame is never delivered") {
        set_server_stream({
            0x81, 0x05, 0xC3, 0x28, 0x81, 0x01, 0x41,       // TEXT, FIN=1; unread tail looks like TEXT "A"
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, 2, timeout) < 0);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
    }

    SECTION("Message ending inside a code point is rejected") {
        set_server_stream({
            0x81, 0x01, 0xCE,                               // TEXT, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
    }

    SECTION("Empty final frame ending inside a code point is rejected") {
        set_server_stream({
            0x01, 0x01, 0xCE,                               // TEXT, FIN=0
            0x80, 0x00,                                     // CONT, FIN=1, empty
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
    }

    SECTION("Invalid sequences are rejected") {
        const std::vector<std::vector<uint8_t>> payloads = {
            {0xC3, 0x28},                                   // invalid continuation byte
            {0xC0, 0xAF},                                   // overlong '/'
            {0xC1, 0xBF},                                   // overlong U+007F
            {0xE0, 0x80, 0xAF},                             // overlong '/'
            {0xF0, 0x80, 0x80, 0xAF},                       // overlong '/'
            {0xED, 0xA0, 0x80},                             // surrogate U+D800
            {0xED, 0xBF, 0xBF},                             // surrogate U+DFFF
            {0xF4, 0x90, 0x80, 0x80},                       // U+110000
            {0xF5, 0x80, 0x80, 0x80},                       // lead byte above U+10FFFF
            {0x80},                                         // lone continuation byte
            {0xFE},
            {0xFF},
        };
        for (size_t i = 0; i < payloads.size(); i++) {
            std::vector<uint8_t> frame = {0x81, static_cast<uint8_t>(payloads[i].size())};
            frame.insert(frame.end(), payloads[i].begin(), payloads[i].end());
            set_server_stream(frame);
            if (i > 0) {
                mock_connect_ExpectAndReturn(parent_handle.get(), host, port, timeout, ESP_OK);
            }
            REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

            REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        }
    }

    SECTION("Binary messages are not validated") {
        set_server_stream({
            0x82, 0x07, 0xFF, 0xFE, 0xC0, 0xAF, 0xED, 0xA0, 0x80,  // BINARY, FIN=1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 7);
        REQUIRE(!close_frame_written());
        REQUIRE(s_close_calls == 0);
    }

    SECTION("Invalid UTF-8 fails the connection: the transport is closed") {
        set_server_stream({
            0x81, 0x02, 0xC3, 0x28,                         // TEXT, FIN=1, invalid continuation byte
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(!close_frame_written());
        REQUIRE(s_close_calls == 1);
        REQUIRE(esp_transport_poll_read(websocket_transport.get(), timeout) < 0);
        REQUIRE(esp_transport_poll_write(websocket_transport.get(), timeout) < 0);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(esp_transport_write(websocket_transport.get(), "x", 1, timeout) < 0);
        REQUIRE(esp_transport_ws_send_raw(websocket_transport.get(), WS_TRANSPORT_OPCODES_TEXT, "x", 1, timeout) < 0);
        REQUIRE(s_close_calls == 1);                        // failed only once, and writes did not touch the parent
        REQUIRE(!close_frame_written());
    }

    SECTION("CLOSE frame with a valid UTF-8 reason is accepted") {
        set_server_stream({
            0x88, 0x05, 0x03, 0xE8, 0xC3, 0xA9, '!',        // CLOSE, code 1000, reason "\u00e9!"
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout);
        REQUIRE(close_frame_written());
        REQUIRE(static_cast<uint8_t>(s_client_bytes_written[s_client_bytes_written.size() - 6]) == 0x88);
    }

    SECTION("CLOSE frame with an invalid UTF-8 reason fails the connection") {
        set_server_stream({
            0x88, 0x04, 0x03, 0xE8, 0xC3, 0x28,             // CLOSE, code 1000, invalid reason
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(!close_frame_written());
        REQUIRE(s_close_calls == 1);
    }

    SECTION("Propagated CLOSE frame with a valid UTF-8 reason is delivered") {
        esp_transport_ws_config_t propagate = ws_config;
        propagate.propagate_control_frames = true;
        REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &propagate) == ESP_OK);
        set_server_stream({
            0x88, 0x05, 0x03, 0xE8, 0xC3, 0xA9, '!',        // CLOSE, code 1000, reason "\u00e9!"
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 5);
        REQUIRE(s_close_calls == 0);
    }

    SECTION("Propagated CLOSE frame with an invalid UTF-8 reason fails the connection") {
        esp_transport_ws_config_t propagate = ws_config;
        propagate.propagate_control_frames = true;
        REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &propagate) == ESP_OK);
        set_server_stream({
            0x88, 0x04, 0x03, 0xE8, 0xC3, 0x28,             // CLOSE, code 1000, invalid reason
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(!close_frame_written());
        REQUIRE(s_close_calls == 1);
        REQUIRE(esp_transport_write(websocket_transport.get(), "x", 1, timeout) < 0);
    }

    SECTION("CLOSE frame with a 1-byte payload fails the connection") {
        set_server_stream({
            0x88, 0x01, 0x03,                               // CLOSE, payload length 1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(!close_frame_written());
        REQUIRE(s_close_calls == 1);
    }

    SECTION("Propagated CLOSE frame with a 1-byte payload fails the connection") {
        esp_transport_ws_config_t propagate = ws_config;
        propagate.propagate_control_frames = true;
        REQUIRE(esp_transport_ws_set_config(websocket_transport.get(), &propagate) == ESP_OK);
        set_server_stream({
            0x88, 0x01, 0x03,                               // CLOSE, payload length 1
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(!close_frame_written());
        REQUIRE(s_close_calls == 1);
    }

    SECTION("CLOSE frame with a reason ending inside a code point fails the connection") {
        set_server_stream({
            0x88, 0x03, 0x03, 0xE8, 0xC3,                   // CLOSE, code 1000, truncated code point
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);

        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(!close_frame_written());
        REQUIRE(s_close_calls == 1);
    }

    SECTION("Reconnect after a failure clears the failed state") {
        set_server_stream({
            0x81, 0x02, 0xC3, 0x28,                         // TEXT, FIN=1, invalid
        });
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) < 0);
        REQUIRE(s_close_calls == 1);

        set_server_stream({
            0x81, 0x02, 'o', 'k',                           // TEXT, FIN=1, valid
        });
        mock_connect_ExpectAndReturn(parent_handle.get(), host, port, timeout, ESP_OK);
        REQUIRE(esp_transport_connect(websocket_transport.get(), host, port, timeout) == 0);
        REQUIRE(esp_transport_poll_read(websocket_transport.get(), timeout) == 1);
        REQUIRE(esp_transport_read(websocket_transport.get(), buffer, sizeof(buffer), timeout) == 2);
        REQUIRE(std::string(buffer, 2) == "ok");
    }
}
