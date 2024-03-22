set( MATHLIB_HEADER_FILES
        sse.h
        )

set( MATHLIB_SOURCE_FILES
        mathlib_base.cpp
        sse.cpp
        color_conversion.cpp)

add_library( mathlib STATIC
        ${MATHLIB_HEADER_FILES}
        ${MATHLIB_SOURCE_FILES}
        )

source_sdk_check_winxp_support( mathlib )

target_include_directories( mathlib PRIVATE
        ${SOURCE_SDK_INCLUDE_PATHS}
        )

target_compile_definitions( mathlib PRIVATE
        ${SOURCE_SDK_DEFINES}
        )

set_target_properties( mathlib PROPERTIES
        COMPILE_FLAGS "${SOURCE_SDK_WARNING_LEVEL}"
        )
