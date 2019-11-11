# Extension to generate the KConfig reference list
import os.path
import sys
import subprocess

from local_util import copy_if_modified

def setup(app):
    # The idf_build_system extension will emit this event once it
    # has parsed the IDF project's information
    app.connect('idf-info', generate_reference)

def generate_reference(app, project_description):
    build_dir = os.path.dirname(app.doctreedir.rstrip(os.sep))

    # Generate 'kconfig.inc' file from components' Kconfig files
    print("Generating kconfig.inc from kconfig contents")
    kconfig_inc_path = '{}/inc/kconfig.inc'.format(build_dir)
    temp_sdkconfig_path = '{}/sdkconfig.tmp'.format(build_dir)

    kconfigs = project_description["config_environment"]["COMPONENT_KCONFIGS"].split(";")
    kconfig_projbuilds = project_description["config_environment"]["COMPONENT_KCONFIGS_PROJBUILD"].split(";")

    sdkconfig_renames = set()
    # TODO: this should be generated in project description as well, if possible
    for k in kconfigs + kconfig_projbuilds:
        component_dir = os.path.dirname(k)
        sdkconfig_rename = os.path.join(component_dir, "sdkconfig.rename")
        if os.path.exists(sdkconfig_rename):
            sdkconfig_renames.add(sdkconfig_rename)

    kconfigs_source_path = '{}/inc/kconfigs_source.in'.format(build_dir)
    kconfig_projbuilds_source_path = '{}/inc/kconfig_projbuilds_source.in'.format(build_dir)

    confgen_args = [sys.executable,
                    "../../tools/kconfig_new/confgen.py",
                    "--kconfig", "../../Kconfig",
                    "--sdkconfig-rename", "../../sdkconfig.rename",
                    "--config", temp_sdkconfig_path,
                    "--env", "COMPONENT_KCONFIGS={}".format(" ".join(kconfigs)),
                    "--env", "COMPONENT_KCONFIGS_PROJBUILD={}".format(" ".join(kconfig_projbuilds)),
                    "--env", "COMPONENT_SDKCONFIG_RENAMES={}".format(" ".join(sdkconfig_renames)),
                    "--env", "COMPONENT_KCONFIGS_SOURCE_FILE={}".format(kconfigs_source_path),
                    "--env", "COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE={}".format(kconfig_projbuilds_source_path),
                    "--env", "IDF_PATH={}".format(app.config.idf_path),
                    "--env", "IDF_TARGET={}".format(app.config.idf_target),
                    "--output", "docs", kconfig_inc_path + '.in'
    ]
    subprocess.check_call(confgen_args)
    copy_if_modified(kconfig_inc_path + '.in', kconfig_inc_path)


