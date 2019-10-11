/*
 * External password backend - internal definitions
 * Copyright (c) 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EXT_PASSWORD_I_H
#define EXT_PASSWORD_I_H

#include "ext_password.h"

struct ext_password_backend {
	const char *name;
	void * (*init)(const char *params);
	void (*deinit)(void *ctx);
	struct wpabuf * (*get)(void *ctx, const char *name);
};

struct wpabuf * ext_password_alloc(size_t len);

#endif /* EXT_PASSWORD_I_H */
