# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import http.server
import logging
import multiprocessing
import os
import socket
import ssl
from typing import Any

import pytest
from common_test_methods import get_host_ip4_by_dest_ip
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_qemu.app import QemuApp
from pytest_embedded_qemu.dut import QemuDut

# ---------------------------------------------------------------------------
# mTLS HTTPS server (runs in a subprocess during tests)
# ---------------------------------------------------------------------------


def _https_mtls_request_handler() -> type[http.server.BaseHTTPRequestHandler]:
    """Request handler for mutual TLS: responds 200 OK to GET, logs at INFO."""

    class RequestHandler(http.server.BaseHTTPRequestHandler):
        protocol_version = 'HTTP/1.1'

        def do_GET(self) -> None:
            logging.info('[HTTPS server] GET from %s', self.address_string())
            self.send_response(200)
            self.send_header('Content-Length', '0')
            self.end_headers()

        def log_message(self, _format: str, *args: object) -> None:
            logging.info(
                '[HTTPS server] %s - - [%s] %s', self.address_string(), self.log_date_time_string(), _format % args
            )

    return RequestHandler


class _MTLSHTTPServer(http.server.HTTPServer):
    """HTTPServer that wraps each connection with TLS and logs handshake success/failure."""

    def __init__(
        self,
        server_address: tuple[str, int],
        RequestHandlerClass: type[http.server.BaseHTTPRequestHandler],
        ssl_context: ssl.SSLContext,
    ) -> None:
        super().__init__(server_address, RequestHandlerClass)
        self.ssl_context = ssl_context

    def get_request(self) -> tuple[socket.socket, Any]:
        conn, addr = self.socket.accept()
        try:
            wrapped = self.ssl_context.wrap_socket(conn, server_side=True)
            logging.info('[HTTPS server] TLS handshake OK from %s:%s', addr[0], addr[1])
            return wrapped, addr
        except ssl.SSLError as e:
            logging.warning('[HTTPS server] TLS handshake failed from %s:%s: %s', addr[0], addr[1], e)
            conn.close()
            raise

    def server_bind(self) -> None:
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        super().server_bind()


def start_https_server_mutual_tls(
    server_cert: str,
    server_key: str,
    client_ca: str,
    host: str,
    port: int,
) -> None:
    """
    Start an HTTPS server that requires and verifies client certificates (mutual TLS).
    Uses server_cert/server_key for the server TLS identity and client_ca to verify the client cert.
    """
    request_handler = _https_mtls_request_handler()
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ssl_context.load_cert_chain(certfile=server_cert, keyfile=server_key)
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.load_verify_locations(cafile=client_ca)
    httpd = _MTLSHTTPServer((host, port), request_handler, ssl_context)
    logging.info('[HTTPS server] Listening on %s:%s (mTLS)', host, port)
    httpd.serve_forever()


# ---------------------------------------------------------------------------
# Paths and constants
# ---------------------------------------------------------------------------

_EXAMPLE_DIR = os.path.dirname(__file__)
_CERTS_DIR = os.path.join(_EXAMPLE_DIR, 'main', 'certs')
_DS_DATA_DIR = os.path.join(_CERTS_DIR, 'esp_secure_cert_data')

_QEMU_EFUSE_FILE = os.path.join(_DS_DATA_DIR, 'qemu_efuse.bin')
_HMAC_KEY_FILE = os.path.join(_DS_DATA_DIR, 'hmac_key.bin')
_ESP_SECURE_CERT_PARTITION = os.path.join(_DS_DATA_DIR, 'esp_secure_cert.bin')

# qemu_extra_args must be a single string (plugin passes it to shlex.split).
_QEMU_EXTRA_DS = (
    f'-drive file={_QEMU_EFUSE_FILE},if=none,format=raw,id=efuse '
    '-global driver=nvram.esp32c3.efuse,property=drive,value=efuse '
    '-global driver=timer.esp32c3.timg,property=wdt_disable,value=true'
)


# ---------------------------------------------------------------------------
# DS eFuse and artifact helpers
# ---------------------------------------------------------------------------


def _ensure_ds_efuse_image() -> None:
    """
    Verify DS artifacts (eFuse image, HMAC key, secure cert partition) are present
    before QEMU starts. All three are committed under main/certs/esp_secure_cert_data/;
    see the DS provisioning section in the example README to regenerate them.
    """
    for path in (_ESP_SECURE_CERT_PARTITION, _HMAC_KEY_FILE, _QEMU_EFUSE_FILE):
        if not os.path.isfile(path):
            pytest.skip(
                f'DS test requires {os.path.basename(path)}. '
                'See the DS peripheral mode section in the example README for provisioning steps.'
            )


