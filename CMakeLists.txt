################################################################################
# Calcium - Domain Model Framework
################################################################################

cmake_minimum_required( VERSION 2.6 )

project( fh )

################################################################################
# Setup Coral
################################################################################

# Load Coral's CMake package
if( NOT CORAL_ROOT )
	file( TO_CMAKE_PATH "$ENV{CORAL_ROOT}" CORAL_ROOT )
endif()
set( CMAKE_MODULE_PATH "${CORAL_ROOT}/cmake" ${CMAKE_MODULE_PATH} )
find_package( Coral REQUIRED )

set( CORAL_PATH
	"${CMAKE_BINARY_DIR}/modules"
	"${CMAKE_SOURCE_DIR}/modules"
	${CORAL_PATH}
)

include_directories( ${CORAL_INCLUDE_DIRS} )

################################################################################
# Packaging
################################################################################

set( CPACK_PACKAGE_NAME					"fh" )
set( CPACK_PACKAGE_VERSION_MAJOR		"0" )
set( CPACK_PACKAGE_VERSION_MINOR		"1" )
set( CPACK_PACKAGE_VERSION_PATCH		"0" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY	"Finance project" )

include( CPack )

################################################################################
# Subdirectories
################################################################################

INSTALL( DIRECTORY ${CMAKE_SOURCE_DIR}/modules/ DESTINATION modules )

add_subdirectory( src )

enable_testing()
add_subdirectory( tests )

