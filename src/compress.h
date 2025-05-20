// clang-format Language: C
#ifndef COMPRESS_H
#define COMPRESS_H

hid_t dcpl_compress(size_t dim,
                    hsize_t *frame_dim,
                    unsigned int compression_level,
                    unsigned int shuffle,
                    char *compressor);

int compress_frame(framebuffer *fb,
                   unsigned int compression_level,
                   unsigned int shuffle,
                   char *compressor,
                   size_t blocksize,
                   int numinternalthreads);

#endif
