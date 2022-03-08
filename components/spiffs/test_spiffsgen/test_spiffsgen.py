#!/usr/bin/env python
# SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import sys
import unittest

try:
    import typing
except ImportError:
    pass

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
try:
    import spiffsgen
except ImportError:
    raise


class SpiffsgenTest(unittest.TestCase):
    def test_configs(self):  # type: () -> None
        """Run spiffsgen with different configs, and check that
        an image is generated (there is no exception), and the image size
        is as expected.
        """
        default_config = dict(
            page_size=256,
            page_ix_len=spiffsgen.SPIFFS_PAGE_IX_LEN,
            block_size=4096,
            block_ix_len=spiffsgen.SPIFFS_BLOCK_IX_LEN,
            meta_len=4,
            obj_name_len=32,
            obj_id_len=spiffsgen.SPIFFS_BLOCK_IX_LEN,
            span_ix_len=spiffsgen.SPIFFS_SPAN_IX_LEN,
            packed=True,
            aligned=True,
            endianness='little',
            use_magic=True,
            use_magic_len=True,
            aligned_obj_ix_tables=False
        )

        def make_config(**kwargs):  # type: (typing.Any) -> spiffsgen.SpiffsBuildConfig
            """Return SpiffsBuildConfig object with configuration set
            by default_config plus any options overridden in kwargs.
            """
            new_config = dict(default_config)
            new_config.update(**kwargs)
            return spiffsgen.SpiffsBuildConfig(**new_config)

        configs = [
            make_config(),
            make_config(use_magic_len=False, use_magic=False, aligned_obj_ix_tables=True),
            make_config(meta_len=4, obj_name_len=16),
            make_config(block_size=8192),
            make_config(page_size=512)
        ]

        image_size = 64 * 1024
        for config in configs:
            spiffs = spiffsgen.SpiffsFS(image_size, config)
            spiffs.create_file('/test', __file__)
            image = spiffs.to_binary()
            self.assertEqual(len(image), image_size)
            # Note: it would be nice to compile spiffs for host with the given
            # config, and verify that the image is parsed correctly.


if __name__ == '__main__':
    unittest.main()
