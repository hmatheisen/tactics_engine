SRC_FILES := $(shell find src -type f -name "*.cpp")
INCLUDE_FILES := $(shell find include -type f -name "*.hpp")
MAIN_FILE := src/main.cpp
BUILD_DIR := build

all: build tactics

format:
	clang-format -i $(SRC_FILES) $(INCLUDE_FILES) $(MAIN_FILE)

tidy:
	clang-tidy -p build $(SRC_FILES) $(INCLUDE_FILES) $(MAIN_FILE) -- -Iinclude

build:
	cmake -B $(BUILD_DIR) -S .

tactics: build
	cmake --build $(BUILD_DIR) -j $(shell nproc) -t tactics

clean:
	rm -rf $(BUILD_DIR)

.PHONY: format tidy tactics clean
