/*
 * SPDX-FileCopyrightText: 2015-2022 The Apache Software Foundation (ASF)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2019-2022 Espressif Systems (Shanghai) CO LTD
 */
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _QUEUE_H_
#define	_QUEUE_H_

/* The common BSD linked list queue macros are already defined here for ESP-IDF */
#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This file defines circular queues. The other types of data structures:
 * singly-linked lists, singly-linked tail queues, lists and tail queues
 * are used from sys/queue.h
 *
 * A singly-linked list is headed by a single forward pointer. The elements
 * are singly linked for minimum space and pointer manipulation overhead at
 * the expense of O(n) removal for arbitrary elements. New elements can be
 * added to the list after an existing element or at the head of the list.
 * Elements being removed from the head of the list should use the explicit
 * macro for this purpose for optimum efficiency. A singly-linked list may
 * only be traversed in the forward direction.  Singly-linked lists are ideal
 * for applications with large datasets and few or no removals or for
 * implementing a LIFO queue.
 *
 * A singly-linked tail queue is headed by a pair of pointers, one to the
 * head of the list and the other to the tail of the list. The elements are
 * singly linked for minimum space and pointer manipulation overhead at the
 * expense of O(n) removal for arbitrary elements. New elements can be added
 * to the list after an existing element, at the head of the list, or at the
 * end of the list. Elements being removed from the head of the tail queue
 * should use the explicit macro for this purpose for optimum efficiency.
 * A singly-linked tail queue may only be traversed in the forward direction.
 * Singly-linked tail queues are ideal for applications with large datasets
 * and few or no removals or for implementing a FIFO queue.
 *
 * A list is headed by a single forward pointer (or an array of forward
 * pointers for a hash table header). The elements are doubly linked
 * so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before
 * or after an existing element or at the head of the list. A list
 * may only be traversed in the forward direction.
 *
 * A tail queue is headed by a pair of pointers, one to the head of the
 * list and the other to the tail of the list. The elements are doubly
 * linked so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before or
 * after an existing element, at the head of the list, or at the end of
 * the list. A tail queue may be traversed in either direction.
 *
 * A circle queue is headed by a pair of pointers, one to the head of the
 * list and the other to the tail of the list. The elements are doubly
 * linked so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before or after
 * an existing element, at the head of the list, or at the end of the list.
 * A circle queue may be traversed in either direction, but has a more
 * complex end of list detection.
 *
 * For details on the use of these macros, see the queue(3) manual page.
 *
 *
 *                      SLIST   LIST    STAILQ  TAILQ   CIRCLEQ
 * _HEAD                +       +       +       +       +
 * _HEAD_INITIALIZER    +       +       +       +       +
 * _ENTRY               +       +       +       +       +
 * _INIT                +       +       +       +       +
 * _EMPTY               +       +       +       +       +
 * _FIRST               +       +       +       +       +
 * _NEXT                +       +       +       +       +
 * _PREV                -       -       -       +       +
 * _LAST                -       -       +       +       +
 * _FOREACH             +       +       +       +       +
 * _FOREACH_REVERSE     -       -       -       +       +
 * _INSERT_HEAD         +       +       +       +       +
 * _INSERT_BEFORE       -       +       -       +       +
 * _INSERT_AFTER        +       +       +       +       +
 * _INSERT_TAIL         -       -       +       +       +
 * _REMOVE_HEAD         +       -       +       -       -
 * _REMOVE              +       +       +       +       +
 *
 */

/*
 * Circular queue declarations.
 */
#define	CIRCLEQ_HEAD(name, type)					\
struct name {								\
	struct type *cqh_first;		/* first element */		\
	struct type *cqh_last;		/* last element */		\
}

#define	CIRCLEQ_HEAD_INITIALIZER(head)					\
	{ (void *)&(head), (void *)&(head) }

#define	CIRCLEQ_ENTRY(type)						\
struct {								\
	struct type *cqe_next;		/* next element */		\
	struct type *cqe_prev;		/* previous element */		\
}

/*
 * Circular queue functions.
 */
#define	CIRCLEQ_EMPTY(head)	((head)->cqh_first == (void *)(head))

#define	CIRCLEQ_FIRST(head)	((head)->cqh_first)

