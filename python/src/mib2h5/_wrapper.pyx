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

def convert(input_files,
            output_dir=DEFAULT_OUTPUT_DIRECTORY,
            bint include_metadata=DEFAULT_INCLUDE_METADATA,
            str dataset_key=DEFAULT_DATASET_KEY,
            str metadata_key=DEFAULT_METADATA_KEY,
            bint use_compression=DEFAULT_USE_COMPRESSION,
            reshape_dims=DEFAULT_RESHAPE_DIMS,
            bint report_progress=DEFAULT_REPORT_PROGRESS,
            int timeout_seconds=DEFAULT_TIMEOUT_SECONDS):
    """
    Convert MIB file(s) to HDF5 file(s).

    Parameters
    ----------
    input_files : str or list of str
        Path(s) to the input MIB file(s)
    output_dir : str, optional
        Directory where the output HDF5 file(s) will be saved (default:
        current directory)
    include_metadata : bool, optional
        Whether to include metadata in the HDF5 file (default: True)
    dataset_key : str, optional
        HDF5 dataset key for frames (default: "/data")
    metadata_key : str, optional
        HDF5 group path for metadata (default: "/metadata")
    use_compression : bool, optional
        Enable Blosc compression if available (default: False)
        Note: Blosc compression settings can be controlled via
        environment variables:
            - MIB2H5_SHUFFLE (0-2, default: 2)
            - MIB2H5_COMPRESSION_LEVEL (0-9, default: 9)
    reshape_dims : str, optional
        Reshape dimensions string like "10x10" (default: None)
    report_progress : bool, optional
        Report conversion progress (default: True)
    timeout_seconds : int, optional
        Timeout in seconds, 0 for no limit (default: 900)

    Returns
    -------
    None

    Raises
    ------
    ValueError
        If required parameters are missing or invalid
    RuntimeError
        If the conversion fails for any file
    """
    # ensure input_files is a list
    if isinstance(input_files, str):
        files = [input_files]
    else:
        files = list(input_files)

    if not files:
        raise ValueError("No input files provided")

    if not dataset_key:
        raise ValueError("Dataset key cannot be empty")

    if timeout_seconds < 0:
        raise ValueError("Timeout must be non-negative")

    # prepare output directory
    cdef bytes b_output_dir = output_dir.encode() if output_dir else b"./"
    cdef bytes b_dataset_key = dataset_key.encode()

    # determine compressor string based on use_compression
    cdef bytes b_compressor = b"blosclz" if use_compression else b""

    # use default values for shuffle and compression_level
    # these may be overridden by environment variables if provided
    cdef unsigned int shuffle = 2 if use_compression else 0
    cdef unsigned int compression_level = 9 if use_compression else 0

    # track errors for reporting
    errors = []
    successful = []

    # process each file
    cdef bytes b_filename
    cdef int result

    for filename in files:
        if not filename:
            errors.append((filename, "Empty filename"))
            continue

        # encode filename
        b_filename = filename.encode()

        # call the C function for single file
        result = mib_to_h5_single_file(
            b_filename,
            b_output_dir,
            b_dataset_key,
            include_metadata,
            b_compressor,
            shuffle,
            compression_level
        )

        if result == 0:
            successful.append(filename)
        else:
            errors.append((filename,
                           f"Conversion failed with error code: {result}")
                          )


    # report succeeded and failed files if there is error
    if errors:
        error_messages = []
        for filename, msg in errors:
            error_messages.append(f"{filename}: {msg}")

        error_report = "\n".join(error_messages)

        if successful:
            success_msg = f"Successfully converted {len(successful)} file(s)"
            failure_msg = (f"Failed to convert {len(errors)} "
                           f"file(s):\n{error_report}")
            raise RuntimeError(f"{success_msg}\n{failure_msg}")
        else:
            msg = (f"Failed to convert all {len(errors)} "
                   f"file(s):\n{error_report}")
            raise RuntimeError(msg)
