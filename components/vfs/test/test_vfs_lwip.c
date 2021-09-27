// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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

#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include "unity.h"
#include "test_utils.h"

TEST_CASE("fstat() sets st_mode to socket type", "[vfs][lwip]")
{
    test_case_uses_tcpip();
    int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct stat stat = { 0 };
    fstat(socket_fd, &stat);

    TEST_ASSERT_TRUE(S_ISSOCK(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISBLK(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISCHR(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISDIR(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISREG(stat.st_mode));
    TEST_ASSERT_FALSE(S_ISLNK(stat.st_mode));

    close(socket_fd);
}
