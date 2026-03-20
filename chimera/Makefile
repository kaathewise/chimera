# Project Name
TARGET = chimera

# Library Locations
LIBDAISY_DIR = ../third_party/libDaisy
DAISYSP_DIR = ../third_party/DaisySP

# Sources
CPP_SOURCES = chimera.cpp \
              patch.cpp \
              ../audrey/biquad_filters.cpp \
			  ../audrey/dc_block.cpp \
			  ../audrey/simpletouch_controls.cpp \
			  ../audrey/engine.cpp \
			  ../audrey/karplus_string.cpp \
			  ../sequencer/sequencer.cpp \
			  ../sequencer/simpletouch_controls.cpp \
			  ../sequencer/t_generator.cpp \
              ../voice/voice.cpp \
              ../voice/simpletouch_controls.cpp \
              ../simpletouch/knobs.cpp \
              ../simpletouch/switches.cpp \
              ../simpletouch/memory/sdram_alloc.cpp

# Add .cc files
CC_SOURCES = ../third_party/eurorack/marbles/resources.cc \
             ../third_party/eurorack/plaits/dsp/engine/particle_engine.cc \
             ../third_party/eurorack/stmlib/dsp/units.cc \
             ../third_party/eurorack/stmlib/utils/random.cc

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
