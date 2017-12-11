#!/usr/bin/env python

# internal use only
# called by CI jobs to determine if it need to be executed

import os
import re
import sys
import json


RE_FILTER_PATTERN = re.compile(r'^r"(.+)?"$')

RE_TYPE = type(re.compile("", 0))


def parse_filter(filter_name):
    filter_raw = os.getenv(filter_name)
    filters = []
    if filter_raw:
        filter_data = json.loads(filter_raw)
        for _filter in filter_data:
            match = RE_FILTER_PATTERN.search(_filter)
            if match:
                filters.append(re.compile(match.group(1)))
            else:
                filters.append(_filter)
    return filters


def process_filter(filter_name, ci_name):
    execute = True
    
    # bot message is case insensitive (processed with lower case). so we also convert ci_name to lower case.
    ci_name = ci_name.lower()

    filter_list = parse_filter(filter_name)

    for _filter in filter_list:
        if isinstance(_filter, RE_TYPE):
            match = _filter.search(ci_name) is not None
        else:
            match = _filter == ci_name

        if match:
            execute = True
            break
        else:
            execute = False
    return execute


if __name__ == "__main__":
    need_to_execute = process_filter("BOT_STAGE_FILTER", os.getenv("CI_JOB_STAGE")) \
                      and process_filter("BOT_JOB_FILTER", os.getenv("CI_JOB_NAME"))
    if need_to_execute:
        sys.exit(0)
    else:
        print("Skip this job as it doesn't fit @bot's filter")
        sys.exit(-1)
