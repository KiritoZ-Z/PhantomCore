
########### next target ###############

SET(phantom-core_SRCS
CliRunnable.cpp
CliRunnable.h
Main.cpp
Master.cpp
Master.h
RASocket.cpp
RASocket.h
WorldRunnable.cpp
WorldRunnable.h
)

include_directories(
  ${ACE_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/game
  ${MYSQL_INCLUDE_DIR}
)

SET(phantom-core_LINK_FLAGS "")

add_executable(phantom-core ${phantom-core_SRCS})
add_definitions(
-D_PHANTOM_CORE_CONFIG='"${CONF_DIR}/phantomcore.conf"'
)
IF (DO_MYSQL)
    SET(phantom-core_LINK_FLAGS "-pthread ${phantom-core_LINK_FLAGS}")
ENDIF(DO_MYSQL)
IF (DO_POSTGRESQL)
    SET(phantom-core_LINK_FLAGS "-pthread ${phantom-core_LINK_FLAGS}")
ENDIF(DO_POSTGRESQL)


IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(phantom-core_LINK_FLAGS "-framework Carbon ${phantom-core_LINK_FLAGS}")
    SET(SCRIPT_LIB "")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

SET_TARGET_PROPERTIES(phantom-core PROPERTIES LINK_FLAGS "${phantom-core_LINK_FLAGS}")

if(DO_SCRIPTS)
    SET(SCRIPT_LIB "scripts")
else(DO_SCRIPTS)
    SET(SCRIPT_LIB "")
endif(DO_SCRIPTS)

target_link_libraries(
phantom-core
game
shared
zlib
phantomframework
phantomsockets
phantomdatabase
phantomauth
phantomconfig
vmaps
g3dlite
if(WIN32)
jmalloc
elseif(LINUX)
jmalloc
endif()
${SCRIPT_LIB}
${READLINE_LIBRARY}
${TERMCAP_LIBRARY}
${MYSQL_LIBRARIES}
${POSTGRESQL_LIBRARIES}
${SSLLIB}
${ACE_LIBRARY}
${ZLIB} 
${OSX_LIBS}
)

install(TARGETS phantom-core DESTINATION bin)


########### install files ###############

install(FILES  phantomcore.conf.dist DESTINATION etc)

