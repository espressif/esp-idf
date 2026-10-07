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

static const esp_stdio_driver_config_t cfg_a = {
    .vfs_ops = &mock_vfs_ops_a, .vfs_ctx = &mock_a, .path = "/",
};
static const esp_stdio_driver_config_t cfg_b = {
    .vfs_ops = &mock_vfs_ops_b, .vfs_ctx = &mock_b, .path = "/",
};

TEST_CASE("set_primary: write routed to the primary", "[esp_stdio]")
{
    esp_stdio_handle_t h, old;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_a, &h));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(h, &old));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
    TEST_ASSERT_EQUAL(1, mock_a.open_count);

    const char *msg = "hello";
    TEST_ASSERT_EQUAL((ssize_t)strlen(msg), write(fd, msg, strlen(msg)));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);
    TEST_ASSERT_EQUAL_STRING_LEN(msg, mock_a.write_buf, strlen(msg));

    TEST_ASSERT_EQUAL(0, close(fd));
    TEST_ASSERT_EQUAL(1, mock_a.close_count);

    /* Restore the previous primary, then remove the backend. */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(old, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(h));
}

TEST_CASE("primary read path", "[esp_stdio]")
{
    strcpy(mock_a.read_buf, "abc");
    mock_a.read_len = 3;

    esp_stdio_handle_t h, old;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_a, &h));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(h, &old));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    char buf[8] = {0};
    TEST_ASSERT_EQUAL(3, read(fd, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("abc", buf);
    TEST_ASSERT_EQUAL(1, mock_a.read_count);

    close(fd);
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(old, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(h));
}

TEST_CASE("auxiliary fan-out", "[esp_stdio]")
{
    esp_stdio_handle_t h;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_b, &h));
    /* stays as auxiliary (never promoted) */

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
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(h));
}

TEST_CASE("entry pool exhaustion", "[esp_stdio]")
{
    /* Pool size = CONFIG_ESP_STDIO_MAX_VFS_ENTRIES=3; default uses 1 -> 2 free */
    esp_stdio_handle_t h1, h2, h3;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_a, &h1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_b, &h2));
    TEST_ASSERT_EQUAL(ESP_ERR_NO_MEM, esp_stdio_register_driver(&cfg_a, &h3));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_EQUAL(1, write(fd, "x", 1));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);
    TEST_ASSERT_EQUAL(1, mock_b.write_count);
    close(fd);

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(h1));
    /* freed a slot */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_a, &h1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(h1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(h2));
}

TEST_CASE("invalid arguments", "[esp_stdio]")
{
    esp_stdio_handle_t h;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_register_driver(NULL, &h));

    esp_stdio_driver_config_t bad = { .vfs_ops = NULL, .path = "/" };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_register_driver(&bad, &h));

    bad.vfs_ops = &mock_vfs_ops_a;
    bad.path = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_register_driver(&bad, &h));

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_register_driver(&cfg_a, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_unregister_driver(NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_set_primary_driver(NULL, &h));
}

TEST_CASE("set_primary on an unregistered driver is rejected", "[esp_stdio]")
{
    esp_stdio_handle_t h;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_a, &h));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(h));

    /* The handle is stale after unregister; promoting it must be rejected. */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_stdio_set_primary_driver(h, NULL));
}

TEST_CASE("active primary cannot be unregistered", "[esp_stdio]")
{
    esp_stdio_handle_t h, old;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_a, &h));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(h, &old));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_EQUAL(1, write(fd, "z", 1));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);

    /* Removing the active primary is rejected. */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_stdio_unregister_driver(h));

    /* After restoring the previous primary, it becomes an auxiliary and can be
     * removed. */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(old, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(h));

    /* Console still works via the restored primary; the removed backend gets
     * no more traffic. */
    TEST_ASSERT_EQUAL(1, write(fd, "q", 1));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);
    close(fd);
}

TEST_CASE("install/uninstall are no-ops with user primary", "[esp_stdio]")
{
    esp_stdio_handle_t h, old;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_a, &h));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(h, &old));

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_install_io_driver());
    esp_stdio_uninstall_io_driver();
    TEST_ASSERT_EQUAL(0, mock_a.open_count);  /* driver not initialised by install */

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(old, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(h));
}

TEST_CASE("unregister one auxiliary keeps others working", "[esp_stdio]")
{
    esp_stdio_handle_t ha, hb;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_a, &ha));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_b, &hb));

    int fd = open("/dev/console", O_RDWR);
    TEST_ASSERT_EQUAL(2, write(fd, "hi", 2));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);
    TEST_ASSERT_EQUAL(1, mock_b.write_count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(ha));
    TEST_ASSERT_EQUAL(1, mock_a.close_count);

    TEST_ASSERT_EQUAL(2, write(fd, "yo", 2));
    TEST_ASSERT_EQUAL(1, mock_a.write_count);  /* A gone */
    TEST_ASSERT_EQUAL(2, mock_b.write_count);  /* B still there */

    close(fd);
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(hb));
}

TEST_CASE("set_primary swaps primaries and returns the old one", "[esp_stdio]")
{
    /* Register A and B, promote A then B; the demoted A stays as an auxiliary
     * and can be removed while B is the active primary. */
    esp_stdio_handle_t ha, hb, old_default, old_a;
    strcpy(mock_b.read_buf, "BBB");
    mock_b.read_len = 3;
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_a, &ha));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_register_driver(&cfg_b, &hb));

    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(ha, &old_default));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(hb, &old_a));
    TEST_ASSERT_EQUAL_PTR(ha, old_a);   /* set_primary returns the previous primary */

    int fd = open("/dev/console", O_RDWR);
    char buf[8] = {0};
    TEST_ASSERT_EQUAL(3, read(fd, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("BBB", buf);           /* B is the active primary */
    TEST_ASSERT_EQUAL(1, write(fd, "x", 1));
    TEST_ASSERT_EQUAL(1, mock_b.write_count);        /* B primary write */
    TEST_ASSERT_EQUAL(1, mock_a.write_count);        /* A still fans out as aux */

    /* Remove A (a demoted auxiliary) — B stays active. */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(ha));
    TEST_ASSERT_EQUAL(1, write(fd, "y", 1));
    TEST_ASSERT_EQUAL(2, mock_b.write_count);
    TEST_ASSERT_EQUAL(1, mock_a.write_count);        /* A gone, no more fan-out */
    close(fd);

    /* Restore the boot default and drop B. */
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_set_primary_driver(old_default, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_stdio_unregister_driver(hb));
}
