# mib2h5 Python Wrapper

Python bindings for `mib2h5`, the `.mib` to HDF5 converter.

## Introduction

This package provides a Python interface to convert MerlinEM `.mib` files from
Quantum Detector to HDF5 format (`.h5`).

## Installation

### Prerequisites

- Python >= 3.10
- Cython >= 3.0
- HDF5 library (1.10.4 or newer)

### pip

```bash
# Set HDF5 location (if not in standard system paths)
export HDF5_ROOT=/path/to/hdf5

# Install the package
python -m pip install .

# For development, install in editable mode
python -m pip install -e .
```

## Usage

```python
import mib2h5

# Convert a MIB file to HDF5
mib2h5.convert(
    filename="input.mib",
    output_directory="./output/",
    merlin_dset_name="data",
    compressor="blosclz",
    shuffle=2,
    compression_level=9
)
```

## Parameters

- `filename`: Path to input MIB file (required)
- `output_directory`: Directory for output HDF5 file (default: "./")
- `merlin_dset_name`: Name of dataset in HDF5 file (default: "MerlinData")
- `compressor`: Compression algorithm (default: "blosclz", use "" to disable)
- `shuffle`: Shuffle filter setting 0-2 (default: 2)
- `compression_level`: Compression level 0-9 (default: 9)

## Licence

MIT
