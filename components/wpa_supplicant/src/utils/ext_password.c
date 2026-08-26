/*
 * External password backend
 * Copyright (c) 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "ext_password_i.h"

#ifdef CONFIG_EXT_PASSWORD_TEST
extern struct ext_password_backend ext_password_test;
#endif /* CONFIG_EXT_PASSWORD_TEST */

#ifdef CONFIG_EXT_PASSWORD
static const struct ext_password_backend *backends[] = {
#ifdef CONFIG_EXT_PASSWORD_TEST
	&ext_password_test,
#endif /* CONFIG_EXT_PASSWORD_TEST */
	NULL
};
#endif

struct ext_password_data {
	const struct ext_password_backend *backend;
	void *priv;
};

#ifdef CONFIG_EXT_PASSWORD
struct ext_password_data * ext_password_init(const char *backend,
					     const char *params)
{
	struct ext_password_data *data;
	int i;

	data = (struct ext_password_data *)os_zalloc(sizeof(*data));
	if (data == NULL)
		return NULL;

	for (i = 0; backends[i]; i++) {
		if (os_strcmp(backends[i]->name, backend) == 0) {
			data->backend = backends[i];
			break;
		}
	}

	if (!data->backend) {
		os_free(data);
		return NULL;
	}

	data->priv = data->backend->init(params);
	if (data->priv == NULL) {
		os_free(data);
		return NULL;
	}

	return data;
}


void ext_password_deinit(struct ext_password_data *data)
{
	if (data && data->backend && data->priv)
		data->backend->deinit(data->priv);
	os_free(data);
}


struct wpabuf * ext_password_get(struct ext_password_data *data,
				 const char *name)
{
	if (data == NULL)
		return NULL;
	return data->backend->get(data->priv, name);
}
#endif /* CONFIG_EXT_PASSWORD */

struct wpabuf * ext_password_alloc(size_t len)
{
	struct wpabuf *buf;

	buf = wpabuf_alloc(len);
	if (buf == NULL)
		return NULL;

#ifdef __linux__
	if (mlock(wpabuf_head(buf), wpabuf_len(buf)) < 0) {
		wpa_printf(MSG_ERROR, "EXT PW: mlock failed: %s",
			   strerror(errno));
	}
#endif /* __linux__ */

	return buf;
}

#ifdef CONFIG_EXT_PASSWORD
void ext_password_free(struct wpabuf *pw)
{
	if (pw == NULL)
		return;
	os_memset(wpabuf_mhead(pw), 0, wpabuf_len(pw));
#ifdef __linux__
	if (munlock(wpabuf_head(pw), wpabuf_len(pw)) < 0) {
		wpa_printf(MSG_ERROR, "EXT PW: munlock failed: %s",
			   strerror(errno));
	}
#endif /* __linux__ */
	wpabuf_free(pw);
}
#endif /* CONFIG_EXT_PASSWORD */
