# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import logging
import os
import re
import sys
import tarfile
import tempfile
import time
import zipfile
from functools import wraps
from typing import Any
from typing import Callable
from typing import Dict
from typing import List
from typing import Optional
from typing import Union

import gitlab

TR = Callable[..., Any]

logging.basicConfig(level=logging.INFO)


def retry(func: TR) -> TR:
    """
    This wrapper will only catch several exception types associated with
    "network issues" and retry the whole function.
    """

    @wraps(func)
    def wrapper(self: 'Gitlab', *args: Any, **kwargs: Any) -> Any:
        retried = 0
        while True:
            try:
                res = func(self, *args, **kwargs)
            except (IOError, EOFError, gitlab.exceptions.GitlabError) as e:
                if isinstance(e, gitlab.exceptions.GitlabError):
                    if e.response_code == 500:
                        # retry on this error
                        pass
                    elif e.response_code == 404 and os.environ.get('LOCAL_GITLAB_HTTPS_HOST', None):
                        # remove the environment variable "LOCAL_GITLAB_HTTPS_HOST" and retry
                        os.environ.pop('LOCAL_GITLAB_HTTPS_HOST', None)
                    else:
                        # other GitlabErrors aren't retried
                        raise e
                retried += 1
                if retried > self.DOWNLOAD_ERROR_MAX_RETRIES:
                    raise e  # get out of the loop
                else:
                    logging.warning(
                        'Network failure in {}, retrying ({})'.format(getattr(func, '__name__', '(unknown callable)'),
                                                                      retried))
                    time.sleep(2 ** retried)  # wait a bit more after each retry
                    continue
            else:
                break
        return res

    return wrapper


