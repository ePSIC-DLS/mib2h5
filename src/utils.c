#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // for strcasecmp
#include <sys/statvfs.h>

const char *only_file_name(const char *absolute_file_path)
{
  const char *rslash = strrchr(absolute_file_path, '/');
  return (rslash != NULL) ? rslash + 1 : absolute_file_path;
}

char *create_output_filename(const char *input_path, const char *output_dir)
{
  // get base filename from path
  const char *base_name = only_file_name(input_path);
  if (!base_name) {
    return NULL;
  }

  // check if filename ends with .mib (case-insensitive)
  size_t base_len     = strlen(base_name);
  const char *dot_mib = NULL;
  if (base_len > 4) {
    // check for .mib or .MIB at the end
    if (strcasecmp(base_name + base_len - 4, ".mib") == 0) {
      dot_mib = base_name + base_len - 4;
    }
  }

  // calculate output filename length
  size_t name_len = dot_mib ? (size_t) (dot_mib - base_name) : base_len;
  // +1 for '/', +4 for '.h5\0'
  size_t output_len = strlen(output_dir) + 1 + name_len + 4;

  char *output_file = malloc(output_len);
  if (!output_file) {
    return NULL;
  }

  // build output filename
  if (dot_mib) {
    // copy basename without .mib, then append .h5
    snprintf(output_file, output_len, "%s/%.*s.h5", output_dir, (int) name_len,
             base_name);
  } else {
    // no .mib extension, just append .h5
    sprintf(output_file, "%s/%s.h5", output_dir, base_name);
  }

  return output_file;
}

unsigned int num_of_headers(FILE *mib_ptr, const unsigned int stride)
{
  unsigned int num_hdr  = 0;
  long int original_fpi = 0, file_size = 0;

  /*store the original position*/
  original_fpi = ftell(mib_ptr);

  /*move to the end*/
  if (fseek(mib_ptr, 0L, SEEK_END) != 0) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr, "%s:%d: error: failed to seek to end of the MIB file\n",
            current_file, __LINE__);
    goto restore;
  }

  /*get the current position (i.e. end position)*/
  file_size = ftell(mib_ptr);

  /*printf("File size: %ld\n", file_size);*/
  /*printf("Stride: %u\n", stride);*/

  if (stride == 0) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr, "%s:%d: error: frame stride cannot be zero\n", current_file,
            __LINE__);
    num_hdr = 0;
  } else {
    if ((file_size % stride) != 0) {
      const char *current_file = only_file_name(__FILE__);
      fprintf(stderr,
              "%s:%d: error: the stride of the frame is apparently "
              "incorrect (it cannot divide the MIB file into an integral "
              "number of frames)\n",
              current_file, __LINE__);
      goto restore;
    }
    num_hdr = file_size / stride;
  }

  /*go back to the original position*/
restore:
  if (fseek(mib_ptr, original_fpi, SEEK_SET) != 0) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(
      stderr,
      "%s:%d: error: failed to seek to the original position of the MIB file\n",
      current_file, __LINE__);
    num_hdr = 0;
  }

  return num_hdr;
}

void header_meta_from_first(FILE *mib_ptr,
                            char *header_id,
                            unsigned int *header_bytes,
                            unsigned int *num_chips,
                            unsigned int *det_x,
                            unsigned int *det_y,
                            char *pixel_depth)
{
  char header_buf[40];
  long int original_fpi = 0;

  /*store the original position*/
  original_fpi = ftell(mib_ptr);

  /*move to the beginning*/
  if (fseek(mib_ptr, 0L, SEEK_SET) != 0) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr,
            "%s:%d: error: failed to seek to beginning of the MIB file\n",
            current_file, __LINE__);
    goto restore;
  }

  /*get enough section of the first header*/
  if (fgets(header_buf, sizeof(header_buf), mib_ptr) == NULL) {
    perror("error: failed to read data from header");
    goto restore;
  }

  /*assign value (skip sequence number)*/
  int n = sscanf(header_buf, "%3s,%*[^,],%u,%u,%u,%u,%[^,]", header_id,
                 header_bytes, num_chips, det_x, det_y, pixel_depth);

  /*fail to assign enough value */
  if (n != 6) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr, "%s:%d: error: failed to assign header meta value\n",
            current_file, __LINE__);
    goto restore;
  }
  if (n == EOF) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr, "%s:%d: error: the first header appears to be empty\n",
            current_file, __LINE__);
    goto restore;
  }

  /*go back to the original position*/
restore:
  if (fseek(mib_ptr, original_fpi, SEEK_SET) != 0) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(
      stderr,
      "%s:%d: error: failed to seek to the original position of the MIB file\n",
      current_file, __LINE__);
  }
}

hid_t bufsize_to_datatype(int dtype)
{
  hid_t datatype;
  switch (dtype) {
    case 1: {
      datatype = H5T_STD_U8LE;
      break;
    }
    case 2: {
      datatype = H5T_STD_U16LE;
      break;
    }
    case 4: {
      datatype = H5T_STD_U32LE;
      break;
    }
    case 8: {
      datatype = H5T_STD_U64LE;
      break;
    }
    default: {
      fprintf(stderr, "Error in datatype, please check input dtype\n");
      return H5I_INVALID_HID;
    }
  }
  return datatype;
}

unsigned long get_filesystem_block_size(const char *path)
{
  struct statvfs stat;
  if (!path) {
    fprintf(stderr, "Null pointer in get_filesystem_block_size\n");
    return 1;
  }

  if (statvfs(path, &stat) != 0) {
    fprintf(stderr, "statvfs failed\n");
    return 1;
  }

  return stat.f_bsize;
}
