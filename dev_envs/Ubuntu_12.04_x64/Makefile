CC := gcc
FLAGS := -Wall -D_GNU_SOURCE
LDFLAGS := -lcrypto -lxml2
TARGET_OS := Ubuntu_12.04_x64
PROG := mhl
SRC_DIR := ../../src
INCLUDE_DIRS := -I$(SRC_DIR)
CONFIGURE_COMMON_DIR := ../../src/facade_info

BUILD_TYPES := DEBUG RELEASE
DEBUG_CFLAGS := $(FLAGS) -ggdb -DDEBUG
RELEASE_CFLAGS := $(FLAGS) -O3
DEBUG_DIR := Debug
RELEASE_DIR := Release
DEBUG_CONFIG_NAME := debug
RELEASE_CONFIG_NAME := release

define VARS_DEF
$(1)_BIN_DIR := ../../bin/$(TARGET_OS)/$$($(1)_DIR)
$(1)_TARGET := $$($(1)_BIN_DIR)/$(PROG)
COMMON_$(1)_BUILD_DIR := build/$$($(1)_DIR)/common
TARGET_$(1)_BUILD_DIR := build/$$($(1)_DIR)/$(PROG)
endef

$(foreach BUILD_T,$(BUILD_TYPES),$(eval $(call VARS_DEF,$(BUILD_T))))

FACADE_INFO_SRC_DIR := $(SRC_DIR)/facade_info
FACADE_INFO_OBJS := error_codes.o
FACADE_INFO_INC_FILES := $(FACADE_INFO_SRC_DIR)/error_codes.h

GENERICS_OBJS := char_conversions.o \
                 std_funcs_os_anonymizer.o \
                 memory_management.o

