# Project Name
TARGET = chimera

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

# Sources
CPP_SOURCES = chimera.cpp \
              patch.cpp \
			  ../sequencer/sequencer.cpp \
			  ../sequencer/controls.cpp \
			  ../sequencer/t_generator.cpp \
              ../voice/voice.cpp \
              ../voice/controls.cpp \
              ../simpletouch/knobs.cpp \
              ../simpletouch/switches.cpp

# Add .cc files
CC_SOURCES = ../eurorack/marbles/resources.cc \
             ../eurorack/plaits/dsp/engine/particle_engine.cc \
             ../eurorack/stmlib/dsp/units.cc \
             ../eurorack/stmlib/utils/random.cc

# Include paths
C_INCLUDES = -I.. \
	-I../eurorack

# Use SRAM since Marbles resources are large
APP_TYPE = BOOT_SRAM

# Include common build rules
include ../include.mk
