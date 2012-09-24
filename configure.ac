AC_INIT([minc-tools],[1.0],[a.janke@gmail.com])

AC_CONFIG_SRCDIR([progs/mincdump/mincdump.h])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

AC_REVISION($Revision: 6.45 $)

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AM_PROG_CC_C_O
AM_PROG_LEX
AC_PROG_YACC

AC_PROG_LIBTOOL

AC_CHECK_HEADERS(sys/time.h sys/stat.h sys/wait.h unistd.h)
AC_CHECK_HEADERS(fcntl.h pwd.h float.h values.h)

AC_PATH_PROGS(POD2MAN, pod2man)
AM_CONDITIONAL(BUILD_POD_DOC, test "x$POD2MAN" != "x" )
if test "x$POD2MAN" = "x"; then
   AC_MSG_WARN([pod2man not installed, unable to build all manual pages])
fi
AC_SUBST(POD2MAN)

smr_WITH_BUILD_PATH

mni_REQUIRE_VOLUMEIO
AC_DEFINE_UNQUOTED([MINC2], [1], [Build against the MINC2 API])

dnl Verify existence of mkstemp, tempnam, and tmpnam                                                          
AC_CHECK_FUNCS(mkstemp tempnam tmpnam)

# Code to enable conditional build of ACR/NEMA tools
AC_ARG_ENABLE(acr-nema,
    [  --enable-acr-nema       install ACR/NEMA tools],
    [ case "${enableval}" in
      yes) acr_nema=true ;;
      no)  acr_nema=false ;;
      *) AC_MSG_ERROR(bad value ${enableval} for --enable-acr-nema) ;;
      esac],
    [acr_nema=false])
AM_CONDITIONAL(ACR_NEMA_TOOLS, test x$acr_nema = xtrue)

AC_CONFIG_FILES([
Makefile 
epm-header
])

AC_OUTPUT(
progs/xfm/xfmflip
progs/minccomplete/minccomplete
progs/minchistory/minchistory
progs/mincpik/mincpik
)

