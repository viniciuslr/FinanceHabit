################################################################################
# The 'tests' executable
################################################################################

project( TESTS_DB )

CORAL_GENERATE_MAPPINGS( _GENERATED_MAPPINGS "fh.ISQLiteConnection" "fh.ISQLiteStatement" "fh.ISQLiteResult" "fh.ITransactionConvert" )

CORAL_GET_PATH_STRING( CORAL_PATH_STR )
set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "CORAL_PATH=\"${CORAL_PATH_STR}\"" )

include_directories(
	${GTEST_INCLUDE_DIRS}
	${CMAKE_BINARY_DIR}/src/fh/generated
	${CMAKE_SOURCE_DIR}/src/fh
	${CMAKE_CURRENT_BINARY_DIR}/generated
)

# Gather source files in the current directory
file( GLOB _HEADERS *.h )
file( GLOB _SOURCES *.cpp )

add_executable( tests_db EXCLUDE_FROM_ALL ${_HEADERS} ${_SOURCES} )

add_dependencies( tests_db fh )

CORAL_TARGET( tests_db )

target_link_libraries( tests_db ${GTEST_LIBRARIES} ${CORAL_LIBRARIES} )

################################################################################
# Register the test
################################################################################

file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/output )
add_test(
	NAME tests_db
	COMMAND $<TARGET_FILE:tests_db> --gtest_output=xml:../../output/tests_$<CONFIGURATION>.xml
)
CORAL_TEST_ENVIRONMENT( tests_db )

source_group( "Source Files" FILES ${_SOURCES} )
source_group( "Header Files" FILES ${_HEADERS} )