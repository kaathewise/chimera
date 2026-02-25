# Project Name
TARGET = controls_check

# Sources
PACKAGES = .

CPP_SOURCES = $(notdir $(wildcard $(patsubst %,%/*.cpp,$(PACKAGES))))

C_INCLUDES = -I../

# Library Locations
LIBDAISY_DIR = ../libDaisy

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
