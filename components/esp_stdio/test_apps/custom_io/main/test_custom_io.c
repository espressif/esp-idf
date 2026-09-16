/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "unity.h"
#include "unity_test_runner.h"
#include "esp_stdio.h"
#include "mock_vfs.h"

static const esp_stdio_io_config_t cfg_a = {
    .vfs_ops = &mock_vfs_ops_a, .vfs_ctx = &mock_a, .path = "/",
};
static const esp_stdio_io_config_t cfg_b = {
    .vfs_ops = &mock_vfs_ops_b, .vfs_ctx = &mock_b, .path = "/",
};

TEST_CASE("register_io/push+pop primary: write and read routed correctly", "[esp_stdio]")
{
    esp_stdio_handle_t h;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_a, &h));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_push_primary(h));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    TEST_ASSERT_EQUAL(1, mock_a.open_count);

    const char *msg = "hello";
    TEST_ASSERT_EQUAL((ssize_t)strlen(msg), write(fd, msg, strlen(msg)));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);
    TEST_ASSERT_EQUAL_STRING_LEN(msg, mock_a.write_buf, strlen(msg));

    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ASSERT_EQUAL(1, mock_a.close_count);

    /* pop_primary: system primary (null VFS) takes over; mock_a receives no more traffic */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_pop_primary(NULL));

    fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    TEST_ASSERT_EQUAL(1, write(fd, "x", 1));   /* goes to null VFS + mock_a as aux */
    TEST_ASSERT_EQUAL(2, mock_a.write_count);  /* mock_a is now aux again */
    close(fd);

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(h));
}

TEST_CASE("primary read path", "[esp_stdio]")
{
    strcpy(mock_a.read_buf, "abc");
    mock_a.read_len = 3;

    esp_stdio_handle_t h;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_a, &h));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_push_primary(h));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    char buf[8] = {0};
    TEST_ASSERT_EQUAL(3, read(fd, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("abc", buf);
    TEST_ASSERT_EQUAL(1, mock_a.read_count);

    close(fd);
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_pop_primary(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(h));
}

TEST_CASE("auxiliary fan-out", "[esp_stdio]")
{
    esp_stdio_handle_t h;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_b, &h));
    /* stays as auxiliary */

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    TEST_ASSERT_EQUAL(1, mock_b.open_count);

    const char *msg = "fan";
    TEST_ASSERT_EQUAL((ssize_t)strlen(msg), write(fd, msg, strlen(msg)));
    TEST_ASSERT_EQUAL(1, mock_b.write_count);
    TEST_ASSERT_EQUAL_STRING_LEN(msg, mock_b.write_buf, strlen(msg));

    TEST_ASSERT_EQUAL(0, fsync(fd));
    TEST_ASSERT_EQUAL(1, mock_b.fsync_count);

    close(fd);
    TEST_ASSERT_EQUAL(1, mock_b.close_count);
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(h));
}

TEST_CASE("entry pool exhaustion", "[esp_stdio]")
{
    /* Pool size = CONFIG_ESP_STDIO_MAX_VFS_ENTRIES=3; system uses 1 -> 2 free */
    esp_stdio_handle_t h1, h2, h3;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_a, &h1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_b, &h2));
    TEST_ASSERT_EQUAL(ESP_ERR_NO_MEM, esp_stdio_register_io(&cfg_a, &h3));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_EQUAL(1, write(fd, "x", 1));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);
    TEST_ASSERT_EQUAL(1, mock_b.write_count);
    close(fd);

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(h1));
    /* freed a slot */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_a, &h1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(h1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(h2));
}

TEST_CASE("invalid arguments", "[esp_stdio]")
{
    esp_stdio_handle_t h;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_register_io(NULL, &h));

    esp_stdio_io_config_t bad = { .vfs_ops = NULL, .path = "/" };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_register_io(&bad, &h));

    bad.vfs_ops = &mock_vfs_ops_a;
    bad.path = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_register_io(&bad, &h));

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_register_io(&cfg_a, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_push_primary(NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_unregister_io(NULL));
}

TEST_CASE("pop_primary with no user primary returns INVALID_STATE", "[esp_stdio]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_stdio_pop_primary(NULL));
}

