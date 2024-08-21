# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import atexit
import binascii
import fnmatch
import json
import os
import shutil
import socket
import subprocess
import sys
import time
from dataclasses import dataclass
from typing import Any
from typing import Dict
from typing import List

from click.core import Context

try:
    from idf_py_actions.tools import PropertyDict, ensure_build_directory, red_print, yellow_print
except ImportError:
    PropertyDict = Any

from idf_py_actions.tools import get_sdkconfig_value

# Some ports used by QEMU. Could be made configurable, if necessary.
# Note that port 3333 is also assumed by gdb extension.
QEMU_PORT_SERIAL = 5555
PYSERIAL_PORT = f'socket://localhost:{QEMU_PORT_SERIAL}'
QEMU_PORT_GDB = 3333
GBD_PORT = f'localhost:{QEMU_PORT_GDB}'


@dataclass
class QemuTarget:
    """
    Target-specific information related to QEMU.
    """
    target: str     # chip name, e.g. esp32, esp32c3
    qemu_prog: str  # name of the QEMU binary, e.g. qemu-system-xtensa
    install_package: str    # name of the tools.json package from which to install the QEMU binary
    qemu_args: str          # chip-specific arguments to pass to QEMU
    default_efuse: bytes    # default efuse values for the target
    boot_mode_arg: str = ''  # additional arguments to pass to QEMU when booting in download mode
    efuse_device: str = ''  # efuse device name, if different from the target nvram.{target}.efuse


# To generate the default eFuse values, follow the instructions in
# https://github.com/espressif/esp-toolchain-docs/blob/main/qemu/esp32/README.md#using-esptoolpy-and-espefusepy-to-interact-with-qemu
# and burn the eFuses which should be set by default. Then take the binary file, convert it to hex, and paste it here.
QEMU_TARGETS: Dict[str, QemuTarget] = {
    'esp32': QemuTarget(
        'esp32',
        'qemu-system-xtensa',
        'qemu-xtensa',
        '-M esp32 -m 4M',
        # Chip revision 3
        binascii.unhexlify(
            '00000000000000000000000000800000000000000000100000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000'),
        '-global driver=esp32.gpio,property=strap_mode,value=0x0f',
        'nvram.esp32.efuse'),

    'esp32c3': QemuTarget(
        'esp32c3',
        'qemu-system-riscv32',
        'qemu-riscv32',
        '-M esp32c3',
        # Chip revision 0.3
        binascii.unhexlify(
            '00000000000000000000000000000000000000000000000000000000000000000000000000000c00'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '000000000000000000000000000000000000000000000000'),
        '-global driver=esp32c3.gpio,property=strap_mode,value=0x02',
        'nvram.esp32c3.efuse'),

    'esp32s3': QemuTarget(
        'esp32s3',
        'qemu-system-xtensa',
        'qemu-xtensa',
        '-M esp32s3',
        # Chip revision 0.3
        binascii.unhexlify(
            '00000000000000000000000000000000000000000000000000000000000000000000000000000c00'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '00000000000000000000000000000000000000000000000000000000000000000000000000000000'
            '000000000000000000000000000000000000000000000000'),
        '-global driver=esp32s3.gpio,property=strap_mode,value=0x07',
        'nvram.esp32c3.efuse'),   # Not esp32s3, QEMU-201
}


class QemuTaskRunOptions:
    """
    Some options related to QEMU execution, which depend on the presence of other tasks: gdb and monitor.
    """
    def __init__(self) -> None:
        self.bg_mode = False
        self.wait_for_gdb = False
        self.wait_for_monitor = False
        self.boot_mode = False


def wait_for_socket(port: int, timeout_sec: float = 10.0) -> None:
    """
    Helper function, waits for a socket to be open.
    Used to detect that QEMU has started up and ready to receive connections.
    """
    start_time = time.time()
    end_time = start_time + timeout_sec
    while time.time() < end_time:
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.connect(('localhost', port))
                return  # connection successful
        except OSError:
            time.sleep(0.1)
            continue
    red_print(f'Timed out waiting for port {port} to be open')
    raise SystemExit(1)


