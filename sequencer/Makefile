# Project Name
TARGET = sequencer_check

# Sources
CPP_SOURCES = sequencer_check.cpp \
              sequencer.cpp \
              controls.cpp \
              t_generator.cpp \
              ../simpletouch/knobs.cpp \
              ../simpletouch/switches.cpp

# Add .cc files as .cpp to trick libDaisy core into adding them to OBJECTS
CC_SOURCES = ../eurorack/marbles/ramp/ramp_extractor.cc \
             ../eurorack/marbles/resources.cc \
             ../eurorack/stmlib/dsp/units.cc

CPP_SOURCES += $(CC_SOURCES:.cc=.cpp)

# Include paths
C_INCLUDES = -I.. \
	-I../eurorack

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

# Use QSPI flash since internal flash is too small for Marbles resources
APP_TYPE = BOOT_QSPI

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

# Add rule for .cc files
$(BUILD_DIR)/%.o: %.cc Makefile | $(BUILD_DIR)
	$(CXX) -c $(CPPFLAGS) $(CPP_STANDARD) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cc=.lst)) $< -o $@

# Set vpath for .cc files
vpath %.cc $(sort $(dir $(CC_SOURCES)))
