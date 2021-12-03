/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0
 *
 * This test code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "system_cxx.hpp"

// TODO: IDF-2693, function definition just to satisfy linker, mock esp_common instead
const char *esp_err_to_name(esp_err_t code) {
    return "test";
}

using namespace std;
using namespace idf;

TEST_CASE("Frequency invalid")
{
    CHECK_THROWS_AS(Frequency(0), ESPException&);
}

TEST_CASE("Frequency constructors correct")
{
    Frequency f0(440);
    CHECK(440 == f0.get_value());
    Frequency f1 = Frequency::Hz(440);
    CHECK(440 == f1.get_value());
    Frequency f2 = Frequency::KHz(440);
    CHECK(440000 == f2.get_value());
    Frequency f3 = Frequency::MHz(440);
    CHECK(440000000 == f3.get_value());
}

TEST_CASE("Frequency op ==")
{
    Frequency f0(440);
    Frequency f1(440);
    CHECK(f1 == f0);
}

TEST_CASE("Frequency op !=")
{
    Frequency f0(440);
    Frequency f1(441);
    CHECK(f1 != f0);
}

TEST_CASE("Frequency op >")
{
    Frequency f0(440);
    Frequency f1(441);
    Frequency f2(440);
    CHECK(f1 > f0);
    CHECK(!(f0 > f1));
    CHECK(!(f0 > f2));
}

TEST_CASE("Frequency op <")
{
    Frequency f0(440);
    Frequency f1(441);
    Frequency f2(440);
    CHECK(f0 < f1);
    CHECK(!(f1 < f0));
    CHECK(!(f0 < f2));
}

TEST_CASE("Frequency op >=")
{
    Frequency f0(440);
    Frequency f1(441);
    Frequency f2(440);
    CHECK  (f1 >= f0);
    CHECK(!(f0 >= f1));
    CHECK  (f0 >= f2);
}

TEST_CASE("Frequency op <=")
{
    Frequency f0(440);
    Frequency f1(441);
    Frequency f2(440);
    CHECK  (f0 <= f1);
    CHECK(!(f1 <= f0));
    CHECK  (f0 <= f2);
}
