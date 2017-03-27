# tabstops are 4
#

# The root location of the current SDK, replace this with wherever you've installed the SDK.
SDK_ROOT		= extlibs/appdynamics-sdk-native

SDK_INC_DIR		= $(SDK_ROOT)/sdk_lib
SDK_LIB_DIR		= $(SDK_ROOT)/sdk_lib/lib
SDK_LIB			= $(SDK_LIB_DIR)/libappdynamics_native_sdk.so
SDK_LOAD_PATH	= LD_LIBRARY_PATH=$(SDK_LIB_DIR)
SDK_INC         = -I$(SDK_INC_DIR)

# LINUX specific library locations.
# Ubuntu/Debian					LIB_DIR	= /usr/lib/x86_64-linux-gnu/
# Centos/Redhat/Fedora			LIB_DIR = /usr/lib64
LIB_DIR			= /usr/lib/x86_64-linux-gnu/

CCP		= g++
CFLAGS  = -g -std=c++11

all:	checkpaths C_SDK_example

.PHONY: C_SDK_example checkpaths clean

clean:
	@echo "cleaning everything..."
	rm C_SDK_example

checkpaths:
	@echo "checking paths..."
	@echo "checking unix library ($(LIB_DIR))"
	@if [ ! -d "$(LIB_DIR)" ] && [ ! -s "$(LIB_DIR)" ]; then \
		echo "$(LIB_DIR) not a dir or symlink"; \
		exit 1; \
	fi 
	@echo "checking SDK_ROOT ($(SDK_ROOT))"
	@if [ ! -d "$(SDK_ROOT)" ] && [ ! -s "$(SDK_ROOT)" ]; then \
		echo "$(SDK_ROOT) not a dir or symlink"; \
		exit 1; \
	fi 

C_SDK_example: src/main/cpp/C_SDK_example.c Makefile
	@echo "\n\n\n\n\n******************************************************\n"
	@echo "Building $@ using current (4.2 or later) SDK calls."
	@echo "******************************************************\n\n\n\n"
	rm -f $@
	$(CCP) $(CFLAGS) $(SDK_INC) $< $(SDK_LIB) -o $@
	@echo "Build completed, starting application..."
	$(SDK_LOAD_PATH) ./$@

