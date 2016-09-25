# Absolute path to this directory. It makes the rest of the Makefiles
# simpler but it sucks a bit. We should be able to get this path
# dynamically but I suck at Makefiles
SDK_ROOT := $(HOME)/src/psx-hardware-tests

# Cross-compilation toolchain prefix
CROSS := /opt/psx-tools/bin/mipsel-unknown-elf-

LD := $(CROSS)ld
AS := $(CROSS)as
AR := $(CROSS)ar
CC := $(CROSS)gcc

AWK := awk

ASFLAGS := -O2 -mtune=R2000 -mips1 -no-mips16 --fatal-warnings

CFLAGS := -g -O2 -Wall -Werror -march=mips1 -msoft-float -mno-gpopt -nostdlib -I $(SDK_ROOT)/runtime/

BUILD_DIR := $(SDK_ROOT)/build

RUNTIME_BUILD_DIR := $(BUILD_DIR)/runtime
RUNTIME_AR := $(RUNTIME_BUILD_DIR)/runtime.a

# elf2psexe binary path ( https://github.com/simias/psx-sdk-rs )
ELF2PSEXE := elf2psexe

# Region for the resulting executable: NA, E or J
REGION := E
