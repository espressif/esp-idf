#!/usr/bin/env python
import sys

download_url = sys.argv[1]
output_filename = sys.argv[2]
if (sys.version_info > (3, 0)):
    import urllib.request
    urllib.request.urlretrieve(download_url, output_filename)
else:
    import urllib2
    response = urllib2.urlopen(download_url)
    with open(output_filename, 'w') as output_file:
        output_file.write(response.read())
