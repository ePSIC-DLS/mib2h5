# Target binary
bin_PROGRAMS = mib2h5

# Source files
mib2h5_SOURCES = main.c mib_to_h5.c utils.c io_header.c parser.c framebuffer.c hdf5_init.c hdf5_init_meta.c read.c append.c compress.c
# Preprocessor flags
#AM_CPPFLAGS = -Isrc \
#              -I$(HDFDIR)/include \
#              -I$(BLOSCDIR)/include \
#              -I$(HBDIR)/src
# Linker flags
#AM_LDFLAGS = -L$(HDFDIR)/lib \
#             -L$(BLOSCDIR)/lib64 \
#             -L$(HBDIR)/build \
#             -Wl,-rpath,$(BLOSCDIR)/lib64 \
#             -Wl,-rpath,$(HBDIR)/build
# Libraries
mib2h5_LDADD = -lhdf5 @LIBCOMPRESSION@
