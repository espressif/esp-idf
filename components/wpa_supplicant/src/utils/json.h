/*
 * JavaScript Object Notation (JSON) parser (RFC7159)
 * Copyright (c) 2017, Qualcomm Atheros, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef JSON_H
#define JSON_H

struct json_token {
	enum json_type {
		JSON_VALUE,
		JSON_OBJECT,
		JSON_ARRAY,
		JSON_STRING,
		JSON_NUMBER,
		JSON_BOOLEAN,
		JSON_NULL,
	} type;
	enum json_parsing_state {
		JSON_EMPTY,
		JSON_STARTED,
		JSON_WAITING_VALUE,
		JSON_COMPLETED,
	} state;
	char *name;
	char *string;
	int number;
	struct json_token *parent, *child, *sibling;
};

void json_escape_string(char *txt, size_t maxlen, const char *data, size_t len);
struct json_token * json_parse(const char *data, size_t data_len);
void json_free(struct json_token *json);
struct json_token * json_get_member(struct json_token *json, const char *name);
struct wpabuf * json_get_member_base64url(struct json_token *json,
					  const char *name);
void json_print_tree(struct json_token *root, char *buf, size_t buflen);
void json_add_int(struct wpabuf *json, const char *name, int val);
void json_add_string(struct wpabuf *json, const char *name, const char *val);
int json_add_string_escape(struct wpabuf *json, const char *name,
			   const void *val, size_t len);
int json_add_base64url(struct wpabuf *json, const char *name, const void *val,
		       size_t len);
void json_start_object(struct wpabuf *json, const char *name);
void json_end_object(struct wpabuf *json);
void json_start_array(struct wpabuf *json, const char *name);
void json_end_array(struct wpabuf *json);
void json_value_sep(struct wpabuf *json);

#endif /* JSON_H */
