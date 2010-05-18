add_subdirectory(Recast)
add_subdirectory(Detour)

########### next target ###############

SET(pathfind_STAT_SRCS
   ChunkyTriMesh.h
   ChunkyTriMesh.cpp
   InputGeom.h
   InputGeom.cpp
   MeshLoaderObj.h
   MeshLoaderObj.cpp
)
add_library(pathfind STATIC ${pathfind_STAT_SRCS})
