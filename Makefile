# this will build all nuts
# author Linus Lotz

subdirs = squirrel

.PHONY: all
all:
	$(foreach dirs,$(subdirs),$(MAKE) -C $(dirs);)

.PHONY: clean
clean:
	$(foreach dirs,$(subdirs),$(MAKE) -C $(dirs) clean;)
