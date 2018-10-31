#!/bin/bash

# Example command to generate a new certificate in the correct format.
# Expiry time and metadata fields can be adjusted in the invocation.

# Please see the openssl man pages (man openssl-req) for more details

openssl req -newkey rsa:2048 -nodes -keyout prvtkey.pem -x509 -days 3650 -out cacert.pem -subj "/CN=ESP32 HTTPS server example"
