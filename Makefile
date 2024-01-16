CC=gcc
BUILDDIR=build
VPATH=build
SRC=main.c
HEAD=types.h

3d: $(SRC) $(HEAD)
	mkdir -p $(BUILDDIR)
	$(CC) -o $(BUILDDIR)/$@ $<


.PHONY: clean run

run:
	$(BUILDDIR)/3d

clean:
	$(RM) -rf $(BUILDDIR)
