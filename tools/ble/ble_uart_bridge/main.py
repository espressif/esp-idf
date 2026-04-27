# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from src import run_connection_check
from src import run_console
from src import run_daemon
from src import run_daemon_send
from src import run_daemon_status
from src import run_list_devices
from src.console import ConsoleEncoding
from src.console import ConsoleTerminator
from typer import Option
from typer import Typer

# CLI app singleton
app = Typer()


# Core service
@app.command()
def list_devices() -> None:
    run_list_devices()


@app.command()
def connection_check(device_id: str) -> None:
    run_connection_check(device_id)


# Console
@app.command()
def console(
    device_id: str,
    terminator: ConsoleTerminator = Option(ConsoleTerminator.lf, help='Line terminator for text mode'),
    encoding: ConsoleEncoding = Option(ConsoleEncoding.text, help='Console encoding'),
    with_response: bool = Option(False, help='Use BLE write-with-response for TX data'),
) -> None:
    run_console(device_id, terminator=terminator, encoding=encoding, with_response=with_response)


# Daemon
@app.command()
def daemon(device_id: str, host: str = '127.0.0.1', port: int = 8888) -> None:
    run_daemon(device_id, host, port)


@app.command()
def daemon_status(host: str = '127.0.0.1', port: int = 8888) -> None:
    run_daemon_status(host=host, port=port)


@app.command()
def daemon_send(
    data: str,
    op: str = Option('raw', help='Operation name in the JSONL request envelope'),
    json_payload: bool = Option(False, '--json', help='Parse DATA as JSON instead of sending it as a string'),
    timeout: float = 10.0,
    host: str = '127.0.0.1',
    port: int = 8888,
) -> None:
    run_daemon_send(data=data, op=op, json_payload=json_payload, timeout=timeout, host=host, port=port)


def main() -> None:
    app()


if __name__ == '__main__':
    main()
