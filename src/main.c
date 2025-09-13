#include "config.h"
#include "mib2h5.h"
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// long options structure for getopt_long
static const struct option long_options[] = {
  {"help", no_argument, NULL, 'h'},
  {"version", no_argument, NULL, 'v'},
  {"no-metadata", no_argument, NULL, 'N'},
  {"with-metadata", no_argument, NULL, 'M'},
  {"output-dir", required_argument, NULL, 'o'},
  {"output-directory", required_argument, NULL, 'o'}, // alias for output-dir
  {"dataset-key", required_argument, NULL, 'd'},
  {"metadata-key", required_argument, NULL, 'k'},
  {"compress", no_argument, NULL, 'c'},
  {"reshape-to", required_argument, NULL, 'r'},
  {"timeout", required_argument, NULL, 't'},
  {NULL, 0, NULL, 0}};

static void print_usage(const char *program_name)
{
  // clang-format off
  fprintf(stderr, "Usage: %s [options] file1.mib [file2.mib ...]\n", program_name);
  fprintf(stderr, "\nOptions:\n");
  fprintf(stderr, "  -o, --output-dir DIR          Output directory (default: current)\n");
  fprintf(stderr, "      --output-directory DIR    Alternative for --output-dir\n");
  fprintf(stderr, "  -d, --dataset-key KEY         Dataset key in HDF5 (default: /data)\n");
  fprintf(stderr, "  -k, --metadata-key KEY        Metadata key in HDF5 (default: /metadata)\n");
  fprintf(stderr, "                                - NOT YET IMPLEMENTED\n");
  fprintf(stderr, "  -c, --compress                Enable compression\n");
  fprintf(stderr, "  -M, --with-metadata           Include metadata in output (default)\n");
  fprintf(stderr, "  -N, --no-metadata             Exclude metadata from output\n");
  fprintf(stderr, "  -r, --reshape-to DIMS         Reshape dimensions (e.g., '10x10')\n");
  fprintf(stderr, "                                - NOT YET IMPLEMENTED\n");
  fprintf(stderr, "  -t, --timeout SECS            Timeout in seconds\n");
  fprintf(stderr, "                                - NOT YET IMPLEMENTED\n");
  fprintf(stderr, "  -v, --version                 Display the version\n");
  fprintf(stderr, "  -h, --help                    Show this help message\n");
  fprintf(stderr, "\nEnvironment variables:\n");
  fprintf(stderr, "  MIB2H5_SHUFFLE                Shuffle level for Blosc compression (default: 2)\n");
  fprintf(stderr, "  MIB2H5_COMPRESSION_LEVEL      Blosc compression level (default: 9)\n");
  // clang-format on
}

int main(int argc, char *argv[])
{
  int opt;
  char *output_directory       = NULL;
  char *dataset_key            = NULL;
  char *metadata_key           = NULL;
  char *reshape_dims           = NULL;
  bool use_compression         = false;
  bool include_metadata        = true;
  bool report_progress         = true;
  unsigned int timeout_seconds = 900;

  // parse options
  int option_index = 0;
  while ((opt = getopt_long(argc, argv, "o:d:k:r:t:cMNvh", long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'o':
        output_directory = optarg;
        break;
      case 'd':
        dataset_key = optarg;
        break;
      case 'k':
        metadata_key = optarg;
        break;
      case 'r':
        reshape_dims = optarg;
        break;
      case 't': {
        char *endptr;
        long val = strtol(optarg, &endptr, 10);
        if (*endptr != '\0' || val < 0 || val > UINT_MAX) {
          fprintf(stderr, "Error: Invalid timeout value: %s\n", optarg);
          return 1;
        }
        timeout_seconds = (unsigned int) val;
      } break;
      case 'c':
        use_compression = true;
        break;
      case 'M':
        include_metadata = true;
        break;
      case 'N':
        include_metadata = false;
        break;
      case 'v':
        printf("%d.%d.%d\n", MIB2H5_VERSION_MAJOR, MIB2H5_VERSION_MINOR,
               MIB2H5_VERSION_PATCH);
        return 0;
      case 'h':
        print_usage(argv[0]);
        return 0;
      default:
        print_usage(argv[0]);
        return 1;
    }
  }

  // collect input files from remaining arguments
  int num_input_files = argc - optind;
  if (num_input_files <= 0) {
    fprintf(stderr, "Error: No input files specified\n\n");
    print_usage(argv[0]);
    return 1;
  }

  const char **input_files =
    (const char **) malloc((size_t) num_input_files * sizeof(char *));
  if (input_files == NULL) {
    fprintf(stderr, "Error: Cannot allocate memory for input files\n");
    return 1;
  }

  for (int i = 0; i < num_input_files; ++i) {
    input_files[i] = argv[optind + i];
  }

  int status =
    mib_to_h5(input_files, num_input_files, output_directory, include_metadata,
              dataset_key, metadata_key, use_compression, reshape_dims,
              report_progress, timeout_seconds);

  free(input_files);

  if (status != 0) {
    const char *error_msg = mib_to_h5_last_error();
    if (error_msg != NULL) {
      fprintf(stderr, "Error: %s\n", error_msg);
    } else {
      fprintf(stderr, "Error: Unknown conversion error (status: %d)\n", status);
    }
  }

  return status;
}
