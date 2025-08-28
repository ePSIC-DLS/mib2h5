"""Command-line interface for mib2h5 package."""

import argparse
import sys
from importlib.metadata import PackageNotFoundError, version

from ._wrapper import convert
from .constants import (
    DEFAULT_COMPRESSION_LEVEL,
    DEFAULT_COMPRESSOR,
    DEFAULT_DATASET_NAME,
    DEFAULT_OUTPUT_DIRECTORY,
    DEFAULT_SHUFFLE,
)


def create_parser() -> argparse.ArgumentParser:
    """Create argument parser for mib2h5 CLI.

    Returns
    -------
    argparse.ArgumentParser
        the argument parser for mib2h5 CLI
    """
    parser = argparse.ArgumentParser(
        description="Convert MIB file to HDF5 file",
        prog="mib2h5"
    )

    parser.add_argument(
        "-i", "--input",
        dest="filename",
        required=True,
        help="input MIB file path"
    )

    parser.add_argument(
        "-o", "--output-directory",
        default=DEFAULT_OUTPUT_DIRECTORY,
        help=f"output directory (default: {DEFAULT_OUTPUT_DIRECTORY})"
    )

    parser.add_argument(
        "-d", "--merlin-dset-name",
        default=DEFAULT_DATASET_NAME,
        help=("Merlin frames dataset name in HDF5 file "
              f"(default: {DEFAULT_DATASET_NAME})")
    )

    parser.add_argument(
        "-c", "--compressor",
        default=DEFAULT_COMPRESSOR,
        help=f"compression used (default: {DEFAULT_COMPRESSOR})"
    )

    parser.add_argument(
        "-s", "--shuffle",
        type=int,
        default=DEFAULT_SHUFFLE,
        help=f"shuffle filter setting (default: {DEFAULT_SHUFFLE})"
    )

    parser.add_argument(
        "-l", "--compression-level",
        type=int,
        default=DEFAULT_COMPRESSION_LEVEL,
        help=f"compression level 0-9 (default: {DEFAULT_COMPRESSION_LEVEL})"
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
    result = convert(
        filename=args.filename,
        output_directory=args.output_directory,
        merlin_dset_name=args.merlin_dset_name,
        compressor=args.compressor,
        shuffle=args.shuffle,
        compression_level=args.compression_level
    )

    return result


if __name__ == "__main__":
    sys.exit(main())
