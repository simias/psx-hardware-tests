include ../config.mk

SRC = start.s

OBJ := $(patsubst %.s,$(RUNTIME_BUILD_DIR)/%.o,$(SRC))

BIOS_VECTOR_ASM = $(RUNTIME_BUILD_DIR)/bios-vectors.s
BIOS_VECTOR_OBJ = $(BIOS_VECTOR_ASM:.s=.o)

all: $(RUNTIME_AR)

$(RUNTIME_AR): $(BIOS_VECTOR_OBJ) $(OBJ)
	$(AR) rcs $@ $^

$(BIOS_VECTOR_OBJ): $(BIOS_VECTOR_ASM)
	$(AS) $(ASFLAGS) -o $@ $^

$(RUNTIME_BUILD_DIR)/%.o : %.s
	$(AS) $(ASFLAGS) -o $@ $^

$(BIOS_VECTOR_ASM): generate-bios-vectors.awk bios-vectors.in
	$(AWK) -f $^ > $@

$(BIOS_VECTOR_ASM) $(OBJ): | $(RUNTIME_BUILD_DIR)

$(RUNTIME_BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(RUNTIME_BUILD_DIR)
