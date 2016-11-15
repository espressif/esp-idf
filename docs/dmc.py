# dmc.py - Doxygen Member Comments
#
# This code is in the Public Domain (or CC0 licensed, at your option.)
# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.
#
# OVERVIEW:
# This script is used to inquire for and update doxygen member comments like:
#
#    /*!< this is a comment   */
#
# in C header files (*.h)
#
# INSTRUCIONS:
# 1. Update path to folders with header files to process.
#    The path is contained in the following varable:
#
#      dir_to_process (search for it in the code below)
#
# 2. To inquire if header files contain doxygen member comments like:
#
#          /*!< this is a comment   */
#      Run:
#          python dmc.py
#
# 3. To change comments in header files to the format like:
#
#          //!< this is a comment
#      Run:
#          python dmc.py -u
#


#!/usr/bin/env python

# -----------------------------
# Doxygen Member Comments Check
# ------------------------------
#
# Check if a file has inside comments like:
#
#     /*!< this is a comment   */
#
def mc_check(file_path):

  import re

  mc_counter = 0
  with open(file_path) as old_file:
    for line in old_file:
      if re.match('(.*)/\*!<.*\*/(.*)', line):
        mc_counter += 1

  return mc_counter


# ------------------------------
# Doxygen Member Comments Update
# ------------------------------
#
# Inside a file replace all comments like:
#
#     /*!< this is a comment   */
#
# with:
#
#     //!< this is a comment
#
# Note: spaces at the end of a comment will be removed
#
def mc_update(file_path):

  
  # check if file contains any doxygen member comments to update
  mc_counter = mc_check(file_path)
  
  if mc_counter > 0:
    # we have some comments - go ahead and update them
    from tempfile import mkstemp
    from shutil import move
    from os import remove, close
    import re
    # Create temp file
    fh, abs_path = mkstemp()
    with open(abs_path,'w') as new_file:
      with open(file_path) as old_file:
        for line in old_file:
          if re.match('(.*)/\*!<.*\*/(.*)', line):
            line = re.sub(r'/\*!<', '//!<', line)
            line = re.sub(r'\s*\*/', '', line)
          new_file.write(line)
    close(fh)
    # Remove original file
    remove(file_path)
    # Move new file
    move(abs_path, file_path)

  return mc_counter
  

# -------------------
# Main script follows  
# -------------------
#
import sys, os

# NOTE:
# This script will process all folders in path below:
# dir_to_process = '/home/krzysztof/esp/esp-idf/components'
dir_to_process = 'C:/Users/Krzysztof/Documents/GitHub/esp-idf/components'
# Update this variable to match your configuration
# Windows users: 
# Use forward-slashes not backslashes for this path, i.e. 'C:/msys32/esp-idf/components'.
# 
# Process input parameters to find out what we are supposed to do
do_update = 0
if len(sys.argv) > 1:
  if sys.argv[1] == "-u":
    do_update = 1

# Confirm our understanding what we are supposed to do
print
if do_update == 1:
  print "Operation: UPDATING doxygen memebr comments"
else:
  print "Operation: Checking doxygen memebr comments"
print "Path traversed: " + dir_to_process

# Now do the job
print
print "Count\tFile Path"
for root, dirs, files in os.walk(dir_to_process):
  for file in files:
    if file.endswith(".h"):
      file_path = os.path.join(root, file)
      mc_counter = mc_check(file_path)
      if do_update == 1:
        mc_counter = mc_update(file_path)
      else:
        mc_counter = mc_check(file_path)
      if mc_counter > 0:
        print str(mc_counter) + "\t" + file_path
      else:
        print "NONE"          + "\t" + file_path
