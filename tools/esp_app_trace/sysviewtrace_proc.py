#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# This is python script to process various types trace data streams in SystemView format.
# Trace data can be provided in multiple trace files (one per CPU). After processing phase
# script prints report for every type of trace data stream which was found.
#

import argparse
import json
import logging
import os.path
import signal
import sys
import tempfile
import traceback
from urllib.parse import urlparse

import espytrace.apptrace as apptrace
import espytrace.sysview as sysview


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
        logging.error('Error checking SEGGER multicore file format:', e)
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
                logging.error('Failed to parse core1 offset')
                return None, None

            # Read the entire file
            f.seek(0)
            data = f.read()

            # Find first 10 sync bytes start offset
            sync_start = data.find(b'\x00' * 10)
            if sync_start == -1:
                logging.error('Sync bytes not found')
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
        logging.error('Failed to process files:', e)
        return None, None


def main():
    verbosity_levels = [logging.CRITICAL, logging.ERROR, logging.WARNING, logging.INFO, logging.DEBUG]

    parser = argparse.ArgumentParser(description='ESP32 SEGGER SystemView Trace Parsing Tool')

    parser.add_argument(
        'trace_sources', help='Trace data sources. Format: [file://]/path/to/file.', nargs='+', type=str
    )
    parser.add_argument('--elf-file', '-b', help='Path to program ELF file.', type=str, default='')
    parser.add_argument('--tmo', '-w', help='Data wait timeout in sec. -1: infinite, 0: no wait', type=int, default=0)
    parser.add_argument('--dump-events', '-d', help='Dump all events.', action='store_true')
    parser.add_argument(
        '--print-events',
        '-p',
        help='Print events of selected types. By default only reports are printed',
        action='store_true',
    )
    parser.add_argument(
        '--include-events',
        '-i',
        help='Events types to be included into report.',
        type=str,
        choices=['heap', 'log', 'all'],
        default='all',
    )
    parser.add_argument('--toolchain', '-t', help='Toolchain prefix.', type=str, default='xtensa-esp32-elf-')
    parser.add_argument(
        '--events-map',
        '-e',
        help='Events map file.',
        type=str,
        default=os.path.join(os.path.dirname(__file__), 'SYSVIEW_FreeRTOS.txt'),
    )
    parser.add_argument('--to-json', '-j', help='Print JSON.', action='store_true', default=False)
    parser.add_argument(
        '--verbose',
        '-v',
        help='Verbosity level. Default 1',
        choices=range(0, len(verbosity_levels)),
        type=int,
        default=1,
    )
    args = parser.parse_args()

    def sig_int_handler(signum, frame):
        reader.cleanup()

    signal.signal(signal.SIGINT, sig_int_handler)

    include_events = {'heap': False, 'log': False}
    if args.include_events == 'all':
        for k in include_events:
            include_events[k] = True
    elif args.include_events == 'heap':
        include_events['heap'] = True
    elif args.include_events == 'log':
        include_events['log'] = True

    logging.basicConfig(level=verbosity_levels[args.verbose], format='[%(levelname)s] %(message)s')

    temp_files = []
    # Only check for SEGGER format if there's exactly one trace source
    if len(args.trace_sources) == 1:
        trace_source = args.trace_sources[0]
        if is_segger_multicore_format(trace_source):
            core0_file, core1_file = split_segger_multicore_file(trace_source)
            if core0_file and core1_file:
                temp_files.extend([core0_file, core1_file])
                args.trace_sources = temp_files
            else:
                sys.exit(2)

    # parse trace files
    parsers = []
    for i, trace_source in enumerate(args.trace_sources):
        try:
            parser = sysview.SysViewMultiTraceDataParser(print_events=False, core_id=i)
            if include_events['heap']:
                parser.add_stream_parser(
                    sysview.SysViewTraceDataParser.STREAMID_HEAP,
                    sysview.SysViewHeapTraceDataParser(print_events=False, core_id=i),
                )
            if include_events['log']:
                parser.add_stream_parser(
                    sysview.SysViewTraceDataParser.STREAMID_LOG,
                    sysview.SysViewLogTraceDataParser(print_events=False, core_id=i),
                )
            parsers.append(parser)
        except Exception as e:
            logging.error('Failed to create data parser (%s)!', e)
            traceback.print_exc()
            sys.exit(2)
        reader = apptrace.reader_create(trace_source, args.tmo)
        if not reader:
            logging.error('Failed to create trace reader!')
            sys.exit(2)
        try:
            # logging.info("Parse trace from '{}'...".format(trace_source))
            logging.info("Parse trace from '%s'...", trace_source)
            sysview.parse_trace(reader, parser, args.events_map)
            logging.info('Parsing completed.')
        except (apptrace.ReaderTimeoutError, apptrace.ReaderShutdownRequest) as e:
            logging.info('Stop parsing trace. (%s)', e)
        except Exception as e:
            logging.error('Failed to parse trace (%s)!', e)
            parser.cleanup()
            traceback.print_exc()
            sys.exit(2)
        finally:
            reader.cleanup()

    # merge and process traces
    try:
        proc = sysview.SysViewMultiStreamTraceDataProcessor(
            traces=parsers, print_events=args.dump_events, keep_all_events=True if args.to_json else False
        )
        if include_events['heap']:
            proc.add_stream_processor(
                sysview.SysViewTraceDataParser.STREAMID_HEAP,
                sysview.SysViewHeapTraceDataProcessor(
                    args.toolchain, args.elf_file, root_proc=proc, print_heap_events=args.print_events
                ),
            )
        if include_events['log']:
            proc.add_stream_processor(
                sysview.SysViewTraceDataParser.STREAMID_LOG,
                sysview.SysViewLogTraceDataProcessor(root_proc=proc, print_log_events=args.print_events),
            )
    except Exception as e:
        logging.error('Failed to create data processor (%s)!', e)
        traceback.print_exc()
        sys.exit(2)

    try:
        logging.info("Process events from '%s'...", args.trace_sources)
        proc.merge_and_process()
        logging.info('Processing completed.')
    except Exception as e:
        logging.error('Failed to process trace (%s)!', e)
        traceback.print_exc()
        sys.exit(2)
    finally:
        if args.to_json:
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
                logging.warning('Failed to remove temporary file %s: %s', file, e)


if __name__ == '__main__':
    main()