#define	CIRCLEQ_FOREACH(var, head, field)				\
	for ((var) = CIRCLEQ_FIRST((head));				\
	    (var) != (void *)(head) || ((var) = NULL);			\
	    (var) = CIRCLEQ_NEXT((var), field))

#define	CIRCLEQ_FOREACH_REVERSE(var, head, field)			\
	for ((var) = CIRCLEQ_LAST((head));				\
	    (var) != (void *)(head) || ((var) = NULL);			\
	    (var) = CIRCLEQ_PREV((var), field))

#define	CIRCLEQ_INIT(head) do {						\
	CIRCLEQ_FIRST((head)) = (void *)(head);				\
	CIRCLEQ_LAST((head)) = (void *)(head);				\
} while (0)

#define	CIRCLEQ_INSERT_AFTER(head, listelm, elm, field) do {		\
	CIRCLEQ_NEXT((elm), field) = CIRCLEQ_NEXT((listelm), field);	\
	CIRCLEQ_PREV((elm), field) = (listelm);				\
	if (CIRCLEQ_NEXT((listelm), field) == (void *)(head))		\
		CIRCLEQ_LAST((head)) = (elm);				\
	else								\
		CIRCLEQ_PREV(CIRCLEQ_NEXT((listelm), field), field) = (elm);\
	CIRCLEQ_NEXT((listelm), field) = (elm);				\
} while (0)

#define	CIRCLEQ_INSERT_BEFORE(head, listelm, elm, field) do {		\
	CIRCLEQ_NEXT((elm), field) = (listelm);				\
	CIRCLEQ_PREV((elm), field) = CIRCLEQ_PREV((listelm), field);	\
	if (CIRCLEQ_PREV((listelm), field) == (void *)(head))		\
		CIRCLEQ_FIRST((head)) = (elm);				\
	else								\
		CIRCLEQ_NEXT(CIRCLEQ_PREV((listelm), field), field) = (elm);\
	CIRCLEQ_PREV((listelm), field) = (elm);				\
} while (0)

#define	CIRCLEQ_INSERT_HEAD(head, elm, field) do {			\
	CIRCLEQ_NEXT((elm), field) = CIRCLEQ_FIRST((head));		\
	CIRCLEQ_PREV((elm), field) = (void *)(head);			\
	if (CIRCLEQ_LAST((head)) == (void *)(head))			\
		CIRCLEQ_LAST((head)) = (elm);				\
	else								\
		CIRCLEQ_PREV(CIRCLEQ_FIRST((head)), field) = (elm);	\
	CIRCLEQ_FIRST((head)) = (elm);					\
} while (0)

#define	CIRCLEQ_INSERT_TAIL(head, elm, field) do {			\
	CIRCLEQ_NEXT((elm), field) = (void *)(head);			\
	CIRCLEQ_PREV((elm), field) = CIRCLEQ_LAST((head));		\
	if (CIRCLEQ_FIRST((head)) == (void *)(head))			\
		CIRCLEQ_FIRST((head)) = (elm);				\
	else								\
		CIRCLEQ_NEXT(CIRCLEQ_LAST((head)), field) = (elm);	\
	CIRCLEQ_LAST((head)) = (elm);					\
} while (0)

#define	CIRCLEQ_LAST(head)	((head)->cqh_last)

#define	CIRCLEQ_NEXT(elm,field)	((elm)->field.cqe_next)

#define	CIRCLEQ_PREV(elm,field)	((elm)->field.cqe_prev)

#define	CIRCLEQ_REMOVE(head, elm, field) do {				\
	if (CIRCLEQ_NEXT((elm), field) == (void *)(head))		\
		CIRCLEQ_LAST((head)) = CIRCLEQ_PREV((elm), field);	\
	else								\
		CIRCLEQ_PREV(CIRCLEQ_NEXT((elm), field), field) =	\
		    CIRCLEQ_PREV((elm), field);				\
	if (CIRCLEQ_PREV((elm), field) == (void *)(head))		\
		CIRCLEQ_FIRST((head)) = CIRCLEQ_NEXT((elm), field);	\
	else								\
		CIRCLEQ_NEXT(CIRCLEQ_PREV((elm), field), field) =	\
		    CIRCLEQ_NEXT((elm), field);				\
} while (0)

#ifdef __cplusplus
}
#endif

#endif /* !_SYS_QUEUE_H_ */
