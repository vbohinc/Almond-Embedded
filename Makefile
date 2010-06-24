# this will build all nuts
# author Linus Lotz

subdirs += squirrel
subdirs += nuts
subdirs += test

.PHONY: all
all:
	$(foreach dirs,$(subdirs),$(MAKE) -C $(dirs);)

.PHONY: clean
clean:
	$(foreach dirs,$(subdirs),$(MAKE) -C $(dirs) clean;)

.PHONY: program
program:
	$(foreach dirs,$(subdirs),$(MAKE) -C $(dirs) program;)


