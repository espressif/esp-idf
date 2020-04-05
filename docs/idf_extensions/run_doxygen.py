# Extension to generate Doxygen XML include files, with IDF config & soc macros included
from __future__ import print_function
from __future__ import unicode_literals
from io import open
import glob
import os
import os.path
import re
import subprocess
from .util import copy_if_modified

ALL_KINDS = [
    ("function", "Functions"),
    ("union", "Unions"),
    ("struct", "Structures"),
    ("define", "Macros"),
    ("typedef", "Type Definitions"),
    ("enum", "Enumerations")
]
"""list of items that will be generated for a single API file
"""


def setup(app):
    # The idf_build_system extension will emit this event once it
    app.connect('idf-info', generate_doxygen)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}


def _parse_defines(header_path, sdk_config_path):
    defines = {}
    # Note: we run C preprocessor here without any -I arguments (except "sdkconfig.h"), so assumption is
    # that these headers are all self-contained and don't include any other headers
    # not in the same directory
    print("Reading macros from %s..." % (header_path))
    processed_output = subprocess.check_output(["xtensa-esp32-elf-gcc", "-I", sdk_config_path,
                                                "-dM", "-E", header_path]).decode()
    for line in processed_output.split("\n"):
        line = line.strip()
        m = re.search("#define ([^ ]+) ?(.*)", line)
        if m and not m.group(1).startswith("_"):
            defines[m.group(1)] = m.group(2)

    return defines


def generate_doxygen(app, project_description):
    build_dir = os.path.dirname(app.doctreedir.rstrip(os.sep))

    sdk_config_path = os.path.join(project_description["build_dir"], "config")

    # Parse kconfig macros to pass into doxygen
    #
    # TODO: this should use the set of "config which can't be changed" eventually,
    # not the header
    defines = _parse_defines(os.path.join(project_description["build_dir"],
                                          "config", "sdkconfig.h"), sdk_config_path)

    # Add all SOC _caps.h headers to the defines
    #
    # kind of a hack, be nicer to add a component info dict in project_description.json
    soc_path = [p for p in project_description["build_component_paths"] if p.endswith("/soc")][0]
    soc_headers = glob.glob(os.path.join(soc_path, "soc", project_description["target"],
                                         "include", "soc", "*_caps.h"))
    assert len(soc_headers) > 0
    for soc_header in soc_headers:
        defines.update(_parse_defines(soc_header, sdk_config_path))

    # Call Doxygen to get XML files from the header files
    print("Calling Doxygen to generate latest XML files")
    doxy_env = os.environ
    doxy_env.update({
        "ENV_DOXYGEN_DEFINES": " ".join('{}={}'.format(key, value) for key, value in defines.items()),
        "IDF_PATH": app.config.idf_path,
        "IDF_TARGET": app.config.idf_target,
    })
    doxyfile = os.path.join(app.config.docs_root, "Doxyfile")
    print("Running doxygen with doxyfile {}".format(doxyfile))
    # note: run Doxygen in the build directory, so the xml & xml_in files end up in there
    subprocess.check_call(["doxygen", doxyfile], env=doxy_env, cwd=build_dir)

    # Doxygen has generated XML files in 'xml' directory.
    # Copy them to 'xml_in', only touching the files which have changed.
    copy_if_modified(os.path.join(build_dir, 'xml/'), os.path.join(build_dir, 'xml_in/'))

    # Generate 'api_name.inc' files from the Doxygen XML files
    convert_api_xml_to_inc(app, doxyfile)


def convert_api_xml_to_inc(app, doxyfile):
    """ Generate header_file.inc files
    with API reference made of doxygen directives
    for each header file
    specified in the 'INPUT' statement of the Doxyfile.
    """
    build_dir = app.config.build_dir

    xml_directory_path = "{}/xml".format(build_dir)
    inc_directory_path = "{}/inc".format(build_dir)

    if not os.path.isdir(xml_directory_path):
        raise RuntimeError("Directory {} does not exist!".format(xml_directory_path))

    if not os.path.exists(inc_directory_path):
        os.makedirs(inc_directory_path)

    header_paths = get_doxyfile_input_paths(app, doxyfile)
    print("Generating 'api_name.inc' files with Doxygen directives")
    for header_file_path in header_paths:
        api_name = get_api_name(header_file_path)
        inc_file_path = inc_directory_path + "/" + api_name + ".inc"
        rst_output = generate_directives(header_file_path, xml_directory_path)

        previous_rst_output = ''
        if os.path.isfile(inc_file_path):
            with open(inc_file_path, "r", encoding='utf-8') as inc_file_old:
                previous_rst_output = inc_file_old.read()

        if previous_rst_output != rst_output:
            with open(inc_file_path, "w", encoding='utf-8') as inc_file:
                inc_file.write(rst_output)


