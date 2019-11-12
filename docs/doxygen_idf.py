# Extension to generate Doxygen XML include files, with IDF config & soc macros included
import glob
import os.path
import re
import sys
import subprocess

from local_util import copy_if_modified, call_with_python

def setup(app):
    # The idf_build_system extension will emit this event once it
    app.connect('idf-info', generate_doxygen)


def _parse_defines(header_path):
    defines = {}
    # Note: we run C preprocessor here without any -I arguments, so assumption is
    # that these headers are all self-contained and don't include any other headers
    # not in the same directory
    print("Reading macros from %s..." % (header_path))
    processed_output = subprocess.check_output(["xtensa-esp32-elf-gcc", "-dM", "-E", header_path])
    for line in processed_output.split("\n"):
        line = line.strip()
        m = re.search("#define ([^ ]+) ?(.*)", line)
        if m and not m.group(1).startswith("_"):
            defines[m.group(1)] = m.group(2)

    return defines


def generate_doxygen(app, project_description):
    build_dir = os.path.dirname(app.doctreedir.rstrip(os.sep))

    # Parse kconfig macros to pass into doxygen
    #
    # TODO: this should use the set of "config which can't be changed" eventually,
    # not the header
    defines = _parse_defines(os.path.join(project_description["build_dir"],
                                          "config", "sdkconfig.h"))

    # Add all SOC _caps.h headers to the defines
    #
    # kind of a hack, be nicer to add a component info dict in project_description.json
    soc_path = [p for p in project_description["build_component_paths"] if p.endswith("/soc")][0]
    for soc_header in glob.glob(os.path.join(soc_path, project_description["target"],
                                             "include", "soc", "*_caps.h")):
        defines.update(_parse_defines(soc_header))

    # Call Doxygen to get XML files from the header files
    print("Calling Doxygen to generate latest XML files")
    doxy_env = {
        "ENV_DOXYGEN_DEFINES": " ".join(defines)
    }
    subprocess.check_call(["doxygen", "../Doxyfile"], env=doxy_env)

    # Doxygen has generated XML files in 'xml' directory.
    # Copy them to 'xml_in', only touching the files which have changed.
    copy_if_modified('xml/', 'xml_in/')

    # Generate 'api_name.inc' files using the XML files by Doxygen
    call_with_python('../gen-dxd.py')

    # Generate 'esp_err_defs.inc' file with ESP_ERR_ error code definitions from inc file
    esp_err_inc_path = '{}/inc/esp_err_defs.inc'.format(build_dir)
    call_with_python('../../tools/gen_esp_err_to_name.py --rst_output ' + esp_err_inc_path + '.in')
    copy_if_modified(esp_err_inc_path + '.in', esp_err_inc_path)

