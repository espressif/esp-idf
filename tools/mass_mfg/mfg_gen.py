#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import csv
import os
from itertools import zip_longest
from pathlib import Path
from types import SimpleNamespace

import esp_idf_nvs_partition_gen.nvs_partition_gen as nvs_partition_gen
import rich_click as click
from esp_pylib.excepthook import install_exception_reporting
from esp_pylib.logger import log
from rich.markup import escape


def create_temp_files(args):
    new_filenames = []
    for filename in [args.conf, args.values]:
        name, ext = os.path.splitext(filename)
        new_filename = name + '_tmp' + ext
        strip_blank_lines(filename, new_filename)
        new_filenames.append(new_filename)
    return new_filenames


def strip_blank_lines(input_filename, output_filename):
    with (
        open(input_filename, encoding='utf-8') as read_from,
        open(output_filename, 'w', newline='', encoding='utf-8') as write_to,
    ):
        for line in read_from:
            if not line.isspace():
                write_to.write(line)


def verify_values_exist(input_values_file, keys_in_values_file):
    """Verify all keys have corresponding values in values file"""
    with open(input_values_file, encoding='utf-8') as values_file:
        values_file_reader = csv.reader(values_file, delimiter=',')
        next(values_file_reader)

        for line_num, line in enumerate(values_file_reader, start=2):
            key_count_in_values_file = len(keys_in_values_file)
            if len(line) != key_count_in_values_file:
                log.die(
                    escape(
                        f'Number of values is not equal to number of keys in file: '
                        f'{input_values_file} at line No:{line_num}'
                    )
                )


def verify_keys_exist(values_file_keys, input_config_file):
    """Verify all keys from config file are present in values file"""
    keys_missing = []

    with open(input_config_file, encoding='utf-8') as config_file:
        config_file_reader = csv.reader(config_file, delimiter=',')

        for line_num, line in enumerate(config_file_reader, start=1):
            if 'namespace' not in line:
                if values_file_keys:
                    if line[0] == values_file_keys[0]:
                        del values_file_keys[0]
                    else:
                        keys_missing.append([line_num, line[0]])
                else:
                    keys_missing.append([line_num, line[0]])

        if keys_missing:
            for line_num, key in keys_missing:
                log.err(escape(f'Key:` {key} ` at line no: {line_num}  in config file is not found in values file.'))
            log.die('One or more keys from config file are missing in values file.')


def verify_datatype_encoding(input_config_file):
    """Verify datatype and encodings from config file is valid"""
    valid_encodings = {'string', 'binary', 'hex2bin', 'u8', 'i8', 'u16', 'u32', 'i32', 'u64', 'i64', 'base64'}
    valid_datatypes = {'file', 'data', 'namespace'}

    with open(input_config_file, encoding='utf-8') as config_file:
        config_file_reader = csv.reader(config_file, delimiter=',')

        for line_num, line in enumerate(config_file_reader, start=1):
            if line[1] not in valid_datatypes:
                log.die(escape(f'config file: {input_config_file} has invalid datatype at line no:{line_num}'))
            if 'namespace' not in line and line[2] not in valid_encodings:
                log.die(escape(f'config file: {input_config_file} has invalid encoding at line no:{line_num}'))


def verify_file_data_count(input_config_file, keys_repeat):
    """Verify count of data on each line in config file is equal to 3
    (as format must be: <key,type and encoding>)
    """
    with open(input_config_file, encoding='utf-8') as config_file:
        config_file_reader = csv.reader(config_file, delimiter=',')

        for line_num, line in enumerate(config_file_reader, start=1):
            if len(line) != 3 and line[0] not in keys_repeat:
                log.die(f'data missing in config file at line no:{line_num} <format needed:key,type,encoding>')


def verify_data_in_file(input_config_file, input_values_file, config_file_keys, keys_in_values_file, keys_repeat):
    """ Verify count of data on each line in config file is equal to 3 \
    (as format must be: <key,type and encoding>)
    Verify datatype and encodings from config file is valid
    Verify all keys from config file are present in values file and \
    Verify each key has corresponding value in values file
    """
    try:
        verify_file_data_count(input_config_file, keys_repeat)

        verify_datatype_encoding(input_config_file)

        # Get keys from values file present in config files
        values_file_keys = get_keys(keys_in_values_file, config_file_keys)

        verify_keys_exist(values_file_keys, input_config_file)

        verify_values_exist(input_values_file, keys_in_values_file)

    except Exception as err:
        log.die(escape(str(err)))


