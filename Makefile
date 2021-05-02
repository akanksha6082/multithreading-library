subdirs = $(wildcard */.)

.PHONY: run clean $(subdirs)

clean: $(subdirs)

run : $(subdirs)

$(subdirs):
	$(MAKE) -C $@ $(MAKECMDGOALS)




