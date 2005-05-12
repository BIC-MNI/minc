#! /bin/sh

cat <<EOF
Messages of the following type may be safely ignored.
Any other diagnostics may be a sign of trouble.  
Let us know if something goes wrong.

    automake: configure.in: installing [...]
    warning: AC_TRY_RUN called without default to allow cross compiling



EOF

aclocal
autoheader
libtoolize --automake
automake --add-missing
autoconf

