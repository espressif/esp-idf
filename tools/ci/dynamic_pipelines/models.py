# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import inspect
import typing as t
from dataclasses import dataclass
from xml.etree.ElementTree import Element

import yaml


class Job:
    def __init__(
        self,
        *,
        name: str,
        extends: t.Optional[t.List[str]] = None,
        tags: t.Optional[t.List[str]] = None,
        stage: t.Optional[str] = None,
        parallel: int = 1,
        variables: t.Dict[str, str] = None,
        script: t.Optional[t.List[str]] = None,
        before_script: t.Optional[t.List[str]] = None,
        after_script: t.Optional[t.List[str]] = None,
        needs: t.Optional[t.List[str]] = None,
        **kwargs: t.Any,
    ) -> None:
        self.name = name
        self.extends = extends
        self.tags = tags
        self.stage = stage
        self.parallel = parallel
        self.variables = variables or {}
        self.script = script
        self.before_script = before_script
        self.after_script = after_script
        self.needs = needs

        for k, v in kwargs.items():
            setattr(self, k, v)

    def __str__(self) -> str:
        return yaml.dump(self.to_dict())  # type: ignore

    def set_variable(self, key: str, value: str) -> None:
        self.variables[key] = value

    def to_dict(self) -> t.Dict[str, t.Any]:
        res = {}
        for k, v in inspect.getmembers(self):
            if k.startswith('_'):
                continue

            # name is the dict key
            if k == 'name':
                continue

            # parallel 1 is not allowed
            if k == 'parallel' and v == 1:
                continue

            if v is None:
                continue

            if inspect.ismethod(v) or inspect.isfunction(v):
                continue

            res[k] = v

        return {self.name: res}


class EmptyJob(Job):
    def __init__(
        self,
        *,
        name: t.Optional[str] = None,
        tags: t.Optional[t.List[str]] = None,
        stage: t.Optional[str] = None,
        before_script: t.Optional[t.List[str]] = None,
        after_script: t.Optional[t.List[str]] = None,
        **kwargs: t.Any,
    ) -> None:
        super().__init__(
            name=name or 'fake_pass_job',
            tags=tags or ['fast_run', 'shiny'],
            stage=stage or 'build',
            script=['echo "This is a fake job to pass the pipeline"'],
            before_script=before_script or [],
            after_script=after_script or [],
            **kwargs,
        )


class BuildJob(Job):
    def __init__(
        self,
        *,
        extends: t.Optional[t.List[str]] = None,
        tags: t.Optional[t.List[str]] = None,
        stage: t.Optional[str] = None,
        **kwargs: t.Any,
    ) -> None:
        super().__init__(
            extends=extends or ['.dynamic_build_template'],
            tags=tags or ['build', 'shiny'],
            stage=stage or 'build',
            **kwargs,
        )


class TargetTestJob(Job):
    def __init__(
        self,
        *,
        extends: t.Optional[t.List[str]] = None,
        stage: t.Optional[str] = None,
        **kwargs: t.Any,
    ) -> None:
        super().__init__(
            extends=extends or ['.dynamic_target_test_template'],
            stage=stage or 'target_test',
            **kwargs,
        )


@dataclass
class TestCase:
    name: str
    file: str
    time: float
    failure: t.Optional[str] = None
    skipped: t.Optional[str] = None
    ci_job_url: t.Optional[str] = None

    @property
    def is_failure(self) -> bool:
        return self.failure is not None

    @property
    def is_skipped(self) -> bool:
        return self.skipped is not None

    @property
    def is_success(self) -> bool:
        return not self.is_failure and not self.is_skipped

    @classmethod
    def from_test_case_node(cls, node: Element) -> t.Optional['TestCase']:
        if 'name' not in node.attrib:
            print('WARNING: Node Invalid: ', node)
            return None

        kwargs = {
            'name': node.attrib['name'],
            'file': node.attrib.get('file'),
            'time': float(node.attrib.get('time') or 0),
            'ci_job_url': node.attrib.get('ci_job_url') or '',
        }

        failure_node = node.find('failure')
        if failure_node is not None:
            kwargs['failure'] = failure_node.attrib['message']

        skipped_node = node.find('skipped')
        if skipped_node is not None:
            kwargs['skipped'] = skipped_node.attrib['message']

        return cls(**kwargs)  # type: ignore
