# -*- Makefile -*-

ACLOCAL_AMFLAGS  = -I m4
SUBDIRS          = extensions geoip

man_MANS := xtables-addons.8

.PHONY: FORCE
FORCE:

xtables-addons.8: FORCE
	${MAKE} -f Makefile.mans all;

clean-local-mans:
	${MAKE} -f Makefile.mans clean;

clean-local: clean-local-mans

config.status: Makefile.iptrules.in

tmpdir := $(shell mktemp -dtu)
packer  = xz
packext = .tar.xz

.PHONY: tarball
tarball:
# do not use mkdir_p here.
	mkdir ${tmpdir}
	pushd ${top_srcdir} && git archive --prefix=${PACKAGE_NAME}-${PACKAGE_VERSION}/ HEAD | tar -C ${tmpdir} -x && popd;
	pushd ${tmpdir}/${PACKAGE_NAME}-${PACKAGE_VERSION} && ./autogen.sh && popd;
	tar --use=${packer} -C ${tmpdir} -cf ${PACKAGE_NAME}-${PACKAGE_VERSION}${packext} --owner=root --group=root ${PACKAGE_NAME}-${PACKAGE_VERSION}/;
	rm -Rf ${tmpdir};
