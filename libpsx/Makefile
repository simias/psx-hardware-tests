SRC = irq.c joy_mc.c

include ../config.mk

CFLAGS += -I include/ -MMD

LIBPSX_BUILD_DIR := $(BUILD_DIR)/libpsx

OBJ := $(patsubst %.c,$(LIBPSX_BUILD_DIR)/%.o,$(SRC))

DEP = $(OBJ:%.o=%.d)

.PHONY: all
all: $(LIBPSX_BUILD_DIR)/libpsx.a

$(LIBPSX_BUILD_DIR)/libpsx.a: $(OBJ)
	$(AR) rcs $@ $^

$(OBJ): | $(LIBPSX_BUILD_DIR)

$(LIBPSX_BUILD_DIR):
	mkdir -p $@

$(LIBPSX_BUILD_DIR)/%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $<

-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) $(LIBPSX_BUILD_DIR)/libpsx.a
