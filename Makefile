# Makefile for Noise Lab VST/AU Plugin
# This Makefile is a wrapper around CMake commands to simplify the build process

# Variables
BUILD_DIR = build
CMAKE = cmake
CMAKE_FLAGS = 
JUCE_PATH ?= /Applications/JUCE

# Check if JUCE_PATH exists
ifeq ($(wildcard $(JUCE_PATH)),)
$(warning JUCE_PATH at $(JUCE_PATH) does not exist. Please set it to your JUCE directory)
$(warning Example: make JUCE_PATH=/path/to/JUCE)
endif

# OS detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	# macOS
	PLUGIN_DIR = $(BUILD_DIR)/NoiseLab_artefacts/Release
	INSTALL_VST3_DIR = ~/Library/Audio/Plug-Ins/VST3
	INSTALL_AU_DIR = ~/Library/Audio/Plug-Ins/Components
else ifeq ($(UNAME_S),Linux)
	# Linux
	PLUGIN_DIR = $(BUILD_DIR)/NoiseLab_artefacts/Release
	INSTALL_VST3_DIR = ~/.vst3
else
	# Windows (assumes MSYS/MinGW or similar)
	PLUGIN_DIR = $(BUILD_DIR)/NoiseLab_artefacts/Release
	INSTALL_VST3_DIR = "$(HOMEDRIVE)$(HOMEPATH)/Common Files/VST3"
endif

.PHONY: all configure build clean install help run vst3 au standalone

# Default target
all: configure build

# Configure the build
configure:
	@echo "Configuring Noise Lab build..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -DJUCE_PATH=$(JUCE_PATH) $(CMAKE_FLAGS)

# Build the plugin
build:
	@echo "Building Noise Lab plugin..."
	@cd $(BUILD_DIR) && $(CMAKE) --build . --config Release --target NoiseLab_VST3 && $(CMAKE) --build . --config Release --target NoiseLab_AU || \
	(echo "Full build failed, attempting a simplified build without GUI..." && \
	 cd .. && rm -rf $(BUILD_DIR) && mkdir -p $(BUILD_DIR) && \
	 cd $(BUILD_DIR) && $(CMAKE) .. -DJUCE_PATH=$(JUCE_PATH) -DHEADLESS_BUILD=ON $(CMAKE_FLAGS) && \
	 $(CMAKE) --build . --config Release --target NoiseLab_VST3)

# Build specific formats
vst3: configure
	@echo "Building VST3 plugin..."
	@cd $(BUILD_DIR) && $(CMAKE) --build . --config Release --target NoiseLab_VST3

au: configure
	@echo "Building AU plugin..."
	@cd $(BUILD_DIR) && $(CMAKE) --build . --config Release --target NoiseLab_AU

standalone: configure
	@echo "Building Standalone application..."
	@cd $(BUILD_DIR) && $(CMAKE) --build . --config Release --target NoiseLab_Standalone

# Clean the build directory
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)

# Install the plugin to the standard location
install:
	@echo "Installing Noise Lab plugin..."
ifeq ($(UNAME_S),Darwin)
	@mkdir -p $(INSTALL_VST3_DIR)
	@mkdir -p $(INSTALL_AU_DIR)
	@cp -r $(PLUGIN_DIR)/VST3/NoiseLab.vst3 $(INSTALL_VST3_DIR)/ || echo "VST3 not built"
	@cp -r $(PLUGIN_DIR)/AU/NoiseLab.component $(INSTALL_AU_DIR)/ || echo "AU not built"
	@echo "Installed to:"
	@echo "  VST3: $(INSTALL_VST3_DIR)"
	@echo "  AU: $(INSTALL_AU_DIR)"
else ifeq ($(UNAME_S),Linux)
	@mkdir -p $(INSTALL_VST3_DIR)
	@cp -r $(PLUGIN_DIR)/VST3/NoiseLab.vst3 $(INSTALL_VST3_DIR)/ || echo "VST3 not built"
	@echo "Installed to: $(INSTALL_VST3_DIR)"
else
	@echo "Installing on Windows - please copy manually from $(PLUGIN_DIR)"
endif

# Run the standalone version of the plugin
run:
	@echo "Running Noise Lab standalone version..."
	@if [ -f "$(PLUGIN_DIR)/Standalone/NoiseLab" ]; then \
		"$(PLUGIN_DIR)/Standalone/NoiseLab"; \
	elif [ -f "$(PLUGIN_DIR)/Standalone/NoiseLab.app/Contents/MacOS/NoiseLab" ]; then \
		open "$(PLUGIN_DIR)/Standalone/NoiseLab.app"; \
	elif [ -f "$(PLUGIN_DIR)/Standalone/NoiseLab.exe" ]; then \
		"$(PLUGIN_DIR)/Standalone/NoiseLab.exe"; \
	else \
		echo "Standalone version not found. Make sure it was built."; \
	fi

# Show help
help:
	@echo "Noise Lab Build System"
	@echo "----------------------"
	@echo "Targets:"
	@echo "  all       - Configure and build VST3 + AU plugins (default)"
	@echo "  configure - Configure the build system"
	@echo "  build     - Build VST3 + AU plugins"
	@echo "  vst3      - Build only VST3 plugin"
	@echo "  au        - Build only AU plugin"
	@echo "  standalone- Build only standalone application"
	@echo "  clean     - Clean the build directory"
	@echo "  install   - Install the plugin to the standard location"
	@echo "  run       - Run the standalone version of the plugin"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  JUCE_PATH   - Path to your JUCE directory (default: $(JUCE_PATH))"
	@echo "  CMAKE_FLAGS - Additional flags to pass to CMake"
	@echo ""
	@echo "Example:"
	@echo "  make JUCE_PATH=/path/to/juce"