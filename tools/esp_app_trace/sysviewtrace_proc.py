#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# This is python script to process various types trace data streams in SystemView format.
# Trace data can be provided in multiple trace files (one per CPU). After processing phase
# script prints report for every type of trace data stream which was found.
#

import json
import os.path
import signal
import sys
import tempfile
from urllib.parse import urlparse

import espytrace.apptrace as apptrace
import espytrace.sysview as sysview
import rich_click as click
from esp_pylib.excepthook import install_exception_reporting
from esp_pylib.logger import Verbosity
from esp_pylib.logger import log
from rich.markup import escape

VERBOSITY_LEVELS = [Verbosity.SILENT, Verbosity.SILENT, Verbosity.NORMAL, Verbosity.VERBOSE, Verbosity.VERBOSE]


def is_segger_multicore_format(file_path):
    """Check if the file has offsets in header"""
    try:
        url = urlparse(file_path)
        if len(url.scheme) == 0 or url.scheme == 'file':
            with open(url.path, 'rb') as f:
                header = f.read(200)
                header_str = header.decode('utf-8', errors='ignore')
                if (
                    '; Version     SEGGER SystemViewer' in header_str
                    and '; Author      Espressif Inc' in header_str
                    and '; Offset Core0' in header_str
                    and '; Offset Core1' in header_str
                ):
                    return True
    except Exception as e:
        log.err(escape(f'Error checking SEGGER multicore file format: {e}'))
    return False


def split_segger_multicore_file(file_path):
    """Split SEGGER multicore file into separate core files."""
    try:
        with open(urlparse(file_path).path, 'rb') as f:
            # Read first few lines to get offsets for each core
            header = f.read(200)
            header_str = header.decode('utf-8', errors='ignore')

            core1_offset = None
            for line in header_str.split('\n'):
                if '; Offset Core1' in line:
                    core1_offset = int(line.strip().split()[-1])

            if core1_offset is None:
                log.err('Failed to parse core1 offset')
                return None, None

            # Read the entire file
            f.seek(0)
            data = f.read()

            # Find first 10 sync bytes start offset
            sync_start = data.find(b'\x00' * 10)
            if sync_start == -1:
                log.err('Sync bytes not found')
                return None, None

            core0_offset = sync_start
            core1_offset += sync_start

            # Parse original header and get version from there, if not found, use default version
            version = 'V3.60'
            for line in header_str.split('\n'):
                if '; Version     SEGGER SystemViewer' in line:
                    version = line.strip().split()[-1]
                    break

            # Rebuild header without offset lines
            core_header = f';\n; Version     SEGGER SystemViewer {version}\n; Author      Espressif Inc\n;\n'
            core_header = core_header.encode('utf-8')
            core_base = core_header + b'\x00' * 10

            core0_data = core_base + data[core0_offset:core1_offset]
            core1_data = core_base + data[core1_offset:]

            core0_file = tempfile.NamedTemporaryFile(delete=False, suffix='.svdat')
            core1_file = tempfile.NamedTemporaryFile(delete=False, suffix='.svdat')

            core0_file.write(core0_data)
            core1_file.write(core1_data)

            core0_file.close()
            core1_file.close()

            return core0_file.name, core1_file.name

    except Exception as e:
        log.err(escape(f'Failed to process files: {e}'))
        return None, None


