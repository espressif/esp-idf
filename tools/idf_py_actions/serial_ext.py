# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os
import shlex
import signal
import sys
from typing import Any
from typing import Dict
from typing import List
from typing import Optional

import click
from idf_py_actions.errors import FatalError
from idf_py_actions.global_options import global_options
from idf_py_actions.tools import ensure_build_directory
from idf_py_actions.tools import get_default_serial_port
from idf_py_actions.tools import get_sdkconfig_value
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import run_target
from idf_py_actions.tools import RunTool
PYTHON = sys.executable


BAUD_RATE = {
    'names': ['-b', '--baud'],
    'help': ("Global baud rate for all idf.py subcommands if they don't overwrite it locally."
             "It can imply monitor baud rate as well if it hasn't been defined locally."),
    'scope': 'global',
    'envvar': 'ESPBAUD',
    'default': 460800,
}

PORT = {
    'names': ['-p', '--port'],
    'help': 'Serial port.',
    'scope': 'global',
    'envvar': 'ESPPORT',
    'type': click.Path(),
    'default': None,
}


def yellow_print(message: str, newline: Optional[str]='\n') -> None:
    """Print a message to stderr with yellow highlighting"""
    sys.stderr.write('%s%s%s%s' % ('\033[0;33m', message, '\033[0m', newline))
    sys.stderr.flush()


