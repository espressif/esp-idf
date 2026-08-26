# Certificate Generation for Mutual TLS Example

All certificates are pre-generated with a 10-year lifetime. To regenerate, run
the following OpenSSL commands from this directory (`main/certs/`).

## Generate CA (signs both client and server certs)

    openssl req -newkey rsa:2048 -nodes -keyout ca_key.pem -x509 -days 3650 \
      -out ca_cert.pem -subj "/CN=Mutual Auth Test CA"

## Generate client certificate (embedded in firmware)

    openssl genrsa -out client_key.pem 2048
    openssl req -out client.csr -key client_key.pem -new -subj "/CN=esp_mutual_auth_client"
    openssl x509 -req -days 3650 -in client.csr -CA ca_cert.pem -CAkey ca_key.pem \
      -sha256 -CAcreateserial -out client_cert.pem
    rm client.csr ca_cert.srl

## Generate server certificate (used by pytest mTLS server)

    openssl genrsa -out server_key.pem 2048
    openssl req -out server.csr -key server_key.pem -new -subj "/CN=esp_mutual_auth_server"
    openssl x509 -req -days 3650 -in server.csr -CA ca_cert.pem -CAkey ca_key.pem \
      -sha256 -CAcreateserial -out server_cert.pem
    rm server.csr ca_cert.srl

## Files

| File | Purpose | Embedded in firmware? |
|------|---------|----------------------|
| `ca_cert.pem` | CA certificate (verifies server identity) | Yes |
| `ca_key.pem` | CA private key (signs certs; keep private) | No |
| `client_cert.pem` | Client certificate (sent to server) | Yes (software mode) |
| `client_key.pem` | Client private key | Yes (software mode) |
| `server_cert.pem` | Server certificate (pytest mTLS server) | No |
| `server_key.pem` | Server private key (pytest mTLS server) | No |

## DS peripheral mode

For DS peripheral mode, see `esp_secure_cert_data/` and the DS provisioning
section in the top-level README.
