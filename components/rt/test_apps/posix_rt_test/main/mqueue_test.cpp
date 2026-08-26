/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include "unity.h"
#include "mqueue.h"

// Note: implementation does not support mode argument
// Note: implementation does not support message priorities (msg_prio arguments)

struct MessageQueueFixture {
    MessageQueueFixture(const char *name = "/test_queue",
            long int maxmsg = 10,
            long int msgsize = sizeof(int),
            int oflag = O_CREAT | O_WRONLY)
            : name(name) {
        struct mq_attr conf = {
            .mq_flags = 0, // ignored by mq_open
            .mq_maxmsg = maxmsg,
            .mq_msgsize = msgsize,
            .mq_curmsgs = 0 // ignored by mq_open
        };
        mq = mq_open(name, oflag, S_IRUSR, &conf);
        TEST_ASSERT_NOT_EQUAL((mqd_t) -1, mq);
    }
    ~MessageQueueFixture() {
        TEST_ASSERT_EQUAL(0, mq_close(mq));
        TEST_ASSERT_EQUAL(0, mq_unlink(name));
    }
    const char *name;
    mqd_t mq;
};

TEST_CASE("open with invalid message queue name fails", "[mqueue]")
{
    struct mq_attr conf = {
        .mq_flags = 0, // ignored by mq_open
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(int),
        .mq_curmsgs = 0 // ignored by mq_open
    };

// 255 + 2 characters, excl. '\0'
#define TOO_LONG_NAME "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"  \
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"  \
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"  \
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \

    TEST_ASSERT_EQUAL((mqd_t) -1, mq_open("", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR , &conf));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_EQUAL((mqd_t) -1, mq_open("my_queue", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR , &conf));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_EQUAL((mqd_t) -1, mq_open(TOO_LONG_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR , &conf));
    TEST_ASSERT_EQUAL(EINVAL, errno);
}

TEST_CASE("name / is valid", "[mqueue]")
{
    mqd_t mq = mq_open("/",  O_CREAT | O_WRONLY, S_IRUSR, NULL);

    TEST_ASSERT_NOT_EQUAL((mqd_t) -1, mq);

    TEST_ASSERT_EQUAL(0, mq_close(mq));
    TEST_ASSERT_EQUAL(0, mq_unlink("/"));
}

TEST_CASE("open without create flag fails", "[mqueue]")
{
    TEST_ASSERT_EQUAL((mqd_t) -1, mq_open("/my_queue", O_WRONLY));
    TEST_ASSERT_EQUAL(ENOENT, errno);
}

TEST_CASE("open mq_maxmsg <= 0", "[mqueue]")
{
    struct mq_attr conf = {
        .mq_flags = 0, // ignored by mq_open
        .mq_maxmsg = 0,
        .mq_msgsize = sizeof(int),
        .mq_curmsgs = 0 // ignored by mq_open
    };

    TEST_ASSERT_EQUAL((mqd_t) -1, mq_open("/my_queue", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR , &conf));
    TEST_ASSERT_EQUAL(EINVAL, errno);

    conf.mq_maxmsg = -1;

    TEST_ASSERT_EQUAL((mqd_t) -1, mq_open("/my_queue", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR , &conf));
    TEST_ASSERT_EQUAL(EINVAL, errno);
}

TEST_CASE("open mq_msgsize <= 0", "[mqueue]")
{
    struct mq_attr conf = {
        .mq_flags = 0, // ignored by mq_open
        .mq_maxmsg = 10,
        .mq_msgsize = 0,
        .mq_curmsgs = 0 // ignored by mq_open
    };

    TEST_ASSERT_EQUAL((mqd_t) -1, mq_open("/my_queue", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR , &conf));
    TEST_ASSERT_EQUAL(EINVAL, errno);

    conf.mq_msgsize = -1;

    TEST_ASSERT_EQUAL((mqd_t) -1, mq_open("/my_queue", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR , &conf));
    TEST_ASSERT_EQUAL(EINVAL, errno);
}

TEST_CASE("creating already existing message queue fails", "[mqueue]")
{
    MessageQueueFixture mq_fix;

    TEST_ASSERT_EQUAL((mqd_t) -1, mq_open(mq_fix.name, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR , NULL));
    TEST_ASSERT_EQUAL(EEXIST, errno);
}

