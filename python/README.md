# mib2h5 Python Wrapper

Python bindings for `mib2h5`, the `.mib` to HDF5 converter.

## Introduction

This package provides a Python interface to convert MerlinEM `.mib` files from
Quantum Detector to HDF5 format (`.h5`).

## Installation

### Prerequisites

- Python >= 3.10
- HDF5 library (1.10.4 or newer)
- Cython >= 3.0 - only required when building from source

### Via pip

```bash
python -m pip install mib2h5
```

### Via conda

```bash
conda install -c conda-forge mib2h5
```

### Via pipx

You can use `pipx` to install only the command-line tool `mib2h5`:

```bash
pipx install mib2h5
```

### Building from Source

```bash
# Set HDF5 location (if not in standard system paths)
export HDF5_ROOT=/path/to/hdf5

# Install the package
python -m pip install .

# For development, install in editable mode
python -m pip install -e .
```

## Usage

### Command-line Interface

This is the same as in `mib2h5`, see the usage [here](../README.md).

### Python API

#### Basic Example

```python
from mib2h5 import convert

# Convert a single file
convert("input.mib")

# Convert with custom output directory
convert("input.mib", output_dir="/path/to/output")
```

#### Advanced Example

```python
from mib2h5 import convert

# Convert multiple files
files = ["file1.mib", "file2.mib", "file3.mib"]
convert(
    files,
    output_dir="/path/to/output",
    include_metadata=True,
    dataset_key="/rawdata",
    use_compression=True
)

# Handle errors for multiple files
try:
    convert(files, output_dir="/path/to/output")
except RuntimeError as e:
    print(f"Some files failed to convert:\n{e}")
```

#### Compression Control

Compression is controlled via environment variables when
`use_compression=True`:

```python
import os
from mib2h5 import convert

# Set compression parameters
os.environ['MIB2H5_SHUFFLE'] = '2'           # 0-2, default: 2
os.environ['MIB2H5_COMPRESSION_LEVEL'] = '5' # 0-9, default: 9

# Convert with compression
convert("input.mib", use_compression=True)
```

## API Reference

```python
convert(
    input_files,
    output_dir=None,
    include_metadata=True,
    dataset_key="/data",
    metadata_key="/metadata",
    use_compression=False,
    reshape_dims=None,
    report_progress=True,
    timeout_seconds=900
)
```

### Parameters

- `input_files`: Path to input MIB file(s) (string or list of strings)
- `output_dir`: Directory for output HDF5 file(s) (default: current directory)
- `include_metadata`: Whether to include metadata in HDF5 file (default: True)
- `dataset_key`: HDF5 dataset path for frames (default: "/data")
- `metadata_key`: HDF5 group path for metadata (default: "/metadata")
*[Not yet implemented]*
- `use_compression`: Enable Blosc compression if available (default: False)
- `reshape_dims`: Reshape dimensions string like "10x10" (default: None)
*[Not yet implemented]*
- `report_progress`: Report conversion progress (default: True)
*[Not yet implemented]*
- `timeout_seconds`: Timeout in seconds, 0 for no limit (default: 900)
*[Not yet implemented]*

### Notes

- When converting multiple files, the function continues processing remaining
files even if some fail
- All errors are collected and reported together at the end
- Compression settings are controlled via environment variables
`MIB2H5_SHUFFLE` and `MIB2H5_COMPRESSION_LEVEL`

## Licence

MIT
