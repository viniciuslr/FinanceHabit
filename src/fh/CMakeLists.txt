################################################################################
# Build the DB
################################################################################
message( "porran" )
CORAL_GENERATE_MODULE( _MODULE_SOURCES fh )

include_directories( ${CMAKE_CURRENT_BINARY_DIR}/generated )

file( GLOB _SOURCE_FILES *.cpp *.c )
file( GLOB _HEADER_FILES *.h )

add_library( fh  MODULE ${_HEADER_FILES} ${_SOURCE_FILES} ${_MODULE_SOURCES} )

CORAL_MODULE_TARGET( "fh" fh  )

target_link_libraries( fh ${CORAL_LIBRARIES} )

SOURCE_GROUP( "@Generated" FILES ${_MODULE_SOURCES} )
SOURCE_GROUP( "Source Files" FILES ${_SOURCE_FILES} )
SOURCE_GROUP( "Header Files" FILES ${_HEADER_FILES} )

# install shared library
install( TARGETS fh
	DESTINATION "modules/fh"
	COMPONENT "Database Infra"
)

################################################################################
# Source Groups
################################################################################

