noinst_PROGRAMS = mmap-viewer

mmap_viewer_CPPFLAGS = \
-I$(top_srcdir)/dep/include \
-I$(top_srcdir)/src/vmap \
-I$(top_srcdir)/src/mmap \
-I$(srcdir)/src \
-I$(top_srcdir)/src/pathfinding/Recast \
-I$(top_srcdir)/src/pathfinding/Detour

mmap_viewer_SOURCES = \
main.cpp \
ModelContainerView.h \
ModelContainerView.cpp \
GridMap.h \
GridMap.cpp

mmap_viewer_LDADD = @DISPLAY_MANAGER_LIBS@ -lmmaps -lvmaps -ldetour -lrecast -lGLG3D -lG3D -lzip -ljpeg -lpng12 -lGLU

mmap_viewer_LDFLAGS = @DISPLAY_MANAGER_LDFLAGS@ \
-L$(top_builddir)/dep/src/GLG3D \
-L$(top_builddir)/dep/src/zip \
-L$(top_builddir)/dep/src/G3D \
-L$(top_builddir)/src/vmap \
-L$(top_builddir)/src/mmap \
-L$(top_builddir)/src/pathfinding/Recast \
-L$(top_builddir)/src/pathfinding/Detour 



