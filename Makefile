SRC_FILES := $(shell find src -type f -name "*.cpp")
INCLUDE_FILES := $(shell find include -type f -name "*.hpp")
MAIN_FILE := src/main.cpp

format:
	clang-format -i $(SRC_FILES) $(INCLUDE_FILES) $(MAIN_FILE)

tidy:
	clang-tidy -p build $(SRC_FILES) $(INCLUDE_FILES) $(MAIN_FILE) -- -Iinclude

.PHONY: format tidy gen-compile-commands
