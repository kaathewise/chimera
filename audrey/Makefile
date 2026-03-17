# Project Name
TARGET = audrey

# Library Locations
LIBDAISY_DIR = ../third_party/libDaisy
DAISYSP_DIR = ../third_party/DaisySP

# Sources
CPP_SOURCES = audrey.cpp \
              biquad_filters.cpp \
              dc_block.cpp \
              feedback_synth_controls.cpp \
              feedback_synth_engine.cpp \
              karplus_string.cpp \
              ../simpletouch/knobs.cpp \
              ../simpletouch/switches.cpp \
              ../simpletouch/memory/sdram_alloc.cpp

# Include paths
C_INCLUDES = -I.. \
	-I../third_party \
	-I../third_party/eurorack

# Use SRAM since Marbles resources are large
APP_TYPE = BOOT_SRAM

# Use DaisySP LGPL for Tone and ReverbSc
USE_DAISYSP_LGPL = 1

# Include common build rules
include ../include.mk
