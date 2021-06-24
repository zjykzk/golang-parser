BUILD_DIR := ./build
SRC_DIR := ./src
TEST_DIR := ./test

# Find all the C and C++ files we want to compile
SRCS := src

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := src
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := $(INC_FLAGS) -g -Wall

# Build step for C source
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.c $(TEST_DIR)/utf8.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test/utf8_is_full_rune: $(BUILD_DIR)/test/utf8_is_full_rune.o $(BUILD_DIR)/utf8.o
	mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

test_utf8_is_full_rune: $(BUILD_DIR)/test/utf8_is_full_rune
	$(BUILD_DIR)/test/utf8_is_full_rune

$(BUILD_DIR)/test/utf8_decode_rune.o: $(TEST_DIR)/utf8_decode_rune.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test/utf8_decode_rune: $(BUILD_DIR)/test/utf8_decode_rune.o $(BUILD_DIR)/utf8.o
	mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

test_utf8_decode_rune: $(BUILD_DIR)/test/utf8_decode_rune
	$(BUILD_DIR)/test/utf8_decode_rune

test: test_utf8_is_full_rune test_utf8_decode_rune

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)
