import json
import os
import sys
import subprocess
import shlex
import time
import re
from threading import Thread
import threading

from idf_py_actions.errors import FatalError
from idf_py_actions.tools import ensure_build_directory

PYTHON = sys.executable


def action_extensions(base_actions, project_path):
    OPENOCD_OUT_FILE = "openocd_out.txt"
    GDBGUI_OUT_FILE = "gdbgui_out.txt"
    # Internal dictionary of currently active processes, threads and their output files
    processes = {"threads_to_join": [], "openocd_issues": None}

    def _check_for_common_openocd_issues(file_name, print_all=True):
        if processes["openocd_issues"] is not None:
            return processes["openocd_issues"]
        try:
            message = "Please check JTAG connection!"
            with open(file_name, "r") as f:
                content = f.read()
                if print_all:
                    print(content)
                if re.search(r"Address already in use", content):
                    message = ("Please check if another process uses the mentioned ports. OpenOCD already running, perhaps in the background?\n"
                               "Please list all processes to check if OpenOCD is already running; if so, terminate it before starting OpenOCD from idf.py")
        finally:
            processes["openocd_issues"] = message
            return message

    def _check_openocd_errors(fail_if_openocd_failed, target, ctx):
        if fail_if_openocd_failed:
            if "openocd" in processes and processes["openocd"] is not None:
                p = processes["openocd"]
                name = processes["openocd_outfile_name"]
                # watch OpenOCD (for 5x500ms) to check if it hasn't terminated or outputs an error
                for _ in range(5):
                    if p.poll() is not None:
                        print("OpenOCD exited with {}".format(p.poll()))
                        break
                    with open(name, "r") as f:
                        content = f.read()
                        if re.search(r"no device found", content):
                            break
                        if re.search(r"Listening on port \d+ for gdb connections", content):
                            # expect OpenOCD has started successfully - stop watching
                            return
                    time.sleep(0.5)
                else:
                    return
                # OpenOCD exited or error message detected -> print possible output and terminate
                raise FatalError('Action "{}" failed due to errors in OpenOCD:\n{}'.format(target, _check_for_common_openocd_issues(name)), ctx)

    def _terminate_async_target(target):
        if target in processes and processes[target] is not None:
            try:
                if target + "_outfile" in processes:
                    processes[target + "_outfile"].close()
                p = processes[target]
                if p.poll() is None:
                    p.terminate()
                    # waiting 10x100ms for the process to terminate gracefully
                    for _ in range(10):
                        if p.poll() is not None:
                            break
                        time.sleep(0.1)
                    else:
                        p.kill()
                if target + "_outfile_name" in processes:
                    if target == "openocd":
                        print(_check_for_common_openocd_issues(processes[target + "_outfile_name"], print_all=False))
                    os.unlink(processes[target + "_outfile_name"])
            except Exception as e:
                print(e)
                print("Failed to close/kill {}".format(target))
            processes[target] = None  # to indicate this has ended

    def _get_commandline_options(ctx):
        """ Return all the command line options up to first action """
        # This approach ignores argument parsing done Click
        result = []

        for arg in sys.argv:
            if arg in ctx.command.commands_with_aliases:
                break

            result.append(arg)

        return result

    def create_local_gdbinit(gdbinit, elf_file):
        with open(gdbinit, "w") as f:
            f.write("target remote :3333\n")
            if os.name == "nt":
                elf_file = elf_file.replace('\\','\\\\')
            f.write("symbol-file {}\n".format(elf_file))
            f.write("mon reset halt\n")
            f.write("flushregs\n")
            f.write("thb app_main\n")
            f.write("c\n")

    def debug_cleanup():
        print("cleaning up debug targets")
        for t in processes["threads_to_join"]:
            if threading.currentThread() != t:
                t.join()
        _terminate_async_target("openocd")
        _terminate_async_target("gdbgui")
        _terminate_async_target("gdb")

    def post_debug(action, ctx, args, **kwargs):
        """ Deal with asynchronous targets, such as openocd running in background """
        if kwargs["block"] == 1:
            for target in ["openocd", "gdbgui"]:
                if target in processes and processes[target] is not None:
                    break
            else:
                return
            try:
                p = processes[target]
                name = processes[target + "_outfile_name"]
                pos = 0
                while True:
                    with open(name, "r") as f:
                        f.seek(pos)
                        for line in f:
                            print(line.rstrip())
                        pos = f.tell()
                    if p.poll() is not None:
                        print('"{}" exited with {}'.format(target, p.poll()))
                        break
                    time.sleep(0.5)
            except KeyboardInterrupt:
                print("Terminated -> exiting debug utility targets")
        _terminate_async_target("openocd")
        _terminate_async_target("gdbgui")

    def get_project_desc(args, ctx):
        desc_path = os.path.join(args.build_dir, "project_description.json")
        if not os.path.exists(desc_path):
            ensure_build_directory(args, ctx.info_name)
        with open(desc_path, "r") as f:
            project_desc = json.load(f)
            return project_desc

    def openocd(action, ctx, args, openocd_scripts, openocd_commands):
        """
        Execute openocd as external tool
        """
        OPENOCD_TAGET_CONFIG = {
            "esp32": "-f board/esp32-wrover-kit-3.3v.cfg",
            "esp32s2": "-f board/esp32s2-kaluga-1.cfg",
        }
        if os.getenv("OPENOCD_SCRIPTS") is None:
            raise FatalError("OPENOCD_SCRIPTS not found in the environment: Please run export.sh/export.bat", ctx)
        openocd_arguments = os.getenv("OPENOCD_COMMANDS") if openocd_commands is None else openocd_commands
        project_desc = get_project_desc(args, ctx)
        if openocd_arguments is None:
            # use default value if commands not defined in the environment nor command line
            target = project_desc["target"]
            default_args = "-f interface/ftdi/esp32_devkitj_v1.cfg -f target/{}.cfg".format(target)
            openocd_arguments = OPENOCD_TAGET_CONFIG.get(target, default_args)
            print('Note: OpenOCD cfg not found (via env variable OPENOCD_COMMANDS nor as a --openocd-commands argument)\n'
                  'OpenOCD arguments default to: "{}"'.format(openocd_arguments))
        # script directory is taken from the environment by OpenOCD, update only if command line arguments to override
        if openocd_scripts is not None:
            openocd_arguments += " -s {}".format(openocd_scripts)
        local_dir = project_desc["build_dir"]
        args = ["openocd"] + shlex.split(openocd_arguments)
        openocd_out_name = os.path.join(local_dir, OPENOCD_OUT_FILE)
        openocd_out = open(openocd_out_name, "a+")
        try:
            process = subprocess.Popen(args, stdout=openocd_out, stderr=subprocess.STDOUT, bufsize=1)
        except Exception as e:
            print(e)
            raise FatalError("Error starting openocd. Please make sure it is installed and is present in executable paths", ctx)

        processes["openocd"] = process
        processes["openocd_outfile"] = openocd_out
        processes["openocd_outfile_name"] = openocd_out_name
        print("OpenOCD started as a background task {}".format(process.pid))

    def gdbui(action, ctx, args, gdbgui_port, gdbinit, require_openocd):
        """
        Asynchronous GDB-UI target
        """
        project_desc = get_project_desc(args, ctx)
        local_dir = project_desc["build_dir"]
        gdb = project_desc["monitor_toolprefix"] + "gdb"
        if gdbinit is None:
            gdbinit = os.path.join(local_dir, 'gdbinit')
            create_local_gdbinit(gdbinit, os.path.join(args.build_dir, project_desc["app_elf"]))
        args = ["gdbgui", "-g", gdb, '--gdb-args="-x={}"'.format(gdbinit)]
        if gdbgui_port is not None:
            args += ["--port", gdbgui_port]
        gdbgui_out_name = os.path.join(local_dir, GDBGUI_OUT_FILE)
        gdbgui_out = open(gdbgui_out_name, 'a+')
        env = os.environ.copy()
        # The only known solution for https://github.com/cs01/gdbgui/issues/359 is to set the following environment
        # variable. The greenlet package cannot be downgraded for compatibility with other requirements (gdbgui,
        # pygdbmi).
        env['PURE_PYTHON'] = '1'
        try:
            process = subprocess.Popen(args, stdout=gdbgui_out, stderr=subprocess.STDOUT, bufsize=1, env=env)
        except Exception as e:
            print(e)
            if sys.version_info[:2] >= (3, 11):
                raise SystemExit('Unfortunately, gdbgui is supported only with Python 3.10 or older. '
                                 'See: https://github.com/espressif/esp-idf/issues/10116. '
                                 'Please use "idf.py gdb" or debug in Eclipse/Vscode instead.')
            raise FatalError('Error starting gdbgui. Please make sure gdbgui can be started', ctx)

        processes["gdbgui"] = process
        processes["gdbgui_outfile"] = gdbgui_out
        processes["gdbgui_outfile_name"] = gdbgui_out_name
        print("gdbgui started as a background task {}".format(process.pid))
        _check_openocd_errors(fail_if_openocd_failed, action, ctx)

    def global_callback(ctx, global_args, tasks):
        def move_to_front(task_name):
            for index, task in enumerate(tasks):
                if task.name == task_name:
                    tasks.insert(0, tasks.pop(index))
                    break

        debug_targets = any([task.name in ("openocd", "gdbgui") for task in tasks])
        if debug_targets:
            # Register the meta cleanup callback -> called on FatalError
            ctx.meta["cleanup"] = debug_cleanup
            move_to_front("gdbgui")     # possibly 2nd
            move_to_front("openocd")    # always 1st
            # followed by "monitor", "gdb" or "gdbtui" in any order

            post_action = ctx.invoke(ctx.command.get_command(ctx, "post_debug"))
            if any([task.name in ("monitor", "gdb", "gdbtui") for task in tasks]):
                post_action.action_args["block"] = 0
            else:
                post_action.action_args["block"] = 1
            tasks.append(post_action)   # always last
        if any([task.name == "openocd" for task in tasks]):
            for task in tasks:
                if task.name in ("gdb", "gdbgui", "gdbtui"):
                    task.action_args["require_openocd"] = True

    def run_gdb(gdb_args):
        p = subprocess.Popen(gdb_args)
        processes["gdb"] = p
        return p.wait()

    def gdbtui(action, ctx, args, gdbinit, require_openocd):
        """
        Synchronous GDB target with text ui mode
        """
        gdb(action, ctx, args, 1, gdbinit, require_openocd)

    def gdb(action, ctx, args, gdb_tui, gdbinit, require_openocd):
        """
        Synchronous GDB target
        """
        watch_openocd = Thread(target=_check_openocd_errors, args=(fail_if_openocd_failed, action, ctx, ))
        watch_openocd.start()
        processes["threads_to_join"].append(watch_openocd)
        desc_path = os.path.join(args.build_dir, "project_description.json")
        if not os.path.exists(desc_path):
            ensure_build_directory(args, ctx.info_name)
        with open(desc_path, "r") as f:
            project_desc = json.load(f)

        elf_file = os.path.join(args.build_dir, project_desc["app_elf"])
        if not os.path.exists(elf_file):
            raise FatalError("ELF file not found. You need to build & flash the project before running debug targets", ctx)
        gdb = project_desc["monitor_toolprefix"] + "gdb"
        local_dir = project_desc["build_dir"]
        if gdbinit is None:
            gdbinit = os.path.join(local_dir, 'gdbinit')
            create_local_gdbinit(gdbinit, elf_file)
        args = [gdb, '-x={}'.format(gdbinit)]
        if gdb_tui is not None:
            args += ['-tui']
        t = Thread(target=run_gdb, args=(args, ))
        t.start()
        while True:
            try:
                t.join()
                break
            except KeyboardInterrupt:
                # Catching Keyboard interrupt, as this is used for breaking running program in gdb
                continue
            finally:
                watch_openocd.join()
                try:
                    processes["threads_to_join"].remove(watch_openocd)
                except ValueError:
                    # Valid scenario: watch_openocd task won't be in the list if openocd not started from idf.py
                    pass

    fail_if_openocd_failed = {
        "names": ["--require-openocd", "--require_openocd"],
        "help":
        ("Fail this target if openocd (this targets dependency) failed.\n"),
        "is_flag": True,
        "default": False,
    }
    gdbinit = {
        "names": ["--gdbinit"],
        "help": ("Specify the name of gdbinit file to use\n"),
        "default": None,
    }
    debug_actions = {
        "global_action_callbacks": [global_callback],
        "actions": {
            "openocd": {
                "callback": openocd,
                "help": "Run openocd from current path",
                "options": [
                    {
                        "names": ["--openocd-scripts", "--openocd_scripts"],
                        "help":
                        ("Script directory for openocd cfg files.\n"),
                        "default":
                        None,
                    },
                    {
                        "names": ["--openocd-commands", "--openocd_commands"],
                        "help":
                        ("Command line arguments for openocd.\n"),
                        "default": None,
                    }
                ],
                "order_dependencies": ["all", "flash"],
            },
            "gdb": {
                "callback": gdb,
                "help": "Run the GDB.",
                "options": [
                    {
                        "names": ["--gdb-tui", "--gdb_tui"],
                        "help":
                        ("run gdb in TUI mode\n"),
                        "default":
                        None,
                    }, gdbinit, fail_if_openocd_failed
                ],
                "order_dependencies": ["all", "flash"],
            },
            "gdbgui": {
                "callback": gdbui,
                "help": "GDB UI in default browser.",
                "options": [
                    {
                        "names": ["--gdbgui-port", "--gdbgui_port"],
                        "help":
                        ("The port on which gdbgui will be hosted. Default: 5000\n"),
                        "default":
                        None,
                    }, gdbinit, fail_if_openocd_failed
                ],
                "order_dependencies": ["all", "flash"],
            },
            "gdbtui": {
                "callback": gdbtui,
                "help": "GDB TUI mode.",
                "options": [gdbinit, fail_if_openocd_failed],
                "order_dependencies": ["all", "flash"],
            },
            "post_debug": {
                "callback": post_debug,
                "help": "Utility target to read the output of async debug action and stop them.",
                "options": [
                    {
                        "names": ["--block", "--block"],
                        "help":
                        ("Set to 1 for blocking the console on the outputs of async debug actions\n"),
                        "default": 0,
                    },
                ],
                "order_dependencies": [],
            },
        },
    }

    return debug_actions
