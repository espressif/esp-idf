/*
 * External password backend
 * Copyright (c) 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EXT_PASSWORD_H
#define EXT_PASSWORD_H

struct ext_password_data;

#ifdef CONFIG_EXT_PASSWORD

struct ext_password_data * ext_password_init(const char *backend,
					     const char *params);
void ext_password_deinit(struct ext_password_data *data);

struct wpabuf * ext_password_get(struct ext_password_data *data,
				 const char *name);
void ext_password_free(struct wpabuf *pw);

#else /* CONFIG_EXT_PASSWORD */

#define ext_password_init(b, p)
#define ext_password_deinit(d)
#define ext_password_get(d, n)
#define ext_password_free(p)

#endif /* CONFIG_EXT_PASSWORD */

#endif /* EXT_PASSWORD_H */
