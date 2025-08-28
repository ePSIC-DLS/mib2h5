# mib2h5

A lightweight utility to convert MerlinEM `.mib` files from Quantum Detector
to HDF5 format (`.h5`) for scientific data processing.

## Introduction

`mib2h5` converts MerlinEM detector output (`.mib` files) to HDF5 files,
enabling easier data sharing, analysis, and visualisation across research
groups. The tool preserves all metadata and supports frame-by-frame processing
for handling large datasets efficiently.

Below shows the hierarchy of the resulting HDF5 file:

```text
/                                   # Root group
├── data                            # Frames group
│   └── shape: (100, 512, 512)      # (number_of_frames, height, width)
│
└── metadata/                       # Metadata group
    ├── header_id                   # Dataset: ['MQ1', 'MQ1', 'MQ1', ...]
    │   └── shape: (100,)
    │
    ├── acquisition_sequence        # Dataset: [1, 2, 3, ...]
    │   └── shape: (100,)
    │
    ├── data_offset                 # Dataset: [768, 768, 768, ...]
    │   └── shape: (100,)
    │
    ├── chip_count                  # Dataset: [4, 4, 4, ...]
    │   └── shape: (100,)
    │
    │── <OTHER HEADERS...>
    │
    └── conversion_info/            # Information about the conversion process
        ├── converter_version       # Version number like 1.0.0
        ├── conversion_date         # Timestamp of the conversion
        └── source_file             # The name of the converted MIB file
```

[Documentation](https://epsic-dls.github.io/mib2h5/) is available.

## Installation

### C

#### Prerequisites

- GCC compiler (7.4 or newer, older versions may work)
- HDF5 development libraries (1.10.4 or newer)
- Make (3.82 or newer)

#### Building from Source

```bash
# Clone the repository
git clone git@github.com:ePSIC-DLS/mib2h5.git
cd mib2h5

# Configure and build
./configure --prefix=/path/to/install
make
make install
```

Without `--prefix`, the library will be installed to `/usr/local`.

### Python

#### Prerequisites

- Python (3.9 or newer)

#### Via pip

```bash
python -m pip install mib2h5
```

#### Via conda

```bash
conda install -c conda-forge mib2h5
```

## Usage

### Command-line Interface

#### Basic Usage

Convert a single MIB file to HDF5:

```bash
mib2h5 input.mib
```

This will create `input.h5` in the current directory.

#### Multiple Files

Convert multiple MIB files and specify an output directory:

```bash
mib2h5 -o /path/to/output file1.mib file2.mib file3.mib
```

This will create `file1.h5`, `file2.h5` and `file3.h5` in the directory
`/path/to/output`.

#### Advanced Options

Convert with compression, custom dataset key, and excluding metadata:

```bash
mib2h5 -c -d '/rawdata' -N -- input.mib
```

This will:

- enable Blosc compression
- store the frames at the dataset key `/rawdata` in the HDF5 file
- exclude metadata from the output (using `-N` or `--no-metadata`)

#### Using Long Options

Long options make commands more readable and self-documenting. You can find the
list of long options by `mib2h5 --help`.

#### Metadata Control

By default, metadata is included in the HDF5 output. You can control this
behavior:

```bash
# Explicitly include metadata (default behavior)
mib2h5 -M input.mib
mib2h5 --with-metadata input.mib

# Exclude metadata from output
mib2h5 -N input.mib
mib2h5 --no-metadata input.mib
```

#### Environment Variables

When compression is enabled with `-c`, you can fine-tune the Blosc compression
settings:

```bash
export MIB2H5_SHUFFLE=0            # Shuffle level (0-2, default: 2)
export MIB2H5_COMPRESSION_LEVEL=5  # Compression level (0-9, default: 9)
mib2h5 -c input.mib
```

### C API Examples

#### Basic C Example

```c
#include <mib2h5.h>
#include <stdio.h>
#include <stdbool.h>

int main() {
    // Basic conversion
    const char* input_files[] = {"input.mib"};
    int result = mib_to_h5(input_files,    // Input files
                           1,              // Number of MIB files
                           NULL,           // NULL to use current directory
                           true,           // Include metadata
                           "/data",        // Default key to save frames
                           "/metadata",    // Default key to save metadata
                           false,          // NOT to use compression
                           NULL,           // No reshaping
                           true,           // Report conversion progress
                           900             // Seconds to timeout
                           );

    // Non-zero exit code when there is an error during conversion
    if (result != 0) {
        printf("Error: %s\n", mib_to_h5_last_error());
        return 1;
    }

    return 0;
}
```

#### Advanced C Example

```c
#include <mib2h5.h>
#include <stdio.h>
#include <stdbool.h>

int main() {
    const char* multiple_files[] = {"file1.mib", "file2.mib", "file3.mib"};
    int result = mib_to_h5(multiple_files,       // Input files
                           3,                    // Number of MIB files
                           "/path/to/output",    // Output directory
                           true,                 // Include metadata
                           "/rawdata",           // Custom dataset key
                           "/meta",              // Custom Metadata key
                           true,                 // Use compression
                           "10x10",              // Reshape dimension
                           true,                 // Report progress
                           300                   // Seconds to timeout
                           );

    // Non-zero exit code when there is an error during conversion
    if (result != 0) {
        printf("Error: %s\n", mib_to_h5_last_error());
        return 1;
    }

    return 0;
}
```

### Python API Examples

#### Basic Python Example

```python
from mib2h5 import convert

# Basic conversion
try:
    convert("input.mib")
except (ValueError, RuntimeError):
    print("Conversion failed.")
else:
    print("Conversion successful!")
```

#### Advanced Python Example

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
        reshape_dims="10x10",
        report_progress=True,
        timeout_seconds=300
    )
except (ValueError, RuntimeError):
    print("Conversion failed.")
else:
    print("Conversion successful!")
```

## API Reference

### C API

```c
int mib_to_h5(
    const char** input_files,
    int num_input_files,
    const char* output_dir,
    bool include_metadata,
    const char* dataset_key,
    const char* metadata_key,
    bool use_compression,
    const char* reshape_dims,
    bool report_progress,
    unsigned int timeout_seconds
);

const char* mib_to_h5_last_error(void);
```

### Python API

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

For detailed parameter descriptions, refer to the header file `mib2h5.h` or the
Python docstrings.

## Contributing

Contribution is very welcomed. Please use the [issue
page](https://github.com/ePSIC-DLS/mib2h5/issues) to report any bug and missing
feature.

### Maintainers

- Timothy Poon (@ptim0626)

### Contributors

- Teo Ching (@teoching0705)
- Yousef Moazzam (@yousefmoazzam)
- Timothy Poon (@ptim0626)

## Licence

MIT
