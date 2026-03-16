# Project Name
TARGET = voice_check

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

# Sources
CPP_SOURCES = voice_check.cpp \
              voice.cpp \
              simpletouch_controls.cpp \
              ../simpletouch/knobs.cpp \
              ../simpletouch/switches.cpp

# Add .cc files
CC_SOURCES = ../eurorack/plaits/dsp/engine/particle_engine.cc \
             ../eurorack/stmlib/dsp/units.cc \
             ../eurorack/stmlib/utils/random.cc

# Include paths
C_INCLUDES = -I.. \
	-I../eurorack

# Use SRAM if needed
APP_TYPE = BOOT_SRAM

# Include common build rules
include ../include.mk
