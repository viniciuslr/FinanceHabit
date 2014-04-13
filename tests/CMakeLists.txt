
project( TESTS )

# Add the tests 'modules' dir to the CORAL_PATH
set( CORAL_PATH "${TESTS_SOURCE_DIR}/modules" ${CORAL_PATH} )

# Locate the GTest Framework. We can only add some tests if GTest is available.
find_package( GTest )
if( GTEST_FOUND )

	add_custom_target( tests )

	# On UNIX, GTest generally depends on pthreads, but CMake does not
	# add pthreads to GTEST_LIBRARIES. So we do it...
	if( UNIX )
		list( APPEND GTEST_LIBRARIES pthread )
	endif()

	add_subdirectory( src )
	add_dependencies( tests tests_db )

else()
	message( "WARNING: gtest not found - most tests have been disabled!" )
endif()