def get_doxyfile_input_paths(app, doxyfile_path):
    """Get contents of Doxyfile's INPUT statement.

    Returns:
        Contents of Doxyfile's INPUT.

    """
    if not os.path.isfile(doxyfile_path):
        raise RuntimeError("Doxyfile '{}' does not exist!".format(doxyfile_path))

    print("Getting Doxyfile's INPUT")

    with open(doxyfile_path, "r", encoding='utf-8') as input_file:
        line = input_file.readline()
        # read contents of Doxyfile until 'INPUT' statement
        while line:
            if line.find("INPUT") == 0:
                break
            line = input_file.readline()

        doxyfile_INPUT = []
        line = input_file.readline()
        # skip input_file contents until end of 'INPUT' statement
        while line:
            if line.isspace():
                # we have reached the end of 'INPUT' statement
                break
            # process only lines that are not comments
            if line.find("#") == -1:
                # extract header file path inside components folder
                m = re.search("components/(.*\.h)", line)  # noqa: W605 - regular expression
                header_file_path = m.group(1)

                # Replace env variable used for multi target header
                header_file_path = header_file_path.replace("$(IDF_TARGET)", app.config.idf_target)

                doxyfile_INPUT.append(header_file_path)

            # proceed reading next line
            line = input_file.readline()

    return doxyfile_INPUT


def get_api_name(header_file_path):
    """Get name of API from header file path.

    Args:
        header_file_path: path to the header file.

    Returns:
        The name of API.

    """
    api_name = ""
    regex = r".*/(.*)\.h"
    m = re.search(regex, header_file_path)
    if m:
        api_name = m.group(1)

    return api_name


def generate_directives(header_file_path, xml_directory_path):
    """Generate API reference with Doxygen directives for a header file.

    Args:
        header_file_path: a path to the header file with API.

    Returns:
        Doxygen directives for the header file.

    """

    api_name = get_api_name(header_file_path)

    # in XLT file name each "_" in the api name is expanded by Doxygen to "__"
    xlt_api_name = api_name.replace("_", "__")
    xml_file_path = "%s/%s_8h.xml" % (xml_directory_path, xlt_api_name)

    rst_output = ""
    rst_output = ".. File automatically generated by 'gen-dxd.py'\n"
    rst_output += "\n"
    rst_output += get_rst_header("Header File")
    rst_output += "* :component_file:`" + header_file_path + "`\n"
    rst_output += "\n"

    try:
        import xml.etree.cElementTree as ET
    except ImportError:
        import xml.etree.ElementTree as ET

    tree = ET.ElementTree(file=xml_file_path)
    for kind, label in ALL_KINDS:
        rst_output += get_directives(tree, kind)

    return rst_output


def get_rst_header(header_name):
    """Get rst formatted code with a header.

    Args:
        header_name: name of header.

    Returns:
        Formatted rst code with the header.

    """

    rst_output = ""
    rst_output += header_name + "\n"
    rst_output += "^" * len(header_name) + "\n"
    rst_output += "\n"

    return rst_output


def select_unions(innerclass_list):
    """Select unions from innerclass list.

    Args:
        innerclass_list: raw list with unions and structures
                         extracted from Dogygen's xml file.

    Returns:
        Doxygen directives with unions selected from the list.

    """

    rst_output = ""
    for line in innerclass_list.splitlines():
        # union is denoted by "union" at the beginning of line
        if line.find("union") == 0:
            union_id, union_name = re.split(r"\t+", line)
            rst_output += ".. doxygenunion:: "
            rst_output += union_name
            rst_output += "\n"

    return rst_output


def select_structs(innerclass_list):
    """Select structures from innerclass list.

    Args:
        innerclass_list: raw list with unions and structures
                         extracted from Dogygen's xml file.

    Returns:
        Doxygen directives with structures selected from the list.
        Note: some structures are excluded as described on code below.

    """

    rst_output = ""
    for line in innerclass_list.splitlines():
        # structure is denoted by "struct" at the beginning of line
        if line.find("struct") == 0:
            # skip structures that are part of union
            # they are documented by 'doxygenunion' directive
            if line.find("::") > 0:
                continue
            struct_id, struct_name = re.split(r"\t+", line)
            rst_output += ".. doxygenstruct:: "
            rst_output += struct_name
            rst_output += "\n"
            rst_output += "    :members:\n"
            rst_output += "\n"

    return rst_output


def get_directives(tree, kind):
    """Get directives for specific 'kind'.

    Args:
        tree: the ElementTree 'tree' of XML by Doxygen
        kind: name of API "kind" to be generated

    Returns:
        Doxygen directives for selected 'kind'.
        Note: the header with "kind" name is included.

    """

    rst_output = ""
    if kind in ["union", "struct"]:
        innerclass_list = ""
        for elem in tree.iterfind('compounddef/innerclass'):
            innerclass_list += elem.attrib["refid"] + "\t" + elem.text + "\n"
        if kind == "union":
            rst_output += select_unions(innerclass_list)
        else:
            rst_output += select_structs(innerclass_list)
    else:
        for elem in tree.iterfind(
                'compounddef/sectiondef/memberdef[@kind="%s"]' % kind):
            name = elem.find('name')
            rst_output += ".. doxygen%s:: " % kind
            rst_output += name.text + "\n"
    if rst_output:
        all_kinds_dict = dict(ALL_KINDS)
        rst_output = get_rst_header(all_kinds_dict[kind]) + rst_output + "\n"

    return rst_output
