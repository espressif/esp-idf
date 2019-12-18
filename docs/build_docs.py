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
from __future__ import print_function
import argparse
import math
import multiprocessing
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
    parser.add_argument("--sphinx-parallel-builds", "-p", choices=["auto"] + [str(x) for x in range(8)],
                        help="Parallel Sphinx builds - number of independent Sphinx builds to run", default="auto")
    parser.add_argument("--sphinx-parallel-jobs", "-j", choices=["auto"] + [str(x) for x in range(8)],
                        help="Sphinx parallel jobs argument - number of threads for each Sphinx build to use", default="auto")

    args = parser.parse_args()

    if args.language is None:
        print("Building all languages")
        languages = LANGUAGES
    else:
        languages = [args.language]

    if args.target is None:
        print("Building all targets")
        targets = TARGETS
    else:
        targets = [args.target]

    num_sphinx_builds = len(languages) * len(targets)
    num_cpus = multiprocessing.cpu_count()

    if args.sphinx_parallel_builds == "auto":
        # at most one sphinx build per CPU, up to the number of CPUs
        args.sphinx_parallel_builds = min(num_sphinx_builds, num_cpus)
    else:
        args.sphinx_parallel_builds = int(args.sphinx_parallel_builds)

    if args.sphinx_parallel_jobs == "auto":
        # N CPUs per build job, rounded up - (maybe smarter to round down to avoid contention, idk)
        args.sphinx_parallel_jobs = int(math.ceil(num_cpus / args.sphinx_parallel_builds))
    else:
        args.sphinx_parallel_jobs = int(args.sphinx_parallel_jobs)

    print("Will use %d parallel builds and %d jobs per build" % (args.sphinx_parallel_builds, args.sphinx_parallel_jobs))
    pool = multiprocessing.Pool(args.sphinx_parallel_builds)

    # make a list of all combinations of build_docs() args as tuples
    #
    # there's probably a fancy way to do this with itertools but this way is actually readable
    entries = []
    for target in targets:
        for language in languages:
            build_dir = os.path.realpath(os.path.join(args.build_dir, language, target))
            entries.append((language, target, build_dir, args.sphinx_parallel_jobs))

    print(entries)
    failures = pool.map(call_build_docs, entries)
    if any(failures):
        if len(entries) > 1:
            print("The following language/target combinations failed to build:")
            for f in failures:
                if f is not None:
                    print("language: %s target: %s" % (f[0], f[1]))
        raise SystemExit(2)


def call_build_docs(entry):
    build_docs(*entry)


def build_docs(language, target, build_dir, sphinx_parallel_jobs=1):
    # Note: because this runs in a multiprocessing Process, everything which happens here should be isolated to a single process
    # (ie it doesn't matter if Sphinx is using global variables, as they're it's own copy of the global variables)

    # wrap stdout & stderr in a way that lets us see which build_docs instance they come from
    #
    # this doesn't apply to subprocesses, they write to OS stdout & stderr so no prefix appears
    prefix = "%s/%s: " % (language, target)

    print("Building in build_dir:%s" % (build_dir))
    try:
        os.makedirs(build_dir)
    except OSError:
        pass

    environ = {}
    environ.update(os.environ)
    environ['BUILDDIR'] = build_dir

    args = [sys.executable, "-u", "-m", "sphinx.cmd.build",
            "-j", str(sphinx_parallel_jobs),
            "-b", "html",  # TODO: PDFs
            "-d", os.path.join(build_dir, "doctrees"),
            # TODO: support multiple sphinx-warning.log files, somehow
            "-w", "sphinx-warning.log",
            "-t", target,
            "-D", "idf_target={}".format(target),
            os.path.join(os.path.abspath(os.path.dirname(__file__)), language),  # srcdir for this language
            os.path.join(build_dir, "html")                    # build directory
            ]
    cwd = build_dir  # also run sphinx in the build directory

    os.chdir(cwd)
    print("Running '%s'" % (" ".join(args)))

    try:
        # Note: we can't call sphinx.cmd.build.main() here as multiprocessing doesn't est >1 layer deep
        # and sphinx.cmd.build() also does a lot of work in the calling thread, especially for j ==1,
        # so using a Pyhthon thread for this part is  a poor option (GIL)
        p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        for c in iter(lambda: p.stdout.readline(), ''):
            sys.stdout.write(prefix)
            sys.stdout.write(c)
    except KeyboardInterrupt:  # this seems to be the only way to get Ctrl-C to kill everything?
        p.kill()
        return


if __name__ == "__main__":
    main()
