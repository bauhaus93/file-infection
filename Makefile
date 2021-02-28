
SRC_DIR=$(PWD)

BUILD_DIR_MXE=$(PWD)/build-mxe
BUILD_TYPE_MXE=Release

MXE_DIR=$(PWD)/mxe
MXE_TARGET=i686-w64-mingw32.static
MXE_BIN=$(MXE_DIR)/usr/bin
CMAKE_MXE=$(MXE_TARGET)-cmake

BUILD_DIR_NATIVE=$(PWD)/build-native
CMAKE_NATIVE=cmake
BUILD_TYPE_NATIVE=Debug

BUILD_JOBS=8

.PHONY: target-native-setup target-native-build target-native-test \
		mxe-toolchain \
		target-windows-setup target-windows-build \
		clean

all: tests
tests: target-native-test
windows-executable: target-windows-build

clean:
	rm -rf $(BUILD_DIR_NATIVE) $(BUILD_DIR_MXE)

reformat:
	$(SRC_DIR)/scripts/reformat.sh

target-native-setup: $(MXE_INCLUDE)
	rm -rf $(BUILD_DIR_NATIVE) && \
	mkdir -p $(BUILD_DIR_NATIVE) && \
	$(CMAKE_NATIVE) \
		-B $(BUILD_DIR_NATIVE) \
		-S $(SRC_DIR) \
		-DCMAKE_BUILD_TYPE="$(BUILD_TYPE_NATIVE)" \
		-DBUILD_TESTING="ON"

target-native-build:
	[ ! -d "$(BUILD_DIR_NATIVE)" ] && make target-native-setup;\
	$(CMAKE_NATIVE) \
		--build $(BUILD_DIR_NATIVE) \
		-j$(BUILD_JOBS) \
		--target infect infect_lib disasm function_discovery block_copy copy_execution

target-native-test: target-native-build
	cd $(BUILD_DIR_NATIVE) && \
	ctest --output-on-failure \
		-j$(BUILD_JOBS) -R "discover_own_functions"

mxe-toolchain $(MXE_BIN)/$(MXE_TARGET) $(MXE_BIN)/$(CMAKE_MXE):
	[ ! -d "$(MXE_DIR)" ] && git clone https://github.com/mxe/mxe.git $(MXE_DIR);\
	[ -d "$(MXE_DIR)" ] && \
		cd $(MXE_DIR) && \
		git pull --ff-only && \
		make -j$(BUILD_JOBS) MXE_TARGETS="$(MXE_TARGET)" gcc cmake


target-windows-setup: $(MXE_BIN)/$(MXE_TARGET) $(MXE_BIN)/$(CMAKE_MXE)
	rm -rf $(BUILD_DIR_MXE) && \
	mkdir -p $(BUILD_DIR_MXE) && \
	PATH="$(MXE_BIN):$$PATH" $(CMAKE_MXE) \
		-B $(BUILD_DIR_MXE) \
		-S $(SRC_DIR) \
		-DCMAKE_BUILD_TYPE="$(BUILD_TYPE_MXE)" \
		-DBUILD_TESTING="OFF"

target-windows-build: $(MXE_BIN)/$(MXE_TARGET) $(MXE_BIN)/$(CMAKE_MXE)
	[ ! -d "$(BUILD_DIR_MXE)" ] && make target-windows-setup;\
	PATH="$(MXE_BIN):$$PATH" $(CMAKE_MXE) --build $(BUILD_DIR_MXE) \
		-j$(BUILD_JOBS) \
		--target infect infect_lib

