// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <stdbool.h>
#include <esp_system.h>
#include <esp_http_client.h>

#include "unity.h"
#include "test_utils.h"

#define HOST  "httpbin.org"
#define USERNAME  "user"
#define PASSWORD  "challenge"

TEST_CASE("Test in common case: Only URL and hostname are specified.", "[ESP HTTP CLIENT]")
{
    esp_http_client_config_t config_incorrect = {0};

    test_case_uses_tcpip();

    esp_http_client_handle_t client = esp_http_client_init(&config_incorrect);
    TEST_ASSERT(client == NULL);

    esp_http_client_config_t config_with_url = {
        .url = "http://httpbin.org/get",
    };
    client = esp_http_client_init(&config_with_url);
    TEST_ASSERT(client != NULL);
    TEST_ASSERT(esp_http_client_cleanup(client) == ESP_OK);


    esp_http_client_config_t config_with_hostname_path = {
        .host = HOST,
        .path = "/get",
    };
    client = esp_http_client_init(&config_with_hostname_path);
    TEST_ASSERT(client != NULL);
    TEST_ASSERT(esp_http_client_cleanup(client) == ESP_OK);
}

TEST_CASE("Get username and password after initialization.", "[ESP HTTP CLIENT]")
{
    esp_http_client_config_t config_with_auth = {
        .host = HOST,
        .path = "/",
        .username = USERNAME,
        .password = PASSWORD
    };
    char *value = NULL;
    esp_http_client_handle_t client = esp_http_client_init(&config_with_auth);
    TEST_ASSERT_NOT_NULL(client);
    // Test with username
    esp_err_t r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(USERNAME, value);
    // Test with password
    value = NULL;
    r = esp_http_client_get_password(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(PASSWORD, value);
    esp_http_client_cleanup(client);
}

/**
 * Test case to test that, the esp_http_client_set_url won't drop username and password
 * when pass a path "/abc" for url.
 **/
TEST_CASE("Username is unmodified when we change to new path", "[ESP HTTP CLIENT]")
{
    esp_http_client_config_t config_with_auth = {
        .host = HOST,
        .path = "/",
        .username = USERNAME,
        .password = PASSWORD
    };
    char *value = NULL;
    esp_http_client_handle_t client = esp_http_client_init(&config_with_auth);
    TEST_ASSERT_NOT_NULL(client);
    esp_err_t r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(USERNAME, value);
    esp_http_client_set_url(client, "/something-else/");
    r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(USERNAME, value);
    esp_http_client_cleanup(client);
}

/**
 * Test case to test that, the esp_http_client_set_url do not reset the auth credentials
 * Explicit APIs esp_http_client_set_username and esp_http_client_set_password are used to change 
 * the auth credentials
 **/
TEST_CASE("Username and password will not reset if new absolute URL doesnot specify auth credentials.", "[ESP HTTP CLIENT]")
{
    esp_http_client_config_t config_with_auth = {
        .host = HOST,
        .path = "/",
        .username = USERNAME,
        .password = PASSWORD
    };
    char *value = NULL;
    esp_http_client_handle_t client = esp_http_client_init(&config_with_auth);
    TEST_ASSERT_NOT_NULL(client);
    esp_err_t r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(USERNAME, value);
    esp_http_client_set_url(client, "http://" HOST "/get");
    esp_http_client_set_username(client, value);
    // esp_http_client_set_username sets new username and thus invalidates the original one
    // which we still reference in the local variable `value` (better forget it)
    value = NULL;
    esp_http_client_set_password(client, (char *)USERNAME);  // Need to cast the string literal (argument is not a const char*)
    //checks if username is set or not
    r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    //If username is set then value should not be NULL
    TEST_ASSERT_NOT_NULL(value);
    //checks if password is set or not
    r = esp_http_client_get_password(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    //If password is set then value should not be NULL
    TEST_ASSERT_NOT_NULL(value);
    esp_http_client_cleanup(client);
}