GENERICS_SRC_DIR := $(SRC_DIR)/generics
GENERICS_INC_FILES := $(wildcard $(GENERICS_SRC_DIR)/*.h) $(FACADE_INFO_INC_FILES)

GENERICS_FILESYSTEM_OBJS := file_path_decomposition.o \
                            os_filepath_handlers.o \
                            os_filesystem_elements.o

GENERICS_FILESYSTEM_SRC_DIR := $(SRC_DIR)/generics/filesystem_handlers
GENERICS_FILESYSTEM_INC_FILES := $(wildcard $(GENERICS_FILESYSTEM_SRC_DIR)/*.h) $(GENERICS_INC_FILES)

MHLTOOLS_COMMON_OBJS := files_data.o \
                        usage_printing.o \
                        mhl_types.o \
                        logging.o \
                        hashing.o \
                        xxhash.o \
                        options.o

MHLTOOLS_COMMON_SRC_DIR := $(SRC_DIR)/mhltools_common
MHLTOOLS_COMMON_INC_FILES := $(wildcard $(MHLTOOLS_COMMON_SRC_DIR)/*.h) $(GENERICS_FILESYSTEM_INC_FILES)

THIRD_PARTY_SRC_DIR := $(SRC_DIR)/third_party

ARGS_SUPPORT_OBJS := file_sequences.o \
                     aux_funcs.o
ARGS_SUPPORT_SRC_DIR := $(SRC_DIR)/args_fileslist_support
ARGS_SUPPORT_FILES := $(wildcard $(ARGS_SUPPORT_SRC_DIR)/*.h) $(MHLTOOLS_COMMON_INC_FILES)

PARSEMHL_OBJS := mhl_file_handlers.o
PARSEMHL_SRC_DIR := $(SRC_DIR)/parsemhl
PARSEMHL_INC_DIRS := -I/usr/include/libxml2
PARSEMHL_INC_FILES := $(wildcard $(PARSEMHL_INC_SRC_DIR)/*.h) $(MHLTOOLS_COMMON_INC_FILES) $(THIRD_PARTY_SRC_DIR)/uthash.h

PRINTMHL_OBJS := print_mhl.o \
                 mhl_creator.o
PRINTMHL_SRC_DIR := $(SRC_DIR)/printmhl
PRINTMHL_INC_FILES := $(wildcard $(PRINTMHL_INC_SRC_DIR)/*.h) $(MHLTOOLS_COMMON_INC_FILES)

MHL_HELP_OBJS := help_topics.o \
                 mhl_help.o
MHL_HELP_SRC_DIR := $(SRC_DIR)/mhl_help
MHL_HELP_INC_FILES := $(wildcard $(MHL_HELP_SRC_DIR)/*.h) $(MHLTOOLS_COMMON_INC_FILES)

MHL_FILE_OBJS := input_parse_mode.o \
                 mhl_file_parse_mode.o \
                 mhl_file.o

MHL_FILE_SRC_DIR := $(SRC_DIR)/mhl_file
MHL_FILE_INC_FILES := $(sort $(wildcard $(MHL_FILE_SRC_DIR)/*.h) $(PARSEMHL_INC_FILES) $(PRINTMHL_INC_FILES) $(MHLTOOLS_COMMON_INC_FILES))

MHL_SEAL_OBJS := mhl_seal.o
MHL_SEAL_SRC_DIR := $(SRC_DIR)/mhl_seal
MHL_SEAL_INC_FILES := $(sort $(wildcard $(MHL_SEAL_SRC_DIR)/*.h) $(ARGS_SUPPORT_FILES) $(PRINTMHL_INC_FILES) $(MHLTOOLS_COMMON_INC_FILES))

MHL_HASH_OBJS := hash_calculate.o \
                 input_verify.o \
                 file_verify.o \
                 mhl_hash.o
MHL_HASH_SRC_DIR := $(SRC_DIR)/mhl_hash
MHL_HASH_INC_FILES := $(sort $(wildcard $(MHL_HASH_SRC_DIR)/*.h) $(ARGS_SUPPORT_FILES) $(MHLTOOLS_COMMON_INC_FILES))

MHL_VERIFICATION_OBJS := check_file.o \
                         mhlverify.o

MHL_VERIFICATION_SRC_DIR := $(SRC_DIR)/mhl_verify/mhl_verification
MHL_VERIFY_SRC_DIR := $(SRC_DIR)/mhl_verify

MHL_VERIFY_INC_FILES := $(sort $(wildcard $(MHL_VERIFICATION_SRC_DIR)/*.h) $(wildcard $(MHL_VERIFY_SRC_DIR)/*.h) $(ARGS_SUPPORT_FILES) $(MHLTOOLS_COMMON_INC_FILES))
MHL_VERIFICATION_INC_FILES := $(sort $(MHL_VERIFY_INC_FILES) $(PARSEMHL_INC_FILES))

MHL_VERIFY_OBJS := verify_options.o \
                   mhl_verify.o

MHL_OBJS := mhl.o
MHL_SRC_DIR := $(SRC_DIR)/mhl
MHL_INC_FILES := $(sort $(wildcard $(MHL_SRC_DIR)/*.h) $(MHL_HELP_INC_FILES) $(MHL_FILE_INC_FILES) $(MHL_SEAL_INC_FILES) $(MHL_HASH_INC_FILES) $(MHL_VERIFY_INC_FILES))

.DEFAULT: all
ifdef DEBUG
all $(PROG): $(DEBUG_CONFIG_NAME) 
else
all $(PROG): $(RELEASE_CONFIG_NAME)
endif

#
# Let's split common and target-specific rules definition for better readability
# Trac manually dependencies from .h files until dependencies are not auto-calculated
#
SUBMODS_COMMON := FACADE_INFO GENERICS GENERICS_FILESYSTEM MHLTOOLS_COMMON

define COMMON_FILES_DEF
$(1)_DEBUG_FILES := $$(foreach obj_f,$$($(1)_OBJS),$(COMMON_DEBUG_BUILD_DIR)/$$(obj_f))
$(1)_RELEASE_FILES := $$(foreach obj_f,$$($(1)_OBJS),$(COMMON_RELEASE_BUILD_DIR)/$$(obj_f))

$$($(1)_RELEASE_FILES) $$($(1)_DEBUG_FILES):%.o: $$($(1)_SRC_DIR)/$$(@F:.o=.c) $$($(1)_INC_FILES)
	@mkdir -p $$(COMMON_BUILD_DIR)
	$$(CC) -c $$(CFLAGS) -o $$@ $(INCLUDE_DIRS) $$($(1)_SRC_DIR)/$$(@F:.o=.c) 

COMMON_DEBUG_FILES += $$($(1)_DEBUG_FILES)
COMMON_RELEASE_FILES += $$($(1)_RELEASE_FILES)
endef

$(foreach SUBMOD,$(SUBMODS_COMMON),$(eval $(call COMMON_FILES_DEF,$(SUBMOD))))

SUBMODS_TARGET := ARGS_SUPPORT PRINTMHL PARSEMHL MHL_HELP MHL_FILE MHL_SEAL MHL_HASH MHL_VERIFICATION MHL_VERIFY MHL

define TARGET_FILES_DEF
$(1)_DEBUG_FILES := $$(foreach obj_f,$$($(1)_OBJS),$(TARGET_DEBUG_BUILD_DIR)/$$(obj_f))
$(1)_RELEASE_FILES := $$(foreach obj_f,$$($(1)_OBJS),$(TARGET_RELEASE_BUILD_DIR)/$$(obj_f))

$$($(1)_RELEASE_FILES) $$($(1)_DEBUG_FILES):%.o: $$($(1)_SRC_DIR)/$$(@F:.o=.c) $$($(1)_INC_FILES)
	@mkdir -p $$(TARGET_BUILD_DIR)
	$$(CC) -c $$(CFLAGS) -o $$@ $(INCLUDE_DIRS) $$($(2)) $$($(1)_SRC_DIR)/$$(@F:.o=.c) 

TARGET_DEBUG_FILES += $$($(1)_DEBUG_FILES)
TARGET_RELEASE_FILES += $$($(1)_RELEASE_FILES)
endef

$(foreach SUBMOD,$(SUBMODS_TARGET),$(eval $(call TARGET_FILES_DEF,$(SUBMOD),$(SUBMOD)_INC_DIRS)))

#
# Define main targets and target-specific variables
#

define TARGET_DEF
$$($(1)_CONFIG_NAME): CFLAGS := $$($(1)_CFLAGS)

$$($(1)_CONFIG_NAME) $$($(1)_CONFIG_NAME)-clean: COMMON_BUILD_DIR := $$(COMMON_$(1)_BUILD_DIR)

$$($(1)_CONFIG_NAME) $$($(1)_CONFIG_NAME)-clean: TARGET_BUILD_DIR := $$(TARGET_$(1)_BUILD_DIR)

$$($(1)_CONFIG_NAME) $$($(1)_CONFIG_NAME)-clean: BIN_DIR := $$($(1)_BIN_DIR)
$$($(1)_CONFIG_NAME) $$($(1)_CONFIG_NAME)-clean: TARGET := $$($(1)_TARGET)


$$($(1)_CONFIG_NAME): configure $$($(1)_TARGET)

$$($(1)_TARGET): $$(COMMON_$(1)_FILES) $$(TARGET_$(1)_FILES)
	@mkdir -p $$(BIN_DIR) 
	$(CC) $$(CFLAGS) -o $$@ $$(COMMON_$(1)_FILES) $$(TARGET_$(1)_FILES) $(LDFLAGS) 
	@echo ============== $(1) $(PROG) DONE ================
endef

$(foreach BUILD_T,$(BUILD_TYPES),$(eval $(call TARGET_DEF,$(BUILD_T))))

clean: $(RELEASE_CONFIG_NAME)-clean $(DEBUG_CONFIG_NAME)-clean

$(RELEASE_CONFIG_NAME)-clean $(DEBUG_CONFIG_NAME)-clean:
	rm -f $(TARGET)
	rm -rf $(COMMON_BUILD_DIR)
	rm -rf $(TARGET_BUILD_DIR)

#
# phony targets
#

.PHONY: clean 

.PHONY: release-clean

.PHONY: debug-clean

.PHONY: configure
