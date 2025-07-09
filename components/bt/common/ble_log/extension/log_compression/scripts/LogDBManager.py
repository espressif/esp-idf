# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# ruff: noqa: UP007
"""
Log Database Manager
====================
Manages the storage and retrieval of compressed log metadata using a multiprocessing-safe database.
"""

import atexit
import hashlib
import json
import logging
import os
import tempfile
import threading
from collections.abc import Mapping
from pathlib import Path
from types import TracebackType
from typing import Any
from typing import Union

LOGGER = logging.getLogger('log_db_manager')


class LogDBManager:
    """Manages log compression metadata in a multiprocessing-safe manner."""

    # Operation result codes
    SUCCESS = 0
    LOG_EXISTS = 1
    OPERATION_FAILED = 2

    SOURCE_LOG_UPDATE_FULL = 0
    SOURCE_LOG_UPDATE_PARTIAL = 1
    SOURCE_LOG_UPDATE_NONE = 2

    def __init__(self, data_dir: str, sources: Mapping[str, str], logger: Union[logging.Logger, None] = None):
        """
        Initialize the log database manager.

        Args:
            data_dir: Directory for database files
            sources: List of log sources/modules
            logger: Optional logger instance
        """
        self.logger = logger or LOGGER
        self.data_dir = Path(data_dir)
        self.sources = [s.upper() for s in sources.keys()]
        self.source_cfg = sources
        self.sources_exist: dict[str, bool] = {s.upper(): False for s in sources.keys()}
        self.sources_updated: dict[str, int] = {s.upper(): 0 for s in sources.keys()}

        # Create database directory
        self.data_dir.mkdir(parents=True, exist_ok=True)

        # Setup multiprocessing infrastructure
        self.shared_data: dict[str, Any] = dict()
        self.locks: dict[str, Any] = dict()
        self.global_lock = threading.Lock()

        # Initialize sources
        self._initialize_sources()

        self._closed = False
        self.stop_event = threading.Event()
        atexit.register(self.safe_close)

        self.logger.info(f'LogDB initialized for {len(self.sources)} sources in {self.data_dir}')

    def _initialize_sources(self) -> None:
        """Initialize database structures for all sources."""
        with self.global_lock:
            for source in self.sources:
                # Create source-specific structures
                self.shared_data[source] = dict(
                    {
                        'config': '',
                        'files': dict(),  # file_path -> {src_hash, compressed_hash}
                        'logs': dict(),  # log_id -> log_data
                        'index': dict(),  # unique_key -> log_id
                        'max_id': 0,
                    }
                )

                # Create source-specific lock
                self.locks[source] = threading.Lock()

                # Load existing data
                self._load_source(source)

    def _source_file_path(self, source: str) -> Path:
        """Get file path for a source's database file."""
        return self.data_dir / f'{source}_logs.json'

    def _file_hash(self, file_path: Union[str, Path]) -> str:
        """Compute SHA256 hash of a file's contents."""
        hasher = hashlib.sha256()
        file_path = Path(file_path)
        try:
            with file_path.open('rb') as f:
                while chunk := f.read(8192):
                    hasher.update(chunk)
            return hasher.hexdigest()
        except OSError as e:
            self.logger.error(f'Failed to compute hash for {file_path}: {e}')
            return ''

    def is_config_updated(self, source: str) -> bool:
        return bool(self.source_cfg[source] != self.shared_data[source]['config'])

    def is_file_processed(self, source: str, src_path: Union[str, Path], compressed_path: Union[str, Path]) -> bool:
        """
        Check if a file has already been processed.

        Args:
            source: Log source/module
            src_path: Original source file path
            compressed_path: Compressed version path

        Returns:
            True if file has been processed, False otherwise
        """
        source = source.upper()
        if source not in self.shared_data:
            self.logger.error(f'Unknown source: {source}')
            return False

        src_hash = self._file_hash(src_path)
        compressed_hash = self._file_hash(compressed_path) if Path(compressed_path).exists() else ''

        with self.locks[source]:
            files = self.shared_data[source]['files']
            file_info = files.get(str(src_path))

            # Check if file is registered and hashes match
            if file_info:
                if file_info.get('src_hash') == src_hash and file_info.get('compressed_hash') == compressed_hash:
                    return True

                # Update hashes if changed
                file_info['src_hash'] = src_hash
                if compressed_hash:
                    file_info['compressed_hash'] = compressed_hash
                return False

            # New file
            files[str(src_path)] = dict({'src_hash': src_hash, 'compressed_hash': compressed_hash})
            return False

    def mark_file_processed(self, source: str, src_path: Union[str, Path], compressed_path: Union[str, Path]) -> None:
        """
        Mark a file as successfully processed.

        Args:
            source: Log source/module
            src_path: Original source file path
            compressed_path: Compressed version path
        """
        source = source.upper()
        if source not in self.shared_data:
            return

        src_hash = self._file_hash(src_path)
        compressed_hash = self._file_hash(compressed_path)

        with self.locks[source]:
            files = self.shared_data[source]['files']
            file_info = files.get(str(src_path), dict())
            file_info.update({'src_hash': src_hash, 'compressed_hash': compressed_hash})
            files[str(src_path)] = file_info

    def _load_source(self, source: str) -> None:
        """Load source data from JSON file."""
        db_file = self._source_file_path(source)

        # Create empty database if not exists
        if not db_file.exists():
            with db_file.open('w') as f:
                json.dump({'config': '', 'files': {}, 'logs': {}}, f)
            return

        try:
            with db_file.open('r') as f:
                data = json.load(f)
        except (OSError, json.JSONDecodeError) as e:
            self.logger.error(f'Error loading {source} database: {e}')
            return

        files = {}
        logs = {}
        indexes = {}

        with self.locks[source]:
            source_db = self.shared_data[source]

            # Load config
            source_db['config'] = data.get('config', '')

            # Load files
            source_db['files'].clear()
            for path, info in data.get('files', {}).items():
                files[path] = dict(info)

            source_db['files'].update(files)

            # Load logs
            source_db['logs'].clear()
            source_db['index'].clear()
            max_id = 0

            for log_id, log_data in data.get('logs', {}).items():
                log_id = int(log_id)
                logs[log_id] = dict(log_data)
                unique_key = self._log_unique_key(log_data)
                indexes[unique_key] = log_id
                if log_id > max_id:
                    max_id = log_id
            source_db['logs'].update(logs)
            source_db['index'].update(indexes)
            source_db['max_id'] = max_id
            self.sources_updated[source] = len(logs)
            self.sources_exist[source] = bool(data.get('logs'))

        self.logger.info(f'Loaded {len(data.get("logs", {}))} logs for {source}')

    def _save_source(self, source: str) -> bool:
        """Save source data to JSON file using atomic write."""
        db_file = self._source_file_path(source)

        # Prepare data
        with self.locks[source]:
            source_db = self.shared_data[source]
            config = self.source_cfg[source]
            files_data = {path: dict(info) for path, info in source_db['files'].items()}
            logs_data = {log_id: dict(data) for log_id, data in source_db['logs'].items()}

            data = {'config': config, 'files': files_data, 'logs': logs_data}

        # Atomic write
        tmp_path = None
        try:
            with tempfile.NamedTemporaryFile(mode='w', dir=self.data_dir, delete=False) as tmp_file:
                json.dump(data, tmp_file, indent=2)
                tmp_path = tmp_file.name

            # Replace original file
            os.replace(tmp_path, db_file)
            return True
        except (OSError, TypeError) as e:
            self.logger.error(f'Error saving {source} database: {e}')
            if tmp_path and Path(tmp_path).exists():
                Path(tmp_path).unlink()
            return False

    def _log_unique_key(self, log_data: dict) -> tuple:
        """
        Create a unique key for a log entry.

        Args:
            log_data: Log entry dictionary

        Returns:
            Unique key tuple
        """
        return (log_data['tag'], log_data['format'], log_data['caller'], log_data['file'], log_data['line_number'])

    def add_log(
        self,
        source: str,
        log_tag: str,
        log_format: str,
        log_line_number: int,
        hexify: bool,
        caller_func: str,
        caller_line: int,
        file_name: str,
    ) -> tuple[int, int]:
        """
        Add a new log entry to the database if it doesn't exist.

        Args:
            source: Log source/module
            log_tag: Log tag
            log_format: Log format string
            log_line_number: Log Line number
            hexify: Whether the log can be hexified
            caller_func: Calling function name
            caller_line: Calling function line number
            file_name: Source file name

        Returns:
            Tuple (result_code, log_id)
        """
        if self._closed:
            return self.OPERATION_FAILED, 0

        source = source.upper()
        if source not in self.shared_data:
            self.logger.error(f'Unknown source: {source}')
            return self.OPERATION_FAILED, 0

        log_data = {
            'tag': log_tag,
            'format': log_format,
            'line_number': log_line_number,
            'hexify': hexify,
            'caller': caller_func,
            'caller_line': caller_line,
            'file': file_name,
        }

        unique_key = self._log_unique_key(log_data)

        with self.locks[source]:
            source_db = self.shared_data[source]

            # Check if log exists
            if unique_key in source_db['index']:
                existing_id = source_db['index'][unique_key]
                return self.LOG_EXISTS, existing_id

            # Create new log entry
            new_id = source_db['max_id'] + 1
            log_data['id'] = new_id

            source_db['logs'][new_id] = dict(log_data)
            source_db['index'][unique_key] = new_id
            source_db['max_id'] = new_id

            self.logger.info(f'Added new log [{source}]: ID={new_id}, Tag={log_tag}')
            return self.SUCCESS, new_id

    def remove_log(self, source: str, log_id: int) -> bool:
        """
        Remove a log entry from the database.

        Args:
            source: Log source/module
            log_id: ID of log to remove

        Returns:
            True if successful, False otherwise
        """
        source = source.upper()
        if source not in self.shared_data:
            return False

        with self.locks[source]:
            source_db = self.shared_data[source]

            if log_id not in source_db['logs']:
                return False

            # Remove from indexes
            log_data = dict(source_db['logs'][log_id])
            unique_key = self._log_unique_key(log_data)

            if unique_key in source_db['index']:
                del source_db['index'][unique_key]
            del source_db['logs'][log_id]

            if log_id == source_db['max_id']:
                source_db['max_id'] = max(source_db['logs'].keys()) if source_db['logs'] else 0

            return True

    def source_update_state(self, source: str) -> int:
        if self.is_config_updated(source):
            return self.SOURCE_LOG_UPDATE_FULL
        if self.sources_updated[source] == len(self.shared_data[source]['logs']):
            return self.SOURCE_LOG_UPDATE_NONE
        elif self.sources_updated[source] == 0 and len(self.shared_data[source]['logs']) != 0:
            return self.SOURCE_LOG_UPDATE_FULL
        else:
            return self.SOURCE_LOG_UPDATE_PARTIAL

    def save_source(self, source: str) -> bool:
        """Save a single source's data to file."""
        return self._save_source(source.upper())

    def save_all(self) -> int:
        """Save all sources' data to files."""
        self.logger.debug('Saving all sources...')
        success_count = 0

        for source in self.sources:
            if (not self.is_config_updated(source)) and self.sources_updated[source] == len(
                self.shared_data[source]['logs']
            ):
                self.logger.info(f'{source} has not updated data')
                continue
            if self._save_source(source):
                success_count += 1

        self.logger.info(f'Saved {success_count}/{len(self.sources)} sources')
        return success_count

    def safe_close(self) -> None:
        """Safe close method that handles atexit callbacks."""
        if not self._closed:
            try:
                self.close()
            except Exception as e:
                # Log to stderr as logging may be shutdown
                print(f'Error during safe_close: {e}', flush=True)

    def close(self) -> None:
        """Cleanup resources."""
        if self._closed:
            return

        self._closed = True

        try:
            if not self.stop_event.is_set():
                self.stop_event.set()
        except Exception:
            pass

        self.logger.info('Closing database manager...')
        self.stop_event.set()

        # Final save
        self.logger.info('Performing final save...')
        self.save_all()

        # Shutdown manager
        self.logger.info('Database manager closed')

    def __enter__(self) -> 'LogDBManager':
        return self

    def __exit__(
        self,
        exc_type: Union[type[BaseException], None],
        exc_val: Union[BaseException, None],
        exc_tb: Union[TracebackType, None],
    ) -> None:
        self.close()
