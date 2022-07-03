/*
 * Event loop based on select() loop
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "includes.h"

#include "common.h"
#include "list.h"
#include "eloop.h"

struct eloop_timeout {
	struct dl_list list;
	struct os_reltime time;
	void *eloop_data;
	void *user_data;
	eloop_timeout_handler handler;
};

struct eloop_data {
	struct dl_list timeout;
	ETSTimer eloop_timer;
	bool eloop_started;
};

#define ELOOP_LOCK() os_mutex_lock(eloop_data_lock)
#define ELOOP_UNLOCK() os_mutex_unlock(eloop_data_lock)

static void *eloop_data_lock = NULL;

static struct eloop_data eloop;

int eloop_init(void)
{
	os_memset(&eloop, 0, sizeof(eloop));
	dl_list_init(&eloop.timeout);
	os_timer_disarm(&eloop.eloop_timer);
	os_timer_setfn(&eloop.eloop_timer, (ETSTimerFunc *)eloop_run, NULL);

	eloop_data_lock = os_recursive_mutex_create();

	if (!eloop_data_lock) {
		wpa_printf(MSG_ERROR, "failed to create eloop data loop");
		return -1;
	}
	eloop.eloop_started = true;

	return 0;
}

int eloop_register_timeout(unsigned int secs, unsigned int usecs,
			   eloop_timeout_handler handler,
			   void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *tmp;
	os_time_t now_sec;

	timeout = os_zalloc(sizeof(*timeout));
	if (timeout == NULL)
		return -1;
	if (os_get_reltime(&timeout->time) < 0) {
		os_free(timeout);
		return -1;
	}
	now_sec = timeout->time.sec;
	timeout->time.sec += secs;
	if (timeout->time.sec < now_sec)
		goto overflow;
	timeout->time.usec += usecs;
	while (timeout->time.usec >= 1000000) {
		timeout->time.sec++;
		timeout->time.usec -= 1000000;
	}
	if (timeout->time.sec < now_sec)
		goto overflow;
	timeout->eloop_data = eloop_data;
	timeout->user_data = user_data;
	timeout->handler = handler;

	/* Maintain timeouts in order of increasing time */
	dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
		if (os_reltime_before(&timeout->time, &tmp->time)) {
			ELOOP_LOCK();
			dl_list_add(tmp->list.prev, &timeout->list);
			ELOOP_UNLOCK();
			goto run;
		}
	}
	ELOOP_LOCK();
	dl_list_add_tail(&eloop.timeout, &timeout->list);
	ELOOP_UNLOCK();

run:
	os_timer_disarm(&eloop.eloop_timer);
	os_timer_arm(&eloop.eloop_timer, 0, 0);

	return 0;

overflow:
	/*
	 * Integer overflow - assume long enough timeout to be assumed
	 * to be infinite, i.e., the timeout would never happen.
	 */
	wpa_printf(MSG_DEBUG,
		   "ELOOP: Too long timeout (secs=%u usecs=%u) to ever happen - ignore it",
		   secs,usecs);
	os_free(timeout);
	return 0;
}


static void eloop_remove_timeout(struct eloop_timeout *timeout)
{
	ELOOP_LOCK();
	dl_list_del(&timeout->list);
	ELOOP_UNLOCK();
	os_free(timeout);
}


int eloop_cancel_timeout(eloop_timeout_handler handler,
			 void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *prev;
	int removed = 0;

	dl_list_for_each_safe(timeout, prev, &eloop.timeout,
			      struct eloop_timeout, list) {
		if (timeout->handler == handler &&
		    (timeout->eloop_data == eloop_data ||
		     eloop_data == ELOOP_ALL_CTX) &&
		    (timeout->user_data == user_data ||
		     user_data == ELOOP_ALL_CTX)) {
			eloop_remove_timeout(timeout);
			removed++;
		}
	}

	return removed;
}


int eloop_cancel_timeout_one(eloop_timeout_handler handler,
			     void *eloop_data, void *user_data,
			     struct os_reltime *remaining)
{
	struct eloop_timeout *timeout, *prev;
	int removed = 0;
	struct os_reltime now;

	os_get_reltime(&now);
	remaining->sec = remaining->usec = 0;

	dl_list_for_each_safe(timeout, prev, &eloop.timeout,
			      struct eloop_timeout, list) {
		if (timeout->handler == handler &&
		    (timeout->eloop_data == eloop_data) &&
		    (timeout->user_data == user_data)) {
			removed = 1;
			if (os_reltime_before(&now, &timeout->time))
				os_reltime_sub(&timeout->time, &now, remaining);
			eloop_remove_timeout(timeout);
			break;
		}
	}
	return removed;
}