@pytest.fixture(autouse=True)
def _ensure_ds_efuse_before_qemu(request: pytest.FixtureRequest) -> None:
    """Ensure eFuse image exists before QEMU starts for DS tests."""
    if 'qemu_ds' not in request.node.name:
        return
    _ensure_ds_efuse_image()


# ---------------------------------------------------------------------------
# Helper: start mTLS server and send URL to device
# ---------------------------------------------------------------------------


def _run_mtls_test(
    dut: QemuDut,
    server_port: int,
    client_ca: str,
    expect_success: bool,
) -> None:
    """
    Start mTLS server, wait for device to request URL, send it, and verify outcome.
    """
    server_cert = os.path.join(_CERTS_DIR, 'server_cert.pem')
    server_key = os.path.join(_CERTS_DIR, 'server_key.pem')

    server_proc = multiprocessing.Process(
        target=start_https_server_mutual_tls,
        args=(server_cert, server_key, client_ca, '0.0.0.0', server_port),
    )
    server_proc.daemon = True
    server_proc.start()
    logging.info('HTTPS server with mutual TLS started on port %s', server_port)
    try:
        ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)', timeout=60)[1].decode()
        host_ip = get_host_ip4_by_dest_ip(ip_address)
        dut.expect('Enter mutual auth server URL:', timeout=30)
        dut.write(f'https://{host_ip}:{server_port}\n')

        if expect_success:
            dut.expect('HTTPS Mutual Auth Status = 200', timeout=30)
            logging.info('mTLS test passed: status 200')
        else:
            dut.expect('mbedtls_ssl_handshake returned -0x7780', timeout=30)
            dut.expect('Request failed:', timeout=30)
            logging.info('Negative test passed: connection failed as expected')

        dut.expect('Finish mutual auth example', timeout=10)
    finally:
        server_proc.terminate()
        server_proc.join(timeout=5)
        if server_proc.is_alive():
            server_proc.kill()


# ---------------------------------------------------------------------------
# Test: software key mutual TLS
# ---------------------------------------------------------------------------


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize('config', ['default'], indirect=True)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_mutual_auth_software_keys(app: QemuApp, dut: QemuDut) -> None:
    """QEMU test: mutual TLS with embedded software client cert + key."""
    if os.environ.get('IDF_TOOLCHAIN') == 'clang':
        pytest.skip('QEMU mTLS test not supported with clang toolchain (Docker SLIRP networking issue)')
    client_ca = os.path.join(_CERTS_DIR, 'ca_cert.pem')
    _run_mtls_test(dut, server_port=8070, client_ca=client_ca, expect_success=True)


# ---------------------------------------------------------------------------
# Tests: DS peripheral mutual TLS (migrated from esp_http_client)
# ---------------------------------------------------------------------------


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize('config', ['qemu_ds'], indirect=True)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
@pytest.mark.parametrize('qemu_extra_args', [_QEMU_EXTRA_DS], indirect=True)
def test_mutual_auth_ds(app: QemuApp, dut: QemuDut) -> None:
    """QEMU + DS peripheral test: mTLS using DS peripheral for client key."""
    if os.environ.get('IDF_TOOLCHAIN') == 'clang':
        pytest.skip('DS QEMU test not supported with clang toolchain (Docker SLIRP networking issue)')
    client_ca = os.path.join(_CERTS_DIR, 'ca_cert.pem')
    _run_mtls_test(dut, server_port=8070, client_ca=client_ca, expect_success=True)


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize('config', ['qemu_ds'], indirect=True)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
@pytest.mark.parametrize('qemu_extra_args', [_QEMU_EXTRA_DS], indirect=True)
def test_mutual_auth_ds_fails_wrong_credentials(app: QemuApp, dut: QemuDut) -> None:
    """Negative test: server uses wrong CA, rejects device client cert."""
    if os.environ.get('IDF_TOOLCHAIN') == 'clang':
        pytest.skip('DS QEMU test not supported with clang toolchain (Docker SLIRP networking issue)')
    # Wrong CA: server uses server_cert as client CA (won't verify our client cert)
    wrong_ca = os.path.join(_CERTS_DIR, 'server_cert.pem')
    _run_mtls_test(dut, server_port=8071, client_ca=wrong_ca, expect_success=False)
