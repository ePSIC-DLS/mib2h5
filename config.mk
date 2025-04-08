# === Directories ===
SRCDIR := src
OBJDIR := obj
BINDIR ?= bin
INCDIR := include
HDFDIR ?= $(HDF5_ROOT)

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

CFLAGS += -I$(SRCDIR) -I$(INCDIR) -I$(HDFDIR)/include
LDFLAGS += -L$(HDFDIR)/lib -lhdf5

# === Sources ===

SOURCES := $(wildcard $(SRCDIR)/*.c)

OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

TARGET := $(BINDIR)/mib2h5