TEST_CASE("push_primary reroutes writes, pop restores previous", "[esp_stdio]")
{
    esp_stdio_handle_t ha, hb;
    strcpy(mock_a.read_buf, "AAA");
    mock_a.read_len = 3;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_a, &ha));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_push_primary(ha));

    int fd = open("/dev/console", O_RDWR);
    char buf[8] = {0};
    TEST_ASSERT_EQUAL(3, read(fd, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("AAA", buf);
    TEST_ASSERT_EQUAL(2, write(fd, "a1", 2));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);
    close(fd);

    /* Push B on top — A drops back to aux */
    strcpy(mock_b.read_buf, "BBB");
    mock_b.read_len = 3;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_b, &hb));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_push_primary(hb));

    fd = open("/dev/console", O_RDWR);
    memset(buf, 0, sizeof(buf));
    TEST_ASSERT_EQUAL(3, read(fd, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("BBB", buf);
    TEST_ASSERT_EQUAL(2, write(fd, "b1", 2));
    TEST_ASSERT_EQUAL(1, mock_b.write_count);
    TEST_ASSERT_EQUAL(2, mock_a.write_count);  /* A is aux, gets fan-out */
    close(fd);

    /* Pop B — A is primary again */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_pop_primary(NULL));
    fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_EQUAL(1, write(fd, "y", 1));
    TEST_ASSERT_EQUAL(3, mock_a.write_count);  /* primary write */
    TEST_ASSERT_EQUAL(2, mock_b.write_count);  /* B now aux, gets fan-out */
    close(fd);

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_pop_primary(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(ha));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(hb));
}

TEST_CASE("unregister active primary succeeds (implicit pop)", "[esp_stdio]")
{
    esp_stdio_handle_t h;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_a, &h));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_push_primary(h));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_EQUAL(1, write(fd, "z", 1));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);

    /* Unregister while it is the active primary — must succeed */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(h));

    /* Console still works via the restored system primary */
    TEST_ASSERT_EQUAL(1, write(fd, "q", 1));
    close(fd);
}

TEST_CASE("install/uninstall are no-ops with user primary", "[esp_stdio]")
{
    esp_stdio_handle_t h;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_a, &h));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_push_primary(h));

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_install_io_driver());
    esp_stdio_uninstall_io_driver();
    TEST_ASSERT_EQUAL(0, mock_a.open_count);  /* driver not initialised by install */

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_pop_primary(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(h));
}

TEST_CASE("unregister one auxiliary keeps others working", "[esp_stdio]")
{
    esp_stdio_handle_t ha, hb;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_a, &ha));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_b, &hb));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_EQUAL(2, write(fd, "hi", 2));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);
    TEST_ASSERT_EQUAL(1, mock_b.write_count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(ha));
    TEST_ASSERT_EQUAL(1, mock_a.close_count);

    TEST_ASSERT_EQUAL(2, write(fd, "yo", 2));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);  /* A gone */
    TEST_ASSERT_EQUAL(2, mock_b.write_count);  /* B still there */

    close(fd);
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(hb));
}

TEST_CASE("pop_primary(handle) removes a buried primary without disturbing the active one", "[esp_stdio]")
{
    /* Models two independent owners: A pushes its primary, then B pushes on
     * top. A releases its own (now buried) primary by handle — B must stay
     * active and A must drop back to being an auxiliary sink. */
    esp_stdio_handle_t ha, hb;
    strcpy(mock_b.read_buf, "BBB");
    mock_b.read_len = 3;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_a, &ha));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_push_primary(ha));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_io(&cfg_b, &hb));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_push_primary(hb));

    /* Remove the buried primary A (not the active top B) */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_pop_primary(ha));

    int fd = open("/dev/console", O_RDWR);
    char buf[8] = {0};
    TEST_ASSERT_EQUAL(3, read(fd, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("BBB", buf);          /* B is still the active primary */
    TEST_ASSERT_EQUAL(1, write(fd, "x", 1));
    TEST_ASSERT_EQUAL(1, mock_b.write_count);       /* B primary write */
    TEST_ASSERT_EQUAL(1, mock_a.write_count);       /* A demoted to aux, gets fan-out */
    close(fd);

    /* A is no longer on the stack — popping it again is INVALID_STATE */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_stdio_pop_primary(ha));

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_pop_primary(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(ha));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_io(hb));
}