TEST_CASE("open with default flags", "[mqueue]")
{
    mqd_t mq = mq_open("/my_queue",  O_CREAT | O_WRONLY, S_IRUSR, NULL);
    TEST_ASSERT_NOT_EQUAL((mqd_t) -1, mq);

    struct mq_attr default_conf;
    TEST_ASSERT_EQUAL(0, mq_getattr(mq, &default_conf));
    TEST_ASSERT_EQUAL(10, default_conf.mq_maxmsg);
    TEST_ASSERT_EQUAL(128, default_conf.mq_msgsize);

    TEST_ASSERT_EQUAL(0, mq_close(mq));
    TEST_ASSERT_EQUAL(0, mq_unlink("/my_queue"));
}

TEST_CASE("open propagates attributes correctly", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 4, 8);

    struct mq_attr conf;
    TEST_ASSERT_EQUAL(0, mq_getattr(mq_fix.mq, &conf));
    TEST_ASSERT_EQUAL(4, conf.mq_maxmsg);
    TEST_ASSERT_EQUAL(8, conf.mq_msgsize);
}

TEST_CASE("message queue does not exceed queue size", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 1, sizeof(int));
    int to_send = 47;
    TEST_ASSERT_EQUAL(0, mq_send(mq_fix.mq, (const char*) &to_send, sizeof(to_send), 0));
    int to_send_2 = 48;
    int received = 0;

    const struct timespec zero_time = {0, 0};
    TEST_ASSERT_EQUAL(-1, mq_timedsend(mq_fix.mq, (const char*) &to_send_2, sizeof(to_send_2), 0, &zero_time));
    TEST_ASSERT_EQUAL(ETIMEDOUT, errno);

    TEST_ASSERT_EQUAL(4, mq_receive(mq_fix.mq, (char*) &received, sizeof(received), nullptr));
    TEST_ASSERT_EQUAL(47, received);
}

TEST_CASE("sending on invalid mq descriptor fails", "[mqueue]")
{
    int to_send = 47;
    TEST_ASSERT_EQUAL(-1, mq_send(nullptr, (char*) &to_send, sizeof(to_send), 0));
    TEST_ASSERT_EQUAL(EBADF, errno);
}

TEST_CASE("sending with invalid timeout fails", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 1, sizeof(int));
    int to_send = 47;
    struct timespec invalid_time = {0, 1'000'000'000};
    TEST_ASSERT_EQUAL(-1, mq_timedsend(mq_fix.mq, (char*) &to_send, sizeof(to_send), 0, &invalid_time));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    invalid_time.tv_nsec = -1;
    TEST_ASSERT_EQUAL(-1, mq_timedsend(mq_fix.mq, (char*) &to_send, sizeof(to_send), 0, &invalid_time));
    TEST_ASSERT_EQUAL(EINVAL, errno);
}

TEST_CASE("sending with invalid message size fails", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 1, sizeof(int));
    int to_send = 47;
    TEST_ASSERT_EQUAL(-1, mq_send(mq_fix.mq, (char*) &to_send, sizeof(to_send) + 1, 0));
    TEST_ASSERT_EQUAL(EMSGSIZE, errno);
}

TEST_CASE("sending does not exceed queue size", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 1, sizeof(int));
    int to_send = 47;
    TEST_ASSERT_EQUAL(0, mq_send(mq_fix.mq, (const char*) &to_send, sizeof(to_send), 0));
    int to_send_2 = 48;
    int received = 0;

    const struct timespec zero_time = {0, 0};
    TEST_ASSERT_EQUAL(-1, mq_timedsend(mq_fix.mq, (const char*) &to_send_2, sizeof(to_send_2), 0, &zero_time));
    TEST_ASSERT_EQUAL(ETIMEDOUT, errno);

    TEST_ASSERT_EQUAL(4, mq_receive(mq_fix.mq, (char*) &received, sizeof(received), nullptr));
    TEST_ASSERT_EQUAL(47, received);
}

TEST_CASE("nonblocking send works", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 1, sizeof(int), O_CREAT | O_WRONLY | O_NONBLOCK);
    int to_send = 47;
    int received = 0;
    TEST_ASSERT_EQUAL(0, mq_send(mq_fix.mq, (char*) &to_send, sizeof(to_send), 0));
    int to_send_2 = 48;
    const struct timespec zero_time = {0, 0};

    TEST_ASSERT_EQUAL(-1, mq_timedsend(mq_fix.mq, (char*) &to_send_2, sizeof(to_send_2), 0, &zero_time));
    TEST_ASSERT_EQUAL(EAGAIN, errno);
    TEST_ASSERT_EQUAL(-1, mq_send(mq_fix.mq, (char*) &to_send_2, sizeof(to_send_2), 0));
    TEST_ASSERT_EQUAL(EAGAIN, errno);

    TEST_ASSERT_EQUAL(4, mq_receive(mq_fix.mq, (char*) &received, sizeof(received), nullptr));
    TEST_ASSERT_EQUAL(47, received);
}

TEST_CASE("receiving on invalid mq descriptor fails", "[mqueue]")
{
    int received;
    TEST_ASSERT_EQUAL(-1, mq_receive(nullptr, (char*) &received, sizeof(received), nullptr));
    TEST_ASSERT_EQUAL(EBADF, errno);
}

TEST_CASE("receiving with invalid timeout fails", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 1, sizeof(int));
    int received;
    struct timespec invalid_time = {0, 1'000'000'000};
    TEST_ASSERT_EQUAL(-1, mq_timedreceive(mq_fix.mq, (char*) &received, sizeof(received), 0, &invalid_time));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    invalid_time.tv_nsec = -1;
    TEST_ASSERT_EQUAL(-1, mq_timedreceive(mq_fix.mq, (char*) &received, sizeof(received), 0, &invalid_time));
    TEST_ASSERT_EQUAL(EINVAL, errno);
}

TEST_CASE("receiving with invalid message size fails", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 1, sizeof(int));
    int received;
    TEST_ASSERT_EQUAL(-1, mq_send(mq_fix.mq, (char*) &received, sizeof(received) + 1, 0));
    TEST_ASSERT_EQUAL(EMSGSIZE, errno);
}

TEST_CASE("receive on empty message queue times out", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 1, sizeof(int));
    int received;
    const struct timespec zero_time = {0, 0};

    TEST_ASSERT_EQUAL(-1, mq_timedreceive(mq_fix.mq, (char*) &received, sizeof(received), 0, &zero_time));
    TEST_ASSERT_EQUAL(ETIMEDOUT, errno);
}

TEST_CASE("nonblocking receive works", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 1, sizeof(int), O_CREAT | O_WRONLY | O_NONBLOCK);
    int received;
    const struct timespec zero_time = {0, 0};

    TEST_ASSERT_EQUAL(-1, mq_timedreceive(mq_fix.mq, (char*) &received, sizeof(received), 0, &zero_time));
    TEST_ASSERT_EQUAL(EAGAIN, errno);
    TEST_ASSERT_EQUAL(-1, mq_receive(mq_fix.mq, (char*) &received, sizeof(received), nullptr));
    TEST_ASSERT_EQUAL(EAGAIN, errno);
}

TEST_CASE("queue sends and receives multiple messages", "[mqueue]")
{
    MessageQueueFixture mq_fix("/test", 2, sizeof(int), O_CREAT | O_WRONLY | O_NONBLOCK);
    int received;
    int to_send = 47;

    TEST_ASSERT_EQUAL(0, mq_send(mq_fix.mq, (char*) &to_send, sizeof(to_send), 0));
    to_send = 48;
    TEST_ASSERT_EQUAL(0, mq_send(mq_fix.mq, (char*) &to_send, sizeof(to_send), 0));

    TEST_ASSERT_EQUAL(4, mq_receive(mq_fix.mq, (char*) &received, sizeof(received), nullptr));
    TEST_ASSERT_EQUAL(47, received);
    TEST_ASSERT_EQUAL(4, mq_receive(mq_fix.mq, (char*) &received, sizeof(received), nullptr));
    TEST_ASSERT_EQUAL(48, received);

}