def get_keys(keys_in_values_file, config_file_keys):
    """Get keys from values file present in config file"""
    values_file_keys = [key for key in keys_in_values_file if key in config_file_keys]
    return values_file_keys


def add_config_data_per_namespace(input_config_file):
    """Add config data per namespace to `config_data_to_write` list"""
    config_data_to_write = []
    config_data_per_namespace = []

    with open(input_config_file, encoding='utf-8') as csv_config_file:
        config_file_reader = csv.reader(csv_config_file, delimiter=',')

        # `config_data_per_namespace` is added to `config_data_to_write` list after reading next namespace
        for config_data in config_file_reader:
            if 'REPEAT' in config_data:
                config_data.remove('REPEAT')
            if 'namespace' in config_data:
                if config_data_per_namespace:
                    config_data_to_write.append(config_data_per_namespace)
                    config_data_per_namespace = []
                    config_data_per_namespace.append(config_data)
                else:
                    config_data_per_namespace.append(config_data)
            else:
                config_data_per_namespace.append(config_data)

        # `config_data_per_namespace` is added to `config_data_to_write` list as EOF is reached
        if (not config_data_to_write) or (config_data_to_write and config_data_per_namespace):
            config_data_to_write.append(config_data_per_namespace)

    return config_data_to_write


def get_fileid_val(file_identifier, key_value_data, fileid_value):
    """Get file identifier value"""
    file_id_found = False

    for key in key_value_data:
        if file_identifier and not file_id_found and file_identifier in key:
            fileid_value = key[1]
            file_id_found = True

    if not file_id_found:
        fileid_value = str(int(fileid_value) + 1)

    return fileid_value


def add_data_to_file(config_data_to_write, key_value_pair, output_csv_file):
    """Add data to csv target file"""
    header = ['key', 'type', 'encoding', 'value']
    data_to_write = []

    with open(output_csv_file, 'w', newline='', encoding='utf-8') as target_csv_file:
        output_file_writer = csv.writer(target_csv_file, delimiter=',')
        output_file_writer.writerow(header)

        for namespace_config_data in config_data_to_write:
            for data in namespace_config_data:
                data_to_write = data[:]
                if 'namespace' in data:
                    data_to_write.append('')
                    output_file_writer.writerow(data_to_write)
                else:
                    key = data[0]
                    while key not in key_value_pair[0]:
                        del key_value_pair[0]
                    if key in key_value_pair[0]:
                        value = key_value_pair[0][1]
                        data_to_write.append(value)
                        del key_value_pair[0]
                        output_file_writer.writerow(data_to_write)


def create_dir(filetype, output_dir_path):
    """Create new directory(if doesn't exist) to store file generated"""
    output_target_dir = os.path.join(output_dir_path, filetype, '')
    if not os.path.isdir(output_target_dir):
        Path(output_target_dir).mkdir(parents=True)

    return output_target_dir


def set_repeat_value(total_keys_repeat, keys, csv_file, target_filename):
    with (
        open(csv_file, encoding='utf-8') as read_from,
        open(target_filename, 'w', newline='', encoding='utf-8') as write_to,
    ):
        csv_file_reader = csv.reader(read_from, delimiter=',')
        headers = next(csv_file_reader)
        values = next(csv_file_reader)
        csv_file_writer = csv.writer(write_to, delimiter=',')
        csv_file_writer.writerow(headers)
        csv_file_writer.writerow(values)

        # read new data, add value if key has repeat tag, write to new file
        for row in csv_file_reader:
            key_val_new = list(zip_longest(keys, row))
            key_val_pair = list(zip_longest(keys, values))
            key_repeated = total_keys_repeat[:]
            index = 0
            while key_val_new and key_repeated:
                #  if key has repeat tag, get its corresponding value, write to file
                if key_val_new[0][0] == key_repeated[0]:
                    row[index] = key_val_pair[0][1]
                    del key_repeated[0]
                del key_val_new[0]
                del key_val_pair[0]
                index += 1
            csv_file_writer.writerow(row)

    return target_filename


