"""Command-line interface for mib2h5 package."""

import argparse
import sys
from importlib.metadata import PackageNotFoundError, version
from pathlib import Path

from ._wrapper import convert
from .constants import (
    DEFAULT_DATASET_KEY,
    DEFAULT_INCLUDE_METADATA,
    DEFAULT_METADATA_KEY,
    DEFAULT_OUTPUT_DIRECTORY,
    DEFAULT_REPORT_PROGRESS,
    DEFAULT_TIMEOUT_SECONDS,
    DEFAULT_USE_COMPRESSION,
)


def create_parser() -> argparse.ArgumentParser:
    """Create argument parser for mib2h5 CLI.

    Returns
    -------
    argparse.ArgumentParser
        the argument parser for mib2h5 CLI
    """
    parser = argparse.ArgumentParser(
        description="Convert MIB file(s) to HDF5 file(s)",
        prog="mib2h5",
        epilog="Environment variables:\n"
               "  MIB2H5_SHUFFLE              Shuffle level for Blosc compression (0-2, default: 2)\n"
               "  MIB2H5_COMPRESSION_LEVEL    Blosc compression level (0-9, default: 9)",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    # positional arguments for input files
    parser.add_argument(
        "input_files",
        nargs="+",
        help="input MIB file(s) to convert"
    )

    # optional arguments
    parser.add_argument(
        "-o", "--output-dir",
        default=DEFAULT_OUTPUT_DIRECTORY,
        help="output directory for HDF5 files (default: current directory)"
    )

    parser.add_argument(
        "-d", "--dataset-key",
        default=DEFAULT_DATASET_KEY,
        help=f"HDF5 dataset path for frames (default: {DEFAULT_DATASET_KEY})"
    )

    parser.add_argument(
        "-c", "--compression",
        action="store_true",
        default=DEFAULT_USE_COMPRESSION,
        help="enable Blosc compression (settings via env vars)"
    )

    parser.add_argument(
        "-N", "--no-metadata",
        action="store_false",
        dest="include_metadata",
        default=DEFAULT_INCLUDE_METADATA,
        help="exclude metadata from HDF5 output"
    )

    parser.add_argument(
        "--metadata-key",
        default=DEFAULT_METADATA_KEY,
        help=f"HDF5 group path for metadata (default: {DEFAULT_METADATA_KEY})"
    )

    parser.add_argument(
        "--no-progress",
        action="store_false",
        dest="report_progress",
        default=DEFAULT_REPORT_PROGRESS,
        help="disable progress reporting"
    )

    parser.add_argument(
        "--timeout",
        type=int,
        default=DEFAULT_TIMEOUT_SECONDS,
        help=("timeout in seconds, 0 for no limit "
              f"(default: {DEFAULT_TIMEOUT_SECONDS})")
    )

    parser.add_argument(
        "--version",
        action="version",
        version=_get_version()
    )

    return parser


def _get_version() -> str:
    """Get package version.

    Returns
    -------
    str
        Package version string
    """
    try:
        return version("mib2h5")
    except PackageNotFoundError:
        return "unknown"


def main() -> int:
    """Entry point for mib2h5 CLI.

    Returns
    -------
    int
        Exit code: 0 for success, non-zero for error
    """
    parser = create_parser()
    args = parser.parse_args()

    # validate input files exist
    missing_files = []
    for filepath in args.input_files:
        if not Path(filepath).exists:
            missing_files.append(filepath)

    if missing_files:
        missing_files_msg = "\n".join(missing_files)
        msg = f"The following input files do not exist: {missing_files_msg}"
        raise FileNotFoundError(msg)

    if args.timeout < 0:
        raise ValueError("Timeout must be non-negative")

    # call the conversion function
    convert(
        input_files=args.input_files,
        output_dir=args.output_dir,
        include_metadata=args.include_metadata,
        dataset_key=args.dataset_key,
        metadata_key=args.metadata_key,
        use_compression=args.compression,
        reshape_dims=None,
        report_progress=args.report_progress,
        timeout_seconds=args.timeout
    )

    return 0


if __name__ == "__main__":
    sys.exit(main())
