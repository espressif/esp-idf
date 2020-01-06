from .common import (
    BuildItem,
    BuildSystem,
    BuildError,
    ConfigRule,
    config_rules_from_str,
    setup_logging,
    DEFAULT_TARGET,
)
from .cmake import CMakeBuildSystem, BUILD_SYSTEM_CMAKE
from .make import MakeBuildSystem, BUILD_SYSTEM_MAKE

BUILD_SYSTEMS = {
    BUILD_SYSTEM_MAKE: MakeBuildSystem,
    BUILD_SYSTEM_CMAKE: CMakeBuildSystem,
}

__all__ = [
    "BuildItem",
    "BuildSystem",
    "BuildError",
    "ConfigRule",
    "config_rules_from_str",
    "setup_logging",
    "DEFAULT_TARGET",
    "CMakeBuildSystem",
    "BUILD_SYSTEM_CMAKE",
    "MakeBuildSystem",
    "BUILD_SYSTEM_MAKE",
    "BUILD_SYSTEMS",
]
