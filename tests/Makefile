SUBDIRS= hello_world w1_test clock_test lcd

.PHONY: all clean $(SUBDIRS)

all clean:
	for dir in $(SUBDIRS); do \
	echo "Making $$dir" && \
	$(MAKE) -C $$dir $@ || exit 0; \
	done