@click.command(
    context_settings={'help_option_names': ['-h', '--help']},
    help=(
        'ESP32 SEGGER SystemView Trace Parsing Tool\n\n'
        'TRACE_SOURCES: Trace data sources. Format: [file://]/path/to/file.'
    ),
)
@click.argument('trace_sources', nargs=-1, required=True)
@click.option('--elf-file', '-b', default='', help='Path to program ELF file.')
@click.option('--tmo', '-w', type=int, default=0, help='Data wait timeout in sec. -1: infinite, 0: no wait')
@click.option('--dump-events', '-d', is_flag=True, help='Dump all events.')
@click.option(
    '--print-events',
    '-p',
    is_flag=True,
    help='Print events of selected types. By default only reports are printed',
)
@click.option(
    '--include-events',
    '-i',
    type=click.Choice(['heap', 'log', 'func', 'all']),
    default='all',
    help='Events types to be included into report.',
)
@click.option('--toolchain', '-t', default='xtensa-esp32-elf-', help='Toolchain prefix.')
@click.option(
    '--events-map',
    '-e',
    default=os.path.join(os.path.dirname(__file__), 'SYSVIEW_FreeRTOS.txt'),
    help='Events map file.',
)
@click.option('--to-json', '-j', is_flag=True, default=False, help='Print JSON.')
@click.option(
    '--verbose',
    '-v',
    type=click.IntRange(0, len(VERBOSITY_LEVELS) - 1),
    default=1,
    help='Verbosity level. Default 1',
)
def main(
    trace_sources, elf_file, tmo, dump_events, print_events, include_events, toolchain, events_map, to_json, verbose
):
    def sig_int_handler(signum, frame):
        reader.cleanup()

    signal.signal(signal.SIGINT, sig_int_handler)

    selected_events = {'heap': False, 'log': False, 'func': False}
    if include_events == 'all':
        for k in selected_events:
            selected_events[k] = True
    else:
        selected_events[include_events] = True

    log.set_verbosity(VERBOSITY_LEVELS[verbose])
    if to_json:
        log.set_info_stream(sys.stderr)

    trace_sources = list(trace_sources)
    temp_files = []
    # Only check for SEGGER format if there's exactly one trace source
    if len(trace_sources) == 1:
        trace_source = trace_sources[0]
        if is_segger_multicore_format(trace_source):
            core0_file, core1_file = split_segger_multicore_file(trace_source)
            if core0_file and core1_file:
                temp_files.extend([core0_file, core1_file])
                trace_sources = temp_files
            else:
                log.die('Failed to split SEGGER multicore trace file', exit_code=2)

    # parse trace files
    parsers = []
    for i, trace_source in enumerate(trace_sources):
        try:
            parser = sysview.SysViewMultiTraceDataParser(print_events=False, core_id=i)
            if selected_events['heap']:
                parser.add_stream_parser(
                    sysview.SysViewTraceDataParser.STREAMID_HEAP,
                    sysview.SysViewHeapTraceDataParser(print_events=False, core_id=i),
                )
            if selected_events['log']:
                parser.add_stream_parser(
                    sysview.SysViewTraceDataParser.STREAMID_LOG,
                    sysview.SysViewLogTraceDataParser(print_events=False, core_id=i),
                )
            if selected_events['func']:
                parser.add_stream_parser(
                    sysview.SysViewTraceDataParser.STREAMID_FUNC,
                    sysview.SysViewFunctionTraceDataParser(print_events=False, core_id=i),
                )
            parsers.append(parser)
        except Exception as e:
            log.die(escape(f'Failed to create data parser ({e})!'), exit_code=2)
        reader = apptrace.reader_create(trace_source, tmo)
        if not reader:
            log.die('Failed to create trace reader!', exit_code=2)
        try:
            log.debug(escape(f"Parse trace from '{trace_source}'..."))
            sysview.parse_trace(reader, parser, events_map)
            log.debug('Parsing completed.')
        except (apptrace.ReaderTimeoutError, apptrace.ReaderShutdownRequest) as e:
            log.debug(escape(f'Stop parsing trace. ({e})'))
        except Exception as e:
            parser.cleanup()
            log.die(escape(f'Failed to parse trace ({e})!'), exit_code=2)
        finally:
            reader.cleanup()

    # merge and process traces
    try:
        proc = sysview.SysViewMultiStreamTraceDataProcessor(
            traces=parsers, print_events=dump_events, keep_all_events=True if to_json else False
        )
        if selected_events['heap']:
            proc.add_stream_processor(
                sysview.SysViewTraceDataParser.STREAMID_HEAP,
                sysview.SysViewHeapTraceDataProcessor(
                    toolchain, elf_file, root_proc=proc, print_heap_events=print_events
                ),
            )
        if selected_events['log']:
            proc.add_stream_processor(
                sysview.SysViewTraceDataParser.STREAMID_LOG,
                sysview.SysViewLogTraceDataProcessor(root_proc=proc, print_log_events=print_events),
            )
        if selected_events['func']:
            proc.add_stream_processor(
                sysview.SysViewTraceDataParser.STREAMID_FUNC,
                sysview.SysViewFunctionTraceDataProcessor(
                    toolchain, elf_file, root_proc=proc, print_func_events=print_events
                ),
            )
    except Exception as e:
        log.die(escape(f'Failed to create data processor ({e})!'), exit_code=2)

    try:
        log.debug(escape(f"Process events from '{trace_sources}'..."))
        proc.merge_and_process()
        log.debug('Processing completed.')
    except Exception as e:
        log.die(escape(f'Failed to process trace ({e})!'), exit_code=2)
    finally:
        if to_json:
            print(
                json.dumps(
                    proc, cls=sysview.SysViewTraceDataJsonEncoder, indent=4, separators=(',', ': '), sort_keys=True
                )
            )
        else:
            proc.print_report()
        proc.cleanup()

        for file in temp_files:
            try:
                os.remove(file)
            except Exception as e:
                log.warn(escape(f'Failed to remove temporary file {file}: {e}'))


if __name__ == '__main__':
    install_exception_reporting()
    main()
