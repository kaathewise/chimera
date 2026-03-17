# Project Name
TARGET = chimera

# Library Locations
LIBDAISY_DIR = ../third_party/libDaisy
DAISYSP_DIR = ../third_party/DaisySP

# Sources
CPP_SOURCES = chimera.cpp \
              patch.cpp \
			  ../sequencer/sequencer.cpp \
			  ../sequencer/simpletouch_controls.cpp \
			  ../sequencer/t_generator.cpp \
              ../voice/voice.cpp \
              ../voice/simpletouch_controls.cpp \
              ../simpletouch/knobs.cpp \
              ../simpletouch/switches.cpp

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

# Include common build rules
include ../include.mk
