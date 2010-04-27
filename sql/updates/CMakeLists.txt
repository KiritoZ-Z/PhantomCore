FILE(GLOB _SQL_UPDATES *.sql)

install(FILES ${_SQL_UPDATES} DESTINATION share/phantom/sql/updates)
