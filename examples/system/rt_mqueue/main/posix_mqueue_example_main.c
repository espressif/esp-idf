/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

static int s_counter = 0;

const unsigned int MSG_PRIO = 0;

static void *sender_function(void * arg)
{
    mqd_t *write_descr = (mqd_t*) arg;

    while (true) {
        printf("sending: %d\n", s_counter);
        int result = mq_send(*write_descr, (const char*) &s_counter, sizeof(s_counter), MSG_PRIO);
        if (result != 0) {
            perror("Sending failed");
            abort();
        }

        s_counter++;
        sleep(1);
    }

    return NULL;
}

static void *receiver_function(void * arg)
{
    mqd_t *read_descr = (mqd_t*) arg;

    while (true) {
        int msg;
        int result = mq_receive(*read_descr, (char*) &msg, sizeof(msg), NULL);
        if (result == -1) {
            perror("Sending failed");
            abort();
        }

        printf("received: %d\n", msg);
    }

    return NULL;
}

void app_main(void)
{
    mqd_t write_descr;
    mqd_t read_descr;
    pthread_t sender;
    pthread_t receiver;

    struct mq_attr configuration = {
        .mq_flags = 0, // ignored by mq_open
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(int),
        .mq_curmsgs = 0 // ignored by mq_open
    };

    write_descr = mq_open("/my_queue", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR , &configuration);
    if (write_descr == (mqd_t) -1) {
        perror("Creating message queue failed");
        abort();
    }

    read_descr = mq_open("/my_queue", O_RDONLY);
    if (read_descr == (mqd_t) -1) {
        perror("Opening message queue for reading failed");
        abort();
    }

    int result;
    result = pthread_create(&sender, NULL, sender_function, &write_descr);
    if (result != 0) {
        printf("Creating sender thread failed: %s", strerror(errno));
        abort();
    }

    result = pthread_create(&receiver, NULL, receiver_function, &read_descr);
    if (result != 0) {
        printf("Creating receiver thread failed: %s", strerror(errno));
        abort();
    }

    while (true) {
        sleep(1000);
    }
}
