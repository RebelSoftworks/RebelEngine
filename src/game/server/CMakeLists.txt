project(server LANGUAGES CXX)

add_library(server SHARED 
    cbase.cpp
    source2_server.cpp
    source2_gameentities.cpp
)

if(NOT LINUX)
	set(OUTBINDIR ${SRCDIR}/../game/vr/bin/win64)
else()
	set(OUTBINDIR ${SRCDIR}/../game/vr/bin/linuxsteamrt64)
endif()

set_target_properties(server PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUTBINDIR}
)

target_include_directories(server PRIVATE
    ${SRCDIR}/public
    ${SRCDIR}/public/tier0
    ${SRCDIR}/public/appframework
    ${SRCDIR}/public/interfaces
)

target_link_directories(server PRIVATE ${SRCDIR}/lib/public)

target_link_libraries(server PRIVATE interfaces tier0 vstdlib)