def action_extensions(base_actions: Dict, project_path: str) -> Dict:
    def _get_project_desc(ctx: click.core.Context, args: PropertyDict) -> Any:
        desc_path = os.path.join(args.build_dir, 'project_description.json')
        if not os.path.exists(desc_path):
            ensure_build_directory(args, ctx.info_name)
        with open(desc_path, 'r', encoding='utf-8') as f:
            project_desc = json.load(f)
        return project_desc

    def _get_esptool_args(args: PropertyDict) -> List:
        esptool_path = os.path.join(
            os.environ['IDF_PATH'], 'components/esptool_py/esptool/esptool.py'
        )
        esptool_wrapper_path = os.environ.get('ESPTOOL_WRAPPER', '')
        if args.port is None:
            args.port = get_default_serial_port()
        result = [PYTHON]
        if os.path.exists(esptool_wrapper_path):
            result += [esptool_wrapper_path]
        result += [esptool_path]
        result += ['-p', args.port]
        result += ['-b', str(args.baud)]

        with open(os.path.join(args.build_dir, 'flasher_args.json'), encoding='utf-8') as f:
            flasher_args = json.load(f)

        extra_esptool_args = flasher_args['extra_esptool_args']
        result += ['--before', extra_esptool_args['before']]
        result += ['--after', extra_esptool_args['after']]
        result += ['--chip', extra_esptool_args['chip']]
        if not extra_esptool_args['stub']:
            result += ['--no-stub']
        return result

    def _get_commandline_options(ctx: click.core.Context) -> List:
        """Return all the command line options up to first action"""
        # This approach ignores argument parsing done Click
        result = []

        for arg in sys.argv:
            if arg in ctx.command.commands_with_aliases:
                break

            result.append(arg)

        return result

    def monitor(
        action: str,
        ctx: click.core.Context,
        args: PropertyDict,
        print_filter: str,
        monitor_baud: str,
        encrypted: bool,
        no_reset: bool,
        timestamps: bool,
        timestamp_format: str,
        force_color: bool,
        disable_auto_color: bool,
    ) -> None:
        """
        Run esp_idf_monitor to watch build output
        """
        project_desc = _get_project_desc(ctx, args)
        elf_file = os.path.join(args.build_dir, project_desc['app_elf'])

        idf_monitor = os.path.join(os.environ['IDF_PATH'], 'tools/idf_monitor.py')
        monitor_args = [PYTHON, idf_monitor]

        if project_desc['target'] != 'linux':
            if no_reset and args.port is None:
                raise FatalError(
                    'Error: --no-reset is only supported when used with a port.'
                    'Please specify the port with the --port argument in order to use this option.'
                )

            args.port = args.port or get_default_serial_port()
            monitor_args += ['-p', args.port]

            baud = (
                monitor_baud
                or os.getenv('IDF_MONITOR_BAUD')
                or os.getenv('MONITORBAUD')
            )

            if baud is None:
                # Baud hasn't been changed locally (by local baud argument nor by environment variables)
                #
                # Use the global baud rate if it has been changed by the command line.
                # Use project_desc['monitor_baud'] as the last option.

                global_baud_defined = (
                    ctx._parameter_source['baud']
                    == click.core.ParameterSource.COMMANDLINE
                )
                baud = (
                    args.baud if global_baud_defined else project_desc['monitor_baud']
                )

            monitor_args += ['-b', baud]

        monitor_args += ['--toolchain-prefix', project_desc['monitor_toolprefix']]

        coredump_decode = get_sdkconfig_value(
            project_desc['config_file'], 'CONFIG_ESP_COREDUMP_DECODE'
        )
        if coredump_decode is not None:
            monitor_args += ['--decode-coredumps', coredump_decode]

        target_arch_riscv = get_sdkconfig_value(
            project_desc['config_file'], 'CONFIG_IDF_TARGET_ARCH_RISCV'
        )
        monitor_args += ['--target', project_desc['target']]
        revision = project_desc.get('min_rev')
        if revision:
            monitor_args += ['--revision', revision]

        if target_arch_riscv:
            monitor_args += ['--decode-panic', 'backtrace']

        if print_filter is not None:
            monitor_args += ['--print_filter', print_filter]

        if elf_file:
            monitor_args += [elf_file]

        if encrypted:
            monitor_args += ['--encrypted']

        if no_reset:
            monitor_args += ['--no-reset']

        if timestamps:
            monitor_args += ['--timestamps']

        if timestamp_format:
            monitor_args += ['--timestamp-format', timestamp_format]

        if force_color or os.name == 'nt':
            monitor_args += ['--force-color']

        if disable_auto_color:
            monitor_args += ['--disable-auto-color']

        idf_py = [PYTHON] + _get_commandline_options(ctx)  # commands to re-run idf.py
        monitor_args += ['-m', ' '.join("'%s'" % a for a in idf_py)]
        hints = not args.no_hints

        # Temporally ignore SIGINT, which is used in idf_monitor to spawn gdb.
        old_handler = signal.getsignal(signal.SIGINT)
        signal.signal(signal.SIGINT, signal.SIG_IGN)
        try:
            RunTool(
                'idf_monitor',
                monitor_args,
                args.project_dir,
                build_dir=args.build_dir,
                hints=hints,
                interactive=True,
                convert_output=True,
            )()
        finally:
            signal.signal(signal.SIGINT, old_handler)

    def flash(
        action: str,
        ctx: click.core.Context,
        args: PropertyDict,
        force: bool,
        extra_args: str,
        trace: bool,
    ) -> None:
        """
        Run esptool to flash the entire project, from an argfile generated by the build system
        """
        ensure_build_directory(args, ctx.info_name)
        project_desc = _get_project_desc(ctx, args)
        if project_desc['target'] == 'linux':
            yellow_print('skipping flash since running on linux...')
            return

        args.port = args.port or get_default_serial_port()

        extra_pre = list()
        if trace:
            extra_pre.append('--trace')

        extra_post = list()
        if force:
            extra_post.append('--force')
        if extra_args:
            extra_post += shlex.split(extra_args)

        env = {
            'ESPBAUD': str(args.baud),
            'ESPPORT': args.port,
            'SERIAL_TOOL_EXTRA_PRE_CMD_ARGS': ';'.join(extra_pre),
            'SERIAL_TOOL_EXTRA_ARGS': ';'.join(extra_post),
        }
        run_target(action, args, env, force_progression=True)

    def erase_flash(action: str, ctx: click.core.Context, args: PropertyDict) -> None:
        ensure_build_directory(args, ctx.info_name)
        esptool_args = _get_esptool_args(args)
        esptool_args += ['erase_flash']
        RunTool('esptool.py', esptool_args, args.build_dir, hints=not args.no_hints)()

    def global_callback(
        ctx: click.core.Context, global_args: Dict, tasks: PropertyDict
    ) -> None:
        encryption = any(
            [task.name in ('encrypted-flash', 'encrypted-app-flash') for task in tasks]
        )
        if encryption:
            for task in tasks:
                if task.name == 'monitor':
                    task.action_args['encrypted'] = True
                    break

    def ota_targets(
        target_name: str, ctx: click.core.Context, args: PropertyDict
    ) -> None:
        """
        Execute the target build system to build target 'target_name'.
        Additionally set global variables for baud and port.
        Calls ensure_build_directory() which will run cmake to generate a build
        directory (with the specified generator) as needed.
        """
        args.port = args.port or get_default_serial_port()
        ensure_build_directory(args, ctx.info_name)
        run_target(target_name, args, {'ESPBAUD': str(args.baud), 'ESPPORT': args.port})

    def merge_bin(action: str,
                  ctx: click.core.Context,
                  args: PropertyDict,
                  output: str,
                  format: str,
                  md5_disable: str,
                  flash_offset: str,
                  fill_flash_size: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        project_desc = _get_project_desc(ctx, args)
        merge_bin_args = [PYTHON, '-m', 'esptool']
        target = project_desc['target']
        merge_bin_args += ['--chip', target]
        merge_bin_args += ['merge_bin']  # needs to be after the --chip option
        if not output:
            if format in ('raw', 'uf2'):
                output = 'merged-binary.bin'
            elif format == 'hex':
                output = 'merged-binary.hex'
        merge_bin_args += ['-o', output]
        if format:
            merge_bin_args += ['-f', format]
        if md5_disable:
            if format != 'uf2':
                yellow_print('idf.py merge-bin: --md5-disable is only valid for UF2 format. Option will be ignored.')
            else:
                merge_bin_args += ['--md5-disable']
        if flash_offset:
            if format != 'raw':
                yellow_print('idf.py merge-bin: --flash-offset is only valid for RAW format. Option will be ignored.')
            else:
                merge_bin_args += ['-t', flash_offset]
        if fill_flash_size:
            if format != 'raw':
                yellow_print('idf.py merge-bin: --fill-flash-size is only valid for RAW format, option will be ignored.')
            else:
                merge_bin_args += ['--fill-flash-size', fill_flash_size]
        merge_bin_args += ['@flash_args']
        print(f'Merged binary {output} will be created in the build directory...')
        RunTool('merge_bin', merge_bin_args, args.build_dir, build_dir=args.build_dir, hints=not args.no_hints)()

    def secure_decrypt_flash_data(
            action: str,
            ctx: click.core.Context,
            args: PropertyDict,
            aes_xts: bool,
            keyfile: str,
            output: str,
            address: str,
            flash_crypt_conf: str,
            **extra_args: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        decrypt_flash_data_args = [PYTHON, '-m', 'espsecure', 'decrypt_flash_data']
        if aes_xts:
            decrypt_flash_data_args += ['--aes_xts']
        if keyfile:
            decrypt_flash_data_args += ['--keyfile', keyfile]
        if output:
            decrypt_flash_data_args += ['--output', output]
        if address:
            decrypt_flash_data_args += ['--address', address]
        if flash_crypt_conf:
            decrypt_flash_data_args += ['--flash_crypt_conf', flash_crypt_conf]
        if extra_args['encrypted_file']:
            decrypt_flash_data_args += [extra_args['encrypted_file']]
        RunTool('espsecure', decrypt_flash_data_args, args.build_dir)()

    def secure_digest_secure_bootloader(
            action: str,
            ctx: click.core.Context,
            args: PropertyDict,
            keyfile: str,
            output: str,
            iv: str,
            **extra_args: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        digest_secure_bootloader_args = [PYTHON, '-m', 'espsecure', 'digest_secure_bootloader']
        if keyfile:
            digest_secure_bootloader_args += ['--keyfile', keyfile]
        if output:
            digest_secure_bootloader_args += ['--output', output]
        if iv:
            digest_secure_bootloader_args += ['--iv', iv]
        if extra_args['image']:
            digest_secure_bootloader_args += [extra_args['image']]
        RunTool('espsecure', digest_secure_bootloader_args, args.build_dir)()

    def secure_encrypt_flash_data(
            action: str,
            ctx: click.core.Context,
            args: PropertyDict,
            aes_xts: bool,
            keyfile: str,
            output: str,
            address: str,
            flash_crypt_conf: str,
            **extra_args: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        encrypt_flash_data_args = [PYTHON, '-m', 'espsecure', 'encrypt_flash_data']
        if aes_xts:
            encrypt_flash_data_args += ['--aes_xts']
        if keyfile:
            encrypt_flash_data_args += ['--keyfile', keyfile]
        if output:
            encrypt_flash_data_args += ['--output', output]
        if address:
            encrypt_flash_data_args += ['--address', address]
        if flash_crypt_conf:
            encrypt_flash_data_args += ['--flash_crypt_conf', flash_crypt_conf]
        if extra_args['plaintext_file']:
            encrypt_flash_data_args += [extra_args['plaintext_file']]
        RunTool('espsecure', encrypt_flash_data_args, args.build_dir)()

    def secure_generate_flash_encryption_key(action: str, ctx: click.core.Context, args: PropertyDict, keylen: str, **extra_args: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        generate_flash_encryption_key_args = [PYTHON, '-m', 'espsecure', 'generate_flash_encryption_key']
        if keylen:
            generate_flash_encryption_key_args += ['--keylen', keylen]
        if extra_args['keyfile']:
            generate_flash_encryption_key_args += [extra_args['keyfile']]
        RunTool('espsecure',  generate_flash_encryption_key_args, args.project_dir)()

    def secure_generate_signing_key(action: str, ctx: click.core.Context, args: PropertyDict, version: str, scheme: str, **extra_args: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        generate_signing_key_args = [PYTHON, '-m', 'espsecure', 'generate_signing_key']
        project_desc = _get_project_desc(ctx, args)
        ecdsa_scheme = get_sdkconfig_value(project_desc['config_file'], 'CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME')
        ecdsa_v2_scheme = get_sdkconfig_value(project_desc['config_file'], 'CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME')
        rsa_scheme = get_sdkconfig_value(project_desc['config_file'], 'CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME')
        if ecdsa_scheme:
            version = '1'
        elif ecdsa_v2_scheme or rsa_scheme:
            version = '2'
        if version:
            generate_signing_key_args += ['--version', version]
        if scheme:
            generate_signing_key_args += ['--scheme', scheme]
        if extra_args['keyfile']:
            generate_signing_key_args += [extra_args['keyfile']]
        RunTool('espsecure', generate_signing_key_args, args.project_dir)()

    def secure_sign_data(action: str,
                         ctx: click.core.Context,
                         args: PropertyDict,
                         version: str,
                         keyfile: str,
                         append_signatures: bool,
                         pub_key: str,
                         signature: str,
                         output: str,
                         **extra_args: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        sign_data_args = [PYTHON, '-m', 'espsecure', 'sign_data']
        if version:
            sign_data_args += ['--version', version]
        if keyfile:
            sign_data_args += ['--keyfile', keyfile]
        if append_signatures:
            sign_data_args += ['--append_signatures']
        if pub_key:
            sign_data_args += ['--pub-key', pub_key]
        if signature:
            sign_data_args += ['--signature', signature]
        if output:
            sign_data_args += ['--output', output]
        if extra_args['datafile']:
            sign_data_args += [extra_args['datafile']]
        RunTool('espsecure', sign_data_args, args.build_dir)()

    def secure_verify_signature(action: str,
                                ctx: click.core.Context,
                                args: PropertyDict,
                                version: str,
                                keyfile: str,
                                **extra_args: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        verify_signature_args = [PYTHON, '-m', 'espsecure', 'verify_signature']
        if version:
            verify_signature_args += ['--version', version]
        if keyfile:
            verify_signature_args += ['--keyfile', keyfile]
        if extra_args['datafile']:
            verify_signature_args += [extra_args['datafile']]
        RunTool('espsecure', verify_signature_args, args.build_dir)()

    def _parse_efuse_args(ctx: click.core.Context, args: PropertyDict, extra_args: Dict) -> List:
        efuse_args = []
        if args.port:
            efuse_args += ['-p', args.port]
        elif not args.port and not extra_args['virt']:  # if --virt, no port will be found and it would cause error
            efuse_args += ['-p', get_default_serial_port()]
        efuse_args += ['--chip', _get_project_desc(ctx, args)['target']]
        if extra_args['virt']:
            efuse_args += ['--virt']
        if extra_args['before']:
            efuse_args += ['--before', extra_args['before'].replace('-', '_')]
        if extra_args['debug']:
            efuse_args += ['--debug']
        if extra_args['do_not_confirm']:
            efuse_args += ['--do-not-confirm']
        return efuse_args

    def efuse_burn(action: str, ctx: click.core.Context, args: PropertyDict, **extra_args: Dict) -> None:
        ensure_build_directory(args, ctx.info_name)
        burn_efuse_args = [PYTHON, '-m' 'espefuse', 'burn_efuse']
        burn_efuse_args += _parse_efuse_args(ctx, args, extra_args)
        if extra_args['efuse_positional_args']:
            burn_efuse_args += list(extra_args['efuse_positional_args'])
        RunTool('espefuse', burn_efuse_args, args.build_dir)()

    def efuse_burn_key(action: str, ctx: click.core.Context, args: PropertyDict, **extra_args: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        burn_key_args = [PYTHON, '-m' 'espefuse', 'burn_key']
        burn_key_args += _parse_efuse_args(ctx, args, extra_args)
        if extra_args['no_protect_key']:
            burn_key_args += ['--no-protect-key']
        if extra_args['force_write_always']:
            burn_key_args += ['--force-write-always']
        if extra_args['show_sensitive_info']:
            burn_key_args += ['--show-sensitive-info']
        if extra_args['efuse_positional_args']:
            burn_key_args += extra_args['efuse_positional_args']
        RunTool('espefuse.py', burn_key_args, args.project_dir, build_dir=args.build_dir)()

    def efuse_dump(action: str, ctx: click.core.Context, args: PropertyDict, file_name: str, **extra_args: Dict) -> None:
        ensure_build_directory(args, ctx.info_name)
        dump_args = [PYTHON, '-m' 'espefuse', 'dump']
        dump_args += _parse_efuse_args(ctx, args, extra_args)
        if file_name:
            dump_args += ['--file_name', file_name]
        RunTool('espefuse', dump_args, args.build_dir)()

    def efuse_read_protect(action: str, ctx: click.core.Context, args: PropertyDict, **extra_args: Dict) -> None:
        ensure_build_directory(args, ctx.info_name)
        read_protect_args = [PYTHON, '-m' 'espefuse', 'read_protect_efuse']
        read_protect_args += _parse_efuse_args(ctx, args, extra_args)
        if extra_args['efuse_positional_args']:
            read_protect_args += list(extra_args['efuse_positional_args'])
        RunTool('espefuse', read_protect_args, args.build_dir)()

    def efuse_summary(action: str, ctx: click.core.Context, args: PropertyDict, format: str, **extra_args: Dict) -> None:
        ensure_build_directory(args, ctx.info_name)
        summary_args = [PYTHON, '-m' 'espefuse', 'summary']
        summary_args += _parse_efuse_args(ctx, args, extra_args)
        if format:
            summary_args += [f'--format={format.replace("-", "_")}']
        if extra_args['efuse_name']:
            summary_args += [str(extra_args['efuse_name'])]
        RunTool('espefuse', summary_args, args.build_dir)()

    def efuse_write_protect(action: str, ctx: click.core.Context, args: PropertyDict, **extra_args: Dict) -> None:
        ensure_build_directory(args, ctx.info_name)
        write_protect_args = [PYTHON, '-m' 'espefuse', 'write_protect_efuse']
        write_protect_args += _parse_efuse_args(ctx, args, extra_args)
        if extra_args['efuse_positional_args']:
            write_protect_args += list(extra_args['efuse_positional_args'])
        RunTool('espefuse', write_protect_args, args.build_dir)()

    BAUD_AND_PORT = [BAUD_RATE, PORT]
    flash_options = BAUD_AND_PORT + [
        {
            'names': ['--trace'],
            'is_flag': True,
            'help': 'Enable trace-level output of flasher tool interactions. Useful when submitting bug reports.',
        },
        {
            'names': ['--force'],
            'is_flag': True,
            'help': 'Force write, skip security and compatibility checks. Use with caution!',
        },
        {
            'names': ['--extra-args'],
            'help': (
                'Pass extra arguments to esptool separated by space. For more details see `esptool.py write_flash --help`. '
                'For example to compress and verify data use: `idf.py flash --extra-args="--compress --verify"`. Use with caution!'
            )
        }
    ]

    EFUSE_OPTS = [PORT] + [
        {
            'names': ['--virt'],
            'is_flag': True,
            'hidden': True,
            'help': 'For host tests, the tool will work in the virtual mode (without connecting to a chip).',
        },
        {
            'names': ['--before'],
            'help': 'What to do before connecting to the chip.',
            'type': click.Choice(['default-reset', 'usb-reset', 'no-reset', 'no-reset-no-sync']),
        },
        {
            'names': ['--debug', '-d'],
            'is_flag': True,
            'help': 'Print debug information (loglevel=DEBUG).',
        },
        {
            'names': ['--do-not-confirm'],
            'is_flag': True,
            'help': 'Do not pause for confirmation before permanently writing eFuses. Use with caution!',
        },
    ]

    serial_actions = {
        'global_action_callbacks': [global_callback],
        'actions': {
            'flash': {
                'callback': flash,
                'help': 'Flash the project.',
                'options': global_options + flash_options,
                'order_dependencies': ['all', 'erase-flash'],
            },
            'erase-flash': {
                'callback': erase_flash,
                'help': 'Erase entire flash chip.',
                'options': BAUD_AND_PORT,
            },
            'erase_flash': {
                'callback': erase_flash,
                'deprecated': {
                    'since': 'v4.4',
                    'removed': 'next major release',
                    'message': 'Have you wanted to run "erase-flash" instead?',
                },
                'hidden': True,
                'help': 'Erase entire flash chip.',
                'options': BAUD_AND_PORT,
            },
            'merge-bin': {
                'callback': merge_bin,
                'options': [
                    {
                        'names': ['--output', '-o'],
                        'help': ('Output filename'),
                        'type': click.Path(),
                    },
                    {
                        'names': ['--format', '-f'],
                        'help': ('Format of the output file'),
                        'type': click.Choice(['hex', 'uf2', 'raw']),
                        'default': 'raw',
                    },
                    {
                        'names': ['--md5-disable'],
                        'is_flag': True,
                        'help': ('[ONLY UF2] Disable MD5 checksum in UF2 output.'),
                    },
                    {
                        'names': ['--flash-offset', '-t'],
                        'help': ('[ONLY RAW] Flash offset where the output file will be flashed.'),
                    },
                    {
                        'names': ['--fill-flash-size'],
                        'help': ('[ONLY RAW] If set, the final binary file will be padded with FF bytes up to this flash size.'),
                        'type': click.Choice(['256KB', '512KB', '1MB', '2MB', '4MB', '8MB', '16MB', '32MB', '64MB', '128MB']),
                    },
                ],
                'dependencies': ['all'],  # all = build
            },
            'secure-decrypt-flash-data': {
                'callback': secure_decrypt_flash_data,
                'options': [
                    {
                        'names': ['--aes-xts', '-x'],
                        'is_flag': True,
                        'help': ('Decrypt data using AES-XTS.'),
                    },
                    {
                        'names': ['--keyfile', '-k'],
                        'help': ('File with flash encryption key.'),
                    },
                    {
                        'names': ['--output', '-o'],
                        'help': ('Output file for plaintext data.'),
                    },
                    {
                        'names': ['--address', '-a'],
                        'help': ('Address offset in flash that file was read from.'),
                    },
                    {
                        'names': ['--flash-crypt-conf'],
                        'help': ('Override FLASH_CRYPT_CONF efuse value (default is 0XF).'),
                    },
                ],
                'arguments': [
                    {
                        'names': ['encrypted_file'],
                        'nargs': 1,
                    },
                ],
            },
            'secure-digest-secure-bootloader': {
                'callback': secure_digest_secure_bootloader,
                'help': ('Take a bootloader binary image and a secure boot key, and output a combined'
                         'digest+binary suitable for flashing along with the precalculated secure boot key.'),
                'options': [
                    {
                        'names': ['--keyfile', '-k'],
                        'help': ('256 bit key for secure boot digest.'),
                    },
                    {
                        'names': ['--output', '-o'],
                        'help': ('Output file for signed digest image.'),
                    },
                    {
                        'names': ['--iv'],
                        'help': (
                            '128 byte IV file. Supply a file for testing purposes only, if not supplied an IV will be randomly generated.'
                        ),
                    },
                ],
                'arguments': [
                    {
                        'names': ['image'],
                        'nargs': 1,
                    },
                ],

            },
            'secure-encrypt-flash-data': {
                'callback': secure_encrypt_flash_data,
                'help': 'Encrypt some data suitable for encrypted flash (using known key).',
                'options': [
                    {
                        'names': ['--aes-xts', '-x'],
                        'is_flag': True,
                        'help': (
                            'Encrypt data using AES-XTS if chip supports it.'
                        ),
                    },
                    {
                        'names': ['--keyfile', '-k'],
                        'help': ('File with flash encryption key.'),
                    },
                    {
                        'names': ['--output', '-o'],
                        'help': ('Output file for encrypted data.'),
                    },
                    {
                        'names': ['--address', '-a'],
                        'help': ('Address offset in flash where file will be flashed.'),
                    },
                    {
                        'names': ['--flash-crypt-conf'],
                        'help': (
                            'Override FLASH_CRYPT_CONF eFuse value (default is 0XF).'
                        ),
                    },
                ],
                'arguments': [
                    {
                        'names': ['plaintext_file'],
                        'nargs': 1,
                    },
                ],
            },
            'secure-generate-flash-encryption-key': {
                'callback': secure_generate_flash_encryption_key,
                'options': [
                    {
                        'names': ['--keylen', '-l'],
                        'help': (
                            'Length of private key digest file to generate (in bits). 3/4 Coding Scheme requires 192 bit key.'
                        ),
                    },
                ],
                'arguments': [
                    {
                        'names': ['keyfile'],
                        'nargs': 1,
                    },
                ],
            },
            'secure-generate-signing-key': {
                'callback': secure_generate_signing_key,
                'help': ('Generate a private key for signing secure boot images as per the secure boot version. Key file is generated in PEM'
                         'format, Secure Boot V1 - ECDSA NIST256p private key. Secure Boot V2 - RSA 3072, ECDSA NIST256p, ECDSA NIST192p'
                         'private key.'),
                'options': [
                    {
                        'names': ['--version', '-v'],
                        'help': ('Version of the secure boot signing scheme to use.'),
                        'type': click.Choice(['1', '2']),
                        'default': '2',
                    },
                    {
                        'names': ['--scheme', '-s'],
                        'help': ('Scheme of secure boot signing.'),
                        'type': click.Choice(['rsa3072', 'ecdsa192', 'ecdsa256']),
                    },
                ],
                'arguments': [
                    {
                        'names': ['keyfile'],
                        'nargs': 1,
                    },
                ],
            },
            'secure-sign-data': {
                'callback': secure_sign_data,
                'help': ('Sign a data file for use with secure boot. Signing algorithm is deterministic ECDSA w/ SHA-512 (V1) or either RSA-'
                         'PSS or ECDSA w/ SHA-256 (V2).'),
                'options': [
                    {
                        'names': ['--version', '-v'],
                        'help': ('Version of the secure boot signing scheme to use.'),
                        'type': click.Choice(['1', '2']),
                        'default': '2',
                    },
                    {
                        'names': ['--keyfile', '-k'],
                        'help': ('Private key file for signing. Key is in PEM format.'),
                    },
                    {
                        'names': ['--append-signatures', '-a'],
                        'is_flag': True,
                        'help': (
                            'Append signature block(s) to already signed image. Valid only for ESP32-S2.'
                        ),
                    },
                    {
                        'names': ['--pub-key'],
                        'help': (
                            'Public key files corresponding to the private key used to generate the pre-calculated signatures. Keys should be in PEM format.'
                        ),
                    },
                    {
                        'names': ['--signature'],
                        'help': (
                            'Pre-calculated signatures. Signatures generated using external private keys e.g. keys stored in HSM.'
                        ),
                    },
                    {
                        'names': ['--output', '-o'],
                        'help': (
                            'Output file for signed digest image. Default is to sign the input file.'
                        ),
                    },
                ],
                'arguments': [
                    {
                        'names': ['datafile'],
                        'nargs': 1,
                    },
                ],
            },
            'secure-verify-signature': {
                'callback': secure_verify_signature,
                'help': ('Verify a previously signed binary image, using the ECDSA (V1) or either RSA or ECDSA (V2) public key.'),
                'options': [
                    {
                        'names': ['--version', '-v'],
                        'help': ('Version of the secure boot signing scheme to use.'),
                        'type': click.Choice(['1', '2']),
                        'default': '2',
                    },
                    {
                        'names': ['--keyfile', '-k'],
                        'help': ('Public key file for verification. Can be private or public key in PEM format.'),
                    },
                ],
                'arguments': [
                    {
                        'names': ['datafile'],
                        'nargs': 1,
                    },
                ],
            },
            'efuse-burn': {
                'callback': efuse_burn,
                'help': 'Burn the eFuse with the specified name.',
                'options': EFUSE_OPTS,
                'arguments': [
                    {
                        'names': ['efuse-positional-args'],
                        'nargs': -1,
                    },
                ],
            },
            'efuse-burn-key': {
                'callback': efuse_burn_key,
                'help': 'Burn a 256-bit key to EFUSE: BLOCK1, flash_encryption, BLOCK2, secure_boot_v1, secure_boot_v2, BLOCK3.',
                'options': EFUSE_OPTS + [
                    {
                        'names': ['--no-protect-key'],
                        'is_flag': True,
                        'help': (
                            'Disable default read- and write-protecting of the key.'
                            'If this option is not set, once the key is flashed it cannot be read back or changed.'
                        ),
                    },
                    {
                        'names': ['--force-write-always'],
                        'is_flag': True,
                        'help': (
                            "Write the eFuse even if it looks like it's already been written, or is write protected."
                            "Note that this option can't disable write protection, or clear any bit which has already been set."
                        ),
                    },
                    {
                        'names': ['--show-sensitive-info'],
                        'is_flag': True,
                        'help': (
                            'Show data to be burned (may expose sensitive data). Enabled if --debug is used.'
                        ),
                    },
                ],
                'arguments': [
                    {
                        'names': ['efuse-positional-args'],
                        'nargs': -1,
                    },
                ],
            },
            'efuse-dump': {
                'callback': efuse_dump,
                'help': 'Dump raw hex values of all eFuses.',
                'options': EFUSE_OPTS + [
                    {
                        'names': ['--file-name'],
                        'help': (
                            'Saves dump for each block into separate file. Provide the common path name /path/blk.bin, it will create:'
                            ' blk0.bin, blk1.bin ... blkN.bin. Use burn_block_data to write it back to another chip.'
                        ),
                    },
                ],
            },
            'efuse-read-protect': {
                'callback': efuse_read_protect,
                'help': 'Disable writing to the eFuse with the specified name.',
                'options': EFUSE_OPTS,
                'arguments': [
                    {
                        'names': ['efuse-positional-args'],
                        'nargs': -1,
                    },
                ],
            },
            'efuse-summary': {
                'callback': efuse_summary,
                'help': 'Get the summary of the eFuses.',
                'options': EFUSE_OPTS + [
                    {
                        'names': ['--format'],
                        'help': ('Summary format.'),
                        'type': click.Choice(['json', 'summary', 'value-only']),
                    },
                ],
                'arguments': [
                    {
                        'names': ['efuse-name'],
                        'nargs': 1,
                        'required': False,
                    },
                ],
            },
            'efuse-write-protect': {
                'callback': efuse_write_protect,
                'help': 'Disable writing to the eFuse with the specified name.',
                'options': EFUSE_OPTS,
                'arguments': [
                    {
                        'names': ['efuse-positional-args'],
                        'nargs': -1,
                    },
                ],
            },
            'monitor': {
                'callback': monitor,
                'help': 'Display serial output.',
                'options': [
                    PORT,
                    {
                        'names': ['--print-filter', '--print_filter'],
                        'help': (
                            'Filter monitor output. '
                            'Restrictions on what to print can be specified as a series of <tag>:<log_level> items '
                            'where <tag> is the tag string and <log_level> is a character from the set '
                            '{N, E, W, I, D, V, *} referring to a level. '
                            'For example, "tag1:W" matches and prints only the outputs written with '
                            'ESP_LOGW("tag1", ...) or at lower verbosity level, i.e. ESP_LOGE("tag1", ...). '
                            'Not specifying a <log_level> or using "*" defaults to Verbose level. '
                            'Please see the IDF Monitor section of the ESP-IDF documentation '
                            'for a more detailed description and further examples.'
                        ),
                        'default': None,
                    },
                    {
                        'names': ['--monitor-baud', '-b'],
                        'type': click.INT,
                        'help': (
                            'Baud rate for monitor. '
                            'If this option is not provided IDF_MONITOR_BAUD and MONITORBAUD '
                            'environment variables, global baud rate and project_description.json in build directory '
                            "(generated by CMake from project's sdkconfig) "
                            'will be checked for default value.'
                        ),
                    },
                    {
                        'names': ['--encrypted', '-E'],
                        'is_flag': True,
                        'help': (
                            'Enable encrypted flash targets. '
                            'IDF Monitor will invoke encrypted-flash and encrypted-app-flash targets '
                            'if this option is set. This option is set by default if IDF Monitor was invoked '
                            'together with encrypted-flash or encrypted-app-flash target.'
                        ),
                    },
                    {
                        'names': ['--no-reset'],
                        'is_flag': True,
                        'help': (
                            'Disable reset on monitor startup. '
                            'IDF Monitor will not reset the MCU target by toggling DTR/RTS lines on startup '
                            'if this option is set. '
                            'This option only works if --port argument is specified.'
                        ),
                    },
                    {
                        'names': ['--timestamps'],
                        'is_flag': True,
                        'help': 'Print a time stamp in the beginning of each line.',
                    },
                    {
                        'names': ['--timestamp-format'],
                        'help': (
                            'Set the formatting of timestamps compatible with strftime(). '
                            'For example, "%Y-%m-%d %H:%M:%S".'
                        ),
                        'default': None,
                    },
                    {
                        'names': ['--force-color'],
                        'is_flag': True,
                        'help': 'Always print ANSI for colors',
                    },
                    {
                        'names': ['--disable-auto-color'],
                        'is_flag': True,
                        'help': 'Disable auto coloring logs',
                    },
                ],
                'order_dependencies': [
                    'flash',
                    'encrypted-flash',
                    'partition-table-flash',
                    'bootloader-flash',
                    'app-flash',
                    'encrypted-app-flash',
                ],
            },
            'partition-table-flash': {
                'callback': flash,
                'help': 'Flash partition table only.',
                'options': flash_options,
                'order_dependencies': ['partition-table', 'erase-flash'],
            },
            'bootloader-flash': {
                'callback': flash,
                'help': 'Flash bootloader only.',
                'options': flash_options,
                'order_dependencies': ['bootloader', 'erase-flash'],
            },
            'app-flash': {
                'callback': flash,
                'help': 'Flash the app only.',
                'options': flash_options,
                'order_dependencies': ['app', 'erase-flash'],
            },
            'encrypted-app-flash': {
                'callback': flash,
                'help': 'Flash the encrypted app only.',
                'options': flash_options,
                'order_dependencies': ['app', 'erase-flash'],
            },
            'encrypted-flash': {
                'callback': flash,
                'help': 'Flash the encrypted project.',
                'options': flash_options,
                'order_dependencies': ['all', 'erase-flash'],
            },

            'erase-otadata': {
                'callback': ota_targets,
                'help': 'Erase otadata partition.',
                'options': global_options + BAUD_AND_PORT,
            },
            'read-otadata': {
                'callback': ota_targets,
                'help': 'Read otadata partition.',
                'options': global_options + BAUD_AND_PORT,
            },
        },
    }

    return serial_actions
