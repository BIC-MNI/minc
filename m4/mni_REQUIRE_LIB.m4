dnl @synopsis mni_REQUIRE_LIB(LIBRARY,INCLUDES,BODY)
dnl
dnl @version $Id: mni_REQUIRE_LIB.m4,v 1.2 2009-11-26 04:37:29 vfonov Exp $
dnl @author Steve M. Robbins <smr@debian.org>


AC_DEFUN([mni_REQUIRE_LIB],
[
  AC_MSG_CHECKING([for library $1])
  LIBS="-l$1 $LIBS"
  AC_TRY_LINK([$2],[$3],[mni_result=yes],[mni_result=no])
  AC_MSG_RESULT([$mni_result])
  if test "$mni_result" = "no"; then
    AC_MSG_ERROR([cannot find required library $1])
  fi
])

