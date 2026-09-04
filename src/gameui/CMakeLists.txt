project(gameui LANGUAGES CXX)

if(NOT LINUX)
	set(OUTBINDIR ${SRCDIR}/../game/vr/bin/win64)
else()
	set(OUTBINDIR ${SRCDIR}/../game/vr/bin/linuxsteamrt64)
endif()

add_library(gameui SHARED
    gameui.cpp
    ${SRCDIR}/thirdparty/imgui/imgui.cpp
    ${SRCDIR}/thirdparty/imgui/imgui_draw.cpp
    ${SRCDIR}/thirdparty/imgui/imgui_tables.cpp
    ${SRCDIR}/thirdparty/imgui/imgui_widgets.cpp
    tinyxml2.cpp
)

set_target_properties(gameui PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUTBINDIR}
)

target_link_directories(gameui PRIVATE ${SRCDIR}/lib/public)

target_include_directories(gameui PRIVATE
    ${SRCDIR}/public
    ${SRCDIR}/public/tier0
    ${SRCDIR}/public/appframework
    ${SRCDIR}/public/interfaces
    ${SRCDIR}/public/gameui
    ${SRCDIR}/public/tier1
)

target_link_libraries(gameui PRIVATE interfaces tier0 vstdlib)