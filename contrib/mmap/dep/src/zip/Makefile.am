



noinst_LIBRARIES = libzip.a

libzip_a_CPPFLAGS = -I$(top_srcdir)/dep/include -I$(top_srcdir)/dep/include/zip

if DEBUG
libzip_a_CPPFLAGS += -DDEBUG -D_DEBUG -DG3D_DEBUG -DG3D_DEBUGRELEASE
endif

libzip_a_SOURCES= \
crypt.h \
ioapi.c \
iowin32.c \
iowin32.h \
unzip.c \
zip.c










