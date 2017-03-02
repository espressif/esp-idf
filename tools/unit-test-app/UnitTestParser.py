import yaml
import os
import re
import sys
import shutil
import CreateSectionTable


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
    def parse_test_addresses(cls):
        table = CreateSectionTable.SectionTable(os.path.join(IDF_PATH, "tools", "unit-test-app", "build", "tmp"))
        file_index = 1
        test_index = 1
        with open(os.path.join(IDF_PATH, "tools", "unit-test-app", "build", "tests"), "r") as file:
            for line in file:
                line = line.split()
                test = int(line[0],16)
                section = line[3]
                name_addr = table.get_unsigned_int(section, test, 4)
                desc_addr = table.get_unsigned_int(section, test + 4, 4)
                name = table.get_string("any", name_addr)
                desc = table.get_string("any", desc_addr)
                Parser.parse_test_cases(file_index, test_index, "%s, %s" % (name, desc))
                file_index += 1
                test_index += 1
        os.chdir(os.path.join("..", ".."))
        Parser.dump_test_cases(test_cases)

    @classmethod
    def parse_test_cases(cls, file_index, test_index, tags):
        tags = re.split(r"[\[\]\"]", tags)
        ci_ready = "Yes"
        test_env = "UT_T1_1"
        for tag in tags:
            if tag == "ignore":
                ci_ready = "No"
            if re.match("test_env=", tag):
                test_env = tag[9:]
        module_name = tags[1]
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
                          "cmd set": ["IDFUnitTest/UnitTest", [tags[0][:-2]]],
                          "ID": id,
                          "test point 2": module_name,
                          "steps": tags[0][:-2],
                          "comment": tags[0][:-2],
                          "test environment": test_env,
                          "sub module": MODULE_MAP[module_name]['sub module'],
                          "summary": tags[0][:-2]})
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
    os.chdir(os.path.join(IDF_PATH, "tools", "unit-test-app", "build"))
    os.system('xtensa-esp32-elf-objdump -t unit-test-app.elf | grep test_desc > tests')
    os.system('xtensa-esp32-elf-objdump -s unit-test-app.elf > tmp')
    Parser.parse_test_addresses()
    Parser.parse_gitlab_ci()
    Parser.dump_ci_config()
    Parser.copy_module_def_file()


if __name__ == '__main__':
    main()
