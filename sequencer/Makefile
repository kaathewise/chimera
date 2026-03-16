# Project Name
TARGET = sequencer_check

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

# Sources
CPP_SOURCES = sequencer_check.cpp \
              sequencer.cpp \
              simpletouch_controls.cpp \
              t_generator.cpp \
              ../simpletouch/knobs.cpp \
              ../simpletouch/switches.cpp

# Add .cc files
CC_SOURCES = ../eurorack/marbles/resources.cc \
             ../eurorack/stmlib/dsp/units.cc

# Include paths
C_INCLUDES = -I.. \
	-I../eurorack

# Use SRAM since internal flash is too small for Marbles resources
APP_TYPE = BOOT_SRAM

# Include common build rules
include ../include.mk
