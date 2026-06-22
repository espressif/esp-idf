# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from doc_auto_translate import MrLabelGuard  # noqa: E402
from doc_auto_translate import TranslationPlan  # noqa: E402
from doc_auto_translate import _add_entire  # noqa: E402
from doc_auto_translate import _parse_label_list  # noqa: E402
from doc_auto_translate import _parse_trans_file  # noqa: E402
from doc_auto_translate import build_plan  # noqa: E402
from doc_auto_translate import is_include_only_zh_cn  # noqa: E402
from doc_auto_translate import run_translation  # noqa: E402


class TestIncludeOnlyZhCn(unittest.TestCase):
    def test_single_en_include(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'docs/zh_CN/foo.rst'
            p.parent.mkdir(parents=True)
            p.write_text('.. include:: ../../../en/api-reference/foo.rst\n', encoding='utf-8')
            self.assertTrue(is_include_only_zh_cn(str(p), Path(tmp)))

    def test_with_link_to_translation(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'docs/zh_CN/security/vulnerabilities.rst'
            p.parent.mkdir(parents=True)
            p.write_text(
                ':link_to_translation:`en:[English]`\n\n.. include:: ../../en/security/vulnerabilities.rst\n',
                encoding='utf-8',
            )
            self.assertTrue(is_include_only_zh_cn(str(p), Path(tmp)))

    def test_chinese_body_is_not_include_only(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'docs/zh_CN/api-guides/external-ram.rst'
            p.parent.mkdir(parents=True)
            p.write_text('片外 RAM\n========\n\n简介\n', encoding='utf-8')
            self.assertFalse(is_include_only_zh_cn(str(p), Path(tmp)))


class TestBuildPlanMergeBaseIncludeStub(unittest.TestCase):
    @mock.patch(
        'doc_auto_translate._load_tcf',
        return_value={'specified_files': ['docs/en'], 'compare_branch': 'master'},
    )
    @mock.patch('doc_auto_translate._merge_base', return_value='base123')
    @mock.patch(
        'doc_auto_translate._changed_doc_paths',
        return_value=['docs/en/security/tee/tee-ota.rst'],
    )
    @mock.patch('doc_auto_translate.is_include_only_zh_cn', return_value=False)
    @mock.patch('doc_auto_translate.is_include_only_zh_cn_at_rev', return_value=True)
    @mock.patch('doc_auto_translate._path_exists_at_rev', return_value=True)
    def test_en_change_entire_when_merge_base_was_include_stub(
        self,
        _exists: mock.Mock,
        _at_rev: mock.Mock,
        _head: mock.Mock,
        _changed: mock.Mock,
        _mb: mock.Mock,
        _tcf: mock.Mock,
    ) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            zh = root / 'docs/zh_CN/security/tee/tee-ota.rst'
            zh.parent.mkdir(parents=True)
            zh.write_text('TEE 空中更新\n========\n', encoding='utf-8')
            plan, _ = build_plan(root)
        self.assertEqual(plan.entire, ['docs/en/security/tee/tee-ota.rst'])
        self.assertEqual(plan.incremental, [])

    @mock.patch(
        'doc_auto_translate._load_tcf',
        return_value={'specified_files': ['docs/zh_CN'], 'compare_branch': 'master'},
    )
    @mock.patch('doc_auto_translate._merge_base', return_value='base123')
    @mock.patch(
        'doc_auto_translate._changed_doc_paths',
        return_value=['docs/zh_CN/security/tee/tee-ota.rst'],
    )
    @mock.patch('doc_auto_translate.is_include_only_zh_cn', return_value=False)
    @mock.patch('doc_auto_translate.is_include_only_zh_cn_at_rev', return_value=True)
    def test_zh_change_entire_when_merge_base_was_include_stub(
        self,
        _at_rev: mock.Mock,
        _head: mock.Mock,
        _changed: mock.Mock,
        _mb: mock.Mock,
        _tcf: mock.Mock,
    ) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            plan, _ = build_plan(Path(tmp))
        self.assertEqual(plan.entire, ['docs/en/security/tee/tee-ota.rst'])
        self.assertEqual(plan.incremental, [])

    @mock.patch(
        'doc_auto_translate._load_tcf',
        return_value={'specified_files': ['docs/en'], 'compare_branch': 'master'},
    )
    @mock.patch('doc_auto_translate._merge_base', return_value='base123')
    @mock.patch(
        'doc_auto_translate._changed_doc_paths',
        return_value=[
            'docs/en/security/tee/tee-ota.rst',
            'docs/zh_CN/security/tee/tee-ota.rst',
        ],
    )
    @mock.patch('doc_auto_translate.is_include_only_zh_cn', return_value=True)
    @mock.patch('doc_auto_translate.is_include_only_zh_cn_at_rev', return_value=False)
    @mock.patch('doc_auto_translate._path_exists_at_rev', return_value=True)
    def test_both_en_and_zh_change_entire_once(
        self,
        _exists: mock.Mock,
        _at_rev: mock.Mock,
        _head: mock.Mock,
        _changed: mock.Mock,
        _mb: mock.Mock,
        _tcf: mock.Mock,
    ) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            zh = root / 'docs/zh_CN/security/tee/tee-ota.rst'
            zh.parent.mkdir(parents=True)
            zh.write_text('.. include:: ../../../en/security/tee/tee-ota.rst\n', encoding='utf-8')
            plan, _ = build_plan(root)
        self.assertEqual(plan.entire, ['docs/en/security/tee/tee-ota.rst'])
        self.assertEqual(plan.incremental, [])


class TestAddEntire(unittest.TestCase):
    def test_deduplicates_en_path(self) -> None:
        plan = TranslationPlan()
        _add_entire(plan, 'docs/en/foo.rst')
        _add_entire(plan, 'docs/en/foo.rst')
        self.assertEqual(plan.entire, ['docs/en/foo.rst'])

    def test_zh_path_resolves_to_single_en_entry(self) -> None:
        plan = TranslationPlan()
        _add_entire(plan, 'docs/en/foo.rst')
        _add_entire(plan, 'docs/zh_CN/foo.rst')
        self.assertEqual(plan.entire, ['docs/en/foo.rst'])


class TestMrLabelGuard(unittest.TestCase):
    def test_parse_label_list(self) -> None:
        self.assertEqual(_parse_label_list('auto-translate::full'), ['auto-translate::full'])
        self.assertEqual(
            _parse_label_list('auto-translate::full, auto-translate::incremental'),
            ['auto-translate::full', 'auto-translate::incremental'],
        )

    @mock.patch.dict(os.environ, {'CI_MERGE_REQUEST_IID': '42'}, clear=False)
    @mock.patch('doc_auto_translate._remove_mr_labels', return_value=['auto-translate::full'])
    @mock.patch('doc_auto_translate._restore_mr_labels')
    def test_restore_on_failure(self, restore: mock.Mock, remove: mock.Mock) -> None:
        with self.assertRaises(RuntimeError):
            with MrLabelGuard(['auto-translate::full']):
                raise RuntimeError('translate failed')
        remove.assert_called_once_with(['auto-translate::full'])
        restore.assert_called_once_with(['auto-translate::full'])

    @mock.patch.dict(os.environ, {'CI_MERGE_REQUEST_IID': '42'}, clear=False)
    @mock.patch('doc_auto_translate._remove_mr_labels', return_value=['auto-translate::full'])
    @mock.patch('doc_auto_translate._restore_mr_labels')
    def test_no_restore_on_success(self, restore: mock.Mock, remove: mock.Mock) -> None:
        with MrLabelGuard(['auto-translate::full']):
            pass
        remove.assert_called_once_with(['auto-translate::full'])
        restore.assert_not_called()

    @mock.patch.dict(os.environ, {}, clear=True)
    @mock.patch('doc_auto_translate._remove_mr_labels')
    def test_skip_without_mr(self, remove: mock.Mock) -> None:
        with MrLabelGuard(['auto-translate::full']):
            pass
        remove.assert_not_called()


class TestTransFilePathNormalization(unittest.TestCase):
    def test_normalize_zh_folder_aliases(self) -> None:
        parsed = _parse_trans_file(
            '["docs/zh_cn/security/tee/tee-ota.rst",'
            ' "docs/zh-cn/security/tee/tee-ota.rst",'
            ' "docs/zh-Hans/security/tee/tee-ota.rst"]'
        )
        self.assertEqual(
            parsed,
            [
                'docs/zh_CN/security/tee/tee-ota.rst',
                'docs/zh_CN/security/tee/tee-ota.rst',
                'docs/zh_CN/security/tee/tee-ota.rst',
            ],
        )


class TestRunTranslationBatchCommit(unittest.TestCase):
    @mock.patch('doc_auto_translate._commit_translated_files')
    @mock.patch('doc_auto_translate._translate_files_local')
    def test_single_commit_for_mixed_buckets(
        self,
        translate_local: mock.Mock,
        commit_files: mock.Mock,
    ) -> None:
        plan = TranslationPlan(
            incremental=['docs/en/a.rst'],
            entire=['docs/en/b.rst'],
        )
        translate_local.side_effect = [
            ['docs/zh_CN/b.rst'],
            ['docs/zh_CN/a.rst'],
        ]
        auto_dir = Path('/tmp/auto-translate')

        run_translation(
            plan,
            auto_dir,
            'key',
            'https://gitlab.example.com',
            'token',
            '123',
            'feature-branch',
            'all',
            diff_base_commit='abc123',
        )

        self.assertEqual(translate_local.call_count, 2)
        translate_local.assert_any_call(
            auto_dir,
            ['docs/en/b.rst'],
            entire=True,
            openai_key='key',
            url='https://gitlab.example.com',
            bot='token',
            project_id='123',
            branch='feature-branch',
        )
        translate_local.assert_any_call(
            auto_dir,
            ['docs/en/a.rst'],
            entire=False,
            openai_key='key',
            url='https://gitlab.example.com',
            bot='token',
            project_id='123',
            branch='feature-branch',
            diff_base_commit='abc123',
        )
        commit_files.assert_called_once_with(
            auto_dir,
            ['docs/zh_CN/b.rst', 'docs/zh_CN/a.rst'],
            'https://gitlab.example.com',
            'token',
            '123',
            'feature-branch',
        )

    @mock.patch('doc_auto_translate._commit_translated_files')
    @mock.patch('doc_auto_translate._translate_files_local')
    def test_no_commit_when_nothing_translated(
        self,
        translate_local: mock.Mock,
        commit_files: mock.Mock,
    ) -> None:
        plan = TranslationPlan()
        auto_dir = Path('/tmp/auto-translate')

        run_translation(
            plan,
            auto_dir,
            'key',
            'https://gitlab.example.com',
            'token',
            '123',
            'feature-branch',
            'all',
        )

        translate_local.assert_not_called()
        commit_files.assert_not_called()


if __name__ == '__main__':
    unittest.main()
