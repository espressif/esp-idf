# Sphinx extension to integrate IDF build system information
# into the Sphinx Build
#
# Runs early in the Sphinx process, runs CMake to generate the dummy IDF project
# in this directory - including resolving paths, etc.
#
# Then emits the new 'idf-info' event which has information read from IDF
# build system, that other extensions can use to generate relevant data.
import os.path
import sys
import subprocess
import json

# this directory also contains the dummy IDF project
project_path = os.path.abspath(os.path.dirname(__file__))

def setup(app):
    builddir = os.path.dirname(app.doctreedir.rstrip(os.sep))
    app.add_config_value('docs_root', "", 'env')
    app.add_config_value('idf_path', os.environ.get("IDF_PATH", ""), 'env')
    app.add_config_value('idf_target', 'esp32', 'env')
    app.add_config_value('build_dir', os.environ.get("BUILDDIR", ""), 'env')  # not actually an IDF thing
    app.add_event('idf-info')

    # Attaching the generate event to env-get-outdated is a bit of a hack,
    # we want this to run early in the docs build but unclear exactly when
    app.connect('env-get-outdated', generate_idf_info)

def generate_idf_info(app, env, added, changed, removed):
    print("Running CMake on dummy project to get build info...")
    build_dir = os.path.dirname(app.doctreedir.rstrip(os.sep))
    cmake_build_dir = os.path.join(build_dir, "build_dummy_project")
    idf_py_path = os.path.join(app.config.idf_path, "tools", "idf.py")
    print("Running idf.py...")
    idf_py = [sys.executable,
              idf_py_path,
              "-B",
              cmake_build_dir,
              "-C",
              project_path]
    if not os.path.exists(os.path.join(cmake_build_dir, "CMakeCache.txt")):
        # if build directory not created yet, run a reconfigure pass over it
        print("Starting new dummy IDF project...")
        subprocess.check_call(idf_py + [ "set-target", app.config.idf_target])
    else:
        print("Re-running CMake on the existing IDF project in {}".format(cmake_build_dir))

    subprocess.check_call(idf_py + [ "reconfigure"])
    with open(os.path.join(cmake_build_dir, "project_description.json")) as f:
        project_description = json.load(f)
    if project_description["target"] != app.config.idf_target:
        # this shouldn't really happen unless someone has been moving around directories inside _build, as
        # the cmake_build_dir path should be target-specific
        raise RuntimeError("Error configuring the dummy IDF project for {}. Target in project description is {}. Is _build directory contents corrupt?".format(app.config.idf_target, project_description["target"]))
    app.emit('idf-info', project_description)

    return []
