/*
 * EAP peer state machine functions (RFC 4137)
 * Copyright (c) 2004-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EAP_H
#define EAP_H

#include "wpa/defs.h"
#include "eap/eap_defs.h"

struct eap_sm;

struct eap_method_type {
	int vendor;
	u32 method;
};

const u8 * eap_get_eapKeyData(struct eap_sm *sm, size_t *len);

#endif /* EAP_H */
