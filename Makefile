package = pm
distfiles = Makefile pm.c README

CC = cc
CFLAGS = -Os -Wall -Wextra -Werror
LDFLAGS = -largon2

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

.PHONY: all check dist clean distclean install uninstall

all: pm

check: pm
	test `echo '' | ./pm ''` = 'hWm5uFl4gwwvebgibt!ccggix'

dist:
	rm -rf $(package) $(package).tar $(package).tar.gz
	mkdir $(package)
	cp -f $(distfiles) $(package)
	tar cf $(package).tar $(package)
	gzip $(package).tar
	rm -rf $(package) $(package).tar

clean:
	rm -f pm
	rm -rf $(package) $(package).tar $(package).tar.gz

distclean: clean

install: all
	mkdir -p $(DESTDIR)$(bindir)
	cp -f pm $(DESTDIR)$(bindir)
	chmod +x $(DESTDIR)$(bindir)/pm

uninstall:
	rm -f $(DESTDIR)$(bindir)/pm

pm: pm.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
