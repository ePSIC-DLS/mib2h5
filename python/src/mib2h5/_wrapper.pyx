# cython: language_level=3

from .constants import (
    DEFAULT_OUTPUT_DIRECTORY,
    DEFAULT_DATASET_KEY,
    DEFAULT_INCLUDE_METADATA,
    DEFAULT_METADATA_KEY,
    DEFAULT_USE_COMPRESSION,
    DEFAULT_RESHAPE_DIMS,
    DEFAULT_REPORT_PROGRESS,
    DEFAULT_TIMEOUT_SECONDS,
)

cdef extern from "mib_to_h5.h":
    int mib_to_h5_single_file(const char *filename,
                              const char *output_directory,
                              const char *dataset_key,
                              bint include_metadata,
                              const char *compressor,
                              unsigned int shuffle,
                              unsigned int compression_level)

def convert(str filename,
            str output_directory=DEFAULT_OUTPUT_DIRECTORY,
            str merlin_dset_name=DEFAULT_DATASET_NAME,
            str compressor=DEFAULT_COMPRESSOR,
            int shuffle=DEFAULT_SHUFFLE,
            int compression_level=DEFAULT_COMPRESSION_LEVEL):
    """
    Convert a MIB file to HDF5 format.

    Parameters
    ----------
    filename : str
        Path to the input MIB file
    output_directory : str, optional
        Directory where the output HDF5 file will be saved (default: "./")
    merlin_dset_name : str, optional
        Name of the dataset in the HDF5 file (default: "MerlinData")
    compressor : str, optional
        Compression algorithm to use (default: "blosclz")
    shuffle : int, optional
        Shuffle filter setting (default: 2)
    compression_level : int, optional
        Compression level 0-9 (default: 9)

    Returns
    -------
    int
        0 on success, negative value on error

    Raises
    ------
    ValueError
        If required parameters are missing or invalid
    RuntimeError
        If the conversion fails
    """
    if not filename:
        raise ValueError("Input filename cannot be empty.")

    if not output_directory:
        raise ValueError("Output directory cannot be empty.")

    if not merlin_dset_name:
        raise ValueError("Dataset name cannot be empty.")

    if shuffle < 0 or shuffle > 2:
        raise ValueError("Shuffle must be 0, 1, or 2.")

    if compression_level < 0 or compression_level > 9:
        raise ValueError("Compression level must be between 0 and 9.")

    # encode strings to bytes
    cdef bytes b_filename = filename.encode()
    cdef bytes b_output_directory = output_directory.encode()
    cdef bytes b_merlin_dset_name = merlin_dset_name.encode()
    cdef bytes b_compressor = compressor.encode() if compressor else b""

    # call the C function
    cdef int result = mib_to_h5(
        b_filename,
        b_output_directory,
        b_merlin_dset_name,
        b_compressor,
        shuffle,
        compression_level
    )

    if result != 0:
        raise RuntimeError(f"Conversion failed with error code: {result}")

    return result
