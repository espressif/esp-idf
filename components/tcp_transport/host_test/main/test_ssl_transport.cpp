/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

extern "C" {
#include "Mockesp_tls.h"
#include "esp_transport.h"
#include "esp_transport_ssl.h"
}

using unique_transport = std::unique_ptr<std::remove_pointer_t<esp_transport_handle_t>, decltype(&esp_transport_destroy)>;

namespace {

esp_tls_client_session_t saved_session{};
esp_tls_client_session_t *session_to_save;
esp_tls_t *fake_tls = reinterpret_cast<esp_tls_t *>(&saved_session);
bool connect_called;
bool connect_config_present;
esp_tls_client_session_t *connected_session;
esp_tls_t *connected_tls;

int connect_callback(const char *, int, int, const esp_tls_cfg_t *cfg, esp_tls_t *tls, int)
{
    connect_called = true;
    connect_config_present = cfg != nullptr;
    connected_session = cfg ? cfg->client_session : nullptr;
    connected_tls = tls;
    return -1;
}

void expect_connect_with_client_session(esp_transport_handle_t transport, esp_tls_client_session_t *expected_session)
{
    connect_called = false;
    connect_config_present = false;
    connected_session = nullptr;
    connected_tls = nullptr;
    esp_tls_init_ExpectAndReturn(fake_tls);
    esp_tls_conn_new_sync_Stub(connect_callback);
    esp_tls_get_error_handle_ExpectAnyArgsAndReturn(ESP_FAIL);
    esp_tls_conn_destroy_ExpectAndReturn(fake_tls, 0);

    REQUIRE(esp_transport_connect(transport, "localhost", 443, 100) == -1);
    Mockesp_tls_Verify();
    REQUIRE(connect_called);
    REQUIRE(connect_config_present);
    REQUIRE(connected_session == expected_session);
    REQUIRE(connected_tls == fake_tls);
}

} // namespace

extern "C" esp_tls_client_session_t *esp_tls_get_client_session(esp_tls_t *)
{
    esp_tls_client_session_t *session = session_to_save;
    session_to_save = nullptr;
    return session;
}

extern "C" void esp_tls_free_client_session(esp_tls_client_session_t *)
{
}

TEST_CASE("TLS session ticket operations update the configured client session", "[tcp_transport][tls]")
{
    Mockesp_tls_Init();
    session_to_save = &saved_session;
    unique_transport transport{esp_transport_ssl_init(), esp_transport_destroy};
    REQUIRE(transport);

    REQUIRE(esp_transport_ssl_session_ticket_operation(transport.get(), ESP_TRANSPORT_SESSION_TICKET_SAVE) == ESP_OK);
    REQUIRE(esp_transport_ssl_session_ticket_operation(transport.get(), ESP_TRANSPORT_SESSION_TICKET_USE) == ESP_OK);
    esp_tls_client_session_t *expected_session = nullptr;

    SECTION("when the saved session is used") {
        expected_session = &saved_session;
    }

    SECTION("when saving the replacement session fails") {
        REQUIRE(esp_transport_ssl_session_ticket_operation(transport.get(), ESP_TRANSPORT_SESSION_TICKET_SAVE) == ESP_OK);
        SECTION("when attempting to use the missing replacement") {
            REQUIRE(esp_transport_ssl_session_ticket_operation(transport.get(), ESP_TRANSPORT_SESSION_TICKET_USE) == ESP_ERR_INVALID_STATE);
        }
        SECTION("without attempting to use the missing replacement") {
            // SAVE must clear the configured session without relying on USE.
        }
    }

    SECTION("when the saved session is freed") {
        REQUIRE(esp_transport_ssl_session_ticket_operation(transport.get(), ESP_TRANSPORT_SESSION_TICKET_FREE) == ESP_OK);
    }

    expect_connect_with_client_session(transport.get(), expected_session);
}
