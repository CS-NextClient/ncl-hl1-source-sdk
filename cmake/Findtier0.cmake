#
# Finds tier0 (GoldSource specific)
#

find_library( TIER0_LIB tier0 PATHS ${tier0_DIR} ${SOURCE_SDK_ROOT}/lib/public NO_DEFAULT_PATH )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( tier0 DEFAULT_MSG TIER0_LIB )

if( TIER0_LIB )
	add_library( tier0 SHARED IMPORTED GLOBAL )
	
	if( MSVC )
		set_property( TARGET tier0 PROPERTY IMPORTED_IMPLIB ${TIER0_LIB} )
	else()
		set_property( TARGET tier0 PROPERTY IMPORTED_LOCATION ${TIER0_LIB} )
	endif()
endif()

unset( TIER0_LIB CACHE )
