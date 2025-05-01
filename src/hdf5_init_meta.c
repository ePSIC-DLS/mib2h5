#include "hdf5_init_meta.h"
#include "macros.h"

#include <hdf5.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void create_meta_mq1_fields_dataset(hid_t file, hid_t lcpl, hid_t *meta_handle)
{
  hid_t pixel_depth_type         = H5I_INVALID_HID;
  hid_t sensor_layout_type       = H5I_INVALID_HID;
  hid_t chip_select_type         = H5I_INVALID_HID;
  hid_t timestamp_type           = H5I_INVALID_HID;
  hid_t header_extension_id_type = H5I_INVALID_HID;
  hid_t extended_timestamp_type  = H5I_INVALID_HID;
  hsize_t threshold_dims[1]      = {MQ1_FLOAT_LEN_THRESHOLD};
  hid_t threshold_type           = H5I_INVALID_HID;
  hid_t dataspace                = H5I_INVALID_HID;
  hid_t dcpl                     = H5I_INVALID_HID;
  hid_t dapl                     = H5I_INVALID_HID;
  hid_t meta_group               = H5I_INVALID_HID;
  if (meta_handle == NULL) {
    fprintf(stderr, "meta_handle is NULL in create_meta_fields_dataset\n");
    return;
  }

  char meta_group_path[64] = "metadata";
  meta_group = H5Gcreate(file, meta_group_path, lcpl, H5P_DEFAULT, H5P_DEFAULT);
  if (meta_group < 0) {
    fprintf(stderr, "Error creating group in create_meta_mq1_fields_dataset\n");
    goto cleanup;
  }

  hsize_t dim[1]       = {0};
  hsize_t max_dim[1]   = {H5S_UNLIMITED};
  hsize_t chunk_dim[1] = {1};

  dataspace = H5Screate_simple(1, dim, max_dim);
  if (dataspace < 0) {
    fprintf(stderr,
            "Error creating dataspace in create_meta_mq1_fields_dataset\n");
    goto cleanup;
  }

  dcpl = H5Pcreate(H5P_DATASET_CREATE);
  if (dcpl < 0) {
    fprintf(stderr, "Error creating dcpl in create_meta_mq1_fields_dataset\n");
    goto cleanup;
  } else {
    if (H5Pset_chunk(dcpl, 1, chunk_dim) < 0) {
      fprintf(stderr,
              "Error setting chunking in create_meta_mq1_fields_dataset\n");
      goto cleanup;
    }
  }

  dapl = H5Pcreate(H5P_DATASET_ACCESS);
  if (dapl < 0) {
    fprintf(stderr,
            "Error in creating dapl in create_meta_mq1_fields_dataset\n");
    goto cleanup;
  } else {
  }

  // hid_t header_id_type = H5Tcopy(H5T_C_S1);
  // H5Tset_size(header_id_type, MQ1_CHAR_LEN_HEADER_ID);

  pixel_depth_type = H5Tcopy(H5T_C_S1);
  if (pixel_depth_type < 0) {
    fprintf(stderr, "H5Tcopy failed for pixel_depth_type\n");
    goto cleanup;
  }
  if (H5Tset_size(pixel_depth_type, MQ1_CHAR_LEN_PIXEL_DEPTH) < 0) {
    fprintf(stderr, "H5Tsetsize failed for pixel_depth_type\n");
    goto cleanup;
  }

  sensor_layout_type = H5Tcopy(H5T_C_S1);
  if (sensor_layout_type < 0) {
    fprintf(stderr, "H5Tcopy failed for sensor_layout_type\n");
    goto cleanup;
  }
  if (H5Tset_size(sensor_layout_type, MQ1_CHAR_LEN_SENSOR_LAYOUT) < 0) {
    fprintf(stderr, "H5Tset_size failed for sensor_layout_type\n");
    goto cleanup;
  }

  chip_select_type = H5Tcopy(H5T_C_S1);
  if (chip_select_type < 0) {
    fprintf(stderr, "H5Tcopy failed for chip_select_type\n");
    goto cleanup;
  }
  if (H5Tset_size(chip_select_type, MQ1_CHAR_LEN_CHIP_SELECT) < 0) {
    fprintf(stderr, "H5Tset_size failed for chip_select_type\n");
    goto cleanup;
  }

  timestamp_type = H5Tcopy(H5T_C_S1);
  if (timestamp_type < 0) {
    fprintf(stderr, "H5Tcopy failed for timestamp_type\n");
    goto cleanup;
  }
  if (H5Tset_size(timestamp_type, MQ1_CHAR_LEN_TIMESTAMP) < 0) {
    fprintf(stderr, "H5Tset_size failed for timestamp_type\n");
    goto cleanup;
  }

  header_extension_id_type = H5Tcopy(H5T_C_S1);
  if (header_extension_id_type < 0) {
    fprintf(stderr, "H5Tcopy failed for header_extension_id_type\n");
    goto cleanup;
  }
  if (H5Tset_size(header_extension_id_type, MQ1_CHAR_LEN_HEADER_EXTENSION_ID) <
      0) {
    fprintf(stderr, "H5Tset_size failed for header_extension_id_type\n");
    goto cleanup;
  }

  extended_timestamp_type = H5Tcopy(H5T_C_S1);
  if (extended_timestamp_type < 0) {
    fprintf(stderr, "H5Tcopy failed for extended_timestamp_type\n");
    goto cleanup;
  }
  if (H5Tset_size(extended_timestamp_type, MQ1_CHAR_LEN_EXTENDED_TIMESTAMP) <
      0) {
    fprintf(stderr, "H5Tset_size failed for extended_timestamp_type\n");
    goto cleanup;
  }

  threshold_type = H5Tarray_create(H5T_NATIVE_FLOAT, 1, threshold_dims);
  if (threshold_type < 0) {
    fprintf(stderr, "H5Tarray_create failed for thresholds\n");
    goto cleanup;
  }

  struct {
    const char *name;
    hid_t type;
  } fields[] = {
    //{"header_id", header_id_type},
    {"max_length", H5T_NATIVE_UINT},
    {"sequence_number", H5T_NATIVE_UINT},
    {"header_bytes", H5T_NATIVE_UINT},
    {"num_chips", H5T_NATIVE_UINT},
    {"det_x", H5T_NATIVE_UINT},
    {"det_y", H5T_NATIVE_UINT},
    {"pixel_depth", pixel_depth_type},
    {"sensor_layout", sensor_layout_type},
    {"chip_select", chip_select_type},
    {"timestamp", timestamp_type},
    {"exposure_time_s", H5T_NATIVE_DOUBLE},
    {"counter", H5T_NATIVE_UINT},
    {"colour_mode", H5T_NATIVE_UINT},
    {"gain_mode", H5T_NATIVE_UINT},
    {"threshold", threshold_type},
    {"header_extension_id", header_extension_id_type},
    {"extended_timestamp", extended_timestamp_type},
    {"exposure_time_ns", H5T_NATIVE_UINT},
    {"bit_depth", H5T_NATIVE_UINT},
  };

  size_t num_datasets = sizeof(fields) / sizeof(fields[0]);

  if (num_datasets != MQ1_FIELDS_NUM_FIELDS) {
    fprintf(stderr,
            "Number of dataset not match in create_meta_mq1_fields_dataset\n");
    goto cleanup;
  }

  for (size_t i = 0; i < num_datasets; i++) {
    char dataset_path[256];

    snprintf(dataset_path, sizeof(dataset_path), "%s", fields[i].name);

    hid_t dataset = H5Dcreate2(meta_group, dataset_path, fields[i].type,
                               dataspace, lcpl, dcpl, dapl);
    if (dataset < 0) {
      fprintf(stderr, "Error creating dataset : %s\n", dataset_path);
      meta_handle[i] = -1;
      continue;
    }

    meta_handle[i] = dataset;
  }

cleanup:
  // H5Tclose(header_id_type);
  if (H5Iis_valid(pixel_depth_type))
    H5Tclose(threshold_type);
  if (H5Iis_valid(extended_timestamp_type))
    H5Tclose(extended_timestamp_type);
  if (H5Iis_valid(header_extension_id_type))
    H5Tclose(header_extension_id_type);
  if (H5Iis_valid(timestamp_type))
    H5Tclose(timestamp_type);
  if (H5Iis_valid(chip_select_type))
    H5Tclose(chip_select_type);
  if (H5Iis_valid(sensor_layout_type))
    H5Tclose(sensor_layout_type);
  if (H5Iis_valid(pixel_depth_type))
    H5Tclose(pixel_depth_type);
  if (H5Iis_valid(dapl))
    H5Pclose(dapl);
  if (H5Iis_valid(dcpl))
    H5Pclose(dcpl);
  if (H5Iis_valid(dataspace))
    H5Sclose(dataspace);
  if (H5Iis_valid(meta_group))
    H5Gclose(meta_group);
}

