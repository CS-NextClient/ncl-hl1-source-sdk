set( MEMTOOLS_HEADER_FILES
	${SOURCE_SDK_ROOT}/public/memtools/hooker.h
	${SOURCE_SDK_ROOT}/public/memtools/patcher.h
)

set( MEMTOOLS_SOURCE_FILES
	hooker.cpp
	patcher.cpp
)

add_library( memtools STATIC
	${MEMTOOLS_HEADER_FILES}
	${MEMTOOLS_SOURCE_FILES}
)

target_include_directories( memtools PRIVATE
	${SOURCE_SDK_INCLUDE_PATHS}
)

target_compile_definitions( memtools PRIVATE
	${SOURCE_SDK_DEFINES}
)