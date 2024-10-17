/*
 * Amazon FreeRTOS+POSIX V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-FileCopyrightText: 2018 Amazon.com, Inc. or its affiliates
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file mqueue.h
 * @brief Message queues.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/mqueue.h.html
 */

#pragma once

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Message queue descriptor.
 */
typedef void * mqd_t;

/**
 * @brief Message queue attributes.
 */
struct mq_attr
{
    long mq_flags;   /**< Message queue flags. */
    long mq_maxmsg;  /**< Maximum number of messages. */
    long mq_msgsize; /**< Maximum message size. */
    long mq_curmsgs; /**< Number of messages currently queued. */
};

/**
 * @brief Close a message queue.
 *
 * Please refer to http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_close.html for more details.
 */
int mq_close( mqd_t mqdes );

/**
 * @brief Get message queue attributes.
 *
 * Please refer to http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_getattr.html for more details.
 */
int mq_getattr( mqd_t mqdes,
                struct mq_attr * mqstat );

/**
 * @brief Open a message queue.
 *
 * Please refer to http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_open.html for more details.
 *
 * @note Supported name pattern: leading &lt;slash&gt; character in name is always required;
 * the maximum length (excluding null-terminator) of the name argument can be NAME_MAX + 2.
 * @note mode argument is not supported.
 * @note Supported oflags: O_RDWR, O_CREAT, O_EXCL, and O_NONBLOCK.
 */
mqd_t mq_open( const char * name,
               int oflag,
               ... );

/**
 * @brief Receive a message from a message queue.
 *
 * Please refer to http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_receive.html for more details.
 *
 * @note msg_prio argument is not supported. Messages are not checked for corruption.
 */
ssize_t mq_receive( mqd_t mqdes,
                    char * msg_ptr,
                    size_t msg_len,
                    unsigned int * msg_prio );

/**
 * @brief Send a message to a message queue.
 *
 * Please refer to http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_send.html for more details.
 *
 * @note msg_prio argument is not supported.
 */
int mq_send( mqd_t mqdes,
             const char * msg_ptr,
             size_t msg_len,
             unsigned msg_prio );

/**
 * @brief Receive a message from a message queue with timeout.
 *
 * Please refer to http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_timedreceive.html for more details.
 *
 * @note msg_prio argument is not supported. Messages are not checked for corruption.
 */
ssize_t mq_timedreceive( mqd_t mqdes,
                         char * msg_ptr,
                         size_t msg_len,
                         unsigned * msg_prio,
                         const struct timespec * abstime );

/**
 * @brief Send a message to a message queue with timeout.
 *
 * Please refer to http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_timedsend.html for more details.
 *
 * @note msg_prio argument is not supported.
 */
int mq_timedsend( mqd_t mqdes,
                  const char * msg_ptr,
                  size_t msg_len,
                  unsigned msg_prio,
                  const struct timespec * abstime );

/**
 * @brief Remove a message queue.
 *
 * Please refer to http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_unlink.html for more details.
 */
int mq_unlink( const char * name );

/* Added by Espressif - specified but not implemented functions, return ENOSYS */
int mq_notify( mqd_t, const struct sigevent * );
int mq_setattr( mqd_t, const struct mq_attr *, struct mq_attr * );

#ifdef __cplusplus
}
#endif
