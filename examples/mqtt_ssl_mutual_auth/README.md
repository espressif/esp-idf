# ESPMQTT SSL Sample application

Navigate to the main directory

```
cd main
```

Generate a client key and a CSR. When you are generating the CSR, do not use the default values. At a minimum, the CSR must include the Country, Organisation and Common Name fields.

```
openssl genrsa -out client.key
openssl req -out client.csr -key client.key -new
```

Paste the generated CSR in the [Mosquitto test certificate signer](https://test.mosquitto.org/ssl/index.php), click Submit and copy the downloaded `client.crt` in the `main` directory.
