# === Directories ===
SRCDIR   := src
OBJDIR   := obj
INCDIR   := include
HDFDIR   ?= $(HDF5_ROOT)
HBDIR    := /home/wck38436/Desktop/hdf5-blosc
BLOSCDIR := /home/wck38436/.blosc

BUILD ?= debug

ifndef HDFDIR
$(error HDFDIR is not defined. Please set HDF5_ROOT or pass HDFDIR=...)
endif

# === Compiler and flags ===
CC := gcc

ifeq ($(BUILD),debug)
	CFLAGS := -Wall -Wextra -std=c11 -g -O0 -fanalyzer
else ifeq ($(BUILD),release)
	CFLAGS := -Wall -Wextra -std=c11 -O2
else ifeq ($(BUILD),asan)
	CFLAGS := -Wall -Wextra -std=c11 -g -O0 -fsanitize=address
	LDFLAGS += -fsanitize=address
else
	$(error Unknown BUILD type: $(BUILD))
endif

CFLAGS += -I$(SRCDIR) -I$(INCDIR) -I$(HDFDIR)/include -I$(BLOSCDIR)/include -I$(HBDIR)/src
LDFLAGS += -L$(HDFDIR)/lib -L$(BLOSCDIR)/lib64 -L$(HBDIR)/build -Wl,-rpath=/home/wck38436/.blosc/lib64 -Wl,-rpath=/home/wck38436/Desktop/hdf5-blosc/build -lhdf5 -lblosc -lblosc_filter

# === Sources ===

SOURCES := $(wildcard $(SRCDIR)/*.c)

OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

TARGET := mib2h5
