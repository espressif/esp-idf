#!/usr/bin/env python
#
# Command line tool to convert simple ESP-IDF Makefile & component.mk files to
# CMakeLists.txt files
#
import argparse
import subprocess
import re
import os.path
import glob

debug = False


def get_make_variables(path, makefile="Makefile", expected_failure=False, variables={}):
    """
    Given the path to a Makefile of some kind, return a dictionary of all variables defined in this Makefile

    Uses 'make' to parse the Makefile syntax, so we don't have to!

    Overrides IDF_PATH= to avoid recursively evaluating the entire project Makefile structure.
    """
    variable_setters = [("%s=%s" % (k,v)) for (k,v) in variables.items()]

    cmdline = ["make", "-rpn", "-C", path, "-f", makefile] + variable_setters
    if debug:
        print("Running %s..." % (" ".join(cmdline)))

    p = subprocess.Popen(cmdline, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (output, stderr) = p.communicate("\n")

    if (not expected_failure) and p.returncode != 0:
        raise RuntimeError("Unexpected make failure, result %d" % p.returncode)

    if debug:
        print("Make stdout:")
        print(output)
        print("Make stderr:")
        print(stderr)

    next_is_makefile = False  # is the next line a makefile variable?
    result = {}
    BUILT_IN_VARS = set(["MAKEFILE_LIST", "SHELL", "CURDIR", "MAKEFLAGS"])

    for line in output.decode('utf-8').split("\n"):
        if line.startswith("# makefile"):  # this line appears before any variable defined in the makefile itself
            next_is_makefile = True
        elif next_is_makefile:
            next_is_makefile = False
            m = re.match(r"(?P<var>[^ ]+) :?= (?P<val>.+)", line)
            if m is not None:
                if not m.group("var") in BUILT_IN_VARS:
                    result[m.group("var")] = m.group("val").strip()

    return result


def get_component_variables(project_path, component_path):
    make_vars = get_make_variables(component_path,
                                   os.path.join(os.environ["IDF_PATH"],
                                                "make",
                                                "component_wrapper.mk"),
                                   expected_failure=True,
                                   variables={
                                       "COMPONENT_MAKEFILE": os.path.join(component_path, "component.mk"),
                                       "COMPONENT_NAME": os.path.basename(component_path),
                                       "PROJECT_PATH": project_path,
                                   })

    if "COMPONENT_OBJS" in make_vars:  # component.mk specifies list of object files
        # Convert to sources
        def find_src(obj):
            obj = os.path.splitext(obj)[0]
            for ext in ["c", "cpp", "S"]:
                if os.path.exists(os.path.join(component_path, obj) + "." + ext):
                    return obj + "." + ext
            print("WARNING: Can't find source file for component %s COMPONENT_OBJS %s" % (component_path, obj))
            return None

        srcs = []
        for obj in make_vars["COMPONENT_OBJS"].split(" "):
            src = find_src(obj)
            if src is not None:
                srcs.append(src)
        make_vars["COMPONENT_SRCS"] = " ".join(srcs)
    else:
        component_srcs = list()
        for component_srcdir in make_vars.get("COMPONENT_SRCDIRS", ".").split(" "):
            component_srcdir_path = os.path.abspath(os.path.join(component_path, component_srcdir))

            srcs = list()
            srcs += glob.glob(os.path.join(component_srcdir_path, "*.[cS]"))
            srcs += glob.glob(os.path.join(component_srcdir_path, "*.cpp"))
            srcs = [('"%s"' % str(os.path.relpath(s, component_path))) for s in srcs]

        make_vars["COMPONENT_ADD_INCLUDEDIRS"] = make_vars.get("COMPONENT_ADD_INCLUDEDIRS", "include")
        component_srcs += srcs
        make_vars["COMPONENT_SRCS"] = " ".join(component_srcs)

    return make_vars


def convert_project(project_path):
    if not os.path.exists(project_path):
        raise RuntimeError("Project directory '%s' not found" % project_path)
    if not os.path.exists(os.path.join(project_path, "Makefile")):
        raise RuntimeError("Directory '%s' doesn't contain a project Makefile" % project_path)

    project_cmakelists = os.path.join(project_path, "CMakeLists.txt")
    if os.path.exists(project_cmakelists):
        raise RuntimeError("This project already has a CMakeLists.txt file")

    project_vars = get_make_variables(project_path, expected_failure=True)
    if "PROJECT_NAME" not in project_vars:
        raise RuntimeError("PROJECT_NAME does not appear to be defined in IDF project Makefile at %s" % project_path)

    component_paths = project_vars["COMPONENT_PATHS"].split(" ")

    # Convert components as needed
    for p in component_paths:
        if "MSYSTEM" in os.environ:
            cmd = ["cygpath", "-w", p]
            p = subprocess.check_output(cmd).strip()

        convert_component(project_path, p)

    project_name = project_vars["PROJECT_NAME"]

    # Generate the project CMakeLists.txt file
    with open(project_cmakelists, "w") as f:
        f.write("""
# (Automatically converted from project Makefile by convert_to_cmake.py.)

# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

""")
        f.write("""
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
""")
        f.write("project(%s)\n" % project_name)

    print("Converted project %s" % project_cmakelists)


def convert_component(project_path, component_path):
    if debug:
        print("Converting %s..." % (component_path))
    cmakelists_path = os.path.join(component_path, "CMakeLists.txt")
    if os.path.exists(cmakelists_path):
        print("Skipping already-converted component %s..." % cmakelists_path)
        return
    v = get_component_variables(project_path, component_path)

    # Look up all the variables before we start writing the file, so it's not
    # created if there's an erro
    component_srcs = v.get("COMPONENT_SRCS", None)

    component_add_includedirs = v["COMPONENT_ADD_INCLUDEDIRS"]
    cflags = v.get("CFLAGS", None)

    with open(cmakelists_path, "w") as f:
        if component_srcs is not None:
            f.write("idf_component_register(SRCS %s)\n" % component_srcs)
            f.write("                       INCLUDE_DIRS %s" % component_add_includedirs)
            f.write("                       # Edit following two lines to set component requirements (see docs)\n")
            f.write("                       REQUIRES "")\n")
            f.write("                       PRIV_REQUIRES "")\n\n")
        else:
            f.write("idf_component_register()\n")
        if cflags is not None:
            f.write("target_compile_options(${COMPONENT_LIB} PRIVATE %s)\n" % cflags)

    print("Converted %s" % cmakelists_path)


def main():
    global debug

    parser = argparse.ArgumentParser(description='convert_to_cmake.py - ESP-IDF Project Makefile to CMakeLists.txt converter', prog='convert_to_cmake')

    parser.add_argument('--debug', help='Display debugging output',
                        action='store_true')

    parser.add_argument('project', help='Path to project to convert (defaults to CWD)', default=os.getcwd(), metavar='project path', nargs='?')

    args = parser.parse_args()
    debug = args.debug
    print("Converting %s..." % args.project)
    convert_project(args.project)


if __name__ == "__main__":
    main()
