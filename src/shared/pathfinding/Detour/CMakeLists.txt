
########### next target ###############

SET(detour_STAT_SRCS
   DetourCommon.cpp
   DetourCommon.h
   DetourNavMeshBuilder.cpp
   DetourNavMeshBuilder.h
   DetourNavMesh.cpp
   DetourNavMesh.h
   DetourNode.cpp
   DetourNode.h
)

add_library(detour STATIC ${detour_STAT_SRCS})
