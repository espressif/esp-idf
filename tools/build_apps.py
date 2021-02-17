#!/usr/bin/env python

# coding=utf-8
#
# ESP-IDF helper script to build multiple applications. Consumes the input of find_apps.py.
#

import argparse
import logging
import os.path
import re
import sys

from find_build_apps import BUILD_SYSTEMS, BuildError, BuildItem, setup_logging
from find_build_apps.common import SIZE_JSON_FN, rmdir

# This RE will match GCC errors and many other fatal build errors and warnings as well
LOG_ERROR_WARNING = re.compile(r'(error|warning):', re.IGNORECASE)

# Log this many trailing lines from a failed build log, also
LOG_DEBUG_LINES = 25


def main():  # type: () -> None
    parser = argparse.ArgumentParser(description='ESP-IDF app builder')
    parser.add_argument(
        '-v',
        '--verbose',
        action='count',
        help='Increase the logging level of the script. Can be specified multiple times.',
    )
    parser.add_argument(
        '--build-verbose',
        action='store_true',
        help='Enable verbose output from build system.',
    )
    parser.add_argument(
        '--log-file',
        type=argparse.FileType('w'),
        help='Write the script log to the specified file, instead of stderr',
    )
    parser.add_argument(
        '--parallel-count',
        default=1,
        type=int,
        help="Number of parallel build jobs. Note that this script doesn't start the jobs, " +
             'it needs to be executed multiple times with same value of --parallel-count and ' +
             'different values of --parallel-index.',
    )
    parser.add_argument(
        '--parallel-index',
        default=1,
        type=int,
        help='Index (1-based) of the job, out of the number specified by --parallel-count.',
    )
    parser.add_argument(
        '--format',
        default='json',
        choices=['json'],
        help='Format to read the list of builds',
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help="Don't actually build, only print the build commands",
    )
    parser.add_argument(
        '--keep-going',
        action='store_true',
        help="Don't exit immediately when a build fails.",
    )
    parser.add_argument(
        '--output-build-list',
        type=argparse.FileType('w'),
        help='If specified, the list of builds (with all the placeholders expanded) will be written to this file.',
    )
    parser.add_argument(
        '--size-info',
        type=argparse.FileType('a'),
        help='If specified, the test case name and size info json will be written to this file'
    )
    parser.add_argument(
        'build_list',
        type=argparse.FileType('r'),
        nargs='?',
        default=sys.stdin,
        help='Name of the file to read the list of builds from. If not specified, read from stdin.',
    )
    args = parser.parse_args()

    setup_logging(args)

    build_items = [BuildItem.from_json(line) for line in args.build_list]
    if not build_items:
        logging.warning('Empty build list')
        SystemExit(0)

    num_builds = len(build_items)
    num_jobs = args.parallel_count
    job_index = args.parallel_index - 1  # convert to 0-based index
    num_builds_per_job = (num_builds + num_jobs - 1) // num_jobs
    min_job_index = num_builds_per_job * job_index
    if min_job_index >= num_builds:
        logging.warn('Nothing to do for job {} (build total: {}, per job: {})'.format(
            job_index + 1, num_builds, num_builds_per_job))
        raise SystemExit(0)

    max_job_index = min(num_builds_per_job * (job_index + 1) - 1, num_builds - 1)
    logging.info('Total {} builds, max. {} builds per job, running builds {}-{}'.format(
        num_builds, num_builds_per_job, min_job_index + 1, max_job_index + 1))

    builds_for_current_job = build_items[min_job_index:max_job_index + 1]
    for i, build_info in enumerate(builds_for_current_job):
        index = i + min_job_index + 1
        build_info.index = index
        build_info.dry_run = args.dry_run
        build_info.verbose = args.build_verbose
        build_info.keep_going = args.keep_going
        logging.debug('    Build {}: {}'.format(index, repr(build_info)))
        if args.output_build_list:
            args.output_build_list.write(build_info.to_json_expanded() + '\n')

    failed_builds = []
    for build_info in builds_for_current_job:
        logging.info('Running build {}: {}'.format(build_info.index, repr(build_info)))
        build_system_class = BUILD_SYSTEMS[build_info.build_system]
        try:
            build_system_class.build(build_info)
        except BuildError as e:
            logging.error(str(e))
            if build_info.build_log_path:
                log_filename = os.path.basename(build_info.build_log_path)
                with open(build_info.build_log_path, 'r') as f:
                    lines = [line.rstrip() for line in f.readlines() if line.rstrip()]  # non-empty lines
                    logging.debug('Error and warning lines from {}:'.format(log_filename))
                    for line in lines:
                        if LOG_ERROR_WARNING.search(line):
                            logging.warning('>>> {}'.format(line))
                    logging.debug('Last {} lines of {}:'.format(LOG_DEBUG_LINES, log_filename))
                    for line in lines[-LOG_DEBUG_LINES:]:
                        logging.debug('>>> {}'.format(line))
            if args.keep_going:
                failed_builds.append(build_info)
            else:
                raise SystemExit(1)
        else:
            if args.size_info:
                build_info.write_size_info(args.size_info)
            if not build_info.preserve:
                logging.info('Removing build directory {}'.format(build_info.build_path))
                # we only remove binaries here, log files are still needed by check_build_warnings.py
                rmdir(build_info.build_path, exclude_file_pattern=SIZE_JSON_FN)

    if failed_builds:
        logging.error('The following build have failed:')
        for build in failed_builds:
            logging.error('    {}'.format(build))
        raise SystemExit(1)


if __name__ == '__main__':
    main()
