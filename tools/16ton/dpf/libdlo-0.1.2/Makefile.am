SUBDIRS=src  \
	test
dist_doc_DATA = README
ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = \
	autogen.sh \
	test/images/test08.bmp \
	test/images/test16.bmp \
	test/images/test24.bmp \
	test/images/test32.bmp \
	Guide-v104.pdf \
	mkdox.sh

TESTS_ENVIRONMENT = sudo
TESTS = test/test1

MAINTAINERCLEANFILES = depcomp INSTALL install-sh missing aclocal.m4 config.guess config.sub configure

clean-local:
	test -d ./docs && rm -Rf ./docs || true

# overloading this to clean known files that shouldn't be in git.
# Much discussion of this on web. Not to standard, but common need.
maintainer-clean-local:
	test -d ./m4 && rm -Rf ./m4 || true
	test -d ./config && rm -Rf ./config || true
	test -d ./autom4te.cache && rm -Rf ./autom4te.cache || true
	find . -name \*~ -exec rm {} \;
	find . -name \*.in -exec rm {} \;
	
