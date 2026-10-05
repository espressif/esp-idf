| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- | ----- |
# ESP HTTP Client Example

See the README.md file in the upper level 'examples' directory for more information about examples.

## TLS ciphersuite and group configuration

`esp_http_client_config_t` has two fields that restrict the TLS handshake of
one client:

* `ciphersuites_list` is a zero-terminated array of IANA ciphersuite
  identifiers.
* `groups_list` is a zero-terminated array of IANA supported group
  identifiers. The groups are the named groups that are used for key exchange.

`https_with_restricted_tls_lists()` in `main/esp_http_client_example.c` shows
both fields. At startup the example also logs the full lists that the TLS
stack supports, through `esp_tls_get_ciphersuites_list()` and
`esp_tls_get_supported_groups_list()`.

Note the rules of the two lists:

* The client does not copy the arrays. Each array must stay valid until
  `esp_http_client_cleanup()`. The example uses static arrays.
* A listed ciphersuite works only if its modules are enabled in `menuconfig`.
  mbedtls does no dependency check on the list.
* A listed group works only if its curve is enabled in `menuconfig`.
* The ciphersuites must match the key type of the server certificate. The
  example list holds an ECDSA suite and an RSA suite for this reason.
* One ciphersuite list covers TLS 1.2 and TLS 1.3. A list that holds only
  TLS 1.2 suites makes a TLS 1.3 handshake fail. The example list adds a
  TLS 1.3 suite when `CONFIG_MBEDTLS_SSL_PROTO_TLS1_3` is enabled.
* A group list that excludes the group of the server costs one
  HelloRetryRequest in TLS 1.3. In TLS 1.2 the handshake fails.

For a server, use the `ciphersuites_list` and `groups_list` fields of
`httpd_ssl_config_t`.
