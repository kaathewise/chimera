# Project Name
TARGET = voice_check

# Library Locations
LIBDAISY_DIR = ../third_party/libDaisy
DAISYSP_DIR = ../third_party/DaisySP

# Sources
CPP_SOURCES = voice_check.cpp \
              voice.cpp \
              simpletouch_controls.cpp \
              ../simpletouch/knobs.cpp \
              ../simpletouch/switches.cpp

# Add .cc files
CC_SOURCES = ../third_party/eurorack/plaits/dsp/engine/particle_engine.cc \
             ../third_party/eurorack/stmlib/dsp/units.cc \
             ../third_party/eurorack/stmlib/utils/random.cc

# Include paths
C_INCLUDES = -I.. \
	-I../third_party \
	-I../third_party/eurorack

# Use SRAM if needed
APP_TYPE = BOOT_SRAM

# Include common build rules
include ../include.mk
