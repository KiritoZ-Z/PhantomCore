AC_INIT( [mmaps], [0.0.1] )

AC_CONFIG_SRCDIR([dep/src/G3D/LineSegment.cpp])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AM_PROG_CC_C_O

# configure options
AC_ARG_ENABLE([debug],
[AS_HELP_STRING([--enable-debug],[Turn on debugging])],
[case "${enableval}" in
yes) debug=true ;;
no)  debug=false ;;
*) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])

case $host_os in 
    darwin*)
        DISPLAY_MANAGER_LDFLAGS="-framework Cocoa -framework Carbon -framework OpenGL -framework GLUT -framework AGL";
        DISPLAY_MANAGER_LIBS="";
        WINOW_MANAGER_LIBS="";;
    *)
	    DISPLAY_MANAGER_LDFLAGS="";
	    DISPLAY_MANAGER_LIBS="-lSDL -lSDLmain";
	    WINOW_MANAGER_LIBS="-lX11";;

esac
AC_SUBST(DISPLAY_MANAGER_LDFLAGS)
AC_SUBST(DISPLAY_MANAGER_LIBS)
AC_SUBST(WINOW_MANAGER_LIBS)

AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_CONFIG_FILES([
    Makefile
    dep/Makefile
    dep/src/G3D/Makefile
    dep/src/GLG3D/Makefile
    dep/src/zip/Makefile
    dep/src/Makefile
    src/Makefile
    src/vmap/Makefile
    src/mmap/Makefile
    src/generator/Makefile
    src/vmap2/Makefile
    src/vmap_assembler/Makefile
    src/vmap_debugger/Makefile
    src/pathfinding/Recast/Makefile
    src/pathfinding/Detour/Makefile
    src/pathfinding/Makefile
])

## Output files.
AC_OUTPUT
