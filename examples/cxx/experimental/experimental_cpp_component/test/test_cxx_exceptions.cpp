#include <stdio.h>
#include <cstring>
#include "unity.h"

#include "unity_cxx.hpp"
#include "esp_exception.hpp"

#ifdef __cpp_exceptions

using namespace std;
using namespace idf;

#define TAG "CXX Exception Test"

#if CONFIG_IDF_TARGET_ESP32
#define LEAKS "300"
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
#define LEAKS "800"
#else
#error "unknown target in CXX tests, can't set leaks threshold"
#endif

TEST_CASE("TEST_THROW catches exception", "[cxx exception][leaks=" LEAKS "]")
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

TEST_CASE("CHECK_THROW continues on ESP_OK", "[cxx exception][leaks=" LEAKS "]")
{
    esp_err_t error = ESP_OK;
    CHECK_THROW(error);
}

TEST_CASE("CHECK_THROW throws", "[cxx exception][leaks=" LEAKS "]")
{
    esp_err_t error = ESP_FAIL;
    TEST_THROW(CHECK_THROW(error), ESPException);
}

TEST_CASE("ESPException has working what() method", "[cxx exception][leaks=" LEAKS "]")
{
    try {
        throw ESPException(ESP_FAIL);
    } catch (ESPException &e) {
        TEST_ASSERT(strcmp(esp_err_to_name(ESP_FAIL), e.what()) == 0);
    }
}

#endif // __cpp_exceptions
