project(vgui2)
set(OUTPUT_NAME "vgui2")

add_library(${PROJECT_NAME} SHARED
        Bitmap.h
        Bitmap.cpp
        InputWin32.cpp
        key_values_export.cpp
        lang_code.h
        lang_code.cpp
        LocalizedStringTable.cpp
        SurfaceNext.cpp
        Scheme.cpp
        System.cpp
        vgui.cpp
        vgui_border.h
        vgui_border.cpp
        vgui_internal.h
        vgui_internal.cpp
        vgui_key_translation.h
        vgui_key_translation.cpp
        VPanel.h
        VPanel.cpp
        VPanelWrapper.cpp
        ${SOURCE_SDK_ROOT}/public/UnicodeFileHelpers.cpp
        ${SOURCE_SDK_ROOT}/public/filesystem_helpers.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        steam_api
        tier0
        tier1
        vstdlib

        OpenGL::GL
        Imm32.lib
        Winmm.dll
        Shlwapi.lib
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

    target_link_options(${PROJECT_NAME} PRIVATE
            /INCREMENTAL:NO  # /INCREMENTAL is not compatible with /LTCG
            /DEBUG           # enables PDB generation
            /OPT:REF         # eliminates functions and data that are never referenced
            /OPT:ICF         # enables COMDAT folding
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        OUTPUT_NAME ${OUTPUT_NAME}
        SUFFIX ".dll"
        PDB_NAME ${OUTPUT_NAME}
)
