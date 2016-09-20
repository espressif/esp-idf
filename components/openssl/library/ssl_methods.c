#include "ssl_lib.h"
#include "ssl_methods.h"
#include "ssl_pm.h"

IMPLEMENT_TLS_METHOD_FUNC(TLS_method_func,
        ssl_pm_new, ssl_pm_free,
        ssl_pm_handshake, ssl_pm_shutdown, ssl_pm_clear,
        ssl_pm_read, ssl_pm_send, ssl_pm_pending,
        ssl_pm_set_fd, ssl_pm_get_fd,
        ssl_pm_set_bufflen,
        ssl_pm_get_state);

IMPLEMENT_TLS_METHOD(TLS_ANY_VERSION, 0, TLS_method_func, TLS_client_method);

IMPLEMENT_TLS_METHOD(TLS1_2_VERSION, 0, TLS_method_func, TLSv1_2_client_method);

IMPLEMENT_TLS_METHOD(TLS1_1_VERSION, 0, TLS_method_func, TLSv1_1_client_method);

IMPLEMENT_TLS_METHOD(TLS1_VERSION, 0, TLS_method_func, TLSv1_client_method);

IMPLEMENT_SSL_METHOD(SSL3_VERSION, 0, TLS_method_func, SSLv3_client_method);


IMPLEMENT_TLS_METHOD(TLS_ANY_VERSION, 1, TLS_method_func, TLS_server_method);

IMPLEMENT_TLS_METHOD(TLS1_1_VERSION, 1, TLS_method_func, TLSv1_1_server_method);

IMPLEMENT_TLS_METHOD(TLS1_2_VERSION, 1, TLS_method_func, TLSv1_2_server_method);

IMPLEMENT_TLS_METHOD(TLS1_VERSION, 0, TLS_method_func, TLSv1_server_method);

IMPLEMENT_SSL_METHOD(SSL3_VERSION, 1, TLS_method_func, SSLv3_server_method);


IMPLEMENT_TLS_METHOD(TLS_ANY_VERSION, -1, TLS_method_func, TLS_method);

IMPLEMENT_SSL_METHOD(TLS1_2_VERSION, -1, TLS_method_func, TLSv1_2_method);

IMPLEMENT_SSL_METHOD(TLS1_1_VERSION, -1, TLS_method_func, TLSv1_1_method);

IMPLEMENT_SSL_METHOD(TLS1_VERSION, -1, TLS_method_func, TLSv1_method);

IMPLEMENT_SSL_METHOD(SSL3_VERSION, -1, TLS_method_func, SSLv3_method);
