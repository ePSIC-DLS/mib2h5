'''
  Options for blosc compression
  blosclz
  lz4
  lz4hc
  snappy
  zlib
  zstd

  py_mib_to_h5(str mib_file,
               str output_dir,
               str dset_name,
               str compressor,
               int shuffle,
               int compression_level)

  str mib_file:
    path to the mib file that is about to be converted

  str output_dir:
    path to put the resulting h5 file

  str dset_name:
    name for the main binary dataset

  str compressor:
    str to put the compressor, only the option above is available

  int shuffle:
    indicate enable shuffle or not, 0: no shuffle, 1: byte-shuffle, 2: bit-shuffle

  int compression_level:
    indicate the compression_level, 0-9, 0 being lowest(no compression)
'''

import mib2h5_wrapper

mib_file = "/path/to/mib"
output_dir = "/output/directory"
dset_name = "MerlinData"

result = mib2h5_wrapper.py_mib_to_h5(mib_file, output_dir, dset_name, "blosclz", 2, 9)

print("Return code:", result)
