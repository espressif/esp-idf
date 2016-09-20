#ifndef _SSL_PM_H_
#define _SSL_PM_H_

#include "ssl_types.h"
#include "esp_common.h"

void* ssl_zalloc(size_t size);
void *ssl_malloc(size_t size);
void ssl_free(void *p);
void* ssl_memcpy(void *to, const void *from, size_t size);

int ssl_pm_new(SSL *ssl);
void ssl_pm_free(SSL *ssl);

int ssl_pm_handshake(SSL *ssl);
int ssl_pm_shutdown(SSL *ssl);
int ssl_pm_clear(SSL *ssl);

int ssl_pm_read(SSL *ssl, void *buffer, int len);
int ssl_pm_send(SSL *ssl, const void *buffer, int len);
int ssl_pm_pending(const SSL *ssl);

void ssl_pm_set_fd(SSL *ssl, int fd, int mode);
int ssl_pm_get_fd(const SSL *ssl, int mode);

OSSL_HANDSHAKE_STATE ssl_pm_get_state(const SSL *ssl);

void ssl_pm_set_bufflen(SSL *ssl, int len);

void* x509_pm_new(void);
void x509_pm_free(void *pm);
int x509_pm_load_crt(void *pm, const unsigned char *buffer, int len);
void x509_pm_unload_crt(void *pm);
void x509_pm_start_ca(void *ssl, void *pm);

void* pkey_pm_new(void);
void pkey_pm_free(void *pm);
int pkey_pm_load_crt(void *pm, const unsigned char *buffer, int len);
void pkey_pm_unload_crt(void *pm);

#endif
