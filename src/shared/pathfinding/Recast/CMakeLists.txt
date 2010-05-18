
########### next target ###############

SET(recast_STAT_SRCS
   RecastArea.cpp
   RecastContour.cpp
   Recast.cpp
   RecastFilter.cpp
   Recast.h
   RecastLog.cpp
   RecastLog.h
   RecastMesh.cpp
   RecastMeshDetail.cpp
   RecastRasterization.cpp
   RecastRegion.cpp
   RecastTimer.cpp
   RecastTimer.h
)

add_library(recast STATIC ${recast_STAT_SRCS})
