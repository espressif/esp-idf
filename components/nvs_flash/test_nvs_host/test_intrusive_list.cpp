// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "catch.hpp"
#include <algorithm>
#include <cstring>
#include "intrusive_list.h"

struct TestNode : public intrusive_list_node<TestNode> {
    TestNode(const char* name_ = "", int num_ = 0) : num(num_)
    {
        strncpy(name, name_, sizeof(name) - 1);
        name[sizeof(name) - 1] = 0;
    }
    char name[32];
    int num;
};

typedef intrusive_list<TestNode> TestList;


TEST_CASE("can add items to the list", "[list]")
{
    TestList list;
    TestNode n1("one", 1);
    TestNode n2("two", 2);
    TestNode n3("three", 3);

    list.push_back(&n1);
    REQUIRE(list.begin()->num == 1);
    REQUIRE(list.front().num == 1);
    REQUIRE(list.back().num == 1);
    list.push_front(&n2);
    REQUIRE(list.begin()->num == 2);
    REQUIRE(list.front().num == 2);
    REQUIRE(list.back().num == 1);

    list.insert(list.begin(), &n3);
    REQUIRE(list.begin()->num == 3);
    REQUIRE(list.front().num == 3);
    REQUIRE(list.back().num == 1);


    auto second = ++list.begin();
    REQUIRE(second->num == 2);

    second++;
    REQUIRE(second->num == 1);
}

TEST_CASE("can iterate over items", "[list]")
{
    TestList list;

    TestNode n1("one", 1);
    TestNode n2("two", 2);
    TestNode n3("three", 3);


    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    int val = 1;
    for (auto it = std::begin(list); it != std::end(list); ++it) {
        REQUIRE(it->num == val);
        ++val;
    }
}

TEST_CASE("iterator's prefix and postfix increments and decrements behave as expected", "[list]")
{
    TestList list;

    TestNode n1("one", 1);
    TestNode n2("two", 2);
    TestNode n3("three", 3);


    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    auto it = std::begin(list);
    REQUIRE((++it)->num == 2);
    REQUIRE(it++->num == 2);
    REQUIRE((--it)->num == 2);
    REQUIRE(it--->num == 2);
}

TEST_CASE("can pop_front from the list", "[list]")
{
    TestList list;

    TestNode n1("one", 1);
    TestNode n2("two", 2);
    TestNode n3("three", 3);

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    list.pop_front();
    list.pop_front();
    list.pop_front();

    REQUIRE(std::begin(list) == std::end(list));
}


TEST_CASE("can erase first item in the list", "[list]")
{
    TestList list;
    TestNode n1("one", 1);
    TestNode n2("two", 2);
    TestNode n3("three", 3);

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    list.erase(std::begin(list));

    REQUIRE(list.front().num == 2);
    REQUIRE(list.back().num == 3);
}


TEST_CASE("can erase last item in the list", "[list]")
{
    TestList list;
    TestNode n1("one", 1);
    TestNode n2("two", 2);
    TestNode n3("three", 3);

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    list.erase(&list.back());

    REQUIRE(list.front().num == 1);
    REQUIRE(list.back().num == 2);
}

TEST_CASE("can erase item in the middle of the list", "[list]")
{
    TestList list;
    TestNode n1("one", 1);
    TestNode n2("two", 2);
    TestNode n3("three", 3);

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    list.erase(++std::begin(list));

    REQUIRE(list.front().num == 1);
    REQUIRE(list.back().num == 3);
}

TEST_CASE("can erase all items in the list", "[list]")
{
    TestList list;
    TestNode n1("one", 1);
    TestNode n2("two", 2);
    TestNode n3("three", 3);

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    list.erase(std::begin(list));
    list.erase(std::begin(list));
    list.erase(std::begin(list));
    REQUIRE(std::begin(list) == std::end(list));
}


TEST_CASE("can erase all items in the list using clear method", "[list]")
{
    TestList list;
    TestNode n1("one", 1);
    TestNode n2("two", 2);
    TestNode n3("three", 3);
    TestNode n4("four", 4);
    TestNode n5("five", 5);
    TestNode n6("six", 6);


    list.push_back(&n1);
    list.push_back(&n2);
    list.insert(++list.begin(), &n3);
    list.insert(++list.begin(), &n4);
    list.push_front(&n5);
    list.insert(list.begin(), &n6);

    list.clear();

    REQUIRE(std::begin(list) == std::end(list));
}
