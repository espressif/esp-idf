#!/usr/bin/env python
#
# Top-level docs builder
#
# This is just a front-end to sphinx-build that can call it multiple times for different language/target combinations
#
# Will build out to _build/LANG/TARGET by default
#
# Specific custom docs functionality should be added in conf_common.py or in a Sphinx extension, not here.
#
import argparse
import os
import os.path
import subprocess
import sys

LANGUAGES = ["en", "zh_CN"]
TARGETS = ["esp32", "esp32s2"]

def main():
    # check Python dependencies for docs
    try:
        subprocess.check_call([sys.executable,
                               os.path.join(os.environ["IDF_PATH"],
                                            "tools",
                                            "check_python_dependencies.py"),
                               "-r",
                               "{}/docs/requirements.txt".format(os.environ["IDF_PATH"])
                               ])
    except subprocess.CalledProcessError:
        raise SystemExit(2)  # stdout will already have these errors

    parser = argparse.ArgumentParser(description='build_docs.py: Build IDF docs',
                                     prog='build_docs.py')
    parser.add_argument("--language", "-l", choices=LANGUAGES,
                        required=False)
    parser.add_argument("--target", "-t", choices=TARGETS, required=False)
    parser.add_argument("--build-dir", "-b", type=str, default="_build")

    args = parser.parse_args()

    if args.language is None:
        print("Building all languages")
        languages = LANGUAGES
    else:
        languages = [ args.language ]

    if args.target is None:
        print("Building all targets")
        targets = TARGETS
    else:
        targets = [ args.target ]

    for language in languages:
        for target in targets:
            build_dir = os.path.realpath(os.path.join(args.build_dir, language, target))
            build_docs(language, target, build_dir)


def build_docs(language, target, build_dir):
    print("Building language:%s target:%s build_dir:%s" % (language, target, build_dir))
    try:
        os.makedirs(build_dir)
    except OSError:
        pass
    try:
        environ = {}
        environ.update(os.environ)
        environ['BUILDDIR'] = build_dir

        args = [sys.executable, "-m", "sphinx",
                "-b", "html",  # TODO: PDFs
                "-d", os.path.join(build_dir, "doctrees"),
                # TODO: support multiple sphinx-warning.log files, somehow
                "-w", "sphinx-warning.log",
                "-D", "idf_target={}".format(target),
                os.path.join(os.path.abspath(os.path.dirname(__file__)), language), # srcdir for this language
                os.path.join(build_dir, "html")                    # build directory
        ]
        cwd = build_dir  # also run sphinx in the build directory
        print("Running '{}'".format(" ".join(args)))
        subprocess.check_call(args, cwd=cwd, env=environ)
    except subprocess.CalledProcessError:
        print("Sphinx failed for language:%s target:%s" % (language, target))
        raise SystemExit(1)  # rest of the details should be in stdout


if __name__ == "__main__":
    main()
