#include "config.h"
#include "mib_to_h5.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int opt;
  char *input_mib        = NULL;
  char *output_directory = "./";
  char *merlin_dset_name = "MerlinData";
  char *compressor       = "blosclz";
  int compression_level  = 9;
  int shuffle            = 2;

  while ((opt = getopt(argc, argv, "i:o:d:c:s:l:")) != -1) {
    switch (opt) {
      case 'i':
        input_mib = optarg;
        break;
      case 'o':
        output_directory = optarg;
        break;
      case 'd':
        merlin_dset_name = optarg;
        break;
      case 'c':
        compressor = optarg;
        break;
      case 's':
        shuffle = atoi(optarg);
        break;
      case 'l':
        compression_level = atoi(optarg);
        break;
      default:
        fprintf(
          stderr,
          "Usage: -i input_mib -o output_directory -d merlin_dataset_name -c "
          "compressor -s shuffle -l compression_level\n");
        return 1;
    }
  }

  if (input_mib == NULL) {
    fprintf(stderr, "Error: input file must be specified\n");
    return 1;
  }

  int status = mib_to_h5(input_mib, output_directory, merlin_dset_name,
                         compressor, shuffle, compression_level);

  return status;
}
