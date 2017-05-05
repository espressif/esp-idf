import yaml
import os
import re
import shutil
import subprocess

from copy import deepcopy
import CreateSectionTable


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


class Parser(object):
    """ parse unit test cases from build files and create files for test bench """

    TAG_PATTERN = re.compile("([^=]+)(=)?(.+)?")
    DESCRIPTION_PATTERN = re.compile("\[([^]\[]+)\]")

    def __init__(self, idf_path=os.getenv("IDF_PATH")):
        self.test_env_tags = {}
        self.unit_jobs = {}
        self.file_name_cache = {}
        self.idf_path = idf_path
        self.tag_def = yaml.load(open(os.path.join(idf_path, "tools", "unit-test-app", "tools",
                                                   "TagDefinition.yml"), "r"))
        self.module_map = yaml.load(open(os.path.join(idf_path, "tools", "unit-test-app", "tools",
                                                      "ModuleDefinition.yml"), "r"))

    def parse_test_cases_from_elf(self, elf_file):
        """
        parse test cases from elf and save test cases to unit test folder
        :param elf_file: elf file path
        """
        subprocess.check_output('xtensa-esp32-elf-objdump -t {} | grep \ test_desc > case_address.tmp'.format(elf_file),
                                shell=True)
        subprocess.check_output('xtensa-esp32-elf-objdump -s {} > section_table.tmp'.format(elf_file), shell=True)

        table = CreateSectionTable.SectionTable("section_table.tmp")
        test_cases = []
        with open("case_address.tmp", "r") as f:
            for line in f:
                # process symbol table like: "3ffb4310 l     O .dram0.data	00000018 test_desc_33$5010"
                line = line.split()
                test_addr = int(line[0], 16)
                section = line[3]

                name_addr = table.get_unsigned_int(section, test_addr, 4)
                desc_addr = table.get_unsigned_int(section, test_addr + 4, 4)
                file_name_addr = table.get_unsigned_int(section, test_addr + 12, 4)
                name = table.get_string("any", name_addr)
                desc = table.get_string("any", desc_addr)
                file_name = table.get_string("any", file_name_addr)

                tc = self.parse_one_test_case(name, desc, file_name)
                if tc["CI ready"] == "Yes":
                    # update test env list and the cases of same env list
                    if tc["test environment"] in self.test_env_tags:
                        self.test_env_tags[tc["test environment"]].append(tc["ID"])
                    else:
                        self.test_env_tags.update({tc["test environment"]: [tc["ID"]]})
                test_cases.append(tc)

        os.remove("section_table.tmp")
        os.remove("case_address.tmp")

        self.dump_test_cases(test_cases)

    def parse_case_properities(self, tags_raw):
        """
        parse test case tags (properities) with the following rules:
            * first tag is always group of test cases, it's mandatory
            * the rest tags should be [type=value]. 
                * if the type have default value, then [type] equal to [type=default_value].
                * if the type don't don't exist, then equal to [type=omitted_value]
            default_value and omitted_value are defined in TagDefinition.yml
        :param tags_raw: raw tag string
        :return: tag dict
        """
        tags = self.DESCRIPTION_PATTERN.findall(tags_raw)
        assert len(tags) > 0
        p = dict([(k, self.tag_def[k]["omitted"]) for k in self.tag_def])
        p["module"] = tags[0]

        if p["module"] not in self.module_map:
            p["module"] = "misc"

        # parsing rest tags, [type=value], =value is optional
        for tag in tags[1:]:
            match = self.TAG_PATTERN.search(tag)
            assert match is not None
            tag_type = match.group(1)
            tag_value = match.group(3)
            if match.group(2) == "=" and tag_value is None:
                # [tag_type=] means tag_value is empty string
                tag_value = ""
            if tag_type in p:
                if tag_value is None:
                    p[tag_type] = self.tag_def[tag_type]["default"]
                else:
                    p[tag_type] = tag_value
            else:
                # ignore not defined tag type
                pass
        return p

    def parse_one_test_case(self, name, description, file_name):
        """
        parse one test case
        :param name: test case name (summary)
        :param description: test case description (tag string)
        :param file_name: the file defines this test case
        :return: parsed test case
        """
        prop = self.parse_case_properities(description)

        if file_name in self.file_name_cache:
            self.file_name_cache[file_name] += 1
        else:
            self.file_name_cache[file_name] = 1

        tc_id = "UT_%s_%s_%03d%02d" % (self.module_map[prop["module"]]['module abbr'],
                                       self.module_map[prop["module"]]['sub module abbr'],
                                       hash(file_name) % 1000,
                                       self.file_name_cache[file_name])
        test_case = deepcopy(TEST_CASE_PATTERN)
        test_case.update({"module": self.module_map[prop["module"]]['module'],
                          "CI ready": "No" if prop["ignore"] == "Yes" else "Yes",
                          "cmd set": ["IDFUnitTest/UnitTest", [name]],
                          "ID": tc_id,
                          "test point 2": prop["module"],
                          "steps": name,
                          "test environment": prop["test_env"],
                          "sub module": self.module_map[prop["module"]]['sub module'],
                          "summary": name})
        return test_case

    def dump_test_cases(self, test_cases):
        """
        dump parsed test cases to YAML file for test bench input
        :param test_cases: parsed test cases
        """
        with open(os.path.join(self.idf_path, "components", "idf_test", "unit_test", "TestCaseAll.yml"), "wb+") as f:
            yaml.dump({"test cases": test_cases}, f, allow_unicode=True, default_flow_style=False)

    def copy_module_def_file(self):
        """ copy module def file to artifact path """
        src = os.path.join(self.idf_path, "tools", "unit-test-app", "tools", "ModuleDefinition.yml")
        dst = os.path.join(self.idf_path, "components", "idf_test")
        shutil.copy(src, dst)


def test_parser():
    parser = Parser()
    # test parsing tags
    # parsing module only and module in module list
    prop = parser.parse_case_properities("[esp32]")
    assert prop["module"] == "esp32"
    # module not in module list
    prop = parser.parse_case_properities("[not_in_list]")
    assert prop["module"] == "misc"
    # parsing a default tag, a tag with assigned value
    prop = parser.parse_case_properities("[esp32][ignore][test_env=ABCD][not_support1][not_support2=ABCD]")
    assert prop["ignore"] == "Yes" and prop["test_env"] == "ABCD" \
        and "not_support1" not in prop and "not_supported2" not in prop
    # parsing omitted value
    prop = parser.parse_case_properities("[esp32]")
    assert prop["ignore"] == "No" and prop["test_env"] == "UT_T1_1"
    # parsing with incorrect format
    try:
        parser.parse_case_properities("abcd")
        assert False
    except AssertionError:
        pass
    # skip invalid data parse, [type=] assigns empty string to type
    prop = parser.parse_case_properities("[esp32]abdc aaaa [ignore=]")
    assert prop["module"] == "esp32" and prop["ignore"] == ""
    # skip mis-paired []
    prop = parser.parse_case_properities("[esp32][[ignore=b]][]][test_env=AAA]]")
    assert prop["module"] == "esp32" and prop["ignore"] == "b" and prop["test_env"] == "AAA"


def main():
    test_parser()

    idf_path = os.getenv("IDF_PATH")
    elf_path = os.path.join(idf_path, "tools", "unit-test-app", "build", "unit-test-app.elf")

    parser = Parser(idf_path)
    parser.parse_test_cases_from_elf(elf_path)
    parser.copy_module_def_file()


if __name__ == '__main__':
    main()
