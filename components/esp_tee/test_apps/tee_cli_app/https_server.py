# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import http.server
import os
import socket
import ssl
import sys

# python https_server.py --help
# usage: https_server.py [-h] [--certfile CERTFILE] [--keyfile KEYFILE] [--port PORT] [--path PATH]
#
# Start a local HTTPS server.
#
# options:
#   -h, --help           show this help message and exit
#   --certfile CERTFILE  Path to the SSL certificate file (default: test_certs/server_cert.pem)
#   --keyfile KEYFILE    Path to the SSL key file (default: test_certs/server_key.pem)
#   --port PORT          Port number to bind the server to (default: 4443)
#   --path PATH          Path to the directory to serve files from (default: current directory)
#
# E.g., python https_server.py --certfile test_certs/server_cert.pem --keyfile test_certs/server_key.pem --port 8070 --path build


# Get the local machine's network IP address
def get_local_addr() -> str:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    local_addr = ''
    try:
        s.connect(('8.8.8.8', 80))  # Connect to a remote server to get the network IP
        local_addr = s.getsockname()[0]
    finally:
        s.close()
    return local_addr


def main() -> None:
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description='Start a local HTTPS server')
    parser.add_argument('--certfile', default='test_certs/server_cert.pem', help='Path to the SSL certificate file (default: test_certs/server_cert.pem)')
    parser.add_argument('--keyfile', default='test_certs/server_key.pem', help='Path to the SSL key file (default: test_certs/server_key.pem)')
    parser.add_argument('--port', type=int, default=4443, help='Port number to bind the server to (default: 4443)')
    parser.add_argument('--path', default='build', help='Path to the directory to serve files from (default: build directory)')
    args = parser.parse_args()

    # Resolve the absolute paths for the cert and key files before changing the working directory
    certfile_path = os.path.abspath(args.certfile)
    keyfile_path = os.path.abspath(args.keyfile)

    # Change to the specified directory
    os.chdir(args.path)

    # Get the network IP address
    local_ip = get_local_addr()

    # Define the server address and port
    server_address = (local_ip, args.port)

    # Create a simple request handler
    Handler = http.server.SimpleHTTPRequestHandler

    # Create the HTTP server
    httpd = http.server.HTTPServer(server_address, Handler)

    # Create an SSL context
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(certfile=certfile_path, keyfile=keyfile_path)

    # Wrap the server's socket with the SSL context
    httpd.socket = context.wrap_socket(httpd.socket, server_side=True)

    # Print the server address for clients to use
    print(f'Server is running at: https://{local_ip}:{args.port}')
    print('Press [Ctrl+C] to shut down the server')

    # Start the server
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print('\nShutting down the server...')
        httpd.shutdown()
        sys.exit(0)


if __name__ == '__main__':
    main()
