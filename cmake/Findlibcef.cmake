#
# Finds libcef
#

find_library( LIBCEF_LIB libcef PATHS ${SOURCE_SDK_ROOT}/lib/public NO_DEFAULT_PATH )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( libcef DEFAULT_MSG LIBCEF_LIB )

if( LIBCEF_LIB )
	add_library( libcef SHARED IMPORTED GLOBAL )

	if( MSVC )
		set_property( TARGET libcef PROPERTY IMPORTED_IMPLIB ${LIBCEF_LIB} )
	else()
		set_property( TARGET libcef PROPERTY IMPORTED_LOCATION ${LIBCEF_LIB} )
	endif()
endif()

unset( LIBCEF_LIB CACHE )