void create_dac_dataset(unsigned int num_chips,
                        hid_t file,
                        hid_t lcpl,
                        hid_t *dac_handle)
{
  hid_t dcpl       = H5I_INVALID_HID;
  hid_t dapl       = H5I_INVALID_HID;
  hid_t chip_group = H5I_INVALID_HID;
  hid_t dataspace  = H5I_INVALID_HID;
  hid_t dataset    = H5I_INVALID_HID;
  hid_t str_type   = H5I_INVALID_HID;

  if (dac_handle == NULL) {
    fprintf(stderr, "dac_handle is NULL in create_dac_meta_dataset\n");
    return;
  }

  char chip_group_path[256];
  char dataset_path[512];
  hsize_t dim[1]       = {0};
  hsize_t max_dim[1]   = {H5S_UNLIMITED};
  hsize_t chunk_dim[1] = {1};

  dcpl = H5Pcreate(H5P_DATASET_CREATE);
  if (dcpl < 0) {
    fprintf(stderr, "Error creating dcpl in create_dac_dataset\n");
    return;
  } else {
    if (H5Pset_chunk(dcpl, 1, chunk_dim) < 0) {
      fprintf(stderr, "Error setting chunking in create_dac_dataset\n");
      goto cleanup;
    }
  }

  dapl = H5Pcreate(H5P_DATASET_ACCESS);
  if (dapl < 0) {
    fprintf(stderr, "Error creating dapl in create_dac_dataset\n");
    H5Pclose(dcpl);
    return;
  }

  str_type = H5Tcopy(H5T_C_S1);
  if (H5Tset_size(str_type, 4) < 0) {
    fprintf(stderr, "Error setting string type size in create_dac_dataset\n");
    goto cleanup;
  }

  struct {
    const char *name;
    hid_t type;
  } datasets[] = {
    {"dac_format", str_type},         {"threshold0", H5T_NATIVE_UINT},
    {"threshold1", H5T_NATIVE_UINT},  {"threshold2", H5T_NATIVE_UINT},
    {"threshold3", H5T_NATIVE_UINT},  {"threshold4", H5T_NATIVE_UINT},
    {"threshold5", H5T_NATIVE_UINT},  {"threshold6", H5T_NATIVE_UINT},
    {"threshold7", H5T_NATIVE_UINT},  {"preamp", H5T_NATIVE_UINT},
    {"ikrum", H5T_NATIVE_UINT},       {"shaper", H5T_NATIVE_UINT},
    {"disc", H5T_NATIVE_UINT},        {"disc_LS", H5T_NATIVE_UINT},
    {"shaper_test", H5T_NATIVE_UINT}, {"dac_disc_L", H5T_NATIVE_UINT},
    {"dac_test", H5T_NATIVE_UINT},    {"dac_disc_H", H5T_NATIVE_UINT},
    {"delay", H5T_NATIVE_UINT},       {"TP_buff_in", H5T_NATIVE_UINT},
    {"TP_buff_out", H5T_NATIVE_UINT}, {"RPZ", H5T_NATIVE_UINT},
    {"GND", H5T_NATIVE_UINT},         {"TP_ref", H5T_NATIVE_UINT},
    {"FBK", H5T_NATIVE_UINT},         {"Cas", H5T_NATIVE_UINT},
    {"TP_ref_A", H5T_NATIVE_UINT},    {"TP_ref_B", H5T_NATIVE_UINT}};

  size_t num_datasets = sizeof(datasets) / sizeof(datasets[0]);
  int handle_pos      = 0;

  for (unsigned int i = 0; i < num_chips; i++) {
    snprintf(chip_group_path, sizeof(chip_group_path), "metadata/Chip%02d", i);

    chip_group =
      H5Gcreate(file, chip_group_path, lcpl, H5P_DEFAULT, H5P_DEFAULT);
    if (chip_group < 0) {
      fprintf(stderr, "Error creating group chip%02d in create_dac_dataset\n",
              i);
      goto cleanup;
    }

    dataspace = H5Screate_simple(1, dim, max_dim);
    if (dataspace < 0) {
      fprintf(stderr, "Error creating dataspace in create_dac_meta_dataset\n");
      goto cleanup;
    }

    handle_pos = num_datasets * (size_t) i;

    for (size_t j = handle_pos; j < (handle_pos + num_datasets); j++) {

      snprintf(dataset_path, sizeof(dataset_path), "%s",
               datasets[j - handle_pos].name);

      dataset =
        H5Dcreate2(chip_group, dataset_path, datasets[j - handle_pos].type,
                   dataspace, lcpl, dcpl, dapl);
      if (dataset < 0) {
        fprintf(stderr, "Error creating dataset: %s\n", dataset_path);
        dac_handle[j] = H5I_INVALID_HID;
        continue;
      }
      dac_handle[j] = dataset;
    }
    H5Sclose(dataspace);
    H5Gclose(chip_group);
  }

cleanup:
  if (H5Iis_valid(str_type))
    H5Tclose(str_type);
  if (H5Iis_valid(dcpl))
    H5Pclose(dcpl);
  if (H5Iis_valid(dapl))
    H5Pclose(dapl);
  if (H5Iis_valid(chip_group))
    H5Gclose(chip_group);
}

void close_dataset_handle(hid_t *handle, size_t count)
{
  if (!handle) {
    printf("No handle\n");
    return;
  }
  for (size_t i = 0; i < count; i++) {
    if (H5Iis_valid(handle[i])) {
      H5Dclose(handle[i]);
    }
  }
}
