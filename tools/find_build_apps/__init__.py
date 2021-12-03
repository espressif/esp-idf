from .cmake import BUILD_SYSTEM_CMAKE, CMakeBuildSystem
from .common import (DEFAULT_TARGET, BuildError, BuildItem, BuildSystem, ConfigRule, config_rules_from_str,
                     setup_logging)

BUILD_SYSTEMS = {
    BUILD_SYSTEM_CMAKE: CMakeBuildSystem,
}

__all__ = [
    'BuildItem',
    'BuildSystem',
    'BuildError',
    'ConfigRule',
    'config_rules_from_str',
    'setup_logging',
    'DEFAULT_TARGET',
    'CMakeBuildSystem',
    'BUILD_SYSTEM_CMAKE',
    'BUILD_SYSTEMS',
]
