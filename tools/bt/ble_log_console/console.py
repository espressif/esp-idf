# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""BLE Log Console entry point.

Usage:
    python console.py                        # interactive setup
    python console.py --port /dev/ttyUSB0    # direct connect
    python console.py ls                     # list saved files
"""

from datetime import datetime
from pathlib import Path

import click
from src.app import BLELogApp
from src.backend.models import format_bytes
from src.backend.uart_transport import validate_uart_port


@click.group(invoke_without_command=True)
@click.option('--port', '-p', default=None, help='UART port. If omitted, shows interactive setup.')
@click.option('--baudrate', '-b', type=int, default=3_000_000, show_default=True, help='Baud rate')
@click.option(
    '--log-dir', '-d', type=click.Path(), default=None, help='Log save directory. Default: current working directory.'
)
@click.option(
    '--output',
    '-o',
    type=click.Path(),
    default=None,
    hidden=True,
    help='[Deprecated] Output binary file path. Use --log-dir instead.',
)
@click.pass_context
def cli(ctx: click.Context, port: str | None, baudrate: int, log_dir: str | None, output: str | None) -> None:
    """BLE Log Console — real-time BLE log monitor."""
    if ctx.invoked_subcommand is not None:
        return

    # Resolve log directory
    resolved_log_dir: Path | None = None
    if output is not None:
        # Legacy --output: treat as full file path, use its parent as log_dir
        click.echo(
            'Warning: --output is deprecated and the filename is ignored. '
            'Use --log-dir instead. Saving to directory: ' + str(Path(output).parent),
            err=True,
        )
        resolved_log_dir = Path(output).parent
    elif log_dir is not None:
        resolved_log_dir = Path(log_dir)

    if port is not None:
        error = validate_uart_port(port)
        if error:
            raise click.BadParameter(error, param_hint="'--port'")

    app = BLELogApp(
        port=port,
        baudrate=baudrate,
        log_dir=resolved_log_dir,
    )
    app.run()


@cli.command(name='ls')
@click.option(
    '--dir',
    '-d',
    'log_dir',
    type=click.Path(exists=True),
    default=None,
    help='Directory to list. Default: current directory.',
)
def list_files(log_dir: str | None) -> None:
    """List saved binary capture files."""
    search_dir = Path(log_dir) if log_dir else Path.cwd()

    files = sorted(search_dir.glob('ble_log_*.bin'), key=lambda f: f.stat().st_mtime, reverse=True)
    if not files:
        click.echo(f'No captures found in {search_dir}')
        return

    click.echo(f'Captures in {search_dir}:\n')
    for f in files:
        size = f.stat().st_size
        mtime = datetime.fromtimestamp(f.stat().st_mtime).strftime('%Y-%m-%d %H:%M:%S')
        size_str = format_bytes(size)
        click.echo(f'  {mtime}  {size_str:>10}  {f.name}')


if __name__ == '__main__':
    cli()