def create_intermediate_csv(args, keys_in_values_file, keys_repeat, is_encr=False):
    file_identifier_value = '0'
    # Add config data per namespace to `config_data_to_write` list
    config_data_to_write = add_config_data_per_namespace(args.conf)

    try:
        with open(args.values, encoding='utf-8') as csv_values_file:
            values_file_reader = csv.reader(csv_values_file, delimiter=',')
            keys = next(values_file_reader)

        filename, file_ext = os.path.splitext(args.values)
        target_filename = filename + '_created' + file_ext
        if keys_repeat:
            target_values_file = set_repeat_value(keys_repeat, keys, args.values, target_filename)
        else:
            target_values_file = args.values

        with open(target_values_file, encoding='utf-8') as csv_values_file:
            values_file_reader = csv.reader(csv_values_file, delimiter=',')
            next(values_file_reader)

            # Create new directory (if doesn't exist) to store csv file generated
            output_csv_target_dir = create_dir('csv', args.outdir)
            # Create new directory (if doesn't exist) to store bin file generated
            output_bin_target_dir = create_dir('bin', args.outdir)

            if args.prefix_num:
                prefix_num_start, prefix_num_digits = args.prefix_num

            for values_data_line in values_file_reader:
                key_value_data = list(zip_longest(keys_in_values_file, values_data_line))

                # Get file identifier value from values file
                file_identifier_value = get_fileid_val(args.fileid, key_value_data, file_identifier_value)

                key_value_pair = key_value_data[:]

                if args.prefix_num:
                    # Create file name prefix based on user supplied start and length
                    prefix_number = f'{prefix_num_start:0{prefix_num_digits}}'
                    csv_filename = args.prefix + '-' + prefix_number + '-' + file_identifier_value + '.' + 'csv'
                    bin_filename = args.prefix + '-' + prefix_number + '-' + file_identifier_value + '.' + 'bin'
                    if args.keygen:
                        args.keyfile = 'keys-' + args.prefix + '-' + prefix_number + '-' + file_identifier_value
                    prefix_num_start += 1
                else:
                    csv_filename = args.prefix + '-' + file_identifier_value + '.' + 'csv'
                    bin_filename = args.prefix + '-' + file_identifier_value + '.' + 'bin'
                    if args.keygen:
                        args.keyfile = 'keys-' + args.prefix + '-' + file_identifier_value

                output_csv_file = output_csv_target_dir + csv_filename
                # Verify if output csv file does not exist
                if os.path.isfile(output_csv_file):
                    log.die(escape(f'Target csv file: {output_csv_file} already exists.'))

                # Add values corresponding to each key to csv intermediate file
                add_data_to_file(config_data_to_write, key_value_pair, output_csv_file)
                log.note(escape(f'Created CSV file: ===> {output_csv_file}'))

                output_bin_file = output_bin_target_dir + bin_filename
                # Verify if output bin file does not exist
                if os.path.isfile(output_bin_file):
                    log.die(escape(f'Target binary file: {output_bin_file} already exists.'))

                args.input = output_csv_file
                args.output = os.path.join('bin', bin_filename)

                if is_encr:
                    nvs_partition_gen.encrypt(args)
                else:
                    nvs_partition_gen.generate(args)

            log.note(escape(f'Files generated in {args.outdir} ...'))

    except Exception as e:
        log.die(escape(str(e)))


def verify_file_format(args):
    keys_in_config_file = []
    keys_in_values_file = []
    keys_repeat = []

    # Verify files have csv extension
    conf_name, conf_extension = os.path.splitext(args.conf)
    if conf_extension != '.csv':
        log.die(escape(f'config file: {args.conf} does not have the .csv extension.'))
    values_name, values_extension = os.path.splitext(args.values)
    if values_extension != '.csv':
        log.die(escape(f'values file: {args.values} does not have the .csv extension.'))

    # Verify files are not empty
    if os.stat(args.conf).st_size == 0:
        log.die(escape(f'config file: {args.conf} is empty.'))
    if os.stat(args.values).st_size == 0:
        log.die(escape(f'values file: {args.values} is empty.'))

    # Extract keys from config file
    with open(args.conf, encoding='utf-8') as config_file:
        config_file_reader = csv.reader(config_file, delimiter=',')
        for config_data in config_file_reader:
            if 'namespace' not in config_data:
                keys_in_config_file.append(config_data[0])
            if 'REPEAT' in config_data:
                keys_repeat.append(config_data[0])

    # Extract keys from values file
    with open(args.values, encoding='utf-8') as values_file:
        values_file_reader = csv.reader(values_file, delimiter=',')
        keys_in_values_file = next(values_file_reader)

    # Verify file identifier exists in values file
    if args.fileid:
        if args.fileid not in keys_in_values_file:
            log.die(escape(f'target_file_identifier: {args.fileid} does not exist in values file.'))
    else:
        args.fileid = 1

    return keys_in_config_file, keys_in_values_file, keys_repeat


