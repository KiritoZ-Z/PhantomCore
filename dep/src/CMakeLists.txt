add_subdirectory(g3dlite)
if(WIN32)
add_subdirectory(jmalloc)
elseif(LINUX)
add_subdirectory(jmalloc)
endif()
add_subdirectory(sockets)
add_subdirectory(zlib)


########### install files ###############