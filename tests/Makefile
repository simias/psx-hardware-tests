TESTS := $(shell find . -type d ! -name '.')

all:
	$(foreach test,$(TESTS), $(MAKE_COMMAND) -C $(test);)

clean:
	$(foreach test,$(TESTS), $(MAKE_COMMAND) -C $(test) clean);)