def generate(args):
    # Create work files with no blank lines
    args.conf, args.values = create_temp_files(args)

    # Verify input config and values file format
    keys_in_config_file, keys_in_values_file, keys_repeat = verify_file_format(args)

    # Verify data in the input_config_file and input_values_file
    verify_data_in_file(args.conf, args.values, keys_in_config_file, keys_in_values_file, keys_repeat)

    encryption_enabled = False
    if args.keygen or args.inputkey:
        encryption_enabled = True
        log.note('Generating encrypted NVS binary images...')
    # Create intermediate csv file
    create_intermediate_csv(args, keys_in_values_file, keys_repeat, is_encr=encryption_enabled)


def generate_key(args):
    nvs_partition_gen.generate_key(args)


HMAC_PROTECT_HELP = (
    'If set, the NVS encryption key protection scheme based on HMAC peripheral is used; '
    'else the default scheme based on Flash Encryption is used'
)
HMAC_KEYGEN_HELP = 'Generate the HMAC key for HMAC-based encryption scheme'
HMAC_KEYFILE_HELP = 'Path to output HMAC key file'
HMAC_INPUTKEY_HELP = 'File having the HMAC key for generating the NVS encryption keys'


def add_outdir_and_hmac_options(function):
    """Add the output directory and HMAC key protection options shared by generate and generate-key."""
    options = [
        click.option(
            '--outdir',
            default=os.getcwd(),
            show_default='current directory',
            help='Output directory to store files created',
        ),
        click.option('--key_protect_hmac', is_flag=True, help=HMAC_PROTECT_HELP),
        click.option('--kp_hmac_keygen', is_flag=True, help=HMAC_KEYGEN_HELP),
        click.option('--kp_hmac_keyfile', default=None, help=HMAC_KEYFILE_HELP),
        click.option('--kp_hmac_inputkey', default=None, help=HMAC_INPUTKEY_HELP),
    ]
    for option in reversed(options):
        function = option(function)
    return function


@click.group(context_settings={'help_option_names': ['-h', '--help']}, help='ESP Manufacturing Utility')
def cli():
    pass


@cli.command(
    'generate',
    help=(
        'Generate NVS partition\n\n'
        'CONF: Path to configuration csv file to parse\n\n'
        'VALUES: Path to values csv file to parse\n\n'
        'PREFIX: Unique name for each output filename prefix\n\n'
        'SIZE: Size of NVS partition in bytes (must be multiple of 4096)'
    ),
)
@click.argument('conf')
@click.argument('values')
@click.argument('prefix')
@click.argument('size')
@click.option(
    '--prefix_num',
    type=int,
    nargs=2,
    metavar='START LENGTH',
    help='Prefix number (counter) start and length (in digits) to be added for each output filename',
)
@click.option(
    '--fileid',
    default=None,
    help='Unique file identifier (any key in values file) for each filename suffix (Default: numeric value(1,2,3...)',
)
@click.option(
    '--version',
    type=click.IntRange(1, 2),
    default=2,
    show_default=True,
    help='Set multipage blob version. Version 1 - Multipage blob support disabled. '
    'Version 2 - Multipage blob support enabled.',
)
@click.option('--keygen', is_flag=True, help='Generates key for encrypting NVS partition')
@click.option('--keyfile', default=None, hidden=True)
@click.option('--inputkey', default=None, help='File having key for encrypting NVS partition')
@add_outdir_and_hmac_options
@click.option('--input', 'input_file', default=None, hidden=True)
@click.option('--output', 'output_file', default=None, hidden=True)
def generate_cmd(input_file, output_file, **kwargs):
    # nvs_partition_gen consumes an argparse-style namespace, so the CLI still builds one.
    generate(SimpleNamespace(input=input_file, output=output_file, **kwargs))


@cli.command('generate-key', help='Generate keys for encryption')
@click.option('--keyfile', default=None, help='Path to output encryption keys file')
@add_outdir_and_hmac_options
def generate_key_cmd(**kwargs):
    generate_key(SimpleNamespace(**kwargs))


def main():
    try:
        cli()
    except ValueError as err:
        log.die(escape(str(err)))
    except Exception as e:
        log.die(escape(str(e)))


if __name__ == '__main__':
    install_exception_reporting()
    main()
