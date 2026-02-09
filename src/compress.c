#include "config.h"
#include "framebuffer.h"
#include "io_header.h"
#include "macros.h"
#include "mib_header.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_COMPRESSION
#include <blosc.h>
#include <blosc_filter.h>
#endif

hid_t dcpl_compress(size_t dim,
                    hsize_t *frame_dim,
                    unsigned int compression_level,
                    unsigned int shuffle,
                    char *compressor)
{
  hid_t dcpl                = H5I_INVALID_HID;
  unsigned int cd_values[7] = {0};
  if ((dcpl = H5Pcreate(H5P_DATASET_CREATE)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating dcpl\n");
    return H5I_INVALID_HID;
  } else {
    if (H5Pset_chunk(dcpl, dim, frame_dim) < 0) {
      fprintf(stderr, "Error in H5Pset_chunk\n");
      H5Pclose(dcpl);
      return H5I_INVALID_HID;
    }
    if (H5Pset_fill_time(dcpl, H5D_FILL_TIME_NEVER) < 0) {
      fprintf(stderr, "Error in H5Pset_fill_time\n");
      H5Pclose(dcpl);
      return H5I_INVALID_HID;
    }
#ifdef HAVE_COMPRESSION
    char *version = (char *) malloc(sizeof(char) * 512);
    if (version == NULL) {
      fprintf(stderr, "malloc for version failed\n");
      H5Pclose(dcpl);
      return H5I_INVALID_HID;
    }
    char *date = (char *) malloc(sizeof(char) * 512);
    if (date == NULL) {
      fprintf(stderr, "malloc for date failed\n");
      H5Pclose(dcpl);
      free(version);
      return H5I_INVALID_HID;
    }
    cd_values[0] = 0;
    cd_values[1] = 0; // unused
    cd_values[2] = 0; // unused
    cd_values[3] = 0; // blocksize
    cd_values[4] = compression_level;
    cd_values[5] = shuffle;
    cd_values[6] = blosc_compname_to_compcode(compressor);
    if (H5Pset_filter(dcpl, FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values) <
        0) {
      fprintf(stderr, "Error in H5Pset_filter\n");
      H5Pclose(dcpl);
      return H5I_INVALID_HID;
    }
    if (register_blosc(&version, &date) < 0) {
      fprintf(stderr, "Error in register_blosc\n");
      H5Pclose(dcpl);
      free(version);
      free(date);
      return H5I_INVALID_HID;
    } else {
      printf("Blosc version info: %s (%s)\n", version, date);
    }
    free(version);
    free(date);
#endif
  }
  return dcpl;
}

int compress_frame(framebuffer *fb,
                   unsigned int compression_level,
                   unsigned int shuffle,
                   char *compressor,
                   size_t blocksize,
                   int numinternalthreads)
{
  int bufsize = (fb->mq1_header->pixel_depth[1] - '0') * 10 +
                (fb->mq1_header->pixel_depth[2] - '0');
  bufsize = bufsize / 8;

  int detx = (int) *(fb->mq1_header->det_x);
  int dety = (int) *(fb->mq1_header->det_y);

  size_t nbytes = dety * detx * bufsize;
#ifdef HAVE_COMPRESSION
  size_t destsize = nbytes + BLOSC_MAX_OVERHEAD;
  void *dest      = malloc(destsize);
  if (!dest) {
    fprintf(stderr, "Error in malloc for dest\n");
    return -1;
  }

  int cbytes = blosc_compress_ctx(compression_level, shuffle, bufsize, nbytes,
                                  fb->data, dest, destsize, compressor,
                                  blocksize, numinternalthreads);

  if (cbytes < 0) {
    fprintf(stderr, "Error in blosc_compress\n");
    free(dest);
    return -1;
  }
  if (cbytes == 0) {
    fprintf(stderr, "Blosc returned 0 bytes (uncompressible?). Forcing "
                    "fallback to uncompressed write.\n");
    cbytes = nbytes;
    free(dest);
    return cbytes;
  }

  void *temp = realloc(fb->data, destsize);
  if (temp) {
    fb->data = temp;
    memcpy(fb->data, dest, cbytes);
  } else {
    fprintf(stderr,
            "Error in realloc for fb->data, fall back on the original data\n");
    free(dest);
    return nbytes;
  }
  free(dest);
  return cbytes;
#else
  return nbytes;
#endif
}
