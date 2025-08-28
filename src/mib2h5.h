// clang-format Language: C
/**
 * @file mib2h5.h
 * @brief Public API for mib2h5 library - MerlinEM MIB to HDF5 converter
 * @version 0.0.1
 *
 * This header defines the public API for converting MerlinEM detector
 * output files (.mib) from Quantum Detector to HDF5 format (.h5).
 *
 * The library preserves all metadata and supports frame-by-frame processing
 * for handling large datasets efficiently.
 */
#ifndef MIB2H5_H
#define MIB2H5_H

#include <stdbool.h>

#define MIB2H5_VERSION_MAJOR 0
#define MIB2H5_VERSION_MINOR 0
#define MIB2H5_VERSION_PATCH 1
#define MIB2H5_VERSION "0.0.1"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert MIB files to HDF5 files
 *
 * Converts MerlinEM .mib files to HDF5 files, saves all the frames and
 * optionally keeps their metadata. Each input .mib file produces a separate
 * HDF5 output file with the same base name.
 *
 * @param[in] input_files      Array of paths to input .mib files
 * @param[in] num_input_files  Number of files in input_files (must be >0)
 * @param[in] output_dir       Output directory (NULL uses current directory)
 * @param[in] include_metadata If true, includes metadata of frames in HDF5
 * @param[in] dataset_key      HDF5 dataset path for frames (NULL defaults to
 * "/data")
 * @param[in] metadata_key     HDF5 group path for metadata (NULL defaults to
 * "/metadata")
 * @param[in] use_compression  Enable Blosc compression if true
 * @param[in] reshape_dims     Reshape string like "10x10" (NOT YET IMPLEMENTED)
 * @param[in] report_progress  Enable progress reporting (NOT YET IMPLEMENTED)
 * @param[in] timeout_seconds  Conversion timeout in seconds, 0 for no limit
 * (NOT YET IMPLEMENTED)
 *
 * @return 0 on success, -1 on error
 *
 * @note Output files are named by replacing .mib extension with .h5
 * @note If multiple files are provided, conversion continues even if one fails
 * @note Some Blosc compression setting can be controlled by
 *       environment variables:
 *       - MIB2H5_SHUFFLE (default: 2, range: 0-2)
 *       - MIB2H5_COMPRESSION_LEVEL (default: 9, range: 0-9)
 *
 * @code
 * // Example: Convert multiple files with metadata
 * const char* files[] = {"data1.mib", "data2.mib"};
 * int result = mib_to_h5(files, 2, "output/", true,
 *                        NULL, NULL, false, NULL, false, 0);
 * if (result != 0) {
 *     fprintf(stderr, "Conversion failed: %s\n", mib_to_h5_last_error());
 * }
 * @endcode
 */
int mib_to_h5(const char **input_files,
              int num_input_files,
              const char *output_dir,
              bool include_metadata,
              const char *dataset_key,
              const char *metadata_key,
              bool use_compression,
              const char *reshape_dims,
              bool report_progress,
              unsigned int timeout_seconds);

/**
 * @brief Get the last error message from mib_to_h5 operations
 * @warning This function is not yet implemented and always returns a
 * placeholder
 * @todo Implement proper thread-safe error message handling
 */
const char *mib_to_h5_last_error(void);

#ifdef __cplusplus
}
#endif

#endif // MIB2H5_H
