project(client LANGUAGES CXX)

if(NOT LINUX)
	set(OUTBINDIR ${SRCDIR}/../game/vr/bin/win64)
else()
	set(OUTBINDIR ${SRCDIR}/../game/vr/bin/linuxsteamrt64)
endif()

add_library(client SHARED
    cbase.cpp
    prediction2.cpp
    source2_client.cpp
    baseentity.cpp
    baseentity.h
    ../../public/murmur2.cpp
    ../../engine2/camerarenderer.cpp
    viewrender.cpp
)

set_target_properties(client PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUTBINDIR}
)

target_link_directories(client PRIVATE ${SRCDIR}/lib/public)

target_include_directories(client PRIVATE
    ${SRCDIR}/public
    ${SRCDIR}/public/tier0
    ${SRCDIR}/public/appframework
    ${SRCDIR}/public/interfaces
    ${SRCDIR}/public/game/client
    ${SRCDIR}/public/tier1
)

target_link_libraries(client PRIVATE interfaces tier0 vstdlib)