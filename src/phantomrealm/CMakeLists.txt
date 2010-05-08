########### next target ###############

SET(phantom-realm_SRCS
AuthCodes.cpp
AuthCodes.h
AuthSocket.cpp
AuthSocket.h
Main.cpp
RealmList.cpp
RealmList.h
RealmSocket.h
RealmSocket.cpp
RealmAcceptor.h
)

include_directories(
  ${ACE_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/src/framework
  ${MYSQL_INCLUDE_DIR}
)

SET(phantom-realm_LINK_FLAGS "")

add_executable(phantom-realm ${phantom-realm_SRCS})
add_definitions(
-D_phantom_REALM_CONFIG='"${CONF_DIR}/phantomrealm.conf"'
)
IF (DO_MYSQL)
   SET(phantom-realm_LINK_FLAGS "-pthread ${phantom-realm_LINK_FLAGS}")
ENDIF(DO_MYSQL)

IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
   SET(phantom-realm_LINK_FLAGS "-framework Carbon ${phantom-realm_LINK_FLAGS}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

SET_TARGET_PROPERTIES(phantom-realm PROPERTIES LINK_FLAGS "${phantom-realm_LINK_FLAGS}")

target_link_libraries(
phantom-realm
shared
phantomframework
phantomsockets
phantomdatabase
phantomauth
phantomconfig
zlib
${SSLLIB}
${MYSQL_LIBRARIES}
${OSX_LIBS}
)

install(TARGETS phantom-realm DESTINATION bin)


########### install files ###############

install(FILES  phantomrealm.conf.dist DESTINATION etc)

