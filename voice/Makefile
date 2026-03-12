# Project Name
TARGET = voice_check

# Sources
CPP_SOURCES = voice_check.cpp \
              voice.cpp \
              controls.cpp \
              ../simpletouch/knobs.cpp \
              ../simpletouch/switches.cpp

# Add .cc files as .cpp to trick libDaisy core into adding them to OBJECTS
CC_SOURCES = ../eurorack/plaits/dsp/engine/particle_engine.cc \
             ../eurorack/stmlib/dsp/units.cc \
             ../eurorack/stmlib/utils/random.cc

CPP_SOURCES += $(CC_SOURCES:.cc=.cpp)

# Include paths
C_INCLUDES = -I. \
	-I.. \
	-I../eurorack

# For CLion
CFLAGS = C_INCLUDES

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

# Use SRAM if needed
APP_TYPE = BOOT_SRAM

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

# Add rule for .cc files
$(BUILD_DIR)/%.o: %.cc Makefile | $(BUILD_DIR)
	$(CXX) -c $(CPPFLAGS) $(CPP_STANDARD) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cc=.lst)) $< -o $@

# Set vpath for .cc files
vpath %.cc $(sort $(dir $(CC_SOURCES)))
