# Project Name
TARGET = Reverb_test

# Sources
CPP_SOURCES += Reverb_test.cpp
CPP_SOURCES += include/MapPot.cpp

# Library Locations
DAISYSP_DIR ?= ../../DaisySP
LIBDAISY_DIR ?= ../../libDaisy

C_INCLUDES += \
	-I$(LIBDAISY_DIR)/Drivers/CMSIS/DSP/Include \
	-I./include

OPT = -O0

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile