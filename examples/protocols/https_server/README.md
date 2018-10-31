# HTTP server with SSL support using OpenSSL

This example creates a SSL server that returns a simple HTML page when you visit its root URL.

See the `esp_https_server` component documentation for details.

## Certificates

You will need to approve a security exception in your browser. This is because of a self signed 
certificate; this will be always the case, unless you preload the CA root into your browser/system
as trusted.

You can generate a new certificate using the OpenSSL command line tool as shown in the script
`main/certs/gencert.sh`. It is **strongly recommended** to not reuse the example certificate in 
your application; it is included only for demonstration.
