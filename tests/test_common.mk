# Relative to the test directory which includes this file
include ../../config.mk

LDSCRIPT := $(SDK_ROOT)/psx.ld
LDFLAGS := --gc-sections --fatal-warnings

TEST_BUILD_DIR := $(BUILD_DIR)/tests/$(NAME)

OBJ := $(patsubst %.s,$(TEST_BUILD_DIR)/%.o,$(SRC))
OBJ := $(patsubst %.c,$(TEST_BUILD_DIR)/%.o,$(OBJ))

PSEXE   := $(TEST_BUILD_DIR)/$(NAME).psexe
ELF     := $(PSEXE:.psexe=.elf)
ARCHIVE := $(PSEXE:.psexe=.a)

$(PSEXE): $(ELF)
	$(ELF2PSEXE) $(REGION) $< $@

$(ELF): $(ARCHIVE) $(LDSCRIPT) $(RUNTIME_AR)
	$(LD) $(LDFLAGS) -o $@ -T $(LDSCRIPT) --start-group $< $(RUNTIME_AR) $(BUILD_DIR)/libpsx/libpsx.a --end-group

$(ARCHIVE): $(OBJ)
	$(AR) rcs $@ $^

$(TEST_BUILD_DIR)/%.o : %.s
	$(AS) $(ASFLAGS) -o $@ $^

$(TEST_BUILD_DIR)/%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ): | $(TEST_BUILD_DIR)

$(TEST_BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(TEST_BUILD_DIR)
