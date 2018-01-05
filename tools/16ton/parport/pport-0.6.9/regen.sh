#!/bin/bash
#
# this script will try to generate all the
# configuration tools needed to compile

cnt=4

echo -en "  +Cleaning source: "
if test -f Makefile; then
	rm Makefile configure config.h
	echo "done"
else
	echo "clean"
fi
until [ "$cnt" -lt 4 ]; do
  echo -en "  +Running aclocal: "
  aclocal 2>/dev/null
  let cnt=cnt-1	
done
echo "done"
until [ "$cnt" -lt 3 ]; do
  echo -en "  +Running autoheader: "
  autoheader --force 2>/dev/null
  let cnt=cnt-1
done
echo "done"
until [ "$cnt" -lt 2 ]; do
  echo -en "  +Generating makefiles: "
  automake --add-missing --copy --force-missing 2>/dev/null
  let cnt=cnt-1
done
echo "done"
until [ "$cnt" -lt 1 ]; do
  echo -en "  +Generating configure script: "
  autoconf 2>/dev/null
  let cnt=cnt-1
done
echo "done"
echo -en "  +Configure ready: "
if test -f Makefile; then
  echo "no"
else
  echo "yes"
fi
