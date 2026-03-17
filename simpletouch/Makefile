# Project Name
TARGET = controls_check

# Library Locations
LIBDAISY_DIR = ../third_party/libDaisy

# Sources
CPP_SOURCES = controls_check.cpp \
              knobs.cpp \
              switches.cpp

# Include paths
C_INCLUDES = -I../ \
	-I../third_party

C_FLAGS = $(C_INCLUDES) + ../third_party/libDaisy/src

# Include common build rules
include ../include.mk
