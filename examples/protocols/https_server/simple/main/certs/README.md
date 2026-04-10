# Certificate Generation Guide

This directory contains certificates for the HTTPS server example. This guide explains how to generate new server and client certificates signed by the existing CA certificate.

## Prerequisites

- OpenSSL installed on your system
- Existing CA certificate (`cacert.pem`) and CA private key (`cakey.pem`)
- Configuration files for certificate extensions (`server_cert.conf` and `client_cert.conf`)

## Generating Server Certificate

Follow these steps to create a new server certificate signed by the CA:

### 1. Generate Server Private Key

```bash
openssl genpkey -algorithm RSA -out new_server.key -pkeyopt rsa_keygen_bits:2048
```

This creates a 2048-bit RSA private key for the server.

### 2. Create Certificate Signing Request (CSR)

```bash
openssl req -new -key new_server.key -out new_server.csr -config server_cert.conf
```

This generates a CSR using the server's private key and the configuration specified in `server_cert.conf`.

### 3. Sign the Server Certificate with CA

```bash
openssl x509 -req -in new_server.csr -CA cacert.pem -CAkey cakey.pem -CAcreateserial -out new_server.pem -days 3650 -extensions v3_req -extfile server_cert.conf
```

This creates the server certificate (`new_server.pem`) valid for 10 years (3650 days), signed by the CA certificate.

## Generating Client Certificate

Follow these steps to create a new client certificate signed by the CA:

### 4. Generate Client Private Key

```bash
openssl genpkey -algorithm RSA -out new_client.key -pkeyopt rsa_keygen_bits:2048
```

This creates a 2048-bit RSA private key for the client.

### 5. Create Certificate Signing Request (CSR)

```bash
openssl req -new -key new_client.key -out new_client.csr -config client_cert.conf
```

This generates a CSR using the client's private key and the configuration specified in `client_cert.conf`.

### 6. Sign the Client Certificate with CA

```bash
openssl x509 -req -in new_client.csr -CA cacert.pem -CAkey cakey.pem -CAcreateserial -out new_client.pem -days 3650 -extensions v3_req -extfile client_cert.conf
```

This creates the client certificate (`new_client.pem`) valid for 10 years (3650 days), signed by the CA certificate.

## Installing the Certificates

### 7. Copy Certificates to Expected Locations

```bash
cp new_server.pem servercert.pem && \
cp new_server.key prvtkey.pem && \
cp new_client.pem client_cert.pem && \
cp new_client.key client_key.pem
```

This copies the newly generated certificates and keys to the filenames expected by the example application.

## File Naming Convention

The example application expects the following files:

- `servercert.pem` - Server certificate
- `prvtkey.pem` - Server private key
- `client_cert.pem` - Client certificate
- `client_key.pem` - Client private key
- `cacert.pem` - CA certificate (for verification)

## Security Notes

⚠️ **Important Security Considerations:**

- The private keys (`prvtkey.pem`, `client_key.pem`, `cakey.pem`) should be kept secure. As these are for demonstration purposes, they are included here, but in a production environment, ensure they are stored securely and access is restricted.
- The certificates in this example directory are for **demonstration purposes only**
- For production use, generate new certificates with appropriate security parameters
- Consider using shorter validity periods for production certificates
- Store private keys with restricted file permissions (e.g., `chmod 600`)

## Verifying Generated Certificates

You can verify the generated certificates using:

```bash
# Verify server certificate
openssl x509 -in servercert.pem -text -noout

# Verify client certificate
openssl x509 -in client_cert.pem -text -noout

# Verify certificate chain
openssl verify -CAfile cacert.pem servercert.pem
openssl verify -CAfile cacert.pem client_cert.pem
```

## Troubleshooting

- If certificate verification fails, ensure the CA certificate and key are valid and match
- Check that the configuration files (`server_cert.conf`, `client_cert.conf`) contain appropriate Subject Alternative Names (SANs) and extensions
- Ensure OpenSSL version is up to date for best compatibility
