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

## Examples

### Basic Example

```python
from mib2h5 import convert

try:
    convert("input.mib")
except (ValueError, RuntimeError):
    print("Conversion failed.")
else:
    print("Conversion successful!")
```

### Advanced Example

```python
from mib2h5 import convert

try:
    convert(
        ["file1.mib", "file2.mib", "file3.mib"],
        output_dir="/path/to/output",
        include_metadata=True,
        dataset_key="/rawdata",
        metadata_key="/meta",
        use_compression=True,
        reshape_to="10x10",
        report_progress=True,
        timeout_seconds=300
    )
except (ValueError, RuntimeError):
    print("Conversion failed.")
else:
    print("Conversion successful!")
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
    reshape_to=None,
    report_progress=True,
    timeout_seconds=900
)
```

### Parameters

- `input_files`: Path to input MIB file(s) (string or list of strings)
- `output_dir`: Directory for output HDF5 file(s) (default: current directory)
- `include_metadata`: Whether to include metadata in HDF5 file (default: True)
- `dataset_key`: HDF5 dataset key for frames (default: "/data")
- `metadata_key`: HDF5 group key for metadata (default: "/metadata")
- `use_compression`: Enable compression (default: False)
- `reshape_to`: Reshape dimensions string like "10x10" (default: None)
- `report_progress`: Report conversion progress (default: True)
- `timeout_seconds`: Timeout in seconds (default: 900)

## Licence

MIT
