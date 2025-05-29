#include "append.h"
#include "framebuffer.h"
#include "io_header.h"
#include "macros.h"

#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>

void append_frame_to_dataset(hid_t dset, framebuffer *fb, int cbytes)
{
  if (!dset || !fb) {
    fprintf(stderr, "Error in passing parameters in append_frame_to_dataset\n");
    return;
  }

  unsigned int detx = *(fb->mq1_header->det_x);
  unsigned int dety = *(fb->mq1_header->det_y);
  int bufsize       = (fb->mq1_header->pixel_depth[1] - '0') * 10 +
                (fb->mq1_header->pixel_depth[2] - '0');
  bufsize = bufsize / 8;

  if (cbytes == 0) {
    cbytes = dety * detx * bufsize;
    printf("cbytes == 0\n");
  }

  hid_t filespace = H5Dget_space(dset);
  if (filespace == H5I_INVALID_HID) {
    fprintf(stderr, "Error in H5Dget_space in append_frame_to_dataset\n");
    return;
  }
  hsize_t dims[3];
  int rank = H5Sget_simple_extent_dims(filespace, dims, NULL);
  if (rank < 0) {
    fprintf(stderr,
            "Error in H5Sget_simple_extent_dims in append_frame_to_dataset\n");
    H5Sclose(filespace);
    return;
  }

  hsize_t frame_index = dims[0];

  hsize_t new_dims[3] = {frame_index + 1, dety, detx};
  if (H5Dset_extent(dset, new_dims) < 0) {
    fprintf(stderr, "Error in extending dataset in append_frame_to_dataset\n");
    return;
  }

  hsize_t offset_chunk[3] = {frame_index, 0, 0};
  uint32_t filter_mask    = 0;

  if (H5Dwrite_chunk(dset, H5P_DEFAULT, filter_mask, offset_chunk, cbytes,
                     fb->data) < 0) {
    fprintf(stderr, "Failed in writing chunk\n");
    return;
  }

  H5Sclose(filespace);
}

void append_meta_to_dataset(hid_t *meta_handle, framebuffer *fb)
{
  if (meta_handle == NULL) {
    fprintf(stderr, "meta_handle is NULL in append_meta_to_dataset\n");
    return;
  }
  if (fb == NULL) {
    fprintf(stderr, "framebuffer is NULL in append_meta_to_dataset\n");
    return;
  }
  hid_t datatype, filespace, memspace;
  info *mq1_iter = mq1_fields_info(fb->mq1_header);

  hsize_t dim[1];
  hsize_t frame_index;
  hsize_t new_dim[1];
  hsize_t start[1];
  hsize_t count[1]   = {1};
  hsize_t mem_dim[1] = {1};

  for (size_t i = 0; i < MQ1_FIELDS_NUM_FIELDS; i++) {
    datatype  = H5Dget_type(meta_handle[i]);
    filespace = H5Dget_space(meta_handle[i]);
    H5Sget_simple_extent_dims(filespace, dim, NULL);
    frame_index = dim[0];
    new_dim[0]  = dim[0] + 1;

    if (H5Dset_extent(meta_handle[i], new_dim) < 0) {
      fprintf(stderr,
              "Error in extending dataset %s in append_meta_to_dataset\n",
              mq1_iter[i].name);
      H5Tclose(datatype);
      H5Sclose(filespace);
      return;
    }
    // update filespace as the dimension is extented
    H5Sclose(filespace);
    filespace = H5Dget_space(meta_handle[i]);
    start[0]  = frame_index;
    H5Sselect_hyperslab(filespace, H5S_SELECT_SET, start, NULL, count, NULL);

    memspace = H5Screate_simple(1, mem_dim, NULL);

    if (H5Dwrite(meta_handle[i], datatype, memspace, filespace, H5P_DEFAULT,
                 mq1_iter[i].data) < 0) {
      fprintf(stderr, "Error writing %s to metadata dataset\n",
              mq1_iter[i].name);
    }

    H5Sclose(memspace);
    H5Sclose(filespace);
    H5Tclose(datatype);
  }
}

void append_dac_to_dataset(unsigned int num_chips,
                           hid_t *dac_handle,
                           framebuffer *fb)
{
  hid_t datatype, filespace, memspace;
  info *d_array[4];
  size_t ind = 0;

  if (!fb) {
    fprintf(stderr, "Null framebuffer in append_dac_to_dataset\n");
    return;
  }

  switch (num_chips) {
    case 1: {
      if (!fb->dac0) {
        fprintf(stderr, "Null dac0 pointer in append_dac_to_dataset\n");
        return;
      } else {
        d_array[0] = dac_info(fb->dac0);
        d_array[1] = NULL;
        d_array[2] = NULL;
        d_array[3] = NULL;
      }
      break;
    }
    case 4: {
      if (!fb->dac0 || !fb->dac1 || !fb->dac2 || !fb->dac3) {
        fprintf(stderr, "Null dac pointer detected in append_dac_to_dataset\n");
        return;
      } else {
        d_array[0] = dac_info(fb->dac0);
        d_array[1] = dac_info(fb->dac1);
        d_array[2] = dac_info(fb->dac2);
        d_array[3] = dac_info(fb->dac3);
      }
      break;
    }
    default: {
      fprintf(stderr, "Num_chips should be 1 or 4 in append_dac_to_dataset\n");
      return;
    }
  }

  hsize_t dim[1];
  hsize_t frame_index;
  hsize_t new_dim[1];
  hsize_t start[1];
  hsize_t count[1]   = {1};
  hsize_t mem_dim[1] = {1};

  for (size_t i = 0; i < (size_t) num_chips; i++) {
    for (size_t j = 0; j < DAC_NUM_FIELDS; j++) {
      ind       = i * DAC_NUM_FIELDS + j;
      datatype  = H5Dget_type(dac_handle[ind]);
      filespace = H5Dget_space(dac_handle[ind]);
      H5Sget_simple_extent_dims(filespace, dim, NULL);
      frame_index = dim[0];
      new_dim[0]  = dim[0] + 1;

      if (H5Dset_extent(dac_handle[ind], new_dim) < 0) {
        fprintf(stderr,
                "Error in extending dataset %s in append_dac_to_dataset\n",
                d_array[i][j].name);
        H5Tclose(datatype);
        H5Sclose(filespace);
        continue;
      }

      H5Sclose(filespace);
      filespace = H5Dget_space(dac_handle[ind]);

      start[0] = frame_index;
      H5Sselect_hyperslab(filespace, H5S_SELECT_SET, start, NULL, count, NULL);

      memspace = H5Screate_simple(1, mem_dim, NULL);

      if (H5Dwrite(dac_handle[ind], datatype, memspace, filespace, H5P_DEFAULT,
                   d_array[i][j].data) < 0) {
        fprintf(stderr, "Error writing chip%ld, %s to metadata dac\n", i,
                d_array[i][j].name);
        return;
      }
      H5Sclose(memspace);
      H5Sclose(filespace);
      H5Tclose(datatype);
    }
  }
}
