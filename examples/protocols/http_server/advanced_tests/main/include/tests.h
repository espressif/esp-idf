#ifndef __HTTPD_TESTS_H__
#define __HTTPD_TESTS_H__

#include <esp_http_server.h>

extern httpd_handle_t   start_tests(void);
extern void              stop_tests(httpd_handle_t hd);

#endif // __HTTPD_TESTS_H__
