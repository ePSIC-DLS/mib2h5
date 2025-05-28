# mib_wrapper.pyx

cdef extern from "mib_to_h5.h":
    int mib_to_h5(char *filename,
                  char *output_directory,
                  char *merlin_dset_name,
                  char *compressor,
                  unsigned int shuffle,
                  unsigned int compression_level)

def py_mib_to_h5(str filename,
                 str output_directory,
                 str merlin_dset_name,
                 str compressor,
                 int shuffle,
                 int compression_level):
    return mib_to_h5(
            filename.encode('utf-8'),
            output_directory.encode('utf-8'),
            merlin_dset_name.encode('utf-8'),
            compressor.encode('utf-8'),
            shuffle,
            compression_level
    )
