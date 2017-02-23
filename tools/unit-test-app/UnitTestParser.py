import yaml
import os
import re
import sys
import shutil


MODULE_MAP = yaml.load(open("ModuleDefinition.yml", "r"))

TEST_CASE_PATTERN = {
    "initial condition": "UTINIT1",
    "SDK": "ESP32_IDF",
    "level": "Unit",
    "execution time": 0,
    "Test App": "UT",
    "auto test": "Yes",
    "category": "Function",
    "test point 1": "basic function",
    "version": "v1 (2016-12-06)",
    "test environment": "UT_T1_1",
    "expected result": "1. set succeed"
}

CONFIG_FILE_PATTERN = {
    "Config": {"execute count": 1, "execute order": "in order"},
    "DUT": [],
    "Filter": [{"Add": {"ID": []}}]
}

test_cases = list()
test_ids = {}
test_ids_by_job = {}
unit_jobs = {}

os.chdir(os.path.join("..", ".."))
IDF_PATH = os.getcwd()


class Parser(object):
    @classmethod
    def parse_test_folders(cls):
        test_folder_paths = list()
        os.chdir(os.path.join(IDF_PATH, "components"))
        component_dirs = os.listdir(".")
        for dir in component_dirs:
            os.chdir(dir)
            if "test" in os.listdir("."):
                test_folder_paths.append(os.path.join(os.getcwd(), "test"))
            os.chdir("..")
        Parser.parse_test_files(test_folder_paths)

    @classmethod
    def parse_test_files(cls, test_folder_paths):
        for path in test_folder_paths:
            os.chdir(path)
            for file_path in os.listdir("."):
                if file_path[-2:] == ".c":
                    Parser.read_test_file(os.path.join(os.getcwd(), file_path), len(test_cases)+1)
        os.chdir(os.path.join("..", ".."))
        Parser.dump_test_cases(test_cases)

    @classmethod
    def read_test_file(cls, test_file_path, file_index):
        test_index = 0
        with open(test_file_path, "r") as file:
            for line in file:
                if re.match("TEST_CASE", line):
                    test_index += 1
                    tags = re.split(r"[\[\]\"]", line)
                    Parser.parse_test_cases(file_index, test_index, tags)


    @classmethod
    def parse_test_cases(cls, file_index, test_index, tags):
        ci_ready = "Yes"
        test_env = "UT_T1_1"
        for tag in tags:
            if tag == "ignore":
                ci_ready = "No"
            if re.match("test_env=", tag):
                test_env = tag[9:]
        module_name = tags[4]
        try:
            MODULE_MAP[module_name]
        except KeyError:
            module_name = "misc"
        id = "UT_%s_%s_%03d%02d" % (MODULE_MAP[module_name]['module abbr'],
                                    MODULE_MAP[module_name]['sub module abbr'],
                                    file_index, test_index)
        test_case = dict(TEST_CASE_PATTERN)
        test_case.update({"module": MODULE_MAP[module_name]['module'], 
                          "CI ready": ci_ready,
                          "cmd set": ["IDFUnitTest/UnitTest", [tags[1]]],
                          "ID": id,
                          "test point 2": module_name,
                          "steps": tags[1],
                          "comment": tags[1],
                          "test environment": test_env,
                          "sub module": MODULE_MAP[module_name]['sub module'],
                          "summary": tags[1]})
        if test_case["CI ready"] == "Yes":
            if test_ids.has_key(test_env):
                test_ids[test_env].append(id)
            else:
                test_ids.update({test_env: [id]})
        test_cases.append(test_case)

    @classmethod
    def dump_test_cases(cls, test_cases):
        os.chdir(os.path.join(IDF_PATH, "components", "idf_test", "unit_test"))
        with open ("TestCaseAll.yml", "wb+") as f:
            yaml.dump({"test cases": test_cases}, f, allow_unicode=True, default_flow_style=False)

    @classmethod
    def dump_ci_config(cls):
        Parser.split_test_cases()
        os.chdir(os.path.join(IDF_PATH, "components", "idf_test", "unit_test"))
        if not os.path.exists("CIConfigs"):
            os.makedirs("CIConfigs")
        os.chdir("CIConfigs")
        for unit_job in unit_jobs:
            job = dict(CONFIG_FILE_PATTERN)
            job.update({"DUT": ["UT1"]})
            job.update({"Filter": [{"Add": {"ID": test_ids_by_job[unit_job]}}]})
            with open (unit_job + ".yml", "wb+") as f:
                yaml.dump(job, f, allow_unicode=True, default_flow_style=False)

    @classmethod
    def split_test_cases(cls):
        for job in unit_jobs:
            test_ids_by_job.update({job: list()})
        for test_env in test_ids:
            available_jobs = list()
            for job in unit_jobs:
                if test_env in unit_jobs[job]:
                    available_jobs.append(job)
            for idx, job in enumerate(available_jobs):
                test_ids_by_job[job] += (test_ids[test_env][idx*len(test_ids[test_env])/len(available_jobs):(idx+1)*len(test_ids[test_env])/len(available_jobs)])

    @classmethod
    def parse_gitlab_ci(cls):
        os.chdir(IDF_PATH)
        with open(".gitlab-ci.yml", "rb") as f:
            gitlab_ci = yaml.load(f)
            keys = gitlab_ci.keys()
            for key in keys:
                if re.match("UT_", key):
                    test_env = gitlab_ci[key]["tags"]
                    unit_job = key
                    key = {}
                    key.update({unit_job: test_env})
                    unit_jobs.update(key)

    @classmethod
    def copy_module_def_file(cls):
        src = os.path.join(IDF_PATH, "tools", "unit-test-app", "ModuleDefinition.yml")
        dst = os.path.join(IDF_PATH, "components", "idf_test", "unit_test")
        shutil.copy(src, dst)


def main():
    Parser.parse_test_folders()
    Parser.parse_gitlab_ci()
    Parser.dump_ci_config()
    Parser.copy_module_def_file()


if __name__ == '__main__':
    main()