def action_extensions(base_actions: Dict, project_path: str) -> Dict:
    # Shared state between "global_callback" and "qemu" action.
    # Stores options which depend on the presence of other tasks.
    options = QemuTaskRunOptions()

    def global_callback(ctx: Context, global_args: Dict, tasks: List) -> None:
        # This callback lets us customize QEMU launch arguments depending on the presence of other tasks.
        def have_task(name: str) -> bool:
            return any(fnmatch.fnmatch(task.name, name) for task in tasks)

        have_qemu = have_task('qemu')
        have_gdb = have_task('gdb')
        have_monitor = have_task('monitor')
        have_efuse = have_task('efuse-*')

        if have_qemu:
            if have_gdb and have_monitor:
                red_print('Cannot run qemu with both gdb and monitor in the same terminal')
                raise SystemExit(1)
            if have_gdb:
                options.wait_for_gdb = True
                options.bg_mode = True
            if have_monitor:
                options.wait_for_monitor = True
                options.bg_mode = True
                yellow_print(f'Running qemu on {PYSERIAL_PORT}')
                global_args['port'] = PYSERIAL_PORT
            if have_efuse:
                options.bg_mode = True
                options.boot_mode = True
                yellow_print(f'Running qemu on {PYSERIAL_PORT}')
                global_args['port'] = PYSERIAL_PORT
                for task in tasks:
                    if fnmatch.fnmatch(task.name, 'efuse-*'):
                        if 'before' in task.action_args.keys():
                            task.action_args['before'] = 'no_reset'

    def _get_project_desc(args: PropertyDict, ctx: Context) -> Any:
        desc_path = os.path.join(args.build_dir, 'project_description.json')
        if not os.path.exists(desc_path):
            ensure_build_directory(args, ctx.info_name)
        with open(desc_path, 'r') as f:
            project_desc = json.load(f)
        return project_desc

    def qemu(action: str, ctx: Context, args: PropertyDict, qemu_extra_args: str, gdb: bool, graphics: bool, efuse_file: str) -> None:
        project_desc = _get_project_desc(args, ctx)

        # Determine the target and check if we have the necessary QEMU binary
        target = project_desc['target']
        qemu_target_info = QEMU_TARGETS.get(target)
        if not qemu_target_info:
            red_print(f'QEMU is not supported for target {target}')
            raise SystemExit(1)
        if not shutil.which(qemu_target_info.qemu_prog):
            red_print(f'{qemu_target_info.qemu_prog} is not installed. Please install it using '
                      f'"python $IDF_PATH/tools/idf_tools.py install {qemu_target_info.install_package}" '
                      'or build it from source if the pre-built version is not available for your platform.')
            raise SystemExit(1)

        # Generate flash image and efuse image
        flash_size = get_sdkconfig_value(project_desc['config_file'], 'CONFIG_ESPTOOLPY_FLASHSIZE')
        bin_path = os.path.join(args.build_dir, 'qemu_flash.bin')
        yellow_print(f'Generating flash image: {bin_path}')
        subprocess.check_call([
            sys.executable, '-m', 'esptool', f'--chip={target}', 'merge_bin', f'--output={bin_path}',
            f'--fill-flash-size={flash_size}', '@flash_args'], cwd=args.build_dir)

        if efuse_file:
            efuse_bin_path = efuse_file
        else:
            efuse_bin_path = os.path.join(args.build_dir, 'qemu_efuse.bin')
        try:
            open(efuse_bin_path, 'rb').close()
            yellow_print(f'Using existing efuse image: {efuse_bin_path}')
        except FileNotFoundError:
            yellow_print(f'Generating efuse image: {efuse_bin_path}')
            with open(efuse_bin_path, 'wb') as f:
                f.write(qemu_target_info.default_efuse)

        # Prepare QEMU launch arguments
        qemu_args = [qemu_target_info.qemu_prog]
        qemu_args += qemu_target_info.qemu_args.split(' ')
        qemu_args += [
            '-drive', f'file={bin_path},if=mtd,format=raw',
            '-drive', f'file={efuse_bin_path},if=none,format=raw,id=efuse',
            '-global', f'driver={qemu_target_info.efuse_device},property=drive,value=efuse',
            '-global', f'driver=timer.{target}.timg,property=wdt_disable,value=true',
        ]
        if '-nic' not in qemu_extra_args:
            qemu_args += ['-nic', 'user,model=open_eth']

        if options.wait_for_gdb or gdb:
            qemu_args += ['-gdb', f'tcp::{QEMU_PORT_GDB}', '-S']

        if qemu_extra_args:
            qemu_args += qemu_extra_args.split(' ')

        if graphics:
            qemu_args += ['-display', 'sdl']
        else:
            qemu_args += ['-nographic']

        if options.boot_mode:
            qemu_args += qemu_target_info.boot_mode_arg.split(' ')

        # Launch QEMU!
        if not options.bg_mode:
            qemu_args += ['-serial', 'mon:stdio']
            yellow_print('Running qemu (fg): ' + ' '.join(qemu_args))
            subprocess.run(qemu_args)
        else:
            if options.wait_for_monitor:
                qemu_args += ['-serial', f'tcp::{QEMU_PORT_SERIAL},server']
            else:
                qemu_args += ['-serial', f'tcp::{QEMU_PORT_SERIAL},server,nowait']

            yellow_print('Running qemu (bg): ' + ' '.join(qemu_args))
            qemu_proc = subprocess.Popen(qemu_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
            wait_for_socket(QEMU_PORT_SERIAL)

            def cleanup_qemu() -> None:
                if qemu_proc:
                    qemu_proc.terminate()
                    qemu_proc.wait()
            atexit.register(cleanup_qemu)
            if qemu_proc.poll() is not None:
                yellow_print('QEMU exited with error')
                if qemu_proc.stderr is not None:
                    yellow_print('Stderr output was:')
                    yellow_print(qemu_proc.stderr.read().decode('utf-8'))
                sys.exit(1)

            if gdb and not options.wait_for_gdb:
                yellow_print('Waiting for GDB to connect. You can now run "idf.py gdb" in another terminal window.')

    qemu_actions = {
        'global_action_callbacks': [global_callback],
        'actions': {
            'qemu': {
                'callback': qemu,
                'help': 'Run QEMU.',
                'dependencies': ['all'],
                'options': [
                    {
                        'names': ['--qemu-extra-args'],
                        'help': 'Extra arguments to pass to QEMU',
                        'hidden': False,
                        'is_flag': False,
                        'default': '',
                    },
                    {
                        'names': ['-d', '--gdb'],
                        'help': ('Wait for gdb to connect. '
                                 'Use this option to run "idf.py qemu --gdb monitor" in one terminal window '
                                 'and "idf.py gdb" in another. The program will start running when gdb connects.'),
                        'is_flag': True,
                        'default': False,
                    },
                    {
                        'names': ['-g', '--graphics'],
                        'help': 'Enable graphical window',
                        'is_flag': True,
                        'default': False,
                    },
                    {
                        'names': ['--efuse-file'],
                        'help': ('File used to store efuse values. If not specified, qemu_efuse.bin file '
                                 'in build directory is used.'),
                        'is_flag': False,
                        'default': '',
                    }
                ]
            }
        }
    }
    return qemu_actions
