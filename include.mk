# Daisy Common Makefile
# Handles directory-preserving object mapping to avoid name clashes.

# Helps CLion to resolve includes.
CFLAGS = C_INCLUDES

# Core location, and generic Makefile from libDaisy.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

# We save the original source lists before the libDaisy core Makefile
# might modify or clear them.
ORIG_CPP_SOURCES := $(CPP_SOURCES)
ORIG_CC_SOURCES := $(CC_SOURCES)
ORIG_C_SOURCES := $(C_SOURCES)

# We clear sources for libDaisy core to avoid its default 'notdir' object mapping.
# It will still add its own startup code to C_SOURCES if we are not careful.
CPP_SOURCES = 
C_SOURCES = 
ASM_SOURCES = 

include $(SYSTEM_FILES_DIR)/Makefile

# 1. Map source files to object files in BUILD_DIR, preserving directory structure.
# e.g., ../sequencer/controls.cpp -> build/sequencer/controls.o
# e.g., chimera.cpp -> build/chimera.o

# Remove '../' prefix for object path calculation
CLEAN_CPP = $(patsubst ../%,%,$(ORIG_CPP_SOURCES))
CLEAN_CC = $(patsubst ../%,%,$(ORIG_CC_SOURCES))
CLEAN_C = $(patsubst ../%,%,$(ORIG_C_SOURCES))

# Define OBJECTS using the cleaned paths
override OBJECTS = $(addprefix $(BUILD_DIR)/,$(CLEAN_CPP:.cpp=.o))
override OBJECTS += $(addprefix $(BUILD_DIR)/,$(CLEAN_CC:.cc=.o))
override OBJECTS += $(addprefix $(BUILD_DIR)/,$(CLEAN_C:.c=.o))

# Add the libDaisy startup object (it's usually in C_SOURCES after including core Makefile)
# libDaisy/core/Makefile usually adds $(SYSTEM_FILES_DIR)/startup_stm32h750xx.c to C_SOURCES.
# We map it to build/startup_stm32h750xx.o
override OBJECTS += $(BUILD_DIR)/startup_stm32h750xx.o

# --- Rules ---

# Rule for local .cpp files
$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CPPFLAGS) $(CPP_STANDARD) $< -o $@

# Rule for .cpp files in parent directories
$(BUILD_DIR)/%.o: ../%.cpp Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CPPFLAGS) $(CPP_STANDARD) $< -o $@

# Rule for .cc files in parent directories
$(BUILD_DIR)/%.o: ../%.cc Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CPPFLAGS) $(CPP_STANDARD) $< -o $@

# Rule for .c files in parent directories
$(BUILD_DIR)/%.o: ../%.c Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(C_STANDARD) $< -o $@

# Rule for core C files (like startup_stm32h750xx.c)
# Matches build/startup_stm32h750xx.o and finds it in SYSTEM_FILES_DIR
$(BUILD_DIR)/%.o: $(SYSTEM_FILES_DIR)/%.c Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(C_STANDARD) $< -o $@

# Override the elf target to ensure our directory-preserving OBJECTS are used for linking
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

# Disable vpath to prevent libDaisy's default rules from finding files incorrectly
vpath %.cpp
vpath %.cc
vpath %.c
vpath %.s
