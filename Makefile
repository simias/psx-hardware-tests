include config.mk

all:
	$(MAKE_COMMAND) -C runtime
	$(MAKE_COMMAND) -C tests

clean:
	rm -rf $(BUILD_DIR)
