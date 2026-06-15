project(vgui2_surfacelib)

add_library(${PROJECT_NAME} STATIC
        FontAmalgam.h
        FontAmalgam.cpp
        FontManager.h
        FontManager.cpp
        Win32Font.h
        Win32Font.cpp
        vguifont.h
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        tier0
        tier1
        vstdlib
)

# PUBLIC so consumers (vgui2) can include the surfacelib headers by linking this target
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${SOURCE_SDK_INCLUDE_PATHS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        ${SOURCE_SDK_DEFINES}
        NOMINMAX
)

target_compile_options(${PROJECT_NAME} PRIVATE
        ${SOURCE_SDK_WARNING_LEVEL}
        /EHsc # NEVER catch SEH exceptions; c functions cannot throw exceptions
)

if (CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /Zi # enables PDB generation
            /O2 # enables maximize speed optimizations
    )
endif()
