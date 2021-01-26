#!/usr/bin/env python
import sys

if (sys.version_info > (3, 0)):
    import subprocess
    subprocess.run('cmd /c echo hello')
