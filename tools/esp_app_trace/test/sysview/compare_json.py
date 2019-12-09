#!/usr/bin/env python

import json
import sys

if len(sys.argv) < 3:
    sys.exit(-1)

objs = []
for i in range(1, len(sys.argv)):
    f = open(sys.argv[i])
    objs.append(json.load(f))

for i in range(1, len(objs)):
    if objs[i - 1] != objs[i]:
        sys.exit(-1)
