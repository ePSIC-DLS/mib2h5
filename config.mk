# === Directories ===
SRCDIR := src
OBJDIR := obj
BINDIR := bin
INCDIR := include
HDFDIR := $(HDF5_ROOT)

# === Compiler and flags ===
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -g -O0 -fanalyzer -I$(SRCDIR) -I$(INCDIR) -I$(HDFDIR)/include
LDFLAGS := -L$(HDFDIR)/lib -lhdf5

# === Sources ===

SOURCES := $(wildcard $(SRCDIR)/*.c)

OBJECTS := \
    $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES)) \

TARGET := $(BINDIR)/main.out
