# this will build all nuts
# author Linus Lotz

subdirs = weatherstation

.PHONY: all
all: $(subdirs)

.PHONY: $(subdirs)
$(subdirs):
	$(MAKE) -C $@
.PHONY: clean
clean:
	$(foreach dirs,$(subdirs),$(MAKE) -C $(dirs) clean)
