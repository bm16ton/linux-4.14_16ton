# -*- Makefile -*-

AM_CPPFLAGS = ${regular_CPPFLAGS} -I${abs_top_srcdir}/extensions
AM_CFLAGS   = ${regular_CFLAGS} ${libxtables_CFLAGS}

include ../../Makefile.extra

sbin_PROGRAMS = iptaccount
iptaccount_LDADD = libxt_ACCOUNT_cl.la

lib_LTLIBRARIES = libxt_ACCOUNT_cl.la

man_MANS = iptaccount.8
