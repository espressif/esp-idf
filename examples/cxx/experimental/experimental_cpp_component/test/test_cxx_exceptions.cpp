#include <stdio.h>
#include "unity.h"

#include "unity_cxx.hpp"
#include "esp_exception.hpp"

#ifdef __cpp_exceptions

using namespace std;
using namespace idf;

#define TAG "CXX Exception Test"

TEST_CASE("TEST_THROW catches exception", "[cxx exception]")
{
    TEST_THROW(throw ESPException(ESP_FAIL);, ESPException);
}

/* The following two test cases are expected to fail */

TEST_CASE("TEST_THROW asserts catching different exception", "[cxx exception][ignore]")
{
    TEST_THROW(throw std::exception();, ESPException);
}

TEST_CASE("TEST_THROW asserts not catching any exception", "[cxx exception][ignore]")
{
    TEST_THROW(printf(" ");, ESPException); // need statement with effect
}

TEST_CASE("CHECK_THROW continues on ESP_OK", "[cxx exception]")
{
    esp_err_t error = ESP_OK;
    CHECK_THROW(error);
}

TEST_CASE("CHECK_THROW throws", "[cxx exception]")
{
    esp_err_t error = ESP_FAIL;
    TEST_THROW(CHECK_THROW(error), ESPException);
}

#endif // __cpp_exceptions