int eloop_is_timeout_registered(eloop_timeout_handler handler,
				void *eloop_data, void *user_data)
{
	struct eloop_timeout *tmp;

	dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
		if (tmp->handler == handler &&
		    tmp->eloop_data == eloop_data &&
		    tmp->user_data == user_data)
			return 1;
	}

	return 0;
}


int eloop_deplete_timeout(unsigned int req_secs, unsigned int req_usecs,
			  eloop_timeout_handler handler, void *eloop_data,
			  void *user_data)
{
	struct os_reltime now, requested, remaining;
	struct eloop_timeout *tmp;

	dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
		if (tmp->handler == handler &&
		    tmp->eloop_data == eloop_data &&
		    tmp->user_data == user_data) {
			requested.sec = req_secs;
			requested.usec = req_usecs;
			os_get_reltime(&now);
			os_reltime_sub(&tmp->time, &now, &remaining);
			if (os_reltime_before(&requested, &remaining)) {
				eloop_cancel_timeout(handler, eloop_data,
						     user_data);
				eloop_register_timeout(requested.sec,
						       requested.usec,
						       handler, eloop_data,
						       user_data);
				return 1;
			}
			return 0;
		}
	}

	return -1;
}


int eloop_replenish_timeout(unsigned int req_secs, unsigned int req_usecs,
			    eloop_timeout_handler handler, void *eloop_data,
			    void *user_data)
{
	struct os_reltime now, requested, remaining;
	struct eloop_timeout *tmp;

	dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
		if (tmp->handler == handler &&
		    tmp->eloop_data == eloop_data &&
		    tmp->user_data == user_data) {
			requested.sec = req_secs;
			requested.usec = req_usecs;
			os_get_reltime(&now);
			os_reltime_sub(&tmp->time, &now, &remaining);
			if (os_reltime_before(&remaining, &requested)) {
				eloop_cancel_timeout(handler, eloop_data,
						     user_data);
				eloop_register_timeout(requested.sec,
						       requested.usec,
						       handler, eloop_data,
						       user_data);
				return 1;
			}
			return 0;
		}
	}

	return -1;
}

void eloop_run(void)
{
	struct os_reltime tv, now;

	while (!dl_list_empty(&eloop.timeout)) {
		struct eloop_timeout *timeout;

		timeout = dl_list_first(&eloop.timeout, struct eloop_timeout,
					list);
		if (timeout) {
			os_get_reltime(&now);
			if (os_reltime_before(&now, &timeout->time)) {
				/* we don't need to process it rn, do it later */
				uint32_t ms;
				os_reltime_sub(&timeout->time, &now, &tv);
				ms = tv.sec * 1000 + tv.usec / 1000;
				os_timer_disarm(&eloop.eloop_timer);
				os_timer_arm(&eloop.eloop_timer, ms, 0);
				goto out;
			}
		}

		/* check if some registered timeouts have occurred */
		timeout = dl_list_first(&eloop.timeout, struct eloop_timeout,
					list);
		if (timeout) {
			os_get_reltime(&now);
			if (!os_reltime_before(&now, &timeout->time)) {
				void *eloop_data = timeout->eloop_data;
				void *user_data = timeout->user_data;
				eloop_timeout_handler handler =
					timeout->handler;
				eloop_remove_timeout(timeout);
				handler(eloop_data, user_data);
			}
		}
	}
out:
	return;
}

void eloop_destroy(void)
{
	struct eloop_timeout *timeout, *prev;
	struct os_reltime now;

	if (!eloop.eloop_started) {
		return;
	}
	os_get_reltime(&now);
	dl_list_for_each_safe(timeout, prev, &eloop.timeout,
			      struct eloop_timeout, list) {
		int sec, usec;
		sec = timeout->time.sec - now.sec;
		usec = timeout->time.usec - now.usec;
		if (timeout->time.usec < now.usec) {
			sec--;
			usec += 1000000;
		}
		wpa_printf(MSG_INFO, "ELOOP: remaining timeout: %d.%06d "
			   "eloop_data=%p user_data=%p handler=%p",
			   sec, usec, timeout->eloop_data, timeout->user_data,
			   timeout->handler);
		eloop_remove_timeout(timeout);
	}
	if (eloop_data_lock) {
		os_semphr_delete(eloop_data_lock);
		eloop_data_lock = NULL;
	}
	os_timer_disarm(&eloop.eloop_timer);
	os_memset(&eloop, 0, sizeof(eloop));
}