class Gitlab(object):
    JOB_NAME_PATTERN = re.compile(r'(\w+)(\s+(\d+)/(\d+))?')

    DOWNLOAD_ERROR_MAX_RETRIES = 3
    DEFAULT_BUILD_CHILD_PIPELINE_NAME = 'Build Child Pipeline'

    def __init__(self, project_id: Union[int, str, None] = None):
        config_data_from_env = os.getenv('PYTHON_GITLAB_CONFIG')
        if config_data_from_env:
            # prefer to load config from env variable
            with tempfile.NamedTemporaryFile('w', delete=False) as temp_file:
                temp_file.write(config_data_from_env)
            config_files = [temp_file.name]  # type: Optional[List[str]]
        else:
            # otherwise try to use config file at local filesystem
            config_files = None
        self._init_gitlab_inst(project_id, config_files)

    @retry
    def _init_gitlab_inst(self, project_id: Optional[int], config_files: Optional[List[str]]) -> None:
        gitlab_id = os.getenv('LOCAL_GITLAB_HTTPS_HOST')  # if None, will use the default gitlab server
        self.gitlab_inst = gitlab.Gitlab.from_config(gitlab_id=gitlab_id, config_files=config_files)

        try:
            self.gitlab_inst.auth()
        except gitlab.exceptions.GitlabAuthenticationError:
            msg = """To call gitlab apis locally, please create ~/.python-gitlab.cfg with the following content:

        [global]
        default = internal
        ssl_verify = true
        timeout = 5

        [internal]
        url = <OUR INTERNAL HTTPS SERVER URL>
        private_token = <YOUR PERSONAL ACCESS TOKEN>
        api_version = 4
        """
            raise SystemExit(msg)

        if project_id:
            self.project = self.gitlab_inst.projects.get(project_id, lazy=True)
        else:
            self.project = None

    @retry
    def get_project_id(self, name: str, namespace: Optional[str] = None) -> int:
        """
        search project ID by name

        :param name: project name
        :param namespace: namespace to match when we have multiple project with same name
        :return: project ID
        """
        projects = self.gitlab_inst.projects.list(search=name)
        res = []
        for project in projects:
            if namespace is None:
                if len(projects) == 1:
                    res.append(project.id)
                    break

            if project.namespace['path'] == namespace:
                if project.name == name:
                    res.insert(0, project.id)
                else:
                    res.append(project.id)

        if not res:
            raise ValueError("Can't find project")
        return int(res[0])

    @retry
    def download_artifacts(self, job_id: int, destination: str) -> None:
        """
        download full job artifacts and extract to destination.

        :param job_id: Gitlab CI job ID
        :param destination: extract artifacts to path.
        """
        job = self.project.jobs.get(job_id)

        with tempfile.NamedTemporaryFile(delete=False) as temp_file:
            job.artifacts(streamed=True, action=temp_file.write)

        with zipfile.ZipFile(temp_file.name, 'r') as archive_file:
            archive_file.extractall(destination)

    @retry
    def download_artifact(self, job_id: int, artifact_path: List[str], destination: Optional[str] = None) -> List[bytes]:
        """
        download specific path of job artifacts and extract to destination.

        :param job_id: Gitlab CI job ID
        :param artifact_path: list of path in artifacts (relative path to artifact root path)
        :param destination: destination of artifact. Do not save to file if destination is None
        :return: A list of artifact file raw data.
        """
        job = self.project.jobs.get(job_id)

        raw_data_list = []

        for a_path in artifact_path:
            try:
                data = job.artifact(a_path)  # type: bytes
            except gitlab.GitlabGetError as e:
                logging.error("Failed to download '{}' from job {}".format(a_path, job_id))
                raise e
            raw_data_list.append(data)
            if destination:
                file_path = os.path.join(destination, a_path)
                try:
                    os.makedirs(os.path.dirname(file_path))
                except OSError:
                    # already exists
                    pass
                with open(file_path, 'wb') as f:
                    f.write(data)

        return raw_data_list

    @retry
    def find_job_id(self, job_name: str, pipeline_id: Optional[str] = None, job_status: str = 'success') -> List[Dict]:
        """
        Get Job ID from job name of specific pipeline

        :param job_name: job name
        :param pipeline_id: If None, will get pipeline id from CI pre-defined variable.
        :param job_status: status of job. One pipeline could have multiple jobs with same name after retry.
                           job_status is used to filter these jobs.
        :return: a list of job IDs (parallel job will generate multiple jobs)
        """
        job_id_list = []
        if pipeline_id is None:
            pipeline_id = os.getenv('CI_PIPELINE_ID')
        pipeline = self.project.pipelines.get(pipeline_id)
        jobs = pipeline.jobs.list(all=True)
        for job in jobs:
            match = self.JOB_NAME_PATTERN.match(job.name)
            if match:
                if match.group(1) == job_name and job.status == job_status:
                    job_id_list.append({'id': job.id, 'parallel_num': match.group(3)})
        return job_id_list

    @retry
    def download_archive(self, ref: str, destination: str, project_id: Optional[int] = None,
                         cache_dir: Optional[str] = None) -> str:
        """
        Download archive of certain commit of a repository and extract to destination path

        :param ref: commit or branch name
        :param destination: destination path of extracted archive file
        :param project_id: download project of current instance if project_id is None
        :return: root path name of archive file
        """
        if project_id is None:
            project = self.project
        else:
            project = self.gitlab_inst.projects.get(project_id)

        if cache_dir:
            local_archive_file = os.path.join(cache_dir, f'{ref}.tar.gz')
            os.makedirs(os.path.dirname(local_archive_file), exist_ok=True)
            if os.path.isfile(local_archive_file):
                logging.info('Use cached archive file. Skipping download...')
            else:
                with open(local_archive_file, 'wb') as fw:
                    try:
                        project.repository_archive(sha=ref, streamed=True, action=fw.write)
                    except gitlab.GitlabGetError as e:
                        logging.error('Failed to archive from project {}'.format(project_id))
                        raise e
                logging.info('Downloaded archive size: {:.03f}MB'.format(
                    float(os.path.getsize(local_archive_file)) / (1024 * 1024)))

            return self.decompress_archive(local_archive_file, destination)

        # no cache
        with tempfile.NamedTemporaryFile(delete=False) as temp_file:
            try:
                project.repository_archive(sha=ref, streamed=True, action=temp_file.write)
            except gitlab.GitlabGetError as e:
                logging.error('Failed to archive from project {}'.format(project_id))
                raise e

        logging.info('Downloaded archive size: {:.03f}MB'.format(float(os.path.getsize(temp_file.name)) / (1024 * 1024)))

        return self.decompress_archive(temp_file.name, destination)

    @staticmethod
    def decompress_archive(path: str, destination: str) -> str:
        full_destination = os.path.abspath(destination)
        # By default max path length is set to 260 characters
        # Prefix `\\?\` extends it to 32,767 characters
        if sys.platform == 'win32':
            full_destination = '\\\\?\\' + full_destination

        try:
            with tarfile.open(path, 'r') as archive_file:
                root_name = archive_file.getnames()[0]
                archive_file.extractall(full_destination)
        except tarfile.TarError as e:
            logging.error(f'Error while decompressing archive {path}')
            raise e

        return os.path.join(os.path.realpath(destination), root_name)

    def get_job_tags(self, job_id: int) -> str:
        """
        Get tags of a job

        :param job_id: job id
        :return: comma-separated tags of the job
        """
        job = self.project.jobs.get(job_id)
        return ','.join(job.tag_list)

    def get_downstream_pipeline_ids(self, main_pipeline_id: int) -> List[int]:
        """
        Retrieve the IDs of all downstream child pipelines for a given main pipeline.

        :param main_pipeline_id: The ID of the main pipeline to start the search.
        :return: A list of IDs of all downstream child pipelines.
        """
        bridge_pipeline_ids = []
        child_pipeline_ids = []

        main_pipeline_bridges = self.project.pipelines.get(main_pipeline_id).bridges.list()
        for bridge in main_pipeline_bridges:
            downstream_pipeline = bridge.attributes.get('downstream_pipeline')
            if not downstream_pipeline:
                continue
            bridge_pipeline_ids.append(downstream_pipeline['id'])

        for bridge_pipeline_id in bridge_pipeline_ids:
            child_pipeline_ids.append(bridge_pipeline_id)
            bridge_pipeline = self.project.pipelines.get(bridge_pipeline_id)

            if not bridge_pipeline.name == self.DEFAULT_BUILD_CHILD_PIPELINE_NAME:
                continue

            child_bridges = bridge_pipeline.bridges.list()
            for child_bridge in child_bridges:
                downstream_child_pipeline = child_bridge.attributes.get('downstream_pipeline')
                if not downstream_child_pipeline:
                    continue
                child_pipeline_ids.append(downstream_child_pipeline.get('id'))

        return [pid for pid in child_pipeline_ids if pid is not None]

    def retry_failed_jobs(self, pipeline_id: int, retry_allowed_failures: bool = False) -> List[int]:
        """
        Retry failed jobs for a specific pipeline. Optionally include jobs marked as 'allowed failures'.

        :param pipeline_id: ID of the pipeline whose failed jobs are to be retried.
        :param retry_allowed_failures: Whether to retry jobs that are marked as allowed failures.
        """
        jobs_succeeded_retry = []
        pipeline_ids = [pipeline_id] + self.get_downstream_pipeline_ids(pipeline_id)
        logging.info(f'Retrying jobs for pipelines: {pipeline_ids}')
        for pid in pipeline_ids:
            pipeline = self.project.pipelines.get(pid)
            job_ids_to_retry = [
                job.id
                for job in pipeline.jobs.list(scope='failed')
                if retry_allowed_failures or not job.attributes.get('allow_failure', False)
            ]
            logging.info(f'Failed jobs for pipeline {pid}: {job_ids_to_retry}')
            for job_id in job_ids_to_retry:
                try:
                    res = self.project.jobs.get(job_id).retry()
                    jobs_succeeded_retry.append(job_id)
                    logging.info(f'Retried job {job_id} with result {res}')
                except Exception as e:
                    logging.error(f'Failed to retry job {job_id}: {str(e)}')

        return jobs_succeeded_retry


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('action')
    parser.add_argument('project_id', type=int)
    parser.add_argument('--pipeline_id', '-i', type=int, default=None)
    parser.add_argument('--ref', '-r', default='master')
    parser.add_argument('--job_id', '-j', type=int, default=None)
    parser.add_argument('--job_name', '-n', default=None)
    parser.add_argument('--project_name', '-m', default=None)
    parser.add_argument('--destination', '-d', default=None)
    parser.add_argument('--artifact_path', '-a', nargs='*', default=None)
    parser.add_argument(
        '--retry-allowed-failures', action='store_true', help='Flag to retry jobs marked as allowed failures'
    )
    args = parser.parse_args()

    gitlab_inst = Gitlab(args.project_id)
    if args.action == 'download_artifacts':
        gitlab_inst.download_artifacts(args.job_id, args.destination)
    if args.action == 'download_artifact':
        gitlab_inst.download_artifact(args.job_id, args.artifact_path, args.destination)
    elif args.action == 'find_job_id':
        job_ids = gitlab_inst.find_job_id(args.job_name, args.pipeline_id)
        print(';'.join([','.join([str(j['id']), j['parallel_num']]) for j in job_ids]))
    elif args.action == 'download_archive':
        gitlab_inst.download_archive(args.ref, args.destination)
    elif args.action == 'get_project_id':
        ret = gitlab_inst.get_project_id(args.project_name)
        print('project id: {}'.format(ret))
    elif args.action == 'retry_failed_jobs':
        res = gitlab_inst.retry_failed_jobs(args.pipeline_id, args.retry_allowed_failures)
        print('jobs retried successfully: {}'.format(res))
    elif args.action == 'get_job_tags':
        ret = gitlab_inst.get_job_tags(args.job_id)
        print(ret)


if __name__ == '__main__':
    main()
