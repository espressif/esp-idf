#!/usr/bin/env python3
#
# CI script to deploy docs to a webserver. Not useful outside of CI environment
#
#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import glob
import os
import os.path
import re
import stat
import sys
import subprocess
import tarfile
import packaging.version


def env(variable, default=None):
    """ Shortcut to return the expanded version of an environment variable """
    return os.path.expandvars(os.environ.get(variable, default) if default else os.environ[variable])


# import sanitize_version from the docs directory, shared with here
sys.path.append(os.path.join(env("IDF_PATH"), "docs"))
from sanitize_version import sanitize_version  # noqa


def main():
    # if you get KeyErrors on the following lines, it's probably because you're not running in Gitlab CI
    git_ver = env("GIT_VER")  # output of git describe --always
    ci_ver = env("CI_COMMIT_REF_NAME", git_ver)  # branch or tag we're building for (used for 'release' & URL)

    version = sanitize_version(ci_ver)
    print("Git version: {}".format(git_ver))
    print("CI Version: {}".format(ci_ver))
    print("Deployment version: {}".format(version))

    if not version:
        raise RuntimeError("A version is needed to deploy")

    build_dir = env("DOCS_BUILD_DIR")  # top-level local build dir, where docs have already been built

    if not build_dir:
        raise RuntimeError("Valid DOCS_BUILD_DIR is needed to deploy")

    url_base = env("DOCS_DEPLOY_URL_BASE")  # base for HTTP URLs, used to print the URL to the log after deploying

    docs_server = env("DOCS_DEPLOY_SERVER")  # ssh server to deploy to
    docs_user = env("DOCS_DEPLOY_SERVER_USER")
    docs_path = env("DOCS_DEPLOY_PATH")  # filesystem path on DOCS_SERVER

    if not docs_server:
        raise RuntimeError("Valid DOCS_DEPLOY_SERVER is needed to deploy")

    if not docs_user:
        raise RuntimeError("Valid DOCS_DEPLOY_SERVER_USER is needed to deploy")

    docs_server = "{}@{}".format(docs_user, docs_server)

    if not docs_path:
        raise RuntimeError("Valid DOCS_DEPLOY_PATH is needed to deploy")

    print("DOCS_DEPLOY_SERVER {} DOCS_DEPLOY_PATH {}".format(docs_server, docs_path))

    tarball_path, version_urls = build_doc_tarball(version, build_dir)

    deploy(version, tarball_path, docs_path, docs_server)

    print("Docs URLs:")
    for vurl in version_urls:
        url = "{}/{}/index.html".format(url_base, vurl)  # (index.html needed for the preview server)
        url = re.sub(r"([^:])//", r"\1/", url)  # get rid of any // that isn't in the https:// part
        print(url)

    # note: it would be neater to use symlinks for stable, but because of the directory order
    # (language first) it's kind of a pain to do on a remote server, so we just repeat the
    # process but call the version 'stable' this time
    if is_stable_version(version):
        print("Deploying again as stable version...")
        tarball_path, version_urls = build_doc_tarball("stable", build_dir)
        deploy("stable", tarball_path, docs_path, docs_server)


def deploy(version, tarball_path, docs_path, docs_server):

    def run_ssh(commands):
        """ Log into docs_server and run a sequence of commands using ssh """
        print("Running ssh: {}".format(commands))
        subprocess.run(["ssh", "-o", "BatchMode=yes", docs_server, "-x", " && ".join(commands)], check=True)

    # copy the version tarball to the server
    run_ssh(["mkdir -p {}".format(docs_path)])
    print("Running scp {} to {}".format(tarball_path, "{}:{}".format(docs_server, docs_path)))
    subprocess.run(["scp", "-B", tarball_path, "{}:{}".format(docs_server, docs_path)], check=True)

    tarball_name = os.path.basename(tarball_path)

    run_ssh(["cd {}".format(docs_path),
             "rm -rf ./*/{}".format(version),   # remove any pre-existing docs matching this version
             "tar -zxvf {}".format(tarball_name),  # untar the archive with the new docs
             "rm {}".format(tarball_name)])

    # Note: deleting and then extracting the archive is a bit awkward for updating stable/latest/etc
    # as the version will be invalid for a window of time. Better to do it atomically, but this is
    # another thing made much more complex by the directory structure putting language before version...


def build_doc_tarball(version, build_dir):
    """ Make a tar.gz archive of the docs, in the directory structure used to deploy as
        the given version """
    version_paths = []
    tarball_path = "{}/{}.tar.gz".format(build_dir, version)

    # find all the 'html/' directories under build_dir
    html_dirs = glob.glob("{}/**/html/".format(build_dir), recursive=True)
    print("Found %d html directories" % len(html_dirs))

    def not_sources_dir(ti):
        """ Filter the _sources directories out of the tarballs """
        if ti.name.endswith("/_sources"):
            return None

        ti.mode |= stat.S_IWGRP  # make everything group-writeable
        return ti

    try:
        os.remove(tarball_path)
    except OSError:
        pass

    with tarfile.open(tarball_path, "w:gz") as tarball:
        for html_dir in html_dirs:
            # html_dir has the form '<ignored>/<language>/<target>/html/'
            target_dirname = os.path.dirname(os.path.dirname(html_dir))
            target = os.path.basename(target_dirname)
            language = os.path.basename(os.path.dirname(target_dirname))

            # when deploying, we want the top-level directory layout 'language/version/target'
            archive_path = "{}/{}/{}".format(language, version, target)
            print("Archiving '{}' as '{}'...".format(html_dir, archive_path))
            tarball.add(html_dir, archive_path, filter=not_sources_dir)
            version_paths.append(archive_path)

    return (os.path.abspath(tarball_path), version_paths)


def is_stable_version(version):
    """ Heuristic for whether this is the latest stable release """
    if not version.startswith("v"):
        return False  # branch name
    if "-" in version:
        return False  # prerelease tag

    git_out = subprocess.run(["git", "tag", "-l"], capture_output=True, check=True)

    versions = [v.strip() for v in git_out.stdout.decode("utf-8").split("\n")]
    versions = [v for v in versions if re.match(r"^v[\d\.]+$", v)]  # include vX.Y.Z only

    versions = [packaging.version.parse(v) for v in versions]

    max_version = max(versions)

    if max_version.public != version[1:]:
        print("Stable version is v{}. This version is {}.".format(max_version.public, version))
        return False
    else:
        print("This version {} is the stable version".format(version))
        return True


if __name__ == "__main__":
    main()
