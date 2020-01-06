import os
from .common import BuildSystem

# Same for the Makefile projects:
MAKE_PROJECT_LINE = r"include $(IDF_PATH)/make/project.mk"

BUILD_SYSTEM_MAKE = "make"


class MakeBuildSystem(BuildSystem):
    NAME = BUILD_SYSTEM_MAKE

    @staticmethod
    def build(build_item):
        raise NotImplementedError()

    @staticmethod
    def is_app(path):
        makefile_path = os.path.join(path, "Makefile")
        if not os.path.exists(makefile_path):
            return False
        with open(makefile_path, "r") as makefile:
            makefile_content = makefile.read()
        if MAKE_PROJECT_LINE not in makefile_content:
            return False
        return True

    @staticmethod
    def supported_targets(app_path):
        return ["esp32"]
