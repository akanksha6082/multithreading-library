subdirs = $(wildcard */.)

.PHONY: $(subdirs)

all: $(subdirs)
clean: $(subdirs)

$(subdirs):
	$(MAKE) -C $@ $(MAKECMDGOALS)

